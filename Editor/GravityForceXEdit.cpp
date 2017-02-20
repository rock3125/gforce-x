#include "stdafx.h"

#include "GravityForceXEdit.h"
#include "MainFrm.h"

#include "interface/propertiesDlg.h"
#include "interface/progressDlg.h"
#include "interface/componentLibraryDlg.h"

#include "system/objectFactory.h"
#include "system/timer.h"

#include "system/xml/XmlParser.h"
#include "system/model/camera.h"
#include "system/model/level.h"
#include "system/fileLog.h"
#include "system/gamePad.h"
#include "system/keyboard.h"

#include "runtime/editorRuntime.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////
// GravityForceXEdit

GravityForceXEdit theApp;


BEGIN_MESSAGE_MAP(GravityForceXEdit, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)

	ON_COMMAND(ID_FILE_OPEN,OnFileOpen)
	ON_COMMAND(ID_FILE_SAVE,OnFileSave)
	ON_COMMAND(ID_FILE_SAVEAS,OnFileSaveAs)
	ON_COMMAND(ID_VIEW_WIREFRAME,OnWireframe)
	ON_COMMAND(ID_VIEW_CULLCCW,OnCullCCW)

END_MESSAGE_MAP()


GravityForceXEdit::GravityForceXEdit()
	: editorRuntime(NULL)
	, propertiesDlg(NULL)
	, componentLibraryDlg(NULL)
	, fileLog(NULL)
{
	totalTime = 0.0;
}

GravityForceXEdit::~GravityForceXEdit()
{
	GamePad::Destroy();
	Keyboard::Destroy();

	safe_delete(editorRuntime);
	safe_delete(propertiesDlg);
	safe_delete(componentLibraryDlg);
	safe_delete(fileLog);
}

GravityForceXEdit* GravityForceXEdit::Get()
{
	return &theApp;
}

void GravityForceXEdit::UpdateScore(void*)
{
}

std::string GravityForceXEdit::WindowSetupToString(const std::string& name,CWnd* w)
{
	std::string str;

	CRect r;
	w->GetWindowRect(&r);
	bool minimised=(w->IsIconic()==TRUE);
	bool visible=(w->IsWindowVisible()==TRUE);
	str="  <"+name;
	str=str+" left='"+System::Int2Str(r.left)+"'";
	str=str+" top='"+System::Int2Str(r.top)+"'";
	str=str+" right='"+System::Int2Str(r.right)+"'";
	str=str+" bottom='"+System::Int2Str(r.bottom)+"'";
	if (minimised)
		str=str+" minimized='true'";
	else
		str=str+" minimized='false'";

	if (visible)
		str=str+" visible='true'";
	else
		str=str+" visible='false'";

	str=str+"></"+name+">\n";
	return str;
}

void GravityForceXEdit::SetWindowPositionFromXML(XmlNode* n,CWnd* w)
{
	CRect r;
	r.left = atoi(n->GetValue("left").c_str());
	r.right = atoi(n->GetValue("right").c_str());
	r.top = atoi(n->GetValue("top").c_str());
	r.bottom = atoi(n->GetValue("bottom").c_str());
	bool minimised = n->GetValue("minimized")=="true";
	bool visible = true; // n->GetValue("visible")=="true";

	w->SetWindowPos(NULL,r.left,r.top,r.Width(),r.Height(),SWP_NOZORDER);
	w->ShowWindow(((!visible)?SW_HIDE:0)|(minimised?SW_SHOWMINIMIZED:(visible?SW_RESTORE:0)));
}

