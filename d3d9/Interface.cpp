//////////////////////////////////////////////////////////////////////
/// GRAVITY FORCE X
///
/// (c)Copyright Peter de Vocht, 2006
///
/// I'm releasing this code under GPL v2.1 (see included file lgpl.txt)
///
//////////////////////////////////////////////////////////////////////

#include "standardFirst.h"

#include "d3d9/Interface.h"
#include "d3d9/invalidatableDevice.h"

////////////////////////////////////////////////////////////

Interface* Interface::singleton=NULL;

Interface::Interface()
	: hwndFocusWindow(NULL)
	, hwndWindowed(NULL)
	, d3d(NULL)
	, d3dDevice(NULL)
	, device(NULL)
	, backBufferSurface(NULL)
	, depthStencilSurface(NULL)
	, d3dEnumeration(NULL)
    , d3dSettings(NULL)
{
	deviceReady = false;
	dwCreateFlags = 0;

	// suggested settings
	width = 0;
	height = 0;
}

Interface::~Interface()
{
	ReleaseDevice();
	safe_delete(device);
}

Device* Interface::GetDevice()
{
	if (singleton == NULL) return NULL;
	return singleton->device;
};

Interface* Interface::GetI()
{
	return singleton;
};

void Interface::Create()
{
	PreCond(singleton==NULL);
	singleton=new Interface();
};

void Interface::Destroy()
{
	safe_delete(singleton);
};

HWND Interface::GetFocusWindow()
{
	return hwndFocusWindow;
}

int Interface::GetWidth() const
{
	return presentParams.BackBufferWidth;
};

int Interface::GetHeight() const
{
	return presentParams.BackBufferHeight;
};

//-----------------------------------------------------------------------------
// Name: FindBestWindowedMode()
// Desc: Sets up m_d3dSettings with best available windowed mode, subject to
//       the bRequireHAL and bRequireREF constraints.  Returns false if no such
//       mode can be found.
//-----------------------------------------------------------------------------
bool Interface::FindBestWindowedMode(HWND hWindowed,bool bRequireHAL,bool bRequireREF)
{
	Log::GetLog() << "finding best windowed mode" << System::CR;

	PreCond(d3dEnumeration!=NULL);

    // Get display mode of primary adapter (which is assumed to be where the window
    // will appear)
    D3DDISPLAYMODE primaryDesktopDisplayMode;
    d3d->GetAdapterDisplayMode(0, &primaryDesktopDisplayMode);

    D3DAdapterInfo* pBestAdapterInfo = NULL;
    D3DDeviceInfo* pBestDeviceInfo = NULL;
    D3DDeviceCombo* pBestDeviceCombo = NULL;

    for (UINT iai = 0; iai < d3dEnumeration->m_pAdapterInfoList->Count(); iai++ )
    {
        D3DAdapterInfo* pAdapterInfo = (D3DAdapterInfo*)d3dEnumeration->m_pAdapterInfoList->GetPtr(iai);
        for( UINT idi = 0; idi < pAdapterInfo->pDeviceInfoList->Count(); idi++ )
        {
            D3DDeviceInfo* pDeviceInfo = (D3DDeviceInfo*)pAdapterInfo->pDeviceInfoList->GetPtr(idi);
            if (bRequireHAL && pDeviceInfo->DevType != D3DDEVTYPE_HAL)
                continue;
            if (bRequireREF && pDeviceInfo->DevType != D3DDEVTYPE_REF)
                continue;
            for( UINT idc = 0; idc < pDeviceInfo->pDeviceComboList->Count(); idc++ )
            {
                D3DDeviceCombo* pDeviceCombo = (D3DDeviceCombo*)pDeviceInfo->pDeviceComboList->GetPtr(idc);
                bool bAdapterMatchesBB = (pDeviceCombo->BackBufferFormat == pDeviceCombo->AdapterFormat);
                if (!pDeviceCombo->IsWindowed)
                    continue;
                if (pDeviceCombo->AdapterFormat != primaryDesktopDisplayMode.Format)
                    continue;
                // If we haven't found a compatible DeviceCombo yet, or if this set
                // is better (because it's a HAL, and/or because formats match better),
                // save it
                if( pBestDeviceCombo == NULL ||
                    pBestDeviceCombo->DevType != D3DDEVTYPE_HAL && pDeviceCombo->DevType == D3DDEVTYPE_HAL ||
                    pDeviceCombo->DevType == D3DDEVTYPE_HAL && bAdapterMatchesBB )
                {
                    pBestAdapterInfo = pAdapterInfo;
                    pBestDeviceInfo = pDeviceInfo;
                    pBestDeviceCombo = pDeviceCombo;
                    if( pDeviceCombo->DevType == D3DDEVTYPE_HAL && bAdapterMatchesBB )
                    {
                        // This windowed device combo looks great -- take it
                        goto EndWindowedDeviceComboSearch;
                    }
                    // Otherwise keep looking for a better windowed device combo
                }
            }
        }
    }
EndWindowedDeviceComboSearch:
    if (pBestDeviceCombo == NULL )
        return false;

	int w, h;
	if (width==0 || height==0)
	{
		RECT m_rcWindowClient;
		GetClientRect(hWindowed,&m_rcWindowClient);
		w = m_rcWindowClient.right - m_rcWindowClient.left;
		h = m_rcWindowClient.bottom - m_rcWindowClient.top;
	}
	else
	{
		w = width;
		h = height;
	}

	Log::GetLog() << "creating d3d settings" << System::CR;

	d3dSettings = new CD3DSettings();

    d3dSettings->pWindowed_AdapterInfo = pBestAdapterInfo;
    d3dSettings->pWindowed_DeviceInfo = pBestDeviceInfo;
    d3dSettings->pWindowed_DeviceCombo = pBestDeviceCombo;
    d3dSettings->IsWindowed = true;
    d3dSettings->Windowed_DisplayMode = primaryDesktopDisplayMode;
    d3dSettings->Windowed_Width = w;
    d3dSettings->Windowed_Height = h;
    if (d3dEnumeration->AppUsesDepthBuffer)
        d3dSettings->Windowed_DepthStencilBufferFormat = *(D3DFORMAT*)pBestDeviceCombo->pDepthStencilFormatList->GetPtr(0);
    d3dSettings->Windowed_MultisampleType = *(D3DMULTISAMPLE_TYPE*)pBestDeviceCombo->pMultiSampleTypeList->GetPtr(0);
    d3dSettings->Windowed_MultisampleQuality = 0;
    d3dSettings->Windowed_VertexProcessingType = *(VertexProcessingType*)pBestDeviceCombo->pVertexProcessingTypeList->GetPtr(0);
    d3dSettings->Windowed_PresentInterval = *(UINT*)pBestDeviceCombo->pPresentIntervalList->GetPtr(0);
    return true;
}

