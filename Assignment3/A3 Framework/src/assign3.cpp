
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
#include "assign3.h"

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

static const int MAX_RT_PASSES = 500;
static const int MAX_RAY_DEPTH = 2;

Assignment3::Assignment3()
{
	m_geometry = 0;
	m_nGeometry = 0;

	m_movementSpeed = 2.0f; // Camera movement speed = 2 units / second
	m_cameraMovement = 0; // Initialized for no camera motion
	m_rotationSpeed = (40.0f * M_PI) / 180.0f; // Camera rotation speed = 40 degrees / second

	// Is the framebuffer automatically gamma-correcting?
	m_sRGBframebuffer = false;
	// Start with wireframe rendering turned off
	m_renderWireframe = false;

	m_shadowmapSize = 1024;
	m_useShadowMap = true;

	m_lastIdleTime = UI::getTime();

	m_rtImage = 0;
	m_isRayTracing = false;
	m_rtFBO = 0;
	m_rtTex = 0;

	m_cameraChanged = true;
}

Assignment3::~Assignment3()
{
	if (m_geometry)
	{
		for (GLuint i=0; i<m_nGeometry; i++)
		{
			if (m_geometry[i]) delete m_geometry[i];
		}
		delete[] m_geometry;
	}
	// Ray tracing cleanup
	if (m_rtImage)
	{
		delete[] m_rtImage;
	}
	if (m_rtFBO)
	{
		glDeleteFramebuffers(1, &m_rtFBO);
	}
	if (m_rtTex)
	{
		glDeleteTextures(1, &m_rtTex);
	}
}