void GravityForceXEdit::SaveWindowPos()
{
	FileStreamer file(BaseStreamer::STREAM_WRITE);
	std::string fname=System::GetAppDirectory()+"editorsettings.xml";
	if (file.Open(fname))
	{
		file.WriteData("<settings>\n");

		file.WriteData(WindowSetupToString("properties",propertiesDlg));
		file.WriteData(WindowSetupToString("components",componentLibraryDlg));

		if (Get()->GetCurrentRuntime()!=NULL)
		{
			XmlNode* xml = NULL;

			Camera* cam = Get()->GetCurrentRuntime()->GetCamera();
			if (cam!=NULL)
			{
				xml = cam->Write();
			}
			if (xml != NULL)
			{
				std::string str = xml->ToString();
				file.WriteData(str);
			}
		}

		file.WriteData("</settings>\n");
	}
}

void GravityForceXEdit::LoadWindowPos(Camera* camera)
{
	XmlParser p;
	std::string fname=System::GetAppDirectory()+"editorsettings.xml";
	if (p.LoadAndParse(fname))
	{
		XmlNode* root = p.GetDocumentRoot();
		XmlNode* properties = root->GetChildNode("properties");
		if (properties != NULL)
		{
			SetWindowPositionFromXML(properties, propertiesDlg);
		}

		XmlNode* components = root->GetChildNode("components");
		if (components != NULL)
		{
			SetWindowPositionFromXML(components, componentLibraryDlg);
		}

		if (camera != NULL)
		{
			if (root->HasChild(camera->Signature()))
			{
				camera->Read(root->GetChild(camera->Signature()));
			}
		}
	}
}

