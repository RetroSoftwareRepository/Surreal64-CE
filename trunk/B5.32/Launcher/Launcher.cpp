 /**
 * Surreal 64 Launcher (C) 2003
 * 
 * This program is free software; you can redistribute it and/or modify it under 
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option) any later
 * version. This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details. You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA. To contact the
 * authors: email: buttza@hotmail.com, lantus@lantus-x.com
 */

#include "Launcher.h"
#include "IniFile.h"
#include "BoxArtTable.h"
#include "IoSupport.h"
#include "RomList.h"
#include "config.h"
#include <xbapp.h>
#include <xbresource.h>
#include <xbfont.h>
#include "../ingamemenu/xlmenu.h"
#include "128meg.h"
#include "musicmanager.h"
#include "Panel.h"


float fGameSelect;
float fCursorPos;
float fMaxCount;
float m_fFrameTime;			// amount of time per frame

int	  iGameSelect;
int	  iCursorPos;
int	  iNumGames;
int	  LastPos;
int	  m_iMaxWindowList;
int	  m_iWindowMiddle;

const int GAMESEL_MaxWindowList	= 12;		 
const int GAMESEL_WindowMiddle = 6;	
const float	GAMESEL_cfDeadZone = 0.3f;
const float	GAMESEL_cfMaxThresh	= 0.93f;
const float	GAMESEL_cfMaxPossible =	1.0f;
const float	GAMESEL_cfRectifyScale = GAMESEL_cfMaxPossible/(GAMESEL_cfMaxPossible-GAMESEL_cfDeadZone);
const float	GAMESEL_cfSpeedBandFastest = 2.0f;	// seconds (don`t worry	for	PAL	NTSC dif xbapp handles that)
const float	GAMESEL_cfFastestScrollMult	= 6.0f;
const float	GAMESEL_cfSpeedBandMedium =	1.0f;	// if the pad is held at max for given seconds list	will move faster
const float	GAMESEL_cfMediumScrollMult = 5.0f;
const float	GAMESEL_cfSpeedBandLowest =	0.5f;
const float	GAMESEL_cfLowestScrollMult = 2.0f;	 

extern "C" void Enable128MegCaching();
extern void InitLogo();
extern void DrawLogo(bool Menu);
extern int romcounter;
int actualrom = 0;
extern void LaunchMenu();
bool g_bQuit = false;
extern void MainMenu(void);
CXBFont		m_Font;					// Font	for	text display
CXBFont		m_MSFont;					// Font	for	buttons
extern char romCRC[32];
extern char romname[256];
void ConstructCredits();
CMusicManager  music;
LPDIRECTSOUND8  m_pDSound;          // DirectSound object
bool bMusicPlaying;
extern char skinname[32];
extern MM_SONG g_aGameSoundtrack[1][1];


// Ez0n3 - needed for user pref and CMD launch
extern int m_emulator;
extern int preferedemu;
extern int videoplugin;
extern int iAudioPlugin;
extern void Launch();
extern int ConfigAppLoad3();
extern bool has128ram;

extern void LaunchHideScreens(); // launch rom no screens

WIN32_FIND_DATA wfd;
HANDLE hFind;



class CXBoxSample :	public CXBApplication
{

public:
	HRESULT	Initialize();		// Initialize the sample
	HRESULT	Render();			// Render the scene
	HRESULT	FrameMove();		// Perform per-frame updates
	CXBoxSample();
	CXBPackedResource		m_xprResource;

private:
	void MoveCursor();
	CIoSupport m_IOSupport;
};

VOID __cdecl main()
{
	CXBoxSample	xbApp;
	if(	FAILED(	xbApp.Create() ) )
		return;
	xbApp.Run();

	fGameSelect	= 0.0f;
	iGameSelect	= 0;
	fCursorPos = 0.0f;
	iCursorPos = 0;
	fMaxCount =	0.0f;
}

