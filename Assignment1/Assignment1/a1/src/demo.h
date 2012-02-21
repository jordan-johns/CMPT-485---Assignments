

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
 * Demo OpenGL callback bindings.
 * When a DemoProgram object is setup as the UI's callback
 * object, then this "program" will be run.
 *
 * See UI/ui.h for more information about GUI callbacks & events
 */


#pragma once
#ifndef __INC_DEMO_H_
#define __INC_DEMO_H_

#include "Objects/geometry.h"
#include "Objects/object.h"
#include "Shaders/manager.h"

#include "UI/ui.h"

class DemoProgram : public UI::Callbacks
{
protected:
	Shader::Manager m_shaderManager;

	// Objects in the scene to render
	Object::Object **m_scene;
	GLuint m_nObjects;

	// Array of geometry that has been allocated
	// so that we can delete it.
	Object::Geometry **m_geometry;
	GLuint m_nGeometry;

	// Window width & height
	int m_width, m_height;

	// Flag indicating whether the framebuffer is automatically doing gamma correction.
	bool m_sRGBframebuffer;

	// Whether to render in wireframe or not
	bool m_renderWireframe;
public:
	DemoProgram();
	virtual ~DemoProgram();

	bool init();

	virtual void windowResize(int width, int height);
	virtual void specialKeyboard(UI::KeySpecial_t key, UI::ButtonState_t state);
	virtual void repaint();
};

#endif
