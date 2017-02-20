#include "editor/stdafx.h"

#include "d3d9/texture.h"

#include "interface/editModelMapDlg.h"

#include "system/BaseApp.h"
#include "game/modelMap.h"

/////////////////////////////////////////////////////////////////////////////
// log dialog

EditModelMapDlg::EditModelMapDlg(CWnd* pParent)
	: GenericPropertyPage(EditModelMapDlg::IDD,pParent)
	, obj(NULL)
{
	initialised=false;
}

EditModelMapDlg::~EditModelMapDlg()
{
	obj = NULL;
}

void EditModelMapDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(EditModelMapDlg)
	DDX_Control(pDX,IDC_MODELMAP_SELECT,bSelectModelMap);
	DDX_Control(pDX,IDC_MODELMAP_TEXTURE,modelMapFilename);
	DDX_Control(pDX,IDC_BACK_TEXTUREFILENAME,backgroundFilename);
	DDX_Control(pDX,IDC_FORE_TEXTUREFILENAME,foregroundFilename);
	DDX_Control(pDX,IDC_SIDE_COLOURTOP,sideColourTop);
	DDX_Control(pDX,IDC_SIDE_COLOURBOTTOM,sideColourBottom);
	DDX_Control(pDX,IDC_EDIT_GRAVITY,gravity);
	//}}AFX_DATA_MAP

	initialised=true;
	Resize();
	SetupData();
}

BEGIN_MESSAGE_MAP(EditModelMapDlg,CDialog)
	ON_WM_SIZE()

	ON_BN_CLICKED(IDC_MODELMAP_SELECT,OnSelectModelMap)
	ON_BN_CLICKED(IDC_BACKGROUND_TEXTURE,OnSelectBackgroundTexture)
	ON_BN_CLICKED(IDC_FOREGROUND_TEXTURE,OnSelectForegroundTexture)
	ON_BN_CLICKED(IDC_UPDATEBUTTON,OnUpdate)

	ON_BN_CLICKED(IDC_SELECT_SIDE_COLOURTOP,OnSelectSideColourTop)
	ON_BN_CLICKED(IDC_SELECT_SIDE_COLOURBOTTOM,OnSelectSideColourBottom)

	ON_EN_KILLFOCUS(IDC_EDIT_GRAVITY, OnKillfocusGravity)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// EditModelMapDlg message handlers

void EditModelMapDlg::OnOK()
{
}

void EditModelMapDlg::OnCancel()
{
	ShowWindow(SW_HIDE);
}

void EditModelMapDlg::OnSize(UINT,int,int)
{
	Resize();
}

void EditModelMapDlg::Resize()
{
	if (initialised)
	{
		CRect r;
		GetClientRect(&r);
	}
}

void EditModelMapDlg::SetupData()
{
	if (obj!=NULL)
	{
		modelMapFilename.SetWindowText(obj->GetMapFilename().c_str());
		backgroundFilename.SetWindowText(obj->GetBackgroundTextureFilename().c_str());
		foregroundFilename.SetWindowText(obj->GetForegroundTextureFilename().c_str());
		sideColourTop.SetColour(obj->GetSideColourTop());
		sideColourBottom.SetColour(obj->GetSideColourBottom());
		gravity.SetWindowText(System::Float2Str(obj->GetGravity()).c_str());
	}
}

void EditModelMapDlg::Activate()
{
	bSelectModelMap.SetFocus();
}

void EditModelMapDlg::SetData(WorldObject* _obj)
{
	obj=dynamic_cast<ModelMap*>(_obj);
	SetupData();
}

void EditModelMapDlg::OnSelectBackgroundTexture()
{
	if (obj!=NULL)
	{
		std::string fname = BaseApp::Get()->GetFilenameForLoad("tga, bmp, png, jpg|*.tga;*.bmp;*.png;*.jpg;*.jpeg|all files|*.*||",".tga","");
		if (!fname.empty())
		{
			fname = System::RemoveDataDirectory(fname);
			obj->SetBackgroundTextureFilename(fname);
			backgroundFilename.SetWindowText(obj->GetBackgroundTextureFilename().c_str());
		}
	}
}

void EditModelMapDlg::OnSelectForegroundTexture()
{
	if (obj!=NULL)
	{
		std::string fname = BaseApp::Get()->GetFilenameForLoad("tga, bmp, png, jpg|*.tga;*.bmp;*.png;*.jpg;*.jpeg|all files|*.*||",".tga","");
		if (!fname.empty())
		{
			fname = System::RemoveDataDirectory(fname);
			obj->SetForegroundTextureFilename(fname);
			foregroundFilename.SetWindowText(obj->GetForegroundTextureFilename().c_str());
		}
	}
}

void EditModelMapDlg::OnSelectModelMap()
{
	if (obj != NULL)
	{
		std::string fname = BaseApp::Get()->GetFilenameForLoad("tga, bmp, png, jpg|*.tga;*.bmp;*.png;*.jpg;*.jpeg|all files|*.*||",".tga","");
		if (!fname.empty())
		{
			fname = System::RemoveDataDirectory(fname);
			obj->SetMapFilename(fname);
			modelMapFilename.SetWindowText(obj->GetMapFilename().c_str());
		}
	}
}

void EditModelMapDlg::OnUpdate()
{
	if (obj != NULL)
	{
		obj->Update();
	}
}

void EditModelMapDlg::OnSelectSideColourTop()
{
	if (obj!=NULL)
	{
		D3DXCOLOR newColour;
		D3DXCOLOR oldColour = obj->GetSideColourTop();
		if (BaseApp::Get()->ColourPicker(oldColour,newColour))
		{
			newColour.a = oldColour.a;
			obj->SetSideColourTop(newColour);
			sideColourTop.SetColour(newColour);
		}
	}
}

void EditModelMapDlg::OnSelectSideColourBottom()
{
	if (obj!=NULL)
	{
		D3DXCOLOR newColour;
		D3DXCOLOR oldColour = obj->GetSideColourBottom();
		if (BaseApp::Get()->ColourPicker(oldColour,newColour))
		{
			newColour.a = oldColour.a;
			obj->SetSideColourBottom(newColour);
			sideColourBottom.SetColour(newColour);
		}
	}
}

void EditModelMapDlg::OnKillfocusGravity()
{
	if (obj!=NULL)
	{
		char buf[256];
		gravity.GetWindowText(buf,255);
		float f = System::Str2Float(buf);
		obj->SetGravity(f);
	}
}

