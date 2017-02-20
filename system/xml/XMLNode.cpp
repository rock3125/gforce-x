#include "standardFirst.h"

// for base64
#include <atlenc.h>

#include "system/xml/XmlNode.h"

///////////////////////////////////////////////////////////////////////////////////

XmlNode::XmlNode()
{
}

XmlNode::XmlNode(const std::string& _tag,const std::string& _name,
				 const std::string& _value,const std::string& _innerValue)
{
	tag=_tag;
	values[_name] = _value;
	innerValue=_innerValue;
}

XmlNode::~XmlNode()
{
	std::vector<XmlNode*>::iterator pos = children.begin();
	while (pos != children.end())
	{
		if ((*pos) != NULL)
			delete (*pos);
		pos++;
	}
	children.clear();
	values.clear();
}

std::vector<XmlNode*> XmlNode::GetChildren()
{
	return children;
}

void XmlNode::RecursiveToString(XmlNode* xml, StringBuilder& sb, int indent)
{
	if (xml==NULL)
		return;

	std::string str;
	std::string identStr;
	for (int i=0; i<indent; i++)
	{
		identStr=identStr+"  ";
	}

	if (!xml->GetTag().empty())
	{
		sb.Append(identStr+"<");
		sb.Append(xml->GetTag());

		stdext::hash_map<std::string, std::string >::iterator pos = xml->values.begin();
		while (pos != xml->values.end())
		{
			sb.Append(" " + (*pos).first);
			sb.Append("=\"" + (*pos).second + "\"");
			pos++;
		}
		if (!(xml->children.empty() && xml->innerValue.empty()))
		{
			sb.Append(">");
		}
	}

	if (xml->children.empty())
	{
		sb.Append(xml->innerValue);
	}
	else
	{
		if (!xml->GetTag().empty())
		{
			sb.Append("\n");
		}
		std::vector<XmlNode*>::iterator pos = xml->children.begin();
		while (pos != xml->children.end())
		{
			RecursiveToString((*pos), sb, indent + 1);
			pos++;
		}
		sb.Append(identStr);
	}

	if (!xml->GetTag().empty())
	{
		if (xml->children.empty() && xml->innerValue.empty())
		{
			sb.Append(" />\n");
		}
		else
		{
			sb.Append("</"+xml->GetTag()+">\n");
		}
	}
}

std::string XmlNode::ToString()
{
	StringBuilder sb;
	RecursiveToString(this,sb,0);
	return sb.ToString();
}

void XmlNode::SetNameValue(const std::string& name, const std::string& value)
{
	values[name] = value;
}

std::string XmlNode::GetValue(const std::string& name)
{
	stdext::hash_map<std::string, std::string >::iterator pos = values.find(name);
	if (pos != values.end())
	{
		return (*pos).second;
	}
	return "";
}

bool XmlNode::HasName(const std::string& name)
{
	stdext::hash_map<std::string, std::string >::iterator pos = values.find(name);
	return (pos != values.end());
}

bool XmlNode::HasNameValue(const std::string& name, const std::string& value)
{
	stdext::hash_map<std::string, std::string >::iterator pos = values.find(name);
	return (pos != values.end());
	return (*pos).second == value;
}

const std::string& XmlNode::GetInnerValue() const
{
	return innerValue;
}

void XmlNode::SetInnerValue(const std::string& _innerValue)
{
	innerValue=_innerValue;
}

const std::string& XmlNode::GetTag() const
{
	return tag;
}

void XmlNode::SetTag(const std::string& _tag)
{
	tag=_tag;
}

XmlNode* XmlNode::NewChild(const std::string& tag, int version)
{
	XmlNode* n = new XmlNode();
	n->tag = tag;
	n->values["version"] = System::Int2Str(version);
	return n;
}

void XmlNode::Add(XmlNode* node)
{
	children.push_back(node);
}

XmlNode* XmlNode::GetChildRecursive(XmlNode* b, const std::string& tag)
{
	if (b == NULL || b->tag == tag)
		return b;

	for (int i=0; i < b->children.size(); i++)
	{
		XmlNode* ret = GetChildRecursive(b->children[i], tag);
		if (ret != NULL)
			return ret;
	}
	return NULL;
}

void XmlNode::CheckVersion(XmlNode* node, std::string tag, int versionNumber)
{
	if (node == NULL)
	{
		throw new Exception("node is NULL");
	}
	if (node->GetTag() != tag)
	{
		throw new Exception("node tag incorrect, expected \"" + tag + "\", found \"" + node->GetTag() + "\"");
	}

	std::string version = node->GetValue("version");
	if (version != System::Int2Str(versionNumber))
	{
		std::string errStr = "node with tag " + tag + " actual version \"" + version;
		errStr = errStr + "\", expected version \"" + System::Int2Str(versionNumber) + "\"";
		throw new Exception(errStr);
	}
}