CXBoxSample::CXBoxSample() 
:CXBApplication()
{

}
extern bool onhd;
HRESULT	CXBoxSample::Initialize()
{
	// Ez0n3 - determine if the current phys ram is greater than 100MB
	if ( PhysRam128() ) has128ram = true;
	else has128ram = false;

	Enable128MegCaching();
	XSetFileCacheSize(8 * 1024 * 1024);

	// initialise direct 3d
	if (!g_d3d.Create())
		return S_OK;

	// mount drives
	g_IOSupport.Mount("A:","cdrom0");
	g_IOSupport.Mount("E:","Harddisk0\\Partition1");
	g_IOSupport.Mount("Z:","Harddisk0\\Partition2");
	g_IOSupport.Mount("F:","Harddisk0\\Partition6");
	g_IOSupport.Mount("G:","Harddisk0\\Partition7");
	
	
	//This method is more reliable (CD/DVD check) - freakdave
	//The size of a fixed sector on the hard disk is 512 bytes. 
	//The size of a sector on a CD or DVD drive is 2048 bytes.
	FILE *fp;
	if(XGetDiskSectorSize("D:\\") == 2048){
		onhd = FALSE;
		//fp=fopen("T:\\log.txt","wb");
		//fprintf(fp,"Running from CD/DVD\n");
	}else{
		onhd = TRUE;
		//fp=fopen("D:\\log.txt","wb");
		//fprintf(fp,"Running from HD\n");
	}
	//fprintf(fp,"Disc sector size : %d bytes\n",XGetDiskSectorSize("D:\\"));
	//fclose(fp);

	ConfigAppLoad(); // GogoAckman
	ConfigAppSave(); // freakdave - If there's no ini in the directory -> Crash !

	char fontname[256];
	sprintf(fontname,"D:\\Skins\\%s\\Font.xpr",skinname);
	if(	FAILED(	m_Font.Create(fontname) ) )
		return XBAPPERR_MEDIANOTFOUND;
	sprintf(fontname,"D:\\Skins\\%s\\MsFont.xpr",skinname);
	if(	FAILED(	m_MSFont.Create(fontname) ) )
		return XBAPPERR_MEDIANOTFOUND;
	sprintf(fontname,"D:\\Skins\\%s\\main.wma",skinname);
	g_aGameSoundtrack[0][0].strFilename = fontname;

	//Make sure Surreal10.ini does not exist
	hFind = FindFirstFile("T:\\Surreal10.ini", &wfd);
	if (hFind == INVALID_HANDLE_VALUE)
		{
		OutputDebugString("Could not find Surreal10.ini");
		}
	else
		if (DeleteFile("T:\\Surreal10.ini") == NULL)
		{
		OutputDebugString("Could not delete Surreal10.ini");
		}
		FindClose(hFind);


	// load the ini file
	//FIXME: needs testing
	if (!onhd)
	{
	if (!g_iniFile.Load("T:\\Surreal.ini"))
		g_iniFile.Load("D:\\Surreal.ini");
	}
	else g_iniFile.Load("D:\\Surreal.ini");


	InitLogo();

	
	// Ez0n3 - change this to prevent it from crashing if the media folder is empry
	/*
	// build the box art filename table
	g_boxArtTable.Build();

	if (onhd) {
    CreateDirectory("D:\\ini", NULL);
	CreateDirectory("D:\\ini\\games", NULL);}
	else {
    CreateDirectory("T:\\ini", NULL);
	CreateDirectory("T:\\ini\\games", NULL);
	}
	*/

	if (onhd) {
		CreateDirectory("D:\\ini", NULL);
		CreateDirectory("D:\\ini\\games", NULL);
		
		CreateDirectory("D:\\Media", NULL);
		CreateDirectory("D:\\Media\\boxart", NULL);
		CreateDirectory("D:\\Media\\ui", NULL);
		CreateDirectory("D:\\Media\\ui\\controller", NULL);
	} else {
		CreateDirectory("T:\\ini", NULL);
		CreateDirectory("T:\\ini\\games", NULL);
		
		CreateDirectory("T:\\Media", NULL);
		CreateDirectory("T:\\Media\\boxart", NULL);
		CreateDirectory("T:\\Media\\ui", NULL);
		CreateDirectory("T:\\Media\\ui\\controller", NULL);
	}
	g_boxArtTable.Build();
	

	// load the rom list if it isn't already loaded
	// we do it here so that the user can see the rom list is loading
	if (!g_romList.IsLoaded())
	g_romList.Load();

	iNumGames =	romcounter;

	if (iNumGames <	GAMESEL_MaxWindowList)
	{
		m_iMaxWindowList = iNumGames;
		m_iWindowMiddle	 = iNumGames/2;
	}
	else
	{
		m_iMaxWindowList = GAMESEL_MaxWindowList;
		m_iWindowMiddle	 = GAMESEL_WindowMiddle;
	}
	if ((onhd)&&((fp=fopen("D:\\Credits.txt","r+")) == NULL)) ConstructCredits();
	else if((!onhd)&&((fp=fopen("T:\\Credits.txt","r+")) == NULL)) ConstructCredits();
	else fclose(fp);

	if(FAILED(DirectSoundCreate(NULL, &m_pDSound, NULL)))
	return E_FAIL;

    music.Initialize();
	bMusicPlaying = true;
	music.Play();

	
	//Ez0n3 - Xport - NATEDOGG - CMD launch code
	DWORD launchType;
	CUSTOM_LAUNCH_DATA g_launchData;
	memset( &g_launchData, 0, sizeof( CUSTOM_LAUNCH_DATA ) );

	if ( ( XGetLaunchInfo( &launchType,(PLAUNCH_DATA)&g_launchData) == ERROR_SUCCESS ) ) {

		const char* szCmdLine;
		if  ( ( launchType == LDT_TITLE ) && ( g_launchData.magic == CUSTOM_LAUNCH_MAGIC ) ) {
			szCmdLine = ((PCUSTOM_LAUNCH_DATA)(&g_launchData))->szFilename;
		}
		else if ( launchType == LDT_FROM_DEBUGGER_CMDLINE ) {
			szCmdLine = ((PLD_FROM_DEBUGGER_CMDLINE)(&g_launchData))->szCmdLine;
		}
		else {
		}

		if (szCmdLine && strlen(szCmdLine)) {
		
			char cmdname[300];
			sprintf(cmdname,szCmdLine);

			if ( strchr(cmdname, '\\') ) {
			
				/* remove cmd rom path */
				int cmdcounterbackslash;
				// find last backslash
				for (int i=0;i<300;i++){
					if (cmdname[i]=='\\')  cmdcounterbackslash=i;
					if (cmdname[i] == '\0') break;
				}
				// remove it
				for (int i=0;i<300;i++){
					for (int j= cmdcounterbackslash+1;j<300;j++){
						cmdname[i]=cmdname[j];
						i++;
						if (cmdname[j] == '\0') break;
					}
					cmdname[i+1] = '\0';
					break;
				}
			
			}
			
			bool match = false;
			for (int ii = 0, n = g_romList.GetRomListSize(); ii < n; ii++) {
				Rom* rom = g_romList.GetRomAt(ii);

				char zipname[120];
				sprintf(zipname,rom->GetFileName().c_str());

				/* remove rom path */
				int counterbackslash;
				// find last backslash
				for (int i=0;i<120;i++){
					if (zipname[i]=='\\')  counterbackslash=i;
					if (zipname[i] == '\0') break;
				}
				// remove it
				for (int i=0;i<120;i++){
					for (int j= counterbackslash+1;j<120;j++){
						zipname[i]=zipname[j];
						i++;
						if (zipname[j] == '\0') break;
					}
					zipname[i+1] = '\0';
					break;
				}

				if (strcmp(zipname, cmdname) == 0) {
					actualrom = ii;
					match = true;
					
					ConfigAppLoad3(); // load user pref for rom
					//Sleep(100); //too fast for ini load?
					
					WCHAR m_currentname[120];
					char nameofgame[120];
					bool caps = true;
					bool end = false;
					sprintf(nameofgame,rom->GetProperName().c_str());

					// Here we "clean" our name
					for (int j=0;j<120;j++){
						if (j==0){
							if (!(nameofgame[j]>=97) && (nameofgame[j]<=122))
								if (!(nameofgame[j]>=65) && (nameofgame[j]<=90))
									if (!(nameofgame[j]>=48) && (nameofgame[j]<=57))
							{
								sprintf(nameofgame,"Unknown");
								break;
							}
						}
						if (caps) {
							if ((nameofgame[j]>=97) && (nameofgame[j]<=122)) {
								nameofgame[j] = nameofgame[j]-32;
							}
							caps = false;
						}
						else {
							if ((nameofgame[j]>=65) && (nameofgame[j]<=90)) {
								nameofgame[j] = nameofgame[j]+32;
							}
						}
						if (nameofgame[j]==' ') caps=true;
						if ((nameofgame[j]=='(')|| (nameofgame[j]=='[')) end=true;
						if (end) 
						{nameofgame[j]='\0';break;}
					}
					
					swprintf(m_currentname, L"%S",nameofgame);
					sprintf(romname,"%S",m_currentname);
					sprintf(romCRC,"%x",rom->m_dwCrc1);

					// set in ini now
					/*s
					char plugin[32];
					if ((onhd)&&((fp=fopen("D:\\default-plugin.cfg","r+")) != NULL)) {
						fgets(plugin,32,fp);
						fclose(fp);
					}
					else if((!onhd)&&((fp=fopen("T:\\default-plugin.cfg","r+")) != NULL)) {
						fgets(plugin,32,fp);
						fclose(fp);
					}
					//else fclose(fp);
					*/

					//ConfigAppLoad3(); // load user pref for rom
					//Sleep(100); //too fast for ini load?
					
					// set in ini now
					//if (videoplugin == _VideoPluginMissing && strcmp(plugin, "") != 0 && atoi(plugin) < _VideoPluginMissing) videoplugin = atoi(plugin);

					// just to be safe
					switch (videoplugin) {
						case _VideoPluginRice510 : videoplugin = _VideoPluginRice510; //5.1.0
							break;
						case _VideoPluginRice531 : videoplugin = _VideoPluginRice531; //5.3.1
							break;
						case _VideoPluginRice560 : videoplugin = _VideoPluginRice560; //5.6.0
							break;
						case _VideoPluginRice612 : videoplugin = _VideoPluginRice612; //6.1.2
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

					ConfigAppSave2(); // save the rom ini
					Launch();
					
					break;
				}
			}
			
			if (match == false) {
				// launch data isn't used anymore for roms
				//LAUNCH_DATA ld;
				//ZeroMemory(&ld, sizeof(LAUNCH_DATA));
				//XLaunchNewImage("D:\\default.xbe", &ld);
				XLaunchNewImage("D:\\default.xbe", NULL);
			}
		}
	}

return S_OK;

}

HRESULT	CXBoxSample::FrameMove()
{ 
	static bool bDpadUp = false;
	static bool bDpadDown = false;
	WORD wButtons;

		MoveCursor();

			if ((bDpadUp || bDpadDown) && XBUtil_Timer(TIMER_GETAPPTIME) > 0.5f) {
				XBUtil_Timer(TIMER_STOP);
				wButtons = m_DefaultGamepad.wButtons;
			} else {
				wButtons = m_DefaultGamepad.wPressedButtons;
			}

			if(/*m_DefaultGamepad.wPressedButtons*/wButtons	& XINPUT_GAMEPAD_DPAD_DOWN)
			{
				if (wButtons == m_DefaultGamepad.wPressedButtons) XBUtil_Timer(TIMER_RESET);

				bDpadUp = false;
				bDpadDown = true;
				// default don`t clamp cursor
				bool bClampCursor =	FALSE;

				fCursorPos ++;

				if(	fCursorPos > m_iWindowMiddle )
				{
					// clamp cursor	position
					bClampCursor = TRUE;

					// advance gameselect
					if(fGameSelect == 0) fGameSelect +=	(fCursorPos	- m_iWindowMiddle);
					else fGameSelect ++;

					// clamp game window range (high)
					// clamp game window range (high)
					if((fGameSelect	+ m_iMaxWindowList)	> iNumGames)
					{

						// clamp to	end
						fGameSelect	= iNumGames	- m_iMaxWindowList;

						// advance cursor pos after	all!
						bClampCursor = FALSE;

						// clamp cursor	to end
						if((fGameSelect	+ fCursorPos) >= iNumGames)
							fCursorPos = m_iMaxWindowList-1;
					}
				}

				// check for cursor	clamp
				if(	bClampCursor )
					fCursorPos = m_iWindowMiddle;	


			}
			else if( /*m_DefaultGamepad.wPressedButtons*/wButtons &	XINPUT_GAMEPAD_DPAD_UP)
			{
				if (wButtons == m_DefaultGamepad.wPressedButtons) XBUtil_Timer(TIMER_RESET);

				bDpadUp = true;
				bDpadDown = false;

				// default don`t clamp cursor
				bool bClampCursor =	FALSE;

				fCursorPos --;
				if(	fCursorPos < m_iWindowMiddle )
				{
					// clamp cursor	position
					bClampCursor = TRUE;

					// backup window pos
					fGameSelect	--;

					// clamp game window range (low)
					if(fGameSelect < 0)
					{
						// clamp to	start
						fGameSelect	= 0;

						// backup cursor pos after all!
						bClampCursor = FALSE;

						// clamp cursor	to end
						if(	fCursorPos < 0 )
							fCursorPos = 0;
					}
				}

				// check for cursor	clamp
				if(	bClampCursor )
					fCursorPos = m_iWindowMiddle;	


			}
		if(m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_A])
		{
			if(!HideLaunchScreens)
			{
				LaunchMenu();
			}
			else
			{
				LaunchHideScreens();
			}
		}
	
		if(m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_X])
		{
			MainMenu();
		}

		//freakdave
		if(m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_RIGHT_THUMB){
			bMusicPlaying = !bMusicPlaying;
			if(!bMusicPlaying){
				music.Pause();
			}else{
				music.Play();
			}
		}

		if(m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_Y])
		{
            g_romList.Refresh();
			iNumGames =	romcounter;

			if (iNumGames <	GAMESEL_MaxWindowList)
			{
				m_iMaxWindowList = iNumGames;
				m_iWindowMiddle	 = iNumGames/2;
			}
			else
			{
				m_iMaxWindowList = GAMESEL_MaxWindowList;
				m_iWindowMiddle	 = GAMESEL_WindowMiddle;
			}
		}


			return S_OK;

}
bool compatible[3];
HRESULT	CXBoxSample::Render()
{ 
		g_d3d.BeginRender();

		WCHAR	   m_currentname[120];	// Game	name written to	a string

	    int iGameidx;
	    int	iTempGameSel;

	    // printf Gamelist
		DrawLogo(false);

		m_Font.Begin();

		// draw	game list entries

		iGameSelect	= fGameSelect;
		iCursorPos = fCursorPos;
		iTempGameSel = iGameSelect;
		actualrom = iGameSelect+iCursorPos;

		if (iNumGames == 0)	
			m_Font.DrawText(320, 202, 0xFFEEEEEE,	L"No Roms Found\nPlease edit surreal.ini and refresh by pressing \403", XBFONT_CENTER_X);	 
		for	(iGameidx=0; iGameidx<m_iMaxWindowList;	iGameidx++)
		{
		Rom *rom = g_romList.GetRomAt(iTempGameSel++);
		char nameofgame[120];
		bool caps = true;
		bool end = false;
		sprintf(nameofgame,rom->GetProperName().c_str());
// Here we "clean" our name
		for (int j=0;j<120;j++){
			if (j==0){
				if (!(nameofgame[j]>=97) && (nameofgame[j]<=122))
					if (!(nameofgame[j]>=65) && (nameofgame[j]<=90))
						if (!(nameofgame[j]>=48) && (nameofgame[j]<=57))
				{
                    sprintf(nameofgame,"Unknown");
					break;
				}
			}
			if (caps) {
				if ((nameofgame[j]>=97) && (nameofgame[j]<=122)) {
				nameofgame[j] = nameofgame[j]-32;
				}
                caps = false;
			}
			else {
				if ((nameofgame[j]>=65) && (nameofgame[j]<=90)) {
				nameofgame[j] = nameofgame[j]+32;
				}
			}
			if (nameofgame[j]==' ') caps=true;
			if ((nameofgame[j]=='(')|| (nameofgame[j]=='[')) end=true;
			if (end) 
			{nameofgame[j]='\0';break;}
		}
		
		swprintf( m_currentname, L"%S",nameofgame );

			if (iGameidx==iCursorPos){
                m_Font.DrawText( 45, 45+(20*iGameidx), 0xFFFF7F7f, m_currentname, XBFONT_TRUNCATED,	530);
				sprintf(romCRC,"%x",rom->m_dwCrc1);
				sprintf(romname,"%S",m_currentname);
				for (int i=0;i<3;i++){
					compatible[i]=rom->GetIniEntry()->pbEmuSupported[i];}
			}
			else
			{
				m_Font.DrawText( 45, 45+(20*iGameidx), 0xFFEEEEEE, m_currentname, XBFONT_TRUNCATED,	530);
			}

		}

		m_Font.End();

     	g_d3d.EndRender();

			return S_OK;


}

