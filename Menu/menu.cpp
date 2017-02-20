//////////////////////////////////////////////////////////////////////
/// GRAVITY FORCE X
///
/// (c)Copyright Peter de Vocht, 2006
///
/// I'm releasing this code under GPL v2.1 (see included file lgpl.txt)
///
//////////////////////////////////////////////////////////////////////

#include "standardFirst.h"

#include "menu/menu.h"
#include "menu/menuItem.h"

/////////////////////////////////////////////////////////////

Menu::Menu()
	: backgroundTexture(NULL)
	, nameTexture(NULL)
{
	id = 0;
}

Menu::~Menu()
{
	safe_delete_stl_array(item);
	backgroundTexture = NULL;
	nameTexture = NULL;
}

Menu::Menu(const Menu& m)
	: backgroundTexture(NULL)
	, nameTexture(NULL)
{
	operator=(m);
}

const Menu& Menu::operator=(const Menu& m)
{
	name = m.name;
	nameTexture = m.nameTexture;
	background = m.background;
	backgroundTexture = m.backgroundTexture;
	id = m.id;

	item = m.item;

	return *this;
}

void Menu::AddMenuItem(MenuItem* mi)
{
	item.push_back(mi);
}

std::vector<MenuItem*> Menu::GetItems()
{
	return item;
}

void Menu::SetItems(std::vector<MenuItem*> _item)
{
	item = _item;
}

const std::string& Menu::GetName()
{
	return name;
}

int Menu::GetId()
{
	return id;
}

void Menu::SetId(int _id)
{
	id = _id;
}

void Menu::SetName(const std::string& _name)
{
	name = _name;
	if (!name.empty())
	{
		nameTexture = TextureCache::GetTexture("menu\\" + name);
	}
	else
	{
		nameTexture = NULL;
	}
}

Texture* Menu::GetNameTexture()
{
	return nameTexture;
}

const std::string& Menu::GetBackground()
{
	return background;
}

void Menu::SetBackground(const std::string& _background)
{
	background = _background;
	if (!background.empty())
	{
		backgroundTexture = TextureCache::GetTexture("menu\\" + background);
	}
	else
	{
		backgroundTexture = NULL;
	}
}

Texture* Menu::GetBackgroundTexture()
{
	return backgroundTexture;
}

