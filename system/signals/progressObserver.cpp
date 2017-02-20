#include "standardFirst.h"

#include "system/signals/progressObserver.h"

//////////////////////////////////////////////////////////

std::vector<ProgressObserver*> ProgressObserver::progressObservers;

void ProgressObserver::NotifyObservers(float progress)
{
	std::vector<ProgressObserver*>::iterator p=progressObservers.begin();
	while (p!=progressObservers.end())
	{
		(*p)->UpdateProgress(progress);
		p++;
	};
};