// check for move cursor and move accordingly (with	clamp etc)
void CXBoxSample::MoveCursor()
{

	// get right trigger state (convert	to float & scale to	0.0f - 1.0f)
	float fWindowVelocity =	(float)(m_DefaultGamepad.bAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER])/256.0f;

	// subract in left trigger state (convert to float)
	fWindowVelocity	-= (float)(m_DefaultGamepad.bAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER])/256.0f;

	// get left stick state
	fWindowVelocity  +=	(float)(-m_DefaultGamepad.sThumbLY)/32256.0f;

	//setup	static locals
	static float fFastestThresh	= m_d3dpp.FullScreen_RefreshRateInHz*GAMESEL_cfSpeedBandFastest;
	static float fMediumThresh = m_d3dpp.FullScreen_RefreshRateInHz*GAMESEL_cfSpeedBandMedium;
	static float fLowestThresh = m_d3dpp.FullScreen_RefreshRateInHz*GAMESEL_cfSpeedBandLowest;

	// do dead zone	and	gamma curve
	if(( fWindowVelocity  >	-GAMESEL_cfDeadZone	) && ( fWindowVelocity < GAMESEL_cfDeadZone	))
	{
		// zero	dead zone (for spring slack	& noise)
		fWindowVelocity	= 0.0f;
		fMaxCount =	0.0f;
	}
	else
	{
		//start	at 0.0f	after deadzone
		if(	fWindowVelocity	< 0.0f )
			//adjust for deadzone
			fWindowVelocity	+= GAMESEL_cfDeadZone;
		else
			//adjust for deadzone
			fWindowVelocity	-= GAMESEL_cfDeadZone;

		//rescale to correct range
		fWindowVelocity	*= GAMESEL_cfRectifyScale;

		//increase max held	count (for speed up	bands)
		if(	fabs(fWindowVelocity) >	GAMESEL_cfMaxThresh	)
		{
			//increase count by	frame time and clamp count (if necc.)
			if(fMaxCount +=	m_fFrameTime)
				fMaxCount =	fFastestThresh;
		}

		//check	for	speed scale	banding
		if(	fMaxCount >	fFastestThresh)
			fWindowVelocity	*= GAMESEL_cfFastestScrollMult;
		else if( fMaxCount > fMediumThresh)
			fWindowVelocity	*= GAMESEL_cfMediumScrollMult;
		else if( fMaxCount > fLowestThresh)
			fWindowVelocity	*= GAMESEL_cfLowestScrollMult;
	}

	// default don`t clamp cursor
	bool bClampCursor =	FALSE;

	// check if	triggers pressed and move window
	fCursorPos += fWindowVelocity;
	if(	fWindowVelocity	> 0.0f )
	{
		if(	fCursorPos > m_iWindowMiddle )
		{
			// clamp cursor	position
			bClampCursor = TRUE;

			// advance gameselect
			fGameSelect	+= fWindowVelocity;

			// clamp game window range (high)
			if((fGameSelect	+ m_iMaxWindowList)	> iNumGames)
			{
				// clamp to	end
				fGameSelect	= iNumGames	- m_iMaxWindowList;

				// advance cursor pos after	all!
				bClampCursor = FALSE;

				// clamp cursor	to end
				if(fCursorPos >	m_iMaxWindowList -1)
					fCursorPos = m_iMaxWindowList-1;
			}
		}

	}
	else if( fWindowVelocity < 0.0f	)	// ok to do	this! because of deadzone clamp
	{
		if(	fCursorPos < m_iWindowMiddle )
		{
			// clamp cursor	position
			bClampCursor = TRUE;

			// backup window pos
			fGameSelect	+= fWindowVelocity;

			// clamp game window range (low)
			if(fGameSelect < 0)
			{
				// clamp to	start
				fGameSelect	= 0;

				// backup cursor pos after all!
				bClampCursor = FALSE;

				// clamp cursor	to end
				if(	fCursorPos < 0 )
					fCursorPos = 0;
			}
		}
	}

	// check for cursor	clamp
	if(	bClampCursor )
		fCursorPos = m_iWindowMiddle;	

}

