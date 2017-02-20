//////////////////////////////////////////////////////////////////////
/// GRAVITY FORCE X
///
/// (c)Copyright Peter de Vocht, 2006
///
/// I'm releasing this code under GPL v2.1 (see included file lgpl.txt)
///
//////////////////////////////////////////////////////////////////////

#include "standardFirst.h"

#include "menu/menuLevel.h"
#include "d3d9/texture.h"

/////////////////////////////////////////////////////////////

MenuLevel::MenuLevel()
	: levelIcon(NULL)
{
	maxPlayers = 0;
	maxTeams = 0;
}

MenuLevel::~MenuLevel()
{
	levelIcon = NULL;
}

const std::string& MenuLevel::GetName()
{
	return nameStr;
}

void MenuLevel::SetName(const std::string& _nameStr)
{
	nameStr = _nameStr;
}

const std::string& MenuLevel::GetIcon()
{
	return iconStr;
}

void MenuLevel::SetIcon(const std::string& _iconStr)
{
	iconStr = _iconStr;
	if (!iconStr.empty())
	{
		levelIcon = TextureCache::GetTexture(iconStr);
	}
	else
	{
		levelIcon = NULL;
	}
}

const std::string& MenuLevel::GetFile()
{
	return fileStr;
}

void MenuLevel::SetFile(const std::string& _fileStr)
{
	fileStr = _fileStr;
}

int MenuLevel::GetMaxPlayers()
{
	return maxPlayers;
}

void MenuLevel::SetMaxPlayers(int _maxPlayers)
{
	maxPlayers = _maxPlayers;
}

int MenuLevel::GetMaxTeams()
{
	return maxTeams;
}

void MenuLevel::SetMaxTeams(int _maxTeams)
{
	maxTeams = _maxTeams;
}

Texture* MenuLevel::GetLevelIcon()
{
	return levelIcon;
}

void MenuLevel::SetLevelIcon(Texture* _levelIcon)
{
	levelIcon = _levelIcon;
}

