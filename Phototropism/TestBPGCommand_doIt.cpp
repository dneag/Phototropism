/*
	TestGrowCommand_doIt.cpp

	Defines doIt() method of TestBPGCommand

	doIt() is the method called when the user executes the command "testBPG"
*/

#include <iostream>
#include <stdlib.h>
#include <string>

#include <maya/MStreamUtils.h>
#include <maya/MArgDatabase.h>
#include <maya/MArgList.h>
#include <maya/MSyntax.h>
#include <maya/MFnDagNode.h>
#include <maya/MFloatPointArray.h>
#include <maya/MFloatPoint.h>
#include <maya/MFloatArray.h>
#include <maya/MIntArray.h>
#include <maya/MFnMesh.h>

#include "TestBPGCommand.h"
#include "PhotMath.h"
#include "BlockPointGrid.h"
#include "MeshMaker.h"

// When the testBPG command is executed, this method is called
MStatus TestBPGCommand::doIt(const MArgList &argList) {

	MStreamUtils::stdOutStream() << "\nBEGINNING TESTBPG()\n\n";

	MStatus status;

	MArgDatabase argData(syntax(), argList, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);


	double gridSize = 3.25;
	BlockPointGrid bpg(gridSize, gridSize, gridSize, .25, 2.4, (MM::PI / 8.), .5);
	bpg.displayGridBorder();
	//bpg.displayGrid();
	//bpg.displayUnitsAffectedByUnit(4, 10, 4);
	BlockPoint *bp;
	bpg.addBlockPoint(Point(-.45, 2.7, -.45), 1., bp);
	bpg.displayUnitsAffectedByBP(bp);

	//// Create a bunch of block points at random locations clustered around another random point, and add them to the grid
	//// ...being careful to keep all points on the grid
	//double clusterRadius = std::min(1., (gridSize/2.));
	//double clusterRange = (gridSize / 2.) - clusterRadius;
	//Point clusterPoint = randPoint(-clusterRange, clusterRange, clusterRadius, gridSize - clusterRadius, -clusterRange, clusterRange);
	//std::size_t totalBlockPoints = 30;
	//for (int i = 0; i < totalBlockPoints; ++i) {

	//	Point randP = randPoint(clusterPoint.x - clusterRadius, clusterPoint.x + clusterRadius, 
	//							clusterPoint.y - clusterRadius, clusterPoint.y + clusterRadius,
	//							clusterPoint.z - clusterRadius, clusterPoint.z + clusterRadius);
	//	BlockPoint *dummyPtr;
	//	bpg.addBlockPoint(randP, 1., dummyPtr);
	//}

	//// Make arrow meshes representing the direction that would be chosen at random points on the grid
	//// The chosenDirection vector should correspond to the lightDirection vector of the Unit in which meriLoc falls
	//std::size_t totalMeris = 13;
	//for (int i = 0; i < totalMeris; ++i) {

	//	Point meriLoc = randPoint(-(gridSize/2.), (gridSize / 2.), .01, gridSize, -(gridSize / 2.), (gridSize / 2.));
	//	CVect chosenDirection(0., 0., 0., 0.);
	//	double blockage = 0;
	//	bpg.getDirectionAndBlockage(meriLoc, chosenDirection, blockage);

	//	makeArrow(meriLoc, chosenDirection, "chosenDirectionArrow", .01);
	//}

	bpg.displayBlockPoints();

	return MS::kSuccess;
}