#include "standardFirst.h"

#include "system/converters/wgckImport/importCompoundObject.h"
#include "system/model/model.h"
#include "system/model/mesh.h"

///////////////////////////////////////////////////////////////////////////////////

ImportCompoundObject::ImportCompoundObject()
{
}

ImportCompoundObject::~ImportCompoundObject()
{
}

std::string ImportCompoundObject::GetString(FileStreamer& f)
{
	char buf[256];

	char ch;
	int index = 0;
	do
	{
		f.ReadData("",&ch,1);
		buf[index++] = ch;
	}
	while (index<255 && ch!=0);

	if (index==255)
	{
		throw new Exception("string too long");
		return "";
	}
	buf[index] = 0;
	return buf;
}

ImportCompoundObject::Material ImportCompoundObject::LoadMaterial(FileStreamer& f)
{
	std::string name = GetString(f);
	std::string materialFilename = GetString(f);

	float rgba[4];
	float power;
	float emissRGB[4];
	float specRGB[4];

	f.ReadData("",rgba,		4*sizeof(float));
	f.ReadData("",&power,	1*sizeof(float));
	f.ReadData("",emissRGB,	3*sizeof(float));
	f.ReadData("",specRGB,	3*sizeof(float));

	Material m;
	m.diffuse = D3DXCOLOR(rgba[0],rgba[1],rgba[2],rgba[3]);
	m.texture = materialFilename;
	return m;
}

