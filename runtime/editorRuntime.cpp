#include "standardFirst.h"

#include "runtime/editorRuntime.h"

#include "system/mainApp.h"
#include "system/objectFactory.h"

#include "system/model/camera.h"
#include "system/model/mesh.h"
#include "system/model/level.h"
#include "system/model/model.h"
#include "system/skin/skin.h"

#include "game/ship.h"
#include "game/base.h"
#include "game/modelMap.h"
#include "game/commonModels.h"

#include "system/signals/addObjectObserver.h"

#include "d3d9/interface.h"
#include "d3d9/texture.h"

////////////////////////////////////////////////////

EditorRuntime::EditorRuntime()
	: camera(NULL)
	, selectedItem(NULL)
{
	// background clear colour
	float col = 0.0f;
	backgroundColour = D3DXCOLOR(col,col,col,0);

	// grid colours
	lightColour = D3DXCOLOR(0.4f,0.4f,0.4f,0);
	darkColour = D3DXCOLOR(0.2f,0.2f,0.2f,0);

	wireframe = false;
	cullCCW = true;

	splitScreenType = ModelMap::SC_HORIZONTAL;
}

EditorRuntime::~EditorRuntime()
{
	// release common models
	CommonModels::Destroy();

	safe_delete(camera);
}

bool EditorRuntime::GetWireframe()
{
	return wireframe;
}

void EditorRuntime::SetWireframe(bool _wireframe)
{
	wireframe = _wireframe;
}

bool EditorRuntime::GetCullCCW()
{
	return cullCCW;
}

void EditorRuntime::SetCullCCW(bool _cullCCW)
{
	cullCCW = _cullCCW;
}

void EditorRuntime::FocusObject(WorldObject* obj)
{
	Runtime::FocusObject(obj);
	D3DXVECTOR3 pos = obj->GetPosition();
	pos.z -= 100;
	pos.y += 20;
	camera->SetPosition(pos);
	camera->SetRotationEuler(D3DXVECTOR3(0,0,0));
}

void EditorRuntime::AcceptCreation(WorldObject* obj)
{
	if (obj!=NULL)
	{
		Level* level=MainApp::GetApp().GetCurrentLevel();
		PreCond(level!=NULL);
		WorldObject* root=level->GetSceneRoot();
		PreCond(root!=NULL);
		int count=level->ObjectTypeCount(obj->GetWorldType());

		std::string name=ObjectFactory::GetWorldTypeName(obj->GetWorldType())+System::Int2Str(count+1);
		obj->SetName(name);

		if (selectedItem==NULL)
		{
			level->AddObjectToScene(root,obj);
			AddObjectObserver::NotifyObservers(root,obj);
		}
		else
		{
			level->AddObjectToScene(selectedItem,obj);
			AddObjectObserver::NotifyObservers(selectedItem,obj);
		}
		SelectionObserver::NotifyObservers(obj);

		// re-setup game objects
		RebuildGameObjects();
	}
}

void EditorRuntime::AcceptCreation(WorldObject::WorldType create)
{
	WorldObject* obj=ObjectFactory::CreateObject(create);
	AcceptCreation(obj);
}

void EditorRuntime::Initialise()
{
	Device* dev=Interface::GetDevice();

	if (camera==NULL)
	{
		camera=new Camera();
		camera->Activate();
	}

	// create common models at startup
	CommonModels::Get();
}

void EditorRuntime::EventKeyDown(int key,bool shift,bool ctrl)
{
	if (selectedItem!=NULL)
	{
		try
		{
			selectedItem->EventKeyDown(key,shift,ctrl);
		}
		catch (Exception* e)
		{
			selectedItem->SetScriptEnabled(false);
			std::string str = "Object "+selectedItem->GetName()+" threw an exception\n";
			str = str + "(" + e->Message() + ")\n";
			str = str + "this object's script is now disabled";
			MainApp::GetApp().ShowErrorMessage(str,"KeyDown Event runtime error");
		}
	}
}

