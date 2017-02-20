//////////////////////////////////////////////////////////////////////
/// GRAVITY FORCE X
///
/// (c)Copyright Peter de Vocht, 2006
///
/// I'm releasing this code under GPL v2.1 (see included file lgpl.txt)
///
//////////////////////////////////////////////////////////////////////

#include "standardFirst.h"

#include "d3d9/texture.h"
#include "system/xml/XmlParser.h"
#include "system/model/level.h"
#include "system/BaseApp.h"
#include "runtime/runtime.h"

#include "menu/menuManager.h"
#include "menu/menu.h"
#include "menu/menuItem.h"
#include "menu/menuLevelManager.h"
#include "menu/menuShipManager.h"
#include "menu/menuLevel.h"
#include "menu/menuShip.h"

#include "game/ship.h"
#include "game/base.h"
#include "game/modelMap.h"


float MenuManager::BLINK_INTERVAL = 0.5f;

/////////////////////////////////////////////////////////////

MenuManager::MenuManager()
	: gameTitle(NULL)
	, menuLevelManager(NULL)
	, menuShipManager(NULL)
{
	active = true;
	activeMenu = 0;
	activeItem = 0;
	blinking = false;
	blinkTime = 0;
	lastAction = LA_NONE;

	menuLevelManager = new MenuLevelManager();
	menuShipManager = new MenuShipManager();

	for (int i=0; i < NUM_CONTROLS; i++)
	{
		control[i] = NULL;
	}

	// setup controls
	control[CT_UP1] = new Input::Event(Input::KEYBOARD, Input::KEY, 0, Input::SK_UP);
	control[CT_DOWN1] = new Input::Event(Input::KEYBOARD, Input::KEY, 0, Input::SK_DOWN);
	control[CT_SELECT1] = new Input::Event(Input::KEYBOARD, Input::KEY, 0, Input::SK_ENTER);
	control[CT_UP2] = new Input::Event(Input::GAMEPAD, Input::GAMEPAD_AXIS, 0, 2);
	control[CT_DOWN2] = new Input::Event(Input::GAMEPAD, Input::GAMEPAD_AXIS, 0, 3);
	control[CT_SELECT2] = new Input::Event(Input::GAMEPAD, Input::GAMEPAD_BUTTON, 0, 0);
	control[CT_ESC] = new Input::Event(Input::KEYBOARD, Input::KEY, 0, Input::SK_ESCAPE);

	// setup ships controls
	inputs[0] =Input::CTRL_KEYBOARD;
	inputs[1] =Input::CTRL_GAMEPAD1;
	inputs[2] =Input::CTRL_GAMEPAD2;
	inputs[3] =Input::CTRL_GAMEPAD3;

	inputNames[0] = NULL;
	inputNames[1] = NULL;
	inputNames[2] = NULL;
	inputNames[3] = NULL;
	inputNames[4] = NULL;
	inputNames[5] = NULL;

	white = D3DXCOLOR(1,1,1,1);
	selColour = D3DXCOLOR(1,1,0.2f,1);
	selColourOff = D3DXCOLOR(0.8f,0.8f,0.8f,1);
}

MenuManager::~MenuManager()
{
	safe_delete(menuShipManager);
	safe_delete(menuLevelManager);

	safe_delete_stl_array(menu);
	gameTitle = NULL;

	for (int i=0; i < NUM_CONTROLS; i++)
	{
		safe_delete(control[i]);
	}

	inputNames[0] = NULL;
	inputNames[1] = NULL;
	inputNames[2] = NULL;
	inputNames[3] = NULL;
	inputNames[4] = NULL;
	inputNames[5] = NULL;
}

MenuManager::MenuManager(const MenuManager& mm)
	: gameTitle(NULL)
	, menuLevelManager(NULL)
	, menuShipManager(NULL)
{
	operator=(mm);
}

const MenuManager& MenuManager::operator=(const MenuManager& mm)
{
	menu = mm.menu;
	active = mm.active;
	activeMenu = mm.activeMenu;
	activeItem = mm.activeItem;
	gameTitle = mm.gameTitle;
	return *this;
}

