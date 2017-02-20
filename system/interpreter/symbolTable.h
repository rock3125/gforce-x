#pragma once

#include "system/interpreter/symbolNode.h"

//////////////////////////////////////////////////////////////

// symbol table for object (do not persist)
class SymbolTable
{
public:
	SymbolTable();
	~SymbolTable();
	SymbolTable(const SymbolTable&);
	const SymbolTable& operator=(const SymbolTable&);

	// get a node by name or index
	SymbolNode* GetNode(const std::string& name);
	SymbolNode* GetNode(int index);
	int GetNodeCount();
	void Add(const SymbolNode& node);
	void Remove(const std::string& name);
	void Clear();

	// setters on symbol table items
	void SetValue(const std::string& name,int);
	void SetValue(const std::string& name,bool);
	void SetValue(const std::string& name,float);
	void SetValue(const std::string& name,void*);
	void SetValue(const std::string& name,const std::string& str);
	void SetValue(const std::string& name,const D3DXVECTOR2& v);
	void SetValue(const std::string& name,const D3DXVECTOR3& v);
	void SetValue(const std::string& name,const D3DXQUATERNION& v);

	// getters on the symbol table
	const std::string& GetString(const std::string& name);
	int GetInt(const std::string& name);
	float GetFloat(const std::string& name);
	bool GetBool(const std::string& name);
	void* GetObject(const std::string& name);
	const D3DXVECTOR2& GetVec2(const std::string& name);
	const D3DXVECTOR3& GetVec3(const std::string& name);
	const D3DXQUATERNION& GetQuat(const std::string& name);

private:
	std::vector<SymbolNode>	symbolTable;
};
