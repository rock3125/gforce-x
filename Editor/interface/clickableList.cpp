#include "editor/stdafx.h"

#include "interface/clickableList.h"
#include "interface/componentLibraryDlg.h"
#include "system/model/worldObject.h"

#pragma warning(disable:4355)

/////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(ClickableList,CListCtrl)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
END_MESSAGE_MAP()

ClickableList::ClickableList(ComponentLibraryDlg* _parent)
	: parent(_parent)
{
	menu=new CMenu();
	menu->CreatePopupMenu();

	menu->AppendMenu(MF_STRING,COMPONENT_DELETE,"Delete");
	menu->AppendMenu(MF_STRING,COMPONENT_SAVE,"Save...");
	menu->AppendMenu(MF_STRING,COMPONENT_LOAD,"Load...");
};

ClickableList::~ClickableList()
{
	safe_delete(menu);
};

int ClickableList::GetSelectedItem()
{
	int objCount=GetItemCount();
	for (int i=0; i<objCount; i++)
	{
		if (GetItemState(i,LVIS_SELECTED)&LVIS_SELECTED)
			return i;
	}
	return -1;
}

void ClickableList::OnRButtonDown(UINT,CPoint p)
{
	ClientToScreen(&p);
	menu->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON,p.x,p.y,this);
}

void ClickableList::OnLButtonDblClk(UINT f,CPoint p)
{
	CListCtrl::OnLButtonDblClk(f,p);

	if (parent!=NULL)
	{
		int index=GetSelectedItem();
		if (index>=0)
		{
			WorldObject::WorldType t=WorldObject::WorldType(GetItemData(index));
			parent->DoubleClickListItem(t);
		}
	}
};

BOOL ClickableList::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message==WM_COMMAND && HIWORD(pMsg->wParam)==0)
	{
		int menu=LOWORD(pMsg->wParam);
		parent->Menu(ComponentCommands(menu));
	};

	// not processed - translate
	return 0;
};

