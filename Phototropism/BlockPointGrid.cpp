/*
	BlockPointGrid.cpp
*/

#include <math.h>

#include "BlockPointGrid.h"
#include "Operators.h"

void BlockPointGrid::initiateGrid() {

	double xCoord = 0. - halfGridXSize;

	for (int xI = 0; xI < xElements; ++xI) {
		
		grid.push_back(std::vector< std::vector<Unit> >());
		double yCoord = yUnitSize / 2.;

		for (int yI = 0; yI < yElements; ++yI) {

			grid.back().push_back(std::vector<Unit>());
			double zCoord = 0. - halfGridZSize;

			for (int zI = 0; zI < zElements; ++zI) {

				grid.back().back().push_back(Unit(xCoord, yCoord, zCoord));

				zCoord += zUnitSize;
			}

			yCoord += yUnitSize;
		}

		xCoord += xUnitSize;
	}
}

BlockPointGrid::BlockPointGrid(double XSIZE, double YSIZE, double ZSIZE, double XUNITSIZE, double YUNITSIZE, double ZUNITSIZE, double DETECTIONRANGE) {

	xSize = XSIZE;
	xUnitSize = XUNITSIZE;
	xElements = std::ceil(XSIZE / XUNITSIZE);

	ySize = YSIZE;
	yUnitSize = YUNITSIZE;
	yElements = std::ceil(YSIZE / YUNITSIZE);

	zSize = ZSIZE;
	zUnitSize = ZUNITSIZE;
	zElements = std::ceil(ZSIZE / ZUNITSIZE);

	yGridSize = yUnitSize * yElements;
	halfGridXSize = xUnitSize * (xElements / 2);
	halfGridZSize = zUnitSize * (zElements / 2);

	this->initiateGrid();

	detectionRange = DETECTIONRANGE;
}

std::size_t BlockPointGrid::findShiftedIndex(double xCoord, double halfGridSize, double unitSize) const {
	
	// Add halfGridSize to put the coordinate in the positive range. E.g. the range corresponding to grid element indices.
	double shiftedCoord = xCoord + halfGridSize;

	// To represent the grid as centered, we must account for a potential additional shift.  We do this by adding 1 to the index
	// if the remainder from truncating temp is greater than .5.
	double temp = shiftedCoord / unitSize;
	std::size_t ind = std::floor(temp);
	double remainder = temp - ind; 
	if (remainder >= .5) { ++ind; }

	return ind;
}

MStatus BlockPointGrid::addToUnitDensity(const Point &p) {

	if (!this->checkRange_Point(p))
		return MS::kFailure;

	std::size_t xInd = findShiftedIndex(p.x, halfGridXSize, xUnitSize);
	std::size_t yInd = std::floor(p.y / yUnitSize);
	std::size_t zInd = findShiftedIndex(p.z, halfGridZSize, zUnitSize);

	MStreamUtils::stdOutStream() << "grid unit indices: " << xInd << ", " << yInd << ", " << zInd << "\n";

	return MS::kSuccess;
}

void BlockPointGrid::displayGrid() const {

	MStreamUtils::stdOutStream() << "\n";

	MStreamUtils::stdOutStream() << xElements << ", " << yElements << ", " << zElements << "\n";
	for (int xI = 0; xI < xElements; ++xI) {
		for (int yI = 0; yI < yElements; ++yI) {
			for (int zI = 0; zI < zElements; ++zI) {

				MStreamUtils::stdOutStream() << "[" << xI << "][" << yI << "][" << zI << "] = ";
				MStreamUtils::stdOutStream() << grid[xI][yI][zI].center << "\n";
			}
		}
	}
}

bool BlockPointGrid::checkRange_Point(const Point &p) const {

	if (p.x > halfGridXSize || p.x < -halfGridXSize) {

		MStreamUtils::stdOutStream() << "Error. x coordinate outside of grid.\nAborting\n";
		return false;
	}
	else if (p.y > yGridSize || p.y < 0) {

		MStreamUtils::stdOutStream() << "Error. y coordinate outside of grid.\nAborting\n";
		return false;
	}
	else if (p.z > halfGridZSize || p.z < -halfGridZSize) {

		MStreamUtils::stdOutStream() << "Error. z coordinate outside of grid.\nAborting\n";
		return false;
	}

	return true;
}