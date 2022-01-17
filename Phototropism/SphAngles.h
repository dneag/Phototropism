/*
	SphAngles.h

	Spherical coordinate angles.  pol is for polar, azi is for azimuth
	Within Maya world space, a pol of 0 radians points down the positive x-axis.  As it increases it rotates clockwise when viewing from above.
	An azi of 0. points up the positive y-axis.  As it increases it rotates downwards
*/

#ifndef SphAngles_h
#define SphAngles_h

#include <iostream>
#include <math.h>

struct SphAngles
{
	double pol = 0.;
	double azi = 0.;

	SphAngles() {}

	SphAngles(double POL, double AZI) : pol(POL), azi(AZI) {}

	void operator=(const SphAngles &rhs) {

		pol = rhs.pol;
		azi = rhs.azi;
	}
};

#endif /* SphAngles.h */
