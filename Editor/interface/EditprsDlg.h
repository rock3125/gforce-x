#pragma once

#include "editor/resource.h"
#include "interface/mtrackButton.h"
#include "interface/genericPropertyPage.h"

class WorldObject;

/////////////////////////////////////////////////////////////////////////////

class EditPRSDlg : public GenericPropertyPage
{
public:
	EditPRSDlg(CWnd* pParent);
	virtual ~EditPRSDlg();

	// Dialog Data
	enum { IDD = IDD_EDIT_PRS };

	void SetData(WorldObject* obj);

	// activate this page
	virtual void Activate();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	virtual void OnCancel();

	// receive message from child to translate
	LRESULT OnTranslate(WPARAM button,LPARAM delta);

	afx_msg void OnKillfocusXpos();
	afx_msg void OnKillfocusYpos();
	afx_msg void OnKillfocusZpos();

	afx_msg void OnKillfocusRotation();
	afx_msg void OnKillfocusScale();

	afx_msg void OnSize(UINT,int,int);
	void Resize();

	// setup data for new object in dialogs
	void SetupData();

protected:
	bool			initialised;
	WorldObject*	obj;

	CEdit			rotz;
	CEdit			roty;
	CEdit			rotx;
	MTrackButton	zslider;
	MTrackButton	yslider;
	MTrackButton	xslider;

	CEdit			xpos;
	CEdit			ypos;
	CEdit			zpos;
	MTrackButton	movez;
	MTrackButton	movey;
	MTrackButton	movex;

	CEdit			scalez;
	CEdit			scaley;
	CEdit			scalex;
	MTrackButton	scalezbut;
	MTrackButton	scaleybut;
	MTrackButton	scalexbut;

	DECLARE_MESSAGE_MAP()
};

