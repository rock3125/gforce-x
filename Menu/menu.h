//////////////////////////////////////////////////////////////////////
/// GRAVITY FORCE X
///
/// (c)Copyright Peter de Vocht, 2006
///
/// I'm releasing this code under GPL v2.1 (see included file lgpl.txt)
///
//////////////////////////////////////////////////////////////////////

#pragma once

class MenuItem;

//////////////////////////////////////////////////////////////

class Menu
{
public:
	Menu();
	~Menu();
	Menu(const Menu&);
	const Menu& operator=(const Menu&);

	// accessors
	std::vector<MenuItem*> GetItems();
	void SetItems(std::vector<MenuItem*> item);

	// add a single item
	void AddMenuItem(MenuItem* item);

	const std::string& GetName();
	void SetName(const std::string& name);

	int GetId();
	void SetId(int id);

	const std::string& GetBackground();
	void SetBackground(const std::string& background);

	// get texture
	Texture* GetBackgroundTexture();
	Texture* GetNameTexture();

private:
	int id;
	std::string name;
	std::string background;

	Texture*	backgroundTexture;
	Texture*	nameTexture;

	std::vector<MenuItem*> item;
};
