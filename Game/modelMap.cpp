//////////////////////////////////////////////////////////////////////
/// GRAVITY FORCE X
///
/// (c)Copyright Peter de Vocht, 2006
///
/// I'm releasing this code under GPL v2.1 (see included file lgpl.txt)
///
//////////////////////////////////////////////////////////////////////

#include "standardFirst.h"

#include "game/modelMap.h"
#include "game/indestructableRegion.h"

#include "system/model/mesh.h"
#include "system/model/camera.h"
#include "system/model/light.h"
#include "system/BaseApp.h"
#include "runtime/runtime.h"

#include "d3d9/texture.h"

std::string ModelMap::modelMapSignature="modelMap";
int ModelMap::modelMapVersion=1;

//////////////////////////////////////////////////////////

ModelMap::ModelMap()
	: map(NULL)
	, texture(NULL)
	, fgTexture(NULL)
	, indices(NULL)
	, vertices(NULL)
	, highIndices(NULL)
	, highVertices(NULL)
	, sideIndices(NULL)
	, sideVertices(NULL)
{
	SetType(OT_MODELMAP);
	SetWorldType(TYPE_MODELMAP);

	scale = 4;
	heightScale = 0; // 0.01f;
	textureScale = 0.01f;

	depth = 10;
	sizeX = 0;
	sizeY = 0;

	// numHits required for a high square to disappear
	numHits = 5;

	// gravity for this map
	gravity = 0.005f;
	sideColourTop = D3DXCOLOR(0.5f,0.5f,0.5f,1);
	sideColourBottom = D3DXCOLOR(0,0,0,1);
}

ModelMap::~ModelMap()
{
	safe_delete_array(map);
	safe_delete_array(indices);
	safe_delete_array(vertices);
	safe_delete_array(highIndices);
	safe_delete_array(highVertices);
	safe_delete_array(sideIndices);
	safe_delete_array(sideVertices);
	texture = NULL;
	fgTexture = NULL;
}

ModelMap::ModelMap(const ModelMap& mm)
	: map(NULL)
	, texture(NULL)
	, fgTexture(NULL)
	, indices(NULL)
	, vertices(NULL)
	, highIndices(NULL)
	, highVertices(NULL)
	, sideIndices(NULL)
	, sideVertices(NULL)
{
	operator=(mm);
}

const ModelMap& ModelMap::operator=(const ModelMap& mm)
{
	scale = mm.scale;
	gravity = mm.gravity;
	heightScale = mm.heightScale;
	textureScale = mm.textureScale;
	depth = mm.depth;
	sizeX = mm.sizeX;
	sizeY = mm.sizeY;
	filename = mm.filename;
	textureFilename = mm.textureFilename;
	texture = mm.texture;
	fgTexture = mm.fgTexture;
	sideColourTop = mm.sideColourTop;
	sideColourBottom = mm.sideColourBottom;
	
	Load();
	return *this;
}

D3DXCOLOR ModelMap::GetSideColourTop()
{
	return sideColourTop;
}

void ModelMap::SetSideColourTop(D3DXCOLOR _sideColourTop)
{
	sideColourTop = _sideColourTop;
}

D3DXCOLOR ModelMap::GetSideColourBottom()
{
	return sideColourBottom;
}

void ModelMap::SetSideColourBottom(D3DXCOLOR _sideColourBottom)
{
	sideColourBottom = _sideColourBottom;
}

float ModelMap::GetGravity()
{
	return gravity;
}

void ModelMap::SetGravity(float _gravity)
{
	gravity = _gravity;
}

void ModelMap::Set(D3DPVERTEX* v, int index, float x, float y, float z,float tx,float ty)
{
	v[index].position.x = x;
	v[index].position.y = y;
	v[index].position.z = z;
	v[index].normal.x = 0;
	v[index].normal.y = 0;
	v[index].normal.z = 1;
	v[index].texture.x = tx;
	v[index].texture.y = ty;
}

