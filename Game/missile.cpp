//////////////////////////////////////////////////////////////////////
/// GRAVITY FORCE X
///
/// (c)Copyright Peter de Vocht, 2006
///
/// I'm releasing this code under GPL v2.1 (see included file lgpl.txt)
///
//////////////////////////////////////////////////////////////////////

#include "standardFirst.h"

#include "system/model/model.h"
#include "system/particleSource.h"
#include "runtime/runtime.h"
#include "system/BaseApp.h"

#include "game/ship.h"
#include "game/missile.h"
#include "game/commonModels.h"
#include "game/modelMap.h"

// setup constants
float Missile::INIT_ACCEL = 0.1f;
float Missile::INIT_FUEL = 5;
float Missile::FUEL_USAGE = 0.01f;
float Missile::ANGULAR_SPEED = 4;
float Missile::MAX_SPEED = 1.5f;
float Missile::LENGTH = 6.0f;
float Missile::INFLUENCE_RANGE = 20;
float Missile::MISSILE_FORCE = 2;

///////////////////////////////////////////////////////////

Missile::Missile()
	: exhaust(NULL)
{
	speed = 0;
	fuel = Missile::INIT_FUEL;
	angle = 0;
	inUse = false;
	exploding = false;

	exhaust = new ParticleSource();
	exhaust->SetColourRange(D3DXCOLOR(0.2f,0.2f,1,1), D3DXCOLOR(1,1,0,1));
	exhaust->SetMaxParticles(20);
	exhaust->SetTimeToLive(exhaust->GetTimeToLive() * 0.4f);
	exhaust->SetStartSize(2);
	exhaust->SetEndSize(5);

	explosion = new ParticleSource();
}

Missile::~Missile()
{
	safe_delete(exhaust);
}

Missile::Missile(const Missile& m)
	: exhaust(NULL)
{
	operator=(m);
}

const Missile& Missile::operator=(const Missile& m)
{
	WorldObject::operator=(m);

	speed = m.speed;
	angle = m.angle;
	target = m.target;
	fuel = m.fuel;
	inUse = m.inUse;
	exploding = m.exploding;

	return *this;
}

void Missile::Explode()
{
	exhaust->ClearParticles();
	D3DXVECTOR3 pos = GetPosition();
	pos.z = 0;
	explosion->SetupDoughnut(pos, Missile::LENGTH * 0.5f, 
							 1, 3, 15, 
							 D3DXCOLOR(1,1,0.2f,1), D3DXCOLOR(1,0.2f,0,1));
	exploding = true;

	// push ships away from the missile as if by force
	Runtime* rt = BaseApp::GetApp().GetCurrentRuntime();
	std::vector<Ship*> ships = rt->GetShips();
	D3DXVECTOR3 shipPos, dirn;
	shipPos.z = 0;
	pos.z = 0;
	for (int i=0; i < ships.size(); i++)
	{
		shipPos = ships[i]->GetPosition();
		float dx = shipPos.x - pos.x;
		float dy = shipPos.y - pos.y;
		float dist = dx * dx + dy * dy;
		float rangeSq = Missile::INFLUENCE_RANGE *  Missile::INFLUENCE_RANGE;
		if (dist < rangeSq)
		{
			float force = 1 - (dist / rangeSq);
			dirn.x = dx;
			dirn.y = dy;
			dirn.z = 0;
			D3DXVec3Normalize(&dirn, &dirn);
			ships[i]->Push(dirn.x, dirn.y, force * Missile::MISSILE_FORCE);
		}
	}
}

void Missile::Fire(D3DXVECTOR3 startPos, float _angle, float baseOffset, 
				   float _speed, WorldObject* _target)
{
	angle = _angle;
	target = _target;
	speed = _speed;
	fuel = Missile::INIT_FUEL;

	startPos = System::OffsetVector(startPos, angle, baseOffset);
	SetPosition(startPos);

	inUse = true;
}

void Missile::SetTarget(WorldObject* _target)
{
	target = _target;
}

WorldObject* Missile::GetTarget()
{
	return target;
}

void Missile::SetAngle(float _angle)
{
	angle = _angle;
}

float Missile::GetAngle()
{
	return angle;
}

bool Missile::Collision()
{
	return CollisionMissleWithWall() || CollisionMissileWithShip();
}

