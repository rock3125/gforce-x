#pragma once

///////////////////////////////////////////////////////////////////////

class StringBuilder
{
public:
	StringBuilder();
	StringBuilder(int);
	~StringBuilder();

	// append to stringbuilder
	void Append(const std::string& str);
	void Append(int);
	void Append(float);
	void Append(char);

	// convert current internals to string
	std::string ToString();

	// slightly unorthodox way of accessing the object
	const char* GetString();

	// clear current stringbuilder obj
	void Clear();

	// return the length of the string
	int Length();

private:
	// do the actual work
	void Add(const std::string& str);

private:
	char*	buffer;

	int		initialSize;
	int		bufferSize;
	int		currentSize;
};

