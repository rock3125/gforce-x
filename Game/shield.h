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

///////////////////////////////////////////////////////////////////

// a shield - force field around a ship

class Shield : public WorldObject
{
public:
	Shield();
	~Shield();

	// draw the shield
	void Draw(double time);

	// timer logic
	void EventLogic(double time);

private:
	// shield angles
	float	xa, ya, za;
};


