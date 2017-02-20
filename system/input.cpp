#include "standardFirst.h"

#include "system/input.h"

#include "system/keyboard.h"
#include "system/gamePad.h"

/////////////////////////////////////////////////////////

Input::Input()
{
}

Input::~Input()
{
}

int Input::StringToKey(std::string str)
{
	if (str == "SK_ESCAPE")
		return SK_ESCAPE;
	if (str == "SK_UP")
		return SK_UP;
	if (str == "SK_DOWN")
		return SK_DOWN;
	if (str == "SK_LEFT")
		return SK_LEFT;
	if (str == "SK_RIGHT")
		return SK_RIGHT;
	if (str == "SK_ENTER")
		return SK_ENTER;
	if (str.length() != 1)
	{
		throw new Exception("unknown key type");
		return 0;
	}
	return (int)str[0];
}

bool Input::CheckEvent(Event* event)
{
	if (event != NULL)
	{
		if (event->type == KEYBOARD)
		{
			switch (event->subType)
			{
				case KEY:
				{
					return Keyboard::Get()->GetKeyDown(event->specific);
				}
				case SHIFT_KEY:
				{
					return Keyboard::Get()->GetShiftDown();
				}
				case CTRL_KEY:
				{
					return Keyboard::Get()->GetCtrlDown();
				}
			}
		}
		else if (event->type == GAMEPAD)
		{
			switch (event->subType)
			{
				case GAMEPAD_BUTTON:
				{
					return GamePad::Get()->GetButtonMapping(event->gamePadId, event->specific);
				}
				case GAMEPAD_AXIS:
				{
					return GamePad::Get()->GetAxisMapping(event->gamePadId, event->specific);
				}
			}
		}
	}
	return false;
}


