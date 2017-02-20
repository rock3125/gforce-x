#include "standardFirst.h"

#include "system/skin/boneAnimation.h"

std::string BoneAnimation::boneAnimationSignature = "boneAnimation";
int BoneAnimation::boneAnimationVersion = 1;

int BoneAnimation::framesPerSecond = 30;
float BoneAnimation::frameDuration = 0.0333333f;

/////////////////////////////////////////////////////////////////////////////

BoneAnimation::BoneAnimation()
	: rot(NULL)
	, trans(NULL)
{
	numFrames = 0;
}

BoneAnimation::BoneAnimation(const BoneAnimation& b)
	: rot(NULL)
	, trans(NULL)
{
	operator=(b);
}

const BoneAnimation& BoneAnimation::operator=(const BoneAnimation& b)
{
	Object::operator=(b);

	Release();

	numFrames = b.numFrames;
	if (numFrames>0)
	{
		rot = new D3DXQUATERNION[numFrames];
		trans = new D3DXVECTOR3[numFrames];
		memcpy(rot,b.rot,sizeof(D3DXQUATERNION)*numFrames);
		memcpy(trans,b.trans,sizeof(D3DXVECTOR3)*numFrames);
	}
	return *this;
}

BoneAnimation::~BoneAnimation()
{
	Release();
}

void BoneAnimation::Release()
{
	safe_delete_array(rot);
	safe_delete_array(trans);
	numFrames = 0;
}

int BoneAnimation::GetFramesPerSecond()
{
	return framesPerSecond;
}

float BoneAnimation::GetFrameDuration()
{
	return frameDuration;
}

int BoneAnimation::GetNumFrames()
{
	return numFrames;
}

void BoneAnimation::SetNumFrames(int _numFrames)
{
	numFrames = _numFrames;
}

D3DXQUATERNION* BoneAnimation::GetRotations()
{
	return rot;
}

void BoneAnimation::SetRotations(int _numFrames,D3DXQUATERNION* _rot)
{
	safe_delete_array(rot);
	numFrames = _numFrames;
	rot = new D3DXQUATERNION[numFrames];
	memcpy(rot,_rot,sizeof(D3DXQUATERNION)*numFrames);
}

D3DXVECTOR3* BoneAnimation::GetTranslations()
{
	return trans;
}

void BoneAnimation::SetTranslations(int _numFrames,D3DXVECTOR3* _trans)
{
	safe_delete_array(trans);
	numFrames = _numFrames;
	trans = new D3DXVECTOR3[numFrames];
	memcpy(trans,_trans,sizeof(D3DXVECTOR3)*numFrames);
}

void BoneAnimation::Read(int index, BaseStreamer& _f)
{
	BaseStreamer& f = _f.GetChild(boneAnimationSignature + System::Int2Str(index),boneAnimationVersion);
	f.Read("numFrames", numFrames);
	safe_delete_array(rot);
	safe_delete_array(trans);
	if (numFrames > 0)
	{
		rot = new D3DXQUATERNION[numFrames];
		trans = new D3DXVECTOR3[numFrames];
		for (int i=0; i < numFrames; i++)
		{
			f.Read("rot" + System::Int2Str(i+1), rot[i]);
			f.Read("trans" + System::Int2Str(i+1), trans[i]);
		}
	}
}

void BoneAnimation::Write(int index, BaseStreamer& _f)
{
	BaseStreamer& f = _f.NewChild(boneAnimationSignature + System::Int2Str(index),boneAnimationVersion);
	f.Write("numFrames", numFrames);
	for (int i=0; i < numFrames; i++)
	{
		f.Write("rot" + System::Int2Str(i+1), rot[i]);
		f.Write("trans" + System::Int2Str(i+1), trans[i]);
	}
}

