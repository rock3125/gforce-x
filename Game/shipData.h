//////////////////////////////////////////////////////////////////////
/// GRAVITY FORCE X
///
/// (c)Copyright Peter de Vocht, 2006
///
/// I'm releasing this code under GPL v2.1 (see included file lgpl.txt)
///
//////////////////////////////////////////////////////////////////////

#pragma once

class Texture;
class Ship;
class WorldObject;

///////////////////////////////////////////////////////////

class ShipData
{
	ShipData(const ShipData&);
	const ShipData& operator=(const ShipData&);
public:
	ShipData();
	~ShipData();

	const std::string& GetModel();
	void SetModel(const std::string& model);

	const std::string& GetIcon();
	void SetIcon(const std::string& name);

	int ShipId()
	{
		return id;
	}

	int GetPlayer();
	void SetPlayer(int players);

	Texture* GetShipIcon();
	void SetShipIcon(Texture* shipIcon);

	// get base offset
	float GetBaseOffset();

	// get the ship (if selected) that is equivalent to the settings
	Ship* GetShip();

	// set controller to be used on this ship
	void SetControl(int control);

private:
	friend class Runtime;

	std::string modelStr;
	std::string iconStr;
	int id; // the id of the ship in the xml (i.e. index)
	int player;
	int control;

	// ship settings
	float maxSpeed;
	float maxAccel;
	float baseOffset;
	float accel;
	float rotationalVelocity;
	float fuelUsage;
	int numBullets;
	int numMissiles;
	float shieldStrength;
	D3DXCOLOR bulletColour;

	float missileacceleration;
	float missilefuel;
	float missilefuelusage;
	float missilemaxspeed;
	float missilestrength;
	float missileforce;

	WorldObject* shipModel;
	WorldObject* bulletModel;

	Texture* shipIcon;
};

