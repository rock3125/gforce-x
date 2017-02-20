//////////////////////////////////////////////////////////////////////
/// GRAVITY FORCE X
///
/// (c)Copyright Peter de Vocht, 2006
///
/// I'm releasing this code under GPL v2.1 (see included file lgpl.txt)
///
//////////////////////////////////////////////////////////////////////

#include "standardFirst.h"

#include "game/explosion.h"

///////////////////////////////////////////////////////////

Explosion::Explosion()
{
	numAnimations = 7;
	animationCounter = 0;
	counter = 0;

	inUse = false;

	std::string filename;
	explosion = new Texture*[numAnimations];
	for (int i=0; i < numAnimations; i++)
	{
		filename = "explosion\\expl"+System::Int2Str(i)+".tga";
		explosion[i] = TextureCache::GetTexture(filename);
	}

	// setup indices
	indices[0] = 2;
	indices[1] = 1;
	indices[2] = 0;
	indices[3] = 0;
	indices[4] = 3;
	indices[5] = 2;
}

Explosion::~Explosion()
{
}

void Explosion::Start()
{
	inUse = true;
	animationCounter = 0;
	counter = 0;
}

void Explosion::EventLogic(double time)
{
	if (inUse)
	{
		// slow it down a bit
		animationCounter = counter / 3;
		if (animationCounter >= (numAnimations*2))
		{
			inUse = false;
		}
		counter++;
	}
}

bool Explosion::Finished()
{
	return !inUse;
}

void Explosion::Draw(const D3DXVECTOR3& pos, float size)
{
	if (animationCounter < numAnimations)
	{
		Device* dev = Interface::GetDevice();
		dev->SetWorldTransform(System::GetIdentityMatrix());

		dev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
		dev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

		dev->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);

		dev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		dev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

		dev->SetRenderState(D3DRS_LIGHTING, FALSE);

		dev->SetTextureStageState(0,D3DTSS_COLOROP, D3DTOP_MODULATE);
		dev->SetTextureStageState(0,D3DTSS_COLORARG1, D3DTA_TEXTURE);
		dev->SetTextureStageState(0,D3DTSS_COLORARG2, D3DTA_DIFFUSE);

		dev->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_MODULATE);
		dev->SetTextureStageState(0,D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		dev->SetTextureStageState(0,D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

		dev->SetRenderState(D3DRS_ZENABLE, TRUE);

		D3DXVECTOR3 normal = D3DXVECTOR3(0,0,-1);

		vertices[0].position = pos;
		vertices[1].position = pos;
		vertices[2].position = pos;
		vertices[3].position = pos;

		vertices[0].position.x -= size;
		vertices[0].position.y -= size;
		vertices[0].position.z = 0;
		vertices[0].texture.x = 0;
		vertices[0].texture.y = 0;
		vertices[0].normal = normal;

		vertices[1].position.x += size;
		vertices[1].position.y -= size;
		vertices[1].position.z = 0;
		vertices[1].texture.x = 1;
		vertices[1].texture.y = 0;
		vertices[1].normal = normal;

		vertices[2].position.x += size;
		vertices[2].position.y += size;
		vertices[2].position.z = 0;
		vertices[2].texture.x = 1;
		vertices[2].texture.y = 1;
		vertices[2].normal = normal;

		vertices[3].position.x -= size;
		vertices[3].position.y += size;
		vertices[3].position.z = 0;
		vertices[3].texture.x = 0;
		vertices[3].texture.y = 1;
		vertices[3].normal = normal;

		dev->SetTexture(0, explosion[animationCounter]);
		dev->SetFVF(D3DPFVF);
		dev->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, 4, 2, indices,  
									D3DFMT_INDEX16, vertices, sizeof(D3DPVERTEX));

		// Reset render states
		dev->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);
		dev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	}
}

