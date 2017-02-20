#include "standardFirst.h"

#include "system/model/light.h"

std::string Light::lightSignature="light";
int Light::lightVersion=1;

/////////////////////////////////////////////////////////////////////////////

Light::Light()
{
	SetType(OT_LIGHT);
	SetWorldType(TYPE_LIGHT);

	// Default light properties
	type = D3DLIGHT_POINT;
	attenuation0 = 1;
	attenuation1 = 0;
	attenuation2 = 0;

	range = 175;
	falloff = 0;
	theta = 0.5f;
	phi = 1.3333f;

	diffuse = D3DXCOLOR(1,1,1,1);
	ambient = D3DXCOLOR(1,1,1,1);
	specular = D3DXCOLOR(1,1,1,1);
	direction = D3DXVECTOR3(0,-1,0);

	outOfDate = false;
	Update();
}

Light::~Light()
{
}

Light::Light(const Light& l)
{
	operator=(l);
}

const Light& Light::operator=(const Light& l)
{
	WorldObject::operator =(l);

	type = l.type;

	diffuse = l.diffuse;
	specular = l.specular;
	ambient = l.ambient;
	direction = l.direction;

	range = l.range;
	falloff = l.falloff;
	attenuation0 = l.attenuation0;
	attenuation1 = l.attenuation1;
	attenuation2 = l.attenuation2;

	theta = l.theta;
	phi = l.phi;

	Update();

	return *this;
}

D3DLIGHT9* Light::GetLight()
{
	if (outOfDate)
	{
		Update();
	}
	return &light;
}

void Light::SetPosition(const D3DXVECTOR3& pos)
{
	outOfDate = true;
	WorldObject::SetPosition(pos);
}

void Light::Update()
{
	outOfDate = false;

	ZeroMemory(&light, sizeof(light));
	switch (type)
	{
		case D3DLIGHT_POINT:
		{
			light.Type = type;
			light.Diffuse = diffuse;
			light.Ambient  = ambient;
			light.Specular = specular;
			light.Attenuation0 = attenuation0;
			light.Range = range;
			light.Position = GetPosition();
			break;
		}
		default:
		{
			throw new Exception("not implemented");
		}
	}
}

D3DLIGHTTYPE Light::GetLightType()
{
	return type;
}

void Light::SetLightType(D3DLIGHTTYPE _type)
{
	outOfDate = true;
	type = _type;
}

const D3DXCOLOR& Light::GetSpecular() const
{
	return specular;
}

void Light::SetSpecular(const D3DXCOLOR& _specular)
{
	outOfDate = true;
	specular = _specular;
}

const D3DXCOLOR& Light::GetAmbient() const
{
	return ambient;
}

void Light::SetAmbient(const D3DXCOLOR& _ambient)
{
	outOfDate = true;
	ambient = _ambient;
}

const D3DXCOLOR& Light::GetDiffuse()
{
	return diffuse;
}

void Light::SetDiffuse(const D3DXCOLOR& _diffuse)
{
	outOfDate = true;
	diffuse = _diffuse;
}

float Light::GetAttenuation0()
{
	return attenuation0;
}

void Light::SetAttenuation0(float _attenuation0)
{
	outOfDate = true;
	attenuation0 = _attenuation0;
}

float Light::GetAttenuation1()
{
	return attenuation1;
}

void Light::SetAttenuation1(float _attenuation1)
{
	outOfDate = true;
	attenuation1 = _attenuation1;
}

float Light::GetAttenuation2()
{
	return attenuation2;
}

void Light::SetAttenuation2(float _attenuation2)
{
	outOfDate = true;
	attenuation2 = _attenuation2;
}

float Light::GetRange()
{
	return range;
}

void Light::SetRange(float _range)
{
	outOfDate = true;
	range = _range;
}

float Light::GetFalloff()
{
	return falloff;
}

void Light::SetFalloff(float _falloff)
{
	outOfDate = true;
	falloff = _falloff;
}

float Light::GetPhi()
{
	return phi;
}

void Light::SetPhi(float _phi)
{
	outOfDate = true;
	phi = _phi;
}

float Light::GetTheta()
{
	return theta;
}

void Light::SetTheta(float _theta)
{
	outOfDate = true;
	theta = _theta;
}

void Light::Write(BaseStreamer& _f)
{
	BaseStreamer& f = _f.NewChild(lightSignature,lightVersion);

	WorldObject::Write(f);

	int t=type;
	f.Write("type",t);

	f.Write("diffuse", diffuse);
	f.Write("specular", specular);
	f.Write("ambient", ambient);
	f.Write("direction", direction);

	f.Write("range", range);
	f.Write("falloff", falloff);
	f.Write("attenuation0", attenuation0);
	f.Write("attenuation1", attenuation1);
	f.Write("attenuation2", attenuation2);

	f.Write("theta", theta);
	f.Write("phi", phi);
}

void Light::Read(BaseStreamer& _f)
{
	BaseStreamer& f = _f.GetChild(lightSignature,lightVersion);

	WorldObject::Read(f);

	int t;
	f.Read("type",t);
	type=D3DLIGHTTYPE(t);

	f.Read("diffuse", diffuse);
	f.Read("specular", specular);
	f.Read("ambient", ambient);
	f.Read("direction", direction);

	f.Read("range", range);
	f.Read("falloff", falloff);
	f.Read("attenuation0", attenuation0);
	f.Read("attenuation1", attenuation1);
	f.Read("attenuation2", attenuation2);

	f.Read("theta", theta);
	f.Read("phi", phi);

	Update();
}

