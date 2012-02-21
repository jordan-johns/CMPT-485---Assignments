

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

#include "../../../GL3/gl3w.h"
#include <GL/gl.h>

#include "gouraud.h"
#include "../../../glUtils.h"

namespace Shader
{
namespace Constant
{
namespace Specular
{

static const char vertShader[] =
		// Shader only works with GLSL version 3.3+
		"#version 330\n"
		// See: glprogram.h for #defines of UNIF_
		// These are preprocessor macros that expand to strings (variable names)
		"uniform vec3 " UNIF_SURFREF ";\n"
		"uniform vec3 " UNIF_AMBIENT ";\n"
		"uniform mat4 " UNIF_MODELVIEW ";\n"
		"uniform mat4 " UNIF_PROJECTION ";\n"
		"uniform mat4 " UNIF_NORMALTRANS ";\n"
		"uniform vec3 " UNIF_LIGHTPOS ";\n"
		"uniform vec3 " UNIF_LIGHTRAD ";\n"
		"uniform vec3 " UNIF_SPECREF ";\n"
		"uniform float " UNIF_SPECEXP ";\n"
		//  Declare position to be a vec3 (3-element vector)
		// attribute, and bind it to attribute location 0.
		// Normal should be location 1
		// Texture coordinates should be location 2
		//  'layout (location=#)' sets the bind point to #
		//  'in' declares this as an input
		//  'vec3' declares its type to be a 3d vector
		"layout (location=0) in vec3 position;\n"
		"layout (location=1) in vec3 normal;\n"
		"layout (location=2) in vec3 texCoord;\n"
		// vertColor is an output from the vertex shader, to the
		// fragment shader.
		//   'smooth' says the value should be smoothly interpolated
		//     to fragments
		//   'out' declares this as an output
		//   'vec4' is its type
		"smooth out vec4 vertColor;\n"
		"void main(void) {\n"
		// Transform the position into camera coordinates
		// have to convert to homogeneous coords first (via vec4 constructor)
		" vec4 p = " UNIF_MODELVIEW " * vec4(position, 1.0);\n"
		" vec3 fragPos = p.xyz;\n"
		" vec3 fragNormal = normalize((" UNIF_NORMALTRANS " * vec4(normal, 0)).xyz);\n"
		" vec3 L = normalize(" UNIF_LIGHTPOS " - fragPos);\n"
		" vec3 E = normalize( -fragPos);\n"
		" vec3 R = normalize(reflect(-L, fragNormal));\n"
		// Specular highlight calculation.
		" vec3 highlight = " UNIF_SPECREF " * max(0, pow(dot(R,E), " UNIF_SPECEXP ")) * " UNIF_LIGHTRAD ";\n"

		" vec3 diffuse = " UNIF_SURFREF " * " UNIF_LIGHTRAD " * max(0, dot(fragNormal, L));\n"
		" vec3 ambient = " UNIF_SURFREF " * " UNIF_AMBIENT ";\n"
		// Use p as your vertex position
		" vertColor = clamp(vec4(diffuse + ambient + highlight, 1.0), 0, 1);\n"
		// Transform p into device coordinates, and do the perspective
		// correction
		" gl_Position = " UNIF_PROJECTION " * p;\n"
		" gl_Position = gl_Position / abs(gl_Position.w);\n"
		"}";
static const char fragShader[] =
		// Shader only works with GLSL version 3.3+
		"#version 330\n"
		// Input to the fragment shader
		//  'in' declares it to be an input
		//  'vec4' is its type
		//  Note: Must be the same type & name as an out from vertex
		//   shader
		"in vec4 vertColor;\n"
		// Declare the output of the fragment shader.
		//  For our purposes this will usually be a 4-element vector
		//  representing an rgba color
		//  You can call it whatever you want
		"out vec4 vFragColor;\n"
		"void main(void) {\n"
		// Just copy the input color into the output
		" vFragColor = vertColor;\n"
		"}";

Gouraud::Gouraud()
{
	if ( !m_program.init(vertShader, fragShader) || isGLError() )
	{
		fprintf(stderr, "ERROR: Gouraud specular failed to initialize\n");
	}

	// Get the uniform variable's handle/id for each uniform you use
	m_isReady = (m_program.getUniformID(UNIFORM_AMBIENT) >= 0) &&
			(m_program.getUniformID(UNIFORM_SURFREF) >= 0) &&
			(m_program.getUniformID(UNIFORM_MODELVIEW) >= 0) &&
			(m_program.getUniformID(UNIFORM_PROJECTION) >= 0) &&
			(m_program.getUniformID(UNIFORM_LIGHTPOS) >= 0) &&
			(m_program.getUniformID(UNIFORM_LIGHTRAD) >= 0) &&
			(m_program.getUniformID(UNIFORM_NORMALTRANS) >= 0) &&
			(m_program.getUniformID(UNIFORM_SPECREF) >= 0) &&
			(m_program.getUniformID(UNIFORM_SPECEXP) >= 0);

#if !defined(NDEBUG)
	if ( !m_isReady )
	{
		fprintf(stderr, "ERROR: Gouraud specular missing uniforms\n");
	}
#endif
}
Gouraud::~Gouraud()
{

}

bool Gouraud::setUniforms(const GLProgUniforms &uniforms) const
{
	// Copy data from the GLProgUniforms instance into your shader program
	// Must copy a value for every uniform that you are using
	glUniform3fv(m_program.getUniformID(UNIFORM_AMBIENT), 1, (GLfloat*)&uniforms.m_ambientRad);
	glUniform3fv(m_program.getUniformID(UNIFORM_SURFREF), 1, (GLfloat*)&uniforms.m_surfRefl);
	glUniform3fv(m_program.getUniformID(UNIFORM_LIGHTPOS), 1, (GLfloat*)&uniforms.m_lightPos);
	glUniform3fv(m_program.getUniformID(UNIFORM_LIGHTRAD), 1, (GLfloat*)&uniforms.m_lightRad);

	// Specular stuff.
	glUniform3fv(m_program.getUniformID(UNIFORM_SPECREF), 1, (GLfloat*)&uniforms.m_specRefl);
	glUniform1f(m_program.getUniformID(UNIFORM_SPECEXP), uniforms.m_specExp);

	// Note: GL_FALSE on these means that the input is in column-major order already
	glUniformMatrix4fv(m_program.getUniformID(UNIFORM_MODELVIEW), 1, GL_FALSE, (GLfloat*)&uniforms.m_modelView);
	glUniformMatrix4fv(m_program.getUniformID(UNIFORM_PROJECTION), 1, GL_FALSE, (GLfloat*)&uniforms.m_projection);
	glUniformMatrix4fv(m_program.getUniformID(UNIFORM_NORMALTRANS), 1, GL_FALSE, (GLfloat*)&uniforms.m_normalTrans);

	return !isGLError();
}

}
}
}
