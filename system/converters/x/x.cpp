#include "standardFirst.h"

#include "system/converters/x/x.h"
#include "system/skin/skin.h"
#include "system/skin/bone.h"
#include "system/skin/boneAnimation.h"
#include "system/skin/boneInfluence.h"
#include "system/model/mesh.h"
#include "system/model/model.h"

/////////////////////////////////////////////////////////////////////////////

X::X()
{
	// these primitives are used
	EnableInt();
	EnableFloat();
	EnableDoubleQuoteString();
	EnableIdent();

	Add("xof 0303txt 0032",true);
	Add("Frame",true);
	Add("FrameTransformMatrix",true);
	Add("{",false);
	Add("}",false);
	Add(";",false);
	Add(",",false);
	Add("Mesh",true);
	Add("MeshNormals",true);
	Add("MeshTextureCoords",true);
	Add("VertexDuplicationIndices",true);
	Add("MeshMaterialList",true);
	Add("Material",true);
	Add("TextureFilename",true);
	Add("XSkinMeshHeader",true);
	Add("SkinWeights",true);
	Add("AnimationSet",true);
	Add("Animation",true);
	Add("AnimationKey",true);
}

X::~X()
{
}

X::X(const X& x)
{
	operator=(x);
}

const X& X::operator=(const X& x)
{
	Tokeniser::operator =(x);

	file = x.file;

	return *this;
}

void X::ParseFile()
{
	GetCompulsaryToken("xof 0303txt 0032");
	if (error) return;

	Token t1 = Get("Frame");
	Token t2 = Get("AnimationSet");

	do
	{
		do
		{
			token = GetNextToken();
		}
		while (token!=t1 && token!=t2 && token!=TEOF && !error);

		if (token==TEOF)
		{
			break;
		}
		else if (token==t1)
		{
			UngetToken();
			Frame f = ParseFrame();
			if (error) return;
			file.frames.push_back(f);
		}
		else if (token==t2)
		{
			UngetToken();
			AnimationSet s = ParseAnimationSet();
			if (error) return;
			file.animations.push_back(s);
		}
	}
	while (!error);
}

// frame -> 'Frame' [ident|e] '{' 'FrameTransformMatrix' '{' matrix '}' [frame|mesh] '}'
//
X::Frame X::ParseFrame()
{
	Frame f;

	GetCompulsaryToken("Frame");
	if (error) return f;

	if (GetOptionalToken(TIDENT))
		f.name = yyString;

	GetCompulsaryToken("{");
	if (error) return f;

	GetCompulsaryToken("FrameTransformMatrix");
	if (error) return f;

	GetCompulsaryToken("{");
	if (error) return f;

	f.frameTransformationMatrix = ParseMatrix();
	if (error) return f;

	GetCompulsaryToken("}");
	if (error) return f;

	do
	{
		token = GetNextToken();
		UngetToken();
		if (token==Get("Mesh"))
		{
			f.meshes.push_back(ParseMesh());
		}
		else if (token==Get("Frame"))
		{
			f.frames.push_back(ParseFrame());
		}
		else
		{
			break;
		}
	}
	while (!error);

	if (!error)
		GetOptionalToken("}");

	return f;
}

// matrix -> f,f,f,f, f,f,f,f, f,f,f,f, f,f,f,f ; ;
//
X::Matrix X::ParseMatrix()
{
	Matrix m;

	for (int i=0; i<16; i++)
	{
		GetCompulsaryToken(TFLOAT);
		m.f[i] = yyFloat;

		token = GetNextToken();
		if (i==15 && token!=Get(";"))
		{
			SetError("expected ';'");
			return m;
		}
		else if (i<15 && token!=Get(","))
		{
			SetError("expected ','");
			return m;
		}
	}
	GetCompulsaryToken(";");
	return m;
}

