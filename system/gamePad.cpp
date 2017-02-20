#include "standardFirst.h"

#include "system/gamePad.h"

GamePad* GamePad::singleton = NULL;

/////////////////////////////////////////////////////////

GamePad* GamePad::Get()
{
	if (singleton == NULL)
	{
		singleton = new GamePad();
		singleton->RestoreDeviceObjects();
	}
	return singleton;
}

void GamePad::Destroy()
{
	safe_delete(singleton);
}

GamePad::GamePad()
	: pDirectInput(NULL)
{
	for (size_t i = 0; i < MAX_GAMEPADS; i++)
	{
		lpGamePad[i] = NULL;
	}
}

GamePad::~GamePad()
{
	InvalidateDeviceObjects();
}

int GamePad::GetNumGamePads()
{
	return numGamePads;
}

GamePad::Info* GamePad::GetGamePad(int index)
{
	PreCond(index < numGamePads);
	return &gamePad[index];
}

BOOL CALLBACK GamePad::EnumGamePadCallback(const DIDEVICEINSTANCE* pdidInstance, VOID* pContext)
{
    HRESULT hr;

	GamePad* instance = reinterpret_cast<GamePad*>(pContext);
	
    //! Obtain an interface to the enumerated joystick.
    hr = instance->pDirectInput->CreateDevice(pdidInstance->guidInstance, &instance->lpGamePad[instance->numGamePads], NULL);

	// on failure move onto next
    if (FAILED(hr))
        return DIENUM_CONTINUE;

	instance->numGamePads++;

	if (instance->numGamePads < MAX_GAMEPADS)
	{
        return DIENUM_CONTINUE;
	}
	else
	{
		return DIENUM_STOP;
	}
}

BOOL CALLBACK GamePad::EnumObjectsCallback(const DIDEVICEOBJECTINSTANCE* pdidoi,VOID* pContext)
{
	ObjData* obj = reinterpret_cast<ObjData*>(pContext);
	GamePad* instance = obj->gamePadInstance;

    //! For axes that are returned, set the DIPROP_RANGE property for the
    //! enumerated axis in order to scale min/max values.
    if (pdidoi->dwType & DIDFT_AXIS)
    {
        DIPROPRANGE diprg;
        diprg.diph.dwSize       = sizeof(DIPROPRANGE);
        diprg.diph.dwHeaderSize = sizeof(DIPROPHEADER);
        diprg.diph.dwHow        = DIPH_BYID;
        diprg.diph.dwObj        = pdidoi->dwType;
		// set axis deflection to these values
        diprg.lMin              = NEGATIVE_MAX;
        diprg.lMax              = POSITIVE_MAX;

        //! Set the range for the axis
		if (FAILED(instance->lpGamePad[obj->gamePadIndex]->SetProperty(DIPROP_RANGE, &diprg.diph)))
		{
            return DIENUM_STOP;
		}
    }
    return DIENUM_CONTINUE;
}

void GamePad::InitialiseGamePads()
{
	// init gamepad info data
	for (int i=0; i<MAX_GAMEPADS; i++ )
	{
		gamePad[i].analog1 = D3DXVECTOR2(0,0);
		gamePad[i].analog2 = D3DXVECTOR2(0,0);

		for (int j=0; j < MAX_BUTTONS; j++)
		{
			gamePad[i].buttons[j] = false;
		}
	}
}

void GamePad::InvalidateDeviceObjects()
{
	for (int i=0; i < MAX_GAMEPADS; i++)
	{
		safe_release(lpGamePad[i]);
	}
	InitialiseGamePads();
}

