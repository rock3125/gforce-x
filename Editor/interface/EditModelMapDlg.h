#pragma once

#include "system/log.h"
#include "editor/resource.h"

#include "interface/genericPropertyPage.h"
#include "interface/colourControl.h"

class ModelMap;
class PropertiesDlg;

/////////////////////////////////////////////////////////////////////////////

class EditModelMapDlg : public GenericPropertyPage
{
public:
	EditModelMapDlg(CWnd* pParent);
	virtual ~EditModelMapDlg();

	// Dialog Data
	enum { IDD = IDD_EDITMODELMAP };

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

	void OnSelectModelMap();
	void OnSelectBackgroundTexture();
	void OnSelectForegroundTexture();
	void OnUpdate();

	void OnSelectForeColour();
	void OnSelectBackColour();
	void OnSelectSideColourTop();
	void OnSelectSideColourBottom();

	void OnKillfocusGravity();

protected:
	bool			initialised;
	ModelMap*		obj;

	CButton			bSelectModelMap;
	CEdit			modelMapFilename;

	CEdit			backgroundFilename;
	CEdit			foregroundFilename;

	CEdit			gravity;

	CColourControl	sideColourTop;
	CColourControl	sideColourBottom;

	DECLARE_MESSAGE_MAP()
};

