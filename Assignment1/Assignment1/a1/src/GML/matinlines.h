
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

#pragma once

// Inline function definitions for matrix functions.
// This file should not be directly included by anything other than gmath.h


inline mat2x2_t identity2()
{
	mat2x2_t dst;
	dst[0] = vec2_t(1.0f, 0.0f);
	dst[1] = vec2_t(0.0f, 1.0f);
	return dst;
}
inline mat3x3_t identity3()
{
	mat3x3_t dst;
	dst[0] = vec3_t(1.0f, 0.0f, 0.0f);
	dst[1] = vec3_t(0.0f, 1.0f, 0.0f);
	dst[2] = vec3_t(0.0f, 0.0f, 1.0f);
	return dst;
}
inline mat4x4_t identity4()
{
	mat4x4_t dst;
	dst[0] = vec4_t(1.0f, 0.0f, 0.0f, 0.0f);
	dst[1] = vec4_t(0.0f, 1.0f, 0.0f, 0.0f);
	dst[2] = vec4_t(0.0f, 0.0f, 1.0f, 0.0f);
	dst[3] = vec4_t(0.0f, 0.0f, 0.0f, 1.0f);
	return dst;
}

// 2d rotation
inline mat2x2_t rotate(const float angle)
{
	mat2x2_t dst;
	float cosA = cosf(angle);
	float sinA = sinf(angle);
	dst[0] = vec2_t(cosA, sinA);
	dst[1] = vec2_t(-sinA, cosA);
	return dst;
}
// 3d rotations
inline mat3x3_t rotateX(const float angle)
{
	mat3x3_t dst;
	float cosA = cosf(angle);
	float sinA = sinf(angle);
	dst[0] = vec3_t(1.0f, 0.0f, 0.0f);
	dst[1] = vec3_t(0.0f, cosA, sinA);
	dst[2] = vec3_t(0.0f, -sinA, cosA);
	return dst;
}
inline mat3x3_t rotateY(const float angle)
{
	mat3x3_t dst;
	float cosA = cosf(angle);
	float sinA = sinf(angle);
	dst[0] = vec3_t(cosA, 0.0f, -sinA);
	dst[1] = vec3_t(0.0f, 1.0f, 0.0f);
	dst[2] = vec3_t(sinA, 0.0f, cosA);
	return dst;
}
inline mat3x3_t rotateZ(const float angle)
{
	mat3x3_t dst;
	float cosA = cosf(angle);
	float sinA = sinf(angle);
	dst[0] = vec3_t(cosA, sinA, 0.0f);
	dst[1] = vec3_t(-sinA, cosA, 0.0f);
	dst[2] = vec3_t(0.0f, 0.0f, 1.0f);
	return dst;
}
inline mat3x3_t rotateAxis(const float angle, const vec3_t axis)
{
	// From: 2.3.3 of Eberly, "3D Game Engine Design", 1st Edition
	// R = I + sin(angle) S + (1-cos(angle)) S^2
	//  where S = crossMatrix(axis)
	// This could probably be made more efficient, but we'll just
	// cross our fingers and hope the compiler does a good job
	// of it.
	const mat3x3_t S = crossMatrix(axis);
	mat3x3_t rotMat = identity3();
	rotMat = add(rotMat, scale(sinf(angle), S));
	rotMat = add(rotMat, scale(1-cosf(angle), mul(S,S)));
	return rotMat;

}

// 3d homogeneous rotations.
inline mat4x4_t rotateXh(const float angle)
{
	mat4x4_t dst;
	float cosA = cosf(angle);
	float sinA = sinf(angle);
	dst[0] = vec4_t(1.0f, 0.0f, 0.0f, 0.0f);
	dst[1] = vec4_t(0.0f, cosA, sinA, 0.0f);
	dst[2] = vec4_t(0.0f, -sinA, cosA, 0.0f);
	dst[3] = vec4_t(0.0f, 0.0f, 0.0f, 1.0f);
	return dst;
}
inline mat4x4_t rotateYh(const float angle)
{
	mat4x4_t dst;
	float cosA = cosf(angle);
	float sinA = sinf(angle);
	dst[0] = vec4_t(cosA, 0.0f, -sinA, 0.0f);
	dst[1] = vec4_t(0.0f, 1.0f, 0.0f, 0.0f);
	dst[2] = vec4_t(sinA, 0.0f, cosA, 0.0f);
	dst[3] = vec4_t(0.0f, 0.0f, 0.0f, 1.0f);
	return dst;
}
inline mat4x4_t rotateZh(const float angle)
{
	mat4x4_t dst;
	float cosA = cosf(angle);
	float sinA = sinf(angle);
	dst[0] = vec4_t(cosA, sinA, 0.0f, 0.0f);
	dst[1] = vec4_t(-sinA, cosA, 0.0f, 0.0f);
	dst[2] = vec4_t(0.0f, 0.0f, 1.0f, 0.0f);
	dst[3] = vec4_t(0.0f, 0.0f, 0.0f, 1.0f);
	return dst;
}

