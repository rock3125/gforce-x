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

#include "game/ship.h"
#include "game/modelMap.h"
#include "game/base.h"
#include "game/bullet.h"
#include "game/shield.h"
#include "game/explosion.h"
#include "game/missile.h"

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

void Ship::SetupKeyboard()
{
	control[THRUST] = new Input::Event(Input::KEYBOARD, Input::KEY, 0, BaseApp::GetApp().Down());
	control[LEFT]   = new Input::Event(Input::KEYBOARD, Input::KEY, 0, BaseApp::GetApp().Left());
	control[RIGHT]  = new Input::Event(Input::KEYBOARD, Input::KEY, 0, BaseApp::GetApp().Right());
	control[FIRE_BULLET]  = new Input::Event(Input::KEYBOARD, Input::KEY, 0, BaseApp::GetApp().Up());
   	control[FIRE_MISSILE] = new Input::Event(Input::KEYBOARD, Input::KEY, 0, BaseApp::GetApp().Fire1());
}

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

	// ammo
	maxBulletLoad = 1000;
	numBullets = maxBulletLoad;
	maxMissiles = 5;
	numMissiles = maxMissiles;
	maxActiveBullets = 20;
	bulletTTL = 100;
	bulletSpeed = 2;
	dropOffFactor = 4;
	fireDelay = 3;
	fireCounter = 0;
	SetMaxActiveBullets(maxActiveBullets);

	// shield
	shieldStrength = 0.1f;
	shield = 1;
	shieldHitCount = 0;

	exploding = false;
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
	DecreaseShields(force * shieldStrength);
}

void Ship::EventLogic(double time)
{
	Runtime* rt = BaseApp::GetApp().GetCurrentRuntime();
	std::vector<Ship*> ships = rt->GetShips();

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

	if (!exploding)
	{
		// weight speed vs. time
		float weight = min(1, (float)time * 25);

		if (Input::CheckEvent(control[THRUST]) && fuel > 0)
		{
			speed += accel * weight;
			if (speed > maxSpeed) speed = maxAccel;

			// add particle
			D3DXVECTOR3 pos = GetPosition();
			pos.z = 8;
			exhaust->AddParticle(pos, baseOffset * 1.25f, angle + 180);

			// use up fuel to move
			fuel = fuel - fuelUsage * weight;
			if (fuel < 0)
				fuel = 0;
		}
		if (Input::CheckEvent(control[FIRE_BULLET]))
		{
			Fire();
		}
		if (Input::CheckEvent(control[FIRE_MISSILE]) && !missile->GetInUse() && numMissiles > 0)
		{
			numMissiles--;
			int targetId = 0;
			if (ships.size() > 1)
			{
				if (ships[0] == this)
					targetId = 1;
			}
			missile->Fire(GetPosition(),angle,baseOffset,speed,ships[targetId]); 
		}
		if (Input::CheckEvent(control[LEFT]))
		{
			if (!hasLanded)
			{
				angle += rotationalVelocity * weight;
				if (angle > 360) angle -= 360;
			}
		}
		if (Input::CheckEvent(control[RIGHT]))
		{
			if (!hasLanded)
			{
				angle -= rotationalVelocity * weight;
				if (angle < 0) angle += 360;
			}
		}

		if (speed > 0)
		{
			speed = speed * speedDeccel;
			hasLanded = false;
		}

		float gravity = 0;
		Runtime* rt = BaseApp::GetApp().GetCurrentRuntime();
		ModelMap* mm = rt->GetModelMap();
		if (mm != NULL)
		{
			gravity = mm->GetGravity();
		}

		if (!hasLanded)
		{
			xSpeed += cosf((angle+90) * System::deg2rad) * speed;
			ySpeed += ((sinf((angle+90) * System::deg2rad) * speed) - gravity * weight);

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

		D3DXVECTOR3 pos = GetPosition();

		xSpeed = xSpeed * xSpeedDeccel;
		pos.x += xSpeed;
		pos.y += ySpeed;

		SetPosition(pos);

		SetRotationEuler(D3DXVECTOR3(0,0,angle));

		// or do we land?
		bool accelerating = Input::CheckEvent(control[THRUST]);
		if (!accelerating && !hasLanded)
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
					float dist = (pos.y - (aabbmax.y+baseOffset));

					// then it must be just above the base
					if (aabbmin.x < pos.x && pos.x < aabbmax.x && dist > 0 && dist < landHeight)
					{
						hasLanded = true;
						pos.y = aabbmax.y + baseOffset;
						xSpeed = 0;
						ySpeed = 0;
						speed = 0;
						angle = 0;
					}
				}
			}
		}

		// check damage
		CheckShipToShipCollision();
		CheckCaveCollision();

		// update shield timing
		if (shieldHitCount > 0)
		{
			shieldHitCount--;
		}

		// shield animation
		shieldModel->EventLogic(time);
	}
	else
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
}