bool Missile::CollisionMissleWithWall()
{
	// check tip of missile or bottom of missile hitting wall(s)
	D3DXVECTOR3 top, bottom;
	bottom = GetPosition();
	top = System::OffsetVector(bottom, angle, Missile::LENGTH);
	Runtime* rt = BaseApp::GetApp().GetCurrentRuntime();
	ModelMap* mm = rt->GetModelMap();
	if (mm != NULL)
	{
		if (mm->CheckBulletCollision(bottom) ||
			mm->CheckBulletCollision(top))
		{
			return true;
		}
	}
	return false;
}

bool Missile::CollisionMissileWithShip()
{
	D3DXVECTOR3 top, bottom;
	bottom = GetPosition();
	top = System::OffsetVector(bottom, angle, Missile::LENGTH);
	Runtime* rt = BaseApp::GetApp().GetCurrentRuntime();
	std::vector<Ship*> ships = rt->GetShips();

	for (int i=0; i < ships.size(); i++)
	{
		// ignore z
		D3DXVECTOR3 pos = ships[i]->GetPosition();
		float baseOffset = ships[i]->GetBaseOffset();
		float dx = top.x - pos.x;
		float dy = top.y - pos.y;
		float dist = dx * dx + dy * dy;
		if (dist < (baseOffset*baseOffset))
		{
			return true;
		}
	}
	return false;
}

bool Missile::GetExploding()
{
	return exploding;
}

bool Missile::GetInUse()
{
	return inUse;
}

void Missile::DoMapDamage()
{
	Runtime* rt = BaseApp::GetApp().GetCurrentRuntime();
	ModelMap* map = rt->GetModelMap();
	if (map != NULL)
	{
		map->CalculateMissileImpact(GetPosition(), Missile::INFLUENCE_RANGE);
	}
}

void Missile::EventLogic(double time)
{
	if (inUse)
	{
		if (exploding)
		{
			// finished?
			if (!explosion->GetParticleInUse(0))
			{
				exploding = false;
				inUse = false;
				DoMapDamage();
			}
			else
			{
				explosion->EventLogic(time);
			}
		}
		else
		{
			// point towards target target
			D3DXVECTOR3 pos = GetPosition();
			D3DXVECTOR3 targetPos = pos;
			if (target != NULL)
			{
				targetPos = target->GetPosition();
			}
			D3DXVECTOR3 vec = targetPos - pos;
			D3DXVECTOR3 vec2(0, 1, 0);
			vec.z = 0; // ignore depth in 2d
			D3DXVec3Normalize(&vec, &vec);
			float dotp = D3DXVec3Dot(&vec, &vec2);
			float resAngle = acosf(dotp) * System::rad2deg;

			resAngle = vec.x > 0 ? 360 - resAngle : resAngle;
			float dist = fabsf(resAngle - angle);

			if (dist > Missile::ANGULAR_SPEED)
			{
				if (dist > 180)
				{
					if (resAngle > angle)
					{
						angle -= Missile::ANGULAR_SPEED;
						if (angle < 0) angle += 360;
					}
					else if (resAngle < angle)
					{
						angle += Missile::ANGULAR_SPEED;
						if (angle > 360) angle -= 360;
					}
				}
				else
				{
					if (resAngle > angle)
					{
						angle += Missile::ANGULAR_SPEED;
						if (angle > 360) angle -= 360;
					}
					else if (resAngle < angle)
					{
						angle -= Missile::ANGULAR_SPEED;
						if (angle < 0) angle += 360;
					}
				}
			}
			else
			{
				angle = resAngle;
			}

			// get missile up to speed
			if (speed < Missile::MAX_SPEED)
			{
				speed += INIT_ACCEL;
			}

			// move the missile and set its angle
			pos = System::OffsetVector(pos, angle, speed);
			SetPosition(pos);

			SetRotationEuler(D3DXVECTOR3(0,0,angle));

			// decrease fuel
			fuel = fuel - Missile::FUEL_USAGE;
			if (fuel <= 0)
			{
				fuel = 0;
				Explode();
			}
			exhaust->EventLogic(time);

			// check for collision - and act
			if (Collision())
			{
				Explode();
			}
		}
	}
}

void Missile::Draw(double time, const D3DXVECTOR3& _pos)
{
	if (inUse)
	{
		if (exploding)
		{
			explosion->Draw(time, _pos);
		}
		else
		{
			Model* model = CommonModels::Get()->GetMissile();
			if (model != NULL)
			{
				model->SetPosition(GetPosition() - _pos);
				model->SetRotationQuat(GetRotationQuat());
				model->Draw(time);
			}
			D3DXVECTOR3 pos = GetPosition();
			pos.z = 8;
			exhaust->AddParticle(pos, 0, angle + 180);
			exhaust->Draw(time, _pos);
		}
	}
}

