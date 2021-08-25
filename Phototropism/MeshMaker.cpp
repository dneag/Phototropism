/*
	MeshMaker.cpp

	Functions for making simple shapes in Maya
*/

#include "MeshMaker.h"

void makeCube(const Point &location, double width, std::string name) {

	double halfWidth = width / 2.;

	// Create all vertex points and add them to the list
	std::vector<Point> vertLocs;
	vertLocs.push_back({ location.x - halfWidth, location.y - halfWidth, location.z - halfWidth });
	vertLocs.push_back({ location.x - halfWidth, location.y - halfWidth, location.z + halfWidth });
	vertLocs.push_back({ location.x + halfWidth, location.y - halfWidth, location.z + halfWidth });
	vertLocs.push_back({ location.x + halfWidth, location.y - halfWidth, location.z - halfWidth });
	vertLocs.push_back({ location.x - halfWidth, location.y + halfWidth, location.z - halfWidth });
	vertLocs.push_back({ location.x - halfWidth, location.y + halfWidth, location.z + halfWidth });
	vertLocs.push_back({ location.x + halfWidth, location.y + halfWidth, location.z + halfWidth });
	vertLocs.push_back({ location.x + halfWidth, location.y + halfWidth, location.z - halfWidth });

	// Add the vertices to the Maya array
	MFloatPointArray cubeVertLocs;
	for (int i = 0; i<8; i++)
		cubeVertLocs.append(MFloatPoint(vertLocs[i].x, vertLocs[i].y, vertLocs[i].z));

	// A cube always has 6 faces and each face is always a quad
	MIntArray iaCubeFaceCounts;
	for (int i = 0; i<6; i++)
		iaCubeFaceCounts.append(4);

	MIntArray iaCubeFaceConnects;

	// Add faceConnects for the bottom face
	iaCubeFaceConnects.append(0);
	iaCubeFaceConnects.append(3);
	iaCubeFaceConnects.append(2);
	iaCubeFaceConnects.append(1);

	// Add faceConnects for 3 of the middle faces
	for (int i = 0; i < 3; i++) {

		iaCubeFaceConnects.append(i);
		iaCubeFaceConnects.append(i + 1);
		iaCubeFaceConnects.append(i + 4 + 1);
		iaCubeFaceConnects.append(i + 4);
	}

	// Add faceConnects for the last middle face
	iaCubeFaceConnects.append(3);
	iaCubeFaceConnects.append(0);
	iaCubeFaceConnects.append(4);
	iaCubeFaceConnects.append(7);

	// Add faceConnects for the top face
	iaCubeFaceConnects.append(4);
	iaCubeFaceConnects.append(5);
	iaCubeFaceConnects.append(6);
	iaCubeFaceConnects.append(7);

	MFnMesh fnCube;
	MObject cubeTransform = fnCube.create(8, 6, cubeVertLocs, iaCubeFaceCounts, iaCubeFaceConnects);

	// Give our object a name

	MFnDependencyNode nodeFn;
	nodeFn.setObject(cubeTransform);

	// Create an MString and pass it to a dependency node's setName()
	std::string cubeName = name;
	char cubeName_c[32];
	strcpy(cubeName_c, cubeName.c_str());
	MString MCubeName = cubeName_c;
	nodeFn.setName(MCubeName);
}

