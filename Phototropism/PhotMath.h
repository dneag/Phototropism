/*
	PhotMath.h

	Mathematical classes and functions
*/

#pragma once
#ifndef PhotMath_h
#define PhotMath_h

#include <cmath>
#include <iostream>
#include <random>

#include <maya/MStreamUtils.h>

#include "Point.h"
#include "SphAngles.h"
#include "CVect.h"

namespace MM {

	const double PI = 3.14159265358979;
	const double PID2 = 1.5707963267949;
	const double PID4 = PI / 4.;
	const double PIM2 = 6.28318530717959;
	const double PIANDAHALF = PI + PID2;
	const double sinPID2 = std::sin(PID2);
	const double cosPID2 = std::cos(PID2);
}

// Uses a 3x3 matrix to represent the orientation of a 3D space
// Creates vectors relative to the orientation using spherical coordinates
class Space
{
	double aziMatrix[3][3];
	double polarOrientation = 0.;
	double aziOrientation = 0.;
	double u[3];

public:

	// create a Space oriented to the angles parameter, the angles represent the positive y-axis
	Space(SphAngles angles);

	// takes spherical coordinates as arguments and returns a CVect relative to the current orientation
	CVect makeVector(double polar, double azimuth, double distance) const;
};

// polar angles of the CVect passed
SphAngles findVectorAngles(const CVect &v);

// distance between p and q
double distance(const Point &p, const Point &q);

// angle (in radians) between p and q
double findAngBetween(const CVect &a, const CVect &b);

// returns a CVect corresponding to the angles and magnitude passed
CVect sphAnglesToCartVect(const SphAngles &angles, double mag);

double randBetween(double mn, double mx);

Point randPoint(double xMin, double xMax, double yMin, double yMax, double zMin, double zMax);

double trunc4(double value);

#endif /* PhotMath_h */

