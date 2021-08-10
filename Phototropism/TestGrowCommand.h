/*
	TestGrowCommand.h

	Creates the TestGrowCommand class
*/

#pragma once
#ifndef TestGrowCommand_h
#define TestGrowCommand_h

#include <maya/MPxCommand.h>

class TestGrowCommand : public MPxCommand
{
public:

	TestGrowCommand() {}

	virtual MStatus doIt(const MArgList& argList);

	static void* creator() {

		return new TestGrowCommand;
	}

	//static MSyntax newSyntax();
};

#endif /* TestGrowCommand_h */
