#include "standardFirst.h"

#include "system/tokeniser.h"

#include "system/signals/progressObserver.h"

///////////////////////////////////////////////////////////////////////////////////

Tokeniser::Tokeniser()
	: buffer(NULL)
{
	enableInt = false;
	enableFloat = false;
	enableBoolean = false;
	enableDQString = false;
	enableSQString = false;
	enableIdent = false;

	error=false;
	index=0;
	prevIndex=0;
	yyInt = 0;
	yyFloat = 0;
	lastProgress = 0;
	yyBool = false;
}

Tokeniser::~Tokeniser()
{
	safe_delete(buffer);
}

const Tokeniser& Tokeniser::operator=(const Tokeniser& tp)
{
	return *this;
}

Tokeniser::Tokeniser(const Tokeniser& tp)
{
	operator=(tp);
}

void Tokeniser::Cancel()
{
	SetError("user cancelled");
}

bool Tokeniser::CanStartIdentifier(char ch)
{
	return ((ch>='a' && ch<='z') ||
			(ch>='A' && ch<='Z') ||
			ch=='_');
}

void Tokeniser::EnableInt(bool e)
{
	enableInt = e;
}

void Tokeniser::EnableFloat(bool e)
{
	enableFloat = e;
}

void Tokeniser::EnableBoolean(bool e)
{
	enableBoolean = e;
}

void Tokeniser::EnableDoubleQuoteString(bool e)
{
	enableDQString = e;
}

void Tokeniser::EnableSingleQuoteString(bool e)
{
	enableSQString = e;
}

void Tokeniser::EnableIdent(bool e)
{
	enableIdent = e;
}

bool Tokeniser::HasErrors()
{
	return error;
}

std::string Tokeniser::GetError()
{
	return errStr;
}

void Tokeniser::Load(const std::string& fname)
{
	std::string prefix = System::RemoveDataDirectory(fname);
	int off = prefix.find_last_of("\\");
	if (off >= 0)
	{
		filePrefix = prefix.substr(0, off + 1);
	}
	else
	{
		filePrefix.clear();
	}

	FileStreamer file(BaseStreamer::STREAM_READ);
	if (file.Open(fname))
	{
		safe_delete(buffer);

		bufferSize=file.Size();
		if (bufferSize==0)
		{
			SetError("file is empty");
			file.Close();
		}
		else
		{
			buffer=new char[bufferSize+1];
			file.ReadData("",buffer,bufferSize);
			buffer[bufferSize]=0;
			file.Close();

			error=false;
			index=0;
			prevIndex=0;
			yyString.clear();
			yyInt = 0;
			yyFloat = 0;
			lastProgress = 0;
			yyBool = false;
		}
	}
	else
	{
		SetError(file.Error());
	}
}

bool Tokeniser::IsValidForIdentifier(char ch)
{
	// valid VRML ident names
	return (CanStartIdentifier(ch) || (ch>='0' && ch<='9') || ch=='-');
}

void Tokeniser::ScanToEol()
{
	if (index>=bufferSize)
		return;
	bool cr;
	do
	{
		cr=false;
		switch (buffer[index])
		{
			case 10:
			case 13:
			{
				cr=true;
				break;
			}
		}
		index++;
	}
	while (!cr && index<bufferSize);
}

