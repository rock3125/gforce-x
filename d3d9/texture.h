//////////////////////////////////////////////////////////////////////
/// GRAVITY FORCE X
///
/// (c)Copyright Peter de Vocht, 2006
///
/// I'm releasing this code under GPL v2.1 (see included file lgpl.txt)
///
//////////////////////////////////////////////////////////////////////

#pragma once

#include "d3d9/Device.h"

/////////////////////////////////////////////////////////////////////////////

class Texture
{
public:
	// create an empty texture
	Texture(Device* device);

	// create a texture of the specified dimension
	Texture(Device* device,int width,int height,int levels,DWORD usage,D3DFORMAT format,D3DPOOL pool);

	virtual ~Texture();

	// original filename
	const std::string& GetFilename();

	// Lock a texture
	D3DLOCKED_RECT Lock();
	D3DLOCKED_RECT LockReadOnly();

	// unlock a locked texture
	void Unlock();

	// texture dimensions
	int GetWidth() const;
	int GetHeight() const;

	// invalidate this texture
	// sets d3d pointer to NULL and Releases it
	void Invalidate();

protected:
	// reload texture
	void Reload();

	// set texture at a specified stage
	bool SetTexture(int stage=0);

	// try and load texture from file
	bool LoadTextureFromFile(const std::string& fname,D3DFORMAT format, D3DPOOL pool);

	// access format and pool
	D3DFORMAT GetFormat() const;
	D3DPOOL GetPool() const;

	// get pixel size of format
	int GetPixelSizeInBytes();

	// get underlying pointer
	IDirect3DTexture9* GetTexture();

protected:
	friend class TextureCache;
	friend class Device;

	// the texture
	IDirect3DTexture9*	texture;
	Device*				device;

	D3DFORMAT			format;
	D3DPOOL				pool;
	DWORD				usage;

	int					width;
	int					height;

	std::string			filename;

	static std::string textureSignature;
	static int textureVersion;
};