D3DPVERTEX ModelMap::Convert(int xp, int yp, byte c)
{
	float x,y,z,tx,ty;
	D3DPVERTEX v;

	x = (float)xp * scale;
	y = (float)yp * scale;
	z = (float)c;

	// threshold scale at 128
	bool useDeepColour = true;
	if (z < 128)
	{
		z = depth - (z * heightScale);
	}
	else
	{
		useDeepColour = false;
		z = -(z - 128) * heightScale;
	}

	tx = fabsf((float)x * textureScale);
	ty = fabsf((float)y * textureScale);

	// remove int component
	tx = (float)((int)(tx * 10000) - (((int)tx) * 10000));
	ty = (float)((int)(ty * 10000) - (((int)ty) * 10000));
	tx *= 0.0001f;
	ty *= 0.0001f;

	Set(&v,0,x,y,z,tx,ty);

	return v;
}

int ModelMap::AlignUV(D3DPVERTEX* v1, D3DPVERTEX* v2, D3DPVERTEX* v3)
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

void ModelMap::Load()
{
	std::string filenameStr = System::RemoveDataDirectory(filename);
	std::string textureFilenameStr = System::RemoveDataDirectory(textureFilename);
	std::string fgTextureFilenameStr = System::RemoveDataDirectory(fgTextureFilename);

	safe_delete_array(map);
	safe_delete_array(indices);
	safe_delete_array(vertices);
	safe_delete_array(highIndices);
	safe_delete_array(highVertices);
	safe_delete_array(sideIndices);
	safe_delete_array(sideVertices);

	Texture* mapTexture = TextureCache::GetTexture(filenameStr);
	D3DLOCKED_RECT lr = mapTexture->LockReadOnly();
	int pitch = lr.Pitch;
	byte* bits = (byte*)lr.pBits;

	sizeX = mapTexture->GetWidth();
	sizeY = mapTexture->GetHeight();
	int size = sizeX * sizeY;

	// screen indices - lower ground and high set
	// for different texture

	// maximum allowed resolution = 1600 x 1200
	int width = 200;
	int height = 150;

	// 3 indices per triangle, 2 triangles per square
	indices = new WORD[width*height*6];
	highIndices = new WORD[width*height*6];
	// 4 indices per side, 3 sides per triangle, 2 triangles per square
	sideIndices = new WORD[width*height*24];

	// vertices - 3 per triangle, 2 triangles per square
	vertices = new D3DPVERTEX[width*height*6];
	highVertices = new D3DPVERTEX[width*height*6];

	// 4 tiangles per side, 3 sides per triangle, 2 triangles
	sideVertices = new D3DPCOLOURONLYVERTEX[width*height*24];

	// each vertex creates 2 new triangle heights
	map = new Square[size];

	// re-setup background and foreground textures
	if (!textureFilenameStr.empty())
		texture = TextureCache::GetTexture(textureFilenameStr);
	if (!fgTextureFilenameStr.empty())
		fgTexture = TextureCache::GetTexture(fgTextureFilenameStr);

	// setup a map where each pixel forms a set of 2 triangles making a square
	// align the UVs too
	int s = 1; // (int)scale;
	D3DPVERTEX v1,v2,v3;
	for (int y=0; y < sizeY; y++)
	{
		for (int x=0; x < sizeX; x++)
		{
			byte c = bits[x*4 + (sizeY-(y+1))*pitch + 0];

			v1 = Convert(x+0,y+0,c);
			v2 = Convert(x+s,y+s,c);
			v3 = Convert(x+0,y+s,c);
			AlignUV(&v1,&v2,&v3);
			map[y*sizeX + x].triangle1.v1 = v1;
			map[y*sizeX + x].triangle1.v2 = v2;
			map[y*sizeX + x].triangle1.v3 = v3;

			v1 = Convert(x+s,y+s,c);
			v2 = Convert(x+0,y+0,c);
			v3 = Convert(x+s,y+0,c);
			AlignUV(&v1,&v2,&v3);
			map[y*sizeX + x].triangle2.v1 = v1;
			map[y*sizeX + x].triangle2.v2 = v2;
			map[y*sizeX + x].triangle2.v3 = v3;
		}
	}
	mapTexture->Unlock();

	UpdateBoundingBox();
}

