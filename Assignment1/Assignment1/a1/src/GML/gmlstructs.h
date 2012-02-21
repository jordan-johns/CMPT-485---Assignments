
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

// Vector & matrix definitions for gml.
// You do not want to directly include this file.

// Vector types.
//   Components: xyzw (position/direction), rgba (color), and/or stpq (texture)
struct _vec2_t;
struct _vec3_t;
struct _vec4_t;

struct _vec2_t
{
	union { float x, s, r; }; // First coordinate
	union { float y, t, g; }; // Second coordinate

	_vec2_t() { x=y=0.0f; }
	_vec2_t(float _x, float _y) { x = _x; y = _y; }
	_vec2_t(const _vec2_t &_v) { x = _v.x; y = _v.y; }

	bool operator==(const _vec2_t b) const { return x==b.x && y==b.y; }
	float operator[](const int i) const { return *((&x)+i); }
	float& operator[](const int i) { return *((&x)+i); }
};
struct _vec3_t
{
	union { float x, s, r; }; // First coordinate
	union { float y, t, g; }; // Second coordinate
	union { float z, p, b; }; // Third coordinate

	_vec3_t() { x=y=z=0.0f; }
	_vec3_t(float _x, float _y, float _z) { x = _x; y = _y; z = _z; }
	_vec3_t(const _vec2_t _v, float _z) { x = _v.x; y = _v.y; z = _z; }
	_vec3_t(const _vec3_t &_v) { x = _v.x; y = _v.y; z = _v.z; }

	bool operator==(const _vec3_t b) const { return x==b.x && y==b.y && z==b.z; }
	float operator[](const int i) const { return *((&x)+i); }
	float& operator[](const int i) { return *((&x)+i); }
};
struct _vec4_t
{
	union { float x, s, r; }; // First coordinate
	union { float y, t, g; }; // Second coordinate
	union { float z, p, b; }; // Third coordinate
	union { float w, q, a; }; // Fourth coordinate
	_vec4_t() { x=y=z=w=0.0f; }
	_vec4_t(float _x, float _y, float _z, float _w) { x = _x; y = _y; z = _z; w = _w; }
	_vec4_t(const _vec3_t _v, float _w) { x = _v.x; y = _v.y; z = _v.z; w = _w; }
	_vec4_t(const _vec4_t &_v) { x = _v.x; y = _v.y; z = _v.z; w = _v.w; }
	bool operator==(const _vec4_t b) const { return x==b.x && y==b.y && z==b.z && w==b.w; }
	float operator[](const int i) const { return *((&x)+i); }
	float& operator[](const int i) { return *((&x)+i); }
};

// Column-major order matrices
//  Note: Array of columns. So, each vector is a single column.

// Create the macro to define an NxM matrix
#define DEF_MAT(N,M) \
	struct _mat ## N ## x ## M ## _t \
	{\
		_vec ## N ## _t m[M];\
		_mat ## N ## x ## M ## _t() {};\
		_vec ## N ## _t const & operator[](const int i) const { return m[i]; } \
		_vec ## N ## _t& operator[](const int i) { return m[i]; } \
		bool operator==(const _mat ## N ## x ## M ## _t b) const \
		{\
			bool result=true;\
			for (int i=0; result && i < M; i++) result = result && m[i]==b.m[i];\
			return result;\
		}\
	};

// Use the macro to define matrix structs
DEF_MAT(2,2)
DEF_MAT(2,3)
DEF_MAT(2,4)
DEF_MAT(3,2)
DEF_MAT(3,3)
DEF_MAT(3,4)
DEF_MAT(4,2)
DEF_MAT(4,3)
DEF_MAT(4,4)

#undef DEF_MAT

