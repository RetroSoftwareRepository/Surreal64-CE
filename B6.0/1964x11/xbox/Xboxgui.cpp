//    wingui.c The Windows User Interface

// 1964 Copyright (C) 1999-2004 Joel Middendorf, <schibo@emulation64.com> This
// program is free software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the Free Software
// Foundation; either version 2 of the License, or (at your option) any later
// version. This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
// details. You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation, Inc.,
// 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA. To contact the
// authors: email: schibo@emulation64.com, rice1964@yahoo.com

#include "../stdafx.h"

//surreal old
#include "rompaging.h"

#include "../dynarec/x86.h" // Ez0n3 - etemp - include again? - in stdafx.h

#include "IOSupport.h"
#include "128meg.h"

struct EMU1964GUI	gui;
struct DIRECTORIES	directories;
struct GUISTATUS	guistatus;
int					ActiveApp;

char				game_country_name[10];
int					game_country_tvsystem = 0;
int					Audio_Is_Initialized = 0;
int					timer;
int					StateFileNumber = 0;
extern BOOL			Is_Reading_Rom_File;
extern BOOL			To_Stop_Reading_Rom_File;
extern BOOL			opcode_debugger_memory_is_allocated;
extern HINSTANCE	hinstControllerPlugin;

//int					MenuCausedPause = FALSE;

HANDLE				StopEmulatorEvent = NULL;
HANDLE				ResumeEmulatorEvent = NULL;
HANDLE				PauseEmulatorEvent = NULL;

int					stateindex = 0;
int				DoState = DO_NOT_DO_PJ64_STATE;

void					UpdateCIC(void);

extern HANDLE	AudioThreadStopEvent;
extern HANDLE	AudioThreadEvent;

// like windows
/*BOOL PathFileExists(const char *pszPath)
{   
    return GetFileAttributes(pszPath) != INVALID_FILE_ATTRIBUTES;   
}
BOOL MessageBox(HWND hWnd, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType)
{
	OutputDebugString(lpText);
	OutputDebugString("\n");
	return FALSE;
}
BOOL TerminateThread(HANDLE hThread, DWORD dwExitCode)
{
	ExitThread(dwExitCode);
	return TRUE;
}*/
BOOL WindowMsgLoop()
{
	return FALSE;
}
void __cdecl SetStatusBarText(int fieldno, char *text)
{
	return;
}

char g_szPathSaves[256] = "D:\\Saves\\";
//extern void _VIDEO_SetMaxTextureMem(DWORD mem);

