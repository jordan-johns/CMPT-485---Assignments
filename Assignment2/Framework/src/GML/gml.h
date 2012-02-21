
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
 * "Graphics Math Library" (catchy, I know :-P)
 *
 * Types include:
 *   2 to 4 element vectors of floats
 *   MxN matrices of floats where M,M\in{2,3,4}
 *
 *  Matrices are stored in column-major order.
 *   - This is different from 2D arrays in C/Java; 2D arrays in
 *  C are stored in row-major order.
 *   - Column-major order is done to be OpenGL compatible
 *    and to provide opportunities to the compiler to use
 *    SIMD operations when optimizing.
 *   - Note that because the matrices are in column-major
 *    order, the first index is the _COLUMN_ not the row.
 *
 *  ex:
 *   mat3x3_t A;
 *     A[1][0] is the element in the 2nd column, and first row.
 *
 *  Everything in this library is implemented as an inline function
 *  to allow for maximum speed.
 *
 *  All function parameters are pass-by-copy to give the compiler flexibility
 *  in allocating space for the parameters.
 *    - When passing by reference, or by pointer, then the compiler must
 *      put the data in memory, but with pass-by-copy it can choose to
 *      leave the data in a (SIMD) register.
 *
 *  Every single function returns its calculated value via the function return
 *  mechanism instead of through reference parameters. The motivation for
 *  doing so is the same as for pass-by-value parameters.
 *
 *  It looks like there is a lot here, but it is a lot of duplication.
 *  Familiarize yourself with what's available here. It will make your
 *  life easier.
 */


#pragma once
#ifndef __INC_GML_H__
#define __INC_GML_H__

#include <cmath>

namespace gml
{

#include "gmlstructs.h"

// Vector types
//   Components: xyzw (position/direction), rgba (color), and/or stpq (texture)
typedef struct _vec2_t vec2_t;
typedef struct _vec3_t vec3_t;
typedef struct _vec4_t vec4_t;

// Column-major order matrix types. NxM = N rows, M columns
// Accessible as an array of M N-dimensional vectors
typedef struct _mat2x2_t mat2x2_t;
typedef struct _mat2x3_t mat2x3_t;
typedef struct _mat2x4_t mat2x4_t;

typedef struct _mat3x2_t mat3x2_t;
typedef struct _mat3x3_t mat3x3_t;
typedef struct _mat3x4_t mat3x4_t;

typedef struct _mat4x2_t mat4x2_t;
typedef struct _mat4x3_t mat4x3_t;
typedef struct _mat4x4_t mat4x4_t;

/*
 * Vector functions
 */
// Vector scaling: s v
vec2_t scale(const float s, const vec2_t v);
vec3_t scale(const float s, const vec3_t v);
vec4_t scale(const float s, const vec4_t v);

// Component-wise addition: a + b
vec2_t add(const vec2_t a, const vec2_t b);
vec3_t add(const vec3_t a, const vec3_t b);
vec4_t add(const vec4_t a, const vec4_t b);

// Component-wise subtraction: a - b
vec2_t sub(const vec2_t a, const vec2_t b);
vec3_t sub(const vec3_t a, const vec3_t b);
vec4_t sub(const vec4_t a, const vec4_t b);

// Dot product functions: a <dot> b
float dot(const vec2_t a, const vec2_t b);
float dot(const vec3_t a, const vec3_t b);
float dot(const vec4_t a, const vec4_t b);

// Vector length
float length(const vec2_t v);
float length(const vec3_t v);
float length(const vec4_t v);

// Vector length squared
float length2(const vec2_t v);
float length2(const vec3_t v);
float length2(const vec4_t v);

vec2_t normalize(const vec2_t v);
vec3_t normalize(const vec3_t v);
vec4_t normalize(const vec4_t v);

// Cross product: a x b
vec3_t cross(const vec3_t a, const vec3_t b);
// Gives the matrix A such that cross(a, v) = Av
mat3x3_t crossMatrix(const vec3_t a);

// Outer products. Also known as the tensor product
mat2x2_t outer(const vec2_t a, const vec3_t b);
mat3x3_t outer(const vec3_t a, const vec3_t b);
mat4x4_t outer(const vec4_t a, const vec4_t b);

// extractN(v) -- extract the first N components of v
vec2_t extract2(const vec3_t v);
vec2_t extract2(const vec4_t v);
vec3_t extract3(const vec4_t v);

/*
 * Matrix-vector multiplication
 */

// Matrix-vector multiplication: Ax
// All possible combinations
vec2_t mul(const mat2x2_t A, const vec2_t x);
vec2_t mul(const mat2x3_t A, const vec3_t x);
vec2_t mul(const mat2x4_t A, const vec4_t x);

vec3_t mul(const mat3x2_t A, const vec2_t x);
vec3_t mul(const mat3x3_t A, const vec3_t x);
vec3_t mul(const mat3x4_t A, const vec4_t x);

vec4_t mul(const mat4x2_t A, const vec2_t x);
vec4_t mul(const mat4x3_t A, const vec3_t x);
vec4_t mul(const mat4x4_t A, const vec4_t x);


/*
 * Matrix creation.
 *  Each of these functions completely overwrites the values in dst.
 */
 // Identity matrix
mat2x2_t identity2();
mat3x3_t identity3();
mat4x4_t identity4();

