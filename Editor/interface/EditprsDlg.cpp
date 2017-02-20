#include "editor/stdafx.h"

#include "interface/EditPRSDlg.h"
#include "system/model/worldObject.h"

/////////////////////////////////////////////////////////////////////////////
// log dialog

EditPRSDlg::EditPRSDlg(CWnd* pParent)
	: GenericPropertyPage(EditPRSDlg::IDD,pParent)
	, obj(NULL)
{
	initialised=false;
}

EditPRSDlg::~EditPRSDlg()
{
	obj=NULL;
}

void EditPRSDlg::SetData(WorldObject* _obj)
{
	obj=_obj;
	SetupData();
};

void EditPRSDlg::Activate()
{
	xpos.SetFocus();
};

void EditPRSDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(EditPRSDlg)
	DDX_Control(pDX,IDC_PRS_XROT,rotx);
	DDX_Control(pDX,IDC_PRS_YROT,roty);
	DDX_Control(pDX,IDC_PRS_ZROT,rotz);
	DDX_Control(pDX,IDC_PRS_XPOSBUT,movex);
	DDX_Control(pDX,IDC_PRS_YPOSBUT,movey);
	DDX_Control(pDX,IDC_PRS_ZPOSBUT,movez);

	DDX_Control(pDX,IDC_PRS_XPOS,xpos);
	DDX_Control(pDX,IDC_PRS_YPOS,ypos);
	DDX_Control(pDX,IDC_PRS_ZPOS,zpos);
	DDX_Control(pDX,IDC_PRS_XROTBUT,xslider);
	DDX_Control(pDX,IDC_PRS_YROTBUT,yslider);
	DDX_Control(pDX,IDC_PRS_ZROTBUT,zslider);

	DDX_Control(pDX,IDC_PRS_XSCALE,scalex);
	DDX_Control(pDX,IDC_PRS_YSCALE,scaley);
	DDX_Control(pDX,IDC_PRS_ZSCALE,scalez);
	DDX_Control(pDX,IDC_PRS_XSCALEBUT,scalexbut);
	DDX_Control(pDX,IDC_PRS_YSCALEBUT,scaleybut);
	DDX_Control(pDX,IDC_PRS_ZSCALEBUT,scalezbut);
	//}}AFX_DATA_MAP

	SetupData();

	initialised=true;
	Resize();
}

BEGIN_MESSAGE_MAP(EditPRSDlg,CDialog)
	ON_WM_SIZE()

	ON_MESSAGE(MTrackButton::WM_USER_TRANSLATE,OnTranslate)

	ON_EN_KILLFOCUS(IDC_PRS_XPOS,OnKillfocusXpos)
	ON_EN_KILLFOCUS(IDC_PRS_YPOS,OnKillfocusYpos)
	ON_EN_KILLFOCUS(IDC_PRS_ZPOS,OnKillfocusZpos)

	ON_EN_KILLFOCUS(IDC_PRS_XROT,OnKillfocusRotation)
	ON_EN_KILLFOCUS(IDC_PRS_YROT,OnKillfocusRotation)
	ON_EN_KILLFOCUS(IDC_PRS_ZROT,OnKillfocusRotation)

	ON_EN_KILLFOCUS(IDC_PRS_XSCALE,OnKillfocusScale)
	ON_EN_KILLFOCUS(IDC_PRS_YSCALE,OnKillfocusScale)
	ON_EN_KILLFOCUS(IDC_PRS_ZSCALE,OnKillfocusScale)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// EditPRSDlg message handlers

void EditPRSDlg::SetupData()
{
	if (obj!=NULL)
	{
		rotx.EnableWindow(TRUE);
		roty.EnableWindow(TRUE);
		rotz.EnableWindow(TRUE);

		scalex.EnableWindow(TRUE);
		scaley.EnableWindow(TRUE);
		scalez.EnableWindow(TRUE);

		xpos.EnableWindow(TRUE);
		ypos.EnableWindow(TRUE);
		zpos.EnableWindow(TRUE);

		// get local position
		D3DXVECTOR3 p=obj->GetPosition();
		xpos.SetWindowText(System::Float2Str(p.x).c_str());
		ypos.SetWindowText(System::Float2Str(p.y).c_str());
		zpos.SetWindowText(System::Float2Str(p.z).c_str());

		// scale
		p=obj->GetScale();
		scalex.SetWindowText(System::Float2Str(p.x).c_str());
		scaley.SetWindowText(System::Float2Str(p.y).c_str());
		scalez.SetWindowText(System::Float2Str(p.z).c_str());
	}
	else
	{
		rotx.EnableWindow(FALSE);
		roty.EnableWindow(FALSE);
		rotz.EnableWindow(FALSE);

		xpos.EnableWindow(FALSE);
		ypos.EnableWindow(FALSE);
		zpos.EnableWindow(FALSE);

		scalex.EnableWindow(FALSE);
		scaley.EnableWindow(FALSE);
		scalez.EnableWindow(FALSE);
	}
};