// Scaling matrix
  // 2d scale: x axis by sx; y axis by sy
inline mat2x2_t scale(const float sx, const float sy)
{
	mat2x2_t dst;
	dst[0] = vec2_t(sx, 0.0f);
	dst[1] = vec2_t(0.0f, sy);
	return dst;
}
inline mat2x2_t scale(const vec2_t s) { return scale(s.x, s.y); }
  // 3d scale: x axis by sx; y axis by sy; z axis by sz
inline mat3x3_t scale(const float sx, const float sy, const float sz)
{
	mat3x3_t dst;
	dst[0] = vec3_t(sx, 0.0f, 0.0f);
	dst[1] = vec3_t(0.0f, sy, 0.0f);
	dst[2] = vec3_t(0.0f, 0.0f, sz);
	return dst;
}
inline mat3x3_t scale(const vec3_t s) { return scale(s.x, s.y, s.z); }
  // 3d homogeneous coordinates
inline mat4x4_t scaleh(const float sx, const float sy, const float sz)
{
	mat4x4_t dst;
	dst[0] = vec4_t(sx, 0.0f, 0.0f, 0.0f);
	dst[1] = vec4_t(0.0f, sy, 0.0f, 0.0f);
	dst[2] = vec4_t(0.0f, 0.0f, sz, 0.0f);
	dst[3] = vec4_t(0.0f, 0.0f, 0.0f, 1.0f);
	return dst;
}
inline mat4x4_t scaleh(const vec3_t s) { return scaleh(s.x, s.y, s.z); }

// Shearing matrix
//  dx = displacement along x-axis by the shear axis
//  dy = displacement along y-axis by the shear axis
//  dz = displacement along z-axis by the shear axis
// 2d shear
inline mat2x2_t shear_x(const float dy)
{
	mat2x2_t dst;
	dst[0] = vec2_t(1.0f, dy);
	dst[1] = vec2_t(0.0f, 1.0f);
	return dst;
}
inline mat2x2_t shear_y(const float dx)
{
	mat2x2_t dst;
	dst[0] = vec2_t(1.0f, 0.0f);
	dst[1] = vec2_t(dx, 1.0f);
	return dst;
}
// 3d shear
inline mat3x3_t shear_x(const float dy, const float dz)
{
	mat3x3_t dst;
	dst[0] = vec3_t(1.0f, dy, dz);
	dst[1] = vec3_t(0.0f, 1.0f, 0.0f);
	dst[2] = vec3_t(0.0f, 0.0f, 1.0f);
	return dst;
}
inline mat3x3_t shear_y(const float dx, const float dz)
{
	mat3x3_t dst;
	dst[0] = vec3_t(1.0f, 0.0f, 0.0f);
	dst[1] = vec3_t(dx, 1.0f, dz);
	dst[2] = vec3_t(0.0f, 0.0f, 1.0f);
	return dst;
}
inline mat3x3_t shear_z(const float dx, const float dy)
{
	mat3x3_t dst;
	dst[0] = vec3_t(1.0f, 0.0f, 0.0f);
	dst[1] = vec3_t(0.0f, 1.0f, 0.0f);
	dst[2] = vec3_t(dx, dy, 1.0f);
	return dst;
}
// Homogeneous 3d shears
inline mat4x4_t shearh_x(const float dy, const float dz)
{
	mat4x4_t dst;
	dst[0] = vec4_t(1.0f, dy, dz, 0.0f);
	dst[1] = vec4_t(0.0f, 1.0f, 0.0f, 0.0f);
	dst[2] = vec4_t(0.0f, 0.0f, 1.0f, 0.0f);
	dst[3] = vec4_t(0.0f, 0.0f, 0.0f, 1.0f);
	return dst;
}
inline mat4x4_t shearh_y(const float dx, const float dz)
{
	mat4x4_t dst;
	dst[0] = vec4_t(1.0f, 0.0f, 0.0f, 0.0f);
	dst[1] = vec4_t(dx, 1.0f, dz, 0.0f);
	dst[2] = vec4_t(0.0f, 0.0f, 1.0f, 0.0f);
	dst[3] = vec4_t(0.0f, 0.0f, 0.0f, 1.0f);
	return dst;
}
inline mat4x4_t shearh_z(const float dx, const float dy)
{
	mat4x4_t dst;
	dst[0] = vec4_t(1.0f, 0.0f, 0.0f, 0.0f);
	dst[1] = vec4_t(0.0f, 1.0f, 0.0f, 0.0f);
	dst[2] = vec4_t(dx, dy, 1.0f, 0.0f);
	dst[3] = vec4_t(0.0f, 0.0f, 0.0f, 1.0f);
	return dst;
}

