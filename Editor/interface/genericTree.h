#pragma once

class TreeNotifier;

/////////////////////////////////////////////////////////////////////////////

class GenericTree : public CTreeCtrl
{
	GenericTree(const GenericTree&);
	const GenericTree& operator=(const GenericTree&);
public:
	GenericTree(TreeNotifier* parent);
	virtual ~GenericTree();

	// menu managmenet
	void ClearMenu();
	void AddMenuItem(int id,const std::string& name);
	void AddMenuSeperator();
	void EnableMenuItem(int id,bool enabled);

	// messages to send back to parent
	enum TreeMessage
	{
		TMSG_TEXTCHANGED,
		TMSG_MENUCOMMAND
	};

protected:
	afx_msg void OnBeginLabelEdit(NMHDR *pNMHDR,LRESULT *pResult);
	afx_msg void OnEndLabelEdit(NMHDR *pNMHDR,LRESULT *pResult);
	afx_msg void OnRButtonDown(UINT,CPoint p);

	virtual BOOL PreTranslateMessage(MSG* pMsg);

private:
	TreeNotifier*	parent;
	CMenu*			menu;
	bool			dragging;

	std::vector<int>	menuId;

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// notification system for messages from the tree

class TreeNotifier
{
public:
	// implement this to get tree messages
	virtual void TreeMessage(GenericTree::TreeMessage msg,HTREEITEM item,int id)=0;
};

