#pragma once

#include "ChildView.h"

class GravityForceXEdit;

////////////////////////////////////////////////////

class CMainFrame : public CFrameWnd
{

public:
	CMainFrame(GravityForceXEdit* parent);
	virtual ~CMainFrame();

protected:
	DECLARE_DYNAMIC(CMainFrame)

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd *pOldWnd);
	afx_msg void OnEnterMenuLoop(BOOL b);
	afx_msg void OnExitMenuLoop(BOOL b);

	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual void OnClose();
	virtual void OnSize(UINT,int,int);
//	virtual void OnTimer(UINT_PTR id);

protected:
	CChildView	m_wndView;

private:
	GravityForceXEdit*	parent;

	DECLARE_MESSAGE_MAP()
};


