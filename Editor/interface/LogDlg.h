#pragma once

#include "system/signals/logObserver.h"
#include "editor/resource.h"
#include "interface/genericPropertyPage.h"

/////////////////////////////////////////////////////////////////////////////

class LogDlg : public GenericPropertyPage, public LogObserver
{
public:
	LogDlg(CWnd* pParent);
	virtual ~LogDlg();

	// Dialog Data
	enum { IDD = IDD_LOGDLG };

	// receive a string from the log system
	virtual void Add(const std::string&);

	// overwritten - but useless for my purposes
	void SetData(WorldObject* obj);
	void Activate();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	virtual void OnCancel();

	afx_msg void OnSize(UINT,int,int);
	void Resize();

protected:
	CListBox		list;
	bool			initialised;

	DECLARE_MESSAGE_MAP()
};

