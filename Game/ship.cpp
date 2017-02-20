//////////////////////////////////////////////////////////////////////
/// GRAVITY FORCE X
///
/// (c)Copyright Peter de Vocht, 2006
///
/// I'm releasing this code under GPL v2.1 (see included file lgpl.txt)
///
//////////////////////////////////////////////////////////////////////

#include "standardFirst.h"

#include "runtime/runtime.h"

#include "system/BaseApp.h"
#include "system/particleSource.h"
#include "system/model/model.h"
#include "system/network/dataPacket.h"

#include "game/ship.h"
#include "game/modelMap.h"
#include "game/base.h"
#include "game/bullet.h"
#include "game/shield.h"
#include "game/explosion.h"
#include "game/missile.h"
#include "game/water.h"

std::string Ship::shipSignature="ship";
int Ship::shipVersion=1;

// constants
float Ship::MISSILE_HIT_DAMAGE = 1;
float Ship::BULLET_HIT_DAMAGE = 0.1f;
float Ship::WALL_HIT_DAMAGE = 0.2f;
int Ship::SHIELD_ANIM_COUNT = 20;
float Ship::BULLET_MISSILE_HIT_DISTANCE = 4;

///////////////////////////////////////////////////////////

Ship::Ship()
	: model(NULL)
	, modelMap(NULL)
	, bulletModel(NULL)
	, bullets(NULL)
	, shieldModel(NULL)
	, exhaust(NULL)
	, explosion(NULL)
	, missile(NULL)
	, dummyTarget(NULL)
{
	SetType(OT_SHIP);
	SetWorldType(TYPE_SHIP);

	SetInit();

	// setup controls
	control[THRUST] = NULL;
	control[LEFT]   = NULL;
	control[RIGHT]  = NULL;
	control[FIRE_BULLET]  = NULL;
	control[FIRE_MISSILE] = NULL;

	model = new Model();
	bulletModel = new Model();

	exhaust = new ParticleSource();
	exhaust->SetStartSize(2);
	exhaust->SetEndSize(6);

	explosion = new Explosion();
	shieldModel = new Shield();
	missile = new Missile();
	dummyTarget = new WorldObject();

	// load icons
	icons[ICON_SHIELD] = TextureCache::GetTexture("icons\\shield.tga");
	icons[ICON_FUEL]   = TextureCache::GetTexture("icons\\fuel.tga");
	icons[ICON_BULLET] = TextureCache::GetTexture("icons\\bullet.tga");
}

Ship::~Ship()
{
	icons[ICON_SHIELD] = NULL;
	icons[ICON_FUEL] = NULL;
	icons[ICON_BULLET] = NULL;

	safe_delete(model);
	safe_delete(bulletModel);
	safe_delete_array(bullets);
	safe_delete(shieldModel);
	safe_delete(exhaust);
	safe_delete(explosion);
	safe_delete(missile);
	safe_delete(dummyTarget);

	safe_delete(control[THRUST]);
	safe_delete(control[LEFT]);
	safe_delete(control[RIGHT]);
	safe_delete(control[FIRE_BULLET]);
	safe_delete(control[FIRE_MISSILE]);
}

Ship::Ship(const Ship& s)
	: model(NULL)
	, modelMap(NULL)
	, bulletModel(NULL)
	, bullets(NULL)
	, shieldModel(NULL)
	, explosion(NULL)
	, missile(NULL)
	, dummyTarget(NULL)
{
	SetInit();
	operator=(s);
}

const Ship& Ship::operator=(const Ship& s)
{
	WorldObject::operator=(s);

	// copy models
	*model = *s.model;
	*bulletModel = *s.bulletModel;

	// global settings for the level
	speedDeccel = s.speedDeccel;
	xSpeedDeccel = s.xSpeedDeccel;
	modelMap = s.modelMap;
	baseOffset = s.baseOffset;

	// customisable settings
	maxSpeed = s.maxSpeed;
	maxAccel = s.maxAccel;
	accel = s.accel;
	rotationalVelocity = s.rotationalVelocity;
	angle = s.angle;
	dropOffFactor = s.dropOffFactor;
	initialPosition = s.initialPosition;

	// shield
	shieldStrength = s.shieldStrength;
	shield = s.shield;

	exploding = s.exploding;

	// ammo
	maxBulletLoad = s.maxBulletLoad;
	numBullets = s.numBullets;
	maxActiveBullets = s.maxActiveBullets;
	safe_delete_array(bullets);
	bullets = new Bullet[maxActiveBullets];
	bulletTTL = s.bulletTTL;
	bulletSpeed = s.bulletSpeed;
	dropOffFactor = s.dropOffFactor;
	numMissiles = s.numMissiles;
	maxMissiles = s.maxMissiles;

	return *this;
}

