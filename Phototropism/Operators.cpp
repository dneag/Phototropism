/*

operators.cpp
Created by \ on 9/26/20.

*/

#include "operators.h"

bool operator!=(const Point &lhs, const Point &rhs)
{
	return ((lhs.x != rhs.x) || (lhs.y != rhs.y) || (lhs.z != rhs.z));
}

bool operator!=(const SphAngles &lhs, const SphAngles &rhs) {

	return ((lhs.pol != rhs.pol) || (lhs.azi != rhs.azi));
}

bool operator!=(const CVect &lhs, const CVect &rhs) {

	return ((lhs.x != rhs.x) || (lhs.y != rhs.y) || (lhs.z != rhs.z));
}

std::ostream& operator<<(std::ostream& os, const Point &p) {

	os << "[ " << p.x << ", " << p.y << ", " << p.z << " ]";
	return os;
}

std::ostream& operator<<(std::ostream& os, const CVect &p) {

	os << "[ " << p.x << ", " << p.y << ", " << p.z << " ]" << "(" << p.mag << ")";
	return os;
}

std::ostream& operator<<(std::ostream& os, const SphAngles &a) {

	os << "[ " << a.pol << ", " << a.azi << " ]";
	return os;
}

Point& operator+=(Point &lhs, const CVect &rhs) {

	lhs.x += rhs.x;
	lhs.y += rhs.y;
	lhs.z += rhs.z;

	return lhs;
}

Point& operator-=(Point &lhs, const CVect &rhs) {

	lhs.x -= rhs.x;
	lhs.y -= rhs.y;
	lhs.z -= rhs.z;

	return lhs;
}

Point operator+(Point const &lhs, Point const &rhs) {

	return Point(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
}

Point operator+(Point const &lhs, CVect const &rhs) {

	return Point(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
}

Point operator-(Point const &lhs, CVect const &rhs) {

	return Point(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z);
}

CVect operator+(CVect const &lhs, CVect const &rhs) {

	return CVect(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
}

CVect operator-(Point const &lhs, Point const &rhs) {

	return CVect(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z);
}

CVect operator-(CVect const &lhs, CVect const &rhs) {

	return CVect(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z);
}

CVect& operator+=(CVect &lhs, const CVect &rhs) {

	lhs.x += rhs.x;
	lhs.y += rhs.y;
	lhs.z += rhs.z;
	lhs.mag = std::sqrt(lhs.x*lhs.x + lhs.y*lhs.y + lhs.z*lhs.z);

	return lhs;
}

CVect& operator-=(CVect &lhs, const CVect &rhs) {

	lhs.x -= rhs.x;
	lhs.y -= rhs.y;
	lhs.z -= rhs.z;
	lhs.mag = std::sqrt(lhs.x*lhs.x + lhs.y*lhs.y + lhs.z*lhs.z);

	return lhs;
}

CVect& operator*=(CVect &lhs, const double multiplier) {

	lhs.x *= multiplier;
	lhs.y *= multiplier;
	lhs.z *= multiplier;
	lhs.mag *= multiplier;

	return lhs;
}

CVect operator*(CVect const &lhs, const double multiplier) {

	return CVect(lhs.x * multiplier, lhs.y * multiplier, lhs.z * multiplier, lhs.mag * multiplier);
}

CVect operator/(CVect const &lhs, const double divisor) {

	return CVect(lhs.x / divisor, lhs.y / divisor, lhs.z / divisor, lhs.mag / divisor);
}