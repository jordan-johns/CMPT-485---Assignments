
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
#ifndef __INC_TEXTURE_IMAGEDECODER_H_
#define __INC_TEXTURE_IMAGEDECODER_H_

#include <cstdint>
#include <cstdio>

namespace Texture
{
namespace Image
{


// Base class for reading an image from a file.
// Notes:
//	 1) Image decoders _always_ output linearized intensity values.
//	 2) RGB images are output in RGB order.
//	 3) All output images are output as 8 bits per channel.
class Decoder {
public:
	Decoder();
	virtual ~Decoder();

	// Check the signature bytes at the current file pointer positions, then
	// rewinds file pointer back to the current position.
	// Input:
	//	 ifile - FILE descriptor opened in BINARY mode.
	// Returns true if the signature is that for a file of the
	// Decoder's type.
	virtual bool checkSig(FILE *ifile) const = 0;

	// Decode the given open file stream.
	// **Assumes that the file is the correct type/format for the
	// decoder **
	// Input:
	//	 ifile - FILE descriptor opened in BINARY mode.
	// Return:
	//	 width -- Width of the image, in pixels.
	//	 height -- Height of the image, in pixels.
	//	 nChannels -- Number of channels in the image.
	//	 bitdepth -- The bitdepth of the returned image data. Either
	//							 8 or 16
	//   rowbytes -- The number of bytes per row of allocated image data
	//               Set up so each row occupies a multiple of 4 bytes.
	//	 return val -- malloc'd array of image data on success, 0 on
	//								 failure.
	//								 8-bit, this is a uint8_t*
	//								 16-bit, this is a uint16_t*
	virtual void *decode(FILE *ifile,
			uint16_t &width, uint16_t &height,
			uint8_t &nChannels, uint8_t &bitdepth,
			uint16_t &rowbytes) const = 0;
};

}

}

#endif
