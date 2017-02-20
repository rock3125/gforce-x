#pragma once

#include "system/model/object.h"
#include "system/model/boundingBox.h"

/////////////////////////////////////////////////////////////////////////////

class PRS : public Object
{
public:
	PRS();
	PRS(const PRS&);
	virtual ~PRS();
	const PRS& operator=(const PRS& t);

	// updates world (absolute) position by combining
	// local transform with parent's world transform
	virtual void UpdateWorldTransform();
	// update my worldtransform and anyone in my heirarchy too
	void UpdateWorldTransformHierarchy();

	// marks itself and all children as dirty (update their transform matrices next update loop)
	virtual void OutOfDate();

	// clean all attached children's memory
	void ClearAndDeleteChildren();

	// clean all attached children but not their memory and set parent to NULL
	void ClearHierarchy();

	// get local transformations
	virtual const D3DXVECTOR3& GetPosition();
	virtual const D3DXQUATERNION& GetRotationQuat();
	virtual const D3DXVECTOR3& GetScale();

	// set local transformations
	virtual void SetPosition(const D3DXVECTOR3& p);
	virtual void SetScale(const D3DXVECTOR3& p);
	virtual void SetRotationEuler(const D3DXVECTOR3& r);
	virtual void SetRotationQuat(const D3DXQUATERNION& q);
	virtual void SetLocalTransform(const D3DXMATRIXA16& m);
	virtual void SetLocalTransform(const D3DXVECTOR3&, const D3DXQUATERNION&);

	// modify local transformations
	virtual void Translate(const D3DXVECTOR3& t);
	virtual void RotateEuler(const D3DXVECTOR3& t);
	virtual void RotateQuat(const D3DXQUATERNION& q);

	// gets world space transformations
	virtual const D3DXVECTOR3& GetWorldPosition();
	virtual const D3DXQUATERNION& GetWorldRotationQuat();

	// get matrix to transform this point into world space
	virtual const D3DXMATRIXA16& GetWorldTransform();
	virtual const D3DXMATRIXA16& GetLocalTransform();
	virtual const D3DXMATRIXA16& GetLocalRotationMatrix();

	virtual const D3DXMATRIXA16& GetInvWorldTransform();
	virtual const D3DXMATRIXA16& GetInvLocalTransform();

	// parent management
	virtual void SetParent(PRS* p, bool keepWorldPRS=false);
	virtual PRS* GetParent();

	// child management
	virtual void AttachChild(PRS* node);
	virtual void DetachChild(PRS* node);
	virtual void DetachFromParent();
	virtual const std::vector<PRS*>& GetChildren() const;
	bool HasChild(PRS* child) const;

	// access bounding volume
	virtual BoundingBox* GetBoundingBox();

	// update internal bounding volume
	virtual void UpdateBoundingBox();

	// get parent oid (or invalid oid) - MUST NOT BE SET - auto maintained
	virtual int GetParentOid() const;

	// save and load PRS
	virtual XmlNode* Write();
	virtual void Read(XmlNode*);

	// return the signature of this object
	virtual std::string Signature()
	{
		return prsSignature;
	}

protected:
	friend class Skin;

	// prs
	D3DXVECTOR3		localPosition;
	D3DXQUATERNION	localRotation;
	D3DXVECTOR3		localScale;
	D3DXMATRIXA16	localRotationMatrix;

	// matrices calculated from prs
	D3DXVECTOR3		worldPosition;
	D3DXQUATERNION	worldRotation;

	// world matrix
	D3DXMATRIXA16 	localTransform;
	D3DXMATRIXA16 	worldTransform;

	// inverse world matrix
	D3DXMATRIXA16 	invWorldTransform;
	D3DXMATRIXA16 	invLocalTransform;

private:
	PRS*				parent;
	int					parentOid;
	std::vector<PRS*>	children;

	// bounding volume of this object
	BoundingBox			boundingBox;

	// don't save - local only
	mutable bool outofdate;

	static std::string prsSignature;
	static int prsVersion;
};

