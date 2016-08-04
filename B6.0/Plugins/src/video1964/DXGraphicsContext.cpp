/*
Copyright (C) 2003 Rice1964

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include "stdafx.h"
#include "Debugger.h" // To be used for XBOX debug functions

#ifdef _XBOX
#include "../../../config.h"
#include "typedefs.h"
//#include "../../../ingamemenu/panel.h"

#include <xbapp.h>
#include <xbresource.h>

extern void XboxDrawOSD();
extern int AntiAliasMode;
char emuvidname[128];

extern "C" void _INPUT_LoadButtonMap(int *cfgData); 
extern int ControllerConfig[76];
extern void 	CreateRenderTarget();
extern void     SetAsRenderTarget();
extern void     RestoreRenderTarget();
#endif

MYLPDIRECT3DDEVICE g_pD3DDev = NULL;
CD3DDevWrapper    gD3DDevWrapper;
MYD3DCAPS g_D3DDeviceCaps;

int FormatToSize(D3DFORMAT fmt)
{
#ifdef _XBOX
		return 16;
#else
	switch(fmt)
	{
	case D3DFMT_R8G8B8:
	case D3DFMT_A8R8G8B8:
	case D3DFMT_X8R8G8B8:
	case D3DFMT_INDEX32:
	case D3DFMT_D32:
	case D3DFMT_D24S8:
	case D3DFMT_D24X8:
	case D3DFMT_D24X4S4:
	case TEXTURE_FMT_A8R8G8B8:
		return 32;
	default:
		/*
		case D3DFMT_R5G6B5:
		case D3DFMT_X1R5G5B5:
		case D3DFMT_A1R5G5B5:
		case D3DFMT_A4R4G4B4:
		case D3DFMT_A8:
		case D3DFMT_R3G3B2:
		case D3DFMT_A8R3G3B2:
		case D3DFMT_X4R4G4B4:
		case D3DFMT_A8P8:
		case D3DFMT_P8:
		case D3DFMT_L8:
		case D3DFMT_A8L8:
		case D3DFMT_A4L4:
		case D3DFMT_INDEX16:
		case D3DFMT_D16_LOCKABLE:
		case D3DFMT_D15S1:
		case D3DFMT_D16:
		*/
		return 16;

	}
#endif
}

#ifdef _XBOX
//-----------------------------------------------------------------------------
// Name: SetAntiAliasMode()
// Desc: Surreal64 function to set the antialiasing mode determined by the
//       Launcher. Edge AntiAliasing may work better on the xbox compared to
//	     the FSAA modes. 4x Gaussian is the reccomended then to 2x Quincunx if 
//	     the framerate plunges. Linear modes are also available if those
//	     methods are prefered.
//-----------------------------------------------------------------------------
DWORD SetAntiAliasMode(int AAMode){
	DWORD useAAMode;
	switch (AAMode)
	{
		case 0:
			useAAMode = D3DMULTISAMPLE_NONE;
		break;

		case 1:
			useAAMode = D3DMULTISAMPLE_NONE;
		break;

		case 2:
			useAAMode = D3DMULTISAMPLE_2_SAMPLES_MULTISAMPLE_LINEAR;
		break;

		case 3:
			useAAMode = D3DMULTISAMPLE_2_SAMPLES_MULTISAMPLE_QUINCUNX;
		break;

		case 4:
			useAAMode = D3DMULTISAMPLE_4_SAMPLES_MULTISAMPLE_LINEAR;
		break;

		case 5:
			useAAMode = D3DMULTISAMPLE_4_SAMPLES_MULTISAMPLE_GAUSSIAN;
		break;
	}
	return useAAMode;
}
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
int				CDXGraphicsContext::m_dwNumAdapters;
D3DAdapterInfo	CDXGraphicsContext::m_1stAdapters;
MYD3DCAPS		CDXGraphicsContext::m_d3dCaps;           // Caps for the device
bool			CDXGraphicsContext::m_bSupportAnisotropy;
#ifdef _XBOX
const	uint32		dwNumDeviceTypes = 1;
#else //win32
const	uint32		dwNumDeviceTypes = 2;
#endif //_XBOX
extern const char*	strDXDeviceDescs[];
const	D3DDEVTYPE	DeviceTypes[] = { D3DDEVTYPE_HAL, D3DDEVTYPE_REF };


CDXGraphicsContext::CDXGraphicsContext() :
	m_pd3dDevice(NULL),
	m_pD3D(NULL),
#ifndef _XBOX
	m_hFont(NULL),
	m_pID3DFont(NULL),
#endif
	m_dwAdapter(0),			// Default Adapter
	m_dwCreateFlags(0),
	m_dwMinDepthBits(16),
	m_dwMinStencilBits(0),
	m_desktopFormat(D3DFMT_A8R8G8B8),
	m_FSAAIsEnabled(false),
	m_bFontIsCreated(false)
{
	m_strDeviceStats[0] = '\0';
#ifndef _RICE6FB
	memset(&m_textureColorBufferInfo, 0, sizeof(TextureBufferShortInfo));
	memset(&m_textureDepthBufferInfo, 0, sizeof(TextureBufferShortInfo));
#endif
}

//*****************************************************************************
//
//*****************************************************************************
CDXGraphicsContext::~CDXGraphicsContext()
{
#ifndef _XBOX
	if( m_hFont )	DeleteObject(m_hFont);
#endif
	CGraphicsContext::Get()->CleanUp();
}

//*****************************************************************************
//
//*****************************************************************************
void CDXGraphicsContext::Clear(ClearFlag dwFlags, uint32 color, float depth)
{
	uint32 flag=0;
	if( dwFlags&CLEAR_COLOR_BUFFER )	flag |= D3DCLEAR_TARGET;
	if( dwFlags&CLEAR_DEPTH_BUFFER )	flag |= D3DCLEAR_ZBUFFER;
	Lock();
	if (m_pd3dDevice != NULL)
		m_pd3dDevice->Clear(0, NULL, flag, color, depth, 0);
	Unlock();
}

//*****************************************************************************
//
//*****************************************************************************
bool FrameBufferInRDRAMCheckCRC();
void ClearFrameBufferToBlack(uint32 left=0, uint32 top=0, uint32 width=0, uint32 height=0);
bool ProcessFrameWriteRecord();
extern RECT frameWriteByCPURect;

void CDXGraphicsContext::UpdateFrame(bool swaponly)
{
	HRESULT hr;

	status.gFrameCount++;
#ifndef _RICE560
	if( CRender::g_pRender )	
	{
		CRender::g_pRender->BeginRendering();
	}
#endif

#ifdef _RICE6FB
	g_pFrameBufferManager->UpdateFrameBufferBeforeUpdateFrame();
#else
	CGraphicsContext::UpdateFrameBufferBeforeUpdateFrame();
#endif

	if( options.bDisplayOnscreenFPS && !status.bDisableFPS )
	{

		char str[256];
		RECT rect={windowSetting.uDisplayWidth-100,windowSetting.toolbarHeight+windowSetting.uDisplayHeight-100,windowSetting.uDisplayWidth-10,windowSetting.toolbarHeight+windowSetting.uDisplayHeight};
		if( (options.bDisplayOnscreenFPS == ONSCREEN_DISPLAY_DLIST_PER_SECOND ||
			options.bDisplayOnscreenFPS == ONSCREEN_DISPLAY_DLIST_PER_SECOND_WITH_CORE_MSG ) && windowSetting.dps > 0 )
		{
			sprintf(str,"%.1f dps", windowSetting.dps);
			DrawText(str, rect,2);
		}
		else if( (options.bDisplayOnscreenFPS == ONSCREEN_DISPLAY_FRAME_PER_SECOND ||
			options.bDisplayOnscreenFPS == ONSCREEN_DISPLAY_FRAME_PER_SECOND_WITH_CORE_MSG ) && windowSetting.fps > 0 )
		{
			sprintf(str,"%.1f fps", windowSetting.fps);
			DrawText(str, rect,2);
		}
	
		if( options.bDisplayOnscreenFPS >= ONSCREEN_DISPLAY_TEXT_FROM_CORE_ONLY && status.CPUCoreMsgIsSet && strlen(status.CPUCoreMsgToDisplay) > 0 && windowSetting.dps > 0 )
		{
			RECT rect2={10,windowSetting.toolbarHeight+windowSetting.uDisplayHeight-100,windowSetting.uDisplayWidth-100,windowSetting.toolbarHeight+windowSetting.uDisplayHeight};
			DrawText(status.CPUCoreMsgToDisplay, rect2,1);
		}
	}
#ifndef _RICE560 // missing in Rice 5.60
	if( CRender::g_pRender )
	{
		CRender::g_pRender->EndRendering();
	}
#endif

#ifdef _XBOX
	// Free Textures
	gTextureManager.FreeTextures();

	if( !g_curRomInfo.bForceScreenClear )	
			Clear(CLEAR_DEPTH_BUFFER);
	XboxDrawOSD();
#ifndef _MUDLORD_SAVEBB		
	m_backBufferIsSaved = false;
#endif
	//Unlock();
	m_pd3dDevice->Present( NULL, NULL, NULL, NULL );
	status.bScreenIsDrawn = false;
	if( g_curRomInfo.bForceScreenClear )	needCleanScene = true;
#else
	
	Lock();
	if (m_pd3dDevice == NULL)
	{
		hr = E_FAIL;
	}
	else
	{
		// Test the cooperative level to see if it's okay to render
		if( FAILED( hr = m_pd3dDevice->TestCooperativeLevel() ) )

		{
			// If the device was lost, do not render until we get it back
			if( hr == D3DERR_DEVICELOST )
			{
				hr = S_OK;
				goto exit;
			}
			
			// Check if the device needs to be resized.
			if( hr == D3DERR_DEVICENOTRESET )
			{
				// If we are windowed, read the desktop mode and use the same format for
				// the back buffer
				//if( m_bWindowed )
				//{
				//	D3DAdapterInfo* pAdapterInfo = &m_Adapters[m_dwAdapter];
				//	m_pD3D->GetAdapterDisplayMode( m_dwAdapter, &pAdapterInfo->d3ddmDesktop );
				//	m_d3dpp.BackBufferFormat = pAdapterInfo->d3ddmDesktop.Format;
				//}
				
				if( FAILED( hr = ResizeD3DEnvironment() ) )
				{
					goto exit;
				}
			}
			
			// return hr
			goto exit;
		}

		if( !g_curRomInfo.bForceScreenClear )	
			Clear(CLEAR_DEPTH_BUFFER);

		if( m_bWindowed )
		{
			RECT dstrect={0,windowSetting.toolbarHeight,windowSetting.uDisplayWidth,windowSetting.toolbarHeight+windowSetting.uDisplayHeight};
			RECT srcrect={0,0,windowSetting.uDisplayWidth,windowSetting.uDisplayHeight};
			hr = m_pd3dDevice->Present( &srcrect, &dstrect, NULL, NULL );
		}
		else
		{
			hr = m_pd3dDevice->Present( NULL, NULL, NULL, NULL );
		}

#ifndef _MUDLORD_SAVEBB		
		m_backBufferIsSaved = false;
#endif
	}

exit:

	
	Unlock();

	//m_pd3dDevice->Present( NULL, NULL, NULL, NULL ); // Needed? seems to cause issues in windowed mode...
	status.bScreenIsDrawn = false;
	if( g_curRomInfo.bForceScreenClear )	needCleanScene = true;

#endif
}

