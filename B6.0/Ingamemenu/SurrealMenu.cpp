#include "../config.h"
#include "../ultraxle/IoSupport.h"
#include <xbapp.h>
#include <xbresource.h>
#include <xbfont.h>
#include "../Ingamemenu/xlmenu.h"
#include "Panel.h"

#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
using namespace std;

extern "C" void __EMU_SaveState(int index);

extern "C" void __EMU_LoadState(int index);
extern "C" void __EMU_Load1964State(int index);
extern "C" void __EMU_LoadPJ64State(int index);

extern "C" void __EMU_GetStateFilename(int index, char *timestamp, int mode);
extern "C" void __EMU_Get1964StateFilename(int index, char *timestamp, int mode);
extern "C" void __EMU_GetPJ64StateFilename(int index, char *timestamp, int mode);

void GetStateTimestamp(int index, int emulator, char *timestamp);
void CreateSaveStatePreview(unsigned int index);
bool LoadSaveStatePreview(unsigned int index, int emulator);
bool bNoPreview = false;

extern void _VIDEO_DisplayTemporaryMessage(const char *msg);

//weinerschnitzel - Skin Control
extern char emuname[256];
extern DWORD dwMenuItemColor;
extern DWORD dwMenuTitleColor;
extern char skinname[32];
extern int iIGMMenuTxtPosX;
extern int iIGMMenuTxtPosY;
extern int iIGMMenuTxtAlign;
extern int iPanelX;
extern int iPanelY;
extern int iPanelNW;
extern int iPanelNH;
extern int iIGMMenuTxtPosX_HD;
extern int iIGMMenuTxtPosY_HD;
extern int iIGMMenuTxtAlign_HD;
extern int iPanelX_HD;
extern int iPanelY_HD;
extern int iPanelNW_HD;
extern int iPanelNH_HD;
extern bool isIGM;
extern int iIGMHilightPosX;
extern int iIGMHilightPosX_HD;

DWORD GetFontAlign(int iAlign);
DWORD GetMenuFontAlign(int iAlign);

//freakdave
enum Emulators
{
	_1964x085,
	_PJ64x14,
	_UltraHLE,
	_PJ64x16,
	_1964x11,
	//_Mupen64PlusX,
	_None
};

typedef struct _LaunchData
{

	// Ez0n3 - launchdata isn't used anymore, but this struct is used for exit to dash "LD_LAUNCH_DASHBOARD LaunchData" i believe
	/*
	// 1964 settings
	DWORD dw1964DynaMem;
	DWORD dw1964PagingMem;
	
	// Pj64 settings
	DWORD dwPJ64DynaMem;
	DWORD dwPJ64PagingMem;

	// common settings
	BOOL bUseLLERSP;
	
	// Ez0n3 - reinstate max video mem
	DWORD dwMaxVideoMem;
	*/
	
} LaunchData;

LPDIRECT3DTEXTURE8 pStateTexture = NULL;

/*#define SAVESTATE_XPOS 60
#define SAVESTATE_YPOS 270
#define SAVESTATE_XWIDTH 256
#define SAVESTATE_YHEIGHT 143//128 (256:143 = 16:9) (256:191 = 4:3)*/
extern int iIGMStateScreenX;
extern int iIGMStateScreenY;
extern int iIGMStateScreenW;
extern int iIGMStateScreenH;
extern int iIGMStateScreenX_HD;
extern int iIGMStateScreenY_HD;
extern int iIGMStateScreenW_HD;
extern int iIGMStateScreenH_HD;

XBGAMEPAD gamepad;

extern CPanel m_RenderPanel;
extern CPanel m_BgPanel;
extern CPanel m_MenuBgPanel;
extern CPanel m_MenuBg2Panel;
extern CPanel m_ControlConfigPanel;
extern void TakeScreenshot();

XLMenu    *m_pMainMenu;
XLMenu    *m_pSettingsMenu;
XLMenu    *m_pControllerMenu;
extern CXBFont		m_Font;					// Font	for	text display

extern void DrawLogo();
extern void DestroyLogo();
extern void ClearIGM();
static int currentItem;

void ExitToRomList(); 
void ExitToDash();
void ShutdownXbox();
void ToggleCredits();
void ToggleFlickerFilter(bool inc);
void VideoSettingsMenu();
void ToggleTextureFilter(bool inc);
void ToggleSoftDisplayFilter();
void ToggleFrameSkip(bool inc);
void ToggleAutoCF(bool inc);
void ToggleCF5toCF3StepUp(bool inc);
void ToggleCF3toCF1StepUp(bool inc);
void ToggleCF1toCF3StepDown(bool inc);
void ToggleCF3toCF5StepDown(bool inc);
void ToggleFogMode();
void ToggleXboxPitch();
void ToggleSensitivity(bool inc);
void ToggleDeadzone(bool inc);
void ToggleButtonToAxisThresh(bool inc);
void ControllerSettingsMenu();
void TogglePak(bool inc);
void ShowDebug();
void ResetRom();

// controller config

void Control();
void Control1();
void Control2();
void Control3();
void Control4();
void ControllerMenu();
extern void ChangeControl();

// Load-Save state
void Load1964StateMenu();
void Load1964State1();
void Load1964State2();
void Load1964State3();
void Load1964State4();
void Load1964State5();
void LoadPJ64StateMenu();
void LoadPJ64State1();
void LoadPJ64State2();
void LoadPJ64State3();
void LoadPJ64State4();
void LoadPJ64State5();
void LoadStateMenu();
void LoadState1();
void LoadState2();
void LoadState3();
void LoadState4();
void LoadState5();
void SaveStateMenu();
void SaveState1();
void SaveState2();
void SaveState3();
void SaveState4();
void SaveState5();
bool bloadstate[MAX_SAVE_STATES]; //5
bool bsavestate[MAX_SAVE_STATES]; //5
bool bload1964state[MAX_SAVE_STATES]; //5
bool bloadPJ64state[MAX_SAVE_STATES]; //5
bool bSatesUpdated = false;

extern void CalculateEndCredits();
extern void DrawCredits();
extern int defilement;
extern void InitLogo();

//
void incflicker(){ ToggleFlickerFilter(true); }
void decflicker(){ ToggleFlickerFilter(false); }
void incTextureFilter(){ ToggleTextureFilter(true); }
void decTextureFilter(){ ToggleTextureFilter(false); }
void incFrameSkip(){ ToggleFrameSkip(true); }
void decFrameSkip(){ ToggleFrameSkip(false); }
void incAutoCF(){ ToggleAutoCF(true); }
void decAutoCF(){ ToggleAutoCF(false); }
void incCF3toCF1StepUp(){ ToggleCF3toCF1StepUp(true); }
void decCF3toCF1StepUp(){ ToggleCF3toCF1StepUp(false); }
void incCF5toCF3StepUp(){ ToggleCF5toCF3StepUp(true); }
void decCF5toCF3StepUp(){ ToggleCF5toCF3StepUp(false); }
void incCF1toCF3StepDown(){ ToggleCF1toCF3StepDown(true); }
void decCF1toCF3StepDown(){ ToggleCF1toCF3StepDown(false); }
void incCF3toCF5StepDown(){ ToggleCF3toCF5StepDown(true); }
void decCF3toCF5StepDown(){ ToggleCF3toCF5StepDown(false); }
void incSensitivity(){ ToggleSensitivity(true); }
void decSensitivity(){ ToggleSensitivity(false); }
void incDeadzone(){ ToggleDeadzone(true); }
void decDeadzone(){ ToggleDeadzone(false); }
void incButtonToAxisThresh(){ ToggleButtonToAxisThresh(true); }
void decButtonToAxisThresh(){ ToggleButtonToAxisThresh(false); }

void incPakPlugin(){ TogglePak(true); }
void decPakPlugin(){ TogglePak(false); }
//
extern bool showdebug;
extern bool bEnableHDTV;
extern bool bFullScreen;
extern int XboxPitch;

