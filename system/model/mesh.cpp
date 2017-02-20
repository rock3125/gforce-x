#include "standardFirst.h"

#include "system/model/mesh.h"
#include "d3d9/interface.h"

std::string	Mesh::meshSignature="mesh";
int Mesh::meshVersion=1;

/////////////////////////////////////////////////////////////////////////////

Mesh::Mesh()
	: vertices(NULL)
	, indices(NULL)
	, indexBuffer(NULL)
	, vertexBuffer(NULL)
	, usage(D3DUSAGE_WRITEONLY)
{
	Free();
}

Mesh::~Mesh()
{
	Free();
}

Mesh::Mesh(const Mesh& t)
	: vertices(NULL)
	, indices(NULL)
	, indexBuffer(NULL)
	, vertexBuffer(NULL)
	, usage(D3DUSAGE_WRITEONLY)
{
	operator=(t);
}

const Mesh& Mesh::operator=(const Mesh& t)
{
	InvalidateDeviceObjects();

	safe_delete_array(indices);
	safe_delete_array(vertices);

	name=t.name;
	numVertices=t.numVertices;
	numTriangles=t.numTriangles;

	// allocate memory for vertices and indices
	numVertices=t.numVertices;
	if (numVertices>0)
	{
		vertices=new D3DPVERTEX[numVertices];
		memcpy(vertices,t.vertices,sizeof(D3DPVERTEX)*numVertices);
	}

	numTriangles=t.numTriangles;
	if (numTriangles>0)
	{
		indices=new D3DPINDEX[numTriangles];
		memcpy(indices,t.indices,sizeof(D3DPINDEX)*numTriangles);
	}

	if (numVertices>0 && numTriangles>0)
	{
		RestoreDeviceObjects();
	}

	return *this;
}

D3DPVERTEX* Mesh::GetVertices()
{
	return vertices;
}

void Mesh::SetVertices(int count,D3DPVERTEX* _vertices)
{
	numVertices = count;
	safe_delete(vertices);
	if (numVertices>0)
	{
		vertices = new D3DPVERTEX[numVertices];
		memcpy(vertices,_vertices,sizeof(D3DPVERTEX)*numVertices);
	}
}

void Mesh::OptimiseMesh()
{
	if (numVertices > 0 && numTriangles > 0)
	{
		bool* inUse = new bool[numVertices];
		for (int i=0; i < numVertices; i++)
		{
			inUse[i] = false;
		}

		// for each triangle - set the used vertices
		for (int i=0; i < numTriangles; i++)
		{
			if (indices[i].v1 < 0 || indices[i].v1 >= numVertices ||
				indices[i].v2 < 0 || indices[i].v2 >= numVertices ||
				indices[i].v3 < 0 || indices[i].v3 >= numVertices)
			{
				throw new Exception("index out of range (<0 or larger than numVertices");
			}

			inUse[ indices[i].v1 ] = true;
			inUse[ indices[i].v2 ] = true;
			inUse[ indices[i].v3 ] = true;
		}

		// for each triangle entry not used - decrease all triangle
		// indexes by one above it
		int newCount = 0;
		for (int i=0; i < numVertices; i++)
		{
			if (!inUse[i])
			{
				// decrease indices to account for this vertex disappearing
				for (int j=0; j < numTriangles; j++)
				{
					if (indices[j].v1 > i)
					{
						indices[j].v1--;
					}
					if (indices[j].v2 > i)
					{
						indices[j].v2--;
					}
					if (indices[j].v3 > i)
					{
						indices[j].v3--;
					}
				}
			}
			else
			{
				newCount++;
			}
		}

		// create a new vertex array without the unused vertices
		D3DPVERTEX* newVertices = new D3DPVERTEX[newCount];
		int newIndex = 0;
		for (int i=0; i < numVertices; i++)
		{
			if (inUse[i])
			{
				newVertices[newIndex++] = vertices[i];
			}
		}

		// remove check memory
		safe_delete_array(inUse);
		safe_delete_array(vertices);

		// setup new count and array
		vertices = newVertices;
		numVertices = newIndex;

		// activate the new vertices
		RestoreDeviceObjects();
	}
}

void Mesh::SetName(const std::string& _name)
{
	name = _name;
}

const std::string& Mesh::GetName() const
{
	return name;
}

