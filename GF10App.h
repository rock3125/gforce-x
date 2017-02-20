//////////////////////////////////////////////////////////////////////
/// GRAVITY FORCE X
///
/// (c)Copyright Peter de Vocht, 2006
///
/// I'm releasing this code under GPL v2.1 (see included file lgpl.txt)
///
//////////////////////////////////////////////////////////////////////

#pragma once

#include "resource.h"
#include "runtime/GF10Runtime.h"
#include "system/BaseApp.h"

class FileLog;
class MenuManager;

////////////////////////////////////////////////////////////////////////////

class GF10App : public BaseApp
{
	GF10App(const GF10App&);
	const GF10App& operator=(const GF10App&);
	GF10App();
	~GF10App();
public:

	// access singleton
	static GF10App* Get();

	// used to check for program end condition
	static bool IsNull();

	// destoy the instance when its all over
	static void Destroy();

	// called by system when window has been created
	bool Initialise(HWND hWnd);

	// show windows based error message
	void ShowErrorMessage(std::string msg,std::string title);

	// handle keyboard messages
	void OnKeyDown(int kchar, int flags);
	void OnKeyUp(int kchar, int flags);
	void OnKeyPress(int kchar, int flags);

	// exit program
	void Quit();

protected:
	std::string GetFilenameForLoad(const std::string& filter, const std::string& extn, const std::string& directory);
	std::string GetFilenameForSave(const std::string& filter, const std::string& extn, const std::string& directory);
	bool ColourPicker(const D3DXCOLOR& in, D3DXCOLOR& out);

	// pump message queue
	void ForceProcessMessages();

public:
	enum
	{
		MAX_LOADSTRING = 100
	};

	// Forward declarations of functions included in this code module:
	ATOM MyRegisterClass(HINSTANCE hInstance);
	BOOL InitInstance(HINSTANCE, int);
	static LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);

	// idle processing
	BOOL OnIdle();

private:
	friend int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, 
								  LPTSTR lpCmdLine, int nCmdShow);

	// Global Variables:
	HINSTANCE	hInstance;
	TCHAR		szTitle[MAX_LOADSTRING];
	TCHAR		szWindowClass[MAX_LOADSTRING];

	// runtime of system
	GF10Runtime* runtime;

	// file log device
	FileLog*	fileLog;

	// logic speed
	double logicFrameTime;
	double LOGIC_FRAME_RATE;

	static GF10App* singleton;
};