enum Pak
{
	NoPak = 1,
	MemPak = 2,
	RumblePak = 3
};
extern bool tookscreenshot;
void MainMenu(void)
{
	isIGM = true;
	InitLogo();

	if (pStateTexture) {
		pStateTexture->Release();
		pStateTexture = NULL;
	}

	DWORD dwMenuCommand = 0;

	XLMenu_CurRoutine = NULL;
	XLMenu_CurMenu = NULL;
	XLMenu_SetFont(&m_Font);

	int x = 0;
	int y = 0;
	int nx = 640;
	int ny = 480;

	//fd - hack to properly display HD IGM
	DWORD videoFlags = XGetVideoFlags();
	if(XGetAVPack() == XC_AV_PACK_HDTV){
		if(videoFlags & XC_VIDEO_FLAGS_HDTV_720p && bEnableHDTV)
		{
			nx = 1280;
			ny = 720;
			iPanelX = iPanelX_HD;
			iPanelY = iPanelY_HD;
			iPanelNW = iPanelNW_HD;
			iPanelNH = iPanelNH_HD;
			iIGMMenuTxtPosX = iIGMMenuTxtPosX_HD;
			iIGMMenuTxtPosY = iIGMMenuTxtPosY_HD;
			iIGMHilightPosX = iIGMHilightPosX_HD;
			iIGMMenuTxtAlign = iIGMMenuTxtAlign_HD;
			
			iIGMStateScreenX = iIGMStateScreenX_HD;
			iIGMStateScreenY = iIGMStateScreenY_HD;
			iIGMStateScreenW = iIGMStateScreenW_HD;
			iIGMStateScreenH = iIGMStateScreenH_HD;
		}
	}
	
	if (iIGMStateScreenW > 512/*256*/)
		iIGMStateScreenW = 512/*256*/;
	if (iIGMStateScreenH > 512/*256*/)
		iIGMStateScreenH = 512/*256*/;
	
	if(preferedemu == _UltraHLE) // 12 Menu Items for UHLE for State Menus
		m_pMainMenu = XLMenu_Init((float)iIGMMenuTxtPosX,(float)iIGMMenuTxtPosY,12, GetMenuFontAlign(iIGMMenuTxtAlign)|MENU_WRAP,NULL);
	else						 // 13 Menu Items for 1964/PJ64 State Menus
		m_pMainMenu = XLMenu_Init((float)iIGMMenuTxtPosX,(float)iIGMMenuTxtPosY,13, GetMenuFontAlign(iIGMMenuTxtAlign)|MENU_WRAP,NULL);

	m_pMainMenu->itemcolor = dwMenuItemColor;
	m_pMainMenu->parent = NULL;

	while ((x != iPanelX) || (y != iPanelY) || (nx != iPanelNW) || (ny != iPanelNH))
	{
		g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER , D3DCOLOR_XRGB(0,0,0), 1.0f, 0 );
		m_BgPanel.Render(0,0);

		x = x + 32;
		y = y + 8;
		nx = nx - 38;
		ny = ny - 28;

		if (x >iPanelX)
			x = iPanelX;

		if (y >iPanelY)
			y = iPanelY;

		if (nx <iPanelNW)
			nx = iPanelNW;

		if (ny <iPanelNH)
			ny = iPanelNH;

		m_RenderPanel.Render((float)x ,(float)y , (float)nx , (float)ny , false, 0);
		g_pd3dDevice->Present( NULL, NULL, NULL, NULL );

	}

	XLMenu_SetTitle(m_pMainMenu,L"Main Menu",dwMenuTitleColor);

	if(preferedemu == _UltraHLE)
	XLMenu_AddItem(m_pMainMenu,MITEM_ROUTINE,L"Load State",LoadStateMenu);
	else
	{
		XLMenu_AddItem(m_pMainMenu,MITEM_ROUTINE,L"Load 1964 State",Load1964StateMenu);
		XLMenu_AddItem(m_pMainMenu,MITEM_ROUTINE,L"Load PJ64 State",LoadPJ64StateMenu);
	}

	XLMenu_AddItem(m_pMainMenu,MITEM_ROUTINE,L"Save State",SaveStateMenu);
	if (showdebug) 
	XLMenu_AddItem(m_pMainMenu,MITEM_ROUTINE,L"Show Debug info : on",ShowDebug);
	else
	XLMenu_AddItem(m_pMainMenu,MITEM_ROUTINE,L"Show Debug info : off",ShowDebug);
	XLMenu_AddItem(m_pMainMenu,MITEM_ROUTINE,L"Video Settings",VideoSettingsMenu);
	XLMenu_AddItem(m_pMainMenu,MITEM_ROUTINE,L"Controller Settings",ControllerSettingsMenu);
	XLMenu_AddItem(m_pMainMenu,MITEM_ROUTINE,L"Controller Config",ControllerMenu);
	XLMenu_AddItem(m_pMainMenu,MITEM_ROUTINE,L"Take a Screenshot",TakeScreenshot);
	XLMenu_AddItem(m_pMainMenu,MITEM_DISABLED,L"------------------------",NULL);
	XLMenu_AddItem(m_pMainMenu,MITEM_ROUTINE,L"Reset Rom",ResetRom);
	XLMenu_AddItem(m_pMainMenu,MITEM_ROUTINE,L"Quit To RomList",ExitToRomList);
	XLMenu_AddItem(m_pMainMenu,MITEM_ROUTINE,L"Quit Surreal",ExitToDash);
	XLMenu_AddItem(m_pMainMenu,MITEM_ROUTINE,L"Shutdown Xbox",ShutdownXbox);

	XLMenu_Activate(m_pMainMenu);

	while( XLMenu_CurMenu == m_pMainMenu)
	{
		DrawLogo();
		dwMenuCommand = getAllGamepadsCommand(&gamepad);
		XLMenu_Routine(dwMenuCommand);
		g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
		if (tookscreenshot) {
		tookscreenshot=false;
        XLMenu_Activate(NULL);
		}
	}
	
	XLMenu_CurRoutine = NULL;
	XLMenu_CurMenu = NULL;

	if (m_pMainMenu)
		XLMenu_Delete(m_pMainMenu);

	DestroyLogo();
	//ClearIGM();

}

void ShowDebug()
{
	WCHAR currentname[120];
	currentItem = m_pMainMenu->curitem;
	showdebug =! showdebug;
	
	XLMenu_CurRoutine = NULL;
	
  	if (showdebug)
	swprintf(currentname,L"Show Debug info : on");
	else 
	swprintf(currentname,L"Show Debug info : off");
	XLMenu_SetItemText(&m_pMainMenu->items[currentItem], currentname);
	ConfigAppSave2();
}

void ExitToRomList() 
{
	// Ez0n3 - launchdata isn't used anymore
	//LAUNCH_DATA ld;
	//ZeroMemory(&ld, sizeof(LAUNCH_DATA));
	
	//Ez0n3 - passing the zeroed launch data crashes surreal sometimes (usually 1964/UltraHLE - unknown reason). NULL seems to be more reliable with no adverse affects.
	//XLaunchNewImage("D:\\default.xbe", &ld);
	//weinersch - 4xAA will cause a frag when using PersistDisplay(), let's only use it without AA
	if(AntiAliasMode == 0)
		D3DDevice::PersistDisplay();
	XLaunchNewImage("D:\\default.xbe", NULL);
}

void ExitToDash(void)
{
	D3DDevice::PersistDisplay();
	LD_LAUNCH_DASHBOARD LaunchData = { XLD_LAUNCH_DASHBOARD_MAIN_MENU };
	XLaunchNewImage( NULL, (LAUNCH_DATA*)&LaunchData );
}

void ShutdownXbox(void)
{
	CIoSupport IOSupport;
	IOSupport.Shutdown();
}

void ResetRom(void)
{
	char szLaunchXBE[128];
	
	if (preferedemu == _UltraHLE)
	{
		sprintf(szLaunchXBE, "D:\\UltraHLE.xbe");
	}
	else
	{
		char szEmulator[16];
		char szAudioPlugin[16];
		char szVideoPlugin[16];
		//char szPagingMethod[16];
		
		switch (preferedemu) {
			case _1964x085: 		sprintf(szEmulator, "1964x085"); break;
			case _PJ64x16: 	sprintf(szEmulator, "PJ64x16"); break;
			//case _UltraHLE: 	sprintf(szEmulator, "UltraHLE"); break;
			//case _Mupen64Plus: 		sprintf(szEmulator, "M64P"); break;
			case _PJ64x14: 	sprintf(szEmulator, "PJ64x14"); break;
			case _1964x11: 	sprintf(szEmulator, "1964x11"); break;
			default:
				sprintf(szEmulator, "1964x085"); break;
		}
		
		switch (videoplugin) {
			case _VideoPluginRice510: 	sprintf(szVideoPlugin, "-510"); break;
			case _VideoPluginRice531: 	sprintf(szVideoPlugin, "-531"); break;
			case _VideoPluginRice560: 	sprintf(szVideoPlugin, "-560"); break;
			case _VideoPluginRice611: 	sprintf(szVideoPlugin, "-611"); break;
			case _VideoPluginRice612: 	sprintf(szVideoPlugin, "-612"); break;
			default:
				sprintf(szVideoPlugin, "-560"); break;
		}
		
		switch (iAudioPlugin) {
			//case _AudioPluginNone: 	sprintf(szAudioPlugin, ""); break;
			//case _AudioPluginBasic: 	sprintf(szAudioPlugin, ""); break;
			//case _AudioPluginJttl: 	sprintf(szAudioPlugin, ""); break;
			//case _AudioPluginAzimer: 	sprintf(szAudioPlugin, ""); break;
			case _AudioPluginMusyX: 	sprintf(szAudioPlugin, "M"); break;
			default:
				sprintf(szAudioPlugin, ""); break;
		}

		sprintf(szLaunchXBE, "D:\\%s%s%s.xbe", szEmulator, szVideoPlugin, szAudioPlugin);
	}

	OutputDebugString("Launching: ");
	OutputDebugString(szLaunchXBE);
	OutputDebugString("\n");
	//Sleep(100); // to see debug string
	
	// create a temporary pagefile just in case something goes wrong
	CreateDirectory("T:\\Temp", NULL);
	HANDLE hFile = CreateFile( "T:\\Temp\\codetemp.dat", GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, 0, NULL );
	CloseHandle(hFile);

	// launch the file
	XLaunchNewImage(szLaunchXBE, NULL);
}

void GetStateTimestamp(int index, int emulator, char *timestamp)
{
	char filename[255];
	if(preferedemu == _UltraHLE)
		__EMU_GetStateFilename(index, filename, 1); // Check for Screenie instead, not sure why savefiles show as empty.
	else if(emulator == 1)
		__EMU_Get1964StateFilename(index, filename, 1);
	else if(emulator == 2)
		__EMU_GetPJ64StateFilename(index, filename, 1);

	struct _stat buf;
	if(_stat(filename, &buf ) != 0) {
		sprintf(timestamp, "%i: Empty", index);
	} else {
		char time[100] = "";
		strftime(time, 100, "%Y-%m-%d %H:%M:%S", localtime(&buf.st_mtime));
		sprintf(timestamp, "%i: %s", index, time);
	}
}