void Ship::SetMissileData(float acceleration, float fuel, float fuelusage, float maxspeed, float strength, float force)
{
	missile->SetData(acceleration, fuel, fuelusage, maxspeed, strength, force);
}

void Ship::WriteToPacket(DataPosition* packet)
{
	// transmit my details to other players
	packet->pos = GetPosition();
	packet->rot = GetRotationQuat();
	packet->angle = angle;

	// thrust
	packet->hasThrust = hasThrust;
	packet->doFire = doFire;
	packet->doWaterSound = doWaterSound;
	packet->startExplosion = startExplosion;
	packet->speed = speed;
	packet->baseOffset = baseOffset;
	packet->doFireMissile = doFireMissile;
	packet->missileTargetId = missileTargetId;
	packet->shieldHitCount = shieldHitCount;

	// indicators
	packet->shield = shield;
	packet->fuel = fuel;
	packet->numBullets = numBullets;
	packet->numMissiles = numMissiles;
	packet->myScore = myScore;
}

void Ship::ReadFromPacket(DataPosition* dataPosition)
{
	Runtime* rt = BaseApp::Get()->GetCurrentRuntime();
	std::vector<Ship*> ships = rt->GetShips();

	SetPosition(dataPosition->pos);
	SetRotationQuat(dataPosition->rot);
	UpdateWorldTransform();

	angle = dataPosition->angle;
	speed = dataPosition->speed;
	baseOffset = dataPosition->baseOffset;
	missileTargetId = dataPosition->missileTargetId;
	shieldHitCount = dataPosition->shieldHitCount;

	// fire a bullet?
	if (dataPosition->doFire)
	{
		DoFire();
	}

	// the missile
	if (dataPosition->doFireMissile)
	{
		// find the ship with the right missileTargetId (isa playerId)
		Ship* ship = NULL;
		for (int i=0; i < ships.size(); i++)
		{
			if (ships[i]->PlayerId() == missileTargetId)
			{
				ship = ships[i];
				break;
			}
		}
		if (ship != NULL)
		{
			missile->Fire(GetPosition(),angle, baseOffset, speed, ship); 
			rt->GP_PlaySound(Runtime::FIRE_MISSILE, false);
		}
	}

	// explosion?
	if (dataPosition->startExplosion)
	{
		shield = 0;
		DecreaseShields(0);
	}

	// indicators
	shield = dataPosition->shield;
	fuel = dataPosition->fuel;
	numBullets = dataPosition->numBullets;
	numMissiles = dataPosition->numMissiles;
	myScore = dataPosition->myScore;

	// sounds
	if (dataPosition->hasThrust)
	{
		// add particle
		D3DXVECTOR3 pos = GetPosition();
		pos.z = 8;
		exhaust->AddParticle(pos, baseOffset * 1.25f, angle + 180);

		// make sound
		rt->GP_PlaySound(Runtime::THRUST, true);
	}
	if (dataPosition->doWaterSound)
	{
		rt->GP_PlaySound(Runtime::UNDERWATER, false);
	}
}

// configure this ship to use the keyboard
void Ship::SetupKeyboard(int kbId)
{
	control[THRUST] = new Input::Event(Input::KEYBOARD, Input::KEY, 0, BaseApp::Get()->Down(kbId));
	control[LEFT]   = new Input::Event(Input::KEYBOARD, Input::KEY, 0, BaseApp::Get()->Left(kbId));
	control[RIGHT]  = new Input::Event(Input::KEYBOARD, Input::KEY, 0, BaseApp::Get()->Right(kbId));
	control[FIRE_BULLET]  = new Input::Event(Input::KEYBOARD, Input::KEY, 0, BaseApp::Get()->Up(kbId));
   	control[FIRE_MISSILE] = new Input::Event(Input::KEYBOARD, Input::KEY, 0, BaseApp::Get()->Fire1(kbId));
}

