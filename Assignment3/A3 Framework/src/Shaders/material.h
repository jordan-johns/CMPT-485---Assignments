
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
 * Material properties for objects.
 *
 * Includes:
 *  - How the object should be shaded (GOURAUD or PHONG);
 *  - Whether it should use a texture for surface reflectance or
 *    a single constant RGB triplet instead.
 *  - The specularity values of the material, if any.
 */

#pragma once
#ifndef __INC_SHADERS_MATERIAL_H_
#define __INC_SHADERS_MATERIAL_H_

#include "../GML/gml.h"
#include "../Texture/texture.h"

namespace Material
{

// What is the source for lambertian reflectance
typedef enum
{
	CONSTANT, // A single reflectance value
	TEXTURE // A texture map
} LambertianSource;

typedef enum
{
	SIMPLE,
	GOURAUD,
	PHONG
} ShaderType;

class Material
{
protected:
	LambertianSource m_lSource; // default: SINGLE
	// Surface reflectance
	gml::vec3_t m_surfRefl; // default: pure white

	bool m_hasSpecular; // default: false
	float m_specExp; // specular exponent
	// specular reflectance
	gml::vec3_t m_specRefl; // default: pure white

	ShaderType m_shadeType; // default: GOURAUD

	const Texture::Texture *m_texture; // default: 0

	bool m_mirror; //default :false
	gml::vec3_t m_mirrorRefl; // default: 1,1,1

public:
	Material();
	~Material();

	ShaderType getShaderType() const { return m_shadeType; }
	LambertianSource getLambSource() const { return m_lSource; }
	const gml::vec3_t& getSurfRefl() const { return m_surfRefl; }
	bool hasSpecular() const { return m_hasSpecular; }
	float getSpecExp() const { return m_specExp; }
	const gml::vec3_t& getSpecRefl() const { return m_specRefl; }
	const Texture::Texture* getTexture() const { return m_texture; }

	// Sets the surface reflectance, and sets the source to SINGLE
	void setSurfReflectance(const gml::vec3_t refl);
	// Set the specular exponent.
	// A value <= 0 will turn off specularity
	void setSpecExp(const float sExp);
	// Set the specular reflectance.
	// Does not change the value of hasSpecular
	void setSpecReflectance(const gml::vec3_t &specref);
	// Set the type of shader to use
	void setShaderType(const ShaderType type);
	// Set the texture. Set to 0 to turn off texture
	void setTexture(const Texture::Texture *texture);

	// Set the material to be a mirror.
	void setMirror(bool b) { m_mirror = b; }
	// Returns whether or not the material is a mirror.
	bool isMirror() { return m_mirror; }
	// Returns mirror reflectance.
	const gml::vec3_t& getMirrorRefl() { return m_mirrorRefl; }
	// Sets the mirror reflectance.
	void setMirrorReflectance(const gml::vec3_t &mirrorRef) { m_mirrorRefl = mirrorRef;}
};

}

#endif
