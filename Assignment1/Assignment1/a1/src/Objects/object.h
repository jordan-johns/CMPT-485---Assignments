
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

namespace Object
{

class Object
{
protected:
	const Geometry *m_geometry;

	// Surface material
	Material::Material m_material;

	// object -> world space transformation
	gml::mat4x4_t m_objectToWorld;
public:
	Object(const Geometry *geom, const Material::Material &mat,
			const gml::mat4x4_t &objectToWorld);
	~Object();

	void setTransform(const gml::mat4x4_t transform);
	gml::mat4x4_t getObjectToWorld() const { return m_objectToWorld; }
	Material::Material getMaterial() const { return m_material; }

	void setMaterial(const Material::Material &mat) { m_material = mat; }

	void rasterize() const { m_geometry->rasterize(); }
};

}

#endif
