
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

#include "GL3/gl3w.h"
#include <GL/glext.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "Objects/Models/sphere.h"
#include "Objects/Models/octahedron.h"
#include "Objects/Models/plane.h"

#include "Shaders/material.h"

#include "glUtils.h"
#include "assign1.h"

// Values for bitfield for camera movement
static const int CAMERA_FORWARD = 0x001;
static const int CAMERA_BACKWARD = 0x002;
static const int CAMERA_STRAFE_RIGHT = 0x004;
static const int CAMERA_STRAFE_LEFT = 0x008;
static const int CAMERA_UP = 0x010;
static const int CAMERA_DOWN = 0x020;
static const int CAMERA_ROTATE_RIGHT = 0x040;
static const int CAMERA_ROTATE_LEFT = 0x080;
static const int CAMERA_ROTATE_UP = 0x100;
static const int CAMERA_ROTATE_DOWN = 0x200;
static const int CAMERA_SPIN_LEFT = 0x400;
static const int CAMERA_SPIN_RIGHT = 0x800;

Assignment1::Assignment1()
{
	m_scene = 0;
	m_nObjects = 0;
	m_geometry = 0;
	m_nGeometry = 0;

	// Position of a point light: (0,10,10)
	m_lightPos = gml::vec4_t(0.0f,10.0f,10.0f,1.0f);
	// Radiance of the point light
	m_lightRad = gml::vec3_t(0.6f,0.6f,0.6f);
	// Ambient radiance
	m_ambientRad = gml::vec3_t(0.025f, 0.025f, 0.025f);

	m_movementSpeed = 2.0f; // Camera movement speed = 2 units / second
	m_cameraMovement = 0; // Initialized for no camera motion
	m_rotationSpeed = (40.0f * M_PI) / 180.0f; // Camera rotation speed = 40 degrees / second

	// Is the framebuffer automatically gamma-correcting?
	m_sRGBframebuffer = glIsEnabled(GL_FRAMEBUFFER_SRGB_EXT) == GL_TRUE;
	// Start with wireframe rendering turned off
	m_renderWireframe = false;
}

Assignment1::~Assignment1()
{
	if (m_scene)
	{
		for (GLuint i=0; i<m_nObjects; i++)
		{
			if (m_scene[i]) delete m_scene[i];
		}
		delete[] m_scene;
	}
	if (m_geometry)
	{
		for (GLuint i=0; i<m_nGeometry; i++)
		{
			if (m_geometry[i]) delete m_geometry[i];
		}
		delete[] m_geometry;
	}
}