void CreateSaveStatePreview(unsigned int index)
{
	if (pStateTexture) {
		pStateTexture->Release();
		pStateTexture = NULL;
	}

	//we need a temporary surface
	D3DSurface *surf = NULL;
	//D3DSURFACE_DESC surfdsc;

	//D3DTexture *tex = NULL;
	D3DSurface *texsurf = NULL;

	//clear from possible artifacts
	//FIXME: destroy panels here?
	//Clear Skin Images
	m_ControlConfigPanel.Destroy();
	m_MenuBgPanel.Destroy();
	m_MenuBg2Panel.Destroy();
	m_BgPanel.Destroy();

	//Render the taken ScreenShot
	m_RenderPanel.Render(0,0);

	//Clear the Artifacts
	g_pd3dDevice->Present(0, 0, 0, 0);
	

	//get the backbuffer surface and its description
	g_pd3dDevice->GetBackBuffer(-1, D3DBACKBUFFER_TYPE_MONO, &surf);
	//surf->GetDesc(&surfdsc);

	//create the preview texture
	//g_pd3dDevice->CreateTexture(/*iIGMStateScreenW*/128, /*iIGMStateScreenH*/128, 0, D3DX_DEFAULT, D3DFMT_LIN_A8R8G8B8/*surfdsc.Format*/, D3DPOOL_DEFAULT, &tex);

	//go down to surface level
	//tex->GetSurfaceLevel(0, &texsurf);
	g_pd3dDevice->CreateImageSurface(256, 256, D3DFMT_LIN_A8R8G8B8, &texsurf);
	
	//Render the taken ScreenShot
	m_RenderPanel.Render(0,0);

	//Clear the Artifacts
	g_pd3dDevice->Present(0, 0, 0, 0);

	//copy the backbuffer surface to the texture surface
	D3DXLoadSurfaceFromSurface(texsurf, NULL, NULL, surf, NULL, NULL, D3DX_DEFAULT, 0);
	

	g_pd3dDevice->Clear(0,NULL,D3DCLEAR_TARGET,0x00000000,0,0);
	
	char filename[255];
	if(preferedemu == _UltraHLE)
		__EMU_GetStateFilename((index+1), filename, 1);
	else if((preferedemu == _1964x11)||(preferedemu == _1964x085))
		__EMU_Get1964StateFilename((index+1), filename, 1);
	else if((preferedemu == _PJ64x16)||(preferedemu == _PJ64x14))
		__EMU_GetPJ64StateFilename((index+1), filename, 1);
	

	//write out the preview image
	XGWriteSurfaceToFile(texsurf, filename);

	//show BG again and erase.
	m_RenderPanel.Render(0,0);
	g_pd3dDevice->Present(0, 0, 0, 0);
	m_RenderPanel.Destroy();

	//release both surfaces
	texsurf->Release();
	texsurf = NULL;
	surf->Release();
	surf = NULL;

	//release the texture
	//tex->Release();
	//tex = NULL;
	
	//LoadSaveStatePreview(index); // show new one
}

bool LoadSaveStatePreview(unsigned int index, int emulator)
{
	if (pStateTexture) {
		pStateTexture->Release();
		pStateTexture = NULL;
	}
	
	//load our texture from file
	char filename[255];
	if(preferedemu == _UltraHLE)
		__EMU_GetStateFilename((index+1), filename, 1);
	else if(emulator == 1)
		__EMU_Get1964StateFilename((index+1), filename, 1);
	else if(emulator == 2)
		__EMU_GetPJ64StateFilename((index+1), filename, 1);
	
	int iScreenWidth = 0;
	int iScreenHeight = 0;
	
	if (iIGMStateScreenW > iIGMStateScreenH) {
		iScreenWidth = iIGMStateScreenW;
		iScreenHeight = iIGMStateScreenW;
	} else {
		iScreenWidth = iIGMStateScreenH;
		iScreenHeight = iIGMStateScreenH;
	}
	
	if(PathFileExists(filename)){
		D3DXCreateTextureFromFileEx(g_pd3dDevice, filename, iScreenWidth, iScreenHeight, 0, 0, D3DFMT_A4R4G4B4, D3DPOOL_MANAGED, D3DX_FILTER_NONE, D3DX_FILTER_NONE, 0x00000000, NULL, NULL, &pStateTexture);
	}else{
		//D3DXCreateTextureFromFileEx(g_pd3dDevice, "D:\\Media\\ui\\defsave.png", 256, 128, 0, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, D3DX_FILTER_NONE, D3DX_FILTER_NONE, 0x00000000,NULL, NULL, &saveGamePreview[index].texture);
		char pathmedia[256];
		
		if (bEnableHDTV)
			sprintf(pathmedia, "%sui\\defsave_hd.png", szPathMedia);
		else
			sprintf(pathmedia, "%sui\\defsave.png", szPathMedia);
		
		if(PathFileExists(pathmedia)){
			D3DXCreateTextureFromFileEx(g_pd3dDevice, pathmedia, iScreenWidth, iScreenHeight, 0, 0, D3DFMT_A4R4G4B4, D3DPOOL_MANAGED, D3DX_FILTER_NONE, D3DX_FILTER_NONE, 0x00000000, NULL, NULL, &pStateTexture);
		}else{
			return false;
		}
	}
	
	return true;
}
void LoadPJ64StateMenu(void)
{

	DWORD dwMenuCommand = 0;

	XLMenu_CurRoutine = NULL;
	XLMenu_CurMenu = NULL;
	XLMenu_SetFont(&m_Font);

	m_pSettingsMenu = XLMenu_Init((float)iIGMMenuTxtPosX,(float)iIGMMenuTxtPosY,MAX_SAVE_STATES, GetMenuFontAlign(iIGMMenuTxtAlign)|MENU_WRAP, NULL);

	m_pSettingsMenu->itemcolor = dwMenuItemColor;
	m_pSettingsMenu->parent = m_pMainMenu;

	XLMenu_SetTitle(m_pSettingsMenu,L"Load PJ64 State",dwMenuTitleColor);

	char timestamp[256];
	WCHAR currentname[256];
	
	GetStateTimestamp(1, 2, timestamp);
	swprintf(currentname,L"%S",timestamp);
	XLMenu_AddItem(m_pSettingsMenu,MITEM_ROUTINE,currentname,LoadPJ64State1);
	
	GetStateTimestamp(2, 2, timestamp);
	swprintf(currentname,L"%S",timestamp);
	XLMenu_AddItem(m_pSettingsMenu,MITEM_ROUTINE,currentname,LoadPJ64State2);
	
	GetStateTimestamp(3, 2, timestamp);
	swprintf(currentname,L"%S",timestamp);
	XLMenu_AddItem(m_pSettingsMenu,MITEM_ROUTINE,currentname,LoadPJ64State3);
	
	GetStateTimestamp(4, 2, timestamp);
	swprintf(currentname,L"%S",timestamp);
	XLMenu_AddItem(m_pSettingsMenu,MITEM_ROUTINE,currentname,LoadPJ64State4);

	GetStateTimestamp(5, 2, timestamp);
	swprintf(currentname,L"%S",timestamp);
	XLMenu_AddItem(m_pSettingsMenu,MITEM_ROUTINE,currentname,LoadPJ64State5);

	XLMenu_Activate(m_pSettingsMenu);

	bool bLoadPreview = false;
	
	int selected = m_pSettingsMenu->curitem;
	bLoadPreview = LoadSaveStatePreview(selected, 2); // just the 1st one

	while( XLMenu_CurMenu == m_pSettingsMenu)
	{
		DrawLogo();
		dwMenuCommand = getAllGamepadsCommand(&gamepad);
		XLMenu_Routine(dwMenuCommand);
		
		// update preview on change
		if (selected != m_pSettingsMenu->curitem) {
			selected = m_pSettingsMenu->curitem;
			
			bLoadPreview = LoadSaveStatePreview(selected, 2);
		}
		
		//@weinersch do your skinning magic here
		if (bLoadPreview && !bNoPreview) {
		g_pd3dDevice->SetTexture(0, pStateTexture);

		g_pd3dDevice->SetVertexShader(D3DFVF_XYZRHW | D3DFVF_TEX1);
		g_pd3dDevice->Begin(D3DPT_QUADLIST);
		g_pd3dDevice->SetVertexData2f( D3DVSDE_TEXCOORD0, 0.0f, 0.0f );
		g_pd3dDevice->SetVertexData4f( D3DVSDE_VERTEX, (float)iIGMStateScreenX,  (float)iIGMStateScreenY, 0.0f, 1.0f );

		g_pd3dDevice->SetVertexData2f( D3DVSDE_TEXCOORD0, 1.0f, 0.0f );
		g_pd3dDevice->SetVertexData4f( D3DVSDE_VERTEX, (float)(iIGMStateScreenX + iIGMStateScreenW/*iScreenWidth*/),  (float)iIGMStateScreenY,    0.0f, 1.0f );

		g_pd3dDevice->SetVertexData2f( D3DVSDE_TEXCOORD0, 1.0f, 1.0f );
		g_pd3dDevice->SetVertexData4f( D3DVSDE_VERTEX, (float)(iIGMStateScreenX + iIGMStateScreenW/*iScreenWidth*/), (float)(iIGMStateScreenY + iIGMStateScreenH/*iScreenHeight*/),    0.0f, 1.0f );

		g_pd3dDevice->SetVertexData2f( D3DVSDE_TEXCOORD0, 0.0f, 1.0f );
		g_pd3dDevice->SetVertexData4f( D3DVSDE_VERTEX, (float)iIGMStateScreenX, (float)(iIGMStateScreenY + iIGMStateScreenH/*iScreenHeight*/), 0.0f, 1.0f );
		g_pd3dDevice->End();

		g_pd3dDevice->SetTexture(0, NULL);
		}

		g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
	}

	if (pStateTexture) {
		pStateTexture->Release();
		pStateTexture = NULL;
	}

	if (m_pSettingsMenu){
		XLMenu_Delete(m_pSettingsMenu);
	}
}

void LoadPJ64State1()
{
	// Prevent preview texture from loading and clean up
	bNoPreview = true;
	if (pStateTexture) {
		pStateTexture->Release();
		pStateTexture = NULL;
	}
	ClearIGM();

	// Seed a Load State
	bSatesUpdated=true;
	bloadPJ64state[1]=true;

	// Disable Menu to exit
	XLMenu_CurRoutine = NULL;
	XLMenu_CurMenu = NULL;
}

void LoadPJ64State2()
{
	// Prevent preview texture from loading and clean up
	bNoPreview = true;
	if (pStateTexture) {
		pStateTexture->Release();
		pStateTexture = NULL;
	}
	ClearIGM();

	// Seed a Load State
	bSatesUpdated=true;
	bloadPJ64state[2]=true;
	
	// Disable Menu to exit
	XLMenu_CurRoutine = NULL;
	XLMenu_CurMenu = NULL;
}