void Mesh::UseForSkinning()
{
	usage = D3DUSAGE_DYNAMIC | D3DLOCK_NOSYSLOCK;
}

void Mesh::UnlockVertexBuffer()
{
	if (vertexBuffer != NULL)
	{
		vertexBuffer->Unlock();
	}
}

bool Mesh::LockVertexBuffer(void** bytes)
{
	if (vertexBuffer != NULL)
	{
		HRESULT hr = vertexBuffer->Lock(0,0,bytes,D3DLOCK_NOSYSLOCK);
		return SUCCEEDED(hr);
	}
	else
	{
		bytes = NULL;
		return false;
	}
}

D3DPINDEX* Mesh::GetIndices()
{
	return indices;
}

void Mesh::SetIndices(int count,D3DPINDEX* _indices)
{
	numTriangles = count;
	safe_delete(indices);
	if (numTriangles>0)
	{
		indices = new D3DPINDEX[numTriangles];
		memcpy(indices,_indices,sizeof(D3DPINDEX)*numTriangles);
	}
}

unsigned int Mesh::GetNumVertices()
{
	return numVertices;
}

void Mesh::SetNumVertices(unsigned int n)
{
	numVertices = n;
}

unsigned int Mesh::GetNumTriangles()
{
	return numTriangles;
}

IDirect3DVertexBuffer9* Mesh::GetVertexBuffer()
{
	return vertexBuffer;
}

IDirect3DIndexBuffer9* Mesh::GetIndexBuffer()
{
	return indexBuffer;
}

void Mesh::ReplaceVertices(D3DPVERTEX* verts, unsigned numVerts)
{
	safe_delete_array(vertices);
	vertices = verts;
	numVertices = numVerts;
}

void Mesh::ReplaceIndices(D3DPINDEX* inds, unsigned numTris)
{
	safe_delete_array(indices);
	indices = inds;
	numTriangles = numTris;
}

void Mesh::Free()
{
	InvalidateDeviceObjects();

	numVertices=0;
	numTriangles=0;

	safe_delete_array(vertices);
	safe_delete_array(indices);
}

void Mesh::InvalidateDeviceObjects()
{
	safe_release(vertexBuffer);
	safe_release(indexBuffer);
}

void Mesh::ApplyTransformation(const D3DXMATRIXA16& transformation)
{
	if (vertices!=NULL && numVertices>0)
	{
		// apply transformation
		for (int i=0; i<numVertices; i++)
		{
			D3DPVERTEX* v = &vertices[i];
			D3DXVec3TransformCoord(&v->position,&v->position,&transformation);
			D3DXVec3TransformCoord(&v->normal,&v->normal,&transformation);
		}
		RestoreDeviceObjects();
	}
}

void Mesh::RestoreDeviceObjects()
{
	InvalidateDeviceObjects();

	Device* dev=Interface::GetDevice();

	D3DPVERTEX*	vertsArray=NULL;	// pointer to vertex array to be built from
	D3DPINDEX*	indicesArray=NULL;	// pointer to index array to be built from

	// use default buffers if not shadowed
	vertsArray = vertices;
	indicesArray = indices;

	// release the vertex buffer
	safe_release(vertexBuffer);

	// index buffer
	safe_release(indexBuffer);

	if (numVertices >0 && numTriangles > 0)
	{
		D3DPOOL pool = D3DPOOL_MANAGED; //D3DPOOL_DEFAULT;
		int size = sizeof(D3DPVERTEX)*numVertices;
		vertexBuffer = dev->CreateVertexBuffer(size,usage,D3DPFVF,pool);
		if (vertexBuffer==NULL)
			return;

		void* buffer=NULL;
		if (!SUCCEEDED(vertexBuffer->Lock(0,0,&buffer,0)))
			return;

		// copy in verts
		memcpy(buffer,(void*)vertsArray,size);
		vertexBuffer->Unlock();

		size = numTriangles*sizeof(D3DPINDEX);
		indexBuffer = dev->CreateIndexBuffer(size,usage,D3DFMT_INDEX16,D3DPOOL_DEFAULT);
		if (indexBuffer==NULL)
			return;

		void* indexBufferPtr = NULL;
		if (!SUCCEEDED(indexBuffer->Lock(0,0,&indexBufferPtr,0)))
			return;

		// copy in indices
		memcpy(indexBufferPtr,(void*)indicesArray,size);
		indexBuffer->Unlock();
	}
}

