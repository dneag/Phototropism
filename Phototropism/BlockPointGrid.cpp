/*
BlockPointGrid.cpp
*/

#include <math.h>

#include "BlockPointGrid.h"
#include "operators.h"
#include "MeshMaker.h"

void BlockPointGrid::setIndexVectorsAndMaximums() {

	int maxIndexDiff = detectionRange / unitSize;
	int xzIndexBound = (maxIndexDiff * 2) + 1; // to ensure that the top center unit is indeed centered, make xzIndexBound odd 

	// topCenterUnit is really not a vector, just makes sense to use IndexVector since it stores int coords
	IndexVector topCenterUnit(xzIndexBound / 2, maxIndexDiff, xzIndexBound / 2, CVect(0., 0., 0., 0.), 0.);

	for (int xI = 0; xI < xzIndexBound; ++xI) {
		for (int yI = 0; yI < maxIndexDiff; ++yI) { // note that no units at the same y index as topCenterUnit will be affected
			for (int zI = 0; zI < xzIndexBound; ++zI) {

				int xIDist = xI - topCenterUnit.x;
				int yIDist = yI - topCenterUnit.y;
				int zIDist = zI - topCenterUnit.z;
				double indexDistanceToUnit = std::sqrt(xIDist * xIDist + yIDist * yIDist + zIDist * zIDist);

				if (indexDistanceToUnit >= maxIndexDiff)
					continue;

				// trueVectToUnit is resized to match the true size of the grid
				CVect trueVectToUnit = CVect(xIDist, yIDist, zIDist, indexDistanceToUnit).resized(indexDistanceToUnit * unitSize);


				if (findAngBetween(trueVectToUnit, CVect(0., -1., 0., 1.)) > coneRangeAngle)
					continue;

				// Feels a little awkward doing this here, but it so happens that this should work.  That is, we will calculate the maximum
				// blockage and maximum light vector here since we are already looping through the correct range of grid units
				// What we are adding to maximumBlockage here is the same as blockageStrength times grid[xI][yI][zI].density, since
				// we are finding the maximum, all densities are 1. so no need to multiply

				double blockageStrength = 1. - (trueVectToUnit.getMag() / detectionRange);
				blockageStrength = trunc4(blockageStrength);
				maximumBlockage += blockageStrength;
				CVect blockageVector = trueVectToUnit.resized(blockageStrength);
				maximumLightVector += blockageVector;

				IndexVector indexVectToUnit(xIDist, yIDist, zIDist, blockageVector, blockageStrength);
				indexVectorsToUnitsInCone.push_back(indexVectToUnit);
			}
		}
	}

	// since vectors to units are calculated going downward in the above loops, flip the light vector
	maximumLightVector.y *= -1.;

	maximumLightVector.x = trunc4(maximumLightVector.x);
	maximumLightVector.y = trunc4(maximumLightVector.y);
	maximumLightVector.z = trunc4(maximumLightVector.z);

	// Resize all index vectors' trueVects to account for intensity
	for (auto &v : indexVectorsToUnitsInCone) 
		v.blockageVect.resize(v.blockageVect.getMag() * maximumLightVector.getMag() * intensity);
}

void BlockPointGrid::initiateGrid() {

	double xCoord = 0. - halfGridXSize + (unitSize*.5);

	for (int xI = 0; xI < xElements; ++xI) {

		double yCoord = unitSize / 2.;

		grid.push_back(std::vector< std::vector<Unit> >());

		for (int yI = 0; yI < yElements; ++yI) {

			double zCoord = 0. - halfGridZSize + (unitSize*.5);

			grid.back().push_back(std::vector<Unit>());

			for (int zI = 0; zI < zElements; ++zI) {

				grid.back().back().push_back(Unit(xCoord, yCoord, zCoord, maximumLightVector.x, maximumLightVector.y, maximumLightVector.z));

				zCoord += unitSize;
			}

			yCoord += unitSize;
		}

		xCoord += unitSize;
	}
}

