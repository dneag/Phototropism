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

// Creates the meshes within Maya
// When the testGrow command is executed, this method is called
// argList will contain all the information in the flags
MStatus TestGrowCommand::doIt(const MArgList &argList) {

	MStatus status;

	MArgDatabase argData(syntax(), argList, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	return MS::kSuccess;
}