void Mesh::GetBoundingBox(D3DXVECTOR3& aabbMin,D3DXVECTOR3& aabbMax)
{
	D3DXVECTOR3 minp = D3DXVECTOR3(0,0,0);
	D3DXVECTOR3 maxp = D3DXVECTOR3(0,0,0);
	if (numVertices>0)
	{
		minp.x=vertices[0].position.x;
		minp.y=vertices[0].position.y;
		minp.z=vertices[0].position.z;
		maxp.x=vertices[0].position.x;
		maxp.y=vertices[0].position.y;
		maxp.z=vertices[0].position.z;

		for (int j=0; j<numVertices; j++)
		{
			minp.x=min(vertices[j].position.x,minp.x);
			minp.y=min(vertices[j].position.y,minp.y);
			minp.z=min(vertices[j].position.z,minp.z);

			maxp.x=max(vertices[j].position.x,maxp.x);
			maxp.y=max(vertices[j].position.y,maxp.y);
			maxp.z=max(vertices[j].position.z,maxp.z);
		}
	}
	aabbMin=minp;
	aabbMax=maxp;
}

void Mesh::SetVertex(D3DPVERTEX* v,int index,
					 const D3DXVECTOR3& position,
					 const D3DXVECTOR3& normal,
					 const D3DXVECTOR2& texture)
{
	v[index].position = position;
	v[index].normal = normal;
	v[index].texture = texture;
}

void Mesh::SetIndex(D3DPINDEX* i,int index,WORD i1,WORD i2,WORD i3)
{
	i[index].v1=i1;
	i[index].v2=i2;
	i[index].v3=i3;
}

XmlNode* Mesh::Write()
{
	XmlNode* node = XmlNode::NewChild(meshSignature, meshVersion);

	node->Write("name", name);
	node->Write("numVertices", numVertices);
	node->Write("numTriangles", numTriangles);

	if (numVertices>0)
	{
		XmlNode* node2 = XmlNode::NewChild("vertices", 1);
		for (int i=0; i<numVertices; i++)
		{
			std::string tag = "v";
			XmlNode* newNode = XmlNode::NewChild(tag, 1);
			newNode->Write(tag, vertices[i]);
			node2->Add(newNode);
		}
		node->Add(node2);
	}

	if (numTriangles>0)
	{
		XmlNode* node2 = XmlNode::NewChild("indices", 1);
		for (int i=0; i<numTriangles; i++)
		{
			std::string tag = "t";
			XmlNode* newNode = XmlNode::NewChild(tag, 1);
			newNode->Write(tag, indices[i]);
			node2->Add(newNode);
		}
		node->Add(node2);
	}
	return node;
}

void Mesh::Read(XmlNode* node)
{
	XmlNode::CheckVersion(node, meshSignature, meshVersion);

	Free();

	node->Read("name",name);
	node->Read("numVertices",numVertices);
	node->Read("numTriangles",numTriangles);

	if (numVertices>0)
	{
		XmlNode* node2 = node->GetChild("vertices");
		XmlNode::CheckVersion(node2, "vertices", 1);

		vertices = new D3DPVERTEX[numVertices];
		std::vector<XmlNode*> verts = node2->GetChildren();
		if (verts.size() != numVertices)
		{
			throw new Exception("num vertices does not match actual vertices");
		}
		int cntr = 0;
		std::vector<XmlNode*>::iterator pos = verts.begin();
		while (pos != verts.end())
		{
			XmlNode* n = *pos;
			n->Read("v", vertices[cntr++]);
			pos++;
		}
	}

	if (numTriangles>0)
	{
		XmlNode* node2 = node->GetChild("indices");
		XmlNode::CheckVersion(node2, "indices", 1);

		indices = new D3DPINDEX[numTriangles];
		std::vector<XmlNode*> inds = node2->GetChildren();
		if (inds.size() != numTriangles)
		{
			throw new Exception("num triangles does not match actual triangles");
		}
		int cntr = 0;
		std::vector<XmlNode*>::iterator pos = inds.begin();
		while (pos != inds.end())
		{
			XmlNode* n = *pos;
			n->Read("t", indices[cntr++]);
			pos++;
		}
	}

	RestoreDeviceObjects();
}