void MenuManager::SetActiveMenu(int _activeMenu, int _activeItem)
{
	activeMenu = _activeMenu;
	activeItem = _activeItem;
}

void MenuManager::Load(const std::string& filename)
{
	safe_delete_stl_array(menu);

	XmlParser parser;
	if (!parser.LoadAndParse(filename))
	{
		throw new Exception(parser.GetError());
	}
	XmlNode* root = parser.GetDocumentRoot();
	if (root != NULL)
	{
		// parser xml menu document
		std::vector<XmlNode*> children = root->GetChildren();
		for (int i=0; i < children.size(); i++)
		{
			if (children[i]->GetTag() == "menu")
			{
				int id = System::Str2Int(children[i]->GetValue("id"));
				std::string name = children[i]->GetValue("name");
				std::string background = children[i]->GetValue("background");

				Menu* menu1 = new Menu();
				menu1->SetId(id);
				menu1->SetName(name);
				menu1->SetBackground(background);

				std::vector<XmlNode*> children2 = children[i]->GetChildren();
				for (int j=0; j < children2.size();  j++)
				{
					if (children2[j]->GetTag() == "item")
					{
						std::string name = children2[j]->GetValue("name");
						std::string action = children2[j]->GetValue("action");
						bool showLevels = false;
						bool showShips = false;
						int numPlayers = 0;
						int player = 0;
						bool showInputs = false;
						int inputId = 0;
						if (children2[j]->GetValue("showlevels") == "true")
						{
							showLevels = true;
							numPlayers = System::Str2Int(children2[j]->GetValue("numplayers"));
						}
						if (children2[j]->GetValue("showships") == "true")
						{
							showShips = true;
							player = System::Str2Int(children2[j]->GetValue("player"));
						}
						if (!children2[j]->GetValue("selectinput").empty())
						{
							showInputs = true;
							inputId = System::Str2Int(children2[j]->GetValue("selectinput"));
							std::string str = children2[j]->GetValue("inputdefault");
							if (str == "keyboard")
							{
								inputs[inputId] = Input::CTRL_KEYBOARD;
							}
							else if (str == "gamepad1")
							{
								inputs[inputId] = Input::CTRL_GAMEPAD1;
							}
							else if (str == "gamepad2")
							{
								inputs[inputId] = Input::CTRL_GAMEPAD2;
							}
							else if (str == "gamepad3")
							{
								inputs[inputId] = Input::CTRL_GAMEPAD3;
							}
							else if (str == "gamepad4")
							{
								inputs[inputId] = Input::CTRL_GAMEPAD4;
							}
						}

						MenuItem* menuItem = new MenuItem();
						menuItem->SetName(name);
						menuItem->SetAction(action);
						menuItem->SetShowLevels(showLevels);
						menuItem->SetNumPlayers(numPlayers);
						menuItem->SetShowShips(showShips);
						menuItem->SetPlayer(player);
						menuItem->SetShowInputs(showInputs);
						menuItem->SetInputId(inputId);

						menu1->AddMenuItem(menuItem);
					}
				}

				// add menu
				menu.push_back(menu1);
			}
		}
	}

	// load title texture
	gameTitle = TextureCache::GetTexture("menu\\GravityForceX.tga");

	// load all available levels
	menuLevelManager->Load(System::GetDataDirectory() + "levels\\levels.xml");

	// load all available ships
	menuShipManager->Load(System::GetDataDirectory() + "ships\\ships.xml");

	// get input names
	inputNames[Input::CTRL_KEYBOARD] = TextureCache::GetTexture("menu\\Keyboard.tga");
	inputNames[Input::CTRL_GAMEPAD1] = TextureCache::GetTexture("menu\\Gamepad1.tga");
	inputNames[Input::CTRL_GAMEPAD2] = TextureCache::GetTexture("menu\\Gamepad2.tga");
	inputNames[Input::CTRL_GAMEPAD3] = TextureCache::GetTexture("menu\\Gamepad3.tga");
	inputNames[Input::CTRL_GAMEPAD4] = TextureCache::GetTexture("menu\\Gamepad4.tga");
}