// Translations.
//   Translations are in homogeneous coordinates, by necessity.
inline mat3x3_t translate(const vec2_t t)
{
	mat3x3_t dst;
	dst[0] = vec3_t(1.0f, 0.0f, 0.0f);
	dst[1] = vec3_t(0.0f, 1.0f, 0.0f);
	dst[2] = vec3_t(t.x, t.y, 1.0f);
	return dst;
}
inline mat4x4_t translate(const vec3_t t)
{
	mat4x4_t dst;
	dst[0] = vec4_t(1.0f, 0.0f, 0.0f, 0.0f);
	dst[1] = vec4_t(0.0f, 1.0f, 0.0f, 0.0f);
	dst[2] = vec4_t(0.0f, 0.0f, 1.0f, 0.0f);
	dst[3] = vec4_t(t.x, t.y, t.z, 1.0f);
	return dst;
}


 // Scale by a scalar: set dst = sM
inline mat2x2_t scale(const float s, const mat2x2_t M)
{
	mat2x2_t dst;
	dst[0] = scale(s, M[0]);
	dst[1] = scale(s, M[1]);
	return dst;
}
inline mat2x3_t scale(const float s, const mat2x3_t M)
{
	mat2x3_t dst;
	dst[0] = scale(s, M[0]);
	dst[1] = scale(s, M[1]);
	dst[2] = scale(s, M[2]);
	return dst;
}
inline mat2x4_t scale(const float s, const mat2x4_t M)
{
	mat2x4_t dst;
	dst[0] = scale(s, M[0]);
	dst[1] = scale(s, M[1]);
	dst[2] = scale(s, M[2]);
	dst[3] = scale(s, M[3]);
	return dst;
}
inline mat3x2_t scale(const float s, const mat3x2_t M)
{
	mat3x2_t dst;
	dst[0] = scale(s, M[0]);
	dst[1] = scale(s, M[1]);
	return dst;
}
inline mat3x3_t scale(const float s, const mat3x3_t M)
{
	mat3x3_t dst;
	dst[0] = scale(s, M[0]);
	dst[1] = scale(s, M[1]);
	dst[2] = scale(s, M[2]);
	return dst;
}
inline mat3x4_t scale(const float s, const mat3x4_t M)
{
	mat3x4_t dst;
	dst[0] = scale(s, M[0]);
	dst[1] = scale(s, M[1]);
	dst[2] = scale(s, M[2]);
	dst[3] = scale(s, M[3]);
	return dst;
}
inline mat4x2_t scale(const float s, const mat4x2_t M)
{
	mat4x2_t dst;
	dst[0] = scale(s, M[0]);
	dst[1] = scale(s, M[1]);
	return dst;
}
inline mat4x3_t scale(const float s, const mat4x3_t M)
{
	mat4x3_t dst;
	dst[0] = scale(s, M[0]);
	dst[1] = scale(s, M[1]);
	dst[2] = scale(s, M[2]);
	return dst;
}
inline mat4x4_t scale(const float s, const mat4x4_t M)
{
	mat4x4_t dst;
	dst[0] = scale(s, M[0]);
	dst[1] = scale(s, M[1]);
	dst[2] = scale(s, M[2]);
	dst[3] = scale(s, M[3]);
	return dst;
}


 // Component-wise addition: A + B
