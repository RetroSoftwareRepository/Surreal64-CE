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
#include "../../../config.h"

#include <xbapp.h>
#include <xbresource.h>
#include <xbfont.h>

LPDIRECT3DDEVICE8 g_pD3DDev = NULL;
D3DCAPS8 g_D3DDeviceCaps;
extern DWORD statusBarHeightToUse;
extern DWORD toolbarHeight;
D3DPRESENT_PARAMETERS d3dpp;
extern LPDIRECT3DSURFACE8		g_pBackBuffer;
extern LPDIRECT3DSURFACE8		g_pDepthBuffer;
CXBFont		m_Font;					// Font	for	text display
CXBFont		m_MSFont;					// Font	for	buttons
extern void CreateRenderTarget();
extern char skinname[32];
extern "C" char emuname[256];
/*
 *	Constants
 */
/*
D3DFMT_D16_LOCKABLE         = 70,
D3DFMT_D32                  = 71,
D3DFMT_D15S1                = 73,
D3DFMT_D24S8                = 75,
D3DFMT_D16                  = 80,
D3DFMT_D24X8                = 77,
D3DFMT_D24X4S4              = 79,
*/

BufferSettingInfo DirectXRenderBufferSettings[] =
{
	"Double Buffer Copy",		1,		D3DSWAPEFFECT_COPY,
	"Double Buffer Copy Sync",	1,		D3DSWAPEFFECT_COPY_VSYNC,
	"Double Buffer Flip",		1,		D3DSWAPEFFECT_FLIP,
	//"Double Buffer Discard",	1,		D3DSWAPEFFECT_DISCARD,
	"Triple Buffer Flip",		2,		D3DSWAPEFFECT_FLIP,
	"Quadruple Buffer Flip",	3,		D3DSWAPEFFECT_FLIP,
	//"Triple Buffer Discard",	2,		D3DSWAPEFFECT_DISCARD,
};

BufferSettingInfo DirectXDepthBufferSetting[] =
{
	"Default",					D3DFMT_D16,				D3DFMT_D16,
	"16-bit",					D3DFMT_D16,				D3DFMT_D16,
//	"16-bit signed",			D3DFMT_D15S1,			D3DFMT_D15S1,
//	"16-bit lockable",			D3DFMT_D16_LOCKABLE,	D3DFMT_D16_LOCKABLE,
//	"32-bit Depth Buffer",		D3DFMT_D32,				D3DFMT_D32,
//	"32-bit signed",			D3DFMT_D24S8,			D3DFMT_D24S8,
//	"32-bit D24X8",				D3DFMT_D24X4S4,			D3DFMT_D24X4S4,
//	"32-bit D24X4S4",			D3DFMT_D24X8,			D3DFMT_D24X8,
};

BufferSettingInfo DirectXCombinerSettings[] =
{
	"To Fit Your Video Card",			DX_BEST_FIT,		DX_BEST_FIT,
	"For Lowest End Video Cards",		DX_LOWEST_END,		DX_LOWEST_END,
	//"For Lower End Video Cards",		DX_LOWER_END,		DX_LOWER_END,
	"For Middle Level Video Cards",		DX_MIDDLE_LEVEL,	DX_MIDDLE_LEVEL,
	"For Higher End Video Card",		DX_HIGHER_END,		DX_HIGHER_END,
	"For NVidia TNT/TNT2/Geforce or better",	DX_NVIDIA_TNT,		DX_NVIDIA_TNT,
	//"For NVidia Geforce2 or better",	DX_NVIDIA_GEFORCE_PLUS,	DX_NVIDIA_GEFORCE_PLUS,
	//"For ATI Radeon or better",		DX_ATI_RADEON_PLUS,	DX_ATI_RADEON_PLUS,
};

int numberOfDirectXRenderBufferSettings = sizeof(DirectXRenderBufferSettings)/sizeof(BufferSettingInfo);
int numberOfDirectXCombinerSettings = sizeof(DirectXCombinerSettings)/sizeof(BufferSettingInfo);
int numberOfDirectXDepthBufferSettings = sizeof(DirectXDepthBufferSetting)/sizeof(BufferSettingInfo);

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDirectXGraphicsContext::CDirectXGraphicsContext() :
	m_pd3dDevice(NULL),
	m_pD3D(NULL),
	m_dwNumAdapters(0),
	m_dwAdapter(0),			// Default Adapter
	m_dwCreateFlags(0),
	m_dwMinDepthBits(16),
	m_dwMinStencilBits(0),
	//m_desktopFormat(D3DFMT_A8R8G8B8),
	m_desktopFormat(D3DFMT_LIN_R5G6B5),
	pCurrentRenderBuffer(NULL)
{
	m_strDeviceStats[0] = '\0';
	m_currentBackBufferIdx = 0;
}

//*****************************************************************************
//
//*****************************************************************************
CDirectXGraphicsContext::~CDirectXGraphicsContext()
{
	CGraphicsContext::Get()->CleanUp();
}

