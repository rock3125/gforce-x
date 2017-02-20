#include "editor/stdafx.h"
#include "standardFirst.h"

#include "editor/resource.h"
#include "interface/SelectStringDlg.h"

#pragma warning(disable:4355)

/////////////////////////////////////////////////////////////////////////////

DoubleClickNotifyListBox::DoubleClickNotifyListBox(SelectStringDlg* _parent)
	: parent(_parent)
{
};

void DoubleClickNotifyListBox::OnLButtonDblClk(UINT f,CPoint p)
{
	parent->OnLButtonDblClk(f,p);
};

BEGIN_MESSAGE_MAP(DoubleClickNotifyListBox,CListBox)
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SelectStringDlg dialog

IMPLEMENT_DYNAMIC(SelectStringDlg, CDialog)
SelectStringDlg::SelectStringDlg(CWnd* pParent,const std::string& _title,const std::vector<std::string>& _list,
								 const std::string& _name)
	: CDialog(SelectStringDlg::IDD, pParent)
	, listCtrl(this)
{
	list=_list;
	name=_name;
	title=_title;
	id=-1;
}

SelectStringDlg::~SelectStringDlg()
{
}

int SelectStringDlg::GetId() const
{
	return id;
};

void SelectStringDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SelectStringDlg)
	DDX_Control(pDX, IDC_STRINGLIST, listCtrl);
	//}}AFX_DATA_MAP

	SetWindowText(title.c_str());

	listCtrl.ResetContent();
	int index=0;
	int selectedIndex=-1;
	for (int i=0; i<list.size(); i++)
	{
		listCtrl.AddString(list[i].c_str());
		if (list[i]==name)
			selectedIndex=i;
	}
	if (selectedIndex>=0)
		listCtrl.SetCurSel(selectedIndex);
}

BEGIN_MESSAGE_MAP(SelectStringDlg, CDialog)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SelectStringDlg message handlers

void SelectStringDlg::OnLButtonDblClk(UINT,CPoint)
{
	int sel=listCtrl.GetCurSel();
	if (sel>=0)
	{
		id=sel;
		CDialog::OnOK();
	}
};

void SelectStringDlg::OnOK()
{
	int sel=listCtrl.GetCurSel();
	if (sel>=0)
	{
		id=sel;
		CDialog::OnOK();
	}
};
