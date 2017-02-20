//////////////////////////////////////////////////////////////////////
/// GRAVITY FORCE X
///
/// (c)Copyright Peter de Vocht, 2006
///
/// I'm releasing this code under GPL v2.1 (see included file lgpl.txt)
///
//////////////////////////////////////////////////////////////////////

#include "standardFirst.h"

#include "menu/menuShip.h"
#include "d3d9/texture.h"
#include "game/ship.h"

#include "system/modelImporter.h"
#include "system/model/model.h"

/////////////////////////////////////////////////////////////

MenuShip::MenuShip()
	: shipIcon(NULL)
{
	player = 0;

	maxSpeed = 1.0f;
	maxAccel = 0.01f;
	accel = 0.0035f;
	rotationalVelocity = 5;
	fuelUsage = 0.0004f;
	numBullets = 200;
	numMissiles = 5;
	baseOffset = 6.5f;
	shieldStrength = 0.1f;
	control = Input::CTRL_NONE;
}

MenuShip::~MenuShip()
{
	shipIcon = NULL;
}

const std::string& MenuShip::GetModel()
{
	return modelStr;
}

void MenuShip::SetModel(const std::string& _modelStr)
{
	modelStr = _modelStr;
}

const std::string& MenuShip::GetIcon()
{
	return iconStr;
}

void MenuShip::SetIcon(const std::string& _iconStr)
{
	iconStr = _iconStr;
	if (!iconStr.empty())
	{
		shipIcon = TextureCache::GetTexture(iconStr);
	}
	else
	{
		shipIcon = NULL;
	}
}

int MenuShip::GetPlayer()
{
	return player;
}

void MenuShip::SetPlayer(int _player)
{
	player = _player;
}

Texture* MenuShip::GetShipIcon()
{
	return shipIcon;
}

void MenuShip::SetShipIcon(Texture* _shipIcon)
{
	shipIcon = _shipIcon;
}

void MenuShip::SetControl(int _control)
{
	control = _control;
}

Ship* MenuShip::GetShip()
{
	Ship* ship = new Ship();
	ship->SetMaxSpeed(maxSpeed);
	ship->SetMaxAccel(maxAccel);
	ship->SetAccel(accel);
	ship->SetRotationalVelocity(rotationalVelocity);
	ship->SetFuelUsage(fuelUsage);
	ship->SetMaxBulletLoad(numBullets);
	ship->SetMaxMissiles(numMissiles);
	ship->SetShieldStrength(shieldStrength);
	ship->SetBaseOffset(baseOffset);

	// setup controller
	switch (control)
	{
		case Input::CTRL_KEYBOARD:
		{
			ship->SetupKeyboard();
			break;
		}
		case Input::CTRL_GAMEPAD1:
		{
			ship->SetupGamePad(0);
			break;
		}
		case Input::CTRL_GAMEPAD2:
		{
			ship->SetupGamePad(1);
			break;
		}
		case Input::CTRL_GAMEPAD3:
		{
			ship->SetupGamePad(2);
			break;
		}
		case Input::CTRL_GAMEPAD4:
		{
			ship->SetupGamePad(3);
			break;
		}
		default:
		{
			throw new Exception("controller not selected");
			break;
		}
	}

	// get model
	ModelImporter import;
	WorldObject* obj = import.Import(System::GetDataDirectory() + modelStr);
	ship->SetModel(dynamic_cast<Model*>(obj));

	ModelImporter bullet;
	WorldObject* bobj = import.Import(System::GetDataDirectory() + "objects\\bullet.wrl");
	Model* bulletObj = dynamic_cast<Model*>(bobj);
	bulletObj->SetColour(bulletColour);
	ship->SetBulletModel(bulletObj);

	return ship;
}

float MenuShip::GetBaseOffset()
{
	return baseOffset;
}

