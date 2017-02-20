#pragma once

//////////////////////////////////////////////////////////
// this class channels creation between systems

class CancelObserver
{
public:
	CancelObserver()
	{
		cancelObservers.push_back(this);
	};

	virtual ~CancelObserver()
	{
		std::vector<CancelObserver*>::iterator p=std::find(cancelObservers.begin(),
															 cancelObservers.end(),this);
		if (p!=cancelObservers.end())
		{
			cancelObservers.erase(p);
		}
	};

	// progress = [0..1]
	virtual void Cancel()=0;

	// tell everyone its happened
	static void NotifyObservers();

private:
	// anyone who wants to know about selections inherits from
	// SelectionObserver
	static std::vector<CancelObserver*> cancelObservers;
};