//*****************************************************************************
//
//*****************************************************************************
void CDirectXGraphicsContext::Clear(ClearFlag dwFlags, DWORD color, float depth)
{
	DWORD flag=0;
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
void ClearFrameBufferToBlack(DWORD left=0, DWORD top=0, DWORD width=0, DWORD height=0);
bool ProcessFrameWriteRecord();
extern RECT frameWriteByCPURect;
extern bool bloadstate[5];
extern bool bsavestate[5];
extern "C" void __EMU_SaveState(int index);
extern "C" void __EMU_LoadState(int index);
extern bool onhd;
extern bool showdebug;
extern void RenderScreen();
extern void SetAsRenderTarget();
extern LPDIRECT3DSURFACE8 pTargetSurface;
bool draw = true;
void CDirectXGraphicsContext::UpdateFrame(bool swaponly)
{

	HRESULT hr;

				for (int i=0;i<5;i++){
			if (bloadstate [i]) {
				__EMU_LoadState(i+1);
			    bloadstate[i]=false;}
			if (bsavestate [i]) {
				__EMU_SaveState(i+1);
			    bsavestate[i]=false;}
		}

	CGraphicsContext::UpdateFrameBufferBeforeUpdateFrame();

	if (showdebug) {
		static DWORD lastTick = GetTickCount() / 1000;
		static int lastTickFPS = 0;
		static int frameCount = 0;

		if (lastTick != GetTickCount() / 1000)
		{
			lastTickFPS = frameCount;
			frameCount = 0;
			lastTick = GetTickCount() / 1000;
		}
frameCount++;
WCHAR str[10];
swprintf(str,L"%i fps", lastTickFPS);
MEMORYSTATUS memStat;
WCHAR szMemStatus[128];

GlobalMemoryStatus(&memStat);
swprintf(szMemStatus,L"%d Mb Free",(memStat.dwAvailPhys /1024 /1024));
WCHAR debugemu[256];
swprintf(debugemu,L"%S",emuname);

// m_pd3dDevice->Clear(0,NULL,D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,D3DCOLOR_XRGB(100,100,100),1.0f,0);
//  SetAsRenderTarget();
//  Clear(CLEAR_DEPTH_BUFFER);
  m_Font.Begin();
  m_Font.DrawText(60, 35, 0xFFFF7F7f, szMemStatus, XBFONT_LEFT);
  m_Font.DrawText(60, 50, 0xFFFF7F7f, str, XBFONT_LEFT);
  m_Font.DrawText(60, 65, 0xFFFF7F7f, debugemu, XBFONT_LEFT);
  m_Font.End();

//  RenderScreen();
//  m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

	}

  m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

/*	Lock();
	if (m_pd3dDevice == NULL)
	{
		hr = E_FAIL;
	}
	else
	{
		if( !currentRomOptions.forceBufferClear )	
				   Clear(CLEAR_DEPTH_BUFFER);
		if( swaponly )
		{
			const RECT rect={0,0,1,1};
			hr = m_pd3dDevice->Present( &rect, &rect, NULL, NULL );
		}
		else {
			hr = m_pd3dDevice->Present( NULL, NULL, NULL, NULL );
		}
#ifdef _DEBUG
		if( pauseAtNext && eventToPause == NEXT_FRAME )
		{
			TRACE0("Update screen");
		}
#endif

	}
 
	Unlock();*/

	if( currentRomOptions.forceBufferClear )	needCleanScene = true;
}


//*****************************************************************************
//
//*****************************************************************************
void CDirectXGraphicsContext::DumpScreenShot()
{
	 
}

//-----------------------------------------------------------------------------
// Name: FindDepthStencilFormat()
// Desc: Finds a depth/stencil format for the given device that is compatible
//       with the render target format and meets the needs of the app.
//-----------------------------------------------------------------------------
BOOL CDirectXGraphicsContext::FindDepthStencilFormat( UINT iAdapter, D3DDEVTYPE DeviceType,
											  D3DFORMAT TargetFormat,
											  D3DFORMAT* pDepthStencilFormat )
{
	
	/*if( m_dwMinDepthBits <= 16 && m_dwMinStencilBits == 0 )
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
	
 
	
	if( m_dwMinDepthBits <= 24 && m_dwMinStencilBits == 0 )
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
	
	if( m_dwMinDepthBits <= 32 && m_dwMinStencilBits == 0 )
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
	
	return FALSE;*/

	*pDepthStencilFormat = D3DFMT_D16;
	return TRUE;
}

//*****************************************************************************
//
//*****************************************************************************
extern void WriteConfiguration(void);
extern "C" void _INPUT_LoadButtonMap(int *cfgData); 
extern int ControllerConfig[72];

bool CDirectXGraphicsContext::Initialize(HWND hWnd, HWND hWndStatus,
									 DWORD dwWidth, DWORD dwHeight,
									 BOOL bWindowed )
{
	HRESULT hr;
  
	Lock();

	  // Create the Direct3D object
    m_pD3D = Direct3DCreate8( D3D_SDK_VERSION );
    if( m_pD3D == NULL )
	{
		Unlock();
        DisplayErrorMsg( D3DAPPERR_NODIRECT3D, MSGERR_APPMUSTEXIT );
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
        DisplayErrorMsg( hr, MSGERR_APPMUSTEXIT );
		return false;
    }

	CGraphicsContext::Initialize(hWnd, hWndStatus, dwWidth, dwHeight, bWindowed );
	
	hr = Create( bWindowed );

	// Clear/Update a few times to ensure that all of the buffers are cleared
	if ( m_pd3dDevice )
	{
		for (int i=0; i<DirectXRenderBufferSettings[options.RenderBufferSetting].number+1; i++ )
		{
			Clear(CLEAR_COLOR_AND_DEPTH_BUFFER);
			m_pd3dDevice->BeginScene();
			m_pd3dDevice->EndScene();
			//SwapBuffer();
			m_pd3dDevice->Present( NULL, NULL, NULL, NULL );
		}
	}

	D3DVIEWPORT8 vp = {
		0, 
			toolbarHeightToUse, 
			windowSetting.uDisplayWidth, 
			windowSetting.uDisplayHeight, 0, 1
	};
	g_pD3DDev->SetViewport(&vp);
	
	Unlock();

	g_pD3DDev->GetDeviceCaps(&g_D3DDeviceCaps);
	CGraphicsContext::Get()->m_supportTextureMirror = false;
	if( g_D3DDeviceCaps.TextureAddressCaps & D3DPTADDRESSCAPS_MIRROR )
	{
		CGraphicsContext::Get()->m_supportTextureMirror = true;
	}
	
	if( g_D3DDeviceCaps.DevCaps&D3DDEVCAPS_HWTRANSFORMANDLIGHT || g_D3DDeviceCaps.DevCaps&D3DDEVCAPS_PUREDEVICE )
	{
		// Force to use Software T&L
		//g_pD3DDev->SetRenderState(D3DRS_SOFTWAREVERTEXPROCESSING,TRUE);
	}
  

	// GogoAckman
	g_pd3dDevice = g_pD3DDev;
	char fontname[256];
	sprintf(fontname,"D:\\Skins\\%s\\Font.xpr",skinname);
	m_Font.Create(fontname);
	sprintf(fontname,"D:\\Skins\\%s\\MsFont.xpr",skinname);
	m_MSFont.Create(fontname); 

	_INPUT_LoadButtonMap(ControllerConfig);

    strcat(emuname," Video 5.10");
	//D3DDevice::GetDepthStencilSurface(&g_pDepthBuffer);


	//CreateRenderTarget();
	return hr==S_OK;
}


//*****************************************************************************
//
//*****************************************************************************
HRESULT CDirectXGraphicsContext::Create( BOOL bWindowed )
{
    HRESULT hr;
	
    // Get access to current adapter, device, and mode
//    D3DAdapterInfo* pAdapterInfo = &m_Adapters[m_dwAdapter];
//    D3DDeviceInfo*  pDeviceInfo  = &pAdapterInfo->devices[pAdapterInfo->dwCurrentDevice];
	
 	m_bWindowed = (bWindowed==TRUE);		// Make user Toggle manually for now!
    //pDeviceInfo->bWindowed = m_bWindowed;

    // Initialize the 3D environment for the app
    if( FAILED( hr = InitializeD3DEnvironment() ) )
    {
        SAFE_RELEASE( m_pD3D );
        return DisplayErrorMsg( hr, MSGERR_APPMUSTEXIT );
    }
	
    // The app is ready to go
    m_bReady = true;
	
    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: InitializeD3DEnvironment()
// Desc:
//-----------------------------------------------------------------------------

HRESULT CDirectXGraphicsContext::InitializeD3DEnvironment()
{
     HRESULT hr;
	
//    D3DAdapterInfo* pAdapterInfo = &m_Adapters[m_dwAdapter];
//    D3DDeviceInfo*  pDeviceInfo  = &pAdapterInfo->devices[pAdapterInfo->dwCurrentDevice];
//    D3DModeInfo*    pModeInfo    = &pDeviceInfo->modes[pDeviceInfo->dwCurrentMode];
 
    // Set up the presentation parameters
    ZeroMemory( &m_d3dpp, sizeof(m_d3dpp) );
    m_d3dpp.Windowed               = FALSE;
    m_d3dpp.BackBufferCount        = 1;
	//m_d3dpp.MultiSampleType        = D3DMULTISAMPLE_2_SAMPLES_MULTISAMPLE_QUINCUNX;
    //m_d3dpp.SwapEffect             = bufferSettings[curBufferSetting].swapEffect;
    m_d3dpp.SwapEffect             =  D3DSWAPEFFECT_COPY;
	m_d3dpp.EnableAutoDepthStencil = TRUE; /*m_bUseDepthBuffer;*/
    //m_d3dpp.AutoDepthStencilFormat = pModeInfo->DepthStencilFormat;
	m_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
    m_d3dpp.hDeviceWindow          = m_hWnd;
	m_d3dpp.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE/*D3DPRESENT_INTERVAL_ONE*/;
	
	m_d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	//m_d3dpp.Flags = D3DPRESENTFLAG_EMULATE_REFRESH_RATE;

    //m_d3dpp.BackBufferWidth  = pModeInfo->Width;
    //m_d3dpp.BackBufferHeight = pModeInfo->Height;
	m_d3dpp.BackBufferWidth = 640;
	m_d3dpp.BackBufferHeight = 480;
    m_d3dpp.BackBufferFormat = D3DFMT_LIN_R5G6B5;
	//m_d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;

	windowSetting.uDisplayWidth = m_d3dpp.BackBufferWidth;
	windowSetting.uDisplayHeight = m_d3dpp.BackBufferHeight;

	//m_desktopFormat = D3DFMT_A8R8G8B8/*D3DFMT_X1R5G5B5*/;
	m_desktopFormat = D3DFMT_LIN_R5G6B5;

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

		//480p
	 if(XGetAVPack() == XC_AV_PACK_HDTV){
		if( videoFlags & XC_VIDEO_FLAGS_HDTV_480p){
			m_d3dpp.Flags = D3DPRESENTFLAG_PROGRESSIVE;
			m_d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
		}
	 }
    
	//freakdave
	if(VertexMode == 0){
    // Create the device
    hr = m_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,
								NULL, D3DCREATE_PUREDEVICE, &m_d3dpp,
								&m_pd3dDevice );

	}


	if(VertexMode == 1){
    // Create the device
    hr = m_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,
								NULL, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &m_d3dpp,
								&m_pd3dDevice );

	}

	if(VertexMode == 2){
    // Create the device
    hr = m_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,
								NULL, D3DCREATE_HARDWARE_VERTEXPROCESSING, &m_d3dpp,
								&m_pd3dDevice );

	}

	if(VertexMode == 3){
    // Create the device
    hr = m_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,
								NULL, D3DCREATE_MIXED_VERTEXPROCESSING, &m_d3dpp,
								&m_pd3dDevice );

	}



    if( SUCCEEDED(hr) && m_pd3dDevice )
    {
		g_pD3DDev = m_pd3dDevice;
		d3dpp = m_d3dpp;
		
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

		//freakdave
		if(VertexMode == 0){
        m_dwCreateFlags = D3DCREATE_PUREDEVICE;
		}

		if(VertexMode == 1){
        m_dwCreateFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
		}

		if(VertexMode == 2){
        m_dwCreateFlags = D3DCREATE_HARDWARE_VERTEXPROCESSING;
		}

		if(VertexMode == 3){
        m_dwCreateFlags = D3DCREATE_MIXED_VERTEXPROCESSING;
		}



	/*
        // Store device description
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
        } */

	 
		
        // Store render target surface desc
      LPDIRECT3DSURFACE8 pBackBuffer;
        m_pd3dDevice->GetBackBuffer( 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer );
        pBackBuffer->GetDesc( &m_d3dsdBackBuffer );
        pBackBuffer->Release();
 	    //g_pBackBuffer 
		/*m_pd3dDevice->GetBackBuffer( 0, D3DBACKBUFFER_TYPE_MONO, &g_pBackBuffer );
        g_pBackBuffer->GetDesc( &m_d3dsdBackBuffer );
        g_pBackBuffer->Release();*/
        // Initialize the app's device-dependent objects
        hr = InitDeviceObjects();
        if( IsResultGood(hr,true) )
        {
            hr = RestoreDeviceObjects();
            if( SUCCEEDED(hr) )
            {
                m_bActive = true;
                return S_OK;
            }
        }
		
        // Cleanup before we try again (- shouldn't get here)
        InvalidateDeviceObjects();
        DeleteDeviceObjects();
        SAFE_RELEASE( m_pd3dDevice );
    }
	else
	{
		 
	}
	
	/*
	extern LPDIRECT3DSURFACE8 g_pLockableBackBuffer;
	if( g_pLockableBackBuffer == NULL )
	{
		if( IsResultGood(g_pD3DDev->CreateDepthStencilSurface(windowSetting.uDisplayWidth, windowSetting.uDisplayHeight, D3DFMT_D16_LOCKABLE, D3DMULTISAMPLE_NONE, &g_pLockableBackBuffer)) && g_pLockableBackBuffer )
		{
			g_pD3DDev->SetRenderTarget(NULL, g_pLockableBackBuffer);
			TRACE0("Created and use lockable depth buffer");
		}
		else
		{
			if( g_pLockableBackBuffer )
			{
				g_pLockableBackBuffer->Release();
				g_pLockableBackBuffer = NULL;
			}
			TRACE0("Can not create lockable depth buffer");
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
HRESULT CDirectXGraphicsContext::ResizeD3DEnvironment()
{
   /* HRESULT hr;
	
    // Release all vidmem objects
    if( FAILED( hr = InvalidateDeviceObjects() ) )
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
    LPDIRECT3DSURFACE8 pBackBuffer;
    m_pd3dDevice->GetBackBuffer( 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer );
    pBackBuffer->GetDesc( &m_d3dsdBackBuffer );
    pBackBuffer->Release();
 
    // Initialize the app's device-dependent objects
    hr = RestoreDeviceObjects();
    if( FAILED(hr) )
        return hr; */
	
    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: ToggleFullScreen()
