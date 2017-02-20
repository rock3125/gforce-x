#pragma once

#include "editor/resource.h"
#include "system/model/worldObject.h"
#include "system/signals/objectLibraryObserver.h"
#include "interface/genericPropertyPage.h"
#include "interface/clickableList.h"

/////////////////////////////////////////////////////////////////////////////

class ComponentLibraryDlg : public CDialog, public ObjectLibraryObserver
{
public:
	ComponentLibraryDlg(CWnd* pParent);
	virtual ~ComponentLibraryDlg();

	// Dialog Data
	enum { IDD = IDD_COMPONENTLIBRARYDLG };

	// double clicking the list fires this item
	void DoubleClickListItem(WorldObject::WorldType type);

	// menu item selected on list
	void Menu(ClickableList::ComponentCommands cmd);

	// overwritten - but useless for my purposes
	void SetData(WorldObject* obj);
	virtual void Activate();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	virtual void OnCancel();

	afx_msg void OnSize(UINT,int,int);
	void Resize();

	void RebuildList();

	// the library receives a new item
	void NewItem(WorldObject* newItem);

protected:
	ClickableList	list;
	CBitmap			bitmap;
	CImageList		imageList;
	bool			initialised;

	DECLARE_MESSAGE_MAP()
};

