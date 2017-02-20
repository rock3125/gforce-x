#include "standardFirst.h"

#include "system/model/prs.h"

#pragma warning(disable:4355)

std::string PRS::prsSignature="prs";
int PRS::prsVersion=1;

/////////////////////////////////////////////////////////////////////////////

PRS::PRS()
	: parent(NULL)
{
	SetType(OT_PRS);

	parentOid=System::GetInvalidOid();

	D3DXQuaternionIdentity(&localRotation);
	D3DXMatrixIdentity(&localRotationMatrix);
	D3DXMatrixIdentity(&localTransform);

	D3DXMatrixIdentity(&worldTransform);
	D3DXQuaternionIdentity(&worldRotation);

	D3DXMatrixIdentity(&invWorldTransform);
	D3DXMatrixIdentity(&invLocalTransform);

	parentOid=System::GetInvalidOid();
	outofdate=true;
	localPosition=D3DXVECTOR3(0,0,0);
	localScale=D3DXVECTOR3(1,1,1);
	worldPosition=D3DXVECTOR3(0,0,0);
}

PRS::PRS(const PRS& prs)
{
	operator=(prs);
}

PRS::~PRS()
{
	// TODO: seems to be an error with the memory
	// management surrounding deleting children
	// somewhere there is a double pointer delete - FIND IT!
	//ClearAndDeleteChildren();

	if (parent!=NULL)
	{
		parent->DetachChild(this);
	}
	parent=NULL;
}

const PRS& PRS::operator=(const PRS& prs)
{
	Object::operator=(prs);

	localPosition = prs.localPosition;
	localRotation = prs.localRotation;
	localScale = prs.localScale;
	parentOid = prs.parentOid;
	boundingBox = prs.boundingBox;

	OutOfDate();

	return *this;
}

BoundingBox* PRS::GetBoundingBox()
{
	return &boundingBox;
}

void PRS::UpdateBoundingBox()
{
	boundingBox.Update(GetWorldTransform());
}

void PRS::AttachChild(PRS* node)
{
	if (node!=NULL && node!=this)
	{
		std::vector<PRS*>::iterator pos=std::find(children.begin(),children.end(),node);
		if (pos==children.end())
		{
			if (parent!=NULL)
			{
				parent->DetachChild(node);
			}
			children.push_back(node);
			node->SetParent(this);
			node->OutOfDate();
			node->UpdateBoundingBox();
		}
	}
}

void PRS::DetachChild(PRS* node)
{
	std::vector<PRS*>::iterator pos=std::find(children.begin(),children.end(),node);
	if (pos!=children.end())
	{
		node->parent=NULL;
		node->UpdateBoundingBox();
		children.erase(pos);
	}
}

void PRS::DetachFromParent()
{
	if (parent!=NULL)
	{
		parent->DetachChild(this);
		UpdateBoundingBox();
	}
}

void PRS::ClearAndDeleteChildren()
{
	for (int i=0; i<children.size(); i++)
	{
		PRS* parent=children[i]->GetParent();
		if (parent!=NULL)
		{
			parent->DetachChild(children[i]);
		}
		safe_delete(children[i]);
	}
	children.clear();
}

void PRS::ClearHierarchy()
{
	children.clear();
	parentOid = 0;
	parent = NULL;
}

void PRS::UpdateWorldTransformHierarchy()
{
	UpdateWorldTransform();
	for (int i=0; i < children.size(); i++)
	{
		children[i]->UpdateWorldTransformHierarchy();
	}
}

void PRS::UpdateWorldTransform()
{
	D3DXMATRIXA16 translation,localScaleMatrix;

	// calculate local transform and inverse
	D3DXMatrixTranslation(&translation,localPosition.x,localPosition.y,localPosition.z);
	D3DXMatrixScaling(&localScaleMatrix,localScale.x,localScale.y,localScale.z);
	D3DXMatrixRotationQuaternion(&localRotationMatrix,&localRotation);

	D3DXMatrixMultiply(&localTransform,&localRotationMatrix,&translation);
	D3DXMatrixMultiply(&localTransform,&localScaleMatrix,&localTransform);
	D3DXMatrixInverse(&invLocalTransform, NULL,&localTransform);

	if (parent!=NULL)
	{
		D3DXMatrixMultiply(&worldTransform,&localTransform,&parent->GetWorldTransform());
		D3DXMatrixInverse(&invWorldTransform,NULL,&worldTransform);

		// transform local position and rotation by parent's transform
		D3DXVec3TransformCoord(&worldPosition,&localPosition,&parent->GetWorldTransform());
		D3DXQuaternionMultiply(&worldRotation,&localRotation,&parent->GetWorldRotationQuat());
	}
	else
	{
		// local transform is world transform
		worldTransform=localTransform;
		invWorldTransform=invLocalTransform;

		worldPosition=localPosition;
		worldRotation=localRotation;
	}

	outofdate=false;

	// setup my bounding box too - this MUST be after outofdate=false;
	UpdateBoundingBox();
}

void PRS::OutOfDate()
{
	// invalidate me and my children
	outofdate=true;
	for (int i=0; i<children.size(); i++)
	{
		children[i]->OutOfDate();
	}
}

//////////////////////////////////////////////////////////////////////////////////
// Local transform accessors

const D3DXVECTOR3& PRS::GetPosition()
{
	if (outofdate)
		UpdateWorldTransform();
	return localPosition;
}

const D3DXVECTOR3& PRS::GetScale()
{
	if (outofdate)
		UpdateWorldTransform();
	return localScale;
}