// Desc: Called when user toggles between fullscreen mode and windowed mode
//-----------------------------------------------------------------------------
HRESULT CDirectXGraphicsContext::DoToggleFullscreen()
{
  /*  // Get access to current adapter, device, and mode
    D3DAdapterInfo* pAdapterInfo = &m_Adapters[m_dwAdapter];
    D3DDeviceInfo*  pDeviceInfo  = &pAdapterInfo->devices[pAdapterInfo->dwCurrentDevice];
    D3DModeInfo*    pModeInfo    = &pDeviceInfo->modes[pDeviceInfo->dwCurrentMode];
	
    // Need device change if going windowed and the current device
    // can only be fullscreen
    if( !m_bWindowed && !pDeviceInfo->bCanDoWindowed )
	{
		statusBarHeightToUse = statusBarHeight;
		toolbarHeightToUse = toolbarHeight;
		return ForceWindowed();
	}
	
    m_bReady = false;
	
    // Toggle the windowed state
    m_bWindowed = !m_bWindowed;
    pDeviceInfo->bWindowed = m_bWindowed;
	if( m_bWindowed )
	{
		statusBarHeightToUse = statusBarHeight;
		toolbarHeightToUse = toolbarHeight;
	}
	else
	{
		statusBarHeightToUse = 0;
		toolbarHeightToUse = 0;
	}
	
    // Prepare window for windowed/fullscreen change
    AdjustWindowForChange();
	
	Lock();
	CleanUp();
    m_pD3D = Direct3DCreate8( D3D_SDK_VERSION );
    if( m_pD3D == NULL )
	{
		Unlock();
        return DisplayErrorMsg( D3DAPPERR_NODIRECT3D, MSGERR_APPMUSTEXIT );
	}
	InitializeD3DEnvironment();
	Unlock();

	
    // When moving from fullscreen to windowed mode, it is important to
    // adjust the window size after resetting the device rather than
    // beforehand to ensure that you get the window size you want.  For
    // example, when switching from 640x480 fullscreen to windowed with
    // a 1000x600 window on a 1024x768 desktop, it is impossible to set
    // the window size to 1000x600 until after the display mode has
    // changed to 1024x768, because windows cannot be larger than the
    // desktop.
 
	*/
    m_bReady = true;
	
    return S_OK; 
}




