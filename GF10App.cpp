//////////////////////////////////////////////////////////////////////
/// GRAVITY FORCE X
///
/// (c)Copyright Peter de Vocht, 2006
///
/// I'm releasing this code under GPL v2.1 (see included file lgpl.txt)
///
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "GF10App.h"

#include "system/objectFactory.h"
#include "system/timer.h"

#include "system/model/camera.h"
#include "system/model/level.h"
#include "system/sound/soundSystem.h"
#include "system/fileLog.h"
#include "system/gamePad.h"
#include "system/keyboard.h"

#include "game/ship.h"

GF10App* GF10App::singleton = NULL;

////////////////////////////////////////////////////////////////////////

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	// init and get
	GF10App* rt = GF10App::Get();

	// initialise my system
	System::Initialise();

	// override default settings
	rt->LoadSettings();

	// setup
	rt->hInstance = hInstance;
	LoadString(hInstance, IDS_APP_TITLE, rt->szTitle, GF10App::MAX_LOADSTRING);
	LoadString(hInstance, IDC_GF10RUNTIME, rt->szWindowClass, GF10App::MAX_LOADSTRING);
	rt->MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!rt->InitInstance(hInstance, nCmdShow)) 
	{
		return FALSE;
	}

	// load accelerator
	HACCEL hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_GF10RUNTIME);

	// Main message loop:
	MSG msg;
	bool isActive = true;
	while (isActive)
	{
		// see if we have a message
		if (PeekMessage(&msg, NULL, 0, 0, 0))
		{
			// get it
			isActive = GetMessage(&msg, NULL, 0, 0) > 0;
			if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			// when app is null - the game is over and we quit
			if (!GF10App::IsNull())
			{
				// idle processing whenever we can
				GF10App::Get()->OnIdle();
			}
			else
			{
				break;
			}
		}
	}
	GF10App::Destroy();
	return (int)msg.wParam;
}

GF10App::GF10App()
	: runtime(NULL)
{
	// defaults
	Width(800);
	Height(600);

	hWnd = NULL;

	elapsedTime = 0;

	// 10 fps
	LOGIC_FRAME_RATE = 0.05;
	logicFrameTime = 0.0;
}

GF10App::~GF10App()
{
	safe_delete(runtime);

	SoundSystem::Destroy();
	GamePad::Destroy();
	Keyboard::Destroy();

	safe_delete(fileLog);
}

std::string GF10App::GetFilenameForLoad(const std::string& filter,const std::string& extn,const std::string& directory)
{
	return "";
}

std::string GF10App::GetFilenameForSave(const std::string& filter,const std::string& extn,const std::string& directory)
{
	return "";
}

bool GF10App::ColourPicker(const D3DXCOLOR& in,D3DXCOLOR& out)
{
	return false;
}

void GF10App::ShowErrorMessage(std::string msg,std::string title)
{
	MessageBox(hWnd, msg.c_str(), title.c_str(), MB_OK | MB_ICONERROR);
}

void GF10App::ForceProcessMessages()
{
}

bool GF10App::IsNull()
{
	return (singleton == NULL);
}

GF10App* GF10App::Get()
{
	if (singleton == NULL)
	{
		singleton = new GF10App();
	}
	return singleton;
}

void GF10App::Destroy()
{
	if (singleton != NULL)
	{
		delete singleton;
		singleton = NULL;
	}
}

ATOM GF10App::MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize			= sizeof(WNDCLASSEX); 
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)GF10App::WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_GF10RUNTIME);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= NULL;
	wcex.lpszMenuName	= (LPCTSTR)IDC_GF10RUNTIME;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_GF10RUNTIME);

	return RegisterClassEx(&wcex);
}

BOOL GF10App::InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	long windowType;
	if (Fullscreen())
	{
		windowType = WS_POPUP | WS_VISIBLE;
	}
	else
	{
		windowType = WS_POPUP | WS_VISIBLE | WS_BORDER | WS_CAPTION | WS_SYSMENU;
	}

	HWND hWnd = CreateWindow(szWindowClass, szTitle, windowType, 0, width, 0, height, NULL, NULL, hInstance, NULL);
	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

