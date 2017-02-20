//////////////////////////////////////////////////////////////////////
/// GRAVITY FORCE X
///
/// (c)Copyright Peter de Vocht, 2006
///
/// I'm releasing this code under GPL v2.1 (see included file lgpl.txt)
///
//////////////////////////////////////////////////////////////////////

#include "standardFirst.h"

#include "runtime/runtime.h"

#include "system/baseApp.h"
#include "system/objectFactory.h"

#include "system/model/camera.h"
#include "system/model/level.h"
#include "system/modelImporter.h"
#include "system/model/model.h"
#include "system/network/networkClient.h"
#include "system/network/baseProtocolPackets.h"
#include "system/network/dataPacket.h"
#include "system/sound/soundSystem.h"
#include "system/signals/levelObserver.h"
#include "system/xml/XmlParser.h"

#include "game/ship.h"
#include "game/base.h"
#include "game/modelMap.h"
#include "game/commonModels.h"
#include "game/indestructableRegion.h"
#include "game/Turret.h"
#include "game/Water.h"
#include "game/shipData.h"

#include "menu/menuManager.h"

////////////////////////////////////////////////////

Runtime::Runtime()
	: currentLevel(NULL)
	, modelMap(NULL)
	, menuManager(NULL)
	, camera(NULL)
{
	isNetworkGame = false;
	playerId = 0;

	// game type to play
	GP_SetGameType(GT_FIRST_OUT_OF_FIVE);
	numPlayers = 0;

	for (int i=0; i < MAX_PLAYERS; i++)
	{
		inputIds[i] = 0;
		shipIds[i] = 0;
	}

	// set new level
	currentLevel=new Level();
	LevelObserver::NotifyObservers(currentLevel);

	// create the menu manager
	menuManager = new MenuManager();
	activity = ACT_INMENU;

	FRAME_RATE_MS = 0.01f;
}

Runtime::~Runtime()
{
	modelMap = NULL;
	safe_delete(currentLevel);
	safe_delete(menuManager);
	safe_delete(camera);
}

Level* Runtime::Load(const std::string& fname)
{
	XmlParser p;
	std::string strippedFname = System::RemoveDataDirectory(fname);
	bool result = p.LoadAndParse(fname);
	if (result)
	{
		XmlNode* root = p.GetDocumentRoot();

		safe_delete(currentLevel);
		ClearCurrentLevel();

		currentLevel = new Level();
		currentLevel->Read(root->GetChild(currentLevel->Signature()));

		RebuildGameObjects();

		// and the current camera
		camera = new Camera();
		camera->Read(root->GetChild(camera->Signature()));

		LevelObserver::NotifyObservers(currentLevel);

		return currentLevel;
	}
	else
	{
		GP_SetErrorMessage("error loading level " + fname + " (" + p.GetError() + ")");
	}
	return NULL;
}

void Runtime::LoadShipDefinitions(const std::string& filename)
{
	safe_delete_stl_array(shipDefinitions);

	XmlParser parser;
	if (!parser.LoadAndParse(filename))
	{
		throw new Exception(parser.GetError());
	}

	XmlNode* root = parser.GetDocumentRoot();
	if (root != NULL)
	{
		// parser xml menu document
		std::vector<XmlNode*> children = root->GetChildren();
		for (int i=0; i < children.size(); i++)
		{
			if (children[i]->GetTag() == "ship")
			{
				std::string icon = children[i]->GetValue("icon");
				std::string model = children[i]->GetValue("model");

				ShipData* ship1 = new ShipData();
				ship1->SetIcon(icon);
				ship1->SetModel(model);

				ship1->id = i;

				// ship settings
				ship1->maxSpeed = System::Str2Float(children[i]->GetValue("maxspeed"));
				ship1->maxAccel = System::Str2Float(children[i]->GetValue("maxacceleration"));
				ship1->baseOffset = System::Str2Float(children[i]->GetValue("offset"));
				ship1->accel = System::Str2Float(children[i]->GetValue("acceleration"));
				ship1->rotationalVelocity  = System::Str2Float(children[i]->GetValue("rotationalvelocity"));
				ship1->fuelUsage = System::Str2Float(children[i]->GetValue("fuelusage"));
				ship1->numBullets = System::Str2Int(children[i]->GetValue("bullets"));
				ship1->numMissiles = System::Str2Int(children[i]->GetValue("missiles"));
				ship1->shieldStrength = System::Str2Float(children[i]->GetValue("shieldstrength"));

				// missile settings
				ship1->missileacceleration = System::Str2Float(children[i]->GetValue("missileacceleration"));
				ship1->missilefuel = System::Str2Float(children[i]->GetValue("missilefuel"));
				ship1->missilefuelusage = System::Str2Float(children[i]->GetValue("missilefuelusage"));
				ship1->missilemaxspeed = System::Str2Float(children[i]->GetValue("missilemaxspeed"));
				ship1->missilestrength = System::Str2Float(children[i]->GetValue("missilestrength"));
				ship1->missileforce = System::Str2Float(children[i]->GetValue("missileforce"));

				std::string bulletcolour = children[i]->GetValue("bulletcolour");
				ship1->bulletColour = D3DXCOLOR(System::Str2Float(System::GetItem(bulletcolour,',',0)),
												System::Str2Float(System::GetItem(bulletcolour,',',1)),
												System::Str2Float(System::GetItem(bulletcolour,',',2)),1);

				// get model
				ModelImporter import;
				ship1->shipModel = import.Import(System::GetDataDirectory() + model);
				if (ship1->shipModel == NULL)
				{
					throw new Exception("could not load ship-model \"" + model + "\"");
				}

				ModelImporter bullet;
				ship1->bulletModel = import.Import(System::GetDataDirectory() + "objects\\bullet.wrl");
				if (ship1->bulletModel == NULL)
				{
					throw new Exception("could not load bullet-model \"objects\\bullet.wrl\"");
				}

				// add menu
				shipDefinitions.push_back(ship1);
			}
		}
	}
}

