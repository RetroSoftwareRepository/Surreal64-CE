#include "Launcher.h"
#include "IniFile.h"
#include "BoxArtTable.h"
#include "IoSupport.h"
#include "RomList.h"
#include "config.h"
#include <xbapp.h>
#include <xbresource.h>
#include <xbfont.h>
#include "zlib/zlib.h"
#include "zlib/unzip.h"
#include "../Ingamemenu/xlmenu.h"
#include "musicmanager.h"
#include "Network.h"

extern BOOL IsXboxConnected();
extern BOOL UpdateIni(int Version);

extern CMusicManager  music;
extern bool bMusicPlaying;
XBGAMEPAD gamepad;
extern int preferedemu;
XLMenu    *m_pMainMenu;
XLMenu    *m_pSettingsMenu;
XLMenu    *m_pSkinMenu;
extern CXBFont		m_Font;					// Font	for	text display
extern void DrawLogo(bool Menu);
static int currentItem;
extern bool onhd;
extern int videoplugin;
extern char skinname[32];
extern void ReloadSkin();

// Ez0n3 - used for fixing user preferences in main menu
//extern bool PhysRam128();
extern int ConfigAppLoad3();
extern bool has128ram;
extern int iAudioPlugin;
string GetAudioPluginName(int iAudioPlugin);
string GetVideoPluginName(int iVideoPlugin);
string GetEmulatorName(int iEmulator);

void ToggleAudioPlugin(bool inc); // replace usellersp bool with iAudioPlugin
void ToggleVideoPlugin(bool inc);
void ToggleEmulator(bool inc);

void ToggleHideLaunchScreens();


void ToggleMaxVideoMem(bool inc);
void ToggleVideoplugin(bool inc);
void Toggle1964DynaMem(bool inc);
void Toggle1964PagingMem(bool inc);
void TogglePJ64DynaMem(bool inc);
void TogglePJ64PagingMem(bool inc);
void SettingsMenu();
void ExitToDash();
void ShutdownXbox();
void ToggleCredits();
void ToggleFlickerFilter(bool inc);
void VideoSettingsMenu();
void ToggleVertexMode(bool inc);
void ToggleTextureFilter(bool inc);
void ToggleSoftDisplayFilter();
void ToggleFrameSkip();
void ToggleSensitivity(bool inc);
void ControllerSettingsMenu();
void TogglePak();
void ToggleController1();
void ToggleController2();
void ToggleController3();
void ToggleController4();
void DownloadsMenu(void);
void UpdateRice(void);
DWORD WINAPI ThreadUpdateIni( LPVOID lpParam );
void UpdateRoms(void);
static int ChangeSkin(void);
static int SkinMenu(void);


// controller config

void Control();
void Control1();
void Control2();
void Control3();
void Control4();
void ControllerMenu();
extern void ChangeControl();

extern void CalculateEndCredits();
extern void DrawCredits();
extern int defilement;


//void ToggleAudioPlugin();
void incAudioPlugin(){ ToggleAudioPlugin(true); }
void decAudioPlugin(){ ToggleAudioPlugin(false); }
void incVideoPlugin(){ ToggleVideoPlugin(true); }
void decVideoPlugin(){ ToggleVideoPlugin(false); }
void incEmulator(){ ToggleEmulator(true); }
void decEmulator(){ ToggleEmulator(false); }

//
void incflicker(){ ToggleFlickerFilter(true); }
void decflicker(){ ToggleFlickerFilter(false); }
void incMaxVideoMem(){ ToggleMaxVideoMem(true); }
void decMaxVideoMem(){ ToggleMaxVideoMem(false); }
void inc1964DynaMem(){ Toggle1964DynaMem(true); }
void dec1964DynaMem(){ Toggle1964DynaMem(false); }
void inc1964PagingMem(){ Toggle1964PagingMem(true); }
void dec1964PagingMem(){ Toggle1964PagingMem(false); }
void incPJ64DynaMem(){ TogglePJ64DynaMem(true); }
void decPJ64DynaMem(){ TogglePJ64DynaMem(false); }
void incPJ64PagingMem(){ TogglePJ64PagingMem(true); }
void decPJ64PagingMem(){ TogglePJ64PagingMem(false); }
void incTextureFilter(){ ToggleTextureFilter(true); }
void decTextureFilter(){ ToggleTextureFilter(false); }
void incVertexMode(){ ToggleVertexMode(true); }
void decVertexMode(){ ToggleVertexMode(false); }
void incSensitivity(){ ToggleSensitivity(true); }
void decSensitivity(){ ToggleSensitivity(false); }
//
enum Pak
{
NoPak,
MemPak,
RumblePak,
};

// Ez0n3 - it's 256 everywhere else. why buck the sys
//extern char romname[32];
extern char romname[256];

// Ez0n3 - get the audio plugins name from int
string GetAudioPluginName(int iAudioPluginNum)
{
	string szAudioPlugin;

	switch (iAudioPluginNum) {
		case _AudioPluginNone : szAudioPlugin = "None"; break;
		case _AudioPluginLleRsp : szAudioPlugin = "LLE RSP"; break;
		case _AudioPluginBasic : szAudioPlugin = "Basic"; break;
		case _AudioPluginJttl : szAudioPlugin = "JttL"; break;
		//case _AudioPluginAzimer : szAudioPlugin = "Azimer"; break;
		//case _AudioPluginMissing : 
		default : szAudioPlugin = "JttL"; break;//freakdave - set JttL plugin as default
	}	

	return szAudioPlugin;
}

// Ez0n3 - get the video plugins name from int
string GetVideoPluginName(int iVideoPlugin)
{
	string szVideoPlugin;

	switch (iVideoPlugin) {
		case _VideoPluginRice510 : szVideoPlugin = "Rice 5.1.0"; break;
		case _VideoPluginRice531 : szVideoPlugin = "Rice 5.3.1"; break;
		case _VideoPluginRice560 : szVideoPlugin = "Rice 5.6.0"; break;
		case _VideoPluginRice610 : szVideoPlugin = "Rice 6.1.0"; break;
		case _VideoPluginMissing : 
		default : szVideoPlugin = "Unknown"; break;
	}	

	return szVideoPlugin;
}

// Ez0n3 - get the video plugins name from int
string GetEmulatorName(int iEmulator)
{
	string szEmulator;

	switch (iEmulator) {
		case _1964 : szEmulator = "1964"; break;
		case _Project64 : szEmulator = "Project64"; break;
		case _UltraHLE : szEmulator = "UltraHLE"; break;
		case _None : 
		default : szEmulator = "Unknown"; break;
	}	

	return szEmulator;
}



