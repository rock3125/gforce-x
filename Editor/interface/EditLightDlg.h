#pragma once

#include "editor/resource.h"
#include "interface/genericPropertyPage.h"

class WorldObject;
class Light;

/////////////////////////////////////////////////////////////////////////////

class EditLightDlg : public GenericPropertyPage
{
public:
	EditLightDlg(CWnd* pParent);
	virtual ~EditLightDlg();

	// Dialog Data
	enum { IDD = IDD_EDIT_LIGHT };

	void SetData(WorldObject* obj);

	// activate this page
	virtual void Activate();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	virtual void OnCancel();

	afx_msg void OnSize(UINT,int,int);
	afx_msg void OnChangeType();
	afx_msg void OnKillfocusRange();
	afx_msg void OnKillfocusPhi();
	afx_msg void OnKillfocusAttenuation1();
	afx_msg void OnKillfocusAttenuation0();
	afx_msg void OnAmbient();
	afx_msg void OnDiffuse();
	afx_msg void OnSpecular();

	void Resize();

	// helper function - converts data in edit to a float
	float GetFloatFromEdit(CEdit& edit);
	std::string GetStringFromWindow(CWnd& window);

	// helper function for converting 3d colour to gdi colour
	COLORREF D3DToColorRef(const D3DXCOLOR& col);
	D3DXCOLOR ColorRefToD3D(COLORREF c,float alpha=1.0f);

	// setup data in controls when appropriate
	void SetupData();

	// and put it back into the light when the time comes
	void SetupLightFromDlg();

protected:
	bool			initialised;

	// controls
	CComboBox		comboType;

	// colour boxes
	CListCtrl		specularColour;
	CListCtrl		diffuseColour;
	CListCtrl		ambientColour;

	CEdit			attenuation0;
	CEdit			attenuation1;
	CEdit			editRange;

	// light object
	Light*			obj;

	DECLARE_MESSAGE_MAP()
};

