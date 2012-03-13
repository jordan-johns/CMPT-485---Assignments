
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
#ifndef __INC_TEXTURE_DECODER_PNG_H_
#define __INC_TEXTURE_DECODER_PNG_H_

#include "decoder.h"

namespace Texture
{
namespace Image
{

class PNGDecoder : public Decoder {
public:
	PNGDecoder();
	virtual ~PNGDecoder();

	virtual bool checkSig(FILE *ifile) const;
	virtual void *decode(FILE *ifile,
			uint16_t &width, uint16_t &height,
			uint8_t &nChannels, uint8_t &bitdepth,
			uint16_t &rowbytes) const;
};

}
}

#endif