void LoadPJ64State3()
{
	// Prevent preview texture from loading and clean up
	bNoPreview = true;
	if (pStateTexture) {
		pStateTexture->Release();
		pStateTexture = NULL;
	}
	ClearIGM();

	// Seed a Load State
	bSatesUpdated=true;
	bloadPJ64state[3]=true;
	
	// Disable Menu to exit
	XLMenu_CurRoutine = NULL;
	XLMenu_CurMenu = NULL;
}

void LoadPJ64State4()
{
	// Prevent preview texture from loading and clean up
	bNoPreview = true;
	if (pStateTexture) {
		pStateTexture->Release();
		pStateTexture = NULL;
	}
	ClearIGM();

	// Seed a Load State
	bSatesUpdated=true;
	bloadPJ64state[4]=true;
	
	// Disable Menu to exit
	XLMenu_CurRoutine = NULL;
	XLMenu_CurMenu = NULL;
}

void LoadPJ64State5()
{
	// Prevent preview texture from loading and clean up
	bNoPreview = true;
	if (pStateTexture) {
		pStateTexture->Release();
		pStateTexture = NULL;
	}
	ClearIGM();

	// Seed a Load State
	bSatesUpdated=true;
	bloadPJ64state[5]=true;
	
	// Disable Menu to exit
	XLMenu_CurRoutine = NULL;
	XLMenu_CurMenu = NULL;
}


void Load1964StateMenu(void)
{

	DWORD dwMenuCommand = 0;

	XLMenu_CurRoutine = NULL;
	XLMenu_CurMenu = NULL;
	XLMenu_SetFont(&m_Font);

	m_pSettingsMenu = XLMenu_Init((float)iIGMMenuTxtPosX,(float)iIGMMenuTxtPosY,MAX_SAVE_STATES, GetMenuFontAlign(iIGMMenuTxtAlign)|MENU_WRAP, NULL);

	m_pSettingsMenu->itemcolor = dwMenuItemColor;
	m_pSettingsMenu->parent = m_pMainMenu;

	XLMenu_SetTitle(m_pSettingsMenu,L"Load 1964 State",dwMenuTitleColor);

	char timestamp[256];
	WCHAR currentname[256];
	
	GetStateTimestamp(1, 1, timestamp);
	swprintf(currentname,L"%S",timestamp);
	XLMenu_AddItem(m_pSettingsMenu,MITEM_ROUTINE,currentname,Load1964State1);
	
	GetStateTimestamp(2, 1, timestamp);
	swprintf(currentname,L"%S",timestamp);
	XLMenu_AddItem(m_pSettingsMenu,MITEM_ROUTINE,currentname,Load1964State2);
	
	GetStateTimestamp(3, 1, timestamp);
	swprintf(currentname,L"%S",timestamp);
	XLMenu_AddItem(m_pSettingsMenu,MITEM_ROUTINE,currentname,Load1964State3);
	
	GetStateTimestamp(4, 1, timestamp);
	swprintf(currentname,L"%S",timestamp);
	XLMenu_AddItem(m_pSettingsMenu,MITEM_ROUTINE,currentname,Load1964State4);

	GetStateTimestamp(5, 1, timestamp);
	swprintf(currentname,L"%S",timestamp);
	XLMenu_AddItem(m_pSettingsMenu,MITEM_ROUTINE,currentname,Load1964State5);

	XLMenu_Activate(m_pSettingsMenu);

	bool bLoadPreview = false;
	
	int selected = m_pSettingsMenu->curitem;
	bLoadPreview = LoadSaveStatePreview(selected, 1); // just the 1st one

	while( XLMenu_CurMenu == m_pSettingsMenu)
	{
		DrawLogo();
		dwMenuCommand = getAllGamepadsCommand(&gamepad);
		XLMenu_Routine(dwMenuCommand);
		
		// update preview on change
		if (selected != m_pSettingsMenu->curitem) {
			selected = m_pSettingsMenu->curitem;
			
			bLoadPreview = LoadSaveStatePreview(selected, 1);
		}
		
		//@weinersch do your skinning magic here
		if (bLoadPreview && !bNoPreview) {
		g_pd3dDevice->SetTexture(0, pStateTexture);

		g_pd3dDevice->SetVertexShader(D3DFVF_XYZRHW | D3DFVF_TEX1);
		g_pd3dDevice->Begin(D3DPT_QUADLIST);
		g_pd3dDevice->SetVertexData2f( D3DVSDE_TEXCOORD0, 0.0f, 0.0f );
		g_pd3dDevice->SetVertexData4f( D3DVSDE_VERTEX, (float)iIGMStateScreenX,  (float)iIGMStateScreenY, 0.0f, 1.0f );

		g_pd3dDevice->SetVertexData2f( D3DVSDE_TEXCOORD0, 1.0f, 0.0f );
		g_pd3dDevice->SetVertexData4f( D3DVSDE_VERTEX, (float)(iIGMStateScreenX + iIGMStateScreenW/*iScreenWidth*/),  (float)iIGMStateScreenY,    0.0f, 1.0f );

		g_pd3dDevice->SetVertexData2f( D3DVSDE_TEXCOORD0, 1.0f, 1.0f );
		g_pd3dDevice->SetVertexData4f( D3DVSDE_VERTEX, (float)(iIGMStateScreenX + iIGMStateScreenW/*iScreenWidth*/), (float)(iIGMStateScreenY + iIGMStateScreenH/*iScreenHeight*/),    0.0f, 1.0f );

		g_pd3dDevice->SetVertexData2f( D3DVSDE_TEXCOORD0, 0.0f, 1.0f );
		g_pd3dDevice->SetVertexData4f( D3DVSDE_VERTEX, (float)iIGMStateScreenX, (float)(iIGMStateScreenY + iIGMStateScreenH/*iScreenHeight*/), 0.0f, 1.0f );
		g_pd3dDevice->End();

		g_pd3dDevice->SetTexture(0, NULL);
		}

		g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
	}

	if (pStateTexture) {
		pStateTexture->Release();
		pStateTexture = NULL;
	}

	if (m_pSettingsMenu){
		XLMenu_Delete(m_pSettingsMenu);
	}
}

void Load1964State1()
{
	// Prevent preview texture from loading and clean up
	bNoPreview = true;
	if (pStateTexture) {
		pStateTexture->Release();
		pStateTexture = NULL;
	}
	ClearIGM();

	// Seed a Load State
	bSatesUpdated=true;
	bload1964state[1]=true;

	// Disable Menu to exit
	XLMenu_CurRoutine = NULL;
	XLMenu_CurMenu = NULL;
}

void Load1964State2()
{
	// Prevent preview texture from loading and clean up
	bNoPreview = true;
	if (pStateTexture) {
		pStateTexture->Release();
		pStateTexture = NULL;
	}
	ClearIGM();

	// Seed a Load State
	bSatesUpdated=true;
	bload1964state[2]=true;
	
	// Disable Menu to exit
	XLMenu_CurRoutine = NULL;
	XLMenu_CurMenu = NULL;
}

void Load1964State3()
{
	// Prevent preview texture from loading and clean up
	bNoPreview = true;
	if (pStateTexture) {
		pStateTexture->Release();
		pStateTexture = NULL;
	}
	ClearIGM();

	// Seed a Load State
	bSatesUpdated=true;
	bload1964state[3]=true;
	
	// Disable Menu to exit
	XLMenu_CurRoutine = NULL;
	XLMenu_CurMenu = NULL;
}

void Load1964State4()
{
	// Prevent preview texture from loading and clean up
	bNoPreview = true;
	if (pStateTexture) {
		pStateTexture->Release();
		pStateTexture = NULL;
	}
	ClearIGM();

	// Seed a Load State
	bSatesUpdated=true;
	bload1964state[4]=true;
	
	// Disable Menu to exit
	XLMenu_CurRoutine = NULL;
	XLMenu_CurMenu = NULL;
}

void Load1964State5()
{
	// Prevent preview texture from loading and clean up
	bNoPreview = true;
	if (pStateTexture) {
		pStateTexture->Release();
		pStateTexture = NULL;
	}
	ClearIGM();

	// Seed a Load State
	bSatesUpdated=true;
	bload1964state[5]=true;
	
	// Disable Menu to exit
	XLMenu_CurRoutine = NULL;
	XLMenu_CurMenu = NULL;
}


