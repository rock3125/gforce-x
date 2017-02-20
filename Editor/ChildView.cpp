#include "stdafx.h"

#include "GravityForceXEdit.h"
#include "ChildView.h"
#include "runtime/editorRuntime.h"
#include "system/keyboard.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CChildView* CChildView::singleton=NULL;

/////////////////////////////////////////////////////
// CChildView

BEGIN_MESSAGE_MAP(CChildView, CWnd)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
END_MESSAGE_MAP()

CChildView::CChildView()
{
	if (singleton==NULL)
		singleton=this;

	ldown=false;
	rdown=false;
	mdown=false;
	altDown=false;
	shiftDown=false;
	ctrlDown=false;
}

CChildView::~CChildView()
{
	singleton=NULL;
}

CChildView* CChildView::GetChildView()
{
	return singleton;
}

BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs)
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	HBRUSH backgroundColour=(HBRUSH)::GetStockObject(GRAY_BRUSH);

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS,
					::LoadCursor(NULL, IDC_ARROW),backgroundColour, NULL);

	return TRUE;
}

void CChildView::UpdateSystemStates()
{
	D3DXVECTOR2 p;
	p.x=float(mouse.x);
	p.y=float(mouse.y);

	System::SetAction(System::A_SELECT1,ldown);
	System::SetAction(System::A_SELECT2,rdown);
	System::SetAction(System::A_SELECT3,mdown);
	System::SetAction(System::A_ABSMOUSE,p);
	System::SetAction(System::A_MODIFIER1,shiftDown);
	System::SetAction(System::A_MODIFIER2,ctrlDown);
	System::SetAction(System::A_MODIFIER3,altDown);
}

void CChildView::OnLButtonDown(UINT flags,CPoint point)
{
	GetCapture();
	ldown=true;
	mouse=point;

	altDown=(GetAsyncKeyState(VK_MENU)&0x8000)>0;
	shiftDown=(flags&MK_SHIFT);
	ctrlDown=(flags&MK_CONTROL);

	UpdateSystemStates();
}

void CChildView::OnLButtonUp(UINT flags,CPoint point)
{
	ReleaseCapture();
	ldown=false;
	mouse=point;

	altDown=(GetAsyncKeyState(VK_MENU)&0x8000)>0;
	shiftDown=(flags&MK_SHIFT);
	ctrlDown=(flags&MK_CONTROL);

	UpdateSystemStates();
}

void CChildView::OnRButtonDown(UINT flags,CPoint point)
{
	GetCapture();
	rdown=true;
	mouse=point;

	altDown=(GetAsyncKeyState(VK_MENU)&0x8000)>0;
	shiftDown=(flags&MK_SHIFT);
	ctrlDown=(flags&MK_CONTROL);

	UpdateSystemStates();
}

void CChildView::OnRButtonUp(UINT flags,CPoint point)
{
	ReleaseCapture();
	rdown=false;
	mouse=point;

	altDown=(GetAsyncKeyState(VK_MENU)&0x8000)>0;
	shiftDown=(flags&MK_SHIFT);
	ctrlDown=(flags&MK_CONTROL);

	UpdateSystemStates();
}

void CChildView::OnMouseMove(UINT flags,CPoint point)
{
	bool altDown=(GetAsyncKeyState(VK_MENU)&0x8000)>0;
	bool shiftDown=(flags&MK_SHIFT);
	bool ctrlDown=(flags&MK_CONTROL);

	CPoint delta=(point-mouse);
	mouse=point;

	D3DXVECTOR2 d;
	d.x=float(delta.x);
	d.y=float(delta.y);

	System::SetAction(System::A_DELTAMOUSE,d.x,d.y);
	System::SetAction(System::A_ABSMOUSE,(float)mouse.x,(float)mouse.y);

	altDown=(GetAsyncKeyState(VK_MENU)&0x8000)>0;
	shiftDown=(flags&MK_SHIFT);
	ctrlDown=(flags&MK_CONTROL);

	UpdateSystemStates();
}

void CChildView::OnKeyDown(UINT kchar,UINT,UINT flags)
{
	switch (kchar)
	{
		case 'W':
		{
			System::SetAction(System::A_WIREFRAME,true);
			break;
		}
		case 'C':
		{
			System::SetAction(System::A_CULLCCW,true);
			break;
		}
	}

	bool shift=(flags&MK_SHIFT);
	bool ctrl=(flags&MK_CONTROL);

	Keyboard::Get()->KeyDown(kchar);
	Keyboard::Get()->SetCtrlDown(ctrl);
	Keyboard::Get()->SetShiftDown(shift);
}

void CChildView::OnKeyUp(UINT kchar,UINT,UINT flags)
{
	bool shift=(flags&MK_SHIFT);
	bool ctrl=(flags&MK_CONTROL);

	Keyboard::Get()->KeyUp(kchar);
	Keyboard::Get()->SetCtrlDown(ctrl);
	Keyboard::Get()->SetShiftDown(shift);
}
