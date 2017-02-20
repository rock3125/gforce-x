#pragma once

#include "system/signals/logObserver.h"

////////////////////////////////////////////////

class FileLog : public LogObserver
{
public:
	FileLog();
	~FileLog();

	void Clear();
	void Add(const std::string& str);
};

