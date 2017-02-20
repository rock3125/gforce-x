#pragma once

/////////////////////////////////////////////////////////

class LogObserver
{
public:
	LogObserver()
	{
		logObservers.push_back(this);
	};

	virtual ~LogObserver()
	{
		std::vector<LogObserver*>::iterator p=std::find(logObservers.begin(),
														logObservers.end(),this);
		if (p!=logObservers.end())
		{
			logObservers.erase(p);
		};
	};

	virtual void Add(const std::string& str)=0;

	// tell everyone its happened
	static void NotifyObservers(const std::string& str);

private:
	// anyone who wants to know about selections inherits from
	// LogObserver
	static std::vector<LogObserver*> logObservers;
};

