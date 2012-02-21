
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
 *   - Extract surface reflectance at each vertex from a texture
 *   - Use ambient light
 *   - Use direct light from an omni-directional point light
 *   - Perform geometric transformation from object to device coordinates
 *   - Calculate shading per-vertex, and have it interpolated to fragments
 */

#pragma once
#ifndef __INC_SHADERS_TEXTURE_LAMBERTIAN_GOURAND_H_
#define __INC_SHADERS_TEXTURE_LAMBERTIAN_GOURAND_H_

#include "../../shader.h"

namespace Shader
{
namespace Texture
{
namespace Lambertian
{

class Gouraud : public Shader
{
protected:
public:
	Gouraud();
	virtual ~Gouraud();
	virtual void bindGL(const bool useShadow=false) const;
	virtual bool setUniforms(const GLProgUniforms &uniforms, const bool usingShadow=false) const;
};

}
}
}

#endif
