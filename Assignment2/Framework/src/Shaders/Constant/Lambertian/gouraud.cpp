
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
#include <cstdio>

#include "gouraud.h"
#include "../../../glUtils.h"

namespace Shader
{
namespace Constant
{
namespace Lambertian
{

static const char vertShader[] =
		"#version 330\n"
		"uniform vec3 " UNIF_LIGHTPOS ";\n"
		"uniform vec3 " UNIF_LIGHTRAD ";\n"
		"uniform vec3 " UNIF_SURFREF ";\n"
		"uniform vec3 " UNIF_AMBIENT ";\n"
		"uniform mat4 " UNIF_MODELVIEW ";\n"
		"uniform mat4 " UNIF_PROJECTION ";\n"
		"uniform mat4 " UNIF_NORMALTRANS ";\n"
		"layout (location=0) in vec3 position;\n"
		"layout (location=1) in vec3 normal;\n"
		"smooth out vec4 vertColor;\n"
		"void main(void) {\n"
		" vec4 p = " UNIF_MODELVIEW " * vec4(position, 1.0);\n"
		// Lambertian + ambient
		" vec3 n = normalize( (" UNIF_NORMALTRANS " * vec4(normal,0.0)).xyz );\n"
		" vec3 l = normalize(" UNIF_LIGHTPOS " - p.xyz );\n"
		" vec3 c = " UNIF_SURFREF " * ( " UNIF_AMBIENT " + max(0.0,dot(l,n)) * " UNIF_LIGHTRAD " );\n"
		" vertColor = vec4(  clamp(c, 0.0, 1.0), 1.0);\n"
		" gl_Position = " UNIF_PROJECTION " * p;\n"
		"}";
static const char fragShader[] =
		"#version 330\n"
		"in vec4 vertColor;\n"
		"in vec3 norm;\n"
		"out vec4 vFragColor;\n"
		"void main(void) {\n"
		" vFragColor = vertColor;\n"
		"}";

Gouraud::Gouraud()
{
	//printf("Vert shader:\n%s\n\nFrag shader:\n%s\n", vertShader, fragShader);
	if ( !m_program.init(vertShader, fragShader) || isGLError() )
	{
		fprintf(stderr, "ERROR: Gouraud failed to initialize\n");
	}
	m_isReady =
			(m_program.getUniformID(UNIFORM_LIGHTPOS) >= 0) &&
			(m_program.getUniformID(UNIFORM_LIGHTRAD) >= 0) &&
			(m_program.getUniformID(UNIFORM_AMBIENT) >= 0) &&
			(m_program.getUniformID(UNIFORM_SURFREF) >= 0) &&
			(m_program.getUniformID(UNIFORM_MODELVIEW) >= 0) &&
			(m_program.getUniformID(UNIFORM_PROJECTION) >= 0) &&
			(m_program.getUniformID(UNIFORM_NORMALTRANS) >= 0);
	m_isShadowReady = m_isReady;
#if !defined(NDEBUG)
	if ( !m_isReady )
	{
		fprintf(stderr, "ERROR: Gouraud missing uniforms\n");
	}
#endif
}
Gouraud::~Gouraud()
{
}

void Gouraud::bindGL(const bool useShadow) const
{
	if (m_isReady) m_program.bind();
#if !defined(NDEBUG)
	else
	{
		fprintf(stderr, "Attempt to bind non-ready shader\n");
	}
#endif
}

bool Gouraud::setUniforms(const GLProgUniforms &uniforms, const bool usingShadow) const
{
	glUniform3fv(m_program.getUniformID(UNIFORM_LIGHTPOS), 1, (GLfloat*)&uniforms.m_lightPos);
	glUniform3fv(m_program.getUniformID(UNIFORM_LIGHTRAD), 1, (GLfloat*)&uniforms.m_lightRad);
	glUniform3fv(m_program.getUniformID(UNIFORM_AMBIENT), 1, (GLfloat*)&uniforms.m_ambientRad);
	glUniform3fv(m_program.getUniformID(UNIFORM_SURFREF), 1, (GLfloat*)&uniforms.m_surfRefl);
	glUniformMatrix4fv(m_program.getUniformID(UNIFORM_MODELVIEW), 1, GL_FALSE, (GLfloat*)&uniforms.m_modelView);
	glUniformMatrix4fv(m_program.getUniformID(UNIFORM_PROJECTION), 1, GL_FALSE, (GLfloat*)&uniforms.m_projection);
	glUniformMatrix4fv(m_program.getUniformID(UNIFORM_NORMALTRANS), 1, GL_FALSE, (GLfloat*)&uniforms.m_normalTrans);

	return !isGLError();
}

}
}
}
