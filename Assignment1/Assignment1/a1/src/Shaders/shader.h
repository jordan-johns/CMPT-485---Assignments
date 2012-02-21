
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

#include <GL/gl.h>
#include "glprogram.h"
namespace Shader
{

class Shader
{
protected:
	// Handle for the GLSL program
	GLProgram m_program;

	// true iff the GLSL program has been compiled & linked without
	// error
	bool m_isReady;

	// Get the handle to one of the shader uniform variables.
	inline GLint getUniformLoc(UniformVars var) { return m_program.getUniformID(var); }
public:
	Shader();
	virtual ~Shader();

	inline bool getIsReady() const { return m_isReady; }
	inline GLuint getID() const { return m_program.getID(); }

	// Bind/unbind the GLSL program for this shader
	virtual void bindGL() const;
	virtual void unbindGL() const;

	// Set the uniforms for the shader.
	// Assumes that the shader has already been bound.
	// Returns true iff successful
	virtual bool setUniforms(const GLProgUniforms &uniforms) const;
};

} // namespace

#endif