void MainMenu(void)
{
	// Ez0n3 - get user pref instead of always loading defaults
	//ConfigAppLoad2();	
	ConfigAppLoad3();
	

	DWORD dwMenuCommand = 0;
	WCHAR currentname[256];

	XLMenu_CurRoutine = NULL;
	XLMenu_CurMenu = NULL;
	XLMenu_SetFont(&m_Font);

	m_pMainMenu = XLMenu_Init(60,80,11,MENU_LEFT|MENU_WRAP,NULL);

	m_pMainMenu->topcolor = 0x40254365;
	m_pMainMenu->bottomcolor = 0x40556486;
	m_pMainMenu->seltopcolor = 0x4055FF86;
	m_pMainMenu->selbotcolor = 0x4055FF86;
	m_pMainMenu->itemcolor = 0xFFFFFFFF;
	m_pMainMenu->parent = NULL;

	XLMenu_SetTitle(m_pMainMenu,L"Main Menu",0xFF8080FF);

	XLMenu_AddItem(m_pMainMenu,MITEM_DISABLED,L"General Settings",NULL);
	XLMenu_AddItem(m_pMainMenu,MITEM_ROUTINE,L"Video Settings",VideoSettingsMenu);
	XLMenu_AddItem(m_pMainMenu,MITEM_ROUTINE,L"Choose Skin",SkinMenu);

	swprintf(currentname,L"%S specific settings",romname);
	XLMenu_AddItem(m_pMainMenu,MITEM_DISABLED,currentname,NULL);
	XLMenu_AddItem(m_pMainMenu,MITEM_ROUTINE,L"Emulator Settings",SettingsMenu);
	XLMenu_AddItem(m_pMainMenu,MITEM_ROUTINE,L"Controller Settings",ControllerSettingsMenu);
	XLMenu_AddItem(m_pMainMenu,MITEM_ROUTINE,L"Controller Edit",ControllerMenu);

	XLMenu_AddItem(m_pMainMenu,MITEM_DISABLED,L"--------------------------------",NULL);

	if ((onhd)&&(!fopen("D:\\Credits.txt","r"))) XLMenu_AddItem(m_pMainMenu,MITEM_DISABLED,L"Credits",NULL);
	else if ((!onhd)&&(!fopen("T:\\Credits.txt","r"))) XLMenu_AddItem(m_pMainMenu,MITEM_DISABLED,L"Credits",NULL);
	else XLMenu_AddItem(m_pMainMenu,MITEM_ROUTINE,L"Credits",ToggleCredits);

	//Network stuff - freakdave
/*	if (IsXboxConnected()){
	XLMenu_AddItem(m_pMainMenu,MITEM_ROUTINE,L"Content Download",DownloadsMenu);
	}else{
	XLMenu_AddItem(m_pMainMenu,MITEM_DISABLED,L"Content Download",NULL);
	}*/

	XLMenu_AddItem(m_pMainMenu,MITEM_ROUTINE,L"Quit Surreal",ExitToDash);
	XLMenu_AddItem(m_pMainMenu,MITEM_ROUTINE,L"Shutdown Xbox",ShutdownXbox);


	XLMenu_Activate(m_pMainMenu);
	XLMenu_Routine(MENU_NEXTITEM);

	while( XLMenu_CurMenu == m_pMainMenu)
	{
		DrawLogo(true);
		dwMenuCommand = getAllGamepadsCommand(&gamepad);
		XLMenu_Routine(dwMenuCommand);

		//freakdave - check for music playing
		if(gamepad.wPressedButtons&XINPUT_GAMEPAD_RIGHT_THUMB){
			bMusicPlaying = !bMusicPlaying;
			if(!bMusicPlaying){
				music.Pause();
			}else{
				music.Play();
			}
		}

		g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
	}
	
	XLMenu_CurRoutine = NULL;
	XLMenu_CurMenu = NULL;

	if (m_pMainMenu)
		XLMenu_Delete(m_pMainMenu);

}

void ToggleCredits()
{
	DWORD dwMenuCommand = 0;

	XLMenu_CurRoutine = NULL;
	XLMenu_CurMenu = NULL;
	XLMenu_SetFont(&m_Font);

	m_pSettingsMenu = XLMenu_Init(60,80,1, MENU_LEFT|MENU_WRAP, NULL);

	// make the menu transparent
	m_pSettingsMenu->bottomcolor = 0x00FFFFFF;
	m_pSettingsMenu->itemcolor = 0x00FFFFFF;
	m_pSettingsMenu->selbotcolor = 0x00FFFFFF;
	m_pSettingsMenu->seltopcolor = 0x00FFFFFF;
	m_pSettingsMenu->titlecolor = 0x00FFFFFF;
	m_pSettingsMenu->topcolor = 0x00FFFFFF;
	m_pSettingsMenu->parent = m_pMainMenu;
    
	XLMenu_SetTitle(m_pSettingsMenu,L" ",0x00FFFFFF);


	XLMenu_Activate(m_pSettingsMenu);

	CalculateEndCredits();
    bool bquit=false;

	while(!bquit)
	{
		DrawCredits();
		if(g_Gamepads->bLastAnalogButtons[XINPUT_GAMEPAD_A]) defilement=defilement-5;
		else defilement--;
		if(g_Gamepads->bPressedAnalogButtons[XINPUT_GAMEPAD_B]) bquit=true;
		XBInput_GetInput();
		g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
	}
	XLMenu_Routine(MENU_BACK);
	if (m_pSettingsMenu)
		XLMenu_Delete(m_pSettingsMenu);
}

// Skin
static int SkinMenu(void)
{
	XBGAMEPAD gamepad;
	char skins_path[] = "d:\\skins\\*";
	HANDLE				hFind;	
	WIN32_FIND_DATAA	oFindData;
	WCHAR				wstr[MITEM_STRINGLEN];
//	FILE				*fp;
//	char				config_path[MAX_PATH];
	int					nSkinCount = 0;

	DWORD dwMenuCommand = 0;

	XLMenu_CurRoutine	= NULL;
	XLMenu_CurMenu		= NULL;
	XLMenu_SetFont(&m_Font);

	// On débute la recherche
	hFind = FindFirstFile(skins_path, &oFindData);
	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			// Un repertoire donc on regarde si il y a un config.ini dedans
			if (oFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
					nSkinCount++;
			}
		} while (FindNextFile(hFind, &oFindData));
	}
	// Fin de la recherche
	FindClose(hFind);

	if (nSkinCount < 2) return MROUTINE_RETURN;

	m_pSkinMenu = XLMenu_Init(60, 80, nSkinCount + 2, MENU_LEFT|MENU_WRAP, NULL);
	m_pSkinMenu->topcolor = 0x40254365;
	m_pSkinMenu->bottomcolor = 0x40556486;
	m_pSkinMenu->seltopcolor = 0x4055FF86;
	m_pSkinMenu->selbotcolor = 0x4055FF86;
	m_pSkinMenu->itemcolor = 0xFFFFFFFF;
	m_pSkinMenu->parent = m_pMainMenu;

	XLMenu_SetTitle(m_pSkinMenu, L"Change Skin  ",0xFF8080FF);
	XLMenu_SetMaxShow(m_pSkinMenu, nSkinCount + 2);

	swprintf(wstr, L"Current: %S", skinname);
	XLMenu_AddItem(m_pSkinMenu, MITEM_DISABLED, wstr, NULL);
	XLMenu_AddItem(m_pSkinMenu, MITEM_DISABLED, L"----------------------------", NULL);

	// On débute la recherche
	hFind = FindFirstFile(skins_path, &oFindData);

	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			// Un repertoire donc on regarde si il y a un config.ini dedans
			if (oFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
					swprintf(wstr, L"%S", oFindData.cFileName);
					XLMenu_AddItem(m_pSkinMenu, MITEM_ROUTINE, wstr, ChangeSkin);
			}
		} while (FindNextFile(hFind, &oFindData));
	}
	// Fin de la recherche
	FindClose(hFind);

	m_pSkinMenu->curitem = 2;
	XLMenu_Activate(m_pSkinMenu);

	while(XLMenu_CurMenu == m_pSkinMenu) {
		DrawLogo(true);
		dwMenuCommand = getAllGamepadsCommand(&gamepad);
		XLMenu_Routine(dwMenuCommand);
		g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
	}

	if (m_pSkinMenu) {
		XLMenu_Delete(m_pSkinMenu);
		m_pSkinMenu = NULL;
	}

	return MROUTINE_RETURN;
}

