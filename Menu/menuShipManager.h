//////////////////////////////////////////////////////////////////////
/// GRAVITY FORCE X
///
/// (c)Copyright Peter de Vocht, 2006
///
/// I'm releasing this code under GPL v2.1 (see included file lgpl.txt)
///
//////////////////////////////////////////////////////////////////////

#pragma once

#include "system/input.h"

class ShipData;

//////////////////////////////////////////////////////////////

class MenuShipManager
{
public:
	MenuShipManager();
	~MenuShipManager();
	MenuShipManager(const MenuShipManager&);
	const MenuShipManager& operator=(const MenuShipManager&);

	void SetShipDefinitions(std::vector<ShipData*> ships)
	{
		this->ships = ships;
	}

	// draw menu manager
	void Draw(bool selected, float& xpos, float& ypos);

	// events
	void EventLogic(double time);

	// get the selected ship
	ShipData* GetSelectedShip();

private:
	enum
	{
		CT_LEFT1 = 0,
		CT_LEFT2,
		CT_RIGHT1,
		CT_RIGHT2,
		NUM_CONTROLS = 4
	};

	enum LastAction
	{
		LA_NONE,
		LA_LEFT,
		LA_RIGHT
	};

	// inputs
	Input::Event* control[NUM_CONTROLS];

	// collection of all menus
	std::vector<ShipData*> ships;

	// flashing menu item
	double blinkTime;
	bool blinking;
	int lastAction;

	// blink speed
	double BLINK_INTERVAL;

	D3DXCOLOR colour1;
	D3DXCOLOR colour2;

	// is menu system active?
	int		activeItem;
};