const D3DXQUATERNION& PRS::GetRotationQuat()
{
	if (outofdate)
		UpdateWorldTransform();
	return localRotation;
}

void PRS::SetPosition(const D3DXVECTOR3 &p)
{
    localPosition = p;
	OutOfDate();
}

void PRS::SetScale(const D3DXVECTOR3 &p)
{
    localScale = p;
	OutOfDate();
}

void PRS::SetRotationEuler(const D3DXVECTOR3 &r)
{
	D3DXQuaternionRotationYawPitchRoll(&localRotation,r.y*System::deg2rad,r.x*System::deg2rad,r.z*System::deg2rad);
	OutOfDate();
}

void PRS::SetRotationQuat(const D3DXQUATERNION& q)
{
	localRotation = q;
	OutOfDate();
}

void PRS::Translate(const D3DXVECTOR3 &t)
{
	localPosition += t;
	OutOfDate();
}

void PRS::RotateEuler(const D3DXVECTOR3 &r)
{
	D3DXQUATERNION q;
	D3DXQuaternionRotationYawPitchRoll(&q, r.y*System::deg2rad, r.x*System::deg2rad, r.z*System::deg2rad);
	RotateQuat(q);
}

void PRS::RotateQuat(const D3DXQUATERNION& q)
{
	D3DXQuaternionMultiply(&localRotation,&q,&localRotation);
	OutOfDate();
}

const D3DXMATRIXA16& PRS::GetLocalRotationMatrix()
{
	if (outofdate)
		UpdateWorldTransform();

	return localRotationMatrix;
}

const D3DXMATRIXA16& PRS::GetLocalTransform()
{
	if (outofdate)
		UpdateWorldTransform();

	return localTransform;
}

const D3DXMATRIXA16& PRS::GetInvLocalTransform()
{
	if (outofdate)
		UpdateWorldTransform();

	return invLocalTransform;
}

const D3DXVECTOR3& PRS::GetWorldPosition()
{
	if (outofdate)
		UpdateWorldTransform();

	return worldPosition;
}

const D3DXQUATERNION& PRS::GetWorldRotationQuat()
{
	if (outofdate)
		UpdateWorldTransform();

	return worldRotation;
}

const D3DXMATRIXA16& PRS::GetWorldTransform()
{
	if (outofdate)
		UpdateWorldTransform();

	return worldTransform;
}

const D3DXMATRIXA16& PRS::GetInvWorldTransform()
{
	if (outofdate)
		UpdateWorldTransform();

	return invWorldTransform;
}

void PRS::SetLocalTransform(const D3DXVECTOR3& pos, const D3DXQUATERNION& quat)
{
	localPosition = pos;
	localRotation = quat;
	OutOfDate();
}

void PRS::SetLocalTransform(const D3DXMATRIXA16& mat)
{
	D3DXQUATERNION quat;
	D3DXQuaternionRotationMatrix(&quat,&mat);
	D3DXVECTOR3 pos = D3DXVECTOR3(mat._41,mat._42,mat._43);
	SetRotationQuat(quat);
	SetPosition(pos);
}

void PRS::SetParent(PRS* p, bool keepWorldPRS)
{
	if (p==NULL)
	{
		parent=NULL;
	}
	else
	{
		// detach from current parent
		if (parent!=NULL)
			parent->DetachChild(this);
		// attach to new parent
		parent=p;
		parent->AttachChild(this);
	}

	if (keepWorldPRS && p!=NULL)
	{
		D3DXQUATERNION invRot;
		D3DXQuaternionInverse(&invRot, &p->GetWorldRotationQuat());

		RotateQuat(invRot);
	}
	else if (keepWorldPRS && p==NULL && parent!=NULL)
	{
		// we are detaching from the parent so
		// take out the current parents transform
		// to maintain current world prs
		D3DXQUATERNION invRot;
		D3DXQuaternionInverse(&invRot,&parent->GetWorldRotationQuat());
		RotateQuat(invRot);
	}
	parent = p;
	OutOfDate();
	UpdateBoundingBox();
}

PRS* PRS::GetParent()
{
	return parent;
}

const std::vector<PRS*>& PRS::GetChildren() const
{
	return children;
}

int PRS::GetParentOid() const
{
	return parentOid;
}

bool PRS::HasChild(PRS* child) const
{
	if (child==NULL)
		return false;
	for (int i=0; i<children.size(); i++)
	{
		if (child->GetOid() == children[i]->GetOid())
			return true;
		else if (children[i]->HasChild(child))
			return true;
	}
	return false;
}

XmlNode* PRS::Write()
{
	XmlNode* node = XmlNode::NewChild(prsSignature, prsVersion);

	node->Add(Object::Write());

	node->Write("localPosition",localPosition);
	node->Write("localRotation",localRotation);
	node->Write("localScale",localScale);

	// save parent oid
	parentOid = System::GetInvalidOid();
	if (parent != NULL)
		parentOid = parent->GetOid();

	node->Write("parentOid",parentOid);
	return node;
}

void PRS::Read(XmlNode* node)
{
	XmlNode::CheckVersion(node, prsSignature, prsVersion);

	Object::Read(node->GetChild(Object::Signature()));

	// remove all my existing children
	ClearAndDeleteChildren();

	node->Read("localPosition", localPosition);
	node->Read("localRotation", localRotation);
	node->Read("localScale", localScale);

	// get ready to attach myself to my proper parent
	node->Read("parentOid", parentOid);

	// make sure I get calculated when needed
	OutOfDate();
}


