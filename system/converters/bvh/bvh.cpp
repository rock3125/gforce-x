#include "standardFirst.h"

#include "system/converters/bvh/bvh.h"
#include "system/skin/skin.h"
#include "system/skin/bone.h"
#include "system/skin/boneAnimation.h"

///////////////////////////////////////////////////////////////////////////////////

BVH::BVH()
{
	EnableInt();
	EnableFloat();
	EnableIdent();

	Add("HIERARCHY",true);
	Add("ROOT",true);
	Add("OFFSET",true);
	Add("CHANNELS",true);
	Add("JOINT",true);
	Add("End",true);
	Add("Site",true);
	Add("MOTION",true);
	Add("Frames",true);
	Add("Frame",true);
	Add("Time",true);
	Add("Xposition",true);
	Add("Yposition",true); 
	Add("Zposition",true); 
	Add("Xrotation",true); 
	Add("Zrotation",true); 
	Add("Yrotation",true);
	Add("{",false);
	Add("}",false);
	Add(":",false);
}

BVH::~BVH()
{
}

WorldObject* BVH::LoadBinary(std::string filename)
{
	throw new Exception("load binary not implemented");
}

WorldObject* BVH::ParseLoadedFile()
{
	if (!error)
	{
		ParseBVH();
		if (!error)
		{
			// convert bvh data to skin
			Skin* skin = new Skin();
			skin->SetName(root->name);
			int offset = 0;
			Bone* rootBone = ConvertToBones(root, offset);
			skin->SetRootBone(rootBone);

			return skin;
		}
	}
	return NULL;
}

void BVH::GetPosRot(D3DXVECTOR3& pos, D3DXQUATERNION& quat, int animIndex, int offset,
					const D3DXVECTOR3& inpos, const std::vector<int>& channels)
{
	D3DXQUATERNION tempq;

	pos = inpos;
	quat = *System::GetIdentityQuaternion();
	if (animIndex > animation.size())
	{
		throw new Exception("index out of bounds");
	}
	for (int i=0; i < channels.size(); i++)
	{
		switch (channels[i])
		{
			case X_POS:
			{
				pos.x += animation[animIndex][offset + i];
				break;
			}
			case Y_POS:
			{
				pos.y += animation[animIndex][offset + i];
				break;
			}
			case Z_POS:
			{
				pos.z += animation[animIndex][offset + i];
				break;
			}
			case X_ROT:
			{
				float rot = animation[animIndex][offset + i] * System::deg2rad;
				D3DXQuaternionRotationYawPitchRoll(&tempq, 0, rot, 0);
				D3DXQuaternionMultiply(&quat,&quat,&tempq);
				break;
			}
			case Y_ROT:
			{
				float rot = animation[animIndex][offset + i] * System::deg2rad;
				D3DXQuaternionRotationYawPitchRoll(&tempq, rot, 0, 0);
				D3DXQuaternionMultiply(&quat,&quat,&tempq);
				break;
			}
			case Z_ROT:
			{
				float rot = animation[animIndex][offset + i] * System::deg2rad;
				D3DXQuaternionRotationYawPitchRoll(&tempq, 0, 0, rot);
				D3DXQuaternionMultiply(&quat,&quat,&tempq);
				break;
			}
			default:
			{
				throw new Exception("unknown channel type");
			}
		}
	}
}

