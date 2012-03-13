
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

#pragma once
#ifndef _INC_RAYSHADER_H_
#define _INC_RAYSHADER_H_

#include "../GML/gml.h"
#include "../Shaders/material.h"

namespace RayTracing
{

// Structure that is passed to Shader::shade() to
// provide data for calculating point color
typedef struct _ShaderValues
{
	gml::vec3_t lightRad; // Light radiance
	gml::vec3_t lightDir; // Direction from point to light
	gml::vec3_t e; // View direction
	gml::vec3_t p; // Point being shaded (world-space)
	gml::vec3_t n; // Normal of p (world-space)
	gml::vec2_t tex; // Texture coordinates of p
	const Material::Material &mat; // Material properties of p

	_ShaderValues(const Material::Material &m) : mat(m) { }
} ShaderValues;

class Shader
{
protected:
public:
	Shader() {};
	virtual ~Shader() {};

	//  Given material properties, incoming radiance, and geometric information about
	// the intersection point and the incoming radiance (all in vals). Calculate
	// the "shade" (i.e. radiance) of a point.
	//  Return value: The calculated shade/color/radiance
	virtual gml::vec3_t shade(const ShaderValues &vals) const = 0;
};

}

#endif
