#pragma once

#include "editor/resource.h"
#include "interface/genericPropertyPage.h"
#include "system/signals/objectObserver.h"

class WorldObject;

/////////////////////////////////////////////////////////////////////////////

class EditWorldObject : public GenericPropertyPage,
						public ObjectObserver
{
public:
	EditWorldObject(CWnd* pParent);
	virtual ~EditWorldObject();

	// Dialog Data
	enum { IDD = IDD_EDIT_WORLDOBJECT };

	// set data from propertiesDlg
	void SetData(WorldObject* data);

	// activate this page
	virtual void Activate();

	// announce a change to an object to anyone interested
	void ObjectChanged(ObjectObserver::Operation operation,WorldObject* object);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	virtual void OnCancel();

	afx_msg void OnSize(UINT,int,int);
	void Resize();

	// set all data in the fields
	void SetupData();
	// read data from dialog
	void GetData();

	afx_msg void OnKillFocusName();
	afx_msg void OnClickButton();
	afx_msg void OnUpdateClicked();

protected:
	bool			initialised;
	WorldObject*	obj;

	// name edit field
	CStatic			status;
	CEdit			name;
	CEdit			code;

	CButton			bVisible;
	CButton			bScriptable;

	CButton			bUpdate;

	CFont*			font;

	DECLARE_MESSAGE_MAP()
};

