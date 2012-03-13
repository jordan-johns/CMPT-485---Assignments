
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
 * Base class for all geometric objects
 *
 * Any new geometric objects you define (ex: plane, cone, cylinder)
 * must use this as its base class.
 */

#pragma once
#ifndef __INC_GEOMETRY_H_
#define __INC_GEOMETRY_H_

#include "../RayTracing/rayintersector.h"
#include "../RayTracing/types.h"

namespace Object
{

// Base class for all geometric models.
class Geometry
{
protected:
public:
	Geometry();
	virtual ~Geometry();

	// Rasterize this object via OpenGL
	virtual void rasterize() const = 0;

	// Ray intersector virtuals
	//   ASSUMES: ray is in object-space
	virtual bool rayIntersects(const RayTracing::Ray_t &ray, const float t0, const float t1, RayTracing::HitInfo_t &hitinfo) const = 0;
	virtual bool shadowsRay(const RayTracing::Ray_t &ray, const float t0, const float t1) const = 0;
	//   Gives back object-space normal
	virtual void hitProperties(const RayTracing::HitInfo_t &hitinfo, gml::vec3_t &normal, gml::vec2_t &texCoords) const = 0;
};

} // namespace

#endif
