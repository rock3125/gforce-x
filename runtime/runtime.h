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
#include "system/network/packetObserver.h"
#include "game/networkPlayer.h"

class Level;
class Camera;

// game items
class Base;
class Ship;
class ShipData;
class ModelMap;
class IndestructableRegion;
class Turret;
class Water;

class BasePacket;
class PacketServerWelcome;
class MenuManager;

////////////////////////////////////////////////////

class Runtime : public PacketObserver
{
	Runtime(const Runtime&);
	const Runtime& operator=(const Runtime&);
public:
	Runtime();
	virtual ~Runtime();

	// load the ship definitions
	void LoadShipDefinitions(const std::string& filename);

	// access ship definitions
	std::vector<ShipData*> GetShipDefinitions()
	{
		return shipDefinitions;
	}

	// receive a packet from the server
	virtual void PacketReceived(BasePacket* packet);

	// system state engine - draw & events
	enum ActivityEnum
	{
		ACT_INMENU,
		ACT_INGAME,
		ACT_ERROR,
		ACT_ASKEXITGAME,
		ACT_GAMEOVER
	};

	enum Consts
	{
		MAX_PLAYERS = 2
	};

	// game type to play
	enum GameType
	{
		GT_FIRST_OUT_OF_FIVE,
		GT_FIRST_OUT_OF_TEN,
		GT_TEAM_FIRST_OUT_OF_FIVE,
		GT_TEAM_FIRST_OUT_OF_TEN
	};

	// in game sounds
	enum SoundsEnum
	{
		LAND_SHIP,
		FIRE_LASER,
		FIRE_MISSILE,
		EXPLODE,
		THRUST,
		UNDERWATER
	};

	// split screen types
	enum SplitScreenType
	{
		SC_SINGLE,
		SC_HORIZONTAL,
		SC_VERTICAL,
		SC_MIXED
	};


	// Get the current level
	Level* GetCurrentLevel()
	{
		return currentLevel;
	}

	// set current level to a new level and remove the old one
	void SetCurrentLevel(Level*);

	// notification from the level listener that a new level is here
	virtual void FocusObject(WorldObject* obj);

	// set selected item
	virtual void SetSelectedItem(WorldObject* selectedItem) = 0;

	// initialise this system
	virtual void Initialise()=0;

	// update runtime
	virtual void EventLogic(double time);

	// render runtime
	virtual void Draw(double time)=0;

	// get camera of runtime
	virtual Camera* GetCamera();

	// set camera of runtime
	virtual void SetCamera(Camera* camera);

	// get set wireframe
	virtual bool GetWireframe()=0;
	virtual void SetWireframe(bool wireframe)=0;

	// get set cull ccw
	virtual bool GetCullCCW()=0;
	virtual void SetCullCCW(bool _cullCCW)=0;

	// get game items
	ModelMap* GetModelMap()
	{
		return modelMap;
	}

	std::vector<Base*> GetBases()
	{
		return bases;
	}

	std::vector<Ship*> GetShips()
	{
		return ships;
	}

	std::vector<IndestructableRegion*> GetIndestructableRegions()
	{
		return indestructableRegions;
	}

	std::vector<Turret*> GetTurrets()
	{
		return turrets;
	}

	std::vector<Water*> GetWater()
	{
		return water;
	}

	// load menus from xml
	void LoadMenus();

	MenuManager* GetMenuManager()
	{
		return menuManager;
	}

	/////////////////////////////////////////////////////////////////////////////////
	// GamePlay (GP) specific settings for gravity force
	
	// end a network game
	void GP_StopNetworkGame();
	// start hosting a new network game
	bool GP_StartNetworkGame(std::string playerName, std::string server, int port);
	// receive a network player confirmation list
	void GP_ConfirmNetworkPlayers(PacketServerWelcome* packet);
	// start the background server
	void GP_StartServer(int gameType, int numPoints);
	// start a normal game
	bool GP_StartGame();
	// stop any type of game
	void GP_StopGame();
	// set game type (first out of 10, etc)
	void GP_SetGameType(int gameType);
	// return the game type
	int GP_GetGameType();
	// set screen type (split horizontal, vertical, fullscreen)
	void GP_SetScreenType(int screenType);
	// set player's ship
	void GP_SetShip(int player, int shipId);
	// set player's input method
	void GP_SetInput(int player, int inputId);
	// set selected level
	void GP_SetLevel(std::string levelFilename);
	// number of players on this machine/client
	void GP_NumPlayers(int numPlayers);
	// update the score
	void GP_UpdateScore(void* ship);
	// play one of the possible in-game sounds
	void GP_PlaySound(SoundsEnum sound, bool checkIsAlreadyPlaying);

	// show an error message system wide
	void GP_SetErrorMessage(std::string msg);

	// get/set winner
	void* GP_Winner()
	{
		return winner;
	}

	void GP_Winner(void* winner)
	{
		this->winner = winner;
	}

	// start a new game
	void GP_StartNewGame();

	// signal game over
	// exit current game in progress
	void GP_QuitGame();

protected:
	// load a new level
	Level* Load(const std::string& fname);

	// rebuild modelMap, bases, ships, etc.
	void RebuildGameObjects();

	// clear current level
	void ClearCurrentLevel();

protected:
	// current level
	Level* currentLevel;

	// collections of game objects - set from object graph
	// don't delete these - they're all pointer references
	ModelMap*								modelMap;
	std::vector<Base*>						bases;
	std::vector<Ship*>						ships;
	std::vector<IndestructableRegion*>		indestructableRegions;
	std::vector<Turret*>					turrets;
	std::vector<Water*>						water;

	// library of possible ships
	std::vector<ShipData*>					shipDefinitions;

	// list of network players
	std::vector<NetworkPlayer>				networkPlayers;

	// winner reference pointer
	void* winner;

	// number of players on this machine
	int numPlayers;

	// are we hostign a network game
	bool isNetworkGame;
	// network assigned player id
	int playerId;

	// game in error state?
	std::string errorString;

	// inputs and ships
	int shipIds[MAX_PLAYERS];
	int inputIds[MAX_PLAYERS];

	// the game camera
	Camera* camera;

	// menu system
	MenuManager* menuManager;

	// frame rate indicator
	float FRAME_RATE_MS;

	// type of game we're playing
	int gameType;
	int screenType;
	std::string levelFilename;

	// one of the above
	int activity;
};

