
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

#include <cassert>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include "sphere.h"

#include "../object.h"

namespace Object
{
namespace Models
{

// Vertices, normals, and texture coordinates for an octahedron
// We'll recursively subdivide all faces
static const uint32_t NUM_L0_VERTS = 13;
static gml::vec3_t level0Verts[NUM_L0_VERTS] =
{
		{0.0f, 1.0f, 0.0f},
		{0.0f, 1.0f, 0.0f},
		{0.0f, 1.0f, 0.0f},
		{0.0f, 1.0f, 0.0f},
		{1.0f, 0.0f, 0.0f},
		{0.0f, 0.0f, -1.0f},
		{-1.0f, 0.0f, 0.0f},
		{0.0f, 0.0f, 1.0f},
		{1.0f, 0.0f, 0.0f},
		{0.0f, -1.0f, 0.0f},
		{0.0f, -1.0f, 0.0f},
		{0.0f, -1.0f, 0.0f},
		{0.0f, -1.0f, 0.0f}
};
static gml::vec2_t level0texcoords[NUM_L0_VERTS] =
{
		{0.125f, 0.0f},
		{0.375f, 0.0f},
		{0.625f, 0.0f},
		{0.875f, 0.0f},
		{0.0f, 0.5f},
		{0.25f, 0.5f},
		{0.5f, 0.5f},
		{0.75f, 0.5f},
		{1.0f, 0.5f},
		{0.125f, 1.0f},
		{0.375f, 1.0f},
		{0.625f, 1.0f},
		{0.875f, 1.0f}
};
static GLuint level0indices[8*3] =
{
		0, 4, 5,
		1, 5, 6,
		2, 6, 7,
		3, 7, 8,
		9, 5, 4,
		10, 6, 5,
		11, 7, 6,
		12, 8, 7
};

Sphere::Sphere() {}
Sphere::~Sphere() {}

struct SphereInfo
{
	gml::vec3_t *posn;
	gml::vec2_t *texcoords;
	GLuint *faces;
	GLuint numVerts;
	GLuint numFaces;
#if !defined(NDEBUG)
	GLuint maxVerts, maxFaces;
#endif
};

static void tessellate(SphereInfo &info, const uint8_t currIter, const uint8_t nIters, const GLuint v0, const GLuint v1, const GLuint v2);

bool Sphere::init(const uint8_t nFacetIterations)
{
	// The tessellation will generate:
	//  8 x 4^iterations faces
	// The Euler Characteristic formula for the sphere gives: V - E + f = 2
	// The mesh is all triangles, so: 2E = 3f
	//  => V = 2 + f/2
	// But, I added 6 more vertices on account of texture coordinates
	//  => V = 8 + f/2
	// Add another \sum_i=0^{nIterations} 2^i points on account of the duplicate
	// x=0 edge for texture coordinates

	gml::vec3_t *positions; // also normals
	gml::vec2_t *texcoords;
	GLuint *indices;


	GLuint numFaces = 8 * (1 << (2*nFacetIterations));
	GLuint numVerts = 6 + 2 + numFaces / 2 + ( (1<<(nFacetIterations+1)) - 1);

	positions = (gml::vec3_t*)malloc((sizeof(gml::vec3_t) + sizeof(gml::vec2_t))*numVerts + sizeof(GLuint)*3*numFaces);
	if (!positions)
	{
		return false;
	}
	texcoords = (gml::vec2_t*)(positions + numVerts);
	indices = (GLuint*)(texcoords + numVerts);

	memcpy(positions, level0Verts, sizeof(gml::vec3_t)*NUM_L0_VERTS);
	memcpy(texcoords, level0texcoords, sizeof(gml::vec2_t)*NUM_L0_VERTS);

	// Tessellate
	SphereInfo info;
	info.posn = positions;
	info.texcoords = texcoords;
	info.numVerts = NUM_L0_VERTS;
	info.faces = indices;
	info.numFaces = 0;
#if !defined(NDEBUG)
	info.maxVerts = numVerts;
	info.maxFaces = numFaces;
#endif

	if (nFacetIterations > 0)
	{
		for (int i=0; i<8; i++)
		{
			tessellate(info, 0, nFacetIterations, level0indices[3*i],level0indices[3*i+1],level0indices[3*i+2]);
		}
#if !defined(NDEBUG)
		assert(info.numVerts == numVerts);
		assert(info.numFaces == numFaces);
#endif
	}
	else
	{
		info.numFaces = 8;
		memcpy(indices, level0indices, sizeof(GLuint)*8*3);
	}

	// Create the mesh
	bool success = m_mesh.init(GL_TRIANGLES, numVerts, positions, positions, texcoords, numFaces*3, indices);

	// All done. Exit
	free(positions);
	return success;
}

void Sphere::rasterize() const
{
	m_mesh.rasterize();
}

bool Sphere::rayIntersects(const RayTracing::Ray_t &ray, const float t0, const float t1, RayTracing::HitInfo_t &hitinfo) const
{
	// TODO!!
	// Calculate the ray-sphere intersection.
	//  Note: ray is given in _object space_ coordinates
	//  If an intersection is found, then return true and set the appropriate fields
	// of the hitinfo struct.
	//  hitinfo struct should _not_ be altered unless there is definitely an intersection.
	gml::vec3_t o_c = gml::sub(ray.o, gml::vec3_t(0,0,0));

	// Initialize the 3 terms from the formula breakdown.
	float A = gml::dot(ray.d, ray.d);
	float B = 2.0f * gml::dot(ray.d, o_c);
	float C = gml::dot(o_c, o_c) - 1.0f;

	// Discriminant from quadratic formula (using previous terms).
	float discriminant = sqrt(pow(B, 2) - 4.0f*A*C);

	// discriminant cannot be zero.
	if (discriminant < 0)
	{
		// fail.
		return false;
	}


	float newt0 = 0.0f;
	float newt1 = 0.0f;
	newt0 = (-B + discriminant) / (2.0f*A);
	newt1 = (-B - discriminant) / (2.0f*A);

	float smallT = newt0;
	if (newt1 < newt0)
	{
		smallT = newt1;
	}

	// We have a hit!
	if (smallT >= t0 && smallT <= t1)
	{
		// Record the hit distance.
		hitinfo.hitDist = smallT;

		// Record the hit distance.
		gml::vec3_t hitPos = gml::add(ray.o, gml::scale(smallT, ray.d));
		hitinfo.sphere.hitPos = hitPos;

		return true;
	}


	return false;
}

bool Sphere::shadowsRay(const RayTracing::Ray_t &ray, const float t0, const float t1) const
{
	// TODO!!
	//  Return true if and only if the ray intersects this sphere.
	//  ray is given in _object space_ coordinates.

	gml::vec3_t o_c = gml::sub(ray.o, gml::vec3_t(0,0,0));

	// Initialize the 3 terms from the formula breakdown.
	float A = gml::dot(ray.d, ray.d);
	float B = 2.0f * gml::dot(ray.d, o_c);
	float C = gml::dot(o_c, o_c) - 1.0f;

	// Discriminant from quadratic formula (using previous terms).
	float discriminant = sqrtf(powf(B, 2.0f) - 4.0f*A*C);



	// discriminant cannot be zero.
	if (discriminant < 0)
	{
			// fail.
			return false;
	}

	float newt0 = 0.0f;
	float newt1 = 0.0f;
	newt0 = (-B + discriminant) / (2.0f*A);
	newt1 = (-B - discriminant) / (2.0f*A);

	float smallT = newt0;

	if (newt1 < newt0)
	{
			smallT = newt1;
	}
	if (smallT >= t0 && smallT <= t1)
	{
			return true;
	}


	return false;
}

static inline gml::vec2_t getTexCoords(const gml::vec3_t &position)
{
        gml::vec2_t texcoords;
        texcoords.s = ( atan2(position.z, -position.x) / M_PI + 1 ) / 2.0f;
        texcoords.t = ( asin(-position.y )/M_PI + 1) / 2;
        return texcoords;
}

void Sphere::hitProperties(const RayTracing::HitInfo_t &hitinfo, gml::vec3_t &normal, gml::vec2_t &texCoords) const
{
        // TODO
        // Calculate the normal and texture coordinates of a hit point.
        //  Note: It is up to you to figure out what information should be recorded in hitinfo.sphere
        // to simplify this calculation.
        //  Note2: You can/should use the getTexCoords() function, above, to calculate the texture
        //  coordinates of a point on the sphere given the object-space position of a point on the sphere.
        texCoords = getTexCoords(hitinfo.sphere.hitPos);
        normal = gml::normalize(hitinfo.sphere.hitPos);
}



static const float EPSILON = 1e-5;

// We don't want duplicate vertices in the mesh, so this function will find whether or not
// a vertex has already been generated. If it has, then it will return the index of that vertex.
// If it's not there, then add it to the list.
static GLuint findVertex(const gml::vec3_t *v, const gml::vec2_t *tc, SphereInfo &info)
{
	for (GLuint i=0; i<info.numVerts; i++)
	{
		if (gml::length(gml::sub(*v,info.posn[i])) < EPSILON &&
			gml::length(gml::sub(*tc,info.texcoords[i])) < EPSILON)
			return i;
	}
#if !defined(NDEBUG)
	assert(info.numVerts < info.maxVerts);
#endif
	info.posn[info.numVerts] = *v;
	info.texcoords[info.numVerts] = *tc;
	info.numVerts += 1;
	return info.numVerts - 1;
}


static void tessellate(SphereInfo &info, const uint8_t currIter,  const uint8_t nIters, const GLuint i0, const GLuint i1, const GLuint i2)
{
	if (currIter == nIters)
	{
#if !defined(NDEBUG)
		assert(info.numFaces < info.maxFaces);
#endif
		GLuint face = info.numFaces;
		info.faces[3*face] = i0;
		info.faces[3*face+1] = i1;
		info.faces[3*face+2] = i2;
		info.numFaces += 1;
	}
	else
	{
		GLuint i01, i12, i20;
		gml::vec3_t v01, v12, v20;
		gml::vec2_t tc01, tc12, tc20;

		v01 = gml::normalize( gml::add(info.posn[i0], info.posn[i1]) );
		v12 = gml::normalize( gml::add(info.posn[i1], info.posn[i2]) );
		v20 = gml::normalize( gml::add(info.posn[i2], info.posn[i0]) );

		// Annoying hack to deal with points along z=0 with x>0 having an
		// s texture coordinate of either 0 or 1, depending on which
		// side of the line the triangle is on.
		bool positiveZ = (v01.z > 0) || (v12.z > 0) || (v20.z > 0);
		if (!positiveZ && v01.x > 0 && fabs(v01.z) < EPSILON)
			v01.z = -0.0f;
		if (!positiveZ && v12.x > 0 && fabs(v12.z) < EPSILON)
			v12.z = -0.0f;
		if (!positiveZ && v20.x > 0 && fabs(v20.z) < EPSILON)
			v20.z = -0.0f;

		tc01 = getTexCoords(v01);
		tc12 = getTexCoords(v12);
		tc20 = getTexCoords(v20);

		i01 = findVertex(&v01, &tc01, info);
		i12 = findVertex(&v12, &tc12, info);
		i20 = findVertex(&v20, &tc20, info);

		tessellate(info, currIter+1, nIters, i0,i01,i20);
		tessellate(info, currIter+1, nIters, i01,i1,i12);
		tessellate(info, currIter+1, nIters, i01,i12,i20);
		tessellate(info, currIter+1, nIters, i20,i12,i2);
	}
}


}
}
