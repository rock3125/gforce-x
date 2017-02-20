//////////////////////////////////////////////////////////////////////
/// GRAVITY FORCE X
///
/// (c)Copyright Peter de Vocht, 2006
///
/// I'm releasing this code under GPL v2.1 (see included file lgpl.txt)
///
//////////////////////////////////////////////////////////////////////

#pragma once

#include "system/model/worldObject.h"

class Model;
class Bullet;

///////////////////////////////////////////////////////////

class Turret : public WorldObject
{
public:
	Turret();
	~Turret();
	Turret(const Turret&);
	const Turret& operator=(const Turret&);

	// draw it
	virtual void Draw(double time);
	// draw it relative to point
	void Draw(double time, const D3DXVECTOR3& pos);

	// event callbacks
	virtual void EventLogic(double time);

	// overwrite
	void UpdateBoundingBox();

	// save and load region
	virtual void Read(XmlNode* node);
	virtual XmlNode* Write();

	// return the signature of this object
	virtual std::string Signature()
	{
		return turretSignature;
	}

	Model* GetModel()
	{
		return model;
	}

	void SetModel(Model* model)
	{
		this->model = model;
	}

protected:
	// setup common values
	void InitVars();
	// setup bullet system
	void SetMaxActiveBullets(int maxActiveBullets);
	// see if my bullets impact anything
	void CheckShipCollision();

private:

	// model to be drawn
	Model*	model;
	// bullet model
	Model*	bulletModel;

	// distance at which turret will react
	float turretDist;

	// turret's total ammo supply (whats left)
	int	numBullets;
	// bullets at any one time
	int	maxActiveBullets;
	// bullet ttl
	int bulletTTL;
	// bullet speed
	float bulletSpeed;
	// bullet dropoff factor
	float dropOffFactor;
	// each bullet for the system
	Bullet* bullets;
	// bullet fire management
	int fireCounter;
	int fireDelay;

	static std::string turretSignature;
	static int turretVersion;
};