Tokeniser::Token Tokeniser::GetNextToken()
{
	static char buf[4096];

	// passed end?
	if (index>=bufferSize)
		return TEOF;

	// progress updates if applicable
	float progress = (float)(int)(((float)index / (float)bufferSize) * 100);
	if (progress!=lastProgress)
	{
		lastProgress = progress;
		ProgressObserver::NotifyObservers(progress*0.01f);
	}

	// ready to rewind
	prevIndex=index;

	bool whiteSpace=false;
	do
	{
		whiteSpace=false;

		switch (buffer[index])
		{
			case ' ':
			case 10:
			case 13:
			case '\t':
			{
				whiteSpace=true;
				index++;
				break;
			}
		}
	}
	while (whiteSpace && index<bufferSize);

	if (index>=bufferSize)
		return TEOF;

	// do tokens
	for (int i=0; i<tokens.size(); i++)
	{
		int s = tokens[i].str.size();
		if ((index+s)<bufferSize)
		{
			if (strncmp(&buffer[index],tokens[i].str.c_str(),s)==0)
			{
				// character based tokens are not delimited by other characters
				if (tokens[i].isCharacterBased)
				{
					if ((index+s) < bufferSize && !IsValidForIdentifier(buffer[index+s]))
					{
						index += s;
						return (Token)tokens[i].id;
					}
				}
				else
				{
					index += s;
					return (Token)tokens[i].id;
				}
			}
		}
	}

	// number?
	if (enableInt || enableFloat)
	{
		if ((buffer[index]=='-' && buffer[index+1]>='0' && buffer[index+1]<='9') || (buffer[index]>='0' && buffer[index]<='9'))
		{
			bool bMinus=false;
			if (buffer[index]=='-')
			{
				index++;
				bMinus=true;
			}

			int num=0;
			do
			{
				num=num*10 + int(buffer[index++]-'0');
			}
			while (buffer[index]>='0' && buffer[index]<='9');
			yyInt=num;

			// #QO
			if (buffer[index]=='.' && buffer[index+1]=='#' && buffer[index+2]=='Q')
			{
				yyFloat=float(yyInt);
				index += 4;
				return TFLOAT;
			}

			if (buffer[index]=='.' && buffer[index+1]>='0' && buffer[index+1]<='9')
			{
				index++;
				float mantissa=0.0f;
				float dividor=0.1f;
				do
				{
					mantissa=mantissa + float(buffer[index++]-'0') * dividor;
					dividor=dividor*0.1f;
				}
				while (buffer[index]>='0' && buffer[index]<='9');

				// e+012
				if (buffer[index]=='e' && (buffer[index+1]=='+' || buffer[index+1]=='-'))
				{
					int num = yyInt;
					index++;
					if (buffer[index]=='+') index++;
					Token t = GetNextToken();
					if (t != TINT)
					{
						SetError("incorrect float e-extension");
						return TUNKNOWN;
					}
					yyFloat = powf(float(num) + mantissa, float(yyInt));
					return TFLOAT;
				}

				yyFloat=float(yyInt)+mantissa;

				if (bMinus)
					yyFloat=-yyFloat;

				if (!enableFloat)
				{
					yyInt = (int)yyFloat;
					return TINT;
				}
				else
				{
					yyInt=(int)yyFloat;
					return TFLOAT;
				}
			}

			if (bMinus)
				yyInt=-num;
			else
				yyInt=num;

			if (!enableInt)
			{
				yyFloat = (float)yyInt;
				return TFLOAT;
			}
			else
			{
				yyFloat = (float)yyInt;
				return TINT;
			}
		}
	}

	// identifier ( [a..z|A..z|_]
	if (enableIdent)
	{
		if (IsValidForIdentifier(buffer[index]))
		{
			buf[0]=buffer[index++];
			buf[1]=0;
			int bufferIndex=1;
			do
			{
				if (IsValidForIdentifier(buffer[index]))
				{
					buf[bufferIndex++]=buffer[index++];
					buf[bufferIndex]=0;
				}
			}
			while (IsValidForIdentifier(buffer[index]) && bufferIndex<255 && index<bufferSize);

			if (bufferIndex>255)
			{
				SetError("identifier too long");
				return TUNKNOWN;
			}
			yyString=buf;
			return TIDENT;
		}
	}

	// check string
	if ((enableDQString && buffer[index] == '"') ||
		(enableSQString && buffer[index] == '\''))
	{
		int i = 0;
		char term = buffer[index++];
		buf[0] = 0;
		while (buffer[index] != term && i < 255 && index < bufferSize)
		{
			buf[i++] = buffer[index++];
		}
		if (index>=bufferSize || i>=255)
		{
			SetError("unterminated string or string too long");
			return TEOF;
		}
		buf[i]=0;
		if (buffer[index] == term) index++;
		yyString = buf;
		return TSTRING;
	}

	index++;
	return TUNKNOWN;
}

int Tokeniser::GetMarker()
{
	return index;
}

void Tokeniser::SetMarker(int _index)
{
	index = _index;
	prevIndex = _index;
}

void Tokeniser::UngetToken()
{
	index=prevIndex;
}