#if defined __cplusplus
extern "C" {
#endif

char emuname[256];

extern "C" void __EMU_SaveState(int index)
{
	DoState = SAVE_1964_CREATED_PJ64_STATE;
	stateindex = index;

	CPUNeedToCheckInterrupt = TRUE;
	CPUNeedToDoOtherTask = TRUE;
	Set_Check_Interrupt_Timer_Event();
}

extern "C" void __EMU_LoadState(int index)
{
	//deprecated
}

extern "C" void __EMU_Load1964State(int index)
{
	DoState = LOAD_1964_CREATED_PJ64_STATE;
	stateindex = index;

	CPUNeedToCheckInterrupt = TRUE;
	CPUNeedToDoOtherTask = TRUE;
	Set_Check_Interrupt_Timer_Event();	
}

extern "C" void __EMU_LoadPJ64State(int index)
{
	DoState = LOAD_PJ64_CREATED_PJ64_STATE;
	stateindex = index;

	CPUNeedToCheckInterrupt = TRUE;
	CPUNeedToDoOtherTask = TRUE;
	Set_Check_Interrupt_Timer_Event();	
}

// Shouldn't be used anymore, unless UHLE uses video plugins
extern "C" void __EMU_GetStateFilename(int index, char *filename, int mode)
{
	if(mode == 0){
		sprintf(filename, "%s%08x\\%08X-%08X-%02X.%i", g_szPathSaves, currentromoptions.crc1, currentromoptions.crc1, currentromoptions.crc2, currentromoptions.countrycode, index);
	}
	else if(mode == 1){
		sprintf(filename, "%s%08x\\%08X-%08X-%02X.%i.bmp", g_szPathSaves, currentromoptions.crc1, currentromoptions.crc1, currentromoptions.crc2, currentromoptions.countrycode, index);
	}
	
	return;
}

extern "C" void __EMU_Get1964StateFilename(int index, char *filename, int mode)
{
	if(mode == 0){
		sprintf(filename, "%s%08x\\%08X-%08X-%02X.%i.1964", g_szPathSaves, currentromoptions.crc1, currentromoptions.crc1, currentromoptions.crc2, currentromoptions.countrycode, index);
	}
	else if(mode == 1){
		sprintf(filename, "%s%08x\\%08X-%08X-%02X.%i.1964.bmp", g_szPathSaves, currentromoptions.crc1, currentromoptions.crc1, currentromoptions.crc2, currentromoptions.countrycode, index);
	}
	
	return;
}

extern "C" void __EMU_GetPJ64StateFilename(int index, char *filename, int mode)
{
	if(mode == 0){
		sprintf(filename, "%s%08x\\%08X-%08X-%02X.%i.pj64", g_szPathSaves, currentromoptions.crc1, currentromoptions.crc1, currentromoptions.crc2, currentromoptions.countrycode, index);
	}
	else if(mode == 1){
		sprintf(filename, "%s%08x\\%08X-%08X-%02X.%i.pj64.bmp", g_szPathSaves, currentromoptions.crc1, currentromoptions.crc1, currentromoptions.crc2, currentromoptions.countrycode, index);
	}
	
	return;
}

extern void loadinis();

extern int loaddw1964PagingMem();
extern int loaddw1964DynaMem();

extern int loadbUseRspAudio();
extern int loadiRspPlugin();
extern int loadiAudioPlugin();

extern int loaddwMaxVideoMem();

extern int loadiPagingMethod();
extern int loadbAudioBoost();

extern void GetPathSaves(char *pszPathSaves);

#if defined __cplusplus
};
#endif

//#ifdef DEBUG
MEMORYSTATUS stat;
char buf[100];
//#endif

