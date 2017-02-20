#include "standardFirst.h"

#include "system/converters/obj/objImport.h"
#include "system/model/model.h"
#include "system/model/mesh.h"

///////////////////////////////////////////////////////////////////////////////////

ObjImport::ObjImport()
{
	EnableInt();
	EnableFloat();
	EnableIdent();

	Add("#",false);
	Add("v",true);
	Add("vt",true);
	Add("vn",true);
	Add("mtllib",true);
	Add("g",true);
	Add("usemtl",true);
	Add("f",true);
	Add("/",false);
	Add(".",false);
	Add("newmtl",true);
	Add("Ns",true);
	Add("Ka",true);
	Add("Kd",true);
	Add("Ks",true);
	Add("map_Kd",true);
}

ObjImport::~ObjImport()
{
}

WorldObject* ObjImport::ParseLoadedFile()
{
	if (!error)
	{
		ParseObj();
		if (!error)
		{
			// collect models and turn this into a system of models
			if (models.size() > 1)
			{
				Model* m = new Model();
				m->SetName("container");
				for (int j=0; j < models.size(); j++)
				{
					Mesh* mesh = new Mesh();
					Model* model = new Model();
					std::string name = models[j]->name;
					model->SetName(name);
					ObjMaterial* mat = GetMaterial(name);
					if (mat != NULL)
					{
						if (!mat->mapName.empty())
						{
							model->SetTexture(TextureCache::GetTexture(mat->mapName));
						}
						else
						{
							model->SetColour(mat->diffuse);
						}
					}

					// create the set of vertices
					int size = models[j]->vertices.size();
					D3DPVERTEX* verts = new D3DPVERTEX[size];
					for (int i=0; i < size; i++)
					{
						verts[i] = models[j]->vertices[i];
					}
					mesh->SetVertices(size, verts);
					safe_delete_array(verts);

					// create indices
					int isize = size / 3;
					D3DPINDEX* ind = new D3DPINDEX[isize];
					for (int i=0; i < isize; i++)
					{
						ind[i].v1 = i * 3 + 0;
						ind[i].v2 = i * 3 + 1;
						ind[i].v3 = i * 3 + 2;
					}
					mesh->SetIndices(isize, ind);
					safe_delete_array(ind);

					model->SetMesh(mesh);
					safe_delete(mesh);

					// add it to the parent model
					m->AttachChild(model);
				}
				return m;
			}
			else if (!models.empty() && vertices.size() > 0)
			{
				Mesh* mesh = new Mesh();
				Model* model = new Model();
				std::string name = models[0]->name;
				model->SetName(name);
				ObjMaterial* mat = GetMaterial(name);
				if (mat != NULL)
				{
					if (!mat->mapName.empty())
					{
						model->SetTexture(TextureCache::GetTexture(mat->mapName));
					}
					else
					{
						model->SetColour(mat->diffuse);
					}
				}

				// create the set of vertices
				int size = models[0]->vertices.size();
				D3DPVERTEX* verts = new D3DPVERTEX[size];
				for (int i=0; i < size; i++)
				{
					verts[i] = models[0]->vertices[i];
				}
				mesh->SetVertices(size, verts);
				safe_delete_array(verts);

				// create indices
				int isize = size / 3;
				D3DPINDEX* ind = new D3DPINDEX[isize];
				for (int i=0; i < isize; i++)
				{
					ind[i].v1 = i * 3 + 0;
					ind[i].v2 = i * 3 + 1;
					ind[i].v3 = i * 3 + 2;
				}
				mesh->SetIndices(isize, ind);
				safe_delete_array(ind);

				model->SetMesh(mesh);
				return model;
			}
		}
	}
	return NULL;
}

bool ObjImport::IsModel(Token token)
{
	if (token == Get("v")	||
		token == Get("vt")	||
		token == Get("vn"))
	{
		return true;
	}
	return false;
}

//obj	->			'#' eol		|
//					model*
//
void ObjImport::ParseObj()
{
	do
	{
		token = GetNextToken();
		if (token == Get("#"))
		{
			ScanToEol();
		}
		else if (IsModel(token))
		{
			UngetToken();
			ParseModel();
		}
		else if (token != TEOF)
		{
			SetError("unexpected token in obj");
			break;
		}
	}
	while (token != TEOF && !error);
}

