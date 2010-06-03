//-----------------------------------------------------------------------------
// File: XBApp.cpp
//
// Desc: Application class for the XBox samples.
//
// Hist: 11.01.00 - New for November XDK release
//       12.15.00 - Changes for December XDK release
//       12.19.01 - Changes for March XDK release
//       11.04.03 - Profiling changes for December 2003 XDK release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <xtl.h>
#include "XBApp.h"
#include "XBProfiling.h"
#include <D3D8Perf.h>

//-----------------------------------------------------------------------------
// Global access to common members
//-----------------------------------------------------------------------------
CXBApplication*    g_pXBApp     = NULL;
LPDIRECT3DDEVICE8  g_pd3dDevice = NULL;
DWORD g_Width;
DWORD g_Height;

// Deadzone for the gamepad inputs
const SHORT XINPUT_DEADZONE = (SHORT)( 0.24f * FLOAT(0x7FFF) );


//-----------------------------------------------------------------------------
// Name: CXBApplication()
// Desc: Constructor
//-----------------------------------------------------------------------------
CXBApplication::CXBApplication()
{
    // Initialize member variables
    g_pXBApp          = this;

    // Direct3D variables
    m_pD3D            = NULL;
    m_pd3dDevice      = NULL;
    m_pDepthBuffer    = NULL;
    m_pBackBuffer     = NULL;

    // Variables to perform app timing
    m_bPaused         = FALSE;
    m_fTime           = 0.0f;
    m_fElapsedTime    = 0.0f;
    m_fAppTime        = 0.0f;
    m_fElapsedAppTime = 0.0f;
    m_fFPS            = 0.0f;
    m_strFrameRate[0] = L'\0';

    // Set up the presentation parameters for a double-buffered, 640x480,
    // 32-bit display using depth-stencil. Override these parameters in
    // your derived class as your app requires.
    ZeroMemory( &m_d3dpp, sizeof(m_d3dpp) );
    m_d3dpp.BackBufferWidth					= 640;
    m_d3dpp.BackBufferHeight				= 480;
    m_d3dpp.BackBufferFormat				= D3DFMT_LIN_R5G6B5; //D3DFMT_A8R8G8B8;
    m_d3dpp.BackBufferCount					= 1;
    m_d3dpp.EnableAutoDepthStencil			= TRUE;
    m_d3dpp.AutoDepthStencilFormat			= D3DFMT_D16/*D3DFMT_D24S8*/;
    m_d3dpp.SwapEffect						= D3DSWAPEFFECT_DISCARD;
	m_d3dpp.Flags							= D3DPRESENTFLAG_INTERLACED;
	m_d3dpp.FullScreen_PresentationInterval	= D3DPRESENT_INTERVAL_DEFAULT;
	m_d3dpp.FullScreen_RefreshRateInHz		= 60;


	//TESTME! HDTV Modes (Launcher only!), Skins should be modified accordingly
	//Parts of code by XPORT and nes6502
	DWORD videoFlags = XGetVideoFlags();
	if(XGetVideoStandard() == XC_VIDEO_STANDARD_PAL_I)
	{
		if(videoFlags & XC_VIDEO_FLAGS_PAL_60Hz)		// PAL 60 user
			m_d3dpp.FullScreen_RefreshRateInHz = 60;
		else
			m_d3dpp.FullScreen_RefreshRateInHz = 50;
	}

/*
    if(XGetAVPack() == XC_AV_PACK_HDTV)
        {
                if(videoFlags & XC_VIDEO_FLAGS_HDTV_1080i)
                {
                        m_d3dpp.Flags            = D3DPRESENTFLAG_WIDESCREEN | D3DPRESENTFLAG_INTERLACED;
                        m_d3dpp.BackBufferWidth  = 1920;
                        m_d3dpp.BackBufferHeight = 1080;
                }
                else if(videoFlags & XC_VIDEO_FLAGS_HDTV_720p)
                {
                        m_d3dpp.Flags            = D3DPRESENTFLAG_PROGRESSIVE | D3DPRESENTFLAG_WIDESCREEN;
                        m_d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
                        m_d3dpp.BackBufferWidth  = 1280;
                        m_d3dpp.BackBufferHeight = 720;
                }
				 else if(videoFlags & XC_VIDEO_FLAGS_HDTV_480p)
                {
                        m_d3dpp.Flags            = D3DPRESENTFLAG_PROGRESSIVE;
                        m_d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
                        m_d3dpp.BackBufferWidth  = 640;
                        m_d3dpp.BackBufferHeight = 480;
                }
		}
*/
	//Widescreen
	if((videoFlags & XC_VIDEO_FLAGS_WIDESCREEN) !=0)
	 {
		m_d3dpp.Flags = D3DPRESENTFLAG_WIDESCREEN;
	 }

		//480p 1.6 needs 32 bit backbuffer format
	 if(XGetAVPack() == XC_AV_PACK_HDTV){
		if( videoFlags & XC_VIDEO_FLAGS_HDTV_480p){
			m_d3dpp.Flags = D3DPRESENTFLAG_PROGRESSIVE;
			m_d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8; 
		}
	 }


	// use an orthogonal matrix for the projection matrix
    D3DXMATRIX mat;
	g_Width = 640;
	g_Height = 480;
	D3DXMatrixOrthoOffCenterLH(&mat, 0, g_Width, g_Height, 0, 0.0f, 1.0f);
/*
	if(XGetAVPack() == XC_AV_PACK_HDTV)
	{
          if(videoFlags & XC_VIDEO_FLAGS_HDTV_1080i)
		  {
			    g_Width = 1920;
				g_Height = 1080;
				D3DXMatrixOrthoOffCenterLH(&mat, 0, g_Width, g_Height, 0, 0.0f, 1.0f);
		  }
		  else
		  if(videoFlags & XC_VIDEO_FLAGS_HDTV_720p)
		  {
				g_Width = 1280;
				g_Height = 720;
				D3DXMatrixOrthoOffCenterLH(&mat, 0, g_Width, g_Height, 0, 0.0f, 1.0f);
		  }
		  else
		  if(videoFlags & XC_VIDEO_FLAGS_HDTV_480p)
		  {
				g_Width = 640;
				g_Height = 480;
				D3DXMatrixOrthoOffCenterLH(&mat, 0, g_Width, g_Height, 0, 0.0f, 1.0f);
		  }
	}
	else
	{
    //if HDTV is not supported
				g_Width = 640;
				g_Height = 480;
				D3DXMatrixOrthoOffCenterLH(&mat, 0, g_Width, g_Height, 0, 0.0f, 1.0f);
	}
*/


    // Specify number and type of input devices this app will be using. By
    // default, you can use 0 and NULL, which triggers XInputDevices() to
    // pre-alloc the default number and types of devices. To use chat or
    // other devices, override these variables in your derived class.
    m_dwNumInputDeviceTypes = 0;
    m_InputDeviceTypes      = NULL;
}