void EditPRSDlg::OnOK()
{
};

void EditPRSDlg::OnCancel()
{
};

void EditPRSDlg::OnSize(UINT,int,int)
{
	Resize();
};

void EditPRSDlg::Resize()
{
	if (initialised)
	{
		CRect r;
		GetClientRect(&r);
	};
};

LRESULT EditPRSDlg::OnTranslate(WPARAM button,LPARAM delta)
{
	if (obj!=NULL)
	{
		CButton* but=(CButton*)button;
		short x=LOWORD(delta);
		short y=HIWORD(delta);

		const float sensitivity = 0.5f;

		float dx=0.0f;
		float dy=0.0f;
		float dz=0.0f;

		bool scaling = false;
		if (but==&scalezbut || but==&scaleybut || but==&scalexbut)
			scaling=true;

		bool rotating = false;
		if (but==&xslider || but==&yslider || but==&zslider)
			rotating=true;

		if (but==&movex || but==&scalexbut || but==&xslider)
		{
			if (x<0)
				dx -= sensitivity;
			if (x>0)
				dx += sensitivity;
		}
		else if (but==&movey || but==&scaleybut || but==&yslider)
		{
			if (x<0)
				dy -= sensitivity;
			if (x>0)
				dy += sensitivity;
		}
		else if (but==&movez || but==&scalezbut || but==&zslider)
		{
			if (x<0)
				dz -= sensitivity;
			if (x>0)
				dz += sensitivity;
		};

		if (scaling)
		{
			D3DXVECTOR3 scale = obj->GetScale();
			scale += D3DXVECTOR3(dx,dy,dz);
			scale.x = max(0,scale.x);
			scale.y = max(0,scale.y);
			scale.z = max(0,scale.z);
			obj->SetScale(scale);

			scalex.SetWindowText(System::Float2Str(scale.x).c_str());
			scaley.SetWindowText(System::Float2Str(scale.y).c_str());
			scalez.SetWindowText(System::Float2Str(scale.z).c_str());
		}
		else if (rotating)
		{
			obj->RotateEuler(D3DXVECTOR3(dx,dy,dz));
		}
		else
		{
			D3DXVECTOR3 pos = D3DXVECTOR3(dx,dy,dz);
			obj->Translate(pos);

			// reset new pos
			pos=obj->GetPosition();
			xpos.SetWindowText(System::Float2Str(pos.x).c_str());
			ypos.SetWindowText(System::Float2Str(pos.y).c_str());
			zpos.SetWindowText(System::Float2Str(pos.z).c_str());
		}
		obj->UpdateWorldTransform();

	}
	return 0;
};

void EditPRSDlg::OnKillfocusXpos()
{
	char buf[256];
	xpos.GetWindowText(buf,255);

	if (obj!=NULL)
	{
		D3DXVECTOR3 cpos(obj->GetPosition());
		cpos.x=float(atof(buf));

		obj->SetPosition(cpos);
		obj->UpdateWorldTransform();
	}
}

void EditPRSDlg::OnKillfocusYpos()
{
	char buf[256];
	ypos.GetWindowText(buf,255);

	if (obj!=NULL)
	{
		D3DXVECTOR3 cpos(obj->GetPosition());
		cpos.y=float(atof(buf));

		obj->SetPosition(cpos);
		obj->UpdateWorldTransform();
	}
}

void EditPRSDlg::OnKillfocusZpos()
{
	char buf[256];
	zpos.GetWindowText(buf,255);

	if (obj!=NULL)
	{
		D3DXVECTOR3 cpos(obj->GetPosition());
		cpos.z=float(atof(buf));

		obj->SetPosition(cpos);
		obj->UpdateWorldTransform();
	}
}

void EditPRSDlg::OnKillfocusRotation()
{
	char buf[256];
	rotz.GetWindowText(buf,255);
	float rz = float(atof(buf));

	rotx.GetWindowText(buf,255);
	float rx = float(atof(buf));

	roty.GetWindowText(buf,255);
	float ry = float(atof(buf));

	if (obj!=NULL)
	{
		D3DXQUATERNION q;
		D3DXQuaternionRotationYawPitchRoll(&q,System::deg2rad*ry,System::deg2rad*rx,System::deg2rad*rz);
		obj->SetRotationQuat(q);
		obj->UpdateWorldTransform();
	}
}

void EditPRSDlg::OnKillfocusScale()
{
	char buf[256];
	scalez.GetWindowText(buf,255);
	float sz = float(atof(buf));

	scalex.GetWindowText(buf,255);
	float sx = float(atof(buf));

	scaley.GetWindowText(buf,255);
	float sy = float(atof(buf));

	if (obj!=NULL)
	{
		sx = max(0,sx);
		sy = max(0,sy);
		sz = max(0,sz);
		obj->SetScale(D3DXVECTOR3(sx,sy,sz));
		obj->UpdateWorldTransform();
	}
}

