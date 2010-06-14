/*
 * Project 64 - A Nintendo 64 emulator.
 *
 * (c) Copyright 2001 zilmar (zilmar@emulation64.com) and 
 * Jabo (jabo@emulation64.com).
 *
 * pj64 homepage: www.pj64.net
 *
 * Permission to use, copy, modify and distribute Project64 in both binary and
 * source form, for non-commercial purposes, is hereby granted without fee,
 * providing that this license information and copyright notice appear with
 * all copies and any derived work.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event shall the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Project64 is freeware for PERSONAL USE only. Commercial users should
 * seek permission of the copyright holders first. Commercial use includes
 * charging money for Project64 or software derived from Project64.
 *
 * The copyright holders request that bug fixes and improvements to the code
 * should be forwarded to them so if they want them.
 *
 */
#include <xtl.h>
#include <stdio.h>
#include "main.h"
#include "CPU.h"
#include "Debugger.h"
#include "plugin.h"
#include "resource.h"

// Ez0n3 - plugins
#include "../Plugins.h"

 
extern void _VIDEO_CloseDLL (void);
extern void _VIDEO_DllConfig ( HWND hParent );
extern void _VIDEO_GetDllInfo( PLUGIN_INFO *PluginInfo);
extern void _VIDEO_ChangeWindow(void);
extern void _VIDEO_DrawScreen(void);
extern BOOL _VIDEO_InitiateGFX(GFX_INFO Gfx_Info);
extern void _VIDEO_MoveScreen(int xpos, int ypos);
extern void _VIDEO_ProcessDList(void);
extern void _VIDEO_RomClosed(void);
extern void _VIDEO_RomOpen(void);
extern void _VIDEO_UpdateScreen(void);
extern void _VIDEO_ViStatusChanged(void);
extern void _VIDEO_ViStatusChanged(void);
extern void _VIDEO_ViWidthChanged(void);
extern void _RSP_GetDllInfo ( PLUGIN_INFO * PluginInfo );
extern void _RSP_RomClosed (void);
extern void _RSP_CloseDLL (void);
extern void _RSP_DllConfig (HWND hWnd);

//freakdave - JttL
extern void _AUDIO_CloseDLL (void);
extern void _AUDIO_RomClosed(void);
extern void _AUDIO_DllConfig ( HWND hParent );
extern void _AUDIO_GetDllInfo(PLUGIN_INFO *PluginInfo);
extern void _AUDIO_AiDacrateChanged(int SystemType);
extern void _AUDIO_AiLenChanged(void);
extern void _AUDIO_AiReadLength(void);
extern void _AUDIO_AiUpdate(BOOL Wait);
extern BOOL _AUDIO_InitiateAudio (AUDIO_INFO Audio_Info);
extern void _AUDIO_ProcessAList(void);


// Basic Audio
extern void _AUDIO_BASIC_CloseDLL (void);
extern void _AUDIO_BASIC_RomClosed(void);
extern void _AUDIO_BASIC_DllConfig ( HWND hParent );
extern void _AUDIO_BASIC_GetDllInfo(PLUGIN_INFO *PluginInfo);
extern void _AUDIO_BASIC_AiDacrateChanged(int SystemType);
extern void _AUDIO_BASIC_AiLenChanged(void);
extern void _AUDIO_BASIC_AiReadLength(void);
extern void _AUDIO_BASIC_AiUpdate(BOOL Wait);
extern BOOL _AUDIO_BASIC_InitiateAudio (AUDIO_INFO Audio_Info);
extern void _AUDIO_BASIC_ProcessAList(void);


// Ez0n3 - No Audio
extern void _AUDIO_NONE_CloseDLL (void);
extern void _AUDIO_NONE_RomClosed(void);
extern void _AUDIO_NONE_DllConfig ( HWND hParent );
extern void _AUDIO_NONE_GetDllInfo(PLUGIN_INFO *PluginInfo);
extern void _AUDIO_NONE_AiDacrateChanged(int SystemType);
extern void _AUDIO_NONE_AiLenChanged(void);
extern void _AUDIO_NONE_AiReadLength(void);
extern void _AUDIO_NONE_AiUpdate(BOOL Wait);
extern BOOL _AUDIO_NONE_InitiateAudio (AUDIO_INFO Audio_Info);
extern void _AUDIO_NONE_ProcessAList(void);

// freakdave - New MusyX audio plugin
extern void _AUDIO_MUSYX_CloseDLL (void);
extern void _AUDIO_MUSYX_RomClosed(void);
extern void _AUDIO_MUSYX_DllConfig ( HWND hParent );
extern void _AUDIO_MUSYX_GetDllInfo(PLUGIN_INFO *PluginInfo);
extern void _AUDIO_MUSYX_AiDacrateChanged(int SystemType);
extern void _AUDIO_MUSYX_AiLenChanged(void);
extern void _AUDIO_MUSYX_AiReadLength(void);
extern void _AUDIO_MUSYX_AiUpdate(BOOL Wait);
extern BOOL _AUDIO_MUSYX_InitiateAudio (AUDIO_INFO Audio_Info);
extern void _AUDIO_MUSYX_ProcessAList(void);


// Ez0n3 - old Azimer
// freakdave - readded Azimer