// mesh -> 'Mesh' [ident] '{' int ; [f;f;f;,]+ f;f;f;; int ; [3;i,i,i;,]+ 3;i,i,i;;
//			[meshNormals] [meshTextureCoords] [vertexDuplicationIndices]
//			[meshMaterialList] [xskinMeshHeader] [skinWeights]* '}'
//
X::XMesh X::ParseMesh()
{
	XMesh m;

	GetCompulsaryToken("Mesh");
	if (error) return m;

	token = GetNextToken();
	if (token == TIDENT)
	{
		m.name = yyString;
	}
	else
	{
		UngetToken();
	}

	GetCompulsaryToken("{");
	if (error) return m;

	GetCompulsaryToken(TINT);
	if (error) return m;
	m.numVertices = yyInt;
	GetCompulsaryToken(";");
	if (error) return m;

	m.vertices = new Vertex[m.numVertices];
	for (int i=0; i<m.numVertices; i++)
	{
		GetCompulsaryToken(TFLOAT);
		float v1 = yyFloat;
		GetCompulsaryToken(";");

		GetCompulsaryToken(TFLOAT);
		float v2 = yyFloat;
		GetCompulsaryToken(";");

		GetCompulsaryToken(TFLOAT);
		float v3 = yyFloat;
		GetCompulsaryToken(";");

		if ((i+1)<m.numVertices)
		{
			GetCompulsaryToken(",");
		}
		else
		{
			GetCompulsaryToken(";");
		}
		if (error) return m;

		m.vertices[i].x = v1;
		m.vertices[i].y = v2;
		m.vertices[i].z = v3;
	}

	GetCompulsaryToken(TINT);
	if (error) return m;
	m.numFaces = yyInt;
	GetCompulsaryToken(";");
	if (error) return m;

	m.faces = new Face[m.numFaces];
	for (int i=0; i<m.numFaces; i++)
	{
		GetCompulsaryToken(TINT);
		if (yyInt!=3)
		{
			SetError("can only parse triangles");
		}
		GetCompulsaryToken(";");

		GetCompulsaryToken(TINT);
		int f1 = yyInt;
		GetCompulsaryToken(",");

		GetCompulsaryToken(TINT);
		int f2 = yyInt;
		GetCompulsaryToken(",");

		GetCompulsaryToken(TINT);
		int f3 = yyInt;
		GetCompulsaryToken(";");

		if ((i+1)<m.numFaces)
		{
			GetCompulsaryToken(",");
		}
		else
		{
			GetCompulsaryToken(";");
		}
		if (error) return m;

		m.faces[i].v1 = f1;
		m.faces[i].v2 = f2;
		m.faces[i].v3 = f3;
	}

//			[meshNormals] [meshTextureCoords] [vertexDuplicationIndices]
//			[meshMaterialList] [xskinMeshHeader] [skinWeights]* '}'
	do
	{
		token = GetNextToken();
		if (token==Get("MeshNormals"))
		{
			UngetToken();
			m.normals.push_back(ParseMeshNormals());
		}
		else if (token==Get("MeshTextureCoords"))
		{
			UngetToken();
			m.textureCoords.push_back(ParseMeshTextureCoords());
		}
		else if (token==Get("VertexDuplicationIndices"))
		{
			UngetToken();
			m.duplicationIndices.push_back(ParseVertexDuplicationIndices());
		}
		else if (token==Get("MeshMaterialList"))
		{
			UngetToken();
			m.materialList.push_back(ParseMeshMaterialList());
		}
		else if (token==Get("XSkinMeshHeader"))
		{
			UngetToken();
			m.xskinMeshHeader.push_back(ParseXSkinMeshHeader());
		}
		else if (token==Get("SkinWeights"))
		{
			UngetToken();
			m.skinWeights.push_back(ParseSkinWeights());
		}
		else if (token==Get("}"))
		{
			break;
		}
		if (token==TEOF || token==TUNKNOWN)
		{
			SetError("expected '}'");
			break;
		}
		if (error) break;
	}
	while (!error);

	if (!error)
		GetCompulsaryToken("}");

	return m;
}

// meshNormals -> 'MeshNormals' '{' int ; [f;f;f;,]+ f;f;f;; int ; [3;i,i,i;,]+ 3;i,i,i;; '}'
//
X::MeshNormals X::ParseMeshNormals()
{
	MeshNormals mn;

	GetCompulsaryToken("MeshNormals");
	if (error) return mn;
	GetCompulsaryToken("{");
	if (error) return mn;

	GetCompulsaryToken(TINT);
	if (error) return mn;
	mn.numNormals = yyInt;
	GetCompulsaryToken(";");
	if (error) return mn;

	mn.normals = new Vertex[mn.numNormals];
	for (int i=0; i<mn.numNormals; i++)
	{
		GetCompulsaryToken(TFLOAT);
		float v1 = yyFloat;
		GetCompulsaryToken(";");

		GetCompulsaryToken(TFLOAT);
		float v2 = yyFloat;
		GetCompulsaryToken(";");

		GetCompulsaryToken(TFLOAT);
		float v3 = yyFloat;
		GetCompulsaryToken(";");

		if ((i+1)<mn.numNormals)
		{
			GetCompulsaryToken(",");
		}
		else
		{
			GetCompulsaryToken(";");
		}
		if (error) return mn;

		mn.normals[i].x = v1;
		mn.normals[i].y = v2;
		mn.normals[i].z = v3;
	}

	GetCompulsaryToken(TINT);
	if (error) return mn;
	mn.numFaces = yyInt;
	GetCompulsaryToken(";");
	if (error) return mn;

	mn.faces = new Face[mn.numFaces];
	for (int i=0; i<mn.numFaces; i++)
	{
		GetCompulsaryToken(TINT);
		if (yyInt!=3)
		{
			SetError("can only parse triangles");
		}
		GetCompulsaryToken(";");

		GetCompulsaryToken(TINT);
		int f1 = yyInt;
		GetCompulsaryToken(",");

		GetCompulsaryToken(TINT);
		int f2 = yyInt;
		GetCompulsaryToken(",");

		GetCompulsaryToken(TINT);
		int f3 = yyInt;
		GetCompulsaryToken(";");

		if ((i+1)<mn.numFaces)
		{
			GetCompulsaryToken(",");
		}
		else
		{
			GetCompulsaryToken(";");
		}
		if (error) return mn;

		mn.faces[i].v1 = f1;
		mn.faces[i].v2 = f2;
		mn.faces[i].v3 = f3;
	}

	if (!error)
		GetCompulsaryToken("}");

	return mn;
}