std::vector<Model*> ImportCompoundObject::LoadMesh(FileStreamer& f)
{
	std::string name = GetString(f);
	std::vector<Model*> modelList;

	// vertices
	int numVertices;
	f.Read("",numVertices);

	float* vertices = new float[numVertices*3];
	f.ReadData("",vertices,numVertices*3*sizeof(float));

	// copy vertices
	D3DPVERTEX* v = new D3DPVERTEX[numVertices];
	for (int i=0; i<numVertices; i++)
	{
		v[i].position = D3DXVECTOR3(vertices[i*3+0],vertices[i*3+1],vertices[i*3+2]) * 100.0f;
		v[i].normal = D3DXVECTOR3(0,1,0);
		v[i].texture = D3DXVECTOR2(0,0);
	}

	// colourmap?
	bool isColourMap = false;
	if (numVertices>0)
	{
		if ( vertices[(numVertices-1)*3+0]==3.141592f )
		{
			throw new Exception("can't load wgck colourmaps");
			return modelList;
		}
	}

	safe_delete(vertices);

	// faces
	int numFaces;
	f.Read("",numFaces);
	int* faces = new int[numFaces*3];
	f.ReadData("",faces,numFaces*3*sizeof(int));

	// setup faces
	D3DPINDEX* indices = new D3DPINDEX[numFaces];
	for (int i=0; i<numFaces; i++)
	{
		indices[i].v1 = faces[i*3+0];
		indices[i].v2 = faces[i*3+1];
		indices[i].v3 = faces[i*3+2];
	}

	// num material faces
	int numMaterialFaces;
	f.Read("",numMaterialFaces);
	PreCond((numMaterialFaces == numFaces) || (numMaterialFaces == 1));
	int* materialFaces = new int[numMaterialFaces];
	f.ReadData("",materialFaces,numMaterialFaces*sizeof(int));

	// normals
	int numNormals;
	f.Read("",numNormals);
	if (numNormals>0)
	{
		float* normals = new float[numNormals*3];
		f.ReadData("",normals,numNormals*3*sizeof(float));

		// normal faces
		int numNormalFaces;
		f.Read("",numNormalFaces);
		if (numNormalFaces>0)
		{
			PreCond(numNormalFaces == numFaces);

			int* normalFaces = new int[numNormalFaces*3];
			f.ReadData("",normalFaces,numNormalFaces*3*sizeof(int));

			D3DXVECTOR3 n1,n2,n3;

			for (int i=0; i<numNormalFaces; i++)
			{
				int f1 = normalFaces[i*3+0];
				int f2 = normalFaces[i*3+1];
				int f3 = normalFaces[i*3+2];
				n1 = D3DXVECTOR3(normals[f1*3+0],normals[f1*3+1],normals[f1*3+2]);
				n2 = D3DXVECTOR3(normals[f2*3+0],normals[f2*3+1],normals[f2*3+2]);
				n3 = D3DXVECTOR3(normals[f3*3+0],normals[f3*3+1],normals[f3*3+2]);

				// set these in the equivalent vertex array
				int s1 = faces[i*3+0];
				int s2 = faces[i*3+1];
				int s3 = faces[i*3+2];

				v[s1].normal += n1;
				v[s2].normal += n2;
				v[s3].normal += n3;
			}

			for (int i=0; i<numVertices; i++)
			{
				D3DXVec3Normalize(&v[i].normal,&v[i].normal);
			}

			// set correct face windings
			for (int i=0; i<numFaces; i++)
			{
				int s1 = faces[i*3+0];
				int s2 = faces[i*3+1];
				int s3 = faces[i*3+2];

				n1 = v[s1].position - v[s2].position;
				n2 = v[s1].position - v[s3].position;
				D3DXVec3Normalize(&n1,&n1);
				D3DXVec3Normalize(&n2,&n2);

				// take cross product to get normal vector
				D3DXVec3Cross(&n3,&n1,&n2);
				D3DXVec3Normalize(&n3,&n3);

				// is the winding correct?
				// i.e. does the dotp of the two normals > 0
				n2 = v[s1].normal + v[s2].normal + v[s3].normal;
				D3DXVec3Normalize(&n2,&n2);

				float dotp = D3DXVec3Dot(&n3,&n2);
				if (dotp < 0)
				{
					int temp = indices[i].v1;
					indices[i].v1 = indices[i].v3;
					indices[i].v3 = temp;
				}
			}
		}
	}

    // texture coords
	int numTextureCoords;
    f.Read("",numTextureCoords);
	if (numTextureCoords>0)
	{
		if (numTextureCoords!=numVertices)
		{
			throw new Exception("mismatch between number of vertices and texture coordinates");
			return modelList;
		}
		float* textureCoords = new float[numTextureCoords*2];
		f.ReadData("",textureCoords,numTextureCoords*2*sizeof(float));

		D3DXVECTOR2 uv;
		for (int i=0; i<numVertices; i++)
		{
			uv = D3DXVECTOR2(textureCoords[i*2+0],textureCoords[i*2+1]);
			if (uv.y < 0 ) uv.y += 1.0f;
			if (uv.x < 0 ) uv.x += 1.0f;
			v[i].texture = uv;
		}

		safe_delete_array(textureCoords);
	}

    // materials
	int numMaterials;
	f.Read("",numMaterials);
	if (numMaterials>0)
	{
		std::vector<Mesh*> meshes;
		std::vector<int> materialIndex;
		std::vector<Material> materials;

		int mati = 0;
		for (int i=0; i<numMaterials; i++)
		{
			f.Read("",mati);
			materialIndex.push_back(mati);
			materials.push_back(GetMaterial(mati));
		}
/*
		// create meshes based on materialFaces
		if (numMaterials>1)
		{
			// exceptional case - need to re-arrange meshes according to
			// material usage
			int index = 0;
			for (int i=0; i<numMaterials; i++)
			{
				index = materialIndex[i];

				// get a count first
				// find all old indexes, and rename and add them
				int scount = 0;
				for (int j=0; j<numFaces; j++)
				{
					if (materialFaces[j] == index)
					{
						scount++;
					}
				}

				// now we have an accurate count for the number of faces to map
				D3DPINDEX* newIndices = new D3DPINDEX[scount];
				scount = 0;
				for (int j=0; j<numFaces; j++)
				{
					if (materialFaces[j] == index)
					{
						newIndices[scount++] = indices[j];
					}
				}

				// still a bit heavy handed since it includes ALL vertices
				// and not just the ones not used
				if (scount>0 && numVertices>0)
				{
					Mesh* nm = new Mesh();
					nm->SetVertices(numVertices,v);
					nm->SetIndices(scount,newIndices);
					nm->RestoreDeviceObjects();

					Model* m2 = new Model();
					m2->SetName(name + System::Int2Str(i));
					m2->SetMesh(nm);
					m2->SetColour(materials[index].diffuse);
					if (!materials[index].texture.empty())
					{
						Texture* t = TextureCache::GetTexture(path + materials[index].texture);
						m2->SetTexture(t);
					}

					modelList.push_back(m2);
				}
			}

			safe_delete_array(materialFaces);
			safe_delete_array(faces);

			// clear memory used
			safe_delete_array(v);
			safe_delete_array(indices);

			return modelList;
		}
		else
*/
		{
			Mesh* mesh = new Mesh();
			mesh->SetVertices(numVertices,v);
			mesh->SetIndices(numFaces,indices);

			Model* m = new Model();
			m->SetName(name);
			m->SetMesh(mesh);
			m->SetColour(materials[0].diffuse);

			if (!materials[0].texture.empty())
			{
				Texture* t = TextureCache::GetTexture(path + materials[0].texture);
				m->SetTexture(t);
			}

			safe_delete_array(materialFaces);
			safe_delete_array(faces);

			// clear memory used
			safe_delete_array(v);
			safe_delete_array(indices);
			safe_delete(mesh);

			modelList.push_back(m);

			return modelList;
		}
	}
	return modelList;
}

