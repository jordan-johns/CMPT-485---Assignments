
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
 * Class that encapsulates a GLSL shader as well
 * as supporting enums, structures, and preprocessor
 * macros
 *
 *
 */

#pragma once
#ifndef __INC_SHADERS_GLPROGRAM_H_
#define __INC_SHADERS_GLPROGRAM_H_

#include "../GL3/gl3.h"
#include "../Texture/texture.h"
#include "../GML/gml.h"

namespace Shader
{

// layout locations of vertex attributes in all OpenGL shaders
// There must be one enum element for each vertex attribute that
// will be used by your shaders.
//  Note: If you decide to use this framework for something that
// requires more than three attributes, then you must also bind
// the attribute data in Objects/mesh.cpp
typedef enum
{
	VERTEX_POSITION = 0,
	VERTEX_NORMAL,
	VERTEX_TEXCOORDS,
	NUM_VERTEX_ATTRIBS
} VertexAttribLocations;



// variable names to use for uniforms in GLSL programs
// Use these macros instead of writing the variable names
// in your program. This will ensure that you don't have
// any variable name typos that stop your GLSL program from
// compiling.
//
// There should be one preprocessor macro for each of the
// uniforms required by your shaders.
#define UNIF_LIGHTPOS "lightPos"
#define UNIF_LIGHTRAD "lightRadiance"
#define UNIF_AMBIENT "ambientRadiance"
#define UNIF_SURFREF "surfaceReflectance"
#define UNIF_TEXTURE0 "texture0"
#define UNIF_SPECEXP "specularExponent"
#define UNIF_SPECREF "specularReflectance"
#define UNIF_MODELVIEW "modelView"
#define UNIF_PROJECTION "projection"
#define UNIF_NORMALTRANS "normalsTransform"
#define UNIF_SHADOWMAP "shadowMap"


// enum that gives the offset into the GLProgram::m_uniformLocs[]
// array to find the handle for a uniform.
//
// There should be one enum element for each uniform being
// used by your GLSL shaders.
typedef enum
{
	UNIFORM_LIGHTPOS=0, // Position of the light. vec3
	UNIFORM_LIGHTRAD,   // Radiance of the light. vec3
	UNIFORM_AMBIENT,    // Radiance of ambient light. vec3
	UNIFORM_SURFREF,    // Surface reflectance. vec3
	UNIFORM_TEXTURE0,   // Texture unit 0; surface reflectance. sampler2D
	UNIFORM_SPECEXP,    // Specular exponent. float
	UNIFORM_SPECREF,    // Specular reflectance. vec3
	UNIFORM_MODELVIEW,  // Modelview matrix. mat4x4
	UNIFORM_PROJECTION, // Projection matrix. mat4x4. view -> clip coordinates
	UNIFORM_NORMALTRANS, // Matrix for transforming normals. mat4x4. = transpose(inverse(modelview))
	UNIFORM_SHADOWMAP,  // CubeMap depth-texture for shadow mapping
	NUM_UNIFORM_VARS
} UniformVars;


// Data to be passed to GLSL program uniforms
// You should have a member in this struct for
// each uniform that you want to pass to a GLSL
// program
//
// There is only one GLProgUniforms struct, each
// GLSL shader should just load the pieces of
// data that it needs.
typedef struct _GLProgUniforms
{
	// World properties
	gml::vec3_t m_lightPos; // Light position
	gml::vec3_t m_lightRad; // Light radiance
	gml::vec3_t m_ambientRad; // Ambient radiance
	gml::mat4x4_t m_projection; // Projection matrix

	// Object-specific properties
	gml::vec3_t m_surfRefl; // Surface reflectance
	GLfloat m_specExp; // Specular exponent
	gml::vec3_t m_specRefl; // Specular reflectance
	gml::mat4x4_t m_modelView; // Modelview matrix
	gml::mat4x4_t m_normalTrans; // Normal transform matrix. = transpose(inverse(modelview))
	Texture::Texture *m_texture0; // Texture 0
} GLProgUniforms;

class GLProgram
{
protected:
	// ID for the linked program
	GLuint m_prog;

	// Handle/ID for each of this program's uniforms.
	//  m_uniformLocs[i] < 0 => no corresponding uniform
	//  being used by the program.
	GLint m_uniformLocs[NUM_UNIFORM_VARS];

	bool compileShader(const char *code, const GLuint handle) const;
public:
	GLProgram();
	~GLProgram();

	// Try to compile & link a GLSL program from the given
	// vertex shader & fragment shader source
	bool init(const char *vertCode, const char *fragCode);

	// Bind & unbind the shader to the OpenGL context
	void bind() const;
	void unbind() const;

	// Retrieve the program ID of the GLSL program
	inline GLuint getID() const { return m_prog; }
	// Retrieve the handle/ID of one of the program's uniforms
	inline GLint getUniformID(UniformVars var) const { return m_uniformLocs[var]; }
};

}

#endif
