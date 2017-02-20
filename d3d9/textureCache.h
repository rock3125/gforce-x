//////////////////////////////////////////////////////////////////////
/// GRAVITY FORCE X
///
/// (c)Copyright Peter de Vocht, 2006
///
/// I'm releasing this code under GPL v2.1 (see included file lgpl.txt)
///
//////////////////////////////////////////////////////////////////////

#pragma once

#include "d3d9/InvalidatableDevice.h"
#include "d3d9/Device.h"

/////////////////////////////////////////////////////////////////////////////
// currently implemented only for d3d9 but that can change later for xbox

class Texture;

class TextureCache : public InvalidatableDevice
{
	TextureCache(Device* device);
	TextureCache();
	TextureCache(const TextureCache&);
	const TextureCache& operator=(const TextureCache&);
public:
	virtual ~TextureCache();

	// access singleton
	static TextureCache& GetTextureCache();

	// remove texture cache
	static void Destroy();

	// callbacks from device object
	void RestoreDeviceObjects();
	void InvalidateDeviceObjects();

	// load/get a texture from cache
	static Texture* GetTexture(const std::string& fname,D3DFORMAT format=D3DFMT_UNKNOWN,D3DPOOL pool=D3DPOOL_DEFAULT);

	// purge cache
	void Purge();

private:
	typedef stdext::hash_map<std::string, Texture* > TextureMap;

	TextureMap textures;

	static TextureCache* singleton;

	Device* device;
};