inline mat2x2_t add(const mat2x2_t A, const mat2x2_t B)
{
	mat2x2_t dst;
	dst[0] = add(A[0], B[0]);
	dst[1] = add(A[1], B[1]);
	return dst;
}
inline mat2x3_t add(const mat2x3_t A, const mat2x3_t B)
{
	mat2x3_t dst;
	dst[0] = add(A[0], B[0]);
	dst[1] = add(A[1], B[1]);
	dst[2] = add(A[2], B[2]);
	return dst;
}
inline mat2x4_t add(const mat2x4_t A, const mat2x4_t B)
{
	mat2x4_t dst;
	dst[0] = add(A[0], B[0]);
	dst[1] = add(A[1], B[1]);
	dst[2] = add(A[2], B[2]);
	dst[3] = add(A[3], B[3]);
	return dst;
}
inline mat3x2_t add(const mat3x2_t A, const mat3x2_t B)
{
	mat3x2_t dst;
	dst[0] = add(A[0], B[0]);
	dst[1] = add(A[1], B[1]);
	return dst;
}
inline mat3x3_t add(const mat3x3_t A, const mat3x3_t B)
{
	mat3x3_t dst;
	dst[0] = add(A[0], B[0]);
	dst[1] = add(A[1], B[1]);
	dst[2] = add(A[2], B[2]);
	return dst;
}
inline mat3x4_t add(const mat3x4_t A, const mat3x4_t B)
{
	mat3x4_t dst;
	dst[0] = add(A[0], B[0]);
	dst[1] = add(A[1], B[1]);
	dst[2] = add(A[2], B[2]);
	dst[3] = add(A[3], B[3]);
	return dst;
}
inline mat4x2_t add(const mat4x2_t A, const mat4x2_t B)
{
	mat4x2_t dst;
	dst[0] = add(A[0], B[0]);
	dst[1] = add(A[1], B[1]);
	return dst;
}
inline mat4x3_t add(const mat4x3_t A, const mat4x3_t B)
{
	mat4x3_t dst;
	dst[0] = add(A[0], B[0]);
	dst[1] = add(A[1], B[1]);
	dst[2] = add(A[2], B[2]);
	return dst;
}
inline mat4x4_t add(const mat4x4_t A, const mat4x4_t B)
{
	mat4x4_t dst;
	dst[0] = add(A[0], B[0]);
	dst[1] = add(A[1], B[1]);
	dst[2] = add(A[2], B[2]);
	dst[3] = add(A[3], B[3]);
	return dst;
}

 // Component-wise subtraction: dst = A - B
inline mat2x2_t sub(const mat2x2_t A, const mat2x2_t B)
{
	mat2x2_t dst;
	dst[0] = sub(A[0], B[0]);
	dst[1] = sub(A[1], B[1]);
	return dst;
}
inline mat2x3_t sub(const mat2x3_t A, const mat2x3_t B)
{
	mat2x3_t dst;
	dst[0] = sub(A[0], B[0]);
	dst[1] = sub(A[1], B[1]);
	dst[2] = sub(A[2], B[2]);
	return dst;
}
inline mat2x4_t sub(const mat2x4_t A, const mat2x4_t B)
{
	mat2x4_t dst;
	dst[0] = sub(A[0], B[0]);
	dst[1] = sub(A[1], B[1]);
	dst[2] = sub(A[2], B[2]);
	dst[3] = sub(A[3], B[3]);
	return dst;
}
inline mat3x2_t sub(const mat3x2_t A, const mat3x2_t B)
{
	mat3x2_t dst;
	dst[0] = sub(A[0], B[0]);
	dst[1] = sub(A[1], B[1]);
	return dst;
}
inline mat3x3_t sub(const mat3x3_t A, const mat3x3_t B)
{
	mat3x3_t dst;
	dst[0] = sub(A[0], B[0]);
	dst[1] = sub(A[1], B[1]);
	dst[2] = sub(A[2], B[2]);
	return dst;
}
inline mat3x4_t sub(const mat3x4_t A, const mat3x4_t B)
{
	mat3x4_t dst;
	dst[0] = sub(A[0], B[0]);
	dst[1] = sub(A[1], B[1]);
	dst[2] = sub(A[2], B[2]);
	dst[3] = sub(A[3], B[3]);
	return dst;
}
inline mat4x2_t sub(const mat4x2_t A, const mat4x2_t B)
{
	mat4x2_t dst;
	dst[0] = sub(A[0], B[0]);
	dst[1] = sub(A[1], B[1]);
	return dst;
}
inline mat4x3_t sub(const mat4x3_t A, const mat4x3_t B)
{
	mat4x3_t dst;
	dst[0] = sub(A[0], B[0]);
	dst[1] = sub(A[1], B[1]);
	dst[2] = sub(A[2], B[2]);
	return dst;
}
inline mat4x4_t sub(const mat4x4_t A, const mat4x4_t B)
{
	mat4x4_t dst;
	dst[0] = sub(A[0], B[0]);
	dst[1] = sub(A[1], B[1]);
	dst[2] = sub(A[2], B[2]);
	dst[3] = sub(A[3], B[3]);
	return dst;
}



