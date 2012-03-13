
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

#include "../../GML/gml.h"
#include "plane.h"

namespace Object
{
namespace Models
{

static gml::vec3_t _verts[4] =
{
		{-1.0, 0.0, -1.0},
		{-1.0, 0.0, 1.0},
		{1.0, 0.0, 1.0},
		{1.0, 0.0, -1.0}
};
static gml::vec3_t _normals[4] =
{
		{0.0, 1.0, 0.0},
		{0.0, 1.0, 0.0},
		{0.0, 1.0, 0.0},
		{0.0, 1.0, 0.0}
};
static gml::vec2_t _texCoords[4] =
{
		{0.0, 0.0},
		{0.0, 1.0},
		{1.0, 1.0},
		{1.0, 0.0}
};
static GLuint _indices[] =
{
		0, 1, 2,
		2, 3, 0
};

Plane::Plane() {}
Plane::~Plane() {}

bool Plane::init()
{
	return m_mesh.init(GL_TRIANGLES, 4, _verts, _normals, _texCoords, 2*3, _indices);
}

void Plane::rasterize() const
{
	m_mesh.rasterize();
}

bool Plane::rayIntersects(const RayTracing::Ray_t &ray, const float t0, const float t1, RayTracing::HitInfo_t &hitinfo) const
{
	gml::vec3_t E1(0.0f, 0.0f, 2.0f);
	gml::vec3_t E2(2.0f, 0.0f, 0.0f);

	gml::vec3_t P = gml::cross( ray.d, E2 );

	float detM = gml::dot(P, E1);

	if (fabs(detM) < 1e-4)
	{
		return false;
	}

	gml::vec3_t T = gml::sub( ray.o, _verts[0] );

	float u = gml::dot( P, T ) / detM;
	if ( u < 0.0f || 1.0f < u )
	{
		return false;
	}

	gml::vec3_t TxE1 = gml::cross(T, E1);
	float v = gml::dot( TxE1, ray.d ) / detM;
	if ( v < 0.0f || 1.0f < v)
	{
		return false;
	}

	float t = gml::dot( TxE1, E2 ) / detM;
	if (t < t0 || t1 < t)
	{
		return false;
	}

	hitinfo.hitDist =  t;

	hitinfo.plane.u = u;
	hitinfo.plane.v = v;

	return true;
}
bool Plane::shadowsRay(const RayTracing::Ray_t &ray, const float t0, const float t1) const
{
	gml::vec3_t E1(0.0f, 0.0f, 2.0f);
	gml::vec3_t E2(2.0f, 0.0f, 0.0f);

	gml::vec3_t P = gml::cross( ray.d, E2 );

	float detM = gml::dot(P, E1);

	if (fabs(detM) < 1e-4)
	{
		return false;
	}

	gml::vec3_t T = gml::sub( ray.o, _verts[0] );

	float u = gml::dot( P, T ) / detM;
	if ( u < 0.0f || 1.0f < u )
	{
		return false;
	}

	gml::vec3_t TxE1 = gml::cross(T, E1);
	float v = gml::dot( TxE1, ray.d ) / detM;
	if ( v < 0.0f || 1.0f < v)
	{
		return false;
	}

	float t = gml::dot( TxE1, E2 ) / detM;
	if (t < t0 || t1 < t)
	{
		return false;
	}

	return true;
}
void Plane::hitProperties(const RayTracing::HitInfo_t &hitinfo, gml::vec3_t &normal, gml::vec2_t &texCoords) const
{
	texCoords = gml::vec2_t(hitinfo.plane.u, hitinfo.plane.v);
	normal = gml::vec3_t(0.0f, 1.0f, 0.0f);
}


}
}