// configure this stip to use gamepad with id
void Ship::SetupGamePad(int gpId)
{
	control[THRUST] = new Input::Event(Input::GAMEPAD, Input::GAMEPAD_AXIS, gpId, 3);
	control[LEFT]   = new Input::Event(Input::GAMEPAD, Input::GAMEPAD_AXIS, gpId, 0);
	control[RIGHT]  = new Input::Event(Input::GAMEPAD, Input::GAMEPAD_AXIS, gpId, 1);
	control[FIRE_BULLET]  = new Input::Event(Input::GAMEPAD, Input::GAMEPAD_BUTTON, gpId, 0);
	control[FIRE_MISSILE] = new Input::Event(Input::GAMEPAD, Input::GAMEPAD_BUTTON, gpId, 2);
}

void Ship::SetInit()
{
	// landing conditions
	landSpeed = 0.2f;
	landHeight = 1;
	landAngle = 10;
	baseOffset = 6.25f;

	speed = 0;
	xSpeed = 0;
	ySpeed = 0;
	hasLanded = true;
	angle = 0;

	isUnderwater = false;

	// global settings for the level
	speedDeccel = 0.90f;
	xSpeedDeccel = 0.98f;

	// customisable settings
	maxSpeed = 1;
	maxAccel = 0.01f;
	accel = 0.0035f;
	rotationalVelocity = 5;
	fuel = 1;
	fuelUsage = 0.0001f;

	// count score
	myScore = 0;

	// ammo
	maxBulletLoad = 1000;
	numBullets = maxBulletLoad;
	maxMissiles = 5;
	numMissiles = maxMissiles;
	maxActiveBullets = 20;
	bulletTTL = 100;
	bulletSpeed = 2;
	dropOffFactor = 4;
	SetMaxActiveBullets(maxActiveBullets);

	bulletTime = 0.0;
	BULLET_DELAY = 0.1;

	// shield
	shieldStrength = 0.1f;
	shield = 1;
	shieldHitCount = 0;

	exploding = false;

	// missile default settings
	missileAcceleration = 0.25f;
	missileFuel = 5.0f;
	missileFuelusage = 0.01f;
	missileMaxspeed = 3.5f;
	missileStrength = 40.0f;
	missileForce = 5.0f;

	// network settigns
	isNetworkControlled = false;
	hasThrust = false;
	doFire = false;
	doWaterSound = false;
	startExplosion = false;
	doFireMissile = false;
	missileTargetId = 0;
}

void Ship::SetModel(Model* _model)
{
	*model = *_model;
}

Model* Ship::GetModel()
{
	return model;
}

void Ship::SetBulletModel(Model* _bulletModel)
{
	*bulletModel = *_bulletModel;
}

int Ship::GetMaxMissiles()
{
	return maxMissiles;
}

void Ship::SetMaxMissiles(int _maxMissiles)
{
	maxMissiles = _maxMissiles;
}

Model* Ship::GetBulletModel()
{
	return bulletModel;
}

float Ship::GetMaxSpeed()
{
	return maxSpeed;
}

void Ship::SetMaxSpeed(float _maxSpeed)
{
	maxSpeed = _maxSpeed;
}

float Ship::GetMaxAccel()
{
	return maxAccel;
}

void Ship::SetMaxAccel(float _maxAccel)
{
	maxAccel = _maxAccel;
}

float Ship::GetAccel()
{
	return accel;
}

void Ship::SetAccel(float _accel)
{
	accel = _accel;
}

float Ship::GetFuelUsage()
{
	return fuelUsage;
}

void Ship::SetFuelUsage(float _fuelUsage)
{
	fuelUsage = _fuelUsage;
}

float Ship::GetRotationalVelocity()
{
	return rotationalVelocity;
}

void Ship::SetRotationalVelocity(float _rotationalVelocity)
{
	rotationalVelocity = _rotationalVelocity;
}

float Ship::GetBaseOffset()
{
	return baseOffset;
}

void Ship::SetBaseOffset(float _baseOffset)
{
	baseOffset = _baseOffset;
}

void Ship::SetMaxBulletLoad(int _maxBulletLoad)
{
	maxBulletLoad = _maxBulletLoad;
	numBullets = maxBulletLoad;
}

