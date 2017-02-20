#pragma once

/////////////////////////////////////////////////////////////////////////////

class WorldObject;

class SelectionObserver
{
public:
	SelectionObserver()
	{
		selectionObservers.push_back(this);
	};

	virtual ~SelectionObserver()
	{
		std::vector<SelectionObserver*>::iterator p=std::find(selectionObservers.begin(),
															  selectionObservers.end(),this);
		if (p!=selectionObservers.end())
		{
			selectionObservers.erase(p);
		};
	};

	virtual void SelectionChanged(WorldObject* newSelection)=0;

	// tell everyone its happened
	static void NotifyObservers(WorldObject* newItem);

private:
	// anyone who wants to know about selections inherits from
	// SelectionObserver
	static std::vector<SelectionObserver*> selectionObservers;
};

