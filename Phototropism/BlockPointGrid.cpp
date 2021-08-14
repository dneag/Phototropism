/*
	BlockPointGrid.cpp
*/

#include "BlockPointGrid.h"

void BlockPointGrid::checkRange_Point(const Point &p) {

	if (p.x > halfGridXSize || p.x < -halfGridXSize) {

		MStreamUtils::stdOutStream() << "Error. x coordinate outside of grid.\nAborting\n";
		exit(1);
	}
	else if (p.y > yGridSize || p.y < 0) {

		MStreamUtils::stdOutStream() << "Error. y coordinate outside of grid.\nAborting\n";
		exit(1);
	}
	else if (p.z > halfGridZSize || p.z < -halfGridZSize) {

		MStreamUtils::stdOutStream() << "Error. z coordinate outside of grid.\nAborting\n";
		exit(1);
	}
}

void BlockPointGrid::checkRange_GridElements() {

	if (xElements > xElements_MAX) {

		MStreamUtils::stdOutStream() << "Failed to instantiate BlockPointGrid. Too many x elements.\nAborting\n";
		exit(1);
	}
	else if (yElements > yElements_MAX) {

		MStreamUtils::stdOutStream() << "Failed to instantiate BlockPointGrid. Too many y elements.\nAborting\n";
		exit(1);
	}
	else if (zElements > zElements_MAX) {

		MStreamUtils::stdOutStream() << "Failed to instantiate BlockPointGrid. Too many z elements.\nAborting\n";
		exit(1);
	}
}
