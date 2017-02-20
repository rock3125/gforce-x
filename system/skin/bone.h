#pragma once

#include "system/model/worldObject.h"

class BoneAnimation;

/////////////////////////////////////////////////////////////////////////////

// a bone is an endpoint of a joint and has a local transformation
// expressed as a matrix.  It also has a set of children (connected
// bones (e.g. hip bone has two legs connected to it)) and it has a
// set of animations that apply to this bone in particular (each bone
// keeps its own set of animations)

class Bone : public WorldObject
{
public:
	Bone();
	Bone(const Bone&);
	const Bone& operator=(const Bone&);
	virtual ~Bone();

	// get one of the many animations for this bone
	BoneAnimation* GetAnimation(int);

	// get number of frames given an animation Id
	int GetNumFrames(int animId);

	// add a new animation
	void AddAnimation(BoneAnimation*);

	// get bone by oid in hierarchy (or NULL)
	static Bone* GetBoneByOid(Bone* rootBone, int oid);

	// read and write bones
	virtual void Read(BaseStreamer&);
	virtual void Write(BaseStreamer&);

private:
	// get num frames helper
	static int GetNumFrames(Bone* bone, int animId);

private:
	std::vector<BoneAnimation*>	animation;

	static std::string boneSignature;
	static int boneVersion;
};

