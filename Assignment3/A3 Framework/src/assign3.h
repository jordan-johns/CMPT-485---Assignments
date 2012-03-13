
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
 * Program Object class for CMPT 485/829 Assignment #3
 */

#pragma once
#ifndef __INC_ASSIGN3_H_
#define __INC_ASSIGN3_H_

#include "GML/gml.h"
#include "Scene/scene.h"
#include "Camera/camera.h"
#include "Objects/geometry.h"
#include "Texture/texture.h"
#include "ShadowMapping/shadowmap.h"
#include "UI/ui.h"

class Assignment3 : public UI::Callbacks
{
protected:
	int m_windowWidth, m_windowHeight;

	// Array of geometry that has been allocated
	// so that we can delete it.
	Object::Geometry **m_geometry;
	GLuint m_nGeometry;

	Scene::Scene m_scene;

	// Camera for the scene
	Camera m_camera;

	// Members for keyboard camera control
	int m_cameraMovement;
	double m_lastCamMoveTime;

	float m_movementSpeed; // Number of units/second to move
	float m_rotationSpeed; // radians/sec to rotate

	// Flag indicating whether the framebuffer is automatically doing gamma correction.
	bool m_sRGBframebuffer;

	// Whether to render in wireframe or not
	bool m_renderWireframe;

	Texture::Texture *m_texture;

	bool m_useShadowMap;
	int m_shadowmapSize;
	ShadowMap m_shadowmap;

	// For animation
	double m_lastIdleTime; // Time that idle was last called

	// For ray tracing
	gml::vec3_t *m_rtImage; // Ray traced image. Allocated as an m_windowWidth x m_windowHeight array
	// Note: Row 0 in the image is the bottom of the window, not the top
	bool m_isRayTracing; // true iff ray tracing mode is toggled 'on'
	GLuint m_rtFBO; // Framebuffer object for ray tracing
	GLuint m_rtTex; // Texturebuffer object to copy ray traced image data to for display.
	int m_rtRow; // Which row to ray trace next.
	bool m_cameraChanged;
	int m_rtPassNum; // How many rays have been cast through each pixel

	void toggleCameraMoveDirection(bool enable, int direction);

	// Rasterize the scene with full color shaders
	void rasterizeScene();
public:
	Assignment3();
	virtual ~Assignment3();

	bool init();

	virtual void windowResize(int width, int height);
	virtual void specialKeyboard(UI::KeySpecial_t key, UI::ButtonState_t state);
	virtual void repaint();
	virtual void idle();
};


#endif
