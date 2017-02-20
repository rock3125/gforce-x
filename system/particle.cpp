#include "standardFirst.h"

#include "system/particle.h"

/////////////////////////////////////////////////////////

Particle::Particle()
{
	ttl = 0;
	startSize = 10;
	endSize = 10;
	position = D3DXVECTOR3(0,0,0);
	dirn = D3DXVECTOR3(0,0,0);
}

Particle::~Particle()
{
}

Particle::Particle(const Particle& p)
{
	operator=(p);
}

const Particle& Particle::operator=(const Particle& p)
{
	ttl = p.ttl;
	position = p.position;
	dirn = p.dirn;
	startSize = p.startSize;
	endSize = p.endSize;

	return *this;
}

void Particle::Set(float _ttl, float _startSize, float _endSize, const D3DXVECTOR3& _pos, const D3DXVECTOR3& _dirn)
{
	ttl = _ttl;
	position = _pos;
	dirn = _dirn;
	dirn.z = 0;
	startSize = _startSize;
	endSize = _endSize;
}

const D3DXVECTOR3& Particle::GetPosition()
{
	return position;
}

void Particle::SetPosition(const D3DXVECTOR3& _position)
{
	position = _position;
}

float Particle::GetStartSize()
{
	return startSize;
}

void Particle::SetStartSize(float _startSize)
{
	startSize = _startSize;
}

float Particle::GetEndSize()
{
	return endSize;
}

void Particle::SetEndSize(float _endSize)
{
	endSize = _endSize;
}

float Particle::GetTTL()
{
	return ttl;
}

void Particle::SetTTL(float _ttl)
{
	ttl = _ttl;
}

const D3DXVECTOR3& Particle::GetDirn()
{
	return dirn;
}

void Particle::SetDirn(const D3DXVECTOR3& _dirn)
{
	dirn = _dirn;
	dirn.z = 0;
}


