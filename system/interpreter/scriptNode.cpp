#include "standardFirst.h"

#include "system/interpreter/scriptNode.h"

#pragma warning(disable:4172)

/////////////////////////////////////////////////////////////////////////////

ScriptNode::ScriptNode()
	: data(NULL)
{
	type = None;
}

ScriptNode::ScriptNode(Type _type)
	: data(NULL)
{
	type = _type;
}

ScriptNode::ScriptNode(Type _type,std::vector<ScriptNode*> _nodes)
	: data(NULL)
{
	type = _type;
	nodes = _nodes;
}

ScriptNode::ScriptNode(std::string _str)
	: data(NULL)
{
	SetData(_str);
}

ScriptNode::ScriptNode(int _i)
	: data(NULL)
{
	SetData(_i);
}

ScriptNode::ScriptNode(float _f)
	: data(NULL)
{
	SetData(_f);
}

ScriptNode::ScriptNode(bool _b)
	: data(NULL)
{
	SetData(_b);
}

ScriptNode::ScriptNode(void* _data)
	: data(NULL)
{
	SetData(_data);
}

ScriptNode::ScriptNode(const D3DXVECTOR2& vec2)
	: data(NULL)
{
	SetData(vec2);
}

ScriptNode::ScriptNode(const D3DXVECTOR3& vec3)
	: data(NULL)
{
	SetData(vec3);
}

ScriptNode::ScriptNode(const D3DXQUATERNION& q)
	: data(NULL)
{
	SetData(q);
}

ScriptNode::~ScriptNode()
{
	safe_delete_stl_array(nodes);
	if (type != Object)
	{
		safe_delete(data);
	}
	type = None;
}

ScriptNode::ScriptNode(const ScriptNode& s)
	: data(NULL)
{
	operator=(s);
}

const ScriptNode& ScriptNode::operator=(const ScriptNode& s)
{
	if (type != Object)
	{
		safe_delete(data);
	}

	type = s.type;

	safe_delete(data);
	if (s.data!=NULL)
	{
		switch (type)
		{
			case Int:
			{
				SetData(*(int*)s.data);
				break;
			}
			case String:
			{
				SetData(*(std::string*)s.data);
				break;
			}
			case Float:
			{
				SetData(*(float*)s.data);
				break;
			}
			case Bool:
			{
				SetData(*(bool*)s.data);
				break;
			}
			case Object:
			{
				data = s.data;
				break;
			}
			case Vec2:
			{
				SetData(*(D3DXVECTOR2*)s.data);
				break;
			}
			case Vec3:
			{
				SetData(*(D3DXVECTOR3*)s.data);
				break;
			}
			case Quat:
			{
				SetData(*(D3DXQUATERNION*)s.data);
				break;
			}
		}
	}

	safe_delete_stl_array(nodes);

	for (int i=0; i<s.nodes.size(); i++)
	{
		nodes.push_back(new ScriptNode(*s.nodes[i]));
	}
	return *this;
}

void ScriptNode::SetData(int d)
{
	type = Int;
	safe_delete(data);
	data = new int;
	*(int*)data = d;
}

void ScriptNode::SetData(float d)
{
	type = Float;
	safe_delete(data);
	data = new float;
	*(float*)data = d;
}

void ScriptNode::SetData(bool d)
{
	type = Bool;
	safe_delete(data);
	data = new bool;
	*(bool*)data = d;
}

void ScriptNode::SetData(void* d)
{
	type = Object;
	data = d;
}

void ScriptNode::SetData(std::string d)
{
	type = String;
	safe_delete(data);
	data = new std::string;
	*(std::string*)data = d;
}

void ScriptNode::SetData(D3DXVECTOR2 d)
{
	type = Vec2;
	safe_delete(data);
	data = new D3DXVECTOR2;
	*(D3DXVECTOR2*)data = d;
}

void ScriptNode::SetData(D3DXVECTOR3 d)
{
	type = Vec3;
	safe_delete(data);
	data = new D3DXVECTOR3;
	*(D3DXVECTOR3*)data = d;
}

void ScriptNode::SetData(D3DXQUATERNION d)
{
	type = Quat;
	safe_delete(data);
	data = new D3DXQUATERNION;
	*(D3DXQUATERNION*)data = d;
}

int ScriptNode::GetInt()
{
	if (type != Int)
	{
		throw new Exception("wrong data-type");
		return 0;
	}
	if (data==NULL)
	{
		data = new int;
		*(int*)data = 0;
	}
	return *(int*)data;
}