int Ship::GetMaxBulletLoad()
{
	return maxBulletLoad;
}

void Ship::SetMaxActiveBullets(int _maxActiveBullets)
{
	maxActiveBullets = _maxActiveBullets;

	safe_delete_array(bullets);
	bullets = new Bullet[maxActiveBullets];

	// set bullet parameters
	for (int i=0; i < maxActiveBullets; i++)
	{
		bullets[i].SetParameters((float)bulletTTL, (float)bulletSpeed, dropOffFactor);
	}
}

int Ship::GetMaxActiveBullets()
{
	return maxActiveBullets;
}

void Ship::SetBulletTTL(int _bulletTTL)
{
	bulletTTL = _bulletTTL;

	// set bullet parameters
	for (int i=0; i < maxActiveBullets; i++)
	{
		bullets[i].SetParameters((float)bulletTTL, (float)bulletSpeed, dropOffFactor);
	}
}

void Ship::SetBulletDropOffFactor(float bulletDropOffFactor)
{
	dropOffFactor = bulletDropOffFactor;

	// set bullet parameters
	for (int i=0; i < maxActiveBullets; i++)
	{
		bullets[i].SetParameters((float)bulletTTL, (float)bulletSpeed, dropOffFactor);
	}
}

float Ship::GetBulletDropOffFactor()
{
	return dropOffFactor;
}

int Ship::GetBulletTTL()
{
	return bulletTTL;
}

void Ship::SetBulletSpeed(float _bulletSpeed)
{
	bulletSpeed = _bulletSpeed;

	// set bullet parameters
	for (int i=0; i < maxActiveBullets; i++)
	{
		bullets[i].SetParameters((float)bulletTTL, (float)bulletSpeed, dropOffFactor);
	}
}

float Ship::GetBulletSpeed()
{
	return bulletSpeed;
}

void Ship::SetShieldStrength(float _shieldStrength)
{
	shieldStrength = _shieldStrength;
}

float Ship::GetShieldStrength()
{
	return shieldStrength;
}

void Ship::Push(float dx, float dy, float force)
{
	xSpeed += dx * force;
	ySpeed += dy * force;

	hasLanded = false;

	// start animation
	shieldHitCount = Ship::SHIELD_ANIM_COUNT;
	DecreaseShields(force * shieldStrength * 0.1f);
}