BlockPointGrid::BlockPointGrid(double XSIZE, double YSIZE, double ZSIZE, double UNITSIZE, double DETECTIONRANGE, double CONERANGEANGLE,
							   double INTENSITY) {

	unitSize = UNITSIZE;

	// Note to self: after dividing two doubles that divide evenly in reality, the result is represented internally as
	// ~ .00000000001 less than its integer counterpart.  So truncating will effectively reduce by 1.  Thus the ceil here.

	xElements = std::ceil(XSIZE / UNITSIZE);
	yElements = std::ceil(YSIZE / UNITSIZE);
	zElements = std::ceil(ZSIZE / UNITSIZE);
	halfGridXSize = unitSize * (xElements / 2.);
	halfGridZSize = unitSize * (zElements / 2.);
	detectionRange = DETECTIONRANGE;
	coneRangeAngle = CONERANGEANGLE;
	intensity = INTENSITY;
	this->setIndexVectorsAndMaximums();
	this->initiateGrid();
}

void BlockPointGrid::displayGrid() const {

	for (int xI = 0; xI < xElements; ++xI) {

		for (int yI = 0; yI < yElements; ++yI) {

			for (int zI = 0; zI < zElements; ++zI) {

				std::string unitName = "[" + std::to_string(xI) + "][" + std::to_string(yI) + "][" + std::to_string(zI) + "]"; +
					"_Density: " + std::to_string(grid[xI][yI][zI].density);

				//this->displayUnitLightDirection(xI, yI, zI);
				this->displayAffectedUnitLightDirection(xI, yI, zI);
				//this->displayUnitBlockage(xI, yI, zI);
				//this->displayUnitDensity(xI, yI, zI);

				//makeCube(grid[xI][yI][zI].center, unitSize, unitName);
			}
		}
	}
}

void BlockPointGrid::displayGridBorder() const {

	// assumes cubic grid
	makeCube(Point(0., (yElements * unitSize) / 2., 0.), (xElements * unitSize), "gridBorder"); 
}

void BlockPointGrid::displayUnitsAffectedByUnit(int uX, int uY, int uZ) const {

	for (auto indexVect : indexVectorsToUnitsInCone) {

		int X = uX + indexVect.x;
		int Y = uY + indexVect.y;
		int Z = uZ + indexVect.z;

		if (this->indicesAreInRange(X, Y, Z)) {

			makeCube(grid[X][Y][Z].center, unitSize, "unit");
		}
	}
}

void BlockPointGrid::displayUnitsAffectedByBP(const BlockPoint *bp) const {

	std::size_t xInd = (bp->loc.x + halfGridXSize) / unitSize;
	std::size_t yInd = bp->loc.y / unitSize;
	std::size_t zInd = (bp->loc.z + halfGridZSize) / unitSize;

	displayUnitDensity(xInd, yInd, zInd);

	for (auto indexVect : indexVectorsToUnitsInCone) {

		int X = xInd + indexVect.x;
		int Y = yInd + indexVect.y;
		int Z = zInd + indexVect.z;

		if (this->indicesAreInRange(X, Y, Z)) {

			displayUnitBlockage(X, Y, Z);
			//displayUnitDensity(X, Y, Z);
			displayUnitLightDirection(X, Y, Z);
		}
	}
}

void BlockPointGrid::displayUnitLightDirection(int uX, int uY, int uZ) const {

	// make it so that the arrow's length represents the magnitude of the lightDirection vector, but scaled down so that
	// the maximum length is equal to unitSize
	double arrowLength = unitSize * (grid[uX][uY][uZ].lightDirection.getMag() / maximumLightVector.getMag());


	makeArrow(grid[uX][uY][uZ].center + Point(0., -unitSize*.5, 0.), CVect(grid[uX][uY][uZ].lightDirection).resized(arrowLength),
		"lightDirectionArrow", .01);

}