void Tokeniser::SetError(const std::string& _errStr)
{
	errStr=_errStr;

	// count the number of lines up to now
	int line=1;
	int charCntr=1;
	for (int i=0; i<index; i++)
	{
		if (buffer[i]=='\n')
		{
			charCntr=1;
			line++;
		}
		else
		{
			charCntr++;
		}
	}

	errStr=errStr+" (line "+System::Int2Str(line)+", character "+System::Int2Str(charCntr)+")";
	error=true;
}

bool Tokeniser::GetCompulsaryToken(Token _token)
{
	token = GetNextToken();
	if (token!=_token)
	{
		SetError("expected token");
		return false;
	}
	return true;
}

bool Tokeniser::GetOptionalToken(Token _token)
{
	token = GetNextToken();
	if (token!=_token)
	{
		UngetToken();
		return false;
	}
	return true;
}

bool Tokeniser::GetCompulsaryToken(const std::string& token)
{
	Token t = Get(token);
	if (t==TUNKNOWN)
	{
		SetError("unknown token: " + token);
		return false;
	}
	if (!GetCompulsaryToken(t))
	{
		SetError("expected token: " + token);
		return false;
	}
	return true;
}

bool Tokeniser::GetOptionalToken(const std::string& token)
{
	Token t = Get(token);
	if (t==TUNKNOWN)
	{
		SetError("unknown token: " + token);
		return false;
	}
	return GetOptionalToken(t);
}

bool Tokeniser::GetCompulsaryTokenOption(const std::string& t1,const std::string& t2)
{
	Token token1 = Get(t1);
	if (token1 == TUNKNOWN)
	{
		SetError("unknown token: " + t1);
		return false;
	}

	Token token2 = Get(t2);
	if (token2 == TUNKNOWN)
	{
		SetError("unknown token: " + t2);
		return false;
	}
	return GetCompulsaryTokenOption(token1, token2);
}

bool Tokeniser::GetCompulsaryTokenOption(Token t1,Token t2)
{
	token = GetNextToken();
	if (token==t1 || token==t2)
	{
		return true;
	}
	SetError("expected token");
	return false;
}

void Tokeniser::Add(const std::string& token, bool isCharacterBased)
{
	TokenItem t;
	t.str = token;
	t.id = TCUSTOM + tokens.size();
	t.isCharacterBased = isCharacterBased;

	tokenLookup[t.str] = t.id;

	// make sure its added sorted by biggest first (size wise)
	int s = token.size();
	if (tokens.size()>0)
	{
		std::vector<TokenItem> newList;
		if (s>tokens[0].str.size())
		{
			// first in list
			newList.push_back(t);
			for (int i=0; i<tokens.size(); i++)
			{
				newList.push_back(tokens[i]);
			}
		}
		else
		{
			// some where else
			int count = tokens.size();
			int index = 0;
			while (index<count && tokens[index].str.size() >= s)
			{
				newList.push_back(tokens[index]);
				index++;
			}
			// right place
			newList.push_back(t);

			// do rest
			while (index<count)
			{
				newList.push_back(tokens[index]);
				index++;
			}
		}
		tokens = newList;
	}
	else
	{
		tokens.push_back(t);
	}
}

Tokeniser::Token Tokeniser::Get(const std::string& token)
{
	stdext::hash_map<std::string,int>::iterator p = tokenLookup.find(token);
	if (p!=tokenLookup.end())
	{
		return (Tokeniser::Token)(*p).second;
	}
	throw new Exception("unknown token");
	return TUNKNOWN;
}

void Tokeniser::Parse(const std::string& str)
{
	error=false;
	index=0;
	prevIndex=0;
	yyString.clear();
	yyInt = 0;
	yyFloat = 0;
	lastProgress = 0;
	yyBool = false;

	safe_delete(buffer);

	bufferSize = str.size();
	if (bufferSize == 0)
	{
		errStr = "parse string is empty";
		error = true;
	}
	else
	{
		buffer = new char[bufferSize+1];
		strcpy_s(buffer, bufferSize+1, str.c_str());
	}
}

float Tokeniser::GetFloat()
{
	if (GetCompulsaryTokenOption(TFLOAT,TINT))
	{
		if (token==TFLOAT)
		{
			return yyFloat;
		}
		else
		{
			return (float)yyInt;
		}
	}
	return 0;
}

int Tokeniser::GetInt()
{
	if (GetCompulsaryToken(TINT))
	{
		return yyInt;
	}
	return 0;
}

