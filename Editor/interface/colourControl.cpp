#include "editor/stdafx.h"

#include "interface/colourControl.h"

/////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CColourControl,CStatic)
	ON_WM_PAINT()
END_MESSAGE_MAP()

CColourControl::CColourControl()
{
	colourBrush = NULL;
	colour = 0;
};

void CColourControl::SetColour(const D3DXCOLOR& _colour)
{
	COLORREF cr = _colour;
	colour = (cr&0xff0000)>>16  | (cr&0xff00) | (cr&0xff)<<16;

	if (colourBrush!=NULL)
		::DeleteObject(colourBrush);
	colourBrush = ::CreateSolidBrush(colour);
	Invalidate();
};

D3DXCOLOR CColourControl::GetColour()
{
	D3DXCOLOR col = (colour&0xff0000)>>16  | (colour&0xff00) | (colour&0xff)<<16;
	return col;
};

void CColourControl::OnPaint()
{
	RECT rect;
	::GetClientRect(*this,&rect);

	PAINTSTRUCT ps;
	HDC dc = ::BeginPaint(*this,&ps);
	::FillRect(dc,&rect,colourBrush);
	::ValidateRect(*this,NULL);
	::EndPaint(*this,&ps);
};