// meshTextureCoords -> 'MeshTextureCoords' '{' int ; [f;f;,]+ f;f;; '}'
//
X::MeshTextureCoords X::ParseMeshTextureCoords()
{
	MeshTextureCoords f;

	GetCompulsaryToken("MeshTextureCoords");
	if (error) return f;
	GetCompulsaryToken("{");
	if (error) return f;

	GetCompulsaryToken(TINT);
	if (error) return f;
	f.count = yyInt;
	GetCompulsaryToken(";");
	if (error) return f;

	f.uvs = new UV[f.count];
	for (int i=0; i<f.count; i++)
	{
		GetCompulsaryToken(TFLOAT);
		float u = yyFloat;
		GetCompulsaryToken(";");

		GetCompulsaryToken(TFLOAT);
		float v = yyFloat;
		GetCompulsaryToken(";");

		if ((i+1)<f.count)
		{
			GetCompulsaryToken(",");
		}
		else
		{
			GetCompulsaryToken(";");
		}
		if (error) return f;

		f.uvs[i].u = u;
		f.uvs[i].v = v;
	}

	if (!error)
		GetCompulsaryToken("}");

	return f;
}

// vertexDuplicationIndices -> 'VertexDuplicationIndices' '{' int ; int ; [int,]+ int; '}'
//
X::VertexDuplicationIndices X::ParseVertexDuplicationIndices()
{
	VertexDuplicationIndices vd;

	GetCompulsaryToken("VertexDuplicationIndices");
	if (error) return vd;
	GetCompulsaryToken("{");
	if (error) return vd;

	GetCompulsaryToken(TINT);
	if (error) return vd;
	vd.numIndices = yyInt;
	GetCompulsaryToken(";");
	if (error) return vd;

	GetCompulsaryToken(TINT);
	if (error) return vd;
	vd.numOriginalIndices = yyInt;
	GetCompulsaryToken(";");
	if (error) return vd;

	vd.indices = new int[vd.numIndices];
	for (int i=0; i<vd.numIndices; i++)
	{
		GetCompulsaryToken(TINT);
		vd.indices[i] = yyInt;

		if ((i+1)<vd.numIndices)
		{
			GetCompulsaryToken(",");
		}
		else
		{
			GetCompulsaryToken(";");
		}
		if (error) return vd;
	}

	if (!error)
		GetCompulsaryToken("}");

	return vd;
}

// meshMaterialList -> 'MeshMaterialList' '{' int ; int ; [int,]+ int; [material] '}'
//
X::MeshMaterialList X::ParseMeshMaterialList()
{
	MeshMaterialList mm;

	GetCompulsaryToken("MeshMaterialList");
	if (error) return mm;
	GetCompulsaryToken("{");
	if (error) return mm;

	GetCompulsaryToken(TINT);
	if (error) return mm;
	mm.numMaterials = yyInt;
	GetCompulsaryToken(";");
	if (error) return mm;

	GetCompulsaryToken(TINT);
	if (error) return mm;
	mm.numVertices = yyInt;
	GetCompulsaryToken(";");
	if (error) return mm;

	mm.vertexMaterials = new int[mm.numVertices];
	for (int i=0; i<mm.numVertices; i++)
	{
		GetCompulsaryToken(TINT);
		mm.vertexMaterials[i] = yyInt;

		if ((i+1)<mm.numVertices)
		{
			GetCompulsaryToken(",");
		}
		else
		{
			GetCompulsaryToken(";");
		}
		if (error) return mm;
	}

	if (mm.numMaterials>0)
	{
		mm.materials = new Material[mm.numMaterials];
		for (int i=0; i<mm.numMaterials; i++)
		{
			mm.materials[i] = ParseMaterial();
			if (error) return mm;
		}
	}

	if (!error)
		GetCompulsaryToken("}");

	return mm;
}

