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

void makeSphere(const Point &location, double radius);

void makeCube(const Point &location, double width, std::string name);

#endif /* MeshMaker_h */