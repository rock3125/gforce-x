#include "standardFirst.h"

#include "system/interpreter/symbolNode.h"

/////////////////////////////////////////////////////////////////////////////

SymbolNode::SymbolNode()
{
}

SymbolNode::SymbolNode(const std::string& _name,ScriptNode::Type _dataType)
{
	name = _name;
	value.type = _dataType;
	// make sure its allocated and initialised
	switch (_dataType)
	{
		case ScriptNode::Int:
		{
			value.SetData(0);
			break;
		}
		case ScriptNode::Float:
		{
			value.SetData(0.0f);
			break;
		}
		case ScriptNode::Object:
		{
			value.SetData(NULL);
			break;
		}
		case ScriptNode::String:
		{
			value.SetData("");
			break;
		}
		case ScriptNode::Bool:
		{
			value.SetData(false);
			break;
		}
		case ScriptNode::Vec2:
		{
			value.SetData(D3DXVECTOR2(0,0));
			break;
		}
		case ScriptNode::Vec3:
		{
			value.SetData(D3DXVECTOR3(0,0,0));
			break;
		}
		case ScriptNode::Quat:
		{
			value.SetData(D3DXQUATERNION(0,0,0,0));
			break;
		}
	}
}

SymbolNode::~SymbolNode()
{
}

SymbolNode::SymbolNode(const SymbolNode& s)
{
	operator=(s);
}

const SymbolNode& SymbolNode::operator=(const SymbolNode& s)
{
	name = s.name;
	value = s.value;
	return *this;
}