// material -> 'Material' '{' f;f;f;f;;  f;  f;f;f;;  f;f;f;; [textureFilename] '}'
//
// textureFilename -> 'TextureFilename' '{' " string " ; '}'
//
X::Material X::ParseMaterial()
{
	Material m;

	GetCompulsaryToken("Material");
	if (error) return m;
	GetCompulsaryToken("{");
	if (error) return m;

	// diffuse colour
	GetCompulsaryToken(TFLOAT);
	if (error) return m;
	m.diff[0] = yyFloat;
	GetCompulsaryToken(";");
	if (error) return m;

	GetCompulsaryToken(TFLOAT);
	if (error) return m;
	m.diff[1] = yyFloat;
	GetCompulsaryToken(";");
	if (error) return m;

	GetCompulsaryToken(TFLOAT);
	if (error) return m;
	m.diff[2] = yyFloat;
	GetCompulsaryToken(";");
	if (error) return m;

	GetCompulsaryToken(TFLOAT);
	if (error) return m;
	m.diff[3] = yyFloat;
	GetCompulsaryToken(";");
	if (error) return m;
	GetCompulsaryToken(";");
	if (error) return m;

	// opacity
	GetCompulsaryToken(TFLOAT);
	if (error) return m;
	m.opacity = yyFloat;
	GetCompulsaryToken(";");
	if (error) return m;

	// col1
	GetCompulsaryToken(TFLOAT);
	if (error) return m;
	m.col1[0] = yyFloat;
	GetCompulsaryToken(";");
	if (error) return m;

	GetCompulsaryToken(TFLOAT);
	if (error) return m;
	m.col1[1] = yyFloat;
	GetCompulsaryToken(";");
	if (error) return m;

	GetCompulsaryToken(TFLOAT);
	if (error) return m;
	m.col1[2] = yyFloat;
	GetCompulsaryToken(";");
	if (error) return m;
	GetCompulsaryToken(";");
	if (error) return m;

	// col2
	GetCompulsaryToken(TFLOAT);
	if (error) return m;
	m.col2[0] = yyFloat;
	GetCompulsaryToken(";");
	if (error) return m;

	GetCompulsaryToken(TFLOAT);
	if (error) return m;
	m.col2[1] = yyFloat;
	GetCompulsaryToken(";");
	if (error) return m;

	GetCompulsaryToken(TFLOAT);
	if (error) return m;
	m.col2[2] = yyFloat;
	GetCompulsaryToken(";");
	if (error) return m;
	GetCompulsaryToken(";");
	if (error) return m;

	// textureFilename
	token = GetNextToken();
	if (token==Get("TextureFilename"))
	{
		GetCompulsaryToken("{");
		if (error) return m;

		GetCompulsaryToken(TSTRING);
		if (error) return m;
		m.textureFilename = yyString;

		GetCompulsaryToken(";");
		if (error) return m;
		GetCompulsaryToken("}");
		if (error) return m;
	}
	else
	{
		UngetToken();
	}

	GetCompulsaryToken("}");

	return m;
}

// xskinMeshHeader -> 'XSkinMeshHeader' '{' int ; int ; int ; '}'
//
X::XSkinMeshHeader X::ParseXSkinMeshHeader()
{
	XSkinMeshHeader x;
	x.maxSkinWeightsPerFace = 0;
	x.numBones = 0;
	x.maxSkinWeightsPerVertex = 0;

	GetCompulsaryToken("XSkinMeshHeader");
	if (error) return x;
	GetCompulsaryToken("{");
	if (error) return x;

	GetCompulsaryToken(TINT);
	if (error) return x;
	x.maxSkinWeightsPerVertex = yyInt;
	GetCompulsaryToken(";");
	if (error) return x;

	GetCompulsaryToken(TINT);
	if (error) return x;
	x.maxSkinWeightsPerFace = yyInt;
	GetCompulsaryToken(";");
	if (error) return x;

	GetCompulsaryToken(TINT);
	if (error) return x;
	x.numBones = yyInt;
	GetCompulsaryToken(";");
	if (error) return x;

	GetCompulsaryToken("}");
	if (error) return x;

	return x;
}