void BlockPointGrid::displayAffectedUnitLightDirection(int uX, int uY, int uZ) const {

	//only makeArrow() if the unit has some blockage
	double percentBlocked = grid[uX][uY][uZ].blockage / maximumBlockage;
	if (percentBlocked > 0.) {

		double arrowLength = unitSize * (1. - percentBlocked);
		//double arrowLength = unitSize * (grid[uX][uY][uZ].lightDirection.getMag() / maximumLightVector.getMag());
		//MStreamUtils::stdOutStream() << "arrowLength = " << arrowLength << ", unitSize = " << unitSize << ", percentBlocked = " << percentBlocked << "\n";
		// size the arrow so that it represents the light strength of the unit 
		//arrowLength = arrowLength * (1. - grid[uX][uY][uZ].blockage);

		makeArrow(grid[uX][uY][uZ].center + Point(0., -unitSize*.5, 0.), CVect(grid[uX][uY][uZ].lightDirection).resized(arrowLength),
			"lightDirectionArrow", .01);
	}
}

void BlockPointGrid::displayUnitDensity(int uX, int uY, int uZ) const {

	if (grid[uX][uY][uZ].density == 0.)
		return;

	// a unit with maximum density will show an arrow the same size as itself
	double arrowLength = unitSize * std::min(grid[uX][uY][uZ].density, 1.);
	Point arrowStart = grid[uX][uY][uZ].center + Point(-unitSize*.25, -unitSize*.5, 0.);
	makeArrow(arrowStart, CVect(0., arrowLength, 0., arrowLength), "densityArrow", .01);

}

void BlockPointGrid::displayUnitBlockage(int uX, int uY, int uZ) const {

	if (grid[uX][uY][uZ].blockage == 0.)
		return;

	// a unit with maximum blockage will show an arrow the same size as itself
	double arrowLength = unitSize * (grid[uX][uY][uZ].blockage / maximumBlockage);
	Point arrowStart = grid[uX][uY][uZ].center + Point(unitSize*.25, -unitSize*.5, 0.);
	makeArrow(arrowStart, CVect(0., arrowLength, 0., arrowLength), "blockageArrow", .01);
}

void BlockPointGrid::displayBlockPoints() const {

	for (auto bp : bps) {
		int density = bp->density * 1000.;
		std::string name = "Density--0." + std::to_string(density);
		makeSphere(bp->loc, .05, name);
	}
}

void BlockPointGrid::displayAll() const {

	this->displayBlockPoints();

	for (int xI = 0; xI < xElements; ++xI) {

		for (int yI = 0; yI < yElements; ++yI) {

			for (int zI = 0; zI < zElements; ++zI) {

				displayUnitLightDirection(xI, yI, zI);
				displayUnitBlockage(xI, yI, zI);
				displayUnitDensity(xI, yI, zI);
			}
		}
	}
}

MStatus BlockPointGrid::addBlockPoint(const Point loc, double bpDensity, BlockPoint *&ptrForSeg) {

	std::size_t xInd = (loc.x + halfGridXSize) / unitSize;
	std::size_t yInd = loc.y / unitSize;
	std::size_t zInd = (loc.z + halfGridZSize) / unitSize;
	//MStreamUtils::stdOutStream() << "Adding bp with loc: " << loc << " and indices: " << xInd << ", " << yInd << ", " << zInd << "\n";
	if (!this->indicesAreInRange_showError(xInd, yInd, zInd))
		return MS::kFailure;

	BlockPoint *newBP = new BlockPoint(loc, bpDensity, xInd, yInd, zInd);
	bps.push_back(newBP);
	ptrForSeg = newBP;

	this->adjustGrid(newBP, add);

	return MS::kSuccess;
}

MStatus BlockPointGrid::moveBlockPoint(BlockPoint *bp, const Point newLoc) {

	// Calculate the BlockPoint's new indices on the bpg
	std::size_t xInd = (newLoc.x + halfGridXSize) / unitSize;
	std::size_t yInd = newLoc.y / unitSize;
	std::size_t zInd = (newLoc.z + halfGridZSize) / unitSize;

	if (!this->indicesAreInRange_showError(xInd, yInd, zInd))
		return MS::kFailure;

	if (xInd != bp->gridX || yInd != bp->gridY || zInd != bp->gridZ) {

		this->adjustGrid(bp, subtract);
		bp->changeGridUnit(xInd, yInd, zInd);
		this->adjustGrid(bp, add);
	}

	// set the new location for the block point
	bp->loc = newLoc;

	return MS::kSuccess;
}

