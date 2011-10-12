#include <stdlib.h>
#include <stdio.h>
#include <GL/glfw.h>

#include "game.h"

int
main(int argc, char **argv)
{
	int network_type;
	shmup_game *g;
	GLFWvidmode d_mode;    

	network_type = CLIENT;
	for (int i=0; i<argc; i++) {
		if (strcmp(argv[i], "-a") == 0) network_type = SERVER;
	}

	if(!glfwInit()) {
		fprintf( stderr, "Failed to initialize GLFW\n" );
		exit(EXIT_FAILURE);
	}

	glfwGetDesktopMode(&d_mode);
	if(!glfwOpenWindow(1280, 800, d_mode.RedBits, d_mode.GreenBits, 
			   d_mode.BlueBits, 8, 8, 0, GLFW_WINDOW)) {
		fprintf(stderr, "Failed to open GLFW window\n");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	g = shmup_game_init(network_type);
	
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
