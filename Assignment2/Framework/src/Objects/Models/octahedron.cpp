
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

#include <cstdint>
#include "octahedron.h"

namespace Object
{
namespace Models
{

// Vertices, normals, and texture coordinates for an octahedron
static const uint32_t NUM_VERTS = 24;
static gml::vec3_t _verts[NUM_VERTS] =
{
		{0.0f, 1.0f, 0.0f},
		{1.0f, 0.0f, 0.0f},
		{0.0f, 0.0f, -1.0f},

		{0.0f, 1.0f, 0.0f},
		{0.0f, 0.0f, -1.0f},
		{-1.0f, 0.0f, 0.0f},

		{0.0f, 1.0f, 0.0f},
		{-1.0f, 0.0f, 0.0f},
		{0.0f, 0.0f, 1.0f},

		{0.0f, 1.0f, 0.0f},
		{0.0f, 0.0f, 1.0f},
		{1.0f, 0.0f, 0.0f},

		{1.0f, 0.0f, 0.0f},
		{0.0f, -1.0f, 0.0f},
		{0.0f, 0.0f, -1.0f},

		{0.0f, 0.0f, -1.0f},
		{0.0f, -1.0f, 0.0f},
		{-1.0f, 0.0f, 0.0f},

		{-1.0f, 0.0f, 0.0f},
		{0.0f, -1.0f, 0.0f},
		{0.0f, 0.0f, 1.0f},

		{0.0f, 0.0f, 1.0f},
		{0.0f, -1.0f, 0.0f},
		{1.0f, 0.0f, 0.0f}

};
static gml::vec3_t _normals[NUM_VERTS] =
{
		gml::normalize( gml::vec3_t(1.0,1.0,-1.0) ),
		gml::normalize( gml::vec3_t(1.0,1.0,-1.0) ),
		gml::normalize( gml::vec3_t(1.0,1.0,-1.0) ),

		gml::normalize( gml::vec3_t(-1.0,1.0,-1.0) ),
		gml::normalize( gml::vec3_t(-1.0,1.0,-1.0) ),
		gml::normalize( gml::vec3_t(-1.0,1.0,-1.0) ),

		gml::normalize( gml::vec3_t(-1.0,1.0,1.0) ),
		gml::normalize( gml::vec3_t(-1.0,1.0,1.0) ),
		gml::normalize( gml::vec3_t(-1.0,1.0,1.0) ),

		gml::normalize( gml::vec3_t(1.0,1.0,1.0) ),
		gml::normalize( gml::vec3_t(1.0,1.0,1.0) ),
		gml::normalize( gml::vec3_t(1.0,1.0,1.0) ),

		gml::normalize( gml::vec3_t(1.0,-1.0,-1.0) ),
		gml::normalize( gml::vec3_t(1.0,-1.0,-1.0) ),
		gml::normalize( gml::vec3_t(1.0,-1.0,-1.0) ),

		gml::normalize( gml::vec3_t(-1.0,-1.0,-1.0) ),
		gml::normalize( gml::vec3_t(-1.0,-1.0,-1.0) ),
		gml::normalize( gml::vec3_t(-1.0,-1.0,-1.0) ),

		gml::normalize( gml::vec3_t(-1.0,-1.0,1.0) ),
		gml::normalize( gml::vec3_t(-1.0,-1.0,1.0) ),
		gml::normalize( gml::vec3_t(-1.0,-1.0,1.0) ),

		gml::normalize( gml::vec3_t(1.0,-1.0,1.0) ),
		gml::normalize( gml::vec3_t(1.0,-1.0,1.0) ),
		gml::normalize( gml::vec3_t(1.0,-1.0,1.0) )
};
static gml::vec2_t _texcoords[NUM_VERTS] =
{
		{0.125f, 0.0f},
		{0.0f, 0.5f},
		{0.25f, 0.5f},

		{0.375f, 0.0f},
		{0.25f, 0.5f},
		{0.5f, 0.5f},

		{0.625f, 0.0f},
		{0.5f, 0.5f},
		{0.75f, 0.5f},

		{0.875f, 0.0f},
		{0.75f, 0.5f},
		{1.0f, 0.5f},

		{0.0f, 0.5f},
		{0.125f, 1.0f},
		{0.25f, 0.5f},

		{0.25f, 0.5f},
		{0.375f, 1.0f},
		{0.5f, 0.5f},

		{0.5f, 0.5f},
		{0.625f, 1.0f},
		{0.75f, 0.5f},

		{0.75f, 0.5f},
		{0.875f, 1.0f},
		{1.0f, 0.5f}
};
static GLuint _indices[8*3] =
{
		0, 1, 2,
		3, 4, 5,
		6, 7, 8,
		9, 10, 11,
		12, 13, 14,
		15, 16, 17,
		18, 19, 20,
		21, 22, 23
};

Octahedron::Octahedron() {}
Octahedron::~Octahedron() {}

bool Octahedron::init()
{
	return m_mesh.init(GL_TRIANGLES, NUM_VERTS, _verts, _normals, _texcoords, 8*3, _indices);
}

void Octahedron::rasterize() const
{
	m_mesh.rasterize();
}

}
}
