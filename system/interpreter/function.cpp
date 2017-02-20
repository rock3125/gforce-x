#include "standardFirst.h"

#include "system/interpreter/function.h"

/////////////////////////////////////////////////////////////////////////////

Function::Function()
{
}

Function::~Function()
{
}

Function::Function(const Function& f)
{
	operator=(f);
}

const Function& Function::operator=(const Function& f)
{
	name = f.name;
	symbolTable = f.symbolTable;
	actions = f.actions;
	params = f.params;
	return *this;
}

void Function::Clear()
{
	symbolTable.Clear();
	actions.clear();
	params.clear();
}

