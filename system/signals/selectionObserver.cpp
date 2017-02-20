#include "standardFirst.h"

#include "system/signals/selectionObserver.h"

/////////////////////////////////////////////////////////////////////////////

std::vector<SelectionObserver*> SelectionObserver::selectionObservers;

void SelectionObserver::NotifyObservers(WorldObject* newSelection)
{
	std::vector<SelectionObserver*>::iterator p=selectionObservers.begin();
	while (p!=selectionObservers.end())
	{
		(*p)->SelectionChanged(newSelection);
		p++;
	};
};

