/*
	MeshMaker.h

	Functions for making simple shapes in Maya
*/

#pragma once
#ifndef MeshMaker_h
#define MeshMaker_h

#include <maya/MFnMesh.h>
#include <maya/MFloatPointArray.h>
#include <maya/MFloatPoint.h>
#include <maya/MFloatArray.h>
#include <maya/MIntArray.h>
#include <maya/MDoubleArray.h>

#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <algorithm>
#include <vector>
#include <array>
#include <string>

#include "PhotMath.h"

void makeCube(const Point &location, double width, std::string name);

void makeSphere(const Point &location, double radius, std::string name);

void makeArrow(const Point &location, const CVect &vect, std::string name, double radius);

#endif /* MeshMaker_h */