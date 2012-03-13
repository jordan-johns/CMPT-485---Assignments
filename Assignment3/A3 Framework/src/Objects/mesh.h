
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
 * A triangular mesh. Stored for index rendering.
 *
 * Each vertex must have the following vertex attributes:
 *   - position
 *   - normal
 *   - texture coordinates (2D)
 *
 * Vertex attributes are stored in separate arrays, and
 * triangles are formed according to the primitive type used
 * by traversing the index array.
 *
 * The type of primitive formed by the index array
 * may be one of:
 *   GL_TRIANGLES
 *   GL_TRIANGLE_STRIP
 *   GL_TRIANGLE_FAN
 *
 * See Mesh::init() for some description of how meshes are
 * specified in OpenGL
 */
#pragma once
#ifndef __INC_MESH_H_
#define __INC_MESH_H_

#include "../GL3/gl3.h"
#include "../GML/gml.h"
#include "../Shaders/shader.h"
#include "../RayTracing/rayintersector.h"
#include "../RayTracing/types.h"

namespace Object
{

class Mesh : public RayTracing::RayIntersector
{
protected:
	GLuint m_vertArrayObj;
	GLuint m_vertBuffers[Shader::NUM_VERTEX_ATTRIBS];

	gml::vec3_t *m_vertPositions;
	gml::vec3_t *m_vertNormals;
	gml::vec2_t *m_vertTexcoords;
	GLuint *m_indices;
	GLuint m_numIndices;

	GLenum m_primitiveType;

	void destroy();

	bool rayIntersectsTriangle(GLuint i0, GLuint i1, GLuint i2,
			const RayTracing::Ray_t &ray, const float t0, const float t1, RayTracing::HitInfo_t &hitinfo) const;
	bool rayShadowsTriangle(GLuint i0, GLuint i1, GLuint i2,
				const RayTracing::Ray_t &ray, const float t0, const float t1) const;
public:
	Mesh();
	~Mesh();

	// Return: true iff successful
	bool init(GLenum primitive,
			GLuint numVerts, const gml::vec3_t *positions, const gml::vec3_t *normals, const gml::vec2_t *texcoords,
			GLuint numIndices, const GLuint *indices);

	// Rasterize this mesh with OpenGL
	// Assumes that the shader has already been set up.
	void rasterize() const;

	// Ray intersector virtuals
	virtual bool rayIntersects(const RayTracing::Ray_t &ray, const float t0, const float t1, RayTracing::HitInfo_t &hitinfo) const;
	virtual bool shadowsRay(const RayTracing::Ray_t &ray, const float t0, const float t1) const;
	virtual void hitProperties(const RayTracing::HitInfo_t &hitinfo, gml::vec3_t &normal, gml::vec2_t &texCoords) const;
};

}

#endif /* __INC_MESH_H_ */