//-----------------------------------------------------------------------------
// Name: FindDepthStencilFormat()
// Desc: Finds a depth/stencil format for the given device that is compatible
//       with the render target format and meets the needs of the app.
//-----------------------------------------------------------------------------
BOOL CDXGraphicsContext::FindDepthStencilFormat( UINT iAdapter, D3DDEVTYPE DeviceType,
											  D3DFORMAT TargetFormat,
											  D3DFORMAT* pDepthStencilFormat )
{
#ifndef _XBOX
	if( m_dwMinDepthBits <= 16 && m_dwMinStencilBits == 0 )
	{
		if( SUCCEEDED( m_pD3D->CheckDeviceFormat( iAdapter, DeviceType,
			TargetFormat, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D16 ) ) )
		{
			if( SUCCEEDED( m_pD3D->CheckDepthStencilMatch( iAdapter, DeviceType,
				TargetFormat, TargetFormat, D3DFMT_D16 ) ) )
			{
				
				*pDepthStencilFormat = D3DFMT_D16;
				return TRUE;
			}
		}
	}

	if( m_dwMinDepthBits <= 24 && m_dwMinStencilBits <= 8 )
	{
		if( SUCCEEDED( m_pD3D->CheckDeviceFormat( iAdapter, DeviceType,
			TargetFormat, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D24S8 ) ) )
		{
			if( SUCCEEDED( m_pD3D->CheckDepthStencilMatch( iAdapter, DeviceType,
				TargetFormat, TargetFormat, D3DFMT_D24S8 ) ) )
			{
				
				
				*pDepthStencilFormat = D3DFMT_D24S8;
				return TRUE;
				
			}
		}
	}

	
	if( m_dwMinDepthBits <= 16 && m_dwMinStencilBits == 0 )
	{
		if( SUCCEEDED( m_pD3D->CheckDeviceFormat( iAdapter, DeviceType,
			TargetFormat, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D16 ) ) )
		{
			if( SUCCEEDED( m_pD3D->CheckDepthStencilMatch( iAdapter, DeviceType,
				TargetFormat, TargetFormat, D3DFMT_D16 ) ) )
			{
				
				*pDepthStencilFormat = D3DFMT_D16;
				return TRUE;
			}
		}
	}

	if( m_dwMinDepthBits <= 15 && m_dwMinStencilBits <= 1 )
	{
		if( SUCCEEDED( m_pD3D->CheckDeviceFormat( iAdapter, DeviceType,
			TargetFormat, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D15S1 ) ) )
		{
			if( SUCCEEDED( m_pD3D->CheckDepthStencilMatch( iAdapter, DeviceType,
				TargetFormat, TargetFormat, D3DFMT_D15S1 ) ) )
			{
				*pDepthStencilFormat = D3DFMT_D15S1;
				return TRUE;
			}
		}
	}
	
	if( m_dwMinDepthBits <= 24 && m_dwMinStencilBits == 0 )
	{
		if( SUCCEEDED( m_pD3D->CheckDeviceFormat( iAdapter, DeviceType,
			TargetFormat, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D24X8 ) ) )
		{
			if( SUCCEEDED( m_pD3D->CheckDepthStencilMatch( iAdapter, DeviceType,
				TargetFormat, TargetFormat, D3DFMT_D24X8 ) ) )
			{
				*pDepthStencilFormat = D3DFMT_D24X8;
				return TRUE;
			}
		}
	}
	
	if( m_dwMinDepthBits <= 24 && m_dwMinStencilBits <= 8 )
	{
		if( SUCCEEDED( m_pD3D->CheckDeviceFormat( iAdapter, DeviceType,
			TargetFormat, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D24S8 ) ) )
		{
			if( SUCCEEDED( m_pD3D->CheckDepthStencilMatch( iAdapter, DeviceType,
				TargetFormat, TargetFormat, D3DFMT_D24S8 ) ) )
			{
				
				
				*pDepthStencilFormat = D3DFMT_D24S8;
				return TRUE;
				
			}
		}
	}
	
	if( m_dwMinDepthBits <= 24 && m_dwMinStencilBits <= 4 )
	{
		if( SUCCEEDED( m_pD3D->CheckDeviceFormat( iAdapter, DeviceType,
			TargetFormat, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D24X4S4 ) ) )
		{
			if( SUCCEEDED( m_pD3D->CheckDepthStencilMatch( iAdapter, DeviceType,
				TargetFormat, TargetFormat, D3DFMT_D24X4S4 ) ) )
			{
				*pDepthStencilFormat = D3DFMT_D24X4S4;
				return TRUE;
			}
		}
	}
	
	if( m_dwMinDepthBits <= 32 && m_dwMinStencilBits == 0 )
	{
		if( SUCCEEDED( m_pD3D->CheckDeviceFormat( iAdapter, DeviceType,
			TargetFormat, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D32 ) ) )
		{
			if( SUCCEEDED( m_pD3D->CheckDepthStencilMatch( iAdapter, DeviceType,
				TargetFormat, TargetFormat, D3DFMT_D32 ) ) )
			{
				*pDepthStencilFormat = D3DFMT_D32;
				return TRUE;
			}
		}
	}
#else
// Force 16bit for XBOX
	*pDepthStencilFormat = D3DFMT_D16;
	return TRUE;
#endif
	
	return FALSE;
}

//*****************************************************************************
//
//*****************************************************************************
extern void WriteConfiguration(void);
extern "C" void _INPUT_LoadButtonMap(int *cfgData); 
extern int ControllerConfig[76];

bool CDXGraphicsContext::Initialize(HWND hWnd, HWND hWndStatus,
									 uint32 dwWidth, uint32 dwHeight,
									 BOOL bWindowed )
{
	HRESULT hr;
#ifndef _XBOX
	if( g_GraphicsInfo.hStatusBar )
	{
		SetWindowText(g_GraphicsInfo.hStatusBar,"Initializing DirectX Device, please wait");
	}
#endif

	Lock();

	  // Create the Direct3D object
#if DX_VERSION == 8
	m_pD3D = Direct3DCreate8( D3D_SDK_VERSION );
#elif DX_VERSION == 9
	m_pD3D = Direct3DCreate9( D3D_SDK_VERSION );
#endif
	if( m_pD3D == NULL )
	{
		Unlock();
		DisplayD3DErrorMsg( D3DAPPERR_NODIRECT3D, MSGERR_APPMUSTEXIT );
		return false;
	}

	// Build a list of Direct3D adapters, modes and devices. The
    // ConfirmDevice() callback is used to confirm that only devices that
    // meet the app's requirements are considered.
    if( FAILED( hr = BuildDeviceList() ) )
    {
        if ( m_pD3D )
		{
			m_pD3D->Release();
			m_pD3D = NULL;
		}
		Unlock();
        DisplayD3DErrorMsg( hr, MSGERR_APPMUSTEXIT );
		return false;
    }

	CGraphicsContext::Initialize(hWnd, hWndStatus, dwWidth, dwHeight, bWindowed );
	
	hr = Create3D( bWindowed );

	// Clear/Update a few times to ensure that all of the buffers are cleared
	if ( m_pd3dDevice )
	{
		for (int i=0; i<DirectXRenderBufferSettings[options.RenderBufferSetting].number+1; i++ )
		{
			Clear(CLEAR_COLOR_AND_DEPTH_BUFFER);
			m_pd3dDevice->BeginScene();
			m_pd3dDevice->EndScene();
			UpdateFrame();
		}
	}

	MYD3DVIEWPORT vp = {
		0, 
			windowSetting.toolbarHeightToUse, 
			windowSetting.uDisplayWidth, 
			windowSetting.uDisplayHeight, 0, 1
	};
	gD3DDevWrapper.SetViewport(&vp);
	
	Unlock();

	g_pD3DDev->GetDeviceCaps(&g_D3DDeviceCaps);
	CGraphicsContext::Get()->m_supportTextureMirror = false;
	if( g_D3DDeviceCaps.TextureAddressCaps & D3DPTADDRESSCAPS_MIRROR )
	{
		CGraphicsContext::Get()->m_supportTextureMirror = true;
	}
	
	// DIRECTX8 Force to use Software T&L, Not Supported on XBOX
#if DX_VERSION == 8 && !defined(_XBOX)
	if( options.bForceSoftwareTnL )
		gD3DDevWrapper.SetRenderState(D3DRS_SOFTWAREVERTEXPROCESSING,TRUE); // This is not supported on XBOX
	else
		gD3DDevWrapper.SetRenderState(D3DRS_SOFTWAREVERTEXPROCESSING,FALSE);

	// DIRECTX9 Force to use Software T&L
#elif DX_VERSION == 9
	if( options.bForceSoftwareTnL )
		g_pD3DDev->SetSoftwareVertexProcessing(TRUE);
	else
		g_pD3DDev->SetSoftwareVertexProcessing(FALSE);
#endif

#ifndef _XBOX
	if( g_GraphicsInfo.hStatusBar )
	{
		SetWindowText(g_GraphicsInfo.hStatusBar,"DirectX device is ready");
	}
#else


	// GogoAckman
	g_pd3dDevice = g_pD3DDev;
	sprintf(emuvidname,"Video 1964");
    //d3dpp = m_d3dpp;

	//CreateRenderTarget();
	_INPUT_LoadButtonMap(ControllerConfig);
#endif

	return hr==S_OK;
}


//-----------------------------------------------------------------------------
// Name: SortModesCallback()
// Desc: Callback function for sorting display modes (used by BuildDeviceList).
//-----------------------------------------------------------------------------
static int _cdecl SortModesCallback( const VOID* arg1, const VOID* arg2 )
{
	D3DDISPLAYMODE* p1 = (D3DDISPLAYMODE*)arg1;
	D3DDISPLAYMODE* p2 = (D3DDISPLAYMODE*)arg2;

	if( p1->Format > p2->Format )   return -1;
	if( p1->Format < p2->Format )   return +1;
	if( p1->Width  < p2->Width )    return -1;
	if( p1->Width  > p2->Width )    return +1;
	if( p1->Height < p2->Height )   return -1;
	if( p1->Height > p2->Height )   return +1;
//#endif //_XBOX

	return 0;
}

// This is a static function, will be called when the plugin DLL is initialized
void CDXGraphicsContext::InitDeviceParameters()
{
#ifndef _XBOX
	SetWindowText(m_hWndStatus, "Initialize DirectX Device");
#endif

	// Create Direct3D object
	MYLPDIRECT3D pD3D;
#if DX_VERSION == 8
	pD3D = Direct3DCreate8( D3D_SDK_VERSION );
#elif DX_VERSION == 9
	pD3D = Direct3DCreate9( D3D_SDK_VERSION );
#endif
	if( pD3D == NULL )
	{
        DisplayD3DErrorMsg( D3DAPPERR_NODIRECT3D, MSGERR_APPMUSTEXIT );
		return;
	}

	// Get number of adapters
	int numAvailableAdapters = pD3D->GetAdapterCount();

	// For the 1st adapter, looking through each of its devices
	int iAdapter=0;
	D3DAdapterInfo* pAdapter  = &m_1stAdapters;
	pD3D->GetAdapterIdentifier( iAdapter, 0, &pAdapter->d3dAdapterIdentifier );
	pD3D->GetAdapterDisplayMode( iAdapter, &pAdapter->d3ddmDesktop );	// Get current display mode

	// Get number of devices for each adapters
	pAdapter->dwNumDevices    = dwNumDeviceTypes;
	pAdapter->dwCurrentDevice = 0;

	// Add devices to adapter
	for( UINT iDevice = 0; iDevice < dwNumDeviceTypes; iDevice++ )
	{
		// Fill in device info
		D3DDeviceInfo* pDevice	= &pAdapter->devices[iDevice];
		pDevice->DeviceType     = DeviceTypes[iDevice];
		pDevice->strDesc        = strDXDeviceDescs[iDevice];

		// for each devices in each adapter
		// Get device caps
		pD3D->GetDeviceCaps( iAdapter, DeviceTypes[iDevice], &pDevice->d3dCaps );

		pDevice->dwNumModes     = 0;
#ifdef _XBOX
		pDevice->MultiSampleType = SetAntiAliasMode(AntiAliasMode);
#else
		pDevice->MultiSampleType = D3DMULTISAMPLE_NONE;
#endif
	}

	// Check FSAA maximum
	for( m_maxFSAA = 16; m_maxFSAA >= 2; m_maxFSAA-- )
	{
#if DX_VERSION == 8
		if( SUCCEEDED(pD3D->CheckDeviceMultiSampleType( D3DADAPTER_DEFAULT, 
			D3DDEVTYPE_HAL , D3DFMT_X8R8G8B8, FALSE, D3DMULTISAMPLE_TYPE(D3DMULTISAMPLE_NONE+m_maxFSAA) ) ) )
#elif DX_VERSION == 9
		if( SUCCEEDED(pD3D->CheckDeviceMultiSampleType( D3DADAPTER_DEFAULT, 
			D3DDEVTYPE_HAL , D3DFMT_X8R8G8B8, FALSE, D3DMULTISAMPLE_TYPE(D3DMULTISAMPLE_NONE+m_maxFSAA), NULL ) ) )
#endif
		{
			break;
		}
		else
		{
			continue;
		}
	}

	if( m_maxFSAA < 2 )
	{
		m_maxFSAA = 0;
	}
	else
	{
	}
	if( options.DirectXMaxFSAA != m_maxFSAA )
	{
		options.DirectXMaxFSAA = m_maxFSAA;
		WriteConfiguration();
	}


	// Check Anisotropy Filtering maximum

	m_maxAnisotropy = pAdapter->devices[0].d3dCaps.MaxAnisotropy;
	if( options.DirectXMaxAnisotropy != m_maxAnisotropy )
	{
		options.DirectXMaxAnisotropy = m_maxAnisotropy;
		WriteConfiguration();
	}

	// Get available refresh rates

	// Get available color quality, 16 bit and 32 bit

	// Get available back buffer format

	// Get available depth buffer format

	// Release the Direct3D object
	pD3D->Release();
}