XmlNode* XmlNode::GetChild(const std::string& tag, int version)
{
	XmlNode* node = GetChildRecursive(this,tag);
	if (node == NULL)
		throw new Exception("node '" + this->tag + "' does not have a child '" + tag + "'");

	if (!node->HasName("version"))
		throw new Exception("version tag missing");

	int version2 = System::Str2Int(node->GetValue("version"));
	if (version2 > version)
		throw new Exception("version too new, update your client");
	return node;
}

bool XmlNode::HasChild(const std::string& tag)
{
	XmlNode* node = GetChildRecursive(this,tag);
	return (node != NULL);
}

XmlNode* XmlNode::GetChild(const std::string& tag)
{
	XmlNode* node = GetChildRecursive(this,tag);
	if (node == NULL)
		throw new Exception("node " + this->tag + " does not have a child " + tag);
	return node;
}

XmlNode* XmlNode::GetChildNode(const std::string& tag)
{
	return GetChildRecursive(this,tag);
}

void XmlNode::WriteNode(const std::string& name,int version)
{
	tag = name;
	values["version"] = System::Int2Str(version);
}

int XmlNode::ReadNode(const std::string& name,int version)
{
	if (tag != name)
		throw new Exception("incorrect node type/name");
	if (!HasName("version"))
		throw new Exception("node missing version information");
	int v = System::Str2Int(GetValue("version"));
	if (v > version)
		throw new Exception("node version too new");
	return v;
}

void XmlNode::WriteData(const std::string& tag,const void* buf,int bytes)
{
	static char bufStr[System::MAX_STRING_SIZE*2];
	PreCond(bytes < System::MAX_STRING_SIZE);
	int s = System::MAX_STRING_SIZE*2;
	PreCond(::Base64Encode((const byte*)buf,bytes,bufStr,&s));
	values[tag] = bufStr;
}

bool XmlNode::ReadData(const std::string& tag,void* buf,int bytes)
{
	PreCond(HasName(tag));
	std::string val = GetValue(tag);
	PreCond(val.size()>0);

	int b = bytes;
	PreCond(::Base64Decode(val.c_str(),val.size(),(byte*)buf,&b));

	return true;
}

void XmlNode::Read(const std::string& tag,bool& b)
{
	if (HasName(tag))
	{
		std::string val = GetValue(tag);
		b = (val=="true" || val=="True" || val=="t" || val=="1");
	}
	else
	{
		b = false;
	}
}

void XmlNode::Read(const std::string& tag,int& i)
{
	PreCond(HasName(tag));
	i = System::Str2Int(GetValue(tag));
}

void XmlNode::Read(const std::string& tag,float& f)
{
	PreCond(HasName(tag));
	f = System::Str2Float(GetValue(tag));
}

void XmlNode::Read(const std::string& tag,double& d)
{
	PreCond(HasName(tag));
	d = System::Str2Float(GetValue(tag));
}

void XmlNode::Read(const std::string& tag,short& s)
{
	PreCond(HasName(tag));
	s = System::Str2Int(GetValue(tag));
}

void XmlNode::Read(const std::string& tag,unsigned char& c)
{
	PreCond(HasName(tag));
	c = (unsigned char)System::Str2Int(GetValue(tag));
}

void XmlNode::Read(const std::string& tag,std::string& str)
{
	PreCond(HasName(tag));
	str = GetValue(tag);
}

void XmlNode::Read(const std::string& tag,D3DXVECTOR2& v2)
{
	PreCond(HasName(tag));
	std::string str = GetValue(tag);
	v2.x = System::Str2Float(System::GetItem(str,',',0));
	v2.y = System::Str2Float(System::GetItem(str,',',1));
}

void XmlNode::Read(const std::string& tag,D3DXVECTOR3& v3)
{
	PreCond(HasName(tag));
	std::string str = GetValue(tag);
	v3.x = System::Str2Float(System::GetItem(str,',',0));
	v3.y = System::Str2Float(System::GetItem(str,',',1));
	v3.z = System::Str2Float(System::GetItem(str,',',2));
}

void XmlNode::Read(const std::string& tag,D3DXQUATERNION& q)
{
	PreCond(HasName(tag));
	std::string str = GetValue(tag);
	q.x = System::Str2Float(System::GetItem(str,',',0));
	q.y = System::Str2Float(System::GetItem(str,',',1));
	q.z = System::Str2Float(System::GetItem(str,',',2));
	q.w = System::Str2Float(System::GetItem(str,',',3));
}