//-----------------------------------------------------------------------------
// Name: ForceWindowed()
// Desc: Switch to a windowed mode, even if that means picking a new device
//       and/or adapter
//-----------------------------------------------------------------------------
HRESULT CDirectXGraphicsContext::ForceWindowed()
{
/*    HRESULT hr;
    D3DAdapterInfo* pAdapterInfoCur = &m_Adapters[m_dwAdapter];
    D3DDeviceInfo*  pDeviceInfoCur  = &pAdapterInfoCur->devices[pAdapterInfoCur->dwCurrentDevice];
    BOOL bFoundDevice = FALSE;
	
    if( pDeviceInfoCur->bCanDoWindowed )
    {
        bFoundDevice = TRUE;
    }
    else
    {
        // Look for a windowable device on any adapter
        D3DAdapterInfo* pAdapterInfo;
        DWORD dwAdapter;
        D3DDeviceInfo* pDeviceInfo;
        DWORD dwDevice;
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
	
    pDeviceInfoCur->bWindowed = TRUE;
    m_bWindowed = true;
	
    // Now destroy the current 3D device objects, then reinitialize
	
    m_bReady = false;
	
    // Release all scene objects that will be re-created for the new device
    InvalidateDeviceObjects();
    DeleteDeviceObjects();
	
    // Release display objects, so a new device can be created
	LONG nRefCount = m_pd3dDevice->Release();
    if( nRefCount > 0L )
	{
		//DBGConsole_Msg(0, "Refcount of device is %d", nRefCount );
        return DisplayErrorMsg( D3DAPPERR_NONZEROREFCOUNT, MSGERR_APPMUSTEXIT );
	}
	
    // Create the new device
    if( FAILED( hr = InitializeD3DEnvironment() ) )
        return DisplayErrorMsg( hr, MSGERR_APPMUSTEXIT );
    m_bReady = true; */
	
    return S_OK;
}



