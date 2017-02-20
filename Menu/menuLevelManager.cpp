//////////////////////////////////////////////////////////////////////
/// GRAVITY FORCE X
///
/// (c)Copyright Peter de Vocht, 2006
///
/// I'm releasing this code under GPL v2.1 (see included file lgpl.txt)
///
//////////////////////////////////////////////////////////////////////

#include "standardFirst.h"

#include "menu/menuLevelManager.h"
#include "menu/menuLevel.h"

#include "d3d9/texture.h"
#include "system/xml/XmlParser.h"

float MenuLevelManager::BLINK_INTERVAL = 0.5f;

/////////////////////////////////////////////////////////////

MenuLevelManager::MenuLevelManager()
{
	activeItem = 0;
	blinking = false;
	blinkTime = 0;
	lastAction = LA_NONE;

	levelTracker.numPlayers = 0;

	control[CT_LEFT1] = new Input::Event(Input::KEYBOARD, Input::KEY, 0, Input::SK_LEFT);
	control[CT_RIGHT1] = new Input::Event(Input::KEYBOARD, Input::KEY, 0, Input::SK_RIGHT);
	control[CT_LEFT2] = new Input::Event(Input::GAMEPAD, Input::GAMEPAD_AXIS, 0, 0);
	control[CT_RIGHT2] = new Input::Event(Input::GAMEPAD, Input::GAMEPAD_AXIS, 0, 1);

	colour1 = D3DXCOLOR(1,1,1,1);
	colour2 = D3DXCOLOR(0.8f,0.8f,0.8f,1);
}

MenuLevelManager::~MenuLevelManager()
{
	for (int i=0; i < NUM_CONTROLS; i++)
	{
		safe_delete(control[i]);
	}
}

MenuLevelManager::MenuLevelManager(const MenuLevelManager& mm)
{
	operator=(mm);
}

const MenuLevelManager& MenuLevelManager::operator=(const MenuLevelManager& mm)
{
	activeItem = mm.activeItem;
	levelTracker = mm.levelTracker;
	lastAction = mm.lastAction;
	return *this;
}

void MenuLevelManager::Load(const std::string& filename)
{
	safe_delete_stl_array(level);

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
			if (children[i]->GetTag() == "level")
			{
				std::string name = children[i]->GetValue("name");
				std::string icon = children[i]->GetValue("icon");
				std::string file = children[i]->GetValue("file");
				int maxPlayers = System::Str2Int(children[i]->GetValue("maxplayers"));
				int maxTeams = System::Str2Int(children[i]->GetValue("maxteams"));

				MenuLevel* level1 = new MenuLevel();
				level1->SetName(name);
				level1->SetIcon(icon);
				level1->SetFile(file);
				level1->SetMaxPlayers(maxPlayers);
				level1->SetMaxTeams(maxTeams);

				// add menu
				level.push_back(level1);
			}
		}
	}
}

void MenuLevelManager::Draw(int numPlayers, bool selected, float& xpos, float& ypos)
{
	// reload level info?
	if (levelTracker.numPlayers != numPlayers)
	{
		levelTracker.numPlayers = numPlayers;
		levelTracker.level.clear();

		for (int i=0; i < level.size(); i++)
		{
			if (level[i]->GetMaxPlayers() >= numPlayers)
			{
				levelTracker.level.push_back(level[i]);
			}
		}
	}

	// draw levels
	Device* dev = Interface::GetDevice();
	Font* font = dev->GetFont();
	D3DXVECTOR2 leftTop;
	D3DXVECTOR2 rightBottom;

	D3DXCOLOR colour;
	colour = colour1;
	for (int i=0; i < levelTracker.level.size(); i++)
	{
		if (selected && activeItem == i)
		{
			if (blinking)
			{
				colour = colour1;
			}
			else
			{
				colour = colour2;
			}
		}
		else
		{
			colour = colour1;
		}

		float xp = xpos + (i%8) * 80;
		float yp = ypos + (i/8) * 80;

		MenuLevel* ml = levelTracker.level[i];
		Texture* t = ml->GetLevelIcon();
		leftTop = D3DXVECTOR2(xp,yp);
		float w = (float)t->GetWidth();
		float h = (float)t->GetHeight();
		rightBottom = D3DXVECTOR2(xp + w, yp + h);
		dev->FillRect(leftTop,rightBottom,colour,t);
		float width = font->GetWidth(ml->GetName()) * 0.5f;
		font->Write(xp + w * 0.5f - width, yp + h + 4, ml->GetName(), colour1);
	}
	ypos += ((levelTracker.level.size() / 8)+1) * 80;
	ypos += 40;
}

void MenuLevelManager::EventLogic(double time)
{
	blinkTime += time;
	if (blinkTime > BLINK_INTERVAL)
	{
		blinkTime = 0;
		blinking = !blinking;
	}

	// check inputs
	if ((Input::CheckEvent(control[CT_LEFT1]) || Input::CheckEvent(control[CT_LEFT2])) && lastAction != LA_LEFT)
	{
		lastAction = LA_LEFT;

		if (activeItem > 0)
		{
			activeItem--;
		}
	}
	else if ((Input::CheckEvent(control[CT_RIGHT1]) || Input::CheckEvent(control[CT_RIGHT2])) && lastAction != LA_RIGHT)
	{
		lastAction = LA_RIGHT;

		if ((activeItem+1) < levelTracker.level.size())
		{
			activeItem++;
		}
	}
	if (!(Input::CheckEvent(control[CT_LEFT1]) || Input::CheckEvent(control[CT_LEFT2])) && lastAction == LA_LEFT)
		lastAction = LA_NONE;
	if (!(Input::CheckEvent(control[CT_RIGHT1]) || Input::CheckEvent(control[CT_RIGHT2])) && lastAction == LA_RIGHT)
		lastAction = LA_NONE;
}

MenuLevel* MenuLevelManager::GetSelectedLevel()
{
	return level[activeItem];
}

