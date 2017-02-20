#include "editor/stdafx.h"

#include "interface/editIndestructableRegionDlg.h"
#include "interface/progressDlg.h"

#include "game/indestructableRegion.h"

/////////////////////////////////////////////////////////////////////////////
// log dialog

EditIndestructableRegionDlg::EditIndestructableRegionDlg(CWnd* pParent)
	: GenericPropertyPage(EditIndestructableRegionDlg::IDD,pParent)
	, obj(NULL)
{
	initialised = false;
}

EditIndestructableRegionDlg::~EditIndestructableRegionDlg()
{
	obj = NULL;
}

void EditIndestructableRegionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(EditIndestructableRegionDlg)
	DDX_Control(pDX,IDC_WIDTH,edtWidth);
	DDX_Control(pDX,IDC_HEIGHT,edtHeight);
	//}}AFX_DATA_MAP

	initialised=true;
	Resize();
	SetupData();
}

BEGIN_MESSAGE_MAP(EditIndestructableRegionDlg,CDialog)
	ON_WM_SIZE()

	ON_EN_KILLFOCUS(IDC_WIDTH, OnKillfocusWidth)
	ON_EN_KILLFOCUS(IDC_HEIGHT, OnKillfocusHeight)

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// EditIndestructableRegionDlg message handlers

void EditIndestructableRegionDlg::OnOK()
{
}

void EditIndestructableRegionDlg::OnCancel()
{
	ShowWindow(SW_HIDE);
}

void EditIndestructableRegionDlg::OnSize(UINT,int,int)
{
	Resize();
}

void EditIndestructableRegionDlg::Resize()
{
	if (initialised)
	{
		CRect r;
		GetClientRect(&r);
	}
}

void EditIndestructableRegionDlg::SetupData()
{
	if (obj!=NULL)
	{
		edtWidth.SetWindowText(System::Float2Str(obj->GetWidth()).c_str());
		edtHeight.SetWindowText(System::Float2Str(obj->GetHeight()).c_str());
	}
}

void EditIndestructableRegionDlg::Activate()
{
	edtWidth.SetFocus();
}

void EditIndestructableRegionDlg::SetData(WorldObject* _obj)
{
	obj = dynamic_cast<IndestructableRegion*>(_obj);
	SetupData();
}

void EditIndestructableRegionDlg::OnKillfocusWidth()
{
	if (obj!=NULL)
	{
		char buf[256];
		edtWidth.GetWindowText(buf,255);
		float v = System::Str2Float(buf);
		obj->SetWidth(v);
	}
}

void EditIndestructableRegionDlg::OnKillfocusHeight()
{
	if (obj!=NULL)
	{
		char buf[256];
		edtHeight.GetWindowText(buf,255);
		float v = System::Str2Float(buf);
		obj->SetHeight(v);
	}
}