bool Interface::ConfirmDeviceHelper(D3DCAPS9* pCaps, VertexProcessingType vertexProcessingType,
									D3DFORMAT adapterFormat, D3DFORMAT backBufferFormat )
{
    DWORD dwBehavior;

    if (vertexProcessingType == SOFTWARE_VP)
        dwBehavior = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
    else if (vertexProcessingType == MIXED_VP)
        dwBehavior = D3DCREATE_MIXED_VERTEXPROCESSING;
    else if (vertexProcessingType == HARDWARE_VP)
        dwBehavior = D3DCREATE_HARDWARE_VERTEXPROCESSING;
    else if (vertexProcessingType == PURE_HARDWARE_VP)
        dwBehavior = D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_PUREDEVICE;
    else
        dwBehavior = 0; // TODO: throw exception

    return true;
}

void Interface::BuildPresentParamsFromSettings(HWND hWindow)
{
	Log::GetLog() << "building present parameters" << System::CR;

	presentParams.Windowed               = d3dSettings->IsWindowed;
    presentParams.BackBufferCount        = 1;
    presentParams.MultiSampleType        = d3dSettings->MultisampleType();
    presentParams.MultiSampleQuality     = d3dSettings->MultisampleQuality();
    presentParams.SwapEffect             = D3DSWAPEFFECT_COPY; // D3DSWAPEFFECT_DISCARD;
    presentParams.EnableAutoDepthStencil = d3dEnumeration->AppUsesDepthBuffer;
    presentParams.hDeviceWindow          = hWindow;
    if (d3dEnumeration->AppUsesDepthBuffer)
    {
        presentParams.Flags = D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL;
        presentParams.AutoDepthStencilFormat = d3dSettings->DepthStencilBufferFormat();
    }
    else
    {
        presentParams.Flags = 0;
    }

	RECT m_rcWindowClient;
	GetClientRect(hWindow,&m_rcWindowClient);

	bool windowed = true;
    if (windowed)
    {
        presentParams.BackBufferWidth  = max(m_rcWindowClient.right - m_rcWindowClient.left,640);
        presentParams.BackBufferHeight = max(m_rcWindowClient.bottom - m_rcWindowClient.top,480);
        presentParams.BackBufferFormat = d3dSettings->PDeviceCombo()->BackBufferFormat;
        presentParams.FullScreen_RefreshRateInHz = 0;
        presentParams.PresentationInterval = d3dSettings->PresentInterval();
    }
    else
    {
        presentParams.BackBufferWidth  = d3dSettings->DisplayMode().Width;
        presentParams.BackBufferHeight = d3dSettings->DisplayMode().Height;
        presentParams.BackBufferFormat = d3dSettings->PDeviceCombo()->BackBufferFormat;
        presentParams.FullScreen_RefreshRateInHz = d3dSettings->Fullscreen_DisplayMode.RefreshRate;
        presentParams.PresentationInterval = d3dSettings->PresentInterval();
    }
}

