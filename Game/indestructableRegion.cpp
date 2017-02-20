//////////////////////////////////////////////////////////////////////
/// GRAVITY FORCE X
///
/// (c)Copyright Peter de Vocht, 2006
///
/// I'm releasing this code under GPL v2.1 (see included file lgpl.txt)
///
//////////////////////////////////////////////////////////////////////

#include "standardFirst.h"

#include "game/indestructableRegion.h"

std::string IndestructableRegion::irSignature="indestructableRegion";
int IndestructableRegion::irVersion=1;

///////////////////////////////////////////////////////////

IndestructableRegion::IndestructableRegion()
{
	SetType(OT_INDESTRUCTABLEREGION);
	SetWorldType(TYPE_INDESTRUCTABLEREGION);

	width = 100;
	height = 100;
}

IndestructableRegion::~IndestructableRegion()
{
}

IndestructableRegion::IndestructableRegion(const IndestructableRegion& i)
{
	operator=(i);
}

const IndestructableRegion& IndestructableRegion::operator=(const IndestructableRegion& i)
{
	WorldObject::operator =(i);

	width = i.width;
	height = i.height;

	return *this;
}

void IndestructableRegion::Draw(double time)
{
	D3DXVECTOR3 pos1 = GetPosition();
	pos1.y -= height;
	D3DXVECTOR3 pos2 = pos1 + D3DXVECTOR3(width,height,0);
	Device* dev = Interface::GetDevice();
	dev->SetIdentityTransform();
	D3DXCOLOR colour = D3DXCOLOR(0.6f, 0.6f, 0.6f, 0.5f);
	dev->FillRect(pos1,pos2,colour);
}

bool IndestructableRegion::Inside(const D3DXVECTOR3& p)
{
	D3DXVECTOR3 pos = GetPosition();
	return (pos.x <= p.x && p.x <= (pos.x+width) &&
			(pos.y-height) <= p.y && p.y <= pos.y);
}

float IndestructableRegion::GetWidth()
{
	return width;
}

void IndestructableRegion::SetWidth(float _width)
{
	width = _width;
}

float IndestructableRegion::GetHeight()
{
	return height;
}

void IndestructableRegion::SetHeight(float _height)
{
	height = _height;
}

void IndestructableRegion::UpdateBoundingBox()
{
	D3DXVECTOR3 pos = GetPosition();
	BoundingBox* bb = GetBoundingBox();
	pos.y -= height;
	bb->SetMin(pos);
	pos += D3DXVECTOR3(width,height,0);
	bb->SetMax(pos);
}

void IndestructableRegion::Read(XmlNode* node)
{
	XmlNode::CheckVersion(node, irSignature, irVersion);
	WorldObject::Read(node->GetChild(WorldObject::Signature()));

	node->Read("width", width);
	node->Read("height", height);

	UpdateBoundingBox();
}

XmlNode* IndestructableRegion::Write()
{
	XmlNode* node = XmlNode::NewChild(irSignature, irVersion);
	node->Add(WorldObject::Write());

	node->Write("width", width);
	node->Write("height", height);
	return node;
}