static int ChangeSkin(void)
{
//	XLMenu		*menu_list;
	char		skin_name[64];
	char		old_skin_name[64];
	WCHAR		wstr[256];
	char        testname[256];
	FILE *fp;
	int elementstocheck;

	currentItem = m_pSkinMenu->curitem;

	strcpy(old_skin_name, skinname);

	// On convertit en char
	wcstombs( skin_name, m_pSkinMenu->items[currentItem].string, 256 );

	// Check if all we need is in the folder
	elementstocheck = 6;
	bool checkpass = true;
	for (int i=0;i<elementstocheck;i++){
		switch (i) {
	case 0 :
		sprintf(testname,"D:\\skins\\%s\\MSFont.xpr",skin_name);
		break;
	case 1 :
		sprintf(testname,"D:\\skins\\%s\\Font.xpr",skin_name);
		break;
	case 2 :
		sprintf(testname,"D:\\skins\\%s\\bg.jpg",skin_name);
		break;
	case 3 :
		sprintf(testname,"D:\\skins\\%s\\ingamebg.jpg",skin_name);
		break;
	case 4 :
		sprintf(testname,"D:\\skins\\%s\\main.wma",skin_name);
		break;
	case 5 :
		sprintf(testname,"D:\\skins\\%s\\progress.png",skin_name);
		break;
	}
	fp = fopen(testname,"r");
	if (!fp) { checkpass = false;break;}
    fclose(fp);
	}

	if (checkpass)	{

	// On change la skin
	strcpy( skinname, skin_name );
	
	// On charge la nouvelle skin
    ReloadSkin();

	// On sauvegarde la configuration
	ConfigAppSave();}

	// Pour finir on change le texte de la skin current
	swprintf( wstr, L"Current: %S", skinname );
	XLMenu_SetItemText( &m_pSkinMenu->items[0], wstr );

	return MROUTINE_RETURN;
}

void ExitToDash(void)
{
	LD_LAUNCH_DASHBOARD LaunchData = { XLD_LAUNCH_DASHBOARD_MAIN_MENU };
	XLaunchNewImage( NULL, (LAUNCH_DATA*)&LaunchData );
}

void ShutdownXbox(void)
{
	CIoSupport IOSupport;
	IOSupport.Shutdown();
}

void SettingsMenu(void)
{
	DWORD dwMenuCommand = 0;

	XLMenu_CurRoutine = NULL;
	XLMenu_CurMenu = NULL;
	XLMenu_SetFont(&m_Font);

    WCHAR currentname[120];

	// Ez0n3 - more items
	//m_pSettingsMenu = XLMenu_Init(60,80,8, MENU_LEFT|MENU_WRAP, NULL);
	m_pSettingsMenu = XLMenu_Init(60,80,10, MENU_LEFT|MENU_WRAP, NULL);

	m_pSettingsMenu->topcolor = 0x40254365;
	m_pSettingsMenu->bottomcolor = 0x40556486;
	m_pSettingsMenu->seltopcolor = 0x4055FF86;
	m_pSettingsMenu->selbotcolor = 0x4055FF86;
	m_pSettingsMenu->itemcolor = 0xFFFFFFFF;
	m_pSettingsMenu->parent = m_pMainMenu;

	XLMenu_SetTitle(m_pSettingsMenu,L"Settings",0xFF8080FF);

	
	
	
	// Ez0n3 - emulator selector in menu
	swprintf(currentname, L"Emulator : %S", GetEmulatorName(preferedemu).c_str());
	XLMenu_AddItem2(m_pSettingsMenu,MITEM_ROUTINE,currentname,incEmulator,decEmulator);
	
	// Ez0n3 - video plugin selector in menu
	swprintf(currentname, L"Video Plugin : %S", GetVideoPluginName(videoplugin).c_str());
	XLMenu_AddItem2(m_pSettingsMenu,MITEM_ROUTINE,currentname,incVideoPlugin,decVideoPlugin);


	
	
	// Ez0n3 - use iAudioPlugin instead to replace bool
	//if (bUseLLERSP)
	//XLMenu_AddItem(m_pSettingsMenu,MITEM_ROUTINE,L"Audio Plugin : Basic Audio",ToggleAudioPlugin);
	//else
	//XLMenu_AddItem(m_pSettingsMenu,MITEM_ROUTINE,L"Audio Plugin : JttL",ToggleAudioPlugin);
	swprintf(currentname, L"Audio Plugin : %S", GetAudioPluginName(iAudioPlugin).c_str());
	XLMenu_AddItem2(m_pSettingsMenu,MITEM_ROUTINE,currentname,incAudioPlugin,decAudioPlugin);
	
	
	// Ez0n3 - reinstate max video mem until freakdave finishes this
	//Doesn't work at the moment -> Keep Memory Free has currently a static value
	//swprintf(currentname,L"Keep Memory Free : %d Mbytes",dwFreeMem);
	//XLMenu_AddItem2(m_pSettingsMenu,MITEM_ROUTINE,currentname,incMaxVideoMem,decMaxVideoMem);
	swprintf(currentname,L"Max Video Memory : %d Mbytes",dwMaxVideoMem);
	XLMenu_AddItem2(m_pSettingsMenu,MITEM_ROUTINE,currentname,incMaxVideoMem,decMaxVideoMem);
	

	XLMenu_AddItem(m_pSettingsMenu,MITEM_DISABLED,L"1964",NULL);

	swprintf(currentname,L"Dynarec Memory : %d Mbytes",dw1964DynaMem);
	XLMenu_AddItem2(m_pSettingsMenu,MITEM_ROUTINE,currentname,inc1964DynaMem,dec1964DynaMem);

	swprintf(currentname,L"Paging Memory : %d Mbytes",dw1964PagingMem);
	XLMenu_AddItem2(m_pSettingsMenu,MITEM_ROUTINE,currentname,inc1964PagingMem,dec1964PagingMem);

	XLMenu_AddItem(m_pSettingsMenu,MITEM_DISABLED,L"Project 64",NULL);

	swprintf(currentname,L"Dynarec Memory : %d Mbytes",dwPJ64DynaMem);
	XLMenu_AddItem2(m_pSettingsMenu,MITEM_ROUTINE,currentname,incPJ64DynaMem,decPJ64DynaMem);

	swprintf(currentname,L"Paging Memory : %d Mbytes",dwPJ64PagingMem);
	XLMenu_AddItem2(m_pSettingsMenu,MITEM_ROUTINE,currentname,incPJ64PagingMem,decPJ64PagingMem);
	
	
	XLMenu_Activate(m_pSettingsMenu);

	while( XLMenu_CurMenu == m_pSettingsMenu)
	{
		DrawLogo(true);
		dwMenuCommand = getAllGamepadsCommand(&gamepad);
		XLMenu_Routine(dwMenuCommand);
		//freakdave - check for music playing
		if(gamepad.wPressedButtons&XINPUT_GAMEPAD_RIGHT_THUMB){
			bMusicPlaying = !bMusicPlaying;
			if(!bMusicPlaying){
				music.Pause();
			}else{
				music.Play();
			}
		}
		g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
	}

	if (m_pSettingsMenu)
		XLMenu_Delete(m_pSettingsMenu);

	ConfigAppSave2();

}

