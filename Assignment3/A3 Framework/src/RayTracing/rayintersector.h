
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
#ifndef _INC_RAYINTERSECTOR_H_
#define _INC_RAYINTERSECTOR_H_

#include "types.h"

namespace RayTracing
{

class RayIntersector
{
protected:
public:
	RayIntersector();
	virtual ~RayIntersector();

	// Determine whether the ray segment from r(t0) to r(t1) intersects the object.
	// Return:
	//   return value -- true iff ray intersects object in range [t0,t1]
	//   hitinfo -- set to cache information useful for future shading only if returning true
	virtual bool rayIntersects(const Ray_t &ray, const float t0, const float t1, HitInfo_t &hitinfo) const = 0;

	// Determine whether this object shadows the given ray segment from r(t0) to r(t1)
	// Return:
	//   return value -- true iff this object shadows the ray
	virtual bool shadowsRay(const Ray_t &ray, const float t0, const float t1) const = 0;

	// Given the hit information
	// Return the world-space normal of the hit point, and its texture coordinates
	//  normal is outward-facing for the surface
	//  -- Note: Only sets the texture coordinates if the material properties of the
	//       object hit call for texturing
	virtual void hitProperties(const HitInfo_t &hitinfo, gml::vec3_t &normal, gml::vec2_t &texCoords) const = 0;

};

}

#endif
