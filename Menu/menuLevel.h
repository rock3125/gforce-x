//////////////////////////////////////////////////////////////////////
/// GRAVITY FORCE X
///
/// (c)Copyright Peter de Vocht, 2006
///
/// I'm releasing this code under GPL v2.1 (see included file lgpl.txt)
///
//////////////////////////////////////////////////////////////////////

#pragma once

class Texture;

///////////////////////////////////////////////////////////

class MenuLevel
{
	MenuLevel(const MenuLevel&);
	const MenuLevel& operator=(const MenuLevel&);
public:
	MenuLevel();
	~MenuLevel();

	const std::string& GetName();
	void SetName(const std::string& name);

	const std::string& GetIcon();
	void SetIcon(const std::string& name);

	const std::string& GetFile();
	void SetFile(const std::string& name);

	int GetMaxPlayers();
	void SetMaxPlayers(int maxPlayers);

	int GetMaxTeams();
	void SetMaxTeams(int maxTeams);

	Texture* GetLevelIcon();
	void SetLevelIcon(Texture* levelIcon);

private:
	std::string nameStr;
	std::string iconStr;
	std::string fileStr;
	int maxPlayers;
	int maxTeams;

	Texture* levelIcon;
};

