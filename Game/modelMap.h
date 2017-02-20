//////////////////////////////////////////////////////////////////////
/// GRAVITY FORCE X
///
/// (c)Copyright Peter de Vocht, 2006
///
/// I'm releasing this code under GPL v2.1 (see included file lgpl.txt)
///
//////////////////////////////////////////////////////////////////////

#pragma once

class Hexagon;
class Texture;
class Light;

#include "system/model/worldObject.h"

/////////////////////////////////////////////////////////////////////////

class ModelMap : public WorldObject
{
public:
	ModelMap();
	~ModelMap();
	ModelMap(const ModelMap&);
	const ModelMap& operator=(const ModelMap&);

	// update button interface - update landscape interally
	// with possible new level, new textures, etc.
	void Update();

	// draw it all from pos' point of view with a screen offset
	// of topLeftOffset
	void Draw(D3DXVECTOR3 pos, int splitScreenType);
	void Draw(double time);

	// check bullet collision - returns true if it collided
	// and adjust landscape accordingly
	bool CheckBulletCollision(D3DXVECTOR3 pos);

	// calculate missile impact
	void CalculateMissileImpact(D3DXVECTOR3 pos, float range);

	// access properties
	const std::string& GetMapFilename();
	void SetMapFilename(const std::string&);

	// get set background texture filename
	const std::string& GetBackgroundTextureFilename();
	void SetBackgroundTextureFilename(const std::string&);

	// get set foreground texture filename
	const std::string& GetForegroundTextureFilename();
	void SetForegroundTextureFilename(const std::string&);

	// gravity is a property of the map - a powerful ally
	float GetGravity();
	void SetGravity(float _gravity);

	// get set side skirt colour
	D3DXCOLOR GetSideColourTop();
	void SetSideColourTop(D3DXCOLOR sideColourTop);

	// get set side skirt colour
	D3DXCOLOR GetSideColourBottom();
	void SetSideColourBottom(D3DXCOLOR sideColourBottom);

	// save and load ModelMap
	virtual void Read(XmlNode* node);
	virtual XmlNode* Write();

	// update internal bounding volume
	virtual void UpdateBoundingBox();
	virtual BoundingBox* GetBoundingBox();

	// return the signature of this object
	virtual std::string Signature()
	{
		return modelMapSignature;
	}

protected:
	// load a map from file
	void Load();

	// convert a height to a vertex
	D3DPVERTEX Convert(int x, int y, byte c);

	// setup vertex
	void Set(D3DPVERTEX* v, int index, float x, float y, float z,float tx,float ty);

	// check uv alignments
	int AlignUV(D3DPVERTEX* v1, D3DPVERTEX* v2, D3DPVERTEX* v3);

private:
	// does pos collide with vertex (for bullets)
	bool Collides(const D3DPVERTEX& pos1, const D3DXVECTOR3& pos2);

	// add a square helper method
	void AddSquare(float height, const D3DXVECTOR3& v1, const D3DXVECTOR3& v2,
				   int& i, WORD* index, int& v, D3DPCOLOURONLYVERTEX* verts);

private:
	float		scale;
	float		heightScale;
	float		textureScale;
	float		depth;
	int			sizeX;
	int			sizeY;
	int			numHits;
	std::string	filename;

	// width and height of map
	// i.e. screenResH / 8 and screenResV / 8
	int			width;
	int			height;

	// main data structure holds one unit triangle
	struct Triangle
	{
		D3DPVERTEX	v1;
		D3DPVERTEX	v2;
		D3DPVERTEX	v3;
	};

	// a square in the map
	struct Square
	{
		Triangle triangle1;
		Triangle triangle2;
	};

	// local cache for drawing meshes using primitives
	WORD*				indices;
	D3DPVERTEX*			vertices;

	// local cache for drawing meshes using primitives
	WORD*				highIndices;
	D3DPVERTEX*			highVertices;

	// for drawing in between triangles
	WORD*					sideIndices;
	D3DPCOLOURONLYVERTEX*	sideVertices;

	// the map - heights etc
	Square*				map;

	// gravity for this map
	float				gravity;

	// background
	std::string			textureFilename;
	Texture*			texture;

	// foreground
	std::string			fgTextureFilename;
	Texture*			fgTexture;

	// side colour of 'skirts'
	D3DXCOLOR			sideColourTop;
	D3DXCOLOR			sideColourBottom;

private:
	// flatten a complete square by a colour and depth
	void FlattenSquare(Square* sq, float depth);
	void FlattenSquare(int index, int mapx, int mapy);
	void FlattenSquare(int index, int mapx, int mapy, float amount);

private:
	// signature and version
	static std::string modelMapSignature;
	static int modelMapVersion;
};

