#pragma once

#include "system/Particle.h"

class Texture;

////////////////////////////////////////////////////////////

class ParticleSource
{
	ParticleSource(const ParticleSource&);
	const ParticleSource& operator=(const ParticleSource&);
public:
	ParticleSource();
	~ParticleSource();

	// access particle start and end size
	float GetStartSize();
	void SetStartSize(float startSize);

	float GetEndSize();
	void SetEndSize(float endSize);

	// remove all residual particles
	void ClearParticles();

	// set source colour range
	void SetColourRange(const D3DXCOLOR& startColour, const D3DXCOLOR& endColour);

	// set time to live for particles
	void SetTimeToLive(float timeToLive);
	float GetTimeToLive();

	// add a new particle to the source
	void AddParticle(D3DXVECTOR3 pos, float offset, float angle);

	// set how many particles can exist maximum
	void SetMaxParticles(int maxParticles);

	// draw the particle source
	void Draw(double time, const D3DXVECTOR3& pos);

	// returns true if particle[index] is still in use (i.e. has a ttl > 0)
	bool GetParticleInUse(int index);

	// do events / logic
	void EventLogic(double time);

	// setup outward expanding doughnut
	void SetupDoughnut(D3DXVECTOR3 position, float initRadius, float speed, 
					   float startSize, float endSize, 
					   D3DXCOLOR startColour, D3DXCOLOR endColour);

private:
	Texture*				particleTexture;
	Particle*				particle;
	D3DXCOLOR				startColour;
	D3DXCOLOR				endColour;
	int						maxParticles;
	float					timeToLive;
	float					startSize;
	float					endSize;

	// for blitting the particles all in one go
	D3DPCOLOURVERTEX*		vertices;
	WORD*					indices;

	// constants

	// ttl in seconds
	static float			INIT_TIME_TO_LIVE;
	static int				INIT_MAX_PARTICLES;
	static float			INIT_PARTICLE_START_SIZE;
	static float			INIT_PARTICLE_END_SIZE;
	static float			SOURCE_DEPTH;
};

