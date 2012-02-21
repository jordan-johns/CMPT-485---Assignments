
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
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "Objects/Models/sphere.h"
#include "Objects/Models/octahedron.h"
#include "Objects/Models/plane.h"

#include "Shaders/material.h"

#include "glUtils.h"
#include "assign2.h"

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

Assignment2::Assignment2()
{
	m_scene = 0;
	m_nObjects = 0;
	m_geometry = 0;
	m_nGeometry = 0;

	// Position of a point light: (0,0,0)
	m_lightPos = gml::vec4_t(0.0f,0.0f,0.0f,1.0f);
	// Radiance of the point light
	m_lightRad = gml::vec3_t(0.6f,0.6f,0.6f);
	// Ambient radiance
	m_ambientRad = gml::vec3_t(0.025f, 0.025f, 0.025f);

	m_movementSpeed = 2.0f; // Camera movement speed = 2 units / second
	m_cameraMovement = 0; // Initialized for no camera motion
	m_rotationSpeed = (40.0f * M_PI) / 180.0f; // Camera rotation speed = 40 degrees / second

	// Is the framebuffer automatically gamma-correcting?
	m_sRGBframebuffer = true;
	// Start with wireframe rendering turned off
	m_renderWireframe = false;

	m_shadowmapSize = 512;
	m_enableShadows = false;

	m_lastIdleTime = UI::getTime();
}

Assignment2::~Assignment2()
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

bool Assignment2::init()
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
	m_camera.lookAt(gml::vec3_t(5.0,0.0,5.0), gml::vec3_t(0.0,0.0,0.0) );

	// Set near & far clip distances
	//  near = 1.0
	//  far = 50.0
	m_camera.setDepthClip(1.0f, 50.0f);

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

	Material::Material mat;

	m_nObjects = 8;
	m_scene = new Object::Object*[m_nObjects];
	GLuint obj = 0;

	const float pi2 = (90.0f * M_PI) / 180.0f;
	const float sz = 15.0f;

	m_lightPos = gml::vec4_t(3.0f, 1.0f, 3.0, 1.0);

	mat.setShaderType(Material::PHONG);
	mat.setSpecExp(16.0f);
	mat.setSurfReflectance(gml::vec3_t(0.4, 0.5, 0.6));
	m_scene[obj++] = new Object::Object(m_geometry[SPHERE_LOC], mat,
			gml::mul(gml::translate(gml::vec3_t(0.0,0.0,0.0)), gml::scaleh(2.5,0.75,1.5)));

	m_scene[obj++] = new Object::Object(m_geometry[OCTAHEDRON_LOC], mat,
			gml::mul(gml::translate(gml::vec3_t(0.0,1.5,0.0)), gml::scaleh(1.0,1.5,1.0)));


	gml::vec3_t beige(0.76, 0.75, 0.5);
	gml::vec3_t red(0.63, 0.06, 0.04);
	gml::vec3_t green(0.15, 0.48, 0.09);

	mat.setSpecExp(-1.0f); // turn off specular

	mat.setSurfReflectance(beige);
	m_scene[obj++] = new Object::Object(m_geometry[PLANE_LOC], mat,
			gml::mul(gml::translate(gml::vec3_t(0.0,-sz/2,0.0)), gml::scaleh(sz, 1.0, sz)) );
	m_scene[obj++] = new Object::Object(m_geometry[PLANE_LOC], mat,
			gml::mul(gml::translate(gml::vec3_t(0.0,sz/2,0.0)), gml::mul(gml::rotateZh(2*pi2),gml::scaleh(sz, 1.0, sz))) );
	mat.setSurfReflectance(green);
	m_scene[obj++] = new Object::Object(m_geometry[PLANE_LOC], mat,
			gml::mul(gml::translate(gml::vec3_t(sz/2,0.0,0.0)), gml::mul(gml::rotateZh(pi2),gml::scaleh(sz, 1.0, sz))) );
	m_scene[obj++] = new Object::Object(m_geometry[PLANE_LOC], mat,
			gml::mul(gml::translate(gml::vec3_t(-sz/2,0.0,0.0)), gml::mul(gml::rotateZh(-pi2),gml::scaleh(sz, 1.0, sz))) );
	mat.setSurfReflectance(red);
	m_scene[obj++] = new Object::Object(m_geometry[PLANE_LOC], mat,
			gml::mul(gml::translate(gml::vec3_t(0.0,0.0,sz/2)), gml::mul(gml::rotateXh(-pi2),gml::scaleh(sz, 1.0, sz))) );
	m_scene[obj++] = new Object::Object(m_geometry[PLANE_LOC], mat,
			gml::mul(gml::translate(gml::vec3_t(0.0,0.0,-sz/2)), gml::mul(gml::rotateXh(pi2),gml::scaleh(sz, 1.0, sz))) );

	assert(obj == m_nObjects);

	// =============================================================================================

	if ( !m_shadowmap.init(m_shadowmapSize, &m_shaderManager) )
	{
		fprintf(stderr, "Failed to initialize shadow mapping members.\n");
		return false;
	}

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
			"  [F1] -- Toggle shadows\n"
			"  [g] -- Toggle sRGB framebuffer\n"
			"  [f] -- Toggle wireframe rendering\n"
			"  [o] -- Set to orthographic camera\n"
			"  [p] -- Set to perspective camera\n"
			"  [ESC] -- Quit\n"
	);

	return true;
}

