#include "standardFirst.h"

#include "d3d9/texture.h"

#include "system/skin/skin.h"
#include "system/model/mesh.h"
#include "system/skin/bone.h"
#include "system/skin/boneAnimation.h"
#include "system/skin/boneInfluence.h"
#include "system/objectFactory.h"

std::string Skin::skinSignature="skin";
int Skin::skinVersion=1;

/////////////////////////////////////////////////////////////////////////////

Skin::Skin()
	: rootBone(NULL)
	, vertexInfluences(NULL)
	, skin(NULL)
	, texture(NULL)
{
	SetType(OT_SKIN);
	SetWorldType(TYPE_SKIN);

	numVertexInfluences = 0;
	colour = D3DXCOLOR(1,1,1,1);
}

Skin::~Skin()
{
	safe_delete(skin);
	safe_delete(rootBone);
	safe_delete_array(vertexInfluences);
	numVertexInfluences = 0;
}

Skin::Skin(const Skin& s)
	: rootBone(NULL)
	, vertexInfluences(NULL)
	, skin(NULL)
	, texture(NULL)
{
	operator=(s);
}

const Skin& Skin::operator=(const Skin& s)
{
	WorldObject::operator=(s);

	safe_delete(rootBone);
	safe_delete(skin);
	safe_delete_array(vertexInfluences);
	texture = s.texture;

	numVertexInfluences = s.numVertexInfluences;
	if (s.rootBone!=NULL)
	{
		rootBone = new Bone(*s.rootBone);
	}

	if (s.skin!=NULL)
	{
		skin = new Mesh(*s.skin);
	}

	if (numVertexInfluences>0)
	{
		vertexInfluences = new BoneInfluence[numVertexInfluences];
		for (int i=0; i<numVertexInfluences; i++)
		{
			vertexInfluences[i] = s.vertexInfluences[i];
		}
	}
	return *this;
}

Bone* Skin::GetRootBone()
{
	return rootBone;
}

void Skin::InvalidateDeviceObjects()
{
	skin->InvalidateDeviceObjects();
}

void Skin::RestoreDeviceObjects()
{
	skin->RestoreDeviceObjects();
}

void Skin::SetRootBone(Bone* _rootBone)
{
	PreCond(_rootBone!=NULL);
	safe_delete(rootBone);
	rootBone = dynamic_cast<Bone*>(ObjectFactory::DeepCopy(_rootBone));
}

void Skin::SetTexture(Texture* _texture)
{
	texture = _texture;
}

Mesh* Skin::GetSkin()
{
	return skin;
}

void Skin::SetSkin(Mesh* _skin)
{
	safe_delete(skin);
	if (_skin!=NULL)
	{
		skin = new Mesh(*_skin);
		skin->UseForSkinning();

		// copy bounding volume into the container
		D3DXVECTOR3 aabbMin,aabbMax;
		skin->GetBoundingBox(aabbMin,aabbMax);
		GetBoundingBox()->SetMin(aabbMin);
		GetBoundingBox()->SetMax(aabbMax);
		UpdateBoundingBox();
	}
}

BoneInfluence* Skin::GetBoneInfluences()
{
	return vertexInfluences;
}

void Skin::SetBoneInfluences(int _numVertexInfluences,BoneInfluence* _vertexInfluences)
{
	safe_delete_array(vertexInfluences);
	numVertexInfluences = _numVertexInfluences;

	if (numVertexInfluences > 0)
	{
		vertexInfluences = new BoneInfluence[numVertexInfluences];
		for (int i=0; i<numVertexInfluences; i++)
		{
			vertexInfluences[i] = _vertexInfluences[i];
		}
	}
}

void Skin::SetTime(int animId, float time)
{
	// get animation parameters
	float fps = (float)BoneAnimation::GetFramesPerSecond();
	float fdure = BoneAnimation::GetFrameDuration();
	int numFrames = rootBone->GetNumFrames(animId);

	// turn time into indices
	// multiply by fps (since 1 sec -> index = # fps)
	int timeIndex1 = (int)(time*fps);
	// next frame after this one
	int timeIndex2 = timeIndex1 + 1;

	// time into frame
	float perc2 = time - ((float)timeIndex1 / fps);
	// make it a percentage [0..1]
	perc2 = perc2 / fdure;
	float perc1 = 1.0f - perc2;

	Skin::SetTime(rootBone, animId, time, true, timeIndex1 % numFrames, perc1, timeIndex2 % numFrames, perc2);
}

