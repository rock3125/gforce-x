//////////////////////////////////////////////////////////////////////
/// GRAVITY FORCE X
///
/// (c)Copyright Peter de Vocht, 2006
///
/// I'm releasing this code under GPL v2.1 (see included file lgpl.txt)
///
//////////////////////////////////////////////////////////////////////

#pragma once

#include "system/model/worldObject.h"

//////////////////////////////////////////////////////////////////////////

class Explosion : public WorldObject
{
	Explosion(const Explosion&);
	const Explosion& operator=(const Explosion&);
public:
	Explosion();
	~Explosion();

	// start a new eplosion using this animation
	void Start();

	// has the animation finished?
	bool Finished();

	// handle event logic
	void EventLogic(double time);

	// draw the animation
	void Draw(const D3DXVECTOR3& pos, float size);

private:
	int			numAnimations;
	int			animationIndex;
	bool		inUse;
	double		explosionTimer;
	double		EXPLOSION_DURATION;
	double		ANIMATION_DURATION;

	Texture**	explosion;

	// graphic display
	D3DPVERTEX	vertices[4];
	WORD		indices[6];
};

