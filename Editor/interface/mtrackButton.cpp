#include "editor/stdafx.h"

#include "interface/mtrackButton.h"

/////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(MTrackButton,CButton)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

MTrackButton::MTrackButton()
{
	leftDown=false;
	cntr=0;
};

MTrackButton::~MTrackButton()
{
};

void MTrackButton::OnLButtonDown(UINT f,CPoint _p)
{
	CButton::OnLButtonDown(f,p);

	leftDown=true;
	p=_p;
	startPos=p;
	ClientToScreen(&startPos);

	HCURSOR c = LoadCursor(NULL,IDC_SIZEWE);
	::SetCursor(c);
};

void MTrackButton::OnLButtonUp(UINT f,CPoint p)
{
	CButton::OnLButtonUp(f,p);
	leftDown=false;

	// reset cursor type and position
	HCURSOR c = LoadCursor(NULL,IDC_ARROW);
	::SetCursor(c);
	::SetCursorPos(startPos.x, startPos.y);

};

void MTrackButton::OnMouseMove(UINT f,CPoint _p)
{
	CButton::OnMouseMove(f,p);
	if (leftDown)
	{
		int dx=_p.x-p.x;
		int dy=_p.y-p.y;
		GetParent()->PostMessage(WM_USER_TRANSLATE,(WPARAM)this,(LPARAM)MAKELONG(dx,dy));
		p.x=_p.x;
		p.y=_p.y;

		// make sure mouse doesn't move too far away
		cntr++;
		if ((cntr%4)==0)
		{
			SetCursorPos(startPos.x,startPos.y);
			CPoint clientPos;
			clientPos=startPos;
			ScreenToClient(&clientPos);
			p=clientPos;
		}
	}
};