bool GF10App::Initialise(HWND hWnd)
{
	this->hWnd = hWnd;
	ShowWindow(hWnd, SW_SHOW);
	SetWindowPos(hWnd, NULL, 0, 0, width, height, SWP_NOZORDER);

	// init interface
	Interface::Get();

	fileLog = new FileLog();

	if (!Interface::Get()->InitialiseDevice(hWnd, width, height))
	{
		MessageBox(hWnd, "Fatal error\nYour display device does not support this application.", "Startup Error", MB_OK | MB_ICONERROR);
		return false;
	}

	// load app main font
	Interface::GetDevice()->GetSmallFont()->Load("objects\\neuropol24.fnt");
	Interface::GetDevice()->GetLargeFont()->Load("objects\\neuropol28.fnt");

	// initialise sound system
	SoundSystem::Get()->Initialise(hWnd);

	// initialise renderer and runtimes
	runtime = new GF10Runtime();
	currentRuntime = runtime;
	runtime->Initialise();

	// initialise object system
	ObjectFactory::Initialise();

	// initialise game pads
	GamePad::Get();
	Keyboard::Get();

	// load menus
	runtime->LoadMenus();

	// load sounds
	SoundSystem::Get()->LoadStoreSample(Runtime::FIRE_LASER,   "fire.wav");
	SoundSystem::Get()->LoadStoreSample(Runtime::FIRE_MISSILE, "missile.wav");
	SoundSystem::Get()->LoadStoreSample(Runtime::EXPLODE,      "explosion.wav");
	SoundSystem::Get()->LoadStoreSample(Runtime::LAND_SHIP,    "land.wav");
	SoundSystem::Get()->LoadStoreSample(Runtime::THRUST,       "engine.wav");
	SoundSystem::Get()->LoadStoreSample(Runtime::UNDERWATER,   "drop.wav");

	Log::GetLog() << "initialisation successful" << System::CR << System::CR;

	// start main soundtrack
	SoundSystem::Get()->ST_Play("music\\GravityForce.mp3");
	return true;
}

void GF10App::OnKeyDown(int kchar,int flags)
{
	bool shift=(flags & MK_SHIFT);
	bool ctrl=(flags & MK_CONTROL);

	Keyboard::Get()->KeyDown(kchar);
	Keyboard::Get()->SetCtrlDown(ctrl);
	Keyboard::Get()->SetShiftDown(shift);
}

void GF10App::OnKeyPress(int kchar,int flags)
{
	Keyboard::Get()->KeyPress(kchar);
}

void GF10App::OnKeyUp(int kchar,int flags)
{
	bool shift=(flags & MK_SHIFT);
	bool ctrl=(flags & MK_CONTROL);

	Keyboard::Get()->KeyUp(kchar);
	Keyboard::Get()->SetCtrlDown(ctrl);
	Keyboard::Get()->SetShiftDown(shift);
}

BOOL GF10App::OnIdle()
{
	if (Interface::Get()->IsDeviceOK())
	{
		// if the device is not ready then attempt to reset it
		if (Interface::Get()->IsDeviceReady())
		{
			double frameTime = Time::Get()->FrameTime();
			Time::Get()->Update();
			
			try
			{
				runtime->EventLogic(frameTime);
				runtime->Draw(frameTime);
			}
			catch (Exception* ex)
			{
				runtime->GP_SetErrorMessage(ex->Message());
			}

		}
		return TRUE;
	}
	else
	{
		Interface::Get()->ResetDevice();
		return FALSE;
	}
}

void GF10App::Quit()
{
	PostQuitMessage(0);
}

LRESULT CALLBACK GF10App::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) 
	{
	case WM_CREATE:
		{
			// init system at window creation time - setup D3D
			if (!GF10App::Get()->Initialise(hWnd))
			{
				// exit now!
				PostQuitMessage(-1);
			}
			break;
		}
	case WM_COMMAND:
		{
			// process command messages
			int wmId    = LOWORD(wParam); 
			int wmEvent = HIWORD(wParam); 
			switch (wmId)
			{
			case IDM_EXIT:
				{
					DestroyWindow(hWnd);
					break;
				}
			default:
				{
					return DefWindowProc(hWnd, message, wParam, lParam);
				}
			}
			break;
		}
	case WM_CHAR:
		{
			if (!GF10App::IsNull())
			{
				GF10App::Get()->OnKeyPress(wParam,lParam);
			}
			break;
		}
	case WM_KEYDOWN:
		{
			// pass keys down to game system
			if (!GF10App::IsNull())
			{
				GF10App::Get()->OnKeyDown(wParam,lParam);
			}
			break;
		}
	case WM_KEYUP:
		{
			// pass keys down to game system
			if (!GF10App::IsNull())
			{
				GF10App::Get()->OnKeyUp(wParam,lParam);
			}
			break;
		}
	case WM_DESTROY:
		{
			// the way we quit the system
			if (!GF10App::IsNull())
			{
				GF10App::Get()->Quit();
			}
			break;
		}
	default:
		{
			// pass it back to win32
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	// we've dealt with it
	return 0;
}