// Left for UltraHLE
void LoadStateMenu(void)
{

	DWORD dwMenuCommand = 0;

	XLMenu_CurRoutine = NULL;
	XLMenu_CurMenu = NULL;
	XLMenu_SetFont(&m_Font);

	m_pSettingsMenu = XLMenu_Init((float)iIGMMenuTxtPosX,(float)iIGMMenuTxtPosY,MAX_SAVE_STATES, GetMenuFontAlign(iIGMMenuTxtAlign)|MENU_WRAP, NULL);

	m_pSettingsMenu->itemcolor = dwMenuItemColor;
	m_pSettingsMenu->parent = m_pMainMenu;

	XLMenu_SetTitle(m_pSettingsMenu,L"Load 1964 State",dwMenuTitleColor);

	char timestamp[256];
	WCHAR currentname[256];
	
	GetStateTimestamp(1, 0, timestamp);
	swprintf(currentname,L"%S",timestamp);
	XLMenu_AddItem(m_pSettingsMenu,MITEM_ROUTINE,currentname,LoadState1);
	
	GetStateTimestamp(2, 0, timestamp);
	swprintf(currentname,L"%S",timestamp);
	XLMenu_AddItem(m_pSettingsMenu,MITEM_ROUTINE,currentname,LoadState2);
	
	GetStateTimestamp(3, 0, timestamp);
	swprintf(currentname,L"%S",timestamp);
	XLMenu_AddItem(m_pSettingsMenu,MITEM_ROUTINE,currentname,LoadState3);
	
	GetStateTimestamp(4, 0, timestamp);
	swprintf(currentname,L"%S",timestamp);
	XLMenu_AddItem(m_pSettingsMenu,MITEM_ROUTINE,currentname,LoadState4);

	GetStateTimestamp(5, 0, timestamp);
	swprintf(currentname,L"%S",timestamp);
	XLMenu_AddItem(m_pSettingsMenu,MITEM_ROUTINE,currentname,LoadState5);

	XLMenu_Activate(m_pSettingsMenu);

	bool bLoadPreview = false;
	
	int selected = m_pSettingsMenu->curitem;
	bLoadPreview = LoadSaveStatePreview(selected, 0); // just the 1st one

	while( XLMenu_CurMenu == m_pSettingsMenu)
	{
		DrawLogo();
		dwMenuCommand = getAllGamepadsCommand(&gamepad);
		XLMenu_Routine(dwMenuCommand);
		
		// update preview on change
		if (selected != m_pSettingsMenu->curitem) {
			selected = m_pSettingsMenu->curitem;
			
			bLoadPreview = LoadSaveStatePreview(selected, 0);
		}
		
		//@weinersch do your skinning magic here
		if (bLoadPreview && !bNoPreview) {
		g_pd3dDevice->SetTexture(0, pStateTexture);

		g_pd3dDevice->SetVertexShader(D3DFVF_XYZRHW | D3DFVF_TEX1);
		g_pd3dDevice->Begin(D3DPT_QUADLIST);
		g_pd3dDevice->SetVertexData2f( D3DVSDE_TEXCOORD0, 0.0f, 0.0f );
		g_pd3dDevice->SetVertexData4f( D3DVSDE_VERTEX, (float)iIGMStateScreenX,  (float)iIGMStateScreenY, 0.0f, 1.0f );

		g_pd3dDevice->SetVertexData2f( D3DVSDE_TEXCOORD0, 1.0f, 0.0f );
		g_pd3dDevice->SetVertexData4f( D3DVSDE_VERTEX, (float)(iIGMStateScreenX + iIGMStateScreenW/*iScreenWidth*/),  (float)iIGMStateScreenY,    0.0f, 1.0f );

		g_pd3dDevice->SetVertexData2f( D3DVSDE_TEXCOORD0, 1.0f, 1.0f );
		g_pd3dDevice->SetVertexData4f( D3DVSDE_VERTEX, (float)(iIGMStateScreenX + iIGMStateScreenW/*iScreenWidth*/), (float)(iIGMStateScreenY + iIGMStateScreenH/*iScreenHeight*/),    0.0f, 1.0f );

		g_pd3dDevice->SetVertexData2f( D3DVSDE_TEXCOORD0, 0.0f, 1.0f );
		g_pd3dDevice->SetVertexData4f( D3DVSDE_VERTEX, (float)iIGMStateScreenX, (float)(iIGMStateScreenY + iIGMStateScreenH/*iScreenHeight*/), 0.0f, 1.0f );
		g_pd3dDevice->End();

		g_pd3dDevice->SetTexture(0, NULL);
		}

		g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
	}

	if (pStateTexture) {
		pStateTexture->Release();
		pStateTexture = NULL;
	}

	if (m_pSettingsMenu){
		XLMenu_Delete(m_pSettingsMenu);
	}
}

void LoadState1()
{
	// Prevent preview texture from loading and clean up
	bNoPreview = true;
	if (pStateTexture) {
		pStateTexture->Release();
		pStateTexture = NULL;
	}
	ClearIGM();

	// Seed a Load State
	bSatesUpdated=true;
	bloadstate[1]=true;

	// Disable Menu to exit
	XLMenu_CurRoutine = NULL;
	XLMenu_CurMenu = NULL;
}

void LoadState2()
{
	// Prevent preview texture from loading and clean up
	bNoPreview = true;
	if (pStateTexture) {
		pStateTexture->Release();
		pStateTexture = NULL;
	}
	ClearIGM();

	// Seed a Load State
	bSatesUpdated=true;
	bloadstate[2]=true;
	
	// Disable Menu to exit
	XLMenu_CurRoutine = NULL;
	XLMenu_CurMenu = NULL;
}

void LoadState3()
{
	// Prevent preview texture from loading and clean up
	bNoPreview = true;
	if (pStateTexture) {
		pStateTexture->Release();
		pStateTexture = NULL;
	}
	ClearIGM();

	// Seed a Load State
	bSatesUpdated=true;
	bloadstate[3]=true;
	
	// Disable Menu to exit
	XLMenu_CurRoutine = NULL;
	XLMenu_CurMenu = NULL;
}

void LoadState4()
{
	// Prevent preview texture from loading and clean up
	bNoPreview = true;
	if (pStateTexture) {
		pStateTexture->Release();
		pStateTexture = NULL;
	}
	ClearIGM();

	// Seed a Load State
	bSatesUpdated=true;
	bloadstate[4]=true;
	
	// Disable Menu to exit
	XLMenu_CurRoutine = NULL;
	XLMenu_CurMenu = NULL;
}

void LoadState5()
{
	// Prevent preview texture from loading and clean up
	bNoPreview = true;
	if (pStateTexture) {
		pStateTexture->Release();
		pStateTexture = NULL;
	}
	ClearIGM();

	// Seed a Load State
	bSatesUpdated=true;
	bloadstate[5]=true;
	
	// Disable Menu to exit
	XLMenu_CurRoutine = NULL;
	XLMenu_CurMenu = NULL;
}

void SaveStateMenu(void)
{
	DWORD dwMenuCommand = 0;

	XLMenu_CurRoutine = NULL;
	XLMenu_CurMenu = NULL;
	XLMenu_SetFont(&m_Font);


	m_pSettingsMenu = XLMenu_Init((float)iIGMMenuTxtPosX,(float)iIGMMenuTxtPosY,MAX_SAVE_STATES, GetMenuFontAlign(iIGMMenuTxtAlign)|MENU_WRAP, NULL);

	m_pSettingsMenu->itemcolor = dwMenuItemColor;
	m_pSettingsMenu->parent = m_pMainMenu;

	XLMenu_SetTitle(m_pSettingsMenu,L"Save State",dwMenuTitleColor);

  	/*XLMenu_AddItem(m_pSettingsMenu,MITEM_ROUTINE,L"Save State 1",SaveState1);
  	XLMenu_AddItem(m_pSettingsMenu,MITEM_ROUTINE,L"Save State 2",SaveState2);
  	XLMenu_AddItem(m_pSettingsMenu,MITEM_ROUTINE,L"Save State 3",SaveState3);
  	XLMenu_AddItem(m_pSettingsMenu,MITEM_ROUTINE,L"Save State 4",SaveState4);
  	XLMenu_AddItem(m_pSettingsMenu,MITEM_ROUTINE,L"Save State 5",SaveState5);*/

	char timestamp[256];
	WCHAR currentname[256];

	int emu;

	if(preferedemu == _UltraHLE)
		emu = 0;
	else if((preferedemu == _1964x11)||(preferedemu == _1964x085))
		emu = 1;
	else if((preferedemu == _PJ64x16)||(preferedemu == _PJ64x14))
		emu = 2;
	
	GetStateTimestamp(1, emu, timestamp);
	swprintf(currentname,L"%S",timestamp);
	XLMenu_AddItem(m_pSettingsMenu,MITEM_ROUTINE,currentname,SaveState1);
	//LoadSaveStatePreview(0);
	
	GetStateTimestamp(2, emu, timestamp);
	swprintf(currentname,L"%S",timestamp);
	XLMenu_AddItem(m_pSettingsMenu,MITEM_ROUTINE,currentname,SaveState2);
	//LoadSaveStatePreview(1);

	GetStateTimestamp(3, emu, timestamp);
	swprintf(currentname,L"%S",timestamp);
	XLMenu_AddItem(m_pSettingsMenu,MITEM_ROUTINE,currentname,SaveState3);
	//LoadSaveStatePreview(2);

	GetStateTimestamp(4, emu, timestamp);
	swprintf(currentname,L"%S",timestamp);
	XLMenu_AddItem(m_pSettingsMenu,MITEM_ROUTINE,currentname,SaveState4);
	//LoadSaveStatePreview(3);

	GetStateTimestamp(5, emu, timestamp);
	swprintf(currentname,L"%S",timestamp);
	XLMenu_AddItem(m_pSettingsMenu,MITEM_ROUTINE,currentname,SaveState5);
	//LoadSaveStatePreview(4);

	XLMenu_Activate(m_pSettingsMenu);

	bool bLoadPreview = false;
	
	//int selected = 0;
	int selected = m_pSettingsMenu->curitem;
	bLoadPreview = LoadSaveStatePreview(selected, emu); // just the 1st one
/*
	int iScreenWidth = 0;
	int iScreenHeight = 0;
	
	if (iIGMStateScreenW > iIGMStateScreenH) {
		iScreenWidth = iIGMStateScreenW;
		iScreenHeight = iIGMStateScreenW;
	} else {
		iScreenWidth = iIGMStateScreenH;
		iScreenHeight = iIGMStateScreenH;
	}
	*/
	while( XLMenu_CurMenu == m_pSettingsMenu)
	{
		DrawLogo();
		dwMenuCommand = getAllGamepadsCommand(&gamepad);
		XLMenu_Routine(dwMenuCommand);

		// update preview on change
		if (selected != m_pSettingsMenu->curitem) {
			selected = m_pSettingsMenu->curitem;
			
			bLoadPreview = LoadSaveStatePreview(selected, emu);
		}

		//fd: FIXME!
		//@weinersch do your skinning magic here
		if (bLoadPreview && !bNoPreview) {
		g_pd3dDevice->SetTexture(0, pStateTexture);

		g_pd3dDevice->SetVertexShader(D3DFVF_XYZRHW | D3DFVF_TEX1);
		g_pd3dDevice->Begin(D3DPT_QUADLIST);
		g_pd3dDevice->SetVertexData2f( D3DVSDE_TEXCOORD0, 0.0f, 0.0f );
		g_pd3dDevice->SetVertexData4f( D3DVSDE_VERTEX, (float)iIGMStateScreenX,  (float)iIGMStateScreenY, 0.0f, 1.0f );

		g_pd3dDevice->SetVertexData2f( D3DVSDE_TEXCOORD0, 1.0f, 0.0f );
		g_pd3dDevice->SetVertexData4f( D3DVSDE_VERTEX, (float)(iIGMStateScreenX + iIGMStateScreenW/*iScreenWidth*/),  (float)iIGMStateScreenY,    0.0f, 1.0f );

		g_pd3dDevice->SetVertexData2f( D3DVSDE_TEXCOORD0, 1.0f, 1.0f );
		g_pd3dDevice->SetVertexData4f( D3DVSDE_VERTEX, (float)(iIGMStateScreenX + iIGMStateScreenW/*iScreenWidth*/), (float)(iIGMStateScreenY + iIGMStateScreenH/*iScreenHeight*/),    0.0f, 1.0f );

		g_pd3dDevice->SetVertexData2f( D3DVSDE_TEXCOORD0, 0.0f, 1.0f );
		g_pd3dDevice->SetVertexData4f( D3DVSDE_VERTEX, (float)iIGMStateScreenX, (float)(iIGMStateScreenY + iIGMStateScreenH/*iScreenHeight*/), 0.0f, 1.0f );
		g_pd3dDevice->End();

		g_pd3dDevice->SetTexture(0, NULL);
		}
		
		g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
	}

	if (pStateTexture) {
		pStateTexture->Release();
		pStateTexture = NULL;
	}

	if (m_pSettingsMenu){
		XLMenu_Delete(m_pSettingsMenu);
	}
}