BOOL GravityForceXEdit::OnIdle(LONG lCount)
{
	CWinApp::OnIdle(lCount);

	Interface* intf=Interface::Get();

	if (!GetPaused() && intf->IsDeviceReady())
	{
		// if the device is not ready then attempt to reset it
		if (intf->IsDeviceReady())
		{
			Time* t = Time::Get();
			double frameTime = t->FrameTime();
			t->Update();

			GamePad::Get()->EventLogic(frameTime);
			EventLogic(frameTime);
			Draw(frameTime);
		}
		else
		{
			intf->ResetDevice();
		}
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

void GravityForceXEdit::OnClose()
{
	SaveWindowPos();
}

void GravityForceXEdit::EventLogic(double time)
{
	if (currentRuntime != NULL)
	{
		currentRuntime->EventLogic(time);
	}
}

void GravityForceXEdit::Draw(double time)
{
	if (currentRuntime!=NULL)
	{
		currentRuntime->Draw(time);
	}
}

BOOL GravityForceXEdit::InitInstance()
{
	// initialise my system
	System::Initialise();

	oldTime = 0;

	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);
	AfxEnableControlContainer();
	AfxInitRichEdit2();

	CWinApp::InitInstance();

	// Initialize OLE libraries
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	AfxEnableControlContainer();
	// To create the main window, this code creates a new frame window
	// object and then sets it as the application's main window object
	CMainFrame* pFrame = new CMainFrame(this);
	if (!pFrame)
	{
		return FALSE;
	}
	m_pMainWnd = pFrame;
	// create and load the frame with its resources
	pFrame->LoadFrame(IDR_MAINFRAME,WS_BORDER | WS_SYSMENU | WS_MINIMIZE | FWS_ADDTOTITLE, NULL,NULL);

	// The one and only window has been initialized, so show and update it
	pFrame->SetWindowPos(NULL,0,0,806,644,SWP_NOZORDER);
	pFrame->ShowWindow(SW_SHOW);
	pFrame->UpdateWindow();

	fileLog = new FileLog();

	// initialise renderer and runtimes
	editorRuntime = new EditorRuntime();
	currentRuntime = editorRuntime;

	// properties dialog
	propertiesDlg = new PropertiesDlg(pFrame);
	propertiesDlg->Create(PropertiesDlg::IDD,pFrame);
	propertiesDlg->ShowWindow(SW_SHOW);
	propertiesDlg->SetIcon(LoadIcon(MAKEINTRESOURCE(IDR_MAINFRAME)),TRUE);

	// components dialog
	componentLibraryDlg = new ComponentLibraryDlg(pFrame);
	componentLibraryDlg->Create(ComponentLibraryDlg::IDD,pFrame);
	componentLibraryDlg->ShowWindow(SW_SHOW);
	componentLibraryDlg->SetIcon(LoadIcon(MAKEINTRESOURCE(IDR_MAINFRAME)),TRUE);

	pFrame->SetWindowPos(NULL,0,0,806,644,SWP_NOZORDER);
	pFrame->UpdateWindow();
	pFrame->ShowWindow(SW_SHOW);

	// setup windows as before
	Camera camera;
	LoadWindowPos(&camera);

	CWnd* childView = CChildView::GetChildView();
	if (!Interface::Get()->InitialiseDevice(*childView))
	{
		ShowErrorMessage("Fatal error\nYour display device does not support this application.", "Startup Error");
		return FALSE;
	}

	pFrame->SetIcon(LoadIcon(MAKEINTRESOURCE(IDR_MAINFRAME)),TRUE);
	pFrame->ActivateFrame();
	pFrame->UpdateWindow();

	editorRuntime->Initialise();

	// notify scene grap and runtime of the new (initial) level
	editorRuntime->SetCamera(camera);

	// load app main font
	Interface::GetDevice()->GetSmallFont()->Load("neuropol10.fnt");

	// initialise object system
	ObjectFactory::Initialise();
	Log::GetLog() << "loaded library\n";

	// initialise game pads
	GamePad::Get();
	Keyboard::Get();

	Log::GetLog() << "initialisation successful\n";

	return TRUE;
}

void GravityForceXEdit::OnAppAbout()
{
}

void GravityForceXEdit::OnWireframe()
{
	if (currentRuntime!=NULL)
	{
		currentRuntime->SetWireframe(!currentRuntime->GetWireframe());
		CMenu* menu = GetMainWnd()->GetMenu();
		menu->CheckMenuItem(ID_VIEW_WIREFRAME,currentRuntime->GetWireframe()?MF_CHECKED:MF_UNCHECKED);
	}
}

void GravityForceXEdit::OnCullCCW()
{
	if (currentRuntime!=NULL)
	{
		currentRuntime->SetCullCCW(!currentRuntime->GetCullCCW());
		CMenu* menu = GetMainWnd()->GetMenu();
		menu->CheckMenuItem(ID_VIEW_CULLCCW,currentRuntime->GetCullCCW()?MF_CHECKED:MF_UNCHECKED);
	}
}

void GravityForceXEdit::OnFileOpen()
{
	std::string fname=GetFilenameForLoad("level files (.xml)|*.xml|all files|*.*||",".xml","");
	if (!fname.empty())
	{
		levelFilename=fname;

		if (System::ContainsLwr(fname,".xml"))
		{
			XmlParser p;
			std::string strippedFname = System::RemoveDataDirectory(fname);
			ProgressDlg* dlg = ProgressDlg::Start(m_pMainWnd,"loading " + strippedFname);
			bool result = p.LoadAndParse(fname);
			dlg->End();

			if (result)
			{
				try
				{
					XmlNode* root = p.GetDocumentRoot();

					Runtime* rt = BaseApp::Get()->GetCurrentRuntime();
					
					Level* level = new Level();
					level->Read(root->GetChild(level->Signature()));

					// and the current camera
					Camera* camera = new Camera();
					camera->Read(root->GetChild(camera->Signature()));
					rt->SetCamera(camera);

					// set level
					rt->SetCurrentLevel(level);

				}
				catch (Exception* ex)
				{
					ShowErrorMessage(ex->Message(), "error loading level");
				}
			}
		}
	}
}

void GravityForceXEdit::SaveLevel(const std::string& fname)
{
	Level* level = editorRuntime->GetCurrentLevel();

	if (System::ContainsLwr(fname,".xml"))
	{
		XmlNode f;
		f.SetTag("gravityforcex");

		// save level
		f.Add(level->Write());

		// and current camera
		Camera* camera=Get()->GetCurrentRuntime()->GetCamera();
		f.Add(camera->Write());

		FileStreamer file(BaseStreamer::STREAM_WRITE);
		std::string str = f.ToString();
		if (file.Open(levelFilename))
		{
			file.WriteData("gravityforcex", str.c_str(), str.size());
			file.Close();
		}
	}
}

void GravityForceXEdit::OnFileSave()
{
	if (!levelFilename.empty())
	{
		SaveLevel(levelFilename);
	}
	else
	{
		OnFileSaveAs();
	}
}

void GravityForceXEdit::OnFileSaveAs()
{
	std::string fname=GetFilenameForSave("xml level (.xml)|*.xml|binary level (.lvl)|*.lvl|all files|*.*||",".lvl","");
	if (!fname.empty())
	{
		levelFilename=fname;
		SaveLevel(levelFilename);
	}
}

bool GravityForceXEdit::ColourPicker(const D3DXCOLOR& in,D3DXCOLOR& out)
{
	COLORREF cr = in;
	cr = (cr&0xff0000)>>16  | (cr&0xff00) | (cr&0xff)<<16;
	CColorDialog dlg(cr,0,m_pMainWnd);
	if (dlg.DoModal()==IDOK)
	{
		cr = dlg.GetColor();
		cr = (cr&0xff0000)>>16  | (cr&0xff00) | (cr&0xff)<<16;
		out = cr;
		return true;
	}
	return false;
}

std::string GravityForceXEdit::GetFilenameForLoad(const std::string& filter,const std::string& extn,const std::string& directory)
{
	CFileDialog dlg(TRUE,NULL,NULL,OFN_HIDEREADONLY,filter.c_str(),m_pMainWnd);
	dlg.m_ofn.lpstrDefExt=extn.c_str();
	std::string initDir = System::GetDataDirectory();
	dlg.m_ofn.lpstrInitialDir=initDir.c_str();
	if (directory.empty())
	{
		dlg.m_ofn.lpstrInitialDir=System::GetDataDirectory().c_str();
	}
	else
	{
		std::string dir=System::GetDataDirectory()+directory;
		dlg.m_ofn.lpstrInitialDir=dir.c_str();
	}
	if (dlg.DoModal()==IDOK)
	{
		std::string str=dlg.GetPathName();
		return str;
	}
	return "";
}

std::string GravityForceXEdit::GetFilenameForSave(const std::string& filter,const std::string& extn,const std::string& directory)
{
	CFileDialog dlg(FALSE,NULL,NULL,OFN_HIDEREADONLY,filter.c_str(),m_pMainWnd);
	dlg.m_ofn.lpstrDefExt=extn.c_str();
	std::string initDir = System::GetDataDirectory();
	dlg.m_ofn.lpstrInitialDir=initDir.c_str();
	if (directory.empty())
	{
		dlg.m_ofn.lpstrInitialDir=System::GetDataDirectory().c_str();
	}
	else
	{
		std::string dir=System::GetDataDirectory()+directory;
		dlg.m_ofn.lpstrInitialDir=dir.c_str();
	}
	if (dlg.DoModal()==IDOK)
	{
		std::string str=dlg.GetPathName();
		return str;
	}
	return "";
}

void GravityForceXEdit::ForceProcessMessages()
{
	MSG msg;
	while (::PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE ))
	{
		PumpMessage();
	}
}

void GravityForceXEdit::ShowErrorMessage(std::string msg,std::string title)
{
	m_pMainWnd->MessageBox(msg.c_str(),title.c_str(),MB_OK|MB_ICONERROR);
}

void GravityForceXEdit::Quit()
{
}

void GravityForceXEdit::QuitGame()
{
}

Level* GravityForceXEdit::Load(const std::string& fname)
{
	return NULL;
}

