#include "standardFirst.h"

#include "system/ParticleSource.h"

float ParticleSource::INIT_TIME_TO_LIVE = 2.0f;
int ParticleSource::INIT_MAX_PARTICLES = 40;
float ParticleSource::INIT_PARTICLE_START_SIZE = 4;
float ParticleSource::INIT_PARTICLE_END_SIZE = 10;
float ParticleSource::SOURCE_DEPTH = 2.0f;

/////////////////////////////////////////////////////////

ParticleSource::ParticleSource()
	: particleTexture(NULL)
	, vertices(NULL)
	, indices(NULL)
{
	timeToLive = ParticleSource::INIT_TIME_TO_LIVE;
	maxParticles = ParticleSource::INIT_MAX_PARTICLES;

	particleTexture = TextureCache::GetTexture("objects\\particle.tga");
	SetColourRange(D3DXCOLOR(1,1,0.2f,1),D3DXCOLOR(1,0,0,0.5f));

	particle = new Particle[maxParticles];
	vertices = new D3DPCOLOURVERTEX[maxParticles*4];
	indices = new WORD[maxParticles*6];

	startSize = ParticleSource::INIT_PARTICLE_START_SIZE;
	endSize = ParticleSource::INIT_PARTICLE_END_SIZE;
}

ParticleSource::~ParticleSource()
{
	particleTexture = NULL;
	safe_delete_array(vertices);
	safe_delete_array(indices);
	safe_delete_array(particle);
}

void ParticleSource::SetupDoughnut(D3DXVECTOR3 position, float initRadius, float speed, 
								   float startSize, float endSize, 
								   D3DXCOLOR startColour, D3DXCOLOR endColour)
{
	int numParticles = 35;
	float degreeIncrement = 10;

	// setup the required number of particles
	if (maxParticles < numParticles)
	{
		SetMaxParticles(numParticles);
	}
	else
	{
		ClearParticles();
	}

	// set startsize and endsize and startcolour and endcolour
	SetStartSize(startSize);
	SetEndSize(endSize);
	SetColourRange(startColour, endColour);

	// initialise particles
	D3DXVECTOR3 pos, dirn, center;
	center = D3DXVECTOR3(0,0,0);
	for (int i=0; i<numParticles; i++)
	{
		pos = System::OffsetVector(position,(float)i * degreeIncrement, initRadius);
		dirn = System::OffsetVector(center,(float)i * degreeIncrement, speed);
		particle[i].Set(ParticleSource::INIT_TIME_TO_LIVE,startSize,endSize,pos,dirn);
	}
}

bool ParticleSource::GetParticleInUse(int index)
{
	PreCond(index < maxParticles);
	return particle[index].GetTTL() > 0;
}

float ParticleSource::GetStartSize()
{
	return startSize;
}

void ParticleSource::SetStartSize(float _startSize)
{
	startSize = _startSize;
}

float ParticleSource::GetEndSize()
{
	return endSize;
}

void ParticleSource::SetEndSize(float _endSize)
{
	endSize = _endSize;
}

void ParticleSource::ClearParticles()
{
	for (int i=0; i < maxParticles; i++)
	{
		if (particle[i].GetTTL() >= 0)
		{
			particle[i].SetTTL(0);
		}
	}
}

void ParticleSource::AddParticle(D3DXVECTOR3 pos, float offset, float angle)
{
	Particle p;

	angle += System::Random(-2,2);

	pos.x += cosf((angle+90) * System::deg2rad) * offset;
	pos.y += sinf((angle+90) * System::deg2rad) * offset;

	D3DXVECTOR3 dirn;
	dirn.x = cosf((angle+90) * System::deg2rad);
	dirn.y = sinf((angle+90) * System::deg2rad);
	dirn.z = 0;

	// find first free slot
	for (int i=0; i < maxParticles; i++)
	{
		if (particle[i].GetTTL() <= 0)
		{
			particle[i].Set(timeToLive, startSize, endSize, pos, dirn);
			return;
		}
	}
}

void ParticleSource::SetColourRange(const D3DXCOLOR& _startColour, const D3DXCOLOR& _endColour)
{
	startColour = _startColour;
	endColour = _endColour;
}

void ParticleSource::SetTimeToLive(float _timeToLive)
{
	timeToLive = _timeToLive;
}

