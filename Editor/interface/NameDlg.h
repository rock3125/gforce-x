#pragma once

#include "editor/resource.h"

/////////////////////////////////////////////////////////////////////////////
// NameDlg dialog

class NameDlg : public CDialog
{
public:
	NameDlg(CWnd* pParent,std::string _title,std::string _text);   // standard constructor

	enum { IDD = IDD_NAMEDLG };

	CButton	m_ok;
	CEdit	m_name;

	void SetName(std::string _name);
	std::string GetName() const;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	std::string name;
	std::string title;

	virtual void OnOK();
	afx_msg void OnChangeNameedit();

	DECLARE_MESSAGE_MAP()
};
