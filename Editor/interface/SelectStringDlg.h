#pragma once

/////////////////////////////////////////////////////////////////////////////

class SelectStringDlg;

class DoubleClickNotifyListBox : public CListBox
{
public:
	DoubleClickNotifyListBox(SelectStringDlg* parent);
protected:
	virtual void OnLButtonDblClk(UINT,CPoint p);
private:
	SelectStringDlg* parent;
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

class SelectStringDlg : public CDialog
{
	DECLARE_DYNAMIC(SelectStringDlg)

public:
	SelectStringDlg(CWnd* pParent,const std::string& title,const std::vector<std::string>& list,const std::string& name);   // standard constructor
	virtual ~SelectStringDlg();

	// access name
	int GetId() const;

	// Dialog Data
	enum { IDD = IDD_SELECT_STRING };

	virtual void OnLButtonDblClk(UINT,CPoint p);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();

protected:
	DoubleClickNotifyListBox	listCtrl;
	int							id;
	std::string					title;
	std::string					name;
	std::vector<std::string>	list;

	DECLARE_MESSAGE_MAP()
};