// skinWeights -> 'SkinWeights' '{' " string " ; int ; [int,]+ int; [f,]+ f; matrix '}'
//
X::SkinWeights X::ParseSkinWeights()
{
	SkinWeights sw;

	GetCompulsaryToken("SkinWeights");
	if (error) return sw;
	GetCompulsaryToken("{");
	if (error) return sw;

	GetCompulsaryToken(TSTRING);
	if (error) return sw;
	sw.name = yyString;
	GetCompulsaryToken(";");
	if (error) return sw;

	GetCompulsaryToken(TINT);
	if (error) return sw;
	sw.count = yyInt;
	GetCompulsaryToken(";");
	if (error) return sw;

	sw.vertexIndices = new int[sw.count];
	for (int i=0; i<sw.count; i++)
	{
		GetCompulsaryToken(TINT);
		sw.vertexIndices[i] = yyInt;

		if ((i+1)<sw.count)
		{
			GetCompulsaryToken(",");
		}
		else
		{
			GetCompulsaryToken(";");
		}
		if (error) return sw;
	}

	sw.weights = new float[sw.count];
	for (int i=0; i<sw.count; i++)
	{
		GetCompulsaryToken(TFLOAT);
		sw.weights[i] = yyFloat;

		if ((i+1)<sw.count)
		{
			GetCompulsaryToken(",");
		}
		else
		{
			GetCompulsaryToken(";");
		}
		if (error) return sw;
	}

	sw.matrix = ParseMatrix();

	GetCompulsaryToken("}");
	return sw;
}

// animationSet -> 'AnimationSet' '{' animation* '}'
//
X::AnimationSet X::ParseAnimationSet()
{
	AnimationSet as;

	GetCompulsaryToken("AnimationSet");
	if (error) return as;
	GetCompulsaryToken("{");
	if (error) return as;

	do
	{
		token = GetNextToken();
		if (token==TUNKNOWN || token==TEOF)
		{
			break;
		}
		UngetToken();
		if (token==Get("Animation"))
		{
			as.set.push_back(ParseAnimation());
			if (error) break;
		}
		else
		{
			break;
		}
	}
	while (!error);

	if (!error)
		GetOptionalToken("}");

	return as;
}

// animation -> 'Animation' '{' animationKey '{' ident '}' '}'
//
X::Animation X::ParseAnimation()
{
	Animation a;

	GetCompulsaryToken("Animation");
	if (error) return a;
	GetCompulsaryToken("{");
	if (error) return a;

	a.key = ParseAnimationKey();
	if (error) return a;

	GetCompulsaryToken("{");
	if (error) return a;
	GetCompulsaryToken(TIDENT);
	if (error) return a;
	a.name = yyString;
	GetCompulsaryToken("}");
	if (error) return a;

	GetCompulsaryToken("}");
	return a;
}

// animationKey -> 'AnimationKey' '{' i; i; [ i; i; matrix,]+ i; i; matrix; '}'
//
X::AnimationKey X::ParseAnimationKey()
{
	AnimationKey ak;

	GetCompulsaryToken("AnimationKey");
	if (error) return ak;
	GetCompulsaryToken("{");
	if (error) return ak;

	GetCompulsaryToken(TINT);
	if (error) return ak;
	ak.four = yyInt;
	GetCompulsaryToken(";");
	if (error) return ak;

	GetCompulsaryToken(TINT);
	if (error) return ak;
	ak.count = yyInt;
	GetCompulsaryToken(";");
	if (error) return ak;

	ak.keys = new AnimationKeyItem[ak.count];
	for (int i=0; i<ak.count; i++)
	{
		GetCompulsaryToken(TINT);
		if (error) return ak;
		ak.keys[i].time = yyInt;
		GetCompulsaryToken(";");
		if (error) return ak;

		GetCompulsaryToken(TINT);
		if (error) return ak;
		ak.keys[i].sixteen = yyInt;
		GetCompulsaryToken(";");
		if (error) return ak;

		ak.keys[i].matrix = ParseMatrix();
		if (error) return ak;

		if ((i+1)<ak.count)
		{
			GetCompulsaryToken(",");
		}
		else
		{
			GetCompulsaryToken(";");
		}
	}

	GetCompulsaryToken("}");
	return ak;
}

X::XMesh* X::GetMesh(Frame& f)
{
	if (f.meshes.size()>0)
	{
		return &f.meshes[0];
	}

	for (int i=0; i<f.frames.size(); i++)
	{
		XMesh* xm = GetMesh(f.frames[i]);
		if (xm!=NULL)
			return xm;
	}
	return NULL;
}

