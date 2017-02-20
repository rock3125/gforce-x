#pragma once

#include "system/log.h"
#include "editor/resource.h"

#include "interface/genericPropertyPage.h"
#include "interface/colourControl.h"

class Ship;
class PropertiesDlg;

/////////////////////////////////////////////////////////////////////////////

class EditShipDlg : public GenericPropertyPage
{
public:
	EditShipDlg(CWnd* pParent);
	virtual ~EditShipDlg();

	// Dialog Data
	enum { IDD = IDD_EDIT_SHIP };

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

	void OnSelectMesh();
	void OnSelectTexture();
	void OnSelectColour();
	void OnKillfocusTransparency();

	void OnKillfocusMaxSpeed();
	void OnKillfocusMaxAccel();
	void OnKillfocusAccel();
	void OnKillfocusFuelUsage();
	void OnKillfocusRotationalVelocity();
	void OnKillfocusBaseOffset();

	void OnSelectBulletMesh();
	void OnSelectBulletTexture();
	void OnSelectBulletColour();

protected:
	bool			initialised;
	Ship*			obj;

	CButton			bSelectFile;
	CEdit			meshFilename;
	CEdit			textureFilename;
	CColourControl	colour;

	CEdit			transparency;

	CEdit			maxSpeed;
	CEdit			maxAccel;
	CEdit			accel;
	CEdit			fuelUsage;
	CEdit			rotationalVelocity;
	CEdit			baseOffset;

	CEdit			bulletMeshFilename;
	CEdit			bulletTextureFilename;
	CColourControl	bulletColour;

	DECLARE_MESSAGE_MAP()
};