void ModelMap::Update()
{
	if (!GetMapFilename().empty())
	{
		Load();
	}
}

const std::string& ModelMap::GetMapFilename()
{
	return filename;
}

void ModelMap::SetMapFilename(const std::string& _filename)
{
	filename = _filename;
}

const std::string& ModelMap::GetBackgroundTextureFilename()
{
	return textureFilename;
}

void ModelMap::SetBackgroundTextureFilename(const std::string& _textureFilename)
{
	textureFilename = _textureFilename;
}

const std::string& ModelMap::GetForegroundTextureFilename()
{
	return fgTextureFilename;
}

void ModelMap::SetForegroundTextureFilename(const std::string& _fgTextureFilename)
{
	fgTextureFilename = _fgTextureFilename;
}

bool ModelMap::Collides(const D3DPVERTEX& pos1, const D3DXVECTOR3& pos2)
{
	float dx = pos1.position.x - pos2.x;
	float dy = pos1.position.y - pos2.y;
	float d = dx * dx + dy * dy;
	if (d <= (scale * scale) && pos1.position.z < depth)
	{
		return true;
	}
	return false;
}

void ModelMap::FlattenSquare(int index, int mapx, int mapy)
{
	// make sure it can be flattened
	D3DXVECTOR3 pos = map[mapy * sizeX + mapx].triangle1.v1.position;
	std::vector<IndestructableRegion*> irs = BaseApp::Get()->GetCurrentRuntime()->GetIndestructableRegions();
	for (int i=0; i < irs.size(); i++)
	{
		if (irs[i]->Inside(pos))
			return;
	}

	if (index == 0)
	{
		float d = depth / (float)numHits;
		map[mapy * sizeX + mapx].triangle1.v1.position.z += d;
		map[mapy * sizeX + mapx].triangle1.v2.position.z += d;
		map[mapy * sizeX + mapx].triangle1.v3.position.z += d;
	}
	else
	{
		float d = depth / (float)numHits;
		map[mapy * sizeX + mapx].triangle2.v1.position.z += d;
		map[mapy * sizeX + mapx].triangle2.v2.position.z += d;
		map[mapy * sizeX + mapx].triangle2.v3.position.z += d;
	}
}

void ModelMap::FlattenSquare(int index, int mapx, int mapy, float amount)
{
	// make sure it can be flattened
	D3DXVECTOR3 pos = map[mapy * sizeX + mapx].triangle1.v1.position;
	std::vector<IndestructableRegion*> irs = BaseApp::Get()->GetCurrentRuntime()->GetIndestructableRegions();
	for (int i=0; i < irs.size(); i++)
	{
		if (irs[i]->Inside(pos))
			return;
	}

	map[mapy * sizeX + mapx].triangle1.v1.position.z += amount;
	if (map[mapy * sizeX + mapx].triangle1.v1.position.z > depth)
		map[mapy * sizeX + mapx].triangle1.v1.position.z = depth;
	map[mapy * sizeX + mapx].triangle1.v2.position.z += amount;
	if (map[mapy * sizeX + mapx].triangle1.v2.position.z > depth)
		map[mapy * sizeX + mapx].triangle1.v2.position.z = depth;
	map[mapy * sizeX + mapx].triangle1.v3.position.z += amount;
	if (map[mapy * sizeX + mapx].triangle1.v3.position.z > depth)
		map[mapy * sizeX + mapx].triangle1.v3.position.z = depth;
	map[mapy * sizeX + mapx].triangle2.v1.position.z += amount;
	if (map[mapy * sizeX + mapx].triangle2.v1.position.z > depth)
		map[mapy * sizeX + mapx].triangle2.v1.position.z = depth;
	map[mapy * sizeX + mapx].triangle2.v2.position.z += amount;
	if (map[mapy * sizeX + mapx].triangle2.v2.position.z > depth)
		map[mapy * sizeX + mapx].triangle2.v2.position.z = depth;
	map[mapy * sizeX + mapx].triangle2.v3.position.z += amount;
	if (map[mapy * sizeX + mapx].triangle2.v3.position.z > depth)
		map[mapy * sizeX + mapx].triangle2.v3.position.z = depth;
}