extern CPanel m_PgPanel;
extern LPDIRECT3DTEXTURE8 bgTexture;

void ReloadSkin() {
// fonts
char fontname[256];
m_MSFont.Destroy();
m_Font.Destroy();
sprintf(fontname,"D:\\Skins\\%s\\Font.xpr",skinname);
m_Font.Create(fontname);
sprintf(fontname,"D:\\Skins\\%s\\MsFont.xpr",skinname);
m_MSFont.Create(fontname);
// background
m_PgPanel.Destroy();
bgTexture->Release();
InitLogo();
// music
music.Stop();
sprintf(fontname,"D:\\Skins\\%s\\main.wma",skinname);
g_aGameSoundtrack[0][0].strFilename = fontname;
music.Initialize();
bMusicPlaying = true;
music.Play();
}


// Ez0n3 - update the credits
void ConstructCredits()
{
//write out the Credits
FILE *fp;
if (onhd) fp=fopen("D:\\Credits.txt","w+");
else fp=fopen("T:\\Credits.txt","w+");
fprintf(fp,"Surreal64 XXX CE 'Community Edition' Modification\n");
fprintf(fp,"\n");
fprintf(fp,"Authors:\n");
fprintf(fp,"freakdave, Ez0n3, weinerschnitzel\n");
fprintf(fp,"\n");
fprintf(fp,"Additional Rice Video Fixes:\n");
fprintf(fp,"death2droid, microdev\n");
fprintf(fp,"\n");
fprintf(fp,"Testing / Ideas / Support:\n");
fprintf(fp,"Neo369, Bomb Bloke, greatant, edwardar, Darknior\n");
fprintf(fp,"\n");
fprintf(fp,"Skins by:\n");
fprintf(fp,"Darknior\n");
fprintf(fp,"NeverWill\n");
fprintf(fp,"\n");
fprintf(fp,"Visit us at:\n");
fprintf(fp,"http://sourceforge.net/projects/surreal64xxxce\n");
fprintf(fp,"http://forums.xbox-scene.com\n");
fprintf(fp,"#surreal64ce on EFNET\n");
fprintf(fp,"\n\n");
fprintf(fp,"Surreal64 XXX Modification\n");
fprintf(fp,"\n");
fprintf(fp,"Authors:\n");
fprintf(fp,"Team XXX\n");
fprintf(fp,"GogoAckman, freakdave, Artik\n");
fprintf(fp,"http://www.logic-sunrise.com/forums\n");
fprintf(fp,"http://freakdave.xbox-scene.com\n");
fprintf(fp,"\n");
fprintf(fp,"Skin by:\n");
fprintf(fp,"NeverWill\n");
fprintf(fp,"\n");
fprintf(fp,"Music found on OCRemix:\n");
fprintf(fp,"http://www.ocremix.org\n");
fprintf(fp,"\n");
fprintf(fp,"Beta Tester:\n");
fprintf(fp,"Dukedoom\n");
fprintf(fp,"\n\n");
fprintf(fp,"Shouts go out to:\n");
fprintf(fp,"oDD and Lantus for bringing N64 emulation to the XBOX\n");
fprintf(fp,"XBINS\n");
fprintf(fp,"Xantium\n");
fprintf(fp,"Iriez\n");
fprintf(fp,"XPort\n");
fprintf(fp,"the XBOX-SCENE community for waiting too long for this\n");
fprintf(fp,"\n\n\n\n");
fprintf(fp,"and last but not least:\n");
fprintf(fp,"\n\n\n\n");
fprintf(fp,"the #A-Shock Kindergarden!!!!1111Shift(1)\n");
fprintf(fp,"and the rest of our beloved scene whiners...\n");
fprintf(fp,"\n\n\n");
fprintf(fp,"1964 / UltraHLE / Launcher /Plugins\n");
fprintf(fp,"oDD\n");
fprintf(fp,"\n\n");
fprintf(fp,"Project64 / Plugins\n");
fprintf(fp,"Lantus\n");
fprintf(fp,"\n\n");
fprintf(fp,"Testing / Ideas / Support\n");
fprintf(fp,"Iriez\n");
fprintf(fp,"Very Special Thanks\n");
fprintf(fp,"Azimer for his HLE audio code\n");
fprintf(fp,"Rice for his support with his video plugin\n");
fprintf(fp,"Greets and Thanks\n");
fprintf(fp,"Carcharius, Xport, TJ, DragonZ\n");
fprintf(fp,"and anyone else we forgot\n");
fprintf(fp,"\n\n");
fprintf(fp,"1964 Credits\n");
fprintf(fp,"Authors\n");
fprintf(fp,"Schibo and Rice\n");
fprintf(fp,"Copyright (C) 1989 - 1991\n");
fprintf(fp,"Thanks\n");
fprintf(fp,"steb, zilmar, anarko, Azimer, Jabo\n");
fprintf(fp,"Greets\n");
fprintf(fp,"Angelo, ContraSF, Cricket,\n");
fprintf(fp,"_Demo_, duddie, Emul8or, epsilon,\n");
fprintf(fp,"F|RES, FlyingNR6, hartec, Houe, hWnd,\n");
fprintf(fp,"icepir8, LaC, Lemmy Lionel, Niki Waibel,\n");
fprintf(fp,"Phrodide, rcp, Scav, slacka, Smiff,\n");
fprintf(fp,"SpectRe, Spikez, subice, StrmnNrmn,\n");
fprintf(fp,"TNSe\n");
fprintf(fp,"\n\n");
fprintf(fp,"PJ64 Credits\n");
fprintf(fp,"Authors\n");
fprintf(fp,"Zilmar and Jabo\n");
fprintf(fp,"Copyright (C) 1998 - 2001\n");
fprintf(fp,"Contributors\n");
fprintf(fp,"Tooie and Witten\n");
fprintf(fp,"Support\n");
fprintf(fp,"Smiff and The Gentleman\n");
fprintf(fp,"Thanks and Greets\n");
fprintf(fp,"Anarko, Breakpoint, Cricket, _Demo_,\n");
fprintf(fp,"Duddie, F|ReS, Icepir8, LaC,\n");
fprintf(fp,"Lemmy, rcp, schibo, slacka,\n");
fprintf(fp,"smiff and anyone i missed\n");
fprintf(fp,"\n\n");
fprintf(fp,"UltraHLE Credits\n");
fprintf(fp,"Authors\n");
fprintf(fp,"Epsilon and RealityMan\n");
fprintf(fp,"Copyright (C) Epsilon and RealityMan, 1998\n");
fprintf(fp,"Greets\n");
fprintf(fp,"Fox and _64Bit_ (Emulators Unlimited),\n");
fprintf(fp,"JoseQ (EmuViews), Atila (Retrogames),\n");
fprintf(fp,"Dextrose, Niki Waibel, Mike Tedder,\n");
fprintf(fp,"Anarko, Romberman 64, Brian Faber,\n");
fprintf(fp,"Jim at EmuNews and the whole\n");
fprintf(fp,"emulation community.\n");
fprintf(fp,"\n\n");
fprintf(fp,"Rice Video Plugins\n");
fprintf(fp,"Author\n");
fprintf(fp,"Rice\n");
fprintf(fp,"Copyright (C) 2003 Rice1964\n");
fprintf(fp,"\n\n");
fprintf(fp,"Daedalus Video Plugins\n");
fprintf(fp,"Author\n");
fprintf(fp,"StrmnNrmn\n");
fprintf(fp,"Copyright (C) 2001 StrmnNrmn\n");
fprintf(fp,"\n\n");
fprintf(fp,"Input Plugin\n");
fprintf(fp,"Authors\n");
fprintf(fp,"oDD and Lantus\n");
fprintf(fp,"Copyright (C) 1989 - 1991\n");
fprintf(fp,"\n\n");
fprintf(fp,"SDL Sound Plugin\n");
fprintf(fp,"Author\n");
fprintf(fp,"JttL\n");
fprintf(fp,"Copyright (C) 2003 by Juha Luotio aka JttL\n");
fprintf(fp,"\n\n");
fprintf(fp,"HLE Sound Plugin\n");
fprintf(fp,"Author\n");
fprintf(fp,"Azimer\n");
fprintf(fp,"Copyright (C) 2000 - 2001 Azimer\n");
fprintf(fp,"\n\n");
fprintf(fp,"RSP Plugin\n");
fprintf(fp,"Author\n");
fprintf(fp,"Hacktarux\n");
fprintf(fp,"Copyright (C) 2001 Jabo and Zilmar\n");
fprintf(fp,"\n\n");
fclose(fp);
}


