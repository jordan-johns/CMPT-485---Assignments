
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
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "glUtils.h"

static const char *getErrorString(GLenum err)
{
	switch (err)
	{
	case GL_NO_ERROR: return "No error";
	case GL_INVALID_ENUM: return "invalid enum";
	case GL_INVALID_VALUE: return "invalid value";
	case GL_INVALID_OPERATION: return "invalid operation";
	case GL_OUT_OF_MEMORY: return "out of memory";
	case GL_INVALID_FRAMEBUFFER_OPERATION: return "invalid framebuffer operation";
	default:
		break;
	}
	return "unknown error";
}

#define EXIT_ON_ERROR
bool _isGLError(const char *file, const int line)
{
	GLenum err = glGetError();
	if (err != GL_NO_ERROR)
	{
		fprintf(stderr, "GL ERROR: File '%s' at line %d: %s\n",
						file, line, getErrorString(err));
#if defined(EXIT_ON_ERROR)
		exit(1);
#endif
		return true;
	}
	return false;
}

bool isExtensionSupported(const char *name)
{
	GLint n=0;
	glGetIntegerv(GL_NUM_EXTENSIONS, &n);
	for (GLint i=0; i<n; i++)
	{
		const char* extension =
				(const char*)glGetStringi(GL_EXTENSIONS, i);
		if (!strcmp(name, extension))
		{
			return true;
		}
	}
	return false;
}
