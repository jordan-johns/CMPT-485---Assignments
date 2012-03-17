
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

	float maxIndex = 0;
	float max = n.x;
	if (n.y > max)
	{
			max = n.y;
			maxIndex = 1;
	}
	if (n.z > max)
	{
			max = n.z;
			maxIndex = 2;
	}



	//     b) Create vector 't' from n by swapping the element from (a) with
	//        any other element of 'n' and negating the sign of the element from 'n'
	//        with largest magnitude in t.
	//         ex: If 'n' is (1, -2, 3), then a candidate for 't' is (1, 3, 2)
	//                i.e. swapped -2 & 3, then flipped sign of -2

	gml::vec3_t t = gml::vec3_t(n.x, n.y, n.z);
	if (maxIndex == 0)
	{
			t.z = n.x;
			t.x = n.z * -1;
	}
	else if (maxIndex == 1)
	{
			t.x = n.y;
			t.y = n.x * -1;
	}
	else if (maxIndex == 2)
	{
			t.y = n.z;
			t.z = n.y * -1;
	}



	//     c) u = cross(n, t)

	// You will find the randFloat() function above useful for generating
	// the random numbers required.

	gml::vec3_t w = gml::normalize(n);
	gml::vec3_t u = gml::normalize(gml::cross(n, t));
	gml::vec3_t v = gml::cross(w, u);

	float r1 = randFloat();
	float r2 = randFloat();
	//float r3 = randFloat();

	float scalarU = cosf(2.0f * M_PI * r1) * sqrtf(r2);
	float scalarV = sinf(2.0f * M_PI * r1) * sqrtf(r2);
	float scalarW = sqrtf(1.0f - r2);

	gml::vec3_t newU = gml::scale(scalarU, u);
	gml::vec3_t newV = gml::scale(scalarV, v);
	gml::vec3_t newW = gml::scale(scalarW, w);

	// You will find the randFloat() function above useful for generating
	// the random numbers required.
	this->d = gml::add(newU, gml::add(newV, newW));



}

}
