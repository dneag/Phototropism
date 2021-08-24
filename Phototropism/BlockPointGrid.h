#pragma once
#ifndef BlockPointGrid_h
#define BlockPointGrid_h

#include <cmath>
#include <vector>

#include <maya/MStreamUtils.h>
#include <maya/MStatus.h>

#include "PhotMath.h"
#include "MeshMaker.h"

struct BlockPoint {

	Point loc;
	double density;

	BlockPoint(const Point &LOC, double DENSITY) : loc(LOC), density(DENSITY) {}
};

class BlockPointGrid {

	struct Unit {

		Point center;
		double density = 0.;

		Unit(double x, double y, double z) {
			
			center.x = x;
			center.y = y;
			center.z = z;
		}
	};

	// We want the grid to be represented as centered on the Maya grid.  This means that x and z elements must always be an odd
	// number.  E.g. xSize / xUnitSize is always an odd number.  Also, this means that the center element itself is centered on
	// the Maya grid.  E.g. the x and z coordinates at the center of the center element are 0. and 0.
	std::vector< std::vector< std::vector<Unit> > > grid;
	double xSize = 24.2;
	double ySize = 24.2;
	double zSize = 24.2;
	double xUnitSize = .2;
	double yUnitSize = .2;
	double zUnitSize = .2;
	// Note to self: after dividing two doubles that divide evenly in reality, the result is represented internally as
	// ~ .00000000001 less than its integral counterpart.  So truncating will reduce by 1.  Thus the ceil here.
	int xElements = std::ceil(xSize / xUnitSize);
	int yElements = std::ceil(ySize / yUnitSize);
	int zElements = std::ceil(zSize / zUnitSize);
	double yGridSize = yUnitSize * yElements;

	// The following two are technically half - 1 of the grid size
	double halfGridXSize = xUnitSize * (xElements / 2);
	double halfGridZSize = zUnitSize * (zElements / 2);

	double detectionRange = 3.;

	std::vector<BlockPoint> bps;

	// Set the center points of all units
	void initiateGrid();

	// Returns the index of the grid unit corresponding to the coordinate passed
	std::size_t findShiftedIndex(double coord, double halfGridSize, double unitSize) const;

	// Checks that the Point is within the range of the grid. Exits the program if it isn't.
	bool checkRange_Point(const Point &p) const;

	// Checks that each index is within the range of the grid
	bool checkRange_Indices(int x, int y, int z) const;

public:

	BlockPointGrid() {
		
		this->initiateGrid();
	}

	BlockPointGrid(double XSIZE, double YSIZE, double ZSIZE, double XUNITSIZE, double YUNITSIZE, double ZUNITSIZE, double DETECTIONRANGE);

	MStatus addBlockPoint(BlockPoint bp);

	MStatus addToUnitDensity(const BlockPoint &bp);

	// Determines the direction that the meristem wants to go
	// coneRangeAngle represents the maximum angle from meriDirection for block point detection
	CVect chooseDirection(const Point &meriLoc, const CVect_m &meriDirection, const double coneRangeAngle);

	void displayGrid() const;

	void displayBlockPoints() const;

	void BlockPointGrid::displayAll() const;
};

#endif /* BlockPointGrid_h */