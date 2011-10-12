/*
 *  game.c
 *  ShmupEngine
 */

#include "game.h"

shmup_game *
shmup_game_init(int network_type, char *hostname)
{
	ENetEvent event;
	ENetAddress address;
	shmup_game *g;
	
	g = malloc(sizeof(shmup_game));
	glfwGetWindowSize(&g->window_width, &g->window_height);
	
	g->network_type = network_type;
	g->render_type = 2;
	g->quit = 0;	
	g->emitter = v2(g->window_width / 2, g->window_height / 2);
	g->gravity = v2(0, -250);	
	g->bpool = bpool_new(8000);	
	
	g->bpool->tex[0] = SOIL_load_OGL_texture("./data/flare.tga",
						 SOIL_LOAD_AUTO,
						 SOIL_CREATE_NEW_ID, 0);	
	if(g->bpool->tex[0] == 0)
		fprintf(stderr, "loading error: '%s'\n", SOIL_last_result());
	
	g->bpool->tex[1] = SOIL_load_OGL_texture("./data/arrow.tga",
						 SOIL_LOAD_AUTO,
						 SOIL_CREATE_NEW_ID, 0);
	if(g->bpool->tex[1] == 0)
		fprintf(stderr, "loading error: '%s'\n", SOIL_last_result());
	
	g->bpool->prog = load_shaders("./data/glsl/bullets.vsh",
				      "./data/glsl/bullets.fsh");
	
	if (enet_initialize () != 0) {
		fprintf (stderr, "An error occurred while initializing ENet.\n");
		exit(EXIT_FAILURE);
	}
	
	g->num_players = 1;
	g->player[0].pos = v2(g->window_width/2, g->window_height/2);
	g->player[0].vel = v2zero;
	g->player[0].acc = v2zero;
	
	if (g->network_type == SERVER) {		
		address.host = ENET_HOST_ANY;
		address.port = 4000;		
		fprintf(stderr, "initializing server on port %d...\n", address.port);
		g->host = enet_host_create(&address, 4, 2, 0, 0);		
	} else {		
		fprintf(stderr, "initializing client...\n");
		enet_address_set_host(&address, hostname);
		address.port = 4000;
		g->host = enet_host_create(NULL, 1, 2, 0, 0);
		g->peer = enet_host_connect(g->host, &address, 2, 0);		
		if (g->peer == NULL) {
			fprintf (stderr, "No available peers for initiating an ENet connection.\n");
			exit (EXIT_FAILURE);
		}		
		if (enet_host_service (g->host, &event, 5000) > 0 &&
		    event.type == ENET_EVENT_TYPE_CONNECT) {
			printf("Connection to %s:4000 succeeded.\n", hostname);
			g->player[g->num_players].pos = v2(g->window_width/2, g->window_height/2);
			g->player[g->num_players].vel = v2zero;
			g->player[g->num_players].acc = v2zero;
			g->num_players++;
		} else {
			enet_peer_reset(g->peer);
			printf("Connection to %s:4000 failed.\n", hostname);
		}
	}
	
	return g;
}

void 
shmup_game_close(shmup_game *g) 
{
	enet_host_destroy(g->host);
	enet_deinitialize();
	bpool_destroy(g->bpool);
	free(g);

}

void 
shmup_game_run(shmup_game *g) 
{
	double accumulator, current_time, t;
	double frame_time, new_time;
	const double dt = 1.0f/60;

	t = 0.0;
	current_time = glfwGetTime();
	accumulator = 0.0;
	
	while (!g->quit) {
		
		new_time = glfwGetTime();
		frame_time = new_time - current_time;
		current_time = new_time;        
		accumulator += frame_time;
		
		while (accumulator >= dt) {
			shmup_game_update(g, t, dt);
			accumulator -= dt;
			t += dt;			
		}
		
		shmup_game_draw(g);
		
		g->quit = glfwGetKey(GLFW_KEY_ESC);
		g->quit |= !glfwGetWindowParam(GLFW_OPENED);			
	}
}

