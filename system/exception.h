#pragma once

//////////////////////////////////////////////////////////

class Exception
{
public:
	Exception(std::string _msg)
	{
		msg = _msg;
	};

	std::string Message() const
	{
		return msg;
	}

private:
	std::string msg;
};

