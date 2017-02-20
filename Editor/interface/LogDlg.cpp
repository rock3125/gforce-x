#include "editor/stdafx.h"

#include "interface/logDlg.h"
#include "interface/componentLibraryDlg.h"

/////////////////////////////////////////////////////////////////////////////
// log dialog

LogDlg::LogDlg(CWnd* pParent)
	: GenericPropertyPage(LogDlg::IDD,pParent)
{
	initialised=false;
}

LogDlg::~LogDlg()
{
}

void LogDlg::SetData(WorldObject* obj)
{
};

void LogDlg::Activate()
{
};

void LogDlg::DoDataExchange(CDataExchange* pDX)
{
	GenericPropertyPage::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(LogDlg)
	DDX_Control(pDX,IDC_LOGLIST,list);
	//}}AFX_DATA_MAP

	initialised=true;
	Resize();
}

BEGIN_MESSAGE_MAP(LogDlg,GenericPropertyPage)
	ON_WM_SIZE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// LogDlg message handlers

void LogDlg::OnOK()
{
};

void LogDlg::OnCancel()
{
	ShowWindow(SW_HIDE);
};

void LogDlg::OnSize(UINT,int,int)
{
	Resize();
};

void LogDlg::Resize()
{
	if (initialised)
	{
		CRect r;
		GetClientRect(&r);

		int border=2;
		list.SetWindowPos(0,border,border,r.Width()-(border*2),r.Height()-(border*2),SWP_NOZORDER);
	}
}

void LogDlg::Add(const std::string& str)
{
	if (!str.empty())
	{
		int size = str.size() - 1;
		if (str[size] == '\n')
		{
			list.AddString(str.substr(0,size).c_str());
		}
		else
		{
			list.AddString(str.c_str());
		}
	}
}


