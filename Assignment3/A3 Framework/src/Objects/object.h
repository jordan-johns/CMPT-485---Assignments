
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
 * Class definition for a scene object.
 *
 * A single scene object includes its:
 *  - object to world transformation matrix
 *  - material properties
 *  - pointer to a Geometry object
 *
 * Note: The geometry object is not copied, this object
 *  will just use the pointer you give it. So, _do not_
 *  delete your geometry object until you are certain
 *  there are no scene objects that use it. This class
 *  does not try to delete the Geometry object it is
 *  given.
 */

#pragma once
#ifndef __INC_OBJECTS_OBJECT_H_
#define __INC_OBJECTS_OBJECT_H_

#include "../GML/gml.h"
#include "../Shaders/material.h"
#include "geometry.h"
#include "../RayTracing/rayintersector.h"
#include "../RayTracing/types.h"

namespace Object
{

class Object : public RayTracing::RayIntersector
{
protected:
	const Geometry *m_geometry;

	// Surface material
	Material::Material m_material;

	// object <-> world space transformations
	gml::mat4x4_t m_objectToWorld;
	gml::mat4x4_t m_objectToWorld_Normals; // Transforming normals
	gml::mat4x4_t m_worldToObject;
public:
	Object(const Geometry *geom, const Material::Material &mat,
			const gml::mat4x4_t &objectToWorld);
	~Object();

	void setTransform(const gml::mat4x4_t transform);
	gml::mat4x4_t getObjectToWorld() const { return m_objectToWorld; }
	Material::Material getMaterial() const { return m_material; }

	void setMaterial(const Material::Material &mat) { m_material = mat; }

	void rasterize() const { m_geometry->rasterize(); }

	// Ray intersector virtuals
	//   Both default to returning false.
	virtual bool rayIntersects(const RayTracing::Ray_t &ray, const float t0, const float t1, RayTracing::HitInfo_t &hitinfo) const;
	virtual bool shadowsRay(const RayTracing::Ray_t &ray, const float t0, const float t1) const;
	virtual void hitProperties(const RayTracing::HitInfo_t &hitinfo, gml::vec3_t &normal, gml::vec2_t &texCoords) const;
};

}

#endif