bool ObjImport::IsFace(Token token)
{
	if (token == Get("mtllib")	||
		token == Get("g")		||
		token == Get("usemtl")	||
		token == Get("f"))
	{
		return true;
	}
	return false;
}

//model	->		['v' f f f]*
//				['vt' f f]*
//				['vn' f f f]*
//
//				faces
//
void ObjImport::ParseModel()
{
	do
	{
		token = GetNextToken();
		UngetToken();
		if (token == Get("#"))
		{
			ScanToEol();
		}
		else if (token == Get("v"))
		{
			D3DXVECTOR3 v;
			do
			{
				GetCompulsaryToken(Get("v"));

				float v1,v2,v3;
				GetCompulsaryTokenOption(TFLOAT,TINT);
				if (error) break;
				if (token == TINT) v1 = (float)yyInt;
				if (token == TFLOAT) v1 = yyFloat;

				GetCompulsaryTokenOption(TFLOAT,TINT);
				if (error) break;
				if (token == TINT) v2 = (float)yyInt;
				if (token == TFLOAT) v2 = yyFloat;

				GetCompulsaryTokenOption(TFLOAT,TINT);
				if (error) break;
				if (token == TINT) v3 = (float)yyInt;
				if (token == TFLOAT) v3 = yyFloat;

				v.x = v1;
				v.y = v2;
				v.z = v3;

				vertices.push_back(v);

				token = GetNextToken();
				UngetToken();
				if (token != Get("v")) break;
			}
			while (true);
		}
		else if (token == Get("vt"))
		{
			D3DXVECTOR2 v;
			do
			{
				GetCompulsaryToken(Get("vt"));

				float v1,v2;
				GetCompulsaryTokenOption(TFLOAT,TINT);
				if (error) break;
				if (token == TINT) v1 = (float)yyInt;
				if (token == TFLOAT) v1 = yyFloat;

				GetCompulsaryTokenOption(TFLOAT,TINT);
				if (error) break;
				if (token == TINT) v2 = (float)yyInt;
				if (token == TFLOAT) v2 = yyFloat;

				v.x = v1;
				v.y = 1 - v2;

				textures.push_back(v);

				token = GetNextToken();
				UngetToken();
				if (token != Get("vt")) break;
			}
			while (true);
		}
		else if (token == Get("vn"))
		{
			D3DXVECTOR3 n;
			do
			{
				GetCompulsaryToken(Get("vn"));

				float v1,v2,v3;
				GetCompulsaryTokenOption(TFLOAT,TINT);
				if (error) break;
				if (token == TINT) v1 = (float)yyInt;
				if (token == TFLOAT) v1 = yyFloat;

				GetCompulsaryTokenOption(TFLOAT,TINT);
				if (error) break;
				if (token == TINT) v2 = (float)yyInt;
				if (token == TFLOAT) v2 = yyFloat;

				GetCompulsaryTokenOption(TFLOAT,TINT);
				if (error) break;
				if (token == TINT) v3 = (float)yyInt;
				if (token == TFLOAT) v3 = yyFloat;

				n.x = v1;
				n.y = v2;
				n.z = v3;
				normals.push_back(n);

				token = GetNextToken();
				UngetToken();
				if (token != Get("vn")) break;
			}
			while (true);
		}
		else if (IsFace(token))
		{
			ParseFaces();
		}
		else
		{
			break;
		}
	}
	while (token != TEOF && !error);
}

// get model by name
ObjImport::ObjModel* ObjImport::GetModelByName(const std::string& name)
{
	for (int i=0; i < models.size(); i++)
	{
		if (models[i]->name == name)
			return models[i];
	}
	return NULL;
}

void ObjImport::GetIndices(int& i1, int& i2, int& i3)
{
	GetCompulsaryToken(TINT);
	if (error) return;
	i1 = yyInt - 1;
	GetCompulsaryToken("/");
	if (error) return;
	GetCompulsaryToken(TINT);
	if (error) return;
	i2 = yyInt - 1;
	GetCompulsaryToken("/");
	if (error) return;
	GetCompulsaryToken(TINT);
	if (error) return;
	i3 = yyInt - 1;
}

