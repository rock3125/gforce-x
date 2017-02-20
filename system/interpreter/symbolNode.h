#pragma once

#include "system/interpreter/scriptNode.h"

//////////////////////////////////////////////////////////////

// symbol table for object (do not persist)
class SymbolNode
{
public:
	SymbolNode();
	SymbolNode(const std::string& name,ScriptNode::Type dataType);
	~SymbolNode();
	SymbolNode(const SymbolNode&);
	const SymbolNode& operator=(const SymbolNode&);

	// entry variable name
	std::string			name;
	ScriptNode			value;
};
