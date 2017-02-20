#include "editor/stdafx.h"

#include "d3d9/texture.h"

#include "interface/editModelDlg.h"
#include "interface/progressDlg.h"

#include "system/BaseApp.h"
#include "system/model/model.h"

#include "system/converters/vrml/vrmlParser.h"

/////////////////////////////////////////////////////////////////////////////
// log dialog

EditModelDlg::EditModelDlg(CWnd* pParent)
	: GenericPropertyPage(EditModelDlg::IDD,pParent)
	, obj(NULL)
{
	initialised=false;
}

EditModelDlg::~EditModelDlg()
{
	obj=NULL;
}

void EditModelDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(EditModelDlg)
	DDX_Control(pDX,IDC_MODEL_SELECTMESH,bSelectFile);
	DDX_Control(pDX,IDC_MODEL_MESHFILENAME,meshFilename);
	DDX_Control(pDX,IDC_MODEL_TEXTUREFILENAME,textureFilename);
	DDX_Control(pDX,IDC_MODEL_TRANSPARENCY,transparency);
	DDX_Control(pDX,IDC_MODEL_COLOUR,colour);
	//}}AFX_DATA_MAP

	initialised=true;
	Resize();
	SetupData();
}

BEGIN_MESSAGE_MAP(EditModelDlg,CDialog)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_MODEL_SELECTMESH,OnSelectMesh)
	ON_BN_CLICKED(IDC_MODEL_SELECTTEXTURE,OnSelectTexture)
	ON_BN_CLICKED(IDC_MODEL_SELECTCOLOUR,OnSelectColour)
	ON_EN_KILLFOCUS(IDC_MODEL_TRANSPARENCY,OnKillfocusTransparency)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// EditModelDlg message handlers

void EditModelDlg::OnOK()
{
};

void EditModelDlg::OnCancel()
{
	ShowWindow(SW_HIDE);
};

void EditModelDlg::OnSize(UINT,int,int)
{
	Resize();
};

void EditModelDlg::Resize()
{
	if (initialised)
	{
		CRect r;
		GetClientRect(&r);
	};
};

void EditModelDlg::SetupData()
{
	if (obj!=NULL)
	{
		meshFilename.SetWindowText("");
		Texture* texture = obj->GetTexture();
		if (texture != NULL)
			textureFilename.SetWindowText(texture->GetFilename().c_str());
		else
			textureFilename.SetWindowText("");
		float t = obj->GetColour().a;
		transparency.SetWindowText(System::Float2Str(t).c_str());
		colour.SetColour(obj->GetColour());
	};
};

void EditModelDlg::Activate()
{
	bSelectFile.SetFocus();
};

void EditModelDlg::SetData(WorldObject* _obj)
{
	obj=dynamic_cast<Model*>(_obj);
	SetupData();
};

void EditModelDlg::OnSelectTexture()
{
	if (obj!=NULL)
	{
		std::string fname=BaseApp::Get()->GetFilenameForLoad("tga, bmp, png, jpg|*.tga;*.bmp;*.png;*.jpg;*.jpeg|all files|*.*||",".tga","");
		if (!fname.empty())
		{
			fname=System::RemoveDataDirectory(fname);
			Texture* texture = TextureCache::GetTexture(fname);
			obj->SetTexture(texture);
			textureFilename.SetWindowText(texture->GetFilename().c_str());
		}
	}
}

void EditModelDlg::OnSelectColour()
{
	if (obj!=NULL)
	{
		D3DXCOLOR newColour;
		D3DXCOLOR oldColour = obj->GetColour();
		if (BaseApp::Get()->ColourPicker(oldColour,newColour))
		{
			newColour.a = oldColour.a;
			obj->SetColour(newColour);
			colour.SetColour(newColour);
		}
	}
}

void EditModelDlg::OnSelectMesh()
{
	if (obj!=NULL)
	{
		std::string fname=BaseApp::Get()->GetFilenameForLoad("vrml model (wrl)|*.wrl|xml mesh (xml)|*.xml|all files|*.*||",".xml","");
		if (!fname.empty())
		{
			std::string errStr;
			VRMLParser p;
			p.Load(fname);
			if (!p.HasErrors())
			{
				std::string title = "Loading " + System::RemoveDataDirectory(fname);
				ProgressDlg* dlg = ProgressDlg::Start(this,title);
				Model* model = dynamic_cast<Model*>(p.ParseLoadedFile());
				dlg->End();

				if (model==NULL)
				{
					MessageBox(p.GetError().c_str(),"Error loading VRML model",MB_OK|MB_ICONERROR);
				}
				else
				{
					*obj = *model;
					fname=System::RemoveDataDirectory(fname);
					meshFilename.SetWindowText(fname.c_str());
				}
			}
			else
			{
				MessageBox(p.GetError().c_str(),"Error loading VRML model",MB_OK|MB_ICONERROR);
			}
		}
	}
};

void EditModelDlg::OnKillfocusTransparency()
{
	if (obj!=NULL)
	{
		char buf[256];
		transparency.GetWindowText(buf,255);
		float a = System::Str2Float(buf);
		if (a<0) a = 0;
		if (a>1 ) a = 1;

		transparency.SetWindowText(System::Float2Str(a).c_str());

		D3DXCOLOR c = obj->GetColour();
		c.a = a;
		obj->SetColour(c);
	}
};


