#include "editor/stdafx.h"

#include "interface/editLightDlg.h"
#include "system/model/light.h"

/////////////////////////////////////////////////////////////////////////////
// edit light dialog

BEGIN_MESSAGE_MAP(EditLightDlg,CDialog)
	ON_WM_SIZE()

	ON_CBN_SELENDOK(IDC_LIGHT_TYPE,OnChangeType)

	ON_EN_KILLFOCUS(IDC_LIGHT_RANGE,OnKillfocusRange)
	ON_EN_KILLFOCUS(IDC_LIGHT_ATTENUATION1,OnKillfocusAttenuation1)
	ON_EN_KILLFOCUS(IDC_LIGHT_ATTENUATION0,OnKillfocusAttenuation0)

	ON_BN_CLICKED(IDC_LIGHT_SPECULAR,OnSpecular)
	ON_BN_CLICKED(IDC_LIGHT_DIFFUSE,OnDiffuse)
	ON_BN_CLICKED(IDC_LIGHT_AMBIENT,OnAmbient)

END_MESSAGE_MAP()

EditLightDlg::EditLightDlg(CWnd* pParent)
	: GenericPropertyPage(EditLightDlg::IDD,pParent)
	, obj(NULL)
{
	initialised=false;
}

EditLightDlg::~EditLightDlg()
{
	obj=NULL;
}

void EditLightDlg::Activate()
{
	comboType.SetFocus();
};

void EditLightDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(EditLightDlg)
	DDX_Control(pDX,IDC_LIGHT_TYPE,comboType);
	DDX_Control(pDX,IDC_LIGHT_SPECULARCOLOUR,specularColour);
	DDX_Control(pDX,IDC_LIGHT_DIFFUSECOLOUR,diffuseColour);
	DDX_Control(pDX,IDC_LIGHT_AMBIENTCOLOUR,ambientColour);
	DDX_Control(pDX,IDC_LIGHT_ATTENUATION0,attenuation0);
	DDX_Control(pDX,IDC_LIGHT_ATTENUATION1,attenuation1);
	DDX_Control(pDX,IDC_LIGHT_RANGE,editRange);
	//}}AFX_DATA_MAP

	// add list drop down contents
	CRect r;
	comboType.ResetContent();
	comboType.AddString("Point Light-source");
	comboType.AddString("Spot Light");
	comboType.AddString("Directional Light");
	comboType.GetWindowRect(&r);
	comboType.SetWindowPos(NULL,0,0,r.Width(),132,SWP_NOMOVE|SWP_NOZORDER);

	SetupData();

	initialised = true;
	Resize();
}

void EditLightDlg::SetData(WorldObject* _obj)
{
	obj = dynamic_cast<Light*>(_obj);
	SetupData();
};

/////////////////////////////////////////////////////////////////////////////
// EditLightDlg message handlers

void EditLightDlg::SetupData()
{
	if (obj!=NULL)
	{
		// light type
		switch (obj->GetLightType())
		{
			case D3DLIGHT_POINT:
			{
				comboType.SetCurSel(0);
				break;
			};
			case D3DLIGHT_SPOT:
			{
				comboType.SetCurSel(1);
				break;
			};
			case D3DLIGHT_DIRECTIONAL:
			{
				comboType.SetCurSel(2);
				break;
			};
		}

		// colour boxes
		ambientColour.SetBkColor(D3DToColorRef(obj->GetAmbient()));
		diffuseColour.SetBkColor(D3DToColorRef(obj->GetDiffuse()));
		specularColour.SetBkColor(D3DToColorRef(obj->GetSpecular()));

		attenuation0.SetWindowText(System::Float2Str(obj->GetAttenuation0()).c_str());
		attenuation1.SetWindowText(System::Float2Str(obj->GetAttenuation1()).c_str());
		editRange.SetWindowText(System::Float2Str(obj->GetRange()).c_str());
	}
};

void EditLightDlg::SetupLightFromDlg()
{
	if (obj!=NULL)
	{
		// light type
		switch (comboType.GetCurSel())
		{
			case 0:
			{
				obj->SetLightType(D3DLIGHT_POINT);
				break;
			};
			case 1:
			{
				obj->SetLightType(D3DLIGHT_SPOT);
				break;
			};
			case 2:
			{
				obj->SetLightType(D3DLIGHT_DIRECTIONAL);
				break;
			};
		}

		// ambient
		obj->SetAmbient(ColorRefToD3D(ambientColour.GetBkColor()));

		// diffuse
		obj->SetDiffuse(ColorRefToD3D(diffuseColour.GetBkColor()));

		// specular
		obj->SetSpecular(ColorRefToD3D(specularColour.GetBkColor()));
	}
};

COLORREF EditLightDlg::D3DToColorRef(const D3DXCOLOR& col)
{
	return RGB(int(col.r*255.0f),int(col.g*255.0f),int(col.b*255.0f));
}

D3DXCOLOR EditLightDlg::ColorRefToD3D(COLORREF c,float alpha)
{
	D3DXCOLOR col;
	col.r=float(GetRValue(c))/255.0f;
	col.g=float(GetGValue(c))/255.0f;
	col.b=float(GetBValue(c))/255.0f;
	col.a=alpha;
	return col;
}

float EditLightDlg::GetFloatFromEdit(CEdit& edit)
{
	char buf[256];
	edit.GetWindowText(buf, 255);
	return float(atof(buf));
}

std::string EditLightDlg::GetStringFromWindow(CWnd& window)
{
	char buf[256];
	window.GetWindowText(buf, 255);
	return buf;
}

void EditLightDlg::OnOK()
{
};

void EditLightDlg::OnCancel()
{
};

void EditLightDlg::OnSize(UINT,int,int)
{
	Resize();
};

void EditLightDlg::Resize()
{
	if (initialised)
	{
		CRect r;
		GetClientRect(&r);
	};
};

void EditLightDlg::OnChangeType()
{
	SetupLightFromDlg();
};

void EditLightDlg::OnKillfocusRange()
{
	SetupLightFromDlg();
}

void EditLightDlg::OnKillfocusPhi()
{
	SetupLightFromDlg();
}

void EditLightDlg::OnKillfocusAttenuation1()
{
	SetupLightFromDlg();
}

void EditLightDlg::OnKillfocusAttenuation0()
{
	SetupLightFromDlg();
}

void EditLightDlg::OnAmbient()
{
	if (obj!=NULL)
	{
		CColorDialog dlg(ambientColour.GetBkColor(),CC_FULLOPEN);
		if (dlg.DoModal()==IDOK)
		{
			ambientColour.SetBkColor(dlg.GetColor());
			ambientColour.InvalidateRect(NULL);
		}
		SetupLightFromDlg();
	}
}

void EditLightDlg::OnDiffuse()
{
	if (obj!=NULL)
	{
		CColorDialog dlg(diffuseColour.GetBkColor(),CC_FULLOPEN);
		if (dlg.DoModal()==IDOK)
		{
			diffuseColour.SetBkColor(dlg.GetColor());
			diffuseColour.InvalidateRect(NULL);
		}
		SetupLightFromDlg();
	}
}

void EditLightDlg::OnSpecular()
{
	if (obj!=NULL)
	{
		CColorDialog dlg(specularColour.GetBkColor(),CC_FULLOPEN);
		if (dlg.DoModal()==IDOK)
		{
			specularColour.SetBkColor(dlg.GetColor());
			specularColour.InvalidateRect(NULL);
		}
		SetupLightFromDlg();
	}
}

