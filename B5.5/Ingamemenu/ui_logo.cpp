#include "Panel.h"
#include <xbfont.h>
#include "../config.h"
#include <xgraphics.h>

#define VERSION L"Surreal64 XXX CE B5.5"
extern DWORD MenuItemColor;
extern DWORD TitleColor;
extern void LoadSkinFont();

extern char romname[256];
CPanel m_BgPanel;
CPanel m_BoxPanel;
CPanel m_PgPanel;
CPanel m_RenderPanel;
CPanel m_TargetPanel;
LPDIRECT3DTEXTURE8 bgTexture;
LPDIRECT3DTEXTURE8	GameTexture;
extern CXBFont	m_Font;					// Font	for	text display
extern LPDIRECT3DDEVICE8 g_pd3dDevice;

LPDIRECT3DSURFACE8 surface;
D3DSURFACE_DESC    surfDesc;
LPDIRECT3DTEXTURE8 texture;
LPDIRECT3DSURFACE8 texSurface;
//extern LPDIRECT3DDEVICE8 g_pD3DDev;	<- ???
extern bool onhd;
extern bool showdebug;

/*extern 	D3DPRESENT_PARAMETERS d3dpp;
D3DPRESENT_PARAMETERS*	g_pd3dpp		= NULL;
LPDIRECT3DSURFACE8		g_pBackBuffer	= NULL;
LPDIRECT3DSURFACE8		g_pDepthBuffer	= NULL;
LPDIRECT3DTEXTURE8 pTargetPicture;
LPDIRECT3DSURFACE8 pTargetSurface;
BOOL TargetIsTexture = FALSE;
float xoffsettarget, yoffsettarget, xscaletarget, yscaletarget;*/
extern char skinname[32];

void InitLogo(void)
{
 // thx to gamedev.net for the code - GogoAckman

  // get the back surface and its description
  g_pd3dDevice->GetBackBuffer(-1,0,&surface);
  surface->GetDesc(&surfDesc);
 
  // create a texture the same as the back surface
  g_pd3dDevice->CreateTexture(surfDesc.Width, surfDesc.Height, 0, D3DX_DEFAULT, surfDesc.Format, D3DPOOL_DEFAULT, &texture);
 
  // get the textures surface
  texture->GetSurfaceLevel(0, &texSurface);
 
  // copy the back surface to the textures surface
  D3DXLoadSurfaceFromSurface(texSurface, NULL, NULL, surface, NULL, NULL, D3DX_DEFAULT, 0);
 
  // release both surfaces
  texSurface->Release();
  surface->Release();

	m_RenderPanel.Create(g_pd3dDevice,	texture, true);
	char gamebgname[256];
	sprintf(gamebgname,"D:\\skins\\%s\\ingamebg.jpg",skinname);
	D3DXCreateTextureFromFileEx( g_pd3dDevice, gamebgname,D3DX_DEFAULT, D3DX_DEFAULT,	1, 0, D3DFMT_LIN_A8R8G8B8 ,	D3DPOOL_MANAGED,D3DX_FILTER_NONE , D3DX_FILTER_NONE, 0x00000000,NULL, NULL,&bgTexture);		
	m_BgPanel.Create(g_pd3dDevice,	bgTexture, true);
}

void DrawLogo()
{
	LoadSkinFont();
	m_BgPanel.Render(0,0);
	m_RenderPanel.Render(305 ,65 , 267 , 200 , false, 0);

	m_Font.Begin();

	// Title
	m_Font.DrawText(305, 20, TitleColor, VERSION, XBFONT_CENTER_X);

	if (showdebug) {
	MEMORYSTATUS memStat;
	WCHAR szMemStatus[128];

	GlobalMemoryStatus(&memStat);
	swprintf(szMemStatus,L"%d MB Free",(memStat.dwAvailPhys /1024 /1024));
	m_Font.DrawText(60, 35, TitleColor, szMemStatus, XBFONT_LEFT);
	
		// Ez0n3 - temp
		//WCHAR szVidMemStatus[128];
		//swprintf(szVidMemStatus,L"%d MB Free Vid",dwMaxVideoMem);
		//m_Font.DrawText(60, 60, 0xFFFF7F7f, szVidMemStatus, XBFONT_LEFT);
	
	}

	m_Font.End();
}

void DestroyLogo()
{
m_BgPanel.Destroy();
D3DXCreateTextureFromFileEx( g_pd3dDevice, "D:\\Media\\black.jpg",D3DX_DEFAULT, D3DX_DEFAULT,	1, 0, D3DFMT_LIN_A8R8G8B8 ,	D3DPOOL_MANAGED,D3DX_FILTER_NONE , D3DX_FILTER_NONE, 0x00000000,NULL, NULL,&bgTexture);		
m_BgPanel.Create(g_pd3dDevice,	bgTexture, true);
m_BgPanel.Render(0,0);

m_BgPanel.Destroy();
m_RenderPanel.Destroy();
}