float ScriptNode::GetFloat()
{
	if (type != Float)
	{
		throw new Exception("wrong data-type");
		return 0.0f;
	}
	if (data==NULL)
	{
		data = new float;
		*(float*)data = 0;
	}
	return *(float*)data;
}

bool ScriptNode::GetBool()
{
	if (type != Bool)
	{
		throw new Exception("wrong data-type");
		return false;
	}
	if (data==NULL)
	{
		data = new bool;
		*(bool*)data = false;
	}
	return *(bool*)data;
}

void* ScriptNode::GetObject()
{
	if (type != Object)
	{
		throw new Exception("wrong data-type");
		return 0;
	}
	return data;
}

const std::string& ScriptNode::GetString()
{
	if (type != String)
	{
		throw new Exception("wrong data-type");
		return "";
	}
	if (data==NULL)
	{
		data = new std::string;
	}
	return *(std::string*)data;
}

const D3DXVECTOR2& ScriptNode::GetVec2()
{
	if (type != Vec2)
	{
		throw new Exception("wrong data-type");
		return D3DXVECTOR2(0,0);
	}
	if (data==NULL)
	{
		data = new D3DXVECTOR2;
	}
	return *(D3DXVECTOR2*)data;
}

const D3DXVECTOR3& ScriptNode::GetVec3()
{
	if (type != Vec3)
	{
		throw new Exception("wrong data-type");
		return D3DXVECTOR3(0,0,0);
	}
	if (data==NULL)
	{
		data = new D3DXVECTOR3;
	}
	return *(D3DXVECTOR3*)data;
}

const D3DXQUATERNION& ScriptNode::GetQuat()
{
	if (type != Quat)
	{
		throw new Exception("wrong data-type");
		return D3DXQUATERNION(0,0,0,0);
	}
	if (data==NULL)
	{
		data = new D3DXQUATERNION;
	}
	return *(D3DXQUATERNION*)data;
}

void ScriptNode::GetWhileData(ScriptNode*& expr,ScriptNode*& compound)
{
	PreCond(type == While);
	PreCond(nodes.size() == 2);
	expr = nodes[0];
	PreCond(nodes[1]->type == CompoundStatement);
	compound = nodes[1];
}

void ScriptNode::GetIfData(ScriptNode*& expr,ScriptNode*& compound,
						   std::vector<ScriptNode*>& elseList)
{
	PreCond(type == If);
	PreCond(nodes.size() >= 2);
	expr = nodes[0];
	PreCond(nodes[1]->type == CompoundStatement);
	compound = nodes[1];

	elseList.clear();
	for (int i=2; i<nodes.size(); i++)
	{
		elseList.push_back(nodes[i]);
	}
}

void ScriptNode::GetElseIfData(ScriptNode*& expr,ScriptNode*& compound)
{
	PreCond(type == ElseIf);
	PreCond(nodes.size() == 2);

	expr = nodes[0];
	PreCond(nodes[1]->type == CompoundStatement);
	compound = nodes[1];
}

void ScriptNode::GetElseData(ScriptNode*& compound)
{
	PreCond(type == Else);
	PreCond(nodes.size() == 1);

	PreCond(nodes[0]->type == CompoundStatement);
	compound = nodes[0];
}

void ScriptNode::GetVariableDeclaration(std::string& ident,ScriptNode::Type& _type)
{
	PreCond(type == VariableDeclaration);
	PreCond(nodes.size() == 2);

	_type = nodes[0]->type;
	PreCond(nodes[1]->type == String);
	ident = nodes[1]->GetString();
}

void ScriptNode::GetAssignment(ScriptNode*& ident,ScriptNode*& expr)
{
	PreCond(type == Assignment);
	PreCond(nodes.size() == 2);

	PreCond(nodes[0]->type == String || nodes[0]->type == Accessor);
	ident = nodes[0];
	expr = nodes[1];
}

void ScriptNode::GetFunctionCall(std::string& ident,std::vector<ScriptNode*>& params)
{
	PreCond(type == FunctionCall);
	PreCond(nodes.size() == 2);

	PreCond(nodes[0]->type == String);
	ident = nodes[0]->GetString();

	PreCond(nodes[1]->type == ParameterList);
	params = nodes[1]->nodes;
}

void ScriptNode::GetIdentifier(std::string& name)
{
	PreCond(type == Identifier);
	PreCond(nodes.size() == 1);

	PreCond(nodes[0]->type == String);
	name = nodes[0]->GetString();
}

void ScriptNode::GetAccessor(std::string& ident,Type& atype)
{
	PreCond(type == Accessor);
	PreCond(nodes.size() == 2);

	PreCond(nodes[0]->type == String);
	ident = nodes[0]->GetString();
	atype = nodes[1]->type;
}

