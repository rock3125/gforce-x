#pragma once

#include "system/signals/cancelObserver.h"

///////////////////////////////////////////////////////////////////////////////////

class Tokeniser : public CancelObserver
{
public:
	Tokeniser();
	virtual ~Tokeniser();
	const Tokeniser& operator=(const Tokeniser&);
	Tokeniser(const Tokeniser&);

	// setup tokeniser from file
	virtual void Load(const std::string& fname);

	virtual void Parse(const std::string& str);
	virtual void Cancel();

	// error management
	virtual bool HasErrors();
	virtual std::string GetError();

	enum Token
	{
		TEOF,
		TINT,
		TFLOAT,
		TBOOLEAN,
		TSTRING,
		TIDENT,
		TUNKNOWN,

		TCUSTOM
	};

	// token management for grammars
	void Add(const std::string& token, bool isCharacterBased);
	Token Get(const std::string& token);

	// setup basics
	void EnableInt(bool e=true);
	void EnableFloat(bool e=true);
	void EnableBoolean(bool e=true);
	void EnableDoubleQuoteString(bool e=true);
	void EnableSingleQuoteString(bool e=true);
	void EnableIdent(bool e=true);

	// for advanced index control
	int GetMarker();
	void SetMarker(int);

protected:

	struct TokenItem
	{
		int			id;
		std::string	str;
		bool		isCharacterBased;
	};

	std::vector<TokenItem>				tokens;
	stdext::hash_map<std::string,int>	tokenLookup;

protected:

	// parse a compulsary token
	bool GetCompulsaryToken(Token token);
	bool GetOptionalToken(Token token);
	bool GetCompulsaryTokenOption(Token t1,Token t2);
	bool GetCompulsaryToken(const std::string& token);
	bool GetOptionalToken(const std::string& token);
	bool GetCompulsaryTokenOption(const std::string& t1,const std::string& t2);
	float GetFloat();
	int GetInt();

	// can only unget last token
	void UngetToken();

	// for comments - scans to eol
	void ScanToEol();

	// get next token out of system
	Token GetNextToken();

	// force an error
	void SetError(const std::string& errStr);

	// can start an identifier
	bool CanStartIdentifier(char ch);
	bool IsValidForIdentifier(char ch);

protected:
	// temporary buffer used by parser
	char*				buffer;

	// sizeof parse buffer
	int					bufferSize;

	// for error checking/aborting
	bool				error;

	// for error checking/aborting
	std::string			errStr;

	// directory prefix after load
	std::string			filePrefix;

	// index into text right now
	int					index;

	// for undoing last character get
	int					prevIndex;

	// current token
	Token				token;

	// yy values
	std::string			yyString;
	float				yyFloat;
	int					yyInt;
	bool				yyBool;
	float				lastProgress;

	// settings for basic
	bool				enableInt;
	bool				enableFloat;
	bool				enableBoolean;
	bool				enableDQString;
	bool				enableSQString;
	bool				enableIdent;
};

