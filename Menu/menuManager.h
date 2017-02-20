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
class MenuShip;

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

	// draw menu manager
	void Draw(double time);

	// events
	void EventLogic(double time);

	// access a menu by name
	Menu* GetMenuById(int id);

	// accessors
	std::vector<Menu*> GetMenus();
	void SetMenus(std::vector<Menu*> menu);

	// activate / deactivate the menu system
	bool GetActive();
	void SetActive(bool active);

	// callback for progress uptdating
	void UpdateProgress(float progress);

	// set active menu and its item
	void SetActiveMenu(int activeMenu, int activeItem);

private:
	void SetupSceneForRendering();
	void WriteText(float x, float y, D3DXCOLOR c, Texture* t);

	// draw the level icons
	void DrawLevels(MenuItem* levelInfo, float xpos, float ypos);

	// load and setup a level after its all done
	bool LoadLevel(int splitScreenType, int gameType);

	// parse screen and game type
	void GetScreenAndGameType(std::string action, int& screenType, int& gameType);

private:
	// collection of all menus
	std::vector<Menu*>	menu;

	// camera
	Camera*	camera;

	Texture* gameTitle;

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
	float blinkTime;
	bool blinking;

	// inputs for up to four players
	int inputs[4];

	// possible input types by id
	Texture* inputNames[Input::CTRL_GAMEPAD4+1];

	// blink speed
	static float BLINK_INTERVAL;

	// is menu system active?
	bool	active;
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
	std::vector<MenuShip*> ships;
};