void SaveState1()
{
	// Create a small screenshot.
	CreateSaveStatePreview(0);
	
	// Prevent preview texture from loading on exit and clean up
	bNoPreview = true;
	if (pStateTexture) {
		pStateTexture->Release();
		pStateTexture = NULL;
	}
	ClearIGM();
	
	// Seed a Save State
	bSatesUpdated=true;
	bsavestate[1]=true;

	// Disable Menu to exit
	XLMenu_CurRoutine = NULL;
	XLMenu_CurMenu = NULL;
}

void SaveState2()
{
	// Create a small screenshot.
	CreateSaveStatePreview(1);
	
	// Prevent preview texture from loading on exit and clean up
	bNoPreview = true;
	if (pStateTexture) {
		pStateTexture->Release();
		pStateTexture = NULL;
	}
	ClearIGM();
	
	// Seed a Save State
	bSatesUpdated=true;
	bsavestate[2]=true;
	
	// Disable Menu to exit
	XLMenu_CurRoutine = NULL;
	XLMenu_CurMenu = NULL;
}

void SaveState3()
{
	// Create a small screenshot.
	CreateSaveStatePreview(2);
	
	// Prevent preview texture from loading on exit and clean up
	bNoPreview = true;
	if (pStateTexture) {
		pStateTexture->Release();
		pStateTexture = NULL;
	}
	ClearIGM();
	
	// Seed a Save State
	bSatesUpdated=true;
	bsavestate[3]=true;
	
	// Disable Menu to exit
	XLMenu_CurRoutine = NULL;
	XLMenu_CurMenu = NULL;
}

void SaveState4()
{
	// Create a small screenshot.
	CreateSaveStatePreview(3);
	
	// Prevent preview texture from loading on exit and clean up
	bNoPreview = true;
	if (pStateTexture) {
		pStateTexture->Release();
		pStateTexture = NULL;
	}
	ClearIGM();
	
	// Seed a Save State
	bSatesUpdated=true;
	bsavestate[4]=true;

	// Disable Menu to exit
	XLMenu_CurRoutine = NULL;
	XLMenu_CurMenu = NULL;
}

void SaveState5()
{
	// Create a small screenshot.
	CreateSaveStatePreview(4);
	
	// Prevent preview texture from loading on exit and clean up
	bNoPreview = true;
	if (pStateTexture) {
		pStateTexture->Release();
		pStateTexture = NULL;
	}
	ClearIGM();
	
	// Seed a Save State
	bSatesUpdated=true;
	bsavestate[5]=true;

	// Disable Menu to exit
	XLMenu_CurRoutine = NULL;
	XLMenu_CurMenu = NULL;
}

void VideoSettingsMenu(void)
{
	ConfigAppLoad2();

	DWORD dwMenuCommand = 0;

	XLMenu_CurRoutine = NULL;
	XLMenu_CurMenu = NULL;
	XLMenu_SetFont(&m_Font);

    WCHAR currentname[120];
	if(preferedemu != _1964x11)
		m_pSettingsMenu = XLMenu_Init((float)iIGMMenuTxtPosX,(float)iIGMMenuTxtPosY,5, GetMenuFontAlign(iIGMMenuTxtAlign)|MENU_WRAP, NULL);
	else
		m_pSettingsMenu = XLMenu_Init((float)iIGMMenuTxtPosX,(float)iIGMMenuTxtPosY,11, GetMenuFontAlign(iIGMMenuTxtAlign)|MENU_WRAP, NULL);

	m_pSettingsMenu->itemcolor = dwMenuItemColor;
	m_pSettingsMenu->parent = m_pMainMenu;

	XLMenu_SetTitle(m_pSettingsMenu,L"Video Settings",dwMenuTitleColor);

    if (!FlickerFilter)
	swprintf(currentname,L"Flicker Filter : Disabled");
	else 
	swprintf(currentname,L"Flicker Filter : %d",FlickerFilter);
	XLMenu_AddItem2(m_pSettingsMenu,MITEM_ROUTINE,currentname,incflicker,decflicker);

	if (!SoftDisplayFilter)
	swprintf(currentname,L"Soft Display Filter : No");
	else 
	swprintf(currentname,L"Soft Display Filter : Yes");
	XLMenu_AddItem(m_pSettingsMenu,MITEM_ROUTINE,currentname,ToggleSoftDisplayFilter);

	switch (TextureMode){
		case 1 : 	swprintf(currentname,L"Texture Filter : Point");
			break;
		case 2 : 	swprintf(currentname,L"Texture Filter : Linear");
			break;
		case 3 : 	swprintf(currentname,L"Texture Filter : Anisotropic");
			break;
		case 4 : 	swprintf(currentname,L"Texture Filter : Quincunx");
			break;
		case 5 : 	swprintf(currentname,L"Texture Filter : Gaussian");
			break;	}
	XLMenu_AddItem2(m_pSettingsMenu,MITEM_ROUTINE,currentname,incTextureFilter,decTextureFilter);

	//FrameSkip
	if((FrameSkip == 2) && (preferedemu != _1964x11))
		FrameSkip = 0;
	switch (FrameSkip){
		case 0 : 	swprintf(currentname,L"Frame Skip : Off");
			break;
		case 1 : 	swprintf(currentname,L"Frame Skip : On");
			break;
		case 2 : 	swprintf(currentname,L"Frame Skip : Auto");
			break;	}
	XLMenu_AddItem2(m_pSettingsMenu,MITEM_ROUTINE,currentname,incFrameSkip,decFrameSkip);
	
	if(preferedemu == _1964x11){
		
		//AutoCF 
		switch (AutoCF_1964){
			case 0 : 	swprintf(currentname,L"Counter Factor : Use Ini");
				break;
			case 1 : 	swprintf(currentname,L"Counter Factor : Auto");
				break;
			case 2 : 	swprintf(currentname,L"Counter Factor : Auto with CF1");
				break;	}
		XLMenu_AddItem2(m_pSettingsMenu,MITEM_ROUTINE,currentname,incAutoCF,decAutoCF);
		
		//AutoCF separator
		XLMenu_AddItem(m_pSettingsMenu,MITEM_DISABLED,L"Auto Counter Factor Settings",NULL);

		//AutoCF Tweaks
		swprintf(currentname,L"CF5-CF3 Step Up : %2.0d%%", CF5toCF3StepUp);
		XLMenu_AddItem2(m_pSettingsMenu,MITEM_ROUTINE,currentname,incCF5toCF3StepUp,decCF5toCF3StepUp);
		
		swprintf(currentname,L"CF3-CF1 Step Up : %2.0d%%", CF3toCF1StepUp);
		XLMenu_AddItem2(m_pSettingsMenu,MITEM_ROUTINE,currentname,incCF3toCF1StepUp,decCF3toCF1StepUp);

		swprintf(currentname,L"CF1-CF3 Step Down : %2.0d%%", CF1toCF3StepDown);
		XLMenu_AddItem2(m_pSettingsMenu,MITEM_ROUTINE,currentname,incCF1toCF3StepDown,decCF1toCF3StepDown);

		swprintf(currentname,L"CF3-CF5 Step Down : %2.0d%%", CF3toCF5StepDown);
		XLMenu_AddItem2(m_pSettingsMenu,MITEM_ROUTINE,currentname,incCF3toCF5StepDown,decCF3toCF5StepDown);

	}
	

	//Fog Mode
	if (!bUseLinFog)
	swprintf(currentname,L"Fog Mode : Range");
	else 
	swprintf(currentname,L"Fog Mode : Linear");
	//Disable Menu item. Requires Restart, should be modified by Launcher
	//XLMenu_AddItem(m_pSettingsMenu,MITEM_ROUTINE,currentname,ToggleFogMode);

	//Texture Depth
	if (XboxPitch == 2){
	swprintf(currentname,L"Texture Depth : 16bit");
	}
	else{
	swprintf(currentname,L"Texture Depth : 32bit");
	XboxPitch = 4;
	}
	//Disable menu item, no improvement from 16bit?
	//Uncomment and increase menu items to use
	//XLMenu_AddItem(m_pSettingsMenu,MITEM_ROUTINE,currentname,ToggleXboxPitch);


	XLMenu_Activate(m_pSettingsMenu);


	while( XLMenu_CurMenu == m_pSettingsMenu)
	{
		DrawLogo();
		dwMenuCommand = getAllGamepadsCommand(&gamepad);
		XLMenu_Routine(dwMenuCommand);
		g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
	}

	if (m_pSettingsMenu)
		XLMenu_Delete(m_pSettingsMenu);

}


