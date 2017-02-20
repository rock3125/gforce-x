#pragma once

#include "system/modelImporter.h"

class WorldObject;
class Bone;
class Mesh;
class Skin;

/////////////////////////////////////////////////////////////////////////////
// grammar
//
// file -> 'xof 0303txt 0032' [?]* frame+ animationSet+
//
// frame -> 'Frame' [ident|e] '{' 'FrameTransformMatrix' '{' matrix '}' [frame|mesh] '}'
//
// matrix -> f,f,f,f, f,f,f,f, f,f,f,f, f,f,f,f ; ;
//
// mesh -> 'Mesh' [ident] '{' int ; [f;f;f;,]+ f;f;f;; int ; [3;i,i,i;,]+ 3;i,i,i;;
//			[meshNormals] [meshTextureCoords] [vertexDuplicationIndices]
//			[meshMaterialList] [xskinMeshHeader] [skinWeights]* '}'
//
// meshNormals -> 'MeshNormals' '{' int ; [f;f;f;,]+ f;f;f;; int ; [3;i,i,i;,]+ 3;i,i,i;; '}'
//
// meshTextureCoords -> 'MeshTextureCoords' '{' int ; [f;f;,]+ f;f;; '}'
//
// vertexDuplicationIndices -> 'VertexDuplicationIndices' '{' int ; int ; [int,]+ int; '}'
//
// meshMaterialList -> 'MeshMaterialList' '{' int ; int ; [int,]+ int; [material] '}'
//
// material -> 'Material' '{' f;f;f;f;;  f;  f;f;f;;  f;f;f;; [textureFilename] '}'
//
// textureFilename -> 'TextureFilename' '{' " string " ; '}'
//
// xskinMeshHeader -> 'XSkinMeshHeader' '{' int ; int ; int ; '}'
//
// skinWeights -> 'SkinWeights' '{' " string " ; int ; [int,]+ int; [f,]+ f; matrix '}'
//
// animationSet -> 'AnimationSet' '{' animation* '}'
//
// animation -> 'Animation' '{' animationKey '{' ident '}' '}'
//
// animationKey -> 'AnimationKey' '{' i; i; [ i; i; matrix,]+ i; i; matrix; '}'
//

class X : public ModelImporter
{
public:
	X();
	~X();
	X(const X&);
	const X& operator=(const X&);

	// convert parsed file to a skin
	WorldObject* ParseLoadedFile();
	// not supported - but needs to be implemented
	WorldObject* LoadBinary(std::string filename);

private:
	// data structures internal to x file parser

	class Matrix
	{
	public:
		Matrix() { for (int i=0; i<16; i++) f[i]=0; }
		float f[16];
	};

	class AnimationKeyItem
	{
	public:
		AnimationKeyItem() { time = 0; sixteen=0; }
		int		time;
		int		sixteen;
		Matrix	matrix;
	};

	// animationKey -> 'AnimationKey' '{' i; i; [ i; i; matrix,]+ i; i; matrix; '}'
	struct AnimationKey
	{
		AnimationKey()
			: keys(NULL)
		{
			four=0;
			count=0;
		}
		~AnimationKey()
		{
			safe_delete_array(keys);
		}
		AnimationKey(const AnimationKey& ak)
			: keys(NULL)
		{
			operator=(ak);
		}
		const AnimationKey& operator=(const AnimationKey& ak)
		{
			safe_delete_array(keys);

			four = ak.four;
			count = ak.count;
			if (count>0)
			{
				keys = new AnimationKeyItem[count];
				for (int i=0; i<count; i++)
				{
					keys[i] = ak.keys[i];
				}
			}
			return *this;
		}

		int					four;
		int					count;
		AnimationKeyItem*	keys;
	};

	// animation -> 'Animation' '{' animationKey '{' ident '}' '}'
	class Animation
	{
	public:
		std::string		name;
		AnimationKey	key;
	};

	// animationSet -> 'AnimationSet' '{' animation* '}'
	class AnimationSet
	{
	public:
		std::vector<Animation>	set;
	};

	// skinWeights -> 'SkinWeights' '{' " string " ; int ; [int,]+ int; [f,]+ f; matrix '}'
	class SkinWeights
	{
	public:
		SkinWeights()
			: vertexIndices(NULL)
			, weights(NULL)
		{
			count = 0;
		}
		~SkinWeights()
		{
			safe_delete_array(vertexIndices);
			safe_delete_array(weights);
		}
		SkinWeights(const SkinWeights& sw)
			: vertexIndices(NULL)
			, weights(NULL)
		{
			operator=(sw);
		}
		const SkinWeights& operator=(const SkinWeights& sw)
		{
			safe_delete_array(vertexIndices);
			safe_delete_array(weights);

			name = sw.name;
			count = sw.count;
			matrix = sw.matrix;
			if (count>0)
			{
				weights = new float[count];
				vertexIndices = new int[count];
				memcpy(weights,sw.weights,count*sizeof(float));
				memcpy(vertexIndices,sw.vertexIndices,count*sizeof(int));
			}
			return *this;
		}

