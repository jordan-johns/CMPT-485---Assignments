

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
 * A very basic shader.
 * Does no vertex transformations.
 *  - i.e. Assumes that it is given normalized device coordinates
 * Sets the color of a fragment based on the
 *  ambient light times the object's constant surface reflectance
 *
 * Use this shader implementation as a guide for your own shaders.
 */


#pragma once
#ifndef __SHADERS_SIMPLE_H_
#define __SHADERS_SIMPLE_H_

#include "../shader.h"

namespace Shader
{
namespace Constant
{

class Simple : public Shader
{
protected:

public:
	Simple();
	virtual ~Simple();

	virtual bool setUniforms(const GLProgUniforms &uniforms, const bool usingShadow=false) const;
};

}
}

#endif
