/*

CVect.h
Created by \ on 9/26/20.

*/

#ifndef CVect_h
#define CVect_h

#include <math.h>
#include <iostream>

#include <maya/MStreamUtils.h>

#include "Point.h"
#include "SphAngles.h"

class CVect
{
	double x = 0.;
	double y = 0.;
	double z = 0.;
	double mag = 0.;

public:

	CVect(double X, double Y, double Z) : x(X), y(Y), z(Z), mag(std::sqrt(X*X + Y*Y + Z*Z)) {}

	CVect(double X, double Y, double Z, double M) : x(X), y(Y), z(Z), mag(M) {}

	CVect(double v[3]) : x(v[0]), y(v[1]), z(v[2]), mag(std::sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2])) {}

	CVect(const CVect &rhs) : x(rhs.x), y(rhs.y), z(rhs.z), mag(rhs.mag) {}

	CVect(const Point &rhs) : x(rhs.x), y(rhs.y), z(rhs.z), mag(std::sqrt(rhs.x * rhs.x + rhs.y * rhs.y + rhs.z * rhs.z)) {}

	void set(double X, double Y, double Z, double M) {

		x = X;
		y = Y;
		z = Z;
		mag = M;
	}

	void operator=(const CVect &rhs) {

		x = rhs.x;
		y = rhs.y;
		z = rhs.z;
		mag = rhs.mag;
	}

	double getX() const { return x; }
	double getY() const { return y; }
	double getZ() const { return z; }
	double getMag() const { return mag; }

	void resize(double newLength);

	CVect resized(double newLength) const;

	void changeDirection(const CVect &v);

	void vectTrunc4();

	friend double dotProduct(const CVect &a, const CVect &b);

	friend CVect crossProduct(const CVect &a, const CVect &b);

	friend std::ostream& operator<<(std::ostream& os, const CVect &p);

	friend bool operator!=(const CVect &lhs, const CVect &rhs);

	friend Point& operator+=(Point &lhs, const CVect &rhs);

	friend Point& operator-=(Point &lhs, const CVect &rhs);

	friend Point operator+(Point const &lhs, CVect const &rhs);

	friend Point operator-(Point const &lhs, CVect const &rhs);

	friend CVect operator+(CVect const &lhs, CVect const &rhs);

	friend CVect operator-(CVect const &lhs, CVect const &rhs);

	friend CVect& operator+=(CVect &lhs, const CVect &rhs);

	friend CVect& operator-=(CVect &lhs, const CVect &rhs);

	friend CVect& operator*=(CVect &lhs, const double multiplier);

	friend CVect operator*(CVect const &lhs, const double multiplier);

	friend CVect operator/(CVect const &lhs, const double divisor);

	friend SphAngles findVectorAngles(const CVect &v);
};

#endif /* CVect_h */