Bone* BVH::ConvertToBones(HierarchyObj* obj, int& offset)
{
	if (obj == NULL)
		return NULL;

	Bone* bone = new Bone();
	bone->SetName(obj->name);

	D3DXVECTOR3 pos;
	D3DXQUATERNION quat;
	D3DXVECTOR3* a1pos = new D3DXVECTOR3[animation.size()];
	D3DXQUATERNION* a1quat = new D3DXQUATERNION[animation.size()];
	for (int i=0; i < animation.size(); i++)
	{
		GetPosRot(pos, quat, i, offset, obj->position, obj->channels);
		if (i == 0)
		{
			bone->SetPosition(pos);
			bone->SetRotationQuat(quat);
		}
		a1pos[i] = pos;
		a1quat[i] = quat;
	}

	BoneAnimation* ba = new BoneAnimation();
	ba->SetTranslations(animation.size(), a1pos);
	ba->SetRotations(animation.size(), a1quat);
	bone->AddAnimation(ba);

	safe_delete_array(a1pos);
	safe_delete_array(a1quat);

	offset += obj->channels.size();
	
	for (int i=0; i < obj->children.size(); i++)
	{
		Bone* child = ConvertToBones(obj->children[i], offset);
		bone->AttachChild(child);
	}
	return bone;
}

//	bvh			->	'HIERARCHY' 'ROOT' ident '{' hierarchy '}'
//					motion
//
void BVH::ParseBVH()
{
	GetCompulsaryToken(Get("HIERARCHY"));
	if (error) return;
	root = Hierarchy(NULL);
	if (error) return;
	Motion();
}

void BVH::Get3Floats(float& f1, float& f2, float& f3)
{
	GetCompulsaryTokenOption(TFLOAT,TINT);
	if (error) return;
	if (token == TINT) f1 = (float)yyInt;
	if (token == TFLOAT) f1 = yyFloat;

	GetCompulsaryTokenOption(TFLOAT,TINT);
	if (error) return;
	if (token == TINT) f2 = (float)yyInt;
	if (token == TFLOAT) f2 = yyFloat;

	GetCompulsaryTokenOption(TFLOAT,TINT);
	if (error) return;
	if (token == TINT) f3 = (float)yyInt;
	if (token == TFLOAT) f3 = yyFloat;
}

int BVH::GetChannelCount(HierarchyObj* root)
{
	int total = 0;
	if (root != NULL)
	{
		total += root->channels.size();
		for (int i=0; i < root->children.size(); i++)
		{
			total += GetChannelCount(root->children[i]);
		}
	}
	return total;
}

//	hierarchy	->	'ROOT' ident '{' offsetChannels hierarchy '}'			|
//					'JOINT' ident '{' offsetChannels hierarchy'}'			|
//					'End' 'Site' '{' 'OFFSET' f f f '}'						|
//					{e}
//
BVH::HierarchyObj* BVH::Hierarchy(HierarchyObj* parent)
{
	HierarchyObj*	root = NULL;

	token = GetNextToken();
	if (token == Get("ROOT"))
	{
		GetCompulsaryToken(TIDENT);
		std::string name = yyString;
		if (error) return NULL;
		GetCompulsaryToken(Get("{"));
		if (error) return NULL;
		float o1,o2,o3;
		std::vector<int> channels;
		OffsetChannels(o1,o2,o3,channels);
		if (error) return NULL;
		root = new HierarchyObj();
		root->position = D3DXVECTOR3(o1,o2,o3);
		root->channels = channels;
		root->name = name;
		HierarchyObj* obj = Hierarchy(root);
		if (error) return NULL;
		root->children.push_back(obj);
		GetCompulsaryToken(Get("}"));
		if (error) return NULL;
	}
	else if (token == Get("JOINT"))
	{
		GetCompulsaryToken(TIDENT);
		std::string name = yyString;
		if (error) return NULL;
		GetCompulsaryToken(Get("{"));
		if (error) return NULL;
		float o1,o2,o3;
		std::vector<int> channels;
		OffsetChannels(o1,o2,o3,channels);
		if (error) return NULL;

		root = new HierarchyObj();
		root->position = D3DXVECTOR3(o1,o2,o3);
		root->channels = channels;
		root->name = name;

		HierarchyObj* obj = Hierarchy(root);
		if (error) return NULL;
		root->children.push_back(obj);

		token = GetNextToken();
		UngetToken();
		if (token == Get("JOINT") || token == Get("End"))
		{
			if (parent == NULL)
				throw new Exception("hierarchy on null parent");
			HierarchyObj* obj = Hierarchy(parent);
			if (error) return NULL;
			parent->children.push_back(obj);
		}
		GetCompulsaryToken(Get("}"));
		if (error) return NULL;
	}
	else if (token == Get("End"))
	{
		GetCompulsaryToken(Get("Site"));
		if (error) return NULL;
		GetCompulsaryToken(Get("{"));
		if (error) return NULL;
		GetCompulsaryToken(Get("OFFSET"));
		if (error) return NULL;
		float o1,o2,o3;
		Get3Floats(o1,o2,o3);
		if (error) return NULL;
		GetCompulsaryToken(Get("}"));
		if (error) return NULL;

		root = new HierarchyObj();
		root->position = D3DXVECTOR3(o1,o2,o3);
	}

	token = GetNextToken();
	UngetToken();
	if (token == Get("JOINT") || token == Get("End"))
	{
		HierarchyObj* obj = Hierarchy(root);
		if (error) return NULL;

		if (root == NULL)
			return obj;
		parent->children.push_back(obj);
	}

	if (root == NULL)
		throw new Exception("root obj is null");

	return root;
}