bool Assignment3::init()
{
	/*
	 * This function is called by main() when the program is starting up.
	 * It should initialize whatever data the assignment requires.
	 */

	if ( !m_scene.init() )
	{
		fprintf(stderr, "Could not initialize scene object\n");
		return false;
	}

	m_texture = new Texture::Texture("gray_wall.png");
	if ( !m_texture->getIsReady() )
	{
		fprintf(stderr, "ERROR! Texture not ready\n");
		return false;
	}

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



	const float pi2 = (90.0f * M_PI) / 180.0f;

	m_scene.setAmbient(gml::vec3_t(0.0, 0.0, 0.0));
	m_scene.setLightPos(gml::vec4_t(0.0, 4.5, 0.0 , 1.0));
	m_scene.setLightRad(gml::vec3_t(1.0, 1.0, 1.0));

	m_camera.lookAt(gml::vec3_t(0.0,2.0,3.0), gml::vec3_t(0.0,0.0,0.0) );
	m_camera.setDepthClip(0.5f, 30.0f);

	Material::Material mat;

	// derp
		mat.setMirror(true);

	mat.setShaderType(Material::PHONG);

	gml::vec3_t beige(0.76, 0.75, 0.5);
	gml::vec3_t red(0.63, 0.06, 0.04);
	gml::vec3_t green(0.15, 0.48, 0.09);

	mat.setSpecExp(-1.0f); // turn off specular

	mat.setSurfReflectance(beige);
	// Ground plane
	mat.setTexture(m_texture);
	m_scene.addObject(new Object::Object(m_geometry[PLANE_LOC], mat,
			gml::mul(gml::translate(gml::vec3_t(0.0,0.0,0.0)), gml::scaleh(5.0, 1.0, 5.0)) ) );
	// Box "top"
	mat.setTexture(0);
	m_scene.addObject(new Object::Object(m_geometry[PLANE_LOC], mat,
			gml::mul(gml::translate(gml::vec3_t(0.5,5.0,0.0)), gml::mul(gml::rotateZh(2*pi2),gml::scaleh(5.0, 1.0, 5.0))) ) );

	// "Box" walls
	mat.setSurfReflectance(green);
	m_scene.addObject(new Object::Object(m_geometry[PLANE_LOC], mat,
			gml::mul(gml::translate(gml::vec3_t(5.0,2.5,0.0)), gml::mul(gml::rotateZh(pi2),gml::scaleh(2.5, 1.0, 5.0))) ) );
	m_scene.addObject(new Object::Object(m_geometry[PLANE_LOC], mat,
			gml::mul(gml::translate(gml::vec3_t(-5.0,2.5,0.0)), gml::mul(gml::rotateZh(-pi2),gml::scaleh(2.5, 1.0, 5.0))) ));
	mat.setSurfReflectance(red);
	m_scene.addObject(new Object::Object(m_geometry[PLANE_LOC], mat,
			gml::mul(gml::translate(gml::vec3_t(0.0,2.5,5.0)), gml::mul(gml::rotateXh(-pi2),gml::scaleh(5.0, 1.0, 2.5))) ));
	m_scene.addObject(new Object::Object(m_geometry[PLANE_LOC], mat,
			gml::mul(gml::translate(gml::vec3_t(0.0,2.5,-5.0)), gml::mul(gml::rotateXh(pi2),gml::scaleh(5.0, 1.0, 2.5))) ));


	// Light blocker
	mat.setSurfReflectance(beige);
	m_scene.addObject(new Object::Object(m_geometry[SPHERE_LOC], mat,
			gml::mul(gml::translate(gml::vec3_t(0.0,2.0,0.0)), gml::scaleh(1.5, 0.15, 2.5)) ) );


	gml::mat4x4_t rotScale = gml::mul( gml::rotateYh((25.0f * M_PI)/180.0), gml::scaleh(0.5,0.5,0.5) );
	// Some other objects
	mat.setSurfReflectance(beige);
	m_scene.addObject(new Object::Object(m_geometry[OCTAHEDRON_LOC], mat,
			gml::mul(gml::translate(gml::vec3_t(0.0,0.75,0.0)), rotScale)) );

	// Left Sphere
	mat.setMirror(true);
	mat.setSurfReflectance(gml::vec3_t(0,0,0));
		m_scene.addObject(new Object::Object(m_geometry[SPHERE_LOC], mat,
				gml::mul(gml::translate(gml::vec3_t(-2.0,0.75,-2.0)), rotScale)) );


	// Right Sphere
	mat.setMirror(false);
	mat.setSurfReflectance(beige);
		m_scene.addObject(new Object::Object(m_geometry[SPHERE_LOC], mat,
				gml::mul(gml::translate(gml::vec3_t(2.0,0.75,-2.0)), rotScale)) );

	// =============================================================================================

	if ( !m_shadowmap.init(m_shadowmapSize) )
	{
		fprintf(stderr, "Failed to initialize shadow mapping members.\n");
		return false;
	}

	// Ray tracing inits
	if (m_rtFBO) glDeleteFramebuffers(1, &m_rtFBO);
	glGenFramebuffers(1, &m_rtFBO);

	if (m_rtTex) glDeleteTextures(1, &m_rtTex);
	glGenTextures(1, &m_rtTex);

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
			"  [F2] -- Toggle ray tracing\n"
			"  [g] -- Toggle sRGB framebuffer\n"
			"  [f] -- Toggle wireframe rendering\n"
			"  [o] -- Set to orthographic camera\n"
			"  [p] -- Set to perspective camera\n"
			"  [ESC] -- Quit\n"
	);

	return true;
}

void Assignment3::windowResize(int width, int height)
{
	/*
	 * This function is called automatically when the
	 * window is resized.
	 * width & height are the new width & height, in pixels,
	 * of the window
	 */
	m_camera.setImageDimensions(width, height);
	m_cameraChanged = true;
	m_windowWidth = width;
	m_windowHeight = height;

	// This function is always called when this object is attached to the UI
	// So, we can safely allocate the m_rtImage here and not have to worry
	// about trying to use a null ptr.

	if (m_rtImage)
	{
		delete[] m_rtImage;
	}
	m_rtImage = new gml::vec3_t[width * height];

	assert(m_rtTex != 0);
	glBindTexture(GL_TEXTURE_2D, m_rtTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_FLOAT, 0);
}

void Assignment3::toggleCameraMoveDirection(bool enable, int direction)
{
	if (enable) // enable movement
		m_cameraMovement = m_cameraMovement | direction;
	else
		m_cameraMovement = m_cameraMovement & (~direction);
	m_lastCamMoveTime = UI::getTime();
}

