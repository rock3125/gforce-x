//////////////////////////////////////////////////////////////////////
/// GRAVITY FORCE X
///
/// (c)Copyright Peter de Vocht, 2006
///
/// I'm releasing this code under GPL v2.1 (see included file lgpl.txt)
///
//////////////////////////////////////////////////////////////////////

#include "standardFirst.h"

#include "game/base.h"

#include "system/model/model.h"

std::string Base::baseSignature="base";
int Base::baseVersion=1;

///////////////////////////////////////////////////////////

Base::Base()
	: model(NULL)
{
	SetType(OT_BASE);
	SetWorldType(TYPE_BASE);

	model = new Model();
}

Base::~Base()
{
	safe_delete(model);
}

Base::Base(const Base& b)
	: model(NULL)
{
	model = new Model();
	operator=(b);
}

const Base& Base::operator=(const Base& b)
{
	WorldObject::operator=(b);
	*model = *b.model;
	return *this;
}

void Base::GetTransformedAABB(D3DXVECTOR3& aabbmin, D3DXVECTOR3& aabbmax)
{
	BoundingBox* bb = model->GetBoundingBox();
	aabbmax = bb->GetTransformedMax();
	aabbmin = bb->GetTransformedMin();
}

void Base::Draw(double time, const D3DXVECTOR3& _pos)
{
	model->SetPosition(GetPosition());
	model->SetRotationQuat(GetRotationQuat());
	model->SetScale(GetScale());
	model->Draw(time, _pos);
}

void Base::Draw(double time)
{
	model->SetPosition(GetPosition());
	model->SetRotationQuat(GetRotationQuat());
	model->SetScale(GetScale());
	model->Draw(time);
}

Model* Base::GetModel()
{
	return model;
}

void Base::SetModel(Model* _model)
{
	*model = *_model;
	model->UpdateBoundingBox();
}

void Base::UpdateBoundingBox()
{
	if (model != NULL)
	{
		model->UpdateBoundingBox();
	}
}

BoundingBox* Base::GetBoundingBox()
{
	if (model != NULL)
		return model->GetBoundingBox();
	return NULL;
}

void Base::Read(XmlNode* node)
{
	XmlNode::CheckVersion(node, baseSignature, baseVersion);
	WorldObject::Read(node->GetChild(WorldObject::Signature()));
	model->Read(node->GetChild(model->Signature()));
	UpdateBoundingBox();
}

XmlNode* Base::Write()
{
	XmlNode* node = XmlNode::NewChild(baseSignature, baseVersion);
	node->Add(WorldObject::Write());
	node->Add(model->Write());
	return node;
}

