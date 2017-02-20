#include "editor/stdafx.h"

#include "d3d9/texture.h"

#include "interface/EditAmmoDlg.h"
#include "interface/progressDlg.h"

#include "system/BaseApp.h"
#include "system/model/model.h"
#include "game/ship.h"

#include "system/converters/vrml/vrmlParser.h"

/////////////////////////////////////////////////////////////////////////////
// log dialog

EditAmmoDlg::EditAmmoDlg(CWnd* pParent)
	: GenericPropertyPage(EditAmmoDlg::IDD,pParent)
	, obj(NULL)
{
	initialised = false;
}

EditAmmoDlg::~EditAmmoDlg()
{
	obj = NULL;
}

void EditAmmoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(EditAmmoDlg)
	DDX_Control(pDX,IDC_BULLET_LOAD,bulletLoad);
	DDX_Control(pDX,IDC_ACTIVE_BULLETS,bulletNumActive);
	DDX_Control(pDX,IDC_TTL,bulletTTL);
	DDX_Control(pDX,IDC_BULLET_SPEED,bulletSpeed);
	DDX_Control(pDX,IDC_DROP_OFF_FACTOR,bulletDropOff);
	DDX_Control(pDX,IDC_MAX_MISSILES,maxMissiles);
	//}}AFX_DATA_MAP

	initialised=true;
	Resize();
	SetupData();
}

BEGIN_MESSAGE_MAP(EditAmmoDlg,CDialog)
	ON_WM_SIZE()

	ON_EN_KILLFOCUS(IDC_BULLET_LOAD, OnKillfocusBulletLoad)
	ON_EN_KILLFOCUS(IDC_ACTIVE_BULLETS, OnKillfocusBulletNumActive)
	ON_EN_KILLFOCUS(IDC_TTL, OnKillfocusBulletTTL)
	ON_EN_KILLFOCUS(IDC_BULLET_SPEED, OnKillfocusBulletSpeed)
	ON_EN_KILLFOCUS(IDC_DROP_OFF_FACTOR, OnKillfocusBulletDropOff)
	ON_EN_KILLFOCUS(IDC_MAX_MISSILES, OnKillfocusMaxMissiles)

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// EditAmmoDlg message handlers

void EditAmmoDlg::OnOK()
{
}

void EditAmmoDlg::OnCancel()
{
	ShowWindow(SW_HIDE);
}

void EditAmmoDlg::OnSize(UINT,int,int)
{
	Resize();
}

void EditAmmoDlg::Resize()
{
	if (initialised)
	{
		CRect r;
		GetClientRect(&r);
	}
}

void EditAmmoDlg::SetupData()
{
	if (obj!=NULL)
	{
		bulletLoad.SetWindowText(System::Int2Str(obj->GetMaxBulletLoad()).c_str());
		bulletNumActive.SetWindowText(System::Int2Str(obj->GetMaxActiveBullets()).c_str());
		bulletTTL.SetWindowText(System::Int2Str(obj->GetBulletTTL()).c_str());
		bulletSpeed.SetWindowText(System::Float2Str(obj->GetBulletSpeed()).c_str());
		bulletDropOff.SetWindowText(System::Float2Str(obj->GetBulletDropOffFactor()).c_str());
		maxMissiles.SetWindowText(System::Int2Str(obj->GetMaxMissiles()).c_str());
	}
}

void EditAmmoDlg::Activate()
{
	bulletLoad.SetFocus();
}

void EditAmmoDlg::SetData(WorldObject* _obj)
{
	obj = dynamic_cast<Ship*>(_obj);
	SetupData();
}

void EditAmmoDlg::OnKillfocusBulletLoad()
{
	if (obj!=NULL)
	{
		char buf[256];
		bulletLoad.GetWindowText(buf,255);
		int v = System::Str2Int(buf);
		obj->SetMaxBulletLoad(v);
	}
}

void EditAmmoDlg::OnKillfocusBulletNumActive()
{
	if (obj!=NULL)
	{
		char buf[256];
		bulletNumActive.GetWindowText(buf,255);
		int v = System::Str2Int(buf);
		obj->SetMaxActiveBullets(v);
	}
}

void EditAmmoDlg::OnKillfocusBulletTTL()
{
	if (obj!=NULL)
	{
		char buf[256];
		bulletTTL.GetWindowText(buf,255);
		int v = System::Str2Int(buf);
		obj->SetBulletTTL(v);
	}
}

void EditAmmoDlg::OnKillfocusBulletSpeed()
{
	if (obj!=NULL)
	{
		char buf[256];
		bulletSpeed.GetWindowText(buf,255);
		float f = System::Str2Float(buf);
		obj->SetBulletSpeed(f);
	}
}

void EditAmmoDlg::OnKillfocusBulletDropOff()
{
	if (obj!=NULL)
	{
		char buf[256];
		bulletDropOff.GetWindowText(buf,255);
		float f = System::Str2Float(buf);
		obj->SetBulletDropOffFactor(f);
	}
}

void EditAmmoDlg::OnKillfocusMaxMissiles()
{
	if (obj!=NULL)
	{
		char buf[256];
		maxMissiles.GetWindowText(buf,255);
		int i = System::Str2Int(buf);
		obj->SetMaxMissiles(i);
	}
}

