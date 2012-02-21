
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

#include "phong.h"
#include "../../../glUtils.h"
#include "../../../ShadowMapping/shadowmap.h"

namespace Shader
{
namespace Constant
{
namespace Specular
{

static const char vertShader[] =
		"#version 330\n"
		"uniform vec3 " UNIF_LIGHTPOS ";\n"
		"uniform mat4 " UNIF_MODELVIEW ";\n"
		"uniform mat4 " UNIF_PROJECTION ";\n"
		"uniform mat4 " UNIF_NORMALTRANS ";\n"
		"layout (location=0) in vec3 position;\n"
		"layout (location=1) in vec3 normal;\n"
		"smooth out vec4 vertColor;\n"
		"smooth out vec3 l;\n"
		"smooth out vec3 n;\n"
		"smooth out vec3 r;\n"
		"smooth out vec3 e;\n"
		"smooth out float distToLight;\n"
		"void main(void) {\n"
		" vec4 p = " UNIF_MODELVIEW " * vec4(position, 1.0);\n"
		// Setup for lambertian + ambient
		" n = normalize( (" UNIF_NORMALTRANS " * vec4(normal,0.0)).xyz );\n"
		" l = " UNIF_LIGHTPOS " - p.xyz;\n"
		// Setup for specular
		" r = normalize( reflect( -l, n ) );\n"
		" e = -normalize( p.xyz );\n"
		// For shadow map
		" distToLight = length(l) / " SHADOWMAP_FAR_STR ";\n"
		" l = normalize(l);\n"
		" gl_Position = " UNIF_PROJECTION " * p;\n"
		"}";
static const char fragShader[] =
		"#version 330\n"
		"uniform vec3 " UNIF_LIGHTRAD ";\n"
		"uniform vec3 " UNIF_SURFREF ";\n"
		"uniform vec3 " UNIF_AMBIENT ";\n"
		"uniform float " UNIF_SPECEXP ";\n"
		"uniform vec3 " UNIF_SPECREF ";\n"
		"in vec4 vertColor;\n"
		"in vec3 l;\n"
		"in vec3 n;\n"
		"in vec3 e;\n"
		"in vec3 r;\n"
		"out vec4 vFragColor;\n"
		"void main(void) {\n"
		" vec3 _l = normalize(l);\n"
		// Lambertian + ambient
		" float diff = max(0.0, dot(_l, normalize(n)));\n"
		" vec3 c = " UNIF_SURFREF " * ( " UNIF_AMBIENT " + diff * " UNIF_LIGHTRAD " );\n"
		// Specular
		" float spec = max(0.0, dot(normalize(e),normalize(r)));\n"
		" if (spec > 0)"
		"   c = c + " UNIF_LIGHTRAD " * " UNIF_SPECREF " * pow( spec, " UNIF_SPECEXP " );\n"
		" vFragColor = vec4(  clamp(c, 0.0, 1.0), 1.0);\n"
		"}";
static const char shadowFragShader[] =
		"#version 330\n"
		"uniform vec3 " UNIF_LIGHTRAD ";\n"
		"uniform vec3 " UNIF_SURFREF ";\n"
		"uniform vec3 " UNIF_AMBIENT ";\n"
		"uniform float " UNIF_SPECEXP ";\n"
		"uniform vec3 " UNIF_SPECREF ";\n"
		"uniform samplerCubeShadow " UNIF_SHADOWMAP ";\n"
		"in vec4 vertColor;\n"
		"in vec3 l;\n"
		"in vec3 n;\n"
		"in vec3 e;\n"
		"in vec3 r;\n"
		"in float distToLight;\n"
		"out vec4 vFragColor;\n"
		"void main(void) {\n"
		" vec3 _l = normalize(l);\n"
		// Shadow map lookup
		" float notShadow = texture(" UNIF_SHADOWMAP ", vec4(-_l,distToLight) );\n"
		// Lambertian + ambient
		" float diff = notShadow * max(0.0, dot(_l, normalize(n)));\n"
		" vec3 c = " UNIF_SURFREF " * ( " UNIF_AMBIENT " + diff * " UNIF_LIGHTRAD " );\n"
		// Specular
		" float spec = notShadow * max(0.0, dot(normalize(e),normalize(r)));\n"
		" if (spec > 0)"
		"   c = c + " UNIF_LIGHTRAD " * " UNIF_SPECREF " * pow( spec, " UNIF_SPECEXP " );\n"
		" vFragColor = vec4(  clamp(c, 0.0, 1.0), 1.0);\n"
		"}";

Phong::Phong()
{
	//printf("Vert shader:\n%s\n\nFrag shader:\n%s\n", vertShader, fragShader);
	if ( !m_program.init(vertShader, fragShader) || isGLError() )
	{
		fprintf(stderr, "ERROR: Specular Phong failed to initialize\n");
	}
	m_isReady =
			(m_program.getUniformID(UNIFORM_LIGHTPOS) >= 0) &&
			(m_program.getUniformID(UNIFORM_LIGHTRAD) >= 0) &&
			(m_program.getUniformID(UNIFORM_AMBIENT) >= 0) &&
			(m_program.getUniformID(UNIFORM_SURFREF) >= 0) &&
			(m_program.getUniformID(UNIFORM_SPECEXP) >= 0) &&
			(m_program.getUniformID(UNIFORM_SPECREF) >= 0) &&
			(m_program.getUniformID(UNIFORM_MODELVIEW) >= 0) &&
			(m_program.getUniformID(UNIFORM_PROJECTION) >= 0) &&
			(m_program.getUniformID(UNIFORM_NORMALTRANS) >= 0);

	if ( !m_shadowProgram.init(vertShader, shadowFragShader) || isGLError() )
	{
		fprintf(stderr, "ERROR: Phong-shadow failed to initialize\n");
	}
	m_isShadowReady =
			(m_shadowProgram.getUniformID(UNIFORM_LIGHTPOS) >= 0) &&
			(m_shadowProgram.getUniformID(UNIFORM_LIGHTRAD) >= 0) &&
			(m_shadowProgram.getUniformID(UNIFORM_AMBIENT) >= 0) &&
			(m_shadowProgram.getUniformID(UNIFORM_SURFREF) >= 0) &&
			(m_shadowProgram.getUniformID(UNIFORM_SPECEXP) >= 0) &&
			(m_shadowProgram.getUniformID(UNIFORM_SPECREF) >= 0) &&
			(m_shadowProgram.getUniformID(UNIFORM_MODELVIEW) >= 0) &&
			(m_shadowProgram.getUniformID(UNIFORM_PROJECTION) >= 0) &&
			(m_shadowProgram.getUniformID(UNIFORM_NORMALTRANS) >= 0) &&
			(m_shadowProgram.getUniformID(UNIFORM_SHADOWMAP) >= 0);
#if !defined(NDEBUG)
	if ( !m_isReady || !m_isShadowReady )
	{
		fprintf(stderr, "ERROR: Specular Phong missing uniforms\n");
	}
#endif
}
Phong::~Phong()
{
}

bool Phong::setUniforms(const GLProgUniforms &uniforms, const bool usingShadow) const
{
	if (!usingShadow)
	{
		glUniform3fv(m_program.getUniformID(UNIFORM_LIGHTPOS), 1, (GLfloat*)&uniforms.m_lightPos);
		glUniform3fv(m_program.getUniformID(UNIFORM_LIGHTRAD), 1, (GLfloat*)&uniforms.m_lightRad);
		glUniform3fv(m_program.getUniformID(UNIFORM_AMBIENT), 1, (GLfloat*)&uniforms.m_ambientRad);
		glUniform3fv(m_program.getUniformID(UNIFORM_SURFREF), 1, (GLfloat*)&uniforms.m_surfRefl);
		glUniform1f(m_program.getUniformID(UNIFORM_SPECEXP), uniforms.m_specExp);
		glUniform3fv(m_program.getUniformID(UNIFORM_SPECREF), 1, (GLfloat*)&uniforms.m_specRefl);
		glUniformMatrix4fv(m_program.getUniformID(UNIFORM_MODELVIEW), 1, GL_FALSE, (GLfloat*)&uniforms.m_modelView);
		glUniformMatrix4fv(m_program.getUniformID(UNIFORM_PROJECTION), 1, GL_FALSE, (GLfloat*)&uniforms.m_projection);
		glUniformMatrix4fv(m_program.getUniformID(UNIFORM_NORMALTRANS), 1, GL_FALSE, (GLfloat*)&uniforms.m_normalTrans);
	}
	else
	{
		glUniform3fv(m_shadowProgram.getUniformID(UNIFORM_LIGHTPOS), 1, (GLfloat*)&uniforms.m_lightPos);
		glUniform3fv(m_shadowProgram.getUniformID(UNIFORM_LIGHTRAD), 1, (GLfloat*)&uniforms.m_lightRad);
		glUniform3fv(m_shadowProgram.getUniformID(UNIFORM_AMBIENT), 1, (GLfloat*)&uniforms.m_ambientRad);
		glUniform3fv(m_shadowProgram.getUniformID(UNIFORM_SURFREF), 1, (GLfloat*)&uniforms.m_surfRefl);
		glUniform1f(m_shadowProgram.getUniformID(UNIFORM_SPECEXP), uniforms.m_specExp);
		glUniform3fv(m_shadowProgram.getUniformID(UNIFORM_SPECREF), 1, (GLfloat*)&uniforms.m_specRefl);
		glUniformMatrix4fv(m_shadowProgram.getUniformID(UNIFORM_MODELVIEW), 1, GL_FALSE, (GLfloat*)&uniforms.m_modelView);
		glUniformMatrix4fv(m_shadowProgram.getUniformID(UNIFORM_PROJECTION), 1, GL_FALSE, (GLfloat*)&uniforms.m_projection);
		glUniformMatrix4fv(m_shadowProgram.getUniformID(UNIFORM_NORMALTRANS), 1, GL_FALSE, (GLfloat*)&uniforms.m_normalTrans);
		glUniform1i(m_shadowProgram.getUniformID(UNIFORM_SHADOWMAP), 1); // Shadow map on texture unit 1
	}
	return !isGLError();
}

}
}
}
