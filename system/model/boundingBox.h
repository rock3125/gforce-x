#pragma once

/////////////////////////////////////////////////////////////////////////////

class BoundingBox
{
public:
	BoundingBox();
	virtual ~BoundingBox();
	BoundingBox(const BoundingBox&);
	const BoundingBox& operator=(const BoundingBox&);

	void Draw();

	// property accessors
	const D3DXVECTOR3& GetCenter();
	void SetCenter(const D3DXVECTOR3&);

	const D3DXVECTOR3& GetMin();
	D3DXVECTOR3 GetTransformedMin();
	void SetMin(const D3DXVECTOR3&);

	const D3DXVECTOR3& GetMax();
	D3DXVECTOR3 GetTransformedMax();
	void SetMax(const D3DXVECTOR3&);

	const D3DXCOLOR& GetColour();
	void SetColour(const D3DXCOLOR&);

	void Update(const D3DXMATRIXA16& transform);

	// check dirn intersects
	bool IntersectRay(const D3DXVECTOR3& orig,const D3DXVECTOR3& dirn, float* dist);

private:
	void Calculate();
	void OutOfDate();

private:
	D3DXVECTOR3 aabbMin;
	D3DXVECTOR3	aabbMax;
	D3DXCOLOR	colour;

	// locals - not persistent
	D3DXMATRIXA16 transform;
	bool outOfDate;
	D3DXVECTOR3 verts[8];

	// always shared
	static WORD	indices[24];
};

