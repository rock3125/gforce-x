#include "editor/stdafx.h"

#include "d3d9/texture.h"

#include "interface/EditShipDlg.h"
#include "interface/progressDlg.h"

#include "system/BaseApp.h"
#include "system/model/model.h"
#include "game/ship.h"

#include "system/converters/vrml/vrmlParser.h"

/////////////////////////////////////////////////////////////////////////////
// log dialog

EditShipDlg::EditShipDlg(CWnd* pParent)
	: GenericPropertyPage(EditShipDlg::IDD,pParent)
	, obj(NULL)
{
	initialised=false;
}

EditShipDlg::~EditShipDlg()
{
	obj=NULL;
}

void EditShipDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(EditShipDlg)
	DDX_Control(pDX,IDC_MODEL_SELECTMESH,bSelectFile);
	DDX_Control(pDX,IDC_MODEL_MESHFILENAME,meshFilename);
	DDX_Control(pDX,IDC_MODEL_TEXTUREFILENAME,textureFilename);
	DDX_Control(pDX,IDC_MODEL_TRANSPARENCY,transparency);
	DDX_Control(pDX,IDC_MODEL_COLOUR,colour);

	DDX_Control(pDX,IDC_MAX_SPEED, maxSpeed);
	DDX_Control(pDX,IDC_MAX_ACCEL, maxAccel);
	DDX_Control(pDX,IDC_ACCELERATION, accel);
	DDX_Control(pDX,IDC_FUEL_USAGE, fuelUsage);
	DDX_Control(pDX,IDC_ROTATIONAL_VELOCITY, rotationalVelocity);
	DDX_Control(pDX,IDC_HEIGHT_OFFSET, baseOffset);

	DDX_Control(pDX,IDC_BULLETMODEL_MESHFILENAME,bulletMeshFilename);
	DDX_Control(pDX,IDC_BULLETMODEL_TEXTUREFILENAME,bulletTextureFilename);
	DDX_Control(pDX,IDC_BULLETMODEL_COLOUR,bulletColour);
	//}}AFX_DATA_MAP

	initialised=true;
	Resize();
	SetupData();
}

BEGIN_MESSAGE_MAP(EditShipDlg,CDialog)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_MODEL_SELECTMESH,OnSelectMesh)
	ON_BN_CLICKED(IDC_MODEL_SELECTTEXTURE,OnSelectTexture)
	ON_BN_CLICKED(IDC_MODEL_SELECTCOLOUR,OnSelectColour)
	ON_EN_KILLFOCUS(IDC_MODEL_TRANSPARENCY,OnKillfocusTransparency)

	ON_EN_KILLFOCUS(IDC_MAX_SPEED, OnKillfocusMaxSpeed)
	ON_EN_KILLFOCUS(IDC_MAX_ACCEL, OnKillfocusMaxAccel)
	ON_EN_KILLFOCUS(IDC_ACCELERATION, OnKillfocusAccel)
	ON_EN_KILLFOCUS(IDC_FUEL_USAGE, OnKillfocusFuelUsage)
	ON_EN_KILLFOCUS(IDC_ROTATIONAL_VELOCITY, OnKillfocusRotationalVelocity)
	ON_EN_KILLFOCUS(IDC_HEIGHT_OFFSET, OnKillfocusBaseOffset)

	ON_BN_CLICKED(IDC_BULLETMODEL_SELECTMESH,OnSelectBulletMesh)
	ON_BN_CLICKED(IDC_BULLETMODEL_SELECTTEXTURE,OnSelectBulletTexture)
	ON_BN_CLICKED(IDC_BULLETMODEL_SELECTCOLOUR,OnSelectBulletColour)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// EditShipDlg message handlers

void EditShipDlg::OnOK()
{
}

void EditShipDlg::OnCancel()
{
	ShowWindow(SW_HIDE);
}

void EditShipDlg::OnSize(UINT,int,int)
{
	Resize();
}

void EditShipDlg::Resize()
{
	if (initialised)
	{
		CRect r;
		GetClientRect(&r);
	}
}

void EditShipDlg::SetupData()
{
	if (obj!=NULL)
	{
		Model* model = obj->GetModel();
		meshFilename.SetWindowText(model->GetName().c_str());
		Texture* texture = model->GetTexture();
		if (texture != NULL)
			textureFilename.SetWindowText(texture->GetFilename().c_str());
		else
			textureFilename.SetWindowText("");
		float t = model->GetColour().a;
		transparency.SetWindowText(System::Float2Str(t).c_str());
		colour.SetColour(model->GetColour());

		maxSpeed.SetWindowText(System::Float2Str(obj->GetMaxSpeed()).c_str());
		maxAccel.SetWindowText(System::Float2Str(obj->GetMaxAccel()).c_str());
		accel.SetWindowText(System::Float2Str(obj->GetAccel()).c_str());
		fuelUsage.SetWindowText(System::Float2Str(obj->GetFuelUsage()).c_str());
		rotationalVelocity.SetWindowText(System::Float2Str(obj->GetRotationalVelocity()).c_str());
		baseOffset.SetWindowText(System::Float2Str(obj->GetBaseOffset()).c_str());

		// set bullet properties
		model = obj->GetBulletModel();
		bulletMeshFilename.SetWindowText(model->GetName().c_str());
		texture = model->GetTexture();
		if (texture != NULL)
			bulletTextureFilename.SetWindowText(texture->GetFilename().c_str());
		else
			bulletTextureFilename.SetWindowText("");
		bulletColour.SetColour(model->GetColour());
	}
}