//*****************************************************************************
//
//*****************************************************************************
HRESULT CDXGraphicsContext::Create3D( BOOL bWindowed )
{
    HRESULT hr;
	
    // Get access to current adapter, device, and mode
    D3DAdapterInfo* pAdapterInfo = &m_Adapters[m_dwAdapter];
    //D3DDeviceInfo*  pDeviceInfo  = &pAdapterInfo->devices[pAdapterInfo->dwCurrentDevice];
	D3DDeviceInfo*  pDeviceInfo  = &pAdapterInfo->devices[options.DirectXDevice];
	
 	m_bWindowed = (bWindowed==TRUE);		// Make user Toggle manually for now!
    pDeviceInfo->bWindowed = m_bWindowed;

    // Initialize the 3D environment for the app
    if( FAILED( hr = InitializeD3D() ) )
    {
        SAFE_RELEASE( m_pD3D );
        return DisplayD3DErrorMsg( hr, MSGERR_APPMUSTEXIT );
    }
	
    // The app is ready to go
    m_bReady = true;
	
    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: InitializeD3D()
// Desc:
//-----------------------------------------------------------------------------

HRESULT CDXGraphicsContext::InitializeD3D()
{
	HRESULT hr;

    // Set up the presentation parameters
    ZeroMemory( &m_d3dpp, sizeof(m_d3dpp) );
#ifdef _XBOX
	windowSetting.statusBarHeightToUse = 0;
	windowSetting.toolbarHeightToUse = 0;

	m_d3dpp.Windowed               = FALSE;
	m_d3dpp.BackBufferCount        = 1;
	m_d3dpp.BackBufferWidth	= 640;
	m_d3dpp.BackBufferHeight = 480;
	m_d3dpp.BackBufferFormat =  D3DFMT_A8R8G8B8;//D3DFMT_LIN_R5G6B5;
	m_d3dpp.MultiSampleType        = SetAntiAliasMode(AntiAliasMode);
	//m_d3dpp.SwapEffect             = bufferSettings[curBufferSetting].swapEffect;
	m_d3dpp.SwapEffect             =  D3DSWAPEFFECT_COPY;
	m_d3dpp.EnableAutoDepthStencil = TRUE; /*m_bUseDepthBuffer;*/
	//m_d3dpp.AutoDepthStencilFormat = pModeInfo->DepthStencilFormat;
	m_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	m_d3dpp.hDeviceWindow          = m_hWnd;

	switch (VSync){
		case 0 : 	m_d3dpp.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
			break;
		case 1 : 	m_d3dpp.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_ONE;
			break;
		case 2 : 	m_d3dpp.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_ONE_OR_IMMEDIATE;
			break;
/*		case 2 : 	m_d3dpp.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_TWO;
			break;
		case 3 : 	m_d3dpp.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_THREE;
			break;
		case 4 : 	m_d3dpp.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
			break;
		case 5 : 	m_d3dpp.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_ONE_OR_IMMEDIATE;
			break;
		case 6 : 	m_d3dpp.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_TWO_OR_IMMEDIATE;
			break;
		case 7 : 	m_d3dpp.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_THREE_OR_IMMEDIATE;
			break;*/
	}
	m_d3dpp.FullScreen_RefreshRateInHz = 60;
	

	DWORD videoFlags = XGetVideoFlags();
	if(XGetVideoStandard() == XC_VIDEO_STANDARD_PAL_I)
	{
		if(videoFlags & XC_VIDEO_FLAGS_PAL_60Hz)		// PAL 60 user
			m_d3dpp.FullScreen_RefreshRateInHz = 60;
		else
			m_d3dpp.FullScreen_RefreshRateInHz = 50;
	}

	//Widescreen
	if((videoFlags & XC_VIDEO_FLAGS_WIDESCREEN) !=0)
	 {
		m_d3dpp.Flags = D3DPRESENTFLAG_WIDESCREEN;
	 }
	
	 if(XGetAVPack() == XC_AV_PACK_HDTV){
		//720p
		if( videoFlags & XC_VIDEO_FLAGS_HDTV_720p && bEnableHDTV){
			m_d3dpp.BackBufferWidth = 1280;
			m_d3dpp.BackBufferHeight = 720;
			m_d3dpp.Flags = D3DPRESENTFLAG_PROGRESSIVE | D3DPRESENTFLAG_WIDESCREEN;
			m_d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
		}
		//480p
		else if( videoFlags & XC_VIDEO_FLAGS_HDTV_480p){
			m_d3dpp.Flags = D3DPRESENTFLAG_PROGRESSIVE;
			m_d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
		}
	 }

	windowSetting.uDisplayWidth = m_d3dpp.BackBufferWidth;
	windowSetting.uDisplayHeight = m_d3dpp.BackBufferHeight;

	m_desktopFormat = D3DFMT_X1R5G5B5;
	//m_desktopFormat = D3DFMT_A8R8G8B8;

	//freakdave
	if(VertexMode == 0){
		// Create the device
		hr = m_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,
			NULL, D3DCREATE_PUREDEVICE, &m_d3dpp,
			&m_pd3dDevice );
	}


	else if(VertexMode == 1){ //Xbox doesn't do Software vertex processing. Do Software Vertex Clipper instead.
		// Create the device
		hr = m_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,
			NULL, D3DCREATE_HARDWARE_VERTEXPROCESSING, &m_d3dpp,
			&m_pd3dDevice );

		options.bForceSoftwareClipper = TRUE;

	}

	else if(VertexMode == 2){
		// Create the device
		hr = m_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,
			NULL, D3DCREATE_HARDWARE_VERTEXPROCESSING, &m_d3dpp,
			&m_pd3dDevice );

	}

	else if(VertexMode == 3){
		// Create the device
		hr = m_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,
			NULL, D3DCREATE_MIXED_VERTEXPROCESSING, &m_d3dpp,
			&m_pd3dDevice );

	}
#else
	m_d3dpp.Windowed               = m_bWindowed;
	m_d3dpp.BackBufferCount        = DirectXRenderBufferSettings[options.RenderBufferSetting].number;
	m_d3dpp.EnableAutoDepthStencil = TRUE; /*m_bUseDepthBuffer;*/
	//m_d3dpp.AutoDepthStencilFormat = pModeInfo->DepthStencilFormat;
	m_d3dpp.AutoDepthStencilFormat = (D3DFORMAT)(DirectXDepthBufferSetting[options.DirectXDepthBufferSetting].number);
	m_d3dpp.hDeviceWindow          = m_hWnd;
	m_d3dpp.Flags				   = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;

	m_d3dpp.MultiSampleType        = D3DMULTISAMPLE_NONE;
	m_FSAAIsEnabled = false;
	if (options.DirectXAntiAliasingValue != 0)
	{
		m_d3dpp.MultiSampleType = (D3DMULTISAMPLE_TYPE)(D3DMULTISAMPLE_NONE + (int)options.DirectXAntiAliasingValue);
		m_FSAAIsEnabled = true;
	}

	if( currentRomOptions.N64FrameBufferEmuType != FRM_BUF_NONE && m_FSAAIsEnabled )
	{
		m_FSAAIsEnabled = false;
		m_d3dpp.MultiSampleType        = D3DMULTISAMPLE_NONE;
		SetWindowText(g_GraphicsInfo.hStatusBar, "FSAA is turned off in order to use BackBuffer emulation");
	}

    if( m_bWindowed )
    {

		if( !m_FSAAIsEnabled )
			m_d3dpp.SwapEffect		= D3DSWAPEFFECT_COPY;	// Always use COPY for window mode
		else
			m_d3dpp.SwapEffect		= D3DSWAPEFFECT_DISCARD;	// Anti-Aliasing mode
		windowSetting.uDisplayWidth = windowSetting.uWindowDisplayWidth;
		windowSetting.uDisplayHeight= windowSetting.uWindowDisplayHeight;

		m_d3dpp.BackBufferFormat	= D3DFMT_X8R8G8B8;
		m_d3dpp.FullScreen_RefreshRateInHz = 0;

    }
    else
    {
		if( !m_FSAAIsEnabled )
			m_d3dpp.SwapEffect		= (D3DSWAPEFFECT)DirectXRenderBufferSettings[options.RenderBufferSetting].setting;
		else
			m_d3dpp.SwapEffect		= D3DSWAPEFFECT_DISCARD;	// Anti-Aliasing mode
		windowSetting.uDisplayWidth = windowSetting.uFullScreenDisplayWidth;
		windowSetting.uDisplayHeight = windowSetting.uFullScreenDisplayHeight;
        //m_d3dpp.BackBufferFormat	= pAdapterInfo->d3ddmDesktop.Format;
        m_d3dpp.BackBufferFormat	= D3DFMT_X8R8G8B8;
		
		m_d3dpp.FullScreen_RefreshRateInHz = windowSetting.uFullScreenRefreshRate;
		/*if( m_d3dpp.FullScreen_RefreshRateInHz > pModeInfo->RefreshRate )
		{
			m_d3dpp.FullScreen_RefreshRateInHz = pModeInfo->RefreshRate;
			windowSetting.uFullScreenRefreshRate = pModeInfo->RefreshRate;
		}*/
    }
	m_desktopFormat = D3DFMT_X8R8G8B8;
	m_d3dpp.BackBufferWidth		= windowSetting.uDisplayWidth;
	m_d3dpp.BackBufferHeight	= windowSetting.uDisplayHeight;

	
    // Create the device
	hr = m_pD3D->CreateDevice( m_dwAdapter, D3DDEVTYPE_HAL,m_hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_PUREDEVICE, &m_d3dpp,	&m_pd3dDevice );
	if( !SUCCEEDED(hr) && m_d3dpp.MultiSampleType == D3DMULTISAMPLE_NONE )
	{
		// Try again without FSAA
		SetWindowText(g_GraphicsInfo.hStatusBar,"Can not initialize DX8, try again without FSAA");
		m_d3dpp.MultiSampleType        = D3DMULTISAMPLE_NONE;
		hr = m_pD3D->CreateDevice( m_dwAdapter, D3DDEVTYPE_HAL,m_hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_PUREDEVICE, &m_d3dpp,	&m_pd3dDevice );
	}
#endif

    if( m_pd3dDevice )
    {
		g_pD3DDev = m_pd3dDevice;
		gD3DDevWrapper.SetD3DDev(m_pd3dDevice);
		
        // When moving from fullscreen to windowed mode, it is important to
        // adjust the window size after recreating the device rather than
        // beforehand to ensure that you get the window size you want.  For
        // example, when switching from 640x480 fullscreen to windowed with
        // a 1000x600 window on a 1024x768 desktop, it is impossible to set
        // the window size to 1000x600 until after the display mode has
        // changed to 1024x768, because windows cannot be larger than the
        // desktop.
		
        // Store device Caps
        m_pd3dDevice->GetDeviceCaps( &m_d3dCaps );

#ifdef _XBOX
		//freakdave
		if(VertexMode == 0){
			m_dwCreateFlags = D3DCREATE_PUREDEVICE;
		}

		else if(VertexMode == 1){
			m_dwCreateFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
		}

		else if(VertexMode == 2){
			m_dwCreateFlags = D3DCREATE_HARDWARE_VERTEXPROCESSING;
		}

		else if(VertexMode == 3){
			m_dwCreateFlags = D3DCREATE_MIXED_VERTEXPROCESSING;
		}
#else
        m_dwCreateFlags = D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_PUREDEVICE;
		
        if( m_bWindowed )
        {
            SetWindowPos( m_hWnd, HWND_NOTOPMOST,
						m_rcWindowBounds.left, m_rcWindowBounds.top,
						( m_rcWindowBounds.right - m_rcWindowBounds.left ),
						( m_rcWindowBounds.bottom - m_rcWindowBounds.top ),
						SWP_SHOWWINDOW );
        }

        // Store device description
		/*
        if( pDeviceInfo->DeviceType == D3DDEVTYPE_REF )
            lstrcpy( m_strDeviceStats, "REF" );
        else if( pDeviceInfo->DeviceType == D3DDEVTYPE_HAL )
            lstrcpy( m_strDeviceStats, "HAL" );
        else if( pDeviceInfo->DeviceType == D3DDEVTYPE_SW )
            lstrcpy( m_strDeviceStats, "SW" );
		
        if( pModeInfo->dwBehavior & D3DCREATE_HARDWARE_VERTEXPROCESSING &&
            pModeInfo->dwBehavior & D3DCREATE_PUREDEVICE )
        {
            if( pDeviceInfo->DeviceType == D3DDEVTYPE_HAL )
                lstrcat( m_strDeviceStats, " (pure hw vp)" );
            else
                lstrcat( m_strDeviceStats, " (simulated pure hw vp)" );
        }
        else if( pModeInfo->dwBehavior & D3DCREATE_HARDWARE_VERTEXPROCESSING )
        {
            if( pDeviceInfo->DeviceType == D3DDEVTYPE_HAL )
                lstrcat( m_strDeviceStats, " (hw vp)" );
            else
                lstrcat( m_strDeviceStats, " (simulated hw vp)" );
        }
        else if( pModeInfo->dwBehavior & D3DCREATE_MIXED_VERTEXPROCESSING )
        {
            if( pDeviceInfo->DeviceType == D3DDEVTYPE_HAL )
                lstrcat( m_strDeviceStats, " (mixed vp)" );
            else
                lstrcat( m_strDeviceStats, " (simulated mixed vp)" );
        }
        else if( pModeInfo->dwBehavior & D3DCREATE_SOFTWARE_VERTEXPROCESSING )
        {
            lstrcat( m_strDeviceStats, " (sw vp)" );
        }
		
        if( pDeviceInfo->DeviceType == D3DDEVTYPE_HAL )
        {
            lstrcat( m_strDeviceStats, ": " );
            lstrcat( m_strDeviceStats, pAdapterInfo->d3dAdapterIdentifier.Description );
        }
		
	
		if ( IsWindow(m_hWndStatus ) )
		{
			SetWindowText(m_hWndStatus, m_strDeviceStats);
		}
		TRACE0(m_strDeviceStats);
		*/
#endif
	
		
        // Store render target surface desc
        MYLPDIRECT3DSURFACE pBackBuffer;
#if DX_VERSION == 8
        m_pd3dDevice->GetBackBuffer( 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer );
#elif DX_VERSION == 9
		m_pd3dDevice->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer );
