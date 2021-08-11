#pragma once
#ifndef BlockPointGrid_h
#define BlockPointGrid_h

#include "PhotMath.h"

struct BlockPoint {

};

class BlockPointGrid {
	
	double grid[128][128][128];
	double xSize = 24.;
	double ySize = 24.;
	double zSize = 24.;
	double xUnitSize = .2;
	double yUnitSize = .2;
	double zUnitSize = .2;
	int xElements = xSize / xUnitSize;
	int yElements = ySize / yUnitSize;
	int zElements = zSize / zUnitSize;

public:

	BlockPointGrid() {}

	BlockPointGrid(double XSIZE, double YSIZE, double ZSIZE, double XUNITSIZE, double YUNITSIZE, double ZUNITSIZE) {

		xSize = XSIZE;
		xUnitSize = XUNITSIZE;
		xElements = XSIZE / XUNITSIZE;

		ySize = YSIZE;
		yUnitSize = YUNITSIZE;
		yElements = YSIZE / YUNITSIZE;

		zSize = ZSIZE;
		zUnitSize = ZUNITSIZE;
		zElements = ZSIZE / ZUNITSIZE;
	}
};

#endif /* BlockPointGrid_h */