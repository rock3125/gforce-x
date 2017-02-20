#include "standardFirst.h"

#include "runtime/runtime.h"

#include "system/BaseApp.h"
#include "system/input.h"
#include "system/model/level.h"
#include "system/signals/levelObserver.h"

#include "system/interpreter/SystemFacade.h"

#include "d3d9/textureCache.h"

BaseApp* BaseApp::singleton=NULL;

//////////////////////////////////////////////////////////

BaseApp::BaseApp()
	: currentRuntime(NULL)
	, currentLevel(NULL)
	, facade(NULL)
{
	// special case of singleton - since the main MFC app
	// inherits from me, I already have a singleton (and one only!)
	PreCond(singleton==NULL);
	singleton=this;

	currentLevel=new Level();
	LevelObserver::NotifyObservers(currentLevel);

	// setup facade
	facade = new SystemFacade();

	// make sure interface is up and running
	Interface::GetI();
	paused = false;

	rect = D3DXVECTOR4(0,0,800,600);

	// init keyboard system
	left = Input::SK_LEFT;
	right = Input::SK_RIGHT;
	down = Input::SK_DOWN;
	up = Input::SK_UP;
	fire1 = ' ';
}

BaseApp::~BaseApp()
{
	paused = true;

	safe_delete(currentLevel);
	safe_delete(facade);

	// make sure we terminate all the system's singletons too
	TextureCache::Destroy();
	Interface::Destroy();
}

BaseApp& BaseApp::GetApp()
{
	PreCond(singleton != NULL);
	return *singleton;
}

Runtime* BaseApp::GetCurrentRuntime()
{
	return currentRuntime;
}

void BaseApp::SetCurrentRuntime(Runtime* _currentRuntime)
{
	currentRuntime = _currentRuntime;
}

SystemFacade* BaseApp::GetFacade()
{
	return facade;
}

bool BaseApp::GetPaused() const
{
	return paused;
}

void BaseApp::SetPaused(bool _paused)
{
	paused = _paused;
}

const D3DXVECTOR4& BaseApp::GetRect()
{
	return rect;
}

void BaseApp::SetRect(const D3DXVECTOR4& _rect)
{
	rect = _rect;
}

Level* BaseApp::GetCurrentLevel()
{
	return currentLevel;
}

void BaseApp::SetCurrentLevelPtr(Level* level)
{
	if (currentLevel != level && level != NULL)
	{
		LevelObserver::NotifyObservers((Level*)NULL);
		safe_delete(currentLevel);
		currentLevel = level;
		LevelObserver::NotifyObservers(currentLevel);
	}
}

