//////////////////////////////////////////////////////////////////////
/// GRAVITY FORCE X
///
/// (c)Copyright Peter de Vocht, 2006
///
/// I'm releasing this code under GPL v2.1 (see included file lgpl.txt)
///
//////////////////////////////////////////////////////////////////////

#include "standardFirst.h"

#include "d3d9/interface.h"
#include "d3d9/textureCache.h"
#include "d3d9/texture.h"

/////////////////////////////////////////////////////////////////////////////

TextureCache* TextureCache::singleton=NULL;

TextureCache::TextureCache(Device* _device)
	: device(_device)
{
}

TextureCache::~TextureCache()
{
	TextureMap::iterator iter = textures.begin();
	while(iter != textures.end())
	{
		Texture* texture = iter->second;
		safe_delete(texture);
		++iter;
	}
	textures.clear();
}

TextureCache& TextureCache::GetTextureCache()
{
	// get/create texture cache system
	if (singleton == NULL)
		singleton = new TextureCache(Interface::GetDevice());
	return *singleton;
}

void TextureCache::Destroy()
{
	safe_delete(singleton);
}

void TextureCache::InvalidateDeviceObjects()
{
	// release all loaded textures
	TextureMap::iterator iter = textures.begin();
	while (iter != textures.end())
	{
		Texture* texture = iter->second;
		texture->Invalidate();
		++iter;
	}
}

void TextureCache::RestoreDeviceObjects()
{
	TextureMap::iterator iter = textures.begin();
	while(iter != textures.end())
	{
		// managed textures do not need to be reloaded
		if (iter->second->GetPool() != D3DPOOL_MANAGED)
		{
			iter->second->Reload();
			++iter;
		}
	}
}

void TextureCache::Purge()
{
	// release texture first
	InvalidateDeviceObjects();

	TextureMap::iterator iter=textures.begin();
	while (iter!=textures.end())
	{
		safe_delete(iter->second);
		++iter;
	}
	textures.clear();
}

Texture* TextureCache::GetTexture(const std::string& fname,D3DFORMAT format,D3DPOOL pool)
{
	if (singleton==NULL)
	{
		singleton=new TextureCache(Interface::GetDevice());
	}

	// filename to lower case
	std::string f = System::ToLower(fname);

	// see if texture is already loaded
	TextureMap::iterator iter = singleton->textures.find(f);
	if (iter!=singleton->textures.end())
		return iter->second;

	// if texture not found then create new texture item and load from file
	Texture* texture = new Texture(singleton->device);
	texture->LoadTextureFromFile(f,format,pool);

	singleton->textures[f] = texture;

	return texture;
}

