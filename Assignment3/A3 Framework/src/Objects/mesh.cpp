
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

#include "../GL3/gl3w.h"
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "mesh.h"
#include "../glUtils.h"

namespace Object
{


Mesh::Mesh()
{
	m_vertArrayObj = 0;
	memset(m_vertBuffers, 0x00, sizeof(m_vertBuffers));
	m_vertPositions = 0;
	m_vertNormals = 0;
	m_vertTexcoords = 0;
	m_indices = 0;

	m_primitiveType = GL_TRIANGLES;
}

Mesh::~Mesh()
{
	destroy();
}

void Mesh::destroy()
{
	// Have to delete the VAO & VBOs to avoid leaking resources.
	glDeleteVertexArrays(1, &m_vertArrayObj);
	glDeleteBuffers(Shader::NUM_VERTEX_ATTRIBS, m_vertBuffers);

	// All geometry data was allocated contiguously with one malloc call
	if (m_vertPositions) free(m_vertPositions);
	m_vertPositions = 0;
	m_vertNormals = 0;
	m_vertTexcoords = 0;
	m_indices = 0;
}

bool Mesh::init(GLenum primitive,
		GLuint numVerts, const gml::vec3_t *positions, const gml::vec3_t *normals, const gml::vec2_t *texcoords,
		GLuint numIndices, const GLuint *indices)
{
	assert(positions != 0);
	assert(normals != 0);
	assert(texcoords != 0);
	assert(indices != 0);

	destroy();

	m_primitiveType = primitive;

	// Create storage for vertex positions, normals, texture coordinates, and triangle indices
	m_vertPositions = (gml::vec3_t*)malloc((2*sizeof(gml::vec3_t)+sizeof(gml::vec2_t))*numVerts + sizeof(GLuint)*numIndices);
	if (m_vertPositions == 0)
	{
		fprintf(stderr, "ERROR(Mesh): Out of memory\n");
		return false;
	}
	m_vertNormals = m_vertPositions + numVerts;
	m_vertTexcoords = (gml::vec2_t*)(m_vertNormals + numVerts);
	m_indices = (GLuint*)(m_vertTexcoords + numVerts);

	// Copy the given data into a local storage
	memcpy(m_vertPositions, positions, sizeof(gml::vec3_t)*numVerts);
	memcpy(m_vertNormals, normals, sizeof(gml::vec3_t)*numVerts);
	memcpy(m_vertTexcoords, texcoords, sizeof(gml::vec2_t)*numVerts);
	memcpy(m_indices, indices, sizeof(GLuint)*numIndices);
	m_numIndices = numIndices;

	// To render objects in OpenGL you first create a "Vertex Array Object" (VAO)
	// The VAO is basically a container for the object's geometry
	// So, create one VAO
	glGenVertexArrays(1, &m_vertArrayObj);
	if (isGLError())
	{
		return false;
	}

	// To set up the VAO, we have to bind it
	glBindVertexArray(m_vertArrayObj);
	if (isGLError())
	{
		return false;
	}

	// Each vertex attribute that is going to be passed to a GLSL shader must be passed
	// in a buffer. There are many different forms these buffers can take; this
	// is just one of them

	// We're going to use one buffer per vertex attribute.
	// So, ask OpenGL to create a buffer per attribute for us, and store it
	// in the array m_vertBuffers
	//  These buffers are also called 'Vertex Buffer Objects' (VBO)
	glGenBuffers(Shader::NUM_VERTEX_ATTRIBS, m_vertBuffers);

	// Bind the attribute buffer that we'll use for vertex positions
	//  Binding makes the buffer the active buffer
	glBindBuffer(GL_ARRAY_BUFFER, m_vertBuffers[Shader::VERTEX_POSITION]);
	//  Copy the vertex position data into the active buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(gml::vec3_t)*numVerts, m_vertPositions, GL_STATIC_DRAW);
	//  Tell OpenGL that this buffer should be mapped to the vertex attribute
	// at location 'Shader::VERTEX_POSITION'
	glVertexAttribPointer(Shader::VERTEX_POSITION, 3, GL_FLOAT, GL_FALSE, 0, 0);
	//  We have to enable attributes at the location 'Shader::VERTEX_POSITION'
	// If we don't, then the data will not actually be passed to the GLSL shader.
	glEnableVertexAttribArray(Shader::VERTEX_POSITION);
	if (isGLError())
	{
		return false;
	}

	// Same as above, but for the vertex normals
	glBindBuffer(GL_ARRAY_BUFFER, m_vertBuffers[Shader::VERTEX_NORMAL]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(gml::vec3_t)*numVerts, m_vertNormals, GL_STATIC_DRAW);
	glVertexAttribPointer(Shader::VERTEX_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(Shader::VERTEX_NORMAL);
	if (isGLError())
	{
		return false;
	}

	// Same as above, but for the texture coordinates.
	// Not all of our objects will use texture coordinates, but we still
	// have to bind them for the objects that will.
	glBindBuffer(GL_ARRAY_BUFFER, m_vertBuffers[Shader::VERTEX_TEXCOORDS]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(gml::vec2_t)*numVerts, m_vertTexcoords, GL_STATIC_DRAW);
	glVertexAttribPointer(Shader::VERTEX_TEXCOORDS, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(Shader::VERTEX_TEXCOORDS);
	if (isGLError())
	{
		return false;
	}

	// Binding VAO 0 will unbind whatever VAO is currently bound
	glBindVertexArray(0);

	return true;
}


void Mesh::rasterize() const
{
	assert(m_vertArrayObj != 0);

	// To render/rasterize the object, we first have to bind the VAO
	// for the geometry.
	glBindVertexArray(m_vertArrayObj);
	if (!isGLError())
	{
		// Tell OpenGL to render the geometry defined by the index array
		// using the data in the currently bound VAO
		glDrawElements(m_primitiveType, m_numIndices, GL_UNSIGNED_INT, m_indices);
		isGLError();
	}
	glBindVertexArray(0);

}

bool Mesh::rayIntersectsTriangle(GLuint i0, GLuint i1, GLuint i2,
		const RayTracing::Ray_t &ray, const float t0, const float t1, RayTracing::HitInfo_t &hitinfo) const
{
	gml::vec3_t E1 = gml::sub( m_vertPositions[i1], m_vertPositions[i0] );
	gml::vec3_t E2 = gml::sub( m_vertPositions[i2], m_vertPositions[i0] );

	gml::vec3_t P = gml::cross( ray.d, E2 );

	float detM = gml::dot(P, E1);

	if (fabs(detM) < 1e-4)
	{
		return false;
	}

	gml::vec3_t T = gml::sub( ray.o, m_vertPositions[i0] );

	float u = gml::dot( P, T ) / detM;
	if ( u < 0.0f || 1.0f < u )
	{
		return false;
	}

	gml::vec3_t TxE1 = gml::cross(T, E1);
	float v = gml::dot( TxE1, ray.d ) / detM;
	if ( v < 0.0f || 1.0f < (v+u) )
	{
		return false;
	}

	float t = gml::dot( TxE1, E2 ) / detM;
	if (t < t0 || t1 < t)
	{
		return false;
	}

	hitinfo.hitDist = t;

	hitinfo.mesh.i0 = i0;
	hitinfo.mesh.i1 = i1;
	hitinfo.mesh.i2 = i2;
	hitinfo.mesh.u = u;
	hitinfo.mesh.v = v;

	return true;
}

bool Mesh::rayShadowsTriangle(GLuint i0, GLuint i1, GLuint i2,
		const RayTracing::Ray_t &ray, const float t0, const float t1) const
{
	gml::vec3_t E1 = gml::sub( m_vertPositions[i1], m_vertPositions[i0] );
	gml::vec3_t E2 = gml::sub( m_vertPositions[i2], m_vertPositions[i0] );

	gml::vec3_t P = gml::cross( ray.d, E2 );

	float detM = gml::dot(P, E1);

	if (fabs(detM) < 1e-4)
	{
		return false;
	}

	gml::vec3_t T = gml::sub( ray.o, m_vertPositions[i0] );

	float u = gml::dot( P, T ) / detM;
	if ( u < 0.0f || 1.0f < u )
	{
		return false;
	}

	gml::vec3_t TxE1 = gml::cross(T, E1);
	float v = gml::dot( TxE1, ray.d ) / detM;
	if ( v < 0.0f || 1.0f < (v+u) )
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

// Ray intersector virtuals
//   Both default to returning false.
bool Mesh::rayIntersects(const RayTracing::Ray_t &ray, const float t0, const float t1, RayTracing::HitInfo_t &hitinfo) const
{
	// Only supporting GL_TRIANGLES for now.
	if (m_primitiveType != GL_TRIANGLES)
		return false;

	float t_max = t1;
	bool isHit = false;
	for (GLuint ind=0; ind < m_numIndices; ind += 3 )
	{
		bool triHit = rayIntersectsTriangle(m_indices[ind], m_indices[ind+1], m_indices[ind+2], ray, t0, t_max, hitinfo);
		if (triHit)
		{
			t_max = hitinfo.hitDist;
		}
		isHit = isHit || triHit;
	}
	return isHit;
}

bool Mesh::shadowsRay(const RayTracing::Ray_t &ray, const float t0, const float t1) const
{
	// Only supporting GL_TRIANGLES for now.
	if (m_primitiveType != GL_TRIANGLES)
		return false;

	for (GLuint ind=0; ind < m_numIndices; ind += 3 )
	{
		if (rayShadowsTriangle(m_indices[ind], m_indices[ind+1], m_indices[ind+2], ray, t0, t1) )
		{
			return true;
		}
	}
	return false;
}

void Mesh::hitProperties(const RayTracing::HitInfo_t &hitinfo, gml::vec3_t &normal, gml::vec2_t &texCoords) const
{
	const float u = hitinfo.mesh.u, v = hitinfo.mesh.v;
	const float w0 = 1.0-u-v, w1 = u, w2 = v;
	texCoords = gml::add(
			gml::add( gml::scale(w0, m_vertTexcoords[hitinfo.mesh.i0]), gml::scale(w1, m_vertTexcoords[hitinfo.mesh.i1]) ),
			gml::scale(w2, m_vertTexcoords[hitinfo.mesh.i2])
	);

	normal = gml::add(
			gml::add( gml::scale(w0, m_vertNormals[hitinfo.mesh.i0]), gml::scale(w1, m_vertNormals[hitinfo.mesh.i1]) ),
			gml::scale(w2, m_vertNormals[hitinfo.mesh.i2])
	);
	normal = gml::normalize(normal);
}

} // namespace