D3DXMATRIXA16 X::ConvertMatrix(Matrix& m)
{
	D3DXMATRIXA16 mat;

	for (int i=0; i<16; i++)
	{
		mat[i] = m.f[i];
	}
	return mat;
}

Bone* X::FramesToSkeleton(Frame& f)
{
	Bone* b = new Bone();
	b->SetName(f.name);

	D3DXQUATERNION quat;
	D3DXMATRIXA16 mat = ConvertMatrix(f.frameTransformationMatrix);
	D3DXQuaternionRotationMatrix(&quat,&mat);
	D3DXVECTOR3 pos = D3DXVECTOR3(mat._41,mat._42,mat._43);
	b->SetLocalTransform(pos,quat);

	for (int i=0; i<f.frames.size(); i++)
	{
		Bone* bone = FramesToSkeleton(f.frames[i]);
		if (bone != NULL)
			b->AttachChild(bone);
	}
	return b;
}

Mesh* X::ProcessMesh(XMesh* mesh, std::string& textureFilename)
{
	if (mesh != NULL)
	{
		if (mesh->numFaces > 0 && mesh->numVertices > 0)
		{
			Mesh* newMesh = new Mesh();

			// setup indices
			D3DPINDEX* index = new D3DPINDEX[mesh->numFaces];
			for (int i=0; i < mesh->numFaces; i++)
			{
				index[i].v1 = mesh->faces[i].v1;
				index[i].v2 = mesh->faces[i].v2;
				index[i].v3 = mesh->faces[i].v3;
			}
			newMesh->SetIndices(mesh->numFaces, index);
			safe_delete_array(index);

			// setup vertices
			D3DPVERTEX* verts = new D3DPVERTEX[mesh->numVertices];
			for (int i=0; i < mesh->numVertices; i++)
			{
				verts[i].position.x = mesh->vertices[i].x;
				verts[i].position.y = mesh->vertices[i].y;
				verts[i].position.z = mesh->vertices[i].z;
			}
			newMesh->SetVertices(mesh->numVertices, verts);
			safe_delete_array(verts);

			// get pointers to mesh data
			D3DPVERTEX* v = newMesh->GetVertices();
			D3DPINDEX* ind = newMesh->GetIndices();

			// setup uvs
			if (!mesh->textureCoords.empty())
			{
				int numUVs = mesh->textureCoords[0].count;
				if (numUVs != mesh->numVertices)
					throw new Exception("uv count does not match vertex count");

				UV* uvs = mesh->textureCoords[0].uvs;
				for (int i=0; i < numUVs; i++)
				{
					v[i].texture.x = uvs[i].u;
					v[i].texture.y = uvs[i].v;
				}
			}

			// setup normals
			if (!mesh->normals.empty())
			{
				MeshNormals* mn = &mesh->normals[0];

				if (mn->numFaces != newMesh->GetNumTriangles())
					throw new Exception("normal face count does not match number of triangles");

				D3DXVECTOR3 n1, n2, n3;
				for (int i=0; i< mn->numFaces; i++)
				{
					n1 = D3DXVECTOR3( mn->normals[ mn->faces[i].v1 ].x, mn->normals[ mn->faces[i].v1 ].y, mn->normals[ mn->faces[i].v1 ].z);
					n2 = D3DXVECTOR3( mn->normals[ mn->faces[i].v2 ].x, mn->normals[ mn->faces[i].v2 ].y, mn->normals[ mn->faces[i].v2 ].z);
					n3 = D3DXVECTOR3( mn->normals[ mn->faces[i].v3 ].x, mn->normals[ mn->faces[i].v3 ].y, mn->normals[ mn->faces[i].v3 ].z);

					v[ ind[i].v1 ].normal = n1;
					v[ ind[i].v2 ].normal = n2;
					v[ ind[i].v3 ].normal = n3;
				}
			}

			// texture?
			textureFilename.clear();
			if (!mesh->materialList.empty())
			{
				if (mesh->materialList[0].numMaterials == 1)
				{
					Material mm = mesh->materialList[0].materials[0];
					if (!mm.textureFilename.empty())
					{
						textureFilename = mm.textureFilename;
					}
				}
			}

			// and set the mesh into the skin
			return newMesh;
		}
	}
	return NULL;
}

Bone* X::GetBoneByName(Bone* root, const std::string& name)
{
	if (root == NULL)
		return NULL;
	if (root->GetName() == name)
		return root;

	std::vector<PRS*> children = root->GetChildren();
	for (int i=0; i < children.size(); i++)
	{
		Bone* c = GetBoneByName(dynamic_cast<Bone*>(children[i]), name);
		if (c != NULL)
			return c;
	}
	return NULL;
}

