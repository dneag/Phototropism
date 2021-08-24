#include <maya/MStreamUtils.h>

#include "BranchMesh.h"
#include "Segment.h"
#include "PhotMath.h"
#include "Operators.h"

BranchMesh::BranchMesh(Segment *firstSeg, const int currentOrderSides) {

	//MStreamUtils::stdOutStream() << "ENTER FUNCTION - BranchMesh::BranchMesh() " << "\n";

	sides = currentOrderSides;

	initialRadius = firstSeg->getRadius();

	if (sides > 2) {

		Space meshSpace(findVectorAngles(firstSeg->getVect()));
		double polarAngleToNextVert = 0., polarIncrement = MM::PIM2 / sides;

		for (int i = 0; i < sides; i++)
		{
			CVect vectorToVert = meshSpace.makeVector(polarAngleToNextVert, MM::PID2, initialRadius);

			Point newVert = firstSeg->getStartPoint() + vectorToVert;
			verts.push_back(newVert);

			polarAngleToNextVert -= polarIncrement;
		}
	}
	else {

		double distTimesSinAzi = initialRadius * MM::sinPID2;
		CVect vectorToStartVert;
		double polarAngleToNextVert = findVectorAngles(firstSeg->getVect()).pol + MM::PID2;

		for (int i = 0; i<sides; i++)
		{
			vectorToStartVert.x = distTimesSinAzi*std::cos(polarAngleToNextVert);
			vectorToStartVert.y = initialRadius * MM::cosPID2;
			vectorToStartVert.z = distTimesSinAzi*std::sin(polarAngleToNextVert);

			Point newVert = firstSeg->getStartPoint() + vectorToStartVert;
			verts.push_back(newVert);

			polarAngleToNextVert -= MM::PI;
		}
	}
}

void BranchMesh::go(Segment *seg, const std::vector<double> &preadjusts, std::queue<Segment*> &firstSegsOfBMeshes) {

	//MStreamUtils::stdOutStream() << "ENTER FUNCTION - BranchMesh::go()" << "\n";

	// Check for the beginnings of any new branch meshes
	std::vector<Segment*> potentialFirstSegs = seg->getConnectedUpperSegs();
	for (auto connectedSeg : potentialFirstSegs) {

		// If a connected seg has a different meri, then it is the start of a new branch
		if (connectedSeg->getMeri() != seg->getMeri())
			firstSegsOfBMeshes.push(connectedSeg);
	}

	// Check if there is a segment ahead of this one
	Segment *nextSegOnPath = findNextSegOnPath(seg);
	if (!nextSegOnPath) {

		// This should mean this is the last segment for this mesh
		//MStreamUtils::stdOutStream() << "last seg..." << "\n\n";

		this->completePath(seg, preadjusts);

		//MStreamUtils::stdOutStream() << "path completed.  verts: " << verts.size() << ", faceConnects: " << faceConnects.size() <<
		//	", faceCounts: " << faceCounts.size() << "\n\n";

		return;
	}

	double halfDividerWidth = findDividerIfAny(seg->getRadius(), nextSegOnPath);

	std::vector<Point> ringToAddTo = this->makeRingToAddTo(seg->getRadius() - nextSegOnPath->getRadius(), seg->getStartPoint(), halfDividerWidth);

	std::vector<double> nextPreadjusts = this->createNextRing(seg, nextSegOnPath, preadjusts, halfDividerWidth, ringToAddTo);

	this->addNextFaceConnectsAndCounts();

	if (halfDividerWidth > 0.) {

		this->createDividerRing(halfDividerWidth, seg, nextSegOnPath, nextPreadjusts);
		this->addNextFaceConnectsAndCounts();

		// the preadjusts are applied to the divider ring, so we reset them to -halfDividerWidth. for the next go()
		std::fill(nextPreadjusts.begin(), nextPreadjusts.end(), -halfDividerWidth);
	}

	this->go(nextSegOnPath, nextPreadjusts, firstSegsOfBMeshes);
}

