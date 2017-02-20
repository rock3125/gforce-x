#pragma once

#include "d3d9/InvalidatableDevice.h"
#include "system/model/boundingBox.h"

/////////////////////////////////////////////////////////////////////////////

class Mesh : public InvalidatableDevice
{
public:
	Mesh();
	virtual ~Mesh();
	Mesh(const Mesh&);
	const Mesh& operator=(const Mesh&);

	void ReplaceVertices(D3DPVERTEX* verts, unsigned numVerts);
	void ReplaceIndices(D3DPINDEX* inds, unsigned numTris);

	// permanently apply a transformation to the vertices
	// of this mesh
	void ApplyTransformation(const D3DXMATRIXA16& transformation);

	// set get name
	void SetName(const std::string& name);
	const std::string& GetName() const;

	D3DPVERTEX* GetVertices();
	void SetVertices(int count,D3DPVERTEX* vertices);

	D3DPINDEX* GetIndices();
	void SetIndices(int count,D3DPINDEX* indices);

	unsigned int GetNumVertices();
	void SetNumVertices(unsigned int n);

	unsigned int GetNumTriangles();

	// set usage for skin types
	void UseForSkinning();

	// lock the vertex buffer of a mesh and return the bytes
	bool LockVertexBuffer(void** bytes);

	// unlock the vertex buffer of a mesh
	void UnlockVertexBuffer();

	// create d3d buffers regardless of their existence
	void RestoreDeviceObjects();

	// release d3d buffers
	void InvalidateDeviceObjects();

	// free all allocated memory by this mesh
	void Free();

	// update bounding box volume
	void GetBoundingBox(D3DXVECTOR3& aabbMin,D3DXVECTOR3& aabbMax);

	// optimise the mesh internally - throw away redundant verts
	void OptimiseMesh();

	void Write(BaseStreamer& f);
	void Read(BaseStreamer& f);

protected:
	friend class Device;

	// only very special routines should call these directly
	IDirect3DVertexBuffer9* GetVertexBuffer();
	IDirect3DIndexBuffer9* GetIndexBuffer();
	int GetVertexSize();

private:
	// Device dependent stuff
	IDirect3DVertexBuffer9* vertexBuffer;

	// d3d: the faces
	IDirect3DIndexBuffer9* indexBuffer;

private:
	static void SetVertex(D3DPVERTEX* v,int index,
						  const D3DXVECTOR3& position,
						  const D3DXVECTOR3& normal,
						  const D3DXVECTOR2& texture);
	static void SetIndex(D3DPINDEX* i,int index,WORD i1,WORD i2,WORD i3);
	static D3DXVECTOR2 SphericalUV(const D3DXVECTOR3& pos);
	static void FixSphericalMesh(Mesh* m);

protected:
	friend class VRMLParser;

	std::string				name;

	//index data
	int						numVertices;
	int						numTriangles;

	D3DPVERTEX*				vertices;
	D3DPINDEX*				indices;

	DWORD					usage;

	static std::string		meshSignature;
	static int				meshVersion;
};

