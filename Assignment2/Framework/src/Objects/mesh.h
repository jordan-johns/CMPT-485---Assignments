
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

namespace Object
{

class Mesh
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

};

}

#endif /* __INC_MESH_H_ */
