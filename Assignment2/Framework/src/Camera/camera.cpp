
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

#include <cassert>
#include <cmath>
#include "camera.h"

Camera::Camera()
{
	m_right = gml::vec3_t(1.0,0.0,0.0);
	m_up = gml::vec3_t(0.0,1.0,0.0);
	m_viewDir = gml::vec3_t(0.0,0.0,-1.0);
	m_camPos = gml::vec3_t(0.0,0.0,0.0);
	this->setWorldView();

	m_projectionType = CAMERA_PROJECTION_PERSPECTIVE;
	m_perspective = gml::identity4();
	m_depthClip = gml::vec2_t(1.0f, 30.0f);
	this->setPerspective();

	m_fov = (60.0f * M_PI)/180.0f; // 60 degrees
	m_aspect = 1.0f;
	m_ortho = gml::identity4();
	this->setOrtho();
	this->setProjection();
}

Camera::~Camera() {}

void Camera::setWorldView()
{
	/* Set the matrix m_worldView to be the transformation matrix
	 * from world coordinates to the camera frame.
	 *
	 * Use equation 7.4 of section 7.1.3 to calculate m_worldView (= M_cam)
	 *  with: u = m_right
	 *        v = m_up
	 *        w = m_viewDir
	 *        e = m_camPos
	 */
	m_worldView[0] = gml::vec4_t(m_right.x, m_up.x, m_viewDir.x, 0.0);
	m_worldView[1] = gml::vec4_t(m_right.y, m_up.y, m_viewDir.y, 0.0);
	m_worldView[2] = gml::vec4_t(m_right.z, m_up.z, m_viewDir.z, 0.0);
	m_worldView[3] = gml::vec4_t( -gml::dot(m_right, m_camPos), -gml::dot(m_up, m_camPos), -gml::dot(m_viewDir, m_camPos), 1.0);
}

void Camera::setPerspective()
{
	/*
	 * Set m_perspective to be the perspective transformation matrix.
	 *  m_perspective is called "P" in section 7.3
	 *
	 *  n = near plane = this->getNearClip()
	 *  f = far plane = this->getFarClip()
	 */
	float near = this->getNearClip();
	float far = this->getFarClip();
	m_perspective[0][0] = near;
	m_perspective[1][1] = near;
	m_perspective[2][2] = (near + far);
	m_perspective[3][2] = near * far;
	m_perspective[2][3] = -1.0f;
	m_perspective[3][3] = 0.0f;
}

void Camera::setOrtho()
{
	/*
	 * Set m_ortho as in equation 7.3 of section 7.1.2 with
	 * l, r, t, & b derived from the field of view (section 7.5)
	 *
	 *  n = near plane = this->getNearClip()
	 *  f = far plane = this->getFarClip()
	 *
	 *  theta = m_fov  (section 7.5)
	 *  n_x / n_y = m_aspect (section 7.5)
	 *
	 */
	float near = this->getNearClip();
	float far = this->getFarClip();
	float tanHalf = tanf(0.5f * m_fov);
	float t = near * tanHalf; // near * tan(fov/2)
	float r = m_aspect * t;
	m_ortho[0][0] = 1.0 / r;
	m_ortho[1][1] = 1.0 / t;
	m_ortho[2][2] = 2.0 / (near - far);
	m_ortho[3][2] = (near + far)/(near - far);
}

void Camera::setProjection()
{
	if (m_projectionType == CAMERA_PROJECTION_PERSPECTIVE)
	{
		m_projection = gml::mul(m_ortho, m_perspective);
	}
	else
	{
		m_projection = m_ortho;
	}
}

void Camera::setPosition(const gml::vec3_t camPos)
{
	m_camPos = camPos;
	this->setWorldView();
}

void Camera::lookAt(const gml::vec3_t camPos, const gml::vec3_t target, const gml::vec3_t up)
{

	/*
	 * Set m_camPos to camPos
	 * Then, m_viewDir to be the unit-length vector in the direction of
	 * 'target' from 'camPos'
	 *
	 * m_right & m_up are then set as in section 2.4.6, with
	 * w = m_viewDir, t = up,
	 * u = m_right,
	 * v = m_up
	 *
	 * This will place the camera at 'camPos' and point it in the
	 * direction of 'target', with 'up' being the general direction
	 * of up for the camera.
	 *
	 * Note: When using this function, 'up' cannot be collinear with (camPos - target)
	 */
	// Set view direction
	m_viewDir = gml::normalize( gml::sub( camPos, target ) );
	// Set right direction
	m_right = gml::normalize( gml::cross( up, m_viewDir ) );
	// Set the up direction
	m_up = gml::normalize( gml::cross( m_viewDir, m_right ) );

	assert( fabsf(gml::length(m_viewDir) - 1.0) < 1e-6 );
	assert( fabsf(gml::length(m_up) - 1.0) < 1e-6 );
	assert( fabsf(gml::length(m_right) - 1.0) < 1e-6 );
	assert( fabsf(gml::dot(m_viewDir, m_right)) < 1e-6 );
	assert( fabsf(gml::dot(m_viewDir, m_up)) < 1e-6 );
	assert( fabsf(gml::dot(m_up, m_right)) < 1e-6 );

	m_camPos = camPos;
	this->setWorldView();
}