void ModelMap::CalculateMissileImpact(D3DXVECTOR3 pos, float range)
{
	Device* dev = Interface::GetDevice();

	int w = dev->GetWidth();
	int h = dev->GetHeight();

	pos.x -= w / scale;
	pos.y -= h / scale;

	int width = w / ((int)scale*2);
	int height = h / ((int)scale*2);

	D3DXVECTOR3 p1, p2, p3, p4;
	p1 = System::OffsetVector(pos,180,range);
	p2 = System::OffsetVector(pos,0,range);
	p3 = System::OffsetVector(pos,-90,range);
	p4 = System::OffsetVector(pos,90,range);

	int mapx = (int)(pos.x / scale) + (width/2);
	int mapy = (int)(pos.y / scale) + (height/2);

	int mapTop = (int)(p1.y / scale) + (height/2);
	int mapBottom = (int)(p2.y / scale) + (height/2);
	int mapLeft = (int)(p4.x / scale) + (width/2);
	int mapRight = (int)(p3.x / scale) + (width/2);

	// divide range by scale
	range = range / scale;

	for (int y=mapTop; y <= mapBottom; y++)
	{
		for (int x=mapLeft; x <= mapRight; x++)
		{
			if (x > 0 && x < sizeX && y > 0 && y < sizeY)
			{
				int dx = mapx - x;
				int dy = mapy - y;
				float dist = sqrtf((float)(dx * dx + dy * dy));
				// inside the circle?
				if (dist < range)
				{
					// calculate how many this tile needs to drop by
					dist = 1 - (dist / range);
					float drop = dist * depth * 2;
					FlattenSquare(0,x,y,drop);
				}
			}
		}
	}
}

bool ModelMap::CheckBulletCollision(D3DXVECTOR3 pos)
{
	Device* dev = Interface::GetDevice();
	Runtime* rt = BaseApp::Get()->GetCurrentRuntime();
	if (rt != NULL)
	{
		int w = dev->GetWidth();
		int h = dev->GetHeight();

		pos.x -= w / scale;
		pos.y -= h / scale;

		int width = w / ((int)scale*2);
		int height = h / ((int)scale*2);

		int mapx = (int)(pos.x / scale) + (width/2);
		int mapy = (int)(pos.y / scale) + (height/2);

		pos.x += width * 2.0f;
		pos.y += height * 2.0f;

		if (mapx > 0 && mapx < sizeX && mapy > 0 && mapy < sizeY)
		{
			// t1 = 1,3,2
			// t2 = 3,1,4

			// get the appropriate square
			Square* sq = &map[mapy * sizeX + mapx];
			if (Collides(sq->triangle1.v1, pos) ||
				Collides(sq->triangle1.v2, pos) ||
				Collides(sq->triangle1.v3, pos) ||
				Collides(sq->triangle2.v1, pos) ||
				Collides(sq->triangle2.v2, pos) ||
				Collides(sq->triangle2.v3, pos))
			{
				FlattenSquare(0,mapx,mapy);
				FlattenSquare(1,mapx,mapy);
				return true;
			}
		}
	}
	return false;
}

