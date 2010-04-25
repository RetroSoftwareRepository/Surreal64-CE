////////////////////////////////////////////////////////////////////////////////
// UltraHLE - Ultra64 High Level Emulator
// Copyright (c) 1999, Epsilon and RealityMan
// THIS IS A PRIVATE NONPUBLIC VERSION. NOT FOR PUBLIC DISTRIBUTION!
// ultrahle.c


#include "ultrahle.h"
#include "Controller.h"
#include "IoSupport.h"
#include "../config.h"

#include <xfont.h>

#include <stdlib.h>
#include <io.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string.h>
#include <stdio.h>

#include <xbapp.h>
#include <xbresource.h>
#include <xbfont.h>

// GogoAckman config file
#include "../config.h"

void __EMU_LoadState(int index);
void __EMU_SaveState(int index);

extern "C" void _INPUT_LoadButtonMap(int *cfgData);

extern char skinname[32];
char szBuffer[ MAX_PATH ];             
HANDLE hInst;                          
HWND hwndMain;                         
HWND hwndStatus;              

HANDLE mainthread;                     
LPDWORD mainthreadid;
D3DPRESENT_PARAMETERS d3dpp; 

bool g_exitCmd = false;
BOOL menuWaiting = FALSE;

IDirect3D8				*g_pDirect3D;	
IDirect3DDevice8		*g_pDevice;
IDirect3DVertexBuffer8  *g_pVertexBuffer;


HRESULT Direct3DInit();

int emuthread(int value)
{
    main_thread();
	g_exitCmd = true;
    return(0);
}

HRESULT Direct3DInit()
{
    // Create the D3D object.
    if( NULL == ( g_pDirect3D = Direct3DCreate8( D3D_SDK_VERSION ) ) )
        return E_FAIL;

    // Set up the structure used to create the D3DDevice.
    ZeroMemory( &d3dpp, sizeof(d3dpp) );
    d3dpp.BackBufferWidth        = 640;
    d3dpp.BackBufferHeight       = 480;
    d3dpp.BackBufferFormat       = /*D3DFMT_A8R8G8B8*/D3DFMT_X1R5G5B5;
    d3dpp.BackBufferCount        = 1;
    d3dpp.EnableAutoDepthStencil = TRUE;
    d3dpp.AutoDepthStencilFormat = /*D3DFMT_D24S8*/D3DFMT_D16;
    d3dpp.SwapEffect             = D3DSWAPEFFECT_DISCARD;
	//d3dpp.MultiSampleType		 = D3DMULTISAMPLE_4_SAMPLES_SUPERSAMPLE_LINEAR;
    d3dpp.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_ONE;
	//d3dpp.FullScreen_RefreshRateInHz = 50;

	DWORD videoFlags = XGetVideoFlags();
	if(XGetVideoStandard() == XC_VIDEO_STANDARD_PAL_I)
	{
		if(videoFlags & XC_VIDEO_FLAGS_PAL_60Hz)		// PAL 60 user
			d3dpp.FullScreen_RefreshRateInHz = 60;
		else
			d3dpp.FullScreen_RefreshRateInHz = 50;
	}

	//Widescreen support ?
	if((videoFlags & XC_VIDEO_FLAGS_WIDESCREEN) !=0)
	{
		d3dpp.Flags = D3DPRESENTFLAG_WIDESCREEN;
	}


//The user can now choose between different vertex processing modes - freakdave

	if(VertexMode == 0){

    // Create the Direct3D device.
    if( FAILED( g_pDirect3D->CreateDevice( 0, D3DDEVTYPE_HAL, NULL,
                                      D3DCREATE_PUREDEVICE,
                                      &d3dpp, &g_pDevice ) ) )
        return E_FAIL;
	}


	if(VertexMode == 1){

    // Create the Direct3D device.
    if( FAILED( g_pDirect3D->CreateDevice( 0, D3DDEVTYPE_HAL, NULL,
                                      D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                                      &d3dpp, &g_pDevice ) ) )
        return E_FAIL;
	}


	if(VertexMode == 2){

    // Create the Direct3D device.
    if( FAILED( g_pDirect3D->CreateDevice( 0, D3DDEVTYPE_HAL, NULL,
                                      D3DCREATE_HARDWARE_VERTEXPROCESSING,
                                      &d3dpp, &g_pDevice ) ) )
        return E_FAIL;
	}

	if(VertexMode == 3){

    // Create the Direct3D device.
    if( FAILED( g_pDirect3D->CreateDevice( 0, D3DDEVTYPE_HAL, NULL,
                                      D3DCREATE_MIXED_VERTEXPROCESSING,
                                      &d3dpp, &g_pDevice ) ) )
        return E_FAIL;
	}

	// setup the view matrices
	D3DXMATRIX mat;
	
	D3DXMatrixPerspectiveFovLH(&mat, D3DXToRadian(90.0f), init.gfxwid/init.gfxhig, 0.9, 32768.0);
	g_pDevice->SetTransform(D3DTS_PROJECTION, (D3DMATRIX *)&mat);

	D3DXMatrixIdentity(&mat);
	g_pDevice->SetTransform(D3DTS_WORLD, (D3DMATRIX *)&mat);

	D3DXMatrixTranslation(&mat, 0, 0, 0);
	g_pDevice->SetTransform(D3DTS_VIEW, (D3DMATRIX *)&mat);
	
    g_pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	g_pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	// freakdave - edited by GogoAckman - reedited by freakdave
	g_pDevice->SetTextureStageState(0, D3DTSS_MINFILTER, TextureMode);
	g_pDevice->SetTextureStageState(0, D3DTSS_MAGFILTER, TextureMode);


	g_pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
	g_pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);

	g_pDevice->SetRenderState( D3DRS_SHADEMODE,  D3DSHADE_GOURAUD );
	g_pDevice->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_RGBA(0, 0, 0, 0));

	g_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

	g_pDevice->CreateVertexBuffer(sizeof(CVertex)*4, NULL, D3DFVF_CVertex, D3DPOOL_MANAGED, &g_pVertexBuffer);


	return S_OK;
}
CXBFont		m_Font;					// Font	for	text display
CXBFont		m_MSFont;					// Font	for	buttons

