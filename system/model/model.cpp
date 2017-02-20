#include "standardFirst.h"

#include "system/model/model.h"
#include "system/model/mesh.h"
#include "d3d9/texture.h"

std::string Model::modelSignature="mod";
int Model::modelVersion=1;

/////////////////////////////////////////////////////////////////////////////

Model::Model()
	: mesh(NULL)
	, texture(NULL)
{
	SetType(OT_MODEL);
	SetWorldType(TYPE_MODEL);

	mesh = new Mesh();
	colour = D3DXCOLOR(1,1,1,1);
}

Model::~Model()
{
	safe_delete(mesh);
	texture = NULL;
}

Model::Model(const Model& t)
	: mesh(NULL)
	, texture(NULL)
{
	operator=(t);
}

const Model& Model::operator=(const Model& t)
{
	WorldObject::operator =(t);

	safe_delete(mesh);
	texture = t.texture;
	mesh = new Mesh(*t.mesh);
	colour = t.colour;

	return *this;
}

Mesh* Model::GetMesh()
{
	return mesh;
}

Mesh* Model::GetMesh(std::string name)
{
	if (mesh->GetName()==name)
		return mesh;
	std::vector<PRS*> children = GetChildren();
	for (int i=0; i<children.size(); i++)
	{
		Model* model = dynamic_cast<Model*>(children[i]);
		if (model!=NULL)
		{
			Mesh* m = model->GetMesh(name);
			if (m!=NULL)
				return m;
		}
	}
	return NULL;
}

void Model::UpdateBoundingBox()
{
	// copy bounding volume into the container
	D3DXVECTOR3 aabbMin,aabbMax;
	mesh->GetBoundingBox(aabbMin,aabbMax);
	GetBoundingBox()->SetMin(aabbMin);
	GetBoundingBox()->SetMax(aabbMax);
	WorldObject::UpdateBoundingBox();
}

void Model::SetMesh(Mesh* _mesh)
{
	PreCond(_mesh!=NULL);
	*mesh = *_mesh;
	UpdateBoundingBox();
}

void Model::ApplyTransformation()
{
	if (mesh!=NULL)
	{
		mesh->ApplyTransformation(GetLocalTransform());
		SetPosition(D3DXVECTOR3(0,0,0));
		SetScale(D3DXVECTOR3(1,1,1));
		SetRotationQuat(*System::GetIdentityQuaternion());
		UpdateBoundingBox();
	}
}

Texture* Model::GetTexture()
{
	return texture;
}

void Model::SetTexture(Texture* _texture)
{
	PreCond(_texture!=NULL);
	texture = _texture;
}

const D3DXCOLOR& Model::GetColour()
{
	return colour;
}

void Model::SetColour(const D3DXCOLOR& _colour)
{
	colour = _colour;
}

void Model::InvalidateDeviceObjects()
{
	mesh->InvalidateDeviceObjects();
}

void Model::RestoreDeviceObjects()
{
	mesh->RestoreDeviceObjects();
}

void Model::OptimiseMeshesRecursive(Model* model)
{
	if (model == NULL)
		return;
	if (model->mesh != NULL)
	{
		model->mesh->OptimiseMesh();
	}

	std::vector<PRS*> children = model->GetChildren();
	for (int i=0; i < children.size(); i++)
	{
		OptimiseMeshesRecursive(dynamic_cast<Model*>(children[i]));
	}
}

void Model::OptimiseMeshes()
{
	OptimiseMeshesRecursive(this);
}

void Model::Draw(double time)
{
	Device* dev=Interface::GetDevice();

	dev->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TEXTURE);
	dev->SetTextureStageState(0,D3DTSS_COLORARG2,D3DTA_TFACTOR);
	dev->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_MODULATE);

	if (texture == NULL)
	{
		if (colour.a < 1)
		{
			dev->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
			dev->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);
			dev->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_TEXTURE);
			dev->SetTextureStageState(0,D3DTSS_ALPHAARG2,D3DTA_TFACTOR);
			dev->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_SELECTARG2);
			dev->SetRenderState(D3DRS_ALPHATESTENABLE,FALSE);
			dev->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);
		}
		else
		{
			dev->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);
		}
		dev->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_SELECTARG2);
	}
	else
	{
		if (colour.a < 1)
		{
			dev->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
			dev->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);
			dev->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_TEXTURE);
			dev->SetTextureStageState(0,D3DTSS_ALPHAARG2,D3DTA_TFACTOR);
			dev->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_MODULATE);
			dev->SetRenderState(D3DRS_ALPHATESTENABLE,FALSE);
			dev->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);
		}
		else
		{
			dev->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_SELECTARG1);
			dev->SetRenderState(D3DRS_ALPHATESTENABLE,FALSE);
			dev->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);
		}
	}

	dev->SetRenderState(D3DRS_TEXTUREFACTOR,colour);
	dev->SetTexture(0,texture);

	dev->RenderMesh(mesh,GetWorldTransform());
}

void Model::Draw(double time, const D3DXVECTOR3& pos)
{
	Device* dev=Interface::GetDevice();

	dev->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TEXTURE);
	dev->SetTextureStageState(0,D3DTSS_COLORARG2,D3DTA_TFACTOR);
	dev->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_MODULATE);

	if (texture == NULL)
	{
		if (colour.a < 1)
		{
			dev->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
			dev->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);
			dev->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_TEXTURE);
			dev->SetTextureStageState(0,D3DTSS_ALPHAARG2,D3DTA_TFACTOR);
			dev->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_SELECTARG2);
			dev->SetRenderState(D3DRS_ALPHATESTENABLE,FALSE);
			dev->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);
		}
		else
		{
			dev->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);
		}
		dev->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_SELECTARG2);
	}
	else
	{
		if (colour.a < 1)
		{
			dev->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
			dev->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);
			dev->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_TEXTURE);
			dev->SetTextureStageState(0,D3DTSS_ALPHAARG2,D3DTA_TFACTOR);
			dev->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_MODULATE);
			dev->SetRenderState(D3DRS_ALPHATESTENABLE,FALSE);
			dev->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);
		}
		else
		{
			dev->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_SELECTARG1);
			dev->SetRenderState(D3DRS_ALPHATESTENABLE,FALSE);
			dev->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);
		}
	}

	dev->SetRenderState(D3DRS_TEXTUREFACTOR,colour);
	dev->SetTexture(0,texture);

	D3DXMATRIXA16 matrix = GetWorldTransform();
	matrix._41 -= pos.x;
	matrix._42 -= pos.y;
	matrix._43 -= pos.z;
	dev->RenderMesh(mesh,matrix);
}

XmlNode* Model::Write()
{
	XmlNode* node = XmlNode::NewChild(modelSignature, modelVersion);

	node->Add(WorldObject::Write());

	node->Add(mesh->Write());

	std::string textureFilename;
	if (texture != NULL)
	{
		textureFilename = texture->GetFilename();
	}
	node->Write("texture", textureFilename);
	node->Write("colour",colour);

	return node;
}

void Model::Read(XmlNode* node)
{
	XmlNode::CheckVersion(node, modelSignature, modelVersion);

	WorldObject::Read(node->GetChild(WorldObject::Signature()));

	safe_delete(mesh);
	texture = NULL;

	mesh = new Mesh();
	mesh->Read(node->GetChild(Mesh::Signature()));

	std::string textureFilename;
	node->Read("texture", textureFilename);
	if (!textureFilename.empty())
	{
		texture = TextureCache::GetTexture(textureFilename);
	}

	node->Read("colour",colour);

	UpdateBoundingBox();
}

