#include "editor/stdafx.h"

#include "interface/editWaterDlg.h"
#include "interface/progressDlg.h"

#include "game/water.h"

/////////////////////////////////////////////////////////////////////////////
// log dialog

EditWaterDlg::EditWaterDlg(CWnd* pParent)
	: GenericPropertyPage(EditWaterDlg::IDD, pParent)
	, obj(NULL)
{
	initialised = false;
}

EditWaterDlg::~EditWaterDlg()
{
	obj = NULL;
}

void EditWaterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(EditWaterDlg)
	DDX_Control(pDX,IDC_WIDTH,edtWidth);
	DDX_Control(pDX,IDC_HEIGHT,edtHeight);
	//}}AFX_DATA_MAP

	initialised=true;
	Resize();
	SetupData();
}

BEGIN_MESSAGE_MAP(EditWaterDlg,CDialog)
	ON_WM_SIZE()

	ON_EN_KILLFOCUS(IDC_WIDTH, OnKillfocusWidth)
	ON_EN_KILLFOCUS(IDC_HEIGHT, OnKillfocusHeight)

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// EditWaterDlg message handlers

void EditWaterDlg::OnOK()
{
}

void EditWaterDlg::OnCancel()
{
	ShowWindow(SW_HIDE);
}

void EditWaterDlg::OnSize(UINT,int,int)
{
	Resize();
}

void EditWaterDlg::Resize()
{
	if (initialised)
	{
		CRect r;
		GetClientRect(&r);
	}
}

void EditWaterDlg::SetupData()
{
	if (obj!=NULL)
	{
		edtWidth.SetWindowText(System::Float2Str(obj->GetWidth()).c_str());
		edtHeight.SetWindowText(System::Float2Str(obj->GetHeight()).c_str());
	}
}

void EditWaterDlg::Activate()
{
	edtWidth.SetFocus();
}

void EditWaterDlg::SetData(WorldObject* _obj)
{
	obj = dynamic_cast<Water*>(_obj);
	SetupData();
}

void EditWaterDlg::OnKillfocusWidth()
{
	if (obj!=NULL)
	{
		char buf[256];
		edtWidth.GetWindowText(buf,255);
		float v = System::Str2Float(buf);
		obj->SetWidth(v);
	}
}

void EditWaterDlg::OnKillfocusHeight()
{
	if (obj!=NULL)
	{
		char buf[256];
		edtHeight.GetWindowText(buf,255);
		float v = System::Str2Float(buf);
		obj->SetHeight(v);
	}
}