void GamePad::RestoreDeviceObjects()
{
	HRESULT hr;
	numGamePads = 0;

	InitialiseGamePads();

	if (pDirectInput == NULL)
	{
		// get pDI
		if (FAILED(hr = DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION, IID_IDirectInput8, (VOID**)&pDirectInput, NULL)))
		{
			throw new Exception("DirectInput8Create failed " + System::Int2Str(hr));
		}
	}
		
	if (FAILED(hr = pDirectInput->EnumDevices(DI8DEVCLASS_GAMECTRL, GamePad::EnumGamePadCallback, this, DIEDFL_ATTACHEDONLY)))
	{
		throw new Exception("EnumJoysticksCallback failed " + System::Int2Str(hr));
	}

	Log::GetLog() << "detected " + System::Int2Str(numGamePads) + " game pads" << System::CR << System::CR;

	for (int i=0; i < numGamePads; i++)
	{
		hr = lpGamePad[i]->SetDataFormat(&c_dfDIJoystick2);
		if (hr == DI_OK)
		{
			HWND hWnd = Interface::Get()->GetFocusWindow();
			::SetFocus(hWnd);
			::SetActiveWindow(hWnd);
			hr = lpGamePad[i]->SetCooperativeLevel(hWnd, DISCL_BACKGROUND);

			ObjData obj;
			obj.gamePadInstance = this;
			obj.gamePadIndex = i;
			hr = lpGamePad[i]->EnumObjects(EnumObjectsCallback,(VOID*)&obj,DIDFT_ALL);
		}
	}
}

void GamePad::EventLogic(double time)
{
	//! test the joysticks and feed them into the main system
	for (int i=0; i < numGamePads; i++)
	{
	    DIJOYSTATE2 js;
		HRESULT hr = lpGamePad[i]->Poll();
		if (FAILED(hr))
		{
			int maxCntr = 0;
			hr = lpGamePad[i]->Acquire();
			while (hr == DIERR_INPUTLOST && maxCntr < 100)
			{
				hr = lpGamePad[i]->Acquire();
			}
			InitialiseGamePads();
			return;
		}

		//! Get the input's device state
		if (lpGamePad[i]->GetDeviceState(sizeof(DIJOYSTATE2),&js) == DI_OK)
		{
			gamePad[i].analog1 = D3DXVECTOR2(0,0);
			if (js.lX < NEGATIVE_TH)
				gamePad[i].analog1.x =  -1;
			else if (js.lX > POSITIVE_TH)
				gamePad[i].analog1.x = 1;

			if (js.lY < NEGATIVE_TH)
				gamePad[i].analog1.y =  -1;
			else if (js.lY > POSITIVE_TH)
				gamePad[i].analog1.y = 1;

			gamePad[i].analog2 = D3DXVECTOR2(0,0);
			if (js.lRz < NEGATIVE_TH)
				gamePad[i].analog2.x =  -1;
			else if (js.lRz > POSITIVE_TH)
				gamePad[i].analog2.x = 1;

			if (js.rglSlider[0] < NEGATIVE_TH)
				gamePad[i].analog2.y =  -1;
			else if (js.rglSlider[0] > POSITIVE_TH)
				gamePad[i].analog2.y = 1;

			for (int j=0; j < MAX_BUTTONS; j++)
			{
				gamePad[i].buttons[j] = (js.rgbButtons[j] & 0x80) != 0;
			}
		}
	}
}

bool GamePad::GetAxisMapping(int gamePadId, int axisId)
{
	if (gamePadId < numGamePads)
	{
		switch (axisId)
		{
			case 0:
			{
				return gamePad[gamePadId].analog1.x < 0;
			}
			case 1:
			{
				return gamePad[gamePadId].analog1.x > 0;
			}
			case 2:
			{
				return gamePad[gamePadId].analog1.y < 0;
			}
			case 3:
			{
				return gamePad[gamePadId].analog1.y > 0;
			}
			case 4:
			{
				return gamePad[gamePadId].analog2.x < 0;
			}
			case 5:
			{
				return gamePad[gamePadId].analog2.x > 0;
			}
			case 6:
			{
				return gamePad[gamePadId].analog2.y < 0;
			}
			case 7:
			{
				return gamePad[gamePadId].analog2.y > 0;
			}
		}
	}
	return false;
}

bool GamePad::GetButtonMapping(int gamePadId, int buttonId)
{
	if (gamePadId < numGamePads && buttonId < MAX_BUTTONS)
	{
		return gamePad[gamePadId].buttons[buttonId];
	}
	return false;
}