 // Right-handed coordinate system: Counter-clockwise rotation matrix.
 // angle given in __radians__
 // 2d rotations
mat2x2_t rotate(const float angle); // Around x-axis
 // 3d rotations
mat3x3_t rotateX(const float angle); // Around x-axis
mat3x3_t rotateY(const float angle); // Around y-axis
mat3x3_t rotateZ(const float angle); // Around z-axis
mat3x3_t rotateAxis(const float angle, const vec3_t axis);
 // 3d homogeneous rotations.
mat4x4_t rotateXh(const float angle); // Homogeneous coords. Around x-axis
mat4x4_t rotateYh(const float angle); // Homogeneous coords. Around y-axis
mat4x4_t rotateZh(const float angle); // Homogeneous coords. Around z-axis

 // Scaling matrix
  // 2d scale: x axis by sx; y axis by sy
mat2x2_t scale(const float sx, const float sy);
mat2x2_t scale(const vec2_t s);
  // 3d scale: x axis by sx; y axis by sy; z axis by sz
mat3x3_t scale(const float sx, const float sy, const float sz);
mat3x3_t scale(const vec3_t s);
  // 3d homogeneous coordinates
mat4x4_t scaleh(const float sx, const float sy, const float sz);
mat4x4_t scaleh(const vec3_t s);

 // Shearing matrix
 //  dx = displacement along x-axis by the shear axis
 //  dy = displacement along y-axis by the shear axis
 //  dz = displacement along z-axis by the shear axis
 // 2d shear
mat2x2_t shear_x(const float dy); // Shear along x-axis
mat2x2_t shear_y(const float dx); // Shear along y-axis
 // 3d shear
mat3x3_t shear_x(const float dy, const float dz); // Along x-axis
mat3x3_t shear_y(const float dx, const float dz); // Along y-axis
mat3x3_t shear_z(const float dx, const float dy); // Along z-axis
 // Homogeneous 3d shears
mat4x4_t shearh_x(const float dy, const float dz); // Along x-axis
mat4x4_t shearh_y(const float dx, const float dz); // Along y-axis
mat4x4_t shearh_z(const float dx, const float dy); // Along z-axis

 // Translations.
 //   Translations are in homogeneous coordinates, by necessity.
mat3x3_t translate(const vec2_t t);
mat4x4_t translate(const vec3_t t);


/*
 * Matrix operations
 */

 // Scale by a scalar: sM
mat2x2_t scale(const float s, const mat2x2_t M);
mat2x3_t scale(const float s, const mat2x3_t M);
mat2x4_t scale(const float s, const mat2x4_t M);
mat3x2_t scale(const float s, const mat3x2_t M);
mat3x3_t scale(const float s, const mat3x3_t M);
mat3x4_t scale(const float s, const mat3x4_t M);
mat4x2_t scale(const float s, const mat4x2_t M);
mat4x3_t scale(const float s, const mat4x3_t M);
mat4x4_t scale(const float s, const mat4x4_t M);

 // Component-wise addition: A + B
mat2x2_t add(const mat2x2_t A, const mat2x2_t B);
mat2x3_t add(const mat2x3_t A, const mat2x3_t B);
mat2x4_t add(const mat2x4_t A, const mat2x4_t B);
mat3x2_t add(const mat3x2_t A, const mat3x2_t B);
mat3x3_t add(const mat3x3_t A, const mat3x3_t B);
mat3x4_t add(const mat3x4_t A, const mat3x4_t B);
mat4x2_t add(const mat4x2_t A, const mat4x2_t B);
mat4x3_t add(const mat4x3_t A, const mat4x3_t B);
mat4x4_t add(const mat4x4_t A, const mat4x4_t B);

 // Component-wise subtraction: A - B
mat2x2_t sub(const mat2x2_t A, const mat2x2_t B);
mat2x3_t sub(const mat2x3_t A, const mat2x3_t B);
mat2x4_t sub(const mat2x4_t A, const mat2x4_t B);
mat3x2_t sub(const mat3x2_t A, const mat3x2_t B);
mat3x3_t sub(const mat3x3_t A, const mat3x3_t B);
mat3x4_t sub(const mat3x4_t A, const mat3x4_t B);
mat4x2_t sub(const mat4x2_t A, const mat4x2_t B);
mat4x3_t sub(const mat4x3_t A, const mat4x3_t B);
mat4x4_t sub(const mat4x4_t A, const mat4x4_t B);

 // Matrix transpose: M^T
mat2x2_t transpose(const mat2x2_t M);
mat2x3_t transpose(const mat3x2_t M);
mat2x4_t transpose(const mat4x2_t M);
mat3x2_t transpose(const mat2x3_t M);
mat3x3_t transpose(const mat3x3_t M);
mat3x4_t transpose(const mat4x3_t M);
mat4x2_t transpose(const mat2x4_t M);
mat4x3_t transpose(const mat3x4_t M);
mat4x4_t transpose(const mat4x4_t M);