bool Assignment1::init()
{
	/*
	 * This function is called by main() when the program is starting up.
	 * It should initialize whatever data the assignment requires.
	 */

	// Initialize the shader manager
	if ( !m_shaderManager.init() )
	{
		fprintf(stderr, "ERROR! Could not initialize Shader Manager.\n");
		return false;
	}

	// Set the initial camera position
	// Camera position: (0,3,3)
	//   Camera target: (0,0,0)
	m_camera.lookAt(gml::vec3_t(0.0,3.0,3.0), gml::vec3_t(0.0,0.0,0.1) );

	// Set near & far clip distances
	//  near = 1.0
	//  far = 30.0
	m_camera.setDepthClip(1.0f, 30.0f);

	// Create the geometry.
	const int SPHERE_LOC = 0;
	const int OCTAHEDRON_LOC = 1;
	const int PLANE_LOC = 2;

	m_nGeometry = 3;
	m_geometry = new Object::Geometry*[m_nGeometry];
	memset(m_geometry, 0x00, sizeof(Object::Geometry*)*m_nGeometry);

	m_geometry[SPHERE_LOC] = new Object::Models::Sphere();
	if ( !m_geometry[SPHERE_LOC] || !((Object::Models::Sphere*)m_geometry[SPHERE_LOC])->init(3) || isGLError() )
	{
		return false;
	}
	m_geometry[OCTAHEDRON_LOC] = new Object::Models::Octahedron();
	if ( !m_geometry[OCTAHEDRON_LOC] || !((Object::Models::Octahedron*)m_geometry[OCTAHEDRON_LOC])->init() || isGLError())
	{
		return false;
	}
	m_geometry[PLANE_LOC] = new Object::Models::Plane();
	if ( !m_geometry[PLANE_LOC] || !((Object::Models::Plane*)m_geometry[PLANE_LOC])->init() || isGLError())
	{
		return false;
	}

	// Feel free to change the objects created in this part as you see fit
	// for testing your program
	m_nObjects = 9;
	m_scene = new Object::Object*[m_nObjects];

	Material::Material mat;

	/********************************************************************************
	 * Second group pairing:
	 *
	 * LAMBERTIAN - Gouraud
	 ********************************************************************************/
	// Sphere.
	mat.setShaderType(Material::GOURAUD);
	mat.setSurfReflectance(gml::vec3_t(0.5, 0.5, 0.5));
	m_scene[0] = new Object::Object(m_geometry[SPHERE_LOC], mat,
			gml::mul(gml::translate(gml::vec3_t(-1.5,0.0,0.0)), gml::scaleh(0.5, 0.5, 0.5)) );
	// Octahedron.
	mat.setShaderType(Material::GOURAUD);
	mat.setSurfReflectance(gml::vec3_t(0, 1, 0));
	m_scene[1] = new Object::Object(m_geometry[OCTAHEDRON_LOC], mat,
			gml::mul(gml::translate(gml::vec3_t(-1.5,0.0,-1.0)), gml::scaleh(0.5, 0.5, 0.5)) );

	/********************************************************************************
	 * Second group pairing:
	 *
	 * LAMBERTIAN - Phong
	 ********************************************************************************/
	// Sphere.
	mat.setShaderType(Material::PHONG);
	mat.setSurfReflectance(gml::vec3_t(0.5, 0.5, 0.5));
	m_scene[2] = new Object::Object(m_geometry[SPHERE_LOC], mat,
			gml::mul(gml::translate(gml::vec3_t(-0.5,0.0,0.0)), gml::scaleh(0.5, 0.5, 0.5)) );

	// Octahedron.
	mat.setShaderType(Material::PHONG);
	mat.setSurfReflectance(gml::vec3_t(0, 1, 0));
	m_scene[3] = new Object::Object(m_geometry[OCTAHEDRON_LOC], mat,
			gml::mul(gml::translate(gml::vec3_t(-0.5,0.0,-1.0)), gml::scaleh(0.5, 0.5, 0.5)) );

	/********************************************************************************
	 * Third group pairing:
	 *
	 * SPECULAR - Gouraud
	 *********************************************************************************/
	// Sphere
	mat.setShaderType(Material::GOURAUD);
	mat.setSurfReflectance(gml::vec3_t(0.5, 0.5, 0.5));
	mat.setSpecExp(20.0); // Enabled the highlight.
	m_scene[4] = new Object::Object(m_geometry[SPHERE_LOC], mat,
			gml::mul(gml::translate(gml::vec3_t(0.5,0.0,0.0)), gml::scaleh(0.5, 0.5, 0.5)) );
	// Octahedron.
	mat.setShaderType(Material::GOURAUD);
	mat.setSurfReflectance(gml::vec3_t(0, 1, 0));
	mat.setSpecExp(20.0); // Enabled the highlight.
	m_scene[5] = new Object::Object(m_geometry[OCTAHEDRON_LOC], mat,
			gml::mul(gml::translate(gml::vec3_t(0.5,0.0,-1.0)), gml::scaleh(0.5, 0.5, 0.5)) );

	/********************************************************************************
	 * Fourth group pairing:
	 *
	 * SPECULAR - Phong
	 *********************************************************************************/
	// Sphere
	mat.setShaderType(Material::PHONG);
	mat.setSurfReflectance(gml::vec3_t(0.5, 0.5, 0.5));
	mat.setSpecExp(50.0); // Enabled the highlight.
	m_scene[6] = new Object::Object(m_geometry[SPHERE_LOC], mat,
			gml::mul(gml::translate(gml::vec3_t(1.5,0.0,0.0)), gml::scaleh(0.5, 0.5, 0.5)) );
	// Octahedron.
	mat.setShaderType(Material::PHONG);
	mat.setSurfReflectance(gml::vec3_t(0, 1, 0));
	mat.setSpecExp(50.0); // Enabled the highlight.
	m_scene[7] = new Object::Object(m_geometry[OCTAHEDRON_LOC], mat,
			gml::mul(gml::translate(gml::vec3_t(1.5,0.0,-1.0)), gml::scaleh(0.5, 0.5, 0.5)) );

	// plane
	mat.setShaderType(Material::GOURAUD);
	mat.setSurfReflectance(gml::vec3_t(0.5, 0.5, 0.1));
	mat.setSpecExp(0);
	m_scene[8] = new Object::Object(m_geometry[PLANE_LOC], mat,
			gml::mul(gml::translate(gml::vec3_t(0,-1.0,-0.5)), gml::scaleh(3, 1, 3)));
	// =============================================================================================

	printf(
			"Camera movement:\n"
			"  [w] -- Camera forward\n"
			"  [s] -- Camera backward\n"
			"  [q] -- Camera up\n"
			"  [e] -- Camera down\n"
			"  [a] -- Camera strafe left\n"
			"  [d] -- Camera strafe right\n"
			"Camera rotation:\n"
			"  [keypad 8] -- Rotate camera up\n"
			"  [keypad 5] -- Rotate camera down\n"
			"  [keypad 4] -- Rotate camera right\n"
			"  [keypad 6] -- Rotate camera left\n"
			"  [keypad 7] -- Spin camera left\n"
			"  [keypad 9] -- Spin camera right\n"
			"Other Controls:\n"
			"  [g] -- Toggle sRGB framebuffer\n"
			"  [f] -- Toggle wireframe rendering\n"
			"  [o] -- Set to orthographic camera\n"
			"  [p] -- Set to perspective camera\n"
			"  [ESC] -- Quit\n"
	);

	return true;
}

