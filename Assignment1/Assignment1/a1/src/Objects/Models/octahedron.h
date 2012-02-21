
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
 * Geometry for an octahedron
 *  - An 8-sided regular solid.
 *
 * This octahedron has flat faces.
 * It is also centered at (0,0,0).
 * The faces face 45 degree angles relative to the xz plane
 */

#pragma once
#ifndef __INC_OCTAHEDRON_H_
#define __INC_OCTAHEDRON_H_

#include "../geometry.h"
#include "../mesh.h"

namespace Object
{
namespace Models
{

class Octahedron : public Geometry
{
protected:
	Mesh m_mesh;
public:
	Octahedron();
	~Octahedron();

	bool init();

	virtual void rasterize() const;
};

}
}

#endif