extern void _AUDIO_AZIMER_CloseDLL (void);
extern void _AUDIO_AZIMER_RomClosed(void);
extern void _AUDIO_AZIMER_DllConfig ( HWND hParent );
extern void _AUDIO_AZIMER_GetDllInfo(PLUGIN_INFO *PluginInfo);
extern void _AUDIO_AZIMER_AiDacrateChanged(int SystemType);
extern void _AUDIO_AZIMER_AiLenChanged(void);
extern void _AUDIO_AZIMER_AiReadLength(void);
extern void _AUDIO_AZIMER_AiUpdate(BOOL Wait);
extern BOOL _AUDIO_AZIMER_InitiateAudio (AUDIO_INFO Audio_Info);
extern void _AUDIO_AZIMER_ProcessAList(void);




extern DWORD _RSP_DoRspCycles(DWORD Cycles);
extern void _RSP_InitiateRSP ( RSP_INFO_1_1 Rsp_Info, DWORD * CycleCount);
//extern DWORD _RSP_HLE_DoRspCycles(DWORD Cycles);
//extern void _RSP_HLE_InitiateRSP ( RSP_INFO_1_1 Rsp_Info, DWORD * CycleCount);
extern void _INPUT_CloseDLL(void);
extern void _INPUT_ControllerCommand(int Control, BYTE *Command);
extern void _INPUT_DllConfig(HWND hParent);
extern void _INPUT_GetDllInfo(PLUGIN_INFO *PluginInfo);
extern void _INPUT_GetKeys(int Control, BUTTONS *Keys);
extern void _INPUT_InitiateControllers(HWND hMainWindow, CONTROL Controls[]);
extern void _INPUT_ReadController(int Control, BYTE *Command);
extern void _INPUT_RomClosed(void);
extern void _INPUT_RomOpen(void);
extern void _INPUT_WM_KeyDown(WPARAM wParam, LPARAM lParam);
extern void _INPUT_WM_KeyUp(WPARAM wParam, LPARAM lParam);
 
HINSTANCE hAudioDll, hControllerDll, hGfxDll, hRspDll;
DWORD PluginCount, RspTaskValue, AudioIntrReg;
WORD RSPVersion,ContVersion;
HANDLE hAudioThread = NULL;
CONTROL Controllers[4];

// Ez0n3 - use iAudioPlugin instead to determine if basic audio is used
//BOOL g_bUseLLERspPlugin = FALSE;
//BOOL g_bUseBasicAudio = FALSE;
int g_iAudioPlugin = _AudioPluginJttl; // set default to jttl


BOOL PluginsChanged ( HWND hDlg );
BOOL ValidPluginVersion ( PLUGIN_INFO * PluginInfo );
 
void GetCurrentDlls (void) {
	 
 
}

void GetPluginDir( char * Directory ) 
{

}

void GetSnapShotDir( char * Directory ) 
{

}

BOOL LoadAudioBasicDll(void) {
	PLUGIN_INFO PluginInfo;
	 
	hAudioDll = (HANDLE)100;
	if (hAudioDll == NULL) {  return FALSE; }

	GetDllInfo = (void *)_AUDIO_BASIC_GetDllInfo;
	if (GetDllInfo == NULL) { return FALSE; }

	GetDllInfo(&PluginInfo);
	if (!ValidPluginVersion(&PluginInfo) || PluginInfo.MemoryBswaped == FALSE) { return FALSE; }

	AiCloseDLL = (void *)_AUDIO_BASIC_CloseDLL;
	if (AiCloseDLL == NULL) { return FALSE; }
	AiDacrateChanged = (void *)(int)_AUDIO_BASIC_AiDacrateChanged;
	if (AiDacrateChanged == NULL) { return FALSE; }
	AiLenChanged = (void *)_AUDIO_BASIC_AiLenChanged;
	if (AiLenChanged == NULL) { return FALSE; }
	AiReadLength = (DWORD ( *)(void))_AUDIO_BASIC_AiReadLength;
	if (AiReadLength == NULL) { return FALSE; }
	InitiateAudio = (BOOL ( *)(AUDIO_INFO))_AUDIO_BASIC_InitiateAudio;
	if (InitiateAudio == NULL) { return FALSE; }
	AiRomClosed = (void ( *)(void))_AUDIO_BASIC_RomClosed;
	if (AiRomClosed == NULL) { return FALSE; }
	ProcessAList = (void ( *)(void))_AUDIO_BASIC_ProcessAList;	
	if (ProcessAList == NULL) { return FALSE; }

	AiDllConfig = (void ( *)(HWND))_AUDIO_BASIC_DllConfig;
	AiUpdate = (void ( *)(BOOL))_AUDIO_BASIC_AiUpdate;
	return TRUE;
}

