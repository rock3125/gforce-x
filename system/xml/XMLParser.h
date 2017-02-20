#pragma once

#include "system/xml/XmlNode.h"
#include "system/tokeniser.h"

///////////////////////////////////////////////////////////////////////////////////
//
// XML grammar: (starts with xml)
//
// xml ->		'<' ident [ident ['=' string]]* '>' xmlNode '<' '/' ident '>' |
//				'<' ident [ident ['=' string]]* '>' text '<' '/' ident '>' |
//				'<' ident [ident ['=' string]]* '/>' |
//				'<' ident [ident ['=' string]]* '/>' |
//				{e}
//
// xmlNode ->	xml xmlNode |
//				{e}
//
// string ->	['"'|'''] ident* ['"'|''']
// text ->		[' '..'~' & not('<')]*
//
///////////////////////////////////////////////////////////////////////////////////

class XmlParser : public Tokeniser
{
public:
	XmlParser();
	virtual ~XmlParser();
	const XmlParser& operator=(const XmlParser&);
	XmlParser(const XmlParser&);

	// write resources to a file
	void Save(const std::string& fname);

	// parse an xmlString - set document root on success
	virtual void Parse(const std::string& xmlString);

	// load and parse an xml file - return error-code
	bool LoadAndParse(const std::string& fname);

	// access XML document
	XmlNode* GetDocumentRoot();

protected:
	// grammar
	XmlNode* Xml();
	void ParseEndTag(XmlNode* node);
	std::vector<XmlNode*> XmlNodeList();

private:
	// the resuling xml document
	XmlNode*	root;
};