// Matrix transpose: M^T
inline mat2x2_t transpose(const mat2x2_t M)
{
	mat2x2_t dst;
	dst[0] = vec2_t(M[0].x, M[1].x);
	dst[1] = vec2_t(M[0].y, M[1].y);
	return dst;
}
inline mat2x3_t transpose(const mat3x2_t M)
{
	mat2x3_t dst;
	dst[0] = vec2_t(M[0].x, M[1].x);
	dst[1] = vec2_t(M[0].y, M[1].y);
	dst[2] = vec2_t(M[0].z, M[1].z);
	return dst;
}
inline mat2x4_t transpose(const mat4x2_t M)
{
	mat2x4_t dst;
	dst[0] = vec2_t(M[0].x, M[1].x);
	dst[1] = vec2_t(M[0].y, M[1].y);
	dst[2] = vec2_t(M[0].z, M[1].z);
	dst[3] = vec2_t(M[0].w, M[1].w);
	return dst;
}
inline mat3x2_t transpose(const mat2x3_t M)
{
	mat3x2_t dst;
	dst[0] = vec3_t(M[0].x, M[1].x, M[2].x);
	dst[1] = vec3_t(M[0].y, M[1].y, M[2].y);
	return dst;
}
inline mat3x3_t transpose(const mat3x3_t M)
{
	mat3x3_t dst;
	dst[0] = vec3_t(M[0].x, M[1].x, M[2].x);
	dst[1] = vec3_t(M[0].y, M[1].y, M[2].y);
	dst[2] = vec3_t(M[0].z, M[1].z, M[2].z);
	return dst;
}
inline mat3x4_t transpose(const mat4x3_t M)
{
	mat3x4_t dst;
	dst[0] = vec3_t(M[0].x, M[1].x, M[2].x);
	dst[1] = vec3_t(M[0].y, M[1].y, M[2].y);
	dst[2] = vec3_t(M[0].z, M[1].z, M[2].z);
	dst[3] = vec3_t(M[0].w, M[1].w, M[2].w);
	return dst;
}
inline mat4x2_t transpose(const mat2x4_t M)
{
	mat4x2_t dst;
	dst[0] = vec4_t(M[0].x, M[1].x, M[2].x, M[3].x);
	dst[1] = vec4_t(M[0].y, M[1].y, M[2].y, M[3].y);
	return dst;
}
inline mat4x3_t transpose(const mat3x4_t M)
{
	mat4x3_t dst;
	dst[0] = vec4_t(M[0].x, M[1].x, M[2].x, M[3].x);
	dst[1] = vec4_t(M[0].y, M[1].y, M[2].y, M[3].y);
	dst[2] = vec4_t(M[0].z, M[1].z, M[2].z, M[3].z);
	return dst;
}
inline mat4x4_t transpose(const mat4x4_t M)
{
	mat4x4_t dst;
	dst[0] = vec4_t(M[0].x, M[1].x, M[2].x, M[3].x);
	dst[1] = vec4_t(M[0].y, M[1].y, M[2].y, M[3].y);
	dst[2] = vec4_t(M[0].z, M[1].z, M[2].z, M[3].z);
	dst[3] = vec4_t(M[0].w, M[1].w, M[2].w, M[3].w);
	return dst;
}




 // Determinant
inline float det(const mat2x2_t A)
{
	return A[0].x * A[1].y - A[0].y * A[1].x;
}
inline float det(const mat3x3_t A)
{
	return dot(A[0], cross(A[1], A[2]));
}
inline float det(const mat4x4_t A)
{
	// From: http://www.geometrictools.com/Documentation/LaplaceExpansionTheorem.pdf
	float s0, s1, s2, s3, s4, s5;
	float c5, c4, c3, c2, c1, c0;
	s0 = A[0][0]*A[1][1] - A[0][1]*A[1][0];
	s1 = A[0][0]*A[1][2] - A[0][2]*A[1][0];
	s2 = A[0][0]*A[1][3] - A[0][3]*A[1][0];
	s3 = A[0][1]*A[1][2] - A[0][2]*A[1][1];
	s4 = A[0][1]*A[1][3] - A[0][3]*A[1][1];
	s5 = A[0][2]*A[1][3] - A[0][3]*A[1][2];
	c5 = A[2][2]*A[3][3] - A[2][3]*A[3][2];
	c4 = A[2][1]*A[3][3] - A[2][3]*A[3][1];
	c3 = A[2][1]*A[3][2] - A[2][2]*A[3][1];
	c2 = A[2][0]*A[3][3] - A[2][3]*A[3][0];
	c1 = A[2][0]*A[3][2] - A[2][2]*A[3][0];
	c0 = A[2][0]*A[3][1] - A[2][1]*A[3][0];
	return s0*c5 - s1*c4 + s2*c3 + s3*c2 - s4*c1 + s5*c0;
}
 // Matrix inverse.
 //  -- Assumes the matrix is invertible.
 //  -- If you're not sure, ensure that det(A) is non-zero first.
