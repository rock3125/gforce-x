#pragma once

#include "d3d9/InvalidatableDevice.h"

//////////////////////////////////////////////////////////

class GamePad : public InvalidatableDevice
{
	GamePad();
	~GamePad();
	const GamePad& operator=(const GamePad&);
	GamePad(const GamePad&);
public:
	// constants
	enum
	{
		NEGATIVE_MAX	= -1000,
		NEGATIVE_TH		= -300,
		POSITIVE_MAX	=  1000,
		POSITIVE_TH		=  300,
		MAX_GAMEPADS = 4,
		MAX_BUTTONS  = 12
	};

	// info gamepad structure
	struct Info
	{
		D3DXVECTOR2	analog1;
		D3DXVECTOR2	analog2;
		bool		buttons[MAX_BUTTONS];
	};

	// singleton access
	static GamePad* Get();

	// destroy singleton when finished
	static void Destroy();

	// process logic and set internal state
	void EventLogic(double time);

	// implement invalidate device interface
	void RestoreDeviceObjects();
	void InvalidateDeviceObjects();

	// access interface

	// see how many there are
	int GetNumGamePads();

	// get info interface for an indexed member
	Info* GetGamePad(int index);

	// get a gamepad axis mapping (0..7)
	bool GetAxisMapping(int gamePadId, int axisId);

	// get a button mapping for a gamepad
	bool GetButtonMapping(int gamePadId, int buttonId);

private:
	// callbacks

	// enumerate joysticks
	static BOOL CALLBACK EnumGamePadCallback(const DIDEVICEINSTANCE* pdidInstance, VOID* pContext);

	// enumerate objects on joystick
	static BOOL CALLBACK EnumObjectsCallback(const DIDEVICEOBJECTINSTANCE* pdidoi,VOID* pContext);

	// init game pad data
	void InitialiseGamePads();

private:

	// how many we have detected
	int	numGamePads;
	
	struct ObjData
	{
		GamePad*	gamePadInstance;
		int			gamePadIndex;
	};

	// direct input
	LPDIRECTINPUT8			pDirectInput;

	// joystick devices
	LPDIRECTINPUTDEVICE8	lpGamePad[MAX_GAMEPADS];

	Info					gamePad[MAX_GAMEPADS];

	// singleton access
	static GamePad*			singleton;
};