//-----------------------------------------------------------------------------
// Name: AdjustWindowForChange()
// Desc: Prepare the window for a possible change between windowed mode and
//       fullscreen mode.  This function is virtual and thus can be overridden
//       to provide different behavior, such as switching to an entirely
//       different window for fullscreen mode (as in the MFC sample apps).
//-----------------------------------------------------------------------------
HRESULT CDirectXGraphicsContext::AdjustWindowForChange()
{
 
    return S_OK;
}



//-----------------------------------------------------------------------------
// Name: SortModesCallback()
// Desc: Callback function for sorting display modes (used by BuildDeviceList).
//-----------------------------------------------------------------------------
static int _cdecl SortModesCallback( const VOID* arg1, const VOID* arg2 )
{
    
    return 0;
}




//-----------------------------------------------------------------------------
// Name: BuildDeviceList()
// Desc: From DX8 SDK Copyright (c) 1998-2000 Microsoft
//-----------------------------------------------------------------------------
HRESULT CDirectXGraphicsContext::BuildDeviceList()
{
   /* const DWORD dwNumDeviceTypes = 2;
    const CHAR* strDeviceDescs[] = { "HAL", "REF" };
    const D3DDEVTYPE DeviceTypes[] = { D3DDEVTYPE_HAL, D3DDEVTYPE_REF };
	
    BOOL bHALExists = FALSE;
    BOOL bHALIsWindowedCompatible = FALSE;
    BOOL bHALIsDesktopCompatible = FALSE;
    BOOL bHALIsSampleCompatible = FALSE;
	
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
        DWORD dwNumFormats      = 0;
        DWORD dwNumModes        = 0;
        DWORD dwNumAdapterModes = m_pD3D->GetAdapterModeCount( iAdapter );
		
        // Add the adapter's current desktop format to the list of formats
        formats[dwNumFormats++] = pAdapter->d3ddmDesktop.Format;
		
        for( UINT iMode = 0; iMode < dwNumAdapterModes; iMode++ )
        {
            // Get the display mode attributes
            D3DDISPLAYMODE DisplayMode;
            m_pD3D->EnumAdapterModes( iAdapter, iMode, &DisplayMode );
			
            // Filter out low-resolution modes
            if( DisplayMode.Width  < 320 || DisplayMode.Height < 200 )
                continue;
			
            // Check if the mode already exists (to filter out refresh rates)
            for( DWORD m=0L; m<dwNumModes; m++ )
            {
                if( ( modes[m].Width  == DisplayMode.Width  ) &&
                    ( modes[m].Height == DisplayMode.Height ) &&
                    ( modes[m].Format == DisplayMode.Format ) )
                    break;
            }
			
            // If we found a new mode, add it to the list of modes
            if( m == dwNumModes )
            {
                modes[dwNumModes].Width       = DisplayMode.Width;
                modes[dwNumModes].Height      = DisplayMode.Height;
                modes[dwNumModes].Format      = DisplayMode.Format;
                modes[dwNumModes].RefreshRate = 0;
                dwNumModes++;
				
                // Check if the mode's format already exists
                for( DWORD f=0; f<dwNumFormats; f++ )
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
            pDevice->strDesc        = strDeviceDescs[iDevice];
            pDevice->dwNumModes     = 0;
            pDevice->dwCurrentMode  = 0;
            pDevice->bCanDoWindowed = FALSE;
            pDevice->bWindowed      = FALSE;
            pDevice->MultiSampleType = D3DMULTISAMPLE_NONE;
			
            // Examine each format supported by the adapter to see if it will
            // work with this device and meets the needs of the application.
            BOOL  bFormatConfirmed[20];
            DWORD dwBehavior[20];
            D3DFORMAT fmtDepthStencil[20];
			
            for( DWORD f=0; f<dwNumFormats; f++ )
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
					
                    if( pDevice->d3dCaps.Caps2 & D3DCAPS2_CANRENDERWINDOWED )
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
                    if( pDevice->d3dCaps.DevCaps&D3DDEVCAPS_PUREDEVICE )
                    {
                        dwBehavior[f] = D3DCREATE_SOFTWARE_VERTEXPROCESSING  
							 ;
						
                        if( SUCCEEDED( ConfirmDevice( &pDevice->d3dCaps, dwBehavior[f],
							formats[f] ) ) )
                            bFormatConfirmed[f] = TRUE;
                    }
				
                    if ( FALSE == bFormatConfirmed[f] )
                    {
                        dwBehavior[f] = D3DCREATE_HARDWARE_VERTEXPROCESSING;
						
                        if( SUCCEEDED( ConfirmDevice( &pDevice->d3dCaps, dwBehavior[f],
							formats[f] ) ) )
                            bFormatConfirmed[f] = TRUE;
                    }
					
                    if ( FALSE == bFormatConfirmed[f] )
                    {
                        dwBehavior[f] = D3DCREATE_MIXED_VERTEXPROCESSING;
						
                        if( SUCCEEDED( ConfirmDevice( &pDevice->d3dCaps, dwBehavior[f],
							formats[f] ) ) )
                            bFormatConfirmed[f] = TRUE;
                    }
                }
				
                // Confirm the device/format for SW vertex processing
                if( FALSE == bFormatConfirmed[f] )
                {
                    dwBehavior[f] = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
					
                    if( SUCCEEDED( ConfirmDevice( &pDevice->d3dCaps, dwBehavior[f],
						formats[f] ) ) )
                        bFormatConfirmed[f] = TRUE;
                }
				
                // Find a suitable depth/stencil buffer format for this device/format
                if( bFormatConfirmed[f] ) //&& m_bUseDepthBuffer
                {
                    if( !FindDepthStencilFormat( iAdapter, pDevice->DeviceType,
                        formats[f], &fmtDepthStencil[f] ) )
                    {
                        bFormatConfirmed[f] = FALSE;
                    }
                }
            }
			
            // Add all enumerated display modes with confirmed formats to the
            // device's list of valid modes
            for( DWORD m=0L; m<dwNumModes; m++ )
            {
                for( DWORD f=0; f<dwNumFormats; f++ )
                {
                    if( modes[m].Format == formats[f] )
                    {
                        if( bFormatConfirmed[f] == TRUE )
                        {
                            // Add this mode to the device's list of valid modes
                            pDevice->modes[pDevice->dwNumModes].Width      = modes[m].Width;
                            pDevice->modes[pDevice->dwNumModes].Height     = modes[m].Height;
                            pDevice->modes[pDevice->dwNumModes].Format     = modes[m].Format;
                            pDevice->modes[pDevice->dwNumModes].dwBehavior = dwBehavior[f];
                            pDevice->modes[pDevice->dwNumModes].DepthStencilFormat = fmtDepthStencil[f];
                            pDevice->dwNumModes++;
							
                            if( pDevice->DeviceType == D3DDEVTYPE_HAL )
                                bHALIsSampleCompatible = TRUE;
                        }
                    }
                }
            }
			
            // Select any 640x480 mode for default (but prefer a 16-bit mode)
            for( m=0; m<pDevice->dwNumModes; m++ )
            {
//                if( pDevice->modes[m].Width==640 && pDevice->modes[m].Height==480 )
                if( pDevice->modes[m].Width==windowSetting.uFullScreenDisplayWidth && pDevice->modes[m].Height==windowSetting.uFullScreenDisplayHeight )
                {
                    pDevice->dwCurrentMode = m;
                    if( pDevice->modes[m].Format == D3DFMT_R5G6B5 ||
                        pDevice->modes[m].Format == D3DFMT_X1R5G5B5 ||
                        pDevice->modes[m].Format == D3DFMT_A1R5G5B5 )
                    {
                        break;
                    }
                }
            }
			
            // Check if the device is compatible with the desktop display mode
            // (which was added initially as formats[0])
            if( bFormatConfirmed[0] && (pDevice->d3dCaps.Caps2 & D3DCAPS2_CANRENDERWINDOWED) )
            {
                pDevice->bCanDoWindowed = TRUE;
                pDevice->bWindowed      = TRUE;
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
    for( DWORD a=0; a<m_dwNumAdapters; a++ )
    {
        for( DWORD d=0; d < m_Adapters[a].dwNumDevices; d++ )
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
                        DisplayErrorMsg( D3DAPPERR_NOHARDWAREDEVICE, MSGWARN_SWITCHEDTOREF );
                    else if( !bHALIsSampleCompatible )
                        DisplayErrorMsg( D3DAPPERR_HALNOTCOMPATIBLE, MSGWARN_SWITCHEDTOREF );
                    else if( !bHALIsWindowedCompatible )
                        DisplayErrorMsg( D3DAPPERR_NOWINDOWEDHAL, MSGWARN_SWITCHEDTOREF );
                    else if( !bHALIsDesktopCompatible )
                        DisplayErrorMsg( D3DAPPERR_NODESKTOPHAL, MSGWARN_SWITCHEDTOREF );
                    else // HAL is desktop compatible, but not sample compatible
                        DisplayErrorMsg( D3DAPPERR_NOHALTHISMODE, MSGWARN_SWITCHEDTOREF );
                }
				
                return S_OK;
            }
        }
    } */
	
    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: ConfirmDevice()
// Desc: Called during device initialization, this code checks the device
//       for some minimum set of capabilities
//-----------------------------------------------------------------------------
HRESULT CDirectXGraphicsContext::ConfirmDevice( D3DCAPS8* pCaps, DWORD dwBehavior,
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
// Name: DisplayErrorMsg()
// Desc: Displays error messages in a message box
//-----------------------------------------------------------------------------
HRESULT CDirectXGraphicsContext::DisplayErrorMsg( HRESULT hr, DWORD dwType )
{
     
 
    return hr;
}






//-----------------------------------------------------------------------------
// Name: CleanUp()
// Desc: Cleanup scene objects
//-----------------------------------------------------------------------------
VOID CDirectXGraphicsContext::CleanUp()
{
	CGraphicsContext::CleanUp();
	
    if( m_pd3dDevice )
    {
        InvalidateDeviceObjects();
        DeleteDeviceObjects();
		
        m_pd3dDevice->Release();
        m_pD3D->Release();
		
        m_pd3dDevice = NULL;
        m_pD3D       = NULL;
    }
	
	// FinalCleanup();
}


int CDirectXGraphicsContext::ToggleFullscreen()
{
    // Toggle the fullscreen/window mode
   /* if( m_bActive && m_bReady )
    {
        if( FAILED( DoToggleFullscreen() ) )
        {
            DisplayErrorMsg( D3DAPPERR_RESIZEFAILED, MSGERR_APPMUSTEXIT );
        }
		CDaedalusRender::GetRender()->Initialize();
    }
	return m_bWindowed?0:1;*/

	return 1;
}


//-----------------------------------------------------------------------------
// Name: Pause()
// Desc: Called in to toggle the pause state of the app.
//-----------------------------------------------------------------------------
void CDirectXGraphicsContext::Pause( bool bPause )
{
    static DWORD dwAppPausedCount = 0L;
	
    dwAppPausedCount += ( bPause ? +1 : -1 );
    m_bReady          = ( dwAppPausedCount ? false : true );
}





//-----------------------------------------------------------------------------
// Name: InitDeviceObjects()
// Desc: Initialize device-dependent objects. This is the place to create mesh
//       and texture objects.
//-----------------------------------------------------------------------------
HRESULT CDirectXGraphicsContext::InitDeviceObjects()
{
	// Set the device's states for rendering
	//return RDPInit();

	return S_OK;
}



//-----------------------------------------------------------------------------
// Name: RestoreDeviceObjects()
// Desc: Restore device-memory objects and state after a device is created or
//       resized.
//-----------------------------------------------------------------------------
HRESULT CDirectXGraphicsContext::RestoreDeviceObjects()
{
	if ( CDaedalusRender::IsAvailable() )
	{
		CDaedalusRender::GetRender()->RestoreDeviceObjects();
	}
	
	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: InvalidateDeviceObjects()
// Desc: Called when the device-dependent objects are about to be lost.
//-----------------------------------------------------------------------------
HRESULT CDirectXGraphicsContext::InvalidateDeviceObjects()
{
	// Kill all textures?
	gTextureCache.InvalidateDeviceObjects();

	if ( CDaedalusRender::IsAvailable() )
	{
		CDaedalusRender::GetRender()->InvalidateDeviceObjects();
	}

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: DeleteDeviceObjects()
// Desc: Called when the app is exiting, or the device is being changed,
//       this function deletes any device dependent objects.
//-----------------------------------------------------------------------------
HRESULT CDirectXGraphicsContext::DeleteDeviceObjects()
{
	//RDPCleanup();
    return S_OK;
}


bool CDirectXGraphicsContext::IsResultGood(HRESULT hr, bool displayError)
{
	if(FAILED(hr))
	{
		if( displayError )
		{
			char szError[200+1];
			D3DXGetErrorString(hr, szError, 200);
			TRACE1("D3D Error: %s", szError);
			ErrorMsg(szError);
		}
		return false;
	}
	else
		return true;
}

void CDirectXGraphicsContext::SetFrontBuffer(DWORD viorg)	// Change front buffer to the viorg addr
{
	//UpdateFrame();
	CGraphicsContext::SetFrontBuffer(viorg);
}
void CDirectXGraphicsContext::SetBackBuffer(DWORD ciaddr)	// Change back buffer to the ci addr
{
	u32 lastBackBufferIdx = m_currentBackBufferIdx;
	CGraphicsContext::SetBackBuffer(ciaddr);

	if( lastBackBufferIdx!=m_currentBackBufferIdx )
	{
#ifdef _DEBUG
		if( pauseAtNext && eventToPause == NEXT_FRAME )
		{
			DebuggerAppendMsg("Draw screen %d", m_currentBackBufferIdx);
		}
#endif

		m_SwapChainInfos[lastBackBufferIdx].beingRendered = false;
		m_SwapChainInfos[m_currentBackBufferIdx].beingRendered = true;
		int idx = (m_currentBackBufferIdx+m_numOfBackBuffers)-m_currentFrontBufferIdx-1;
		idx %= m_numOfBackBuffers;
		m_pd3dDevice->GetBackBuffer(idx, D3DBACKBUFFER_TYPE_MONO, &pCurrentRenderBuffer);
		LPDIRECT3DSURFACE8 pDepthBufferSurf;
		g_pD3DDev->GetDepthStencilSurface(&pDepthBufferSurf);
		m_pd3dDevice->SetRenderTarget(pCurrentRenderBuffer, pDepthBufferSurf);
		SAFE_RELEASE(pDepthBufferSurf);
		SAFE_RELEASE(pCurrentRenderBuffer);
	}
}