void makeSphere(const Point &location, double radius, std::string name)
{
	int axisDivisions = 8;
	int heightDivisions = 8;
	int sphereDivisions = axisDivisions - 1;
	int sphereSides = heightDivisions;
	double sphereRadius = radius;
	int numSphereVerts = (sphereDivisions * sphereSides) + 2;
	int numSphereFaces = (sphereDivisions + 1) * sphereSides;
	std::vector<Point> sphereVertList(numSphereVerts);
	sphereVertList[0].x = location.x;
	sphereVertList[0].y = location.y - sphereRadius;
	sphereVertList[0].z = location.z;
	int vertIndex = 1;

	double azi = MM::PI;
	double polarIncrement = MM::PIM2 / sphereSides;
	double aziIncrement = MM::PI / (sphereDivisions + 1);

	for (int i = 0; i<sphereDivisions; i++) {

		double pol = 0.;
		azi -= aziIncrement;
		for (int j = 0; j<sphereSides; j++)
		{
			double lengthTimesSinAzi = sphereRadius * std::sin(azi);

			sphereVertList[vertIndex].x = location.x + (lengthTimesSinAzi * std::cos(pol));
			sphereVertList[vertIndex].y = location.y + (sphereRadius * std::cos(azi));
			sphereVertList[vertIndex].z = location.z + (lengthTimesSinAzi * std::sin(pol));
			pol -= polarIncrement;
			vertIndex++;
		}
	}

	sphereVertList[vertIndex].x = location.x;
	sphereVertList[vertIndex].y = location.y + sphereRadius;
	sphereVertList[vertIndex].z = location.z;

	int lastVertIndex = vertIndex;

	MFloatPointArray sphereVertLocs;
	for (int i = 0; i<numSphereVerts; i++)
		sphereVertLocs.append(MFloatPoint(sphereVertList[i].x, sphereVertList[i].y, sphereVertList[i].z));

	std::vector<int> sphereFaceCounts(numSphereFaces);
	int quadCount = (sphereDivisions - 1) * sphereSides;
	int faceIndex = 0;
	while (faceIndex < sphereSides) {
		sphereFaceCounts[faceIndex] = 3;
		faceIndex++;
	}

	while (faceIndex < (sphereSides + quadCount)) {

		sphereFaceCounts[faceIndex] = 4;
		faceIndex++;
	}

	while (faceIndex < sphereSides + quadCount + sphereSides) {

		sphereFaceCounts[faceIndex] = 3;
		faceIndex++;
	}

	MIntArray iaSphereFaceCounts;
	for (int i = 0; i<numSphereFaces; i++)
		iaSphereFaceCounts.append(sphereFaceCounts[i]);

	int triConnects = sphereSides * 2 * 3, quadConnects = quadCount * 4;
	int numSphereFaceConnects = quadConnects + triConnects;
	std::vector<int> sphereFaceConnects(numSphereFaceConnects);

	int connectIndex = 0;

	for (int side = 0; side<sphereSides; side++)
	{
		//set faceConnects for the first ring of tris

		sphereFaceConnects[connectIndex] = 0;

		if (side == sphereSides - 1)
			sphereFaceConnects[connectIndex + 1] = 1;
		else
			sphereFaceConnects[connectIndex + 1] = side + 2;

		sphereFaceConnects[connectIndex + 2] = side + 1;

		connectIndex += 3;
	}

	for (int i = 0; i<sphereDivisions - 1; i++)
	{
		for (int side = 1; side <= sphereSides; side++)
		{
			int vertex = side + (i * sphereSides);

			if (side == sphereSides)
			{
				sphereFaceConnects[connectIndex] = vertex;
				sphereFaceConnects[connectIndex + 1] = vertex + 1 - sphereSides;
				sphereFaceConnects[connectIndex + 2] = vertex + 1;
				sphereFaceConnects[connectIndex + 3] = vertex + sphereSides;
			}
			else
			{
				sphereFaceConnects[connectIndex] = vertex;
				sphereFaceConnects[connectIndex + 1] = vertex + 1;
				sphereFaceConnects[connectIndex + 2] = vertex + 1 + sphereSides;
				sphereFaceConnects[connectIndex + 3] = vertex + sphereSides;
			}

			connectIndex += 4;
		}
	}


	for (int side = 1; side <= sphereSides; side++) {

		int vertex = ((sphereDivisions - 1) * sphereSides) + side;

		sphereFaceConnects[connectIndex] = vertex;
		if (side == sphereSides) { sphereFaceConnects[connectIndex + 1] = (vertex + 1) - sphereSides; }
		else { sphereFaceConnects[connectIndex + 1] = vertex + 1; }
		sphereFaceConnects[connectIndex + 2] = lastVertIndex;

		connectIndex += 3;
	}

	MIntArray iaSphereFaceConnects;
	for (int i = 0; i<numSphereFaceConnects; i++)
		iaSphereFaceConnects.append(sphereFaceConnects[i]);

	std::vector< std::array <double, 3> > vertLocs;

	for (int i = 0; i<numSphereVerts; i++) {
		std::array< double, 3 > vertCatcher;

		vertCatcher[0] = sphereVertList[i].x;
		vertCatcher[1] = sphereVertList[i].y;
		vertCatcher[2] = sphereVertList[i].z;

		vertLocs.push_back(vertCatcher);
	}

	std::vector<int> sphFaceCounts;

	for (int i = 0; i<numSphereFaces; i++)
		sphFaceCounts.push_back(sphereFaceCounts[i]);


	std::vector<int> sphFaceConnects;

	for (int i = 0; i<numSphereFaceConnects; i++)
		sphFaceConnects.push_back(sphereFaceConnects[i]);


	MFnMesh newSphere;
	MObject sphereTransform = newSphere.create(numSphereVerts, numSphereFaces, sphereVertLocs, iaSphereFaceCounts, iaSphereFaceConnects);

	// Give our object a name

	MFnDependencyNode nodeFn;
	nodeFn.setObject(sphereTransform);

	// Create an MString and pass it to a dependency node's setName()
	std::string sphereName = name;
	char sphereName_c[32];
	strcpy(sphereName_c, sphereName.c_str());
	MString MSphereName = sphereName_c;
	nodeFn.setName(MSphereName);
}