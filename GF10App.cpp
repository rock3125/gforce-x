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

#include "system/xml/XmlParser.h"
#include "system/model/camera.h"
#include "system/model/level.h"
#include "system/fileLog.h"
#include "system/gamePad.h"
#include "system/keyboard.h"
#include "system/cpuid.h"

#include "menu/menuManager.h"

GF10App* GF10App::singleton = NULL;

////////////////////////////////////////////////////////////////////////

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	// init and get
	GF10App* rt = GF10App::Get();

	// load app settings
	XmlParser* parser = new XmlParser();
	if (!parser->LoadAndParse(System::GetAppDirectory() + "settings.xml"))
	{
		rt->ShowErrorMessage("could not load \"settings.xml\"", "init error");
		return 0;
	}

	// get screen settings node
	XmlNode* node = parser->GetDocumentRoot()->GetChildNode("screen");
	if (node == NULL)
	{
		rt->ShowErrorMessage("could not find \"screen\" settings in \"settings.xml\"", "init error");
		return 0;
	}
	// check node has right attributes
	if (!node->HasName("width") || !node->HasName("height") || !node->HasName("fullscreen"))
	{
		rt->ShowErrorMessage("\"screen\" settings in \"settings.xml\" missing \"width\", \"fullscreen\" or \"height\"", "init error");
		return 0;
	}

	// set runtime parameters
	rt->Width(System::Str2Int(node->GetValue("width")));
	rt->Height(System::Str2Int(node->GetValue("height")));
	rt->Fullscreen(System::ToLower(node->GetValue("fullscreen")) == "true");

	// get keyboard settings node
	XmlNode* keysNode = parser->GetDocumentRoot()->GetChildNode("keys");
	if (keysNode == NULL)
	{
		rt->ShowErrorMessage("could not find \"keys\" settings in \"settings.xml\"", "init error");
		return 0;
	}
	// check node has right attributes
	if (!keysNode->HasName("left") || !keysNode->HasName("right") || !keysNode->HasName("up") ||
		!keysNode->HasName("down") || !keysNode->HasName("fire"))
	{
		rt->ShowErrorMessage("\"keys\" settings in \"settings.xml\" missing \"left\", \"right\", \"up\", \"down\", or \"fire\"", "init error");
		return 0;
	}
	// setup keyboard mappings for game-play
	rt->Up(Input::StringToKey(keysNode->GetValue("up")));
	rt->Down(Input::StringToKey(keysNode->GetValue("down")));
	rt->Left(Input::StringToKey(keysNode->GetValue("left")));
	rt->Right(Input::StringToKey(keysNode->GetValue("right")));
	rt->Fire1(Input::StringToKey(keysNode->GetValue("fire")));

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
	while (true) 
	{
		// see if we have a message
		if (PeekMessage(&msg, NULL, 0, 0, 0))
		{
			// get it
			GetMessage(&msg, NULL, 0, 0);
			if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			// when app is null - the game is over and we quit
			if (GF10App::IsNull()) break;
			// idle processing whenever we can
			GF10App::Get()->OnIdle();
		}
	}
	GF10App::Destroy();
	return (int)msg.wParam;
}

GF10App::GF10App()
{
	// defaults
	Width(800);
	Height(600);

	hWnd = NULL;

	menuManager = new MenuManager();
}

GF10App::~GF10App()
{
	safe_delete(menuManager);
	GamePad::Destroy();
	Keyboard::Destroy();
	safe_delete(fileLog);
}

void GF10App::SetActivePlayer(int shipId)
{
	runtime.SetActivePlayer(shipId);
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

Level* GF10App::Load(const std::string& fname)
{
	XmlParser p;
	std::string strippedFname = System::RemoveDataDirectory(fname);
	bool result = p.LoadAndParse(fname);
	if (result)
	{
		try
		{
			XmlNode* root = p.GetDocumentRoot();

			Level* level = new Level();
			level->Read(*root);

			// set level
			SetCurrentLevelPtr(level);

			// and the current camera
			Camera camera;
			camera.Read(*root);
			runtime.SetCamera(camera);

			return level;
		}
		catch (Exception* ex)
		{
			ShowErrorMessage(ex->Message(), "error loading level");
		}
	}
	return NULL;
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

	SetCurrentRuntime(&runtime);

	return TRUE;
}

bool GF10App::Initialise(HWND hWnd)
{
	this->hWnd = hWnd;
	ShowWindow(hWnd, SW_SHOW);
	SetWindowPos(hWnd, NULL, 0, 0, width, height, SWP_NOZORDER);

	// check cpu
	/*
	if (!CPU::CPUSupported())
	{
		MessageBox(hWnd,"this CPU is not supported", "Error", MB_OK | MB_ICONERROR);
		return false;
	}
	*/

	// initialise my system
	System::Initialise();
	Interface::Create();

	fileLog = new FileLog();

	if (!Interface::GetI()->InitialiseDevice(hWnd, width, height))
	{
		MessageBox(hWnd, "Fatal error\nYour display device does not support this application.", "Startup Error", MB_OK | MB_ICONERROR);
		return false;
	}

	// load app main font
	Interface::GetDevice()->GetFont()->Load("objects\\neuropol10.fnt");

	// initialise renderer and runtimes
	runtime.Initialise();

	// notify scene grap and runtime of the new (initial) level
	runtime.NewLevel(NULL);

	// initialise object system
	ObjectFactory::Initialise();

	// initialise game pads
	GamePad::Get();
	Keyboard::Get();

	// load menus
	menuManager->Load(System::GetDataDirectory() + "menu\\menus.xml");

	Log::GetLog() << "initialisation successful" << System::CR << System::CR;

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
	Interface* intf = Interface::GetI();

	if (intf->IsDeviceOK())
	{
		// if the device is not ready then attempt to reset it
		if (intf->IsDeviceReady())
		{
			Time& t = Time::GetTimer();
			double frameTime = t.FrameTime();
			t.Update();

			// poll gamepad
			GamePad::Get()->EventLogic(frameTime);

			if (menuManager->GetActive())
			{
				// careful - order is vital
				menuManager->Draw(frameTime);
				menuManager->EventLogic(frameTime);
			}
			else
			{
				// careful - order is vital
				runtime.Draw(frameTime);
				runtime.EventLogic(frameTime);
			}
		}
		return TRUE;
	}
	else
	{
		intf->ResetDevice();
		return FALSE;
	}
}

void GF10App::QuitGame()
{
	// remove level from the app
	SetCurrentLevelPtr(NULL);
	// notify all listeners that the level is gone
	LevelObserver::NotifyObservers((Level*)NULL);
	// back to the menu
	menuManager->SetActive(true);
	// first menu, first item
	menuManager->SetActiveMenu(0,0);
}

void GF10App::Quit()
{
	GF10App::Destroy();
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

