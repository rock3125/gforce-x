#pragma once

#include "editor/resource.h"
#include "system/signals/progressObserver.h"

/////////////////////////////////////////////////////////////////////////////
// ProgressDlg dialog

class ProgressDlg : public CDialog
				  , public ProgressObserver
{
public:
	ProgressDlg(CWnd* pParent,std::string title);   // standard constructor

	static ProgressDlg* Start(CWnd* parent,std::string title);
	void End();

	enum { IDD = IDD_PROGRESS };

	// set progress bar [0..1]
	virtual void UpdateProgress(float progress);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	virtual void OnCancel();

private:
	CProgressCtrl	progressBar;
	bool			cancelled;
	std::string		title;

	DECLARE_MESSAGE_MAP()
};
