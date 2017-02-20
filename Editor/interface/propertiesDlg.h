#pragma once

#include "editor/resource.h"
#include "interface/sceneGraphDlg.h"
#include "interface/genericPropertyPage.h"

#include "system/signals/selectionObserver.h"

class EditPRSDlg;
class EditLightDlg;
class EditWorldObject;
class EditModelDlg;
class EditBoundingBoxDlg;
class LogDlg;
class SceneGraphDlg;
class EditModelMapDlg;
class EditShipDlg;
class EditBaseDlg;
class EditAmmoDlg;
class EditIndestructableRegionDlg;
class EditTurretDlg;
class EditWaterDlg;

/////////////////////////////////////////////////////////////////////////////

class PropertiesDlg : public CDialog, public SelectionObserver
{
public:
	PropertiesDlg(CWnd* pParent);
	virtual ~PropertiesDlg();

	// get a message from the selection system that things have changed
	virtual void SelectionChanged(WorldObject* newSelection);

	// Dialog Data
	enum { IDD = IDD_PROPERTIESDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	virtual void OnCancel();

	afx_msg void OnSize(UINT,int,int);
	afx_msg void OnTabSelchange(NMHDR*,LRESULT* pResult);

	void Resize();

protected:
	CTabCtrl		tab;
	bool			initialised;

	// focus item
	WorldObject*	selectedItem;

	// item cache
	std::vector<GenericPropertyPage*> pages;

	// dialogs
	EditPRSDlg*					prsDlg;
	EditLightDlg*				lightDlg;
	EditWorldObject*			worldObjectDlg;
	EditModelDlg*				modelDlg;
	EditBoundingBoxDlg*			aabbDlg;
	LogDlg*						logDlg;
	SceneGraphDlg*				sceneDlg;
	EditModelMapDlg*			modelMapDlg;
	EditShipDlg*				shipDlg;
	EditBaseDlg*				baseDlg;
	EditAmmoDlg*				ammoDlg;
	EditIndestructableRegionDlg*irDlg;
	EditTurretDlg*				turretDlg;
	EditWaterDlg*				waterDlg;

	DECLARE_MESSAGE_MAP()
};

