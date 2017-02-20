#include "standardFirst.h"

#include "system/signals/creationObserver.h"

//////////////////////////////////////////////////////////

std::vector<CreationObserver*> CreationObserver::creationObservers;

void CreationObserver::NotifyObservers(WorldObject::WorldType create)
{
	std::vector<CreationObserver*>::iterator p=creationObservers.begin();
	while (p!=creationObservers.end())
	{
		(*p)->AcceptCreation(create);
		p++;
	};
};

void CreationObserver::NotifyObservers(WorldObject* obj)
{
	std::vector<CreationObserver*>::iterator p=creationObservers.begin();
	while (p!=creationObservers.end())
	{
		(*p)->AcceptCreation(obj);
		p++;
	};
};

