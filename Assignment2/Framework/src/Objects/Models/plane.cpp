
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


}
}