void Ship::EventLogic(double time)
{
	Runtime* rt = BaseApp::Get()->GetCurrentRuntime();
	std::vector<Ship*> ships = rt->GetShips();

	// reset the explosion flag for networking purposes
	startExplosion = false;
	doFireMissile = false;
	bulletTime = bulletTime + time;

	// process missile logic
	missile->EventLogic(time);

	// logic for bullets - always!
	for (int i=0; i < maxActiveBullets; i++)
	{
		if (bullets[i].GetInUse())
		{
			bullets[i].EventLogic(time);
		}
	}

	// do exhaust
	exhaust->EventLogic(time);

	if (!exploding  && !isNetworkControlled)
	{
		// weight speed vs. time
		float weight = time * 25.0f;

		hasThrust = false;
		if (Input::CheckEvent(control[THRUST]) && fuel > 0)
		{
			hasThrust = true;
			speed += accel * weight;
			if (speed > maxSpeed) speed = maxAccel;

			// add particle
			D3DXVECTOR3 pos = GetPosition();
			pos.z = 8;
			exhaust->AddParticle(pos, baseOffset * 1.25f, angle + 180);

			// use up fuel to move
			fuel = fuel - (fuelUsage * weight * 0.25f);
			if (fuel < 0)
			{
				fuel = 0;
			}

			// make sound
			// make sound
			rt->GP_PlaySound(Runtime::THRUST, true);
		}

		doFire = false;
		if (Input::CheckEvent(control[FIRE_BULLET]) && bulletTime > BULLET_DELAY)
		{
			bulletTime = 0.0;
			Fire();
		}
		if (Input::CheckEvent(control[FIRE_MISSILE]) && !missile->GetInUse() && numMissiles > 0)
		{
			missileTargetId = 0;

			numMissiles--;
			Ship* ship = NULL;
			float closest = -1.0f;
			D3DXVECTOR3 pos1 = GetPosition();
			D3DXVECTOR3 pos2;
			for (int i=0; i < ships.size(); i++)
			{
				Ship* ship2 = ships[i];
				if (ship2 != this)
				{
					pos2 = ship2->GetPosition();
					float dx = pos1.x - pos2.x;
					float dy = pos1.y - pos2.y;
					float dz = pos1.z - pos2.z;
					float dist = dx * dx + dy * dy + dz * dz;
					if (dist < closest || closest == -1.0f)
					{
						missileTargetId = ship2->playerId;
						closest = dist;
						ship = ship2;
					}
				}
			}

			// fire the missile
			if (ship != NULL)
			{
				doFireMissile = true;
				missile->Fire(GetPosition(), angle, baseOffset, speed, ship); 
				rt->GP_PlaySound(Runtime::FIRE_MISSILE, false);
			}
		}
		if (Input::CheckEvent(control[LEFT]))
		{
			if (!hasLanded)
			{
				angle += rotationalVelocity * weight * 2.0f;
				if (angle > 360) angle -= 360;
			}
		}
		if (Input::CheckEvent(control[RIGHT]))
		{
			if (!hasLanded)
			{
				angle -= rotationalVelocity * weight * 2.0f;
				if (angle < 0) angle += 360;
			}
		}

		if (speed > 0)
		{
			speed = speed * speedDeccel * weight;
			hasLanded = false;
		}

		Runtime* rt = BaseApp::Get()->GetCurrentRuntime();

		// are we under water?
		D3DXVECTOR3 myPos = GetPosition();
		bool underWater = false;
		std::vector<Water*> waters = rt->GetWater();
		std::vector<Water*>::iterator pos = waters.begin();
		while (pos != waters.end())
		{
			Water* water = *pos;
			if (water->Inside(myPos))
			{
				underWater = true;
				break;
			}
			pos++;
		}

		// make the sound?
		doWaterSound = false;
		if (underWater && !isUnderwater)
		{
			isUnderwater = true;
			doWaterSound = true;
			rt->GP_PlaySound(Runtime::UNDERWATER, false);
		}
		else if (!underWater)
		{
			isUnderwater = false;
		}

		// get gravity - and reverse it under water
		float gravity = 0;
		ModelMap* mm = rt->GetModelMap();
		if (mm != NULL)
		{
			gravity = mm->GetGravity();
			if (underWater)
			{
				gravity = -gravity;
			}
		}

		if (!hasLanded)
		{
			if (hasThrust)
			{
				xSpeed += cosf((angle+90) * System::deg2rad) * speed * weight * 50.0f;
				ySpeed += ((sinf((angle+90) * System::deg2rad) * speed) * weight * 50.0f - (gravity * weight));
			}
			else
			{
				xSpeed = xSpeed * 0.99f;
				ySpeed -= (gravity * weight);
			}
			// incur slowdown if changing from air to water
			if (doWaterSound)
			{
				xSpeed = xSpeed * 0.5f;
				ySpeed = ySpeed * 0.5f;
			}

			if (xSpeed > maxSpeed) xSpeed = maxSpeed;
			if (xSpeed < -maxSpeed) xSpeed = -maxSpeed;

			if (ySpeed > maxSpeed) ySpeed = maxSpeed;
			if (ySpeed < -maxSpeed) ySpeed = -maxSpeed;

			// if we collide with another ship - we get pushed off
			D3DXVECTOR3 delta;
			for (int i=0; i < ships.size(); i++)
			{
				if (GetOid() != ships[i]->GetOid())
				{
					delta = GetPosition() - ships[i]->GetPosition();
					float dist = delta.x * delta.x + delta.y * delta.y;
					float dist2 = baseOffset*baseOffset;
					if (dist < dist2)
					{
						float force = 1; // - (dist / dist2);
						xSpeed = -xSpeed * force;
						ySpeed = -ySpeed * force;

						// if other ships hasn't landed - then reverse it too
						if (!ships[i]->hasLanded)
						{
							ships[i]->xSpeed = -xSpeed * force;
							ships[i]->ySpeed = -ySpeed * force;
						}
					}
				}
			}
		}
		else
		{
			// start recharging fuel, ammo, and shields
			if (fuel < 1)
			{
				fuel = fuel + 0.005f * weight;
				if (fuel > 1) fuel = 1;
			}
			else if (shield < 1)
			{
				shield = shield + 0.005f * weight;
				if (shield > 1) shield = 1;
			}
			else if (numBullets < maxBulletLoad)
			{
				numBullets += 5;
				if (numBullets > maxBulletLoad)
					numBullets = maxBulletLoad;
			}
			else if (numMissiles < maxMissiles)
			{
				numMissiles++;
			}
		}

		myPos.x += xSpeed;
		myPos.y += ySpeed;

		SetPosition(myPos);

		SetRotationEuler(D3DXVECTOR3(0,0,angle));

		// or do we land?
		if (!hasThrust && !hasLanded)
		{
			D3DXVECTOR3 aabbmin, aabbmax;
			std::vector<Base*> base = rt->GetBases();
			for (int i=0; i < base.size(); i++)
			{
				// both aabb are zero
				base[i]->GetTransformedAABB(aabbmin, aabbmax);

				// ship must be positioned correctly and be at right speed
				if (fabsf(xSpeed) < landSpeed && fabsf(ySpeed) < landSpeed && 
					(angle >= (360-landAngle) || angle <= landAngle))
				{
					float dist = (myPos.y - (aabbmax.y+baseOffset));

					// then it must be just above the base
					if (aabbmin.x < myPos.x && myPos.x < aabbmax.x && dist > 0 && dist < landHeight)
					{
						// make sound
						rt->GP_PlaySound(Runtime::LAND_SHIP, false);

						hasLanded = true;
						myPos.y = aabbmax.y + baseOffset;
						xSpeed = 0;
						ySpeed = 0;
						speed = 0;
						angle = 0;
						SetPosition(myPos);
					}
				}
			}
		}

		// check damage
		CheckShipToShipCollision();
		CheckCaveCollision();
	}
	else if (exploding)
	{
		if (explosion->Finished())
		{
			speed = 0;
			xSpeed = 0;
			ySpeed = 0;
			hasLanded = true;
			angle = 0;
			shield = 1;
			fuel = 1;
			numBullets = maxBulletLoad;
			exploding = false;

			SetPosition(initialPosition);
		}
		explosion->EventLogic(time);
	}

	if (!exploding)
	{
		// update shield timing
		if (shieldHitCount > 0)
		{
			shieldHitCount--;
		}
		// shield animation
		shieldModel->EventLogic(time);
	}
}