void Assignment1::windowResize(int width, int height)
{
	/*
	 * This function is called automatically when the
	 * window is resized.
	 * width & height are the new width & height, in pixels,
	 * of the window
	 */
	float aspect = width / (float)height;
	m_camera.setAspect(aspect);
}

void Assignment1::toggleCameraMoveDirection(bool enable, int direction)
{
	if (enable) // enable movement
		m_cameraMovement = m_cameraMovement | direction;
	else
		m_cameraMovement = m_cameraMovement & (~direction);
	m_lastCamMoveTime = UI::getTime();
}

void Assignment1::specialKeyboard(UI::KeySpecial_t key, UI::ButtonState_t state)
{
	/*
	 * Called automatically when the user presses a key on the keyboard
	 * key will be a code indicating which key was pressed
	 * state will be one of UI::BUTTON_UP or UI::BUTTON_DOWN
	 *   indicating whether the key was released, or pressed
	 */
	switch (key)
	{
	case UI::KEY_ESC:
		if (state == UI::BUTTON_UP)
		{
			UI::exitMainLoop();
		}
		break;
	case UI::KEY_W:
		toggleCameraMoveDirection(state == UI::BUTTON_DOWN, CAMERA_FORWARD);
		break;
	case UI::KEY_S:
		toggleCameraMoveDirection(state == UI::BUTTON_DOWN, CAMERA_BACKWARD);
		break;

	case UI::KEY_A:
		toggleCameraMoveDirection(state == UI::BUTTON_DOWN, CAMERA_STRAFE_LEFT);
		break;
	case UI::KEY_D:
		toggleCameraMoveDirection(state == UI::BUTTON_DOWN, CAMERA_STRAFE_RIGHT);
		break;

	case UI::KEY_Q:
		toggleCameraMoveDirection(state == UI::BUTTON_DOWN, CAMERA_UP);
		break;
	case UI::KEY_E:
		toggleCameraMoveDirection(state == UI::BUTTON_DOWN, CAMERA_DOWN);
		break;


	case UI::KEY_KP_8:
		toggleCameraMoveDirection(state == UI::BUTTON_DOWN, CAMERA_ROTATE_UP);
		break;
	case UI::KEY_KP_5:
		toggleCameraMoveDirection(state == UI::BUTTON_DOWN, CAMERA_ROTATE_DOWN);
		break;

	case UI::KEY_KP_4:
		toggleCameraMoveDirection(state == UI::BUTTON_DOWN, CAMERA_ROTATE_LEFT);
		break;
	case UI::KEY_KP_6:
		toggleCameraMoveDirection(state == UI::BUTTON_DOWN, CAMERA_ROTATE_RIGHT);
		break;

	case UI::KEY_KP_7:
		toggleCameraMoveDirection(state == UI::BUTTON_DOWN, CAMERA_SPIN_LEFT);
		break;
	case UI::KEY_KP_9:
		toggleCameraMoveDirection(state == UI::BUTTON_DOWN, CAMERA_SPIN_RIGHT);
		break;

	case UI::KEY_G:
		if (state == UI::BUTTON_DOWN)
		{
			if (m_sRGBframebuffer)
			{
				glDisable(GL_FRAMEBUFFER_SRGB_EXT);
			}
			else
			{
				glEnable(GL_FRAMEBUFFER_SRGB_EXT);
			}
			m_sRGBframebuffer = glIsEnabled(GL_FRAMEBUFFER_SRGB_EXT) == GL_TRUE;
			if (m_sRGBframebuffer)
			{
				printf("sRGB framebuffer enabled\n");
			}
			else
			{
				printf("sRGB framebuffer disabled\n");
			}
		}
		break;
	case UI::KEY_F:
		if (state == UI::BUTTON_DOWN)
		{
			m_renderWireframe = !m_renderWireframe;
		}
		break;

	case UI::KEY_O:
		m_camera.setCameraProjection(CAMERA_PROJECTION_ORTHOGRAPHIC);
		break;

	case UI::KEY_P:
		m_camera.setCameraProjection(CAMERA_PROJECTION_PERSPECTIVE);
		break;

	default:
		break;
	}
}