//faces	->		['mtllib' ident]
//				['g' ident]*
//
//				[
//					['usemtl' ident]
//					['f'  i '/' i '/' i     i '/' i '/' i    i '/' i '/' i    [i '/' i '/' i]]*
//				]*
//				faces						|
//				e
void ObjImport::ParseFaces()
{
	ObjModel* model = NULL;
	D3DPINDEX indices;
	do
	{
		token = GetNextToken();
		if (token == Get("#"))
		{
			ScanToEol();
		}
		else if (token == Get("mtllib"))
		{
			// for now - ignore material libraries
			GetCompulsaryToken(TIDENT);
			if (error) break;
			std::string materialFname = yyString;
			token = GetNextToken();
			if (token == Get("."))
			{
				GetCompulsaryToken(TIDENT);
				if (error) break;
				materialFname = materialFname + "." + yyString;
			}
			else
			{
				UngetToken();
			}

			// try and load the material library
			ObjImport parser;
			parser.Load(System::GetDataDirectory() + filePrefix + materialFname);
			if (parser.HasErrors())
			{
				SetError(parser.GetError());
				break;
			}

			parser.ParseMaterialLib();
			if (parser.HasErrors())
			{
				SetError(parser.GetError());
				break;
			}

			materials = parser.materials;
		}
		else if (token == Get("g"))
		{
			// basically ignore groupings
			GetCompulsaryToken(TIDENT);
			if (error) break;
			do
			{
				token = GetNextToken();
				if (token != TIDENT)
				{
					UngetToken();
					break;
				}
			}
			while (true);
		}
		else if (token == Get("usemtl"))
		{
			// material use determines object grouping
			GetCompulsaryToken(TIDENT);
			if (error) break;
			std::string name = yyString;

			// do we have a previous model that needs saving?
			if (model != NULL)
			{
				if (GetModelByName(model->name) == NULL)
				{
					if (model->vertices.size() > 0)
					{
						models.push_back(model);
					}
				}
			}

			// does it already exist?
			if (model == NULL)
			{
				model = GetModelByName(name);
			}
			if (model == NULL)
			{
				model = new ObjModel();
				model->name = name;
			}
		}
		else if (token == Get("f"))
		{
			// see if there already is a model for 'no material'
			if (model == NULL)
			{
				model = GetModelByName("no material");

				if (model == NULL)
				{
					model = new ObjModel();
					model->name = "no material";
				}
			}

			D3DPVERTEX temp;
			int v1,t1,n1;
			GetIndices(v1,t1,n1);
			if (error) break;

			int v2,t2,n2;
			GetIndices(v2,t2,n2);
			if (error) break;

			int v3,t3,n3;
			GetIndices(v3,t3,n3);
			if (error) break;

			// see if there is a fourth quantity
			token = GetNextToken();
			UngetToken();
			if (token == TINT)
			{
				// two triangles - get fourth index
				int v4,t4,n4;
				GetIndices(v4,t4,n4);
				if (error) break;

				temp.position = vertices[v1];
				temp.normal = normals[n1];
				temp.texture = textures[t1];
				model->vertices.push_back(temp);

				temp.position = vertices[v2];
				temp.normal = normals[n2];
				temp.texture = textures[t2];
				model->vertices.push_back(temp);

				temp.position = vertices[v3];
				temp.normal = normals[n3];
				temp.texture = textures[t3];
				model->vertices.push_back(temp);

				// second triangle
				temp.position = vertices[v3];
				temp.normal = normals[n3];
				temp.texture = textures[t3];
				model->vertices.push_back(temp);

				temp.position = vertices[v4];
				temp.normal = normals[n4];
				temp.texture = textures[t4];
				model->vertices.push_back(temp);

				temp.position = vertices[v1];
				temp.normal = normals[n1];
				temp.texture = textures[t1];
				model->vertices.push_back(temp);
			}
			else
			{
				// only one triangle
				temp.position = vertices[v1];
				temp.normal = normals[n1];
				temp.texture = textures[t1];
				model->vertices.push_back(temp);

				temp.position = vertices[v2];
				temp.normal = normals[n2];
				temp.texture = textures[t2];
				model->vertices.push_back(temp);

				temp.position = vertices[v3];
				temp.normal = normals[n3];
				temp.texture = textures[t3];
				model->vertices.push_back(temp);
			}
		}
		else
		{
			UngetToken();
			break;
		}
	}
	while (token != TEOF && !error);

	// see if the model needs adding
	if (model != NULL)
	{
		if (GetModelByName(model->name) == NULL)
		{
			if (model->vertices.size() > 0)
			{
				models.push_back(model);
			}
		}
	}
}