void Runtime::SetCurrentLevel(Level* level)
{
	safe_delete(currentLevel);
	ClearCurrentLevel();
	currentLevel = level;
	LevelObserver::NotifyObservers(currentLevel);
}

void Runtime::LoadMenus()
{
	menuManager->Load(System::GetDataDirectory() + "menu\\menus.xml");
}

Camera* Runtime::GetCamera()
{
	return camera;
}

void Runtime::SetCamera(Camera* camera)
{
	safe_delete(this->camera);
	this->camera = camera;
}

void Runtime::FocusObject(WorldObject* obj)
{
}

void Runtime::PacketReceived(BasePacket* packet)
{
	switch (packet->id)
	{
		// receive a message that another player quit - remove this player
	case PID_SERVER_OTHERPLAYERQUIT:
		{
			PacketServerOtherPlayerQuit* playerQuit = dynamic_cast<PacketServerOtherPlayerQuit*>(packet);
			// re-constitute the player's minus the one quitting
			std::vector<Ship*> newList;
			for (int i=0; i < ships.size(); i++)
			{
				if (ships[i]->PlayerId() != playerQuit->playerId)
				{
					newList.push_back(ships[i]);
				}
			}
			ships = newList;
			break;
		}
		// receive another player's position packet
	case DP_POSITION:
		{
			DataPosition* dp = dynamic_cast<DataPosition*>(packet);
			for (int i=0; i < ships.size(); i++)
			{
				Ship* ship = ships[i];
				if (ship->PlayerId() == dp->playerId)
				{
					ship->ReadFromPacket(dp);
					break;
				}
			}
			break;
		}
		// receive a welcome list of players - reconstitute the players
	case PID_SERVER_WELCOME:
		{
			GP_ConfirmNetworkPlayers(dynamic_cast<PacketServerWelcome*>(packet));
			break;
		}
	}
}

void Runtime::EventLogic(double time)
{
	if (isNetworkGame)
	{
		NetworkClient* client = NetworkClient::Get();

		client->EventLogic();

		// are we ready to join?
		if (client->IsWaitingToJoin())
		{
			levelFilename = client->LevelName();
			playerId = client->GetPlayerId();

			Load(System::GetDataDirectory() + levelFilename);
			if (currentLevel == NULL)
			{
				GP_StopNetworkGame();
				GP_SetErrorMessage("Error loading level " + levelFilename);
				return;
			}

			// setup player's ship & input
			ships.clear();
			shipDefinitions[shipIds[0]]->SetControl(inputIds[0]);
			Ship* ship = shipDefinitions[shipIds[0]]->GetShip();
			ship->PlayerId(playerId);
			ships.push_back(ship);

			// set the ships in the level
			if (ships.size() <= bases.size())
			{
				for (int i=0; i < ships.size(); i++)
				{
					Ship* ship = ships[i];
					if (i == 0)
					{
						SetSelectedItem(ship);
					}

					Base* base = bases[ship->PlayerId()];
					D3DXVECTOR3 pos = base->GetPosition();
					pos.z = 5;
					pos.y += ships[i]->GetBaseOffset() * 2;
					ship->SetPosition(pos);
					ship->SetInitialPosition(pos);
				}
			}

			SetSelectedItem(ship);
			GP_StartNewGame();
			client->AcknowledgeReady(shipIds[0], 0);
		}

		// are we go go go?
		if (client->IsActive() && activity == ACT_INGAME)
		{
			// send my data to the server
			DataPosition packet(playerId);
			Ship* ship = ships[0];
			ship->WriteToPacket(&packet);
			// notify everyone else of my position
			if (!client->SendPacketToServer(&packet))
			{
				GP_StopNetworkGame();
				GP_SetErrorMessage("Network error sending data to client");
			}
		}
	}
}

