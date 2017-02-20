#include "standardFirst.h"

#include "system/StringBuilder.h"

//////////////////////////////////////////////////////////

StringBuilder::StringBuilder()
	: buffer(NULL)
{
	initialSize = 8192;
	bufferSize = initialSize;
	currentSize = 0;
	buffer = new char[bufferSize];
}

StringBuilder::StringBuilder(int _initialSize)
	: buffer(NULL)
{
	initialSize = _initialSize;
	bufferSize = initialSize;
	currentSize = 0;
	buffer = new char[bufferSize];
}

StringBuilder::~StringBuilder()
{
	safe_delete_array(buffer);
	bufferSize = 0;
	currentSize = 0;
}

void StringBuilder::Clear()
{
	safe_delete_array(buffer);
	initialSize = 8192;
	bufferSize = initialSize;
	currentSize = 0;
	buffer = new char[bufferSize];
}

int StringBuilder::Length()
{
	return currentSize;
}

const char* StringBuilder::GetString()
{
	return buffer;
}

void StringBuilder::Append(const std::string& str)
{
	Add(str);
}

void StringBuilder::Append(int i)
{
	Add(System::Int2Str(i));
}

void StringBuilder::Append(float f)
{
	Add(System::Float2Str(f));
}

void StringBuilder::Append(char ch)
{
	char buf[2];
	buf[0] = ch;
	buf[1] = 0;
	Add(buf);
}

void StringBuilder::Add(const std::string& str)
{
	if ((currentSize + str.size() + 1) > bufferSize)
	{
		int increase = bufferSize * 2;
		if ((str.size() + 1) > bufferSize)
			increase = (currentSize + str.size() + 100);

		char* newBuffer = new char[increase];
		memcpy(newBuffer,buffer,bufferSize);
		bufferSize = increase;
		safe_delete_array(buffer);
		buffer = newBuffer;
	}
	memcpy(&buffer[currentSize], str.c_str(), str.size()+1);
	currentSize += str.size();
}

std::string StringBuilder::ToString()
{
	buffer[currentSize] = 0;
	return buffer;
}

