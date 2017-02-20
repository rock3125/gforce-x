#include "editor/stdafx.h"

#include "interface/componentLibraryDlg.h"
#include "interface/progressDlg.h"

#include "d3d9/interface.h"

#include "system/BaseApp.h"
#include "system/objectFactory.h"

#include "system/signals/creationObserver.h"

#pragma warning(disable:4355)

/////////////////////////////////////////////////////////////////////////////

ComponentLibraryDlg::ComponentLibraryDlg(CWnd* pParent)
	: CDialog(ComponentLibraryDlg::IDD,pParent)
	, list(this)
{
	initialised=false;
}

ComponentLibraryDlg::~ComponentLibraryDlg()
{
}

void ComponentLibraryDlg::SetData(WorldObject* obj)
{
};

void ComponentLibraryDlg::Activate()
{
};

void ComponentLibraryDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(ComponentLibraryDlg)
	DDX_Control(pDX, IDC_COMPONENTLIBRARY_LIST, list);
	//}}AFX_DATA_MAP

	// done
	initialised=true;

	bitmap.LoadBitmap(IDB_WHEEL);
	imageList.Create(16,16,ILC_COLOR8,1,0);
	imageList.Add(&bitmap,RGB(0,0,0));
	list.SetImageList(&imageList,LVSIL_SMALL);

	Resize();
	RebuildList();
}

BEGIN_MESSAGE_MAP(ComponentLibraryDlg,CDialog)
	ON_WM_SIZE()
END_MESSAGE_MAP()

void ComponentLibraryDlg::OnOK()
{
};

void ComponentLibraryDlg::OnCancel()
{
	ShowWindow(SW_HIDE);
};

void ComponentLibraryDlg::OnSize(UINT,int,int)
{
	if (initialised)
	{
		Resize();
	}
};

void ComponentLibraryDlg::Resize()
{
	CRect r;
	GetClientRect(&r);
	int border=2;
	list.SetWindowPos(0,border,border,r.Width()-(border*2),r.Height()-(border*2),SWP_NOZORDER);
};

void ComponentLibraryDlg::DoubleClickListItem(WorldObject::WorldType type)
{
	ProgressDlg* dlg = ProgressDlg::Start(this,"Parsing " + ObjectFactory::GetWorldTypeName(type));
	CreationObserver::NotifyObservers(type);
	dlg->End();
};

void ComponentLibraryDlg::RebuildList()
{
	if (initialised)
	{
		list.DeleteAllItems();

		std::vector<WorldObject::WorldType> objList=ObjectFactory::GetCreatableObjectList();
		for (int i=0; i<objList.size(); i++)
		{
			std::string str=ObjectFactory::GetWorldTypeName(objList[i]);
			int index=list.InsertItem(i,str.c_str(),0);
			list.SetItemData(index,DWORD_PTR(objList[i]));
		}
	};
};

void ComponentLibraryDlg::NewItem(WorldObject* newItem)
{
	RebuildList();
};

void ComponentLibraryDlg::Menu(ClickableList::ComponentCommands cmd)
{
	switch (cmd)
	{
		case ClickableList::COMPONENT_DELETE:
		{
			// only custom items can be deleted
			int index = list.GetSelectedItem();
			if (index>0)
			{
				WorldObject::WorldType t=WorldObject::WorldType(list.GetItemData(index));
				if (t > WorldObject::TYPE_LAST)
				{
					ObjectFactory::RemoveCustomObject(t);
				}
			}
			break;
		}
		case ClickableList::COMPONENT_LOAD:
		{
			break;
		}
		case ClickableList::COMPONENT_SAVE:
		{
			break;
		}
	}
};

