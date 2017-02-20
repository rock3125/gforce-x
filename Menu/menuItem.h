//////////////////////////////////////////////////////////////////////
/// GRAVITY FORCE X
///
/// (c)Copyright Peter de Vocht, 2006
///
/// I'm releasing this code under GPL v2.1 (see included file lgpl.txt)
///
//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////

class MenuItem
{
public:
	MenuItem();
	~MenuItem();
	MenuItem(const MenuItem&);
	const MenuItem& operator=(const MenuItem&);

	// accessors
	const std::string& GetName();
	void SetName(const std::string& name);

	const std::string& GetAction();
	void SetAction(const std::string& action);

	bool GetShowLevels();
	void SetShowLevels(bool showLevels);

	bool GetShowShips();
	void SetShowShips(bool showShips);

	bool GetShowInputs();
	void SetShowInputs(bool showInputs);

	bool ControlMusic();
	void ControlMusic(bool controlMusic);

	bool ControlFullscreen();
	void ControlFullscreen(bool controlFullscreen);

	bool ControlResolution();
	void ControlResolution(bool controlResolution);

	int GetNumPlayers();
	void SetNumPlayers(int numPlayers);

	int GetPlayer();
	void SetPlayer(int player);

	int GetInputId();
	void SetInputId(int inputId);

	const std::string& UserText();
	void UserText(const std::string& action);

private:
	std::string	name;
	std::string action;
	std::string usertext;
	bool showLevels;
	int numPlayers;
	int player;
	int inputId;
	bool showShips;
	bool showInputs;
	bool controlMusic;
	bool controlFullscreen;
	bool controlResolution;
};

