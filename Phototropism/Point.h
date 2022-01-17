/*
	Point.h
*/

#ifndef Point_h
#define Point_h

#include <iostream>
#include <math.h>

struct Point
{
	double x = 0.;
	double y = 0.;
	double z = 0.;

	Point() {}

	Point(double X, double Y, double Z) : x(X), y(Y), z(Z) {}

	void operator=(const Point &rhs) {

		x = rhs.x;
		y = rhs.y;
		z = rhs.z;
	}

	double getMag() const { return std::sqrt(x*x + y*y + z*z); }
};

#endif /* Point.h */