extern int ControllerConfig[72];
extern bool bloadstate[5];
extern bool bsavestate[5];
extern bool onhd;
extern "C" void loadinis();

void __cdecl main()
{


	g_IOSupport.Mount("A:","cdrom0");
	g_IOSupport.Mount("E:","Harddisk0\\Partition1");
	g_IOSupport.Mount("Z:","Harddisk0\\Partition2");
	g_IOSupport.Mount("F:","Harddisk0\\Partition6");
	g_IOSupport.Mount("G:","Harddisk0\\Partition7");

	init.gfxwid = 640;
    init.gfxhig = 480;

	loadinis();

	_INPUT_LoadButtonMap(ControllerConfig);

    // Initialize Direct3D
    if( FAILED( Direct3DInit() ) )
        return;

	g_pd3dDevice = g_pDevice;
	
	char fontname[256];
	sprintf(fontname,"D:\\Skins\\%s\\Font.xpr",skinname);
	m_Font.Create(fontname);
	sprintf(fontname,"D:\\Skins\\%s\\MsFont.xpr",skinname);
	m_MSFont.Create(fontname); 
	

	main_startup();

    int mainthreadid;
    mainthread = CreateThread(NULL, 0,
							 (LPTHREAD_START_ROUTINE)emuthread,
							  NULL, 0, (LPDWORD)&mainthreadid);

	bool loaded = false;

	CHAR cmd[MAX_PATH];
	strcpy(cmd, "rom Z:\\TemporaryRom.dat");

	while(TRUE)
	{
		Sleep(1000);
		for (int i=0;i<5;i++){
			if (bloadstate [i]) {
				__EMU_LoadState(i+1);
			    bloadstate[i]=false;}
			if (bsavestate [i]) {
				__EMU_SaveState(i+1);
			    bsavestate[i]=false;}
		}
		if (!loaded)
		{
			main_command(cmd);
			if(main_commanderrors()>1)				break;
			else  main_start();
			loaded = true;
		}

// Menu moved to RDP.cpp , more fast :) - GogoAckman

		if (g_exitCmd)
		{
			LAUNCH_DATA ld;
			ZeroMemory(&ld, sizeof(LAUNCH_DATA));

			Controller_Finalise();
			
			XLaunchNewImage("D:\\default.xbe", &ld);
			break;
		}
	}
}

void __EMU_SaveState(int index)
{
	CHAR stateFileName[255];
	CHAR tmp[350];
	sprintf(stateFileName, "T:\\%s%i.sav", cart.title, index);

	sprintf(tmp, "save %s", stateFileName);
	main_command(tmp);
	if(main_commanderrors()>1)
	{
		g_exitCmd = true;
	}
	else
	{
		main_start();
	}
}

void __EMU_LoadState(int index)
{
	CHAR stateFileName[255];
	CHAR tmp[350];
	sprintf(stateFileName, "T:\\%s%i.sav", cart.title, index);

	sprintf(tmp, "load %s", stateFileName);
	main_command(tmp);
	if(main_commanderrors()>1)
	{
		g_exitCmd = true;
	}
	else
	{
		main_start();
	}
}
