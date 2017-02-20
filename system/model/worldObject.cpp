#include "standardFirst.h"

#include "system/model/worldObject.h"

#include "system/interpreter/script.h"
#include "system/interpreter/interpreter.h"

std::string WorldObject::worldObjectSignature="worldObject";
int WorldObject::worldObjectVersion=1;

/////////////////////////////////////////////////////////////////////////////

WorldObject::WorldObject()
{
	SetType(OT_WORLDOBJECT);
	SetWorldType(TYPE_CONTAINER);

	visible = true;
	scriptable = false;
	scriptEnabled = false;
}

WorldObject::~WorldObject()
{
}

WorldObject::WorldObject(const WorldObject& w)
{
	operator=(w);
}

const WorldObject& WorldObject::operator=(const WorldObject& w)
{
	PRS::operator =(w);

	visible = w.visible;
	scriptable = w.scriptable;
	scriptEnabled = w.scriptEnabled;

	worldType = w.worldType;

	// code related
	code = w.code;

	// do not persist - cache
	symbolTable = w.symbolTable;
	KeyUp = w.KeyUp;
	KeyDown = w.KeyDown;
	Logic = w.Logic;
	Init = w.Init;
	functions = w.functions;

	return *this;
}

Function* WorldObject::GetFunction(const std::string& name)
{
	for (int i=0; i<functions.size(); i++)
	{
		if (functions[i].name == name)
			return &functions[i];
	}
	return NULL;
}

void WorldObject::UpdateCode()
{
	// reset
	symbolTable.Clear();
	KeyDown.Clear();
	KeyUp.Clear();
	Init.Clear();
	Logic.Clear();

	// parse
	Script s;
	std::vector<ScriptNode*> list = s.ParseFunctionsOnly(code);
	if (s.HasErrors())
		throw new Exception(s.GetError());

	// go through list and build symbol table and event routines
	SymbolNode sym;
	for (int i=0; i<list.size(); i++)
	{
		ScriptNode::Type type = list[i]->type;
		switch (type)
		{
			case ScriptNode::VariableDeclaration:
			{
				list[i]->GetVariableDeclaration(sym.name,sym.value.type);
				symbolTable.Add(sym);
				break;
			}
			case ScriptNode::KeyDownEvent:
			{
				KeyDown = Script::GetEventFunction(list[i]);
				break;
			}
			case ScriptNode::KeyUpEvent:
			{
				KeyUp = Script::GetEventFunction(list[i]);
				break;
			}
			case ScriptNode::InitEvent:
			{
				Init = Script::GetEventFunction(list[i]);
				break;
			}
			case ScriptNode::LogicEvent:
			{
				Logic = Script::GetEventFunction(list[i]);
				break;
			}
			case ScriptNode::Function:
			{
				functions.push_back(Script::GetFunction(list[i]));
				break;
			}
		}
	}

	// execute initialisation code
	EventInit();
}

void WorldObject::EventKeyDown(int key,bool shift,bool ctrl)
{
	if (!KeyDown.actions.empty() && scriptEnabled)
	{
		KeyDown.symbolTable.Clear();
		KeyDown.symbolTable.Add(SymbolNode("key",ScriptNode::Int));
		KeyDown.symbolTable.Add(SymbolNode("shift",ScriptNode::Bool));
		KeyDown.symbolTable.Add(SymbolNode("ctrl",ScriptNode::Bool));
		KeyDown.symbolTable.SetValue("key",key);
		KeyDown.symbolTable.SetValue("shift",shift);
		KeyDown.symbolTable.SetValue("ctrl",ctrl);
		ScriptNode result;
		Interpreter::ExecuteFunction(this,KeyDown,symbolTable,result);
	}
}

void WorldObject::EventKeyUp(int key,bool shift,bool ctrl)
{
	if (!KeyUp.actions.empty() && scriptEnabled)
	{
		KeyUp.symbolTable.Clear();
		KeyUp.symbolTable.Add(SymbolNode("key",ScriptNode::Int));
		KeyUp.symbolTable.Add(SymbolNode("shift",ScriptNode::Bool));
		KeyUp.symbolTable.Add(SymbolNode("ctrl",ScriptNode::Bool));
		KeyUp.symbolTable.SetValue("key",key);
		KeyUp.symbolTable.SetValue("shift",shift);
		KeyUp.symbolTable.SetValue("ctrl",ctrl);
		ScriptNode result;
		Interpreter::ExecuteFunction(this,KeyUp,symbolTable,result);
	}
}

void WorldObject::EventLogic(double time)
{
	if (!Logic.actions.empty() && scriptEnabled)
	{
		Logic.symbolTable.Clear();
		ScriptNode result;
		Interpreter::ExecuteFunction(this,Logic,symbolTable,result);
	}
}

void WorldObject::EventInit()
{
	if (!Init.actions.empty() && scriptEnabled)
	{
		Init.symbolTable.Clear();
		ScriptNode result;
		Interpreter::ExecuteFunction(this,Init,symbolTable,result);
	}
}

WorldObject::WorldType WorldObject::GetWorldType()
{
	return worldType;
}

void WorldObject::SetWorldType(WorldType _worldType)
{
	worldType=_worldType;
}

std::string WorldObject::GetCode() const
{
	return code;
}

void WorldObject::SetCode(std::string _code)
{
	code = _code;
}

bool WorldObject::GetVisible() const
{
	return visible;
}

void WorldObject::SetVisible(bool _visible)
{
	visible=_visible;
}

bool WorldObject::GetScriptable() const
{
	return scriptable;
}

bool WorldObject::GetScriptEnabled() const
{
	return scriptEnabled;
}

void WorldObject::SetScriptEnabled(bool _scriptEnabled)
{
	scriptEnabled = _scriptEnabled;
}

void WorldObject::SetScriptable(bool _scriptable)
{
	scriptable = _scriptable;
	if (scriptable && code.empty())
	{
		code = "Init()\r\n{\r\n}\r\n\r\n";
		code = code + "KeyDown(int key, bool shift, bool ctrl)\r\n{\r\n}\r\n\r\n";
		code = code + "KeyUp(int key, bool shift, bool ctrl)\r\n{\r\n}\r\n\r\n";
		code = code + "Logic()\r\n{\r\n}\r\n\r\n";
	}
	scriptEnabled = scriptable;
}

void WorldObject::Draw(double time)
{
}

void WorldObject::Write(BaseStreamer& _f)
{
	BaseStreamer& f = _f.NewChild(worldObjectSignature,worldObjectVersion);

	PRS::Write(f);

	f.Write("visible",visible);
	f.Write("scriptable",scriptable);
	f.Write("scriptenabled",scriptEnabled);

	if (!code.empty())
	{
		std::string codeStr = System::EncodeBase64(code);
		f.Write("code",codeStr);
	}
	else
	{
		f.Write("code",code);
	}

	int t=worldType;
	f.Write("worldType",t);
}

void WorldObject::Read(BaseStreamer& _f)
{
	BaseStreamer& f = _f.GetChild(worldObjectSignature,worldObjectVersion);

	PRS::Read(f);

	f.Read("visible",visible);
	f.Read("scriptable",scriptable);
	f.Read("scriptenabled",scriptEnabled);

	std::string codeStr;
	f.Read("code",codeStr);
	if (!codeStr.empty())
	{
		code = System::DecodeBase64(codeStr);
	}
	else
	{
		code.clear();
	}

	int t;
	f.Read("worldType",t);
	worldType=WorldType(t);
}


