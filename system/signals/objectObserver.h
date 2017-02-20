#pragma once

/////////////////////////////////////////////////////////////////////////////

class WorldObject;

class ObjectObserver
{
public:
	ObjectObserver()
	{
		objectObservers.push_back(this);
	};

	virtual ~ObjectObserver()
	{
		std::vector<ObjectObserver*>::iterator p=std::find(objectObservers.begin(),
														   objectObservers.end(),this);
		if (p!=objectObservers.end())
		{
			objectObservers.erase(p);
		};
	};

	// operations for changing object notification system
	enum Operation
	{
		OP_RENAMED
	};

	virtual void ObjectChanged(Operation op, WorldObject* obj) = 0;

	// tell everyone its happened
	static void NotifyObservers(Operation op, WorldObject* obj);

private:
	// anyone who wants to know about selections inherits from
	// ObjectObserver
	static std::vector<ObjectObserver*> objectObservers;
};