void Skin::SetTime(Bone* bone, int animId, float time, bool root,
				   int timeIndex1, float perc1, int timeIndex2, float perc2)
{
	if (bone!=NULL)
	{
		BoneAnimation* ba = bone->GetAnimation(animId);
		if (ba != NULL)
		{
			D3DXQUATERNION* qlist = ba->GetRotations();
			D3DXVECTOR3* plist = ba->GetTranslations();

			D3DXQUATERNION* q1;
			D3DXQUATERNION* q2;
			D3DXQUATERNION quat;
			D3DXVECTOR3* v1;
			D3DXVECTOR3* v2;
			D3DXVECTOR3 pos;

			q1 = &qlist[timeIndex1];
			v1 = &plist[timeIndex1];

			q2 = &qlist[timeIndex2];
			v2 = &plist[timeIndex2];

			// verified - correct order
			D3DXQuaternionSlerp(&quat, q1, q2, perc2);

			pos.x = v1->x*perc1 + v2->x*perc2;
			pos.y = v1->y*perc1 + v2->y*perc2;
			pos.z = v1->z*perc1 + v2->z*perc2; //(root)?0:(v1->z*perc1+v2->z*perc2);

			bone->SetLocalTransform(pos,quat);
		}

		// and all my children too
		std::vector<PRS*> children = bone->GetChildren();
		for (int i=0; i<children.size(); i++)
		{
			SetTime(dynamic_cast<Bone*>(children[i]), animId, time, false, 
					timeIndex1, perc1, timeIndex2, perc2);
		}

	}
}

void Skin::Update()
{
	D3DXMATRIXA16 transform;
	if (skin==NULL || numVertexInfluences == 0) return;

	if (numVertexInfluences == skin->GetNumVertices())
	{
		// make sure all matrices are up to date
		rootBone->UpdateWorldTransformHierarchy();

		// get old vertices (originals) and lock vertex buffer for update
		D3DPVERTEX* source = skin->GetVertices();
		D3DPVERTEX* dest = NULL;
		skin->LockVertexBuffer((void**)&dest);
		if (dest == NULL) return;

		// predefine vars
		Bone** bones;
		float* weights;
		int count;
		BoneInfluence* infl;
		float* sourceMatrix;
		float* destinationMatrix;

		// for each vertex perform a weighted average of the bone matrices
		int numVertices = skin->GetNumVertices();

//		Time::GetTimer().Update();
//		double dt1 = Time::GetTimer().CurrentTime();
/*
		// C++ algorithm
		for (int i=0; i<numVertices; i++)
		{
			infl = &vertexInfluences[i];
			count = infl->count;
			weights = infl->weights;
			bones = infl->bones;

			sourceMatrix = &bones[0]->worldTransform._11;
			destinationMatrix = &transform._11;
			transform = (weights[0] * bones[0]->worldTransform);
			for (int j=1; j < count; j++)
			{
				transform += (weights[j] * bones[j]->worldTransform);
			}
			D3DXVec3TransformCoord(&dest[i].position, &source[i].position, &transform);
			D3DXVec3TransformNormal(&dest[i].normal, &source[i].normal, &transform);
//			dest[i].texture = source[i].texture;
		}
*/

		destinationMatrix = &transform._11;

		__m128 weightReg;
		__m128 dest1;
		__m128 dest2;
		__m128 dest3;
		__m128 dest4;
		__m128 src1;
		__m128 src2;

		for (int i=0; i<numVertices; i++)
		{
			infl = &vertexInfluences[i];
			count = infl->count;
			weights = infl->weights;
			bones = infl->bones;

			sourceMatrix = &bones[0]->worldTransform._11;

			// weight into all 4 float values of the 128 bit register
			weightReg = _mm_load_ps1(&weights[0]);

			// load un-aligned source matrix into dest1 128 bit reg
			// matrix column 1 and 2
			dest1 = _mm_loadu_ps(&sourceMatrix[0]);
			dest2 = _mm_loadu_ps(&sourceMatrix[4]);
			// mutliply both by weights
			dest1 = _mm_mul_ps(dest1, weightReg);
			dest2 = _mm_mul_ps(dest2, weightReg);

			// next two columns
			dest3 = _mm_loadu_ps(&sourceMatrix[8]);
			dest4 = _mm_loadu_ps(&sourceMatrix[12]);
			dest3 = _mm_mul_ps(dest3, weightReg);
			dest4 = _mm_mul_ps(dest4, weightReg);

			// save 4 dest registers into the transform matrix
			_mm_store_ps(&destinationMatrix[0], dest1);
			_mm_store_ps(&destinationMatrix[4], dest2);
			_mm_store_ps(&destinationMatrix[8], dest3);
			_mm_store_ps(&destinationMatrix[12], dest4);

			for (int j=1; j < count; j++)
			{
				sourceMatrix = &bones[j]->worldTransform._11;

				// get weight
				weightReg = _mm_load_ps1(&weights[j]);

				// get new bone weights, first two columns and scale them
				src1 = _mm_loadu_ps(&sourceMatrix[0]);
				src2 = _mm_loadu_ps(&sourceMatrix[4]);
				src1 = _mm_mul_ps(src1, weightReg);
				src2 = _mm_mul_ps(src2, weightReg);

				// get previous transform value first two columns
				// and add new weighted values to them
				dest1 = _mm_loadu_ps(&destinationMatrix[0]);
				dest2 = _mm_loadu_ps(&destinationMatrix[4]);
				dest1 = _mm_add_ps(dest1, src1);
				dest2 = _mm_add_ps(dest2, src2);

				// next two columns of bone-weights and scale
				src1 = _mm_loadu_ps(&sourceMatrix[8]);
				src2 = _mm_loadu_ps(&sourceMatrix[12]);
				src1 = _mm_mul_ps(src1, weightReg);
				src2 = _mm_mul_ps(src2, weightReg);

				// next two previous values - load and add new weights
				dest3 = _mm_loadu_ps(&destinationMatrix[8]);
				dest4 = _mm_loadu_ps(&destinationMatrix[12]);
				dest3 = _mm_add_ps(dest3, src1);
				dest4 = _mm_add_ps(dest4, src2);

				// save 4 dest registers into the transform matrix
				_mm_store_ps(&destinationMatrix[0], dest1);
				_mm_store_ps(&destinationMatrix[4], dest2);
				_mm_store_ps(&destinationMatrix[8], dest3);
				_mm_store_ps(&destinationMatrix[12], dest4);
			}
			D3DXVec3TransformCoord(&dest[i].position, &source[i].position, &transform);
			D3DXVec3TransformNormal(&dest[i].normal, &source[i].normal, &transform);
		}

		skin->UnlockVertexBuffer();
	}
}

