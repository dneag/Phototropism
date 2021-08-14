#pragma once
#ifndef BlockPointGrid_h
#define BlockPointGrid_h

#include <stdexcept>

#include <maya/MStreamUtils.h>

#include "PhotMath.h"

struct BlockPoint {


};

class BlockPointGrid {
	
	struct Unit {

		Point center;
		double density = 0.;
	};

	static const int xElements_MAX = 128;
	static const int yElements_MAX = 128;
	static const int zElements_MAX = 128;

	// We want the grid to be represented as centered on the Maya grid.  This means that x and z elements must always be an odd
	// number.  E.g. xSize / xUnitSize is always an odd number.  Also, this means that the center element itself is centered on
	// the Maya grid.  E.g. the x and z coordinates at the center of the center element are 0. and 0.  This is implemented in the
	// findShiftedIndex() method.
	Unit grid[xElements_MAX][yElements_MAX][zElements_MAX];
	double xSize = 24.2;
	double ySize = 24.2;
	double zSize = 24.2;
	double xUnitSize = .2;
	double yUnitSize = .2;
	double zUnitSize = .2;
	int xElements = xSize / xUnitSize;
	int yElements = ySize / yUnitSize;
	int zElements = zSize / zUnitSize;
	double yGridSize = yUnitSize * yElements;
	double halfGridXSize = xUnitSize * (xElements / 2);
	double halfGridZSize = zUnitSize * (zElements / 2);
	double detectionRange = 3.;

	// Checks that the Point is within the range of the grid. Exits the program if it isn't.
	void checkRange_Point(const Point &p)const ;

	// Checks that the x, y, and zElements attributes within the max elements range. Exits the program if they are not.
	void checkRange_GridElements() const;

	// Returns the index of the grid unit corresponding to the coordinate passed
	std::size_t findShiftedIndex(double xCoord, double halfGridSize, double unitSize) const;

public:

	BlockPointGrid() {}

	BlockPointGrid(double XSIZE, double YSIZE, double ZSIZE, double XUNITSIZE, double YUNITSIZE, double ZUNITSIZE, double DETECTIONRANGE) {

		xSize = XSIZE;
		xUnitSize = XUNITSIZE;
		xElements = XSIZE / XUNITSIZE;

		ySize = YSIZE;
		yUnitSize = YUNITSIZE;
		yElements = YSIZE / YUNITSIZE;

		zSize = ZSIZE;
		zUnitSize = ZUNITSIZE;
		zElements = ZSIZE / ZUNITSIZE;

		this->checkRange_GridElements();

		detectionRange = DETECTIONRANGE;
	}

	//Unit addToUnitDensity(const Point &p) {

	//	// p.x = .8 and halfGridXSize = .8
	//	double xShifted = p.x + halfGridXSize; // xShifted = 1.6
	//	double temp = xShifted / xUnitSize; // temp = 8.
	//	int ind = static_cast<int>(temp);// ind = 8
	//	double remainder = temp - ind; // remainder = 0

	//	if (remainder >= .5) { ++ind; } // ind = 8

	//	return grid[xIndex][y][zIndex];
	//}
};

#endif /* BlockPointGrid_h */