void Camera::setCameraProjection(CameraProjection type)
{
	if (type == m_projectionType)
		return;
	m_projectionType = type;
	this->setProjection();
}

void Camera::setFOV(float fov)
{
	assert(fov > 0);
	m_fov = fov;
	this->setOrtho();
	this->setProjection();
}

void Camera::setAspect(float aspect)
{
	assert(aspect > 0);
	m_aspect = aspect;
	this->setOrtho();
	this->setProjection();
}

void Camera::setDepthClip(float near, float far)
{
	assert(0 < near);
	assert(near < far);
	m_depthClip = gml::vec2_t(near, far);
	this->setOrtho();
	this->setPerspective();
	this->setProjection();
}

void Camera::moveForward(const float distance)
{
	/*
	 * Adjust m_camPos so that the camera moves forward by distance
	 */
	m_camPos = gml::add(m_camPos, gml::scale(-distance, m_viewDir));
	this->setWorldView();
}

void Camera::moveUp(const float distance)
{
	/*
	 * Adjust m_camPos so that the camera moves backward by distance
	 */
	m_camPos = gml::add(m_camPos, gml::scale(distance, m_up));
	this->setWorldView();
}

void Camera::strafeRight(const float distance)
{
	/*
	 * Adjust m_camPos so that the camera strafes right by distance
	 */
	m_camPos = gml::add(m_camPos, gml::scale(distance, m_right));
	this->setWorldView();
}

// rotate the camera about the up vector
void Camera::rotateRight(const float angle)
{
	/*
	 * Adjust m_viewDir, m_right, and m_up to effectively
	 * rotate the camera about the m_up vector
	 *
	 * Note: You will want to use gml::rotateAxis()
	 * Note2: You must ensure that m_viewDir, m_up, and m_right
	 *  are all unit-length when you are done.
	 */
	gml::mat3x3_t rotMat = gml::rotateAxis(angle, m_up);
	m_viewDir = gml::normalize( gml::mul(rotMat, m_viewDir) );
	m_right = gml::normalize( gml::cross( m_up, m_viewDir ) );
	m_up = gml::normalize( gml::cross( m_viewDir, m_right ) );

	assert( fabsf(gml::length(m_viewDir) - 1.0) < 1e-6 );
	assert( fabsf(gml::length(m_up) - 1.0) < 1e-6 );
	assert( fabsf(gml::length(m_right) - 1.0) < 1e-6 );
	assert( fabsf(gml::dot(m_viewDir, m_right)) < 1e-6 );
	assert( fabsf(gml::dot(m_viewDir, m_up)) < 1e-6 );
	assert( fabsf(gml::dot(m_up, m_right)) < 1e-6 );

	this->setWorldView();
}
// rotate the camera about the right vector
void Camera::rotateUp(const float angle)
{
	/*
	 * Adjust m_viewDir, m_right, and m_up to effectively
	 * rotate the camera about the m_right vector
	 *
	 * Note: You will want to use gml::rotateAxis()
	 * Note2: You must ensure that m_viewDir, m_up, and m_right
	 *  are all unit-length when you are done.
	 */
	gml::mat3x3_t rotMat = gml::rotateAxis(angle, m_right);
	m_viewDir = gml::normalize( gml::mul(rotMat, m_viewDir) );
	m_up = gml::normalize( gml::cross( m_viewDir, m_right ) );
	m_right = gml::normalize( gml::cross( m_up, m_viewDir ) );

	assert( fabsf(gml::length(m_viewDir) - 1.0) < 1e-6 );
	assert( fabsf(gml::length(m_up) - 1.0) < 1e-6 );
	assert( fabsf(gml::length(m_right) - 1.0) < 1e-6 );
	assert( fabsf(gml::dot(m_viewDir, m_right)) < 1e-6 );
	assert( fabsf(gml::dot(m_viewDir, m_up)) < 1e-6 );
	assert( fabsf(gml::dot(m_up, m_right)) < 1e-6 );

	this->setWorldView();
}

// rotate the camera about the view vector
void Camera::spinCamera(const float angle)
{
	/*
	 * Adjust m_viewDir, m_right, and m_up to effectively
	 * rotate the camera about the m_viewDir vector
	 *
	 * Note: You will want to use gml::rotateAxis()
	 * Note2: You must ensure that m_viewDir, m_up, and m_right
	 *  are all unit-length when you are done.
	 */
	gml::mat3x3_t rotMat = gml::rotateAxis(angle, m_viewDir);
	m_up = gml::normalize( gml::mul(rotMat, m_up) );
	m_right = gml::normalize( gml::cross( m_up, m_viewDir ) );
	m_viewDir = gml::normalize( gml::cross(m_right, m_up) );

	assert( fabsf(gml::length(m_viewDir) - 1.0) < 1e-6 );
	assert( fabsf(gml::length(m_up) - 1.0) < 1e-6 );
	assert( fabsf(gml::length(m_right) - 1.0) < 1e-6 );
	assert( fabsf(gml::dot(m_viewDir, m_right)) < 1e-6 );
	assert( fabsf(gml::dot(m_viewDir, m_up)) < 1e-6 );
	assert( fabsf(gml::dot(m_up, m_right)) < 1e-6 );

	this->setWorldView();
}
