
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
#include "shader.h"

namespace Shader
{

Shader::Shader()
{
	m_isReady = false;
	m_isShadowReady = false;
}
Shader::~Shader() {}
void Shader::bindGL(const bool useShadow) const
{
	if (!useShadow)
	{
		if (m_isReady) m_program.bind();
#if !defined(NDEBUG)
		else
		{
			fprintf(stderr, "Attempt to bind non-ready shader\n");
		}
#endif
	}
	else
	{
		if (m_isShadowReady) m_shadowProgram.bind();
#if !defined(NDEBUG)
		else
		{
			fprintf(stderr, "Attempt to bind non-ready shader\n");
		}
#endif
	}
}
void Shader::unbindGL() const
{
	m_program.unbind();
}

bool Shader::setUniforms(const GLProgUniforms &uniforms, const bool usingShadow) const
{
	return true;
}

}
