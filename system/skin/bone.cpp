#include "standardFirst.h"

#include "system/skin/bone.h"
#include "system/skin/boneAnimation.h"

std::string Bone::boneSignature="bone";
int Bone::boneVersion=1;

/////////////////////////////////////////////////////////////////////////////

Bone::Bone()
{
	SetType(OT_BONE);
	SetWorldType(TYPE_BONE);
}

Bone::Bone(const Bone& b)
{
	operator=(b);
}

const Bone& Bone::operator=(const Bone& b)
{
	WorldObject::operator=(b);

	safe_delete_stl_array(animation);
	for (int i=0; i<b.animation.size(); i++)
	{
		animation.push_back(new BoneAnimation(*b.animation[i]));
	}
	return *this;
}

Bone::~Bone()
{
	safe_delete_stl_array(animation);
}

BoneAnimation* Bone::GetAnimation(int index)
{
	if (index < animation.size())
		return animation[index];
	return NULL;
}

void Bone::AddAnimation(BoneAnimation* ba)
{
	animation.push_back(ba);
}

int Bone::GetNumFrames(int animId)
{
	return GetNumFrames(this, animId);
}

int Bone::GetNumFrames(Bone* bone, int animId)
{
	BoneAnimation* ba = bone->GetAnimation(animId);
	if (ba != NULL)
	{
		return ba->GetNumFrames();
	}
	else
	{
		std::vector<PRS*> children = bone->GetChildren();
		for (int i=0; i < children.size(); i++)
		{
			Bone* bone = dynamic_cast<Bone*>(children[i]);
			int numFrames = GetNumFrames(bone, animId);
			if (numFrames != 0)
			{
				return numFrames;
			}
		}
	}
	return 0;
}

Bone* Bone::GetBoneByOid(Bone* rootBone, int oid)
{
	if (rootBone->GetOid() == oid)
		return rootBone;
	std::vector<PRS*> children = rootBone->GetChildren();
	for (int i=0; i < children.size(); i++)
	{
		Bone* bone = Bone::GetBoneByOid(dynamic_cast<Bone*>(children[i]), oid);
		if (bone != NULL)
			return bone;
	}
	return NULL;
}

void Bone::Read(XmlNode* node)
{
	XmlNode::CheckVersion(node, boneSignature, boneVersion);

	WorldObject::Read(node->GetChild(WorldObject::Signature()));
/*
	int animationSize;
	node->Read("animationSize", animationSize);
	safe_delete_stl_array(animation);
	for (int i=0; i < animationSize; i++)
	{
		BoneAnimation* ba = new BoneAnimation();
		ba->Read(i, f);
		animation.push_back(ba);
	}

	// write children
	std::vector<PRS*> children;
	int numChildren;
	node->Read("numChildren", numChildren);
	for (int i=0; i < numChildren; i++)
	{
		Bone* bone = new Bone();
		bone->Read(f);
		AttachChild(bone);
	}
*/
}

XmlNode* Bone::Write()
{
/*
	BaseStreamer& f = _f.NewChild(boneSignature,boneVersion);

	WorldObject::Write(f);

	int animationSize = animation.size();
	f.Write("animationSize", animationSize);
	for (int i=0; i < animation.size(); i++)
	{
		animation[i]->Write(i,f);
	}

	// write children
	std::vector<PRS*> children = GetChildren();
	int numChildren = children.size();
	f.Write("numChildren", numChildren);
	for (int i=0; i < numChildren; i++)
	{
		Bone* bone = dynamic_cast<Bone*>(children[i]);
		bone->Write(f);
	}
*/
	return NULL;
}