int X::CountBones(Bone* bone)
{
	if (bone == NULL)
		return 0;

	int count = 1;
	std::vector<PRS*> children = bone->GetChildren();
	for (int i=0; i < children.size(); i++)
	{
		count += CountBones(dynamic_cast<Bone*>(children[i]));
	}
	return count;
}

Bone* X::GetSkeleton()
{
	// first frame contains mesh and bones
	if (file.frames.empty())
		return NULL;
	Bone* bones = FramesToSkeleton(file.frames[0]);
	int cb = CountBones(bones);
	if (cb <= 2)
		return NULL;
	return bones;
}

void X::ProcessAnimations(Bone* root)
{
	if (file.animations.empty())
	{
		throw new Exception("x-file does not contain any animations");
	}
	else
	{
		AnimationSet set = file.animations[0];

		for (int i=0; i < set.set.size(); i++)
		{
			Animation a = set.set[i];

			// find the corresponding bone
			Bone* b = GetBoneByName(root, a.name);
			if (b == NULL)
			{
				throw new Exception("animation refers to a non-existing bone");
			}

			AnimationKey ak = a.key;

			if (ak.count < 2)
			{
				throw new Exception(b->GetName() + ": found animation with too few frames");
			}

			BoneAnimation* ba = new BoneAnimation();
			int fps = BoneAnimation::GetFramesPerSecond();
			if (fps < 1 || fps > 100)
			{
				throw new Exception("bone animation object fps too low or high: "+System::Int2Str(fps));
			}
			float timerCounterFloat = 1000.0f / (float)fps;
			int timerCounter = (int)timerCounterFloat;

			// sample the animation at x fps
			// get a count first
			int animationKeyIndex = 1;
			int time = 0;
			int prevTime = 0;
			for (int j=0; j < ak.count; j++)
			{
				AnimationKeyItem aki = ak.keys[j];
				
				int delta = aki.time - prevTime;
				prevTime = aki.time;
				time += delta;

				D3DXMATRIXA16 matrix = ConvertMatrix(aki.matrix);
				while (time > 0)
				{
					animationKeyIndex++;
					time = time - timerCounter;
				}
			}

			// set total number of frames
			int numFrames = animationKeyIndex;

			time = 0;
			prevTime = 0;

			D3DXMATRIXA16 prevMatrix;
			D3DXQUATERNION prevQ;
			D3DXVECTOR3 prevPos;

			ba->SetNumFrames(numFrames);
			D3DXQUATERNION* rot = new D3DXQUATERNION[numFrames];
			D3DXVECTOR3* trans = new D3DXVECTOR3[numFrames];

			animationKeyIndex = 0;
			for (int j=0; j < ak.count; j++)
			{
				AnimationKeyItem aki = ak.keys[j];

				int delta = aki.time - prevTime;
				prevTime = aki.time;
				time += delta;

				if (j == 0)
				{
					prevMatrix = ConvertMatrix(aki.matrix);
					D3DXQuaternionRotationMatrix(&prevQ, &prevMatrix);
					prevPos = D3DXVECTOR3(prevMatrix._41, prevMatrix._42, prevMatrix._43);

					rot[animationKeyIndex] = prevQ;
					trans[animationKeyIndex] = prevPos;
					animationKeyIndex++;
				}
				else
				{
					D3DXQUATERNION q;
					D3DXVECTOR3 v;

					D3DXMATRIXA16 matrix = ConvertMatrix(aki.matrix);
					D3DXQuaternionRotationMatrix(&q,&matrix);
					v = D3DXVECTOR3(matrix._41, matrix._42, matrix._43);

					while (time > 0)
					{
						float perc = 1.0f - ((float)time / (float)delta);
						D3DXVECTOR3 pos = prevPos * perc + v * (1.0f - perc);
						D3DXQUATERNION quat;

						// checked and verified right order
						D3DXQuaternionSlerp(&quat, &q, &prevQ, perc);

						rot[animationKeyIndex] = quat;
						trans[animationKeyIndex] = pos;

						// pass on new values for next round
						prevQ = quat;
						prevPos = pos;

						animationKeyIndex++;

						time = time - timerCounter;
					}
				}
			}

			// set the animation onto the bone
			ba->SetRotations(numFrames, rot);
			ba->SetTranslations(numFrames, trans);
			b->AddAnimation(ba);

			safe_delete_array(rot);
			safe_delete_array(trans);
		}
	}
}

