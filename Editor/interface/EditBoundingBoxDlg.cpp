#include "editor/stdafx.h"

#include "interface/editBoundingBoxDlg.h"
#include "system/model/worldObject.h"

/////////////////////////////////////////////////////////////////////////////
// log dialog

EditBoundingBoxDlg::EditBoundingBoxDlg(CWnd* pParent)
	: GenericPropertyPage(EditBoundingBoxDlg::IDD,pParent)
	, obj(NULL)
{
	initialised=false;
}

EditBoundingBoxDlg::~EditBoundingBoxDlg()
{
	obj=NULL;
}

void EditBoundingBoxDlg::SetData(WorldObject* _obj)
{
	obj=_obj;
	SetupData();
};

void EditBoundingBoxDlg::Activate()
{
	xsizeMin.SetFocus();
};

void EditBoundingBoxDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(EditBoundingBoxDlg)
	DDX_Control(pDX,IDC_BB_XSIZE,xsizeMin);
	DDX_Control(pDX,IDC_BB_YSIZE,ysizeMin);
	DDX_Control(pDX,IDC_BB_ZSIZE,zsizeMin);
	DDX_Control(pDX,IDC_BB_XSIZEMAX,xsizeMax);
	DDX_Control(pDX,IDC_BB_YSIZEMAX,ysizeMax);
	DDX_Control(pDX,IDC_BB_ZSIZEMAX,zsizeMax);
	DDX_Control(pDX,IDC_BB_XSIZEBUT,sizexbuttonMin);
	DDX_Control(pDX,IDC_BB_YSIZEBUT,sizeybuttonMin);
	DDX_Control(pDX,IDC_BB_ZSIZEBUT,sizezbuttonMin);
	DDX_Control(pDX,IDC_BB_XSIZEBUTMAX,sizexbuttonMax);
	DDX_Control(pDX,IDC_BB_YSIZEBUTMAX,sizeybuttonMax);
	DDX_Control(pDX,IDC_BB_ZSIZEBUTMAX,sizezbuttonMax);
	//}}AFX_DATA_MAP

	SetupData();

	initialised=true;
	Resize();
}

BEGIN_MESSAGE_MAP(EditBoundingBoxDlg,CDialog)
	ON_WM_SIZE()
	ON_EN_KILLFOCUS(IDC_BB_XSIZE,OnKillFocusXSizeMin)
	ON_EN_KILLFOCUS(IDC_BB_YSIZE,OnKillFocusYSizeMin)
	ON_EN_KILLFOCUS(IDC_BB_ZSIZE,OnKillFocusZSizeMin)
	ON_EN_KILLFOCUS(IDC_BB_XSIZEMAX,OnKillFocusXSizeMax)
	ON_EN_KILLFOCUS(IDC_BB_YSIZEMAX,OnKillFocusYSizeMax)
	ON_EN_KILLFOCUS(IDC_BB_ZSIZEMAX,OnKillFocusZSizeMax)
	ON_MESSAGE(MTrackButton::WM_USER_TRANSLATE,OnTranslate)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// EditBoundingBoxDlg message handlers

void EditBoundingBoxDlg::SetupData()
{
	xsizeMin.EnableWindow(obj!=NULL);
	ysizeMin.EnableWindow(obj!=NULL);
	zsizeMin.EnableWindow(obj!=NULL);
	xsizeMax.EnableWindow(obj!=NULL);
	ysizeMax.EnableWindow(obj!=NULL);
	zsizeMax.EnableWindow(obj!=NULL);

	sizexbuttonMin.EnableWindow(obj!=NULL);
	sizeybuttonMin.EnableWindow(obj!=NULL);
	sizezbuttonMin.EnableWindow(obj!=NULL);
	sizexbuttonMax.EnableWindow(obj!=NULL);
	sizeybuttonMax.EnableWindow(obj!=NULL);
	sizezbuttonMax.EnableWindow(obj!=NULL);

	if (obj!=NULL)
	{
		// get local position
		BoundingBox* bb=obj->GetBoundingBox();

		D3DXVECTOR3 aabmin = bb->GetMin();
		D3DXVECTOR3 aabmax = bb->GetMax();

		xsizeMin.SetWindowText(System::Float2Str(aabmin.x).c_str());
		ysizeMin.SetWindowText(System::Float2Str(aabmin.y).c_str());
		zsizeMin.SetWindowText(System::Float2Str(aabmin.z).c_str());

		xsizeMax.SetWindowText(System::Float2Str(aabmax.x).c_str());
		ysizeMax.SetWindowText(System::Float2Str(aabmax.y).c_str());
		zsizeMax.SetWindowText(System::Float2Str(aabmax.z).c_str());
	}
};

