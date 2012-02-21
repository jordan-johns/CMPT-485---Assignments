
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

#include "object.h"
#include "../glUtils.h"

namespace Object
{

Object::Object(const Geometry *geom, const Material::Material &mat,
		const gml::mat4x4_t &objectToWorld)
{
	// Note: We don't own this item, so we best not delete it!
	m_geometry = geom;
	m_material = mat;
	m_objectToWorld = objectToWorld;
}
Object::~Object()
{
}

}
