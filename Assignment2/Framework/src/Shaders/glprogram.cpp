
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

#include "../GL3/gl3w.h"
#include <stdio.h>
#include "glprogram.h"

namespace Shader
{

GLProgram::GLProgram() : m_prog(0)
{
	for (int i=0; i<NUM_UNIFORM_VARS; i++)
	{
		m_uniformLocs[i] = -1;
	}
}

GLProgram::~GLProgram()
{
	if (m_prog != 0)
	{
		glDeleteProgram(m_prog);
	}
}

bool GLProgram::init(const char *vertCode, const char *fragCode)
{
	GLuint vertHandle, fragHandle;
	if (vertCode == NULL || fragCode == NULL)
	{
		return false;
	}

	// Compiling a GLSL program is just like compiling
	// a C program

	// 1) First we create a vertex shader program object within
	//    the OpenGL context, and try to compile it
	vertHandle = glCreateShader(GL_VERTEX_SHADER); // returns 0 on error
	if (!compileShader(vertCode, vertHandle))
	{
		glDeleteShader(vertHandle);
		return false;
	}
	// 2) Then create a fragment shader program object within
	//   the OpenGL context, and try to compile it
	fragHandle = glCreateShader(GL_FRAGMENT_SHADER); // returns 0 on error
	if (!compileShader(fragCode, fragHandle))
	{
		glDeleteShader(fragHandle);
		glDeleteShader(vertHandle);
		return false;
	}
	// 3) Next we're going to attach the vertex & fragment shader
	//   programs to a GLSL program, and try to link them together
	//     -- Linking will associate outs from the vertex shader
	//      with ins in the fragment shader by name.
	m_prog = glCreateProgram();
	if (m_prog == 0)
	{
		glDeleteShader(fragHandle);
		glDeleteShader(vertHandle);
		return false;
	}
	glAttachShader(m_prog, vertHandle);
	glAttachShader(m_prog, fragHandle);
	glLinkProgram(m_prog);

	// Flag the vert & frag GLProgram objects for deletion
	// when the program object gets deleted.
	glDeleteShader(vertHandle);
	glDeleteShader(fragHandle);

	// Make sure the link was successful
	GLint success;
	glGetProgramiv(m_prog, GL_LINK_STATUS, &success);
	if (success == GL_FALSE)
	{
		char errLog[1024];
		glGetProgramInfoLog(success, 1024, NULL, errLog);
		fprintf(stderr,
				"========== LINK ERROR ===================\n%s\n",
				errLog);
		glDeleteProgram(m_prog);
		m_prog = 0;
		return false;
	}

	// Validate the linked program
	glValidateProgram(m_prog);
	glGetProgramiv(m_prog, GL_VALIDATE_STATUS, &success);
	if (!success) {
		char errLog[1024];
		glGetProgramInfoLog(m_prog, sizeof(errLog), NULL, errLog);
		fprintf(stderr, "========== VALIDATE ERROR ===================\n%s\n", errLog);
		glDeleteProgram(m_prog);
		m_prog = 0;
		return false;
	}

	// Find out the handle for each of the uniform variables in the GLSL program.
	m_uniformLocs[UNIFORM_LIGHTPOS] = glGetUniformLocation(m_prog, UNIF_LIGHTPOS);
	m_uniformLocs[UNIFORM_LIGHTRAD] = glGetUniformLocation(m_prog, UNIF_LIGHTRAD);
	m_uniformLocs[UNIFORM_AMBIENT] = glGetUniformLocation(m_prog, UNIF_AMBIENT);
	m_uniformLocs[UNIFORM_SURFREF]  = glGetUniformLocation(m_prog, UNIF_SURFREF);
	m_uniformLocs[UNIFORM_TEXTURE0] = glGetUniformLocation(m_prog, UNIF_TEXTURE0);
	m_uniformLocs[UNIFORM_SPECEXP] = glGetUniformLocation(m_prog, UNIF_SPECEXP);
	m_uniformLocs[UNIFORM_SPECREF] = glGetUniformLocation(m_prog, UNIF_SPECREF);
	m_uniformLocs[UNIFORM_MODELVIEW] = glGetUniformLocation(m_prog, UNIF_MODELVIEW);
	m_uniformLocs[UNIFORM_PROJECTION] = glGetUniformLocation(m_prog, UNIF_PROJECTION);
	m_uniformLocs[UNIFORM_NORMALTRANS] = glGetUniformLocation(m_prog, UNIF_NORMALTRANS);
	m_uniformLocs[UNIFORM_SHADOWMAP] = glGetUniformLocation(m_prog, UNIF_SHADOWMAP);

	return true;
}

bool GLProgram::compileShader(const char *code, const GLuint handle) const
{
	if (handle == 0)
	{
		return false;
	}
	glShaderSource(handle, 1, &code, 0);
	glCompileShader(handle);

	// Check for errors
	GLint success;
	glGetShaderiv(handle, GL_COMPILE_STATUS, &success);
	if (success == GL_FALSE)
	{
		char errLog[1024];
		glGetShaderInfoLog(handle, 1024, NULL, errLog);
		fprintf(stderr,
				"========== COMPILE ERROR ===================\n%s\n"
				"=================================\n%s\n=================================\n",
				errLog, code);
		return false;
	}
	return true;
}

void GLProgram::bind() const
{
	glUseProgram(m_prog);
}

void GLProgram::unbind() const
{
	// Binding program handle 0 will unbind the current program.
	glUseProgram(0);
}

}

