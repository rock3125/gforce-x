#include "standardFirst.h"

#include "system/system.h"
#include "system/log.h"

#include <atlenc.h>

// static data

std::string System::dataDirectory;
std::string System::appDirectory;
std::string System::soundsDirectory;
stdext::hash_map<System::ActionState, System::Action> System::actions;

int System::oidCounter=0;

float System::deg2rad=0.0174533f;
float System::rad2deg=57.2957795f;
float System::pi=3.14159265f;

const char* System::CR = "\n";

D3DXMATRIXA16 System::identityMatrix;
D3DXQUATERNION System::identityQuaternion;
D3DXVECTOR3 System::zeroVector;

/////////////////////////////////////////////////////////

void System::Initialise()
{
	// initialise timer
	Time::Get()->Initialise();

	// get path
	char buf[MAX_PATH];
	char tempBuf[MAX_PATH];
	_getcwd(buf,MAX_PATH);
	_strlwr_s(buf,MAX_PATH);
	int len = strlen(buf);

	// randomise
	srand(GetTickCount());

	// append \ at end if not yet present
	if (buf[len-1]!='\\')
	{
		strcat_s(buf,MAX_PATH,"\\");
	}
	appDirectory=buf;
	strcat_s(buf, MAX_PATH, "data\\");
	dataDirectory = buf;

	strcpy_s(tempBuf, MAX_PATH, buf);
	strcat_s(tempBuf, MAX_PATH, "sounds\\");
	soundsDirectory = tempBuf;

	std::string f;
	std::transform(dataDirectory.begin(),dataDirectory.end(),std::back_inserter(f),tolower);
	dataDirectory=f;

	actions[A_SELECT1]=Action();
	actions[A_SELECT2]=Action();
	actions[A_SELECT3]=Action();
	actions[A_MODIFIER1]=Action();
	actions[A_MODIFIER2]=Action();
	actions[A_MODIFIER3]=Action();
	actions[A_ABSMOUSE]=Action();
	actions[A_DELTAMOUSE]=Action();

	D3DXMatrixIdentity(&identityMatrix);
	D3DXQuaternionIdentity(&identityQuaternion);
	zeroVector = D3DXVECTOR3(0,0,0);
}

const D3DXMATRIXA16* System::GetIdentityMatrix()
{
	return &identityMatrix;
}

D3DXVECTOR3* System::GetZeroVector()
{
	return &zeroVector;
}

const D3DXQUATERNION* System::GetIdentityQuaternion()
{
	return &identityQuaternion;
}

std::string System::GetDataDirectory()
{
	return dataDirectory;
}

std::string System::GetSoundsDirectory()
{
	return soundsDirectory;
}

std::string System::GetAppDirectory()
{
	return appDirectory;
}

std::string System::RemoveDataDirectory(const std::string& _fname)
{
	if (_fname.size()>dataDirectory.size())
	{
		std::string fname;
		std::transform(_fname.begin(),_fname.end(),std::back_inserter(fname),tolower);
		if (fname.substr(0,dataDirectory.size())==dataDirectory)
		{
			return fname.substr(dataDirectory.size());
		}
	}
	return _fname;
}

bool System::ValidOid(int oid)
{
	return (oid>0);
}

void System::Fail(bool cond,const std::string& expr,int line,const std::string& file)
{
	if (!cond)
	{
		Log::GetLog() << "precond failed: " << expr << System::CR;
		Log::GetLog() << "line: " << line << ", file: " << file << System::CR;
	}
}

long System::Str2Int(const std::string& str)
{
	return atoi(str.c_str());
}

double System::Str2Float(const std::string& str)
{
	return atof(str.c_str());
}

std::string System::Int2Str(long i)
{
	static char buf[256];
	sprintf_s(buf,256,"%d",i);
	return buf;
}

std::string System::Float2Str(double f)
{
	static char buf[256];
	sprintf_s(buf,256,"%1.6f",f);
	return buf;
}

