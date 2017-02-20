#pragma once

//////////////////////////////////////////////////////////

class Keyboard
{
	Keyboard();
	~Keyboard();
	const Keyboard& operator=(const Keyboard&);
	Keyboard(const Keyboard&);
public:
	static Keyboard* Get();
	static void Destroy();

	// event callbacks
	void KeyDown(int key);
	void KeyUp(int key);

	// check if a key is down or now
	bool GetKeyDown(int key);
	bool GetShiftDown();
	bool GetCtrlDown();

	// just in case you want to set it yourself
	void SetKeyDown(int key, bool value);
	void SetShiftDown(bool value);
	void SetCtrlDown(bool value);

	// reset keys
	void Initialise();

private:
	enum
	{
		NUM_KEYS	= 256
	};

	bool keyboard[NUM_KEYS];
	bool shiftDown;
	bool ctrlDown;

	static Keyboard* singleton;
};

