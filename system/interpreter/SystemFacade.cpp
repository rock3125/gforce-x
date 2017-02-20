#include "standardFirst.h"

#include "system/model/worldObject.h"

#include "system/interpreter/systemFacade.h"
#include "system/interpreter/script.h"

/////////////////////////////////////////////////////////////////////////////

SystemFacade::SystemFacade()
{
}

SystemFacade::~SystemFacade()
{
}

// print to log (anything)
std::string SystemFacade::ToString(ScriptNode* n)
{
	switch (n->type)
	{
		case ScriptNode::Object:
		{
			WorldObject* obj = (WorldObject*)n->GetObject();
			return obj->GetName();
		}
		case ScriptNode::Int:
		{
			return System::Int2Str(n->GetInt());
		}
		case ScriptNode::Float:
		{
			return System::Float2Str(n->GetFloat());
		}
		case ScriptNode::String:
		{
			return n->GetString();
			break;
		}
		case ScriptNode::Bool:
		{
			std::string str = n->GetBool() ? "true" : "false";
			return str;
		}
		case ScriptNode::Vec2:
		{
			D3DXVECTOR2 v = n->GetVec2();
			std::string str = "vec2("+System::Float2Str(v.x)+","+System::Float2Str(v.y)+")";
			return str;
		}
		case ScriptNode::Vec3:
		{
			D3DXVECTOR3 v = n->GetVec3();
			std::string str = "vec3("+System::Float2Str(v.x)+","+System::Float2Str(v.y)+","+
									System::Float2Str(v.z)+")";
			return str;
		}
		case ScriptNode::Quat:
		{
			D3DXQUATERNION v = n->GetQuat();
			std::string str = "quat("+System::Float2Str(v.x)+","+System::Float2Str(v.y)+","+
									System::Float2Str(v.z)+","+System::Float2Str(v.w)+")";
			return str;
		}
		default:
		{
			return "(not printable)";
		}
	}
}

//
// currently implemented
//
//	-	void Print([object|int|float|vec2|vec3|quat|bool|string])
//	-	int cint(float)
//	-	float cfloat(int)
//	-	string cstr([object|int|float|vec2|vec3|quat|bool])
//	-	vec3 GetPosition(object)
//	-	void SetPosition(object,vec3)
//	-	float sin(float)
//	-	float cos(float)
//  -   void RotateEulerRelative(object,vec3)
//  -   object GetChild(object,string)
//
bool SystemFacade::ExecuteFunction(const std::string& name,ScriptNode& result,
								   std::vector<ScriptNode*>& parameters)
{
	if (name == "Print")
	{
		if (parameters.size() != 1)
		{
			throw new Exception("Print() takes one parameter");
			return false;
		}
		Log::GetLog() << ToString(parameters[0]) << System::CR;
		result.type = ScriptNode::Void;
		return true;
	}
	else if (name == "cint")
	{
		if (parameters.size() != 1)
		{
			throw new Exception("cint() takes one parameter");
			return false;
		}
		if (parameters[0]->type != ScriptNode::Float)
		{
			throw new Exception("cint() parameter 1 must be a float");
			return false;
		}
		result.SetData( (int)parameters[0]->GetFloat() );
		return true;
	}
	else if (name == "cfloat")
	{
		if (parameters.size() != 1)
		{
			throw new Exception("cfloat() takes one parameter");
			return false;
		}
		if (parameters[0]->type != ScriptNode::Int)
		{
			throw new Exception("cint() parameter 1 must be an int");
			return false;
		}
		result.SetData( (float)parameters[0]->GetInt() );
		return true;
	}
	else if (name == "cstr")
	{
		if (parameters.size() != 1)
		{
			throw new Exception("cstr() takes one parameter");
			return false;
		}
		result.SetData( ToString(parameters[0]) );
		return true;
	}
	else if (name == "GetPosition")
	{
		if (parameters.size() != 1)
		{
			throw new Exception("GetPosition() takes one parameter");
			return false;
		}
		if (parameters[0]->type != ScriptNode::Object)
		{
			throw new Exception("GetPosition() parameter 1 must be an object");
			return false;
		}
		WorldObject* obj = (WorldObject*)parameters[0]->GetObject();
		if (obj != NULL)
			result.SetData( obj->GetPosition() );
		return true;
	}
	else if (name == "SetPosition")
	{
		if (parameters.size() != 2)
		{
			throw new Exception("GetPosition() takes one parameter");
			return false;
		}
		if (parameters[0]->type != ScriptNode::Object)
		{
			throw new Exception("GetPosition() parameter 1 must be an object");
			return false;
		}
		if (parameters[1]->type != ScriptNode::Vec3)
		{
			throw new Exception("GetPosition() parameter 2 must be a vec3");
			return false;
		}
		result.type = ScriptNode::Void;
		WorldObject* obj = (WorldObject*)parameters[0]->GetObject();
		if (obj != NULL)
			obj->SetPosition(parameters[1]->GetVec3());
		return true;
	}
	else if (name == "sin")
	{
		if (parameters.size() != 1)
		{
			throw new Exception("sin() takes one parameter");
			return false;
		}
		if (parameters[0]->type != ScriptNode::Float)
		{
			throw new Exception("sin() parameter 1 must be a float");
			return false;
		}
		result.SetData( sinf(parameters[0]->GetFloat()) );
		return true;
	}
	else if (name == "cos")
	{
		if (parameters.size() != 1)
		{
			throw new Exception("sin() takes one parameter");
			return false;
		}
		if (parameters[0]->type != ScriptNode::Float)
		{
			throw new Exception("sin() parameter 1 must be a float");
			return false;
		}
		result.SetData( cosf(parameters[0]->GetFloat()) );
		return true;
	}
	else if (name == "RotateEulerRelative")
	{
		if (parameters.size() != 2)
		{
			throw new Exception("RotateEulerRelative() takes two parameters");
			return false;
		}
		if (parameters[0]->type != ScriptNode::Object)
		{
			throw new Exception("RotateEulerRelative() parameter 1 must be an object");
			return false;
		}
		if (parameters[1]->type != ScriptNode::Vec3)
		{
			throw new Exception("RotateEulerRelative() parameter 2 must be a vec3");
			return false;
		}
		result.type = ScriptNode::Void;
		WorldObject* obj = (WorldObject*)parameters[0]->GetObject();
		if (obj != NULL)
			obj->RotateEuler(parameters[1]->GetVec3());
		return true;
	}
	else if (name == "GetChild")
	{
//  -   object GetChild(object,string)
		if (parameters.size() != 2)
		{
			throw new Exception("GetChild() takes two parameters");
			return false;
		}
		if (parameters[0]->type != ScriptNode::Object)
		{
			throw new Exception("GetChild() parameter 1 must be an object");
			return false;
		}
		if (parameters[1]->type != ScriptNode::String)
		{
			throw new Exception("GetChild() parameter 2 must be a string");
			return false;
		}
		result.type = ScriptNode::Object;
		result.SetData((void*)NULL);
		WorldObject* obj = (WorldObject*)parameters[0]->GetObject();
		std::string name = parameters[1]->GetString();

		if (obj != NULL)
		{
			std::vector<PRS*> children = obj->GetChildren();
			for (int i=0; i<children.size(); i++)
			{
				if (children[i]->GetName() == name)
				{
					result.SetData(dynamic_cast<WorldObject*>(children[i]));
					break;
				}
			}
		}
		return true;
	}
	return false;
}


