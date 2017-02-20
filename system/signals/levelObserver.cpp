#include "standardFirst.h"

#include "system/signals/levelObserver.h"

//////////////////////////////////////////////////////////

std::vector<LevelObserver*> LevelObserver::levelObservers;

void LevelObserver::NotifyObservers(Level* level)
{
	std::vector<LevelObserver*>::iterator p=levelObservers.begin();
	while (p!=levelObservers.end())
	{
		(*p)->NewLevel(level);
		p++;
	};
};

void LevelObserver::NotifyObservers(WorldObject* obj)
{
	std::vector<LevelObserver*>::iterator p=levelObservers.begin();
	while (p!=levelObservers.end())
	{
		(*p)->FocusObject(obj);
		p++;
	};
};

