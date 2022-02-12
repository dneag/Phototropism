/*
	TestBPGCommand.h

	Creates the TestBPGCommand class
*/

#pragma once
#ifndef TestBPGCommand_h
#define TestBPGCommand_h

#include <maya/MPxCommand.h>

class TestBPGCommand : public MPxCommand
{
public:

	TestBPGCommand() {}

	virtual MStatus doIt(const MArgList& argList);

	static void* creator() {

		return new TestBPGCommand;
	}

	static MSyntax newSyntax();
};

#endif /* TestBPGCommand_h */