//	offsetChannels	->	'OFFSET' f f f
//						[ 'CHANNELS' i ['Xposition' 'Yposition' 'Zposition' 'Xrotation' 'Zrotation' 'Yrotation'] ]
//
void BVH::OffsetChannels(float& o1, float& o2, float& o3, std::vector<int>& channels)
{
	GetCompulsaryToken(Get("OFFSET"));
	if (error) return;
	Get3Floats(o1,o2,o3);
	if (error) return;

	GetCompulsaryToken(Get("CHANNELS"));
	if (error) return;
	GetCompulsaryToken(TINT);
	if (error) return;
	if (yyInt != 6 && yyInt != 3)
	{
		SetError("bvh: can only deal with 3 or 6 channels");
		return;
	}

	int numChannels = yyInt;
	for (int i=0; i < numChannels; i++)
	{
		token = GetNextToken();
		if (token == Get("Xposition"))
		{
			channels.push_back(X_POS);
		}
		else if (token == Get("Yposition"))
		{
			channels.push_back(Y_POS);
		}
		else if (token == Get("Zposition"))
		{
			channels.push_back(Z_POS);
		}
		else if (token == Get("Xrotation"))
		{
			channels.push_back(X_ROT);
		}
		else if (token == Get("Yrotation"))
		{
			channels.push_back(Y_ROT);
		}
		else if (token == Get("Zrotation"))
		{
			channels.push_back(Z_ROT);
		}
		else
		{
			SetError("bvh: expected position or rotation channel");
			return;
		}
	}
}

//	motion		->	'MOTION' 'Frames' ':' i
//					'Frame' 'Time' ':' f
//					[f f f f f...]+
//
void BVH::Motion()
{
	GetCompulsaryToken(Get("MOTION"));
	if (error) return;
	GetCompulsaryToken(Get("Frames"));
	if (error) return;
	GetCompulsaryToken(Get(":"));
	if (error) return;
	GetCompulsaryToken(TINT);
	if (error) return;
	int numFrames = yyInt;

	GetCompulsaryToken(Get("Frame"));
	if (error) return;
	GetCompulsaryToken(Get("Time"));
	if (error) return;
	GetCompulsaryToken(Get(":"));
	if (error) return;

	GetCompulsaryTokenOption(TFLOAT,TINT);
	if (error) return;
	if (token == TINT) frameTime = (float)yyInt;
	if (token == TFLOAT) frameTime = yyFloat;

	channelCount = BVH::GetChannelCount(root);
	if (channelCount == 0)
		throw new Exception("channel count zero");

	for (int i=0; i < numFrames; i++)
	{
		animation.push_back(new float[channelCount]);
		for (int j=0; j < channelCount; j++)
		{
			GetCompulsaryToken(TFLOAT);
			if (error) return;
			animation[i][j] = yyFloat;
		}
	}
}