// Ez0n3 - use iAudioPlugin instead of the usellersp bool
void ToggleAudioPlugin(bool inc)
{	
    WCHAR currentname[120];
	currentItem = m_pSettingsMenu->curitem;
	if (inc)
	{
		iAudioPlugin++;
		if (iAudioPlugin > (_AudioPluginMissing - 2)) iAudioPlugin=0;
	}
	else
	{
		iAudioPlugin--;
		if (iAudioPlugin < 0) iAudioPlugin=(_AudioPluginMissing - 2);
	}

	XLMenu_CurRoutine = NULL;
	
	// Ez0n3 - audio plugins
	swprintf(currentname, L"Audio Plugin : %S", GetAudioPluginName(iAudioPlugin).c_str());
	XLMenu_SetItemText(&m_pSettingsMenu->items[currentItem], currentname);
}

// Ez0n3 - video plugin toggle
void ToggleVideoPlugin(bool inc)
{	
    WCHAR currentname[120];
	currentItem = m_pSettingsMenu->curitem;
	if (inc)
	{
		videoplugin++;
		if (videoplugin > (_VideoPluginMissing - 1)) videoplugin=0;
	}
	else
	{
		videoplugin--;
		if (videoplugin < 0) videoplugin=(_VideoPluginMissing - 1);
	}

	XLMenu_CurRoutine = NULL;
	
	// Ez0n3 - video plugins
	swprintf(currentname, L"Video Plugin : %S", GetVideoPluginName(videoplugin).c_str());
	XLMenu_SetItemText(&m_pSettingsMenu->items[currentItem], currentname);
}

// Ez0n3 - emulator toggle
void ToggleEmulator(bool inc)
{	
    WCHAR currentname[120];
	currentItem = m_pSettingsMenu->curitem;
	if (inc)
	{
		preferedemu++;
		if (preferedemu > (_None - 1)) preferedemu=0;
	}
	else
	{
		preferedemu--;
		if (preferedemu < 0) preferedemu=(_None - 1);
	}

	XLMenu_CurRoutine = NULL;
	
	// Ez0n3 - video plugins
	swprintf(currentname, L"Emulator : %S", GetEmulatorName(preferedemu).c_str());
	XLMenu_SetItemText(&m_pSettingsMenu->items[currentItem], currentname);

}



void ToggleMaxVideoMem(bool inc)
{
	// Ez0n3 - we'll use the old one until freakdave finishes this
	int maxVideoMem = 10;
	
    WCHAR currentname[120];
	currentItem = m_pSettingsMenu->curitem;
	if (inc)
	{
    dwMaxVideoMem++;
	if (dwMaxVideoMem > maxVideoMem) dwMaxVideoMem=2;
	}
	else
	{
    dwMaxVideoMem--;
	if (dwMaxVideoMem < 2) dwMaxVideoMem=maxVideoMem;
	}

	XLMenu_CurRoutine = NULL;
	
	swprintf(currentname,L"Max Video Memory : %d Mbytes",dwMaxVideoMem);
	XLMenu_SetItemText(&m_pSettingsMenu->items[currentItem], currentname);
	
	

//    WCHAR currentname[120];
	//currentItem = m_pSettingsMenu->curitem;
/*
	if (inc)
	{
    dwFreeMem++;
	if (dwFreeMem > 10) dwFreeMem=2;
	}
	else
	{
    dwFreeMem--;
	if (dwFreeMem < 2) dwFreeMem=10;
	}
	
	XLMenu_CurRoutine = NULL;
	
	swprintf(currentname,L"Keep Memory Free : %d Mbytes",dwFreeMem);
	XLMenu_SetItemText(&m_pSettingsMenu->items[currentItem], currentname);
	*/
}

void Toggle1964DynaMem(bool inc)
{
	// Ez0n3 - bump up the memory settings
	int maxDynaMem = 20; //16
	if (has128ram) maxDynaMem = 32;

    WCHAR currentname[120];
	currentItem = m_pSettingsMenu->curitem;
	if (inc) 
	{
    dw1964DynaMem++;
	if (dw1964DynaMem > maxDynaMem) dw1964DynaMem=0;
	}
	else
	{
    dw1964DynaMem--;
	if (dw1964DynaMem < 0) dw1964DynaMem=maxDynaMem;
	}
	
	XLMenu_CurRoutine = NULL;
	
	swprintf(currentname,L"Dynarec Memory : %d Mbytes",dw1964DynaMem);
	XLMenu_SetItemText(&m_pSettingsMenu->items[currentItem], currentname);


}

void Toggle1964PagingMem(bool inc)
{
	// Ez0n3 - bump up the memory settings
	int maxPagingMem = 20; //16
	if (has128ram) maxPagingMem = 64;
	
    WCHAR currentname[120];
	currentItem = m_pSettingsMenu->curitem;
	if (inc)
	{
    dw1964PagingMem++;
	if (dw1964PagingMem > maxPagingMem) dw1964PagingMem=0;
	}
	else
	{
    dw1964PagingMem--;
	if (dw1964PagingMem < 0) dw1964PagingMem=maxPagingMem;
	}

	XLMenu_CurRoutine = NULL;
	
	swprintf(currentname,L"Paging Memory : %d Mbytes",dw1964PagingMem);
	XLMenu_SetItemText(&m_pSettingsMenu->items[currentItem], currentname);

}

void TogglePJ64DynaMem(bool inc)
{
	// Ez0n3 - bump up the memory settings
	int maxDynaMem = 20; //16
	if (has128ram) maxDynaMem = 32;

    WCHAR currentname[120];
	currentItem = m_pSettingsMenu->curitem;
	if (inc)
	{
    dwPJ64DynaMem++;
	if (dwPJ64DynaMem > maxDynaMem) dwPJ64DynaMem=0;
	}
	else
	{
    dwPJ64DynaMem--;
	if (dwPJ64DynaMem < 0) dwPJ64DynaMem=maxDynaMem;
	}
	
	XLMenu_CurRoutine = NULL;
	
	swprintf(currentname,L"Dynarec Memory : %d Mbytes",dwPJ64DynaMem);
	XLMenu_SetItemText(&m_pSettingsMenu->items[currentItem], currentname);

}

void TogglePJ64PagingMem(bool inc)
{
	// Ez0n3 - bump up the memory settings
	int maxPagingMem = 20; //16
	if (has128ram) maxPagingMem = 64;

    WCHAR currentname[120];
	currentItem = m_pSettingsMenu->curitem;
	if (inc) 
	{
    dwPJ64PagingMem++;
	if (dwPJ64PagingMem > maxPagingMem) dwPJ64PagingMem=0;
	}
	else
	{
    dwPJ64PagingMem--;
	if (dwPJ64PagingMem < 0) dwPJ64PagingMem=maxPagingMem;
	}
	
	XLMenu_CurRoutine = NULL;
	
	swprintf(currentname,L"Paging Memory : %d Mbytes",dwPJ64PagingMem);
	XLMenu_SetItemText(&m_pSettingsMenu->items[currentItem], currentname);

}

