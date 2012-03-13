
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

#include <png.h>
#include <cstdio>
#include <cstdlib>
#include "png.h"

namespace Texture
{
namespace Image
{

PNGDecoder::PNGDecoder() {
}
PNGDecoder::~PNGDecoder() {
}

bool PNGDecoder::checkSig(FILE *ifile) const {
	if (ifile == 0) return false;

	// Save the current file position
	fpos_t filePos;
	if (0 != fgetpos(ifile, &filePos))
		return false;

	uint8_t sig[8];
	if (1 != fread(sig, 8, 1, ifile)) {
		fsetpos(ifile, &filePos);
		return false;
	}
	fsetpos(ifile, &filePos);
	bool is_png = 0 == png_sig_cmp(sig, 0, 8);
	return is_png;
}

void *PNGDecoder::decode(FILE *ifile,
		uint16_t &width, uint16_t &height,
		uint8_t &nChannels, uint8_t &bitdepth,
		uint16_t &rowbytes) const {
	// Save the file position. In case of errors.
	fpos_t filePos;
	if (0 != fgetpos(ifile, &filePos))
		return 0;

	// Allocate structs for the header information.
	png_structp png_ptr =
			png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
	if (!png_ptr)
		return 0;

	png_infop info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr) {
		png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
		return 0;
	}

	png_infop end_info = png_create_info_struct(png_ptr);
	if (!end_info) {
		png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
		return 0;
	}

	if (setjmp(png_jmpbuf(png_ptr)))	{
		png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
		fsetpos(ifile, &filePos);
		return 0;
	}

	// Set up the io to use the input file.
	png_init_io(png_ptr, ifile);

	// Using the low-level read interface to read the png

	// Read the header info up to the image data.
	png_read_info(png_ptr, info_ptr);

	int _color_type, _width, _height, _bit_depth, _channels;
	_color_type = png_get_color_type(png_ptr, info_ptr);
	_width = png_get_image_width(png_ptr, info_ptr);
	_height = png_get_image_height(png_ptr, info_ptr);
	_bit_depth = png_get_bit_depth(png_ptr, info_ptr);

	if (_color_type == PNG_COLOR_TYPE_PALETTE ||
			_color_type == PNG_COLOR_TYPE_RGB ||
			_color_type == PNG_COLOR_TYPE_RGB_ALPHA)
		_channels = 3;
	else
		_channels = 1;

	// ---
	// Set up input transforms

	// Convert paletted images to RGB
	if (_color_type == PNG_COLOR_TYPE_PALETTE) {
		png_set_palette_to_rgb(png_ptr);
		// Bit depth for a palette image could be 1, 2, 4, or 8
		// When converting to RGB the bit_depth will become 8
		_bit_depth = 8;
	}

	// 1, 2, or 4-bit greyscale to 8-bit greyscale
	if (_color_type == PNG_COLOR_TYPE_GRAY && _bit_depth < 8) {
		png_set_expand_gray_1_2_4_to_8(png_ptr);
		_bit_depth = 8;
	}

	// Reduce 16-bit per channel to 8-bit
	//			if (_bit_depth == 16)
	//				png_set_strip_16(png_ptr);

	//			if ( isLittleEndian() && _bit_depth > 8 )
	//				png_set_swap(png_ptr);


	// Strip off the alpha channel
	if (_color_type & PNG_COLOR_MASK_ALPHA)
		png_set_strip_alpha(png_ptr);

	// Potentially linearize the gamma-corrected intensity data.
	double _file_gamma = 0.45455; // sRGB standard gamma
	int _sRGB_intent;
	if (png_get_sRGB(png_ptr, info_ptr, &_sRGB_intent) != 0) {
		// the sRGB chunk is set, so we know that the file gAMA must
		// be 0.45455
		png_set_gamma(png_ptr, 1.0, 0.45455);
	}
	else
		if (png_get_gAMA(png_ptr, info_ptr, &_file_gamma) != 0) {
			// gAMA chunk was set.
			png_set_gamma(png_ptr, 1.0, _file_gamma);
		}

	// Number of bytes we expect a row to require
	uint32_t _rowbytes = sizeof(uint8_t) * _channels * _width;
	if (_bit_depth == 16)
		_rowbytes *= 2;
	// If not a multiple of 4, then round up to a multiple of 4
	if ( _rowbytes & 0x00000003 )
	{
		_rowbytes = (_rowbytes + 4) & (~0x00000003);
	}

	// Allocate space for the image data.
	void *toReturn = malloc(_height * _rowbytes);

	// Set up the row pointers.
	png_bytep row_ptrs[_height];
	row_ptrs[0] = (png_bytep)toReturn;
	for (int r=1;r<_height;r++)
		row_ptrs[r] = row_ptrs[r-1] + _rowbytes;

	// Read in the image
	png_read_image(png_ptr, row_ptrs);

	// Read in the post-header
	png_read_end(png_ptr, end_info);

	// Free the info structs.
	png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);

	width = _width;
	height = _height;
	nChannels = _channels;
	bitdepth = _bit_depth;
	rowbytes = _rowbytes;

	return toReturn;
}


}
}

