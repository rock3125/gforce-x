#pragma once

#include "system/log.h"
#include "editor/resource.h"

#include "interface/genericPropertyPage.h"

class IndestructableRegion;
class PropertiesDlg;

/////////////////////////////////////////////////////////////////////////////

class EditIndestructableRegionDlg : public GenericPropertyPage
{
public:
	EditIndestructableRegionDlg(CWnd* pParent);
	virtual ~EditIndestructableRegionDlg();

	// Dialog Data
	enum { IDD = IDD_EDIT_INDESTRUCTABLEREGION };

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
	bool						initialised;
	IndestructableRegion*		obj;

	CEdit			edtWidth;
	CEdit			edtHeight;

	DECLARE_MESSAGE_MAP()
};

