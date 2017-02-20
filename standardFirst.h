#pragma once

//////////////////////////////////////////////////////////
// system

#include <windows.h>
#include <windowsx.h>
#include <winuser.h>
#include <basetsd.h>
#include <commdlg.h>
#include <mmsystem.h>
#include <commctrl.h>
#include <basetsd.h>
#include <direct.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <assert.h>
#include <xmmintrin.h>

//////////////////////////////////////////////////////////
// directX

#define DIRECTINPUT_VERSION	0x0800

#include <d3d9.h>
#include <d3dx9.h>
#include <d3d9caps.h>
#include <d3d9types.h>
#include <d3dx9effect.h>
#include <dsound.h>
#include <dinput.h>

//////////////////////////////////////////////////////////
// stl

#include <string>
#include <vector>
#include <hash_map>
#include <sstream>
#include <ostream>
#include <algorithm>
#include <bitset>
#include <map>

//////////////////////////////////////////////////////////
//! system

#include "d3d9/d3dEnumeration.h"
#include "d3d9/d3dSettings.h"
#include "d3d9/D3DData.h"

#include "system/system.h"
#include "system/exception.h"
#include "system/StringBuilder.h"
#include "system/log.h"
#include "system/macros.h"
#include "system/streamers/fileStreamer.h"
#include "system/timer.h"
#include "system/xml/XmlNode.h"
#include "system/font.h"

//! direct x system
#include "d3d9/device.h"
#include "d3d9/Interface.h"
#include "d3d9/textureCache.h"

#define _ALIGN64 __declspec(align(64))
