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

namespace {

	// Takes a linked list, starting at rootSeg, and converts it into a list of BranchMeshes
	std::vector<BranchMesh*> makeManyBMesh(Segment *rootSeg);

	// Delivers the data from each BranchMesh to the MFnMesh create() method
	void sendMeshesToMaya(std::vector<BranchMesh*> treeMesh);
}

// When the testGrow command is executed, this method is called
MStatus TestGrowCommand::doIt(const MArgList &argList) {

	MStatus status;

	MArgDatabase argData(syntax(), argList, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	BlockPointGrid bpg(3., 3., 3., .2, .2, .2, 4.);

	// Create a bunch of block points and add them to the grid
	std::size_t totalBlockPoints = 13;
	for (int i = 0; i < totalBlockPoints; ++i) {

		Point randP = randPoint(-1.5, 1.5, 0., 3., -1.5, 1.5);
		bpg.addBlockPoint(BlockPoint(randP, randBetween(0.,1.)));
	}

	// Create one segment to represent the current direction of the meristem, and another to represent the chosen direction
	Meristem *meri = new Meristem(.01, 8);
	Segment *meriDirectionSeg = new Segment(sphAnglesToCartVect({ 0.,0. }, .3), { 0.,0.,0. }, .05, meri);
	Segment *meriChosenDirectionseg = new Segment(bpg.chooseDirection(meriDirectionSeg->getEndPoint(), meriDirectionSeg->getVect(), MM::PI),
		meriDirectionSeg->getEndPoint(), .05, meri);
	std::vector<BranchMesh*> meriDirection = makeManyBMesh(meriDirectionSeg);
	sendMeshesToMaya(meriDirection);
	std::vector<BranchMesh*> meriNewDirection = makeManyBMesh(meriChosenDirectionseg);
	sendMeshesToMaya(meriNewDirection);
	
	bpg.displayBlockPoints();

	delete meri;
	delete meriDirectionSeg;
	delete meriChosenDirectionseg;

	return MS::kSuccess;
}

namespace {

	std::vector<BranchMesh*> makeManyBMesh(Segment *rootSeg) {

		std::vector<BranchMesh*> treeMesh;

		std::queue<Segment*> firstSegsOfNewBMeshes;
		firstSegsOfNewBMeshes.push(rootSeg);

		// Each iteration declares a BranchMesh and the go() method completes it.  
		// The go() method also finds any segments that mark the beginning of what will be a new BranchMesh, and adds them to the queue
		while (!firstSegsOfNewBMeshes.empty()) {

			const int orderSides = firstSegsOfNewBMeshes.front()->getMeri()->sides;
			BranchMesh *bMesh = new BranchMesh(firstSegsOfNewBMeshes.front(), orderSides);
			treeMesh.push_back(bMesh);
			std::vector<double> initialPreadjusts(orderSides, 0.);
			bMesh->go(firstSegsOfNewBMeshes.front(), initialPreadjusts, firstSegsOfNewBMeshes);
			firstSegsOfNewBMeshes.pop();
		}

		return treeMesh;
	}

	void sendMeshesToMaya(std::vector<BranchMesh*> treeMesh) {

		int meshNumber = 0;
		// deliver the relevant data in each bMesh to the MFnMesh create() method
		for (auto bMesh : treeMesh)
		{
			++meshNumber;

			MFloatPointArray fpaVertices;
			for (int i = 0; i < bMesh->numVerts(); i++)
			{
				Point vert = bMesh->getVert(i);
				fpaVertices.append(MFloatPoint(vert.x, vert.y, vert.z));
			}

			MIntArray iaUVCounts;
			MIntArray iaFaceCounts;
			for (int i = 0; i < bMesh->numFaces(); i++)
			{
				iaFaceCounts.append(bMesh->getFaceCount(i));
				iaUVCounts.append(bMesh->getFaceCount(i));
			}

			MFloatArray faU;
			MFloatArray faV;

			//bMesh->reportInMaya();

			bMesh->calculateUVs();

			if (bMesh->numUs() != bMesh->numFaceConnects()) { std::cout << "NUMBER OF U's != NUMBER OF FACECONNECTS" << std::endl; }
			if (bMesh->numVs() != bMesh->numFaceConnects()) { std::cout << "NUMBER OF V's != NUMBER OF FACECONNECTS" << std::endl; }

			MIntArray iaUVIDs;
			MIntArray iaFaceConnects;
			for (int i = 0; i < bMesh->numFaceConnects(); i++)
			{
				iaFaceConnects.append(bMesh->getFaceConnect(i));
				iaUVIDs.append(i);
				faU.append(bMesh->getU(i));
				faV.append(bMesh->getV(i));
			}

			MFnMesh fnMesh;
			MObject newTransform = fnMesh.create(bMesh->numVerts(), bMesh->numFaces(), fpaVertices,
				iaFaceCounts, iaFaceConnects, faU, faV);
			fnMesh.assignUVs(iaUVCounts, iaUVIDs);

			// Give our object a name

			MFnDependencyNode nodeFn;
			nodeFn.setObject(newTransform);

			// Create an MString and pass it to a dependency node's setName()
			std::string groupName_str = "BranchMesh_" + std::to_string(meshNumber);
			char groupName_c[32];
			strcpy(groupName_c, groupName_str.c_str());
			MString groupName = groupName_c;
			nodeFn.setName(groupName);
		}
	}
}