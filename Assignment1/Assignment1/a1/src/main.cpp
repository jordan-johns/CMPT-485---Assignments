
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

	if (isExtensionSupported("GL_EXT_framebuffer_sRGB"))
	{
		printf("sRGB framebuffer supported\n");
		glEnable(GL_FRAMEBUFFER_SRGB_EXT);
		if (isGLError())
		{
			UI::shutdown();
			return EXIT_FAILURE;
		}
		printf("sRGB framebuffer enabled\n");
	}

	// Turn on backface culling
	glEnable(GL_CULL_FACE);

	// Turn on the depth buffer
	glEnable(GL_DEPTH_TEST);
	if (isGLError())
	{
		UI::shutdown();
		return EXIT_FAILURE;
	}

	// Set the window's title
	UI::setWindowTitle("Assignment 1");

	// Create & initialize the program object
	// Change this 0 to a 1 to start the assignment
#if 1
	Assignment1 program;
	if ( !program.init() )
	{
		printf("FAIL\n");
		UI::shutdown();
		return EXIT_FAILURE;
	}
	UI::setCallbacks(&program);
#else
	DemoProgram demo;
	if ( !demo.init() )
	{
		UI::shutdown();
		return EXIT_FAILURE;
	}
	UI::setCallbacks(&demo);
#endif

	// Event loop
	UI::mainLoop();

	// Clean up the UI before exit
	UI::shutdown();

	return EXIT_SUCCESS;
}
