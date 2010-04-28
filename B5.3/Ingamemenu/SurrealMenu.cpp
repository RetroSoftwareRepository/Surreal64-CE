#include "../config.h"
#include "../ultraxle/IoSupport.h"
#include <xbapp.h>
#include <xbresource.h>
#include <xbfont.h>
#include "../Ingamemenu/xlmenu.h"
#include "Panel.h"

//freakdave
enum Emulators
{
	_1964,
	_Project64,
	_UltraHLE,
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


XBGAMEPAD gamepad;

extern CPanel m_RenderPanel;
extern CPanel m_BgPanel;
extern void TakeScreenshot();

XLMenu    *m_pMainMenu;
XLMenu    *m_pSettingsMenu;
XLMenu    *m_pControllerMenu;
extern CXBFont		m_Font;					// Font	for	text display

extern void DrawLogo();
extern void DestroyLogo();
static int currentItem;

void ExitToRomList(); 
void ExitToDash();
void ShutdownXbox();
void ToggleCredits();
void ToggleFlickerFilter(bool inc);
void VideoSettingsMenu();
void ToggleTextureFilter(bool inc);
void ToggleSoftDisplayFilter();
void ToggleFrameSkip();
void ToggleSensitivity(bool inc);
void ControllerSettingsMenu();
void TogglePak();
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
bool bloadstate[5];
bool bsavestate[5];

extern void CalculateEndCredits();
extern void DrawCredits();
extern int defilement;
extern void InitLogo();

//
void incflicker(){ ToggleFlickerFilter(true); }
void decflicker(){ ToggleFlickerFilter(false); }
void incTextureFilter(){ ToggleTextureFilter(true); }
void decTextureFilter(){ ToggleTextureFilter(false); }
void incSensitivity(){ ToggleSensitivity(true); }
void decSensitivity(){ ToggleSensitivity(false); }
//
extern bool showdebug;

enum Pak
{
NoPak,
MemPak,
RumblePak,
};
extern bool tookscreenshot;
void MainMenu(void)
{
	InitLogo();

	DWORD dwMenuCommand = 0;

	XLMenu_CurRoutine = NULL;
	XLMenu_CurMenu = NULL;
	XLMenu_SetFont(&m_Font);

	int x = 0;
	int y = 0;
	int nx = 640;
	int ny = 480;

	m_pMainMenu = XLMenu_Init(60,80,12,MENU_LEFT|MENU_WRAP,NULL);

	m_pMainMenu->topcolor = 0x40254365;
	m_pMainMenu->bottomcolor = 0x40556486;
	m_pMainMenu->seltopcolor = 0x4055FF86;
	m_pMainMenu->selbotcolor = 0x4055FF86;
	m_pMainMenu->itemcolor = 0xFFFFFFFF;
	m_pMainMenu->parent = NULL;

	while ((x != 320) || (y != 80) || (nx != 267) || (ny != 200))
	{
		g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER , D3DCOLOR_XRGB(0,0,0), 1.0f, 0 );
		m_BgPanel.Render(15,15);

		x = x + 32;
		y = y + 8;
		nx = nx - 38;
		ny = ny - 28;

		if (x >320)
			x = 320;

		if (y >80)
			y = 80;

		if (nx <267)
			nx = 267;

		if (ny <200)
			ny = 200;

		m_RenderPanel.Render(x ,y , nx , ny , false, 0);
		g_pd3dDevice->Present( NULL, NULL, NULL, NULL );

	}

	XLMenu_SetTitle(m_pMainMenu,L"Main Menu",0xFF8080FF);

