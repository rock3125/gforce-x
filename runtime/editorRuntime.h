#pragma once

#include "runtime/runtime.h"

#include "system/signals/selectionObserver.h"
#include "system/signals/creationObserver.h"

class Camera;
class Mesh;

////////////////////////////////////////////////////

class EditorRuntime :			public Runtime, 
								// notification system
								public SelectionObserver,
								public CreationObserver
{
	EditorRuntime(const EditorRuntime&);
	const EditorRuntime& operator=(const EditorRuntime&);
public:
	EditorRuntime();
	virtual ~EditorRuntime();

	// focus camera on object
	virtual void FocusObject(WorldObject* obj);

	// overwritten to catch level changes
	virtual void SetCurrentLevel(Level* currentLevel);

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

	// add a mesh to be rendered - the scene graph
	// will be walked and each mesh that can draw will
	// add itself and gets rendered this way in the Render() loop
	void RenderMesh(Mesh* mesh,const D3DXMATRIX& transform);

	// object creation notification
	void AcceptCreation(WorldObject::WorldType create);
	void AcceptCreation(WorldObject* obj);

	// get notified of a different selection
	virtual void SelectionChanged(WorldObject* newSelection);

	// event routines
	void EventKeyDown(int key,bool shift,bool ctrl);
	void EventKeyUp(int key,bool shift,bool ctrl);
	void EventInit();
	virtual void EventLogic(double time);

	virtual void SetSelectedItem(WorldObject* selectedItem);

	// set split screen type
	virtual void SetSplitScreenType(int splitScreenType);

protected:
	// update edit camera
	void UpdateCamera();

	// helper function for drawing setup
	void SetupSceneForRendering();

	// rebuild game objects
	void RebuildGameObjects();

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
};