void ToggleFlickerFilter(bool inc)
{
    WCHAR currentname[120];
	currentItem = m_pSettingsMenu->curitem;
	if (inc)
	{
	FlickerFilter++;
    if (FlickerFilter > 5) FlickerFilter = 0;
	}
	else
	{
	FlickerFilter--;
    if (FlickerFilter < 0) FlickerFilter = 5;
	}
	D3DDevice :: SetFlickerFilter(FlickerFilter);

	
	XLMenu_CurRoutine = NULL;
	
    if (!FlickerFilter)
	swprintf(currentname,L"Flicker Filter : Disabled");
	else 
	swprintf(currentname,L"Flicker Filter : %d",FlickerFilter);
	XLMenu_SetItemText(&m_pSettingsMenu->items[currentItem], currentname);

	ConfigAppSave2();
}

void ToggleSoftDisplayFilter()
{
    WCHAR currentname[120];
	currentItem = m_pSettingsMenu->curitem;
	SoftDisplayFilter =! SoftDisplayFilter;
	D3DDevice::SetSoftDisplayFilter(SoftDisplayFilter);

	XLMenu_CurRoutine = NULL;
	
  	if (!SoftDisplayFilter)
	swprintf(currentname,L"Soft Display Filter : No");
	else 
	swprintf(currentname,L"Soft Display Filter : Yes");
	XLMenu_SetItemText(&m_pSettingsMenu->items[currentItem], currentname);

	ConfigAppSave2();
}

void ToggleAutoCF(bool inc)
{
    WCHAR currentname[120];
	currentItem = m_pSettingsMenu->curitem;

	if (inc)
	{
	AutoCF_1964++;
    if (AutoCF_1964 > 2) AutoCF_1964 = 0;
	}
	else
	{
	AutoCF_1964--;
    if (AutoCF_1964 < 0) AutoCF_1964 = 2;
	}
	
	XLMenu_CurRoutine = NULL;
  	
	switch(AutoCF_1964){
		case 0:
			swprintf(currentname,L"Counter Factor : Use Ini");
			break;
		case 1:
			swprintf(currentname,L"Counter Factor : Auto");
			break;
		case 2:
			swprintf(currentname,L"Counter Factor : Auto with CF1");
			break;
	}


	XLMenu_SetItemText(&m_pSettingsMenu->items[currentItem], currentname);

	ConfigAppSave2();
}

void ToggleCF5toCF3StepUp(bool inc)
{
	WCHAR currentname[120];
	currentItem = m_pSettingsMenu->curitem;
	
	if((preferedemu == _1964x11)&&(AutoCF_1964 > 0))
	{
		if (inc)
		{
		CF5toCF3StepUp += 1;
		if(CF5toCF3StepUp > 100) CF5toCF3StepUp = 75;
		}
		else
		{
		CF5toCF3StepUp -= 1;
		if(CF5toCF3StepUp < 75) CF5toCF3StepUp = 100;
		}
	}

	XLMenu_CurRoutine = NULL;

	swprintf(currentname,L"CF5-CF3 Step Up : %2.0d%%",CF5toCF3StepUp);
	XLMenu_SetItemText(&m_pSettingsMenu->items[currentItem], currentname);

	ConfigAppSave2();
}

void ToggleCF3toCF1StepUp(bool inc)
{
	WCHAR currentname[120];
	currentItem = m_pSettingsMenu->curitem;
	
	if((preferedemu == _1964x11)&&(AutoCF_1964 > 0))
	{
		if (inc)
		{
		CF3toCF1StepUp += 1;
		if(CF3toCF1StepUp > 100) CF3toCF1StepUp = 75;
		}
		else
		{
		CF3toCF1StepUp -= 1;
		if(CF3toCF1StepUp < 75) CF3toCF1StepUp = 100;
		}
	}

	XLMenu_CurRoutine = NULL;

	swprintf(currentname,L"CF3-CF1 Step Up : %2.0d%%",CF3toCF1StepUp);
	XLMenu_SetItemText(&m_pSettingsMenu->items[currentItem], currentname);

	ConfigAppSave2();
}



void ToggleCF1toCF3StepDown(bool inc)
{
	WCHAR currentname[120];
	currentItem = m_pSettingsMenu->curitem;
	
	if((preferedemu == _1964x11)&&(AutoCF_1964 > 0))
	{
		if (inc)
		{
		CF1toCF3StepDown += 1;
		if(CF1toCF3StepDown > 100) CF1toCF3StepDown = 75;
		}
		else
		{
		CF1toCF3StepDown -= 1;
		if(CF1toCF3StepDown < 75) CF1toCF3StepDown = 100;
		}
	}

	XLMenu_CurRoutine = NULL;

	swprintf(currentname,L"CF1-CF3 Step Down : %2.0d%%",CF1toCF3StepDown);
	XLMenu_SetItemText(&m_pSettingsMenu->items[currentItem], currentname);

	ConfigAppSave2();
}



void ToggleCF3toCF5StepDown(bool inc)
{
	WCHAR currentname[120];
	currentItem = m_pSettingsMenu->curitem;
	
	if((preferedemu == _1964x11)&&(AutoCF_1964 > 0))
	{
		if (inc)
		{
		CF3toCF5StepDown += 1;
		if(CF3toCF5StepDown > 100) CF3toCF5StepDown = 75;
		}
		else
		{
		CF3toCF5StepDown -= 1;
		if(CF3toCF5StepDown < 75) CF3toCF5StepDown = 100;
		}
	}

	XLMenu_CurRoutine = NULL;

	swprintf(currentname,L"CF3-CF5 Step Down : %2.0d%%",CF3toCF5StepDown);
	XLMenu_SetItemText(&m_pSettingsMenu->items[currentItem], currentname);

	ConfigAppSave2();
}






void ToggleFrameSkip(bool inc)
{
    WCHAR currentname[120];
	currentItem = m_pSettingsMenu->curitem;

	if (inc)
	{
	FrameSkip++;
    if (FrameSkip > 2) FrameSkip = 0;
	}
	else
	{
	FrameSkip--;
    if (FrameSkip < 0) FrameSkip = 2;
	}
	
	XLMenu_CurRoutine = NULL;

	// Auto Frame Skip is only in 1964x11
  	if((FrameSkip == 2) && (preferedemu != _1964x11))
		FrameSkip = 0;

	switch(FrameSkip){
		case 0:
			swprintf(currentname,L"Frame Skip : Off");
			break;
		case 1:
			swprintf(currentname,L"Frame Skip : On");
			break;
		case 2:
			swprintf(currentname,L"Frame Skip : Auto");
			break;
	}

	XLMenu_SetItemText(&m_pSettingsMenu->items[currentItem], currentname);

	ConfigAppSave2();
}

void ToggleFogMode()
{
    WCHAR currentname[120];
	currentItem = m_pSettingsMenu->curitem;
	bUseLinFog =! bUseLinFog;
	
	XLMenu_CurRoutine = NULL;
	
  	if (!bUseLinFog)
	swprintf(currentname,L"Fog Mode : Range");
	else 
	swprintf(currentname,L"Fog Mode : Linear");
	XLMenu_SetItemText(&m_pSettingsMenu->items[currentItem], currentname);

	ConfigAppSave2();
}

void ToggleXboxPitch()
{
    WCHAR currentname[120];
	currentItem = m_pSettingsMenu->curitem;
		
	XLMenu_CurRoutine = NULL;
	
	if(XboxPitch == 2){
		XboxPitch = 4;
		swprintf(currentname,L"Texture Depth : 32bit");
	}
	else if(XboxPitch == 4){
		XboxPitch = 2;
		swprintf(currentname,L"Texture Depth : 16bit");
	}
	XLMenu_SetItemText(&m_pSettingsMenu->items[currentItem], currentname);

	ConfigAppSave2();
}

void ToggleTextureFilter(bool inc)
{
    WCHAR currentname[120];
	currentItem = m_pSettingsMenu->curitem;
	if (inc)
	{
	TextureMode++;
    if (TextureMode > 5) TextureMode = 1;
	}
	else
	{
	TextureMode--;
    if (TextureMode < 1) TextureMode = 5;
	}
	D3DDevice::SetTextureStageState(0, D3DTSS_MINFILTER, TextureMode);
	D3DDevice::SetTextureStageState(0, D3DTSS_MAGFILTER, TextureMode);

	XLMenu_CurRoutine = NULL;
	
	switch (TextureMode){
		case 1 : 	swprintf(currentname,L"Texture Filter : Point");
			break;
		case 2 : 	swprintf(currentname,L"Texture Filter : Linear");
			break;
		case 3 : 	swprintf(currentname,L"Texture Filter : Anisotropic");
			break;
		case 4 : 	swprintf(currentname,L"Texture Filter : Quincunx");
			break;
		case 5 : 	swprintf(currentname,L"Texture Filter : Gaussian");
			break;	}
	XLMenu_SetItemText(&m_pSettingsMenu->items[currentItem], currentname);

	ConfigAppSave2();
}

