#include "standardFirst.h"

#include "system/keyboard.h"

Keyboard* Keyboard::singleton = NULL;

/////////////////////////////////////////////////////////

Keyboard* Keyboard::Get()
{
	if (singleton == NULL)
	{
		singleton = new Keyboard();
	}
	return singleton;
}

void Keyboard::Destroy()
{
	safe_delete(singleton);
}

Keyboard::Keyboard()
{
	Initialise();
}

Keyboard::~Keyboard()
{
}

void Keyboard::Initialise()
{
	for (int i=0; i < NUM_KEYS; i++)
	{
		keyboard[i] = false;
	}
	shiftDown = false;
	ctrlDown = false;
}

void Keyboard::KeyDown(int key)
{
	if (key < NUM_KEYS)
	{
		keyboard[key] = true;
	}
}

void Keyboard::KeyUp(int key)
{
	if (key < NUM_KEYS)
	{
		keyboard[key] = false;
	}
}

bool Keyboard::GetKeyDown(int key)
{
	return keyboard[key];
}

void Keyboard::SetKeyDown(int key, bool value)
{
	keyboard[key] = value;
}

bool Keyboard::GetShiftDown()
{
	return shiftDown;
}

bool Keyboard::GetCtrlDown()
{
	return ctrlDown;
}

void Keyboard::SetShiftDown(bool value)
{
	shiftDown = value;
}

void Keyboard::SetCtrlDown(bool value)
{
	ctrlDown = value;
}

