//////////////////////////////////////////////////////////////////////
/// GRAVITY FORCE X
///
/// (c)Copyright Peter de Vocht, 2006
///
/// I'm releasing this code under GPL v2.1 (see included file lgpl.txt)
///
//////////////////////////////////////////////////////////////////////

#include "standardFirst.h"

#include "game/ship.h"
#include "game/shipData.h"
#include "d3d9/texture.h"

#include "system/model/model.h"

/////////////////////////////////////////////////////////////

ShipData::ShipData()
	: shipIcon(NULL)
	, shipModel(NULL)
	, bulletModel(NULL)
{
	player = 0;

	maxSpeed = 2.5f;
	maxAccel = 0.01f;
	accel = 0.0035f;
	rotationalVelocity = 5;
	fuelUsage = 0.0004f;
	numBullets = 200;
	numMissiles = 5;
	baseOffset = 6.5f;
	shieldStrength = 0.1f;
	control = Input::CTRL_NONE;

	missileacceleration = 0.25f;
	missilefuel = 5.0f;
	missilefuelusage = 0.01f;
	missilemaxspeed = 3.5f;
	missilestrength = 40.0f;
	missileforce = 5.0f;
}

ShipData::~ShipData()
{
	shipIcon = NULL;
	safe_delete(shipModel);
	safe_delete(bulletModel);
}

const std::string& ShipData::GetModel()
{
	return modelStr;
}

void ShipData::SetModel(const std::string& modelStr)
{
	this->modelStr = modelStr;
}

const std::string& ShipData::GetIcon()
{
	return iconStr;
}

void ShipData::SetIcon(const std::string& _iconStr)
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

int ShipData::GetPlayer()
{
	return player;
}

void ShipData::SetPlayer(int player)
{
	this->player = player;
}

Texture* ShipData::GetShipIcon()
{
	return shipIcon;
}

void ShipData::SetShipIcon(Texture* shipIcon)
{
	this->shipIcon = shipIcon;
}

void ShipData::SetControl(int control)
{
	this->control = control;
}

Ship* ShipData::GetShip()
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

	ship->SetMissileData(missileacceleration, missilefuel, missilefuelusage, 
						 missilemaxspeed, missilestrength, missileforce);

	// setup controller
	switch (control)
	{
		case Input::CTRL_KEYBOARD0:
		{
			ship->SetupKeyboard(0);
			break;
		}
		case Input::CTRL_KEYBOARD1:
		{
			ship->SetupKeyboard(1);
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
			// default is keyboard(0)
			ship->SetupKeyboard(0);
			break;
		}
	}

	// get model
	ship->SetModel(dynamic_cast<Model*>(shipModel));

	Model* bulletObj = dynamic_cast<Model*>(bulletModel);
	bulletObj->SetColour(bulletColour);
	ship->SetBulletModel(bulletObj);

	return ship;
}

float ShipData::GetBaseOffset()
{
	return baseOffset;
}