void MenuManager::SetupSceneForRendering()
{
	Device* dev=Interface::GetDevice();
	if (!dev->BeginScene()) return;

	// D3DCLEAR_STENCIL
	dev->Clear(0,NULL,D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,D3DXCOLOR(0,0,0,0),1,0);

	dev->SetViewport(0,0,dev->GetWidth(),dev->GetHeight());

	// set fixed funciton transforms
	dev->SetIdentityTransform();

	// enable color writes
	dev->SetRenderState(D3DRS_COLORWRITEENABLE,D3DCOLORWRITEENABLE_RED   |
											   D3DCOLORWRITEENABLE_BLUE  |
											   D3DCOLORWRITEENABLE_GREEN |
											   D3DCOLORWRITEENABLE_ALPHA);
	// set fillemode
	dev->SetRenderState(D3DRS_FILLMODE,D3DFILL_SOLID);

	// Turn on the zbuffer
	dev->SetRenderState(D3DRS_ZENABLE,TRUE);
	dev->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);

	dev->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
    dev->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
    dev->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );
    dev->SetSamplerState( 0, D3DSAMP_ADDRESSU,  D3DTADDRESS_CLAMP );
    dev->SetSamplerState( 0, D3DSAMP_ADDRESSV,  D3DTADDRESS_CLAMP );

	dev->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	dev->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);

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
	dev->SetTextureStageState(0,D3DTSS_ALPHAARG2, D3DTA_TFACTOR);
}

void MenuManager::WriteText(float x, float y, D3DXCOLOR c, Texture* t)
{
	if (t != NULL)
	{
		Device* dev=Interface::GetDevice();
		dev->FillRect(D3DXVECTOR2(x,y), D3DXVECTOR2(x+t->GetWidth(),y+t->GetHeight()), c, t);
	}
}

void MenuManager::UpdateProgress(float progress)
{
	// display progress (force render)
	Device* dev = Interface::GetDevice();
	if (dev != NULL)
	{
		SetupSceneForRendering();

		D3DXCOLOR barColour = D3DXCOLOR(0.2f,0.2f,1,1);

		float w = (float)dev->GetWidth();
		float h = (float)dev->GetHeight();
		float barw = w * 0.8f * progress;
		float barl = w * 0.1f;
		float barh = 5;
		float bart = h * 0.5f - barh * 0.5f;

		Font* font = dev->GetFont();
		if (font != NULL)
		{
			font->Write(barl,bart-15, "LOADING...", D3DXCOLOR(1,1,1,1));
		}
		dev->FillRect(D3DXVECTOR2(barl,bart), D3DXVECTOR2(barl+barw,bart+barh), barColour);

		dev->EndScene();
		dev->Present();
	}
}

void MenuManager::Draw(double time)
{
	if (active)
	{
		Device* dev = Interface::GetDevice();

		SetupSceneForRendering();

		// draw background
		float w = (float)dev->GetWidth();
		float h = (float)dev->GetHeight();
		Menu* selectedMenu = GetMenuById(activeMenu);
		if (selectedMenu != NULL)
		{
			Texture* texture = selectedMenu->GetBackgroundTexture();
			dev->FillRect(D3DXVECTOR2(0,0),D3DXVECTOR2(w,h),white,texture);

			// draw game title
			WriteText(10,10,white,gameTitle);

			// draw menu name
			float menux = 40;
			float menuy = 80;
			WriteText(menux,menuy,white,menu[activeMenu]->GetNameTexture());

			float spacing = 20;
			float xpos = menux+5;
			float ypos = menuy+25;

			D3DXCOLOR col;
			std::vector<MenuItem*> items = selectedMenu->GetItems();
			for (int i=0; i < items.size(); i++)
			{
				bool selected = (activeItem == i);
				MenuItem* item = items[i];
				if (item->GetShowLevels())
				{
					menuLevelManager->Draw(item->GetNumPlayers(), selected, xpos, ypos);
				}
				else if (item->GetShowShips())
				{
					menuShipManager->Draw(selected, xpos,ypos);
				}
				else
				{
					if (selected)
					{
						if (blinking)
						{
							col = selColour;
						}
						else
						{
							col = selColourOff;
						}
					}
					else
					{
						col = white;
					}
					WriteText(xpos,ypos,col,item->GetNameTexture());

					if (item->GetShowInputs())
					{
						int inputId = item->GetInputId();
						int input = inputs[inputId];
						WriteText(xpos+70,ypos,col, inputNames[input]);
					}

					ypos += spacing;
				}
			}
		}
		dev->EndScene();
		dev->Present();
	}
}

