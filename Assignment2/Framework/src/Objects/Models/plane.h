
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
 * Geometry for a plane
 *
 * The plane is centered at (0,0,0), and is entirely
 * in the xz plane. Coordinates of: x,z \in [-1,1]
 *
 * The front face of the plane is in the +y-axis direction.
 * The normal to the plane is the y-axis.
 */

#pragma once
#ifndef __INC_PLANE_H_
#define __INC_PLANE_H_

#include "../geometry.h"
#include "../mesh.h"

namespace Object
{
namespace Models
{

class Plane : public Geometry
{
protected:
	Mesh m_mesh;
public:
	Plane();
	~Plane();

	bool init();

	virtual void rasterize() const;
};

}
}

#endif