#endif
        pBackBuffer->GetDesc( &m_d3dsdBackBuffer );
        pBackBuffer->Release();
		
        // Initialize the app's device-dependent objects
        hr = InitDeviceObjects();
        if( IsResultGood(hr,true) )
        {
			if ( CRender::IsAvailable() )
			{
				CRender::GetRender()->InitDeviceObjects();
			}
            m_bActive = true;
            return S_OK;
        }
		
#ifndef _RICE560
				// Cleanup before we try again (- shouldn't get here)
        CleanDeviceObjects();
#endif
        SAFE_RELEASE( m_pd3dDevice );
    }
	else
	{
#ifndef _XBOX
		if( status.ToToggleFullScreen || !m_bWindowed )
			SetWindowText(g_GraphicsInfo.hStatusBar,"Can not initialize DX8, check your Direct settings");
		else
			MsgInfo("Can not initialize DX8, check your Direct settings");
#endif
	}
	
	/*
	extern MYLPDIRECT3DSURFACE g_pLockableBackBuffer;
	if( g_pLockableBackBuffer == NULL )
	{
		if( IsResultGood(g_pD3DDev->CreateDepthStencilSurface(windowSetting.uDisplayWidth, windowSetting.uDisplayHeight, D3DFMT_D16_LOCKABLE, D3DMULTISAMPLE_NONE, &g_pLockableBackBuffer)) && g_pLockableBackBuffer )
		{
			g_pD3DDev->SetRenderTarget(NULL, g_pLockableBackBuffer);
		}
		else
		{
			if( g_pLockableBackBuffer )
			{
				g_pLockableBackBuffer->Release();
				g_pLockableBackBuffer = NULL;
			}
		}
	}
	*/

    // If that failed, fall back to the reference rasterizer (removed)
	
    return hr;
}


//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------
HRESULT CDXGraphicsContext::ResizeD3DEnvironment()
{
    HRESULT hr;
	
    // Release all vidmem objects
    if( FAILED( hr = CleanDeviceObjects() ) )
        return hr;
	
    // Reset the device
    if( IsResultGood( hr = m_pd3dDevice->Reset( &m_d3dpp ), true ) )
	{
		while ( hr == D3DERR_DEVICELOST )
		{
			Pause(true);
			hr = m_pd3dDevice->Reset( &m_d3dpp );
		}
	}
	else
	{
        return hr;
	}
	
    // Store render target surface desc
    MYLPDIRECT3DSURFACE pBackBuffer;
#if DX_VERSION == 8
    m_pd3dDevice->GetBackBuffer( 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer );
#elif DX_VERSION == 9
	m_pd3dDevice->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer );
#endif
    pBackBuffer->GetDesc( &m_d3dsdBackBuffer );
    pBackBuffer->Release();
	
    // Initialize the app's device-dependent objects
	if ( CRender::IsAvailable() )
	{
		CRender::GetRender()->InitDeviceObjects();
	}
	
    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: ToggleFullScreen()