		std::string	name;
		int			count;
		int*		vertexIndices;
		float*		weights;
		Matrix		matrix;
	};

	// xskinMeshHeader -> 'XSkinMeshHeader' '{' int ; int ; int ; '}'
	class XSkinMeshHeader
	{
	public:
		int maxSkinWeightsPerVertex;
		int maxSkinWeightsPerFace;
		int numBones;
	};

	// material -> 'Material' '{' f;f;f;f;;  f;  f;f;f;;  f;f;f;; [textureFilename] '}'
	// textureFilename -> 'TextureFilename' '{' " string " ; '}'
	class Material
	{
	public:
		float diff[4];
		float opacity;
		float col1[3];
		float col2[3];
		std::string textureFilename;
	};

	// meshMaterialList -> 'MeshMaterialList' '{' int ; int ; [int,]+ int; [material] '}'
	class MeshMaterialList
	{
	public:
		MeshMaterialList()
			: materials(NULL)
			, vertexMaterials(NULL)
		{
			numMaterials = 0;
			numVertices = 0;
		}
		~MeshMaterialList()
		{
			safe_delete_array(materials);
			safe_delete_array(vertexMaterials);
		}
		MeshMaterialList(const MeshMaterialList& m)
			: materials(NULL)
			, vertexMaterials(NULL)
		{
			operator=(m);
		}
		const MeshMaterialList& operator=(const MeshMaterialList& m)
		{
			safe_delete_array(materials);
			safe_delete_array(vertexMaterials);

			numMaterials = m.numMaterials;
			numVertices = m.numVertices;
			if (numMaterials>0)
			{
				materials = new Material[numMaterials];
				for (int i=0; i<numMaterials; i++)
				{
					materials[i] = m.materials[i];
				}
			}
			if (numVertices>0)
			{
				vertexMaterials = new int[numVertices];
				for (int i=0; i<numVertices; i++)
				{
					vertexMaterials[i] = m.vertexMaterials[i];
				}
			}
			return *this;
		}

		int			numMaterials;
		int			numVertices;
		int*		vertexMaterials;
		Material*	materials;
	};

	// vertexDuplicationIndices -> 'VertexDuplicationIndices' '{' int ; int ; [int,]+ int; '}'
	class VertexDuplicationIndices
	{
	public:
		VertexDuplicationIndices()
			: indices(NULL)
		{
			numIndices = 0;
			numOriginalIndices = 0;
		}
		~VertexDuplicationIndices()
		{
			safe_delete_array(indices);
		}
		VertexDuplicationIndices(const VertexDuplicationIndices& v)
			: indices(NULL)
		{
			operator=(v);
		}
		const VertexDuplicationIndices& operator=(const VertexDuplicationIndices& v)
		{
			safe_delete_array(indices);

			numOriginalIndices = v.numOriginalIndices;
			numIndices = v.numIndices;
			if (numIndices>0)
			{
				indices = new int[numIndices];
				for (int i=0; i<numIndices; i++)
				{
					indices[i] = v.indices[i];
				}
			}
			return *this;
		}
		int		numIndices;
		int		numOriginalIndices;
		int*	indices;
	};

	class UV
	{
	public:
		float u,v;
	};

	// meshTextureCoords -> 'MeshTextureCoords' '{' int ; [f;f;,]+ f;f;; '}'
	class MeshTextureCoords
	{
	public:
		MeshTextureCoords()
			: uvs(NULL)
		{
			count = 0;
		}
		~MeshTextureCoords()
		{
			safe_delete_array(uvs);
		}
		MeshTextureCoords(const MeshTextureCoords& m)
			: uvs(NULL)
		{
			operator=(m);
		}
		const MeshTextureCoords& operator=(const MeshTextureCoords& m)
		{
			safe_delete_array(uvs);

			count = m.count;
			if (count>0)
			{
				uvs = new UV[count];
				for (int i=0; i<count; i++)
				{
					uvs[i] = m.uvs[i];
				}
			}
			return *this;
		}
		int		count;
		UV*		uvs;
	};

	class Vertex
	{
	public:
		float x,y,z;
	};

	class Face
	{
	public:
		int v1,v2,v3;
	};

