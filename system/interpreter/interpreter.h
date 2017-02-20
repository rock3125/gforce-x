#pragma once

#include "system/interpreter/function.h"
#include "system/interpreter/scriptNode.h"

class WorldObject;

//////////////////////////////////////////////////////////////

class Interpreter
{
public:

	// run a list of statements
	static void ExecuteFunction(WorldObject* obj,Function& f,SymbolTable& globals,ScriptNode& result);
	static bool ExecuteFunction(WorldObject* obj,ScriptNode* s,SymbolTable& global,
								SymbolTable& local,ScriptNode& result);

	static void ExecuteStatementList(WorldObject* obj,ScriptNode* sl,SymbolTable& globals,
									 SymbolTable& locals,ScriptNode& result,bool& isReturn);

	// execute a function with a possible return value
	static void ExecuteFunction(WorldObject* obj, Function* function, SymbolTable& globals,
								SymbolTable& locals,ScriptNode& result);

	// run a single statement
	static void ExecuteStatement(WorldObject* obj,ScriptNode* s,SymbolTable& global,
								 SymbolTable& local,ScriptNode& result,bool& isReturn);

	static bool EvaluateExpression(WorldObject* obj,ScriptNode* s,SymbolTable& global,
								   SymbolTable& local,ScriptNode& result);
};