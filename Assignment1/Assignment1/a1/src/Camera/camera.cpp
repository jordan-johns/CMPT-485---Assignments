
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
#include <stdio.h>
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

	m_projectionType = CAMERA_PROJECTION_ORTHOGRAPHIC;
	m_perspective = gml::identity4();
	m_perspective[2][3] = 1.0;
	m_perspective[3][3] = 0.0;
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

	gml::vec3_t u = m_right;
	gml::vec3_t v = m_up;
	gml::vec3_t w = m_viewDir;

	// Set up the left side of the transformation matrix component.
	gml::mat4x4_t left = gml::identity4();
	left[0][0] = u.x;
	left[1][0] = u.y;
	left[2][0] = u.z;

	left[0][1] = v.x;
	left[1][1] = v.y;
	left[2][1] = v.z;

	left[0][2] = w.x;
	left[1][2] = w.y;
	left[2][2] = w.z;

	// Set up the right side of the transformation matrix component.
	gml::mat4x4_t right = gml::identity4();
	right[3][0] = -1.0 * m_camPos.x;
	right[3][1] = -1.0 * m_camPos.y;
	right[3][2] = -1.0 * m_camPos.z;

	// Multiply the damn things together to get your world space -> camera space transformation matrix...
	m_worldView = gml::mul(left, right);

	/*printf("LEFT\n");
	print_mat4x4(left);
	printf("\nRIGHT\n");
	print_mat4x4(right);*/
	//printf("\nWORLD_VIEW\n");
	//print_mat4x4(m_worldView);
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
    m_perspective = gml::identity4();

    float n = m_depthClip.x;
    float f = m_depthClip.y;

    m_perspective[0][0] = n;
    m_perspective[1][1] = n;
    m_perspective[2][2] = f + n;
    m_perspective[3][3] = 0;
    m_perspective[2][3] = 1;
    m_perspective[3][2] = -1 *  n * f;
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
	float n = m_depthClip.x;
	float f = m_depthClip.y;

	float t = tan(m_fov/2.0f) * fabs(n) ;
	float b = -t;

	float r = m_aspect * t;
	float l = -r;

	/*printf("n: %f\n", n);
	printf("f: %f\n", f);
	printf("t: %f\n", t);
	printf("b: %f\n", b);
	printf("r: %f\n", r);
	printf("1/r: %f\n", 1.0f/r);
	printf("l: %f\n", l);*/

	m_ortho = gml::identity4();

	m_ortho[0][0] = 2.0f / (r - l);
	m_ortho[1][1] = 2.0f / (t - b);
	m_ortho[2][2] = 2.0f / (f - n);

	m_ortho[3][0] = - (r + l) / (r - l);
	m_ortho[3][1] = - (t + b) / (t - b);
	m_ortho[3][2] = - (n + f) / (f - n);

	print_mat4x4(m_ortho);
}

void Camera::setProjection()
{
	// Set the projection matrix based on what type of camera
	// this is supposed to be
	if (m_projectionType == CAMERA_PROJECTION_PERSPECTIVE)
	{
		m_projection = gml::mul(m_ortho, m_perspective);
	}
	else
	{
		m_projection = m_ortho;
	}
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

	m_camPos = camPos;

	//m_up = up;
	m_viewDir = gml::sub(target, camPos);
	m_viewDir = gml::normalize(m_viewDir);

	m_right = gml::cross(m_viewDir, up);
	m_right = gml::normalize(m_right);

	m_up = gml::cross(m_right, m_viewDir);
	m_up = gml::normalize(m_up);

	/*printf("CAM_POS\n");
	print_vec3(m_camPos);
	printf("\nTARGET\n");
	print_vec3(target);
	printf("\nVIEW_DIR\n");
	print_vec3(m_viewDir);
	printf("\nRIGHT\n");
	print_vec3(m_right);
	printf("\nUP\n");
	print_vec3(m_up);
*/
	// Do this at the end
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
	this->setPerspective();
	this->setProjection();
}

void Camera::moveForward(const float distance)
{
	/*
	 * Adjust m_camPos so that the camera moves forward by distance
	 */
	gml::vec3_t move = gml::scale(distance, gml::normalize(m_viewDir));
	m_camPos = gml::add(m_camPos, move);

	// Do this at the end
	this->setWorldView();
}

void Camera::moveUp(const float distance)
{
	/*
	 * Adjust m_camPos so that the camera moves backward by distance
	 */
	gml::vec3_t move = gml::scale(distance, gml::normalize(m_up));
	m_camPos = gml::add(m_camPos, move);

	// Do this at the end
	this->setWorldView();
}

void Camera::strafeRight(const float distance)
{
	/*
	 * Adjust m_camPos so that the camera strafes right by distance
	 */
	gml::vec3_t move = gml::scale(distance, gml::normalize(m_right));
	m_camPos = gml::add(m_camPos, move);

	// Do this at the end
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
	m_right = gml::normalize(gml::mul(rotMat, m_right));

	m_viewDir = gml::cross(m_up, m_right);
	m_viewDir = gml::normalize(m_viewDir);

	m_up = gml::cross(m_right, m_viewDir);
	m_up = gml::normalize(m_up);

	// Do this at the end
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
	m_viewDir = gml::normalize(gml::mul(rotMat, m_viewDir));

	m_up = gml::cross(m_right, m_viewDir);
	m_up = gml::normalize(m_up);

	m_right = gml::cross(m_viewDir, m_up);
	m_right = gml::normalize(m_right);

	// Do this at the end
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
	m_up = gml::normalize(gml::mul(rotMat, m_up));

	m_right = gml::cross(m_viewDir, m_up);
	m_right = gml::normalize(m_right);

	m_viewDir = gml::cross(m_up, m_right );
	m_viewDir = gml::normalize(m_viewDir);

	// Do this at the end
	this->setWorldView();
}
