#include "editor/stdafx.h"

#include "interface/genericTree.h"

/////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(GenericTree,CTreeCtrl)
	ON_NOTIFY_REFLECT(TVN_BEGINLABELEDIT,OnBeginLabelEdit)
	ON_NOTIFY_REFLECT(TVN_ENDLABELEDIT,OnEndLabelEdit)
	ON_WM_RBUTTONDOWN()
END_MESSAGE_MAP()

GenericTree::GenericTree(TreeNotifier* _parent)
	: parent(_parent)
	, menu(NULL)
{
	dragging=false;
	menu=new CMenu();
	menu->CreatePopupMenu();
};

GenericTree::~GenericTree()
{
	safe_delete(menu);
};


void GenericTree::OnRButtonDown(UINT,CPoint p)
{
	ClientToScreen(&p);
	menu->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON,p.x,p.y,this);
};

void GenericTree::ClearMenu()
{
	safe_delete(menu);
	menu=new CMenu();
	menu->CreatePopupMenu();
	menuId.clear();
};

void GenericTree::AddMenuItem(int id,const std::string& name)
{
	menu->AppendMenu(MF_STRING,id,(LPCTSTR)name.c_str());
	menuId.push_back(id);
}

void GenericTree::AddMenuSeperator()
{
	menu->AppendMenu(MF_SEPARATOR);
}

void GenericTree::EnableMenuItem(int id,bool enabled)
{
	menu->EnableMenuItem(id,enabled?MF_ENABLED:MF_GRAYED);
};

BOOL GenericTree::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message==WM_COMMAND && HIWORD(pMsg->wParam)==0)
	{
		int menu=LOWORD(pMsg->wParam);
		std::vector<int>::iterator pos=find(menuId.begin(),menuId.end(),menu);
		if (pos!=menuId.end() && parent!=NULL)
		{
			HTREEITEM item=GetSelectedItem();
			parent->TreeMessage(TMSG_MENUCOMMAND,item,menu);
		};
	};

	// not processed - translate
	return 0;
};

void GenericTree::OnBeginLabelEdit(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult=0;
}

void GenericTree::OnEndLabelEdit(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTVDISPINFO pTVDispInfo = reinterpret_cast<LPNMTVDISPINFO>(pNMHDR);
	TVITEM* tvi = &pTVDispInfo->item;
	if (tvi->pszText!=NULL)
	{
		HTREEITEM item=tvi->hItem;
		SetItemText(item,tvi->pszText);
		if (parent!=NULL)
			parent->TreeMessage(TMSG_TEXTCHANGED,item,0);
	}
	*pResult=0;
}

