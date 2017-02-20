//////////////////////////////////////////////////////////////////////
/// GRAVITY FORCE X
///
/// (c)Copyright Peter de Vocht, 2006
///
/// I'm releasing this code under GPL v2.1 (see included file lgpl.txt)
///
//////////////////////////////////////////////////////////////////////

#include "standardFirst.h"

#include "menu/menuShipManager.h"
#include "game/shipData.h"
#include "runtime/runtime.h"
#include "system/baseApp.h"

#include "d3d9/texture.h"
#include "system/xml/XmlParser.h"

/////////////////////////////////////////////////////////////

MenuShipManager::MenuShipManager()
{
	activeItem = 0;
	blinking = false;
	blinkTime = 0;
	lastAction = LA_NONE;
	BLINK_INTERVAL = 0.25;

	control[CT_LEFT1] = new Input::Event(Input::KEYBOARD, Input::KEY, 0, Input::SK_LEFT);
	control[CT_RIGHT1] = new Input::Event(Input::KEYBOARD, Input::KEY, 0, Input::SK_RIGHT);
	control[CT_LEFT2] = new Input::Event(Input::GAMEPAD, Input::GAMEPAD_AXIS, 0, 0);
	control[CT_RIGHT2] = new Input::Event(Input::GAMEPAD, Input::GAMEPAD_AXIS, 0, 1);

	colour1 = D3DXCOLOR(1,1,1,1);
	colour2 = D3DXCOLOR(0.8f,0.8f,0.8f,1);
}

MenuShipManager::~MenuShipManager()
{
	for (int i=0; i < NUM_CONTROLS; i++)
	{
		safe_delete(control[i]);
	}
}

MenuShipManager::MenuShipManager(const MenuShipManager& mm)
{
	operator=(mm);
}

const MenuShipManager& MenuShipManager::operator=(const MenuShipManager& mm)
{
	activeItem = mm.activeItem;
	lastAction = mm.lastAction;
	return *this;
}

void MenuShipManager::Draw(bool selected, float& xpos, float& ypos)
{
	// draw levels
	Device* dev = Interface::GetDevice();

	D3DXVECTOR2 leftTop;
	D3DXVECTOR2 rightBottom;
	D3DXCOLOR colour;

	colour = colour1;
	for (int i=0; i < ships.size(); i++)
	{
		if (selected && activeItem == i)
		{
			if (blinking)
			{
				colour = colour1;
			}
			else
			{
				colour = colour2;
			}
		}
		else
		{
			colour = colour1;
		}

		float xp = xpos + (i%8) * 134;
		float yp = ypos + (i/8) * 70;

		ShipData* ms = ships[i];
		Texture* t = ms->GetShipIcon();
		leftTop = D3DXVECTOR2(xp,yp);
		float w = (float)t->GetWidth();
		float h = (float)t->GetHeight();
		rightBottom = D3DXVECTOR2(xp + w, yp + h);
		dev->FillRect(leftTop,rightBottom,colour,t);
	}
	ypos += ((ships.size() / 8)+1) * 80;
	ypos += 40;
}

void MenuShipManager::EventLogic(double time)
{
	blinkTime += time;
	if (blinkTime > BLINK_INTERVAL)
	{
		blinkTime = 0;
		blinking = !blinking;
	}

	// check inputs
	if ((Input::CheckEvent(control[CT_LEFT1]) || Input::CheckEvent(control[CT_LEFT2])) && lastAction != LA_LEFT)
	{
		lastAction = LA_LEFT;

		if (activeItem > 0)
		{
			activeItem--;
		}
	}
	else if ((Input::CheckEvent(control[CT_RIGHT1]) || Input::CheckEvent(control[CT_RIGHT2])) && lastAction != LA_RIGHT)
	{
		lastAction = LA_RIGHT;

		if ((activeItem+1) < ships.size())
		{
			activeItem++;
		}
	}
	if (!(Input::CheckEvent(control[CT_LEFT1]) || Input::CheckEvent(control[CT_LEFT2])) && lastAction == LA_LEFT)
		lastAction = LA_NONE;
	if (!(Input::CheckEvent(control[CT_RIGHT1]) || Input::CheckEvent(control[CT_RIGHT2])) && lastAction == LA_RIGHT)
		lastAction = LA_NONE;
}

ShipData* MenuShipManager::GetSelectedShip()
{
	return ships[activeItem];
}

