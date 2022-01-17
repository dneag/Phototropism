/*

	operators.h
	Created by \ on 9/26/20.

*/

#ifndef operators_h
#define operators_h

#include "Point.h"
#include "CVect.h"
#include "SphAngles.h"

#include <math.h>
#include <iostream>

bool operator!=(const Point &lhs, const Point &rhs);

bool operator!=(const SphAngles &lhs, const SphAngles &rhs);

std::ostream& operator<<(std::ostream& os, const Point &p);

std::ostream& operator<<(std::ostream& os, const SphAngles &a);

Point operator+(Point const &lhs, Point const &rhs);

CVect operator-(Point const &lhs, Point const &rhs);

#endif /* operators.h */