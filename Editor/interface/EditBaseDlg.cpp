#include "editor/stdafx.h"

#include "d3d9/texture.h"

#include "interface/EditBaseDlg.h"
#include "interface/progressDlg.h"

#include "system/BaseApp.h"
#include "system/model/model.h"

#include "game/base.h"

#include "system/converters/vrml/vrmlParser.h"

/////////////////////////////////////////////////////////////////////////////
// log dialog

EditBaseDlg::EditBaseDlg(CWnd* pParent)
	: GenericPropertyPage(EditBaseDlg::IDD,pParent)
	, obj(NULL)
{
	initialised=false;
}

EditBaseDlg::~EditBaseDlg()
{
	obj=NULL;
}

void EditBaseDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(EditBaseDlg)
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

BEGIN_MESSAGE_MAP(EditBaseDlg,CDialog)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_MODEL_SELECTMESH,OnSelectMesh)
	ON_BN_CLICKED(IDC_MODEL_SELECTTEXTURE,OnSelectTexture)
	ON_BN_CLICKED(IDC_MODEL_SELECTCOLOUR,OnSelectColour)
	ON_EN_KILLFOCUS(IDC_MODEL_TRANSPARENCY,OnKillfocusTransparency)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// EditBaseDlg message handlers

void EditBaseDlg::OnOK()
{
};

void EditBaseDlg::OnCancel()
{
	ShowWindow(SW_HIDE);
};

void EditBaseDlg::OnSize(UINT,int,int)
{
	Resize();
};

void EditBaseDlg::Resize()
{
	if (initialised)
	{
		CRect r;
		GetClientRect(&r);
	};
};

void EditBaseDlg::SetupData()
{
	if (obj!=NULL)
	{
		Model* model = obj->GetModel();
		meshFilename.SetWindowText("");
		Texture* texture = model->GetTexture();
		if (texture != NULL)
			textureFilename.SetWindowText(texture->GetFilename().c_str());
		else
			textureFilename.SetWindowText("");
		float t = model->GetColour().a;
		transparency.SetWindowText(System::Float2Str(t).c_str());
		colour.SetColour(model->GetColour());
	};
};

void EditBaseDlg::Activate()
{
	bSelectFile.SetFocus();
};

void EditBaseDlg::SetData(WorldObject* _obj)
{
	obj=dynamic_cast<Base*>(_obj);
	SetupData();
};

void EditBaseDlg::OnSelectTexture()
{
	if (obj!=NULL)
	{
		std::string fname=BaseApp::Get()->GetFilenameForLoad("tga, bmp, png, jpg|*.tga;*.bmp;*.png;*.jpg;*.jpeg|all files|*.*||",".tga","");
		if (!fname.empty())
		{
			Model* model = obj->GetModel();
			fname=System::RemoveDataDirectory(fname);
			Texture* texture = TextureCache::GetTexture(fname);
			model->SetTexture(texture);
			textureFilename.SetWindowText(texture->GetFilename().c_str());
		}
	}
}

void EditBaseDlg::OnSelectColour()
{
	if (obj!=NULL)
	{
		Model* model = obj->GetModel();
		D3DXCOLOR newColour;
		D3DXCOLOR oldColour = model->GetColour();
		if (BaseApp::Get()->ColourPicker(oldColour,newColour))
		{
			newColour.a = oldColour.a;
			model->SetColour(newColour);
			colour.SetColour(newColour);
		}
	}
}

void EditBaseDlg::OnSelectMesh()
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
					Model* model2 = obj->GetModel();
					*model2 = *model;
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

void EditBaseDlg::OnKillfocusTransparency()
{
	if (obj!=NULL)
	{
		char buf[256];
		transparency.GetWindowText(buf,255);
		float a = System::Str2Float(buf);
		if (a<0) a = 0;
		if (a>1 ) a = 1;

		transparency.SetWindowText(System::Float2Str(a).c_str());

		Model* model = obj->GetModel();
		D3DXCOLOR c = model->GetColour();
		c.a = a;
		model->SetColour(c);
	}
};