void
shmup_game_fire(shmup_game *g, int num, int col, vec2d pos, vec2d vel, vec2d acc)
{
	bullet *b;
	vec2d bpos, bvel;
	unsigned int i, index;
	double angle, speed;
		
	for (i=0; i<num; ++i) {
		index = bpool_activate(g->bpool);
		if (index == -1){
			/* we need to resize the pool */
			int new_size = g->bpool->size * 2;
			fprintf(stderr, "resizing to %d!\n", new_size);
			g->bpool = bpool_resize(g->bpool, new_size);
			index = bpool_activate(g->bpool);
			if (index == -1) {
				fprintf(stderr, "FAILED!\n");
				exit(EXIT_FAILURE);
			}
		}
		
		b = &g->bpool->bdata[index];
		
		if (pos.x == 0 && pos.y == 0) {
			bpos = g->emitter;
		} else {
			bpos = pos;
		}
		
		if (vel.x == 0 && vel.y == 0) {
			speed = 300; //200.0 + (float)rand()/RAND_MAX * 400;
			angle = ((float) i / num) * (M_PI * 2); // (float)rand()/RAND_MAX * M_PI * 2;
			bvel = v2(cos(angle)*speed, sin(angle)*speed);
		} else {
			bvel = vel;
		}
		
		if (col) {
			unsigned char colorbase;
			colorbase = rand() % 128;
			b->color = colorbase;
			b->color += colorbase * 0x100;
			b->color += (colorbase + rand() % (256-colorbase)) * 0x10000;
			b->color += 0xFF000000;	
			b->btype = B_REG;
		} else {
			unsigned char colorbase;
			colorbase = rand() % 128;
			b->color = (colorbase + rand() % (256-colorbase));
			b->color += colorbase * 0x100;
			b->color += colorbase * 0x10000;
			b->color += 0xFF000000;
			b->btype = B_ACCEL;
		}
		
		bullet_emit(b, bpos, bvel, acc);		
	}
}

void 
shmup_game_update(shmup_game *g, double t, double dt)
{	
	int i;
	ENetEvent event;
	ENetPacket *packet;
	
	while (enet_host_service(g->host, &event, 0) > 0) {
		switch (event.type) {
			case ENET_EVENT_TYPE_CONNECT:
				printf("A new client connected from %x:%u.\n", 
					event.peer->address.host,
					event.peer->address.port);
				/* Store any relevant client information here. */
				g->player[g->num_players].pos = v2(g->window_width/2, g->window_height/2);
				g->player[g->num_players].vel = v2zero;
				g->player[g->num_players].acc = v2zero;
				g->num_players++;
				
				event.peer->data = "Client information";				
				break;				
			case ENET_EVENT_TYPE_RECEIVE:
				g->player[1].keyflags = (short) *event.packet->data;
				/* printf("A packet of length %lu containing %s was received from %s on channel %u.\n",
					event.packet->dataLength,
					event.packet->data,
					event.peer->data,
					event.channelID); */
				/* Clean up the packet now that we're done using it. */
				enet_packet_destroy(event.packet);				
				break;				
			case ENET_EVENT_TYPE_DISCONNECT:
				printf("%s disconected.\n", event.peer->data);
				/* Reset the peer's client information. */				
				event.peer->data = NULL;
				break;
			case ENET_EVENT_TYPE_NONE:
				break;
		}
	}

	bullet *b;
	static int mx, my;
	glfwGetMousePos(&mx, &my);
	g->emitter.x = (double) mx;
	g->emitter.y = (double) g->window_height-my;
	
	if (glfwGetMouseButton(GLFW_MOUSE_BUTTON_LEFT))
		shmup_game_fire(g, 40, 0, v2zero, v2zero, v2zero);
	
	if (glfwGetMouseButton(GLFW_MOUSE_BUTTON_RIGHT))
		shmup_game_fire(g, 40, 1, v2zero, v2zero, v2zero);
	
	if (glfwGetKey('1')) 
		g->render_type = 1;
	
	if (glfwGetKey('2')) 
		g->render_type = 2;
	
	g->player->keyflags = KF_NONE;
	if (glfwGetKey('A')) g->player->keyflags |= KF_MOV_L;
	if (glfwGetKey('D')) g->player->keyflags |= KF_MOV_R;
	if (glfwGetKey('W')) g->player->keyflags |= KF_MOV_U;
	if (glfwGetKey('S')) g->player->keyflags |= KF_MOV_D;
	if (glfwGetKey(GLFW_KEY_SPACE)) g->player->keyflags |= KF_FIR_1;
	
	for (i=0; i < g->num_players; i++) {
		player_update(g, &g->player[i], dt);
	}
		
	b = g->bpool->bdata;
	/* 
	 * be careful with this pointer, as this data may be moved by the 
	 * bpool_resize function! Make sure it points to the right place.
	 */	 
	
	/* do updates */
	for (i=0; i < g->bpool->n_active; i++) {
		bullet_update(&b[i], dt);
	}
	
	/* do collisions */
	for (i=0; i < g->bpool->n_active; i++) {		
		if (!point_vs_aabb(b[i].pos, v2zero, v2(g->window_width, g->window_height)))
			bpool_deactivate(g->bpool, i--); 
	}

	
	if (g->network_type == CLIENT) {
		packet = enet_packet_create(&g->player->keyflags, sizeof(short), ENET_PACKET_FLAG_RELIABLE);	
		enet_peer_send(g->peer, 0, packet);
	} else {
		packet = enet_packet_create(&g->player->keyflags, sizeof(short), ENET_PACKET_FLAG_RELIABLE);	
		enet_host_broadcast(g->host, 0, packet);
	}

}

