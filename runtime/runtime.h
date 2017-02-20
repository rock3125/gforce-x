//////////////////////////////////////////////////////////////////////
/// GRAVITY FORCE X
///
/// (c)Copyright Peter de Vocht, 2006
///
/// I'm releasing this code under GPL v2.1 (see included file lgpl.txt)
///
//////////////////////////////////////////////////////////////////////

#pragma once

#include "system/signals/levelObserver.h"

class Level;
class Camera;

// game items
class Base;
class Ship;
class ModelMap;
class IndestructableRegion;

////////////////////////////////////////////////////

class Runtime : public LevelObserver
{
	Runtime(const Runtime&);
	const Runtime& operator=(const Runtime&);
public:
	Runtime();
	virtual ~Runtime();

	// game type to play
	enum GameType
	{
		GT_FIRST_OUT_OF_FIVE,
		GT_FIRST_OUT_OF_TEN,
		GT_TEAM_FIRST_OUT_OF_FIVE,
		GT_TEAM_FIRST_OUT_OF_TEN
	};

	// notification from the level listener that a new level is here
	virtual void NewLevel(Level* level);
	virtual void FocusObject(WorldObject* obj);

	// set selected item
	virtual void SetSelectedItem(WorldObject* selectedItem) = 0;

	// set split screen type
	virtual void SetSplitScreenType(int splitScreenType) = 0;

	// set the current level
	virtual void SetCurrentLevel(Level* currentLevel);

	// initialise this system
	virtual void Initialise()=0;

	// update runtime
	virtual void EventLogic(double time)=0;

	// render runtime
	virtual void Draw(double time)=0;

	// get camera of runtime
	virtual Camera* GetCamera()=0;

	// set camera of runtime
	virtual void SetCamera(const Camera& camera)=0;

	// get set wireframe
	virtual bool GetWireframe()=0;
	virtual void SetWireframe(bool wireframe)=0;

	// get set cull ccw
	virtual bool GetCullCCW()=0;
	virtual void SetCullCCW(bool _cullCCW)=0;

	// get game items
	ModelMap*			GetModelMap();
	std::vector<Base*>	GetBases();
	std::vector<Ship*>	GetShips();
	std::vector<IndestructableRegion*> GetIndestructableRegions();

	// add a ship for
	void AddShip(Ship* ship);

	// get set game type
	int GetGameType();
	void SetGameType(int _gameType);

	// get set score
	void SetScore(int playerId, int score);
	int GetScore(int playerId);

protected:
	// current level
	Level*				currentLevel;

	// collections of game objects
	ModelMap*			modelMap;
	std::vector<Base*>	base;
	std::vector<Ship*>	ship;
	std::vector<IndestructableRegion*> indestructableRegion;

	// type of game we're playing
	int gameType;
	int score[4];
};

