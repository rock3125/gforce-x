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

///////////////////////////////////////////////////////////

class MenuShip
{
	MenuShip(const MenuShip&);
	const MenuShip& operator=(const MenuShip&);
public:
	MenuShip();
	~MenuShip();

	const std::string& GetModel();
	void SetModel(const std::string& model);

	const std::string& GetIcon();
	void SetIcon(const std::string& name);

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
	friend class MenuShipManager;

	std::string modelStr;
	std::string iconStr;
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

	Texture* shipIcon;
};

