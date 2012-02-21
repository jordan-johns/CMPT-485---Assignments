
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
 * Home-brew utility functions for checking on OpenGL error
 * status, and for determining extension support.
 *
 * The error checking function (isGLError) is defined
 * as a macro function.
 *  - If you define the symbol NDEBUG (typical for release
 *    builds to disable asserts), then calls to isGLError()
 *    will become no-ops.
 */

#pragma once
#ifndef __INC_GL_UTILS_H_
#define __INC_GL_UTILS_H_

#if !defined(NDEBUG)
// Note: __FILE__ and __LINE__ are preprocessor magic to get
//  the source file name & line number of where the error
//  occured.
# define isGLError() _isGLError(__FILE__, __LINE__)
#else
# define isGLError() false
#endif

bool _isGLError(const char *file, const int line);

bool isExtensionSupported(const char *name);

#endif
