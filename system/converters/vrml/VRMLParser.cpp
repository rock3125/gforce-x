#include "standardFirst.h"

#include "system/converters/vrml/VRMLParser.h"
#include "system/model/mesh.h"
#include "system/model/model.h"

///////////////////////////////////////////////////////////////////////////////////

VRMLParser::VRMLParser()
{
	// these primitives are used
	EnableInt();
	EnableFloat();
	EnableIdent();
	EnableDoubleQuoteString();

	Add("coordIndex",true);
	Add("TRUE",true);
	Add("FALSE",true);
	Add("coordText",true);
	Add("texCoord",true);
	Add("texCoordIndex",true);
	Add("coord",true);
	Add("texture",true);
	Add("ImageTexture",true);
	Add("url",true);
	Add("DEF",true);
	Add("USE",true);
	Add("IndexedFaceSet",true);
	Add("TextureCoordinate",true);
	Add("TimeSensor",true);
	Add("loop",true);
	Add("cycleInterval",true);
	Add("Coordinate",true);
	Add("Transform",true);
	Add("translation",true);
	Add("rotation",true);
	Add("children",true);
	Add("Shape",true);
	Add("appearance",true);
	Add("material",true);
	Add("diffuseColor",true);
	Add("ambientIntensity",true);
	Add("specularColor",true);
	Add("shininess",true);
	Add("transparency",true);
	Add("geometry",true);
	Add("ccw",true);
	Add("solid",true);
	Add("point",true);
	Add("{",false);
	Add("}",false);
	Add("[",false);
	Add("]",false);
	Add("#",false);
	Add(",",false);
	Add("normal",true);
	Add("Normal",true);
	Add("vector",true);
	Add("normalPerVertex",true);
	Add("normalIndex",true);
}

VRMLParser::~VRMLParser()
{
}

const VRMLParser& VRMLParser::operator=(const VRMLParser& tp)
{
	return *this;
}

VRMLParser::VRMLParser(const VRMLParser& tp)
{
	operator=(tp);
}

WorldObject* VRMLParser::LoadBinary(std::string filename)
{
	throw new Exception("load binary not implemented");
}

// vrml ->	'#' comment til eol		|
//			defList
WorldObject* VRMLParser::ParseLoadedFile()
{
	error = false;

	Token token2;
	Model* container = new Model();
	container->SetName("container");
	do
	{
		token2 = GetNextToken();

		// new model?
		if (token2==Get("DEF"))
		{
			UngetToken();
			Model* temp = Def();
			if (error) return NULL;
			container->AttachChild(temp);
		}
		else if (token2==Get("#"))
		{
			// comment
			ScanToEol();
		}
		else
		{
			UngetToken();
			break;
		}
	}
	while (token2!=TEOF && !error);

	std::vector<PRS*> children = container->GetChildren();
	if (children.size() == 1)
	{
		Model* model = dynamic_cast<Model*>(children[0]);
		// optimise all models
		model->OptimiseMeshes();
		model->ClearHierarchy();

		// cleanup
		container->ClearHierarchy();
		safe_delete(container);

		return model;
	}

	// optimise all models
	container->OptimiseMeshes();
	container->UpdateBoundingBox();

	return container;
}

