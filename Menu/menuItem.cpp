//////////////////////////////////////////////////////////////////////
/// GRAVITY FORCE X
///
/// (c)Copyright Peter de Vocht, 2006
///
/// I'm releasing this code under GPL v2.1 (see included file lgpl.txt)
///
//////////////////////////////////////////////////////////////////////

#include "standardFirst.h"

#include "menu/menuItem.h"

////////////////////////////////////////////////////////////////////

MenuItem::MenuItem()
{
	showLevels = false;
	showShips = false;
	showInputs = false;
	numPlayers = 0;
	player = 0;
	inputId = 0;
	controlMusic = false;
	controlFullscreen = false;
	controlResolution = false;
}

MenuItem::~MenuItem()
{
}

MenuItem::MenuItem(const MenuItem& mi)
{
	operator=(mi);
}

const MenuItem& MenuItem::operator=(const MenuItem& mi)
{
	name = mi.name;
	action = mi.action;
	showLevels = mi.showLevels;
	showShips = mi.showShips;
	numPlayers = mi.numPlayers;
	player = mi.player;
	inputId = mi.inputId;
	showInputs = mi.showInputs;
	controlMusic = mi.controlMusic;
	controlFullscreen = mi.controlFullscreen;
	controlResolution = mi.controlResolution;
	return *this;
}

const std::string& MenuItem::GetName()
{
	return name;
}

void MenuItem::SetName(const std::string& _name)
{
	name = _name;
}

bool MenuItem::GetShowLevels()
{
	return showLevels;
}

void MenuItem::SetShowLevels(bool _showLevels)
{
	showLevels = _showLevels;
}

bool MenuItem::GetShowInputs()
{
	return showInputs;
}

void MenuItem::SetShowInputs(bool _showInputs)
{
	showInputs = _showInputs;
}

bool MenuItem::GetShowShips()
{
	return showShips;
}

void MenuItem::SetShowShips(bool _showShips)
{
	showShips = _showShips;
}

int MenuItem::GetNumPlayers()
{
	return numPlayers;
}

bool MenuItem::ControlMusic()
{
	return controlMusic;
}

void MenuItem::ControlMusic(bool controlMusic)
{
	this->controlMusic = controlMusic;
}

bool MenuItem::ControlFullscreen()
{
	return controlFullscreen;
}

void MenuItem::ControlFullscreen(bool controlFullscreen)
{
	this->controlFullscreen = controlFullscreen;
}

bool MenuItem::ControlResolution()
{
	return controlResolution;
}

void MenuItem::ControlResolution(bool controlResolution)
{
	this->controlResolution = controlResolution;
}

void MenuItem::SetNumPlayers(int _numPlayers)
{
	numPlayers = _numPlayers;
}

const std::string& MenuItem::GetAction()
{
	return action;
}

void MenuItem::SetAction(const std::string& _action)
{
	action = _action;
}

int MenuItem::GetPlayer()
{
	return player;
}

void MenuItem::SetPlayer(int _player)
{
	player = _player;
}

int MenuItem::GetInputId()
{
	return inputId;
}

void MenuItem::SetInputId(int _inputId)
{
	inputId = _inputId;
}

const std::string& MenuItem::UserText()
{
	return usertext;
}

void MenuItem::UserText(const std::string& usertext)
{
	this->usertext = usertext;
}

