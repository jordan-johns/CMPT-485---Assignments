
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

#include <cstdio>

#include "shadowmap.h"
#include "../GL3/gl3.h"
#include "../GL3/gl3w.h"
#include "../glUtils.h"

// Constants for shadow map
static const int SHADOWMAP_POS_X = 0;
static const int SHADOWMAP_POS_Y = 1;
static const int SHADOWMAP_POS_Z = 2;
static const int SHADOWMAP_NEG_X = 3;
static const int SHADOWMAP_NEG_Y = 4;
static const int SHADOWMAP_NEG_Z = 5;

ShadowMap::ShadowMap()
{
	m_fbo = 0;
	m_shadowmap = 0;

	// To create a shadowmap for an omni-directional point light we need
	// 90 degree FOV cameras pointing along each axis in world space
	//  These will be used to render each side of a cubemap depth texture.
	gml::vec3_t zero(0.0,0.0,0.0); // location at 0 for now
	m_cameras[SHADOWMAP_POS_X].lookAt(zero, gml::vec3_t(1.0,0.0,0.0), gml::vec3_t(0.0,-1.0,0.0));
	m_cameras[SHADOWMAP_NEG_X].lookAt(zero, gml::vec3_t(-1.0,0.0,0.0), gml::vec3_t(0.0,-1.0,0.0));

	m_cameras[SHADOWMAP_POS_Y].lookAt(zero, gml::vec3_t(0.0,1.0,0.0), gml::vec3_t(0.0,0.0,1.0));
	m_cameras[SHADOWMAP_NEG_Y].lookAt(zero, gml::vec3_t(0.0,-1.0,0.0), gml::vec3_t(0.0,0.0,-1.0));

	m_cameras[SHADOWMAP_POS_Z].lookAt(zero, gml::vec3_t(0.0,0.0,1.0), gml::vec3_t(0.0,-1.0,0.0));
	m_cameras[SHADOWMAP_NEG_Z].lookAt(zero, gml::vec3_t(0.0,0.0,-1.0), gml::vec3_t(0.0,-1.0,0.0));

	for (int i=0;i<6;i++)
	{
		m_cameras[i].setCameraProjection(CAMERA_PROJECTION_PERSPECTIVE); // perspective projection
		m_cameras[i].setFOV( (90.0f * M_PI)/180.0f ); // 90 degree FOV
		m_cameras[i].setImageDimensions( 512, 512 );
		m_cameras[i].setDepthClip(SHADOWMAP_NEAR, SHADOWMAP_FAR);
	}
}

ShadowMap::~ShadowMap()
{
	if (m_fbo > 0) glDeleteFramebuffers(1, &m_fbo);
	if (m_shadowmap > 0) glDeleteTextures(1, &m_shadowmap);

}

// Initialize the shadowmap
//  smapSize = width & height of shadow map textures; in pixels.
// Return true if successful.
bool ShadowMap::init(const int smapSize)
{
	// Create a framebuffer object for shadow mapping
	// A framebuffer object encapsulates a render-target context
	// We'll have to bind each of the depth-textures for shadow mapping
	// to the GL_DEPTH_ATTACHMENT point in this framebuffer object
	if (m_fbo > 0) glDeleteFramebuffers(1, &m_fbo);
	glGenFramebuffers(1, &m_fbo);

	// Create the cubemap of depth-textures
	if (m_shadowmap > 0) glDeleteTextures(1, &m_shadowmap);
	glGenTextures(1, &m_shadowmap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_shadowmap);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

	GLuint cubeSides[] = {
			GL_TEXTURE_CUBE_MAP_POSITIVE_X,
			GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
			GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
			GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
			GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
			GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
	};
	// Create a texture for each side of the cubemap
	for (int i=0; i<6; i++)
	{
		glTexImage2D(cubeSides[i], 0, GL_DEPTH_COMPONENT,	smapSize, smapSize, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	}

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	for (int i=0; i<6; i++)
	{
		m_cameras[i].setImageDimensions(smapSize, smapSize);
	}

	m_shadowMapSize = smapSize;

	m_isReady = glIsTexture(m_shadowmap) == GL_TRUE;

	return !isGLError();
}

void ShadowMap::create(Scene::Scene &scene, const gml::mat4x4_t worldView)
{

	if ( !m_isReady )
	{
#if !defined(NDEBUG)
		fprintf(stderr, "Trying to create shadow map when cubemap not ready\n");
#endif
		return;
	}

	gml::vec3_t _lightPos; // light position in camera coordinates
	_lightPos = gml::extract3(gml::mul( worldView, scene.getLightPos() ));

	GLuint textures[6];
	textures[SHADOWMAP_POS_X] = GL_TEXTURE_CUBE_MAP_POSITIVE_X;
	textures[SHADOWMAP_NEG_X] = GL_TEXTURE_CUBE_MAP_NEGATIVE_X;
	textures[SHADOWMAP_POS_Y] = GL_TEXTURE_CUBE_MAP_POSITIVE_Y;
	textures[SHADOWMAP_NEG_Y] = GL_TEXTURE_CUBE_MAP_NEGATIVE_Y;
	textures[SHADOWMAP_POS_Z] = GL_TEXTURE_CUBE_MAP_POSITIVE_Z;
	textures[SHADOWMAP_NEG_Z] = GL_TEXTURE_CUBE_MAP_NEGATIVE_Z;

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
	//glDrawBuffer(GL_NONE); // turn off color output
	if ( isGLError() ) return;

	glViewport(0, 0, m_shadowMapSize, m_shadowMapSize);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	// Turn on the depth buffer
	glEnable(GL_DEPTH_TEST);

	if ( isGLError() ) return;

	for (int i=0; i<6; i++)
	{
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, textures[i], m_shadowmap, 0);
		if ( isGLError() ) return;

		if (GL_FRAMEBUFFER_COMPLETE != glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER))
		{
			fprintf(stderr, "Incomplete framebuffer!\n");
			return;
		}

		// Clear the depth buffer
		glClear(GL_DEPTH_BUFFER_BIT);
		if ( isGLError() ) return;

		// Center camera on the camera-coordinates of the light source
		m_cameras[i].setPosition(_lightPos);

		gml::mat4x4_t worldCam = gml::mul(m_cameras[i].getWorldView(), worldView);

		scene.rasterizeDepth(worldCam, m_cameras[i].getProjection());

		glFinish();

	}

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}


void ShadowMap::bindGL(GLenum textureUnit) const
{
	if (m_isReady)
	{
		glActiveTexture(textureUnit);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_shadowmap);
	}
}

void ShadowMap::unbindGL(GLenum textureUnit) const
{
	if (m_isReady)
	{
		glActiveTexture(textureUnit);
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}
}
