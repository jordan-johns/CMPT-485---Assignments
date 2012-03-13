
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

#include "../../GL3/gl3w.h"
#include <cstdio>

#include "simple.h"
#include "../../glUtils.h"

namespace Shader
{
namespace Constant
{

// Note:
//  UNIF_AMBIENT & UNIF_SURFREF are both preprocessor macros
// see glprogram.h

// All GLSL vertex, fragment, and geometry shaders
// are compiled at runtime.
// So, we need to provide the source for the shader as a
// string.
//
// Some people save their shader sources in separate files
// and read them in to compile them, but I like making them
// constants in the program for now.
static const char vertShader[] =
		"#version 330\n"
		"uniform vec3 " UNIF_AMBIENT ";\n"
		"uniform vec3 " UNIF_SURFREF ";\n"
		"layout (location=0) in vec3 vVertex;\n"
		"out vec4 vVaryingColor;\n"
		"void main(void) {\n"
		" vVaryingColor = vec4(" UNIF_AMBIENT " , 1.0) * vec4(" UNIF_SURFREF ", 1.0);\n"
		" gl_Position = vec4(vVertex, 1.0);\n"
		"}";
static const char fragShader[] =
		"#version 330\n"
		"out vec4 vFragColor;\n"
		"in vec4 vVaryingColor;\n"
		"void main(void) {\n"
		" vFragColor = vVaryingColor;\n"
		"}";

Simple::Simple()
{
	// Try to create, compile, & link a GLSL program using the source
	// you give it.
	if ( !m_program.init(vertShader, fragShader) || isGLError() )
	{
		fprintf(stderr, "ERROR: Simple failed to initialize\n");
	}
	// Make sure that every uniform that you are using in your shader
	// is given a handle.
	// variable names that do not correspond with a uniform will have
	// been given the value -1
	m_isReady =
			(m_program.getUniformID(UNIFORM_AMBIENT) >= 0) &&
			(m_program.getUniformID(UNIFORM_SURFREF) >= 0);
}
Simple::~Simple() {}


bool Simple::setUniforms(const GLProgUniforms &uniforms, const bool usingShadow) const
{
	// This gets the values for the GLSL shader uniforms from 'uniforms' and
	// sets the program to use them.
	//
	// Different types of uniforms have different glUniform* functions to
	// be able to set their values.
	//
	// Note: OpenGL wants data for matrix uniforms supplied as an array in column-major
	//  order. All of the matrix types in the supplied gml are stored in column-major
	//  order. So, you can just take the address of the matrix, typecast the address to a GLfloat*
	//  and pass the typecast pointer for the matrix data.
	//  This isn't demonstrated here, but you will need to know it when you
	//  implement your geometry transforms.
	glUniform3fv(m_program.getUniformID(UNIFORM_AMBIENT), 1, (GLfloat*)&uniforms.m_ambientRad);
    glUniform3fv(m_program.getUniformID(UNIFORM_SURFREF), 1, (GLfloat*)&uniforms.m_surfRefl);

 	return !isGLError();
}

}
}