void Assignment3::specialKeyboard(UI::KeySpecial_t key, UI::ButtonState_t state)
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
		if (m_isRayTracing) break;
		toggleCameraMoveDirection(state == UI::BUTTON_DOWN, CAMERA_FORWARD);
		break;
	case UI::KEY_S:
		if (m_isRayTracing) break;
		toggleCameraMoveDirection(state == UI::BUTTON_DOWN, CAMERA_BACKWARD);
		break;

	case UI::KEY_A:
		if (m_isRayTracing) break;
		toggleCameraMoveDirection(state == UI::BUTTON_DOWN, CAMERA_STRAFE_LEFT);
		break;
	case UI::KEY_D:
		if (m_isRayTracing) break;
		toggleCameraMoveDirection(state == UI::BUTTON_DOWN, CAMERA_STRAFE_RIGHT);
		break;

	case UI::KEY_Q:
		if (m_isRayTracing) break;
		toggleCameraMoveDirection(state == UI::BUTTON_DOWN, CAMERA_UP);
		break;
	case UI::KEY_E:
		if (m_isRayTracing) break;
		toggleCameraMoveDirection(state == UI::BUTTON_DOWN, CAMERA_DOWN);
		break;


	case UI::KEY_KP_8:
		if (m_isRayTracing) break;
		toggleCameraMoveDirection(state == UI::BUTTON_DOWN, CAMERA_ROTATE_UP);
		break;
	case UI::KEY_KP_5:
		if (m_isRayTracing) break;
		toggleCameraMoveDirection(state == UI::BUTTON_DOWN, CAMERA_ROTATE_DOWN);
		break;

	case UI::KEY_KP_4:
		if (m_isRayTracing) break;
		toggleCameraMoveDirection(state == UI::BUTTON_DOWN, CAMERA_ROTATE_LEFT);
		break;
	case UI::KEY_KP_6:
		if (m_isRayTracing) break;
		toggleCameraMoveDirection(state == UI::BUTTON_DOWN, CAMERA_ROTATE_RIGHT);
		break;

	case UI::KEY_KP_7:
		if (m_isRayTracing) break;
		toggleCameraMoveDirection(state == UI::BUTTON_DOWN, CAMERA_SPIN_LEFT);
		break;
	case UI::KEY_KP_9:
		if (m_isRayTracing) break;
		toggleCameraMoveDirection(state == UI::BUTTON_DOWN, CAMERA_SPIN_RIGHT);
		break;

	case UI::KEY_F1:
		if (m_isRayTracing) break;
		if (state == UI::BUTTON_DOWN)
			m_useShadowMap = !m_useShadowMap;
		break;

	case UI::KEY_F2:
		if (state == UI::BUTTON_DOWN)
		{
			m_isRayTracing = !m_isRayTracing;
			if (m_isRayTracing && m_cameraChanged)
			{
				m_rtRow = 0; // start at the beginning of the image.
				glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_rtFBO);
				glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_rtTex, 0);
				isGLError();
				glClearColor(1.0, 1.0, 1.0, 1.0);
				glClear(GL_COLOR_BUFFER_BIT);
				glClearColor(0.0, 0.0, 0.0, 1.0);
				isGLError();
				glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
				m_cameraChanged = false;
				// Zero(black)-out the image
				memset(m_rtImage, 0x00, sizeof(gml::vec3_t)*m_windowHeight*m_windowWidth);
				m_rtPassNum = 0;
			}
		}
		break;

	case UI::KEY_G:
		if (m_isRayTracing) break;
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
		if (m_isRayTracing) break;
		if (state == UI::BUTTON_DOWN)
		{
			m_renderWireframe = !m_renderWireframe;
		}
		break;

	case UI::KEY_O:
		if (m_isRayTracing) break;
		m_camera.setCameraProjection(CAMERA_PROJECTION_ORTHOGRAPHIC);
		break;

	case UI::KEY_P:
		if (m_isRayTracing) break;
		m_camera.setCameraProjection(CAMERA_PROJECTION_PERSPECTIVE);
		break;

	default:
		break;
	}
}

void Assignment3::rasterizeScene()
{
	if (m_renderWireframe)
	{
		// Turn on wireframe rendering
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
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
	if (m_useShadowMap)
	{
		m_shadowmap.bindGL(GL_TEXTURE1);
		if (isGLError()) return;
	}

	m_scene.rasterize(m_camera.getWorldView(), m_camera.getProjection(), m_useShadowMap);

	if (m_useShadowMap)
	{
		m_shadowmap.unbindGL(GL_TEXTURE1);
	}

	// Put the render state back the way we found it
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);

	if (m_renderWireframe)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	// Force everything outstanding to complete
	glFinish();
}

