/*
BlockPointGrid.h

This system is designed to simulate a 3D space in which physical objects block the light from above, influencing
the direction and vigor with which any trees below them tend to grow.

A BlockPointGrid represents a 3D grid made of cubic units.  Each unit stores a vector that represents the direction towards the greatest
amount of light.  Each unit also stores a double representing how full it is of BlockPoints.  BlockPoints can be placed on the grid.  When a BlockPoint
is placed, it increases the density of the unit it falls in by an amount equal to its density.  In turn, the unit will adjust the light vectors of
any affected (blocked) units.  Blocked units are those that fall in a spherical sector below the unit with the BlockPoint.  The size of the sector
is determined by the members, coneRangeAngle and detectionRange.
*/

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
	int gridX;
	int gridY;
	int gridZ;

	BlockPoint(const Point &LOC, double DENSITY, int GX, int GY, int GZ) : loc(LOC), density(DENSITY), gridX(GX), gridY(GY), gridZ(GZ) {}

	void changeGridUnit(int GX, int GY, int GZ) {

		gridX = GX;
		gridY = GY;
		gridZ = GZ;
	}
};

class BlockPointGrid {

	struct Unit {

		// A vector indicating the direction towards the most light
		Point lightDirection;

		// A percentage indicating how much light the unit is blocking.  1 means it is blocking 100%, 0 means 0%.
		/* A unit's density is the sum of the densities of all block points in it.  While it can hold any number, it is effectively never
		   greater than one nor less than zero.  This is enforced in addBlockPoint() and moveBlockPoint().  The reason we don't clamp the
		   values between 1 and 0 when storing in this variable is that we need to be able to subtract block points' densities in the future,
		   which, if density were capped at 1, would lead to negative/inaccurate density values since there can be many block points in a unit.
		   Another way to handle this would be to store a separate variable for the full density value */
		double density = 0.;

		// A percentage indicating how much light is blocked from the unit.  At 1, any meristems in the unit receive no energy from light.
		double blockage = 0.;

		Point center;

		Unit(double cX, double cY, double cZ, double ldX, double ldY, double ldZ) {

			center.x = cX;
			center.y = cY;
			center.z = cZ;
			lightDirection.x = ldX;
			lightDirection.y = ldY;
			lightDirection.z = ldZ;
		}
	};

	struct IndexVector {

		int x;
		int y;
		int z;

		// trueVect has the same direction as (x,y,z) but it is resized once to match its true size on the grid, and then once
		// more to account for its distance between detection range (see setIndexVectorsAndMaximums)
		CVect blockageVect = { 0.,0.,0.,0. };

		double blockageStrength;

		IndexVector(int X, int Y, int Z, const CVect &TRUEVECT, double BLOCKAGESTRENGTH) : x(X), y(Y), z(Z),
			blockageVect(TRUEVECT), blockageStrength(BLOCKAGESTRENGTH) {}
	};

	enum adjustment { add = 1, subtract = -1 };

	// We want the grid to be represented as centered on the Maya grid.  This means that x and z elements must always be an odd
	// number.  E.g. xSize / xUnitSize is always an odd number.  Also, this means that the center element itself is centered on
	// the Maya grid.  E.g. the x and z coordinates at the center of the center element are 0. and 0.
	std::vector< std::vector< std::vector<Unit> > > grid;

	double unitSize;
	int xElements;
	int yElements;
	int zElements;
	double halfGridXSize;
	double halfGridZSize;

	// The range through which BlockPoints are effective
	double detectionRange;

	// coneRangeAngle is the angle between straight down and the border of the cone in which BlockPoints affect units
	double coneRangeAngle = MM::PI / 3.;

	// The strength of the effect that BlockPoints will have on units' lightDirection
	// This value must be proportional to the number of units within the conical section - with coneRangeAngle as its angle - of the sphere
	// with detectionRange as its radius. See setIndexVectorsAndMaximums() for how it is used
	double intensity;

	// When updating the grid, these index vectors will be added to the indices of the unit holding a newly placed or moved block point in order 
	// to access each unit affected by the block point.  That is, each index vector points to one of the units within the cone effected by the block point
	std::vector<IndexVector> indexVectorsToUnitsInCone;

	// maximumBlockage represents the total number of units within detectionRange at full density
	double maximumBlockage = 0.;
	Point maximumLightVector = { 0.,0.,0. };

	std::vector<BlockPoint*> bps;

	void initiateGrid();

	void setIndexVectorsAndMaximums();

	// Applies the bp's effect to the grid
	// The s paramater indicates whether the effect of the bp is being added or subtracted from the grid.  A value of add will
	// add, while a value of subtract will subtract
	void adjustGrid(const BlockPoint *bp, const adjustment adj);

	// Checks that each index is within the range of the grid
	bool indicesAreInRange(int x, int y, int z) const;

	// Checks that each index is within the range of the grid and output an error message if not
	bool indicesAreInRange_showError(int x, int y, int z) const;

public:

	// All units are assumed to be cubes
	// XSIZE, YSIZE, and ZSIZE should divide evenly by UNITSIZE
	BlockPointGrid(double XSIZE, double YSIZE, double ZSIZE, double UNITSIZE, double DETECTIONRANGE, double CONERANGEANGLE, double INTENSITY);

	void displayGrid() const;

	void displayGridBorder() const;

	// Arguments are the indices of the unit doing the affecting
	void displayUnitsAffectedByUnit(int uX, int uY, int uZ) const;

	void displayUnitsAffectedByBP(const BlockPoint *bp) const;

	void displayUnitLightDirection(int uX, int uY, int uZ) const;

	void displayAffectedUnitLightDirection(int uX, int uY, int uZ) const;

	void displayUnitDensity(int uX, int uY, int uZ) const;

	void displayUnitBlockage(int uX, int uY, int uZ) const;

	void displayBlockPoints() const;

	void displayAll() const;

	// Creates a new BlockPoint and adjusts any affected units.  
	// The pointer reference is for Segments' pointers to their BlockPoints - they are the only handles to BlockPoints that exist
	// outside of the BlockPointGrid
	MStatus addBlockPoint(const Point loc, double bpDensity, BlockPoint *&ptrForSeg);

	// Moves the passed BlockPoint to the new location.  Subtracts its effects from previously affected units and adds its effects to newly affected ones
	MStatus moveBlockPoint(BlockPoint *bp, const Point newLoc);

	// Gives the chosen direction and blockage for a meristem depending on its current direction and location
	MStatus getDirectionAndBlockage(const Point &meriLoc, CVect &chosenDirection, double &blockage) const;

	// For testing purposes
	// Adds a block point to every unit between and including the indices
	void addBlockPointsThroughGridLevels(int xMin, int xMax, int yMin, int yMax, int zMin, int zMax);
};

#endif /* BlockPointGrid_h */