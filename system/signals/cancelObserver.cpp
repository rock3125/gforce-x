#include "standardFirst.h"

#include "system/signals/cancelObserver.h"

//////////////////////////////////////////////////////////

std::vector<CancelObserver*> CancelObserver::cancelObservers;

void CancelObserver::NotifyObservers()
{
	std::vector<CancelObserver*>::iterator p=cancelObservers.begin();
	while (p!=cancelObservers.end())
	{
		(*p)->Cancel();
		p++;
	};
};

