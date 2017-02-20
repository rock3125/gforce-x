#pragma once

#include "system/streamers/baseStreamer.h"

///////////////////////////////////////////////////////////////////////////////////

class XmlNode : public BaseStreamer
{
	XmlNode(const XmlNode&);
	const XmlNode& operator=(const XmlNode&);
public:
	XmlNode();
	XmlNode(const std::string& _tag, const std::string& _name, 
			const std::string& _value, const std::string& _simpleValue);
	virtual ~XmlNode();

	// convert contents to string
	std::string ToString();

	// create a new child and return a pointer to it and add it to
	// my list of children
	virtual BaseStreamer& NewChild(const std::string& tag,int version);
	virtual BaseStreamer& GetChild(const std::string& tag,int version);
	virtual BaseStreamer& GetChild(const std::string& tag);

	// access all children
	std::vector<XmlNode*> GetChildren();

	// get a child by name
	XmlNode* GetChildNode(const std::string& tag);

	// set name value pair in node, if dne, add a new pair
	void SetNameValue(const std::string& name,const std::string& value);

	// get a name's associated value
	std::string GetValue(const std::string& name);

	// do we have a name entry?
	bool HasName(const std::string& name);

	// do we have a particular name value pair?
	bool HasNameValue(const std::string& name,const std::string& value);

	// Get set simple values
	const std::string& GetInnerValue() const;
	void SetInnerValue(const std::string& _simpleValue);

	// get set tag
	const std::string& GetTag() const;
	void SetTag(const std::string& _tag);

	// version management system - throws Exception
	virtual void WriteNode(const std::string& name,int version);
	virtual int ReadNode(const std::string& name,int version);

	// streamer support
	virtual void WriteData(const std::string& tag,const void* buf,int bytes);
	virtual bool ReadData(const std::string& tag,void* buf,int bytes);

	virtual void Read(const std::string& tag,bool&);
	virtual void Read(const std::string& tag,int&);
	virtual void Read(const std::string& tag,float&);
	virtual void Read(const std::string& tag,double&);
	virtual void Read(const std::string& tag,short&);
	virtual void Read(const std::string& tag,unsigned char&);
	virtual void Read(const std::string& tag,std::string&);
	virtual void Read(const std::string& tag,D3DXVECTOR2&);
	virtual void Read(const std::string& tag,D3DXVECTOR3&);
	virtual void Read(const std::string& tag,D3DXQUATERNION&);
	virtual void Read(const std::string& tag,D3DXMATRIXA16&);
	virtual void Read(const std::string& tag,D3DXCOLOR&);
	virtual void Read(const std::string& tag,D3DPVERTEX&);
	virtual void Read(const std::string& tag,D3DPINDEX&);

	virtual void Write(const std::string& tag,bool);
	virtual void Write(const std::string& tag,int);
	virtual void Write(const std::string& tag,float);
	virtual void Write(const std::string& tag,double);
	virtual void Write(const std::string& tag,short);
	virtual void Write(const std::string& tag,unsigned char);
	virtual void Write(const std::string& tag,const std::string&);
	virtual void Write(const std::string& tag,const D3DXVECTOR2&);
	virtual void Write(const std::string& tag,const D3DXVECTOR3&);
	virtual void Write(const std::string& tag,const D3DXQUATERNION&);
	virtual void Write(const std::string& tag,const D3DXMATRIXA16&);
	virtual void Write(const std::string& tag,const D3DXCOLOR&);
	virtual void Write(const std::string& tag,const D3DPVERTEX&);
	virtual void Write(const std::string& tag,const D3DPINDEX&);

private:
	// helpers
	static void RecursiveToString(XmlNode* xml,StringBuilder& sb,int indent);
	XmlNode* GetChildRecursive(XmlNode* b,const std::string& tag);

private:
	friend class XmlParser;

	// <identifier> of node
	std::string tag;

	// name=value (optional)
	stdext::hash_map<std::string, std::string > values;

	// >simple string value<
	std::string innerValue;

	// children by name
	std::vector<XmlNode*> children;
};

