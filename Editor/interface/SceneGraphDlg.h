#pragma once

#include "editor/resource.h"
#include "interface/genericTree.h"
#include "interface/genericPropertyPage.h"

#include "system/signals/levelObserver.h"
#include "system/signals/objectObserver.h"
#include "system/signals/addObjectObserver.h"
#include "system/signals/selectionObserver.h"

class PRS;
class Model;

/////////////////////////////////////////////////////////////////////////////

class SceneGraphDlg : public GenericPropertyPage,
					  public TreeNotifier,
					  // signals we listen for
					  public LevelObserver,
					  public ObjectObserver,
					  public AddObjectObserver,
					  public SelectionObserver
{
public:
	SceneGraphDlg(CWnd* pParent);
	virtual ~SceneGraphDlg();

	// Dialog Data
	enum { IDD = IDD_SCENEGRAPHDLG };

	enum SceneCommands
	{
		SCENE_DELETENODE	= 100,
		SCENE_COPYNODE,
		SCENE_PASTENODE,
		SCENE_FOCUSNODE,
		SCENE_LOADNODE,
		SCENE_INFO,
		SCENE_CREATELIBRARYITEM,
		SCENE_APPLYTRANSFORMATION,
		SCENE_EXPORTVRML,
		SCENE_OPTIMISE
	};

	// receive notification message from tree
	virtual void TreeMessage(GenericTree::TreeMessage msg,HTREEITEM item,int id);

	// receive a change in one of my objects - signal
	void ObjectChanged(ObjectObserver::Operation operation,WorldObject* object);
	void NewLevel(Level* level);
	void FocusObject(WorldObject* obj);
	void SelectionChanged(WorldObject* newSelection);

	// notification message of the creation of a new object
	void NotifyCreation(WorldObject* parent,WorldObject* obj);

	// overwritten - but useless for my purposes
	void SetData(WorldObject* obj);
	void Activate();

protected:
	// get a string description of a model
	std::string GetObjectDescription(Model* model);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	virtual void OnCancel();

	afx_msg void OnSize(UINT,int,int);
	afx_msg void OnBeginDrag(NMHDR*,LRESULT*);
	afx_msg void OnMouseMove(UINT nFlags,CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags,CPoint point);
	afx_msg void OnSelectionChanged(NMHDR*,LRESULT*);

	void RebuildTree();

	// recursive helpers
	HTREEITEM RebuildTree(WorldObject* root,HTREEITEM parent);
	void CopyBranch(HTREEITEM h,HTREEITEM parent);

protected:
	friend class SelectionObserver;

	GenericTree	tree;
	bool		initialised;
	bool		dragging;

	// for dragging
	CImageList	imageList;
	CBitmap		bitmap1;
	CBitmap		bitmap2;
	CBitmap		bitmap3;
	HTREEITEM	dragTarget;
	HTREEITEM	dragItem;

	// mapping from world object back to tree item
	stdext::hash_map<WorldObject*, HTREEITEM > treeMap;

	// for copy and paste
	WorldObject*	copyObj;

	DECLARE_MESSAGE_MAP()
};