void EditShipDlg::Activate()
{
	bSelectFile.SetFocus();
}

void EditShipDlg::SetData(WorldObject* _obj)
{
	obj = dynamic_cast<Ship*>(_obj);
	SetupData();
}

void EditShipDlg::OnSelectTexture()
{
	if (obj!=NULL)
	{
		std::string fname=BaseApp::Get()->GetFilenameForLoad("tga, bmp, png, jpg|*.tga;*.bmp;*.png;*.jpg;*.jpeg|all files|*.*||",".tga","");
		if (!fname.empty())
		{
			fname=System::RemoveDataDirectory(fname);
			Model* model = obj->GetModel();
			Texture* texture = TextureCache::GetTexture(fname);
			model->SetTexture(texture);
			textureFilename.SetWindowText(texture->GetFilename().c_str());
		}
	}
}

void EditShipDlg::OnSelectColour()
{
	if (obj!=NULL)
	{
		D3DXCOLOR newColour;
		Model* model = obj->GetModel();
		D3DXCOLOR oldColour = model->GetColour();
		if (BaseApp::Get()->ColourPicker(oldColour,newColour))
		{
			newColour.a = oldColour.a;
			model->SetColour(newColour);
			colour.SetColour(newColour);
		}
	}
}

void EditShipDlg::OnSelectMesh()
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
					obj->SetModel(model);
					safe_delete(model);
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
}

void EditShipDlg::OnKillfocusTransparency()
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
}

void EditShipDlg::OnKillfocusMaxSpeed()
{
	if (obj!=NULL)
	{
		char buf[256];
		maxSpeed.GetWindowText(buf,255);
		float a = System::Str2Float(buf);

		maxSpeed.SetWindowText(System::Float2Str(a).c_str());
		obj->SetMaxSpeed(a);
	}
}

void EditShipDlg::OnKillfocusMaxAccel()
{
	if (obj!=NULL)
	{
		char buf[256];
		maxAccel.GetWindowText(buf,255);
		float a = System::Str2Float(buf);

		maxAccel.SetWindowText(System::Float2Str(a).c_str());
		obj->SetMaxAccel(a);
	}
}

void EditShipDlg::OnKillfocusAccel()
{
	if (obj!=NULL)
	{
		char buf[256];
		accel.GetWindowText(buf,255);
		float a = System::Str2Float(buf);

		accel.SetWindowText(System::Float2Str(a).c_str());
		obj->SetAccel(a);
	}
}

void EditShipDlg::OnKillfocusFuelUsage()
{
	if (obj!=NULL)
	{
		char buf[256];
		fuelUsage.GetWindowText(buf,255);
		float a = System::Str2Float(buf);

		fuelUsage.SetWindowText(System::Float2Str(a).c_str());
		obj->SetFuelUsage(a);
	}
}

void EditShipDlg::OnKillfocusRotationalVelocity()
{
	if (obj!=NULL)
	{
		char buf[256];
		rotationalVelocity.GetWindowText(buf,255);
		float a = System::Str2Float(buf);

		rotationalVelocity.SetWindowText(System::Float2Str(a).c_str());
		obj->SetRotationalVelocity(a);
	}
}

void EditShipDlg::OnKillfocusBaseOffset()
{
	if (obj!=NULL)
	{
		char buf[256];
		baseOffset.GetWindowText(buf,255);
		float a = System::Str2Float(buf);

		baseOffset.SetWindowText(System::Float2Str(a).c_str());
		obj->SetBaseOffset(a);
	}
}

void EditShipDlg::OnSelectBulletTexture()
{
	if (obj!=NULL)
	{
		std::string fname=BaseApp::Get()->GetFilenameForLoad("tga, bmp, png, jpg|*.tga;*.bmp;*.png;*.jpg;*.jpeg|all files|*.*||",".tga","");
		if (!fname.empty())
		{
			fname=System::RemoveDataDirectory(fname);
			Model* model = obj->GetBulletModel();
			Texture* texture = TextureCache::GetTexture(fname);
			model->SetTexture(texture);
			bulletTextureFilename.SetWindowText(texture->GetFilename().c_str());
		}
	}
}

void EditShipDlg::OnSelectBulletColour()
{
	if (obj!=NULL)
	{
		D3DXCOLOR newColour;
		Model* model = obj->GetBulletModel();
		D3DXCOLOR oldColour = model->GetColour();
		if (BaseApp::Get()->ColourPicker(oldColour,newColour))
		{
			newColour.a = oldColour.a;
			model->SetColour(newColour);
			bulletColour.SetColour(newColour);
		}
	}
}

void EditShipDlg::OnSelectBulletMesh()
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
					obj->SetBulletModel(model);
					safe_delete(model);
					fname=System::RemoveDataDirectory(fname);
					bulletMeshFilename.SetWindowText(fname.c_str());
				}
			}
			else
			{
				MessageBox(p.GetError().c_str(),"Error loading VRML model",MB_OK|MB_ICONERROR);
			}
		}
	}
}

