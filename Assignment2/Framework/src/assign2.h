
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
 * Program Object class for CMPT 485/829 Assignment #2
 */

#pragma once
#ifndef __INC_ASSIGN2_H_
#define __INC_ASSIGN2_H_

#include "GML/gml.h"
#include "Camera/camera.h"
#include "Objects/object.h"
#include "Objects/geometry.h"
#include "Shaders/manager.h"
#include "Texture/texture.h"
#include "ShadowMapping/shadowmap.h"
#include "UI/ui.h"

class Assignment2 : public UI::Callbacks
{
protected:
	gml::vec4_t m_lightPos;
	gml::vec3_t m_lightRad;
	gml::vec3_t m_ambientRad;

	Shader::Manager m_shaderManager;

	Camera m_camera;

	int m_windowWidth, m_windowHeight;

	// Objects in the scene to render
	Object::Object **m_scene;
	GLuint m_nObjects;

	// Array of geometry that has been allocated
	// so that we can delete it.
	Object::Geometry **m_geometry;
	GLuint m_nGeometry;

	// Members for keyboard camera control
	int m_cameraMovement;
	double m_lastCamMoveTime;

	float m_movementSpeed; // Number of units/second to move
	float m_rotationSpeed; // radians/sec to rotate

	// Flag indicating whether the framebuffer is automatically doing gamma correction.
	bool m_sRGBframebuffer;

	// Whether to render in wireframe or not
	bool m_renderWireframe;

	int m_shadowmapSize;
	ShadowMap m_shadowmap;
	bool m_enableShadows;

	// For animation
	double m_lastIdleTime; // Time that idle was last called

	void toggleCameraMoveDirection(bool enable, int direction);

	// Rasterize the scene with full color shaders
	void rasterizeScene();
public:
	Assignment2();
	virtual ~Assignment2();

	bool init();

	virtual void windowResize(int width, int height);
	virtual void specialKeyboard(UI::KeySpecial_t key, UI::ButtonState_t state);
	virtual void repaint();
	virtual void idle();
};


#endif