void Assignment3::repaint()
{
	/*member
	 * Called automatically whenever the window needs to be
	 * redrawn.
	 * This will end up being called after any user event (keypress, etc),
	 * and pretty much every time through the event loop.
	 */
	if (m_sRGBframebuffer)
	{
		glEnable(GL_FRAMEBUFFER_SRGB_EXT);
	}

	if (m_isRayTracing)
	{
		glBindFramebuffer(GL_READ_FRAMEBUFFER, m_rtFBO);
		glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_rtTex, 0);
		if (isGLError()) return;
		glBlitFramebuffer(
				0, 0, m_windowWidth, m_windowHeight,
				0, 0, m_windowWidth, m_windowHeight,
				GL_COLOR_BUFFER_BIT, GL_NEAREST);
		if (isGLError()) return;
	}
	else {
		if (m_useShadowMap)
		{
			m_shadowmap.create(m_scene, m_camera.getWorldView());
			if ( isGLError() ) return;
		}

		// Bind the default framebuffer.
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glViewport(0,0,m_windowWidth,m_windowHeight);

		rasterizeScene();
	}

	if (m_sRGBframebuffer)
	{
		glDisable(GL_FRAMEBUFFER_SRGB_EXT);
	}
}

void Assignment3::idle()
{
	// idle is called each time through the event loop when there is no
	// pending render call, or interaction event to process.
	// That makes it ideal for animation, and similar actions

	double currTime = UI::getTime(); // current time

	if (m_isRayTracing)
	{
		if (m_rtRow < m_windowHeight)
		{
			GLuint startRow = m_rtRow;

			// Ray trace rows for TIMEOUT s
			float timeout = 0.1f; // 100ms

			RayTracing::Ray_t ray;
			RayTracing::HitInfo_t hitinfo;
			double time = currTime;
			do
			{
				gml::vec3_t *imgPos = m_rtImage + m_rtRow*m_windowWidth;

				for (int c=0; c<m_windowWidth; c++, imgPos++)
				{
					gml::vec3_t clr(0.0, 0.0, 0.0);

					// (x,y) give the screen-space (aka: image-space, or window-space) coordinates of
					// the ray to be cast.
					const float x = c - 0.5 + rand() / ((float)RAND_MAX);
					const float	y = m_rtRow - 0.5 + rand() / ((float)RAND_MAX);


					// TODO!!
					//   Create the ray through (x,y) from the camera, then use the m_scene
					// object to find an intersection of the ray, and shade the ray if there
					// is an intersection.
					//  Assign the shade of the ray to the 'clr' variable.
					//  The recursive depth for shading the ray is given by the
					// constant MAX_RAY_DEPTH (found at top of this file)

					ray = m_camera.genViewRay(x, y);

					// @HACK - fix color shade
					if (m_scene.rayIntersects(ray, m_camera.getNearClip(), m_camera.getFarClip(), hitinfo))
					{
							//clr = hitinfo.objHit->getMaterial().getSurfRefl();
							clr = m_scene.shadeRay(ray, hitinfo, 5);
					}

					// Use 'clr' to update the image
					if (m_rtPassNum == 0)
					{
						*imgPos = clr;
					}
					else
					{
						*imgPos = gml::scale(1.0f/(m_rtPassNum+1),	gml::add( gml::scale(m_rtPassNum,*imgPos), clr ) );
					}
				}

				m_rtRow += 1;
				time = UI::getTime();
			} while (m_rtRow < m_windowHeight && (time-currTime)<timeout);

			// Copy the new image data to the texture for blitting to the screen.
			glBindTexture(GL_TEXTURE_2D, m_rtTex);
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, startRow, m_windowWidth, m_rtRow-startRow, GL_RGB, GL_FLOAT, m_rtImage+startRow*m_windowWidth);
			if (isGLError()) return;
		}
		else
		{
			if (m_rtPassNum < MAX_RT_PASSES)
			{
				m_rtRow = 0;
				m_rtPassNum += 1;
				fprintf(stdout, "Pass %d Complete\n", m_rtPassNum);
			}
		}
	}
	else
	{
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

				m_cameraChanged = true;
				m_lastCamMoveTime = currTime;
			}
		}

	}

	m_lastIdleTime = currTime;
}

