//////////////////////////////////////////////////////////////////////
/// GRAVITY FORCE X
///
/// (c)Copyright Peter de Vocht, 2006
///
/// I'm releasing this code under GPL v2.1 (see included file lgpl.txt)
///
//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////
// TODO:
// change these fixed pipeline primitives to the more modern
// dynamic equivalents
//

struct D3DLINEFVF
{
	D3DXVECTOR3	position;
	DWORD		colour;
};

struct D3DPVERTEX
{
	D3DXVECTOR3	 position;
	D3DXVECTOR3	 normal;
	D3DXVECTOR2	 texture;
};

struct D3DPCOLOURVERTEX
{
	D3DXVECTOR3	position;
	D3DCOLOR	colour;
	float		u,v;
};

struct D3DPCOLOURONLYVERTEX
{
	D3DXVECTOR3	position;
	D3DCOLOR	colour;
};

struct D3DPVERTEX4
{
	D3DXVECTOR4	 position;
	D3DXVECTOR2	 texture;
};

struct D3DPINDEX
{
	WORD v1;
	WORD v2;
	WORD v3;
};

enum
{
	D3DPFVF				=	D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1,
	D3DPCOLOURFVF		=	D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1,
	D3DPCOLOURONLYFVF	=	D3DFVF_XYZ|D3DFVF_DIFFUSE,
	D3DPFVF4			=	D3DFVF_XYZRHW|D3DFVF_TEX1,
	D3DLINE_FVF			=	D3DFVF_XYZ|D3DFVF_DIFFUSE,
};

