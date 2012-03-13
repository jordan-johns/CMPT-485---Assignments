
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

#include "depth.h"
#include "../../glUtils.h"
#include "../../ShadowMapping/shadowmap.h"

namespace Shader
{
namespace Constant
{

static const char vertShader[] =
		"#version 330\n"
		"uniform mat4 " UNIF_MODELVIEW ";\n"
		"uniform mat4 " UNIF_PROJECTION ";\n"
		"layout (location=0) in vec3 position;\n"
		"smooth out float distToLight;\n" // distance to the light
		"void main(void) {\n"
		" vec4 p = " UNIF_MODELVIEW " * vec4(position, 1.0);\n"
		" gl_Position = " UNIF_PROJECTION " * p;\n"
		// Output linear-scale depth, instead of non-linear perspective depth
		//  Also, this places the "near plane" at the light itself w.r.t. depth
		" gl_Position.z = gl_Position.w * (2.0 * (-p.z / " SHADOWMAP_FAR_STR ") - 1);\n"
		// Calculate the depth that will be stored to the depth buffer
		" distToLight = length(p.xyz) / " SHADOWMAP_FAR_STR ";\n"
		"}";
static const char fragShader[] =
		"#version 330\n"
		"in float distToLight;\n" // distance to the light; 1 => at max distance
		"void main(void) {\n"
		// gl_FragDepth is clamped and directly written to the depth buffer
		// Might have to add a small bias to avoid self-shadowing
		" const float bias = 0.0025;\n" //0.0125f;\n"
		" gl_FragDepth = distToLight + bias;\n"
		"}";

Depth::Depth()
{
	// Try to create, compile, & link a GLSL program using the source
	// you give it.
	if ( !m_program.init(vertShader, fragShader) || isGLError() )
	{
		fprintf(stderr, "ERROR: Depth failed to initialize\n");
	}
	// Make sure that every uniform that you are using in your shader
	// is given a handle.
	// variable names that do not correspond with a uniform will have
	// been given the value -1
	m_isReady =
			(m_program.getUniformID(UNIFORM_MODELVIEW) >= 0) &&
			(m_program.getUniformID(UNIFORM_PROJECTION) >= 0);
}
Depth::~Depth() {}


bool Depth::setUniforms(const GLProgUniforms &uniforms, const bool usingShadow) const
{
    glUniformMatrix4fv(m_program.getUniformID(UNIFORM_MODELVIEW), 1, GL_FALSE, (GLfloat*)&uniforms.m_modelView);
    glUniformMatrix4fv(m_program.getUniformID(UNIFORM_PROJECTION), 1, GL_FALSE, (GLfloat*)&uniforms.m_projection);

 	return !isGLError();
}

}
}
