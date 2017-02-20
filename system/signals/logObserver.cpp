#include "standardFirst.h"

#include "system/signals/logObserver.h"

/////////////////////////////////////////////////////////////////////////////

std::vector<LogObserver*> LogObserver::logObservers;

void LogObserver::NotifyObservers(const std::string& str)
{
	std::vector<LogObserver*>::iterator p=logObservers.begin();
	while (p!=logObservers.end())
	{
		(*p)->Add(str);
		p++;
	};
};

