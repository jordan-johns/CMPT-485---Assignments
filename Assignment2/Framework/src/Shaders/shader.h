
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

/*
 * Class definition for a Shader
 *
 * This is the base class of all Shaders that you will define.
 */

#pragma once

#ifndef __INC_SHADER_H__
#define __INC_SHADER_H__

#include "../GL3/gl3.h"
#include "glprogram.h"
namespace Shader
{

class Shader
{
protected:
	// Handle for the GLSL program
	GLProgram m_program;
	// Handle for the GLSL program that does shadowmapping
	GLProgram m_shadowProgram;

	// true iff the GLSL program has been compiled & linked without
	// error
	bool m_isReady;
	// True iff the program for shadow mapping is ready
	bool m_isShadowReady;

public:
	Shader();
	virtual ~Shader();

	inline bool getIsReady(const bool useShadow=false) const { return useShadow?m_isShadowReady:m_isReady; }
	inline GLuint getID(const bool useShadow=false) const { return useShadow?m_shadowProgram.getID():m_program.getID(); }

	// Bind/unbind the GLSL program for this shader
	virtual void bindGL(const bool useShadow=false) const;
	virtual void unbindGL() const;

	// Set the uniforms for the shader.
	// Assumes that the shader has already been bound.
	// Returns true iff successful
	virtual bool setUniforms(const GLProgUniforms &uniforms, const bool usingShadow=false) const;
};

} // namespace

#endif
