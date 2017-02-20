#pragma once

#include "system/model/worldObject.h"

class WorldObject;
class Camera;

/////////////////////////////////////////////////////////////////////////////

class Level : public Object
{
public:
	Level();
	virtual ~Level();
	Level(const Level&);
	const Level& operator=(const Level&);

	// access level's scene root
	WorldObject* GetSceneRoot();

	// get object by oid
	WorldObject* GetObject(int oid);

	// access all objects
	std::vector<WorldObject*> GetObjects();

	// remove an object from the level for ever
	void RemoveObjectFromScene(int oid);

	// add a new object to the scene
	void AddObjectToScene(WorldObject* parent,WorldObject* obj);

	// count the number of types for this object
	int ObjectTypeCount(WorldObject::WorldType type);

	// clear all objects allocated in a level
	void ClearLevel();

	// rebuild the parents
	void ResetParents();

	// save and load level
	void Read(XmlNode* level);
	XmlNode* Write();

	// given a camera and a mouse pos, pick an object from the scene
	WorldObject* Pick(Camera* camera,const D3DXVECTOR2& mousePos,bool getClosest);

	// return the signature of this object
	virtual std::string Signature()
	{
		return levelSignature;
	}

protected:
	void RebuildObjectMap();

	// helper for AddObjectToScene
	void RecursiveAddChildrenToObjects(WorldObject* obj);

protected:
	// the scene root of the system
	WorldObject*	sceneRoot;
	int				sceneRootOid;

	// quick map from oid to object
	stdext::hash_map<int,WorldObject*> objectMap;

	// all objects
	std::vector<WorldObject*> objects;

	static std::string levelSignature;
	static int levelVersion;
};

