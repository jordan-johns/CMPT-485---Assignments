
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
#ifndef __INC_SHADOWMAP_H_
#define __INC_SHADOWMAP_H_

#include "../GL3/gl3.h"
#include "../Scene/scene.h"
#include "../Camera/camera.h"

// Maximum distance of effect for a light
#define SHADOWMAP_NEAR 1.0f
#define SHADOWMAP_NEAR_STR "1.0f"
#define SHADOWMAP_FAR 50.0f
#define SHADOWMAP_FAR_STR "50.0f"

class ShadowMap
{
public:
	// Cameras centered on the light, with 90 degree FOV,
	// pointing in each direction along every axis
	Camera m_cameras[6];

	// The shadowmap itself. This is a handle
	// to a cube map.
	GLuint m_shadowmap;

	// Framebuffer object to use when creating shadowmap
	GLuint m_fbo;

	int m_shadowMapSize;

	// True iff the shadow map texture has been properly created
	bool m_isReady;
public:
	ShadowMap();
	~ShadowMap();

	// Initialize the shadowmap
	//  smapSize = width & height of shadow map textures; in pixels.
	// Return true if successful.
	bool init(const int smapSize);

	// Create a shadowmap from the given light position
	// -- lightPos is expected in world coordinates
	// --
	// The shadowmap will be created relative to the camera-space
	// coordinates of the light source
	void create(Scene::Scene &scene, const gml::mat4x4_t worldView);

	// Bind the shadow map to the given texture unit
	void bindGL(GLenum textureUnit) const;
	void unbindGL(GLenum textureUnit) const;
};


#endif /* SHADOWMAP_H_ */