void ScriptNode::GetReturnData(ScriptNode*& expr)
{
	PreCond(type == Return);
	PreCond(nodes.size() == 1);
	expr = nodes[0];
}

void ScriptNode::SetAccessor(Type atype,float value)
{
	if (type != ScriptNode::Vec2 &&
		type != ScriptNode::Vec3 &&
		type != ScriptNode::Quat)
	{
		throw new Exception("only vec2, vec3, and quat have accessors");
	}

	switch (atype)
	{
		case ScriptNode::X:
		{
			switch (type)
			{
				case ScriptNode::Vec2:
				{
					D3DXVECTOR2* d = (D3DXVECTOR2*)data;
					d->x = value;
					break;
				}
				case ScriptNode::Vec3:
				{
					D3DXVECTOR3* d = (D3DXVECTOR3*)data;
					d->x = value;
					break;
				}
				case ScriptNode::Quat:
				{
					D3DXQUATERNION* d = (D3DXQUATERNION*)data;
					d->x = value;
					break;
				}
			}
			break;
		}
		case ScriptNode::Y:
		{
			switch (type)
			{
				case ScriptNode::Vec2:
				{
					D3DXVECTOR2* d = (D3DXVECTOR2*)data;
					d->y = value;
					break;
				}
				case ScriptNode::Vec3:
				{
					D3DXVECTOR3* d = (D3DXVECTOR3*)data;
					d->y = value;
					break;
				}
				case ScriptNode::Quat:
				{
					D3DXQUATERNION* d = (D3DXQUATERNION*)data;
					d->y = value;
					break;
				}
			}
			break;
		}
		case ScriptNode::Z:
		{
			switch (type)
			{
				case ScriptNode::Vec2:
				{
					throw new Exception("vec2 data-type does not have a 'z'");
					break;
				}
				case ScriptNode::Vec3:
				{
					D3DXVECTOR3* d = (D3DXVECTOR3*)data;
					d->z = value;
					break;
				}
				case ScriptNode::Quat:
				{
					D3DXQUATERNION* d = (D3DXQUATERNION*)data;
					d->z = value;
					break;
				}
			}
			break;
		}
		case ScriptNode::W:
		{
			switch (type)
			{
				case ScriptNode::Vec2:
				{
					throw new Exception("vec2 data-type does not have a 'w'");
					break;
				}
				case ScriptNode::Vec3:
				{
					throw new Exception("vec3 data-type does not have a 'w'");
					break;
				}
				case ScriptNode::Quat:
				{
					D3DXQUATERNION* d = (D3DXQUATERNION*)data;
					d->w = value;
					break;
				}
			}
			break;
		}
	}
}

float ScriptNode::GetAccessor(Type atype)
{
	if (type != ScriptNode::Vec2 &&
		type != ScriptNode::Vec3 &&
		type != ScriptNode::Quat)
	{
		throw new Exception("only vec2, vec3, and quat have accessors");
	}

	switch (atype)
	{
		case ScriptNode::X:
		{
			switch (type)
			{
				case ScriptNode::Vec2:
				{
					return GetVec2().x;
				}
				case ScriptNode::Vec3:
				{
					return GetVec3().x;
				}
				case ScriptNode::Quat:
				{
					return GetQuat().x;
				}
			}
			break;
		}
		case ScriptNode::Y:
		{
			switch (type)
			{
				case ScriptNode::Vec2:
				{
					return GetVec2().y;
				}
				case ScriptNode::Vec3:
				{
					return GetVec3().y;
				}
				case ScriptNode::Quat:
				{
					return GetQuat().y;
				}
			}
			break;
		}
		case ScriptNode::Z:
		{
			switch (type)
			{
				case ScriptNode::Vec2:
				{
					throw new Exception("vec2 data-type does not have a 'z'");
					break;
				}
				case ScriptNode::Vec3:
				{
					return GetVec3().z;
				}
				case ScriptNode::Quat:
				{
					return GetQuat().z;
				}
			}
			break;
		}
		case ScriptNode::W:
		{
			switch (type)
			{
				case ScriptNode::Vec2:
				{
					throw new Exception("vec2 data-type does not have a 'w'");
					break;
				}
				case ScriptNode::Vec3:
				{
					throw new Exception("vec3 data-type does not have a 'w'");
					break;
				}
				case ScriptNode::Quat:
				{
					return GetQuat().w;
				}
			}
			break;
		}
	}
	return 0;
}

