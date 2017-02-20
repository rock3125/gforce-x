//////////////////////////////////////////////////////////////////////
/// GRAVITY FORCE X
///
/// (c)Copyright Peter de Vocht, 2006
///
/// I'm releasing this code under GPL v2.1 (see included file lgpl.txt)
///
//////////////////////////////////////////////////////////////////////

#include "standardFirst.h"

#include "runtime/GF10Runtime.h"

#include "system/BaseApp.h"
#include "system/objectFactory.h"

#include "system/model/camera.h"
#include "system/model/level.h"
#include "system/model/mesh.h"
#include "system/model/model.h"
#include "system/keyboard.h"
#include "system/gamePad.h"

#include "system/network/networkClient.h"

#include "menu/menuManager.h"

#include "game/ship.h"
#include "game/base.h"
#include "game/modelMap.h"
#include "game/commonModels.h"
#include "game/indestructableRegion.h"
#include "game/Turret.h"
#include "game/Water.h"

#include "system/signals/addObjectObserver.h"

#include "d3d9/interface.h"
#include "d3d9/texture.h"

////////////////////////////////////////////////////

GF10Runtime::GF10Runtime()
	: selectedItem(NULL)
{
	// background clear colour
	float col = 0.0f;
	backgroundColour = D3DXCOLOR(col,col,col,0);

	// grid colours
	lightColour = D3DXCOLOR(0.4f,0.4f,0.4f,0);
	darkColour = D3DXCOLOR(0.2f,0.2f,0.2f,0);
	white = D3DXCOLOR(1,1,1,1);

	wireframe = false;
	cullCCW = true;

	screenType = Runtime::SC_HORIZONTAL;

	// load ships
	LoadShipDefinitions(System::GetDataDirectory() + "ships\\ships.xml");
	if (shipDefinitions.size() == 0)
	{
		throw new Exception("could not load any ships");
	}
	// set these values into the menu system
	menuManager->SetShipDefinitions(shipDefinitions);

	// escape key
	activity = ACT_INMENU;
}

GF10Runtime::~GF10Runtime()
{
	// release common models
	CommonModels::Destroy();
	safe_delete(camera);
}

void GF10Runtime::SetSelectedItem(WorldObject* _selectedItem)
{
	selectedItem = _selectedItem;
}

bool GF10Runtime::GetWireframe()
{
	return wireframe;
}

void GF10Runtime::SetWireframe(bool _wireframe)
{
	wireframe = _wireframe;
}

bool GF10Runtime::GetCullCCW()
{
	return cullCCW;
}

void GF10Runtime::SetCullCCW(bool _cullCCW)
{
	cullCCW = _cullCCW;
}

void GF10Runtime::Initialise()
{
	Device* dev=Interface::GetDevice();

	if (camera==NULL)
	{
		camera = new Camera();
		camera->Activate();
	}

	// create common models at startup
	CommonModels::Get();
}

void GF10Runtime::SetActivePlayer(int shipId)
{
	PreCond(shipId < ships.size());
	selectedItem = ships[shipId];
	ships[shipId]->SetupKeyboard(0);
}

void GF10Runtime::EventInit()
{
	if (currentLevel!=NULL)
	{
		std::vector<WorldObject*> objects = currentLevel->GetObjects();

		// draw all bounding boxes of all objects for now (exepting the root)
		for (int i=1; i<objects.size(); i++)
		{
			try
			{
				objects[i]->EventInit();
			}
			catch (Exception* e)
			{
				objects[i]->SetScriptEnabled(false);
				std::string str = "Object "+objects[i]->GetName()+" threw an exception\n";
				str = str + "(" + e->Message() + ")\n";
				str = str + "this object's script is now disabled";
				BaseApp::Get()->ShowErrorMessage(str,"Init Script runtime error");
			}
		}
	}
}