// Desc: Called when user toggles between fullscreen mode and windowed mode
//-----------------------------------------------------------------------------
HRESULT CDXGraphicsContext::DoToggleFullscreen()
{
#ifndef _XBOX
    // Get access to current adapter, device, and mode
	D3DAdapterInfo* pAdapterInfo = &m_Adapters[m_dwAdapter];
	//D3DDeviceInfo*  pDeviceInfo  = &pAdapterInfo->devices[pAdapterInfo->dwCurrentDevice];
	D3DDeviceInfo*  pDeviceInfo  = &pAdapterInfo->devices[options.DirectXDevice];
    D3DModeInfo*    pModeInfo    = &pDeviceInfo->modes[FindCurrentDisplayModeIndex()];
	
    // Need device change if going windowed and the current device
    // can only be fullscreen

   if( !m_bWindowed && !pDeviceInfo->bCanDoWindowed )
	{
		return ForceWindowed();
	}
	
    m_bReady = false;
	
    // Toggle the windowed state
    m_bWindowed = !m_bWindowed;
    pDeviceInfo->bWindowed = m_bWindowed;
	
    // Prepare window for windowed/fullscreen change
    AdjustWindowForChange();
	
	Lock();
	CRender::GetRender()->CleanUp();
	CleanUp();
#if DX_VERSION == 8
    m_pD3D = Direct3DCreate8( D3D_SDK_VERSION );
#elif DX_VERSION == 9
	m_pD3D = Direct3DCreate9( D3D_SDK_VERSION );
#endif
    if( m_pD3D == NULL )
	{
		Unlock();
        return DisplayD3DErrorMsg( D3DAPPERR_NODIRECT3D, MSGERR_APPMUSTEXIT );
	}
	InitializeD3D();
	Unlock();

	
    // When moving from fullscreen to windowed mode, it is important to
    // adjust the window size after resetting the device rather than
    // beforehand to ensure that you get the window size you want.  For
    // example, when switching from 640x480 fullscreen to windowed with
    // a 1000x600 window on a 1024x768 desktop, it is impossible to set
    // the window size to 1000x600 until after the display mode has
    // changed to 1024x768, because windows cannot be larger than the
    // desktop.
    if( m_bWindowed )
    {

        SetWindowPos( m_hWnd, HWND_NOTOPMOST,
			m_rcWindowBounds.left, m_rcWindowBounds.top,
			( m_rcWindowBounds.right - m_rcWindowBounds.left ),
			( m_rcWindowBounds.bottom - m_rcWindowBounds.top ),
			SWP_SHOWWINDOW );

    }
#endif	
    m_bReady = true;
	
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: ForceWindowed()
// Desc: Switch to a windowed mode, even if that means picking a new device
//       and/or adapter
//-----------------------------------------------------------------------------

HRESULT CDXGraphicsContext::ForceWindowed()
{
#ifndef _XBOX // Using Rice 5.60 code below, check against 6.11
    HRESULT hr;
    D3DAdapterInfo* pAdapterInfoCur = &m_Adapters[m_dwAdapter];
    //D3DDeviceInfo*  pDeviceInfoCur  = &pAdapterInfoCur->devices[pAdapterInfoCur->dwCurrentDevice];
	D3DDeviceInfo*  pDeviceInfoCur  = &pAdapterInfoCur->devices[options.DirectXDevice];
    BOOL bFoundDevice = FALSE;
	
    if( pDeviceInfoCur->bCanDoWindowed )
    {
        bFoundDevice = TRUE;
    }
    else
    {
        // Look for a windowable device on any adapter
        D3DAdapterInfo* pAdapterInfo;
        int dwAdapter;
        D3DDeviceInfo* pDeviceInfo;
        int dwDevice;
        for( dwAdapter = 0; dwAdapter < m_dwNumAdapters; dwAdapter++ )
        {
            pAdapterInfo = &m_Adapters[dwAdapter];
            for( dwDevice = 0; dwDevice < pAdapterInfo->dwNumDevices; dwDevice++ )
            {
                pDeviceInfo = &pAdapterInfo->devices[dwDevice];
                if( pDeviceInfo->bCanDoWindowed )
                {
                    m_dwAdapter = dwAdapter;
                    pDeviceInfoCur = pDeviceInfo;
                    pAdapterInfo->dwCurrentDevice = dwDevice;
                    bFoundDevice = TRUE;
                    break;
                }
            }
            if( bFoundDevice )
                break;
        }
    }
	
    if( !bFoundDevice )
        return E_FAIL;
	
    pDeviceInfoCur->bWindowed = true;
    m_bWindowed = true;
	
    // Now destroy the current 3D device objects, then reinitialize
	
    m_bReady = false;
	
    // Release all scene objects that will be re-created for the new device
    CleanDeviceObjects();
	
    // Release display objects, so a new device can be created
	LONG nRefCount = m_pd3dDevice->Release();
    if( nRefCount > 0L )
	{
        return DisplayD3DErrorMsg( D3DAPPERR_NONZEROREFCOUNT, MSGERR_APPMUSTEXIT );
	}
	
    // Create the new device
    if( FAILED( hr = InitializeD3D() ) )
        return DisplayD3DErrorMsg( hr, MSGERR_APPMUSTEXIT );
    m_bReady = true;
	
    return S_OK;

#else // Rice 5.60 code below

    return S_OK;
#endif // Rice 5.60
}



//-----------------------------------------------------------------------------
// Name: AdjustWindowForChange()
// Desc: Prepare the window for a possible change between windowed mode and
//       fullscreen mode.  This function is virtual and thus can be overridden
//       to provide different behavior, such as switching to an entirely
//       different window for fullscreen mode (as in the MFC sample apps).
//-----------------------------------------------------------------------------
HRESULT CDXGraphicsContext::AdjustWindowForChange()
{
#ifndef _XBOX // XBOX will never need to switch between FullScreen and windowed mode
    if( m_bWindowed )
    {
        // Set windowed-mode style - but disable resizing
        SetWindowLong( m_hWnd, GWL_STYLE, m_dwWindowStyle & (~(WS_THICKFRAME|WS_MAXIMIZEBOX)) );

		if ( IsWindow(m_hWndStatus) )
		{
			SetWindowLong( m_hWndStatus, GWL_STYLE, m_dwStatusWindowStyle & (~SBARS_SIZEGRIP));
		}
    }
    else
    {
        // Set fullscreen-mode style
        SetWindowLong( m_hWnd, GWL_STYLE, WS_POPUP|WS_SYSMENU|WS_VISIBLE );
    }
#endif
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: BuildDeviceList()
// Desc: From DX8 SDK Copyright (c) 1998-2000 Microsoft
//-----------------------------------------------------------------------------
HRESULT CDXGraphicsContext::BuildDeviceList()
{
#ifndef _XBOX //win32
    BOOL bHALExists = FALSE;
    BOOL bHALIsWindowedCompatible = FALSE;
    BOOL bHALIsDesktopCompatible = FALSE;
    BOOL bHALIsSampleCompatible = FALSE;

	m_dwNumAdapters = 0;
	
    // Loop through all the adapters on the system (usually, there's just one
    // unless more than one graphics card is present).
    for( UINT iAdapter = 0; iAdapter < m_pD3D->GetAdapterCount(); iAdapter++ )
    {
        // Fill in adapter info
        D3DAdapterInfo* pAdapter  = &m_Adapters[m_dwNumAdapters];
        m_pD3D->GetAdapterIdentifier( iAdapter, 0, &pAdapter->d3dAdapterIdentifier );
        m_pD3D->GetAdapterDisplayMode( iAdapter, &pAdapter->d3ddmDesktop );
        pAdapter->dwNumDevices    = 0;
        pAdapter->dwCurrentDevice = 0;
		
		//
        // Enumerate all display modes on this adapter
		//
        D3DDISPLAYMODE modes[100];
		D3DFORMAT      formats[20];
        uint32 dwNumFormats      = 0;
        uint32 dwNumModes        = 0;
#if DX_VERSION == 8        
		uint32 dwNumAdapterModes = m_pD3D->GetAdapterModeCount( iAdapter );
#elif DX_VERSION == 9
		uint32 dwNumAdapterModes = m_pD3D->GetAdapterModeCount( iAdapter, pAdapter->d3ddmDesktop.Format );
#endif
		
        // Add the adapter's current desktop format to the list of formats
        formats[dwNumFormats++] = pAdapter->d3ddmDesktop.Format;
		
        for( UINT iMode = 0; iMode < dwNumAdapterModes; iMode++ )
        {
            // Get the display mode attributes
            D3DDISPLAYMODE DisplayMode;
#if DX_VERSION == 8
            m_pD3D->EnumAdapterModes( iAdapter, iMode, &DisplayMode );
#elif DX_VERSION == 9
			m_pD3D->EnumAdapterModes( iAdapter, pAdapter->d3ddmDesktop.Format, iMode, &DisplayMode );
#endif
			
            // Filter out low-resolution modes
            if( DisplayMode.Width  < 320 || DisplayMode.Height < 200 )
                continue;
			
			uint32 m=0L;
            // Check if the mode already exists (to filter out refresh rates)
            for( m=0L; m<dwNumModes; m++ )
            {
                if( ( modes[m].Width  == DisplayMode.Width  ) &&
                    ( modes[m].Height == DisplayMode.Height ) &&
                    ( modes[m].Format == DisplayMode.Format ) )
				{
					if( modes[m].RefreshRate < DisplayMode.RefreshRate )
					{
						modes[m].RefreshRate = DisplayMode.RefreshRate;
					}
                    break;
				}
            }
			
            // If we found a new mode, add it to the list of modes
            if( m == dwNumModes )
            {
                modes[dwNumModes].Width       = DisplayMode.Width;
                modes[dwNumModes].Height      = DisplayMode.Height;
                modes[dwNumModes].Format      = DisplayMode.Format;
                modes[dwNumModes].RefreshRate = DisplayMode.RefreshRate;
                dwNumModes++;
				
				uint32 f=0;
                // Check if the mode's format already exists
                for( f=0; f<dwNumFormats; f++ )
                {
                    if( DisplayMode.Format == formats[f] )
                        break;
                }
				
                // If the format is new, add it to the list
                if( f== dwNumFormats )
                    formats[dwNumFormats++] = DisplayMode.Format;
            }
        }
		
        // Sort the list of display modes (by format, then width, then height)
        qsort( modes, dwNumModes, sizeof(D3DDISPLAYMODE), SortModesCallback );
		
        // Add devices to adapter
        for( UINT iDevice = 0; iDevice < dwNumDeviceTypes; iDevice++ )
        {
            // Fill in device info
            D3DDeviceInfo* pDevice;
            pDevice                 = &pAdapter->devices[pAdapter->dwNumDevices];
            pDevice->DeviceType     = DeviceTypes[iDevice];
            m_pD3D->GetDeviceCaps( iAdapter, DeviceTypes[iDevice], &pDevice->d3dCaps );
            pDevice->strDesc        = strDXDeviceDescs[iDevice];
            pDevice->dwNumModes     = 0;
            pDevice->bCanDoWindowed = false;
            pDevice->bWindowed      = false;
			if( options.DirectXAntiAliasingValue == 1 )
				options.DirectXAntiAliasingValue = 0;
			else if( options.DirectXAntiAliasingValue > 16 )
				options.DirectXAntiAliasingValue = 16;

			if( m_maxFSAA > 0 )
				pDevice->MultiSampleType = (D3DMULTISAMPLE_TYPE)(D3DMULTISAMPLE_NONE+min(m_maxFSAA,(int)options.DirectXAntiAliasingValue));
			else
				pDevice->MultiSampleType = D3DMULTISAMPLE_NONE;
			
            // Examine each format supported by the adapter to see if it will
            // work with this device and meets the needs of the application.
            BOOL  bFormatConfirmed[20];
            uint32 dwBehavior[20];
            D3DFORMAT fmtDepthStencil[20];
			
            for( uint32 f=0; f<dwNumFormats; f++ )
            {
                bFormatConfirmed[f] = FALSE;
                fmtDepthStencil[f] = D3DFMT_UNKNOWN;
				
                // Skip formats that cannot be used as render targets on this device
                if( FAILED( m_pD3D->CheckDeviceType( iAdapter, pDevice->DeviceType,
					formats[f], formats[f], FALSE ) ) )
                    continue;
				
                if( pDevice->DeviceType == D3DDEVTYPE_HAL )
                {
                    // This system has a HAL device
                    bHALExists = TRUE;
#if DX_VERSION == 8	// Always true for DX9				
                    if( pDevice->d3dCaps.Caps2 & D3DCAPS2_CANRENDERWINDOWED )
#endif
                    {
                        // HAL can run in a window for some mode
                        bHALIsWindowedCompatible = TRUE;
						
                        if( f == 0 )
                        {
                            // HAL can run in a window for the current desktop mode
                            bHALIsDesktopCompatible = TRUE;
                        }
                    }
                }
				
                // Confirm the device/format for HW vertex processing
                if( pDevice->d3dCaps.DevCaps&D3DDEVCAPS_HWTRANSFORMANDLIGHT )
                {
					bFormatConfirmed[f] = FALSE;

					if( options.bForceSoftwareTnL )
					{
						dwBehavior[f] = D3DCREATE_SOFTWARE_VERTEXPROCESSING;

						if( SUCCEEDED( ConfirmDevice( &pDevice->d3dCaps, dwBehavior[f], formats[f] ) ) )
						{
							bFormatConfirmed[f] = TRUE;
						}
						
						if ( FALSE == bFormatConfirmed[f] )
						{
							dwBehavior[f] = D3DCREATE_MIXED_VERTEXPROCESSING;

							if( SUCCEEDED( ConfirmDevice( &pDevice->d3dCaps, dwBehavior[f], formats[f] ) ) )
								bFormatConfirmed[f] = TRUE;
						}
					}

					if ( FALSE == bFormatConfirmed[f] )
					{
						if( pDevice->d3dCaps.DevCaps&D3DDEVCAPS_PUREDEVICE )
						{
							dwBehavior[f] = D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_PUREDEVICE;

							if( SUCCEEDED( ConfirmDevice( &pDevice->d3dCaps, dwBehavior[f], formats[f] ) ) )
								bFormatConfirmed[f] = TRUE;
						}
					}

					if ( FALSE == bFormatConfirmed[f] )
					{
						dwBehavior[f] = D3DCREATE_HARDWARE_VERTEXPROCESSING;

						if( SUCCEEDED( ConfirmDevice( &pDevice->d3dCaps, dwBehavior[f], formats[f] ) ) )
							bFormatConfirmed[f] = TRUE;
					}

					if ( FALSE == bFormatConfirmed[f] )
					{
						dwBehavior[f] = D3DCREATE_MIXED_VERTEXPROCESSING;

						if( SUCCEEDED( ConfirmDevice( &pDevice->d3dCaps, dwBehavior[f], formats[f] ) ) )
							bFormatConfirmed[f] = TRUE;
					}
                }
				
                // Confirm the device/format for SW vertex processing
                if( FALSE == bFormatConfirmed[f] )
                {
                    dwBehavior[f] = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
					
                    if( SUCCEEDED( ConfirmDevice( &pDevice->d3dCaps, dwBehavior[f], formats[f] ) ) )
                        bFormatConfirmed[f] = TRUE;
                }
				
                // Find a suitable depth/stencil buffer format for this device/format
                if( bFormatConfirmed[f] ) //&& m_bUseDepthBuffer
                {
                    if( !FindDepthStencilFormat( iAdapter, pDevice->DeviceType, formats[f], &fmtDepthStencil[f] ) )
                    {
                        bFormatConfirmed[f] = FALSE;
                    }
                }
            }
			
            // Add all enumerated display modes with confirmed formats to the
            // device's list of valid modes
            for( uint32 m=0L; m<dwNumModes; m++ )
            {
                for( uint32 f=0; f<dwNumFormats; f++ )
                {
                    if( modes[m].Format == formats[f] )
                    {
                        if( bFormatConfirmed[f] == TRUE )
                        {
                            // Add this mode to the device's list of valid modes
                            pDevice->modes[pDevice->dwNumModes].Width			= modes[m].Width;
                            pDevice->modes[pDevice->dwNumModes].Height			= modes[m].Height;
                            pDevice->modes[pDevice->dwNumModes].Format			= modes[m].Format;
							pDevice->modes[pDevice->dwNumModes].RefreshRate     = modes[m].RefreshRate;
                            pDevice->modes[pDevice->dwNumModes].dwBehavior		= dwBehavior[f];
                            pDevice->modes[pDevice->dwNumModes].DepthStencilFormat = fmtDepthStencil[f];
                            pDevice->dwNumModes++;
							
                            if( pDevice->DeviceType == D3DDEVTYPE_HAL )
                                bHALIsSampleCompatible = TRUE;
                        }
                    }
                }
            }
			
            // Check if the device is compatible with the desktop display mode
            // (which was added initially as formats[0])
#if DX_VERSION == 8 // Always true for DX9
            if( bFormatConfirmed[0] && (pDevice->d3dCaps.Caps2 & D3DCAPS2_CANRENDERWINDOWED) )
#endif
            {
                pDevice->bCanDoWindowed = true;
                pDevice->bWindowed      = true;
            }
			
            // If valid modes were found, keep this device
            if( pDevice->dwNumModes > 0 )
                pAdapter->dwNumDevices++;
        }
		
        // If valid devices were found, keep this adapter
        if( pAdapter->dwNumDevices > 0 )
            m_dwNumAdapters++;
    }
	
    // Return an error if no compatible devices were found
    if( 0L == m_dwNumAdapters )
        return D3DAPPERR_NOCOMPATIBLEDEVICES;
	
    // Pick a default device that can render into a window
    // (This code assumes that the HAL device comes before the REF
    // device in the device array).
    for( int a=0; a<m_dwNumAdapters; a++ )
    {
        for( int d=0; d < m_Adapters[a].dwNumDevices; d++ )
        {
            if( m_Adapters[a].devices[d].bWindowed )
            {
                m_Adapters[a].dwCurrentDevice = d;
                m_dwAdapter = a;
				m_bWindowed = true;
				
                // Display a warning message
                if( m_Adapters[a].devices[d].DeviceType == D3DDEVTYPE_REF )
                {
                    if( !bHALExists )
                        DisplayD3DErrorMsg( D3DAPPERR_NOHARDWAREDEVICE, MSGWARN_SWITCHEDTOREF );
                    else if( !bHALIsSampleCompatible )
                        DisplayD3DErrorMsg( D3DAPPERR_HALNOTCOMPATIBLE, MSGWARN_SWITCHEDTOREF );
                    else if( !bHALIsWindowedCompatible )
                        DisplayD3DErrorMsg( D3DAPPERR_NOWINDOWEDHAL, MSGWARN_SWITCHEDTOREF );
                    else if( !bHALIsDesktopCompatible )
                        DisplayD3DErrorMsg( D3DAPPERR_NODESKTOPHAL, MSGWARN_SWITCHEDTOREF );
                    else // HAL is desktop compatible, but not sample compatible
                        DisplayD3DErrorMsg( D3DAPPERR_NOHALTHISMODE, MSGWARN_SWITCHEDTOREF );
                }
				
                return S_OK;
            }
        }
    }
	
    return D3DAPPERR_NOWINDOWABLEDEVICES;
#else //xbox
	return S_OK;
#endif //_XBOX
}

int	CDXGraphicsContext::FindCurrentDisplayModeIndex()
{
	D3DDISPLAYMODE dMode;
	D3DAdapterInfo &adapter = m_Adapters[m_dwAdapter];
	//D3DDeviceInfo &device = adapter.devices[adapter.dwCurrentDevice];
	D3DDeviceInfo &device = adapter.devices[options.DirectXDevice];
	int m;

	m_pD3D->GetAdapterDisplayMode(m_dwAdapter,&dMode);

	for( m=0; m<device.dwNumModes; m++ )
	{
		if( device.modes[m].Width==windowSetting.uFullScreenDisplayWidth && device.modes[m].Height==windowSetting.uFullScreenDisplayHeight 
			//&& device.modes[m].Format == dMode.Format 
			&& FormatToSize(device.modes[m].Format) == FormatToSize((D3DFORMAT)options.colorQuality) 
			)
		{
			return m;
		}
	}

	for( m=0; m<device.dwNumModes; m++ )
	{
		if( device.modes[m].Width==windowSetting.uFullScreenDisplayWidth && device.modes[m].Height==windowSetting.uFullScreenDisplayHeight )
		{
			return m;
		}
	}

	// Cannot find a matching mode
	DebuggerAppendMsg("Cannot find a matching mode");
	for( m=0; m<device.dwNumModes; m++ )
	{
		if( device.modes[m].Width==1280 && device.modes[m].Height==720 )
		{
			return m;
		}
		else if( device.modes[m].Width==640 && device.modes[m].Height==480 )
		{
			return m;
		}
	}

	return 0;
}


//-----------------------------------------------------------------------------
// Name: ConfirmDevice()
// Desc: Called during device initialization, this code checks the device
//       for some minimum set of capabilities
//-----------------------------------------------------------------------------
HRESULT CDXGraphicsContext::ConfirmDevice( MYD3DCAPS* pCaps, uint32 dwBehavior,
										D3DFORMAT Format )
{
    /*if( ( dwBehavior & D3DCREATE_HARDWARE_VERTEXPROCESSING ) ||
        ( dwBehavior & D3DCREATE_MIXED_VERTEXPROCESSING ) )
    {
        if( pCaps->VertexShaderVersion < D3DVS_VERSION(1,0) )
            return E_FAIL;
    }*/
	
    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: DisplayD3DErrorMsg()
// Desc: Displays error messages in a message box
//-----------------------------------------------------------------------------
HRESULT CDXGraphicsContext::DisplayD3DErrorMsg( HRESULT hr, uint32 dwType )
{
#ifdef _XBOX
	return S_OK;
#else
    TCHAR strMsg[512];
	
    switch( hr )
    {
	case D3DAPPERR_NODIRECT3D:
		strcpy( strMsg, "Could not initialize Direct3D. You may\n"
			"want to check that the latest version of\n"
			"DirectX is correctly installed on your\n"
			"system." );
		break;
		
	case D3DAPPERR_NOCOMPATIBLEDEVICES:
		strcpy( strMsg, "Could not find any compatible Direct3D\n"
			"devices." );
		break;
		
	case D3DAPPERR_NOWINDOWABLEDEVICES:
		strcpy( strMsg, "This program cannot run in a desktop\n"
			"window with the current display settings.\n"
			"Please change your desktop settings to a\n"
			"16- or 32-bit display mode and re-run this\n"
			"sample." );
		break;
		
	case D3DAPPERR_NOHARDWAREDEVICE:
		strcpy( strMsg, "No hardware-accelerated Direct3D devices\n"
			"were found." );
		break;
		
	case D3DAPPERR_HALNOTCOMPATIBLE:
		strcpy( strMsg, "This program requires functionality that\n"
			"is not available on your Direct3D hardware\n"
			"accelerator." );
		break;
		
	case D3DAPPERR_NOWINDOWEDHAL:
		strcpy( strMsg, "Your Direct3D hardware accelerator cannot\n"
			"render into a window." );
		break;
		
	case D3DAPPERR_NODESKTOPHAL:
		strcpy( strMsg, "Your Direct3D hardware accelerator cannot\n"
			"render into a window with the current\n"
			"desktop display settings." );
		break;
		
	case D3DAPPERR_NOHALTHISMODE:
		strcpy( strMsg, "This program requires functionality that is\n"
			"not available on your Direct3D hardware\n"
			"accelerator with the current desktop display\n"
			"settings." );
		break;
		
	case D3DAPPERR_RESIZEFAILED:
		strcpy( strMsg, "Could not reset the Direct3D device." );
		break;
		
	case D3DAPPERR_NONZEROREFCOUNT:
		strcpy( strMsg, "A D3D object has a non-zero reference\n"
			"count (meaning things were not properly\n"
			"cleaned up)." );
		break;
		
	case E_OUTOFMEMORY:
		strcpy( strMsg, "Not enough memory." );
		break;
		
	case D3DERR_OUTOFVIDEOMEMORY:
		strcpy( strMsg, "Not enough video memory." );
		break;
		
	default:
		strcpy( strMsg, "Generic application error. Enable\n"
			"debug output for detailed information." );
    }
	
	// TODO: Use IDS_D3DERROR resource
	// IDS_D3DERROR has one %s for the error message
	//wsprintf(szMsg, CResourceString(IDS_D3DERROR), szError);
	
	
    if( MSGERR_APPMUSTEXIT == dwType )
    {
        strcat( strMsg, "\n\nThis program will now exit." );
    }
    else
    {
        if( MSGWARN_SWITCHEDTOREF == dwType )
            strcat( strMsg, "\n\nSwitching to the reference rasterizer,\n"
			"a software device that implements the entire\n"
			"Direct3D feature set, but runs very slowly." );
    }

	OutputDebugString( strMsg );
	if( status.ToToggleFullScreen || !CGraphicsContext::g_pGraphicsContext->IsWindowed() )
		SetWindowText(g_GraphicsInfo.hStatusBar,strMsg);
	else
		MessageBox(NULL,strMsg,"str",MB_OK|MB_ICONERROR);

    return hr;
#endif
}






//-----------------------------------------------------------------------------
// Name: CleanUp()
// Desc: Cleanup scene objects
//-----------------------------------------------------------------------------
VOID CDXGraphicsContext::CleanUp()
{
	CGraphicsContext::CleanUp();
	
    if( m_pd3dDevice )
    {
        CleanDeviceObjects();
		
#ifndef _XBOX
		SAFE_RELEASE(m_pID3DFont);
#endif
		m_bFontIsCreated = false;
		SAFE_RELEASE(m_pd3dDevice);
		SAFE_RELEASE(m_pD3D);
    }
}


int CDXGraphicsContext::ToggleFullscreen()
{
#ifndef _XBOX
    // Toggle the fullscreen/window mode
    if( m_bActive && m_bReady )
    {
        if( FAILED( DoToggleFullscreen() ) )
        {
            DisplayD3DErrorMsg( D3DAPPERR_RESIZEFAILED, MSGERR_APPMUSTEXIT );
        }
		CRender::GetRender()->Initialize();
    }

	if( !m_bWindowed )
	{
		m_pd3dDevice->ShowCursor( FALSE );
	}
	else
	{
		m_pd3dDevice->ShowCursor( TRUE );
	}

	return m_bWindowed?0:1;
#else
	return 1;
#endif // _XBOX
}


//-----------------------------------------------------------------------------
// Name: Pause()
// Desc: Called in to toggle the pause state of the app.
//-----------------------------------------------------------------------------
void CDXGraphicsContext::Pause( bool bPause )
{
    static uint32 dwAppPausedCount = 0L;
	
    dwAppPausedCount += ( bPause ? +1 : -1 );
    m_bReady          = ( dwAppPausedCount ? false : true );
}





//-----------------------------------------------------------------------------
// Name: InitDeviceObjects()
// Desc: Initialize device-dependent objects. This is the place to create mesh
//       and texture objects.
//-----------------------------------------------------------------------------
HRESULT CDXGraphicsContext::InitDeviceObjects()
{
	// Set the device's states for rendering
	//return RDPInit();

	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: CleanDeviceObjects()
// Desc: Called when the device-dependent objects are about to be lost.
//-----------------------------------------------------------------------------
HRESULT CDXGraphicsContext::CleanDeviceObjects()
{
	if ( CRender::IsAvailable() )
	{
		CRender::GetRender()->ClearDeviceObjects();
	}

	// Kill all textures?
	gTextureManager.RecycleAllTextures(); // was DropTextures()
	gTextureManager.CleanUp();

	return S_OK;
}

bool CDXGraphicsContext::IsResultGood(HRESULT hr, bool displayError)
{
	if(FAILED(hr))
	{
		if( displayError )
		{
			char szError[200+1];
#if DX_VERSION == 8 // this is not in DX9? (DX8->DX9 migration)
			D3DXGetErrorString(hr, szError, 200);
#endif
			DebuggerAppendMsg("D3D Error: %s", szError);
			ErrorMsg(szError);
		}
		return false;
	}
	else
		return true;
}


extern RecentCIInfo* g_uRecentCIInfoPtrs[5];
extern TextureBufferInfo gTextureBufferInfos[];
#ifndef _RICE6FB
void CDXGraphicsContext::SaveBackBuffer(int ciInfoIdx, RECT* pSrcRect)
{
#ifndef _DISABLE_VID1964
	if( ciInfoIdx == 1 )	// to save the current front buffer
	{
		UpdateFrame(true);
	}
#endif
	HRESULT res;
	SetImgInfo tempinfo;
	RecentCIInfo &ciInfo = *g_uRecentCIInfoPtrs[ciInfoIdx];
	tempinfo.dwAddr = ciInfo.dwAddr;
	tempinfo.dwFormat = ciInfo.dwFormat;
	tempinfo.dwSize = ciInfo.dwSize;
	tempinfo.dwWidth = ciInfo.dwWidth;

#ifdef _RICE560
	int idx = SetTextureBuffer(tempinfo, ciInfoIdx,true );
#else
	int idx = SetBackBufferAsTextureBuffer(tempinfo, ciInfoIdx);
#endif

	MYLPDIRECT3DSURFACE pSavedBuffer;
	MYLPDIRECT3DTEXTURE(gTextureBufferInfos[idx].pTxtBuffer->m_pTexture->GetTexture())->GetSurfaceLevel(0,&pSavedBuffer);

	if( pSavedBuffer != NULL )
	{
		MYLPDIRECT3DSURFACE pBackBufferToSave = NULL;
#if DX_VERSION == 8
		g_pD3DDev->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &pBackBufferToSave);
#elif DX_VERSION == 9
		g_pD3DDev->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBufferToSave);
#endif

		if( pBackBufferToSave )
		{
			if( defaultRomOptions.bInN64Resolution )
			{
				// Need to scale down from PC resolution to N64 resolution
				if( pSrcRect == NULL )
				{
					RECT srcrect = {0,0,m_d3dpp.BackBufferWidth-1,m_d3dpp.BackBufferHeight-1};
					RECT dstrect = {0,0,ciInfo.dwWidth-1,ciInfo.dwHeight-1};
					res = D3DXLoadSurfaceFromSurface(pSavedBuffer,NULL,&dstrect,pBackBufferToSave,NULL,&srcrect,D3DX_FILTER_LINEAR,0xFF000000);
				}
				else
				{
					float scaleX = windowSetting.uDisplayWidth/(float)ciInfo.dwWidth;
					float scaleY = windowSetting.uDisplayHeight/(float)ciInfo.dwHeight;
					RECT srcr2 = { uint32(pSrcRect->left*scaleX), uint32(pSrcRect->top*scaleY),
						uint32(pSrcRect->right*scaleX), uint32(pSrcRect->bottom*scaleY) };
					res = D3DXLoadSurfaceFromSurface(pSavedBuffer,NULL,pSrcRect,pBackBufferToSave,NULL,&srcr2,D3DX_FILTER_LINEAR,0xFF000000);
				}
			}
			else
			{
				if( pSrcRect == NULL )
				{
#if DX_VERSION == 8 && !defined(_XBOX)
					res = g_pD3DDev->CopyRects(pBackBufferToSave,NULL,0,pSavedBuffer,NULL);
#elif DX_VERSION == 9
					res = g_pD3DDev->UpdateSurface(pBackBufferToSave,NULL,pSavedBuffer,NULL);
#elif defined(_XBOX)
RECT dstrect = {0,0,gTextureBufferInfos[idx].pTxtBuffer->m_pTexture->m_dwWidth,gTextureBufferInfos[idx].pTxtBuffer->m_pTexture->m_dwHeight};
					res = D3DXLoadSurfaceFromSurface(pSavedBuffer,NULL,&dstrect,pBackBufferToSave,NULL,NULL,D3DX_FILTER_LINEAR,0xFF000000);
#endif
				}
				else
				{
					float scaleX = windowSetting.uDisplayWidth/(float)ciInfo.dwWidth;
					float scaleY = windowSetting.uDisplayHeight/(float)ciInfo.dwHeight;
					RECT srcr = { uint32(pSrcRect->left*scaleX), uint32(pSrcRect->top*scaleY),
						uint32(pSrcRect->right*scaleX), uint32(pSrcRect->bottom*scaleY) };
					POINT srcp = {uint32(pSrcRect->left*scaleX), uint32(pSrcRect->top*scaleY)};
#if DX_VERSION == 8 && !defined(_XBOX)
					res = g_pD3DDev->CopyRects(pBackBufferToSave,&srcr,0,pSavedBuffer,&srcp);
#elif DX_VERSION == 9
					res = g_pD3DDev->UpdateSurface(pBackBufferToSave,&srcr,pSavedBuffer,&srcp);
#elif defined(_XBOX)
					res = D3DXLoadSurfaceFromSurface(pSavedBuffer,NULL,&srcr,pBackBufferToSave,NULL,&srcr,D3DX_FILTER_LINEAR,0xFF000000);
#endif
				}
			}

			if( res != S_OK )
			{
				DebuggerAppendMsg("Cannot save back buffer");
			}

			pBackBufferToSave->Release();
		}
	}


	if( frameBufferOptions.bWriteBackBufToRDRAM || frameBufferOptions.bProcessCPURead )
	{
		uint32 width = ciInfo.dwWidth;
		uint32 height = ciInfo.dwHeight;

		if( ciInfo.dwWidth == *g_GraphicsInfo.VI_WIDTH_REG && ciInfo.dwWidth != windowSetting.uViWidth )
		{
			width = windowSetting.uViWidth;
			height = windowSetting.uViHeight;
		}

		CopyBackToRDRAM(ciInfo.dwAddr, ciInfo.dwFormat, ciInfo.dwSize, width, height, 
			windowSetting.uDisplayWidth, windowSetting.uDisplayHeight,
			ciInfo.dwAddr, ciInfo.dwMemSize , ciInfo.dwWidth,
			D3DFMT_X8R8G8B8, pSavedBuffer);

	}

	gTextureBufferInfos[idx].crcCheckedAtFrame = status.gDlistCount;
#ifdef _RICE560	
	gTextureBufferInfos[idx].crcInRDRAM = ComputeTextureBufferCRCInRDRAM(m_curTextureBufferIndex);
#else
	gTextureBufferInfos[idx].crcInRDRAM = ComputeTextureBufferCRCInRDRAM(idx);
#endif
	pSavedBuffer->Release();
	g_uRecentCIInfoPtrs[ciInfoIdx]->bCopied = true;

#ifndef _DISABLE_VID1964	
	if( ciInfoIdx == 1 )	// to save the current front buffer
	{
		UpdateFrame(true);
	}
#endif
}
#endif

CDXTextureBuffer::CDXTextureBuffer(int width, int height, TextureBufferInfo* pInfo, TextureUsage usage)
	: CTextureBuffer(width, height, pInfo, usage)
{
	m_pTexture = new CDirectXTexture(width, height, usage);
	if( m_pTexture )
	{
		m_width = width;
		m_height = height;
	}
	else
	{
		SAFE_DELETE(m_pTexture);
	}

	m_pColorBufferSave = NULL;
	m_pDepthBufferSave = NULL;
	m_beingRendered = false;
}

CDXTextureBuffer::~CDXTextureBuffer()
{
	if( m_beingRendered )
	{
#ifdef _RICE6FB
		g_pFrameBufferManager->CloseRenderTexture(false);
#else
		CGraphicsContext::g_pGraphicsContext->CloseTextureBuffer(false);
#endif
		SetAsRenderTarget(false);
	}

	SAFE_DELETE(m_pTexture);
	
	m_beingRendered = false;
}

bool CDXTextureBuffer::SetAsRenderTarget(bool enable)
{
	if( m_usage != AS_RENDER_TARGET )	return false;

	if( enable )
	{
		if( !m_beingRendered )
		{
			if(m_pTexture )
			{
				MYLPDIRECT3DSURFACE pColorBuffer;

				// Save the current back buffer
#if DX_VERSION == 8
				g_pD3DDev->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &m_pColorBufferSave);
#elif DX_VERSION == 9
				g_pD3DDev->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &m_pColorBufferSave);
#endif
				g_pD3DDev->GetDepthStencilSurface(&m_pDepthBufferSave);

				// Activate the texture buffer
				(MYLPDIRECT3DTEXTURE(m_pTexture->GetTexture()))->GetSurfaceLevel(0,&pColorBuffer);
#if DX_VERSION == 8
				HRESULT res = g_pD3DDev->SetRenderTarget(pColorBuffer, NULL);
#elif DX_VERSION == 9
				HRESULT res = g_pD3DDev->SetRenderTarget(0, pColorBuffer);
#endif
				SAFE_RELEASE(pColorBuffer);
				if( res != S_OK )
				{
					return false;
				}

				m_beingRendered = true;
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			return true;
		}
	}
	else
	{
		if( m_beingRendered )
		{
			if( m_pColorBufferSave && m_pDepthBufferSave )
			{
#if DX_VERSION == 8
				g_pD3DDev->SetRenderTarget(m_pColorBufferSave, m_pDepthBufferSave);
#elif DX_VERSION == 9
g_pD3DDev->SetRenderTarget(0, m_pColorBufferSave);
				g_pD3DDev->SetDepthStencilSurface(m_pDepthBufferSave);
#endif
				m_beingRendered = false;
				SAFE_RELEASE(m_pColorBufferSave);
				SAFE_RELEASE(m_pDepthBufferSave);
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			return true;
		}
	}
}

void CDXTextureBuffer::LoadTexture(TxtrCacheEntry* pEntry)
{
	bool IsBeingRendered = m_beingRendered;
	if( IsBeingRendered )
	{
		SetAsRenderTarget(false);
		//return;
	}

	CTexture *pSurf = pEntry->pTexture;
	TxtrInfo &ti = pEntry->ti;

	// Need to load texture from the saved back buffer

	MYLPDIRECT3DTEXTURE pNewTexture = MYLPDIRECT3DTEXTURE(pSurf->GetTexture());
	MYLPDIRECT3DSURFACE pNewSurface = NULL;
	HRESULT res = pNewTexture->GetSurfaceLevel(0,&pNewSurface);
	MYLPDIRECT3DSURFACE pSourceSurface = NULL;
	(MYLPDIRECT3DTEXTURE(m_pTexture->GetTexture()))->GetSurfaceLevel(0,&pSourceSurface);

	int left = (pEntry->ti.Address- m_pInfo->CI_Info.dwAddr )%m_pInfo->CI_Info.bpl + pEntry->ti.LeftToLoad;
	int top = (pEntry->ti.Address- m_pInfo->CI_Info.dwAddr)/m_pInfo->CI_Info.bpl + pEntry->ti.TopToLoad;
	RECT srcrect = {uint32(left*m_pInfo->scaleX) ,uint32(top*m_pInfo->scaleY), 
		uint32(min(m_width, left+(int)ti.WidthToLoad)*m_pInfo->scaleX), 
		uint32(min(m_height,top+(int)ti.HeightToLoad)*m_pInfo->scaleY) };

	if( pNewSurface != NULL && pSourceSurface != NULL )
	{
		if( left < m_width && top<m_height )
		{
			RECT dstrect = {0,0,ti.WidthToLoad,ti.HeightToLoad};
			HRESULT res = D3DXLoadSurfaceFromSurface(pNewSurface,NULL,&dstrect,pSourceSurface,NULL,&srcrect,D3DX_FILTER_POINT ,0xFF000000);
		}
	}

	if( IsBeingRendered )
	{
		SetAsRenderTarget(true);
	}

	pSurf->SetOthersVariables();
	SAFE_RELEASE(pSourceSurface);
}

#ifdef _RICE6FB
void CDXTextureBuffer::StoreTextureBufferToRDRAM(int infoIdx)
#else
void CDXGraphicsContext::StoreTextureBufferToRDRAM(int infoIdx)
#endif
{
#ifndef _RICE6FB
	if( infoIdx < 0 )
		infoIdx = m_lastTextureBufferIndex;

	if( gTextureBufferInfos[infoIdx].pTxtBuffer && gTextureBufferInfos[infoIdx].pTxtBuffer->IsBeingRendered() )
	{
		return;
	}

	if( gTextureBufferInfos[infoIdx].pTxtBuffer )
#else
	DXFrameBufferManager &FBmgr = *(DXFrameBufferManager*)g_pFrameBufferManager;
	if(1)
#endif
	{
		TextureBufferInfo &info = gTextureBufferInfos[infoIdx];

		uint32 fmt = info.CI_Info.dwFormat;
		if( frameBufferOptions.bTxtBufWriteBack )
		{
			CDXTextureBuffer* pTxtBuffer = (CDXTextureBuffer*)(info.pTxtBuffer);
			MYLPDIRECT3DSURFACE pSourceSurface = NULL;
			(MYLPDIRECT3DTEXTURE(pTxtBuffer->m_pTexture->GetTexture()))->GetSurfaceLevel(0,&pSourceSurface);

			// Ok, we are using texture render target right now
			// Need to copy content from the texture render target back to frame buffer
			// then reset the current render target

			// Here we need to copy the content from the texture frame buffer to RDRAM memory

			if( pSourceSurface )
			{
				uint32 width, height, bufWidth, bufHeight, memsize; 
				width = info.N64Width;
				height = info.N64Height;
				bufWidth = info.bufferWidth;
				bufHeight = info.bufferHeight;
				if( info.CI_Info.dwSize == TXT_SIZE_8b && fmt == TXT_FMT_CI )
				{
					info.CI_Info.dwFormat = TXT_FMT_I;
					height = info.knownHeight ? info.N64Height : info.maxUsedHeight;
					memsize = info.N64Width*height;
#ifdef _RICE6FB
					FBmgr.CopyD3DSurfaceToRDRAM(info.CI_Info.dwAddr, fmt, info.CI_Info.dwSize, width, height,
						bufWidth, bufHeight, info.CI_Info.dwAddr, memsize, info.N64Width, D3DFMT_A8R8G8B8, pSourceSurface);
#else
					CopyBackToRDRAM(info.CI_Info.dwAddr, fmt, info.CI_Info.dwSize, width, height,
						bufWidth, bufHeight, info.CI_Info.dwAddr, memsize, info.N64Width, D3DFMT_A8R8G8B8, pSourceSurface);
#endif
					info.CI_Info.dwFormat = TXT_FMT_CI;
				}
				else
				{
					if( info.CI_Info.dwSize == TXT_SIZE_8b )
					{
						height = info.knownHeight ? info.N64Height : info.maxUsedHeight;
						memsize = info.N64Width*height;
#ifdef _RICE6FB
						FBmgr.CopyD3DSurfaceToRDRAM(info.CI_Info.dwAddr, fmt, info.CI_Info.dwSize, width, height,
							bufWidth, bufHeight, info.CI_Info.dwAddr, memsize, info.N64Width, D3DFMT_A8R8G8B8, pSourceSurface);
#else
						CopyBackToRDRAM(info.CI_Info.dwAddr, fmt, info.CI_Info.dwSize, width, height,
							bufWidth, bufHeight, info.CI_Info.dwAddr, memsize, info.N64Width, D3DFMT_A8R8G8B8, pSourceSurface);
#endif
					}
					else
					{
						height = info.knownHeight ? info.N64Height : info.maxUsedHeight;
						memsize = g_pTxtBufferInfo->N64Width*height*2;
#ifdef _RICE6FB
						FBmgr.CopyD3DSurfaceToRDRAM(info.CI_Info.dwAddr, fmt, info.CI_Info.dwSize, width, height,
							bufWidth, bufHeight, info.CI_Info.dwAddr, memsize, info.N64Width, D3DFMT_X8R8G8B8, pSourceSurface);
#else
						CopyBackToRDRAM(info.CI_Info.dwAddr, fmt, info.CI_Info.dwSize, width, height,
							bufWidth, bufHeight, info.CI_Info.dwAddr, memsize, info.N64Width, D3DFMT_X8R8G8B8, pSourceSurface);
#endif
					}
				}
				SAFE_RELEASE(pSourceSurface);
			}
			else
			{
				DebuggerAppendMsg("Error, cannot lock the texture buffer");
			}
		}

	}

}


uint16 ConvertRGBATo555(uint8 r, uint8 g, uint8 b, uint8 a);
uint16 ConvertRGBATo555(uint32 color32);
void InitTlutReverseLookup(void);
extern uint8 RevTlutTable[0x10000];

void CDXGraphicsContext::CopyBackToRDRAM(uint32 addr, uint32 fmt, uint32 siz, uint32 width, uint32 height, uint32 bufWidth, uint32 bufHeight, uint32 startaddr, uint32 memsize, uint32 pitch, D3DFORMAT surf_fmt, MYIDirect3DSurface *surf)
{
	if( addr == 0 || addr>=g_dwRamSize )	return;

	if( pitch == 0 ) pitch = width;

	MYIDirect3DSurface *backBuffer = surf;
	MYIDirect3DSurface *backBuffer2 = NULL;

	if( surf == NULL )	
	{
#if DX_VERSION == 8
		g_pD3DDev->GetBackBuffer(0,D3DBACKBUFFER_TYPE_MONO, &backBuffer);
#elif DX_VERSION == 9
		g_pD3DDev->GetBackBuffer(0, 0,D3DBACKBUFFER_TYPE_MONO, &backBuffer);
#endif
	}

	//int pixSize = GetPixelSize();
	if( !backBuffer )
	{
		return;
	}

	D3DLOCKED_RECT dlre;
	ZeroMemory( &dlre, sizeof(D3DLOCKED_RECT) );
	if( !SUCCEEDED(backBuffer->LockRect(&dlre, NULL, D3DLOCK_READONLY)) )
	{
		D3DSURFACE_DESC desc;
		backBuffer->GetDesc(&desc);
#if DX_VERSION == 8
		g_pD3DDev->CreateImageSurface(desc.Width,desc.Height,desc.Format,&backBuffer2);
		g_pD3DDev->CopyRects(backBuffer,NULL,0,backBuffer2,NULL);
#elif DX_VERSION == 9
		g_pD3DDev->CreateOffscreenPlainSurface(desc.Width,desc.Height,desc.Format,D3DPOOL_DEFAULT, &backBuffer2, NULL);
		g_pD3DDev->UpdateSurface(backBuffer,NULL,backBuffer2,NULL);
#endif
		ZeroMemory( &dlre, sizeof(D3DLOCKED_RECT) );
		if( !SUCCEEDED(backBuffer2->LockRect(&dlre, NULL, D3DLOCK_READONLY)) )
		{
			return;
		}
	}

	uint32 startline=0;
	if( startaddr == 0xFFFFFFFF )	startaddr = addr;

	startline = (startaddr-addr)/siz/pitch;
	if( startline >= height )
	{
		startline = height;
	}

	uint32 endline = height;
	if( memsize != 0xFFFFFFFF )
	{
		endline = (startaddr+memsize-addr)/siz;
		if( endline % pitch == 0 )
			endline /= pitch;
		else
			endline = endline/pitch+1;
	}
	if( endline > height )
	{
		endline = height;
	}

	if( memsize != 0xFFFFFFFF )
	{
	}

	int indexes[600];
	{
		float sx;
		int sx0;
		float ratio = bufWidth/(float)width;
		for( uint32 j=0; j<width; j++ )
		{
			sx = j*ratio;
			sx0 = int(sx+0.5);
			indexes[j] = 4*sx0;
		}
	}

	if( siz == TXT_SIZE_16b )
	{
		uint16 *frameBufferBase = (uint16*)(g_pRDRAMu8+addr);

		if( surf_fmt==D3DFMT_A8R8G8B8 || surf_fmt==D3DFMT_X8R8G8B8 )
		{
			int  sy0;
			float ratio = bufHeight/(float)height;

			for( uint32 i=startline; i<endline; i++ )
			{
				sy0 = int(i*ratio+0.5);

				uint16 *pD = frameBufferBase + i * pitch;
				uint8 *pS0 = (uint8 *)dlre.pBits + sy0 * dlre.Pitch;

				for( uint32 j=0; j<width; j++ )
				{
					// Point
					uint8 r = pS0[indexes[j]+2];
					uint8 g = pS0[indexes[j]+1];
					uint8 b = pS0[indexes[j]+0];
					uint8 a;
					if( surf_fmt==D3DFMT_X8R8G8B8 )
					{
						a = (r+g+b)>1 ? 0xFF : 0;
					}
					else
					{
						a = pS0[indexes[j]+3];
					}

					// Liner
					*(pD+(j^1)) = ConvertRGBATo555( r, g, b, a);

				}
			}
		}
		else
		{
		}
	}
	else if( siz == TXT_SIZE_8b && fmt == TXT_FMT_CI )
	{
		uint8 *frameBufferBase = (uint8*)(g_pRDRAMu8+addr);

		if( surf_fmt==D3DFMT_A8R8G8B8 )
		{
			uint16 tempword;
			InitTlutReverseLookup();

			for( uint32 i=startline; i<endline; i++ )
			{
				uint8 *pD = frameBufferBase + i * width;
				uint8 *pS = (uint8 *)dlre.pBits + i*bufHeight/height * dlre.Pitch;
				for( uint32 j=0; j<width; j++ )
				{
					int pos = 4*(j*bufWidth/width);
					tempword = ConvertRGBATo555((pS[pos+2]),		// Red
						(pS[pos+1]),		// G
						(pS[pos+0]),		// B
						(pS[pos+3]));		// Alpha
					//*pD = CIFindIndex(tempword);
					*(pD+(j^3)) = RevTlutTable[tempword];
				}
			}
		}
		else
		{
		}
	}
	else if( siz == TXT_SIZE_8b && fmt == TXT_FMT_I )
	{
		uint8 *frameBufferBase = (uint8*)(g_pRDRAMu8+addr);

		if( surf_fmt==D3DFMT_A8R8G8B8 )
		{
			int sy0;
			float ratio = bufHeight/(float)height;

			for( uint32 i=startline; i<endline; i++ )
			{
				sy0 = int(i*ratio+0.5);

				uint8 *pD = frameBufferBase + i * width;
				uint8 *pS0 = (uint8 *)dlre.pBits + sy0 * dlre.Pitch;

				for( uint32 j=0; j<width; j++ )
				{
					// Point
					uint32 r = pS0[indexes[j]+2];
					uint32 g = pS0[indexes[j]+1];
					uint32 b = pS0[indexes[j]+0];

					// Liner
					*(pD+(j^3)) = (uint8)((r+b+g)/3);

				}
			}
		}
		else
		{
		}
	}

	if( backBuffer2 )	
	{
		backBuffer2->UnlockRect();
		backBuffer2->Release();
	}
	else
		backBuffer->UnlockRect();

	if( surf == NULL )
		backBuffer->Release();
}

bool CDXGraphicsContext::CreateFontObjects()
{
#if DX_VERSION == 8 && !defined(_XBOX)// Fonts are not compatible with DX9
	m_hFont = (HFONT)GetStockObject(SYSTEM_FONT);
	D3DXCreateFont(m_pd3dDevice, m_hFont, &m_pID3DFont);
	m_bFontIsCreated = true;
#endif
	return true;
}
bool CDXGraphicsContext::DrawText(char *str, RECT &rect, int alignment)
{
#if DX_VERSION == 8 && !defined(_XBOX)// Fonts are not compatible with DX9
	if( !m_bFontIsCreated )
		CreateFontObjects();

	if( m_hFont && m_pID3DFont )
	{
		CRender::GetRender()->m_pColorCombiner->DisableCombiner();
		m_pID3DFont->Begin();

		if( alignment == 0 )
		{
			m_pID3DFont->DrawText(str, strlen(str), &rect, 
				DT_CENTER | DT_VCENTER | DT_NOCLIP | DT_NOPREFIX | DT_SINGLELINE,
				options.FPSColor);
		}
		else if( alignment == 1)
		{
			// Align to left
			m_pID3DFont->DrawText(str, strlen(str), &rect, 
				DT_LEFT | DT_VCENTER | DT_NOCLIP | DT_NOPREFIX | DT_SINGLELINE,
				options.FPSColor);
		}
		else
		{
			// Align to left
			m_pID3DFont->DrawText(str, strlen(str), &rect, 
				DT_RIGHT | DT_VCENTER | DT_NOCLIP | DT_NOPREFIX | DT_SINGLELINE,
				options.FPSColor);
		}

		m_pID3DFont->End();
	}
#endif
	return true;
}


HRESULT CD3DDevWrapper::SetRenderState(D3DRENDERSTATETYPE State,DWORD Value)
{
	if( m_pD3DDev != NULL )
	{
#ifndef _XBOX // Confirmed needed on Xbox
		if( m_savedRenderStates[State] != Value )
#else
		if (m_savedRenderStates[State] != Value || !m_savedRenderStates[State])
#endif
		{
			m_savedRenderStates[State] = Value;
			return m_pD3DDev->SetRenderState(State, Value);
		}
	}

	return S_OK;
}

HRESULT CD3DDevWrapper::SetTextureStageState(DWORD Stage,D3DTEXTURESTAGESTATETYPE Type,DWORD Value)
{
#ifdef _XBOX_HACK
	if( m_pD3DDev != NULL && Stage < 4 )
#else
	if( m_pD3DDev != NULL )
#endif
	{
#ifndef _XBOX
		if( m_savedTextureStageStates[Stage][Type] != Value )
#else
		if (m_savedTextureStageStates[Stage][Type] != Value || !m_savedTextureStageStates[Stage][Type])
#endif
		{
			switch( Type )
			{
			case D3DTSS_COLORARG1: 
			case D3DTSS_COLORARG2: 
			case D3DTSS_COLORARG0: 
			case D3DTSS_ALPHAARG1: 
			case D3DTSS_ALPHAARG2: 
			case D3DTSS_ALPHAARG0: 
				if( Value != D3DTA_IGNORE )
				{
					m_savedTextureStageStates[Stage][Type] = Value;
					return m_pD3DDev->SetTextureStageState(Stage, Type, Value);
				}
				else
					return S_OK;

				break;
			default:
				m_savedTextureStageStates[Stage][Type] = Value;
				return m_pD3DDev->SetTextureStageState(Stage, Type, Value);
				break;
			}
		}
	}

	return S_OK;
}
#if DX_VERSION == 9 // This replaces SetTextureStageState
HRESULT CD3DDevWrapper::SetSamplerState(DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD Value)
{
	if( m_pD3DDev != NULL )
	{
		if( m_savedSamplerStates[Sampler][Type] != Value )
			return m_pD3DDev->SetSamplerState(Sampler, Type, Value);
	}
}
#endif

#if DX_VERSION == 8
HRESULT CD3DDevWrapper::SetPixelShader(DWORD Handle)
{
	if( m_pD3DDev != NULL )
	{
#ifndef _XBOX // dipset
		if( m_savedPixelShaderHander != Handle )
#endif
		{
			m_savedPixelShaderHander = Handle;
			return m_pD3DDev->SetPixelShader(Handle);
		}
	}

	return S_OK;
}
#elif DX_VERSION == 9
HRESULT CD3DDevWrapper::SetPixelShader(IDirect3DPixelShader9* pShader)
{
	if( m_pD3DDev != NULL )
	{
		if( m_savedPixelShaderHander != pShader )
		{
			m_savedPixelShaderHander = pShader;
			return m_pD3DDev->SetPixelShader(pShader);
		}
	}

	return S_OK;
}
#endif
HRESULT CD3DDevWrapper::SetPixelShaderConstant(DWORD Register, float* pfdata)
{
	if( m_pD3DDev != NULL )
	{
#ifndef _XBOX // Confirmed needed on Xbox
		if( m_savedPixelShaderConstants[Register][0] != pfdata[0] ||
			m_savedPixelShaderConstants[Register][1] != pfdata[1] ||
			m_savedPixelShaderConstants[Register][2] != pfdata[2] ||
			m_savedPixelShaderConstants[Register][3] != pfdata[3] )
#endif
		{
			m_savedPixelShaderConstants[Register][0] = pfdata[0];
			m_savedPixelShaderConstants[Register][1] = pfdata[1];
			m_savedPixelShaderConstants[Register][2] = pfdata[2];
			m_savedPixelShaderConstants[Register][3] = pfdata[3];
			
#if DX_VERSION == 8
			return m_pD3DDev->SetPixelShaderConstant(Register,pfdata,1);
#elif DX_VERSION == 9
			return m_pD3DDev->SetPixelShaderConstantF(Register,pfdata,1);
#endif
		}
	}

	return S_OK;
}
HRESULT CD3DDevWrapper::SetViewport(MYD3DVIEWPORT* pViewport)
{
	if( m_pD3DDev != NULL )
	{
#ifndef _XBOX // Confirmed needed on Xbox
		if( m_savedViewport.X		!= pViewport->X ||
			m_savedViewport.Y		!= pViewport->Y ||
			m_savedViewport.Width	!= pViewport->Width ||
			m_savedViewport.Height	!= pViewport->Height ||
			m_savedViewport.MinZ	!= pViewport->MinZ ||
			m_savedViewport.MaxZ	!= pViewport->MaxZ )
#endif
		{
#ifdef _XBOX
			if( pViewport->Width <= 0 )	pViewport->Width = 1;
			if( pViewport->Height <= 0 )	pViewport->Height = 1;
#endif
			m_savedViewport.X		= pViewport->X;
			m_savedViewport.Y		= pViewport->Y;
			m_savedViewport.Width	= pViewport->Width;
			m_savedViewport.Height	= pViewport->Height;
			m_savedViewport.MinZ	= pViewport->MinZ;
			m_savedViewport.MaxZ	= pViewport->MaxZ;
			
			
			try
			{
				return m_pD3DDev->SetViewport(pViewport);
			}
			catch(...)
			{
			}
		}
	}

	return S_OK;
}
HRESULT CD3DDevWrapper::SetTexture(DWORD Stage,MYIDirect3DBaseTexture* pTexture)
{
#ifdef _XBOX_HACK
	if( m_pD3DDev != NULL && Stage < 4 )
#else
	if( m_pD3DDev != NULL )
#endif
	{
#ifndef _XBOX
		if (m_savedTexturePointers[Stage] != pTexture )
#endif
		{
			m_savedTexturePointers[Stage] = pTexture;
			return m_pD3DDev->SetTexture( Stage, pTexture );
		}
	}

	return S_OK;
}

HRESULT CD3DDevWrapper::SetVertexShader(DWORD Handle)
{
	if( m_pD3DDev != NULL )
	{
#ifndef _XBOX // Confirmed needed on Xbox
		if( m_savedVertexShaderHander != Handle )
#endif
		{
			m_savedVertexShaderHander = Handle;
#if DX_VERSION == 8
			return m_pD3DDev->SetVertexShader(Handle);
#elif DX_VERSION == 9
			return m_pD3DDev->SetFVF(Handle);
#endif
		}
	}

	return S_OK;
}

void CD3DDevWrapper::SetD3DDev(MYLPDIRECT3DDEVICE pD3DDev)
{
	m_pD3DDev = pD3DDev;
	Initalize();
}

void CD3DDevWrapper::Initalize(void)
{
#if DX_VERSION == 8
	m_savedPixelShaderHander = ~0;
#elif DX_VERSION == 9
	m_savedPixelShaderHander = NULL;
#endif
	m_savedVertexShaderHander = ~0;
	memset(&m_savedRenderStates, 0xEE, sizeof(m_savedRenderStates));
	memset(&m_savedPixelShaderConstants, 0xEE, sizeof(m_savedPixelShaderConstants));
	memset(&m_savedViewport, 0xEE, sizeof(m_savedViewport));
	memset(&m_savedTextureStageStates, 0xEE, sizeof(m_savedTextureStageStates));
#if DX_VERSION == 9
	memset(&m_savedSamplerStates, 0xEE, sizeof(m_savedSamplerStates));
#endif
	memset(&m_savedTexturePointers, 0xEE, sizeof(m_savedTexturePointers));

	for( int i=0; i<8; i++ )
	{
		m_savedTextureStageStates[i][D3DTSS_TEXCOORDINDEX] = ~0;
	}
}