Segment * BranchMesh::findNextSegOnPath(Segment *currentSeg) {

	//MStreamUtils::stdOutStream() << "ENTER FUNCTION - BranchMesh::findNextSegOnPath()" << "\n";

	for (auto segAbove : currentSeg->getSegsAbove()) {

		//MStreamUtils::stdOutStream() << "checking meri of seg in segsAbove" << "\n";

		if (segAbove->getMeri() == currentSeg->getMeri()) {
			//MStreamUtils::stdOutStream() << "found next seg on path" << "\n";
			//MStreamUtils::stdOutStream() << "EXIT FUNCTION - BranchMesh::findNextSegOnPath()" << "\n";
			return segAbove;
		}
	}

	//MStreamUtils::stdOutStream() << "EXIT FUNCTION - BranchMesh::findNextSegOnPath()" << "\n";
	return nullptr;
}

void BranchMesh::completePath(Segment *lastSeg, const std::vector<double> &preadjusts) {

	//MStreamUtils::stdOutStream() << "ENTER FUNCTION - BranchMesh::completePath() " << "\n";

	std::size_t lowerRingFirstVert = verts.size() - sides;
	for (int i = 0; i < sides; ++i)
	{
		CVect resizedVector = lastSeg->getVect().resized(lastSeg->getVect().getMag() + preadjusts[i]);

		verts.push_back(verts[lowerRingFirstVert + i] + resizedVector);
	}

	this->addNextFaceConnectsAndCounts();

	CVect vectorToLastVert = lastSeg->getVect().resized(lastSeg->getRadius());
	Point capVert = lastSeg->getStartPoint() + lastSeg->getVect() + vectorToLastVert;
	verts.push_back(capVert);

	this->addCapFaceConnectsAndCounts();

	//MStreamUtils::stdOutStream() << "EXIT FUNCTION - BranchMesh::completePath()" << "\n";
}

double BranchMesh::findDividerIfAny(const double currentSegRadius, Segment *nextSegOnPath) const {

	//MStreamUtils::stdOutStream() << "ENTER FUNCTION - BranchMesh::findDividerIfAny()" << "\n";

	double radiusMinPercentDiff = 1.15;
	double radiusOfSegAbove = nextSegOnPath->getRadius();

	//MStreamUtils::stdOutStream() << "this radius: " << currentSegRadius << ", last radius: " << radiusOfSegAbove << "\n";

	if (currentSegRadius > radiusOfSegAbove * radiusMinPercentDiff) {

		double largestRadius = 0.;

		for (auto lateralSeg : nextSegOnPath->getLateralSegs())
		{
			double skinThickness = lateralSeg->getMeri()->skinThickness;

			double thisRadius = lateralSeg->getRadius() + skinThickness;
			if (thisRadius > largestRadius) { largestRadius = thisRadius; }
		}

		if (largestRadius == 0. || largestRadius >= nextSegOnPath->getLength())
			MStreamUtils::stdOutStream() << "WARNING: creating divider with funny width: " << largestRadius << "\n";

		//MStreamUtils::stdOutStream() << "largestRadius: " << largestRadius << "\n";

		return largestRadius;
	}
	else {

		return 0.;
	}
}

std::vector<Point> BranchMesh::makeRingToAddTo(const double radiusDiff, const Point center, const double halfDividerWidth) const {

	std::vector<Point> ringToAddTo;

	if (halfDividerWidth > 0.) {

		for (int i = verts.size() - sides; i < verts.size(); ++i) {

			ringToAddTo.push_back(verts[i]);
		}
	}
	else {

		for (int i = verts.size() - sides; i < verts.size(); ++i) {

			CVect vectToCenter = center - verts[i];
			ringToAddTo.push_back(verts[i] + vectToCenter.resized(radiusDiff));
		}
	}

	return ringToAddTo;
}

