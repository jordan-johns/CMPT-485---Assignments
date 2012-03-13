
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
 * Definition of a Manager for shader objects.
 *
 * When initialized, a Manager object will/should allocate
 * an object of each of the Shaders that have been implemented.
 *
 * When a rendering thread needs a Shader it should query the
 * manager object with the material properties of the object
 * that it needs to Shade.
 *
 * See Shaders/material.h for information about materials.
 */

#pragma once
#ifndef __INC_SHADER_MANAGER_H_
#define __INC_SHADER_MANAGER_H_

#include "shader.h"
#include "material.h"

namespace Shader
{

class Manager
{
protected:
	Shader **m_shaders;
	int m_nShaders;
public:
	Manager();
	~Manager();

	// Must be called before trying to use.
	// Return true iff successfully initialized
	bool init();

	// Given material properties for an Object, return a GLSL
	// shader that will perform the desired lighting calculations
	const Shader* getShader(const Material::Material &mat) const;

	// Get the depth-only shader; shader that only outputs fragment depths.
	const Shader* getDepthShader() const;
};

}

#endif
