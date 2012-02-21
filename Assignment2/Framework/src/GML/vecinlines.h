
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

// Inline function definitions for vector functions.
// This file should not be directly included by anything other than gml.h

// Vector scaling
inline TYPE scale(const float s, const TYPE v)
{
	TYPE result;
	float *dst = (float*)&result;
	const float * const _v = (const float*const)&v;
	for (int i=0; i < N_COMPONENTS; i++)
	{
		dst[i] = s * _v[i];
	}
	return result;
}


// Component-wise addition
inline TYPE add(const TYPE a, const TYPE b)
{
	TYPE result;
	float *dst = (float*)&result;
	const float * const _a = (const float*const)&a;
	const float * const _b = (const float*const)&b;
	for (int i=0; i < N_COMPONENTS; i++)
	{
		dst[i] = _a[i] + _b[i];
	}
	return result;
}


// Component-wise subtraction
inline TYPE sub(const TYPE a, const TYPE b)
{
	TYPE result;
	float *dst = (float*)&result;
	const float * const _a = (const float*const)&a;
	const float * const _b = (const float*const)&b;
	for (int i=0; i < N_COMPONENTS; i++)
	{
		dst[i] = _a[i] - _b[i];
	}
	return result;
}

// Dot product functions
inline float dot(const TYPE a, const TYPE b)
{
	const float * const _a = (const float*const)&a;
	const float * const _b = (const float*const)&b;
	float result = _a[0] * _b[0];
	for (int i=1; i < N_COMPONENTS; i++)
	{
		result += _a[i] * _b[i];
	}
	return result;
}

// Vector length
inline float length(const TYPE v)
{
	return sqrtf(dot(v,v));
}

// Vector length squared
inline float length2(const TYPE v)
{
	return dot(v,v);
}

inline TYPE normalize(const TYPE v)
{
	return scale(1.0f/length(v), v);
}
