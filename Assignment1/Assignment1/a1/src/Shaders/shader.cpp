
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
}
Shader::~Shader() {}
void Shader::bindGL() const
{
	if (m_isReady) m_program.bind();
}
void Shader::unbindGL() const
{
	if (m_isReady) m_program.unbind();
}

bool Shader::setUniforms(const GLProgUniforms &uniforms) const
{
	return true;
}

}