ObjImport::ObjMaterial* ObjImport::GetMaterial(const std::string& name)
{
	for (int i=0; i < materials.size(); i++)
	{
		if (materials[i].name == name)
			return &materials[i];
	}
	return NULL;
}

//
// material ->		newmtl ident
//					Ns f
// 					Ka f f f
// 					Kd f f f
// 					Ks f f f
// 					map_Kd ident+ ['.' ident]
//
void ObjImport::ParseMaterialLib()
{
	std::string name;
	D3DXCOLOR diffuse;

	do
	{
		float c1,c2,c3;

		GetCompulsaryToken(Get("newmtl"));
		if (error) break;
		GetCompulsaryToken(TIDENT);
		if (error) break;
		name = yyString;

		GetCompulsaryToken(Get("Ns"));
		if (error) break;
		GetCompulsaryTokenOption(TFLOAT,TINT);
		if (error) break;
		if (token == TINT) c1 = (float)yyInt;
		if (token == TFLOAT) c1 = yyFloat;

		GetCompulsaryToken(Get("Ka"));
		if (error) break;
		GetCompulsaryTokenOption(TFLOAT,TINT);
		if (error) break;
		if (token == TINT) c1 = (float)yyInt;
		if (token == TFLOAT) c1 = yyFloat;
		GetCompulsaryTokenOption(TFLOAT,TINT);
		if (error) break;
		if (token == TINT) c2 = (float)yyInt;
		if (token == TFLOAT) c2 = yyFloat;
		GetCompulsaryTokenOption(TFLOAT,TINT);
		if (error) break;
		if (token == TINT) c3 = (float)yyInt;
		if (token == TFLOAT) c3 = yyFloat;

		GetCompulsaryToken(Get("Kd"));
		if (error) break;
		GetCompulsaryTokenOption(TFLOAT,TINT);
		if (error) break;
		if (token == TINT) c1 = (float)yyInt;
		if (token == TFLOAT) c1 = yyFloat;
		GetCompulsaryTokenOption(TFLOAT,TINT);
		if (error) break;
		if (token == TINT) c2 = (float)yyInt;
		if (token == TFLOAT) c2 = yyFloat;
		GetCompulsaryTokenOption(TFLOAT,TINT);
		if (error) break;
		if (token == TINT) c3 = (float)yyInt;
		if (token == TFLOAT) c3 = yyFloat;
		diffuse.r = c1;
		diffuse.g = c2;
		diffuse.b = c3;

		GetCompulsaryToken(Get("Ks"));
		if (error) break;
		GetCompulsaryTokenOption(TFLOAT,TINT);
		if (error) break;
		if (token == TINT) c1 = (float)yyInt;
		if (token == TFLOAT) c1 = yyFloat;
		GetCompulsaryTokenOption(TFLOAT,TINT);
		if (error) break;
		if (token == TINT) c2 = (float)yyInt;
		if (token == TFLOAT) c2 = yyFloat;
		GetCompulsaryTokenOption(TFLOAT,TINT);
		if (error) break;
		if (token == TINT) c3 = (float)yyInt;
		if (token == TFLOAT) c3 = yyFloat;

		std::string mapName;
		token = GetNextToken();
		if (token == Get("map_Kd"))
		{
			bool extension = false;
			do
			{
				token = GetNextToken();
				if (token == Get("."))
				{
					extension = true;
					mapName = mapName + ".";
				}
				else if (token == TIDENT)
				{
					if (!mapName.empty() && !extension)
					{
						mapName = mapName + " ";
					}
					mapName = mapName + yyString;
				}
				else
				{
					UngetToken();
					break;
				}
			}
			while (true);
		}
		else
		{
			UngetToken();
		}

		// add material
		ObjMaterial mat;
		mat.name = name;
		mat.diffuse = diffuse;
		if (!mapName.empty())
		{
			mat.mapName = filePrefix + mapName;
		}
		materials.push_back(mat);

		token = GetNextToken();
		if (token != Get("newmtl"))
		{
			UngetToken();
			break;
		}
		else
		{
			UngetToken();
		}
	}
	while (true);
}

