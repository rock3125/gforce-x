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

////////////////////////////////////////////////////

Runtime::Runtime()
	: currentLevel(NULL)
	, modelMap(NULL)
{
	// game type to play
	SetGameType(GT_FIRST_OUT_OF_FIVE);
}

Runtime::~Runtime()
{
	currentLevel = NULL;
	modelMap = NULL;
}

void Runtime::SetCurrentLevel(Level* _currentLevel)
{
	currentLevel=_currentLevel;
}

void Runtime::NewLevel(Level* level)
{
	SetCurrentLevel(level);
}

void Runtime::FocusObject(WorldObject* obj)
{
}

ModelMap* Runtime::GetModelMap()
{
	return modelMap;
}

std::vector<Base*> Runtime::GetBases()
{
	return base;
}

std::vector<Ship*> Runtime::GetShips()
{
	return ship;
}

std::vector<IndestructableRegion*> Runtime::GetIndestructableRegions()
{
	return indestructableRegion;
}

void Runtime::AddShip(Ship* s)
{
	ship.push_back(s);
}

int Runtime::GetGameType()
{
	return gameType;
}

void Runtime::SetGameType(int _gameType)
{
	gameType = _gameType;

	score[0] = 0;
	score[1] = 0;
	score[2] = 0;
	score[3] = 0;
}

void Runtime::SetScore(int playerId, int _score)
{
	score[playerId] = _score;
}

int Runtime::GetScore(int playerId)
{
	return score[playerId];
}