ImportCompoundObject::Material ImportCompoundObject::GetMaterial(int index)
{
	if (index<0 || index>=materials.size())
	{
		throw new Exception("material index out of bounds");
		return materials[0];
	}
	return materials[index];
}

std::vector<Model*> ImportCompoundObject::LoadBinary(FileStreamer& f)
{
	std::vector<Model*> models;

	char buf[256];
	std::string signature = "PDVX0002";

	f.ReadData("",buf,signature.size()+1);
	if (buf!=signature)
	{
		throw new Exception("signature does not match - not a wgck binary object");
		return models;
	}

    int numMaterials;
	int numMeshes;
	f.Read("",numMaterials);
	f.Read("",numMeshes);

	// load materials
	materials.clear();
    for (int i=0; i<numMaterials; i++)
    {
		materials.push_back(LoadMaterial(f));
    }

	// load meshes
    for (int i=0; i<numMeshes; i++)
    {
		std::vector<Model*> mods = LoadMesh(f);
		for (int j=0; j<mods.size(); j++)
		{
			models.push_back(mods[j]);
		}
    }
	return models;
}

WorldObject* ImportCompoundObject::Load(const std::string& fname)
{
	char buf[256];

	std::string signature = "PDVCO02";

	FileStreamer f(BaseStreamer::STREAM_READ);
	if (f.Open(fname))
	{
		// strip off path
		int index = fname.size() - 1;
		while (index>0 && fname[index]!='\\') index--;
		if (fname[index]=='\\')
		{
			std::string temp = fname.substr(0,index+1);
			std::string str = System::RemoveDataDirectory(temp);
			path = str;
		}

		f.ReadData("",buf,signature.size()+1);
		if (buf!=signature)
		{
			throw new Exception("signature does not match - not a wgck compound file");
			return NULL;
		}

		// texture path
		std::string texturePath = GetString(f);

		// number of objects inside the compound
		int numObjects;
		f.Read("",numObjects);

		// load the remaining objects from file
		std::vector<Model*> models;
		for (int i=0; i<numObjects; i++)
		{
			std::vector<Model*> m = LoadBinary(f);
			for (int j=0; j<m.size(); j++)
			{
				models.push_back(m[j]);
			}
		}
		f.Close();

		// create a container for all objects
		Model* m = new Model();
		m->SetName(path);

		for (int i=0; i<models.size(); i++)
		{
			m->AttachChild(models[i]);
		}

		// return the object
		return m;
	}
	else
	{
		throw new Exception(f.Error());
		return NULL;
	}
}
