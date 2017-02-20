#pragma once

#include "d3d9/InvalidatableDevice.h"

class Texture;

/////////////////////////////////////////////////////

class Font : public InvalidatableDevice
{
public:
	Font(void);
	Font(const Font&);
	const Font& operator=(const Font&);
	virtual ~Font(void);

	bool Load(const std::string& fname);
	void Write(float x,float y,const std::string& str,D3DXCOLOR colour);
	void Write(float x, float y, const std::string& str);

	float GetWidth(const std::string& str);
	float GetCharWidth( char ch );

	// callbacks from device object
	void RestoreDeviceObjects();
	void InvalidateDeviceObjects();

private:
	bool SetupD3DFont(void);
	void WriteLetter(float x,float y,char ch);

private:
	int		width;
	int		height;
	int		widths[256];

	int		byteWidth;
	int		byteHeight;
	byte*	font;

	Texture* texture;

	// signature
	static std::string fontSignature;
	static int fontVersion;
};