void GF10Runtime::EventLogic(double frameTime)
{
	// call down - always - handle networking if applicable
	Runtime::EventLogic(frameTime);
	// handle game-pad polling
	GamePad::Get()->EventLogic(frameTime);
	// for keyboard handling
	Keyboard* keyboard = Keyboard::Get();

	switch (activity)
	{
	case ACT_INMENU:
		{
			// menu system
			menuManager->EventLogic(frameTime);
			break;
		}
	case ACT_ERROR:
		{
			// press enter to continue/leave error mode
			if (keyboard->KeyPress() == Input::SK_ESCAPE)
			{
				GP_QuitGame();
				return;
			}
			break;
		}
	case ACT_ASKEXITGAME:
		{
			int key = keyboard->KeyPress();
			switch (key)
			{
			case Input::SK_ESCAPE:
				{
					GP_QuitGame();
					break;
				}
			case Input::SK_ENTER:
				{
					activity = ACT_INGAME;
					break;
				}
			}
			break;
		}
	case ACT_GAMEOVER:
		{
			if (keyboard->KeyPress() != Input::SK_ENTER)
			{
				GP_QuitGame();
			}
			break;
		}
	case ACT_INGAME:
		{
			UpdateCamera();

			if (keyboard->KeyPress() == Input::SK_ESCAPE)
			{
				activity = ACT_ASKEXITGAME;
			}

			// game over?
			int gameOverScore = 10;
			if (GP_GetGameType() == Runtime::GT_FIRST_OUT_OF_FIVE)
			{
				gameOverScore = 5;
			}

			// check overall scores
			std::vector<Ship*>::iterator pos = ships.begin();
			while (pos != ships.end())
			{
				if ((*pos)->MyScore() >= gameOverScore)
				{
					// set winner
					GP_Winner(*pos);
					activity = ACT_GAMEOVER;
					break;
				}
				pos++;
			}

			if (currentLevel!=NULL)
			{
				std::vector<WorldObject*> objects=currentLevel->GetObjects();

				// draw all bounding boxes of all objects for now (exepting the root)
				for (int i=1; i<objects.size(); i++)
				{
					try
					{
						objects[i]->EventLogic(frameTime);
					}
					catch (Exception* e)
					{
						objects[i]->SetScriptEnabled(false);
						std::string str = "Object "+objects[i]->GetName()+" threw an exception\n";
						str = str + "(" + e->Message() + ")\n";
						str = str + "this object's script is now disabled";
						BaseApp::Get()->ShowErrorMessage(str,"Logic Script runtime error");
					}
				}

				// process the ships' logic seperately
				for (int i=0; i < ships.size(); i++)
				{
					ships[i]->EventLogic(frameTime);
				}

			}
			break;
		}
	}
}

bool GF10Runtime::SetupSceneForRendering()
{
	Device* dev=Interface::GetDevice();
	if (!dev->BeginScene())
	{
		return false;
	}

	// D3DCLEAR_STENCIL
	dev->Clear(0,NULL,D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,backgroundColour,1,0);

	dev->SetViewport(0,0,dev->GetWidth(),dev->GetHeight());

	// set fixed funciton transforms
	dev->SetIdentityTransform();

	//camera->SetAngles(10,0);
	camera->Activate();

	dev->SetViewTransform(camera->GetViewMatrix());
	dev->SetProjectionTransform(camera->GetProjectionMatrix());

	// enable color writes
	dev->SetRenderState(D3DRS_COLORWRITEENABLE,D3DCOLORWRITEENABLE_RED   |
											   D3DCOLORWRITEENABLE_BLUE  |
											   D3DCOLORWRITEENABLE_GREEN |
											   D3DCOLORWRITEENABLE_ALPHA);
	// set fillemode
	dev->SetRenderState(D3DRS_FILLMODE,wireframe?D3DFILL_WIREFRAME:D3DFILL_SOLID);

	// Turn on the zbuffer
	dev->SetRenderState(D3DRS_ZENABLE,TRUE);

	if (cullCCW)
	{
		dev->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);
	}
	else
	{
		dev->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);
	}
	return true;
}

