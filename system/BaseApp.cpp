#include "standardFirst.h"

#include "runtime/runtime.h"

#include "system/BaseApp.h"
#include "system/input.h"
#include "system/xml/XmlParser.h"
#include "system/sound/soundSystem.h"

#include "system/interpreter/SystemFacade.h"

#include "d3d9/textureCache.h"

BaseApp* BaseApp::singleton=NULL;

//////////////////////////////////////////////////////////

BaseApp::BaseApp()
	: currentRuntime(NULL)
	, facade(NULL)
{
	// init com
	CoInitialize(NULL);

	// special case of singleton - since the main MFC app
	// inherits from me, I already have a singleton (and one only!)
	PreCond(singleton==NULL);
	singleton=this;

	// setup facade
	facade = new SystemFacade();

	// make sure interface is up and running
	paused = false;

	playMusic = true;

	rect = D3DXVECTOR4(0,0,800,600);

	// init keyboard system
	left[0] = Input::SK_LEFT;
	right[0] = Input::SK_RIGHT;
	down[0] = Input::SK_DOWN;
	up[0] = Input::SK_UP;
	fire1[0] = ' ';

	left[1] = 'A';
	right[1] = 'D';
	down[1] = 'S';
	up[1] = 'W';
	fire1[1] = 'Q';

	elapsedTime = 0;
}

BaseApp::~BaseApp()
{
	paused = true;

	safe_delete(facade);

	// make sure we terminate all the system's singletons too
	TextureCache::Destroy();
	Interface::Destroy();

	CoUninitialize();
}

void BaseApp::SaveSettings()
{
	FileStreamer file(BaseStreamer::STREAM_WRITE);
	std::string fname=System::GetAppDirectory()+"gamesettings.xml";
	if (file.Open(fname))
	{
		file.WriteData("<settings>\n");
		file.WriteData("  <settings width=\"" + System::Int2Str(Width()) + "\" ");
		file.WriteData("height=\"" + System::Int2Str(Height()) + "\" ");
		std::string str = Fullscreen() ? "true" : "false";
		file.WriteData("fullscreen=\"" + str + "\" ");
		str = PlayMusic() ? "true" : "false";
		file.WriteData("music=\"" + str + "\" />\n");
		file.WriteData("</settings>\n");
	}
}

//
// parse the default/overrides
//
bool BaseApp::LoadSettings()
{
	// load app settings
	XmlParser* parser = new XmlParser();
	if (!parser->LoadAndParse(System::GetAppDirectory() + "gamesettings.xml"))
	{
		ShowErrorMessage("could not load \"gamesettings.xml\"", "init error");
		return false;
	}

	// get all elements in the settings block of the system
	std::vector<XmlNode*> children = parser->GetDocumentRoot()->GetChildren();
	std::vector<XmlNode*>::iterator pos = children.begin();
	while (pos != children.end())
	{
		XmlNode* child = *pos;
		if (child->GetTag() == "keys")
		{
			// check node has right attributes
			if (!child->HasName("left") || !child->HasName("right") || !child->HasName("up") ||
				!child->HasName("down") || !child->HasName("fire") || !child->HasName("keyboard"))
			{
				ShowErrorMessage("\"keys\" settings in \"settings.xml\" missing \"keyboard\", \"left\", \"right\", \"up\", \"down\", or \"fire\"", "init error");
				return false;
			}
			// setup keyboard mappings for game-play
			int keyboardId = System::Str2Int(child->GetValue("keyboard")) - 1;
			if (keyboardId >= 0 && keyboardId < BaseApp::MAX_KEYBOARD)
			{
				Up(keyboardId, Input::StringToKey(child->GetValue("up")));
				Down(keyboardId, Input::StringToKey(child->GetValue("down")));
				Left(keyboardId, Input::StringToKey(child->GetValue("left")));
				Right(keyboardId, Input::StringToKey(child->GetValue("right")));
				Fire1(keyboardId, Input::StringToKey(child->GetValue("fire")));
			}
		}
		else if (child->GetTag() == "settings")
		{
			// check node has right attributes
			if (!child->HasName("width") || !child->HasName("height") || !child->HasName("fullscreen") || 
				!child->HasName("music"))
			{
				ShowErrorMessage("\"screen\" settings in \"settings.xml\" missing \"width\", \"fullscreen\", \"height\" or \"music\"", "init error");
				return false;
			}
			// set runtime parameters
			Width(System::Str2Int(child->GetValue("width")));
			Height(System::Str2Int(child->GetValue("height")));
			Fullscreen(System::ToLower(child->GetValue("fullscreen")) == "true");
			playMusic = System::ToLower(child->GetValue("music")) == "true";
		}
		pos++;
	}
	return true;
}

bool BaseApp::PlayMusic()
{
	return playMusic;
}

void BaseApp::PlayMusic(bool playMusic)
{
	this->playMusic = playMusic;
	if (playMusic)
	{
		SoundSystem::Get()->ST_Play();
	}
	else
	{
		SoundSystem::Get()->ST_Stop();
	}
}

BaseApp* BaseApp::Get()
{
	PreCond(singleton != NULL);
	return singleton;
}

Runtime* BaseApp::GetCurrentRuntime()
{
	return currentRuntime;
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

void BaseApp::ResetResolution(bool fullscreen, int width, int height)
{
	this->width = width;
	this->height = height;
	this->fullscreen = fullscreen;
}

