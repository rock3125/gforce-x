#pragma once

#include "system/model/worldObject.h"

class CreationAccepter;

//////////////////////////////////////////////////////////
// this class channels creation between systems

class CreationObserver
{
public:
	CreationObserver()
	{
		creationObservers.push_back(this);
	};

	virtual ~CreationObserver()
	{
		std::vector<CreationObserver*>::iterator p=std::find(creationObservers.begin(),
															 creationObservers.end(),this);
		if (p!=creationObservers.end())
		{
			creationObservers.erase(p);
		}
	};

	virtual void AcceptCreation(WorldObject::WorldType create)=0;
	virtual void AcceptCreation(WorldObject* obj)=0;

	// tell everyone its happened
	static void NotifyObservers(WorldObject::WorldType create);
	static void NotifyObservers(WorldObject* obj);

private:
	// anyone who wants to know about selections inherits from
	// SelectionObserver
	static std::vector<CreationObserver*> creationObservers;
};

