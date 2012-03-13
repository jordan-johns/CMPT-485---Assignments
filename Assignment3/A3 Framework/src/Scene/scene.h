
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

#pragma once
#ifndef _INC_SCENE_H_
#define _INC_SCENE_H_

#include "../GML/gml.h"
#include "../Objects/object.h"
#include "../Shaders/manager.h"
#include "../RayTracing/rayintersector.h"

namespace Scene
{

// Class for a scene representation
class Scene : public RayTracing::RayIntersector
{
protected:
	Shader::Manager m_shaderManager;

	// Objects in the scene to render
	Object::Object **m_scene;
	GLuint m_nObjects;
	GLuint m_nObjPtrsAlloced; // Size of the m_scene array

	gml::vec4_t m_lightPos; // Point light position
	gml::vec3_t m_lightRad; // Point light radiance
	gml::vec3_t m_ambientRad; // Ambient radiance

public:
	Scene();
	~Scene();

	bool init();

	// Add an object to the scene, return true if successful.
	bool addObject(Object::Object *obj);

	void setLightPos(const gml::vec4_t lp) { m_lightPos = lp; }
	void setLightPos(const gml::vec3_t lp) { m_lightPos = gml::vec4_t(lp, 1.0); }
	void setLightRad(const gml::vec3_t lr) { m_lightRad = lr; }
	void setAmbient(const gml::vec3_t am) { m_ambientRad = am; }
	gml::vec4_t& getLightPos() { return m_lightPos; }

	// -----------------------------------------
	// Rasterization
	// -----------------------------------------

	// Rasterize only using a depth shader
	void rasterizeDepth(const gml::mat4x4_t &worldView, const gml::mat4x4_t &projection);
	// Rasterize the scene. Assumes that the shadowmap, if used, is bound to texture unit 1
	void rasterize(const gml::mat4x4_t &worldView, const gml::mat4x4_t &projection, const bool useShadows);

	// -----------------------------------------
	// Ray tracing
	// -----------------------------------------

	virtual bool rayIntersects(const RayTracing::Ray_t &ray, const float t0, const float t1, RayTracing::HitInfo_t &hitinfo) const;
	virtual bool shadowsRay(const RayTracing::Ray_t &ray, const float t0, const float t1) const;
	virtual void hitProperties(const RayTracing::HitInfo_t &hitinfo, gml::vec3_t &normal, gml::vec2_t &texCoords) const;

	// Calculate the RGB color for the ray
	gml::vec3_t shadeRay(const RayTracing::Ray_t &ray, RayTracing::HitInfo_t &hitinfo, const int remainingRecursionDepth) const;
};

}

#endif
