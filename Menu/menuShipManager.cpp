//////////////////////////////////////////////////////////////////////
/// GRAVITY FORCE X
///
/// (c)Copyright Peter de Vocht, 2006
///
/// I'm releasing this code under GPL v2.1 (see included file lgpl.txt)
///
//////////////////////////////////////////////////////////////////////

#include "standardFirst.h"

#include "menu/menuShipManager.h"
#include "menu/menuShip.h"

#include "d3d9/texture.h"
#include "system/xml/XmlParser.h"

float MenuShipManager::BLINK_INTERVAL = 0.5f;

/////////////////////////////////////////////////////////////

MenuShipManager::MenuShipManager()
{
	activeItem = 0;
	blinking = false;
	blinkTime = 0;
	lastAction = LA_NONE;

	control[CT_LEFT1] = new Input::Event(Input::KEYBOARD, Input::KEY, 0, Input::SK_LEFT);
	control[CT_RIGHT1] = new Input::Event(Input::KEYBOARD, Input::KEY, 0, Input::SK_RIGHT);
	control[CT_LEFT2] = new Input::Event(Input::GAMEPAD, Input::GAMEPAD_AXIS, 0, 0);
	control[CT_RIGHT2] = new Input::Event(Input::GAMEPAD, Input::GAMEPAD_AXIS, 0, 1);

	colour1 = D3DXCOLOR(1,1,1,1);
	colour2 = D3DXCOLOR(0.8f,0.8f,0.8f,1);
}

MenuShipManager::~MenuShipManager()
{
	for (int i=0; i < NUM_CONTROLS; i++)
	{
		safe_delete(control[i]);
	}
}

MenuShipManager::MenuShipManager(const MenuShipManager& mm)
{
	operator=(mm);
}

const MenuShipManager& MenuShipManager::operator=(const MenuShipManager& mm)
{
	activeItem = mm.activeItem;
	lastAction = mm.lastAction;
	return *this;
}

void MenuShipManager::Load(const std::string& filename)
{
	safe_delete_stl_array(ship);

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
			if (children[i]->GetTag() == "ship")
			{
				std::string icon = children[i]->GetValue("icon");
				std::string model = children[i]->GetValue("model");

				MenuShip* ship1 = new MenuShip();
				ship1->SetIcon(icon);
				ship1->SetModel(model);

				// maxspeed="1.0" 
				// maxaccel="0.010"
				// accel="0.0035"
				// rv="5"
				// fu="0.0004"
				// bullets="200"
				// missiles="5"
				// ss="0.1"
				ship1->maxSpeed = System::Str2Float(children[i]->GetValue("maxspeed"));
				ship1->maxAccel = System::Str2Float(children[i]->GetValue("maxaccel"));
				ship1->baseOffset = System::Str2Float(children[i]->GetValue("offset"));
				ship1->accel = System::Str2Float(children[i]->GetValue("accel"));
				ship1->rotationalVelocity  = System::Str2Float(children[i]->GetValue("rv"));
				ship1->fuelUsage = System::Str2Float(children[i]->GetValue("fu"));
				ship1->numBullets = System::Str2Int(children[i]->GetValue("bullets"));
				ship1->numMissiles = System::Str2Int(children[i]->GetValue("missiles"));
				ship1->shieldStrength = System::Str2Float(children[i]->GetValue("ss"));

				std::string bc = children[i]->GetValue("bulletcolour");
				ship1->bulletColour = D3DXCOLOR(System::Str2Float(System::GetItem(bc,',',0)),
												System::Str2Float(System::GetItem(bc,',',1)),
												System::Str2Float(System::GetItem(bc,',',2)),1);

				// add menu
				ship.push_back(ship1);
			}
		}
	}
}

void MenuShipManager::Draw(bool selected, float& xpos, float& ypos)
{
	// draw levels
	Device* dev = Interface::GetDevice();

	D3DXVECTOR2 leftTop;
	D3DXVECTOR2 rightBottom;
	D3DXCOLOR colour;

	colour = colour1;
	for (int i=0; i < ship.size(); i++)
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

		float xp = xpos + (i%8) * 134;
		float yp = ypos + (i/8) * 70;

		MenuShip* ms = ship[i];
		Texture* t = ms->GetShipIcon();
		leftTop = D3DXVECTOR2(xp,yp);
		float w = (float)t->GetWidth();
		float h = (float)t->GetHeight();
		rightBottom = D3DXVECTOR2(xp + w, yp + h);
		dev->FillRect(leftTop,rightBottom,colour,t);
	}
	ypos += ((ship.size() / 8)+1) * 80;
	ypos += 40;
}

void MenuShipManager::EventLogic(double time)
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

		if ((activeItem+1) < ship.size())
		{
			activeItem++;
		}
	}
	if (!(Input::CheckEvent(control[CT_LEFT1]) || Input::CheckEvent(control[CT_LEFT2])) && lastAction == LA_LEFT)
		lastAction = LA_NONE;
	if (!(Input::CheckEvent(control[CT_RIGHT1]) || Input::CheckEvent(control[CT_RIGHT2])) && lastAction == LA_RIGHT)
		lastAction = LA_NONE;
}

MenuShip* MenuShipManager::GetSelectedShip()
{
	return ship[activeItem];
}

