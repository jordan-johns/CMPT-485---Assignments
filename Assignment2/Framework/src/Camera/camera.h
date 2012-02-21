
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
 * Class definition for a camera class.
 *
 * This implementation uses a gaze, up, right frame of reference
 * rather than Euler angles, or quaternions.
 *
 * Has support for moving the camera around the world.
 */

#pragma once
#ifndef __INC_CAMERA_H_
#define __INC_CAMERA_H_

#include "../GML/gml.h"

typedef enum {
	CAMERA_PROJECTION_PERSPECTIVE,
	CAMERA_PROJECTION_ORTHOGRAPHIC
} CameraProjection;

// Camera with a right-handed cam-space coordinate frame
class Camera
{
protected:
	CameraProjection m_projectionType;

	// Camera frame. All normalized
	gml::vec3_t m_viewDir, m_up, m_right;
	// Camera position
	gml::vec3_t m_camPos;

	// world to camera coordinate transform.
	gml::mat4x4_t m_worldView;

	// Perspective projection matrix
	gml::mat4x4_t m_perspective;
	// Orthographic mapping
	//  Projects cam coords to normalized device coordinates (i.e. [-1,1] x [-1,1] x [-1,1])
	gml::mat4x4_t m_ortho;

	// Cache for projetion matrix.
	//   If orthographic, then this should be = m_ortho
	//   else it should be = to m_ortho x m_perspective
	gml::mat4x4_t m_projection;

	// vector for near & far clip planes: .x = near, .y = far)
	gml::vec2_t m_depthClip;
	float m_fov; // vertical field of view in radians
	float m_aspect; // Aspect ratio (w/h) of the view screen

	// Setup m_worldToCam from m_viewDir, m_up, m_right, m_camPos
	void setWorldView();
	// Setup the perspective projection matrix from near & far
	void setPerspective();
	// Setup m_ortho from m_fov, m_aspect, and m_depthClip
	void setOrtho();
	// Set: m_projection to appropriate
	void setProjection();
public:
	Camera();
	~Camera();

	// Place the camera at coordinates 'camPos', aim it toward 'target',
	// with the top of the camera in the 'up' direction.
	void lookAt(const gml::vec3_t camPos, const gml::vec3_t target, const gml::vec3_t up=gml::vec3_t(0.0f,1.0f,0.0f));

	void setPosition(const gml::vec3_t camPos);
	void setCameraProjection(CameraProjection type);
	void setFOV(const float fov);
	void setAspect(const float aspect);
	void setDepthClip(const float near, const float far); // 0 < near < far

	const gml::mat4x4_t& getWorldView() const { return m_worldView; }
	const gml::mat4x4_t& getProjection() const { return m_projection; }
	float getFOV() const { return m_fov; }
	float getAspect() const { return m_aspect; }
	float getNearClip() const { return m_depthClip.x; }
	float getFarClip() const { return m_depthClip.y; }

	// Movement controls
	void moveForward(const float distance); // distance < 0 => backward
	void moveUp(const float distance); // distance < 0 => down
	void strafeRight(const float distance); // distance < 0 => strafe left
	// rotate the camera about the up vector
	void rotateRight(const float angle); // angle in radians
	// rotate the camera about the right vector
	void rotateUp(const float angle); // angle in radians
	// rotate the camera about the view vector
	void spinCamera(const float angle); // angle in radians
};

#endif
