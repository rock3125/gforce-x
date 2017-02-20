#include "standardFirst.h"

#include "system/xml/XmlParser.h"

///////////////////////////////////////////////////////////////////////////////////

XmlParser::XmlParser()
	: root(NULL)
{
	EnableDoubleQuoteString();
	EnableSingleQuoteString();
	EnableIdent();

	Add("<",false);
	Add("</",false);
	Add(">",false);
	Add("/",false);
	Add("/>",false);
	Add("=",false);
}

XmlParser::~XmlParser()
{
	safe_delete(root);
}

XmlNode* XmlParser::GetDocumentRoot()
{
	return root;
}

const XmlParser& XmlParser::operator=(const XmlParser& tp)
{
	return *this;
}

XmlParser::XmlParser(const XmlParser& tp)
{
	operator=(tp);
}

void XmlParser::Save(const std::string& fname)
{
	FileStreamer file(BaseStreamer::STREAM_WRITE);
	if (file.Open(fname))
	{
		if (root != NULL)
		{
			std::string str = root->ToString();
			file.Write(str.c_str(),(int)str.length());
		}
		file.Close();
	}
}

bool XmlParser::LoadAndParse(const std::string& fname)
{
	safe_delete(root);
	Tokeniser::Load(fname);
	if (!error)
	{
		root = Xml();
	}
	else
	{
	}
	return !error;
}

void XmlParser::Parse(const std::string& xmlString)
{
	safe_delete(root);
	Tokeniser::Parse(xmlString);
	if (!error)
	{
		root = Xml();
	}
}

// xml ->		'<' ident [ident ['=' string]]* '>' xmlNode '<' '/' ident '>' |
//				'<' ident [ident ['=' string]]* '>' text '<' '/' ident '>' |
//				'<' ident [ident ['=' string]]* '/>' |
//				'<' ident [ident ['=' string]]* '/>' |
//				{e}
XmlNode* XmlParser::Xml()
{
	token=GetNextToken();
	if (token != Get("<"))
	{
		UngetToken();
		return NULL;
	}

	token=GetNextToken();
	if (error) { return NULL; }
	if (token != TIDENT)
	{
		SetError("expected identifier");
		return NULL;
	}
	std::string ident=yyString;

	token=GetNextToken();
	if (token == TIDENT)
	{
		XmlNode* node=NULL;
		do
		{
			std::string name = yyString;
			std::string value;

			token=GetNextToken();
			if (error) { safe_delete(node); return NULL; }
			if (token == Get("="))
			{
				token=GetNextToken();
				if (error) { return NULL; }
				if (token != TIDENT && token != TSTRING)
				{
					SetError("expected identifier after '='");
					safe_delete(node);
					return NULL;
				}
				value = yyString;
			}
			else
			{
				UngetToken();
			}

			if (node == NULL)
			{
				node = new XmlNode(ident,name,value,"");
			}
			else
			{
				node->SetNameValue(name,value);
			}

			token = GetNextToken();
			if (error) { safe_delete(node); return NULL; }
		}
		while (token == TIDENT);

		if (error) { safe_delete(node); return NULL; }
		if (token != Get(">") && token != Get("/>"))
		{
			SetError("expected '>'");
			safe_delete(node);
			return NULL;
		}

		if (token != Get("/>"))
		{
			ParseEndTag(node);
			if (error) { safe_delete(node); return NULL; }
		}
		return node;
	}
	else if (token == Get(">") || token == Get("/>"))
	{
		XmlNode* node = new XmlNode();
		node=new XmlNode();
		node->tag = ident;

		if (token != Get("/>"))
		{
			ParseEndTag(node);
			if (error) { safe_delete(node); return NULL; }
		}
		return node;
	}
	else
	{
		SetError("expected identifier or '>'");
		return NULL;
	}
}

// [text] '<' |
// <xml
void XmlParser::ParseEndTag(XmlNode* node)
{
	char ch[2];
	ch[1] = 0;

	std::string str;
	while (index < bufferSize && buffer[index] != '<')
	{
		ch[0] = buffer[index];
		str = str + ch;
		index++;
	}

	if (node==NULL)
		node = new XmlNode();
	node->innerValue = str;

	if (buffer[index] == '<' && buffer[index+1] != '/')
	{
		node->innerValue.clear();
		std::vector<XmlNode*> children = XmlNodeList();
		for (int i=0; i < children.size(); i++)
		{
			node->children.push_back(children[i]);
		}
	}

	// end tag
	token=GetNextToken();
	if (token != Get("</"))
	{
		SetError("expected '</'");
		return;
	}

	token=GetNextToken();
	if (token != TIDENT)
	{
		SetError("expected '</ identifier'");
		return;
	}
	if (yyString != node->tag)
	{
		SetError("start and end tags do not match");
		return;
	}

	token = GetNextToken();
	if (token != Get(">"))
	{
		SetError("expected '>'");
		return;
	}
}

// xmlNode ->	xml xmlNode |
//				{e}
std::vector<XmlNode*> XmlParser::XmlNodeList()
{
	std::vector<XmlNode*> list;

	XmlNode* xml = NULL;
	do
	{
		xml = Xml();
		if (xml!=NULL)
		{
			list.push_back(xml);
		}
	}
	while (!error && xml!=NULL);

	if (error)
	{
		for (int i=0; i < list.size(); i++)
		{
			safe_delete(list[i]);
		}
		list.clear();
	}
	return list;
}

