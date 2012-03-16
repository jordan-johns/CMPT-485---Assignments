
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

#include <cstdlib>

#include "types.h"
#include "../GML/gml.h"

namespace RayTracing
{

static float randFloat()
{
	return rand() / ((float)RAND_MAX);
}

void _Ray_t::randomDirection(const gml::vec3_t &n)
{
	// TODO!!
	//   Set this->d to a random unit-length direction from the hemisphere
	//  centered on n (note: n is unit-length)

	//  This will entail calculating a direction according to equation 23.3 from the text.
	//  However, to calculate 'u' for the equation use the following instead of the
	//  text's method:
	//     a) Find the element of n with largest magnitude.
	//     b) Create vector 't' from n by swapping the element from (a) with
	//        any other element of 'n' and negating the sign of the element from 'n'
	//        with largest magnitude in t.
	//         ex: If 'n' is (1, -2, 3), then a candidate for 't' is (1, 3, 2)
	//                i.e. swapped -2 & 3, then flipped sign of -2
	//     c) u = cross(n, t)

	// You will find the randFloat() function above useful for generating
	// the random numbers required.
	this->d = gml::add(n, gml::vec3_t(randFloat(), randFloat(), randFloat()));

}

}
