#pragma once

#include "system/log.h"
#include "editor/resource.h"

#include "interface/genericPropertyPage.h"

class Water;
class PropertiesDlg;

/////////////////////////////////////////////////////////////////////////////

class EditWaterDlg : public GenericPropertyPage
{
public:
	EditWaterDlg(CWnd* pParent);
	virtual ~EditWaterDlg();

	// Dialog Data
	enum { IDD = IDD_EDIT_WATER };

	void SetData(WorldObject* obj);

	// activate this page
	virtual void Activate();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	virtual void OnCancel();

	void OnSize(UINT,int,int);
	void Resize();
	void SetupData();

	void OnKillfocusWidth();
	void OnKillfocusHeight();

protected:
	bool			initialised;
	Water*			obj;

	CEdit			edtWidth;
	CEdit			edtHeight;

	DECLARE_MESSAGE_MAP()
};

