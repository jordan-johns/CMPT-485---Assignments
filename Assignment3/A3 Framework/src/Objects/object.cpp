
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

#include <cassert>

namespace Object
{

Object::Object(const Geometry *geom, const Material::Material &mat,
		const gml::mat4x4_t &objectToWorld)
{
	// Note: We don't own this item, so we best not delete it!
	m_geometry = geom;
	m_material = mat;
	m_objectToWorld = objectToWorld;
	m_worldToObject = gml::inverse(objectToWorld);
	m_objectToWorld_Normals = gml::transpose(m_worldToObject);
}
Object::~Object()
{
}

bool Object::rayIntersects(const RayTracing::Ray_t &ray, const float t0, const float t1, RayTracing::HitInfo_t &hitinfo) const
{
	// 1) Transform the ray into object space
	RayTracing::Ray_t _ray;
	_ray.o = gml::extract3( gml::mul(m_worldToObject, gml::vec4_t(ray.o,1.0) ) );
	_ray.d = gml::extract3( gml::mul(m_worldToObject, gml::vec4_t(ray.d,0.0) ) );

	if ( m_geometry->rayIntersects(_ray, t0, t1, hitinfo) )
	{
		hitinfo.objHit = this;
		return true;
	}
	return false;
}

bool Object::shadowsRay(const RayTracing::Ray_t &ray, const float t0, const float t1) const
{
	// 1) Transform the ray into object space
	RayTracing::Ray_t _ray;
	_ray.o = gml::extract3( gml::mul(m_worldToObject, gml::vec4_t(ray.o,1.0) ) );
	_ray.d = gml::extract3( gml::mul(m_worldToObject, gml::vec4_t(ray.d,0.0) ) );

	return m_geometry->shadowsRay(_ray, t0, t1);
}

void Object::hitProperties(const RayTracing::HitInfo_t &hitinfo, gml::vec3_t &normal, gml::vec2_t &texCoords) const
{
	gml::vec3_t _normal;
	m_geometry->hitProperties(hitinfo, _normal, texCoords);
	normal = gml::normalize( gml::extract3( gml::mul( m_objectToWorld_Normals, gml::vec4_t(_normal, 0.0f) ) ) );
}

}
