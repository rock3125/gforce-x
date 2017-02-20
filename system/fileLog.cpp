#include "standardFirst.h"

#include "system/fileLog.h"

/////////////////////////////////////////////////////////

FileLog::FileLog()
{
	Clear();
}

FileLog::~FileLog()
{
}

void FileLog::Clear()
{
	std::string logPath = System::GetAppDirectory() + "log.txt";
	FILE* fh;
	fopen_s(&fh, logPath.c_str(),"w");
	if (fh != NULL)
	{
		fclose(fh);
	}
}

void FileLog::Add(const std::string& str)
{
	std::string logPath = System::GetAppDirectory() + "log.txt";
	FILE* fh;
	fopen_s(&fh, logPath.c_str(), "a");
	if (fh != NULL)
	{
		fwrite(str.c_str(), str.size(), 1, fh);
		fclose(fh);
	}
}