void EditorRuntime::EventKeyUp(int key,bool shift,bool ctrl)
{
	if (selectedItem!=NULL)
	{
		try
		{
			selectedItem->EventKeyUp(key,shift,ctrl);
		}
		catch (Exception* e)
		{
			selectedItem->SetScriptEnabled(false);
			std::string str = "Object "+selectedItem->GetName()+" threw an exception\n";
			str = str + "(" + e->Message() + ")\n";
			str = str + "this object's script is now disabled";
			MainApp::GetApp().ShowErrorMessage(str,"KeyDown Event runtime error");
		}
	}
}

void EditorRuntime::EventInit()
{
	if (currentLevel!=NULL)
	{
		std::vector<WorldObject*> objects=currentLevel->GetObjects();

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
				MainApp::GetApp().ShowErrorMessage(str,"Init Script runtime error");
			}
		}
	}
}

void EditorRuntime::EventLogic(double time)
{
	UpdateCamera();

	if (System::GetBoolAction(System::A_WIREFRAME))
	{
		System::SetAction(System::A_WIREFRAME,false);
		wireframe=!wireframe;
	}

	if (System::GetBoolAction(System::A_CULLCCW))
	{
		System::SetAction(System::A_CULLCCW,false);
		cullCCW =! cullCCW;
	}

	if (currentLevel!=NULL)
	{
		std::vector<WorldObject*> objects=currentLevel->GetObjects();

		// draw all bounding boxes of all objects for now (exepting the root)
		for (int i=1; i<objects.size(); i++)
		{
			try
			{
				objects[i]->EventLogic(time);
			}
			catch (Exception* e)
			{
				objects[i]->SetScriptEnabled(false);
				std::string str = "Object "+objects[i]->GetName()+" threw an exception\n";
				str = str + "(" + e->Message() + ")\n";
				str = str + "this object's script is now disabled";
				MainApp::GetApp().ShowErrorMessage(str,"Logic Script runtime error");
			}
		}
	}
}

void EditorRuntime::SelectionChanged(WorldObject* newSelection)
{
	selectedItem = newSelection;
}

void EditorRuntime::SetupSceneForRendering()
{
	Device* dev=Interface::GetDevice();
	if (!dev->BeginScene()) return;

	// D3DCLEAR_STENCIL
	dev->Clear(0,NULL,D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,backgroundColour,1,0);

	dev->SetViewport(0,0,dev->GetWidth(),dev->GetHeight());

	// set fixed funciton transforms
	dev->SetIdentityTransform();

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
}

