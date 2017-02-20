#include "standardFirst.h"

#include "system/model/boundingBox.h"

/////////////////////////////////////////////////////////////////////////////

WORD BoundingBox::indices[24] = { 0,1, 1,3, 3,2, 2,0,
								  4,5, 5,7, 7,6, 6,4,
								  0,4, 1,5, 3,7, 2,6 };

/////////////////////////////////////////////////////////////////////////////

BoundingBox::BoundingBox()
{
	aabbMin = D3DXVECTOR3(0,0,0);
	aabbMax = D3DXVECTOR3(0,0,0);
	colour = D3DXCOLOR(0.7f,0.7f,1,1);
	transform = *System::GetIdentityMatrix();
	OutOfDate();
}

BoundingBox::~BoundingBox()
{
}

BoundingBox::BoundingBox(const BoundingBox& bb)
{
	operator=(bb);
}

const BoundingBox& BoundingBox::operator=(const BoundingBox& bb)
{
	aabbMin = bb.aabbMin;
	aabbMax = bb.aabbMax;
	colour = bb.colour;

	transform = bb.transform;
	OutOfDate();

	return *this;
}

void BoundingBox::OutOfDate()
{
	outOfDate = true;
}

void BoundingBox::Draw()
{
	if (aabbMin != aabbMax)
	{
		if (outOfDate)
		{
			Calculate();
		}
		Device* dev = Interface::GetDevice();
		dev->SetWorldTransform(System::GetIdentityMatrix());
		dev->DrawLines(8,12,verts,indices,colour);
	}
}

void BoundingBox::Calculate()
{
	outOfDate = false;

	verts[0] = D3DXVECTOR3(aabbMin.x,aabbMin.y,aabbMin.z);
	verts[1] = D3DXVECTOR3(aabbMin.x,aabbMin.y,aabbMax.z);
	verts[2] = D3DXVECTOR3(aabbMax.x,aabbMin.y,aabbMin.z);
	verts[3] = D3DXVECTOR3(aabbMax.x,aabbMin.y,aabbMax.z);

	verts[4] = D3DXVECTOR3(aabbMin.x,aabbMax.y,aabbMin.z);
	verts[5] = D3DXVECTOR3(aabbMin.x,aabbMax.y,aabbMax.z);
	verts[6] = D3DXVECTOR3(aabbMax.x,aabbMax.y,aabbMin.z);
	verts[7] = D3DXVECTOR3(aabbMax.x,aabbMax.y,aabbMax.z);

	for (int i=0; i<8; i++)
	{
		D3DXVec3TransformCoord(&verts[i],&verts[i],&transform);
	}
}

void BoundingBox::Update(const D3DXMATRIXA16& _transform)
{
	transform = _transform;
	OutOfDate();
}

const D3DXVECTOR3& BoundingBox::GetMin()
{
	return aabbMin;
}

D3DXVECTOR3 BoundingBox::GetTransformedMin()
{
	D3DXVECTOR3 tmin;
	D3DXVec3TransformCoord(&tmin,&aabbMin,&transform);
	return tmin;
}

void BoundingBox::SetMin(const D3DXVECTOR3& _aabbMin)
{
	aabbMin = _aabbMin;
	OutOfDate();
}

const D3DXVECTOR3& BoundingBox::GetMax()
{
	return aabbMax;
}

D3DXVECTOR3 BoundingBox::GetTransformedMax()
{
	D3DXVECTOR3 tmax;
	D3DXVec3TransformCoord(&tmax,&aabbMax,&transform);
	return tmax;
}

void BoundingBox::SetMax(const D3DXVECTOR3& _aabbMax)
{
	aabbMax = _aabbMax;
	OutOfDate();
}

const D3DXCOLOR& BoundingBox::GetColour()
{
	return colour;
}

void BoundingBox::SetColour(const D3DXCOLOR& _colour)
{
	colour = _colour;
}

bool BoundingBox::IntersectRay(const D3DXVECTOR3& orig,const D3DXVECTOR3& dirn, float* dist)
{
    float fBary1, fBary2, fDist;
    for (int i=0; i < 8; i++)
    {
        // Check if the pick ray passes through this point
        if (D3DXIntersectTri(&verts[indices[3 * i + 0]], &verts[indices[3 * i + 1]],
							 &verts[indices[3 * i + 2]], &orig, &dirn, &fBary1, &fBary2, dist))
		{
			return true;
		}
    }
	return false;
}