void Skin::DrawSkeleton(Bone* root,const D3DXMATRIXA16* worldMatrix)
{
	Device* dev=Interface::GetDevice();

	D3DXMATRIXA16 newMatrix;
	D3DXMatrixMultiply(&newMatrix, &root->GetLocalTransform(), worldMatrix);

	// draw line from one to the next
	D3DXVECTOR3 v1,v2;
	v1 = D3DXVECTOR3(worldMatrix->_41, worldMatrix->_42, worldMatrix->_43);
	v2 = D3DXVECTOR3(newMatrix._41, newMatrix._42, newMatrix._43);

	dev->DrawLine(v1,v2,colour);

	std::vector<PRS*> children = root->GetChildren();
	for (int i=0; i<children.size(); i++)
	{
		DrawSkeleton(dynamic_cast<Bone*>(children[i]), &newMatrix);
	}
}

float time = 0;

void Skin::Draw()
{
	Device* dev=Interface::GetDevice();

	dev->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TEXTURE);
	dev->SetTextureStageState(0,D3DTSS_COLORARG2,D3DTA_TFACTOR);
	dev->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_SELECTARG1);
	dev->SetRenderState(D3DRS_ALPHATESTENABLE,FALSE);
	dev->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);
	dev->SetRenderState(D3DRS_TEXTUREFACTOR,(DWORD)colour);
	dev->SetTexture(0,texture);

	if (skin != NULL)
	{
		Update();
		dev->RenderMesh(skin,GetWorldTransform());
	}
	if (rootBone != NULL)
	{
		SetTime(0, time);

		time = time + 0.05f;

		dev->SetWorldTransform(System::GetIdentityMatrix());
		DrawSkeleton(rootBone, &GetWorldTransform());
	}
}

XmlNode* Skin::Write()
{
	XmlNode* node = XmlNode::NewChild(skinSignature, skinVersion);

	node->Add(WorldObject::Write());

	node->Write("colour",colour);

	bool hasBones = (rootBone != NULL);
	node->Write("hasBones", hasBones);
	if (hasBones)
	{
//		rootBone->Write(f);
	}

	node->Write("numVertexInfluences", numVertexInfluences);
	if (numVertexInfluences > 0)
	{
		for (int i=0; i < numVertexInfluences; i++)
		{
//			vertexInfluences[i].Write(i,f);
		}
	}

	bool hasSkin = (skin != NULL);
	node->Write("hasSkin", hasSkin);
	if (hasSkin)
	{
//		skin->Write(f);
	}

	std::string textureFilename;
	if (texture != NULL)
	{
		textureFilename = texture->GetFilename();
	}
	node->Write("textureFilename", textureFilename);

	return node;
}

void Skin::Read(XmlNode* node)
{
	XmlNode::CheckVersion(node, skinSignature, skinVersion);

	WorldObject::Read(node->GetChild(WorldObject::Signature()));

	node->Read("colour",colour);

/*
	safe_delete(rootBone);
	bool hasBones;
	node->Read("hasBones", hasBones);
	if (hasBones)
	{
		rootBone = new Bone();
		rootBone->Read(f);
	}

	node->Read("numVertexInfluences",numVertexInfluences);
	if (numVertexInfluences > 0)
	{
		safe_delete_array(vertexInfluences);
		vertexInfluences = new BoneInfluence[numVertexInfluences];
		for (int i=0; i < numVertexInfluences; i++)
		{
			vertexInfluences[i].Read(i,rootBone, f);
		}
	}

	bool hasSkin;
	node->Read("hasSkin", hasSkin);
	if (hasSkin)
	{
		safe_delete(skin);
		skin = new Mesh();
		skin->Read(f);
	}
*/

	std::string textureFilename;
	node->Read("textureFilename", textureFilename);
	if (!textureFilename.empty())
	{
		texture = TextureCache::GetTexture(textureFilename);
	}
}
