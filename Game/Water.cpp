//////////////////////////////////////////////////////////////////////
/// GRAVITY FORCE X
///
/// (c)Copyright Peter de Vocht, 2006
///
/// I'm releasing this code under GPL v2.1 (see included file lgpl.txt)
///
//////////////////////////////////////////////////////////////////////

#include "standardFirst.h"

#include "game/water.h"

std::string Water::waterSignature="water";
int Water::waterVersion=1;

///////////////////////////////////////////////////////////

Water::Water()
{
	SetType(OT_WATER);
	SetWorldType(TYPE_WATER);

	width = 250;
	height = 250;
}

Water::~Water()
{
}

Water::Water(const Water& i)
{
	operator=(i);
}

const Water& Water::operator=(const Water& i)
{
	WorldObject::operator =(i);

	width = i.width;
	height = i.height;

	return *this;
}

void Water::Draw(double time)
{
	D3DXVECTOR3 pos1 = GetPosition();
	pos1.y -= height;
	// depth = 0 .. 10
	pos1.z = 9;
	D3DXVECTOR3 pos2 = pos1 + D3DXVECTOR3(width,height,0);
	Device* dev = Interface::GetDevice();
	dev->SetIdentityTransform();
	D3DXCOLOR colour = D3DXCOLOR(0.3f, 0.3f, 0.8f, 0.5f);
	dev->FillRect(pos1,pos2,colour);
}

void Water::Draw(double time, const D3DXVECTOR3& zeroOffset)
{
	D3DXVECTOR3 pos1 = GetPosition();
	pos1.x -= zeroOffset.x;
	pos1.y -= zeroOffset.y;

	pos1.y -= height;
	// depth = 0 .. 10
	pos1.z = 9;
	D3DXVECTOR3 pos2 = pos1 + D3DXVECTOR3(width,height,0);
	Device* dev = Interface::GetDevice();
	dev->SetIdentityTransform();
	D3DXCOLOR colour = D3DXCOLOR(0.3f, 0.3f, 0.8f, 0.5f);
	dev->FillRect(pos1,pos2,colour);
}

bool Water::Inside(const D3DXVECTOR3& p)
{
	D3DXVECTOR3 pos = GetPosition();
	return (pos.x <= p.x && p.x <= (pos.x+width) &&
			(pos.y-height) <= p.y && p.y <= pos.y);
}

float Water::GetWidth()
{
	return width;
}

void Water::SetWidth(float _width)
{
	width = _width;
}

float Water::GetHeight()
{
	return height;
}

void Water::SetHeight(float _height)
{
	height = _height;
}

void Water::UpdateBoundingBox()
{
	D3DXVECTOR3 pos = GetPosition();
	BoundingBox* bb = GetBoundingBox();
	pos.y -= height;
	bb->SetMin(pos);
	pos += D3DXVECTOR3(width,height,0);
	bb->SetMax(pos);
}

void Water::Read(XmlNode* node)
{
	XmlNode::CheckVersion(node, waterSignature, waterVersion);
	WorldObject::Read(node->GetChild(WorldObject::Signature()));

	node->Read("width", width);
	node->Read("height", height);

	UpdateBoundingBox();
}

XmlNode* Water::Write()
{
	XmlNode* node = XmlNode::NewChild(waterSignature, waterVersion);
	node->Add(WorldObject::Write());

	node->Write("width", width);
	node->Write("height", height);
	return node;
}

