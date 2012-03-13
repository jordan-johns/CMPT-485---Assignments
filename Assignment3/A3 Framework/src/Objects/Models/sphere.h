
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
 * Definition of sphere geometry.
 *
 * You can create spheres that are approximated by
 * different numbers of triangles by varying the
 * parameter of the init() function.
 *
 * init() will perform a recursive tessellation
 * of an octahedron to arrive at an approximation
 * to a sphere by triangles.
 *
 * The sphere is centered at (0,0,0) and has radius 1
 */

#pragma once
#ifndef __INC_SPHERE_H_
#define __INC_SPHERE_H_

#include "../geometry.h"
#include "../mesh.h"

namespace Object
{
namespace Models
{

class Sphere : public Geometry
{
protected:
	Mesh m_mesh;
public:
	Sphere();
	~Sphere();

	// nFaceIterations = # of times to recursively subdivide
	//  all faces on the sphere.
	// nFaceIterations of 0 will result in an octahedron
	bool init(const uint8_t nFacetIterations=3);

	virtual void rasterize() const;

	// Ray intersector virtuals
	virtual bool rayIntersects(const RayTracing::Ray_t &ray, const float t0, const float t1, RayTracing::HitInfo_t &hitinfo) const;
	virtual bool shadowsRay(const RayTracing::Ray_t &ray, const float t0, const float t1) const;
	virtual void hitProperties(const RayTracing::HitInfo_t &hitinfo, gml::vec3_t &normal, gml::vec2_t &texCoords) const;
};

}
} // namespace

#endif
