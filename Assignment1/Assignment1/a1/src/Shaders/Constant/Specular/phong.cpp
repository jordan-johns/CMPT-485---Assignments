
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

#include "phong.h"
#include "../../../glUtils.h"

namespace Shader
{
namespace Constant
{
namespace Specular
{

//  Write the shader
static const char vertShader[] =
		"#version 330\n"
		"uniform vec3 " UNIF_SURFREF ";\n"
		"uniform vec3 " UNIF_AMBIENT ";\n"
		"uniform mat4 " UNIF_MODELVIEW ";\n"
		"uniform mat4 " UNIF_PROJECTION ";\n"
		"uniform mat4 " UNIF_NORMALTRANS ";\n"
		"layout (location=0) in vec3 position;\n"
		"layout (location=1) in vec3 normal;\n"
		"layout (location=2) in vec3 texCoord;\n"
		"smooth out vec4 vertColor;\n"
		"out vec3 fragPos;\n"
		"out vec3 fragNormal;\n"
		"void main(void) {\n"
		" vec4 p = " UNIF_MODELVIEW " * vec4(position, 1.0);\n"
		" vertColor = vec4(" UNIF_AMBIENT " * " UNIF_SURFREF ", 1.0);\n"
		" gl_Position = " UNIF_PROJECTION " * p;\n"
		" gl_Position = gl_Position / abs(gl_Position.w);\n"
		" fragPos = p.xyz;\n"
		" fragNormal = normalize((" UNIF_NORMALTRANS " * vec4(normal, 0)).xyz);\n"
		"}";
static const char fragShader[] =
		"#version 330\n"
		"uniform vec3 " UNIF_LIGHTPOS ";\n"
		"uniform vec3 " UNIF_LIGHTRAD ";\n"
		"uniform vec3 " UNIF_AMBIENT ";\n"
		"uniform vec3 " UNIF_SURFREF ";\n"
		"uniform vec3 " UNIF_SPECREF ";\n"
		"uniform float " UNIF_SPECEXP ";\n"
		"in vec4 vertColor;\n"
		"in vec3 fragPos;\n"
		"in vec3 fragNormal;\n"
		"out vec4 vFragColor;\n"
		"void main(void) {\n"
		" vec3 L = normalize(" UNIF_LIGHTPOS " - fragPos);\n"
		" vec3 E = normalize( -fragPos);\n"
		" vec3 R = normalize(reflect(-L, fragNormal));\n"
		" vec3 highlight = " UNIF_SPECREF " * max(0, pow(dot(R,E), " UNIF_SPECEXP ")) * " UNIF_LIGHTRAD ";\n"
		" vec3 diffuse = " UNIF_SURFREF " * " UNIF_LIGHTRAD " * max(0, dot(fragNormal, L));\n"
		" vec3 ambient = " UNIF_SURFREF " * " UNIF_AMBIENT ";\n"
		" vFragColor = clamp(vec4(diffuse + ambient + highlight, 1), 0, 1);\n"
		"}";

Phong::Phong()
{
	if ( !m_program.init(vertShader, fragShader) || isGLError() )
	{
		fprintf(stderr, "ERROR: Phong specular failed to initialize\n");
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
		fprintf(stderr, "ERROR: Phong specular missing uniforms\n");
	}
#endif
}
Phong::~Phong()
{
}

bool Phong::setUniforms(const GLProgUniforms &uniforms) const
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