void ModelMap::AddSquare(float height, const D3DXVECTOR3& v1, const D3DXVECTOR3& v2,
						 int& i, WORD* index, int& v, D3DPCOLOURONLYVERTEX* verts)
{
	verts[v+0].position = v1;
	verts[v+0].colour = sideColourTop;
	verts[v+1].position = v2;
	verts[v+1].colour = sideColourTop;
	verts[v+2].position = v1;
	verts[v+2].colour = sideColourBottom;
	verts[v+3].position = v2;
	verts[v+3].colour = sideColourBottom;
	verts[v+2].position.z = height;
	verts[v+3].position.z = height;

	index[i+0] = v + 0;
	index[i+1] = v + 1;
	index[i+2] = v + 2;
	index[i+3] = v + 3;
	index[i+4] = v + 2;
	index[i+5] = v + 1;
	v += 4;
	i += 6;
}

void ModelMap::Draw(D3DXVECTOR3 targetPos, int splitScreenType)
{
	Device* dev = Interface::GetDevice();
	int w = dev->GetWidth();
	int h = dev->GetHeight();

	// offset position by screen size
	targetPos.x -= w / scale;
	targetPos.y -= h / scale;

	// widyhth and height converted to indexes
	int width = w / ((int)scale*2);
	int height = h / ((int)scale*2);

	// my coordinates converted to map absolute indexes
	int offsetX = (int)((targetPos.x) / scale);
	int offsetY = (int)((targetPos.y) / scale);

	if (map != NULL)
	{
		// adjust world matrix so drawn mesh is centered on screen
		D3DXMATRIX	worldTransform;
		worldTransform = GetWorldTransform();

		// offset map so its offset coordinates are visible
		worldTransform._41 = -targetPos.x + (-(width/2) * scale);
		worldTransform._42 = -targetPos.y + (-(height/2) * scale);
		dev->SetWorldTransform(&worldTransform);

		// draw / setup
		// indexes for various map components
		int vindex = 0;
		int iindex = 0;
		int highVindex = 0;
		int highIindex = 0;
		int sideVindex = 0;
		int sideIindex = 0;

		// width and height are determined by split screen mode
		int startWidth = 0;
		int endWidth = width;
		int startHeight = 0;
		int endHeight = height;
		if (splitScreenType == Runtime::SC_HORIZONTAL)
		{
			startHeight = height / 4;
			endHeight = startHeight + height / 2;
		}
		else if (splitScreenType == Runtime::SC_VERTICAL)
		{
			startWidth = width / 4;
			endWidth = startWidth + width / 2;
		}

		startHeight	-= 8;
		endHeight	+= 8;
		startWidth	-= 8;
		endWidth	+= 8;

		// copy bottom and top parts of the map based on depth
		// and create a skirting between the two
		for (int y=startHeight; y < endHeight; y++)
		{
			int yoff = offsetY + y;
			if (yoff > 1 && yoff < (sizeY-1))
			{
				yoff = yoff * sizeX;
				for (int x=startWidth; x < endWidth; x++)
				{
					int xoff = (offsetX+x);
					if (xoff > 1 && xoff < (sizeX-1))
					{
						// up or down?
						if (map[yoff+xoff].triangle1.v1.position.z == depth)
						{
							// add both triangles to the square
							indices[iindex] = iindex++;
							indices[iindex] = iindex++;
							indices[iindex] = iindex++;
							vertices[vindex++] = map[yoff+xoff].triangle1.v3;
							vertices[vindex++] = map[yoff+xoff].triangle1.v2;
							vertices[vindex++] = map[yoff+xoff].triangle1.v1;

							indices[iindex] = iindex++;
							indices[iindex] = iindex++;
							indices[iindex] = iindex++;
							vertices[vindex++] = map[yoff+xoff].triangle2.v3;
							vertices[vindex++] = map[yoff+xoff].triangle2.v2;
							vertices[vindex++] = map[yoff+xoff].triangle2.v1;
						}
						else
						{
							// add raised squares to the map
							highIndices[highIindex] = highIindex++;
							highIndices[highIindex] = highIindex++;
							highIndices[highIindex] = highIindex++;
							highVertices[highVindex++] = map[yoff+xoff].triangle1.v3;
							highVertices[highVindex++] = map[yoff+xoff].triangle1.v2;
							highVertices[highVindex++] = map[yoff+xoff].triangle1.v1;

							highIndices[highIindex] = highIindex++;
							highIndices[highIindex] = highIindex++;
							highIndices[highIindex] = highIindex++;
							highVertices[highVindex++] = map[yoff+xoff].triangle2.v3;
							highVertices[highVindex++] = map[yoff+xoff].triangle2.v2;
							highVertices[highVindex++] = map[yoff+xoff].triangle2.v1;

							float height = map[yoff+xoff].triangle1.v1.position.z;
							float hx1 = map[yoff+xoff-1].triangle1.v1.position.z;
							float hx2 = map[yoff+xoff+1].triangle1.v1.position.z;
							float hy1 = map[yoff+xoff-sizeY].triangle1.v1.position.z;
							float hy2 = map[yoff+xoff+sizeY].triangle1.v1.position.z;

							// add sides to the square if necessairy
							if (height < hx1) // bit of a mind twister - this tile is higher than neighbouring tile
							{
								AddSquare(depth, map[yoff+xoff].triangle1.v3.position, 
									map[yoff+xoff].triangle1.v1.position,
									sideIindex, sideIndices, sideVindex, sideVertices);
							}
							if (height < hx2)
							{
								AddSquare(depth, map[yoff+xoff].triangle2.v3.position, 
									map[yoff+xoff].triangle2.v1.position,
									sideIindex, sideIndices, sideVindex, sideVertices);
							}
							if (height < hy1)
							{
								AddSquare(depth, map[yoff+xoff].triangle2.v2.position, 
									map[yoff+xoff].triangle2.v3.position,
									sideIindex, sideIndices, sideVindex, sideVertices);
							}
							if (height < hy2)
							{
								AddSquare(depth, map[yoff+xoff].triangle1.v2.position, 
									map[yoff+xoff].triangle1.v3.position,
									sideIindex, sideIndices, sideVindex, sideVertices);
							}
						}

					}
				}
			}
		}

		dev->SetRenderState(D3DRS_LIGHTING,FALSE);

		float brightNess = 0.7f;
		D3DXCOLOR colour = D3DXCOLOR(brightNess, brightNess, brightNess, 1);
		dev->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TEXTURE);
		dev->SetTextureStageState(0,D3DTSS_COLORARG2,D3DTA_TFACTOR);
		dev->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_MODULATE);
		dev->SetRenderState(D3DRS_TEXTUREFACTOR,colour);

		// bottom grid
		if (iindex > 0)
		{
			dev->SetTexture(0, texture);
			dev->SetFVF(D3DPFVF);
			dev->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, vindex, iindex/3, indices,  
										D3DFMT_INDEX16, vertices, sizeof(D3DPVERTEX));
		}

		// top grid
		if (highIindex > 0)
		{
			dev->SetTexture(0, fgTexture);
			dev->SetFVF(D3DPFVF);
			dev->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, highVindex, highIindex/3, highIndices,  
										D3DFMT_INDEX16, highVertices, sizeof(D3DPVERTEX));
		}

		// skirting
		if (sideIindex > 0)
		{
			dev->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TEXTURE);
			dev->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_SELECTARG1);
			dev->SetTexture(0, NULL);
			dev->SetFVF(D3DPCOLOURONLYFVF);
			dev->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, sideVindex, sideIindex/3, sideIndices,  
										D3DFMT_INDEX16, sideVertices, sizeof(D3DPCOLOURONLYVERTEX));
		}
	}

}

