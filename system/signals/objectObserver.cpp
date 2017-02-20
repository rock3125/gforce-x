#include "standardFirst.h"

#include "system/signals/objectObserver.h"

/////////////////////////////////////////////////////////////////////////////

std::vector<ObjectObserver*> ObjectObserver::objectObservers;

void ObjectObserver::NotifyObservers(ObjectObserver::Operation op, WorldObject* obj)
{
	std::vector<ObjectObserver*>::iterator p = objectObservers.begin();
	while (p!=objectObservers.end())
	{
		(*p)->ObjectChanged(op,obj);
		p++;
	};
};

