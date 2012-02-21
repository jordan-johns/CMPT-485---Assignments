
/*
 * Copyright:
 * Daniel D. Neilson (ddneilson@ieee.org)
 * University of Saskatchewan
 * All rights reserved
 *
 * Permission granted to use for use in assignments and
 * projects for CMPT 485 & CMPT 829 at the University
 * of Saskatchewan.
 */

#include "GL3/gl3w.h"
#include <GL/glext.h>
#include <GL/glfw.h>

#include <cstdio>
#include <cstdlib>

#include "UI/ui.h"
#include "glUtils.h"
#include "demo.h"
#include "assign1.h"
#include "assign2.h"


int main(int argc, char *argv[])
{
	if ( !UI::init(640,480) || isGLError() )
	{
		fprintf(stderr, "ERROR: Could not initialize UI.\n");
		UI::shutdown();
		return EXIT_FAILURE;
	}

	// Print out some info on the runtime state
	fprintf(stdout, "GL VERSION: %s\n", glGetString(GL_VERSION));
	fprintf(stdout, "GLSL VERSION: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
	fprintf(stdout, "Window settings:\n\trgba bits: (%d,%d,%d,%d)\n"
			"\tdepth bits: %d\n"
			"\tstencil bits: %d\n",
			glfwGetWindowParam(GLFW_RED_BITS),
			glfwGetWindowParam(GLFW_GREEN_BITS),
			glfwGetWindowParam(GLFW_BLUE_BITS),
			glfwGetWindowParam(GLFW_ALPHA_BITS),
			glfwGetWindowParam(GLFW_DEPTH_BITS),
			glfwGetWindowParam(GLFW_STENCIL_BITS));

	// Create & initialize the program object
	int choice=2;
#if 0
	do
	{
		printf("\n"
				"0) Demo\n"
				"1) Assignment 1\n"
				"2) Assignment 2\n"
				"Which program do you want to run? ");
		scanf("%d", &choice);
	} while (choice < 0 || choice > 2);
#endif
	UI::Callbacks *program;

	switch (choice)
	{
	case 0:
		program = new DemoProgram();
		if ( ! ((DemoProgram*)program)->init() )
		{
			fprintf(stderr, "Failed to initialize program\n");
			UI::shutdown();
			return EXIT_FAILURE;
		}
		// Set the window's title
		UI::setWindowTitle("Demo Program");
		break;
	case 1:
		program = new Assignment1();
		if ( ! ((Assignment1*)program)->init() )
		{
			fprintf(stderr, "Failed to initialize program\n");
			UI::shutdown();
			return EXIT_FAILURE;
		}
		// Set the window's title
		UI::setWindowTitle("Assignment 1");
		break;
	case 2:
		program = new Assignment2();
		if ( ! ((Assignment2*)program)->init() )
		{
			fprintf(stderr, "Failed to initialize program\n");
			UI::shutdown();
			return EXIT_FAILURE;
		}
		// Set the window's title
		UI::setWindowTitle("Assignment 2");
		break;

	}
	UI::setCallbacks(program);

	// Event loop
	UI::mainLoop();

	// Clean up the UI before exit
	UI::shutdown();

	delete program;

	return EXIT_SUCCESS;
}
