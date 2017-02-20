#pragma once

#include "editor/resource.h"
#include "interface/mtrackButton.h"
#include "interface/genericPropertyPage.h"

class WorldObject;

/////////////////////////////////////////////////////////////////////////////

class EditBoundingBoxDlg : public GenericPropertyPage
{
public:
	EditBoundingBoxDlg(CWnd* pParent);
	virtual ~EditBoundingBoxDlg();

	// Dialog Data
	enum { IDD = IDD_EDIT_BOUNDINGBOX };

	void SetData(WorldObject* obj);

	// activate this page
	virtual void Activate();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	virtual void OnCancel();

	// receive message from child to translate
	LRESULT OnTranslate(WPARAM button,LPARAM delta);

	afx_msg void OnKillFocusXSizeMin();
	afx_msg void OnKillFocusYSizeMin();
	afx_msg void OnKillFocusZSizeMin();
	afx_msg void OnKillFocusXSizeMax();
	afx_msg void OnKillFocusYSizeMax();
	afx_msg void OnKillFocusZSizeMax();

	afx_msg void OnSize(UINT,int,int);
	void Resize();

	// setup data for new object in dialogs
	void SetupData();

protected:
	bool			initialised;
	WorldObject*	obj;

	CEdit			xsizeMin;
	CEdit			ysizeMin;
	CEdit			zsizeMin;
	CEdit			xsizeMax;
	CEdit			ysizeMax;
	CEdit			zsizeMax;

	MTrackButton	sizexbuttonMin;
	MTrackButton	sizeybuttonMin;
	MTrackButton	sizezbuttonMin;
	MTrackButton	sizexbuttonMax;
	MTrackButton	sizeybuttonMax;
	MTrackButton	sizezbuttonMax;

	DECLARE_MESSAGE_MAP()
};

