#pragma once

#include "system/model/worldObject.h"

/////////////////////////////////////////////////////////////////////////////

class Light : public WorldObject
{
public:
	Light();
	Light(const Light&);
	virtual ~Light();
	const Light& operator=(const Light&);

	// get set d3d light type
	D3DLIGHTTYPE GetLightType();
	void SetLightType(D3DLIGHTTYPE t);

	// overwrite position to take out of dating into account
	virtual void SetPosition(const D3DXVECTOR3& pos);

	// access light
	D3DLIGHT9* GetLight();

	// get/set diffuse colour
	const D3DXCOLOR& GetDiffuse();
	void SetDiffuse(const D3DXCOLOR& d);

	// get/set specular colour
	const D3DXCOLOR& GetSpecular() const;
	void SetSpecular(const D3DXCOLOR& s);

	// get/set ambient colour
	const D3DXCOLOR& GetAmbient() const;
	void SetAmbient(const D3DXCOLOR& a);

	// get/set falloff
	float GetFalloff();
	void SetFalloff(float falloff);

	// get/set attentuation 0
	float GetAttenuation0();
	void SetAttenuation0(float attenuation0);

	// get/set attentuation 1
	float GetAttenuation1();
	void SetAttenuation1(float attenuation1);

	// get/set attentuation 2
	float GetAttenuation2();
	void SetAttenuation2(float attenuation2);

	// get/set light range
	float GetRange();
	void SetRange(float range);

	// get/set umbra
	float GetPhi();
	void SetPhi(float phi);

	// get/set penumbra
	float GetTheta();
	void SetTheta(float theta);

	// save and load light
	void Read(BaseStreamer&);
	void Write(BaseStreamer&);

	// update d3d light with settings supplied
	void Update();

private:
	D3DLIGHT9		light;
	bool			outOfDate;

	D3DLIGHTTYPE	type;
	D3DXCOLOR		diffuse;
	D3DXCOLOR		specular;
	D3DXCOLOR		ambient;
	D3DXVECTOR3		direction;

	float			range;
	float			falloff;
	float			attenuation0;
	float			attenuation1;
	float			attenuation2;

	float			theta;
	float			phi;

	static std::string lightSignature;
	static int lightVersion;
};

