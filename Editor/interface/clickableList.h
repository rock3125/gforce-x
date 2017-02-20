#pragma once

/////////////////////////////////////////////////////////////////////////////

class ComponentLibraryDlg;

class ClickableList : public CListCtrl
{
public:
	ClickableList(ComponentLibraryDlg* parent);
	~ClickableList();

	enum ComponentCommands
	{
		COMPONENT_DELETE	= 100,
		COMPONENT_SAVE,
		COMPONENT_LOAD
	};

	// get selected item
	int GetSelectedItem();

protected:
	afx_msg void OnLButtonDblClk(UINT,CPoint);
	afx_msg void OnRButtonDown(UINT,CPoint);

	virtual BOOL PreTranslateMessage(MSG* pMsg);

private:
	ComponentLibraryDlg* parent;
	CMenu* menu;

	DECLARE_MESSAGE_MAP()
};

