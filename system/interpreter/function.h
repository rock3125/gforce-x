#pragma once

#include "system/interpreter/symbolTable.h"

//////////////////////////////////////////////////////////////

class Function
{
public:
	Function();
	~Function();
	Function(const Function&);
	const Function& operator=(const Function&);

	void Clear();

	// name of the function
	std::string name;

	// parameters of function call
	std::vector<ScriptNode*>	params;

	// local variables
	SymbolTable					symbolTable;

	// instructions
	std::vector<ScriptNode*>	actions;
};

