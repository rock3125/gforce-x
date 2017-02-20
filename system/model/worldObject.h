#pragma once

#include "system/model/prs.h"

#include "system/interpreter/symbolTable.h"
#include "system/interpreter/function.h"

/////////////////////////////////////////////////////////////////////////////

class WorldObject : public PRS
{
public:
	WorldObject();
	virtual ~WorldObject();
	WorldObject(const WorldObject&);
	const WorldObject& operator=(const WorldObject&);

	enum WorldType
	{
		TYPE_FIRST		= 0,

		TYPE_CONTAINER	= 0,
		TYPE_MODEL,
		TYPE_LIGHT,
		TYPE_CAMERA,
		TYPE_SKIN,
		TYPE_BONE,

		// game specific
		TYPE_MODELMAP,
		TYPE_SHIP,
		TYPE_BASE,
		TYPE_INDESTRUCTABLEREGION,

		TYPE_LAST,		// last of the predefined types

		TYPE_CUSTOM		= 100
	};

	// draw whatever it is
	virtual void Draw(double time);

	// re-parse the code and build symbol table
	// and event routines
	void UpdateCode();

	// event implementations
	virtual void EventKeyDown(int key,bool shift,bool ctrl);
	virtual void EventKeyUp(int key,bool shift,bool ctrl);
	virtual void EventLogic(double time);
	virtual void EventInit();
	Function* GetFunction(const std::string& name);

	// get set world type
	WorldType GetWorldType();
	void SetWorldType(WorldType type);

	// get set code of world object
	std::string GetCode() const;
	void SetCode(std::string code);

	// get set is visible property
	bool GetVisible() const;
	void SetVisible(bool visible);

	// get set scriptable property
	bool GetScriptable() const;
	void SetScriptable(bool scriptable);

	// get set scriptable property
	bool GetScriptEnabled() const;
	void SetScriptEnabled(bool enabled);

	// save and load WorldObject
	virtual void Read(BaseStreamer&);
	virtual void Write(BaseStreamer&);

protected:
	// event code and functions for this object
	std::string code;

	SymbolTable symbolTable;

	// main event routines (do not persist)
	Function KeyDown;
	Function KeyUp;
	Function Logic;
	Function Init;

	// locally defined functions
	std::vector<Function> functions;

	// is it visible
	bool visible;

	// does it have script?
	bool scriptable;
	// is the script enabled
	bool scriptEnabled;

	// world object type
	WorldType worldType;

	// signature and version
	static std::string worldObjectSignature;
	static int worldObjectVersion;
};

