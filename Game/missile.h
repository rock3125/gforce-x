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

class ParticleSource;
class Ship;

///////////////////////////////////////////////////////////

class Missile : public WorldObject
{
public:
	Missile();
	~Missile();
	Missile(const Missile&);
	const Missile& operator=(const Missile&);

	// set the values for the missile's behaviour
	void SetData(float acceleration, float fuel, float fuelusage, float maxspeed, float strength, float force);

	// is the missile in use?
	bool GetInUse();
	void SetInUse(bool inUse);

	// is the missile exploding?
	bool GetExploding();

	// fire it
	void Fire(D3DXVECTOR3 startPos, float angle, float baseOffset, 
			  float speed, WorldObject* target);

	// get/set target
	void SetTarget(WorldObject* target);
	WorldObject* GetTarget();

	// get/set initial angle
	void SetAngle(float angle);
	float GetAngle();

	// event logic
	void EventLogic(double time);

	// draw it
	void Draw(double time, const D3DXVECTOR3& pos);

	// tell a missile its time to explode
	void Explode();

private:
	// check collisions
	bool Collision();
	
	// missile collides with wall
	bool CollisionMissleWithWall();

	// missile collides with ship
	bool CollisionMissileWithShip();

	// calculate map damage
	void DoMapDamage();

private:
	// missile active?
	bool		inUse;
	// fuel left in missile
	float		fuel;
	// speed of missile
	float		speed;
	// angle pointing of missile
	float		angle;
	// exploding?
	bool		exploding;
	// target destination
	WorldObject* target;

	// custom particle source for missile thruster
	ParticleSource*	exhaust;
	ParticleSource*	explosion;

	// consts
	float MAX_SPEED;
	float INIT_ACCEL;
	float INIT_FUEL;
	float FUEL_USAGE;
	float ANGULAR_SPEED;
	float LENGTH;
	float INFLUENCE_RANGE;
	float MISSILE_FORCE;
};