/*
 * This function draws the current scene.
 */

void 
shmup_game_draw(shmup_game *g) 
{   	
	int i;
	bullet *b = g->bpool->bdata;
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	
	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);	
	glDisable(GL_POINT_SPRITE);
	glDisable(GL_TEXTURE_2D);

	for(i=0; i<g->num_players; i++) {	
		glTranslated(g->player[i].pos.x, g->player[i].pos.y, 0);	
		glBegin(GL_QUADS);
		{
			glColor4f(1.0, 1.0, 1.0, 1.0);
			glVertex2d(-16, -10);
			glColor4f(1.0, 1.0, 1.0, 1.0);
			glVertex2d(16, -10);
			glColor4f(1.0, 1.0, 1.0, 1.0);
			glVertex2d(16, 10);
			glColor4f(1.0, 1.0, 1.0, 1.0);
			glVertex2d(-16, 10);
		}
		glEnd();
		glLoadIdentity();
	}
	

	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	glEnable(GL_POINT_SPRITE);
	glEnable(GL_TEXTURE_2D);
		
	if (g->render_type == 1) {
		glPointSize(32.0f);
		glBindTexture(GL_TEXTURE_2D, g->bpool->tex[0]);
		glTexEnvi(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE);
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_COLOR_ARRAY);
		glVertexPointer(2, GL_DOUBLE, sizeof(bullet), &b[0].pos);
		glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(bullet), &b[0].color);
	} else {
		glPointSize(16.0f);
		glBindTexture(GL_TEXTURE_2D, g->bpool->tex[1]);
		glTexEnvi(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_FALSE);
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_COLOR_ARRAY);
		glVertexPointer(4, GL_DOUBLE, sizeof(bullet), &b[0].pos);
		glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(bullet), &b[0].color);
		glUseProgram(g->bpool->prog);
//		glUniform1iARB(glGetUniformLocation(g->bpool->prog, "tex"), 0);
	}
	
	glDrawArrays(GL_POINTS, 0, g->bpool->n_active);	
	glUseProgram(0);
	
	glfwSwapBuffers();
}