void GF10Runtime::Draw(double time, float displayOffX, float displayOffY, int shipId, 
						 RECT* source, RECT* dest, int splitScreenType)
{
	if (SetupSceneForRendering())
	{
		Device* dev = Interface::GetDevice();
		dev->SetIdentityTransform();

		dev->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
		dev->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		dev->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
		dev->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
		dev->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
		dev->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
		dev->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
		dev->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );
		dev->SetSamplerState( 0, D3DSAMP_ADDRESSU,  D3DTADDRESS_CLAMP );
		dev->SetSamplerState( 0, D3DSAMP_ADDRESSV,  D3DTADDRESS_CLAMP );

		dev->SetRenderState(D3DRS_LIGHTING,FALSE);
		dev->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);
		dev->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);

		if (currentLevel!=NULL)
		{
			// draw game map
			ModelMap* mm = GetModelMap();
			if (mm != NULL)
			{
				if (shipId < ships.size())
				{
					D3DXVECTOR3 zeroPos = ships[shipId]->GetPosition();
					mm->Draw(zeroPos, splitScreenType);

					// do bases
					std::vector<Base*> bases = GetBases();
					for (int i=0; i < bases.size(); i++)
					{
						dev->SetIdentityTransform();
						bases[i]->Draw(time, zeroPos);
					}

					// draw the turrets
					std::vector<Turret*> turrets = GetTurrets();
					for (int i = 0; i < turrets.size(); i++)
					{
						dev->SetIdentityTransform();
						turrets[i]->Draw(time, zeroPos);
					}

					for (int i=0; i < ships.size(); i++)
					{
						dev->SetIdentityTransform();
						ships[i]->Draw(time, zeroPos);
					}

					// draw the waters
					std::vector<Water*> waters = GetWater();
					for (int i = 0; i < waters.size(); i++)
					{
						dev->SetIdentityTransform();
						waters[i]->Draw(time, zeroPos);
					}

					// display zero pos objects status
					ships[shipId]->DisplayStatus(displayOffX, displayOffY);
				}
			}
		}
		
		// draw black border
		D3DXCOLOR black = D3DXCOLOR(0,0,0,1);
		dev->FillRect(D3DXVECTOR2((float)source->left,(float)source->top+2), D3DXVECTOR2((float)source->right,(float)source->top), black);
		dev->FillRect(D3DXVECTOR2((float)source->left,(float)source->bottom), D3DXVECTOR2((float)source->right,(float)source->bottom-2), black);
		dev->FillRect(D3DXVECTOR2((float)source->left,(float)source->bottom), D3DXVECTOR2((float)source->left+2,(float)source->top), black);
		dev->FillRect(D3DXVECTOR2((float)source->right-2,(float)source->bottom), D3DXVECTOR2((float)source->right,(float)source->top), black);

		dev->EndScene();
		dev->Present(source, dest);
	}
}

void GF10Runtime::DrawMixed(double time, float offsetX, float offsetY1, float offsetY2, float posx, float posy, 
						 RECT* source, RECT* dest, int splitScreenType)
{
	if (SetupSceneForRendering())
	{
		Device* dev = Interface::GetDevice();
		dev->SetIdentityTransform();

		dev->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
		dev->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		dev->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
		dev->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
		dev->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
		dev->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
		dev->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
		dev->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );
		dev->SetSamplerState( 0, D3DSAMP_ADDRESSU,  D3DTADDRESS_CLAMP );
		dev->SetSamplerState( 0, D3DSAMP_ADDRESSV,  D3DTADDRESS_CLAMP );

		dev->SetRenderState(D3DRS_LIGHTING,FALSE);
		dev->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);
		dev->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);

		if (currentLevel!=NULL)
		{
			// draw game map
			ModelMap* mm = GetModelMap();
			if (mm != NULL)
			{
				D3DXVECTOR3 zeroPos;
				zeroPos.x = posx;
				zeroPos.y = posy;
				zeroPos.z = ships[0]->GetPosition().z;

				mm->Draw(zeroPos, splitScreenType);

				// do bases
				std::vector<Base*> bases = GetBases();
				for (int i=0; i < bases.size(); i++)
				{
					dev->SetIdentityTransform();
					bases[i]->Draw(time, zeroPos);
				}

				// draw the turrets
				std::vector<Turret*> turrets = GetTurrets();
				for (int i = 0; i < turrets.size(); i++)
				{
					dev->SetIdentityTransform();
					turrets[i]->Draw(time, zeroPos);
				}

				for (int i=0; i < ships.size(); i++)
				{
					dev->SetIdentityTransform();
					ships[i]->Draw(time, zeroPos);
				}

				// draw the waters
				std::vector<Water*> waters = GetWater();
				for (int i = 0; i < waters.size(); i++)
				{
					dev->SetIdentityTransform();
					waters[i]->Draw(time, zeroPos);
				}

				// display zero pos objects status
				ships[0]->DisplayStatus(offsetX, offsetY1);
				ships[1]->DisplayStatus(offsetX, offsetY2);
			}
		}
		
		// draw black border
		D3DXCOLOR black = D3DXCOLOR(0,0,0,1);
		dev->FillRect(D3DXVECTOR2((float)source->left,(float)source->top+2), D3DXVECTOR2((float)source->right,(float)source->top), black);
		dev->FillRect(D3DXVECTOR2((float)source->left,(float)source->bottom), D3DXVECTOR2((float)source->right,(float)source->bottom-2), black);
		dev->FillRect(D3DXVECTOR2((float)source->left,(float)source->bottom), D3DXVECTOR2((float)source->left+2,(float)source->top), black);
		dev->FillRect(D3DXVECTOR2((float)source->right-2,(float)source->bottom), D3DXVECTOR2((float)source->right,(float)source->top), black);

		dev->EndScene();
		dev->Present(source, dest);
	}
}

