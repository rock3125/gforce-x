#pragma once

#include "system/model/worldObject.h"

/////////////////////////////////////////////////////////////////////////////

class ObjectFactory
{
public:
	// create an object from a factory id
	static WorldObject* CreateObject(WorldObject::WorldType worldType);
	static std::string GetWorldTypeName(WorldObject::WorldType worldType);
	static bool Isa(WorldObject* wo,WorldObject::WorldType worldType);
	static bool IsObjectInCache(WorldObject::WorldType worldType);

	// add a new custom object to the library
	static void AddCustomObject(WorldObject* obj);
	static void AddCustomObject(const std::string& name,const std::string& fname);
	static void RemoveCustomObject(WorldObject::WorldType type);
	static int NumCustomObjects();

	// list of objects that can appear in the creation palettes
	static std::vector<WorldObject::WorldType> GetCreatableObjectList();

	// copy a list of object
	static WorldObject* DeepCopy(WorldObject* wo);

	// loads all objects from library file
	static void Initialise();

private:
	static std::vector<WorldObject*>	customObjects;
	static std::vector<std::string>		customObjectFilenames;
	static std::vector<std::string>		customObjectNames;
};

