/*
	Segment.h

	Segments are the building blocks of a BranchMesh
	Segments are typically cylinders, unless they have only 2 sides, in which case they are rectangular planes
*/

#pragma once
#ifndef Segment_h
#define Segment_h

#include <vector>

#include "PhotMath.h"
#include "Operators.h"

// In the larger program, a Meristem class determines much of what happens to Segments
// In this program it does not at all, we just need two attributes to mimic its usage in BranchMesh
struct Meristem
{
	double skinThickness;
	int sides;

	Meristem(double SKINTHICKNESS, int SIDES) : skinThickness(SKINTHICKNESS), sides(SIDES) {}
};

class Segment
{
	CVect vect;
	Point startPoint;
	double radius;

	// segsAbove are Segments whose start points are at the end point of this one
	std::vector<Segment*> segsAbove;

	// lateralSegs are Segments whose start points are along the length of this one
	std::vector<Segment*> lateralSegs;

	Meristem *meri = nullptr;

public:

	Segment(CVect VECT, Point STARTPOINT, double RADIUS, Meristem *MERI)
		: vect(VECT), startPoint(STARTPOINT), radius(RADIUS), meri(MERI) {}

	Segment(CVect VECT, Point STARTPOINT, double RADIUS)
		: vect(VECT), startPoint(STARTPOINT), radius(RADIUS) {}

	double getRadius() const { return radius; }

	CVect getVect() const { return vect; }

	double getLength() const { return vect.getMag(); }

	void setStartPoint(const Point &sp) { startPoint = sp; }

	Point getStartPoint() const { return startPoint; }

	Point getEndPoint() const { return startPoint + vect; }

	void addSegAbove(Segment *seg) { segsAbove.push_back(seg); }

	std::vector<Segment*> getSegsAbove() const { return segsAbove; }

	void addLateralSeg(Segment *seg) { lateralSegs.push_back(seg); }

	std::vector<Segment*> getLateralSegs() const { return lateralSegs; }

	// Returns a list combined of lateralSegs and segsAbove
	std::vector<Segment*> getConnectedUpperSegs() const {

		std::vector<Segment*> connectedUpperSegs;
		connectedUpperSegs = lateralSegs;
		connectedUpperSegs.insert(std::end(connectedUpperSegs), std::begin(segsAbove), std::end(segsAbove));
		return connectedUpperSegs;
	}

	Meristem * getMeri() const { return meri; }
};
#endif /* Segment_h */