void Runtime::RebuildGameObjects()
{
	if (currentLevel!=NULL)
	{
		bases.clear();
		ships.clear();
		indestructableRegions.clear();
		turrets.clear();
		water.clear();

		modelMap = NULL;

		std::vector<WorldObject*> objects = currentLevel->GetObjects();

		// get all object and arrange them into their appropriate game arrays
		for (int i=1; i<objects.size(); i++)
		{
			WorldObject* worldObject = objects[i];
			if (ObjectFactory::Isa(worldObject, WorldObject::TYPE_MODELMAP))
			{
				modelMap = dynamic_cast<ModelMap*>(worldObject);
			}
			else if (ObjectFactory::Isa(worldObject, WorldObject::TYPE_BASE))
			{
				bases.push_back(dynamic_cast<Base*>(worldObject));
			}
			else if (ObjectFactory::Isa(worldObject, WorldObject::TYPE_SHIP))
			{
				ships.push_back(dynamic_cast<Ship*>(worldObject));
			}
			else if (ObjectFactory::Isa(worldObject, WorldObject::TYPE_INDESTRUCTABLEREGION))
			{
				indestructableRegions.push_back(dynamic_cast<IndestructableRegion*>(worldObject));
			}
			else if (ObjectFactory::Isa(worldObject, WorldObject::TYPE_TURRET))
			{
				turrets.push_back(dynamic_cast<Turret*>(worldObject));
			}
			else if (ObjectFactory::Isa(worldObject, WorldObject::TYPE_WATER))
			{
				water.push_back(dynamic_cast<Water*>(worldObject));
			}
		}

		// check level is valid
		if (modelMap == NULL || bases.size() < 2)
		{
			GP_SetErrorMessage("this level does not have any bases or a map");
		}

	}
}

void Runtime::ClearCurrentLevel()
{
	bases.clear();
	ships.clear();
	indestructableRegions.clear();
	water.clear();
	modelMap = NULL;
}

// stop a network game
void Runtime::GP_StopNetworkGame()
{
	NetworkClient::Get()->StopClient();
}

void Runtime::GP_ConfirmNetworkPlayers(PacketServerWelcome* packet)
{
	gameType = packet->gameType;

	// mark other players
	networkPlayers.clear();
	for (int i = 0; i < packet->cntr; i++)
	{
		NetworkPlayer np;
		np.playerId = packet->playerId[i];
		np.name = packet->name[i];
		np.shipId = packet->shipId[i];
		np.teamId = packet->teamId[i];
		np.score = packet->score[i];
		networkPlayers.push_back(np);
	}

	// create ships for all of them
	Ship* ship = ships[0];
	ship->IsNetworkControlled(false);
	ships.clear();
	ships.push_back(ship);

	for (int i=0; i < networkPlayers.size(); i++)
	{
		if (networkPlayers[i].playerId != playerId)
		{
			Ship* ship = shipDefinitions[networkPlayers[i].shipId]->GetShip();
			ship->PlayerId(networkPlayers[i].playerId);
			ship->IsNetworkControlled(true);
			ships.push_back(ship);
		}
		else
		{
			ships[0]->MyScore(networkPlayers[i].score);
		}
	}

	// set the ships in the level
	if (ships.size() <= bases.size())
	{
		for (int i = 0; i < ships.size(); i++)
		{
			Ship* ship = ships[i];
			if (i == 0)
			{
				SetSelectedItem(ship);
			}

			Base* base = bases[ship->PlayerId()];
			D3DXVECTOR3 pos = base->GetPosition();
			pos.z = 5;
			pos.y += ships[i]->GetBaseOffset() * 2;
			ship->SetPosition(pos);
			ship->SetInitialPosition(pos);
		}
	}
	SetSelectedItem(ship);
}

void Runtime::GP_StopGame()
{
	isNetworkGame = false;
	numPlayers = 0;
	NetworkClient::Get()->StopClient();
}

// start hosting a new network game
bool Runtime::GP_StartNetworkGame(std::string playerName, std::string server, int port)
{
	if (NetworkClient::Get()->StartClient(playerName, server, port))
	{
		// stop main soundtrack in menu
		SoundSystem::Get()->ST_Stop();

		isNetworkGame = true;
		return true;
	}
	else
	{
		GP_SetErrorMessage("Network error: could not start network game");
	}
	return false;
}

