#pragma once

/////////////////////////////////////////////////////////////////////////////

class WorldObject;

class ObjectLibraryObserver
{
public:
	ObjectLibraryObserver()
	{
		objectLibraryObservers.push_back(this);
	};

	virtual ~ObjectLibraryObserver()
	{
		std::vector<ObjectLibraryObserver*>::iterator p=std::find(objectLibraryObservers.begin(),
																  objectLibraryObservers.end(),this);
		if (p!=objectLibraryObservers.end())
		{
			objectLibraryObservers.erase(p);
		};
	};

	virtual void NewItem(WorldObject* newItem)=0;

	// tell everyone its happened
	static void NotifyObservers(WorldObject* newItem);

private:
	// anyone who wants to know about selections inherits from
	// ObjectLibraryObserver
	static std::vector<ObjectLibraryObserver*> objectLibraryObservers;
};

