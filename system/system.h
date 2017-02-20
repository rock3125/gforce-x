#pragma once

/////////////////////////////////////////////////////////
// Sytem class for handling system wide enquiries and solutions
// keep this system clean from DirectX involvement

class System
{
public:

	enum Constants
	{
		MAX_STRING_SIZE	= 2048
	};

	enum ActionState
	{
		A_SELECT1		= 0x0001,
		A_SELECT2		= 0x0002,
		A_SELECT3		= 0x0004,
		A_MODIFIER1		= 0x0008,
		A_MODIFIER2		= 0x0010,
		A_MODIFIER3		= 0x0020,
		A_ABSMOUSE		= 0x0040,
		A_DELTAMOUSE	= 0x0080,
		A_WIREFRAME		= 0x0100,
		A_CULLCCW		= 0x0200
	};

	// complete system init
	static void Initialise();

	// fast access to identity matrix
	static const D3DXMATRIXA16* GetIdentityMatrix();
	// fast access to identity quaternion
	static const D3DXQUATERNION* GetIdentityQuaternion();

	// oid counter management for save and load
	static int GetOidCounter();
	static void SetOidCounter(int oidCounter);
	static int GetNextOid();

	static std::string EncodeBase64(const std::string& str);
	static std::string DecodeBase64(const std::string& str);

	// return true if a path starts with an absolute
	static bool HasAbolutePath(const std::string& fname);

	// fail the program completely
	static void Fail(bool cond,const std::string& expr,int line,const std::string& file);

	// integer to string
	static std::string Int2Str(long i);

	// string to integer
	static long Str2Int(const std::string& str);

	// convert string to a floating point number
	static double Str2Float(const std::string& str);

	// convert a floating point number to string
	static std::string Float2Str(double f);

	// converts a string to lower case
	static std::string ToLower(std::string lc);

	// string contains lower case version of second string
	static bool ContainsLwr(const std::string& str,const std::string& pattern);

	// get item out of string delimited by character seperator and with zero based index
	static std::string GetItem(const std::string& str,char seperator,int index);

	// get current data directory
	static std::string GetDataDirectory();

	// get application directory
	static std::string GetAppDirectory();

	// remove the data directory prefix from a filename if possible
	static std::string RemoveDataDirectory(const std::string& fname);

	// action system
	static void SetAction(ActionState a,bool b);
	static void SetAction(ActionState a,float x,float y);

	// get values out of the action system
	static bool GetBoolAction(ActionState a);
	static void GetVec2Action(ActionState a,float& x,float& y);

	// is an oid a valid oid?
	static bool ValidOid(int oid);

	// defn of an invalid oid
	static int GetInvalidOid();

	// random functions
	static float Random();
	static float Random(float min, float max);

	// offset vector by offset in direction angle
	static D3DXVECTOR3 OffsetVector(D3DXVECTOR3 vec, float angle, float offset);

	// conversion of degrees to radians
	static float deg2rad;
	static float rad2deg;
	static float pi;
	static const char* CR;

private:
	class Action
	{
	public:
		Action()
		{
			b = false;
			x = y = 0;
		};

		bool		b;
		float		x;
		float		y;
	};

	// root file directory of system
	static std::string dataDirectory;
	static std::string appDirectory;

	// state set of system
	static stdext::hash_map<ActionState, Action> actions;

	// static keep track of oids
	static int oidCounter;

	// identity matrix access
	static D3DXMATRIXA16 identityMatrix;

	// identity quaternion access
	static D3DXQUATERNION identityQuaternion;
};

