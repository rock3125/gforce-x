#include "editor/stdafx.h"

#include "interface/nameDlg.h"

/////////////////////////////////////////////////////////////////////////////
// NameDlg dialog


NameDlg::NameDlg(CWnd* pParent,std::string _title,std::string _name)
	: CDialog(NameDlg::IDD, pParent)
	, title(_title)
	, name(_name)
{
}

std::string NameDlg::GetName() const
{
	return name;
};

void NameDlg::SetName(std::string _name)
{
	name=_name;
};

void NameDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(NameDlg)
	DDX_Control(pDX,IDOK,m_ok);
	DDX_Control(pDX,IDC_NAMEEDIT,m_name);
	//}}AFX_DATA_MAP

	SetWindowText(title.c_str());
	if (!name.empty())
	{
		m_name.SetWindowText(name.c_str());
	}
	m_ok.EnableWindow(!name.empty());
}

BEGIN_MESSAGE_MAP(NameDlg, CDialog)
	//{{AFX_MSG_MAP(NameDlg)
	ON_EN_CHANGE(IDC_NAMEEDIT, OnChangeNameedit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// NameDlg message handlers

void NameDlg::OnOK()
{
	CDialog::OnOK();
}

void NameDlg::OnChangeNameedit()
{
	static char buf[256];
	m_name.GetWindowText(buf,255);
	name=buf;
	m_ok.EnableWindow(name.length()>0);
}

