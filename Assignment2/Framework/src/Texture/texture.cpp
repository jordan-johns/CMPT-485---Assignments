
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

#include <cstring>
#include <cstdlib>

#include "texture.h"
#include "../GL3/gl3w.h"
#include "Decoders/png.h"
#include "../glUtils.h"

namespace Texture
{

Texture::Texture(const char *filename,
		FilterType minFilter, FilterType magFilter,
		WrapMode wrap)
{

	m_filename = strdup(filename);
	m_minFilter = minFilter;
	m_magFilter = magFilter;

	m_wrapMode = wrap;

	m_image = 0;
	m_handle = 0;

	m_isReady = false;

	// Try to read in the image file
	FILE *infile = fopen(filename, "r");
	if (!infile)
	{
		return;
	}

	Image::PNGDecoder decoder;
	if ( !decoder.checkSig(infile) )
	{
		fprintf(stderr, "ERROR! Texture file not a png\n");
		fclose(infile);
		return;
	}

	void *m_image = decoder.decode(infile, m_width, m_height, m_nChannels, m_bitDepth, m_rowBytes);

	if ( !m_image )
	{
		return;
	}

	// Upload the texture to the GL context
	glGenTextures(1, &m_handle);
	if ( isGLError() || (0==m_handle) )
	{
		return;
	}
	glBindTexture(GL_TEXTURE_2D, m_handle);

	if ( isGLError() )
	{
		fprintf(stderr, "ERROR!! 1\n");
		return;
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_wrapMode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_wrapMode);

	if ( isGLError() )
	{
		fprintf(stderr, "ERROR!! 1\n");
		return;
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_minFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_magFilter);

	if ( isGLError() )
	{
		fprintf(stderr, "ERROR!! 1\n");
		return;
	}

	// Each row of image data is 4-byte aligned.
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	if ( isGLError() )
	{
		fprintf(stderr, "ERROR!! 1\n");
		return;
	}

	//fprintf(stderr, "Channels = %u\nwidth = %u\nheight = %u\nbitdepth = %u\n", m_nChannels, m_width, m_height, m_bitDepth);

	glTexImage2D(GL_TEXTURE_2D, 0,
			(m_nChannels==1)?GL_RED:GL_RGB8,
			m_width, m_height, 0,
			(m_nChannels==1)?GL_RED:GL_RGB,
			(m_bitDepth==8)?GL_UNSIGNED_BYTE:GL_UNSIGNED_SHORT,
			m_image);

	if ( isGLError() )
	{
		fprintf(stderr, "ERROR!! 1\n");
		return;
	}

	m_isReady = glIsTexture(m_handle) == GL_TRUE;
}

Texture::~Texture()
{
	if (m_filename) free((char*)m_filename);
	if (m_image) free(m_image);
	if (m_handle)
	{
		glDeleteTextures(1, &m_handle);
	}
}


void Texture::bindGL(GLenum textureUnit) const
{
	if (m_isReady)
	{
		glActiveTexture(textureUnit);
		glBindTexture(GL_TEXTURE_2D, m_handle);
	}
}

}