void Ship::Fire()
{
	fireCounter++;
	if (numBullets > 0 && (fireCounter%fireDelay)==0 )
	{
		numBullets--;

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
		}
	}
}

void Ship::DisplayStatus(float x, float y)
{
	D3DXCOLOR yellow = D3DXCOLOR(0.6f,0.8f,0,1);
	D3DXCOLOR red = D3DXCOLOR(0.2f,0.1f,0,1);
	D3DXCOLOR fontBackColour = D3DXCOLOR(0,0,0,1);
	D3DXCOLOR fontForeColour = D3DXCOLOR(1,1,1,1);
	D3DXVECTOR2 leftTop, rightBottom;

	Device* dev = Interface::GetDevice();
	dev->SetWorldTransform(System::GetIdentityMatrix());
	Font* font = dev->GetFont();

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

void Ship::CheckShipToShipCollision()
{
	Runtime* rt = BaseApp::GetApp().GetCurrentRuntime();
	std::vector<Ship*> ships = rt->GetShips();

	// check all my bullets against each ship
	D3DXVECTOR3 pos;
	for (int i=0; i < maxActiveBullets; i++)
	{
		for (int j=0; j < ships.size(); j++)
		{
			if (bullets[i].GetInUse())
			{
				pos = bullets[i].GetPosition() - ships[j]->GetPosition();
				float dist = pos.x * pos.x + pos.y * pos.y;
				if (!exploding && dist < baseOffset*baseOffset)
				{
					bullets[i].SetInUse(false);
					ships[j]->shieldHitCount = Ship::SHIELD_ANIM_COUNT;
					ships[j]->DecreaseShields(Ship::BULLET_HIT_DAMAGE * shieldStrength);
				}
				// does the bullet hit a missile?
				Missile* missile = ships[j]->missile;
				if (missile->GetInUse() && !missile->GetExploding())
				{
					pos = bullets[i].GetPosition() - missile->GetPosition();
					float dist = pos.x * pos.x + pos.y * pos.y;
					if (dist < Ship::BULLET_MISSILE_HIT_DISTANCE)
					{
						bullets[i].SetInUse(false);
						missile->Explode();
					}
				}
			}
		}
	}
}

void Ship::CheckCaveCollision()
{
	Runtime* rt = BaseApp::GetApp().GetCurrentRuntime();
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
	if (shield <= 0)
	{
		shieldHitCount = 0;
		shield = 0;
		explosion->Start();
		exploding = true;
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

void Ship::Read(BaseStreamer& _f)
{
	BaseStreamer& f = _f.GetChild(shipSignature,shipVersion);
	WorldObject::Read(f);

	safe_delete(model);
	safe_delete(bulletModel);

	BaseStreamer& f1 = f.GetChild("shipModel",1);
	model = new Model();
	model->Read(f1);

	BaseStreamer& f2 = f.GetChild("bulletModel",1);
	bulletModel = new Model();
	bulletModel->Read(f2);

	f.Read("maxAccel", maxAccel);
	f.Read("maxSpeed", maxSpeed);
	f.Read("accel", accel);
	f.Read("rotationalVelocity", rotationalVelocity);
	f.Read("baseOffset", baseOffset);

	f.Read("maxBulletLoad", maxBulletLoad);
	f.Read("maxActiveBullets", maxActiveBullets);
	f.Read("bulletTTL", bulletTTL);
	f.Read("bulletSpeed", bulletSpeed);
	f.Read("dropOffFactor", dropOffFactor);
	f.Read("maxMissiles", maxMissiles);

	SetMaxActiveBullets(maxActiveBullets);

	// re-load position
	initialPosition = GetPosition();
	UpdateBoundingBox();
}

void Ship::Write(BaseStreamer& _f)
{
	BaseStreamer& f = _f.NewChild(shipSignature,shipVersion);
	WorldObject::Write(f);

	BaseStreamer& f1 = f.NewChild("shipModel",1);
	model->Write(f1);
	BaseStreamer& f2 = f.NewChild("bulletModel",1);
	bulletModel->Write(f2);

	f.Write("maxAccel", maxAccel);
	f.Write("maxSpeed", maxSpeed);
	f.Write("accel", accel);
	f.Write("rotationalVelocity", rotationalVelocity);
	f.Write("baseOffset", baseOffset);

	f.Write("maxBulletLoad", maxBulletLoad);
	f.Write("maxActiveBullets", maxActiveBullets);
	f.Write("bulletTTL", bulletTTL);
	f.Write("bulletSpeed", bulletSpeed);
	f.Write("dropOffFactor", dropOffFactor);
	f.Write("maxMissiles", maxMissiles);
}

