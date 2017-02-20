#pragma once

#include "system/model/object.h"

class Bone;

/////////////////////////////////////////////////////////////////////////////

// a bone animation is a track of an animation that applies to a particular
// bone (e.g. the hip bone has its own animation track in a full humanoid
// animation)  It is defined as a set of regular sample points in an animation
// (frameDuration and its inverse, framesPerSecond).  Its consists of numFrames
// rotations (quaternions) and translations (vec3s)

class BoneAnimation : public Object
{
public:
	BoneAnimation();
	BoneAnimation(const BoneAnimation&);
	const BoneAnimation& operator=(const BoneAnimation&);
	virtual ~BoneAnimation();

	// release all memory allocated by animation
	void Release();

	// accessors
	int GetNumFrames();
	void SetNumFrames(int);

	// access frame speed and its inverse
	static int GetFramesPerSecond();
	static float GetFrameDuration();

	D3DXQUATERNION* GetRotations();
	void SetRotations(int,D3DXQUATERNION*);

	D3DXVECTOR3* GetTranslations();
	void SetTranslations(int,D3DXVECTOR3*);

	// read and write animation
	virtual void Read(int, BaseStreamer&);
	virtual void Write(int, BaseStreamer&);

private:
	int					numFrames;
	D3DXQUATERNION*		rot;
	D3DXVECTOR3*		trans;

	// statics
	static int			framesPerSecond;
	static float		frameDuration;
	static std::string	boneAnimationSignature;
	static int			boneAnimationVersion;
};