//-----------------------------------------------------------------------------
// Name: Create()
// Desc: Create the app
//-----------------------------------------------------------------------------
HRESULT CXBApplication::Create()
{
    HRESULT hr;

    // Create the Direct3D object
    OUTPUT_DEBUG_STRING( "XBApp: Creating Direct3D...\n" );
    if( NULL == ( m_pD3D = Direct3DCreate8(D3D_SDK_VERSION) ) )
    {
        OUTPUT_DEBUG_STRING( "XBApp: Unable to create Direct3D!\n" );
        return E_FAIL;
    }

    // Create the device
    OUTPUT_DEBUG_STRING( "XBApp: Creating the D3D device...\n" );
    if( FAILED( hr = m_pD3D->CreateDevice( 0, D3DDEVTYPE_HAL, NULL, 
                                           D3DCREATE_HARDWARE_VERTEXPROCESSING, 
                                           &m_d3dpp, &m_pd3dDevice ) ) )
    {
        OUTPUT_DEBUG_STRING( "XBApp: Could not create D3D device!\n" );
        return hr;
    }

    // Allow global access to the device
    g_pd3dDevice = m_pd3dDevice;

    // Store pointers to the depth and back buffers
    m_pd3dDevice->GetDepthStencilSurface( &m_pDepthBuffer );
    m_pd3dDevice->GetBackBuffer( 0, 0, &m_pBackBuffer );

    // Initialize core peripheral port support. Note: If these parameters
    // are 0 and NULL, respectively, then the default number and types of 
    // controllers will be initialized.
    XInitDevices( m_dwNumInputDeviceTypes, m_InputDeviceTypes );

    // Create the gamepad devices
    OUTPUT_DEBUG_STRING( "XBApp: Creating gamepad devices...\n" );
    if( FAILED( hr = XBInput_CreateGamepads( &m_Gamepad ) ) )
    {
        OUTPUT_DEBUG_STRING( "XBApp: Call to CreateGamepads() failed!\n" );
        return hr;
    }

    // Initialize the app's device-dependent objects
    OUTPUT_DEBUG_STRING( "XBApp: Initializing the app...\n" );
    if( FAILED( hr = Initialize() ) )
    {
        OUTPUT_DEBUG_STRING( "XBApp: Call to Initialize() failed!\n" );
        return hr;
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Destroy()
// Desc: Cleanup objects
//-----------------------------------------------------------------------------
VOID CXBApplication::Destroy()
{
    // Perform app-specific cleanup
    Cleanup();

    // Release display objects
    SAFE_RELEASE( m_pd3dDevice );
    SAFE_RELEASE( m_pD3D );
}




//-----------------------------------------------------------------------------
// Name: Run()
// Desc: 
//-----------------------------------------------------------------------------
INT CXBApplication::Run()
{
    OUTPUT_DEBUG_STRING( "XBApp: Running the application...\n" );

    // Get the frequency of the timer
    LARGE_INTEGER qwTicksPerSec;
    QueryPerformanceFrequency( &qwTicksPerSec );
    FLOAT fSecsPerTick = 1.0f / (FLOAT)qwTicksPerSec.QuadPart;

    // Save the start time
    LARGE_INTEGER qwTime, qwLastTime, qwElapsedTime;
    QueryPerformanceCounter( &qwTime );
    qwLastTime.QuadPart = qwTime.QuadPart;

    LARGE_INTEGER qwAppTime, qwElapsedAppTime;
    qwAppTime.QuadPart        = 0;
    qwElapsedTime.QuadPart    = 0;
    qwElapsedAppTime.QuadPart = 0;

    // Run the game loop, animating and rendering frames
    for( ;; )
    {
        // This code exists purely to trigger a FastCAP profile.
        // It disappears in debug and release builds.
        CXBSimpleFastCapProfile profile;
        // A FastCAP profile could also be triggered via controller input.
        if ( XBProfiling_IsFastCAPRequested() )
            profile.BeginProfiling();

        //-----------------------------------------
        // Handle input
        //-----------------------------------------

        // Read the input for all connected gamepads
        XBInput_GetInput( m_Gamepad );

        // Lump inputs of all connected gamepads into one common structure.
        // This is done so apps that need only one gamepad can function with
        // any gamepad.
        ZeroMemory( &m_DefaultGamepad, sizeof(m_DefaultGamepad) );
        INT nThumbLX = 0;
        INT nThumbLY = 0;
        INT nThumbRX = 0;
        INT nThumbRY = 0;

        for( DWORD i=0; i<4; i++ )
        {
            if( m_Gamepad[i].hDevice )
            {
                // Only account for thumbstick info beyond the deadzone
                if( m_Gamepad[i].sThumbLX > XINPUT_DEADZONE ||
                    m_Gamepad[i].sThumbLX < -XINPUT_DEADZONE )
                    nThumbLX += m_Gamepad[i].sThumbLX;
                if( m_Gamepad[i].sThumbLY > XINPUT_DEADZONE ||
                    m_Gamepad[i].sThumbLY < -XINPUT_DEADZONE )
                    nThumbLY += m_Gamepad[i].sThumbLY;
                if( m_Gamepad[i].sThumbRX > XINPUT_DEADZONE ||
                    m_Gamepad[i].sThumbRX < -XINPUT_DEADZONE )
                    nThumbRX += m_Gamepad[i].sThumbRX;
                if( m_Gamepad[i].sThumbRY > XINPUT_DEADZONE ||
                    m_Gamepad[i].sThumbRY < -XINPUT_DEADZONE )
                    nThumbRY += m_Gamepad[i].sThumbRY;

                m_DefaultGamepad.fX1 += m_Gamepad[i].fX1;
                m_DefaultGamepad.fY1 += m_Gamepad[i].fY1;
                m_DefaultGamepad.fX2 += m_Gamepad[i].fX2;
                m_DefaultGamepad.fY2 += m_Gamepad[i].fY2;
                m_DefaultGamepad.wButtons        |= m_Gamepad[i].wButtons;
                m_DefaultGamepad.wPressedButtons |= m_Gamepad[i].wPressedButtons;
                m_DefaultGamepad.wLastButtons    |= m_Gamepad[i].wLastButtons;

                if( m_Gamepad[i].Event )
                    m_DefaultGamepad.Event = m_Gamepad[i].Event;

                for( DWORD b=0; b<8; b++ )
                {
                    m_DefaultGamepad.bAnalogButtons[b]        |= m_Gamepad[i].bAnalogButtons[b];
                    m_DefaultGamepad.bPressedAnalogButtons[b] |= m_Gamepad[i].bPressedAnalogButtons[b];
                    m_DefaultGamepad.bLastAnalogButtons[b]    |= m_Gamepad[i].bLastAnalogButtons[b];
                }
            }
        }

        // Clamp summed thumbstick values to proper range
        m_DefaultGamepad.sThumbLX = (SHORT)nThumbLX;
        m_DefaultGamepad.sThumbLY = (SHORT)nThumbLY;
        m_DefaultGamepad.sThumbRX = (SHORT)nThumbRX;
        m_DefaultGamepad.sThumbRY = (SHORT)nThumbRY;

        // Handle special input combo to trigger a reboot to the Xbox Dashboard
        if( m_DefaultGamepad.bAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER] > 0 )
        {
            if( m_DefaultGamepad.bAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER] > 0 )
            {
                if( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_BLACK] )
                {
                    LD_LAUNCH_DASHBOARD LaunchData = { XLD_LAUNCH_DASHBOARD_MAIN_MENU };
                    XLaunchNewImage( NULL, (LAUNCH_DATA*)&LaunchData );
                }
            }
		}

        //-----------------------------------------
        // Perform app timing
        //-----------------------------------------

        // Check Start button
        if( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_START )
            m_bPaused = !m_bPaused;

        // Get the current time (keep in LARGE_INTEGER format for precision)
        QueryPerformanceCounter( &qwTime );
        qwElapsedTime.QuadPart = qwTime.QuadPart - qwLastTime.QuadPart;
        qwLastTime.QuadPart    = qwTime.QuadPart;
        if( m_bPaused )
            qwElapsedAppTime.QuadPart = 0;
        else
            qwElapsedAppTime.QuadPart = qwElapsedTime.QuadPart;
        qwAppTime.QuadPart    += qwElapsedAppTime.QuadPart;

        // Store the current time values as floating point
        m_fTime           = fSecsPerTick * ((FLOAT)(qwTime.QuadPart));
        m_fElapsedTime    = fSecsPerTick * ((FLOAT)(qwElapsedTime.QuadPart));
        m_fAppTime        = fSecsPerTick * ((FLOAT)(qwAppTime.QuadPart));
        m_fElapsedAppTime = fSecsPerTick * ((FLOAT)(qwElapsedAppTime.QuadPart));

        // Compute the FPS (frames per second) once per second
        static FLOAT fLastTime = 0.0f;
        static DWORD dwFrames  = 0L;
        dwFrames++;

        if( m_fTime - fLastTime > 1.0f )
        {
            m_fFPS    = dwFrames / ( m_fTime - fLastTime );
            fLastTime = m_fTime;
            dwFrames  = 0L;
            swprintf( m_strFrameRate, L"%0.02f fps", m_fFPS );
        }

        //-----------------------------------------
        // Animate and render a frame
        //-----------------------------------------

        // Frame move the scene
        FrameMove();

        // Render the scene
        Render();

        // Render the frame again if the external PIX performance tool is running
        // and has requested us to repeat the frame. Note that we don't move, but
        // rather re-render the scene exactly as before.
        if( TRUE == D3DPERF_QueryRepeatFrame() )
        {
            Render();
        }
    }
}