bool MenuManager::LoadLevel(int splitScreenType, int gameType)
{
	// load the level
	MenuLevel* level = menuLevelManager->GetSelectedLevel();
	std::string filename = level->GetFile();
	Level* l = BaseApp::GetApp().Load(System::GetDataDirectory() + filename);
	Runtime* rt = BaseApp::GetApp().GetCurrentRuntime();
	rt->SetGameType(gameType);
	rt->SetSplitScreenType(splitScreenType);

	// get bases
	std::vector<Base*> bases = rt->GetBases();

	// log level info
	Log::GetLog() << (int)ships.size() << " ships" << System::CR;
	Log::GetLog() << (int)bases.size() << " bases" << System::CR;

	// set the ships in the level
	if (l != NULL && ships.size() <= bases.size())
	{
		for (int i=0; i < ships.size(); i++)
		{
			ships[i]->SetControl(inputs[i]);
			Ship* ship = ships[i]->GetShip();
			if (i == 0)
			{
				rt->SetSelectedItem(ship);
				ship->SetupKeyboard();
			}

			Base* base = bases[i];
			D3DXVECTOR3 pos = base->GetPosition();
			pos.z = 5;
			pos.y += ships[i]->GetBaseOffset() * 2;
			ship->SetPosition(pos);
			ship->SetInitialPosition(pos);
			l->AddObjectToScene(l->GetSceneRoot(), ship);
			rt->AddShip(ship);
		}
		return true;
	}
	return false;
}

void MenuManager::GetScreenAndGameType(std::string action, int& screenType, int& gameType)
{
	int offset = 0;
	if (action.substr(0,13) == "sc_horizontal")
	{
		screenType = ModelMap::SC_HORIZONTAL;
		offset = 14;
	}
	else if (action.substr(0,11) == "sc_vertical")
	{
		screenType = ModelMap::SC_VERTICAL;
		offset = 12;
	}
	else if (action.substr(0,7) == "sc_four")
	{
		screenType = ModelMap::SC_FOUR;
		offset = 8;
	}

	std::string gt = action.substr(offset);
	if (gt == "5")
	{
		gameType = Runtime::GT_FIRST_OUT_OF_FIVE;
	}
	else if (gt == "10")
	{
		gameType = Runtime::GT_FIRST_OUT_OF_TEN;
	}
	else if (gt == "5 team")
	{
		gameType = Runtime::GT_TEAM_FIRST_OUT_OF_FIVE;
	}
	else if (gt == "10 team")
	{
		gameType = Runtime::GT_TEAM_FIRST_OUT_OF_TEN;
	}
}

