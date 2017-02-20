#include "editor/stdafx.h"

#include "interface/progressDlg.h"
#include "system/signals/cancelObserver.h"
#include "system/BaseApp.h"

/////////////////////////////////////////////////////////////////////////////
// ProgressDlg dialog


ProgressDlg::ProgressDlg(CWnd* pParent,std::string _title)
	: CDialog(ProgressDlg::IDD, pParent)
{
	cancelled = false;
	title = _title;
}

void ProgressDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(ProgressDlg)
	DDX_Control(pDX,IDC_PROGRESSBAR,progressBar);
	//}}AFX_DATA_MAP
	progressBar.SetRange(0,100);
	SetWindowText(title.c_str());
}

BEGIN_MESSAGE_MAP(ProgressDlg, CDialog)
	//{{AFX_MSG_MAP(ProgressDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ProgressDlg message handlers

void ProgressDlg::OnCancel()
{
	CDialog::OnCancel();
	CancelObserver::NotifyObservers();
	cancelled = true;
}

void ProgressDlg::UpdateProgress(float perc)
{
	if (!cancelled)
	{
		progressBar.SetPos((int)(perc*100.0f));
		UpdateWindow();
	}
	BaseApp::Get()->ForceProcessMessages();
}

ProgressDlg* ProgressDlg::Start(CWnd* parent,std::string title)
{
	ProgressDlg* p = new ProgressDlg(parent,title);
	p->Create(ProgressDlg::IDD,parent);
	p->ShowWindow(SW_SHOW);
	return p;
}

void ProgressDlg::End()
{
	if (!cancelled)
	{
		CDialog::OnCancel();
	}
}