// Ez0n3 - this function checks for user pref BEFORE it saves the rom ini (was using defaults and then loading those instead of user pref)
// EG: if user sets say dw1964DynaMem in Surreal.ini and then opens the settings menu, dw1964DynaMem would get set to default (even if they change nothing!). because the value in the settings menu was static default.
// settings menu now reflects user pref if set, otherwise - default
// this is basically a substitute for ConfigAppLoad2 where user pref from main ini are needed also
int ConfigAppLoad3()
{
	int confappld2 = ConfigAppLoad2();
	if (confappld2 == 1 && iNumGames > 0) { // has roms and no rom ini found. load defaults and then get user pref BEFORE ConfigAppSave2.

		Rom *sRom = g_romList.GetRomAt(actualrom);
		
		int tmp_preferedemu = sRom->GetIniEntry()->iPreferredEmulator;
		int tmp_videoplugin = sRom->GetIniEntry()->videoplugin;
		int tmp_iAudioPlugin = sRom->GetIniEntry()->iAudioPlugin;
		
		int tmp_dw1964DynaMem = sRom->GetIniEntry()->dw1964DynaMem;
		int tmp_dw1964PagingMem = sRom->GetIniEntry()->dw1964PagingMem;
		int tmp_dwPJ64DynaMem = sRom->GetIniEntry()->dwPJ64DynaMem;
		int tmp_dwPJ64PagingMem = sRom->GetIniEntry()->dwPJ64PagingMem;
		bool tmp_bUseLLERSP = sRom->GetIniEntry()->bUseLLERSP;
		bool tmp_bUseBasicAudio = sRom->GetIniEntry()->bUseBasicAudio;
		
		// reinstate max video mem
		int tmp_dwMaxVideoMem = sRom->GetIniEntry()->dwMaxVideoMem;
		
		if (tmp_preferedemu < _None) preferedemu = tmp_preferedemu;
		if (tmp_videoplugin < _VideoPluginMissing) videoplugin = tmp_videoplugin;
		if (tmp_iAudioPlugin < _AudioPluginMissing) iAudioPlugin = tmp_iAudioPlugin;

		if (tmp_dw1964DynaMem > 0 && tmp_dw1964DynaMem != 8) dw1964DynaMem = tmp_dw1964DynaMem;
		if (tmp_dw1964PagingMem > 0 && tmp_dw1964PagingMem != 4) dw1964PagingMem = tmp_dw1964PagingMem;
		if (tmp_dwPJ64DynaMem > 0 && tmp_dwPJ64DynaMem != 16) dwPJ64DynaMem = tmp_dwPJ64DynaMem;
		if (tmp_dwPJ64PagingMem > 0 && tmp_dwPJ64PagingMem != 4) dwPJ64PagingMem = tmp_dwPJ64PagingMem;
		
		// leave this in case it's set in the ini
		if (tmp_bUseLLERSP) {
			//bUseLLERSP = true;
			iAudioPlugin = _AudioPluginLleRsp;
		}
		else if (tmp_bUseBasicAudio) { //tmp_bUseLLERSP || tmp_bUseBasicAudio
			//bUseLLERSP = true;
			iAudioPlugin = _AudioPluginBasic;
		}
		//else if (!tmp_bUseLLERSP && !tmp_bUseBasicAudio && iAudioPlugin == _AudioPluginBasic) {
		//	bUseLLERSP = true;
		//}
		
		// reinstate max video mem
		if (tmp_dwMaxVideoMem >= 2 && tmp_dwMaxVideoMem != 5 && tmp_dwMaxVideoMem <= 10) dwMaxVideoMem = tmp_dwMaxVideoMem;
		
		return 1;
	}
	
	return 0;
}

// Ez0n3 - determine if the current phys ram is greater than 100MB
bool PhysRam128()
{
  MEMORYSTATUS memStatus;
  GlobalMemoryStatus( &memStatus );
  if( memStatus.dwTotalPhys < (100 * 1024 * 1024) ) return false;
  else return true;  
}

// Ez0n3 - check if file exists
bool FileExists(char *szFilename) {

	FILE *h;
	if ((h = fopen(szFilename, "rt")) == NULL) {
		return false;
	} else {
		fclose(h);
		return true;
	}

}