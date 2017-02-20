#pragma once

//////////////////////////////////////////////////////////
// this class channels creation between systems

class ProgressObserver
{
public:
	ProgressObserver()
	{
		progressObservers.push_back(this);
	};

	virtual ~ProgressObserver()
	{
		std::vector<ProgressObserver*>::iterator p=std::find(progressObservers.begin(),
															 progressObservers.end(),this);
		if (p!=progressObservers.end())
		{
			progressObservers.erase(p);
		}
	};

	// progress = [0..1]
	virtual void UpdateProgress(float progress)=0;

	// tell everyone its happened
	static void NotifyObservers(float progress);

private:
	// anyone who wants to know about selections inherits from
	// SelectionObserver
	static std::vector<ProgressObserver*> progressObservers;
};