 // Determinant
float det(const mat2x2_t A);
float det(const mat3x3_t A);
float det(const mat4x4_t A);

 // Matrix inverse.
 //  -- Assumes the matrix is invertible.
 //  -- If you're not sure, ensure that det(A) is non-zero first.
mat2x2_t inverse(const mat2x2_t A);
mat3x3_t inverse(const mat3x3_t A);
mat4x4_t inverse(const mat4x4_t A);

 // Matrix embedding
 // Set: dst = [ M 0 ]
 //            [ 0 1 ]
mat3x3_t embed(const mat2x2_t M);
mat4x4_t embed(const mat3x3_t M);

 // Matrix extraction
 // extract M from A = [ M a ]
 //                    [ b c ]
mat2x2_t extract(const mat3x3_t A);
mat3x3_t extract(const mat4x4_t A);


/*
 * Matrix-matrix multiplication
 */

// Matrix-matrix multiplication: XY
// All possible combinations
mat2x2_t mul(const mat2x2_t X, const mat2x2_t Y);
mat2x2_t mul(const mat2x3_t X, const mat3x2_t Y);
mat2x2_t mul(const mat2x4_t X, const mat4x2_t Y);
mat2x3_t mul(const mat2x2_t X, const mat2x3_t Y);
mat2x3_t mul(const mat2x3_t X, const mat3x3_t Y);
mat2x3_t mul(const mat2x4_t X, const mat4x3_t Y);
mat2x4_t mul(const mat2x2_t X, const mat2x4_t Y);
mat2x4_t mul(const mat2x3_t X, const mat3x4_t Y);
mat2x4_t mul(const mat2x4_t X, const mat4x4_t Y);

mat3x2_t mul(const mat3x2_t X, const mat2x2_t Y);
mat3x2_t mul(const mat3x3_t X, const mat3x2_t Y);
mat3x2_t mul(const mat3x4_t X, const mat4x2_t Y);
mat3x3_t mul(const mat3x2_t X, const mat2x3_t Y);
mat3x3_t mul(const mat3x3_t X, const mat3x3_t Y);
mat3x3_t mul(const mat3x4_t X, const mat4x3_t Y);
mat3x4_t mul(const mat3x2_t X, const mat2x4_t Y);
mat3x4_t mul(const mat3x3_t X, const mat3x4_t Y);
mat3x4_t mul(const mat3x4_t X, const mat4x4_t Y);

mat4x2_t mul(const mat4x2_t X, const mat2x2_t Y);
mat4x2_t mul(const mat4x3_t X, const mat3x2_t Y);
mat4x2_t mul(const mat4x4_t X, const mat4x2_t Y);
mat4x3_t mul(const mat4x2_t X, const mat2x3_t Y);
mat4x3_t mul(const mat4x3_t X, const mat3x3_t Y);
mat4x3_t mul(const mat4x4_t X, const mat4x3_t Y);
mat4x4_t mul(const mat4x2_t X, const mat2x4_t Y);
mat4x4_t mul(const mat4x3_t X, const mat3x4_t Y);
mat4x4_t mul(const mat4x4_t X, const mat4x4_t Y);






/*
 *  ********************************************************************************
 *  *  Implementations only below this point.
 *  ********************************************************************************
 */

// Bring in the implementations for the vector inlines
#define TYPE vec2_t
#define N_COMPONENTS 2
#include "vecinlines.h"
#undef TYPE
#undef N_COMPONENTS
#define TYPE vec3_t
#define N_COMPONENTS 3
#include "vecinlines.h"
#undef TYPE
#undef N_COMPONENTS
#define TYPE vec4_t
#define N_COMPONENTS 4
#include "vecinlines.h"
#undef TYPE
#undef N_COMPONENTS

inline vec3_t cross(const vec3_t a, const vec3_t b)
{
	vec3_t dst;
	dst.x = a.y * b.z;
	dst.y = a.z * b.x;
	dst.z = a.x * b.y;
	dst.x -= a.z * b.y;
	dst.y -= a.x * b.z;
	dst.z -= a.y * b.x;
	return dst;
}

inline mat3x3_t crossMatrix(const vec3_t a)
{
	mat3x3_t toRet;
	toRet[0] = vec3_t(0.0, a.z, -a.y);
	toRet[1] = vec3_t(-a.z, 0.0, a.x);
	toRet[2] = vec3_t(a.y, -a.x, 0.0);
	return toRet;
}

inline vec2_t extract2(const vec3_t v) { return vec2_t(v.x, v.y); }
inline vec2_t extract2(const vec4_t v) { return vec2_t(v.x, v.y); }
inline vec3_t extract3(const vec4_t v) { return vec3_t(v.x, v.y, v.z); }

#include "matvecinlines.h"
#include "matinlines.h"

}
#endif
