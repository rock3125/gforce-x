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

/////////////////////////////////////////////////////////////

class Bullet : public WorldObject
{
public:
	Bullet();
	~Bullet();
	Bullet(const Bullet&);
	const Bullet& operator=(const Bullet&);

	// fire a bullet (activate it)
	void Fire(const D3DXVECTOR3& pos, float baseOffset, float angle);

	// set bullet parameters, time to live and bullet speed
	void SetParameters(float ttl, float speed, float dropOffFactor);

	// is this bullet in use?
	bool GetInUse();
	void SetInUse(bool inUse);

	// overwrite drawing routine
	void Draw(double time, Model* model, const D3DXVECTOR3& pos);

	// overwrite logic callback
	void EventLogic(double time);

private:
	// bullet speed
	float	speed;
	// in use?
	bool	inUse;
	// time to live
	float	ttl;
	// direction
	float	angle;
	// additional speed caused by parent
	float	xSpeed;
	float	ySpeed;
	// initial ttl
	float	initialTTL;
	// how fast do we go down
	float	dropOffFactor;
};