void Ship::Fire()
{
	if (numBullets > 0)
	{
		numBullets--;
		DoFire();
	}
}

void Ship::DoFire()
{
	// find first available bullet
	int use = -1;
	for (int i=0; i < maxActiveBullets; i++)
	{
		if (!bullets[i].GetInUse())
		{
			use = i;
			break;
		}
	}
	// do we have one?
	if (use != -1)
	{
		bullets[use].Fire(GetPosition(), baseOffset, angle);

		// make sound
		Runtime* rt = BaseApp::Get()->GetCurrentRuntime();
		rt->GP_PlaySound(Runtime::FIRE_LASER, false);
		doFire = true;
	}
}

void Ship::DisplayStatus(float x, float y)
{
	Runtime* rt = BaseApp::Get()->GetCurrentRuntime();

	D3DXCOLOR yellow = D3DXCOLOR(0.6f,0.8f,0,1);
	D3DXCOLOR red = D3DXCOLOR(0.2f,0.1f,0,1);
	D3DXCOLOR fontBackColour = D3DXCOLOR(0,0,0,1);
	D3DXCOLOR fontForeColour = D3DXCOLOR(1,1,1,1);
	D3DXVECTOR2 leftTop, rightBottom;

	Device* dev = Interface::GetDevice();
	dev->SetWorldTransform(System::GetIdentityMatrix());
	Font* font = dev->GetLargeFont();

	// fill background
	dev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	dev->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	dev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	dev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	dev->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_SELECTARG1);
	dev->SetTextureStageState(0,D3DTSS_ALPHAARG1, D3DTA_TFACTOR);
	dev->FillRect(D3DXVECTOR2(x-10,y+70), D3DXVECTOR2(x+100,y-10), D3DXCOLOR(0.1f,0.1f,0.1f,0.4f));

	dev->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_SELECTARG1);
	dev->SetTextureStageState(0,D3DTSS_ALPHAARG1, D3DTA_TEXTURE);

	leftTop = D3DXVECTOR2(x,y);
	rightBottom = D3DXVECTOR2(leftTop.x+16,leftTop.y+16);
	dev->FillRect(leftTop,rightBottom,fontForeColour,icons[ICON_SHIELD]);

	leftTop = D3DXVECTOR2(x,y+18);
	rightBottom = D3DXVECTOR2(leftTop.x+16,leftTop.y+16);
	dev->FillRect(leftTop,rightBottom,fontForeColour,icons[ICON_BULLET]);

	leftTop = D3DXVECTOR2(x,y+36);
	rightBottom = D3DXVECTOR2(leftTop.x+16,leftTop.y+16);
	dev->FillRect(leftTop,rightBottom,fontForeColour,icons[ICON_FUEL]);

	dev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	// display progress bars
	dev->DrawDisplayBar(x+20, y+4, shield, 1, yellow, red, 75, 6);
	dev->DrawDisplayBar(x+20, y+22, (float)numBullets, (float)maxBulletLoad, yellow, red, 75, 6);
	dev->DrawDisplayBar(x+20, y+40, fuel, 1, yellow, red, 75, 6);

	std::string scoreStr = System::Int2Str(myScore);
	font->Write(x + 120, y + 4, scoreStr, fontForeColour);

	float xpos = 1.0f / (float)maxMissiles;
	float yp = y + 64;
	for (int i=0; i < numMissiles; i++)
	{
		float xp = x + 20 + (float)i * 3;
		dev->FillRect(D3DXVECTOR2(xp,yp), D3DXVECTOR2(xp+2,yp-6), fontForeColour);
	}
}

