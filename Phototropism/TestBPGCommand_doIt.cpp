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
#include "MeshMaker.h"

#include "BlockPointGrid.h"

// When the testBPG() command is executed, this method is called
MStatus TestBPGCommand::doIt(const MArgList &argList) {

	MStatus status;

	MArgDatabase argData(syntax(), argList, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	double gridSize = 3.25;
	BlockPointGrid bpg(gridSize, gridSize, gridSize, .25, 2.4, (MM::PI / 4.), .1);

	// Create a bunch of block points at random locations clustered around another random point, and add them to the grid
	// ...being careful to keep all points on the grid
	double clusterRadius = std::min(1., (gridSize/8.));
	double clusterRange = (gridSize / 2.) - clusterRadius;
	Point clusterPoint = randPoint(-clusterRange, clusterRange, clusterRadius, gridSize - clusterRadius, -clusterRange, clusterRange);
	std::size_t totalBlockPoints = 50;
	for (int i = 0; i < totalBlockPoints; ++i) {

		Point randP = randPoint(clusterPoint.x - clusterRadius, clusterPoint.x + clusterRadius, 
								clusterPoint.y - clusterRadius, clusterPoint.y + clusterRadius,
								clusterPoint.z - clusterRadius, clusterPoint.z + clusterRadius);
		BlockPoint *dummyPtr;
		bpg.addBlockPoint(randP, .7, dummyPtr);
	}

	bpg.displayGridBorder();
	bpg.displayGrid();
	bpg.displayBlockPoints();

	return MS::kSuccess;
}