// defList ->	'DEF' ident 'Transform' '{'
//				'translation' f f f
//				['rotation' f f f f]
//				'children' [ shape* ] '}' defList |
//				e
Model* VRMLParser::Def()
{
	Model* model = NULL;

	if (!GetCompulsaryToken(Get("DEF"))) return NULL;
	if (!GetCompulsaryToken(TIDENT)) return NULL;
	std::string name = yyString;

	if (!GetCompulsaryToken(Get("Transform"))) return NULL;
	if (!GetCompulsaryToken(Get("{"))) return NULL;

	float t1,t2,t3;
	if (!GetCompulsaryToken(Get("translation"))) return NULL;
	if (!GetCompulsaryTokenOption(TFLOAT,TINT)) return NULL;
	if (token==TFLOAT) t1 = yyFloat; else t1 = (float)yyInt;
	if (!GetCompulsaryTokenOption(TFLOAT,TINT)) return NULL;
	if (token==TFLOAT) t2 = yyFloat; else t2 = (float)yyInt;
	if (!GetCompulsaryTokenOption(TFLOAT,TINT)) return NULL;
	if (token==TFLOAT) t3 = yyFloat; else t3 = (float)yyInt;

	bool hasRot = false;
	float r1,r2,r3,r4;
	if (GetOptionalToken(Get("rotation")))
	{
		hasRot = true;
		if (!GetCompulsaryTokenOption(TFLOAT,TINT)) return NULL;
		if (token==TFLOAT) r1 = yyFloat; else r1 = (float)yyInt;
		if (!GetCompulsaryTokenOption(TFLOAT,TINT)) return NULL;
		if (token==TFLOAT) r2 = yyFloat; else r2 = (float)yyInt;
		if (!GetCompulsaryTokenOption(TFLOAT,TINT)) return NULL;
		if (token==TFLOAT) r3 = yyFloat; else r3 = (float)yyInt;
		if (!GetCompulsaryTokenOption(TFLOAT,TINT)) return NULL;
		if (token==TFLOAT) r4 = yyFloat; else r4 = (float)yyInt;
	}

	if (!GetCompulsaryToken(Get("children"))) return NULL;
	if (!GetCompulsaryToken(Get("["))) return NULL;

	Token token1;
	do
	{
		token1 = GetNextToken();
		UngetToken();

		if (token1==Get("Shape") || token1==Get("DEF"))
		{
			if (model==NULL)
			{
				model = Shape(NULL);
				if (error) { safe_delete(model); return NULL; }
				model->SetName(name);
				model->SetPosition(D3DXVECTOR3(t1,t2,t3));
				if (hasRot)
				{
					float st = sinf(r4*0.5f);
					float ct = cosf(r4*0.5f);
					model->SetRotationQuat(D3DXQUATERNION(r1*st,r2*st,r3*st,ct));
				}
			}
			else
			{
				Model* m = Shape(model);
				if (error) { return NULL; }

				// it can be null (vrml weird exceptions)
				if (m != NULL)
				{
					std::string uniqueName = name + "_sub" + System::Int2Str(model->GetChildren().size()+1);
					m->SetName(uniqueName);
					model->AttachChild(m);
				}
			}
		}
		else
		{
			break;
		}
	}
	while ((token1==Get("Shape") || token1==Get("DEF")) && !error);

	if (!GetCompulsaryToken(Get("]"))) return NULL;
	if (!GetCompulsaryToken(Get("}"))) return NULL;

	return model;
}