	XLMenu_AddItem(m_pMainMenu,MITEM_ROUTINE,L"Load State",LoadStateMenu);
	XLMenu_AddItem(m_pMainMenu,MITEM_ROUTINE,L"Save State",SaveStateMenu);
	if (showdebug) 
	XLMenu_AddItem(m_pMainMenu,MITEM_ROUTINE,L"Show Debug info : on",ShowDebug);
	else
	XLMenu_AddItem(m_pMainMenu,MITEM_ROUTINE,L"Show Debug info : off",ShowDebug);
	XLMenu_AddItem(m_pMainMenu,MITEM_ROUTINE,L"Video Settings",VideoSettingsMenu);
	XLMenu_AddItem(m_pMainMenu,MITEM_ROUTINE,L"Controller Settings",ControllerSettingsMenu);
	XLMenu_AddItem(m_pMainMenu,MITEM_ROUTINE,L"Controller Edit",ControllerMenu);
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
        XLMenu_Activate(NULL);}
	}
	
	XLMenu_CurRoutine = NULL;
	XLMenu_CurMenu = NULL;

	if (m_pMainMenu)
		XLMenu_Delete(m_pMainMenu);

	DestroyLogo();

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
}

void ExitToRomList() 
{
	// Ez0n3 - launchdata isn't used anymore
	//LAUNCH_DATA ld;
	//ZeroMemory(&ld, sizeof(LAUNCH_DATA));
	
	//Ez0n3 - passing the zeroed launch data crashes surreal sometimes (usually 1964/UltraHLE - unknown reason). NULL seems to be more reliable with no adverse affects.
	//XLaunchNewImage("D:\\default.xbe", &ld);
	XLaunchNewImage("D:\\default.xbe", NULL);
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

void ResetRom(void)
{

// Ez0n3 - launchdata isn't used anymore - params are passed directly
/*
LaunchData ld;

// 1964 settings
ld.dw1964DynaMem	= dw1964DynaMem;
ld.dw1964PagingMem	= dw1964PagingMem;
			
// Pj64 settings
ld.dwPJ64DynaMem	= dwPJ64DynaMem;
ld.dwPJ64PagingMem	= dwPJ64PagingMem;

// common settings
ld.bUseLLERSP		= bUseLLERSP;

// Ez0n3 - reinstate max video mem
ld.dwMaxVideoMem	= dwMaxVideoMem;

LAUNCH_DATA _ld;
memcpy(_ld.Data, &ld, sizeof(LaunchData));

switch (preferedemu)
			{
				case _1964:
				{
					switch (videoplugin) {
				case 0 : XLaunchNewImage("D:\\1964-510.xbe", &_ld);
					     break;
				case 1 : XLaunchNewImage("D:\\1964-531.xbe", &_ld);
					     break;
				case 2 : XLaunchNewImage("D:\\1964-560.xbe", &_ld);
						 break;
				case 3 : XLaunchNewImage("D:\\1964-611.xbe", &_ld);
					break;}
					break;
				}
				case _Project64:
				{
					switch (videoplugin) {
				case 0 : XLaunchNewImage("D:\\Pj64-510.xbe", &_ld);
					     break;
				case 1 : XLaunchNewImage("D:\\Pj64-531.xbe", &_ld);
					     break;
				case 2 : XLaunchNewImage("D:\\Pj64-560.xbe", &_ld);
						 break;
				case 3 : XLaunchNewImage("D:\\Pj64-611.xbe", &_ld);
					break;}
					break;
				}
				case _UltraHLE:
				{
					XLaunchNewImage("D:\\UltraHLE.xbe", NULL);
				}
			}
*/

switch (preferedemu)
			{
				case _1964:
				{
					switch (videoplugin) {
				case 0 : XLaunchNewImage("D:\\1964-510.xbe", NULL);
					     break;
				case 1 : XLaunchNewImage("D:\\1964-531.xbe", NULL);
					     break;
				case 2 : XLaunchNewImage("D:\\1964-560.xbe", NULL);
						 break;
				case 3 : XLaunchNewImage("D:\\1964-611.xbe", NULL);
					break;}
					break;
				}
				case _Project64:
				{
					switch (videoplugin) {
				case 0 : XLaunchNewImage("D:\\Pj64-510.xbe", NULL);
					     break;
				case 1 : XLaunchNewImage("D:\\Pj64-531.xbe", NULL);
					     break;
				case 2 : XLaunchNewImage("D:\\Pj64-560.xbe", NULL);
						 break;
				case 3 : XLaunchNewImage("D:\\Pj64-611.xbe", NULL);
					break;}
					break;
				}
				case _UltraHLE:
				{
					XLaunchNewImage("D:\\UltraHLE.xbe", NULL);
				}
			}

}


void LoadStateMenu(void)
{
	DWORD dwMenuCommand = 0;

	XLMenu_CurRoutine = NULL;
	XLMenu_CurMenu = NULL;
	XLMenu_SetFont(&m_Font);

	m_pSettingsMenu = XLMenu_Init(60,80,5, MENU_LEFT|MENU_WRAP, NULL);

	m_pSettingsMenu->topcolor = 0x40254365;
	m_pSettingsMenu->bottomcolor = 0x40556486;
	m_pSettingsMenu->seltopcolor = 0x4055FF86;
	m_pSettingsMenu->selbotcolor = 0x4055FF86;
	m_pSettingsMenu->itemcolor = 0xFFFFFFFF;
	m_pSettingsMenu->parent = m_pMainMenu;

	XLMenu_SetTitle(m_pSettingsMenu,L"Load State",0xFF8080FF);

  	XLMenu_AddItem(m_pSettingsMenu,MITEM_ROUTINE,L"Load State 1",LoadState1);
  	XLMenu_AddItem(m_pSettingsMenu,MITEM_ROUTINE,L"Load State 2",LoadState2);
  	XLMenu_AddItem(m_pSettingsMenu,MITEM_ROUTINE,L"Load State 3",LoadState3);
  	XLMenu_AddItem(m_pSettingsMenu,MITEM_ROUTINE,L"Load State 4",LoadState4);
  	XLMenu_AddItem(m_pSettingsMenu,MITEM_ROUTINE,L"Load State 5",LoadState5);


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

void LoadState1()
{
bloadstate[0]=true;
XLMenu_CurRoutine = NULL;
XLMenu_CurMenu = NULL;
}

void LoadState2()
{
bloadstate[1]=true;
XLMenu_CurRoutine = NULL;
XLMenu_CurMenu = NULL;
}

void LoadState3()
{
bloadstate[2]=true;
XLMenu_CurRoutine = NULL;
XLMenu_CurMenu = NULL;
}

void LoadState4()
{
bloadstate[3]=true;
XLMenu_CurRoutine = NULL;
XLMenu_CurMenu = NULL;
}

void LoadState5()
{
bloadstate[4]=true;
XLMenu_CurRoutine = NULL;
XLMenu_CurMenu = NULL;
}

void SaveStateMenu(void)
{
	DWORD dwMenuCommand = 0;

	XLMenu_CurRoutine = NULL;
	XLMenu_CurMenu = NULL;
	XLMenu_SetFont(&m_Font);


	m_pSettingsMenu = XLMenu_Init(60,80,5, MENU_LEFT|MENU_WRAP, NULL);

	m_pSettingsMenu->topcolor = 0x40254365;
	m_pSettingsMenu->bottomcolor = 0x40556486;
	m_pSettingsMenu->seltopcolor = 0x4055FF86;
	m_pSettingsMenu->selbotcolor = 0x4055FF86;
	m_pSettingsMenu->itemcolor = 0xFFFFFFFF;
	m_pSettingsMenu->parent = m_pMainMenu;

	XLMenu_SetTitle(m_pSettingsMenu,L"Save State",0xFF8080FF);

  	XLMenu_AddItem(m_pSettingsMenu,MITEM_ROUTINE,L"Save State 1",SaveState1);
  	XLMenu_AddItem(m_pSettingsMenu,MITEM_ROUTINE,L"Save State 2",SaveState2);
  	XLMenu_AddItem(m_pSettingsMenu,MITEM_ROUTINE,L"Save State 3",SaveState3);
  	XLMenu_AddItem(m_pSettingsMenu,MITEM_ROUTINE,L"Save State 4",SaveState4);
  	XLMenu_AddItem(m_pSettingsMenu,MITEM_ROUTINE,L"Save State 5",SaveState5);


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

void SaveState1()
{
bsavestate[0]=true;
XLMenu_CurRoutine = NULL;
XLMenu_CurMenu = NULL;
}

void SaveState2()
{
bsavestate[1]=true;
XLMenu_CurRoutine = NULL;
XLMenu_CurMenu = NULL;
}

void SaveState3()
{
bsavestate[2]=true;
XLMenu_CurRoutine = NULL;
XLMenu_CurMenu = NULL;
}

void SaveState4()
{
bsavestate[3]=true;
XLMenu_CurRoutine = NULL;
XLMenu_CurMenu = NULL;
}

void SaveState5()
{
bsavestate[4]=true;
XLMenu_CurRoutine = NULL;
XLMenu_CurMenu = NULL;
}

void VideoSettingsMenu(void)
{
	DWORD dwMenuCommand = 0;

	XLMenu_CurRoutine = NULL;
	XLMenu_CurMenu = NULL;
	XLMenu_SetFont(&m_Font);

    WCHAR currentname[120];

	m_pSettingsMenu = XLMenu_Init(60,80,5, MENU_LEFT|MENU_WRAP, NULL);

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

	//FrameSkip
	if (!FrameSkip)
	swprintf(currentname,L"Skip Frames : No");
	else 
	swprintf(currentname,L"Skip Frames : Yes");
	XLMenu_AddItem(m_pSettingsMenu,MITEM_ROUTINE,currentname,ToggleFrameSkip);

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


void ControllerSettingsMenu()
{
	DWORD dwMenuCommand = 0;

	XLMenu_CurRoutine = NULL;
	XLMenu_CurMenu = NULL;
	XLMenu_SetFont(&m_Font);

    WCHAR currentname[120];

	m_pSettingsMenu = XLMenu_Init(60,80,2/*6*/, MENU_LEFT|MENU_WRAP, NULL);

	m_pSettingsMenu->topcolor = 0x40254365;
	m_pSettingsMenu->bottomcolor = 0x40556486;
	m_pSettingsMenu->seltopcolor = 0x4055FF86;
	m_pSettingsMenu->selbotcolor = 0x4055FF86;
	m_pSettingsMenu->itemcolor = 0xFFFFFFFF;
	m_pSettingsMenu->parent = m_pMainMenu;

	XLMenu_SetTitle(m_pSettingsMenu,L"Controller Settings",0xFF8080FF);
	
	//Sensitivity
	swprintf(currentname,L"Analog Sensitivity : %d",Sensitivity);
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
	if(Sensitivity > 10) Sensitivity = 10;
	//freakdave
	//Don't forget to alter the floats in the brackets accordingly if you change XBOX_CONTROLLER_DEAD_ZONE
	}
	else
	{
    Sensitivity -= 1;
	if(Sensitivity < 1) Sensitivity = 1;
	}
	//XBOX_CONTROLLER_DEAD_ZONE = float(6500) * ((float(Sensitivity) - 25) / 100);
	
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


// controller config

extern void Drawcontrol();
extern int selectedelement;
extern bool changebutton;
int controller;
extern void ControllerReset();
extern "C" void _INPUT_LoadButtonMap(int *cfgData);
extern int ControllerConfig[72];

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
	_INPUT_LoadButtonMap(ControllerConfig);
	XLMenu_Routine(MENU_BACK);
	if (m_pControllerMenu)
		XLMenu_Delete(m_pControllerMenu);
}