void BlockPointGrid::adjustGrid(const BlockPoint *bp, const adjustment adj) {

	// If adj is add, bp->density will be multiplied by 1.  If s is subtract, bp->density will be multiplied by -1
	double densityAdjustment = bp->density * adj;

	double startingUnitDensity = std::min(grid[bp->gridX][bp->gridY][bp->gridZ].density, 1.);
	grid[bp->gridX][bp->gridY][bp->gridZ].density += densityAdjustment;
	double currentUnitDensity = std::min(grid[bp->gridX][bp->gridY][bp->gridZ].density, 1.);
	double densityChange = currentUnitDensity - startingUnitDensity;
	double maxLVMag = maximumLightVector.getMag();
	
	if (densityChange != 0.) {

		for (const auto &indexVect : indexVectorsToUnitsInCone) {

			int X = bp->gridX + indexVect.x;
			int Y = bp->gridY + indexVect.y;
			int Z = bp->gridZ + indexVect.z;

			if (this->indicesAreInRange(X, Y, Z)) {

				//MStreamUtils::stdOutStream() << "X Y Z: " << X << ", " << Y << ", " << Z << ", vect size: " << indexVect.blockageVect.getMag() << "\n";
				grid[X][Y][Z].lightDirection += indexVect.blockageVect.resized(indexVect.blockageVect.getMag() * densityChange);
				grid[X][Y][Z].lightDirection.resize(maxLVMag);
				grid[X][Y][Z].blockage += indexVect.blockageStrength * densityChange;
			}
		}
	}
}

MStatus BlockPointGrid::getDirectionAndBlockage(const Point &meriLoc, CVect &chosenDirection, double &blockage) const {

	std::size_t xInd = (meriLoc.x + halfGridXSize) / unitSize;
	std::size_t yInd = meriLoc.y / unitSize;
	std::size_t zInd = (meriLoc.z + halfGridZSize) / unitSize;

	if (!this->indicesAreInRange_showError(xInd, yInd, zInd))
		return MS::kFailure;

	this->displayUnitLightDirection(xInd, yInd, zInd);
	//this->displayUnitBlockage(xInd, yInd, zInd);
	// chosenDirection should have a magnitude of 1, so we need to resize lightDirection here
	chosenDirection = CVect(grid[xInd][yInd][zInd].lightDirection).resized(1.);
	blockage = grid[xInd][yInd][zInd].blockage / maximumBlockage;

	return MS::kSuccess;
}

bool BlockPointGrid::indicesAreInRange(int x, int y, int z) const {

	if (x >= xElements || x < 0)
		return false;
	else if (y >= yElements || y < 0)
		return false;
	else if (z >= zElements || z < 0)
		return false;

	return true;
}

bool BlockPointGrid::indicesAreInRange_showError(int x, int y, int z) const {

	if (x >= xElements || x < 0) {

		MStreamUtils::stdOutStream() << "Error. x index outside of grid.\nAborting\n";
		return false;
	}
	else if (y >= yElements || y < 0) {

		MStreamUtils::stdOutStream() << "Error. y index outside of grid.\nAborting\n";
		return false;
	}
	else if (z >= zElements || z < 0) {

		MStreamUtils::stdOutStream() << "Error. z index outside of grid.\nAborting\n";
		return false;
	}

	return true;
}

void BlockPointGrid::addBlockPointsThroughGridLevels(int xMin, int xMax, int yMin, int yMax, int zMin, int zMax) {

	if (!indicesAreInRange(xMin, yMin, zMin) || !indicesAreInRange(xMax, yMax, zMax)) {
		MStreamUtils::stdOutStream() << "Indices are out of range for adding block points\n";
		return;
	}

	for (int xI = xMin; xI <= xMax; ++xI) {

		for (int yI = yMin; yI <= yMax; ++yI) {

			for (int zI = zMin; zI <= zMax; ++zI) {

				BlockPoint *dummyPtr;
				this->addBlockPoint(grid[xI][yI][zI].center, 1., dummyPtr);
			}
		}
	}
}