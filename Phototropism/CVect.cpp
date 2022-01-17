/*

	CVect.cpp

*/

#include <maya/MGlobal.h>
#include <maya/MObject.h>

#include "CVect.h"
#include "Point.h"
#include "operators.h"

void CVect::resize(double newLength) {

	if (mag == 0.)
		MStreamUtils::stdOutStream() << "VECTOR LENGTH IS ZERO (in CVect::resize())\n";

	double normalizer = newLength / mag;
	x *= normalizer;
	y *= normalizer;
	z *= normalizer;
	mag = newLength;
}

CVect CVect::resized(double newLength) const {

	if (mag == 0.)
		MStreamUtils::stdOutStream() << "VECTOR LENGTH IS ZERO (in CVect::resized())\n";

	double normalizer = newLength / mag;

	return CVect(x*normalizer, y*normalizer, z*normalizer, newLength);
}


void CVect::changeDirection(const CVect &v) {

	/* changes the direction of a vector by adding a separate vector, then resizing back to its original length */
	double originalLength = mag;
	*this += v;
	this->resize(originalLength);
}

void CVect::vectTrunc4() {

	x = std::trunc(x * 10000.) / 10000.;
	y = std::trunc(y * 10000.) / 10000.;
	z = std::trunc(z * 10000.) / 10000.;

	mag = std::sqrt(x*x + y*y + z*z);
}