inline mat2x2_t inverse(const mat2x2_t A)
{
	float detAinv = 1.0f/det(A);
	// Set dst to adjoint(A) = cofactors(A)^T
	mat2x2_t dst;
	dst[0] = scale(detAinv, vec2_t(A[1].y, -A[0].x));
	dst[1] = scale(detAinv, vec2_t(-A[0].x, A[0].y));
	return dst;
}
inline mat3x3_t inverse(const mat3x3_t A)
{
 	mat3x3_t C; // cofactors(A)
	C[0] = cross(A[1], A[2]);
	C[1] = cross(A[2], A[0]);
	C[2] = cross(A[0], A[1]);
	float detAinv = 1.0f / dot(A[0], C[0]);
	// inverse = [ 1/|A| ] C^T
	mat3x3_t dst;
	dst[0] = scale(detAinv, vec3_t(C[0].x, C[1].x, C[2].x));
	dst[1] = scale(detAinv, vec3_t(C[0].y, C[1].y, C[2].y));
	dst[2] = scale(detAinv, vec3_t(C[0].z, C[1].z, C[2].z));
	return dst;
}

inline mat4x4_t inverse(const mat4x4_t A)
{
	// From: http://www.geometrictools.com/Documentation/LaplaceExpansionTheorem.pdf
	float s0, s1, s2, s3, s4, s5;
	float c5, c4, c3, c2, c1, c0;
	s0 = A[0][0]*A[1][1] - A[0][1]*A[1][0];
	s1 = A[0][0]*A[1][2] - A[0][2]*A[1][0];
	s2 = A[0][0]*A[1][3] - A[0][3]*A[1][0];
	s3 = A[0][1]*A[1][2] - A[0][2]*A[1][1];
	s4 = A[0][1]*A[1][3] - A[0][3]*A[1][1];
	s5 = A[0][2]*A[1][3] - A[0][3]*A[1][2];
	c5 = A[2][2]*A[3][3] - A[2][3]*A[3][2];
	c4 = A[2][1]*A[3][3] - A[2][3]*A[3][1];
	c3 = A[2][1]*A[3][2] - A[2][2]*A[3][1];
	c2 = A[2][0]*A[3][3] - A[2][3]*A[3][0];
	c1 = A[2][0]*A[3][2] - A[2][2]*A[3][0];
	c0 = A[2][0]*A[3][1] - A[2][1]*A[3][0];
	float detA = s0*c5 - s1*c4 + s2*c3 + s3*c2 - s4*c1 + s5*c0;
	mat4x4_t inv;
	// Set inv = adjoint(A)
	inv[0] = vec4_t(
			A[1][1]*c5-A[1][2]*c4+A[1][3]*c3,
			-(A[0][1]*c5-A[0][2]*c4+A[0][3]*c3),
			A[3][1]*s5-A[3][2]*s4+A[3][3]*s3,
			-(A[2][1]*s5-A[2][2]*s4-A[2][3]*s3)
			);
	inv[1] = vec4_t(
			-(A[1][0]*c5-A[1][2]*c2+A[1][3]*c1),
			A[0][0]*c5-A[0][2]*c2+A[0][3]*c1,
			-(A[3][0]*s5-A[3][2]*s2+A[3][3]*s1),
			A[2][0]*s5-A[2][2]*s2+A[2][3]*s1
			);
	inv[2] = vec4_t(
			A[1][0]*c4-A[1][1]*c2+A[1][3]*c0,
			-(A[0][0]*c4-A[0][1]*c2+A[0][3]*c0),
			A[3][0]*s4-A[3][1]*s2+A[3][3]*s0,
			-(A[2][0]*s4-A[2][1]*s2+A[2][3]*s0)
			);
	inv[3] = vec4_t(
			-(A[1][0]*c3-A[1][1]*c1+A[1][2]*c0),
			A[0][0]*c3-A[0][1]*c1+A[0][2]*c0,
			-(A[3][0]*s3-A[3][1]*s1+A[3][2]*s0),
			A[2][0]*s3-A[2][1]*s1+A[2][2]*s0
			);
	return scale(1.0/detA, inv);
}

 // Matrix embedding
 // Set: dst = [ M 0 ]
 //            [ 0 1 ]