// shape ->	['DEF' ident 'TimeSensor' '{' 'loop' 'TRUE' 'cycleInterval' f '}' ',']
//			'Shape' {
//			'appearance' ident {
//			'material' ident {
//			'diffuseColor' f f f
//          'ambientIntensity' f
//          'specularColor' f f f
//          'shininess' f
//          'transparency' f
//			} [ 'texture' 'ImageTexture' '{' 'url' "string" '}' ]
//			} geom }
Model* VRMLParser::Shape(Model* model)
{
	std::string textureFilename;

	token = GetNextToken();
	if (token == Get("DEF"))
	{
		if (!GetCompulsaryToken(TIDENT)) return NULL;
		if (!GetCompulsaryToken(Get("TimeSensor"))) return NULL;
		if (!GetCompulsaryToken(Get("{"))) return NULL;
		if (!GetCompulsaryToken(Get("loop"))) return NULL;
		if (!GetCompulsaryToken(Get("TRUE"))) return NULL;
		if (!GetCompulsaryToken(Get("cycleInterval"))) return NULL;
		if (!GetCompulsaryTokenOption(TFLOAT,TINT)) return NULL;
		if (!GetCompulsaryToken(Get("}"))) return NULL;
		if (!GetCompulsaryToken(Get(","))) return NULL;
	}
	else
	{
		UngetToken();
	}

	if (!GetCompulsaryToken(Get("Shape"))) return NULL;
	if (!GetCompulsaryToken(Get("{"))) return NULL;
	if (!GetCompulsaryToken(Get("appearance"))) return NULL;
	if (!GetCompulsaryToken(TIDENT)) return NULL;
	if (!GetCompulsaryToken(Get("{"))) return NULL;
	if (!GetCompulsaryToken(Get("material"))) return NULL;
	if (!GetCompulsaryToken(TIDENT)) return NULL;
	if (!GetCompulsaryToken(Get("{"))) return NULL;
	if (!GetCompulsaryToken(Get("diffuseColor"))) return NULL;

	float f1,f2,f3;
	if (!GetCompulsaryTokenOption(TFLOAT,TINT)) return NULL;
	if (token==TINT) f1 = (float)yyInt; else f1 = yyFloat;
	if (!GetCompulsaryTokenOption(TFLOAT,TINT)) return NULL;
	if (token==TINT) f2 = (float)yyInt; else f2 = yyFloat;
	if (!GetCompulsaryTokenOption(TFLOAT,TINT)) return NULL;
	if (token==TINT) f3 = (float)yyInt; else f3 = yyFloat;

	if (GetOptionalToken(Get("ambientIntensity")))
	{
		if (!GetCompulsaryTokenOption(TFLOAT,TINT)) return NULL;
	}

	if (GetOptionalToken(Get("specularColor")))
	{
		if (!GetCompulsaryTokenOption(TFLOAT,TINT)) return NULL;
		if (!GetCompulsaryTokenOption(TFLOAT,TINT)) return NULL;
		if (!GetCompulsaryTokenOption(TFLOAT,TINT)) return NULL;
	}

	if (GetOptionalToken(Get("shininess")))
	{
		if (!GetCompulsaryTokenOption(TFLOAT,TINT)) return NULL;
	}

	if (GetOptionalToken(Get("transparency")))
	{
		if (!GetCompulsaryTokenOption(TFLOAT,TINT)) return NULL;
	}

	if (!GetCompulsaryToken(Get("}"))) return NULL;

	// [ 'texture' 'ImageTexture' '{' 'url' "string" '}' ]
	token = GetNextToken();
	if (token == Get("texture"))
	{
		if (!GetCompulsaryToken(Get("ImageTexture"))) return NULL;
		if (!GetCompulsaryToken(Get("{"))) return NULL;
		if (!GetCompulsaryToken(Get("url"))) return NULL;
		if (!GetCompulsaryToken(TSTRING)) return NULL;
		textureFilename = yyString;
		if (!GetCompulsaryToken(Get("}"))) return NULL;
	}
	else
	{
		UngetToken();
	}

	if (!GetCompulsaryToken(Get("}"))) return NULL;

	Mesh* mesh = Geom(model);
	if (error) { safe_delete(mesh); return NULL; }

	if (!GetCompulsaryToken(Get("}"))) { safe_delete(mesh); return NULL; }

	if (mesh->GetNumTriangles()==0 || mesh->GetNumVertices()==0)
		return NULL;

	Model* mod = new Model();
	mod->SetMesh(mesh);
	safe_delete(mesh);
	mod->SetColour(D3DXCOLOR(f1,f2,f3,1));

	if (!textureFilename.empty())
	{
		mod->SetTexture(TextureCache::GetTexture(textureFilename));
	}

	return mod;
}

