

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
 * Shader that only outputs the depth of fragments
 */


#pragma once
#ifndef __SHADERS_DEPTH_H_
#define __SHADERS_DEPTH_H_

#include "../shader.h"

namespace Shader
{
namespace Constant
{

class Depth : public Shader
{
protected:

public:
	Depth();
	virtual ~Depth();

	virtual bool setUniforms(const GLProgUniforms &uniforms, const bool usingShadow=false) const;
};

}
}

#endif
