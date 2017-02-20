#include "standardFirst.h"

#include "system/signals/addObjectObserver.h"

//////////////////////////////////////////////////////////

std::vector<AddObjectObserver*> AddObjectObserver::addObjectObserver;

void AddObjectObserver::NotifyObservers(WorldObject* parent,WorldObject* obj)
{
	std::vector<AddObjectObserver*>::iterator p=addObjectObserver.begin();
	while (p!=addObjectObserver.end())
	{
		(*p)->NotifyCreation(parent,obj);
		p++;
	};
};