extern void Launch();
int m_emulator;
void Launch1964();
void PJ64Launch();
void UltraHLELaunch();
void launch510();
void launch531();
void launch560();
void launch611();
extern int actualrom;
extern void display_compatible();

void selectvideomode(void)
{
	// Ez0n3 - get user pref instead of always just defaults
	// ConfigAppLoad2();	
	int tmp_preferedemu = preferedemu; // get the just selected pref emu
	ConfigAppLoad3();
	preferedemu = tmp_preferedemu; // re-set it to retain value - doesn't get saved in between emu/plug selections

	DWORD dwMenuCommand = 0;

	XLMenu_CurRoutine = NULL;
	XLMenu_CurMenu = NULL;
	XLMenu_SetFont(&m_Font);

	m_pSettingsMenu = XLMenu_Init(210,160,4,MENU_LEFT|MENU_WRAP,NULL);

	m_pSettingsMenu->topcolor = 0x40254365;
	m_pSettingsMenu->bottomcolor = 0x40556486;
	m_pSettingsMenu->seltopcolor = 0x4055FF86;
	m_pSettingsMenu->selbotcolor = 0x4055FF86;
	m_pSettingsMenu->itemcolor = 0xFFFFFFFF;
	m_pSettingsMenu->parent = m_pMainMenu;

	XLMenu_SetTitle(m_pSettingsMenu,L"Select Video Plugin",0xFF8080FF);

	XLMenu_AddItem(m_pSettingsMenu,MITEM_ROUTINE,L"Rice 5.10",launch510);
	XLMenu_AddItem(m_pSettingsMenu,MITEM_ROUTINE,L"Rice 5.31",launch531);
	XLMenu_AddItem(m_pSettingsMenu,MITEM_ROUTINE,L"Rice 5.60",launch560);
	XLMenu_AddItem(m_pSettingsMenu,MITEM_ROUTINE,L"Rice 6.11",launch611);

	XLMenu_Activate(m_pSettingsMenu);

	
	for (int i=0;i< videoplugin;i++){

	XLMenu_Routine(MENU_NEXTITEM);
	
		//Ez0n3 - had to increase this a tad to give the ini time to load
		//Sleep(200); //too fast
		Sleep(300); //too fast for ini load

	}

	while( XLMenu_CurMenu == m_pSettingsMenu)
	{
		DrawLogo(true);
		dwMenuCommand = getAllGamepadsCommand(&gamepad);
		XLMenu_Routine(dwMenuCommand);
		//freakdave - check for music playing
		if(gamepad.wPressedButtons&XINPUT_GAMEPAD_RIGHT_THUMB){
			bMusicPlaying = !bMusicPlaying;
			if(!bMusicPlaying){
				music.Pause();
			}else{
				music.Play();
			}
		}
		g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
	}
	
	if (m_pSettingsMenu)
		XLMenu_Delete(m_pSettingsMenu);

}

void LaunchMenu(void)
{
	// Ez0n3 - get user pref instead of always just defaults
	// ConfigAppLoad2();	
	ConfigAppLoad3();

	DWORD dwMenuCommand = 0;

	XLMenu_CurRoutine = NULL;
	XLMenu_CurMenu = NULL;
	XLMenu_SetFont(&m_Font);

	m_pMainMenu = XLMenu_Init(210,160,3,MENU_LEFT|MENU_WRAP,NULL);

	m_pMainMenu->topcolor = 0x40254365;
	m_pMainMenu->bottomcolor = 0x40556486;
	m_pMainMenu->seltopcolor = 0x4055FF86;
	m_pMainMenu->selbotcolor = 0x4055FF86;
	m_pMainMenu->itemcolor = 0xFFFFFFFF;
	m_pMainMenu->parent = NULL;

	XLMenu_SetTitle(m_pMainMenu,L"Launch Menu",0xFF8080FF);

	XLMenu_AddItem(m_pMainMenu,MITEM_ROUTINE,L"Launch with 1964",Launch1964);
	XLMenu_AddItem(m_pMainMenu,MITEM_ROUTINE,L"Launch with PJ64",PJ64Launch);
	XLMenu_AddItem(m_pMainMenu,MITEM_ROUTINE,L"Launch with UltraHLE",UltraHLELaunch);


	XLMenu_Activate(m_pMainMenu);

	
	for (int i=0;i< preferedemu;i++){

	XLMenu_Routine(MENU_NEXTITEM);
		
		//Ez0n3 - had to increase this a tad to give the ini time to load
		//Sleep(200); //too fast
		Sleep(300); //too fast for ini load

	}

	while( XLMenu_CurMenu == m_pMainMenu)
	{
		DrawLogo(true);
		//display_compatible();  //- not needed for the moment, nothing in the inifile?
		dwMenuCommand = getAllGamepadsCommand(&gamepad);
		XLMenu_Routine(dwMenuCommand);
		//freakdave - check for music playing
		if(gamepad.wPressedButtons&XINPUT_GAMEPAD_RIGHT_THUMB){
			bMusicPlaying = !bMusicPlaying;
			if(!bMusicPlaying){
				music.Pause();
			}else{
				music.Play();
			}
		}
		g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
	}
	
	XLMenu_CurRoutine = NULL;
	XLMenu_CurMenu = NULL;

	if (m_pMainMenu)
		XLMenu_Delete(m_pMainMenu);

}

void Launch1964(void)
{
	m_emulator = _1964;
	preferedemu = _1964;
	selectvideomode();
}

void PJ64Launch(void)
{
	m_emulator = _Project64;
	preferedemu = _Project64;
	selectvideomode();
}

void UltraHLELaunch(void)
{
	XLMenu_Activate(NULL);            // kill the current menu
    XLMenu_CurRoutine = NULL; 
	m_emulator = _UltraHLE;
	preferedemu = _UltraHLE;
	ConfigAppSave2();
	Launch();
}

void launch510()
{
videoplugin = 0;
ConfigAppSave2();
Launch();
}

void launch531()
{
videoplugin = 1;
ConfigAppSave2();
Launch();
}

void launch560()
{
videoplugin = 2;
ConfigAppSave2();
Launch();
}

void launch611()
{
videoplugin = 3;
ConfigAppSave2();
Launch();
}


// Ez0n3 - launch rom without emulator and video plugin selectors
void LaunchHideScreens(void)
{
	//XLMenu_Activate(NULL);            // kill the current menu
    //XLMenu_CurRoutine = NULL; 

	ConfigAppLoad3();
	
	// just to be safe
	switch (videoplugin) {
		case _VideoPluginRice510 : videoplugin = _VideoPluginRice510; //5.1.0
			break;
		case _VideoPluginRice531 : videoplugin = _VideoPluginRice531; //5.3.1
			break;
		case _VideoPluginRice560 : videoplugin = _VideoPluginRice560; //5.6.0
			break;
		case _VideoPluginRice610 : videoplugin = _VideoPluginRice610; //6.1.1
			break;
		default : videoplugin = _VideoPluginRice560; // launch with 560 if no plugin is set or unrecognized
			break;
	}

	// don't really need to re-set preferedemu - just to be safe
	switch (preferedemu) {
		case _1964 : m_emulator = _1964; preferedemu = _1964;
			break;
		case _Project64 : m_emulator = _Project64; preferedemu = _Project64;
			break;
		case _UltraHLE : m_emulator = _UltraHLE; preferedemu = _UltraHLE;
			break;
		default : m_emulator = _1964; preferedemu = _1964; // launch with 1964 if no preferred emulator is set
			break;
	}
	
	ConfigAppSave2();
	Launch();
}


