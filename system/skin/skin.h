#pragma once

class BoneInfluence;
class Bone;
class Mesh;
class Texture;

#include "system/model/worldObject.h"

/////////////////////////////////////////////////////////////////////////////

// a skin is a set of bones starting at a root bone (usually around the hip
// region for humanoids) a single mesh and texture (cg. a model) and a set of
// influences for each vertex in the mesh that deform the mesh that defines the
// skin.  In essence a skin is a deformable mesh with bones, animations on those
// bones.  The deformations are defined per vertex in the mesh relative to the
// bones of the animation.

class Skin : public WorldObject, public InvalidatableDevice
{
public:
	Skin();
	~Skin();
	Skin(const Skin&);
	const Skin& operator=(const Skin&);

	// release device objects, should be called on a device reset
	void InvalidateDeviceObjects();
	void RestoreDeviceObjects();

	Bone* GetRootBone();
	void SetRootBone(Bone*);

	Mesh* GetSkin();
	void SetSkin(Mesh*);

	// set texture
	void SetTexture(Texture*);

	// draw the skin
	virtual void Draw();

	BoneInfluence* GetBoneInfluences();
	void SetBoneInfluences(int,BoneInfluence*);

	// setup animation on this skin with skeleton
	void SetTime(int animId,float time);

	// update skin mesh transformation / deformation
	void Update();

	// save and load a skin
	virtual void Read(XmlNode* node);
	virtual XmlNode* Write();

	// return the signature of this object
	virtual std::string Signature()
	{
		return skinSignature;
	}

private:
	// given a set of bones with animations and an animation id
	// set the time in the animation onto the bones (recursive)
	static void SetTime(Bone* bone, int animId, float time, bool root,
						int timeIndex1, float perc1, int timeIndex2, float perc2);

	// recursive skeleton draw routine
	void DrawSkeleton(Bone* root, const D3DXMATRIXA16* matrix);

private:
	D3DXCOLOR			colour;
	Bone*				rootBone;
	Mesh*				skin;
	Texture*			texture;

	int					numVertexInfluences;
	BoneInfluence*		vertexInfluences;

	static std::string skinSignature;
	static int skinVersion;
};