void GF10Runtime::Draw(double frameTime)
{
	switch (activity)
	{
	case ACT_INMENU:
		{
			menuManager->Draw(frameTime);
			break;
		}
	case ACT_ERROR:
		{
			SetupSceneForRendering();
			Device* dev = Interface::GetDevice();
			Font* font = dev->GetSmallFont();
			float w = (float)dev->GetWidth();
			float h = (float)dev->GetHeight();
			
			std::string str = "AN ERROR OCCURED";
			float width = font->GetWidth(str);
			font->Write(w * 0.5f - width * 0.5f, h * 0.45f - 10, str, D3DXCOLOR(1,0.4f,0.4f,1));

			width = font->GetWidth(errorString);
			font->Write(w * 0.5f - width * 0.5f, h * 0.55f - 10, errorString, white);

			str = "press [ESC] to continue";
			width = font->GetWidth(str);
			font->Write(w * 0.5f - width * 0.5f, h * 0.75f - 10, str, white);

			PresentFrame();
			break;
		}
	case ACT_ASKEXITGAME:
		{
			SetupSceneForRendering();
			Device* dev = Interface::GetDevice();
			Font* font = dev->GetSmallFont();
			float w = (float)dev->GetWidth();
			float h = (float)dev->GetHeight();
			
			std::string str = "press [ENTER] to continue";
			float width = font->GetWidth(str);
			font->Write(w * 0.5f - width * 0.5f, h * 0.45f - 10, str, white);

			str = "or press [ESC] to return to the main menu";
			width = font->GetWidth(str);
			font->Write(w * 0.5f - width * 0.5f, h * 0.55f - 10, str, white);

			PresentFrame();
			break;
		}
	case ACT_GAMEOVER:
		{
			SetupSceneForRendering();
			Device* dev = Interface::GetDevice();
			Font* font = dev->GetSmallFont();
			Font* largeFont = dev->GetLargeFont();
			float w = (float)dev->GetWidth();
			float h = (float)dev->GetHeight();
			
			// display winner and loser if the game is over
			Ship* winnerShip = (Ship*)GP_Winner();

			std::string name;
			for (int i=0; i < networkPlayers.size(); i++)
			{
				if (networkPlayers[i].playerId == winnerShip->PlayerId())
				{
					name = networkPlayers[i].name;
					break;
				}
			}
			if (name.empty())
			{
				if (ships[0] == winnerShip)
				{
					name = "Player 1";
				}
				else
				{
					name = "Player 2";
				}
			}

			std::string str = "Game Over";
			float width = font->GetWidth(str);
			font->Write(w * 0.5f - width * 0.5f, h * 0.2f, str, white);

			std::string displayStr = "W I N N E R  : " + name;
			width = largeFont->GetWidth(displayStr);
			largeFont->Write((w * 0.5f) - (width * 0.5f), h * 0.5f, displayStr, white);

			str = "press [ENTER] to continue";
			width = font->GetWidth(str);
			font->Write(w * 0.5f - width * 0.5f, h * 0.8f, str, white);

			PresentFrame();
			break;
		}
	case ACT_INGAME:
		{
			if (ObjectFactory::Isa(selectedItem, WorldObject::TYPE_SHIP))
			{
				camera->SetCameraType(Camera::CAM_PERSPECTIVE);

				// find id of ship
				int shipId1 = -1;
				int shipId2 = -1;
				for (int i=0; i < ships.size(); i++)
				{
					if (ships[i]->GetOid() == selectedItem->GetOid())
					{
						shipId1 = i;
						shipId2 = i;
						break;
					}
				}
				if (shipId1 == -1)
				{
					throw new Exception("ship not found");
				}
				if (ships.size() > 1)
				{
					if (shipId1 == 1)
						shipId2 = 0;
					else
						shipId2 = 1;
				}

				Device* dev = Interface::GetDevice();
				RECT dest, source;
				int w = dev->GetWidth();
				int h = dev->GetHeight();

				int useSplitScreen = screenType;
				if (useSplitScreen != Runtime::SC_SINGLE)
				{
					if (ships.size() == 2)
					{
						D3DXVECTOR3 pos1 = ships[0]->GetPosition();
						D3DXVECTOR3 pos2 = ships[1]->GetPosition();
						float dx = (pos1.x - pos2.x);
						dx = dx * dx;
						float dy = (pos1.y - pos2.y);
						dy = dy * dy;

						// adjust distance according to smaller screen aspect
						float w = (float)BaseApp::Get()->Width();
						w = (w * w) * 0.075f;
						float h = (float)BaseApp::Get()->Height();
						h = (h * h) * 0.075f;

						if (dx < w && dy < h)
						{
							useSplitScreen = Runtime::SC_MIXED;
						}
					}
				}

				switch (useSplitScreen)
				{
					case Runtime::SC_VERTICAL:
					{
						// setting for 800 x 600 screen only!
						camera->SetPosition(D3DXVECTOR3(0,0,-250));

						// display status offset
						float offsetX = (float)w / 4 + 20;
						float offsetY = (float)dev->GetHeight() - 60;

						// lhs of split screen
						SetRect(&source, w/4, 0, w/4 + w/2, h);
						SetRect(&dest, 0, 0, w/2, h);
						Draw(frameTime, offsetX, offsetY, shipId1, &source, &dest, screenType);

						// rhs of split screen
						SetRect(&source, w/4, 0, w/4 + w/2, h);
						SetRect(&dest, w/2, 0, w, h);
						Draw(frameTime, offsetX, offsetY, shipId2, &source, &dest, screenType);
						break;
					}
					case Runtime::SC_HORIZONTAL:
					{
						// setting for 800 x 600 screen only!
						camera->SetPosition(D3DXVECTOR3(0,0,-250));

						float offsetX = 20;
						float offsetY = (float)dev->GetHeight() * 0.6f + 20;

						// lhs of split screen
						SetRect(&source, 0, h/4, w, h/4 + h/2);
						SetRect(&dest, 0, 0, w, h/2);
						Draw(frameTime, offsetX, offsetY, shipId1, &source, &dest, screenType);

						// rhs of split screen
						SetRect(&source, 0, h/4, w, h/4 + h/2);
						SetRect(&dest, 0, h/2, w, h);
						Draw(frameTime, offsetX, offsetY, shipId2, &source, &dest, screenType);
						break;
					}
					case Runtime::SC_SINGLE:
					{
						// setting for 800 x 600 screen only!
						camera->SetPosition(D3DXVECTOR3(0,0,-250));

						float offsetX = 20;
						float offsetY = (float)dev->GetHeight() - 60;

						// full screen
						SetRect(&source, 0, 0, w, h);
						SetRect(&dest, 0, 0, w, h);
						Draw(frameTime, offsetX, offsetY, shipId1, &source, &dest, screenType);
						break;
					}
					case Runtime::SC_MIXED:
					{
						// setting for 800 x 600 screen only!
						camera->SetPosition(D3DXVECTOR3(0,0,-250));

						float offsetX = 20;
						float offsetY1 = (float)dev->GetHeight() - 60;
						float offsetY2 = 40;

						// full screen
						SetRect(&source, 0, 0, w, h);
						SetRect(&dest, 0, 0, w, h);
						D3DXVECTOR3 pos1 = ships[0]->GetPosition();
						D3DXVECTOR3 pos2 = ships[1]->GetPosition();
						float posx = (pos1.x + pos2.x) * 0.5f;
						float posy = (pos1.y + pos2.y) * 0.5f;
						DrawMixed(frameTime, offsetX, offsetY1, offsetY2, posx, posy, &source, &dest, Runtime::SC_SINGLE);
						break;
					}
				}
			}
			break;
		}
	}
}

