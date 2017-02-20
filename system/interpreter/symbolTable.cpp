#include "standardFirst.h"

#include "system/interpreter/symbolTable.h"

#pragma warning(disable:4172)

/////////////////////////////////////////////////////////////////////////////

SymbolTable::SymbolTable()
{
}

SymbolTable::~SymbolTable()
{
}

SymbolTable::SymbolTable(const SymbolTable& s)
{
	operator=(s);
}

const SymbolTable& SymbolTable::operator=(const SymbolTable& s)
{
	symbolTable = s.symbolTable;
	return *this;
}

SymbolNode* SymbolTable::GetNode(const std::string& name)
{
	for (int i=0; i<symbolTable.size(); i++)
	{
		if (symbolTable[i].name == name)
			return &symbolTable[i];
	}
	return NULL;
}

SymbolNode* SymbolTable::GetNode(int index)
{
	return &symbolTable[index];
}

int SymbolTable::GetNodeCount()
{
	return symbolTable.size();
}

void SymbolTable::Add(const SymbolNode& node)
{
	symbolTable.push_back(node);
}

void SymbolTable::Remove(const std::string& name)
{
	std::vector<SymbolNode>::iterator pos = symbolTable.begin();
	while (pos != symbolTable.end())
	{
		if ((*pos).name == name)
		{
			symbolTable.erase(pos);
			break;
		}
		pos++;
	}
}

void SymbolTable::Clear()
{
	symbolTable.clear();
}

void SymbolTable::SetValue(const std::string& name,int i)
{
	SymbolNode* n = GetNode(name);
	if (n==NULL)
	{
		throw new Exception("variable "+name+" not declared");
		return;
	}
	n->value.SetData(i);
}

void SymbolTable::SetValue(const std::string& name,bool b)
{
	SymbolNode* n = GetNode(name);
	if (n==NULL)
	{
		throw new Exception("variable "+name+" not declared");
		return;
	}
	n->value.SetData(b);
}

void SymbolTable::SetValue(const std::string& name,float f)
{
	SymbolNode* n = GetNode(name);
	if (n==NULL)
	{
		throw new Exception("variable "+name+" not declared");
		return;
	}
	n->value.SetData(f);
}

void SymbolTable::SetValue(const std::string& name,void* obj)
{
	SymbolNode* n = GetNode(name);
	if (n==NULL)
	{
		throw new Exception("variable "+name+" not declared");
		return;
	}
	n->value.SetData(obj);
}

void SymbolTable::SetValue(const std::string& name,const std::string& str)
{
	SymbolNode* n = GetNode(name);
	if (n==NULL)
	{
		throw new Exception("variable "+name+" not declared");
		return;
	}
	n->value.SetData(str);
}

void SymbolTable::SetValue(const std::string& name,const D3DXVECTOR2& v)
{
	SymbolNode* n = GetNode(name);
	if (n==NULL)
	{
		throw new Exception("variable "+name+" not declared");
		return;
	}
	n->value.SetData(v);
}

void SymbolTable::SetValue(const std::string& name,const D3DXVECTOR3& v)
{
	SymbolNode* n = GetNode(name);
	if (n==NULL)
	{
		throw new Exception("variable "+name+" not declared");
		return;
	}
	n->value.SetData(v);
}

void SymbolTable::SetValue(const std::string& name,const D3DXQUATERNION& v)
{
	SymbolNode* n = GetNode(name);
	if (n==NULL)
	{
		throw new Exception("variable "+name+" not declared");
		return;
	}
	n->value.SetData(v);
}

const std::string& SymbolTable::GetString(const std::string& name)
{
	SymbolNode* n = GetNode(name);
	if (n==NULL)
	{
		throw new Exception("variable "+name+" not declared");
		return "";
	}
	return n->value.GetString();
}

int SymbolTable::GetInt(const std::string& name)
{
	SymbolNode* n = GetNode(name);
	if (n==NULL)
	{
		throw new Exception("variable "+name+" not declared");
		return 0;
	}
	return n->value.GetInt();
}

float SymbolTable::GetFloat(const std::string& name)
{
	SymbolNode* n = GetNode(name);
	if (n==NULL)
	{
		throw new Exception("variable "+name+" not declared");
		return 0;
	}
	return n->value.GetFloat();
}

bool SymbolTable::GetBool(const std::string& name)
{
	SymbolNode* n = GetNode(name);
	if (n==NULL)
	{
		throw new Exception("variable "+name+" not declared");
		return false;
	}
	return n->value.GetBool();
}

void* SymbolTable::GetObject(const std::string& name)
{
	SymbolNode* n = GetNode(name);
	if (n==NULL)
	{
		throw new Exception("variable "+name+" not declared");
		return NULL;
	}
	return n->value.GetObject();
}

const D3DXVECTOR2& SymbolTable::GetVec2(const std::string& name)
{
	SymbolNode* n = GetNode(name);
	if (n==NULL)
	{
		throw new Exception("variable "+name+" not declared");
		return D3DXVECTOR2(0,0);
	}
	return n->value.GetVec2();
}

const D3DXVECTOR3& SymbolTable::GetVec3(const std::string& name)
{
	SymbolNode* n = GetNode(name);
	if (n==NULL)
	{
		throw new Exception("variable "+name+" not declared");
		return D3DXVECTOR3(0,0,0);
	}
	return n->value.GetVec3();
}

const D3DXQUATERNION& SymbolTable::GetQuat(const std::string& name)
{
	SymbolNode* n = GetNode(name);
	if (n==NULL)
	{
		throw new Exception("variable "+name+" not declared");
		return D3DXQUATERNION(0,0,0,0);
	}
	return n->value.GetQuat();
}