//-----------------------------------------------------------------------------
// Name: RenderGradientBackground()
// Desc: Draws a gradient filled background
//-----------------------------------------------------------------------------
HRESULT CXBApplication::RenderGradientBackground( DWORD dwTopColor, 
                                                  DWORD dwBottomColor )
{
    // Normally there is no need to clear the color buffer - if you are
    // rendering to the entire screen then you just need to clear the
    // z-buffer and stencil buffer. However, when doing a PIX push-buffer
    // capture - when D3DPERF_QueryRepeatFrame() returns TRUE - it can be
    // useful to clear the color buffer, to avoid having the previous
    // frame of data show up in the PIX images window.
    // The samples don't do this because they clear the entire screen immediately
    // anyway.
    D3DDevice::Clear( 0, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, 0x00000000, 1.0f, 0L );

    // Set states
    D3DDevice::SetTexture( 0, NULL );
    D3DDevice::SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_DISABLE );
    D3DDevice::SetRenderState( D3DRS_ZENABLE,          FALSE ); 
    D3DDevice::SetRenderState( D3DRS_FILLMODE,         D3DFILL_SOLID ); 
    D3DDevice::SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE ); 
    D3DDevice::SetRenderState( D3DRS_ALPHATESTENABLE,  FALSE ); 
    D3DDevice::SetVertexShader( D3DFVF_XYZRHW|D3DFVF_DIFFUSE );

    // Draw a background-filling quad

	/*
    D3DDISPLAYMODE mode;
    D3DDevice::GetDisplayMode( &mode );
    FLOAT fX1 = -0.5f;
    FLOAT fY1 = -0.5f;
    FLOAT fX2 = (FLOAT)mode.Width - 0.5f;
    FLOAT fY2 = (FLOAT)mode.Height - 0.5f;
*/

	FLOAT fX1 = -0.5f;
	FLOAT fY1 = -0.5f;
	FLOAT fX2 = (FLOAT)g_Width - 0.5f;
	FLOAT fY2 = (FLOAT)g_Height - 0.5f;

    D3DDevice::Begin( D3DPT_QUADLIST );
    D3DDevice::SetVertexDataColor( D3DVSDE_DIFFUSE, dwTopColor );
    D3DDevice::SetVertexData4f( D3DVSDE_VERTEX, fX1, fY1, 1.0f, 1.0f );
    D3DDevice::SetVertexData4f( D3DVSDE_VERTEX, fX2, fY1, 1.0f, 1.0f );
    D3DDevice::SetVertexDataColor( D3DVSDE_DIFFUSE, dwBottomColor );
    D3DDevice::SetVertexData4f( D3DVSDE_VERTEX, fX2, fY2, 1.0f, 1.0f );
    D3DDevice::SetVertexData4f( D3DVSDE_VERTEX, fX1, fY2, 1.0f, 1.0f );
    D3DDevice::End();

    return S_OK;
}



