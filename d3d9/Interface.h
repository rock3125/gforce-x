//////////////////////////////////////////////////////////////////////
/// GRAVITY FORCE X
///
/// (c)Copyright Peter de Vocht, 2006
///
/// I'm releasing this code under GPL v2.1 (see included file lgpl.txt)
///
//////////////////////////////////////////////////////////////////////

#pragma once

class Device;
class CD3DEnumeration;
class CD3DSettings;

////////////////////////////////////////////////////////////

class Interface
{
	Interface(const Interface&);
	const Interface& operator=(const Interface&);
	Interface();
	virtual ~Interface();
public:

	// access interface
	static Device* GetDevice();

	// only to be used by device and app
	static Interface* GetI();

	// remove interface when finished
	static void Create();
	static void Destroy();

	// initialise main window
	bool InitialiseDevice(HWND hWindowed,D3DPRESENT_PARAMETERS* present=NULL,UINT adapter=D3DADAPTER_DEFAULT,
						  D3DDEVTYPE devType=D3DDEVTYPE_HAL,DWORD behaviour=D3DCREATE_PUREDEVICE|D3DCREATE_HARDWARE_VERTEXPROCESSING);
	bool InitialiseDevice(HWND hWindowed, int width, int height);
	bool ResetDevice();
	void ReleaseDevice();

	// return the device status - return true if its ready for rendering
	bool IsDeviceOK();

	// present parameters, set resets the device
	const D3DPRESENT_PARAMETERS& GetPresentParams();
	void SetPresentParams(const D3DPRESENT_PARAMETERS& p);

	// helper func to resize the back buffer ( calls set presentparams)
	bool ResizeBackBuffer(int width, int height);

	// check device format
	bool CheckDeviceFormat(D3DFORMAT fmt, DWORD usage=0, D3DRESOURCETYPE rtype=D3DRTYPE_TEXTURE);

	// get device focus window
	HWND GetFocusWindow();

	// get is device ready
	bool IsDeviceReady();

	const D3DCAPS9& GetCapabilities();

	// access backbuffer width and height
	int GetWidth() const;

	// access backbuffer width and height
	int GetHeight() const;

private:
	bool FindBestWindowedMode(HWND hWindowed,bool bRequireHAL,bool bRequireREF);
	static bool ConfirmDeviceHelper(D3DCAPS9* pCaps, VertexProcessingType vertexProcessingType,
									D3DFORMAT adapterFormat, D3DFORMAT backBufferFormat );
	HRESULT Initialise3DEnvironment(HWND hWindow);
	void BuildPresentParamsFromSettings(HWND hWindow);
	HRESULT ConvertAnsiStringToGenericCch( TCHAR* tstrDestination, const CHAR* strSource, int cchDestChar );
	HRESULT ConvertAnsiStringToWideCch( WCHAR* wstrDestination, const CHAR* strSource, int cchDestChar );

private:
	UINT				adapter;
	D3DDEVTYPE			deviceType;
	int					width;
	int					height;

	HWND				hwndWindowed;

	// Always one of the above pointers
	HWND				hwndFocusWindow;

	DWORD				behaviorFlags;
	D3DPRESENT_PARAMETERS presentParams;

	CD3DEnumeration*	d3dEnumeration;
    CD3DSettings*		d3dSettings;

	// directx pointers
	IDirect3D9*			d3d;
	IDirect3DDevice9*	d3dDevice;
	Device*				device;
	IDirect3DSurface9*	backBufferSurface;
	IDirect3DSurface9*	depthStencilSurface;
	DWORD				dwCreateFlags;
    D3DSURFACE_DESC		d3dsdBackBuffer;   // Surface desc of the backbuffer

	bool				deviceReady;

	// device capabilities
	D3DCAPS9			devCaps;
    TCHAR				deviceStats[90];// String to hold D3D device stats

	// singleton
	static Interface* singleton;
};

