#include <cmath>
#include <iostream>
#include <algorithm>

#include "PhotMath.h"

Space::Space(SphAngles angles) {

	angles.azi *= -1.;

	double cosA = std::cos(angles.azi);
	double mCos = 1 - cosA;
	double sinA = std::sin(angles.azi);
	double unitVectorPolar = angles.pol + MM::PID2;
	double uX = std::cos(unitVectorPolar);
	//double uY = 0.; to simplify the matrix equations, anything multiplied by uY is removed because uY is always 0
	double uZ = std::sin(unitVectorPolar);
	double uXTimesMCos = uX * mCos;
	double uXTimesSinA = uX * sinA;
	double uZTimesSinA = uZ * sinA;
	double uXTimesUZTimesMCos = uXTimesMCos*uZ;

	aziMatrix[0][0] = uX*uXTimesMCos + cosA;
	aziMatrix[0][1] = -1 * uZTimesSinA;
	aziMatrix[0][2] = uXTimesUZTimesMCos;
	aziMatrix[1][0] = uZTimesSinA;
	aziMatrix[1][1] = cosA;
	aziMatrix[1][2] = -1 * uXTimesSinA;
	aziMatrix[2][0] = uXTimesUZTimesMCos;
	aziMatrix[2][1] = uXTimesSinA;
	aziMatrix[2][2] = uZ*uZ*mCos + cosA;

	polarOrientation = angles.pol;
	aziOrientation = angles.azi;

	u[0] = uX;
	u[1] = 0.;
	u[2] = uZ;
}

CVect Space::makeVector(double polar, double azimuth, double distance) const
{
	double vector[3];
	vector[0] = 0., vector[1] = 0., vector[2] = 0.;
	double addedPolar = polar + polarOrientation;
	double distTimesSinAzi = distance*std::sin(azimuth);
	double vectorToRotate[3];
	vectorToRotate[0] = distTimesSinAzi*std::cos(addedPolar);
	vectorToRotate[1] = distance*std::cos(azimuth);
	vectorToRotate[2] = distTimesSinAzi*std::sin(addedPolar);

	for (int r = 0; r<3; r++)
	{
		for (int c = 0; c<3; c++)
		{
			vector[r] += aziMatrix[r][c] * vectorToRotate[c];
		}
	}

	return CVect(vector);
}

SphAngles findVectorAngles(const CVect &v)
{
	double dist = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);

	if (dist == 0.)
		std::cout << "WARNING: DISTANCE IS ZERO (in findVectorAngles())" << std::endl;

	SphAngles angles;

	if ((v.x > 0.) && (v.z > 0.))
	{
		angles.pol = std::atan(v.z / v.x);
	}
	else if ((v.x < 0.) && (v.z > 0.))
	{
		angles.pol = MM::PI - std::atan(v.z / std::fabs(v.x));
	}
	else if ((v.x < 0.) && (v.z < 0.))
	{
		angles.pol = MM::PI + std::atan(std::fabs(v.z) / std::fabs(v.x));
	}
	else if ((v.x > 0.) && (v.z < 0.))
	{
		angles.pol = MM::PIM2 - std::atan(std::fabs(v.z) / v.x);
	}
	else if ((v.z == 0.) && (v.x > 0.))
	{
		angles.pol = 0.;
	}
	else if ((v.z == 0.) && (v.x < 0.))
	{
		angles.pol = MM::PI;
	}
	else if ((v.x == 0.) && (v.z > 0.))
	{
		angles.pol = MM::PID2;
	}
	else if ((v.x == 0.) && (v.z < 0.))
	{
		angles.pol = MM::PIANDAHALF;
	}
	else
	{
		angles.pol = 0.;
	}

	angles.azi = std::acos(v.y / dist);

	return angles;
}

double distance(const Point &p, const Point &q) {

	double diffV[3];
	diffV[0] = q.x - p.x;
	diffV[1] = q.y - p.y;
	diffV[2] = q.z - p.z;

	return std::sqrt(diffV[0] * diffV[0] + diffV[1] * diffV[1] + diffV[2] * diffV[2]);
}

double findAngBetween(const CVect &p, const CVect &q)
{
	double magProduct = p.getMag() * q.getMag();
	if (magProduct == 0.)
		std::cout << "WARNING: MAGPRODUCT IS ZERO (in findAngBetween())" << std::endl;

	double dotProduct = p.x * q.x + p.y * q.y + p.z * q.z;

	return std::acos(std::max(std::min(dotProduct / magProduct, 1.), -1.));
}

CVect sphAnglesToCartVect(const SphAngles &angles, double mag) {

	CVect v;
	double lengthTimesSinAzi = mag * std::sin(angles.azi);
	v.x = lengthTimesSinAzi * std::cos(angles.pol);
	v.y = mag * std::cos(angles.azi);
	v.z = lengthTimesSinAzi * std::sin(angles.pol);

	return v;
}

double findAngBetween(const CVect_m &v1, const CVect_m &v2)
{
	double magProduct = v1.mag * v2.mag;
	if (magProduct == 0.)
		MStreamUtils::stdOutStream() << "Error; vector has 0 magnitude.\n";

	double dotProduct = v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;

	return std::acos(std::max(std::min(dotProduct / magProduct, 1.), -1.));
}

double randBetween(double mn, double mx)
{
	double range = mx - mn;
	return mn + ((double)rand() / RAND_MAX)*range;
}

Point randPoint(double xMin, double xMax, double yMin, double yMax, double zMin, double zMax) {

	return { randBetween(xMin, xMax), randBetween(yMin, yMax), randBetween(zMin, zMax) };
}