
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
 * Base class for all geometric objects
 *
 * Any new geometric objects you define (ex: plane, cone, cylinder)
 * must use this as its base class.
 */

#pragma once
#ifndef __INC_GEOMETRY_H_
#define __INC_GEOMETRY_H_

namespace Object
{

// Base class for all geometric models.
class Geometry
{
protected:
public:
	Geometry();
	virtual ~Geometry();

	// Rasterize this object via OpenGL
	virtual void rasterize() const = 0;
};

} // namespace

#endif