HRESULT Interface::ConvertAnsiStringToWideCch( WCHAR* wstrDestination, const CHAR* strSource, int cchDestChar )
{
    if( wstrDestination==NULL || strSource==NULL || cchDestChar < 1 )
        return E_INVALIDARG;

    int nResult = MultiByteToWideChar( CP_ACP, 0, strSource, -1,
                                       wstrDestination, cchDestChar );
    wstrDestination[cchDestChar-1] = 0;

    if( nResult == 0 )
        return E_FAIL;
    return S_OK;
}

HRESULT Interface::ConvertAnsiStringToGenericCch(TCHAR* tstrDestination, const CHAR* strSource, int cchDestChar )
{
    if( tstrDestination==NULL || strSource==NULL || cchDestChar < 1 )
        return E_INVALIDARG;

#ifdef _UNICODE
    return ConvertAnsiStringToWideCch( tstrDestination, strSource, cchDestChar );
#else
    strncpy_s(tstrDestination, 255, strSource, cchDestChar );
    tstrDestination[cchDestChar-1] = '\0';
    return S_OK;
#endif
}

HRESULT Interface::Initialise3DEnvironment(HWND hWindow)
{
    HRESULT hr;

	Log::GetLog() << "initialising 3d environment" << System::CR;

    D3DAdapterInfo* pAdapterInfo = d3dSettings->PAdapterInfo();
    D3DDeviceInfo* pDeviceInfo = d3dSettings->PDeviceInfo();

    // Set up the presentation parameters
    BuildPresentParamsFromSettings(hWindow);

    if (pDeviceInfo->Caps.PrimitiveMiscCaps & D3DPMISCCAPS_NULLREFERENCE)
    {
        // Warn user about null ref device that can't render anything
		Log::GetLog() << "NULL reference device." << System::CR;
		PreCond("NULL reference device"==NULL);
    }

    DWORD behaviorFlags;
    if (d3dSettings->GetVertexProcessingType() == SOFTWARE_VP)
        behaviorFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
    else if (d3dSettings->GetVertexProcessingType() == MIXED_VP)
        behaviorFlags = D3DCREATE_MIXED_VERTEXPROCESSING;
    else if (d3dSettings->GetVertexProcessingType() == HARDWARE_VP)
        behaviorFlags = D3DCREATE_HARDWARE_VERTEXPROCESSING;
    else if (d3dSettings->GetVertexProcessingType() == PURE_HARDWARE_VP)
        behaviorFlags = D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_PUREDEVICE;
    else
        behaviorFlags = 0; // TODO: throw exception

	Log::GetLog() << "creating device" << System::CR;

    // Create the device
    hr = d3d->CreateDevice(d3dSettings->AdapterOrdinal(), pDeviceInfo->DevType,
                            hWindow, behaviorFlags,&presentParams, &d3dDevice );

    if (SUCCEEDED(hr))
    {
		Log::GetLog() << "device creation successful" << System::CR;

        // When moving from fullscreen to windowed mode, it is important to
        // adjust the window size after recreating the device rather than
        // beforehand to ensure that you get the window size you want.  For
        // example, when switching from 640x480 fullscreen to windowed with
        // a 1000x600 window on a 1024x768 desktop, it is impossible to set
        // the window size to 1000x600 until after the display mode has
        // changed to 1024x768, because windows cannot be larger than the
        // desktop.
		bool windowed = true;
        if (windowed)
        {
			RECT m_rcWindowBounds;
			GetWindowRect(hWindow,&m_rcWindowBounds);

            SetWindowPos( hWindow, HWND_NOTOPMOST,
                          m_rcWindowBounds.left, m_rcWindowBounds.top,
                          ( m_rcWindowBounds.right - m_rcWindowBounds.left ),
                          ( m_rcWindowBounds.bottom - m_rcWindowBounds.top ),
                          SWP_SHOWWINDOW );
        }

        // Store device Caps
        d3dDevice->GetDeviceCaps( &devCaps );
        dwCreateFlags = behaviorFlags;

        // Store device description
        if( pDeviceInfo->DevType == D3DDEVTYPE_REF )
            lstrcpy( deviceStats, TEXT("REF") );
        else if( pDeviceInfo->DevType == D3DDEVTYPE_HAL )
            lstrcpy( deviceStats, TEXT("HAL") );
        else if( pDeviceInfo->DevType == D3DDEVTYPE_SW )
            lstrcpy( deviceStats, TEXT("SW") );

        if( behaviorFlags & D3DCREATE_HARDWARE_VERTEXPROCESSING &&
            behaviorFlags & D3DCREATE_PUREDEVICE )
        {
            if( pDeviceInfo->DevType == D3DDEVTYPE_HAL )
                lstrcat( deviceStats, TEXT(" (pure hw vp)") );
            else
                lstrcat( deviceStats, TEXT(" (simulated pure hw vp)") );
        }
        else if( behaviorFlags & D3DCREATE_HARDWARE_VERTEXPROCESSING )
        {
            if( pDeviceInfo->DevType == D3DDEVTYPE_HAL )
                lstrcat( deviceStats, TEXT(" (hw vp)") );
            else
                lstrcat( deviceStats, TEXT(" (simulated hw vp)") );
        }
        else if( behaviorFlags & D3DCREATE_MIXED_VERTEXPROCESSING )
        {
            if( pDeviceInfo->DevType == D3DDEVTYPE_HAL )
                lstrcat( deviceStats, TEXT(" (mixed vp)") );
            else
                lstrcat( deviceStats, TEXT(" (simulated mixed vp)") );
        }
        else if( behaviorFlags & D3DCREATE_SOFTWARE_VERTEXPROCESSING )
        {
            lstrcat( deviceStats, TEXT(" (sw vp)") );
        }

        if( pDeviceInfo->DevType == D3DDEVTYPE_HAL )
        {
            // Be sure not to overflow deviceStats when appending the adapter
            // description, since it can be long.  Note that the adapter description
            // is initially CHAR and must be converted to TCHAR.
            lstrcat( deviceStats, TEXT(": ") );
            const int cchDesc = sizeof(pAdapterInfo->AdapterIdentifier.Description);
            TCHAR szDescription[cchDesc];
            ConvertAnsiStringToGenericCch( szDescription,
                pAdapterInfo->AdapterIdentifier.Description, cchDesc );
            int maxAppend = sizeof(deviceStats) / sizeof(TCHAR) - lstrlen( deviceStats ) - 1;
            strncat_s(deviceStats, 90, szDescription, maxAppend );
        }

		Log::GetLog() << "device:"<< deviceStats << System::CR;

        // Store render target surface desc
        LPDIRECT3DSURFACE9 pBackBuffer = NULL;
        d3dDevice->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer );
        pBackBuffer->GetDesc( &d3dsdBackBuffer );
        pBackBuffer->Release();

	    return S_OK;
    }
	else
	{
		if (hr == D3DERR_DRIVERINTERNALERROR)
		{
			Log::GetLog() << "device creation failed - this video card does not support 3d (D3DERR_DRIVERINTERNALERROR)" << System::CR;
		}
		else
		{
			Log::GetLog() << "device creation failed "<< hr << System::CR;
		}
	    return hr;
	}
}

