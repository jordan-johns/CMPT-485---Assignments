
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

	void *image = decoder.decode(infile, m_width, m_height, m_nChannels, m_bitDepth, m_rowBytes);

	if ( !image )
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
									image);

	if ( isGLError() )
	{
		fprintf(stderr, "ERROR!! 1\n");
		return;
	}

	m_isReady = glIsTexture(m_handle) == GL_TRUE;

	m_image = new gml::vec3_t[m_width * m_height];
	if (m_nChannels == 1)
	{
		if (m_bitDepth == 8)
		{
			for (int r=0; r<m_height; r++)
			{
				uint8_t *px = (uint8_t*)((uint8_t*)image + r*m_rowBytes);
				for (int c=0; c<m_width; c++, px++)
				{
					float red = (*px) / 255.0f;
					m_image[r*m_width + c] = gml::vec3_t(red, red, red);
				}
			}
		}
		else // 16-bit
		{
			for (int r=0; r<m_height; r++)
			{
				uint16_t *px = (uint16_t*)((uint8_t*)image + r*m_rowBytes);
				for (int c=0; c<m_width; c++, px++)
				{
					float red = (*px) / 65535.0f;
					m_image[r*m_width + c] = gml::vec3_t(red, red, red);
				}
			}
		}
	}
	else // 3 channels
	{
		if (m_bitDepth == 8)
		{
			for (int r=0; r<m_height; r++)
			{
				uint8_t *px = (uint8_t*)((uint8_t*)image + r*m_rowBytes);
				for (int c=0; c<m_width; c++, px+=3)
				{
					float red = (*px) / 255.0f;
					float g = (*(px+1)) / 255.0f;
					float b = (*(px+2)) / 255.0f;
					m_image[r*m_width + c] = gml::vec3_t(red, g, b);
				}
			}
		}
		else // 16-bit
		{
			for (int r=0; r<m_height; r++)
			{
				uint16_t *px = (uint16_t*)((uint8_t*)image + r*m_rowBytes);
				for (int c=0; c<m_width; c++, px+=3)
				{
					float red = (*px) / 65535.0f;
					float g = (*(px+1)) / 65535.0f;
					float b = (*(px+2)) / 65535.0f;
					m_image[r*m_width + c] = gml::vec3_t(red, g, b);
				}
			}
		}

	}

	free(image);
}

Texture::~Texture()
{
	if (m_filename) free((char*)m_filename);
	if (m_image) delete[] m_image;
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

gml::vec3_t Texture::lookup(gml::vec2_t coords) const
{
	if (m_wrapMode == REPEAT)
	{
		coords.s = copysignf( fmodf(coords.s, 1.0f), 1.0f );
		coords.t = copysignf( fmodf(coords.t, 1.0f), 1.0f );
	}
	else // CLAMP mode
	{
		if (coords.s < 0.0f) coords.s = 0.0f;
		else if (coords.s > 1.0f) coords.s = 1.0f;
		if (coords.t < 0.0f) coords.t = 0.0f;
		else if (coords.t > 1.0f) coords.t = 1.0f;
	}

	if (m_magFilter == NEAREST)
	{
		int row = (int)(coords.t * (m_height - 1) + 0.5f);
		int col = (int)(coords.s * (m_width - 1) + 0.5f);

		return m_image[row*m_width + col];
	}
	else // LINEAR (aka: bi-linear interpolation)
	{
		float x = coords.s * (m_width - 1);
		float y = coords.t * (m_height - 1);

		int low_x = (int)floorf(x), high_x = (int)ceilf(x);
		int low_y = (int)floorf(y), high_y = (int)ceilf(y);
		int res_x = x - floorf(x), res_y = y - floorf(y);

		gml::vec3_t c1, c2;
		c1 = gml::add( gml::scale((1.0f-res_x), m_image[low_y*m_width+low_x]), gml::scale(res_x, m_image[low_y*m_width+high_x]) );
		c2 = gml::add( gml::scale((1.0f-res_x), m_image[high_y*m_width+low_x]), gml::scale(res_x, m_image[high_y*m_width+high_x]) );

		return gml::add( gml::scale((1.0f-res_y), c1), gml::scale(res_y, c2) );
	}
}

}