void Assignment1::repaint()
{
	/*
	 * Called automatically whenever the window needs to be
	 * redrawn.
	 * This will end up being called after any user event (keypress, etc),
	 * and pretty much every time through the event loop.
	 */
	if (m_renderWireframe)
	{
		// Turn on wireframe rendering
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	// Clear the pixel data & depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Struct used to pass data values for GLSL uniform variables to
	// the shader program
	Shader::GLProgUniforms shaderUniforms;

	// Set up uniforms constant to the world
	shaderUniforms.m_lightPos = gml::extract3( gml::mul( m_camera.getWorldView(), m_lightPos ) );
	shaderUniforms.m_lightRad = m_lightRad;
	shaderUniforms.m_ambientRad = m_ambientRad;
	shaderUniforms.m_projection = m_camera.getProjection();

	for (GLuint i=0; i<m_nObjects; i++)
	{
		// Fetch the Shader object from the ShaderManager that will perform the
		// shading calculations for this object
		const Shader::Shader *shader = m_shaderManager.getShader(m_scene[i]->getMaterial());

		if (shader->getIsReady())
		{
			shader->bindGL(); // Bind the shader to the OpenGL context
			if (isGLError()) return;

			// Object-specific uniforms
			shaderUniforms.m_modelView = gml::mul(m_camera.getWorldView(), m_scene[i]->getObjectToWorld());
			shaderUniforms.m_normalTrans = gml::transpose( gml::inverse(shaderUniforms.m_modelView) );
			// If the surface material is not using a texture for Lambertian surface reflectance
			if (m_scene[i]->getMaterial().getLambSource() == Material::CONSTANT)
			{
				shaderUniforms.m_surfRefl = m_scene[i]->getMaterial().getSurfRefl();
			}
			else
			{
				// Set up texture
			}
			// Set up the specular components of the uniforms struct if the material
			// is specular
			if (m_scene[i]->getMaterial().hasSpecular())
			{
				shaderUniforms.m_specExp = m_scene[i]->getMaterial().getSpecExp();
				shaderUniforms.m_specRefl = m_scene[i]->getMaterial().getSpecRefl();
			}

			// Set the shader uniform variables
			if ( !shader->setUniforms(shaderUniforms) || isGLError() ) return;

			// Rasterize the object
			m_scene[i]->rasterize();
			if (isGLError()) return;

			// Unbind the shader from the OpenGL context
			shader->unbindGL();
		}
	}


	if (m_renderWireframe)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	// Force everything outstanding to complete
	glFinish();
}

void Assignment1::idle()
{
	// idle is called each time through the event loop when there is no
	// pending render call, or interaction event to process.
	// That makes it ideal for animation, and similar actions

	double currTime = UI::getTime(); // current time

	if (m_cameraMovement) // Is a camera movement key pressed?
	{
		// time since the last time we updated the camera
		double deltaT = currTime - m_lastCamMoveTime;
		if (deltaT > 0)
		{
			// Time has elapsed since the last time idle() was called
			// so, move the camera according to which key(s) are pressed.

			if (m_cameraMovement & CAMERA_FORWARD)
				m_camera.moveForward( m_movementSpeed * deltaT );
			if (m_cameraMovement & CAMERA_BACKWARD)
				m_camera.moveForward( -m_movementSpeed * deltaT );

			if (m_cameraMovement & CAMERA_STRAFE_RIGHT)
				m_camera.strafeRight( m_movementSpeed * deltaT );
			if (m_cameraMovement & CAMERA_STRAFE_LEFT)
				m_camera.strafeRight( -m_movementSpeed * deltaT );

			if (m_cameraMovement & CAMERA_UP)
				m_camera.moveUp( m_movementSpeed * deltaT);
			if (m_cameraMovement & CAMERA_DOWN)
				m_camera.moveUp( -m_movementSpeed * deltaT);

			if (m_cameraMovement & CAMERA_ROTATE_UP)
				m_camera.rotateUp( m_rotationSpeed * deltaT );
			if (m_cameraMovement & CAMERA_ROTATE_DOWN)
				m_camera.rotateUp( -m_rotationSpeed * deltaT );

			if (m_cameraMovement & CAMERA_ROTATE_LEFT)
				m_camera.rotateRight( m_rotationSpeed * deltaT );
			if (m_cameraMovement & CAMERA_ROTATE_RIGHT)
				m_camera.rotateRight( -m_rotationSpeed * deltaT );

			if (m_cameraMovement & CAMERA_SPIN_LEFT)
				m_camera.spinCamera( m_rotationSpeed * deltaT );
			if (m_cameraMovement & CAMERA_SPIN_RIGHT)
				m_camera.spinCamera( -m_rotationSpeed * deltaT );

			m_lastCamMoveTime = currTime;
		}

	}
}

