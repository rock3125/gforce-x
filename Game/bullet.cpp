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
#include "system/model/model.h"

#include "game/bullet.h"
#include "game/modelMap.h"

#include "system/model/model.h"

///////////////////////////////////////////////////////////

Bullet::Bullet()
{
	speed = 2;
	inUse = false;
	ttl = 0;
	angle = 0;
	dropOffFactor = 4;
	initialTTL = 250;
}

Bullet::~Bullet()
{
}

Bullet::Bullet(const Bullet& b)
{
	operator=(b);
}

const Bullet& Bullet::operator=(const Bullet& b)
{
	speed = b.speed;
	inUse = b.inUse;
	ttl = b.ttl;
	angle = b.angle;
	initialTTL = b.initialTTL;
	dropOffFactor = b.dropOffFactor;

	return *this;
}

void Bullet::SetParameters(float _initialTTL, float _speed, float _dropOffFactor)
{
	initialTTL = _initialTTL;
	speed = _speed;
	dropOffFactor = _dropOffFactor;
}

bool Bullet::GetInUse()
{
	return inUse;
}

void Bullet::SetInUse(bool _inUse)
{
	inUse = _inUse;
}

void Bullet::Fire(const D3DXVECTOR3& pos, float baseOffset, float _angle)
{
	SetPosition(System::OffsetVector(pos, _angle, baseOffset * 1.25f));
	inUse = true;
	ttl = initialTTL;
	angle = _angle;
}

void Bullet::Draw(double time, Model* model, const D3DXVECTOR3& _pos)
{
	float weight = 1;

	Runtime* rt = BaseApp::GetApp().GetCurrentRuntime();
	ModelMap* mm = rt->GetModelMap();
	if (mm != NULL && model != NULL && inUse)
	{
		model->SetPosition(GetPosition() - _pos);
		model->SetRotationQuat(GetRotationQuat());
		model->SetScale(GetScale());
		model->Draw(0);

		float delta = (float)(initialTTL - ttl);
		float gravity = mm->GetGravity();
		float xdirn = (cosf((angle+90) * System::deg2rad) * speed);
		float ydirn = (sinf((angle+90) * System::deg2rad) * speed) - gravity * delta * dropOffFactor;

		D3DXVECTOR3 pos = GetPosition();
		D3DXVECTOR3 pos1 = pos;
		D3DXVECTOR3 pos2;
		pos.x += xdirn * weight;
		pos.y += ydirn * weight;
		SetPosition(pos);

		// see if we collide with the map or not
		float dist = sqrtf(xdirn*xdirn + ydirn*ydirn);
		float p = 0;
		bool found = false;
		while (p < dist && !found)
		{
			float scale = (p/dist);
			pos2.x = pos1.x + (pos.x - pos1.x) * scale;
			pos2.y = pos1.y + (pos.y - pos1.y) * scale;
			found = mm->CheckBulletCollision(pos);
			p = p + 2.0f;
		}
		inUse = !found;
	}
}

void Bullet::EventLogic(double time)
{
	float weight = 0.1f;

	if (inUse)
	{
		ttl = max(0, ttl - weight);
		inUse = ttl > 0;
	}
}

