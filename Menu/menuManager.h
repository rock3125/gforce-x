//////////////////////////////////////////////////////////////////////
/// GRAVITY FORCE X
///
/// (c)Copyright Peter de Vocht, 2006
///
/// I'm releasing this code under GPL v2.1 (see included file lgpl.txt)
///
//////////////////////////////////////////////////////////////////////

#pragma once

#include "system/input.h"

#include "system/signals/progressObserver.h"

class Menu;
class Camera;
class MenuLevelManager;
class MenuShipManager;
class MenuItem;
class ShipData;

//////////////////////////////////////////////////////////////

class MenuManager : public ProgressObserver
{
public:
	MenuManager();
	~MenuManager();
	MenuManager(const MenuManager&);
	const MenuManager& operator=(const MenuManager&);

	// load menus from file
	void Load(const std::string& filename);

	// set ship definitions after they have loaded
	void SetShipDefinitions(std::vector<ShipData*> shipDefinitions);

	// draw menu manager
	void Draw(double time);

	// events
	void EventLogic(double time);

	// access a menu by name
	Menu* GetMenuById(int id);

	// accessors
	std::map<int,Menu*> GetMenus();
	void SetMenus(std::map<int,Menu*> menu);

	// callback for progress uptdating
	void UpdateProgress(float progress);

	// set active menu and its item
	void SetActiveMenu(int activeMenu, int activeItem);

private:
	void SetupSceneForRendering();
	void WriteText(float x, float y, D3DXCOLOR c, Texture* t);

	// draw the level icons
	void DrawLevels(MenuItem* levelInfo, float xpos, float ypos);

	// parse screen and game type
	void GetScreenAndGameType(std::string action, int& screenType, int& gameType);

	// execute a menu action
	void ExecuteAction(std::string action);

private:
	// collection of all menus
	std::map<int,Menu*>	menu;

	// camera
	Camera*	camera;

	// level selection systme
	MenuLevelManager* menuLevelManager;

	// ship selection system
	MenuShipManager* menuShipManager;

	enum
	{
		CT_UP1 = 0,
		CT_UP2,
		CT_DOWN1,
		CT_DOWN2,
		CT_SELECT1,
		CT_SELECT2,
		CT_LEFT1,
		CT_LEFT2,
		CT_RIGHT1,
		CT_RIGHT2,
		CT_ESC,
		NUM_CONTROLS
	};

	enum
	{
		LA_NONE,
		LA_UP,
		LA_DOWN,
		LA_SELECT,
		LA_ESC
	};

	// input system
	Input::Event* control[NUM_CONTROLS];
	int lastAction;

	// flashing menu item
	bool blinking;
	double blinkTime;
	double BLINK_INTERVAL;

	double brightNess;
	double brightNessDirn;
	double brightNessTime;
	double BRIGHTNESS_INTERVAL;
	double BRIGHTNESS_MAX;
	double BRIGHTNESS_MIN;


	// inputs for up to four players
	int inputs[4];

	// possible input types by id
	Texture* inputNames[Input::CTRL_GAMEPAD4+1];

	// is menu system active?
	int		activeMenu;
	int		activeItem;

	// loading progress
	float progress;
	bool displayProgress;

	// colours for menu
	D3DXCOLOR white;
	D3DXCOLOR selColour;
	D3DXCOLOR selColourOff;

	// selected ship(s)
	std::vector<ShipData*> ships;
};

