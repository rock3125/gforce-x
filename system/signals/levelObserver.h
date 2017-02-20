#pragma once

//////////////////////////////////////////////////////////

class Level;
class WorldObject;

class LevelObserver
{
public:
	LevelObserver()
	{
		levelObservers.push_back(this);
	};

	virtual ~LevelObserver()
	{
		std::vector<LevelObserver*>::iterator p=std::find(levelObservers.begin(),
														  levelObservers.end(),this);
		if (p!=levelObservers.end())
		{
			levelObservers.erase(p);
		};
	};

	// notify interested parties of a new level
	virtual void NewLevel(Level* level)=0;
	virtual void FocusObject(WorldObject* obj)=0;

	// tell everyone its happened
	static void NotifyObservers(Level* level);
	static void NotifyObservers(WorldObject* obj);

private:
	// anyone who wants to know about selections inherits from
	// SelectionObserver
	static std::vector<LevelObserver*> levelObservers;
};


