#pragma once

/////////////////////////////////////////////////////////

class CPU
{
public:
	// return true if the CPU supports this application
	static bool CPUSupported();

private:

	enum
	{
		_CPU_FEATURE_MMX = 0x0001,
		_CPU_FEATURE_SSE = 0x0002,
		_CPU_FEATURE_SSE2 = 0x0004,
		_CPU_FEATURE_3DNOW = 0x0008,

		// These are the bit flags that get set on calling cpuid 
		// with register eax set to 1 
		_MMX_FEATURE_BIT = 0x00800000,
		_SSE_FEATURE_BIT = 0x02000000,
		_SSE2_FEATURE_BIT = 0x04000000,

		// This bit is set when cpuid is called with 
		// register set to 80000001h (only applicable to AMD) 
		_3DNOW_FEATURE_BIT = 0x80000000,
		_MAX_VNAME_LEN = 13,
		_MAX_MNAME_LEN = 30
	};

	typedef struct _processor_info 
	{ 
		char v_name[_MAX_VNAME_LEN]; // vendor name 
		char model_name[_MAX_MNAME_LEN]; // name of model e.g. Intel Pentium-Pro 
		int family; // family of the processor e.g. 6 = Pentium-Pro architecture 
		int model; // model of processor e.g. 1 = Pentium-Pro for family = 6 
		int stepping; // processor revision number 
		int feature; // processor feature (same as return value from _cpuid) 
		int os_support; // does OS Support the feature? 
		int checks; // mask of checked bits in feature and os_support fields 
	} _p_info; 

private:
	static int IsCPUID();
	static bool _os_support(int feature);
	static void map_mname(int family, int model, const char *v_name, char *m_name);
	static int _cpuid (_p_info *pinfo);

};