BOOL LoadAudioJttLDll(void) {
	PLUGIN_INFO PluginInfo;
	 
	hAudioDll = (HANDLE)100;
	if (hAudioDll == NULL) {  return FALSE; }

	//freakdave
	//GetDllInfo = (void *)_AUDIO_JTTL_GetDllInfo;
	GetDllInfo = (void *)_AUDIO_GetDllInfo;
	if (GetDllInfo == NULL) { return FALSE; }

	GetDllInfo(&PluginInfo);
	if (!ValidPluginVersion(&PluginInfo) || PluginInfo.MemoryBswaped == FALSE) { return FALSE; }
	//freakdave - Implementation of Jttl's audio plugin (replace Azimer)
	/*
	AiCloseDLL = (void *)_AUDIO_JTTL_CloseDLL;
	if (AiCloseDLL == NULL) { return FALSE; }
	AiDacrateChanged = (void *)(int)_AUDIO_JTTL_AiDacrateChanged;
	if (AiDacrateChanged == NULL) { return FALSE; }
	AiLenChanged = (void *)_AUDIO_JTTL_AiLenChanged;
	if (AiLenChanged == NULL) { return FALSE; }
	AiReadLength = (DWORD ( *)(void))_AUDIO_JTTL_AiReadLength;
	if (AiReadLength == NULL) { return FALSE; }
	InitiateAudio = (BOOL ( *)(AUDIO_INFO))_AUDIO_JTTL_InitiateAudio;
	if (InitiateAudio == NULL) { return FALSE; }
	AiRomClosed = (void ( *)(void))_AUDIO_JTTL_RomClosed;
	if (AiRomClosed == NULL) { return FALSE; }
	ProcessAList = (void ( *)(void))_AUDIO_JTTL_ProcessAList;	
	if (ProcessAList == NULL) { return FALSE; }

	AiDllConfig = (void ( *)(HWND))_AUDIO_JTTL_DllConfig;
	AiUpdate = (void ( *)(BOOL))_AUDIO_JTTL_AiUpdate;
	return TRUE;
*/
	//freakdave - Implementation of Jttl's audio plugin (replace Azimer)
	AiCloseDLL = (void *)_AUDIO_CloseDLL;
	if (AiCloseDLL == NULL) { return FALSE; }
	AiDacrateChanged = (void *)(int)_AUDIO_AiDacrateChanged;
	if (AiDacrateChanged == NULL) { return FALSE; }
	AiLenChanged = (void *)_AUDIO_AiLenChanged;
	if (AiLenChanged == NULL) { return FALSE; }
	AiReadLength = (DWORD ( *)(void))_AUDIO_AiReadLength;
	if (AiReadLength == NULL) { return FALSE; }
	InitiateAudio = (BOOL ( *)(AUDIO_INFO))_AUDIO_InitiateAudio;
	if (InitiateAudio == NULL) { return FALSE; }
	AiRomClosed = (void ( *)(void))_AUDIO_RomClosed;
	if (AiRomClosed == NULL) { return FALSE; }
	ProcessAList = (void ( *)(void))_AUDIO_ProcessAList;	
	if (ProcessAList == NULL) { return FALSE; }

	AiDllConfig = (void ( *)(HWND))_AUDIO_DllConfig;
	AiUpdate = (void ( *)(BOOL))_AUDIO_AiUpdate;
	return TRUE;

}

//freakdave - readded Azimer
BOOL LoadAudioAzimerDll(void) {
	PLUGIN_INFO PluginInfo;
	 
	hAudioDll = (HANDLE)100;
	if (hAudioDll == NULL) {  return FALSE; }

	GetDllInfo = (void *)_AUDIO_AZIMER_GetDllInfo;
	if (GetDllInfo == NULL) { return FALSE; }

	GetDllInfo(&PluginInfo);
	if (!ValidPluginVersion(&PluginInfo) || PluginInfo.MemoryBswaped == FALSE) { return FALSE; }	
	
	AiCloseDLL = (void *)_AUDIO_AZIMER_CloseDLL;
	if (AiCloseDLL == NULL) { return FALSE; }
	AiDacrateChanged = (void *)(int)_AUDIO_AZIMER_AiDacrateChanged;
	if (AiDacrateChanged == NULL) { return FALSE; }
	AiLenChanged = (void *)_AUDIO_AZIMER_AiLenChanged;
	if (AiLenChanged == NULL) { return FALSE; }
	AiReadLength = (DWORD ( *)(void))_AUDIO_AZIMER_AiReadLength;
	if (AiReadLength == NULL) { return FALSE; }
	InitiateAudio = (BOOL ( *)(AUDIO_INFO))_AUDIO_AZIMER_InitiateAudio;
	if (InitiateAudio == NULL) { return FALSE; }
	AiRomClosed = (void ( *)(void))_AUDIO_AZIMER_RomClosed;
	if (AiRomClosed == NULL) { return FALSE; }
	ProcessAList = (void ( *)(void))_AUDIO_AZIMER_ProcessAList;	
	if (ProcessAList == NULL) { return FALSE; }

	AiDllConfig = (void ( *)(HWND))_AUDIO_AZIMER_DllConfig;
	AiUpdate = (void ( *)(BOOL))_AUDIO_AZIMER_AiUpdate;
	return TRUE;

}



