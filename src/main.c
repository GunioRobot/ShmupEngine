#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <GL/glfw.h>

#include "game.h"

int
main(int argc, char **argv)
{
	int network_type;
	int o;
	char *hostname;
	shmup_game *g;
	GLFWvidmode d_mode;    

	network_type = CLIENT;	
	hostname = "localhost";
	
	while ((o = getopt (argc, argv, "sc:")) != -1) {
	switch(o) {
		case 's':
			network_type = SERVER;
			break;
		case 'c':
			network_type = CLIENT;
			hostname = optarg;
			break;
		case '?':
			if (optopt == 'c')
				fprintf (stderr, "Option -%c requires an hostname.\n", optopt);
			else if (isprint (optopt))
				fprintf (stderr, "Unknown option `-%c'.\n", optopt);
			else
				fprintf (stderr,
					 "Unknown option character `\\x%x'.\n", optopt);
			return 1;
		default:
			abort();
	}
	}
	
	if(!glfwInit()) {
		fprintf( stderr, "Failed to initialize GLFW\n" );
		exit(EXIT_FAILURE);
	}

	glfwGetDesktopMode(&d_mode);
	if(!glfwOpenWindow(640, 480, d_mode.RedBits, d_mode.GreenBits, 
			   d_mode.BlueBits, 8, 8, 0, GLFW_WINDOW)) {
		fprintf(stderr, "Failed to open GLFW window\n");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	g = shmup_game_init(network_type, hostname);
	
	glfwSetWindowTitle("ShmupEngine");
//	glfwSetWindowSizeCallback(resize);
	glfwSwapInterval(1);
	glfwSetMousePos(g->window_width/2, g->window_height/2);
	glfwEnable(GLFW_MOUSE_CURSOR);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();   
	glOrtho(0, g->window_width, 0, g->window_height, 100, -100);
	glMatrixMode(GL_MODELVIEW);	
			
	shmup_game_run(g);
	shmup_game_close(g);
	
	glfwTerminate();
	exit(EXIT_SUCCESS);
}
