
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
#ifndef __INC_TEXTURE_TEXTURE_H_
#define __INC_TEXTURE_TEXTURE_H_

#include "../GL3/gl3.h"
#include "../GML/gml.h"

namespace Texture
{

// Filter types
typedef enum
{
	NEAREST=GL_NEAREST,  // Nearest neighbour
			LINEAR=GL_LINEAR      // Linear (bi-linear) filtering
} FilterType;

// Ways to wrap out-of-range texture coordinates
typedef enum
{
	REPEAT = GL_REPEAT,
			CLAMP_TO_EDGE = GL_CLAMP_TO_EDGE
} WrapMode;

// A _very_ simple texture class. Only supports 8-bit per channel
// images.
class Texture
{
protected:
	// Filename the texture was read from, if any.
	const char *m_filename;

	// Filter types
	FilterType m_minFilter;
	FilterType m_magFilter;

	WrapMode m_wrapMode;

	GLuint m_handle;

	bool m_isReady;

	// Image properties
	uint16_t m_width, m_height;
	uint8_t m_nChannels;
	uint8_t m_bitDepth;
	uint16_t m_rowBytes;

	// Image data
	// malloc'd array of image data
	//	8-bit, this is a uint8_t*
	//	16-bit, this is a uint16_t*
	void *m_image;
public:

	Texture(const char *filename,
			FilterType minFilter=LINEAR,
			FilterType magFilter=LINEAR,
			WrapMode wrap=REPEAT);

	~Texture();

	bool getIsReady() const { return m_isReady; }

	// textureUnit is one of GL_TEXTURE#, where # is 0,1,2,3,...,etc
	void bindGL(GLenum textureUnit) const;

};


}


#endif
