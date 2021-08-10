#include "Operators.h"

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

Point operator+(Point const &lhs, CVect const &rhs) {

	Point p;
	p.x = lhs.x + rhs.x;
	p.y = lhs.y + rhs.y;
	p.z = lhs.z + rhs.z;
	return p;
}

Point operator-(Point const &lhs, CVect const &rhs) {

	Point p;
	p.x = lhs.x - rhs.x;
	p.y = lhs.y - rhs.y;
	p.z = lhs.z - rhs.z;
	return p;
}

CVect operator+(CVect const &lhs, CVect const &rhs) {

	CVect v;
	v.x = lhs.x + rhs.x;
	v.y = lhs.y + rhs.y;
	v.z = lhs.z + rhs.z;
	return v;
}

CVect operator-(Point const &lhs, Point const &rhs) {

	CVect v;
	v.x = lhs.x - rhs.x;
	v.y = lhs.y - rhs.y;
	v.z = lhs.z - rhs.z;
	return v;
}

CVect operator-(CVect const &lhs, CVect const &rhs) {

	CVect v;
	v.x = lhs.x - rhs.x;
	v.y = lhs.y - rhs.y;
	v.z = lhs.z - rhs.z;
	return v;
}

CVect& operator+=(CVect &lhs, const CVect &rhs) {

	lhs.x += rhs.x;
	lhs.y += rhs.y;
	lhs.z += rhs.z;

	return lhs;
}

CVect& operator*=(CVect &lhs, const double multiplier) {

	lhs.x *= multiplier;
	lhs.y *= multiplier;
	lhs.z *= multiplier;

	return lhs;
}

CVect operator*(CVect const &lhs, const double multiplier) {

	CVect v;
	v.x = lhs.x * multiplier;
	v.y = lhs.y * multiplier;
	v.z = lhs.z * multiplier;
	return v;
}

CVect operator/(CVect const &lhs, const double divisor) {

	CVect v;
	v.x = lhs.x / divisor;
	v.y = lhs.y / divisor;
	v.z = lhs.z / divisor;
	return v;
}

std::ostream& operator<<(std::ostream& os, const CVect &v) {

	os << "[ " << v.x << ", " << v.y << ", " << v.z << " ]";
	return os;
}

std::ostream& operator<<(std::ostream& os, const Point &p) {

	os << "[ " << p.x << ", " << p.y << ", " << p.z << " ]";
	return os;
}

std::ostream& operator<<(std::ostream& os, const CVect_m &v) {

	os << "[ " << v.x << ", " << v.y << ", " << v.z << ", " << v.mag << " ]";
	return os;
}