	// meshNormals -> 'MeshNormals' '{' int ; [f;f;f;,]+ f;f;f;; int ; [3;i,i,i;,]+ 3;i,i,i;; '}'
	class MeshNormals
	{
	public:
		MeshNormals()
			: normals(NULL)
			, faces(NULL)
		{
			numNormals = 0;
			numFaces = 0;
		}
		~MeshNormals()
		{
			safe_delete_array(normals);
			safe_delete_array(faces);
		}
		MeshNormals(const MeshNormals& m)
			: normals(NULL)
			, faces(NULL)
		{
			operator=(m);
		}
		const MeshNormals& operator=(const MeshNormals& m)
		{
			safe_delete_array(normals);
			safe_delete_array(faces);

			numNormals = m.numNormals;
			numFaces = m.numFaces;
			if (numNormals>0)
			{
				normals = new Vertex[numNormals];
				for (int i=0; i<numNormals; i++)
				{
					normals[i] = m.normals[i];
				}
			}
			if (numFaces>0)
			{
				faces = new Face[numFaces];
				for (int i=0; i<numFaces; i++)
				{
					faces[i] = m.faces[i];
				}
			}
			return *this;
		}

		int		numNormals;
		Vertex*	normals;
		int		numFaces;
		Face*	faces;
	};

	// mesh -> 'XMesh' [indent] '{' int ; [f;f;f;,]+ f;f;f;; int ; [3;i,i,i;,]+ 3;i,i,i;;
	//			[meshNormals] [meshTextureCoords] [vertexDuplicationIndices]
	//			[meshMaterialList] [xskinMeshHeader] [skinWeights]* '}'
	class XMesh
	{
	public:
		XMesh()
			: vertices(NULL)
			, faces(NULL)
		{
			numVertices = 0;
			numFaces = 0;
		}
		~XMesh()
		{
			safe_delete_array(vertices);
			safe_delete_array(faces);
		}
		XMesh(const XMesh& m)
			: vertices(NULL)
			, faces(NULL)
		{
			operator=(m);
		}
		const XMesh& operator=(const XMesh& m)
		{
			safe_delete_array(vertices);
			safe_delete_array(faces);

			normals = m.normals;
			textureCoords = m.textureCoords;
			duplicationIndices = m.duplicationIndices;
			materialList = m.materialList;
			xskinMeshHeader = m.xskinMeshHeader;
			skinWeights = m.skinWeights;

			numVertices = m.numVertices;
			numFaces = m.numFaces;
			if (numVertices>0)
			{
				vertices = new Vertex[numVertices];
				for (int i=0; i<numVertices; i++)
				{
					vertices[i] = m.vertices[i];
				}
			}
			if (numFaces>0)
			{
				faces = new Face[numFaces];
				for (int i=0; i<numFaces; i++)
				{
					faces[i] = m.faces[i];
				}
			}
			return *this;
		}

		std::string		name;
		int				numVertices;
		Vertex*			vertices;
		int				numFaces;
		Face*			faces;

		std::vector<MeshNormals>				normals;
		std::vector<MeshTextureCoords>			textureCoords;
		std::vector<VertexDuplicationIndices>	duplicationIndices;
		std::vector<MeshMaterialList>			materialList;
		std::vector<XSkinMeshHeader>			xskinMeshHeader;
		std::vector<SkinWeights>				skinWeights;
	};

	// frame -> 'Frame' [ident|e] '{' 'FrameTransformMatrix' '{' matrix '}' [frame|mesh] '}'
	class Frame
	{
	public:
		std::string			name;
		Matrix				frameTransformationMatrix;
		std::vector<Frame>	frames;
		std::vector<XMesh>	meshes;
	};

	// file -> 'xof 0303txt 0032' [?]* frame+ animationSet+
	class File
	{
	public:
		std::vector<Frame>			frames;
		std::vector<AnimationSet>	animations;
	};

private:
	void ParseFile();

	Frame ParseFrame();

	Matrix ParseMatrix();

	XMesh ParseMesh();

	MeshNormals ParseMeshNormals();

	MeshTextureCoords ParseMeshTextureCoords();

	VertexDuplicationIndices ParseVertexDuplicationIndices();

	MeshMaterialList ParseMeshMaterialList();

	Material ParseMaterial();

	XSkinMeshHeader ParseXSkinMeshHeader();

	SkinWeights ParseSkinWeights();

	AnimationSet ParseAnimationSet();

	Animation ParseAnimation();

	AnimationKey ParseAnimationKey();

private:
	// count bones in hierarchy
	int CountBones(Bone* bone);

	// get the skeleton - top level
	Bone* GetSkeleton();

	// get the mesh
	XMesh* GetMesh(Frame& f);

	// get a skeleton from the frames
	Bone* FramesToSkeleton(Frame& f);

	// take the information from a mesh and put it into the skin
	Mesh* ProcessMesh(XMesh* mesh, std::string& textureFilename);

	D3DXMATRIXA16 ConvertMatrix(Matrix& m);

	// recursively find a bone by name
	Bone* GetBoneByName(Bone* root, const std::string& name);

	// process all animations
	void ProcessAnimations(Bone* root);

	// process all skin weights
	void ProcessSkinWeights(Skin* skin, XMesh* mesh);

private:
	// parsed result of file
	File	file;
};
