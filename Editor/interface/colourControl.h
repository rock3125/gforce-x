#pragma once

/////////////////////////////////////////////////////////////////////////////

class CColourControl : public CStatic
{
public:
	CColourControl();

	void SetColour(const D3DXCOLOR& colour);
	D3DXCOLOR GetColour();

protected:
	virtual void OnPaint();

private:
	COLORREF	colour;
	HBRUSH		colourBrush;

	DECLARE_MESSAGE_MAP()
};

