#pragma once

/////////////////////////////////////////////////////////

class Log
{
public:
	// access singleton
	static Log& GetLog();

	// output options
	Log& operator << (const std::string&);
	Log& operator << (const char*);
	Log& operator << (int);
	Log& operator << (char);
	Log& operator << (bool);
	Log& operator << (float);
	Log& operator << (double);

private:
	static Log* singleton;
};