void EditorRuntime::Draw(double time, float displayOffX, float displayOffY, int shipId, 
						 RECT* source, RECT* dest, int splitScreenType)
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
		// draw game map
		ModelMap* mm = GetModelMap();
		if (mm != NULL)
		{
			std::vector<Ship*> ships = GetShips();
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

				for (int i=0; i < ships.size(); i++)
				{
					dev->SetIdentityTransform();
					ships[i]->Draw(time, zeroPos);
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

void EditorRuntime::Draw(double time)
{
	if (ObjectFactory::Isa(selectedItem, WorldObject::TYPE_SHIP))
	{
		camera->SetCameraType(Camera::CAM_PERSPECTIVE);

		// find id of ship
		int shipId1 = -1;
		int shipId2 = -1;
		std::vector<Ship*> ships = GetShips();
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

		Device* dev=Interface::GetDevice();
		RECT dest, source;
		int w = dev->GetWidth();
		int h = dev->GetHeight();

		switch (splitScreenType)
		{
			case ModelMap::SC_VERTICAL:
			{
				// setting for 800 x 600 screen only!
				camera->SetPosition(D3DXVECTOR3(0,0,-250));

				// display status offset
				float offsetX = (float)w / 4 + 60;
				float offsetY = (float)dev->GetHeight() - 60;

				// lhs of split screen
				SetRect(&source, w/4, 0, w/4 + w/2, h);
				SetRect(&dest, 0, 0, w/2, h);
				Draw(time, offsetX, offsetY, shipId1, &source, &dest, splitScreenType);

				// rhs of split screen
				SetRect(&source, w/4, 0, w/4 + w/2, h);
				SetRect(&dest, w/2, 0, w, h);
				Draw(time, offsetX, offsetY, shipId2, &source, &dest, splitScreenType);
				break;
			}
			case ModelMap::SC_HORIZONTAL:
			{
				// setting for 800 x 600 screen only!
				camera->SetPosition(D3DXVECTOR3(0,0,-250));

				float offsetX = 70;
				float offsetY = (float)dev->GetHeight() * 0.6f + 20;

				// lhs of split screen
				SetRect(&source, 0, h/4, w, h/4 + h/2);
				SetRect(&dest, 0, 0, w, h/2);
				Draw(time, offsetX, offsetY, shipId1, &source, &dest, splitScreenType);

				// rhs of split screen
				SetRect(&source, 0, h/4, w, h/4 + h/2);
				SetRect(&dest, 0, h/2, w, h);
				Draw(time, offsetX, offsetY, shipId2, &source, &dest, splitScreenType);
				break;
			}
			case ModelMap::SC_SINGLE:
			{
				// setting for 800 x 600 screen only!
				camera->SetPosition(D3DXVECTOR3(0,0,-125));

				float offsetX = 60;
				float offsetY = (float)dev->GetHeight() - 60;

				// full screen
				SetRect(&source, 0, 0, w, h);
				SetRect(&dest, 0, 0, w, h);
				Draw(time, offsetX, offsetY, shipId1, &source, &dest, splitScreenType);
				break;
			}
		}
	}
	else
	{
		// normal rendering loop
		DrawNormal(time);
	}
}

void EditorRuntime::DrawNormal(double time)
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

void EditorRuntime::UpdateCamera()
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

	delta.x = -delta.x;

	const D3DXMATRIXA16& crot = camera->GetLocalRotationMatrix();

	// move into the direction of the camera on x and y axis
	D3DXVECTOR3 xaxis = D3DXVECTOR3(crot._11, crot._21, crot._31);
	D3DXVECTOR3 yaxis = D3DXVECTOR3(crot._12, crot._22, crot._32);

	xaxis *= delta.x;
	yaxis *= delta.y;

	// move camera along existing axes
	if (modCam && !fovZoom && !physZoom && !pan)
	{
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
			pick->Translate(-xaxis);
			pick->Translate(-yaxis);

			if (selectedItem != pick)
			{
				SelectionObserver::NotifyObservers(pick);
			}
		}
	}
}

void EditorRuntime::PresentFrame()
{
	Device* dev=Interface::GetDevice();
	dev->EndScene();
	dev->Present();
}

Camera* EditorRuntime::GetCamera()
{
	return camera;
}

void EditorRuntime::SetCamera(const Camera& _camera)
{
	PreCond(camera!=NULL);
	*camera = _camera;
}

void EditorRuntime::RebuildGameObjects()
{
	if (currentLevel!=NULL)
	{
		base.clear();
		ship.clear();
		modelMap = NULL;

		std::vector<WorldObject*> objects = currentLevel->GetObjects();

		// draw all bounding boxes of all objects for now (exepting the root)
		for (int i=1; i<objects.size(); i++)
		{
			if (ObjectFactory::Isa(objects[i], WorldObject::TYPE_MODELMAP))
			{
				modelMap = dynamic_cast<ModelMap*>(objects[i]);
			}
			else if (ObjectFactory::Isa(objects[i], WorldObject::TYPE_BASE))
			{
				base.push_back(dynamic_cast<Base*>(objects[i]));
			}
			else if (ObjectFactory::Isa(objects[i], WorldObject::TYPE_SHIP))
			{
				ship.push_back(dynamic_cast<Ship*>(objects[i]));
			}
		}
	}

	// setup controller for both
	if (ship.size() > 1)
	{
		ship[0]->SetupKeyboard();
		ship[1]->SetupGamePad(0);
	}
}

void EditorRuntime::SetCurrentLevel(Level* currentLevel)
{
	Runtime::SetCurrentLevel(currentLevel);
	RebuildGameObjects();
}

void EditorRuntime::SetSelectedItem(WorldObject* selectedItem)
{
}

void EditorRuntime::SetSplitScreenType(int splitScreenType)
{
}

