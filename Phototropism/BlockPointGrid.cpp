/*
	BlockPointGrid.cpp
*/

#include <math.h>

#include "BlockPointGrid.h"
#include "Operators.h"
#include "MeshMaker.h"

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

std::size_t BlockPointGrid::findShiftedIndex(double coord, double halfGridSize, double unitSize) const {
	
	// Add halfGridSize to put the coordinate in the positive range. E.g. the range corresponding to grid element indices.
	double shiftedCoord = coord + halfGridSize;

	// To represent the grid as centered, we must account for a potential additional shift.  We do this by adding 1 to the index
	// if the remainder from truncating temp is greater than .5.
	double temp = shiftedCoord / unitSize;
	std::size_t ind = std::floor(temp);
	double remainder = temp - ind; 
	if (remainder >= .5) { ++ind; }

	return ind;
}

MStatus BlockPointGrid::addToUnitDensity(const BlockPoint &bp) {

	/*if (!this->checkRange_Point(bp.loc))
		return MS::kFailure;*/

	std::size_t xInd = findShiftedIndex(bp.loc.x, halfGridXSize, xUnitSize);
	std::size_t yInd = std::floor(bp.loc.y / yUnitSize);
	std::size_t zInd = findShiftedIndex(bp.loc.z, halfGridZSize, zUnitSize);

	if (!this->checkRange_Indices(xInd, yInd, zInd)) 
		return MS::kFailure;

	// Add the density of the BlockPoint to the Unit, up to a maximum of 1.
	grid[xInd][yInd][zInd].density = std::min(1., grid[xInd][yInd][zInd].density + bp.density);

	return MS::kSuccess;
}

CVect BlockPointGrid::chooseDirection(const Point &meriLoc, const CVect_m &meriDirection, const double coneRangeAngle) {

	CVect direction(0.,0.,0.);

	std::size_t xMinIndex = this->findShiftedIndex(std::max(meriLoc.x - detectionRange, -halfGridXSize), halfGridXSize, xUnitSize);
	std::size_t xMaxIndex = this->findShiftedIndex(std::min(meriLoc.x + detectionRange, halfGridXSize), halfGridXSize, xUnitSize);
	std::size_t yMinIndex = std::max(meriLoc.y - detectionRange, 0.);
	std::size_t yMaxIndex = std::min(meriLoc.y + detectionRange, ySize);
	std::size_t zMinIndex = this->findShiftedIndex(std::max(meriLoc.z - detectionRange, -halfGridZSize), halfGridZSize, zUnitSize);
	std::size_t zMaxIndex = this->findShiftedIndex(std::min(meriLoc.z + detectionRange, halfGridZSize), halfGridZSize, zUnitSize);

	for (int xI = xMinIndex; xI <= xMaxIndex; ++xI) {
		for (int yI = yMinIndex; yI <= yMaxIndex; ++yI) {
			for (int zI = zMinIndex; zI <= zMaxIndex; ++zI) {

				if (grid[xI][yI][zI].density > 0.) {

					CVect_m vectToUnit = grid[xI][yI][zI].center - meriLoc;

					double angBetween = findAngBetween(meriDirection, vectToUnit);

					if (angBetween < coneRangeAngle) {

						// If a block point is farther away it should have less influence on the direction
						// The length of the vector added to direction represents the strength of the shade produced by the block point
						// Here we adjust the size of the vector subtracted according to the shade strength
						double shadeStrength = 1. - (vectToUnit.mag / detectionRange);
						vectToUnit.resize(shadeStrength);
						direction -= vectToUnit;
					}
				}
			}
		}
	}

	return direction.resized(1.);
}

void BlockPointGrid::displayGrid() const {

	for (int xI = 0; xI < xElements; ++xI) {
		for (int yI = 0; yI < yElements; ++yI) {
			for (int zI = 0; zI < zElements; ++zI) {

				std::string unitName = "[" + std::to_string(xI) + "][" + std::to_string(yI) + "][" + std::to_string(zI) + "]" +
					"_Density: " + std::to_string(grid[xI][yI][zI].density);

				// Assuming the grid units are cubes, this works
				makeCube(grid[xI][yI][zI].center, xUnitSize, unitName);
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

bool BlockPointGrid::checkRange_Indices(int x, int y, int z) const {

	if (x >= grid.size() || x < 0) {

		MStreamUtils::stdOutStream() << "Error. x index outside of grid.\nAborting\n";
		return false;
	}
	else if (y >= grid[0].size() || y < 0) {

		MStreamUtils::stdOutStream() << "Error. y index outside of grid.\nAborting\n";
		return false;
	}
	else if (z >= grid[0][0].size() || z < 0) {

		MStreamUtils::stdOutStream() << "Error. z index outside of grid.\nAborting\n";
		return false;
	}

	return true;
}