#include "stdafx.h"
#include "GravityForceXEdit.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_WM_CLOSE()
	ON_WM_ENTERMENULOOP()
	ON_WM_EXITMENULOOP()
//	ON_WM_TIMER()
END_MESSAGE_MAP()


CMainFrame::CMainFrame(GravityForceXEdit* _parent)
	: parent(_parent)
{
}

CMainFrame::~CMainFrame()
{
}

void CMainFrame::OnSize(UINT type,int cx,int cy)
{
	CFrameWnd::OnSize(type,cx,cy);
	CRect r;
	GetClientRect(&r);

	// exceptional access to interface - don't use it normally
	Interface* intf = Interface::Get();

	float w = float(r.Width());
	float h = float(r.Height());

//	float x1=w*0.5f - intf->GetWidth()*0.5f;
//	float y1=h*0.5f - intf->GetHeight()*0.5f;
//	float x2=w*0.5f + intf->GetWidth()*0.5f;
//	float y2=h*0.5f + intf->GetHeight()*0.5f;

	float x1 = 0;
	float y1 = 0;
	float x2 = 800;
	float y2 = 600;

	parent->SetRect(D3DXVECTOR4(x1,y1,x2,y2));
};

void CMainFrame::OnClose()
{
//	m_wndView.KillTimer(1);
	GravityForceXEdit::Get()->OnClose();
	CFrameWnd::OnClose();
};

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	// create a view to occupy the client area of the frame
	if (!m_wndView.Create(NULL, NULL, AFX_WS_DEFAULT_VIEW,
		CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, NULL))
	{
		TRACE0("Failed to create view window\n");
		return -1;
	}
	m_wndView.ShowWindow(SW_SHOW);
	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.lpszClass = AfxRegisterWndClass(0);
	return TRUE;
}


void CMainFrame::OnSetFocus(CWnd* /*pOldWnd*/)
{
	m_wndView.SetFocus();
}

BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// let the view have first crack at the command
	if (m_wndView.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;

	// otherwise, do default handling
	return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void CMainFrame::OnEnterMenuLoop(BOOL b)
{
	parent->SetPaused(true);
}

void CMainFrame::OnExitMenuLoop(BOOL b)
{
	parent->SetPaused(false);
}

//void CMainFrame::OnTimer(UINT_PTR id)
//{
//	parent->EventLogic();
//}