inline mat3x3_t embed(const mat2x2_t M)
{
	mat3x3_t A;
	A[0] = vec3_t(M[0].x, M[0].y, 0.0f);
	A[1] = vec3_t(M[1].x, M[1].y, 0.0f);
	A[2] = vec3_t(0.0f, 0.0f, 1.0f);
	return A;
}
inline mat4x4_t embed(const mat3x3_t M)
{
	mat4x4_t A;
	A[0] = vec4_t(M[0].x, M[0].y, M[0].z, 0.0f);
	A[1] = vec4_t(M[1].x, M[1].y, M[1].z, 0.0f);
	A[2] = vec4_t(M[2].x, M[2].y, M[2].z, 0.0f);
	A[3] = vec4_t(0.0f, 0.0f, 0.0f, 1.0f);
	return A;
}

 // Matrix extraction
 // extract M from A = [ M a ]
 //                    [ b c ]
inline mat2x2_t extract(const mat3x3_t A)
{
	mat2x2_t M;
	M[0] = vec2_t(A[0].x, A[0].y);
	M[1] = vec2_t(A[1].x, A[1].y);
	return M;
}
inline mat3x3_t extract(const mat4x4_t A)
{
	mat3x3_t M;
	M[0] = vec3_t(A[0].x, A[0].y, A[0].z);
	M[1] = vec3_t(A[1].x, A[1].y, A[1].z);
	M[2] = vec3_t(A[2].x, A[2].y, A[2].z);
	return M;
}




inline mat2x2_t mul(const mat2x2_t X, const mat2x2_t Y)
{
	mat2x2_t dst;
	dst[0] = mul(X,Y[0]);
	dst[1] = mul(X,Y[1]);
	return dst;
}
inline mat2x2_t mul(const mat2x3_t X, const mat3x2_t Y)
{
	mat2x2_t dst;
	dst[0] = mul(X,Y[0]);
	dst[1] = mul(X,Y[1]);
	return dst;
}
inline mat2x2_t mul(const mat2x4_t X, const mat4x2_t Y)
{
	mat2x2_t dst;
	dst[0] = mul(X,Y[0]);
	dst[1] = mul(X,Y[1]);
	return dst;
}
inline mat2x3_t mul(const mat2x2_t X, const mat2x3_t Y)
{
	mat2x3_t dst;
	dst[0] = mul(X,Y[0]);
	dst[1] = mul(X,Y[1]);
	dst[2] = mul(X,Y[2]);
	return dst;
}
inline mat2x3_t mul(const mat2x3_t X, const mat3x3_t Y)
{
	mat2x3_t dst;
	dst[0] = mul(X,Y[0]);
	dst[1] = mul(X,Y[1]);
	dst[2] = mul(X,Y[2]);
	return dst;
}
inline mat2x3_t mul(const mat2x4_t X, const mat4x3_t Y)
{
	mat2x3_t dst;
	dst[0] = mul(X,Y[0]);
	dst[1] = mul(X,Y[1]);
	dst[2] = mul(X,Y[2]);
	return dst;
}
inline mat2x4_t mul(const mat2x2_t X, const mat2x4_t Y)
{
	mat2x4_t dst;
	dst[0] = mul(X,Y[0]);
	dst[1] = mul(X,Y[1]);
	dst[2] = mul(X,Y[2]);
	dst[3] = mul(X,Y[3]);
	return dst;
}
inline mat2x4_t mul(const mat2x3_t X, const mat3x4_t Y)
{
	mat2x4_t dst;
	dst[0] = mul(X,Y[0]);
	dst[1] = mul(X,Y[1]);
	dst[2] = mul(X,Y[2]);
	dst[3] = mul(X,Y[3]);
	return dst;
}
inline mat2x4_t mul(const mat2x4_t X, const mat4x4_t Y)
{
	mat2x4_t dst;
	dst[0] = mul(X,Y[0]);
	dst[1] = mul(X,Y[1]);
	dst[2] = mul(X,Y[2]);
	dst[3] = mul(X,Y[3]);
	return dst;
}