std::vector<double> BranchMesh::createNextRing(Segment *currentSeg, Segment *nextSeg, const std::vector<double> &preadjusts,
	const double halfDividerWidth, const std::vector<Point> &ringToAddTo) {

	//MStreamUtils::stdOutStream() << "ENTER FUNCTION - BranchMesh::createNextRing()" << "\n";

	// We can create the next ring/segment on a mesh by simply adding the segment's vector to each of the vertices on the
	// previous ring, each sum being the location of a new vertex on the next ring.  However, unless the angle between the two
	// segments is 0, the size of the vector added to find these new vertices must be adjusted to account for the angle change.
	// The magnitude of this size adjustment is called the pre-adjust.  The 'preadjusts' and 'newPreadjusts' lists store the pre-adjusts
	// corresponding to each vertex in the ring.  The 'preadjusts' list in this method contains the pre-adjusts from the previous ring creation,
	// and will be used to create this ring.  'newPreadjusts' will be sent along for the next ring creation.
	std::vector<double> newPreadjusts;
	const double angBetweenSegments = findAngBetween(currentSeg->getVect(), nextSeg->getVect());

	//MStreamUtils::stdOutStream() << "angBetweenSegments: " << angBetweenSegments << "\n";

	if (angBetweenSegments > .0001 || angBetweenSegments < -.0001) {

		const double radius = nextSeg->getRadius();
		const double angBetweenOut90 = MM::PID2 - angBetweenSegments;
		const double sinAngBetween = std::sin(angBetweenOut90);

		const double largeOppSideLength = sinAngBetween * nextSeg->getVect().getMag(); // SOH
		const Point nextSegEndPoint = nextSeg->getStartPoint() + nextSeg->getVect();
		const CVect currentVectResized = currentSeg->getVect().resized(largeOppSideLength);
		const Point rightAnglePoint = nextSegEndPoint - currentVectResized;
		const Point currentSegEndPoint = currentSeg->getStartPoint() + currentSeg->getVect();
		CVect rightAngleVector = rightAnglePoint - currentSegEndPoint;
		rightAngleVector.resize(radius);
		const Point pointUnderAngle = currentSegEndPoint + rightAngleVector;
		const Point pointBehindAngle = currentSegEndPoint - rightAngleVector;
		rightAngleVector = pointBehindAngle - pointUnderAngle;

		const double topTriangleHSide = sinAngBetween * radius; //SOH
		const double topTriangleVSide = std::cos(angBetweenOut90) * radius; //CAH
		const double bottomTriangleVSide = std::tan(angBetweenOut90) * (radius - topTriangleHSide); //TOA
		double maxAdjust = bottomTriangleVSide - topTriangleVSide;

		if (std::fabs(maxAdjust) >= currentSeg->getVect().getMag())
			MStreamUtils::stdOutStream() << "WARNING: angle between segments is too big.  Your mesh probably looks funny. " << "\n";

		for (int s = 0; s < sides; ++s) {

			// preadjusts must be added to create a tentative Point. this point lies on the plane at the end of and perpendicular to the 
			// current segment's vector. its position is needed to calculate the next adjustment
			Point vertPosWithPreAdjust = ringToAddTo[s] + currentSeg->getVect().resized(currentSeg->getLength() + preadjusts[s]);
			CVect vectorToPUA = vertPosWithPreAdjust - pointUnderAngle;
			double vectorToPUAMag = vectorToPUA.getMag();
			double adjust = maxAdjust;

			if (vectorToPUAMag > 0.) {

				double angBetween = findAngBetween(rightAngleVector, vectorToPUA);
				double distAlongRightAngleVector = std::cos(angBetween) * vectorToPUAMag; //CAH
				adjust = (1. - (distAlongRightAngleVector / radius)) * maxAdjust;
			}

			// add the new adjustment then save it as a preadjust for the next ring
			Point finalVertPos = vertPosWithPreAdjust + currentSeg->getVect().resized(adjust);
			verts.push_back(finalVertPos);
			newPreadjusts.push_back(adjust);
		}
	}
	else {

		for (int s = 0; s < sides; ++s) {

			verts.push_back(ringToAddTo[s] + currentSeg->getVect().resized(currentSeg->getLength() + preadjusts[s]));
			newPreadjusts.push_back(0.);
		}
	}

	//MStreamUtils::stdOutStream() << "EXIT FUNCTION - BranchMesh::createNextRing()" << "\n";

	return newPreadjusts;
}