// geom ->	'geometry' 'DEF' ident 'IndexedFaceSet' {
//			'ccw' 'TRUE'
//			'solid' 'TRUE'
//			optGeom }
Mesh* VRMLParser::Geom(Model* model)
{
	if (!GetCompulsaryToken(Get("geometry"))) return NULL;
	if (!GetCompulsaryToken(Get("DEF"))) return NULL;
	if (!GetCompulsaryToken(TIDENT)) return NULL;
	if (!GetCompulsaryToken(Get("IndexedFaceSet"))) return NULL;
	if (!GetCompulsaryToken(Get("{"))) { return NULL; }
	if (!GetCompulsaryToken(Get("ccw"))) { return NULL; }
	if (!GetCompulsaryToken(Get("TRUE"))) { return NULL; }
	if (!GetCompulsaryToken(Get("solid"))) { return NULL; }
	if (!GetCompulsaryTokenOption(Get("TRUE"),Get("FALSE"))) { return NULL; }

	bool sharingVerts;
	std::string name;
	Mesh* mesh = OptGeom(name,model,sharingVerts);
	if (error || mesh==NULL) return NULL;
	mesh->SetName(name);

	// if we're sharing verts then we need to cut down
	// on the ones not used and re-number indices
	if (sharingVerts)
	{
		int numVerts = mesh->GetNumVertices();
		int numTrian = mesh->GetNumTriangles();
		if (numVerts>0 && numTrian>0)
		{
			std::vector<D3DPVERTEX> newVerts;
			D3DPINDEX* triangles = mesh->GetIndices();
			D3DPVERTEX* oldVerts = mesh->GetVertices();

			// for each vert
			for (int i=0; i<numVerts && !error; i++)
			{
				bool used = false;
				for (int j=0; j<numTrian && !used; j++)
				{
					used = (triangles[j].v1==i) || (triangles[j].v2==i) || (triangles[j].v3==i);
				}
				if (used)
				{
					// insert new vertex
					int newIndex = newVerts.size();
					newVerts.push_back(oldVerts[i]);

					// renumber any old occurence of the vertex
					for (int j=0; j<numTrian; j++)
					{
						if (triangles[j].v1==i) triangles[j].v1 = newIndex;
						if (triangles[j].v2==i) triangles[j].v2 = newIndex;
						if (triangles[j].v3==i) triangles[j].v3 = newIndex;
					}
				}
			}

			// re-setup the mesh
			int newVertsSize = newVerts.size();
			PreCond(newVertsSize>0);
			D3DPVERTEX* newVertsArray = new D3DPVERTEX[newVertsSize];
			for (int i=0; i<newVertsSize && !error; i++)
			{
				newVertsArray[i] = newVerts[i];
			}
			if (!error)
			{
				mesh->SetVertices(newVertsSize,newVertsArray);
			}
		}
	}

	if (error) { safe_delete(mesh); return NULL; }
	if (!GetCompulsaryToken(Get("}"))) { safe_delete(mesh); return NULL; }

	return mesh;
}

