#pragma once

#include "d3d9/Interface.h"

class Runtime;
class Level;
class WorldObject;
class SystemFacade;
class SoundSystem;

//////////////////////////////////////////////////////////

class BaseApp
{
public:
	BaseApp();
	virtual ~BaseApp();

	// access main app
	static BaseApp* Get();

	// allow up to two keyboard mappings
	enum
	{
		MAX_KEYBOARD = 2,
	};

	// get client rect of window
	const D3DXVECTOR4& GetRect();
	void SetRect(const D3DXVECTOR4& rect);

	// get set paused status of this app
	bool GetPaused() const;
	void SetPaused(bool _paused);

	// access runtime
	virtual Runtime* GetCurrentRuntime();

	// access function interface
	virtual SystemFacade* GetFacade();

	bool PlayMusic();
	void PlayMusic(bool playMusic);

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

	int Left(int kbId)
	{
		return left[kbId];
	}
	void Left(int kbId, int left)
	{
		this->left[kbId] = left;
	}

	int Right(int kbId)
	{
		return right[kbId];
	}
	void Right(int kbId, int right)
	{
		this->right[kbId] = right;
	}

	int Up(int kbId)
	{
		return up[kbId];
	}
	void Up(int kbId, int up)
	{
		this->up[kbId] = up;
	}

	int Down(int kbId)
	{
		return down[kbId];
	}
	void Down(int kbId, int down)
	{
		this->down[kbId] = down;
	}

	int Fire1(int kbId)
	{
		return fire1[kbId];
	}
	void Fire1(int kbId, int fire1)
	{
		this->fire1[kbId] = fire1;
	}

	int Fire2(int kbId)
	{
		return fire2[kbId];
	}
	void Fire2(int kbId, int fire2)
	{
		this->fire2[kbId] = fire2;
	}

	// save xml settings/user preferences
	void SaveSettings();

	// load the user's settings
	bool LoadSettings();

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

	// set another resolution (& reload all)
	void ResetResolution(bool fullscreen, int width, int height);

protected:
	D3DXVECTOR4		rect;

	// has the app been paused?
	bool			paused;

	// runtime accessor
	Runtime*		currentRuntime;

	// system scripting interface 
	SystemFacade*	facade;

	// application window & size
	HWND			hWnd;

	int				width;
	int				height;

	// fullscreen app?
	bool			fullscreen;

	// play music?
	bool			playMusic;

	// frame timer
	float			elapsedTime;

	// basic key-boards (defineable)
	int				left[MAX_KEYBOARD];
	int				right[MAX_KEYBOARD];
	int				up[MAX_KEYBOARD];
	int				down[MAX_KEYBOARD];
	int				fire1[MAX_KEYBOARD];
	int				fire2[MAX_KEYBOARD];

private:
	// special case of singleton - since the main MFC app
	// inherits from me, I already have a singleton (and one only!)
	static BaseApp* singleton;
};

