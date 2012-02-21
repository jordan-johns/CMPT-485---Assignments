
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
// Inline function definitions for matrix-vector functions.
// This file should not be directly included by anything other than gml.h

inline mat2x2_t outer(const vec2_t a, const vec3_t b)
{
	mat2x2_t toRet;
	toRet[0] = scale(b[0], a);
	toRet[1] = scale(b[1], a);
	return toRet;
}
inline mat3x3_t outer(const vec3_t a, const vec3_t b)
{
	mat3x3_t toRet;
	toRet[0] = scale(b[0], a);
	toRet[1] = scale(b[1], a);
	toRet[2] = scale(b[2], a);
	return toRet;
}
inline mat4x4_t outer(const vec4_t a, const vec4_t b)
{
	mat4x4_t toRet;
	toRet[0] = scale(b[0], a);
	toRet[1] = scale(b[1], a);
	toRet[2] = scale(b[2], a);
	toRet[3] = scale(b[3], a);
	return toRet;
}



inline vec2_t mul(const mat2x2_t A, const vec2_t x)
{
	vec2_t b;
	b = add( scale(x.x, A[0]), scale(x.y,A[1]) );
	return b;
}
inline vec2_t mul(const mat2x3_t A, const vec3_t x)
{
	vec2_t b;
	b = add( scale(x.x, A[0]), add(scale(x.y,A[1]),scale(x.z,A[2])) );
	return b;
}
inline vec2_t mul(const mat2x4_t A, const vec4_t x)
{
	vec2_t b;
	b = add( scale(x.x, A[0]), add(scale(x.y,A[1]), add(scale(x.z,A[2]), scale(x.w, A[3]))) );
	return b;
}


inline vec3_t mul(const mat3x2_t A, const vec2_t x)
{
	vec3_t b;
	b = add( scale(x.x, A[0]), scale(x.y,A[1]) );
	return b;
}
inline vec3_t mul(const mat3x3_t A, const vec3_t x)
{
	vec3_t b;
	b = add( scale(x.x, A[0]), add(scale(x.y,A[1]),scale(x.z,A[2])) );
	return b;
}
inline vec3_t mul(const mat3x4_t A, const vec4_t x)
{
	vec3_t b;
	b = add( scale(x.x, A[0]), add(scale(x.y,A[1]), add(scale(x.z,A[2]), scale(x.w, A[3]))) );
	return b;
}

inline vec4_t mul(const mat4x2_t A, const vec2_t x)
{
	vec4_t b;
	b = add( scale(x.x, A[0]), scale(x.y,A[1]) );
	return b;
}
inline vec4_t mul(const mat4x3_t A, const vec3_t x)
{
	vec4_t b;
	b = add( scale(x.x, A[0]), add(scale(x.y,A[1]),scale(x.z,A[2])) );
	return b;
}
inline vec4_t mul(const mat4x4_t A, const vec4_t x)
{
	vec4_t b;
	b = add( scale(x.x, A[0]), add(scale(x.y,A[1]), add(scale(x.z,A[2]), scale(x.w, A[3]))) );
	return b;
}
