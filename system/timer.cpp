#include "standardFirst.h"

#include <mmsystem.h>

#include "system/timer.h"

Time* Time::singleton = NULL;

/////////////////////////////////////////////////////////////////////////////

Time& Time::GetTimer()
{
	if (!singleton)
		singleton=new Time();
	return *singleton;
}

void Time::Destroy()
{
	safe_delete(singleton);
}

Time::Time()
	: pfnUpdate(0),
	  fBaseTime(0.0f),
	  fLastTime(0.0f),
	  fSecsPerTick(0.0f),
	  fCurrentTime(0.0f),
	  fFrameTime(0.0f),
	  dTimerStart(0),
	  dCurTime(0)
{
}

Time::~Time()
{
	pfnUpdate = 0;
}

double Time::CurrentTime() const
{
	return fCurrentTime;
}

double Time::FrameTime() const
{
	return fFrameTime;
}

void Time::Initialise()
{
	_int64	dTicksPerSec =0;

	if (QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER *>(&dTicksPerSec)))
	{
		// performance counter is available, use it instead of multimedia timer
		QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER *>(&dTimerStart));
		fSecsPerTick = (1.0)/(static_cast<float>(dTicksPerSec));
		pfnUpdate = &Time::GetPerformanceCounterTime;
	}
	else
	{
		// Use MM timer if unable to use the High Frequency timer
		dTimerStart = timeGetTime();
		fSecsPerTick = 1.0/1000.0;
		pfnUpdate = &Time::GetMMTime;
	}
}

void Time::Update()
{
	if (pfnUpdate!=NULL)
	{
		// The ->* operator binds the function pointer to the object pointed to by
		// he right hand pointer. which is THIS below
		fCurrentTime =  (this->*pfnUpdate)() - fBaseTime;
		fFrameTime = fCurrentTime - fLastTime;
		fLastTime = fCurrentTime;
	}
}

void Time::Reset()
{
	if (pfnUpdate!=NULL)
	{
		fBaseTime =  (this->*pfnUpdate)();
		fLastTime = fCurrentTime = fFrameTime = 0.0;
	}
}

double Time::GetPerformanceCounterTime()
{
	QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER *>(&dCurTime));
	return (fSecsPerTick * (dCurTime - dTimerStart));
}

double Time::GetMMTime()
{
	return (fSecsPerTick * (timeGetTime()- dTimerStart));
}

