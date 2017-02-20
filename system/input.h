#pragma once

//////////////////////////////////////////////////////////

class Input
{
	Input();
	~Input();
	const Input& operator=(const Input&);
	Input(const Input&);
public:

	// type of control
	enum Type
	{
		KEYBOARD,
		GAMEPAD
	};

	enum SubType
	{
		KEY,
		SHIFT_KEY,
		CTRL_KEY,
		GAMEPAD_BUTTON,
		GAMEPAD_AXIS
	};

	enum SpecialKey
	{
		SK_ESCAPE = 27,
		SK_UP = 38,
		SK_DOWN = 40,
		SK_LEFT = 37,
		SK_RIGHT = 39,
		SK_ENTER = 13
	};

	enum
	{
		CTRL_NONE,
		CTRL_KEYBOARD,
		CTRL_GAMEPAD1,
		CTRL_GAMEPAD2,
		CTRL_GAMEPAD3,
		CTRL_GAMEPAD4,
	};

	// convert a string to a special key enum
	static int StringToKey(std::string str);

	// this is how we check for a specific event in the control
	class Event
	{
	public:
		Event()
		{
		}
		Event(Type _type, SubType _subType, int _gamePadId, int _specific)
		{
			type = _type;
			subType = _subType;
			gamePadId = _gamePadId;
			specific = _specific;
		}

		Type	type;
		SubType	subType;
		int		gamePadId;
		int		specific;
	};

	static bool CheckEvent(Event* event);
};

