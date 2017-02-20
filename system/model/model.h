#pragma once

#include "system/model/worldObject.h"
#include "d3d9/InvalidatableDevice.h"

class Mesh;
class Texture;

/////////////////////////////////////////////////////////////////////////////

class Model : public WorldObject, public InvalidatableDevice
{
public:
	Model();
	virtual ~Model();

	Model(const Model&);
	const Model& operator=(const Model&);

	// release device objects, should be called on a device reset
	void InvalidateDeviceObjects();
	void RestoreDeviceObjects();

	// apply the local transformation to the mesh permanently
	// and set the local transform to identity
	void ApplyTransformation();

	// draw this model
	virtual void Draw(double time);
	virtual void Draw(double time, const D3DXVECTOR3& pos);

	// accessors
	Mesh* GetMesh();
	Mesh* GetMesh(std::string mesh);
	void SetMesh(Mesh* m);

	// update aabb for this model
	virtual void UpdateBoundingBox();

	// for all my children and myself - optimise the meshes
	void OptimiseMeshes();

	Texture* GetTexture();
	void SetTexture(Texture* m);

	const D3DXCOLOR& GetColour();
	void SetColour(const D3DXCOLOR& colour);

	virtual void Write(BaseStreamer& f);
	virtual void Read(BaseStreamer& f);

private:
	// helper function for optimise mesh
	static void OptimiseMeshesRecursive(Model* model);

private:
	Mesh*		mesh;
	Texture*	texture;
	D3DXCOLOR	colour;

	static std::string modelSignature;
	static int modelVersion;
};