void ModelMap::Draw(double time)
{
	if (map != NULL)
	{
		// adjust world matrix so drawn mesh is centered on screen
		Device* dev = Interface::GetDevice();
		dev->SetWorldTransform(&GetWorldTransform());

		int squareSize = 20;

		int y1Size = sizeY / squareSize;
		int x1Size = sizeX / squareSize;

		for (int y1=0; y1 < y1Size; y1++)
		for (int x1=0; x1 < x1Size; x1++)
		{

			// draw / setup
			int vindex = 0;
			int iindex = 0;
			int highVindex = 0;
			int highIindex = 0;
			int sideVindex = 0;
			int sideIindex = 0;

			int startWidth = x1 * squareSize;
			int endWidth = startWidth + squareSize;
			int startHeight = y1 * squareSize;
			int endHeight = startHeight + squareSize;

			// copy bottom and top parts of the map based on depth
			for (int y=startHeight; y < endHeight; y++)
			{
				int yoff = y;
				if (yoff > 1 && yoff < (sizeY-1))
				{
					yoff = yoff * sizeX;
					for (int x=startWidth; x < endWidth; x++)
					{
						int xoff = x;
						if (xoff > 1 && xoff < (sizeX-1))
						{
							// up or down?
							if (map[yoff+xoff].triangle1.v1.position.z == depth)
							{
								// add both triangles to the square
								indices[iindex] = iindex++;
								indices[iindex] = iindex++;
								indices[iindex] = iindex++;
								vertices[vindex++] = map[yoff+xoff].triangle1.v3;
								vertices[vindex++] = map[yoff+xoff].triangle1.v2;
								vertices[vindex++] = map[yoff+xoff].triangle1.v1;

								indices[iindex] = iindex++;
								indices[iindex] = iindex++;
								indices[iindex] = iindex++;
								vertices[vindex++] = map[yoff+xoff].triangle2.v3;
								vertices[vindex++] = map[yoff+xoff].triangle2.v2;
								vertices[vindex++] = map[yoff+xoff].triangle2.v1;
							}
							else
							{
								// add raised squares to the map
								highIndices[highIindex] = highIindex++;
								highIndices[highIindex] = highIindex++;
								highIndices[highIindex] = highIindex++;
								highVertices[highVindex++] = map[yoff+xoff].triangle1.v3;
								highVertices[highVindex++] = map[yoff+xoff].triangle1.v2;
								highVertices[highVindex++] = map[yoff+xoff].triangle1.v1;

								highIndices[highIindex] = highIindex++;
								highIndices[highIindex] = highIindex++;
								highIndices[highIindex] = highIindex++;
								highVertices[highVindex++] = map[yoff+xoff].triangle2.v3;
								highVertices[highVindex++] = map[yoff+xoff].triangle2.v2;
								highVertices[highVindex++] = map[yoff+xoff].triangle2.v1;

								float height = map[yoff+xoff].triangle1.v1.position.z;
								float hx1 = map[yoff+xoff-1].triangle1.v1.position.z;
								float hx2 = map[yoff+xoff+1].triangle1.v1.position.z;
								float hy1 = map[yoff+xoff-sizeY].triangle1.v1.position.z;
								float hy2 = map[yoff+xoff+sizeY].triangle1.v1.position.z;

								// add sides to the square if necessairy
								if (height < hx1) // bit of a mind twister - this tile is higher than neighbouring tile
								{
									AddSquare(depth, map[yoff+xoff].triangle1.v3.position, 
										map[yoff+xoff].triangle1.v1.position,
										sideIindex, sideIndices, sideVindex, sideVertices);
								}
								if (height < hx2)
								{
									AddSquare(depth, map[yoff+xoff].triangle2.v3.position, 
										map[yoff+xoff].triangle2.v1.position,
										sideIindex, sideIndices, sideVindex, sideVertices);
								}
								if (height < hy1)
								{
									AddSquare(depth, map[yoff+xoff].triangle2.v2.position, 
										map[yoff+xoff].triangle2.v3.position,
										sideIindex, sideIndices, sideVindex, sideVertices);
								}
								if (height < hy2)
								{
									AddSquare(depth, map[yoff+xoff].triangle1.v2.position, 
										map[yoff+xoff].triangle1.v3.position,
										sideIindex, sideIndices, sideVindex, sideVertices);
								}
							}

						}
					}
				}
			}

			dev->SetRenderState(D3DRS_LIGHTING,FALSE);

			float brightNess = 0.7f;
			D3DXCOLOR colour = D3DXCOLOR(brightNess, brightNess, brightNess, 1);
			dev->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TEXTURE);
			dev->SetTextureStageState(0,D3DTSS_COLORARG2,D3DTA_TFACTOR);
			dev->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_MODULATE);
			dev->SetRenderState(D3DRS_TEXTUREFACTOR,colour);

			if (iindex > 0)
			{
				dev->SetTexture(0, texture);
				dev->SetFVF(D3DPFVF);
				dev->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, vindex, iindex/3, indices,  
											D3DFMT_INDEX16, vertices, sizeof(D3DPVERTEX));
			}

			// top grid
			if (highIindex > 0)
			{
				dev->SetTexture(0, fgTexture);
				dev->SetFVF(D3DPFVF);
				dev->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, highVindex, highIindex/3, highIndices,  
											D3DFMT_INDEX16, highVertices, sizeof(D3DPVERTEX));
			}

			// skirt
			if (sideIindex > 0)
			{
				dev->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TEXTURE);
				dev->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_SELECTARG1);
				dev->SetTexture(0, NULL);
				dev->SetFVF(D3DPCOLOURONLYFVF);
				dev->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, sideVindex, sideIindex/3, sideIndices,  
											D3DFMT_INDEX16, sideVertices, sizeof(D3DPCOLOURONLYVERTEX));
			}
		}
	}

}

