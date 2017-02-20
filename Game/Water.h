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

///////////////////////////////////////////////////////////

class Water : public WorldObject
{
public:
	Water();
	~Water();
	Water(const Water&);
	const Water& operator=(const Water&);

	// draw it
	virtual void Draw(double time);
	// draw it in an offset position
	void Draw(double time, const D3DXVECTOR3& zeroOffset);

	// is the point inside the 2d region
	bool Inside(const D3DXVECTOR3& p);

	// getters and setters
	float GetWidth();
	void SetWidth(float width);

	float GetHeight();
	void SetHeight(float height);

	// overwrite
	void UpdateBoundingBox();

	// save and load region
	virtual void Read(XmlNode* node);
	virtual XmlNode* Write();

	// return the signature of this object
	virtual std::string Signature()
	{
		return waterSignature;
	}

private:

	// dimensions of region
	float width;
	float height;

	static std::string waterSignature;
	static int waterVersion;
};

