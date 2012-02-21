
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
 * Shader that should:
 *   - Use a constant surface reflectance at all vertices
 *   - Use ambient light
 *   - Use direct light from an omni-directional point light
 *   - Use specular properties of the material
 *   - Perform geometric transformation from object to device coordinates
 *   - Calculate shading per-fragment
 */

#pragma once
#ifndef __INC_SHADERS_CONSTANT_SPECULAR_PHONG_H_
#define __INC_SHADERS_CONSTANT_SPECULAR_PHONG_H_

#include "../../shader.h"

namespace Shader
{
namespace Constant
{
namespace Specular
{

class Phong : public Shader
{
protected:
public:
	Phong();
	virtual ~Phong();

	virtual bool setUniforms(const GLProgUniforms &uniforms, const bool usingShadow=false) const;
};

}
}
}

#endif
