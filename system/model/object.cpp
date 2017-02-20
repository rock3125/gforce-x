#include "standardFirst.h"

#include "system/model/object.h"

// statics for a clean namespace

std::string Object::objectSignature="obj";
int Object::objectVersion=1;

//////////////////////////////////////////////////////////

Object::Object()
{
	oid=System::GetNextOid();
}

Object::~Object()
{
}

Object::Object(const Object& o)
{
	oid=System::GetNextOid();
	operator=(o);
}

const Object& Object::operator=(const Object& o)
{
	name=o.name;
	type=o.type;
	return *this;
}

int Object::GetOid() const
{
	return oid;
}

void Object::SetOid(int _oid)
{
	oid=_oid;
}

const std::string& Object::GetName() const
{
	return name;
}

void Object::SetName(const std::string& _name)
{
	name=_name;
}

void Object::SetType(ObjectType otype)
{
	type[otype]=true;
}

bool Object::Isa(ObjectType otype)
{
	return type[otype];
}

XmlNode* Object::Write()
{
	XmlNode* node = XmlNode::NewChild(objectSignature, objectVersion);

	node->Write("name", name);
	node->Write("oid", oid);

	std::string bits;
	for (int i=OT_FIRST; i<OT_LAST; i++)
	{
		bits = bits + (type[i]?"1":"0");
	}
	node->Write("typeSet", bits);
	return node;
}

void Object::Read(XmlNode* node)
{
	XmlNode::CheckVersion(node, objectSignature, objectVersion);

	node->Read("name",name);
	node->Read("oid",oid);

	std::string bits;
	node->Read("typeSet", bits);
	for (int i = OT_FIRST; i < OT_LAST; i++)
	{
		type[i] = (bits[i-OT_FIRST] == '1');
	}
}

