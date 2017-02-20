#pragma once

#include "system/log.h"
#include "editor/resource.h"

#include "interface/genericPropertyPage.h"
#include "interface/colourControl.h"

class Ship;
class PropertiesDlg;

/////////////////////////////////////////////////////////////////////////////

class EditAmmoDlg : public GenericPropertyPage
{
public:
	EditAmmoDlg(CWnd* pParent);
	virtual ~EditAmmoDlg();

	// Dialog Data
	enum { IDD = IDD_EDIT_AMMO };

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

	void OnKillfocusBulletLoad();
	void OnKillfocusBulletNumActive();
	void OnKillfocusBulletTTL();
	void OnKillfocusBulletSpeed();
	void OnKillfocusBulletDropOff();
	void OnKillfocusMaxMissiles();

protected:
	bool			initialised;
	Ship*			obj;

	CEdit			bulletLoad;
	CEdit			bulletNumActive;
	CEdit			bulletTTL;
	CEdit			bulletSpeed;
	CEdit			bulletDropOff;
	CEdit			maxMissiles;

	DECLARE_MESSAGE_MAP()
};