void ControllerSettingsMenu()
{
	DWORD dwMenuCommand = 0;

	XLMenu_CurRoutine = NULL;
	XLMenu_CurMenu = NULL;
	XLMenu_SetFont(&m_Font);

    WCHAR currentname[120];

	m_pSettingsMenu = XLMenu_Init((float)iIGMMenuTxtPosX,(float)iIGMMenuTxtPosY,4, GetMenuFontAlign(iIGMMenuTxtAlign)|MENU_WRAP, NULL);

	m_pSettingsMenu->itemcolor = dwMenuItemColor;
	m_pSettingsMenu->parent = m_pMainMenu;

	XLMenu_SetTitle(m_pSettingsMenu,L"Controller Settings",dwMenuTitleColor);
	
	//Sensitivity
	switch(Sensitivity){
		case 0:
			swprintf(currentname,L"Sensitivity : Increasing");
			break;
		case 11:
			swprintf(currentname,L"Sensitivity : Decreasing");
			break;
		default:
			swprintf(currentname,L"Sensitivity : %d%%",Sensitivity * 10);
			break;
	}
	XLMenu_AddItem2(m_pSettingsMenu,MITEM_ROUTINE,currentname,incSensitivity,decSensitivity);

	//Deadzone
	swprintf(currentname,L"Analog Deadzone : %2.0f%%", Deadzone);
	XLMenu_AddItem2(m_pSettingsMenu,MITEM_ROUTINE,currentname,incDeadzone,decDeadzone);

	//Button to Axis Threshold
	
	swprintf(currentname,L"Button to Axis Threshold : %2.0f%%", ButtonToAxisThresh);
	XLMenu_AddItem2(m_pSettingsMenu,MITEM_ROUTINE,currentname,incButtonToAxisThresh,decButtonToAxisThresh);


	//Mempak/RumblePak/NoPak
	if (DefaultPak == NoPak)
	{
		swprintf(currentname,L"Default Pak : None");
	}
	else if (DefaultPak == MemPak)
	{
		swprintf(currentname,L"Default Pak : MemPak");
	}
	else if (DefaultPak == RumblePak)
	{
		swprintf(currentname,L"Default Pak : RumblePak");
	}

	XLMenu_AddItem2(m_pSettingsMenu,MITEM_ROUTINE,currentname,incPakPlugin,decPakPlugin);

	XLMenu_Activate(m_pSettingsMenu);

	while( XLMenu_CurMenu == m_pSettingsMenu)
	{
		DrawLogo();
		dwMenuCommand = getAllGamepadsCommand(&gamepad);
		XLMenu_Routine(dwMenuCommand);
		g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
	}

	if (m_pSettingsMenu)
		XLMenu_Delete(m_pSettingsMenu);

}

void ToggleSensitivity(bool inc)
{
	WCHAR currentname[120];
	currentItem = m_pSettingsMenu->curitem;
	if (inc)
	{
	Sensitivity += 1;
	if(Sensitivity > 11) Sensitivity = 0;
	}
	else
	{
    Sensitivity -= 1;
	if(Sensitivity < 0) Sensitivity = 11;
	}
	
	XLMenu_CurRoutine = NULL;
	
	switch(Sensitivity){
		case 0:
			swprintf(currentname,L"Sensitivity : Increasing");
			break;
		case 11:
			swprintf(currentname,L"Sensitivity : Decreasing");
			break;
		default:
			swprintf(currentname,L"Sensitivity : %d%%",Sensitivity * 10);
			break;
	}

	XLMenu_SetItemText(&m_pSettingsMenu->items[currentItem], currentname);

	ConfigAppSave2();
}

void ToggleDeadzone(bool inc)
{
	WCHAR currentname[120];
	currentItem = m_pSettingsMenu->curitem;
	

	if (inc)
	{
	Deadzone += 1;
	if(Deadzone > 100) Deadzone = 0;
	//freakdave
	//Don't forget to alter the floats in the brackets accordingly if you change XBOX_CONTROLLER_DEAD_ZONE
	}
	else
	{
    Deadzone -= 1;
	if(Deadzone < 0) Deadzone = 100;
	}

	XBOX_CONTROLLER_DEAD_ZONE = float(32768) * (float(Deadzone)/float(100));
	
	XLMenu_CurRoutine = NULL;
	
	swprintf(currentname,L"Analog Deadzone: %2.0f%%",Deadzone);
	XLMenu_SetItemText(&m_pSettingsMenu->items[currentItem], currentname);

	ConfigAppSave2();
}

void ToggleButtonToAxisThresh(bool inc)
{
	WCHAR currentname[120];
	currentItem = m_pSettingsMenu->curitem;
	

	if (inc)
	{
	ButtonToAxisThresh += 1;
	if(ButtonToAxisThresh > 100) ButtonToAxisThresh = 0;
	//freakdave
	//Don't forget to alter the floats in the brackets accordingly if you change XBOX_CONTROLLER_DEAD_ZONE
	}
	else
	{
    ButtonToAxisThresh -= 1;
	if(ButtonToAxisThresh < 0) ButtonToAxisThresh = 100;
	}
	
	XLMenu_CurRoutine = NULL;
	
	swprintf(currentname,L"Button to Axis Threshold : %2.0f%%",ButtonToAxisThresh);
	XLMenu_SetItemText(&m_pSettingsMenu->items[currentItem], currentname);

	ConfigAppSave2();
}
void TogglePak(bool inc)
{
	WCHAR currentname[120];
	currentItem = m_pSettingsMenu->curitem;
	
	if (inc)
	{
		DefaultPak++;
		if(DefaultPak > RumblePak) DefaultPak = NoPak;
	}
	else
	{
		DefaultPak--;
		if(DefaultPak < NoPak) DefaultPak = RumblePak;
	}

	XLMenu_CurRoutine = NULL;
	
	//Mempak/RumblePak/NoPak
	if (DefaultPak == NoPak)
	{
		swprintf(currentname,L"Default Pak : None");
	}
	else if (DefaultPak == MemPak)
	{
		swprintf(currentname,L"Default Pak : MemPak");
	}
	else if (DefaultPak == RumblePak)
	{
		swprintf(currentname,L"Default Pak : RumblePak");
	}
	
	XLMenu_SetItemText(&m_pSettingsMenu->items[currentItem], currentname);

	ConfigAppSave2();
}


// controller config

extern void Drawcontrol();
extern void DrawcontrolHD();
extern int selectedelement;
extern bool changebutton;
int controller;
extern void ControllerReset();
extern "C" void _INPUT_LoadButtonMap(int *cfgData);
extern int ControllerConfig[76];

void ControllerMenu(void)
{

	DWORD dwMenuCommand = 0;

	XLMenu_CurRoutine = NULL;
	XLMenu_CurMenu = NULL;
	XLMenu_SetFont(&m_Font);

	m_pSettingsMenu = XLMenu_Init((float)iIGMMenuTxtPosX,(float)iIGMMenuTxtPosY,4, GetMenuFontAlign(iIGMMenuTxtAlign)|MENU_WRAP, NULL);

	m_pSettingsMenu->itemcolor = dwMenuItemColor;
	m_pSettingsMenu->parent = m_pMainMenu;

	XLMenu_SetTitle(m_pSettingsMenu,L"Select Controller",dwMenuTitleColor);

	XLMenu_AddItem(m_pSettingsMenu,MITEM_ROUTINE,L"Controller 1",Control1);
	XLMenu_AddItem(m_pSettingsMenu,MITEM_ROUTINE,L"Controller 2",Control2);
	XLMenu_AddItem(m_pSettingsMenu,MITEM_ROUTINE,L"Controller 3",Control3);
	XLMenu_AddItem(m_pSettingsMenu,MITEM_ROUTINE,L"Controller 4",Control4);


	XLMenu_Activate(m_pSettingsMenu);

	while( XLMenu_CurMenu == m_pSettingsMenu)
	{
		DrawLogo();
		dwMenuCommand = getAllGamepadsCommand(&gamepad);
		XLMenu_Routine(dwMenuCommand);
		g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
	}

	if (m_pSettingsMenu)
		XLMenu_Delete(m_pSettingsMenu);

	ConfigAppSave2();

}

void Control1()
{
controller=0;
Control();
}
void Control2()
{
controller=1;
Control();
}
void Control3()
{
controller=2;
Control();
}
void Control4()
{
controller=3;
Control();
}

void Control()
{

	DWORD dwMenuCommand = 0;

	XLMenu_CurRoutine = NULL;
	XLMenu_CurMenu = NULL;
	XLMenu_SetFont(&m_Font);

	m_pControllerMenu = XLMenu_Init((float)iIGMMenuTxtPosX,(float)iIGMMenuTxtPosY,1, GetMenuFontAlign(iIGMMenuTxtAlign)|MENU_WRAP, NULL);

	
	m_pControllerMenu->parent = m_pSettingsMenu;

	XLMenu_SetTitle(m_pControllerMenu,L" ",dwMenuTitleColor);


	XLMenu_Activate(m_pControllerMenu);

    bool bquit=false;
    selectedelement=0;

	while(!bquit)
	{
	if(bEnableHDTV)
		DrawcontrolHD();
	else
		Drawcontrol();
	XBInput_GetInput();

	if(g_Gamepads[controller].wPressedButtons	& XINPUT_GAMEPAD_DPAD_DOWN)
	{
	selectedelement++;
	if (selectedelement>18) selectedelement=0; }

	if(g_Gamepads[controller].wPressedButtons	& XINPUT_GAMEPAD_DPAD_UP)
	{
	selectedelement--;
	if (selectedelement<0) selectedelement=18; }

    if(g_Gamepads[controller].bPressedAnalogButtons[XINPUT_GAMEPAD_A]) changebutton=true;

    if(g_Gamepads[controller].bPressedAnalogButtons[XINPUT_GAMEPAD_X]) ControllerReset();

	for (int i=0;i<4;i++){
		if(g_Gamepads[i].bPressedAnalogButtons[XINPUT_GAMEPAD_B])	bquit=true;}

	g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
	if (changebutton) {
		if(bEnableHDTV)
			DrawcontrolHD();
		else
			Drawcontrol();
	g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
	ChangeControl();}
	}
	ConfigAppSave2();
	_INPUT_LoadButtonMap(ControllerConfig);
	XLMenu_Routine(MENU_BACK);
	if (m_pControllerMenu)
		XLMenu_Delete(m_pControllerMenu);
}

// non menu align
DWORD GetFontAlign(int iAlign)
{
	switch (iAlign) {
		case 3: return XBFONT_CENTER_Y; // include?
		case 2: return XBFONT_CENTER_X;
		case 1: return XBFONT_RIGHT;
		case 0:
		default: return XBFONT_LEFT;
	}
}

// menu align
DWORD GetMenuFontAlign(int iAlign)
{
	switch (GetFontAlign(iAlign)) {
		//case XBFONT_CENTER_Y: return MENU_CENTER_Y; // include?
		case XBFONT_CENTER_X: return MENU_CENTER_X;
		case XBFONT_RIGHT: return MENU_RIGHT;
		case XBFONT_LEFT:
		default: return MENU_LEFT;
	}
}