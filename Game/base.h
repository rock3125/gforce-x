//////////////////////////////////////////////////////////////////////
/// GRAVITY FORCE X
///
/// (c)Copyright Peter de Vocht, 2006
///
/// I'm releasing this code under GPL v2.1 (see included file lgpl.txt)
///
//////////////////////////////////////////////////////////////////////

#pragma once

#include "system/model/worldObject.h"

class Model;

/////////////////////////////////////////////////////////////////////////////

class Base : public WorldObject
{
public:
	Base();
	~Base();
	Base(const Base&);
	const Base& operator=(const Base&);

	// draw the base
	virtual void Draw(double time, const D3DXVECTOR3& pos);
	void Draw(double time);

	// get set base model
	Model* GetModel();
	void SetModel(Model* _model);

	// get square platform top
	void GetTransformedAABB(D3DXVECTOR3& aabbmin, D3DXVECTOR3& aabbmax);

	// save and load base
	virtual void Read(XmlNode* node);
	virtual XmlNode* Write();

	// update internal bounding volume
	virtual void UpdateBoundingBox();
	virtual BoundingBox* GetBoundingBox();

	// return the signature of this object
	virtual std::string Signature()
	{
		return baseSignature;
	}

private:
	// model to be drawn
	Model*	model;

	// file specific
	static std::string baseSignature;
	static int baseVersion;
};

