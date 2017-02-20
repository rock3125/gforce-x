#pragma once

////////////////////////////////////////////////////////////

class Particle
{
public:
	Particle();
	~Particle();
	Particle(const Particle&);
	const Particle& operator=(const Particle&);

	// set particle particulars
	void Set(float ttl, float startSize, float endSize, const D3DXVECTOR3& pos, const D3DXVECTOR3& dirn);

	// get particle position
	const D3DXVECTOR3& GetPosition();
	void SetPosition(const D3DXVECTOR3&);

	// get particle position
	const D3DXVECTOR3& GetDirn();
	void SetDirn(const D3DXVECTOR3&);

	// get particle size
	float GetStartSize();
	void SetStartSize(float);

	float GetEndSize();
	void SetEndSize(float);

	// get ttl for particle
	float GetTTL();
	void SetTTL(float);

private:
	float			ttl;
	D3DXVECTOR3		position;
	D3DXVECTOR3		dirn;
	float			startSize;
	float			endSize;
};

