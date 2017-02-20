#include "standardFirst.h"

#include "system/skin/boneInfluence.h"
#include "system/skin/bone.h"

std::string BoneInfluence::boneInfluenceSignature = "boneInfluence";
int BoneInfluence::boneInfluenceVersion = 1;

/////////////////////////////////////////////////////////////////////////////

BoneInfluence::BoneInfluence()
	: bones(NULL)
	, weights(NULL)
{
	count = 0;
}

BoneInfluence::~BoneInfluence()
{
	safe_delete_array(bones);
	safe_delete_array(weights);
}

BoneInfluence::BoneInfluence(const BoneInfluence& b)
	: bones(NULL)
	, weights(NULL)
{
	operator=(b);
}

const BoneInfluence& BoneInfluence::operator=(const BoneInfluence& b)
{
	safe_delete_array(bones);
	safe_delete_array(weights);

	count = b.count;
	if (count > 0)
	{
		bones = new Bone*[count];
		weights = new float[count];
		memcpy(bones,b.bones,sizeof(Bone*)*count);
		memcpy(weights,b.weights,sizeof(float)*count);
	}
	return *this;
}

Bone** BoneInfluence::GetBones()
{
	return bones;
}

float* BoneInfluence::GetWeights()
{
	return weights;
}

int BoneInfluence::GetCount()
{
	return count;
}

void BoneInfluence::Add(Bone* bone, float weight)
{
	Bone** newBones = new Bone*[count+1];
	float* newWeights = new float[count+1];
	if (count > 0)
	{
		memcpy(newBones,bones,sizeof(Bone*)*count);
		memcpy(newWeights,weights,sizeof(float)*count);
	}
	newBones[count] = bone;
	newWeights[count] = weight;
	count++;
	safe_delete_array(bones);
	safe_delete_array(weights);

	bones = newBones;
	weights = newWeights;
}

void BoneInfluence::Read(int index, Bone* rootBone, BaseStreamer& _f)
{
	BaseStreamer& f = _f.GetChild(boneInfluenceSignature + System::Int2Str(index),boneInfluenceVersion);
	f.Read("count", count);
	safe_delete_array(weights);
	safe_delete_array(bones);
	if (count > 0)
	{
		weights = new float[count];
		bones = new Bone*[count];
		for (int i=0; i < count; i++)
		{
			BaseStreamer& f1 = f.GetChild("influence" + System::Int2Str(i),1);
			f1.Read("weight" + System::Int2Str(i+1), weights[i]);
			int boneOid;
			f1.Read("bone" + System::Int2Str(i+1), boneOid);
			bones[i] = Bone::GetBoneByOid(rootBone, boneOid);
		}
	}
}

void BoneInfluence::Write(int index,BaseStreamer& _f)
{
	BaseStreamer& f = _f.NewChild(boneInfluenceSignature + System::Int2Str(index),boneInfluenceVersion);
	f.Write("count", count);
	for (int i=0; i < count; i++)
	{
		BaseStreamer& f1 = f.NewChild("influence" + System::Int2Str(i),1);
		f1.Write("weight" + System::Int2Str(i+1), weights[i]);
		int boneOid = bones[i]->GetOid();
		f1.Write("bone" + System::Int2Str(i+1), boneOid);
	}
}