void BranchMesh::createDividerRing(const double halfDividerWidth, Segment *currentSeg, Segment *nextSeg, const std::vector<double> &preadjusts) {

	//MStreamUtils::stdOutStream() << "ENTER FUNCTION - BranchMesh::createDividerRing()" << "\n";

	int topRingFirstVert = verts.size() - sides;
	for (int s = 0; s < sides; ++s) {

		// First add the preadjust from the previous ring so that the vertex sits on the perpendicular plane at the beginning of nextSeg
		Point tempVertPos = verts[topRingFirstVert + s] + nextSeg->getVect().resized(preadjusts[s]);

		// Then using the start point of the nextSeg, shrink the vertex position inward according to the new radius
		CVect vectorTowardsCenter = nextSeg->getStartPoint() - tempVertPos;
		vectorTowardsCenter.resize(currentSeg->getRadius() - nextSeg->getRadius());

		// Add vectorTowardsCenter to tempVertPos to shrink its position inward, then add nextSeg's vector at a length of halfDividerWidth
		// to finalize the position of the new vertex
		verts.push_back(tempVertPos + vectorTowardsCenter + nextSeg->getVect().resized(halfDividerWidth));

		// Now that we are done with the vertex in the previous ring, we can slide it down according to the divider width
		verts[topRingFirstVert + s] -= currentSeg->getVect().resized(halfDividerWidth);
	}

	//MStreamUtils::stdOutStream() << "EXIT FUNCTION - BranchMesh::createDividerRing()" << "\n";
}

// Adds faceConnects for the faces between the last and second-to-last rings of verts created 
// pre: Must have two rings for which faceconnects have not been created
void BranchMesh::addNextFaceConnectsAndCounts() {

	// For each 4 sided face added we have to specify the indices of the verts that make up its 4 corners - these indices are the faceConnects
	// For each face, they start on the lower left and move counter-clockwise
	int initialIndex = verts.size() - sides * 2;
	if (sides > 2) {

		int nextVertIndex = initialIndex;
		for (nextVertIndex; nextVertIndex < verts.size() - sides - 1; ++nextVertIndex) {

			faceConnects.push_back(nextVertIndex);
			faceConnects.push_back(nextVertIndex + 1);
			faceConnects.push_back(nextVertIndex + 1 + sides);
			faceConnects.push_back(nextVertIndex + sides);
		}

		//The pattern for faceConnects is a bit different for the last side in every ring of sides
		faceConnects.push_back(nextVertIndex);
		faceConnects.push_back(initialIndex);
		faceConnects.push_back(initialIndex + sides);
		faceConnects.push_back(nextVertIndex + sides);

		for (int i = 0; i < sides; ++i)
			faceCounts.push_back(4);
	}
	else
	{
		// this is for 2 siders (one face)
		faceConnects.push_back(initialIndex);
		faceConnects.push_back(initialIndex + 1);
		faceConnects.push_back(initialIndex + 1 + sides);
		faceConnects.push_back(initialIndex + sides);

		faceCounts.push_back(4);
	}
}

// The cap is a single vertex and it is the last in the list for this mesh
void BranchMesh::addCapFaceConnectsAndCounts() {

	int initialIndex = verts.size() - sides - 1;
	if (sides > 2) {

		// First do (sides - 1) loops because the pattern changes for the last face in the ring
		int nextVertIndex = initialIndex;
		for (nextVertIndex; nextVertIndex < verts.size() - 2; ++nextVertIndex) {

			faceConnects.push_back(nextVertIndex);
			faceConnects.push_back(nextVertIndex + 1);
			faceConnects.push_back(verts.size() - 1);
		}

		// The pattern for faceConnects is a bit different for the last face in every ring
		faceConnects.push_back(nextVertIndex);
		faceConnects.push_back(initialIndex);
		faceConnects.push_back(verts.size() - 1);

		for (int i = 0; i < sides; ++i)
			faceCounts.push_back(3);
	}
	else {

		//this means we have a 2-sider, so only making one face
		faceConnects.push_back(initialIndex);
		faceConnects.push_back(initialIndex + 1);
		faceConnects.push_back(verts.size() - 1);

		faceCounts.push_back(3);
	}
}