// Ez0n3 - No Audio
BOOL LoadAudioNoneDll(void) {
	PLUGIN_INFO PluginInfo;
	 
	hAudioDll = (HANDLE)100;
	if (hAudioDll == NULL) {  return FALSE; }

	GetDllInfo = (void *)_AUDIO_NONE_GetDllInfo;
	if (GetDllInfo == NULL) { return FALSE; }

	GetDllInfo(&PluginInfo);
	if (!ValidPluginVersion(&PluginInfo) || PluginInfo.MemoryBswaped == FALSE) { return FALSE; }

	AiCloseDLL = (void *)_AUDIO_NONE_CloseDLL;
	if (AiCloseDLL == NULL) { return FALSE; }
	AiDacrateChanged = (void *)(int)_AUDIO_NONE_AiDacrateChanged;
	if (AiDacrateChanged == NULL) { return FALSE; }
	AiLenChanged = (void *)_AUDIO_NONE_AiLenChanged;
	if (AiLenChanged == NULL) { return FALSE; }
	AiReadLength = (DWORD ( *)(void))_AUDIO_NONE_AiReadLength;
	if (AiReadLength == NULL) { return FALSE; }
	InitiateAudio = (BOOL ( *)(AUDIO_INFO))_AUDIO_NONE_InitiateAudio;
	if (InitiateAudio == NULL) { return FALSE; }
	AiRomClosed = (void ( *)(void))_AUDIO_NONE_RomClosed;
	if (AiRomClosed == NULL) { return FALSE; }
	ProcessAList = (void ( *)(void))_AUDIO_NONE_ProcessAList;	
	if (ProcessAList == NULL) { return FALSE; }

	AiDllConfig = (void ( *)(HWND))_AUDIO_NONE_DllConfig;
	AiUpdate = (void ( *)(BOOL))_AUDIO_NONE_AiUpdate;
	return TRUE;
}

// freakdave - MusyX Audio plugin
BOOL LoadAudioMusyXDll(void) {
	PLUGIN_INFO PluginInfo;
	 
	hAudioDll = (HANDLE)100;
	if (hAudioDll == NULL) {  return FALSE; }

	GetDllInfo = (void *)_AUDIO_MUSYX_GetDllInfo;
	if (GetDllInfo == NULL) { return FALSE; }

	GetDllInfo(&PluginInfo);
	if (!ValidPluginVersion(&PluginInfo) || PluginInfo.MemoryBswaped == FALSE) { return FALSE; }

	AiCloseDLL = (void *)_AUDIO_MUSYX_CloseDLL;
	if (AiCloseDLL == NULL) { return FALSE; }
	AiDacrateChanged = (void *)(int)_AUDIO_MUSYX_AiDacrateChanged;
	if (AiDacrateChanged == NULL) { return FALSE; }
	AiLenChanged = (void *)_AUDIO_MUSYX_AiLenChanged;
	if (AiLenChanged == NULL) { return FALSE; }
	AiReadLength = (DWORD ( *)(void))_AUDIO_MUSYX_AiReadLength;
	if (AiReadLength == NULL) { return FALSE; }
	InitiateAudio = (BOOL ( *)(AUDIO_INFO))_AUDIO_MUSYX_InitiateAudio;
	if (InitiateAudio == NULL) { return FALSE; }
	AiRomClosed = (void ( *)(void))_AUDIO_MUSYX_RomClosed;
	if (AiRomClosed == NULL) { return FALSE; }
	ProcessAList = (void ( *)(void))_AUDIO_MUSYX_ProcessAList;	
	if (ProcessAList == NULL) { return FALSE; }

	AiDllConfig = (void ( *)(HWND))_AUDIO_MUSYX_DllConfig;
	AiUpdate = (void ( *)(BOOL))_AUDIO_MUSYX_AiUpdate;
	return TRUE;
}


BOOL LoadControllerDll(void) {
	PLUGIN_INFO PluginInfo;
	 
	hControllerDll = (HINSTANCE)0x600;

	if (hControllerDll == NULL) {  return FALSE; }

	GetDllInfo = (void ( *)(PLUGIN_INFO *))_INPUT_GetDllInfo;
	if (GetDllInfo == NULL) { return FALSE; }

	GetDllInfo(&PluginInfo);
	if (!ValidPluginVersion(&PluginInfo)) { return FALSE; }
	ContVersion = PluginInfo.Version;

	ContCloseDLL = (void ( *)(void))_INPUT_CloseDLL;
	if (ContCloseDLL == NULL) { return FALSE; }

	if (ContVersion == 0x0100) {
		InitiateControllers_1_0 = (void ( *)(HWND, CONTROL *))_INPUT_InitiateControllers;
		if (InitiateControllers_1_0 == NULL) { return FALSE; }
	}
	if (ContVersion == 0x0101) {
		InitiateControllers_1_1 = (void ( *)(CONTROL_INFO))_INPUT_InitiateControllers;
		if (InitiateControllers_1_1 == NULL) { return FALSE; }
	}

	ControllerCommand = (void ( *)(int, BYTE *))_INPUT_ControllerCommand;
	ReadController = (void ( *)(int, BYTE *))_INPUT_ReadController;
	ContConfig = (void ( *)(HWND))_INPUT_DllConfig;

	GetKeys = (void ( *)(int, BUTTONS *))_INPUT_GetKeys;
	WM_KeyDown = (void ( *)(WPARAM,LPARAM))_INPUT_WM_KeyDown;
	WM_KeyUp = (void ( *)(WPARAM,LPARAM))_INPUT_WM_KeyUp;

	ContRomOpen = (void ( *)(void))_INPUT_RomOpen;
	ContRomClosed = (void ( *)(void))_INPUT_RomClosed; 
	return TRUE;
}

