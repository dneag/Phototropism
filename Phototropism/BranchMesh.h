/*
BranchMesh.h

The BranchMesh class takes input in the form of a linked list of Segments, and converts it into data that
can be passed to the create() method of the Maya API's MFnMesh class
*/

#pragma once
#ifndef BranchMesh_h
#define BranchMesh_h

#include <vector>
#include <queue>

#include "Segment.h"
#include "Operators.h"
#include "PhotMath.h"

class BranchMesh
{
	// The number of sides of the cylindrical mesh.  It is also the number of vertices in each ring
	int sides;

	// The radius of the first segment (also the first ring of vertices) of the mesh
	double initialRadius;

	// The remaining attributes are those needed by the MFnMesh create() method.  Each is a list, and the order in which the elements are placed
	// is critical to creating the mesh correctly.  For details on this, read the notes under 'Mesh Construction' here: 
	// https://download.autodesk.com/us/maya/2011help/api/class_m_fn_mesh.html

	// To give a simple example of how the following three attributes work together:
	// To create a 4 sided mesh with only one segment (aka a cube with a hollow top and bottom), the lists would look like this:
	// verts = { {1,0,0}, {0,0,-1}, {-1,0,0}, {0,0,1}, {1,1,0}, {0,1,-1}, {-1,1,0}, {0,1,1} }
	// faceCounts = { 4, 4, 4, 4 }
	// faceConnects = { 0, 1, 5, 4, 1, 2, 6, 5, 2, 3, 7, 6, 3, 0, 4, 7 }
	// Each entry in faceConnects is the index of a vertex (in the verts lis) at a corner of a face/polygon, starting with the lower 
	// left corner of the first face and rotating counter-clockwise until as many elements have been passed as the first entry in 
	// faceCounts (4, that is).  This repeats through the end of the list

	// All of the vertex coordinates, starting with the one on the positive x-axis (if the mesh is on the origin and facing up) and rotating 
	// towards the negative z-axis.  Once as many vertices as there are sides have been placed, the next ring starts.
	std::vector< Point > verts;

	// Each entry indicates the number of edges on each face/polygon on the mesh, starting with the first face between the bottom two rings
	// of the mesh, and rotating with the verts list
	std::vector<int> faceCounts;

	// The indices of the vertices in the verts list, in an order corresponding to the polygons that they create.
	std::vector<int> faceConnects;

	// In the following two lists, each parallel entry combined makes up the 2D uv texture mapping of each vertex in the mesh.
	std::vector<double> us;
	std::vector<double> vs;

public:

	// Creates the object and its first ring of vertices
	BranchMesh(Segment *firstseg, const int currentOrderSides);

	// Finds the next segment on the path
	// Returns null if there isn't one, indicating the end of the mesh
	Segment * findNextSegOnPath(Segment *currentSeg);

	// Creates the last ring and adds the cap vertex
	void completePath(Segment *lastSeg, const std::vector<double> &preadjusts);

	// Returns the width of the divider, returns 0. if there is no divider
	// pre: all segment radii are already calculated
	double findDividerIfAny(const double currentSegRadius, Segment *nextSegOnPath) const;

	// Makes a ring of points (not vertices added to the mesh) that will have vectors added to them to create the next ring
	// This is used when the next segment has a different radius than the last, but it is not a big enough difference to warrant creating a divider
	// Typically this ring is a shrunken version of the top ring of vertices
	std::vector<Point> BranchMesh::makeRingToAddTo(const double radiusDiff, const Point center, const double halfDividerWidth) const;

	// Creates the next ring of vertices
	std::vector<double> createNextRing(Segment *currentSeg, Segment *nextSeg, const std::vector<double> &preadjusts,
		const double halfDividerWidth, const std::vector<Point> &ringToAddTo);

	// Creates and finalizes the positions of the ring of vertices between the top rings of the current seg and next seg
	void createDividerRing(const double halfDividerWidth, Segment *currentSeg, Segment *nextSeg, const std::vector<double> &preadjusts);

	// Traverses all segments until there is no seg above the current seg with the same meristem as the current seg
	// At each segment we check for connected segs with different meristems, adding those segs as new firstSegsOfBMeshes
	// At each segment we set the positions for the ring of vertices at its end, and if there is a divider above it, we set that ring too
	// For each ring of vertices added, a corresponding set of faceConnects and faceCounts is also added
	// When the last segment of the mesh is found, a single cap vertex is added along with a corresponding set of faceConnects and faceCounts
	void go(Segment *seg, const std::vector<double> &preadjusts, std::queue<Segment*> &firstSegsOfBMeshes);

	int numVerts() { return verts.size(); }
	Point getVert(int index) { return verts[index]; }
	double distBetween(int a, int b) { return distance(verts[a], verts[b]); }

	int numFaces() { return faceCounts.size(); }
	int numUs() { return us.size(); }
	int numVs() { return vs.size(); }
	double getU(int index) { return us[index]; }
	double getV(int index) { return vs[index]; }
	int getFaceCount(int index) { return faceCounts[index]; }

	int numFaceConnects() { return faceConnects.size(); }
	int getFaceConnect(int index) { return faceConnects[index]; }

	void addNextFaceConnectsAndCounts();

	void addCapFaceConnectsAndCounts();

	void calculateUVs();

	void reportInMaya();
};

#endif /* BranchMesh_h */