void Assignment2::windowResize(int width, int height)
{
	/*
	 * This function is called automatically when the
	 * window is resized.
	 * width & height are the new width & height, in pixels,
	 * of the window
	 */
	float aspect = width / (float)height;
	m_camera.setAspect(aspect);

	m_windowWidth = width;
	m_windowHeight = height;
}

void Assignment2::toggleCameraMoveDirection(bool enable, int direction)
{
	if (enable) // enable movement
		m_cameraMovement = m_cameraMovement | direction;
	else
		m_cameraMovement = m_cameraMovement & (~direction);
	m_lastCamMoveTime = UI::getTime();
}

void Assignment2::specialKeyboard(UI::KeySpecial_t key, UI::ButtonState_t state)
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

	case UI::KEY_F1:
		if (state == UI::BUTTON_DOWN)
			m_enableShadows = !m_enableShadows;
		break;

	case UI::KEY_G:
		if (state == UI::BUTTON_DOWN)
		{
			m_sRGBframebuffer = !m_sRGBframebuffer;
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

void Assignment2::rasterizeScene()
{
	if (m_renderWireframe)
	{
		// Turn on wireframe rendering
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	if (m_sRGBframebuffer)
	{
		glEnable(GL_FRAMEBUFFER_SRGB_EXT);
	}

	// Turn on backface culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	// Turn on the depth buffer
	glEnable(GL_DEPTH_TEST);

	// Clear the pixel data & depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (isGLError()) return;

	// Bind the shadowmap to texture unit 1
	m_shadowmap.bindGL(GL_TEXTURE1);
	if (isGLError()) return;

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

		if (shader->getIsReady(m_enableShadows))
		{
			shader->bindGL(m_enableShadows); // Bind the shader to the OpenGL context
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
				m_scene[i]->getMaterial().getTexture()->bindGL(GL_TEXTURE0); // Set up texture
			}
			// Set up the specular components of the uniforms struct if the material
			// is specular
			if (m_scene[i]->getMaterial().hasSpecular())
			{
				shaderUniforms.m_specExp = m_scene[i]->getMaterial().getSpecExp();
				shaderUniforms.m_specRefl = m_scene[i]->getMaterial().getSpecRefl();
			}

			// Set the shader uniform variables
			if ( !shader->setUniforms(shaderUniforms, m_enableShadows) || isGLError() ) return;

			// Rasterize the object
			m_scene[i]->rasterize();
			if (isGLError()) return;

			// Unbind the shader from the OpenGL context
			shader->unbindGL();
		}
	}

	m_shadowmap.unbindGL(GL_TEXTURE1);

	// Put the render state back the way we found it
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);

	if (m_sRGBframebuffer)
	{
		glDisable(GL_FRAMEBUFFER_SRGB_EXT);
	}
	if (m_renderWireframe)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	// Force everything outstanding to complete
	glFinish();
}

void Assignment2::repaint()
{
	/*
	 * Called automatically whenever the window needs to be
	 * redrawn.
	 * This will end up being called after any user event (keypress, etc),
	 * and pretty much every time through the event loop.
	 */
	if (m_enableShadows)
	{
		m_shadowmap.create((const Object::Object**)m_scene, m_nObjects, m_lightPos, m_camera);
		if ( isGLError() ) return;
	}

	// Bind the default framebuffer.
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glViewport(0,0,m_windowWidth,m_windowHeight);

	rasterizeScene();
}

void Assignment2::idle()
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

	const float deltaT = currTime - m_lastIdleTime;

	// TODO!
	//  -- Animate the light. Each call to idle should rotate the light
	//   around the Y-axis by 0.5 * deltaT * m_rotationSpeed radians.
	//   Note: Don't rotate if deltaT == 0

	if (deltaT != 0)
	{
		m_lightPos = gml::mul(gml::rotateYh(0.5 * deltaT * m_rotationSpeed), m_lightPos);
	}

	m_lastIdleTime = currTime;
}