std::string System::ToLower(std::string lc)
{
	for (int i=0; i<lc.size(); i++)
	{
		char c = lc[i];
		if (c>='A' && c<='Z')
		{
			c = (c-'A') + 'a';
			lc[i] = c;
		}
	}
	return lc;
}

bool System::ContainsLwr(const std::string& _str,const std::string& pattern)
{
	std::string lc = ToLower(pattern);
	std::string str = ToLower(_str);

	// find it
	int s = (str.size() - lc.size()) + 1;
	for (int i=0; i<s; i++)
	{
		if (str[i]==lc[0])
		{
			bool found = true;
			for (int j=1; j<lc.size() && found; j++)
			{
				found = lc[j] == str[i+j];
			}
			if (found)
				return true;
		}
	}
	return false;
}

int System::GetOidCounter()
{
	return oidCounter;
}

int System::GetNextOid()
{
	oidCounter++;
	return oidCounter;
}

void System::SetOidCounter(int _oidCounter)
{
	oidCounter=_oidCounter;
}

int System::GetInvalidOid()
{
	return 0;
}

std::string System::GetItem(const std::string& str,char seperator,int index)
{
	if ( index==0 )
	{
		int i = 0;
		while ( str[i]!=seperator && str[i]!=0 ) i++;
		std::string temp;
		if ( str[i]==seperator )
		{
			temp=str.substr(0,i);
			return temp;
		}
		else
			return str;
	}
	else
	{
		int prevChar = 0;
		int cntr = 0;
		int len = str.length();
		for (int i=0; i<len; i++)
		{
			if ( str[i]==seperator )
			{
				cntr++;
			}

			if ( (cntr-1)==index )
			{
				std::string temp;
				temp = str.substr(prevChar,i-prevChar);
				return temp;
			}
			else if ( cntr>index )
			{
				return "";
			}

			if ( str[i]==seperator )
			{
				prevChar = i+1;
			}
		}
		if ( cntr==index )
		{
			std::string temp;
			temp = str.substr(prevChar);
			return temp;
		}
	}
	return "";
}

void System::SetAction(ActionState a,bool b)
{
	actions[a].b=b;
}

void System::SetAction(ActionState a,float x,float y)
{
	PreCond(actions.find(a)!=actions.end());
	actions[a].x=x;
	actions[a].y=y;
}

bool System::GetBoolAction(ActionState a)
{
	if (actions.find(a)==actions.end())
		return false;
	return actions[a].b;
}

void System::GetVec2Action(ActionState a,float& x,float& y)
{
	PreCond(actions.find(a)!=actions.end());
	x=actions[a].x;
	y=actions[a].y;
}

std::string System::EncodeBase64(const std::string& str)
{
	int destLen = str.size()*2;
	char* dest = new char[destLen];
	::Base64Encode((const byte*)str.c_str(),str.size(),dest,&destLen);
	dest[destLen] = 0;
	std::string ret = dest;
	safe_delete_array(dest);
	return ret;
}

std::string System::DecodeBase64(const std::string& str)
{
	int destLen = str.size();
	char* dest = new char[destLen];
	::Base64Decode(str.c_str(),str.size(),(byte*)dest,&destLen);
	dest[destLen] = 0;
	std::string ret = dest;
	safe_delete_array(dest);
	return ret;
}

bool System::HasAbolutePath(const std::string& fname)
{
	if (fname.length() > 1)
	{
		if (fname[1] == ':' || (fname[0] == '\\' && fname[1] == '\\'))
			return true;
	}
	return false;
}

float System::Random()
{
	int r = rand();
	return (float)(double(r) / double(RAND_MAX));
}

float System::Random(float min, float max)
{
	return Random()*(max-min) + min;
}

D3DXVECTOR3 System::OffsetVector(D3DXVECTOR3 vec, float angle, float offset)
{
	vec.x += cosf((angle+90) * System::deg2rad) * offset;
	vec.y += sinf((angle+90) * System::deg2rad) * offset;
	return vec;
}
