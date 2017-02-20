#pragma once

class WorldObject;

/////////////////////////////////////////////////////////////////////////////

class GenericPropertyPage : public CDialog
{
public:
	GenericPropertyPage(UINT,CWnd*);

	virtual void SetData(WorldObject* obj)=0;
	virtual void Activate() = 0;
};

