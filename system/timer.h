#pragma once

/////////////////////////////////////////////////////////////////////////////

class Time
{
	Time();
	~Time();
public:
	// get singleton instance
	static Time* Get();
	static void Destroy();

	void Initialise();
	void Reset();

	// Call this onces per game frame
	void Update();

	// Access funcs
	double CurrentTime() const;
	double FrameTime() const;

private:

	typedef double (Time::* TimeUpdateFunc)();
	TimeUpdateFunc pfnUpdate;

	double	fBaseTime;
	double	fLastTime;
	double	fSecsPerTick;

	double	fCurrentTime;
	double	fFrameTime;

	// QueryPerformancesCounter related 64bit integers.
	// These are Microsoft specific, and will have to be changed for
	// different compilers.
	_int64	dTimerStart;
	_int64  dCurTime;

	// One of these gets bound to the TimeUpdateFunc pointer
	double	GetPerformanceCounterTime();
	double	GetMMTime();

	// singelton
	static Time* singleton;
};