void BranchMesh::calculateUVs()
{
	double wedgeAngle = MM::PIM2 / sides;
	double radSqu = initialRadius*initialRadius;

	double baseFaceWidth = std::sqrt((radSqu + radSqu) - (2 * radSqu*std::cos(wedgeAngle)));//law of cosines

	int numTextureColumns = 1;
	int faces = sides;
	if (sides == 2) { faces = 1; }

	double uvFaceWidth = 1. / (numTextureColumns * faces);
	double uvScaler = uvFaceWidth / baseFaceWidth;

	int sideInd = 0;
	int cnctInd = 0;

	sideInd = 0;
	cnctInd = 0;
	int ring = 1;
	int indexDiffOfVertBelow;

	for (int i = 0; i<faceCounts.size(); i++)
	{
		if (faceCounts[i] == 4)
		{
			us.push_back(1. - (sideInd * uvFaceWidth));
			us.push_back(1. - ((sideInd + 1) * uvFaceWidth));
			us.push_back(1. - ((sideInd + 1) * uvFaceWidth));
			us.push_back(1. - (sideInd * uvFaceWidth));

			if (ring > 1)
			{
				int matchingCnctInd = (cnctInd - indexDiffOfVertBelow) + 3;
				vs.push_back(vs[matchingCnctInd]);
				matchingCnctInd = ((cnctInd + 1) - indexDiffOfVertBelow) + 1;
				vs.push_back(vs[matchingCnctInd]);
				double distFromVertBelow = distBetween(faceConnects[cnctInd + 1], faceConnects[cnctInd + 2]);
				vs.push_back(vs.back() + (distFromVertBelow * uvScaler));
				distFromVertBelow = distBetween(faceConnects[cnctInd], faceConnects[cnctInd + 3]);
				vs.push_back(vs[cnctInd] + (distFromVertBelow * uvScaler));
			}
			else
			{
				vs.push_back(0.);
				vs.push_back(0.);

				double distFromVertBelow = distBetween(faceConnects[cnctInd + 1], faceConnects[cnctInd + 2]);
				vs.push_back(distFromVertBelow * uvScaler);
				distFromVertBelow = distBetween(faceConnects[cnctInd], faceConnects[cnctInd + 3]);
				vs.push_back(distFromVertBelow * uvScaler);
			}
		}
		else if (faceCounts[i] == 3)
		{
			indexDiffOfVertBelow = 0;

			for (int j = 1; j <= sides; j++)
			{
				indexDiffOfVertBelow += faceCounts[i - j];
			}

			us.push_back(1. - (sideInd * uvFaceWidth));
			us.push_back(1. - ((sideInd + 1) * uvFaceWidth));
			us.push_back(1. - ((sideInd + 1) * uvFaceWidth) + (uvFaceWidth * .5));

			//here we can assume ring > 1 because only cap faces should have 3 sides
			int matchingCnctInd = (cnctInd - indexDiffOfVertBelow) + 3;
			vs.push_back(vs[matchingCnctInd]);
			matchingCnctInd = ((cnctInd + 1) - indexDiffOfVertBelow) + 1;
			vs.push_back(vs[matchingCnctInd]);
			double distFromVertBelow = distBetween(faceConnects[cnctInd + 1], faceConnects[cnctInd + 2]);
			vs.push_back(vs.back() + (distFromVertBelow * uvScaler));
		}

		cnctInd += faceCounts[i];
		if (sides > 2) { sideInd++; }

		if (sideInd == sides) {
			sideInd = 0;
			indexDiffOfVertBelow = sides * (faceCounts[(i + 1) - sides]);
			ring++;
		}
	}

}

void BranchMesh::reportInMaya() {

	MStreamUtils::stdOutStream() << "\n" << verts.size() << " Verts:\n";

	int vertsPerLine = 4;
	int vertIndexCounter = 0;

	for (auto v : verts) {

		MStreamUtils::stdOutStream() << vertIndexCounter++ << ": " << v << ", ";

		if (vertIndexCounter + 1 % vertsPerLine == 0)
			MStreamUtils::stdOutStream() << "\n";
	}

	MStreamUtils::stdOutStream() << "\n\n" << faceCounts.size() << " Face Counts:\n";

	int faceCountsPerLine = 20;
	int fcIndexCounter = 0;

	for (auto fc : faceCounts) {

		MStreamUtils::stdOutStream() << "[" << fcIndexCounter++ << "]" << ": " << fc << ", ";

		if (fcIndexCounter + 1 % faceCountsPerLine == 0)
			MStreamUtils::stdOutStream() << "\n";
	}

	MStreamUtils::stdOutStream() << "\n\n" << faceConnects.size() << " Face Connects:\n";

	int faceConnectsPerLine = 20;
	int fccIndexCounter = 0;

	for (auto fcc : faceConnects) {

		MStreamUtils::stdOutStream() << "[" << fccIndexCounter++ << "]" << ": " << fcc << ", ";

		if (fccIndexCounter + 1 % faceConnectsPerLine == 0)
			MStreamUtils::stdOutStream() << "\n";
	}

	MStreamUtils::stdOutStream() << "\n\n";
}