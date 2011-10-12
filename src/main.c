#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <GL/glfw.h>

#include "game.h"

int
main(int argc, char **argv)
{
	const int width = 720;
	const int height = 450;	
	int network_type, o;
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
		fprintf(stderr, "Failed to initialize GLFW\n" );
		exit(EXIT_FAILURE);
	}

	glfwGetDesktopMode(&d_mode);
	if(!glfwOpenWindow(width, height, d_mode.RedBits, d_mode.GreenBits, 
			   d_mode.BlueBits, 8, 8, 0, GLFW_WINDOW)) {
		fprintf(stderr, "Failed to open GLFW window\n");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	
	char title[50];
	sprintf(title, "ShmupEngine - %s", network_type ? "SERVER" : "CLIENT"); 
	glfwSetWindowTitle(title);
	/* glfwSetWindowSizeCallback(resize); */
	glfwSwapInterval(1);
	glfwSetMousePos(width/2, height/2);
	glfwEnable(GLFW_MOUSE_CURSOR);
	
	g = shmup_game_init(width, height);
	shmup_game_network_connect(g, network_type, hostname);
	shmup_game_init_gl(g);
	shmup_game_run(g);
	shmup_game_close(g);
	
	glfwTerminate();
	exit(EXIT_SUCCESS);
}
