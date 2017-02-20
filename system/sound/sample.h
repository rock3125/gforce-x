#pragma once

///////////////////////////////////////////////////////////////////////////////////////////////

class Sample
{
	Sample(const Sample&);
	const Sample& operator=(const Sample&);
public:
	Sample(const std::string& filename)
		: buffer(NULL)
	{
		this->filename = filename;
	}

	virtual ~Sample(void)
	{
		safe_release(buffer);
	}


	// sample filename
	std::string filename;

	// sample format
	WAVEFORMATEX format;

	// sample size in bytes
	DWORD size;

	// sample sound buffer
	LPDIRECTSOUNDBUFFER buffer;
};

