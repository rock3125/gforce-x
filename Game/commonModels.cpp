//////////////////////////////////////////////////////////////////////
/// GRAVITY FORCE X
///
/// (c)Copyright Peter de Vocht, 2006
///
/// I'm releasing this code under GPL v2.1 (see included file lgpl.txt)
///
//////////////////////////////////////////////////////////////////////

#include "standardFirst.h"

#include "game/commonModels.h"

#include "system/model/model.h"
#include "system/model/mesh.h"

#include "system/converters/vrml/VRMLParser.h"

CommonModels* CommonModels::singleton = NULL;

///////////////////////////////////////////////////////////

CommonModels* CommonModels::Get()
{
	if (singleton == NULL)
	{
		singleton = new CommonModels();
		singleton->Load();
	}
	return singleton;
}

void CommonModels::Destroy()
{
	safe_delete(singleton);
}

CommonModels::CommonModels()
	: missileModel(NULL)
	, shieldModel(NULL)
{
}

CommonModels::~CommonModels()
{
	safe_delete(missileModel);
	safe_delete(shieldModel);
}

void CommonModels::Load()
{
	LoadMissile();
	LoadShield();
}

Model* CommonModels::GetMissile()
{
	return missileModel;
}

Model* CommonModels::GetShield()
{
	return shieldModel;
}

void CommonModels::LoadMissile()
{
	VRMLParser importer;
	importer.Load(System::GetDataDirectory() + "objects\\missile.wrl");
	missileModel = dynamic_cast<Model*>(importer.ParseLoadedFile());
	if (importer.HasErrors())
	{
		throw new Exception(importer.GetError());
	}
	if (missileModel == NULL)
	{
		throw new Exception("could not load model 'objects\\missile.wrl'");
	}

	// rebuild
	missileModel->RestoreDeviceObjects();
}

void CommonModels::LoadShield()
{
	VRMLParser importer;
	importer.Load(System::GetDataDirectory() + "objects\\shield.wrl");
	shieldModel = dynamic_cast<Model*>(importer.ParseLoadedFile());
	if (importer.HasErrors())
	{
		throw new Exception(importer.GetError());
	}
	if (shieldModel == NULL)
	{
		throw new Exception("could not load model 'objects\\shield.wrl'");
	}

	// add texture coordinates to the sphere and add duplicate
	// vertices for overlapping coordinate points
	Mesh* mesh = shieldModel->GetMesh();
	D3DPVERTEX* vertex = mesh->GetVertices();
	int numVertices = mesh->GetNumVertices();
	for (int i=0; i < numVertices; i++)
	{
		vertex[i].texture = Vec3toUV(vertex[i].position);
		D3DXVec3Normalize(&vertex[i].normal, &vertex[i].position);
	}

	D3DPVERTEX v1,v2,v3;
	D3DPINDEX* indices = mesh->GetIndices();
	int numTriangles = mesh->GetNumTriangles();
	for (int i=0; i < numTriangles; i++)
	{
		v1 = vertex[indices[i].v1];
		v2 = vertex[indices[i].v2];
		v3 = vertex[indices[i].v3];
		int cntr = AlignUV(&v1,&v2,&v3);
		vertex[indices[i].v1] = v1;
		vertex[indices[i].v2] = v2;
		vertex[indices[i].v3] = v3;
	}

	// rebuild
	shieldModel->RestoreDeviceObjects();

	shieldModel->SetTexture(TextureCache::GetTexture("objects\\shield.jpg"));
	shieldModel->SetColour(D3DXCOLOR(1,1,1,0.75f));
}

int CommonModels::AlignUV(D3DPVERTEX* v1, D3DPVERTEX* v2, D3DPVERTEX* v3)
{
	int cntr = 0;
	float threshold = 0.5f;
	if (fabsf(v1->texture.x - v2->texture.x) > threshold)
	{
		cntr++;
		if (v1->texture.x < v2->texture.x)
		{
			v2->texture.x = 1.0f - v2->texture.x;
		}
		else
		{
			v1->texture.x = 1.0f - v1->texture.x;
		}
	}

	if (fabsf(v1->texture.x - v3->texture.x) > threshold)
	{
		cntr++;
		if (v1->texture.x < v3->texture.x)
		{
			v3->texture.x = 1.0f - v3->texture.x;
		}
		else
		{
			v1->texture.x = 1.0f - v1->texture.x;
		}
	}

	if (fabsf(v3->texture.x - v2->texture.x) > threshold)
	{
		cntr++;
		if (v3->texture.x < v2->texture.x)
		{
			v2->texture.x = 1.0f - v2->texture.x;
		}
		else
		{
			v3->texture.x = 1.0f - v3->texture.x;
		}
	}

	if (fabsf(v1->texture.y - v2->texture.y) > threshold)
	{
		cntr++;
		if (v1->texture.y < v2->texture.y)
		{
			v2->texture.y = 1.0f - v2->texture.y;
		}
		else
		{
			v1->texture.y = 1.0f - v1->texture.y;
		}
	}

	if (fabsf(v1->texture.y - v3->texture.y) > threshold)
	{
		cntr++;
		if (v1->texture.y < v3->texture.y)
		{
			v3->texture.y = 1.0f - v3->texture.y;
		}
		else
		{
			v1->texture.y = 1.0f - v1->texture.y;
		}
	}

	if (fabsf(v3->texture.y - v2->texture.y) > threshold)
	{
		cntr++;
		if (v3->texture.y < v2->texture.y)
		{
			v2->texture.y = 1.0f - v2->texture.y;
		}
		else
		{
			v3->texture.y = 1.0f - v3->texture.y;
		}
	}
	return cntr;
}

D3DXVECTOR2 CommonModels::Vec3toUV(const D3DXVECTOR3& pos)
{
	double x = pos.x;
	double y = pos.y;
	double z = pos.z;

	double longitude = atan2((double)-x,z);
	double latitude = atan(y/sqrt(x*x + z*z));

	longitude = 1.0 - longitude / (2*System::pi);
	latitude = fabs(0.5 - latitude / System::pi);

	double u = longitude - floor(longitude);
	double v = latitude;
	D3DXVECTOR2 uv;
	uv.x = (float)u;
	uv.y = (float)v;
	return uv;
}

