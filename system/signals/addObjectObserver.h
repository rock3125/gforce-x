#pragma once

#include "system/model/worldObject.h"

//////////////////////////////////////////////////////////
// this class channels creation between systems

class AddObjectObserver
{
public:
	AddObjectObserver()
	{
		addObjectObserver.push_back(this);
	};

	virtual ~AddObjectObserver()
	{
		std::vector<AddObjectObserver*>::iterator p=std::find(addObjectObserver.begin(),
															 addObjectObserver.end(),this);
		if (p!=addObjectObserver.end())
		{
			addObjectObserver.erase(p);
		}
	};

	virtual void NotifyCreation(WorldObject* parent,WorldObject* obj)=0;

	// tell everyone its happened
	static void NotifyObservers(WorldObject* parent,WorldObject* obj);

private:
	// anyone who wants to know about selections inherits from
	// SelectionObserver
	static std::vector<AddObjectObserver*> addObjectObserver;
};