float ParticleSource::GetTimeToLive()
{
	return timeToLive;
}

void ParticleSource::SetMaxParticles(int _maxParticles)
{
	maxParticles = _maxParticles;

	safe_delete_array(vertices);
	safe_delete_array(indices);
	safe_delete_array(particle);

	particle = new Particle[maxParticles];
	vertices = new D3DPCOLOURVERTEX[maxParticles*4];
	indices = new WORD[maxParticles*6];
}

void ParticleSource::EventLogic(double time)
{
	for (int i=0; i < maxParticles; i++)
	{
		float ttl = particle[i].GetTTL();
		if (ttl > 0)
		{
			particle[i].SetTTL(ttl - 0.1f);
			particle[i].SetPosition(particle[i].GetPosition() + particle[i].GetDirn());
		}
	}
}

void ParticleSource::Draw(double time, const D3DXVECTOR3& pos)
{
	Device* dev = Interface::GetDevice();
	dev->SetWorldTransform(System::GetIdentityMatrix());

	dev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	dev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

	dev->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);

//	dev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
//	dev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	dev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	dev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	dev->SetRenderState(D3DRS_LIGHTING, FALSE);

	dev->SetTextureStageState(0,D3DTSS_COLOROP, D3DTOP_MODULATE);
	dev->SetTextureStageState(0,D3DTSS_COLORARG1, D3DTA_TEXTURE);
	dev->SetTextureStageState(0,D3DTSS_COLORARG2, D3DTA_DIFFUSE);

	dev->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_MODULATE);
	dev->SetTextureStageState(0,D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	dev->SetTextureStageState(0,D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

	dev->SetRenderState(D3DRS_ZENABLE, TRUE);

	D3DXCOLOR col;
	D3DPCOLOURVERTEX v1, v2, v3, v4;
	WORD i1, i2, i3, i4;

	int cntr = 0;
	for (int i=0; i < maxParticles; i++)
	{
		if (particle[i].GetTTL() > 0)
		{
			// create vertex buffer - fill it and draw it
			float weight = 1 - (particle[i].GetTTL() / timeToLive);

			float size = particle[i].GetStartSize() + (particle[i].GetEndSize() - particle[i].GetStartSize()) * weight;

			col.r = startColour.r + (endColour.r - startColour.r) * weight;
			col.g = startColour.g + (endColour.g - startColour.g) * weight;
			col.b = startColour.b + (endColour.b - startColour.b) * weight;
			col.a = startColour.a + (endColour.a - startColour.a) * weight;

			v1.position = particle[i].GetPosition();
			v1.position.x -= pos.x;
			v1.position.y -= pos.y;
			v2.position = v1.position;
			v3.position = v1.position;
			v4.position = v1.position;

			v1.position.x -= size;
			v1.position.y -= size;
			v1.colour = col;
			v1.u = 0;
			v1.v = 0;

			v2.position.x += size;
			v2.position.y -= size;
			v2.colour = col;
			v2.u = 1;
			v2.v = 0;

			v3.position.x += size;
			v3.position.y += size;
			v3.colour = col;
			v3.u = 1;
			v3.v = 1;

			v4.position.x -= size;
			v4.position.y += size;
			v4.colour = col;
			v4.u = 0;
			v4.v = 1;

			vertices[cntr * 4 + 0] = v1;
			vertices[cntr * 4 + 1] = v2;
			vertices[cntr * 4 + 2] = v3;
			vertices[cntr * 4 + 3] = v4;

			indices[cntr * 6 + 0] = cntr * 4 + 2;
			indices[cntr * 6 + 1] = cntr * 4 + 1;
			indices[cntr * 6 + 2] = cntr * 4 + 0;
			indices[cntr * 6 + 3] = cntr * 4 + 0;
			indices[cntr * 6 + 4] = cntr * 4 + 3;
			indices[cntr * 6 + 5] = cntr * 4 + 2;

			cntr++;
		}
	}

	// draw particles
	if (cntr > 0)
	{
		dev->SetTexture(0, particleTexture);
		dev->SetFVF(D3DPCOLOURFVF);
		dev->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, cntr*4, cntr*2, indices,  
									D3DFMT_INDEX16, vertices, sizeof(D3DPCOLOURVERTEX));

	}
	// Reset render states
	dev->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);
	dev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
}