void VideoSettingsMenu(void)
{
	DWORD dwMenuCommand = 0;

	XLMenu_CurRoutine = NULL;
	XLMenu_CurMenu = NULL;
	XLMenu_SetFont(&m_Font);

    WCHAR currentname[120];

	// Ez0n3 - more items
	//m_pSettingsMenu = XLMenu_Init(60,80,4, MENU_LEFT|MENU_WRAP, NULL);
	m_pSettingsMenu = XLMenu_Init(60,80,7, MENU_LEFT|MENU_WRAP, NULL);

	m_pSettingsMenu->topcolor = 0x40254365;
	m_pSettingsMenu->bottomcolor = 0x40556486;
	m_pSettingsMenu->seltopcolor = 0x4055FF86;
	m_pSettingsMenu->selbotcolor = 0x4055FF86;
	m_pSettingsMenu->itemcolor = 0xFFFFFFFF;
	m_pSettingsMenu->parent = m_pMainMenu;

	XLMenu_SetTitle(m_pSettingsMenu,L"Video Settings",0xFF8080FF);

    if (!FlickerFilter)
	swprintf(currentname,L"Flicker Filter : Disabled");
	else 
	swprintf(currentname,L"Flicker Filter : %d",FlickerFilter);
	XLMenu_AddItem2(m_pSettingsMenu,MITEM_ROUTINE,currentname,incflicker,decflicker);

	if (!SoftDisplayFilter)
	swprintf(currentname,L"Soft Display Filter : Disabled");
	else 
	swprintf(currentname,L"Soft Display Filter : Enabled");
	XLMenu_AddItem(m_pSettingsMenu,MITEM_ROUTINE,currentname,ToggleSoftDisplayFilter);

	switch (TextureMode){
		case 1 : 	swprintf(currentname,L"Texture Filter : POINT");
			break;
		case 2 : 	swprintf(currentname,L"Texture Filter : LINEAR");
			break;
		case 3 : 	swprintf(currentname,L"Texture Filter : ANISOTROPIC");
			break;
		case 4 : 	swprintf(currentname,L"Texture Filter : QUINCUNX");
			break;
		case 5 : 	swprintf(currentname,L"Texture Filter : GAUSSIAN-CUBIC");
			break;	}
	XLMenu_AddItem2(m_pSettingsMenu,MITEM_ROUTINE,currentname,incTextureFilter,decTextureFilter);

	switch (VertexMode)	{
	 case 0:
			swprintf(currentname,L"Vertex : Pure Device, crash the sub menu!");
	    break;
	 case 1:
			swprintf(currentname,L"Vertex : Soft Vertex");
	    break;	
	 case 2:
			swprintf(currentname,L"Vertex : Hard Vertex");
	    break;
	 case 3:
			swprintf(currentname,L"Vertex : Mixed Vertex");
		break;	}
	XLMenu_AddItem2(m_pSettingsMenu,MITEM_ROUTINE,currentname,incVertexMode,decVertexMode);


	//FrameSkip
	if (!FrameSkip)
	swprintf(currentname,L"Skip Frames : No");
	else 
	swprintf(currentname,L"Skip Frames : Yes");
	XLMenu_AddItem(m_pSettingsMenu,MITEM_ROUTINE,currentname,ToggleFrameSkip);

	
	// Ez0n3 - more emulator settings
	// freakdave - actually a launcher setting
	XLMenu_AddItem(m_pSettingsMenu,MITEM_DISABLED,L"Launcher Settings",NULL);
	
	// Ez0n3 - launch screens enable / disable
	// freakdave - should be yes or no
	if (HideLaunchScreens)
		swprintf(currentname,L"Hide Launch Screens : Yes");
	else
		swprintf(currentname,L"Hide Launch Screens : No");
		
	XLMenu_AddItem(m_pSettingsMenu,MITEM_ROUTINE,currentname,ToggleHideLaunchScreens);
	
	
	
	XLMenu_Activate(m_pSettingsMenu);

	while( XLMenu_CurMenu == m_pSettingsMenu)
	{
		DrawLogo(true);
		dwMenuCommand = getAllGamepadsCommand(&gamepad);
		XLMenu_Routine(dwMenuCommand);
		//freakdave - check for music playing
		if(gamepad.wPressedButtons&XINPUT_GAMEPAD_RIGHT_THUMB){
			bMusicPlaying = !bMusicPlaying;
			if(!bMusicPlaying){
				music.Pause();
			}else{
				music.Play();
			}
		}
		g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
	}

	if (m_pSettingsMenu)
		XLMenu_Delete(m_pSettingsMenu);

}

// Ez0n3 - enable / disable launch screens
// freakdave - same here, should be yes or no
void ToggleHideLaunchScreens()
{
	currentItem = m_pSettingsMenu->curitem;

	HideLaunchScreens = !HideLaunchScreens;
	XLMenu_CurRoutine = NULL;
	
	if (HideLaunchScreens)
		XLMenu_SetItemText(&m_pSettingsMenu->items[currentItem], L"Hide Launch Screens : Yes");
	else
		XLMenu_SetItemText(&m_pSettingsMenu->items[currentItem], L"Hide Launch Screens : No");
		
	ConfigAppSave();	
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

	ConfigAppSave();
}

void ToggleSoftDisplayFilter()
{
    WCHAR currentname[120];
	currentItem = m_pSettingsMenu->curitem;
	SoftDisplayFilter =! SoftDisplayFilter;
	D3DDevice::SetSoftDisplayFilter(SoftDisplayFilter);
	
	XLMenu_CurRoutine = NULL;
	
  	if (!SoftDisplayFilter)
	swprintf(currentname,L"Soft Display Filter : Disabled");
	else 
	swprintf(currentname,L"Soft Display Filter : Enabled");
	XLMenu_SetItemText(&m_pSettingsMenu->items[currentItem], currentname);

	ConfigAppSave();
}

void ToggleFrameSkip()
{
    WCHAR currentname[120];
	currentItem = m_pSettingsMenu->curitem;
	FrameSkip =! FrameSkip;
	
	XLMenu_CurRoutine = NULL;
	
  	if (!FrameSkip)
	swprintf(currentname,L"Skip Frames : No");
	else 
	swprintf(currentname,L"Skip Frames : Yes");
	XLMenu_SetItemText(&m_pSettingsMenu->items[currentItem], currentname);

	ConfigAppSave();
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
		case 1 : 	swprintf(currentname,L"Texture Filter : POINT");
			break;
		case 2 : 	swprintf(currentname,L"Texture Filter : LINEAR");
			break;
		case 3 : 	swprintf(currentname,L"Texture Filter : ANISOTROPIC");
			break;
		case 4 : 	swprintf(currentname,L"Texture Filter : QUINCUNX");
			break;
		case 5 : 	swprintf(currentname,L"Texture Filter : GAUSSIAN-CUBIC");
			break;	}
	XLMenu_SetItemText(&m_pSettingsMenu->items[currentItem], currentname);

	ConfigAppSave();
}