void Ship::EventInit()
{
	hasLanded = true;
	initialPosition = GetPosition();
}

void Ship::CheckBulletCollision(Bullet* bullet)
{
	D3DXVECTOR3 pos = bullet->GetPosition() - GetPosition();
	float dist = pos.x * pos.x + pos.y * pos.y;
	if (!exploding && dist < baseOffset*baseOffset)
	{
		bullet->SetInUse(false);
		shieldHitCount = Ship::SHIELD_ANIM_COUNT;
		DecreaseShields(Ship::BULLET_HIT_DAMAGE * shieldStrength);
	}
	// does the bullet hit a missile?
	if (missile->GetInUse() && !missile->GetExploding())
	{
		pos = bullet->GetPosition() - missile->GetPosition();
		float dist = pos.x * pos.x + pos.y * pos.y;
		if (dist < Ship::BULLET_MISSILE_HIT_DISTANCE)
		{
			bullet->SetInUse(false);
			missile->Explode();
		}
	}
}

void Ship::CheckShipToShipCollision()
{
	Runtime* rt = BaseApp::Get()->GetCurrentRuntime();
	std::vector<Ship*> ships = rt->GetShips();

	// check all my bullets against each ship
	for (int i=0; i < maxActiveBullets; i++)
	{
		for (int j=0; j < ships.size(); j++)
		{
			if (bullets[i].GetInUse())
			{
				ships[j]->CheckBulletCollision(&bullets[i]);
			}
		}
	}
}

void Ship::CheckCaveCollision()
{
	Runtime* rt = BaseApp::Get()->GetCurrentRuntime();
	ModelMap* mm = rt->GetModelMap();
	if (mm != NULL)
	{
		D3DXVECTOR3 orgpos = GetPosition();
		D3DXVECTOR3 pos;
		for (int i=0; i < 350; i += 20)
		{
			pos = System::OffsetVector(orgpos, (float)i, baseOffset * 0.8f);
			if (mm->CheckBulletCollision(pos))
			{
				shieldHitCount = Ship::SHIELD_ANIM_COUNT;
				DecreaseShields(Ship::WALL_HIT_DAMAGE * shieldStrength);
				xSpeed *= 0.9f;
				ySpeed *= 0.9f;
			}

		}
	}
}

void Ship::DecreaseShields(float damage)
{
	shield = shield - damage;

	// explode?
	if (shield <= 0 && !exploding)
	{
		shieldHitCount = 0;
		shield = 0;
		explosion->Start();
		exploding = true;
		startExplosion = true;

		// make sound
		Runtime* rt = BaseApp::Get()->GetCurrentRuntime();
		rt->GP_PlaySound(Runtime::EXPLODE, false);
		// make sure I get penalised
		rt->GP_UpdateScore(this);
	}
	if (shield < 0)
	{
		shield = 0;
	}
}