// optGeom ->	coord optGeom			|
//				texCoord optGeom		|
//				coordIndex optGeom		|
//				texCoordIndex optGeom	|
//				normal optGeom			|
//				normalIndex optGeom		|
//				e
Mesh* VRMLParser::OptGeom(std::string& name,Model* model,bool& sharingVerts)
{
	std::vector<D3DXVECTOR2> textureCoords;
	std::vector<D3DPINDEX> texCoordIndex;
	std::vector<D3DXVECTOR3> normals;
	std::vector<D3DPINDEX> normalIndex;

	Mesh* mesh = new Mesh();
	do
	{
		token = GetNextToken();
		if (token==Get("coord"))
		{
			UngetToken();
			std::vector<D3DXVECTOR3> v = Coordinate(name,model,sharingVerts);
			if (error) return NULL;

			if (v.size()>0)
			{
				D3DPVERTEX* verts = new D3DPVERTEX[v.size()];
				for (int i=0; i<v.size(); i++)
				{
					verts[i].position = v[i];
					verts[i].texture = D3DXVECTOR2(0,0);
					verts[i].normal = D3DXVECTOR3(0,0,0);
				}
				mesh->SetVertices(v.size(),verts);
				safe_delete(verts);
			}
		}
		else if (token==Get("coordText") || token==Get("texCoord"))
		{
			UngetToken();
			textureCoords = TextureCoordinate();
			if (error) return NULL;
		}
		else if (token==Get("coordIndex"))
		{
			UngetToken();
			std::vector<D3DPINDEX> v = CoordIndex();
			if (error) return NULL;
			if (v.size()>0)
			{
				D3DPINDEX* index = new D3DPINDEX[v.size()];
				for (int i=0; i<v.size(); i++)
				{
					index[i] = v[i];
				}
				mesh->SetIndices(v.size(),index);
				safe_delete(index);
			}
		}
		else if (token==Get("texCoordIndex"))
		{
			UngetToken();
			texCoordIndex = TexCoordIndex();
			if (error) return NULL;
		}
		else if (token == Get("normal"))
		{
			UngetToken();
			normals = Normals();
			if (error) return NULL;
		}
		else if (token == Get("normalIndex"))
		{
			UngetToken();
			normalIndex = NormalIndex();
			if (error) return NULL;
		}
		else
		{
			UngetToken();
			break;
		}
	}
	while (!error);

	D3DPVERTEX* v = mesh->GetVertices();
	D3DPINDEX* ind = mesh->GetIndices();

	// check texture coordinates and normals
	if (!textureCoords.empty())
	{
		if (v == NULL || ind == NULL)
		{
			SetError("no vertices or triangles but texture coordinates?");
			return NULL;
		}

		if (!texCoordIndex.empty())
		{
			if (texCoordIndex.size() != mesh->GetNumTriangles())
			{
				SetError("texture index count does not match triangle count");
				return NULL;
			}

			for (int i=0; i<texCoordIndex.size(); i++)
			{
				v[ind[i].v1].texture = textureCoords[texCoordIndex[i].v1];
				v[ind[i].v2].texture = textureCoords[texCoordIndex[i].v2];
				v[ind[i].v3].texture = textureCoords[texCoordIndex[i].v3];
			}
		}
		else
		{
			int tcSize = min(textureCoords.size(), mesh->GetNumVertices());
			for (int i=0; i<tcSize; i++)
			{
				v[i].texture = textureCoords[i];
			}
		}
	}

	if (!normals.empty())
	{
		if (v == NULL || ind == NULL)
		{
			SetError("no vertices or triangles but normals?");
			return NULL;
		}

		if (!normalIndex.empty())
		{
			if (normalIndex.size() != mesh->GetNumTriangles())
			{
				SetError("normal index count does not match triangle count");
				return NULL;
			}

			for (int i=0; i<normalIndex.size(); i++)
			{
				v[ind[i].v1].normal = normals[normalIndex[i].v1];
				v[ind[i].v2].normal = normals[normalIndex[i].v2];
				v[ind[i].v3].normal = normals[normalIndex[i].v3];
			}
		}
		else
		{
			int nSize = min(normals.size(), mesh->GetNumVertices());
			for (int i=0; i<nSize; i++)
			{
				v[i].normal = normals[i];
			}
		}
	}

	return mesh;
}

// coord -> 'coord' 'DEF' ident 'Coordinate' { 'point' [ f f f ,* ] } |
//			'coord' 'USE' ident
std::vector<D3DXVECTOR3> VRMLParser::Coordinate(std::string& name,Model* model,bool& sharingVerts)
{
	std::vector<D3DXVECTOR3> v;

	if (!GetCompulsaryToken(Get("coord"))) return v;

	sharingVerts = false;
	token = GetNextToken();
	if (token==Get("USE"))
	{
		sharingVerts = true;

		// use another mesh's coords
		if (!GetCompulsaryToken(TIDENT)) return v;
		std::string name = yyString;
		Mesh* mesh = model->GetMesh(name);
		if (mesh==NULL)
		{
			SetError("mesh does not exist");
			return v;
		}
		D3DPVERTEX* verts = mesh->GetVertices();
		int size = mesh->GetNumVertices();
		for (int i=0; i<size; i++)
		{
			v.push_back(verts[i].position);
		}
		return v;
	}

	UngetToken();
	if (!GetCompulsaryToken(Get("DEF"))) return v;
	if (!GetCompulsaryToken(TIDENT)) return v;
	name = yyString;

	if (!GetCompulsaryToken(Get("Coordinate"))) return v;
	if (!GetCompulsaryToken(Get("{"))) return v;
	if (!GetCompulsaryToken(Get("point"))) return v;
	if (!GetCompulsaryToken(Get("["))) return v;

	// read f f f , | f f f ] many times
	do
	{
		float f1,f2,f3;

		token = GetNextToken();
		if (token==Get("]")) break;
		UngetToken();

		if (!GetCompulsaryTokenOption(TFLOAT,TINT)) return v;
		if (token==TFLOAT) f1 = yyFloat; else f1 = (float)yyInt;

		if (!GetCompulsaryTokenOption(TFLOAT,TINT)) return v;
		if (token==TFLOAT) f2 = yyFloat; else f2 = (float)yyInt;

		if (!GetCompulsaryTokenOption(TFLOAT,TINT)) return v;
		if (token==TFLOAT) f3 = yyFloat; else f3 = (float)yyInt;

		if (!GetCompulsaryTokenOption(Get("]"),Get(","))) return v;

		v.push_back(D3DXVECTOR3(f1,f2,f3));
	}
	while (token!=Get("]") && !error);

	if (!GetCompulsaryToken(Get("}"))) return v;

	return v;
}

