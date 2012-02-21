
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
namespace Texture
{
namespace Lambertian
{

static const char vertShader[] =
		"#version 330\n"
		"uniform vec3 " UNIF_LIGHTPOS ";\n"
		"uniform mat4 " UNIF_MODELVIEW ";\n"
		"uniform mat4 " UNIF_PROJECTION ";\n"
		"uniform mat4 " UNIF_NORMALTRANS ";\n"
		"layout (location=0) in vec3 position;\n"
		"layout (location=1) in vec3 normal;\n"
		"layout (location=2) in vec2 texCoords;\n"
		"smooth out vec4 vertColor;\n"
		"smooth out vec3 l;\n"
		"smooth out vec3 n;\n"
		"smooth out vec2 texCoord0;\n"
		"smooth out float distToLight;\n"
		"void main(void) {\n"
		" texCoord0 = texCoords;\n"
		" vec4 p = " UNIF_MODELVIEW " * vec4(position, 1.0);\n"
		// Setup for lambertian + ambient
		" n = normalize( (" UNIF_NORMALTRANS " * vec4(normal,0.0)).xyz );\n"
		" l = " UNIF_LIGHTPOS " - p.xyz;\n"
		// Shadow map
		" distToLight = length(l) / " SHADOWMAP_FAR_STR ";\n"
		" l = normalize(l);\n"
		" gl_Position = " UNIF_PROJECTION " * p;\n"
		"}";
static const char fragShader[] =
		"#version 330\n"
		"uniform vec3 " UNIF_LIGHTRAD ";\n"
		"uniform sampler2D " UNIF_TEXTURE0 ";\n"
		"uniform vec3 " UNIF_AMBIENT ";\n"
		"in vec4 vertColor;\n"
		"in vec3 l;\n"
		"in vec3 n;\n"
		"in vec2 texCoord0;\n"
		"out vec4 vFragColor;\n"
		"void main(void) {\n"
		// Lambertian + ambient
		" float diff = max(0.0, dot(normalize(l), normalize(n)));\n"
		" vec3 surf = texture2D(" UNIF_TEXTURE0 ", texCoord0.st).rgb;\n"
		" vec3 c = surf * ( " UNIF_AMBIENT " + diff * " UNIF_LIGHTRAD " );\n"
		" vFragColor = vec4(  clamp(c, 0.0, 1.0), 1.0);\n"
		"}";

static const char shadowFragShader[] =
		"#version 330\n"
		"uniform vec3 " UNIF_LIGHTRAD ";\n"
		"uniform sampler2D " UNIF_TEXTURE0 ";\n"
		"uniform vec3 " UNIF_AMBIENT ";\n"
		"uniform samplerCubeShadow " UNIF_SHADOWMAP ";\n"
		"in vec4 vertColor;\n"
		"in vec3 l;\n"
		"in vec3 n;\n"
		"in vec2 texCoord0;\n"
		"in float distToLight;\n"
		"out vec4 vFragColor;\n"
		"void main(void) {\n"
		" vec3 _l = normalize(l);\n"
		// Shadow map lookup
		" float notShadow = texture(" UNIF_SHADOWMAP ", vec4(-_l,distToLight) );\n"
		// Lambertian + ambient
		" float diff = notShadow * max(0.0, dot(_l, normalize(n)));\n"
		" vec3 surf = texture2D(" UNIF_TEXTURE0 ", texCoord0.st).rgb;\n"
		" vec3 c = surf * ( " UNIF_AMBIENT " + diff * " UNIF_LIGHTRAD " );\n"
		" vFragColor = vec4(  clamp(c, 0.0, 1.0), 1.0);\n"
		"}";
Phong::Phong()
{
	//printf("Vert shader:\n%s\n\nFrag shader:\n%s\n", vertShader, fragShader);
	if ( !m_program.init(vertShader, fragShader) || isGLError() )
	{
		fprintf(stderr, "ERROR: Phong failed to initialize\n");
	}
	m_isReady =
			(m_program.getUniformID(UNIFORM_LIGHTPOS) >= 0) &&
			(m_program.getUniformID(UNIFORM_LIGHTRAD) >= 0) &&
			(m_program.getUniformID(UNIFORM_AMBIENT) >= 0) &&
			(m_program.getUniformID(UNIFORM_TEXTURE0) >= 0) &&
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
			(m_shadowProgram.getUniformID(UNIFORM_TEXTURE0) >= 0) &&
			(m_shadowProgram.getUniformID(UNIFORM_MODELVIEW) >= 0) &&
			(m_shadowProgram.getUniformID(UNIFORM_PROJECTION) >= 0) &&
			(m_shadowProgram.getUniformID(UNIFORM_NORMALTRANS) >= 0) &&
			(m_shadowProgram.getUniformID(UNIFORM_SHADOWMAP) >= 0);

#if !defined(NDEBUG)
	if ( !m_isReady || !m_isShadowReady )
	{
		fprintf(stderr, "ERROR: Phong missing uniforms\n");
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
		glUniform1i(m_program.getUniformID(UNIFORM_TEXTURE0), 0);
		glUniformMatrix4fv(m_program.getUniformID(UNIFORM_MODELVIEW), 1, GL_FALSE, (GLfloat*)&uniforms.m_modelView);
		glUniformMatrix4fv(m_program.getUniformID(UNIFORM_PROJECTION), 1, GL_FALSE, (GLfloat*)&uniforms.m_projection);
		glUniformMatrix4fv(m_program.getUniformID(UNIFORM_NORMALTRANS), 1, GL_FALSE, (GLfloat*)&uniforms.m_normalTrans);
	}
	else
	{
		glUniform3fv(m_shadowProgram.getUniformID(UNIFORM_LIGHTPOS), 1, (GLfloat*)&uniforms.m_lightPos);
		glUniform3fv(m_shadowProgram.getUniformID(UNIFORM_LIGHTRAD), 1, (GLfloat*)&uniforms.m_lightRad);
		glUniform3fv(m_shadowProgram.getUniformID(UNIFORM_AMBIENT), 1, (GLfloat*)&uniforms.m_ambientRad);
		glUniform1i(m_shadowProgram.getUniformID(UNIFORM_TEXTURE0), 0);
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
