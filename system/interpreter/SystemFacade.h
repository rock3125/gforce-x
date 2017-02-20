#pragma once

#include "system/interpreter/scriptNode.h"

class WorldObject;

///////////////////////////////////////////////////////

class SystemFacade
{
public:
	SystemFacade();
	~SystemFacade();

	// execute one of the system facade functions
	bool ExecuteFunction(const std::string& name,ScriptNode& result,
						 std::vector<ScriptNode*>& parameters);

private:
	// convert anything to a string
	std::string ToString(ScriptNode* n);
};