// start the background server
void Runtime::GP_StartServer(int gameType, int numPoints)
{
	// nShowCmd = 2 = minimized
	std::string command = System::GetAppDirectory() + "reflector.exe";
	std::string directory = System::GetAppDirectory();
	std::string params = levelFilename + " " + System::Int2Str(gameType) + " " + System::Int2Str(numPoints);
	::ShellExecute(NULL, "open", command.c_str(), params.c_str(), directory.c_str(), 2);
}

// start a normal game
bool Runtime::GP_StartGame()
{
	isNetworkGame = false;
	if (numPlayers == 0)
	{
		return false;
	}

	// stop main soundtrack in menu
	SoundSystem::Get()->ST_Stop();

	// load the level
	Load(System::GetDataDirectory() + levelFilename);
	if (currentLevel == NULL)
	{
		return false;
	}

	// set controls on ship defn
	ships.clear();
	for (int i=0; i < numPlayers; i++)
	{
		shipDefinitions[shipIds[i]]->SetControl(inputIds[i]);
		ships.push_back(shipDefinitions[shipIds[i]]->GetShip());
	}

	// set the ships in the level
	if (ships.size() <= bases.size())
	{
		for (int i=0; i < ships.size(); i++)
		{
			Ship* ship = ships[i];
			if (i == 0)
			{
				SetSelectedItem(ship);
			}

			Base* base = bases[i];
			D3DXVECTOR3 pos = base->GetPosition();
			pos.z = 5;
			pos.y += ships[i]->GetBaseOffset() * 2;
			ship->SetPosition(pos);
			ship->SetInitialPosition(pos);
		}
	}
	GP_StartNewGame();
	return true;
}

// set game type (first out of 10, etc)
void Runtime::GP_SetGameType(int gameType)
{
	this->gameType = gameType;
}

int Runtime::GP_GetGameType()
{
	return gameType;
}

// set screen type (split horizontal, vertical, fullscreen)
void Runtime::GP_SetScreenType(int screenType)
{
	this->screenType = screenType;
}

// set player's ship
void Runtime::GP_SetShip(int player, int shipId)
{
	if (player >= numPlayers)
	{
		throw new Exception("exceeded number of players");
	}
	shipIds[player] = shipId;
}

// set player's input method
void Runtime::GP_SetInput(int player, int inputId)
{
	if (player >= numPlayers)
	{
		throw new Exception("exceeded number of players");
	}
	inputIds[player] = inputId;
}

void Runtime::GP_SetLevel(std::string levelFilename)
{
	this->levelFilename = levelFilename;
}

void Runtime::GP_NumPlayers(int numPlayers)
{
	if (numPlayers < 1 || numPlayers > MAX_PLAYERS)
	{
		throw new Exception("incorrect number of players");
	}
	this->numPlayers = numPlayers;
}

void Runtime::GP_SetErrorMessage(std::string errorString)
{
	this->errorString = errorString;

	// stop network (just in case its active)
	NetworkClient::Get()->StopClient();

	// notify all listeners that the level is gone
	LevelObserver::NotifyObservers((Level*)NULL);

	// remove all level specifics
	ClearCurrentLevel();

	activity = ACT_ERROR;
}

void Runtime::GP_UpdateScore(void* _ship)
{
	Ship* ship = (Ship*)_ship;

	// reward the other players with a point when this ship gets hit
	std::vector<Ship*>::iterator pos = ships.begin();
	while (pos != ships.end())
	{
		if (*pos != ship)
		{
			Ship* scoringShip = *pos;
			scoringShip->MyScore(scoringShip->MyScore() + 1);
		}
		pos++;
	}
}

// play one of the possible in-game sounds
void Runtime::GP_PlaySound(SoundsEnum sound, bool checkIsAlreadyPlaying)
{
	SoundSystem::Get()->PlayStoreSample(sound, false, checkIsAlreadyPlaying);
}

void Runtime::GP_StartNewGame()
{
	winner = NULL;
	activity = ACT_INGAME;
}

void Runtime::GP_QuitGame()
{
	// remove level from the app
	currentLevel = NULL;

	// stop network (just in case its active)
	NetworkClient::Get()->StopClient();

	// notify all listeners that the level is gone
	LevelObserver::NotifyObservers((Level*)NULL);

	// remove all level specifics
	ClearCurrentLevel();

	// first menu, first item
	menuManager->SetActiveMenu(0,0);

	// start soundtrack again
	if (BaseApp::Get()->PlayMusic())
	{
		SoundSystem::Get()->ST_Play();
	}

	activity = ACT_INMENU;
}

