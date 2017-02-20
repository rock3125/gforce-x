//////////////////////////////////////////////////////////////////////
/// GRAVITY FORCE X
///
/// (c)Copyright Peter de Vocht, 2006
///
/// I'm releasing this code under GPL v2.1 (see included file lgpl.txt)
///
//////////////////////////////////////////////////////////////////////

#pragma once

#include "runtime/runtime.h"
#include "system/input.h"

class Camera;
class Mesh;

////////////////////////////////////////////////////

class GF10Runtime: public Runtime
{
	GF10Runtime(const GF10Runtime&);
	const GF10Runtime& operator=(const GF10Runtime&);
public:
	GF10Runtime();
	virtual ~GF10Runtime();

	// overwritten to catch level changes
	virtual void SetCurrentLevel(Level* currentLevel);

	// set selected item
	void SetSelectedItem(WorldObject* selectedItem);

	// set split screen type
	void SetSplitScreenType(int splitScreenType);

	// implement update and render
	virtual void Draw(double time);

	// access the renderer camera
	Camera* GetCamera();

	// set camera values
	void SetCamera(const Camera& _camera);

	// initialise renderer systems
	virtual void Initialise();

	// get set wireframe
	virtual bool GetWireframe();
	virtual void SetWireframe(bool wireframe);

	// set ccw or cull-none
	virtual bool GetCullCCW();
	virtual void SetCullCCW(bool _cullCCW);

	// end frame
	void PresentFrame();

	// runtime environments need to be able to set the active player
	void SetActivePlayer(int shipId);

	// add a mesh to be rendered - the scene graph
	// will be walked and each mesh that can draw will
	// add itself and gets rendered this way in the Render() loop
	void RenderMesh(Mesh* mesh,const D3DXMATRIX& transform);

	// event routines
	virtual void EventInit();
	virtual void EventLogic(double time);

protected:
	// update edit camera
	void UpdateCamera();

	// helper function for drawing setup
	// returns true if ready
	bool SetupSceneForRendering();

	// rebuild game objects
	void RebuildGameObjects();

	// clear current level
	void ClearCurrentLevel();

protected:
	// draw single player view according to split screen type
	void Draw(double time, float displayOffX, float displayOffY, int shipId, 
			  RECT* source, RECT* dest, int splitScreenType);

	// draw original way
	void DrawNormal(double time);

private:
	D3DXCOLOR				backgroundColour;
	D3DXCOLOR				lightColour;
	D3DXCOLOR				darkColour;

	int						splitScreenType;

	bool					wireframe;
	bool					cullCCW;

	Camera*					camera;

	// selected item pointer
	WorldObject*			selectedItem;

	enum
	{
		ACT_NONE,
		ACT_ASKING,
		ACT_REPLY,
		ACT_QUIT,
		ACT_CONTINUE
	};

	// ask person to quit or not
	Input::Event* escape;
	Input::Event* cont;

	int activity;
};