void MenuManager::EventLogic(double time)
{
	blinkTime += time;
	if (blinkTime > BLINK_INTERVAL)
	{
		blinkTime = 0;
		blinking = !blinking;
	}

	if (Input::CheckEvent(control[CT_ESC]) && lastAction != LA_ESC)
	{
		lastAction = LA_ESC;

		Menu* menu = GetMenuById(activeMenu);
		activeItem = menu->GetItems().size() - 1;
	}

	// check inputs
	if ((Input::CheckEvent(control[CT_UP1]) || Input::CheckEvent(control[CT_UP2])) && lastAction != LA_UP)
	{
		lastAction = LA_UP;

		// take them up the menu
		Menu* menu = GetMenuById(activeMenu);
		if (menu != NULL)
		{
			int oldActiveItem = activeItem;
			std::vector<MenuItem*> items = menu->GetItems();
			do
			{
				if (activeItem > 0)
				{
					activeItem--;
				}
			}
			while (items[activeItem]->GetAction().empty() && activeItem>0);
			if (items[activeItem]->GetAction().empty())
			{
				activeItem = oldActiveItem;
			}
		}
	}
	else if ((Input::CheckEvent(control[CT_DOWN1]) || Input::CheckEvent(control[CT_DOWN2])) && lastAction != LA_DOWN)
	{
		lastAction = LA_DOWN;

		// take them up the menu
		Menu* menu = GetMenuById(activeMenu);
		if (menu != NULL)
		{
			int oldActiveItem = activeItem;
			std::vector<MenuItem*> items = menu->GetItems();
			do
			{
				if ((activeItem+1) < items.size())
				{
					activeItem++;
				}
			}
			while (items[activeItem]->GetAction().empty() && activeItem < items.size());
			if (items[activeItem]->GetAction().empty())
			{
				activeItem = oldActiveItem;
			}
		}
	}
	else if ((Input::CheckEvent(control[CT_SELECT1]) || Input::CheckEvent(control[CT_SELECT2])) && lastAction != LA_SELECT)
	{
		lastAction = LA_SELECT;

		// do the action
		Menu* menu = GetMenuById(activeMenu);
		if (menu != NULL)
		{
			std::vector<MenuItem*> items = menu->GetItems();
			if (activeItem < items.size())
			{
				// check the current item - if its a ship
				// add the selection of the ship to the ships array
				if (items[activeItem]->GetShowShips())
				{
					int playerId = items[activeItem]->GetPlayer();
					if (playerId < ships.size())
					{
						ships[playerId] = menuShipManager->GetSelectedShip();
					}
					else
					{
						ships.push_back(menuShipManager->GetSelectedShip());
					}
				}

				std::string action = items[activeItem]->GetAction();
				if (action == "exit")
				{
					BaseApp::GetApp().Quit();
					return;
				}
				if (action.substr(0,4) == "goto")
				{
					activeMenu = System::Str2Int(action.substr(5));
					activeItem = 0;
					Menu* menu = GetMenuById(activeMenu);
					if (menu != NULL)
					{
						std::vector<MenuItem*> items = menu->GetItems();
						while (activeItem < items.size() && items[activeItem]->GetAction().empty())
						{
							activeItem++;
						}
					}
					return;
				}
				if (action.substr(0,5) == "play ")
				{
					int screenType;
					int gameType;
					GetScreenAndGameType(action.substr(5), screenType, gameType);
					active = !LoadLevel(screenType,gameType);
					return;
				}
				if (action == "setcontrol")
				{
					if (items[activeItem]->GetShowInputs())
					{
						int inputId = items[activeItem]->GetInputId();
						int input = inputs[inputId];
						input++;
						if (input > Input::CTRL_GAMEPAD4)
						{
							input = Input::CTRL_KEYBOARD;
						}
						inputs[inputId] = input;
					}
				}
			}
		}
	}
	if (!(Input::CheckEvent(control[CT_UP1]) || Input::CheckEvent(control[CT_UP2])) && lastAction == LA_UP)
		lastAction = LA_NONE;
	if (!(Input::CheckEvent(control[CT_DOWN1]) || Input::CheckEvent(control[CT_DOWN2])) && lastAction == LA_DOWN)
		lastAction = LA_NONE;
	if (!(Input::CheckEvent(control[CT_SELECT1]) || Input::CheckEvent(control[CT_SELECT2])) && lastAction == LA_SELECT)
		lastAction = LA_NONE;
	if (!Input::CheckEvent(control[CT_ESC]) && lastAction == LA_ESC)
		lastAction = LA_NONE;

	Menu* menu = GetMenuById(activeMenu);
	if (menu != NULL)
	{
		std::vector<MenuItem*> items = menu->GetItems();
		MenuItem* item = items[activeItem];
		if (item->GetShowLevels())
		{
			menuLevelManager->EventLogic(time);
		}
		else if (item->GetShowShips())
		{
			menuShipManager->EventLogic(time);
		}
	}
}

bool MenuManager::GetActive()
{
	return active;
}

void MenuManager::SetActive(bool _active)
{
	active = _active;
}

Menu* MenuManager::GetMenuById(int id)
{
	for (int i=0; i < menu.size(); i++)
	{
		if (menu[i] != NULL && menu[i]->GetId() == id)
			return menu[i];
	}
	return NULL;
}

std::vector<Menu*> MenuManager::GetMenus()
{
	return menu;
}

void MenuManager::SetMenus(std::vector<Menu*> _menu)
{
	menu = _menu;
}

