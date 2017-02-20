//////////////////////////////////////////////////////////////////////
/// GRAVITY FORCE X
///
/// (c)Copyright Peter de Vocht, 2006
///
/// I'm releasing this code under GPL v2.1 (see included file lgpl.txt)
///
//////////////////////////////////////////////////////////////////////

#include "standardFirst.h"

#include "d3d9/texture.h"
#include "d3d9/interface.h"

std::string Texture::textureSignature="text";
int Texture::textureVersion=1;

/////////////////////////////////////////////////////////////////////////////

Texture::Texture(Device* _device)
	: texture(NULL)
	, device(_device)
{
	width = 0;
	height = 0;
	usage = D3DUSAGE_DYNAMIC;
	format = D3DFMT_UNKNOWN;
	pool = D3DPOOL_DEFAULT;
}

Texture::Texture(Device* _device,int _width,int _height,int levels,DWORD _usage,D3DFORMAT _format,D3DPOOL _pool)
	: texture(NULL)
	, device(_device)
{
	if (_device == NULL)
		throw new Exception("device is NULL");

	IDirect3DDevice9* dev = _device->GetDevice();
	if (dev == NULL)
		throw new Exception("device is NULL");

    dev->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
    dev->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
    dev->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);
    dev->SetSamplerState(0, D3DSAMP_ADDRESSU,  D3DTADDRESS_CLAMP);
    dev->SetSamplerState(0, D3DSAMP_ADDRESSV,  D3DTADDRESS_CLAMP);

	HRESULT hr = dev->CreateTexture(_width,_height,levels,_usage,_format,_pool,&texture,NULL);
	if (!SUCCEEDED(hr))
		throw new Exception("failed to create texture " + System::Int2Str(hr));

	width = _width;
	height = _height;
	usage = _usage;
	format = _format;
	pool = _pool;
}

Texture::~Texture()
{
	texture = NULL;
	device = NULL;
}

D3DLOCKED_RECT Texture::Lock()
{
	if (texture == NULL)
		throw new Exception("texture == NULL");

	D3DLOCKED_RECT lr;
	HRESULT hr = texture->LockRect(0,&lr,NULL,D3DLOCK_DISCARD);
	if (!SUCCEEDED(hr))
		throw new Exception("failed to lock texure " + System::Int2Str(hr));

	return lr;
}

D3DLOCKED_RECT Texture::LockReadOnly()
{
	if (texture == NULL)
		throw new Exception("texture == NULL");

	D3DLOCKED_RECT lr;
	HRESULT hr = texture->LockRect(0,&lr,NULL,D3DLOCK_READONLY);
	if (!SUCCEEDED(hr))
		throw new Exception("failed to lock texure " + System::Int2Str(hr));

	return lr;
}

void Texture::Unlock()
{
	if (texture == NULL)
		throw new Exception("texture == NULL");

	texture->UnlockRect(0);
}

IDirect3DTexture9* Texture::GetTexture()
{
	return texture;
}

int Texture::GetPixelSizeInBytes()
{
	PreCond(texture!=NULL);

	D3DSURFACE_DESC pDesc;
	texture->GetLevelDesc(0,&pDesc);

	switch (pDesc.Format)
	{
		case D3DFMT_R8G8B8: return 3;

		case D3DFMT_A8B8G8R8:
		case D3DFMT_X8B8G8R8:
		case D3DFMT_X8R8G8B8:
		case D3DFMT_A8R8G8B8: return 4;

		case D3DFMT_X1R5G5B5:
		case D3DFMT_A1R5G5B5:
		case D3DFMT_A4R4G4B4:
		case D3DFMT_R5G6B5: return 2;

		default: throw new Exception("unknown pixel format");
	}
	return 1;
}

void Texture::Reload()
{
	if (!filename.empty())
	{
		LoadTextureFromFile(filename,format,pool);
	}
}

bool Texture::SetTexture(int stage)
{
	return (SUCCEEDED(device->SetTexture(stage,this))==TRUE);
}

bool Texture::LoadTextureFromFile(const std::string& _fname,D3DFORMAT _format,D3DPOOL _pool)
{
	D3DXIMAGE_INFO info;

	std::string fname;
	
	if (System::HasAbolutePath(_fname))
	{
		fname = _fname;
	}
	else
	{
		fname = System::GetDataDirectory() + _fname;
	}

	if (!device->CreateTextureFromFile(fname,D3DX_DEFAULT,D3DX_DEFAULT,1,usage,_format,_pool,
									   D3DX_FILTER_NONE,D3DX_DEFAULT,0,&info,NULL,&texture))
	{
		Log::GetLog() << "texture " + _fname + " can not be loaded" << System::CR;
		return false;
	}

	if (texture != NULL)
	{
		width = info.Width;
		height = info.Height;

		format = _format;
		pool = _pool;

		filename = _fname;

		Log::GetLog() << "Loaded Texture: " + _fname << System::CR;

		return true;
	}
	return false;
}

D3DFORMAT Texture::GetFormat() const
{
	return format;
}

D3DPOOL Texture::GetPool() const
{
	return pool;
}

int Texture::GetWidth() const
{
	return width;
}

int Texture::GetHeight() const
{
	return height;
}

const std::string& Texture::GetFilename()
{
	return filename;
}

void Texture::Invalidate()
{
	if (texture != NULL)
	{
		texture->Release();
		texture = NULL;
	}
}