// coordIndex -> 'coordIndex' [ i, i, i, -1,* ]
std::vector<D3DPINDEX> VRMLParser::CoordIndex()
{
	std::vector<D3DPINDEX> index;

	if (!GetCompulsaryToken(Get("coordIndex"))) return index;
	if (!GetCompulsaryToken(Get("["))) return index;

	// read i,i,i,-1, | i,i,i,-1]
	do
	{
		D3DPINDEX in;
		token = GetNextToken();
		if (token==Get("]")) break;
		UngetToken();

		if (!GetCompulsaryToken(TINT)) return index;
		in.v1 = (WORD)yyInt;
		if (!GetCompulsaryToken(Get(","))) return index;

		if (!GetCompulsaryToken(TINT)) return index;
		in.v2 = (WORD)yyInt;
		if (!GetCompulsaryToken(Get(","))) return index;

		if (!GetCompulsaryToken(TINT)) return index;
		in.v3 = (WORD)yyInt;
		if (!GetCompulsaryToken(Get(","))) return index;

		if (!GetCompulsaryToken(TINT)) return index;
		if (yyInt!=-1)
		{
			SetError("expected -1");
			return index;
		}
		if (!GetCompulsaryTokenOption(Get("]"),Get(","))) return index;

		index.push_back(in);
	}
	while (token!=Get("]") && !error);

	return index;
}

// textCoord -> 'texCoord' 'DEF' ident 'TextureCoordinate' { 'point' [ f f,* ] }
std::vector<D3DXVECTOR2> VRMLParser::TextureCoordinate()
{
	std::vector<D3DXVECTOR2> v;

	if (!GetCompulsaryTokenOption(Get("coordText"),Get("texCoord"))) return v;
	if (!GetCompulsaryToken(Get("DEF"))) return v;
	if (!GetCompulsaryToken(TIDENT)) return v;
	if (!GetCompulsaryToken(Get("TextureCoordinate"))) return v;
	if (!GetCompulsaryToken(Get("{"))) return v;
	if (!GetCompulsaryToken(Get("point"))) return v;
	if (!GetCompulsaryToken(Get("["))) return v;

	// read f f, | f f ] many times
	do
	{
		token = GetNextToken();
		if (token==Get("]")) break;
		UngetToken();

		float f1,f2;
		if (!GetCompulsaryTokenOption(TFLOAT,TINT)) return v;
		if (token==TFLOAT) f1 = yyFloat; else f1 = (float)yyInt;

		if (!GetCompulsaryTokenOption(TFLOAT,TINT)) return v;
		if (token==TFLOAT) f2 = yyFloat; else f2 = (float)yyInt;

		if (!GetCompulsaryTokenOption(Get("]"),Get(","))) return v;

		v.push_back(D3DXVECTOR2(f1,1 - f2));
	}
	while (token!=Get("]") && !error);

	if (!error && !GetCompulsaryToken(Get("}"))) return v;

	return v;
}