bool Interface::InitialiseDevice(HWND hWindowed, int _width, int _height)
{
	width = _width;
	height = _height;
	return InitialiseDevice(hWindowed);
}

bool Interface::InitialiseDevice(HWND hWindowed,D3DPRESENT_PARAMETERS* present,UINT _adapter,
								 D3DDEVTYPE devType,DWORD behaviour)
{
	HRESULT hr;

	Log::GetLog() << "initialise device" << System::CR;

	PreCond(hWindowed != NULL);

	if (device != NULL)
	{
		device->SetDevice(NULL);
	};
	safe_release(d3dDevice);
	safe_release(d3d);

	// create the d3d ptr
	d3d = Direct3DCreate9(D3D_SDK_VERSION);
	if (!d3d)
	{
		Log::GetLog() << "Could not create Direct3D9 interface, check DirectX 9 is installed." << System::CR;
		return false;
	}

	d3dEnumeration = new CD3DEnumeration();
    d3dEnumeration->SetD3D(d3d);
    d3dEnumeration->ConfirmDeviceCallback = ConfirmDeviceHelper;
    if (FAILED(hr=d3dEnumeration->Enumerate()))
    {
		Log::GetLog() << "Could not enumerate devices." << System::CR;
		return false;
    }

	if (!FindBestWindowedMode(hWindowed,true,false))
	{
		Log::GetLog() << "Could not find windowed compatible mode." << System::CR;
		return false;
	}

	// can we create a device
	hr = Initialise3DEnvironment(hWindowed);
	if (!SUCCEEDED(hr))
	{
		return false;
	}

	// create the wrapper
	if (device==NULL)
	{
		device=new Device(d3dDevice);
	}
	else
	{
		device->SetDevice(d3dDevice);
	}

	// retrieve the new back buffer
	d3dDevice->GetBackBuffer(0,0,D3DBACKBUFFER_TYPE_MONO,&backBufferSurface);

	// get capabilities
	d3dDevice->GetDeviceCaps(&devCaps);

	// save details
	hwndFocusWindow = hWindowed;
	hwndWindowed = hWindowed;

	adapter = _adapter;
	deviceType = devType;

	device->SetWidthHeightWindowed(presentParams.BackBufferWidth,
								   presentParams.BackBufferHeight,
								   presentParams.Windowed==TRUE);

	deviceReady = true;

	Log::GetLog() << "device is now ready\n";

	return true;
}