void __cdecl main()
{
	// mount the common drives
	g_IOSupport.Mount("A:","cdrom0");
	g_IOSupport.Mount("E:","Harddisk0\\Partition1");
	g_IOSupport.Mount("F:","Harddisk0\\Partition6");
	g_IOSupport.Mount("G:","Harddisk0\\Partition7");

	// utility shoud be mounted automatically
	if(XGetDiskSectorSize("Z:\\") == 0)
		g_IOSupport.Mount("Z:","Harddisk0\\Partition5");
		
	// make sure there's a temp rom
	if (PathFileExists("Z:\\TemporaryRom.dat")) {
		OutputDebugString("Z:\\TemporaryRom.dat File Found!\n");
		strcpy(g_temporaryRomPath, "Z:\\TemporaryRom.dat");
	}
	else {
		OutputDebugString("Z:\\TemporaryRom.dat File Not Found!\n");
		
		// if debugging, a temp rom can be placed in T to skip the launcher
		if (PathFileExists("T:\\Data\\TemporaryRom.dat")) {
			OutputDebugString("T:\\Data\\TemporaryRom.dat File Found!\n");
			strcpy(g_temporaryRomPath, "T:\\Data\\TemporaryRom.dat");
		}
		else {
			OutputDebugString("T:\\Data\\TemporaryRom.dat File Not Found!\n");
			Sleep(100);
			XLaunchNewImage("D:\\default.xbe", NULL);
		}
	}
	
	loadinis(); // Ez0n3 - diff than fd - missing
	sprintf(emuname,"1964x 1.1");
	
	GetPathSaves(g_szPathSaves);

	//freakdave - check for 128mb
	//PhysRam128();

	
#ifdef USE_ROM_PAGING
	g_iPagingMethod = loadiPagingMethod();
	if (g_iPagingMethod == _PagingXXX) {
		g_dwPageSize = 0x40000;
	}
	else if (g_iPagingMethod == _PagingS10) {
		g_dwPageSize = 0x10000;
	}
#endif
	Enable128MegCaching();

	g_dwRecompCodeSize = loaddw1964DynaMem() * 1024 * 1024;

	g_bAudioBoost = (loadbAudioBoost() == 1 ? TRUE : FALSE); // set before audio init

	//g_bUseLLERspPlugin = loadbUseLLERSP(); // not used anymore
	g_bUseRspAudio = loadbUseRspAudio(); // control a listing
	g_iRspPlugin = loadiRspPlugin();
	g_iAudioPlugin = loadiAudioPlugin();

	LoadVideoPlugin("XBOX");
	LoadControllerPlugin("XBOX");

	LoadAudioPlugin("XBOX");
	LoadRSPPlugin("XBOX");
	
	// max texture mem, 0 = auto
	_VIDEO_SetMaxTextureMem(loaddwMaxVideoMem());

	// start the emulator
	gui.szBaseWindowTitle = "1964 1.0";
	gui.hwnd1964main = NULL;		/* handle to main window */
	gui.hClientWindow = NULL;		/* Window handle of the client child window */

	Rom_Loaded = FALSE;
	guistatus.block_menu = TRUE;	/* block all menu commands during startup */
	emustatus.cpucore = DYNACOMPILER;
	emustatus.Emu_Is_Resetting = FALSE;
	guistatus.IsFullScreen = FALSE;

	emustatus.Emu_Is_Running = FALSE;
	emustatus.Emu_Is_Paused = FALSE;
	emustatus.Emu_Keep_Running = FALSE;
	emustatus.processing_exception = FALSE;

	StopEmulatorEvent = CreateEvent( NULL, FALSE, FALSE, NULL );
	if (StopEmulatorEvent == NULL)
	{ 
		DisplayError( "Error creating StopEmulatorEvent events");
	} 
	ResumeEmulatorEvent = CreateEvent( NULL, FALSE, FALSE, NULL );
	AudioThreadStopEvent = CreateEvent( NULL, FALSE, FALSE, NULL );
	AudioThreadEvent = CreateEvent( NULL, FALSE, FALSE, NULL );
    PauseEmulatorEvent = CreateEvent( NULL, FALSE, FALSE, NULL );

	if (ResumeEmulatorEvent == NULL)
	{ 
		DisplayError( "Error creating ResumeEmulatorEvent events");
	} 

	SetDefaultOptions(); //surreal

	Set_1964_Directory();

	//ReadConfiguration();
	ReadConfiguration1964();

	InitVirtualMemory();
	InitPluginData();

	emustatus.cpucore = defaultoptions.Emulator;

	LoadPlugins();

    SetCounterFactor(defaultoptions.Counter_Factor);

	WinLoadRomStep2(g_temporaryRomPath);

		Play(emuoptions.auto_full_screen);

		emustatus.Emu_Is_Resetting = FALSE; //needed? - set above

	{ // create the save directory if it doesn't exist
		char szPathSaves[_MAX_PATH];
		sprintf(szPathSaves, "%s%08x", g_szPathSaves, currentromoptions.crc1);
		if (!PathFileExists(szPathSaves)) {
			if (!CreateDirectory(szPathSaves, NULL)) {
				OutputDebugString(szPathSaves);
				OutputDebugStringA(" Could Not Be Created!\n");
			}
		}
	}

	while (TRUE)
	{
//#ifdef DEBUG
		GlobalMemoryStatus(&stat);
		sprintf(buf, "%.2fMB of RAM available\n", (float)stat.dwAvailPhys / 1024.0f / 1024.0f);
		OutputDebugString(buf);
//#endif
		Sleep(1000);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void Resume(void)
{
	if(emustatus.Emu_Is_Running && emustatus.Emu_Is_Paused)
	{
		QueryPerformanceCounter(&LastSecondTime); //needed?
		ResumeEmulator(DO_NOTHING_AFTER_PAUSE);
	}
}

void AfterStop(void);

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void KillCPUThread(void)
{
	if(emustatus.Emu_Is_Running)
	{
		DWORD exitCode; //surreal
		
		SuspendThread(CPUThreadHandle);

		//TerminateThread(CPUThreadHandle, 1);
		//surreal
		GetExitCodeThread(CPUThreadHandle, &exitCode);
		ExitThread(exitCode);

		CloseHandle(CPUThreadHandle);

		if(currentromoptions.Code_Check == CODE_CHECK_PROTECT_MEMORY) UnprotectAllBlocks();

		AUDIO_RomClosed();
		CONTROLLER_RomClosed();
		VIDEO_RomClosed();

		AfterStop();
	}

	//To finish off the window placement and settings
	Rom_Loaded = TRUE;
	CloseROM();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void Kill(void)
{
	StopEmulator();
	CPUThreadHandle = NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */

extern void (__stdcall StartCPUThread) (void *pVoid);
extern void (__stdcall StartAIInterruptThread) (void *pVoid); //needed?
extern void StartAudio(void);

void __cdecl Play(BOOL WithFullScreen)
{
	int core;
	DWORD ThreadID;

	if(Rom_Loaded)
	{
		if(emustatus.Emu_Is_Running) 
		{
			Stop();
            Sleep(1000); //Fix for Reset: if user holds down F2 long time it works, and does not crash.
		}


		PrepareBeforePlay(guistatus.IsFullScreen);

		core = currentromoptions.Emulator;
		if(core == DYNACOMPILER)
		{					/* Dynarec */
			emustatus.cpucore = DYNACOMPILER;
		}
		else
		{					/* Interpreter */
			emustatus.cpucore = INTERPRETER;
		}

		r4300i_Reset();

		emustatus.Emu_Keep_Running = TRUE;
		emustatus.processing_exception = FALSE;

		if(Audio_Is_Initialized == 1 && CoreDoingAIUpdate == 0 )
		{
			StartAudio();	// Start the audio thread
		}

		r4300i_Reset();
		CPUThreadHandle = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)StartCPUThread,NULL,0, &ThreadID);

		if(WithFullScreen && (emustatus.Emu_Is_Resetting == 0))
		{
			if(guistatus.IsFullScreen == 0)
			{
				VIDEO_ChangeWindow(guistatus.IsFullScreen);
			}
		}
	}
	else
		DisplayError("Please load a ROM first.");
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */

extern int stopEntryCount;
void Stop()
{
	if(emustatus.Emu_Is_Running)
	{
		//MessageBox(0, "Trying to StopEmulator", "Stop", 0);	
		if( stopEntryCount > 0 )
		{
			StopEmulator();
		}
		else
		{
			StopEmulator();
			if (emustatus.Emu_Is_Resetting == 0)
			{
				if(guistatus.IsFullScreen)
				{
					VIDEO_ChangeWindow(guistatus.IsFullScreen);
				}

				AfterStop();
			}
		}

	}
	else if(Is_Reading_Rom_File)
	{
		SetEvent( StopEmulatorEvent ); //needed?
		To_Stop_Reading_Rom_File = TRUE;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void CloseROM(void)
{
	if(emustatus.Emu_Is_Running)
	{
		Stop();

		Close_iPIF();
		FreeVirtualRomMemory();

		Rom_Loaded = FALSE;
	}

	/*
	 * else
	 * DisplayError("Please load a ROM first.");
	 */
}

/*
=======================================================================================================================
=======================================================================================================================
*/
BOOL __cdecl WinLoadRomStep2(char *szFileName)
{
	lstrcpy(gui.szWindowTitle, gui.szBaseWindowTitle);
	lstrcat(gui.szWindowTitle, " - ");
	
	if(ReadRomData(szFileName) == FALSE) 
	{
		Rom_Loaded = FALSE;
		return FALSE;
	}
	
	lstrcat(gui.szWindowTitle, (LPCSTR)rominfo.name);
	
#ifdef USE_ROM_PAGING
	// added by oDD
	{
		FILE *tmpFile = fopen(g_temporaryRomPath, "rb");
		fseek(tmpFile, 0, SEEK_SET);
		fread(&HeaderDllPass[0], sizeof(uint8), 0x40, tmpFile);
		fclose(tmpFile);
	}
#else
	memcpy(&HeaderDllPass[0], &gMemoryState.ROM_Image[0], 0x40);
#endif
	Rom_Loaded = TRUE;
	gHWS_pc = 0xA4000040;	/* We do it in r4300i_inithardware */
	
	UpdateCIC();
	
	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */

extern BYTE __cdecl GetButtonState( int	nID );

void SaveState(void)
{
	if(Rom_Loaded)
	{
		if(emustatus.Emu_Is_Running)
		{
            
            //Please don't change this, It's needed because menu pauses interfere with other Pauses.
            //if ( !(GetButtonState(ID_BUTTON_PAUSE)&TBSTATE_CHECKED))
			{
                emustatus.Emu_Is_Paused = 0;
                PauseEmulator();
            }

			FileIO_gzSaveState();
            
            //if ( !(GetButtonState(ID_BUTTON_PAUSE)&TBSTATE_CHECKED))
            {
                emustatus.Emu_Is_Paused = 1;
				ResumeEmulator(DO_NOTHING_AFTER_PAUSE);
			}
		}
		else
		{
			FileIO_gzSaveState();
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void LoadState(void)
{
	/*~~~~~~~~~~~~~~~~~~~~*/
	int was_running = FALSE;
	/*~~~~~~~~~~~~~~~~~~~~*/

	if(Rom_Loaded)
	{
		if(emustatus.Emu_Is_Running)
		{
            //Please don't change this, It's needed because menu pauses interfere with other Pauses.
            //if ( !(GetButtonState(ID_BUTTON_PAUSE)&TBSTATE_CHECKED))
            {
                emustatus.Emu_Is_Paused = 0;
                PauseEmulator();
            }
			{
				FileIO_gzLoadState();
				Init_Count_Down_Counters();
                //if ( !(GetButtonState(ID_BUTTON_PAUSE)&TBSTATE_CHECKED))
                {
                    emustatus.Emu_Is_Paused = 1;
                    ResumeEmulator(DO_NOTHING_AFTER_PAUSE);
                }
			}
		}
		else
		{
			FileIO_gzLoadState();
			Init_Count_Down_Counters();
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */

void StateSetNumber(int number)
{
	StateFileNumber = number;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SetCounterFactor(int factor)
{
    //if (!emuoptions.AutoCF) //needed?
    {

	if(CounterFactor != factor)
	{
		if(emustatus.Emu_Is_Running)
		{
			if(PauseEmulator())
			{
				CounterFactor = factor;
				ResumeEmulator(REFRESH_DYNA_AFTER_PAUSE);	/* Need to init emu */
			}
		}

		CounterFactor = factor;
	}
}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SetCodeCheckMethod(int method)
{
	if(emustatus.Emu_Is_Running)
	{
		if(PauseEmulator())
		{
			Dyna_Check_Codes = Dyna_Code_Check[method - 1];
			emustatus.CodeCheckMethod = method;
			ResumeEmulator(REFRESH_DYNA_AFTER_PAUSE);	/* Need to init emu */
		}
	}
	else
	{
		emustatus.CodeCheckMethod = method;
		defaultoptions.Code_Check = (CODECHECKTYPE)method;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void PrepareBeforePlay(int IsFullScreen)
{
	//needed?
	ResetEvent(StopEmulatorEvent);
	ResetEvent(AudioThreadEvent);
	ResetEvent(AudioThreadStopEvent);
	ResetEvent(ResumeEmulatorEvent);

	/* Setting options */
	GenerateCurrentRomOptions();
	init_whole_mem_func_array();					/* Needed here. The tlb function pointers change. */
	ResetRdramSize(currentromoptions.RDRAM_Size);

	CounterFactor = currentromoptions.Counter_Factor;

	emustatus.CodeCheckMethod = currentromoptions.Code_Check;

	/*
	 * Using the Check_QWORD to boot, will switch to ROM specified
	 * emustatus.CodeCheckMethod ?
	 * at first FPU exception. I don't know why use NoCheck method will not boot ?
	 * Game like SuperMario should not need to do DynaCodeCheck but how the ROM does
	 * not boot ?
	 * with DynaCodeCheck, need debug
	 */
	if(emustatus.CodeCheckMethod == CODE_CHECK_NONE || emustatus.CodeCheckMethod == CODE_CHECK_DMA_ONLY)
	{
		Dyna_Check_Codes = Dyna_Code_Check_None_Boot;
		TRACE0("Set code check method = Dyna_Code_Check_None_Boot / Check_DMA_only");
	}
	else
	{
		Dyna_Check_Codes = Dyna_Code_Check[emustatus.CodeCheckMethod - 1];
	}

	emustatus.cpucore = currentromoptions.Emulator;

	/* About FPU usage exceptions */
	if((gHWS_COP0Reg[STATUS] & SR_CU1) || currentromoptions.FPU_Hack == USEFPUHACK_NO)
	{
		DisableFPUUnusableException();
	}
	else
	{
        EnableFPUUnusableException();		
	}

	Flashram_Init();
	Init_iPIF();

	emustatus.DListCount = 0;
	emustatus.AListCount = 0;
	emustatus.PIDMACount = 0;
	emustatus.viframeskip = 0;
	emustatus.ControllerReadCount = 0;
	emustatus.FrameBufferProtectionNeedToBeUpdate = TRUE;

	if(!QueryPerformanceFrequency(&Freq))
	{
		currentromoptions.Max_FPS = MAXFPS_NONE;	/* ok, this computer does not support */
		/* accurate timer, don't use speed limiter */
	}

	if(rominfo.TV_System == TV_SYSTEM_PAL)					/* PAL */
	{
		vips_speed_limits[MAXFPS_AUTO_SYNC] = vips_speed_limits[MAXFPS_PAL_50];
	}
	else	/* NTSC */
	{
		vips_speed_limits[MAXFPS_AUTO_SYNC] = vips_speed_limits[MAXFPS_NTSC_60];
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AfterStop(void)
{
	Close_Save();
	Close_iPIF();

	emustatus.Emu_Is_Running = FALSE;

	/* Reset some of the default options */
	defaultoptions.Emulator = DYNACOMPILER;
	emustatus.cpucore = defaultoptions.Emulator;

	SetCounterFactor(defaultoptions.Counter_Factor);
	SetCodeCheckMethod(defaultoptions.Code_Check);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void CaptureScreenToFile(void)
{

}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void Exit1964(void)
{
    __try {    
		static BOOL exiting_1964 = FALSE;

		if (exiting_1964) return;
		exiting_1964 = TRUE;

		if(emustatus.Emu_Is_Running) 
			Stop();

		Close_iPIF();			/* save mempak and eeprom */
		FreeVirtualMemory();

		if (StopEmulatorEvent)			CloseHandle( StopEmulatorEvent );
		if (ResumeEmulatorEvent)		CloseHandle( ResumeEmulatorEvent);
		if (AudioThreadStopEvent)		CloseHandle( AudioThreadStopEvent);
		if (AudioThreadEvent)			CloseHandle( AudioThreadEvent);

		exit(0);
	}
    __except(NULL, EXCEPTION_EXECUTE_HANDLER)
    {
        exit(0);
    }
}
