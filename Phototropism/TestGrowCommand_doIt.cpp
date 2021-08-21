/*
	TestGrowCommand_doIt.cpp

	Defines doIt() method of TestGrowCommand

	doIt() is the method called when the user executes the command "testGrow"
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

#include "TestGrowCommand.h"
#include "BranchMesh.h"
#include "PhotMath.h"
#include "BlockPointGrid.h"
#include "MeshMaker.h"

// When the testGrow command is executed, this method is called
MStatus TestGrowCommand::doIt(const MArgList &argList) {

	MStatus status;

	MArgDatabase argData(syntax(), argList, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	BlockPointGrid bpg(1.2, 1.2, 1.2, .4, .4, .4, 5.);

	Segment meriDirectionSeg(sphAnglesToCartVect({ 0.,0. }, .3), { 0.,0.,0. }, .05);
	//bpg.displayGrid();

	return MS::kSuccess;
}