void X::ProcessSkinWeights(Skin* skin, XMesh* mesh)
{
	if (!mesh->skinWeights.empty())
	{
		// for each vertex - setup a bone influence set
		Mesh* skinMesh = skin->GetSkin();
		if (skinMesh == NULL)
		{
			throw new Exception("skin does not have a mesh");
		}
		int numVertices = skinMesh->GetNumVertices();
		D3DPVERTEX* vertices = skinMesh->GetVertices();
		if (numVertices == 0 || vertices == NULL)
		{
			throw new Exception("skin mesh does not have any vertices");
		}

		BoneInfluence* influenceSet = new BoneInfluence[numVertices];

		for (int i=0; i < mesh->skinWeights.size(); i++)
		{
			// get skinweights and bone of skinweight
			SkinWeights sw = mesh->skinWeights[i];
			Bone* bone = GetBoneByName(skin->GetRootBone(), sw.name);
			if (bone == NULL)
			{
				throw new Exception("bone " + sw.name + " not found for skin weighting");
			}

			// process
			for (int j=0; j < sw.count; j++)
			{
				// add for a specified index the bone and weight
				int ind = sw.vertexIndices[j];
				influenceSet[ind].Add(bone, sw.weights[j]);
			}

		}

		// set the influences
		skin->SetBoneInfluences(numVertices, influenceSet);

		// process the bone weights and invert the vertices
		// to reflect their 'zero position'
		for (int i=0; i < numVertices; i++)
		{
			BoneInfluence* infl = &influenceSet[i];
			int count = infl->GetCount();
			float* weights = infl->GetWeights();
			Bone** bones = infl->GetBones();

			if (count == 0)
			{
				throw new Exception("found vertex without weights");
				return;
			}

			// get first skinweight by name
			D3DXMATRIXA16 invMatrix;
			std::string boneName = bones[0]->GetName();
			for (int k=0; k<mesh->skinWeights.size(); k++)
			{
				if (mesh->skinWeights[k].name == boneName)
				{
					// get inverse of vertices
					invMatrix = ConvertMatrix(mesh->skinWeights[k].matrix);
					break;
				}
			}

			D3DXMATRIXA16 transform = (weights[0] * invMatrix);
			for (int j=1; j < count; j++)
			{
				boneName = bones[j]->GetName();
				for (int k=0; k<mesh->skinWeights.size(); k++)
				{
					if (mesh->skinWeights[k].name == boneName)
					{
						// get inverse of vertices
						invMatrix = ConvertMatrix(mesh->skinWeights[k].matrix);
						break;
					}
				}
				transform += (weights[j] * invMatrix);
			}
			D3DXVec3TransformCoord(&vertices[i].position, &vertices[i].position, &transform);
			D3DXVec3TransformNormal(&vertices[i].normal, &vertices[i].normal, &transform);
		}

		safe_delete_array(influenceSet);
	}
}

WorldObject* X::ParseLoadedFile()
{
	if (error)
		return NULL;

	ParseFile();

	if (error || file.frames.empty())
		return NULL;

	// get skeleton first
	Bone* rootBone = GetSkeleton();
	if (rootBone == NULL)
	{
		// a non skeletal mesh - just a mesh
		Model* model = new Model();

		XMesh* mesh = GetMesh(file.frames[0]);
		if (mesh == NULL)
			throw new Exception("object does not have a mesh");

		// set the meshes transform
		std::string textureFilename;
		Mesh* dxmesh = ProcessMesh(mesh, textureFilename);
		model->SetMesh(dxmesh);
		safe_delete(dxmesh);

		if (!textureFilename.empty())
		{
			model->SetTexture(TextureCache::GetTexture(textureFilename));
		}
		return model;
	}
	else
	{
		// put it all together in a skin
		Skin* skin = new Skin();
		skin->SetRootBone(rootBone);

		// get mesh
		XMesh* mesh = GetMesh(file.frames[0]);
		if (mesh == NULL)
			throw new Exception("object does not have a mesh");

		// set the meshes transform
		std::string textureFilename;
		Mesh* dxmesh = ProcessMesh(mesh, textureFilename);
		dxmesh->UseForSkinning();
		skin->SetSkin(dxmesh);
		safe_delete(dxmesh);

		if (!textureFilename.empty())
		{
			skin->SetTexture(TextureCache::GetTexture(textureFilename));
		}

		// process the animations
		ProcessAnimations(skin->GetRootBone());

		// process skin weights
		ProcessSkinWeights(skin, mesh);

		// clear allocated memory
		safe_delete(rootBone);

		return skin;
	}
}

