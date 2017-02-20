#pragma once

/////////////////////////////////////////////////////////////////////////////

class MTrackButton : public CButton
{
public:
	MTrackButton();
	virtual ~MTrackButton();

	enum
	{
		WM_USER_TRANSLATE		= WM_USER + 1
	};

protected:
	afx_msg void OnLButtonDown(UINT,CPoint);
	afx_msg void OnLButtonUp(UINT, CPoint);
	afx_msg void OnMouseMove(UINT,CPoint);

private:
	bool	leftDown;
	CPoint	startPos;
	int		cntr;
	CPoint	p;

	DECLARE_MESSAGE_MAP()
};

