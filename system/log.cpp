#include "standardFirst.h"

#include "system/log.h"
#include "system/signals/logObserver.h"

Log* Log::singleton = NULL;

/////////////////////////////////////////////////////////

Log& Log::GetLog()
{
	if (singleton == NULL)
		singleton = new Log();
	return *singleton;
}

Log& Log::operator << (const std::string& str)
{
	LogObserver::NotifyObservers(str);
	return *this;
}

Log& Log::operator << (const char* str)
{
	LogObserver::NotifyObservers(str);
	return *this;
}

Log& Log::operator << (int i)
{
	LogObserver::NotifyObservers(System::Int2Str(i));
	return *this;
}

Log& Log::operator << (char c)
{
	char temp[2];
	temp[0] = c;
	temp[1] = 0;
	LogObserver::NotifyObservers(temp);
	return *this;
}

Log& Log::operator << (bool b)
{
	if (b)
		LogObserver::NotifyObservers("true");
	else
		LogObserver::NotifyObservers("false");

	return *this;
}

Log& Log::operator << (float f)
{
	LogObserver::NotifyObservers(System::Float2Str(f));
	return *this;
}

Log& Log::operator << (double d)
{
	LogObserver::NotifyObservers(System::Float2Str((float)d));
	return *this;
}

