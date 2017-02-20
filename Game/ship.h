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
#include "system/input.h"

class Model;
class ModelMap;
class Bullet;
class Shield;
class ParticleSource;
class Explosion;
class Missile;

///////////////////////////////////////////////////////////

class Ship : public WorldObject
{
public:
	Ship();
	~Ship();
	Ship(const Ship&);
	const Ship& operator=(const Ship&);

	// setup test controls
	void SetupKeyboard();
	void SetupGamePad(int gpId);

	// draw the ship
	virtual void Draw(double time, const D3DXVECTOR3& pos);
	void Draw(double time);
	virtual void DisplayStatus(float x, float y);

	// events from system regarding keys and other events
	virtual void EventLogic(double time);
	virtual void EventInit();

	// push the ship (explosions etc)
	void Push(float dx, float dy, float force);

	// save and load Ship
	virtual void Read(BaseStreamer&);
	virtual void Write(BaseStreamer&);

	// get set max missiles
	int GetMaxMissiles();
	void SetMaxMissiles(int maxMissiles);

	// tweakable parameters
	float GetMaxSpeed();
	void SetMaxSpeed(float _maxSpeed);

	float GetMaxAccel();
	void SetMaxAccel(float _maxAccel);

	float GetAccel();
	void SetAccel(float _accel);

	float GetFuelUsage();
	void SetFuelUsage(float _fuelUsage);

	float GetRotationalVelocity();
	void SetRotationalVelocity(float _rotationalVelocity);

	// offset height for landing
	float GetBaseOffset();
	void SetBaseOffset(float _shipOffset);

	// set a model for this ship
	void SetModel(Model* model);
	Model* GetModel();

	// get/set bulletModel model
	void SetBulletModel(Model* _bulletModel);
	Model* GetBulletModel();

	// bullets
	void SetMaxBulletLoad(int maxBulletLoad);
	int GetMaxBulletLoad();

	void SetMaxActiveBullets(int maxActiveBullets);
	int GetMaxActiveBullets();

	void SetBulletTTL(int bulletTTL);
	int GetBulletTTL();

	void SetBulletSpeed(float bulletSpeed);
	float GetBulletSpeed();

	void SetBulletDropOffFactor(float bulletDropOffFactor);
	float GetBulletDropOffFactor();

	void SetShieldStrength(float shieldStrength);
	float GetShieldStrength();

	void SetInitialPosition(const D3DXVECTOR3& initialPosition);

	// update internal bounding volume
	virtual void UpdateBoundingBox();
	virtual BoundingBox* GetBoundingBox();

protected:
	// init values
	void SetInit();

	// shoot the main weapon
	void Fire();

	// decrease shields with hit damage
	void DecreaseShields(float damage);

	// check bullets of other ship collide with me
	void CheckShipToShipCollision();
	void CheckCaveCollision();

private:
	// have we landed
	bool	hasLanded;
	// current speed
	float	speed;
	float	xSpeed;
	float	ySpeed;

	// x deceleration
	float	speedDeccel;
	float	xSpeedDeccel;
	// ships angle hdg
	float	angle;
	// maximum allowed acceleration
	float	maxAccel;
	// maximum speed
	float	maxSpeed;
	// possible acceleration
	float	accel;
	// rotational speed
	float	rotationalVelocity;
	// landing pad offset height
	float	baseOffset;

	// initial position
	D3DXVECTOR3	initialPosition;

	// landing conditions
	float	landSpeed;
	float	landHeight;
	float	landAngle;

	// the model used for this ship
	Model*	model;
	
	// model for bulletModels fired from this ship
	Model*	bulletModel;

	// the current map this ship is on
	ModelMap*	modelMap;

	// ships bullet load capacity
	int maxBulletLoad;
	// ships total ammo supply (whats left)
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

	// fuel usage of ship
	float	fuel;
	// fuel usage
	float	fuelUsage;

	// the physical shield around the ship
	int shieldHitCount;
	int shieldHitCountInit;
	Shield*	shieldModel;

	// shield settings [0..1]
	float shield;
	float shieldStrength;

	// delay firing meganism
	int fireDelay;
	int fireCounter;

	// when shields == 0 -> destroyed
	bool exploding;

	// particle exhaust
	ParticleSource*	exhaust;

	// explosion for ship
	Explosion*		explosion;

	// missile for ship
	Missile*		missile;
	// how many have we got?
	int				maxMissiles;
	int				numMissiles;

	// dummy target for non-specific targets
	WorldObject*	dummyTarget;

	// 5 inputs for system
	enum
	{
		THRUST			= 0,
		LEFT			= 1,
		RIGHT			= 2,
		FIRE_BULLET		= 3,
		FIRE_MISSILE	= 4,
		NUM_CONTROLS	= 5
	};

	Input::Event*	control[NUM_CONTROLS];

	enum
	{
		ICON_FUEL = 0,
		ICON_BULLET,
		ICON_SHIELD
	};

	// icons for status displays
	Texture* icons[3];

	// constants
	static float MISSILE_HIT_DAMAGE;
	static float BULLET_HIT_DAMAGE;
	static float WALL_HIT_DAMAGE;
	static float BULLET_MISSILE_HIT_DISTANCE;
	static int SHIELD_ANIM_COUNT;

	// signature and version
	static std::string shipSignature;
	static int shipVersion;
};
