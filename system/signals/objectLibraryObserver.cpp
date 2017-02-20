#include "standardFirst.h"

#include "system/signals/objectLibraryObserver.h"

/////////////////////////////////////////////////////////////////////////////

std::vector<ObjectLibraryObserver*> ObjectLibraryObserver::objectLibraryObservers;

void ObjectLibraryObserver::NotifyObservers(WorldObject* newItem)
{
	std::vector<ObjectLibraryObserver*>::iterator p=objectLibraryObservers.begin();
	while (p!=objectLibraryObservers.end())
	{
		(*p)->NewItem(newItem);
		p++;
	};
};