BOOL LoadGFXDll(char * RspDll) {
	PLUGIN_INFO PluginInfo;
	 
	hGfxDll = (HANDLE)1;
	if (hGfxDll == NULL) {  return FALSE; }

	GetDllInfo = (void ( *)(PLUGIN_INFO *))_VIDEO_GetDllInfo;
	if (GetDllInfo == NULL) { return FALSE; }

	GetDllInfo(&PluginInfo);
	if (!ValidPluginVersion(&PluginInfo) || PluginInfo.MemoryBswaped == FALSE) { return FALSE; }

	GFXCloseDLL = (void ( *)(void))_VIDEO_CloseDLL;
	if (GFXCloseDLL == NULL) { return FALSE; }
	ChangeWindow = (void ( *)(void))_VIDEO_ChangeWindow;
	if (ChangeWindow == NULL) { return FALSE; }
	GFXDllConfig = (void ( *)(HWND))_VIDEO_DllConfig;
	DrawScreen = (void ( *)(void))_VIDEO_DrawScreen;
	if (DrawScreen == NULL) { return FALSE; }
	InitiateGFX = (BOOL ( *)(GFX_INFO))_VIDEO_InitiateGFX;
	if (InitiateGFX == NULL) { return FALSE; }
	MoveScreen = (void ( *)(int, int))_VIDEO_MoveScreen;
	if (MoveScreen == NULL) { return FALSE; }
	ProcessDList = (void ( *)(void))_VIDEO_ProcessDList;
	if (ProcessDList == NULL) { return FALSE; }
	GfxRomClosed = (void ( *)(void))_VIDEO_RomClosed;
	if (GfxRomClosed == NULL) { return FALSE; }
	GfxRomOpen = (void ( *)(void))_VIDEO_RomOpen;
	if (GfxRomOpen == NULL) { return FALSE; }
	UpdateScreen = (void ( *)(void))_VIDEO_UpdateScreen;
	if (UpdateScreen == NULL) { return FALSE; }
	ViStatusChanged = (void ( *)(void))_VIDEO_ViStatusChanged;
	if (ViStatusChanged == NULL) { return FALSE; }
	ViWidthChanged = (void ( *)(void))_VIDEO_ViWidthChanged;
	if (ViWidthChanged == NULL) { return FALSE; }
	
	ProcessRDPList = NULL;
	CaptureScreen = NULL;
	ShowCFB = NULL;
 
	return TRUE;
}

/*BOOL LoadRSPDll(void) {
	PLUGIN_INFO PluginInfo;
 

	hRspDll = (HANDLE)1;
	if (hRspDll == NULL) {  return FALSE; }

	GetDllInfo = (void ( *)(PLUGIN_INFO *))_RSP_GetDllInfo;
	if (GetDllInfo == NULL) { return FALSE; }

	GetDllInfo(&PluginInfo);
	if (!ValidPluginVersion(&PluginInfo) || PluginInfo.MemoryBswaped == FALSE) { return FALSE; }
	RSPVersion = PluginInfo.Version;
	if (RSPVersion == 1) { RSPVersion = 0x0100; }

	DoRspCycles = (DWORD ( *)(DWORD))_RSP_DoRspCycles;
	if (DoRspCycles == NULL) { return FALSE; }
	InitiateRSP_1_0 = NULL;
	InitiateRSP_1_1 = NULL;
	if (RSPVersion == 0x100) {
		InitiateRSP_1_0 = (void *)_RSP_InitiateRSP;
		if (InitiateRSP_1_0 == NULL) { return FALSE; }
	}
	if (RSPVersion == 0x101) {
		InitiateRSP_1_1 = (void *)_RSP_InitiateRSP;
		if (InitiateRSP_1_1 == NULL) { return FALSE; }
	}
	RSPRomClosed = (void *)_RSP_RomClosed;
	if (RSPRomClosed == NULL) { return FALSE; }
	RSPCloseDLL = (void ( *)(void))_RSP_CloseDLL;
	if (RSPCloseDLL == NULL) { return FALSE; }
	
	RSPDllConfig = (void ( *)(HWND))_RSP_DllConfig;
	return TRUE;
}*/

BOOL LoadHLERSPDll(void) {
	PLUGIN_INFO PluginInfo;
 

	hRspDll = (HANDLE)1;
	if (hRspDll == NULL) {  return FALSE; }

	GetDllInfo = (void ( *)(PLUGIN_INFO *))_RSP_GetDllInfo;
	if (GetDllInfo == NULL) { return FALSE; }

	GetDllInfo(&PluginInfo);
	if (!ValidPluginVersion(&PluginInfo) || PluginInfo.MemoryBswaped == FALSE) { return FALSE; }
	RSPVersion = PluginInfo.Version;
	if (RSPVersion == 1) { RSPVersion = 0x0100; }

	DoRspCycles = (DWORD ( *)(DWORD))_RSP_DoRspCycles;
	if (DoRspCycles == NULL) { return FALSE; }
	InitiateRSP_1_0 = NULL;
	InitiateRSP_1_1 = NULL;
	if (RSPVersion == 0x100) {
		InitiateRSP_1_0 = (void *)_RSP_InitiateRSP;
		if (InitiateRSP_1_0 == NULL) { return FALSE; }
	}
	if (RSPVersion == 0x101) {
		InitiateRSP_1_1 = (void *)_RSP_InitiateRSP;
		if (InitiateRSP_1_1 == NULL) { return FALSE; }
	}
	RSPRomClosed = (void *)_RSP_RomClosed;
	if (RSPRomClosed == NULL) { return FALSE; }
	RSPCloseDLL = (void ( *)(void))_RSP_CloseDLL;
	if (RSPCloseDLL == NULL) { return FALSE; }
	
	RSPDllConfig = (void ( *)(HWND))_RSP_DllConfig;
	return TRUE;
}

