
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

#include "material.h"

namespace Material
{

Material::Material()
{
	m_lSource = CONSTANT;
	m_surfRefl = gml::vec3_t(1.0,1.0,1.0);

	m_hasSpecular = false;
	m_specRefl = gml::vec3_t(1.0, 1.0, 1.0);

	m_shadeType = GOURAUD;
}
Material::~Material()
{
}

// Sets the surface reflectance, and sets the source to SINGLE
void Material::setSurfReflectance(const gml::vec3_t refl)
{
	m_surfRefl = refl;
	m_lSource = CONSTANT;
}
// Set the specular exponent.
// A value <= 0 will turn off specularity
void Material::setSpecExp(const float sExp)
{
	m_hasSpecular = sExp > 0;
	m_specExp = sExp;
}

void Material::setSpecReflectance(const gml::vec3_t &specref)
{
	m_specRefl = specref;
}
// Set the type of shader to use
void Material::setShaderType(const ShaderType type)
{
	m_shadeType = type;
}

void Material::setTexture(const Texture::Texture *texture)
{
	m_texture = texture;
	if (m_texture)
	{
		m_lSource = TEXTURE;
	}
	else
	{
		m_lSource = CONSTANT;
	}
}

}