bool Interface::IsDeviceOK()
{
	if (deviceReady && d3dDevice != NULL)
	{
		return (d3dDevice->TestCooperativeLevel() == D3D_OK);
	}
	return false;
}

bool Interface::ResetDevice()
{
	deviceReady = false;

	HRESULT hr = d3dDevice->TestCooperativeLevel();
	if (hr == D3DERR_DEVICELOST)
	{
		Log::GetLog() << "device list, releasing resources" << System::CR;
		Sleep(500);

		// invalidate any device objects
		InvalidatableDevice::InvalidateObjects();

		safe_release(backBufferSurface);
		safe_release(depthStencilSurface);
	}
	else if (hr == D3DERR_DEVICENOTRESET)
	{
		Log::GetLog() << "resetting device\n";


		HRESULT hr = d3dDevice->Reset(&presentParams);
		if (FAILED(hr))
		{
			Log::GetLog() << "Could not reset the device, D3D returned: " << hr << System::CR;
			return false;
		}

		d3dDevice->GetBackBuffer(0,0,D3DBACKBUFFER_TYPE_MONO,&backBufferSurface);
		d3dDevice->GetDepthStencilSurface(&depthStencilSurface);

		device->SetWidthHeightWindowed(presentParams.BackBufferWidth,presentParams.BackBufferHeight,
									   presentParams.Windowed==TRUE);

		// restore any device objects
		InvalidatableDevice::RestoreObjects();

		deviceReady=true;
		return true;
	}
	return false;
}

void Interface::ReleaseDevice()
{
	deviceReady = false;

	Log::GetLog() << "releasing device" << System::CR;

	safe_release(d3dDevice);
	safe_release(d3d);
	safe_release(backBufferSurface);
	safe_release(depthStencilSurface);
}

	// present parameters, set resets the device
const D3DPRESENT_PARAMETERS& Interface::GetPresentParams()
{
	return presentParams;
}

void Interface::SetPresentParams(const D3DPRESENT_PARAMETERS& p)
{
	Log::GetLog() << "setting present parameters" << System::CR;

	presentParams = p;
	device->SetWidthHeightWindowed(presentParams.BackBufferWidth,presentParams.BackBufferHeight,
								   presentParams.Windowed==TRUE);
	ResetDevice();
}

const D3DCAPS9& Interface::GetCapabilities()
{
	return devCaps;
}

bool Interface::ResizeBackBuffer(int width, int height)
{
	if (!deviceReady)
		return false;

	presentParams.BackBufferWidth = width;
	presentParams.BackBufferHeight = height;

	return ResetDevice();
}

bool Interface::IsDeviceReady()
{
	return deviceReady;
}

bool Interface::CheckDeviceFormat(D3DFORMAT fmt, DWORD usage, D3DRESOURCETYPE rtype)
{
	D3DDISPLAYMODE d;
	d3d->GetAdapterDisplayMode(adapter, &d);
	return SUCCEEDED(d3d->CheckDeviceFormat(adapter, deviceType, d.Format, usage,D3DRTYPE_TEXTURE, fmt))==TRUE;
}