void Ship::SetInitialPosition(const D3DXVECTOR3& _initialPosition)
{
	initialPosition = _initialPosition;
}

void Ship::Draw(double time)
{
	model->SetPosition(GetPosition());
	model->SetRotationQuat(GetRotationQuat());
	model->SetScale(GetScale());
	model->Draw(time);
}

void Ship::Draw(double time, const D3DXVECTOR3& _pos)
{
	// draw my missile if applicable
	missile->Draw(time, _pos);

	if (exploding)
	{
		explosion->Draw(GetPosition() - _pos, baseOffset * 2);
	}
	else
	{
		model->SetPosition(GetPosition() - _pos);
		model->SetRotationQuat(GetRotationQuat());
		model->SetScale(GetScale());
		model->Draw(time);

		// draw shields if being hit
		if (shieldHitCount > 0)
		{
			shieldModel->SetPosition(GetPosition() - _pos);
			shieldModel->SetRotationQuat(GetRotationQuat());
			shieldModel->SetScale(GetScale());
			shieldModel->Draw(time);
		}
	}

	// draw all bullets
	for (int i=0; i < maxActiveBullets; i++)
	{
		if (bullets[i].GetInUse())
		{
			bullets[i].Draw(time, bulletModel, _pos);
		}
	}

	// draw exhaust
	exhaust->Draw(time, _pos);
}

void Ship::UpdateBoundingBox()
{
	if (model != NULL)
		model->UpdateBoundingBox();
	if (bulletModel != NULL)
		bulletModel->UpdateBoundingBox();
}

BoundingBox* Ship::GetBoundingBox()
{
	if (model != NULL)
		return model->GetBoundingBox();
	return NULL;
}

void Ship::Read(XmlNode* node)
{
	XmlNode::CheckVersion(node, shipSignature, shipVersion);
	WorldObject::Read(node->GetChild(WorldObject::Signature()));

	safe_delete(model);
	safe_delete(bulletModel);

	XmlNode* f1 = node->GetChild("shipModel");
	XmlNode::CheckVersion(f1, "shipModel", 1);

	model = new Model();
	model->Read(f1);

	XmlNode* f2 = node->GetChild("bulletModel");
	XmlNode::CheckVersion(f2, "bulletModel", 1);
	bulletModel = new Model();
	bulletModel->Read(f2);

	node->Read("maxAccel", maxAccel);
	node->Read("maxSpeed", maxSpeed);
	node->Read("accel", accel);
	node->Read("rotationalVelocity", rotationalVelocity);
	node->Read("baseOffset", baseOffset);

	node->Read("maxBulletLoad", maxBulletLoad);
	node->Read("maxActiveBullets", maxActiveBullets);
	node->Read("bulletTTL", bulletTTL);
	node->Read("bulletSpeed", bulletSpeed);
	node->Read("dropOffFactor", dropOffFactor);
	node->Read("maxMissiles", maxMissiles);

	SetMaxActiveBullets(maxActiveBullets);

	// re-load position
	initialPosition = GetPosition();
	UpdateBoundingBox();
}

XmlNode* Ship::Write()
{
	XmlNode* node = XmlNode::NewChild(shipSignature, shipVersion);
	node->Add(WorldObject::Write());

	XmlNode* f1 = XmlNode::NewChild("shipModel",1);
	f1->Add(model->Write());
	node->Add(f1);

	XmlNode* f2 = XmlNode::NewChild("bulletModel",1);
	f2->Add(bulletModel->Write());
	node->Add(f2);

	node->Write("maxAccel", maxAccel);
	node->Write("maxSpeed", maxSpeed);
	node->Write("accel", accel);
	node->Write("rotationalVelocity", rotationalVelocity);
	node->Write("baseOffset", baseOffset);

	node->Write("maxBulletLoad", maxBulletLoad);
	node->Write("maxActiveBullets", maxActiveBullets);
	node->Write("bulletTTL", bulletTTL);
	node->Write("bulletSpeed", bulletSpeed);
	node->Write("dropOffFactor", dropOffFactor);
	node->Write("maxMissiles", maxMissiles);

	return node;
}