// normal ->	'normal' 'Normal' '{' 'vector' '[' [f f f,]* [f f f] ']' '}'
//		        'normalPerVertex' 'TRUE'
//
std::vector<D3DXVECTOR3> VRMLParser::Normals()
{
	std::vector<D3DXVECTOR3> v;

	if (!GetCompulsaryToken(Get("normal"))) return v;
	if (!GetCompulsaryToken(Get("Normal"))) return v;
	if (!GetCompulsaryToken(Get("{"))) return v;
	if (!GetCompulsaryToken(Get("vector"))) return v;
	if (!GetCompulsaryToken(Get("["))) return v;

	do
	{
		float f1,f2,f3;

		token = GetNextToken();
		if (token==Get("]")) break;
		UngetToken();

		if (!GetCompulsaryTokenOption(TFLOAT,TINT)) return v;
		if (token==TFLOAT) f1 = yyFloat; else f1 = (float)yyInt;

		if (!GetCompulsaryTokenOption(TFLOAT,TINT)) return v;
		if (token==TFLOAT) f2 = yyFloat; else f2 = (float)yyInt;

		if (!GetCompulsaryTokenOption(TFLOAT,TINT)) return v;
		if (token==TFLOAT) f3 = yyFloat; else f3 = (float)yyInt;

		if (!GetCompulsaryTokenOption(Get("]"),Get(","))) return v;

		v.push_back(D3DXVECTOR3(f1,f2,f3));
	}
	while (token!=Get("]") && !error);

	if (!GetCompulsaryToken(Get("}"))) return v;

	if (!GetCompulsaryToken(Get("normalPerVertex"))) return v;
	if (!GetCompulsaryToken(Get("TRUE"))) return v;

	return v;
}

// normalIndex -> 'normalIndex' '[' [i, i, i, -1,]* [i, i, i, -1] ']'
//
std::vector<D3DPINDEX> VRMLParser::NormalIndex()
{
	std::vector<D3DPINDEX> index;

	if (!GetCompulsaryToken(Get("normalIndex"))) return index;
	if (!GetCompulsaryToken(Get("["))) return index;

	// read i,i,i,-1, | i,i,i,-1]
	do
	{
		D3DPINDEX in;
		token = GetNextToken();
		if (token==Get("]")) break;
		UngetToken();

		if (!GetCompulsaryToken(TINT)) return index;
		in.v1 = (WORD)yyInt;
		if (!GetCompulsaryToken(Get(","))) return index;

		if (!GetCompulsaryToken(TINT)) return index;
		in.v2 = (WORD)yyInt;
		if (!GetCompulsaryToken(Get(","))) return index;

		if (!GetCompulsaryToken(TINT)) return index;
		in.v3 = (WORD)yyInt;
		if (!GetCompulsaryToken(Get(","))) return index;

		if (!GetCompulsaryToken(TINT)) return index;
		if (yyInt!=-1)
		{
			SetError("expected -1");
			return index;
		}
		if (!GetCompulsaryTokenOption(Get("]"),Get(","))) return index;

		index.push_back(in);
	}
	while (token!=Get("]") && !error);

	return index;
}

// texCoordIndex -> 'texCoordIndex' '[' [i, i, i, -1,]* [i, i, i, -1] ']'
//
std::vector<D3DPINDEX> VRMLParser::TexCoordIndex()
{
	std::vector<D3DPINDEX> index;

	if (!GetCompulsaryToken(Get("texCoordIndex"))) return index;
	if (!GetCompulsaryToken(Get("["))) return index;

	// read i,i,i,-1, | i,i,i,-1]
	do
	{
		D3DPINDEX in;
		token = GetNextToken();
		if (token==Get("]")) break;
		UngetToken();

		if (!GetCompulsaryToken(TINT)) return index;
		in.v1 = (WORD)yyInt;
		if (!GetCompulsaryToken(Get(","))) return index;

		if (!GetCompulsaryToken(TINT)) return index;
		in.v2 = (WORD)yyInt;
		if (!GetCompulsaryToken(Get(","))) return index;

		if (!GetCompulsaryToken(TINT)) return index;
		in.v3 = (WORD)yyInt;
		if (!GetCompulsaryToken(Get(","))) return index;

		if (!GetCompulsaryToken(TINT)) return index;
		if (yyInt!=-1)
		{
			SetError("expected -1");
			return index;
		}
		if (!GetCompulsaryTokenOption(Get("]"),Get(","))) return index;

		index.push_back(in);
	}
	while (token!=Get("]") && !error);

	return index;
}