void XmlNode::Read(const std::string& tag,D3DXMATRIXA16& m)
{
	PreCond(HasName(tag));
	std::string str = GetValue(tag);
	for (int i=0; i<16; i++)
	{
		m[i] = System::Str2Float(System::GetItem(str,',',i));
	}
}

void XmlNode::Read(const std::string& tag,D3DXCOLOR& c)
{
	PreCond(HasName(tag));
	std::string str = GetValue(tag);
	c.r = System::Str2Float(System::GetItem(str,',',0));
	c.g = System::Str2Float(System::GetItem(str,',',1));
	c.b = System::Str2Float(System::GetItem(str,',',2));
	c.a = System::Str2Float(System::GetItem(str,',',3));
}

void XmlNode::Read(const std::string& tag,D3DPVERTEX& v)
{
	PreCond(HasName(tag));
	std::string str = GetValue(tag);
	v.position.x = System::Str2Float(System::GetItem(str,',',0));
	v.position.y = System::Str2Float(System::GetItem(str,',',1));
	v.position.z = System::Str2Float(System::GetItem(str,',',2));
	v.normal.x = System::Str2Float(System::GetItem(str,',',3));
	v.normal.y = System::Str2Float(System::GetItem(str,',',4));
	v.normal.z = System::Str2Float(System::GetItem(str,',',5));
	v.texture.x = System::Str2Float(System::GetItem(str,',',6));
	v.texture.y = System::Str2Float(System::GetItem(str,',',7));
}

void XmlNode::Read(const std::string& tag,D3DPINDEX& i)
{
	PreCond(HasName(tag));
	std::string str = GetValue(tag);
	i.v1 = System::Str2Int(System::GetItem(str,',',0));
	i.v2 = System::Str2Int(System::GetItem(str,',',1));
	i.v3 = System::Str2Int(System::GetItem(str,',',2));
}

void XmlNode::Write(const std::string& tag,bool b)
{
	SetNameValue(tag,b?"True":"False");
}

void XmlNode::Write(const std::string& tag,int i)
{
	SetNameValue(tag,System::Int2Str(i));
}

void XmlNode::Write(const std::string& tag,float f)
{
	SetNameValue(tag,System::Float2Str(f));
}

void XmlNode::Write(const std::string& tag,double d)
{
	SetNameValue(tag,System::Float2Str(d));
}

void XmlNode::Write(const std::string& tag,short s)
{
	SetNameValue(tag,System::Int2Str(s));
}

void XmlNode::Write(const std::string& tag,unsigned char c)
{
	int i = c;
	if (i<0) i = -i;
	SetNameValue(tag,System::Int2Str(i));
}

void XmlNode::Write(const std::string& tag,const std::string& str)
{
	SetNameValue(tag,str);
}

void XmlNode::Write(const std::string& tag,const D3DXVECTOR2& v2)
{
	std::string s = System::Float2Str(v2.x)+","+System::Float2Str(v2.y);
	SetNameValue(tag,s);
}

void XmlNode::Write(const std::string& tag,const D3DXVECTOR3& v3)
{
	std::string s = System::Float2Str(v3.x)+","+System::Float2Str(v3.y)+","+System::Float2Str(v3.z);
	SetNameValue(tag,s);
}

void XmlNode::Write(const std::string& tag,const D3DXQUATERNION& q)
{
	std::string s = System::Float2Str(q.x)+","+System::Float2Str(q.y)+
					","+System::Float2Str(q.z)+","+System::Float2Str(q.w);
	SetNameValue(tag,s);
}

void XmlNode::Write(const std::string& tag,const D3DXMATRIXA16& m)
{
	std::string s;
	for (int i=0; i<16; i++)
	{
		if ((i+1)<16)
			s = s + ",";
		s = s + System::Float2Str(m[i]);
	}
	SetNameValue(tag,s);
}

void XmlNode::Write(const std::string& tag,const D3DXCOLOR& c)
{
	std::string s = System::Float2Str(c.r)+","+System::Float2Str(c.g)+
					","+System::Float2Str(c.b)+","+System::Float2Str(c.a);
	SetNameValue(tag,s);
}

void XmlNode::Write(const std::string& tag,const D3DPVERTEX& v)
{
	std::string s = System::Float2Str(v.position.x)+","+System::Float2Str(v.position.y)+
					","+System::Float2Str(v.position.z)+
					","+System::Float2Str(v.normal.x)+","+System::Float2Str(v.normal.y)+
					","+System::Float2Str(v.normal.z)+
					","+System::Float2Str(v.texture.x)+","+System::Float2Str(v.texture.y);
	SetNameValue(tag,s);
}

void XmlNode::Write(const std::string& tag,const D3DPINDEX& i)
{
	std::string s = System::Int2Str(i.v1)+","+System::Int2Str(i.v2)+","+System::Int2Str(i.v3);
	SetNameValue(tag,s);
}

