#pragma once

///////////////////////////////////////////////
// CChildView window

class CChildView : public CWnd
{
public:
	CChildView();
	virtual ~CChildView();

	// access childview
	static CChildView* GetChildView();

protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	afx_msg void OnLButtonDown(UINT flags,CPoint point);
	afx_msg void OnLButtonUp(UINT flags,CPoint point);
	afx_msg void OnRButtonDown(UINT flags,CPoint point);
	afx_msg void OnRButtonUp(UINT flags,CPoint point);
	afx_msg void OnMouseMove(UINT flags,CPoint point);
	afx_msg void OnKeyDown(UINT kchar,UINT,UINT);
	afx_msg void OnKeyUp(UINT kchar,UINT,UINT);

	void UpdateSystemStates();

protected:
	// childview singleton
	static CChildView* singleton;

	CPoint mouse;

	bool ldown;
	bool rdown;
	bool mdown;
	bool altDown;
	bool shiftDown;
	bool ctrlDown;

	DECLARE_MESSAGE_MAP()
};