void EditBoundingBoxDlg::OnOK()
{
};

void EditBoundingBoxDlg::OnCancel()
{
};

void EditBoundingBoxDlg::OnSize(UINT,int,int)
{
	Resize();
};

void EditBoundingBoxDlg::Resize()
{
	if (initialised)
	{
		CRect r;
		GetClientRect(&r);
	};
};

LRESULT EditBoundingBoxDlg::OnTranslate(WPARAM button,LPARAM delta)
{
	if (obj!=NULL)
	{
		CButton* but=(CButton*)button;
		short x=LOWORD(delta);
		short y=HIWORD(delta);

		const float sensitivity = 0.25f;

		float dxmin=0;
		float dymin=0;
		float dzmin=0;
		float dxmax=0;
		float dymax=0;
		float dzmax=0;

		if (but==&sizexbuttonMin)
		{
			dxmin=float(x)*sensitivity;
		}
		else if (but==&sizeybuttonMin)
		{
			dymin=float(x)*sensitivity;
		}
		else if (but==&sizezbuttonMin)
		{
			dzmin=float(x)*sensitivity;
		}
		else if (but==&sizexbuttonMax)
		{
			dxmax=float(x)*sensitivity;
		}
		else if (but==&sizeybuttonMax)
		{
			dymax=float(x)*sensitivity;
		}
		else if (but==&sizezbuttonMax)
		{
			dzmax=float(x)*sensitivity;
		};

		// update data
		BoundingBox* bb=obj->GetBoundingBox();
		D3DXVECTOR3 aabmin = bb->GetMin();
		D3DXVECTOR3 aabmax = bb->GetMax();

		aabmin+=D3DXVECTOR3(dxmin,dymin,dzmin);
		aabmax+=D3DXVECTOR3(dxmax,dymax,dzmax);

		bb->SetMin(aabmin);
		bb->SetMax(aabmax);

		obj->UpdateBoundingBox();

		// update form
		SetupData();
	}
	return 0;
};

void EditBoundingBoxDlg::OnKillFocusXSizeMin()
{
	if (obj!=NULL)
	{
		char buf[256];
		xsizeMin.GetWindowText(buf,255);
		BoundingBox* bb=obj->GetBoundingBox();

		D3DXVECTOR3 aabmin = bb->GetMin();
		aabmin.x=float(atof(buf));
		bb->SetMin(aabmin);
	}
}

void EditBoundingBoxDlg::OnKillFocusYSizeMin()
{
	if (obj!=NULL)
	{
		char buf[256];
		ysizeMin.GetWindowText(buf,255);
		BoundingBox* bb=obj->GetBoundingBox();

		D3DXVECTOR3 aabmin = bb->GetMin();
		aabmin.y=float(atof(buf));
		bb->SetMin(aabmin);
	}
}

void EditBoundingBoxDlg::OnKillFocusZSizeMin()
{
	if (obj!=NULL)
	{
		char buf[256];
		zsizeMin.GetWindowText(buf,255);
		BoundingBox* bb=obj->GetBoundingBox();

		D3DXVECTOR3 aabmin = bb->GetMin();
		aabmin.z=float(atof(buf));
		bb->SetMin(aabmin);
	}
}

void EditBoundingBoxDlg::OnKillFocusXSizeMax()
{
	if (obj!=NULL)
	{
		char buf[256];
		xsizeMax.GetWindowText(buf,255);
		BoundingBox* bb=obj->GetBoundingBox();

		D3DXVECTOR3 aabmax = bb->GetMax();
		aabmax.x=float(atof(buf));
		bb->SetMax(aabmax);
	}
}

void EditBoundingBoxDlg::OnKillFocusYSizeMax()
{
	if (obj!=NULL)
	{
		char buf[256];
		ysizeMax.GetWindowText(buf,255);
		BoundingBox* bb=obj->GetBoundingBox();

		D3DXVECTOR3 aabmax = bb->GetMax();
		aabmax.y=float(atof(buf));
		bb->SetMax(aabmax);
	}
}

void EditBoundingBoxDlg::OnKillFocusZSizeMax()
{
	if (obj!=NULL)
	{
		char buf[256];
		zsizeMax.GetWindowText(buf,255);
		BoundingBox* bb=obj->GetBoundingBox();

		D3DXVECTOR3 aabmax = bb->GetMax();
		aabmax.z=float(atof(buf));
		bb->SetMax(aabmax);
	}
}