void ToggleVertexMode(bool inc)
{
    WCHAR currentname[120];
	currentItem = m_pSettingsMenu->curitem;
	if (inc)
	{
	VertexMode++;
    if (VertexMode > 3) VertexMode = 0;
	}
	else
	{
	VertexMode--;
    if (VertexMode < 0) VertexMode = 3;
	}
	
	XLMenu_CurRoutine = NULL;
	
	switch (VertexMode)	{
	 case 0: swprintf(currentname,L"Vertex : Pure Device, crash the sub menu!");
	    break;
	 case 1 : swprintf(currentname,L"Vertex : Soft Vertex");
	    break;	
	 case 2 : swprintf(currentname,L"Vertex : Hard Vertex");
	    break;
	 case 3 : swprintf(currentname,L"Vertex : Mixed Vertex");
		break;	}
	XLMenu_SetItemText(&m_pSettingsMenu->items[currentItem], currentname);

	ConfigAppSave();
}

void ControllerSettingsMenu()
{
	DWORD dwMenuCommand = 0;

	XLMenu_CurRoutine = NULL;
	XLMenu_CurMenu = NULL;
	XLMenu_SetFont(&m_Font);

    WCHAR currentname[120];

	m_pSettingsMenu = XLMenu_Init(60,80,6, MENU_LEFT|MENU_WRAP, NULL);

	m_pSettingsMenu->topcolor = 0x40254365;
	m_pSettingsMenu->bottomcolor = 0x40556486;
	m_pSettingsMenu->seltopcolor = 0x4055FF86;
	m_pSettingsMenu->selbotcolor = 0x4055FF86;
	m_pSettingsMenu->itemcolor = 0xFFFFFFFF;
	m_pSettingsMenu->parent = m_pMainMenu;

	XLMenu_SetTitle(m_pSettingsMenu,L"Controller Settings",0xFF8080FF);
	
	//Sensitivity
	swprintf(currentname,L"Analog Sensitivity : %d", Sensitivity);
	XLMenu_AddItem2(m_pSettingsMenu,MITEM_ROUTINE,currentname,incSensitivity,decSensitivity);

	//Mempak/RumblePak/NoPak

	if (DefaultPak == NoPak)
	{
	swprintf(currentname,L"Default Pak : None");
	}

	if (DefaultPak == MemPak)
	{
	swprintf(currentname,L"Default Pak : MemPak");
	}

	if (DefaultPak == RumblePak)
	{
	swprintf(currentname,L"Default Pak : RumblePak");
	}
	XLMenu_AddItem(m_pSettingsMenu,MITEM_ROUTINE,currentname,TogglePak);


	//Controller States
	if (!EnableController1)
	swprintf(currentname,L"Controller 1 : Disabled");
	else 
	swprintf(currentname,L"Controller 1 : Enabled");
	XLMenu_AddItem(m_pSettingsMenu,MITEM_ROUTINE,currentname,ToggleController1);

	if (!EnableController2)
	swprintf(currentname,L"Controller 2 : Disabled");
	else 
	swprintf(currentname,L"Controller 2 : Enabled");
	XLMenu_AddItem(m_pSettingsMenu,MITEM_ROUTINE,currentname,ToggleController2);

	if (!EnableController3)
	swprintf(currentname,L"Controller 3 : Disabled");
	else 
	swprintf(currentname,L"Controller 3 : Enabled");
	XLMenu_AddItem(m_pSettingsMenu,MITEM_ROUTINE,currentname,ToggleController3);

	if (!EnableController4)
	swprintf(currentname,L"Controller 4 : Disabled");
	else 
	swprintf(currentname,L"Controller 4 : Enabled");
	XLMenu_AddItem(m_pSettingsMenu,MITEM_ROUTINE,currentname,ToggleController4);




	XLMenu_Activate(m_pSettingsMenu);

	while( XLMenu_CurMenu == m_pSettingsMenu)
	{
		DrawLogo(true);
		dwMenuCommand = getAllGamepadsCommand(&gamepad);
		XLMenu_Routine(dwMenuCommand);
		//freakdave - check for music playing
		if(gamepad.wPressedButtons&XINPUT_GAMEPAD_RIGHT_THUMB){
			bMusicPlaying = !bMusicPlaying;
			if(!bMusicPlaying){
				music.Pause();
			}else{
				music.Play();
			}
		}
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
	if(Sensitivity > 10) Sensitivity = 10;
	//freakdave
	//Don't forget to alter the floats in the brackets accordingly if you change XBOX_CONTROLLER_DEAD_ZONE
	}
	else
	{
    Sensitivity -= 1;
	if(Sensitivity < 1) Sensitivity = 1;
	}
	//XBOX_CONTROLLER_DEAD_ZONE = float(8600) - (float(8600) * (float(Sensitivity) / 100));
	
	XLMenu_CurRoutine = NULL;
	
	swprintf(currentname,L"Analog Sensitivity : %d",Sensitivity);
	XLMenu_SetItemText(&m_pSettingsMenu->items[currentItem], currentname);

	ConfigAppSave2();
}

void TogglePak()
{
WCHAR currentname[120];
currentItem = m_pSettingsMenu->curitem;
DefaultPak++;
if (DefaultPak > 2) DefaultPak = 0;

XLMenu_CurRoutine = NULL;
//Mempak/RumblePak/NoPak
if (DefaultPak == NoPak)
{
	swprintf(currentname,L"Default Pak : None");
}

if (DefaultPak == MemPak)
{
	swprintf(currentname,L"Default Pak : MemPak");
}

if (DefaultPak == RumblePak)
{
	swprintf(currentname,L"Default Pak : RumblePak");
}

XLMenu_SetItemText(&m_pSettingsMenu->items[currentItem], currentname);
ConfigAppSave2();

}


void ToggleController1()
{
    WCHAR currentname[120];
	currentItem = m_pSettingsMenu->curitem;
	EnableController1 =! EnableController1;
	
	XLMenu_CurRoutine = NULL;
	
  	if (!EnableController1)
	swprintf(currentname,L"Controller 1 : Disabled");
	else 
	swprintf(currentname,L"Controller 1 : Enabled");
	XLMenu_SetItemText(&m_pSettingsMenu->items[currentItem], currentname);

	ConfigAppSave2();
}

void ToggleController2()
{
    WCHAR currentname[120];
	currentItem = m_pSettingsMenu->curitem;
	EnableController2 =! EnableController2;
	
	XLMenu_CurRoutine = NULL;
	
  	if (!EnableController2)
	swprintf(currentname,L"Controller 2 : Disabled");
	else 
	swprintf(currentname,L"Controller 2 : Enabled");
	XLMenu_SetItemText(&m_pSettingsMenu->items[currentItem], currentname);

	ConfigAppSave2();
}

void ToggleController3()
{
    WCHAR currentname[120];
	currentItem = m_pSettingsMenu->curitem;
	//3rd Controller bug fixed
	EnableController3 =! EnableController3;
	
	XLMenu_CurRoutine = NULL;
	
  	if (!EnableController3)
	swprintf(currentname,L"Controller 3 : Disabled");
	else 
	swprintf(currentname,L"Controller 3 : Enabled");
	XLMenu_SetItemText(&m_pSettingsMenu->items[currentItem], currentname);

	ConfigAppSave2();
}

void ToggleController4()
{
    WCHAR currentname[120];
	currentItem = m_pSettingsMenu->curitem;
	EnableController4 =! EnableController4;
	
	XLMenu_CurRoutine = NULL;
	
  	if (!EnableController4)
	swprintf(currentname,L"Controller 4 : Disabled");
	else 
	swprintf(currentname,L"Controller 4 : Enabled");
	XLMenu_SetItemText(&m_pSettingsMenu->items[currentItem], currentname);

	ConfigAppSave2();
}

