#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"
#include "system/BaseApp.h"

// fwd decls.
class XmlParser;
class EditorRuntime;
class PropertiesDlg;
class ComponentLibraryDlg;
class ObjectChangeListener;
class XmlNode;
class Camera;
class FileLog;

//////////////////////////////////////////////////////////
// GravityForceXEdit

class GravityForceXEdit : public CWinApp, public BaseApp
{
public:
	GravityForceXEdit();
	virtual ~GravityForceXEdit();

	// access singleton
	static GravityForceXEdit* Get();

	// system logic
	void EventLogic(double time);

	// system render
	void Draw(double time);

	// closedown, called by frame window
	void OnClose();

	EditorRuntime* GetEditorRuntime();

	// implement for editor
	void UpdateScore(void*);

	// get save and load filenames for the system
	virtual std::string GetFilenameForLoad(const std::string& filter,const std::string& extn,const std::string& directory);
	virtual std::string GetFilenameForSave(const std::string& filter,const std::string& extn,const std::string& directory);
	virtual bool ColourPicker(const D3DXCOLOR& in,D3DXCOLOR& out);
	virtual void ForceProcessMessages();

	// show an error message
	virtual void ShowErrorMessage(std::string msg,std::string title);

	// game only
	virtual void Quit();
	virtual void QuitGame();
	virtual Level* Load(const std::string& fname);

protected:
	virtual BOOL InitInstance();
	virtual BOOL OnIdle(LONG lCount);

	// persistent window positions
	void SaveWindowPos();
	void LoadWindowPos(Camera* camera);

	// load and save levels
	afx_msg void OnFileOpen();
	afx_msg void OnFileSave();
	afx_msg void OnFileSaveAs();
	afx_msg void OnCullCCW();

	// 10 fps timer
	double oldTime;

	// save a level - look at the file extension
	// save it as either binary or xml
	void SaveLevel(const std::string& fname);

	// helper functions
	std::string WindowSetupToString(const std::string& name,CWnd* w);
	void SetWindowPositionFromXML(XmlNode* node,CWnd* w);

	// messager handlers
	afx_msg void OnAppAbout();
	afx_msg void OnWireframe();

	// message logger
	PropertiesDlg*			propertiesDlg;
	ComponentLibraryDlg*	componentLibraryDlg;

	EditorRuntime*			editorRuntime;

	// name of last loaded file
	std::string				levelFilename;

	// log observer to external file
	FileLog*				fileLog;

	// total time
	double					totalTime;

	DECLARE_MESSAGE_MAP()
};