void ModelMap::UpdateBoundingBox()
{
	if (map != NULL)
	{
		D3DXVECTOR3 minx = map[0].triangle1.v1.position;
		D3DXVECTOR3 maxx = map[0].triangle1.v1.position;

		int size = sizeX * sizeY;
		for (int i=0; i < size; i++)
		{
			minx.x = min(map[0].triangle1.v1.position.x, minx.x);
			minx.y = min(map[0].triangle1.v1.position.y, minx.y);
			minx.z = min(map[0].triangle1.v1.position.z, minx.z);

			maxx.x = max(map[0].triangle1.v1.position.x, maxx.x);
			maxx.y = max(map[0].triangle1.v1.position.y, maxx.y);
			maxx.z = max(map[0].triangle1.v1.position.z, maxx.z);
		}

		GetBoundingBox()->SetMin(minx);
		GetBoundingBox()->SetMax(maxx);
	}
}

BoundingBox* ModelMap::GetBoundingBox()
{
	return WorldObject::GetBoundingBox();
}

void ModelMap::Read(XmlNode* node)
{
	XmlNode::CheckVersion(node, modelMapSignature, modelMapVersion);
	WorldObject::Read(node->GetChild(WorldObject::Signature()));

	node->Read("scale", scale);
	node->Read("depth", depth);
	node->Read("sizeX", sizeX);
	node->Read("sizeY", sizeY);
	node->Read("width", width);
	node->Read("height", height);
	node->Read("filename", filename);
	node->Read("textureFilename", textureFilename);
	node->Read("fgTextureFilename", fgTextureFilename);

	Update();
}

XmlNode* ModelMap::Write()
{
	XmlNode* node = XmlNode::NewChild(modelMapSignature,modelMapVersion);
	node->Add(WorldObject::Write());

	node->Write("scale", scale);
	node->Write("depth", depth);
	node->Write("sizeX", sizeX);
	node->Write("sizeY", sizeY);
	node->Write("width", width);
	node->Write("height", height);
	node->Write("filename", filename);
	node->Write("textureFilename", textureFilename);
	node->Write("fgTextureFilename", fgTextureFilename);
	return node;
}