void SetupPlugins (HWND hWnd) {
	 
	BOOL success = FALSE;

	ShutdownPlugins();
	GetCurrentDlls();

	if (!LoadGFXDll("")) { 
		DisplayError("Current GFX dll could not be used please selected another one");
		return;
	} else { 
		GFX_INFO GfxInfo;

		GfxInfo.MemoryBswaped = TRUE;
		GfxInfo.CheckInterrupts = CheckInterrupts;
		GfxInfo.hStatusBar = hStatusWnd;
		GfxInfo.hWnd = hWnd;
		GfxInfo.HEADER = RomHeader;
		GfxInfo.RDRAM = N64MEM;
		GfxInfo.DMEM = DMEM;
		GfxInfo.IMEM = IMEM;
		GfxInfo.MI__INTR_REG = &MI_INTR_REG;	
		GfxInfo.DPC__START_REG = &DPC_START_REG;
		GfxInfo.DPC__END_REG = &DPC_END_REG;
		GfxInfo.DPC__CURRENT_REG = &DPC_CURRENT_REG;
		GfxInfo.DPC__STATUS_REG = &DPC_STATUS_REG;
		GfxInfo.DPC__CLOCK_REG = &DPC_CLOCK_REG;
		GfxInfo.DPC__BUFBUSY_REG = &DPC_BUFBUSY_REG;
		GfxInfo.DPC__PIPEBUSY_REG = &DPC_PIPEBUSY_REG;
		GfxInfo.DPC__TMEM_REG = &DPC_TMEM_REG;
		GfxInfo.VI__STATUS_REG = &VI_STATUS_REG;
		GfxInfo.VI__ORIGIN_REG = &VI_ORIGIN_REG;
		GfxInfo.VI__WIDTH_REG = &VI_WIDTH_REG;
		GfxInfo.VI__INTR_REG = &VI_INTR_REG;
		GfxInfo.VI__V_CURRENT_LINE_REG = &VI_CURRENT_REG;
		GfxInfo.VI__TIMING_REG = &VI_TIMING_REG;
		GfxInfo.VI__V_SYNC_REG = &VI_V_SYNC_REG;
		GfxInfo.VI__H_SYNC_REG = &VI_H_SYNC_REG;
		GfxInfo.VI__LEAP_REG = &VI_LEAP_REG;
		GfxInfo.VI__H_START_REG = &VI_H_START_REG;
		GfxInfo.VI__V_START_REG = &VI_V_START_REG;
		GfxInfo.VI__V_BURST_REG = &VI_V_BURST_REG;
		GfxInfo.VI__X_SCALE_REG = &VI_X_SCALE_REG;
		GfxInfo.VI__Y_SCALE_REG = &VI_Y_SCALE_REG;
		
		if (!InitiateGFX(GfxInfo) ) {
			DisplayError("Failed to Initilize Graphics");
			return;
		}
	}


	// Ez0n3 - plugin selection to replace bools: g_bUseBasicAudio = g_bUseLLERspPlugin;
	//if (g_bUseBasicAudio) success = LoadAudioBasicDll();
	//else success = LoadAudioJttLDll();
	if (g_iAudioPlugin == _AudioPluginNone)
	{
		success = LoadAudioNoneDll();
	}
	else if (g_iAudioPlugin == _AudioPluginLleRsp)
	{
		// set audio basic so it doesn't crash
		success = LoadAudioBasicDll();
	}
	else if (g_iAudioPlugin == _AudioPluginBasic)
	{
		success = LoadAudioBasicDll();
	}
	else if (g_iAudioPlugin == _AudioPluginJttl)
	{
		success = LoadAudioJttLDll();
	}
	else if (g_iAudioPlugin == _AudioPluginAzimer)
	{
		success = LoadAudioAzimerDll();
	}
	
	else if (g_iAudioPlugin == _AudioPluginMusyX)
	{
		success = LoadAudioMusyXDll();
	}
	

	if (!success) {
		AiCloseDLL       = NULL;
		AiDacrateChanged = NULL;
		AiLenChanged     = NULL;
		AiReadLength     = NULL;
		AiUpdate         = NULL;
		InitiateAudio    = NULL;
		ProcessAList     = NULL;
		AiRomClosed      = NULL;
		//DisplayError("Could Not load Audio plugin ...");
	} else {
		AUDIO_INFO AudioInfo;
		
		AudioInfo.hwnd = hWnd;
		AudioInfo.hinst = hInst;
		AudioInfo.MemoryBswaped = TRUE;
		AudioInfo.HEADER = RomHeader;
		AudioInfo.RDRAM = N64MEM;
		AudioInfo.DMEM = DMEM;
		AudioInfo.IMEM = IMEM;
		AudioInfo.MI__INTR_REG = &AudioIntrReg;	
		AudioInfo.AI__DRAM_ADDR_REG = &AI_DRAM_ADDR_REG;	
		AudioInfo.AI__LEN_REG = &AI_LEN_REG;	
		AudioInfo.AI__CONTROL_REG = &AI_CONTROL_REG;	
		AudioInfo.AI__STATUS_REG = &AI_STATUS_REG;	
		AudioInfo.AI__DACRATE_REG = &AI_DACRATE_REG;	
		AudioInfo.AI__BITRATE_REG = &AI_BITRATE_REG;	
		AudioInfo.CheckInterrupts = AiCheckInterrupts;
		if (!InitiateAudio(AudioInfo)) {
			AiCloseDLL       = NULL;
			AiDacrateChanged = NULL;
			AiLenChanged     = NULL;
			AiReadLength     = NULL;
			AiUpdate         = NULL;
			InitiateAudio    = NULL;
			ProcessAList     = NULL;
			AiRomClosed      = NULL;
			//DisplayError("Failed to Initilize Audio");
		}
		 
	}

	if (!LoadHLERSPDll())
		return;

	{
		RSP_INFO_1_0 RspInfo10;
		RSP_INFO_1_1 RspInfo11;

		RspInfo10.CheckInterrupts = CheckInterrupts;
		RspInfo11.CheckInterrupts = CheckInterrupts;
		RspInfo10.ProcessDlist = ProcessDList;
		RspInfo11.ProcessDlist = ProcessDList;

		// Ez0n3 - use iAudioPlugin instead to determine if basic audio is used
		if (g_iAudioPlugin == _AudioPluginLleRsp) //g_bUseLLERspPlugin	//_AudioPluginBasic
		{
			RspInfo10.ProcessAlist = NULL;
			RspInfo11.ProcessAlist = NULL;
		}
		else
		{
			RspInfo10.ProcessAlist = ProcessAList;
			RspInfo11.ProcessAlist = ProcessAList;
		}

		RspInfo10.ProcessRdpList = ProcessRDPList;
		RspInfo11.ProcessRdpList = ProcessRDPList;
		RspInfo11.ShowCFB = ShowCFB;

		RspInfo10.hInst = hInst;
		RspInfo11.hInst = hInst;
		RspInfo10.RDRAM = N64MEM;
		RspInfo11.RDRAM = N64MEM;
		RspInfo10.DMEM = DMEM;
		RspInfo11.DMEM = DMEM;
		RspInfo10.IMEM = IMEM;
		RspInfo11.IMEM = IMEM;
		RspInfo10.MemoryBswaped = FALSE;
		RspInfo11.MemoryBswaped = FALSE;

		RspInfo10.MI__INTR_REG = &MI_INTR_REG;
		RspInfo11.MI__INTR_REG = &MI_INTR_REG;
			
		RspInfo10.SP__MEM_ADDR_REG = &SP_MEM_ADDR_REG;
		RspInfo11.SP__MEM_ADDR_REG = &SP_MEM_ADDR_REG;
		RspInfo10.SP__DRAM_ADDR_REG = &SP_DRAM_ADDR_REG;
		RspInfo11.SP__DRAM_ADDR_REG = &SP_DRAM_ADDR_REG;
		RspInfo10.SP__RD_LEN_REG = &SP_RD_LEN_REG;
		RspInfo11.SP__RD_LEN_REG = &SP_RD_LEN_REG;
		RspInfo10.SP__WR_LEN_REG = &SP_WR_LEN_REG;
		RspInfo11.SP__WR_LEN_REG = &SP_WR_LEN_REG;
		RspInfo10.SP__STATUS_REG = &SP_STATUS_REG;
		RspInfo11.SP__STATUS_REG = &SP_STATUS_REG;
		RspInfo10.SP__DMA_FULL_REG = &SP_DMA_FULL_REG;
		RspInfo11.SP__DMA_FULL_REG = &SP_DMA_FULL_REG;
		RspInfo10.SP__DMA_BUSY_REG = &SP_DMA_BUSY_REG;
		RspInfo11.SP__DMA_BUSY_REG = &SP_DMA_BUSY_REG;
		RspInfo10.SP__PC_REG = &SP_PC_REG;
		RspInfo11.SP__PC_REG = &SP_PC_REG;
		RspInfo10.SP__SEMAPHORE_REG = &SP_SEMAPHORE_REG;
		RspInfo11.SP__SEMAPHORE_REG = &SP_SEMAPHORE_REG;
			
		RspInfo10.DPC__START_REG = &DPC_START_REG;
		RspInfo11.DPC__START_REG = &DPC_START_REG;
		RspInfo10.DPC__END_REG = &DPC_END_REG;
		RspInfo11.DPC__END_REG = &DPC_END_REG;
		RspInfo10.DPC__CURRENT_REG = &DPC_CURRENT_REG;
		RspInfo11.DPC__CURRENT_REG = &DPC_CURRENT_REG;
		RspInfo10.DPC__STATUS_REG = &DPC_STATUS_REG;
		RspInfo11.DPC__STATUS_REG = &DPC_STATUS_REG;
		RspInfo10.DPC__CLOCK_REG = &DPC_CLOCK_REG;
		RspInfo11.DPC__CLOCK_REG = &DPC_CLOCK_REG;
		RspInfo10.DPC__BUFBUSY_REG = &DPC_BUFBUSY_REG;
		RspInfo11.DPC__BUFBUSY_REG = &DPC_BUFBUSY_REG;
		RspInfo10.DPC__PIPEBUSY_REG = &DPC_PIPEBUSY_REG;
		RspInfo11.DPC__PIPEBUSY_REG = &DPC_PIPEBUSY_REG;
		RspInfo10.DPC__TMEM_REG = &DPC_TMEM_REG;
		RspInfo11.DPC__TMEM_REG = &DPC_TMEM_REG;

		if (RSPVersion == 0x0100) { InitiateRSP_1_0(RspInfo10, &RspTaskValue); }
		if (RSPVersion == 0x0101) { InitiateRSP_1_1(RspInfo11, &RspTaskValue); }
	}
	
#if (!defined(EXTERNAL_RELEASE))
	if (HaveDebugger) {
		DEBUG_INFO DebugInfo;
		HMENU hSubMenu;
		MENUITEMINFO lpmii;

		if (GetRspDebugInfo != NULL) { GetRspDebugInfo(&RspDebug); }
		
		hSubMenu = GetSubMenu(hMenu,3);
		if (IsMenu(RspDebug.hRSPMenu)) {
			InsertMenu (hSubMenu, 3, MF_POPUP|MF_BYPOSITION, (DWORD)RspDebug.hRSPMenu, "&RSP");
			lpmii.cbSize = sizeof(MENUITEMINFO);
			lpmii.fMask = MIIM_STATE;			
			lpmii.fState = 0;
			//lpmii.fState = MFS_GRAYED;
			//SetMenuItemInfo(hSubMenu, (DWORD)RspDebug.hRSPMenu, MF_BYCOMMAND,&lpmii);
		}
		
		if (GetGfxDebugInfo != NULL) { GetGfxDebugInfo(&GFXDebug); }
		
		hSubMenu = GetSubMenu(hMenu,3);
		if (IsMenu(GFXDebug.hGFXMenu)) {
			InsertMenu (hSubMenu, 3, MF_POPUP|MF_BYPOSITION, (DWORD)GFXDebug.hGFXMenu, "&RDP");
			lpmii.cbSize = sizeof(MENUITEMINFO);
			lpmii.fMask = MIIM_STATE;
			//lpmii.fState = MFS_GRAYED;
			lpmii.fState = 0;
			//SetMenuItemInfo(hSubMenu, (DWORD)GFXDebug.hGFXMenu, MF_BYCOMMAND,&lpmii);
		}

		DebugInfo.UpdateBreakPoints = RefreshBreakPoints;
		DebugInfo.UpdateMemory = Refresh_Memory;
		DebugInfo.UpdateR4300iRegisters = UpdateCurrentR4300iRegisterPanel;
		DebugInfo.Enter_BPoint_Window = Enter_BPoint_Window;
		DebugInfo.Enter_Memory_Window = Enter_Memory_Window;
		DebugInfo.Enter_R4300i_Commands_Window = Enter_R4300i_Commands_Window;
		DebugInfo.Enter_R4300i_Register_Window = Enter_R4300i_Register_Window;
		DebugInfo.Enter_RSP_Commands_Window = RspDebug.Enter_RSP_Commands_Window;
		if (InitiateRSPDebugger != NULL) { InitiateRSPDebugger(DebugInfo); }
		if (InitiateGFXDebugger != NULL) { InitiateGFXDebugger(DebugInfo); }
	}
#endif

	if (!LoadControllerDll()) { 
		DisplayError("Current controller dll could not be used please selected another one");
	} else {
		Controllers[0].Present = TRUE;
		Controllers[0].RawData = FALSE;
		Controllers[0].Plugin  = PLUGIN_NONE;
		
		Controllers[1].Present = FALSE;
		Controllers[1].RawData = FALSE;
		Controllers[1].Plugin  = PLUGIN_NONE;
		
		Controllers[2].Present = FALSE;
		Controllers[2].RawData = FALSE;
		Controllers[2].Plugin  = PLUGIN_NONE;
		
		Controllers[3].Present = FALSE;
		Controllers[3].RawData = FALSE;
		Controllers[3].Plugin  = PLUGIN_NONE;
	
		if (ContVersion == 0x0100) {
			InitiateControllers_1_0(hWnd,Controllers);
		}
		if (ContVersion == 0x0101) {
			CONTROL_INFO ControlInfo;
			ControlInfo.Controls = Controllers;
			ControlInfo.HEADER = RomHeader;
			ControlInfo.hinst = hInst;
			ControlInfo.hMainWindow = hWnd;
			ControlInfo.MemoryBswaped = TRUE;
			InitiateControllers_1_1(ControlInfo);
		}
#ifndef EXTERNAL_RELEASE
//		Controllers[0].Plugin  = PLUGIN_RUMBLE_PAK;
#endif
	}
 
}

void SetupPluginScreen (HWND hDlg) {
 
}

void ShutdownPlugins (void) {
 
}

BOOL ValidPluginVersion ( PLUGIN_INFO * PluginInfo ) {
	switch (PluginInfo->Type) {
	case PLUGIN_TYPE_RSP: 
		if (PluginInfo->Version == 0x0001) { return TRUE; }
		if (PluginInfo->Version == 0x0100) { return TRUE; }
		if (PluginInfo->Version == 0x0101) { return TRUE; }
		break;
	case PLUGIN_TYPE_GFX:
		if (PluginInfo->Version == 0x0102) { return TRUE; }
		if (PluginInfo->Version == 0x0103) { return TRUE; }
		break;
	case PLUGIN_TYPE_AUDIO:
		if (PluginInfo->Version == 0x0101) { return TRUE; }
		break;
	case PLUGIN_TYPE_CONTROLLER:
		if (PluginInfo->Version == 0x0100) { return TRUE; }
		break;
	}
	return FALSE;
}
