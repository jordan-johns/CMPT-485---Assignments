
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
 * Program Object class for CMPT 485/829 Assignment #1
 */

#pragma once
#ifndef __INC_ASSIGN1_H_
#define __INC_ASSIGN1_H_

#include "GML/gml.h"
#include "Objects/geometry.h"
#include "Scene/scene.h"
#include "Camera/camera.h"
#include "Texture/texture.h"
#include "UI/ui.h"

class Assignment1 : public UI::Callbacks
{
protected:
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

	void toggleCameraMoveDirection(bool enable, int direction);
public:
	Assignment1();
	virtual ~Assignment1();

	bool init();

	virtual void windowResize(int width, int height);
	virtual void specialKeyboard(UI::KeySpecial_t key, UI::ButtonState_t state);
	virtual void repaint();
	virtual void idle();
};


#endif
