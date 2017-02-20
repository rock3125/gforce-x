#pragma once

#include "d3d9/Interface.h"

class Runtime;
class Level;
class WorldObject;
class SystemFacade;

//////////////////////////////////////////////////////////

class BaseApp
{
public:
	BaseApp();
	virtual ~BaseApp();

	// access main app
	static BaseApp& GetApp();

	// get client rect of window
	const D3DXVECTOR4& GetRect();
	void SetRect(const D3DXVECTOR4& rect);

	// get set paused status of this app
	bool GetPaused() const;
	void SetPaused(bool _paused);

	// access runtime
	virtual Runtime* GetCurrentRuntime();
	virtual void SetCurrentRuntime(Runtime* currentRuntime);

	// access function interface
	virtual SystemFacade* GetFacade();

	// access current level
	Level* GetCurrentLevel();
	void SetCurrentLevelPtr(Level* level);

	int Width()
	{
		return width;
	}
	void Width(int width)
	{
		this->width = width;
	}

	int Height()
	{
		return height;
	}
	void Height(int height)
	{
		this->height = height;
	}

	bool Fullscreen()
	{
		return fullscreen;
	}
	void Fullscreen(bool fullscreen)
	{
		this->fullscreen = fullscreen;
	}

	int Left()
	{
		return left;
	}
	void Left(int left)
	{
		this->left = left;
	}

	int Right()
	{
		return right;
	}
	void Right(int right)
	{
		this->right = right;
	}

	int Up()
	{
		return up;
	}
	void Up(int up)
	{
		this->up = up;
	}

	int Down()
	{
		return down;
	}
	void Down(int down)
	{
		this->down = down;
	}

	int Fire1()
	{
		return fire1;
	}
	void Fire1(int fire1)
	{
		this->fire1 = fire1;
	}

	int Fire2()
	{
		return fire2;
	}
	void Fire2(int fire2)
	{
		this->fire2 = fire2;
	}

	// services to be implemented for the editor only really - but the runtime can
	// just choose not to implement them
	virtual std::string GetFilenameForLoad(const std::string& filter,const std::string& extn,const std::string& directory) = 0;
	virtual std::string GetFilenameForSave(const std::string& filter,const std::string& extn,const std::string& directory) = 0;
	virtual bool ColourPicker(const D3DXCOLOR& in,D3DXCOLOR& out) = 0;

	// show an error message system wide
	virtual void ShowErrorMessage(std::string msg,std::string title) = 0;

	// pump message queue
	virtual void ForceProcessMessages() = 0;

	// exit system
	virtual void Quit() = 0;

	// exit current game in progress
	virtual void QuitGame() = 0;

	// load a level from file
	virtual Level* Load(const std::string& fname) = 0;

protected:
	D3DXVECTOR4		rect;

	// has the app been paused?
	bool			paused;

	// runtime accessor
	Runtime*		currentRuntime;

	// current level
	Level*			currentLevel;

	// system interface
	SystemFacade*	facade;

	// application window size
	int				width;
	int				height;

	bool			fullscreen;

	// basic key-board (defineable)
	int				left;
	int				right;
	int				up;
	int				down;
	int				fire1;
	int				fire2;

private:
	// special case of singleton - since the main MFC app
	// inherits from me, I already have a singleton (and one only!)
	static BaseApp* singleton;
};

