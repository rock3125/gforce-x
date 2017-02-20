//////////////////////////////////////////////////////////////////////
/// GRAVITY FORCE X
///
/// (c)Copyright Peter de Vocht, 2006
///
/// I'm releasing this code under GPL v2.1 (see included file lgpl.txt)
///
//////////////////////////////////////////////////////////////////////

#include "standardFirst.h"

#include "system/baseApp.h"
#include "system/model/model.h"
#include "game/ship.h"
#include "game/bullet.h"

#include "runtime/runtime.h"

#include "game/Turret.h"

std::string Turret::turretSignature="turret";
int Turret::turretVersion=1;

///////////////////////////////////////////////////////////

Turret::Turret()
	: model(NULL)
	, bullets(NULL)
	, bulletModel(NULL)
{
	SetType(OT_TURRET);
	SetWorldType(TYPE_TURRET);

	model = new Model();
	InitVars();
}

Turret::~Turret()
{
	safe_delete(model);
	safe_delete_array(bullets);
}

Turret::Turret(const Turret& i)
	: model(NULL)
	, bullets(NULL)
	, bulletModel(NULL)
{
	model = new Model();
	InitVars();

	operator=(i);
}

const Turret& Turret::operator=(const Turret& i)
{
	WorldObject::operator =(i);
	return *this;
}

void Turret::InitVars()
{
	// ammo
	numBullets = 10000;
	maxActiveBullets = 20;
	bulletTTL = 100;
	bulletSpeed = 1.5f;
	dropOffFactor = 4;
	SetMaxActiveBullets(maxActiveBullets);

	// delay system

	fireDelay = 25;
	fireCounter = 0;

	// turret reaction distance
	turretDist = 150.0f;
}

void Turret::SetMaxActiveBullets(int maxActiveBullets)
{
	this->maxActiveBullets = maxActiveBullets;

	safe_delete_array(bullets);
	bullets = new Bullet[maxActiveBullets];

	// set bullet parameters
	for (int i=0; i < maxActiveBullets; i++)
	{
		bullets[i].SetParameters((float)bulletTTL, (float)bulletSpeed, dropOffFactor);
	}
}

void Turret::Draw(double time)
{
	model->SetPosition(GetPosition());
	model->SetRotationQuat(GetRotationQuat());
	model->SetScale(GetScale());
	model->Draw(time);
}

void Turret::Draw(double time, const D3DXVECTOR3& pos)
{
	model->SetPosition(GetPosition());
	model->SetRotationQuat(GetRotationQuat());
	model->SetScale(GetScale());
	model->Draw(time, pos);

	// draw all bullets
	for (int i=0; i < maxActiveBullets; i++)
	{
		if (bullets[i].GetInUse())
		{
			bullets[i].Draw(time, bulletModel, pos);
		}
	}
}

void Turret::CheckShipCollision()
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

void Turret::EventLogic(double time)
{
	Runtime* rt = BaseApp::Get()->GetCurrentRuntime();

	// logic for bullets - always!
	for (int i=0; i < maxActiveBullets; i++)
	{
		if (bullets[i].GetInUse())
		{
			bullets[i].EventLogic(time);
		}
	}

	// check bullet havoc
	CheckShipCollision();

	D3DXVECTOR3 myPos = GetPosition();
	float rangeDist = (turretDist * turretDist);

	// get the ship closest to me
	std::vector<Ship*> ships = rt->GetShips();
	std::vector<Ship*>::iterator pos = ships.begin();
	Ship* closest = NULL;
	float closestRange = 0;
	while (pos != ships.end())
	{
		Ship* ship = *pos;
		D3DXVECTOR3 shipPos = ship->GetPosition();
		float dx = (shipPos.x - myPos.x);
		float dy = (shipPos.y - myPos.y);
		float dz = (shipPos.z - myPos.z);
		float dist = (dx * dx) + (dy * dy) + (dz * dz);

		if (dist < rangeDist)
		{
			if (closest == NULL)
			{
				closest = ship;
				closestRange = dist;
			}
			else
			{
				if (dist < closestRange)
				{
					closest = ship;
					closestRange = dist;
				}
			}
		}
		pos++;
	}

	// do we have a target?
	if (closest != NULL)
	{
		bulletModel = closest->GetBulletModel();

		D3DXVECTOR3 shipPos = closest->GetPosition();

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
				// eliminate z pos
				shipPos.z = 0;
				myPos.z = 0;

				D3DXVECTOR3 comparator = D3DXVECTOR3(0,1,0);

				D3DXVECTOR3 direction = shipPos - myPos;
				D3DXVec3Normalize(&direction, &direction);
				float angle = D3DXVec3Dot(&comparator, &direction);
				if (direction.x > 0)
				{
					angle = -acosf(angle);
				}
				else
				{
					angle = acosf(angle);
				}
				angle = (angle * System::rad2deg);
				bullets[use].Fire(GetPosition(), 0, angle);

				// make sound
				rt->GP_PlaySound(Runtime::FIRE_LASER, false);
			}
		}
	}
}

void Turret::UpdateBoundingBox()
{
	BoundingBox* bb = model->GetBoundingBox();
	this->GetBoundingBox()->SetMax(bb->GetTransformedMax());
	this->GetBoundingBox()->SetMin(bb->GetTransformedMin());
}

void Turret::Read(XmlNode* node)
{
	XmlNode::CheckVersion(node, turretSignature, turretVersion);
	WorldObject::Read(node->GetChild(WorldObject::Signature()));
	model->Read(node->GetChild(model->Signature()));
	UpdateBoundingBox();
}

XmlNode* Turret::Write()
{
	XmlNode* node = XmlNode::NewChild(turretSignature, turretVersion);
	node->Add(WorldObject::Write());
	node->Add(model->Write());
	return node;
}