inline mat3x2_t mul(const mat3x2_t X, const mat2x2_t Y)
{
	mat3x2_t dst;
	dst[0] = mul(X,Y[0]);
	dst[1] = mul(X,Y[1]);
	return dst;
}
inline mat3x2_t mul(const mat3x3_t X, const mat3x2_t Y)
{
	mat3x2_t dst;
	dst[0] = mul(X,Y[0]);
	dst[1] = mul(X,Y[1]);
	return dst;
}
inline mat3x2_t mul(const mat3x4_t X, const mat4x2_t Y)
{
	mat3x2_t dst;
	dst[0] = mul(X,Y[0]);
	dst[1] = mul(X,Y[1]);
	return dst;
}
inline mat3x3_t mul(const mat3x2_t X, const mat2x3_t Y)
{
	mat3x3_t dst;
	dst[0] = mul(X,Y[0]);
	dst[1] = mul(X,Y[1]);
	dst[2] = mul(X,Y[2]);
	return dst;
}
inline mat3x3_t mul(const mat3x3_t X, const mat3x3_t Y)
{
	mat3x3_t dst;
	dst[0] = mul(X,Y[0]);
	dst[1] = mul(X,Y[1]);
	dst[2] = mul(X,Y[2]);
	return dst;
}
inline mat3x3_t mul(const mat3x4_t X, const mat4x3_t Y)
{
	mat3x3_t dst;
	dst[0] = mul(X,Y[0]);
	dst[1] = mul(X,Y[1]);
	dst[2] = mul(X,Y[2]);
	return dst;
}
inline mat3x4_t mul(const mat3x2_t X, const mat2x4_t Y)
{
	mat3x4_t dst;
	dst[0] = mul(X,Y[0]);
	dst[1] = mul(X,Y[1]);
	dst[2] = mul(X,Y[2]);
	dst[3] = mul(X,Y[3]);
	return dst;
}
inline mat3x4_t mul(const mat3x3_t X, const mat3x4_t Y)
{
	mat3x4_t dst;
	dst[0] = mul(X,Y[0]);
	dst[1] = mul(X,Y[1]);
	dst[2] = mul(X,Y[2]);
	dst[3] = mul(X,Y[3]);
	return dst;
}
inline mat3x4_t mul(const mat3x4_t X, const mat4x4_t Y)
{
	mat3x4_t dst;
	dst[0] = mul(X,Y[0]);
	dst[1] = mul(X,Y[1]);
	dst[2] = mul(X,Y[2]);
	dst[3] = mul(X,Y[3]);
	return dst;
}



inline mat4x2_t mul(const mat4x2_t X, const mat2x2_t Y)
{
	mat4x2_t dst;
	dst[0] = mul(X,Y[0]);
	dst[1] = mul(X,Y[1]);
	return dst;
}
inline mat4x2_t mul(const mat4x3_t X, const mat3x2_t Y)
{
	mat4x2_t dst;
	dst[0] = mul(X,Y[0]);
	dst[1] = mul(X,Y[1]);
	return dst;
}
inline mat4x2_t mul(const mat4x4_t X, const mat4x2_t Y)
{
	mat4x2_t dst;
	dst[0] = mul(X,Y[0]);
	dst[1] = mul(X,Y[1]);
	return dst;
}
inline mat4x3_t mul(const mat4x2_t X, const mat2x3_t Y)
{
	mat4x3_t dst;
	dst[0] = mul(X,Y[0]);
	dst[1] = mul(X,Y[1]);
	dst[2] = mul(X,Y[2]);
	return dst;
}
inline mat4x3_t mul(const mat4x3_t X, const mat3x3_t Y)
{
	mat4x3_t dst;
	dst[0] = mul(X,Y[0]);
	dst[1] = mul(X,Y[1]);
	dst[2] = mul(X,Y[2]);
	return dst;
}
inline mat4x3_t mul(const mat4x4_t X, const mat4x3_t Y)
{
	mat4x3_t dst;
	dst[0] = mul(X,Y[0]);
	dst[1] = mul(X,Y[1]);
	dst[2] = mul(X,Y[2]);
	return dst;
}
inline mat4x4_t mul(const mat4x2_t X, const mat2x4_t Y)
{
	mat4x4_t dst;
	dst[0] = mul(X,Y[0]);
	dst[1] = mul(X,Y[1]);
	dst[2] = mul(X,Y[2]);
	dst[3] = mul(X,Y[3]);
	return dst;
}
inline mat4x4_t mul(const mat4x3_t X, const mat3x4_t Y)
{
	mat4x4_t dst;
	dst[0] = mul(X,Y[0]);
	dst[1] = mul(X,Y[1]);
	dst[2] = mul(X,Y[2]);
	dst[3] = mul(X,Y[3]);
	return dst;
}
inline mat4x4_t mul(const mat4x4_t X, const mat4x4_t Y)
{
	mat4x4_t dst;
	dst[0] = mul(X,Y[0]);
	dst[1] = mul(X,Y[1]);
	dst[2] = mul(X,Y[2]);
	dst[3] = mul(X,Y[3]);
	return dst;
}
