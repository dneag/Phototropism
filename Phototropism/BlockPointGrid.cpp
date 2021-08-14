/*
	BlockPointGrid.cpp
*/

#include "BlockPointGrid.h"

void BlockPointGrid::checkRange_Point(const Point &p) const {

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

void BlockPointGrid::checkRange_GridElements() const {

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

std::size_t BlockPointGrid::findShiftedIndex(double xCoord, double halfGridSize, double unitSize) const {
	
	// Add halfGridSize to put the coordinate in the positive range. E.g. the range corresponding to grid element indices.
	double shiftedCoord = xCoord + halfGridSize;

	// To represent the grid as centered, we must account for a potential additional half unit shift.  We do this by adding 1 to the index
	// if the remainder from truncating temp is greater than .5.
	double temp = shiftedCoord / unitSize;
	std::size_t ind = static_cast<int>(temp);
	double remainder = temp - ind; 
	if (remainder >= .5) { ++ind; }

	return ind;
}
