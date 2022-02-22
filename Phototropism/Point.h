/*
	Point.h
*/

#ifndef Point_h
#define Point_h

#include <iostream>
#include <math.h>

#include <maya/MStreamUtils.h>

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

	void resize(double newLength) {

		double mag = std::sqrt(x*x + y*y + z*z);
		if (mag == 0.)
			MStreamUtils::stdOutStream() << "POINT LENGTH IS ZERO (in Point::resize())\n";

		double normalizer = newLength / mag;
		x *= normalizer;
		y *= normalizer;
		z *= normalizer;
	}
};

#endif /* Point.h */