// controller config

extern void Drawcontrol();
extern int selectedelement;
extern bool changebutton;
int controller;
extern void ControllerReset();
XLMenu *m_pControllerMenu;

void ControllerMenu(void)
{
	DWORD dwMenuCommand = 0;

	XLMenu_CurRoutine = NULL;
	XLMenu_CurMenu = NULL;
	XLMenu_SetFont(&m_Font);

	m_pSettingsMenu = XLMenu_Init(60,80,4, MENU_LEFT|MENU_WRAP, NULL);

	m_pSettingsMenu->topcolor = 0x40254365;
	m_pSettingsMenu->bottomcolor = 0x40556486;
	m_pSettingsMenu->seltopcolor = 0x4055FF86;
	m_pSettingsMenu->selbotcolor = 0x4055FF86;
	m_pSettingsMenu->itemcolor = 0xFFFFFFFF;
	m_pSettingsMenu->parent = m_pMainMenu;

	XLMenu_SetTitle(m_pSettingsMenu,L"Choose Controller",0xFF8080FF);

	XLMenu_AddItem(m_pSettingsMenu,MITEM_ROUTINE,L"Controller 1",Control1);
	XLMenu_AddItem(m_pSettingsMenu,MITEM_ROUTINE,L"Controller 2",Control2);
	XLMenu_AddItem(m_pSettingsMenu,MITEM_ROUTINE,L"Controller 3",Control3);
	XLMenu_AddItem(m_pSettingsMenu,MITEM_ROUTINE,L"Controller 4",Control4);


	XLMenu_Activate(m_pSettingsMenu);

	while( XLMenu_CurMenu == m_pSettingsMenu)
	{
		DrawLogo(true);
		dwMenuCommand = getAllGamepadsCommand(&gamepad);
		XLMenu_Routine(dwMenuCommand);
		//freakdave - check for music playing
		if(gamepad.wPressedButtons&XINPUT_GAMEPAD_RIGHT_THUMB){
			bMusicPlaying = !bMusicPlaying;
			if(!bMusicPlaying){
				music.Pause();
			}else{
				music.Play();
			}
		}
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

	m_pControllerMenu = XLMenu_Init(60,80,1, MENU_LEFT|MENU_WRAP, NULL);

	// make the menu transparent
	m_pControllerMenu->bottomcolor = 0x00FFFFFF;
	m_pControllerMenu->itemcolor = 0x00FFFFFF;
	m_pControllerMenu->selbotcolor = 0x00FFFFFF;
	m_pControllerMenu->seltopcolor = 0x00FFFFFF;
	m_pControllerMenu->titlecolor = 0x00FFFFFF;
	m_pControllerMenu->topcolor = 0x00FFFFFF;
	m_pControllerMenu->parent = m_pSettingsMenu;

	XLMenu_SetTitle(m_pControllerMenu,L" ",0x00FFFFFF);


	XLMenu_Activate(m_pControllerMenu);

    bool bquit=false;
    selectedelement=0;

	while(!bquit)
	{
	DirectSoundDoWork();
	music.Process();
    Drawcontrol();
	XBInput_GetInput();

	if(g_Gamepads[controller].wPressedButtons	& XINPUT_GAMEPAD_DPAD_DOWN)
	{
	selectedelement++;
	if (selectedelement>17) selectedelement=0; }

	if(g_Gamepads[controller].wPressedButtons	& XINPUT_GAMEPAD_DPAD_UP)
	{
	selectedelement--;
	if (selectedelement<0) selectedelement=17; }

    if(g_Gamepads[controller].bPressedAnalogButtons[XINPUT_GAMEPAD_A]) changebutton=true;

    if(g_Gamepads[controller].bPressedAnalogButtons[XINPUT_GAMEPAD_X]) ControllerReset();

	for (int i=0;i<4;i++){
		if(g_Gamepads[i].bPressedAnalogButtons[XINPUT_GAMEPAD_B])	bquit=true;}

	g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
	if (changebutton) {
    Drawcontrol();
	g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
	ChangeControl();}
	}
	ConfigAppSave2();
	XLMenu_Routine(MENU_BACK);
	if (m_pControllerMenu)
		XLMenu_Delete(m_pControllerMenu);
}




/*
//Network stuff - freakdave
void DownloadsMenu()
{
	DWORD dwMenuCommand = 0;

	XLMenu_CurRoutine = NULL;
	XLMenu_CurMenu = NULL;
	XLMenu_SetFont(&m_Font);

//    WCHAR currentname[120];

	m_pSettingsMenu = XLMenu_Init(60,80,2, MENU_LEFT|MENU_WRAP, NULL);

	m_pSettingsMenu->topcolor = 0x40254365;
	m_pSettingsMenu->bottomcolor = 0x40556486;
	m_pSettingsMenu->seltopcolor = 0x4055FF86;
	m_pSettingsMenu->selbotcolor = 0x4055FF86;
	m_pSettingsMenu->itemcolor = 0xFFFFFFFF;
	m_pSettingsMenu->parent = m_pMainMenu;

	XLMenu_SetTitle(m_pSettingsMenu,L"Content Download",0xFF8080FF);

	if(IsXboxConnected()){
	XLMenu_AddItem(m_pSettingsMenu,MITEM_ROUTINE,L"Update Rice INI",UpdateRice);
	XLMenu_AddItem(m_pSettingsMenu,MITEM_ROUTINE,L"Update Roms INI",UpdateRoms);
	//XLMenu_AddItem(m_pSettingsMenu,MITEM_ROUTINE,L"Update Surreal64 XXX",UpdateSurreal);
	//XLMenu_AddItem(m_pSettingsMenu,MITEM_ROUTINE,"Download Skins,DownloadSkins);
	}else{
	XLMenu_AddItem(m_pSettingsMenu,MITEM_DISABLED,L"Update Rice INI",NULL);
	XLMenu_AddItem(m_pSettingsMenu,MITEM_DISABLED,L"Update Roms INI",NULL);
	}

	XLMenu_Activate(m_pSettingsMenu);

	while( XLMenu_CurMenu == m_pSettingsMenu)
	{
		DrawLogo(true);
		dwMenuCommand = getAllGamepadsCommand(&gamepad);
		XLMenu_Routine(dwMenuCommand);
		g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
	}

	if (m_pSettingsMenu)
		XLMenu_Delete(m_pSettingsMenu);

	ConfigAppSave2();

}



void UpdateRice(){
HANDLE hThread;
//BOOL bThreadInit = true;
DWORD dwThreadId;

OutputDebugString("Creating Thread\n");
hThread = CreateThread( NULL, 0, &ThreadUpdateIni, NULL, 0,&dwThreadId); 

	if (hThread == NULL) {
		OutputDebugString( "CreateThread failed!\n" );
	}
	else {
		OutputDebugString( "CreateThread succeeded!\n" );
		CloseHandle( hThread );
	}
}


DWORD WINAPI ThreadUpdateIni( LPVOID lpParam ) 
{ 
	UpdateIni(510);
	UpdateIni(531);
	UpdateIni(560);
    return 0; 
} 





void UpdateRoms(){
return;
}
*/