bool tookscreenshot=false;
void TakeScreenshot()
{
	//This is broken - freakdave
	/*
	//Ez0n3 - remove invalid chars from romname for screenshot path - fixes problem with ss dir using ftp (exists, but not accessible)
	int trimchk = 0;
	int namecnt = 0;
	char romnamess[256];
	for (int i=0;i<256;i++){
		if (romname[i] != ' ' && trimchk == 0) trimchk = 1;
		if (trimchk == 1) {
			if (romname[i]=='<' || romname[i]=='>' || romname[i]=='=' || romname[i]=='?' || romname[i]==':' || romname[i]==';' || romname[i]=='*' || romname[i]=='+' || romname[i]==',' || romname[i]=='/' || romname[i]=='|') romnamess[namecnt] = '_';
			else if (romname[i] == '\0' || romname[i] == '\t' || romname[i] == '\n' || namecnt >= 42) break;
			else romnamess[namecnt] = romname[i];
			namecnt++;
		}
	}
*/
	if(onhd)
    CreateDirectory("D:\\Screenshots",NULL);
	else 
    CreateDirectory("T:\\Screenshots",NULL);

	FILE *fp;
	char screen[256];
	int actualscreen;

	if (onhd)
	sprintf(screen,"D:\\Screenshots\\%s",romname);
	else
	sprintf(screen,"T:\\Screenshots\\%s",romname);

	CreateDirectory(screen,NULL);
	for (int i=0;i<100;i++) {

		if(onhd)
		sprintf(screen,"D:\\Screenshots\\%s\\screenshot%d.bmp",romname,i);
		else
		sprintf(screen,"T:\\Screenshots\\%s\\screenshot%d.bmp",romname,i);

		if ((fp=fopen(screen,"r")) == NULL)
		{
          actualscreen = i;
		  break;
		}
	}
if (onhd)
sprintf(screen,"D:\\Screenshots\\%s\\screenshot%d.bmp",romname,actualscreen);
else
sprintf(screen,"T:\\Screenshots\\%s\\screenshot%d.bmp",romname,actualscreen);

D3DSurface *surface = NULL;
m_RenderPanel.Render(0,0);
g_pd3dDevice->GetRenderTarget(&surface);
XGWriteSurfaceToFile(surface, screen);
surface->Release();
DrawLogo();
tookscreenshot=true;
}
/*
void CreateRenderTarget()
{
	g_pd3dDevice->CreateTexture(640, 480, 1, 0, D3DFMT_LIN_X8R8G8B8, D3DPOOL_DEFAULT, &pTargetPicture);
	m_TargetPanel.Create(g_pd3dDevice, pTargetPicture, true);
	pTargetPicture->GetSurfaceLevel(0, &pTargetSurface);
//	g_pd3dDevice->GetDepthStencilSurface(&g_pDepthBuffer);
//	g_pd3dDevice->SetRenderTargetFast(pTargetSurface, g_pDepthBuffer, 0);

//	g_pd3dDevice->GetRenderTarget(&g_pBackBuffer);
}

void SetAsRenderTarget()
{
	//if (!TargetIsTexture) {
		g_pd3dDevice->GetDepthStencilSurface(&g_pDepthBuffer);
	    //g_pd3dDevice->GetBackBuffer(0, 0, &g_pBackBuffer);
		//g_pd3dDevice->GetRenderTarget(&g_pBackBuffer);
		//g_pd3dDevice->SetRenderTargetFast(pTargetSurface, NULL, 0);
		//D3DDevice::SetRenderTarget(pTargetSurface,0);
		//D3DDevice::GetDepthStencilSurface(&g_pDepthBuffer);
		//TargetIsTexture = TRUE;
	//}

		g_pd3dDevice->GetRenderTarget(&g_pBackBuffer);
		//g_pd3dDevice->GetBackBuffer(-1, 0, &pTargetSurface);
		g_pd3dDevice->SetRenderTargetFast(pTargetSurface, g_pDepthBuffer, 0);
		//pTargetPicture->GetSurfaceLevel(0, &texSurface);
 
        // copy the back surface to the textures surface
        //D3DXLoadSurfaceFromSurface(texSurface, NULL, NULL, pTargetSurface, NULL, NULL, D3DX_DEFAULT, 0);
		//m_TargetPanel.SetRenderVariables(180,180,180,180);
}

void RestoreRenderTarget()
{
//	if (TargetIsTexture) {
		g_pd3dDevice->SetRenderTargetFast(g_pBackBuffer, g_pDepthBuffer, 0);
		//D3DDevice::SetRenderTarget(g_pBackBuffer,g_pDepthBuffer);
//		TargetIsTexture = FALSE;
//	}
}

void RenderScreen()
{
	RestoreRenderTarget();
//	m_TargetPanel.SetTextureState(1);
	m_TargetPanel.Render(115,115);
}

void SetRenderVariables(float x, float y, float nw, float nh)
{
//	m_TargetPanel.SetRenderVariables(x, y, nw, nh, false, false);
}*/
