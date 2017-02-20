#pragma once

//////////////////////////////////////////////////////////

class Object
{
public:
	Object();
	virtual ~Object();
	Object(const Object&);
	const Object& operator=(const Object&);

	// object types across the whole system
	enum ObjectType
	{
		OT_FIRST=0,

		OT_SCENEROOT=0,
		OT_CAMERA,
		OT_WORLDOBJECT,
		OT_PRS,
		OT_LIGHT,
		OT_MODEL,
		OT_ANIMATION,
		OT_SKIN,
		OT_BONE,

		// game specific
		OT_MODELMAP,
		OT_SHIP,
		OT_BASE,
		OT_INDESTRUCTABLEREGION,

		// for now - thats the number of maximum types
		OT_LAST = 32
	};

	// get set oid
	int GetOid() const;
	void SetOid(int oid);

	// get set name
	const std::string& GetName() const;
	void SetName(const std::string& name);

	// set and get type
	void SetType(ObjectType otype);

	// see if an object is of a particular type
	bool Isa(ObjectType otype);

	// save and load object
	virtual void Write(BaseStreamer&);
	virtual void Read(BaseStreamer&);

private:
	// all objects have a type set
	std::bitset<OT_LAST> type;

	// name
	std::string name;

	// oid id of object
	int oid;

	// file management particulars for this object
	static std::string objectSignature;
	static int objectVersion;
};