void GF10Runtime::DrawNormal(double time)
{
	SetupSceneForRendering();

	Device* dev=Interface::GetDevice();
	dev->SetIdentityTransform();

    dev->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
    dev->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    dev->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
    dev->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
    dev->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
    dev->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
    dev->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
    dev->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );
    dev->SetSamplerState( 0, D3DSAMP_ADDRESSU,  D3DTADDRESS_CLAMP );
    dev->SetSamplerState( 0, D3DSAMP_ADDRESSV,  D3DTADDRESS_CLAMP );

	dev->SetRenderState(D3DRS_LIGHTING,FALSE);
	dev->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);
	dev->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);

	if (currentLevel!=NULL)
	{
		// setting for 800 x 600 screen only!
		camera->SetCameraType(Camera::CAM_ORTHOGRAPHIC);
		D3DXVECTOR3 camPos = camera->GetPosition();
		camPos.z = -250;
		camera->SetPosition(camPos);

		std::vector<WorldObject*> objects=currentLevel->GetObjects();
		for (int i=1; i<objects.size(); i++)
		{
			if (objects[i]->GetVisible())
			{
				objects[i]->Draw(time);
			}
		}
	}

	PresentFrame();
}

void GF10Runtime::UpdateCamera()
{
	// a weighted value for movement independent of frame rate
	float weight = 1;

	// read delta mouse and reset it to zero
	D3DXVECTOR2 delta, abs;
	System::GetVec2Action(System::A_ABSMOUSE,abs.x,abs.y);
	System::GetVec2Action(System::A_DELTAMOUSE,delta.x,delta.y);
	delta*=weight;
	// clear deltas after use
	System::SetAction(System::A_DELTAMOUSE,0,0);

	// only allow one degree of movement at a time
	if (fabsf(delta.x) > fabsf(delta.y))
	{
		delta.y = 0;
	}
	else
	{
		delta.x = 0;
	}

	// get key/mouse states related to camera
	bool select		= System::GetBoolAction(System::A_SELECT1);		// l button in editor
	bool modCam		= System::GetBoolAction(System::A_SELECT2);		// r button in editor
	bool physZoom	= System::GetBoolAction(System::A_MODIFIER1);	// shift
	bool fovZoom	= System::GetBoolAction(System::A_MODIFIER2);	// ctrl
	bool pan		= System::GetBoolAction(System::A_MODIFIER3);	// alt

	// move camera along existing axes
	if (modCam && !fovZoom && !physZoom && !pan)
	{
		delta.x = -delta.x;

		const D3DXMATRIXA16& crot = camera->GetLocalRotationMatrix();

		// move into the direction of the camera on x and y axis
		D3DXVECTOR3 xaxis = D3DXVECTOR3(crot._11, crot._21, crot._31);
		D3DXVECTOR3 yaxis = D3DXVECTOR3(crot._12, crot._22, crot._32);

		xaxis *= delta.x;
		yaxis *= delta.y;

		camera->Translate(xaxis);
		camera->Translate(yaxis);
	}
	camera->CalculateMatrices();

	// select an item and move it if possible
	if (select && currentLevel != NULL)
	{
		WorldObject* pick = currentLevel->Pick(camera, abs, true);
		if (pick != NULL)
		{
			pick = NULL;
		}
	}
}

void GF10Runtime::PresentFrame()
{
	Device* dev = Interface::GetDevice();
	dev->EndScene();
	dev->Present();
}

