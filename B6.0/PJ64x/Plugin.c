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
#include "settings.h"
#include "resource.h"

#include "../Plugins.h"

extern int loaddwMaxVideoMem();

BOOL g_bAudioBoost = FALSE;

#if defined(_VIDEO_1964_11)
#include <Video1964.h>
#elif defined(_VIDEO_RICE_510)

extern void _VIDEO_RICE_510_CloseDLL					(void);
extern void _VIDEO_RICE_510_DllConfig				( HWND hParent );
extern void _VIDEO_RICE_510_GetDllInfo				( PLUGIN_INFO *PluginInfo);
extern void _VIDEO_RICE_510_ChangeWindow				(void);
extern void _VIDEO_RICE_510_DrawScreen				(void);
extern void _VIDEO_RICE_510_FBRead					( DWORD addr );
extern void _VIDEO_RICE_510_FBWrite					( DWORD addr, DWORD Bytes );
extern BOOL _VIDEO_RICE_510_InitiateGFX				(GFX_INFO Gfx_Info);
extern void _VIDEO_RICE_510_MoveScreen				(int xpos, int ypos);
extern void _VIDEO_RICE_510_ProcessDList				(void);
extern void _VIDEO_RICE_510_RomClosed				(void);
extern void _VIDEO_RICE_510_RomOpen					(void);
extern void _VIDEO_RICE_510_UpdateScreen				(void);
extern void _VIDEO_RICE_510_ViStatusChanged			(void);
extern void _VIDEO_RICE_510_ViStatusChanged			(void);
extern void _VIDEO_RICE_510_ViWidthChanged			(void);

extern void _VIDEO_RICE_510_ProcessRDPList			(void);
extern void _VIDEO_RICE_510_CaptureScreen			(char *);
extern void _VIDEO_RICE_510_ShowCFB					(void);

extern void _VIDEO_RICE_510_SetMaxTextureMem(DWORD mem);

#elif defined(_VIDEO_RICE_531)

extern void _VIDEO_RICE_531_CloseDLL					(void);
extern void _VIDEO_RICE_531_DllConfig				( HWND hParent );
extern void _VIDEO_RICE_531_GetDllInfo				( PLUGIN_INFO *PluginInfo);
extern void _VIDEO_RICE_531_ChangeWindow				(void);
extern void _VIDEO_RICE_531_DrawScreen				(void);
extern void _VIDEO_RICE_531_FBRead					( DWORD addr );
extern void _VIDEO_RICE_531_FBWrite					( DWORD addr, DWORD Bytes );
extern BOOL _VIDEO_RICE_531_InitiateGFX				(GFX_INFO Gfx_Info);
extern void _VIDEO_RICE_531_MoveScreen				(int xpos, int ypos);
extern void _VIDEO_RICE_531_ProcessDList				(void);
extern void _VIDEO_RICE_531_RomClosed				(void);
extern void _VIDEO_RICE_531_RomOpen					(void);
extern void _VIDEO_RICE_531_UpdateScreen				(void);
extern void _VIDEO_RICE_531_ViStatusChanged			(void);
extern void _VIDEO_RICE_531_ViStatusChanged			(void);
extern void _VIDEO_RICE_531_ViWidthChanged			(void);

extern void _VIDEO_RICE_531_ProcessRDPList			(void);
extern void _VIDEO_RICE_531_CaptureScreen			(char *);
extern void _VIDEO_RICE_531_ShowCFB					(void);

extern void _VIDEO_RICE_531_SetMaxTextureMem(DWORD mem);

#elif defined(_VIDEO_RICE_560)

extern void _VIDEO_RICE_560_CloseDLL					(void);
extern void _VIDEO_RICE_560_DllConfig				( HWND hParent );
extern void _VIDEO_RICE_560_GetDllInfo				( PLUGIN_INFO *PluginInfo);
extern void _VIDEO_RICE_560_ChangeWindow				(void);
extern void _VIDEO_RICE_560_DrawScreen				(void);
extern void _VIDEO_RICE_560_FBRead					( DWORD addr );
extern void _VIDEO_RICE_560_FBWrite					( DWORD addr, DWORD Bytes );
extern BOOL _VIDEO_RICE_560_InitiateGFX				(GFX_INFO Gfx_Info);
extern void _VIDEO_RICE_560_MoveScreen				(int xpos, int ypos);
extern void _VIDEO_RICE_560_ProcessDList				(void);
extern void _VIDEO_RICE_560_RomClosed				(void);
extern void _VIDEO_RICE_560_RomOpen					(void);
extern void _VIDEO_RICE_560_UpdateScreen				(void);
extern void _VIDEO_RICE_560_ViStatusChanged			(void);
extern void _VIDEO_RICE_560_ViStatusChanged			(void);
extern void _VIDEO_RICE_560_ViWidthChanged			(void);

extern void _VIDEO_RICE_560_ProcessRDPList			(void);
extern void _VIDEO_RICE_560_CaptureScreen			(char *);
extern void _VIDEO_RICE_560_ShowCFB					(void);

extern void _VIDEO_RICE_560_SetMaxTextureMem(DWORD mem);

#elif defined(_VIDEO_RICE_611)

extern void _VIDEO_RICE_611_CloseDLL					(void);
extern void _VIDEO_RICE_611_DllConfig				( HWND hParent );
extern void _VIDEO_RICE_611_GetDllInfo				( PLUGIN_INFO *PluginInfo);
extern void _VIDEO_RICE_611_ChangeWindow				(void);
extern void _VIDEO_RICE_611_DrawScreen				(void);
extern void _VIDEO_RICE_611_FBRead					( DWORD addr );
extern void _VIDEO_RICE_611_FBWrite					( DWORD addr, DWORD Bytes );
extern BOOL _VIDEO_RICE_611_InitiateGFX				(GFX_INFO Gfx_Info);
extern void _VIDEO_RICE_611_MoveScreen				(int xpos, int ypos);
extern void _VIDEO_RICE_611_ProcessDList				(void);
extern void _VIDEO_RICE_611_RomClosed				(void);
extern void _VIDEO_RICE_611_RomOpen					(void);
extern void _VIDEO_RICE_611_UpdateScreen				(void);
extern void _VIDEO_RICE_611_ViStatusChanged			(void);
extern void _VIDEO_RICE_611_ViStatusChanged			(void);
extern void _VIDEO_RICE_611_ViWidthChanged			(void);

extern void _VIDEO_RICE_611_ProcessRDPList			(void);
extern void _VIDEO_RICE_611_CaptureScreen			(char *);
extern void _VIDEO_RICE_611_ShowCFB					(void);

extern void _VIDEO_RICE_611_SetMaxTextureMem(DWORD mem);

#elif defined(_VIDEO_RICE_612)
#include <VideoRice612.h>
/*
extern void _VIDEO_RICE_612_CloseDLL					(void);
extern void _VIDEO_RICE_612_DllConfig				( HWND hParent );
extern void _VIDEO_RICE_612_GetDllInfo				( PLUGIN_INFO *PluginInfo);
extern void _VIDEO_RICE_612_ChangeWindow				(void);
extern void _VIDEO_RICE_612_DrawScreen				(void);
extern void _VIDEO_RICE_612_FBRead					( DWORD addr );
extern void _VIDEO_RICE_612_FBWrite					( DWORD addr, DWORD Bytes );
extern BOOL _VIDEO_RICE_612_InitiateGFX				(GFX_INFO Gfx_Info);
extern void _VIDEO_RICE_612_MoveScreen				(int xpos, int ypos);
extern void _VIDEO_RICE_612_ProcessDList				(void);
extern void _VIDEO_RICE_612_RomClosed				(void);
extern void _VIDEO_RICE_612_RomOpen					(void);
extern void _VIDEO_RICE_612_UpdateScreen				(void);
extern void _VIDEO_RICE_612_ViStatusChanged			(void);
extern void _VIDEO_RICE_612_ViStatusChanged			(void);
extern void _VIDEO_RICE_612_ViWidthChanged			(void);

extern void _VIDEO_RICE_612_ProcessRDPList			(void);
extern void _VIDEO_RICE_612_CaptureScreen			(char *);
extern void _VIDEO_RICE_612_ShowCFB					(void);

extern void _VIDEO_RICE_612_SetMaxTextureMem(DWORD mem);
*/
#endif

void (*_VIDEO_LINK_CloseDLL)			(void)						= NULL;
void (*_VIDEO_LINK_DllConfig)		( HWND hParent )			= NULL;
void (*_VIDEO_LINK_GetDllInfo)		( PLUGIN_INFO *PluginInfo)	= NULL;
void (*_VIDEO_LINK_ChangeWindow)		(void)						= NULL;
void (*_VIDEO_LINK_DrawScreen)		(void)						= NULL;
void (*_VIDEO_LINK_FBRead)			( DWORD addr )				= NULL;
void (*_VIDEO_LINK_FBWrite)			( DWORD addr, DWORD Bytes ) = NULL;
BOOL (*_VIDEO_LINK_InitiateGFX)		(GFX_INFO Gfx_Info)			= NULL;
void (*_VIDEO_LINK_MoveScreen)		(int xpos, int ypos)		= NULL;
void (*_VIDEO_LINK_ProcessDList)		(void)						= NULL;
void (*_VIDEO_LINK_RomClosed)		(void)						= NULL;
void (*_VIDEO_LINK_RomOpen)			(void)						= NULL;
void (*_VIDEO_LINK_UpdateScreen)		(void)						= NULL;
void (*_VIDEO_LINK_ViStatusChanged)	(void)						= NULL;
//void (*_VIDEO_LINK_ViStatusChanged)	(void)						= NULL;
void (*_VIDEO_LINK_ViWidthChanged)	(void)						= NULL;

void (*_VIDEO_LINK_ProcessRDPList)	(void)						= NULL;
void (*_VIDEO_LINK_CaptureScreen)	(char *)					= NULL;
void (*_VIDEO_LINK_ShowCFB)			(void)						= NULL;

void (*_VIDEO_LINK_SetMaxTextureMem)	(DWORD mem)					= NULL;



#if defined(USE_MUSYX)
// freakdave - New MusyX audio plugin
extern void _AUDIO_MUSYX_CloseDLL			(void);
extern void _AUDIO_MUSYX_RomClosed			(void);
extern void _AUDIO_MUSYX_DllConfig			( HWND hParent );
extern void _AUDIO_MUSYX_GetDllInfo			(PLUGIN_INFO *PluginInfo);
extern void _AUDIO_MUSYX_AiDacrateChanged	(int SystemType);
extern void _AUDIO_MUSYX_AiLenChanged		(void);
extern void _AUDIO_MUSYX_AiReadLength		(void);
extern void _AUDIO_MUSYX_AiUpdate			(BOOL Wait);
extern BOOL _AUDIO_MUSYX_InitiateAudio		(AUDIO_INFO Audio_Info);
extern void _AUDIO_MUSYX_ProcessAList		(void);
extern void _AUDIO_MUSYX_AudioBoost			(BOOL Boost);
#else

// mupen 1.5 audio
/*
//extern void _AUDIO_M64P_CloseDLL			(void);
extern void _AUDIO_M64P_RomClosed			(void);
//extern void _AUDIO_M64P_DllConfig			( HWND hParent );
//extern void _AUDIO_M64P_GetDllInfo			(PLUGIN_INFO *PluginInfo);
extern void _AUDIO_M64P_AiDacrateChanged	(int SystemType);
extern void _AUDIO_M64P_AiLenChanged		(void);
//extern void _AUDIO_M64P_AiReadLength		(void);
//extern void _AUDIO_M64P_AiUpdate			(BOOL Wait);
extern BOOL _AUDIO_M64P_InitiateAudio		(AUDIO_INFO Audio_Info);
extern void _AUDIO_M64P_ProcessAList		(void);
//extern void _AUDIO_M64P_AudioBoost			(BOOL Boost);
*/
//freakdave - JttL
extern void _AUDIO_JTTL_CloseDLL			(void);
extern void _AUDIO_JTTL_RomClosed			(void);
extern void _AUDIO_JTTL_DllConfig			( HWND hParent );
extern void _AUDIO_JTTL_GetDllInfo			(PLUGIN_INFO *PluginInfo);
extern void _AUDIO_JTTL_AiDacrateChanged	(int SystemType);
extern void _AUDIO_JTTL_AiLenChanged		(void);
extern void _AUDIO_JTTL_AiReadLength		(void);
extern void _AUDIO_JTTL_AiUpdate			(BOOL Wait);
extern BOOL _AUDIO_JTTL_InitiateAudio		(AUDIO_INFO Audio_Info);
extern void _AUDIO_JTTL_ProcessAList		(void);
extern void _AUDIO_JTTL_AudioBoost			(BOOL Boost);

// Basic Audio
extern void _AUDIO_BASIC_CloseDLL			(void);
extern void _AUDIO_BASIC_RomClosed			(void);
extern void _AUDIO_BASIC_DllConfig			( HWND hParent );
extern void _AUDIO_BASIC_GetDllInfo			(PLUGIN_INFO *PluginInfo);
extern void _AUDIO_BASIC_AiDacrateChanged	(int SystemType);
extern void _AUDIO_BASIC_AiLenChanged		(void);
extern void _AUDIO_BASIC_AiReadLength		(void);
extern void _AUDIO_BASIC_AiUpdate			(BOOL Wait);
extern BOOL _AUDIO_BASIC_InitiateAudio		(AUDIO_INFO Audio_Info);
extern void _AUDIO_BASIC_ProcessAList		(void);

// Ez0n3 - No Audio
extern void _AUDIO_NONE_CloseDLL			(void);
extern void _AUDIO_NONE_RomClosed			(void);
extern void _AUDIO_NONE_DllConfig			( HWND hParent );
extern void _AUDIO_NONE_GetDllInfo			(PLUGIN_INFO *PluginInfo);
extern void _AUDIO_NONE_AiDacrateChanged	(int SystemType);
extern void _AUDIO_NONE_AiLenChanged		(void);
extern void _AUDIO_NONE_AiReadLength		(void);
extern void _AUDIO_NONE_AiUpdate			(BOOL Wait);
extern BOOL _AUDIO_NONE_InitiateAudio		(AUDIO_INFO Audio_Info);
extern void _AUDIO_NONE_ProcessAList		(void);

// freakdave - readded Azimer
extern void _AUDIO_AZIMER_CloseDLL			(void);
extern void _AUDIO_AZIMER_RomClosed			(void);
extern void _AUDIO_AZIMER_DllConfig			( HWND hParent );
extern void _AUDIO_AZIMER_GetDllInfo		(PLUGIN_INFO *PluginInfo);
extern void _AUDIO_AZIMER_AiDacrateChanged	(int SystemType);
extern void _AUDIO_AZIMER_AiLenChanged		(void);
extern void _AUDIO_AZIMER_AiReadLength		(void);
extern void _AUDIO_AZIMER_AiUpdate			(BOOL Wait);
extern BOOL _AUDIO_AZIMER_InitiateAudio		(AUDIO_INFO Audio_Info);
extern void _AUDIO_AZIMER_ProcessAList		(void);
extern void _AUDIO_AZIMER_AudioBoost		(BOOL Boost);

#endif

void (*_AUDIO_LINK_AiDacrateChanged)		(int SystemType)			= NULL;
void (*_AUDIO_LINK_AiLenChanged)			(void)						= NULL;
//DWORD (*_AUDIO_LINK_AiReadLength)			(void)						= NULL;
void (*_AUDIO_LINK_AiReadLength)			(void)						= NULL;
void (*_AUDIO_LINK_AiUpdate)				(BOOL Wait)					= NULL;
void (*_AUDIO_LINK_CloseDLL)				(void)						= NULL;
//void (*_AUDIO_LINK_DllAbout)				(HWND hParent)				= NULL;
void (*_AUDIO_LINK_DllConfig)				(HWND hParent)				= NULL;
//void (*_AUDIO_LINK_DllTest)				(HWND hParent)				= NULL;
void (*_AUDIO_LINK_GetDllInfo)				(PLUGIN_INFO *PluginInfo)	= NULL;
BOOL (*_AUDIO_LINK_InitiateAudio)			(AUDIO_INFO Audio_Info)		= NULL;
void (*_AUDIO_LINK_ProcessAList)			(void)						= NULL;
void (*_AUDIO_LINK_RomClosed)				(void)						= NULL;

void (*_AUDIO_LINK_AudioBoost)				(BOOL Boost)				= NULL;

extern void _RSP_GetDllInfo					( PLUGIN_INFO * PluginInfo );
extern void _RSP_RomClosed					(void);
extern void _RSP_CloseDLL					(void);
extern void _RSP_DllConfig					(HWND hWnd);
extern DWORD _RSP_DoRspCycles				(DWORD Cycles);
extern void _RSP_InitiateRSP				( RSP_INFO_1_1 Rsp_Info, DWORD * CycleCount);

extern void _RSP_HLE_GetDllInfo				( PLUGIN_INFO * PluginInfo );
extern void _RSP_HLE_RomClosed				(void);
extern void _RSP_HLE_CloseDLL				(void);
extern void _RSP_HLE_DllConfig				(HWND hWnd);
extern DWORD _RSP_HLE_DoRspCycles			(DWORD Cycles);
extern void	_RSP_HLE_InitiateRSP			( RSP_INFO_1_1 Rsp_Info, DWORD * CycleCount);

extern void _RSP_M64p_GetDllInfo			( PLUGIN_INFO * PluginInfo );
extern void _RSP_M64p_RomClosed				(void);
extern void _RSP_M64p_CloseDLL				(void);
extern void _RSP_M64p_DllConfig				(HWND hWnd);
extern DWORD _RSP_M64p_DoRspCycles			(DWORD Cycles);
extern void _RSP_M64p_InitiateRSP			( RSP_INFO_1_1 Rsp_Info, DWORD * CycleCount);

extern void _RSP_CXD4_GetDllInfo			( PLUGIN_INFO * PluginInfo );
extern void _RSP_CXD4_RomClosed				(void);
extern void _RSP_CXD4_CloseDLL				(void);
extern void _RSP_CXD4_DllConfig				(HWND hWnd);
extern DWORD _RSP_CXD4_DoRspCycles			(DWORD Cycles);
extern void _RSP_CXD4_InitiateRSP			( RSP_INFO_1_1 Rsp_Info, DWORD * CycleCount);

DWORD (*_RSP_LINK_DoRspCycles)				(DWORD)						= NULL;
void (*_RSP_LINK_CloseDLL)					(void)						= NULL;
void (*_RSP_LINK_RomClosed)					(void)						= NULL;
void (*_RSP_LINK_DllConfig)					(HWND hWnd)					= NULL;
//void (*_RSP_LINK_DllAbout)				(HWND hWnd)					= NULL;
void (*_RSP_LINK_GetDllInfo)				(PLUGIN_INFO *PluginInfo)	= NULL;
//void (*_RSP_LINK_InitiateRSP_1_0)			(RSP_INFO_1_0 Rsp_Info, DWORD * CycleCount) = NULL; //Cycles
void (*_RSP_LINK_InitiateRSP_1_1)			(RSP_INFO_1_1 Rsp_Info, DWORD * CycleCount) = NULL; //Cycles

extern void _INPUT_CloseDLL					(void);
extern void _INPUT_ControllerCommand		(int Control, BYTE *Command);
extern void _INPUT_DllConfig				(HWND hParent);
extern void _INPUT_GetDllInfo				(PLUGIN_INFO *PluginInfo);
extern void _INPUT_GetKeys					(int Control, BUTTONS *Keys);
extern void _INPUT_InitiateControllers		(HWND hMainWindow, CONTROL Controls[]);
extern void _INPUT_ReadController			(int Control, BYTE *Command);
extern void _INPUT_RomClosed				(void);
extern void _INPUT_RomOpen					(void);
extern void _INPUT_WM_KeyDown				(WPARAM wParam, LPARAM lParam);
extern void _INPUT_WM_KeyUp					(WPARAM wParam, LPARAM lParam);
extern void _INPUT_SetRumble				(int Control, BOOL bRumble );
 
HINSTANCE hAudioDll, hControllerDll, hGfxDll, hRspDll;
DWORD PluginCount, RspTaskValue, AudioIntrReg;
WORD RSPVersion,ContVersion;
HANDLE hAudioThread = NULL;
CONTROL Controllers[4];
//BOOL PluginsInitilized = FALSE;

BOOL g_bUseRspAudio = FALSE; // control a listing

int g_iRspPlugin = _RSPPluginHLE; // default hle

#if defined(USE_MUSYX)
int g_iAudioPlugin = _AudioPluginMusyX; // set default to musyx
#elif defined(USE_M64PAUDIO)
int g_iAudioPlugin = _AudioPluginM64p; // set default to musyx
#else
int g_iAudioPlugin = _AudioPluginJttl; // set default to jttl
#endif

BOOL PluginsChanged ( HWND hDlg );
BOOL ValidPluginVersion ( PLUGIN_INFO * PluginInfo );


/*void AudioThread (void) {
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL );
	for (;;) { AiUpdate(TRUE); }
}*/

void GetCurrentDlls (void) {}

void GetPluginDir( char * Directory ) {}

void GetSnapShotDir( char * Directory ) {}

BOOL LoadAudioDll(void) {
	PLUGIN_INFO PluginInfo;

	hAudioDll = (HANDLE)100;
	if (hAudioDll == NULL) {  return FALSE; }
#if defined(USE_MUSYX)
	// freakdave - MusyX Audio plugin
	//if (g_iAudioPlugin == _AudioPluginSchibo)
	{
		_AUDIO_LINK_AiDacrateChanged 	= _AUDIO_MUSYX_AiDacrateChanged;
		_AUDIO_LINK_AiLenChanged	 	= _AUDIO_MUSYX_AiLenChanged;
		_AUDIO_LINK_AiReadLength	 	= _AUDIO_MUSYX_AiReadLength;
		_AUDIO_LINK_AiUpdate		 	= _AUDIO_MUSYX_AiUpdate;
		_AUDIO_LINK_CloseDLL		 	= _AUDIO_MUSYX_CloseDLL;
		//_AUDIO_LINK_DllAbout		 	= _AUDIO_MUSYX_DllAbout;
		_AUDIO_LINK_DllConfig		 	= _AUDIO_MUSYX_DllConfig;
		//_AUDIO_LINK_DllTest			 	= _AUDIO_MUSYX_DllTest;
		_AUDIO_LINK_GetDllInfo		 	= _AUDIO_MUSYX_GetDllInfo;
		_AUDIO_LINK_InitiateAudio	 	= _AUDIO_MUSYX_InitiateAudio;
		_AUDIO_LINK_ProcessAList	 	= _AUDIO_MUSYX_ProcessAList;
		_AUDIO_LINK_RomClosed		 	= _AUDIO_MUSYX_RomClosed;
		
		_AUDIO_LINK_AudioBoost		 	= _AUDIO_MUSYX_AudioBoost;
	}
#else
	/*
	if (g_iAudioPlugin == _AudioPluginM64P) // so it doesn't break for now
	{
		// Mupen 1.5 Audio plugin
		_AUDIO_LINK_AiDacrateChanged 	= _AUDIO_M64P_AiDacrateChanged;
		_AUDIO_LINK_AiLenChanged	 	= _AUDIO_M64P_AiLenChanged;
		//_AUDIO_LINK_AiReadLength	 	= _AUDIO_M64P_AiReadLength;
		//_AUDIO_LINK_AiUpdate		 	= _AUDIO_M64P_AiUpdate;
		//_AUDIO_LINK_CloseDLL		 	= _AUDIO_M64P_CloseDLL;
		//_AUDIO_LINK_DllAbout		 	= _AUDIO_M64P_DllAbout;
		//_AUDIO_LINK_DllConfig		 	= _AUDIO_M64P_DllConfig;
		//_AUDIO_LINK_DllTest			 	= _AUDIO_M64P_DllTest;
		//_AUDIO_LINK_GetDllInfo		 	= _AUDIO_M64P_GetDllInfo;
		_AUDIO_LINK_InitiateAudio	 	= _AUDIO_M64P_InitiateAudio;
		_AUDIO_LINK_ProcessAList	 	= _AUDIO_M64P_ProcessAList;
		_AUDIO_LINK_RomClosed		 	= _AUDIO_M64P_RomClosed;
		//_AUDIO_LINK_ProcessAListCountCycles = _AUDIO_MUSYX_ProcessAListCountCycles;
		
		//_AUDIO_LINK_AudioBoost		 	= _AUDIO_M64P_AudioBoost;
	}
	*/
	if (g_iAudioPlugin == _AudioPluginNone)
	{
		// Ez0n3 - No Audio (declared in plugin.h include)
		_AUDIO_LINK_AiDacrateChanged 	= _AUDIO_NONE_AiDacrateChanged;
		_AUDIO_LINK_AiLenChanged	 	= _AUDIO_NONE_AiLenChanged;
		_AUDIO_LINK_AiReadLength	 	= _AUDIO_NONE_AiReadLength;
		_AUDIO_LINK_AiUpdate		 	= _AUDIO_NONE_AiUpdate;
		_AUDIO_LINK_CloseDLL		 	= _AUDIO_NONE_CloseDLL;
		//_AUDIO_LINK_DllAbout		 	= _AUDIO_NONE_DllAbout;
		_AUDIO_LINK_DllConfig		 	= _AUDIO_NONE_DllConfig;
		//_AUDIO_LINK_DllTest			 	= _AUDIO_NONE_DllTest;
		_AUDIO_LINK_GetDllInfo		 	= _AUDIO_NONE_GetDllInfo;
		_AUDIO_LINK_InitiateAudio	 	= _AUDIO_NONE_InitiateAudio;
		_AUDIO_LINK_ProcessAList	 	= _AUDIO_NONE_ProcessAList;
		_AUDIO_LINK_RomClosed		 	= _AUDIO_NONE_RomClosed;
	}
	/*else if (g_iAudioPlugin == _AudioPluginLleRsp)
	{
		// deprecated
	}*/
	else if (g_iAudioPlugin == _AudioPluginBasic)
	{
		_AUDIO_LINK_AiDacrateChanged 	= _AUDIO_BASIC_AiDacrateChanged;
		_AUDIO_LINK_AiLenChanged	 	= _AUDIO_BASIC_AiLenChanged;
		_AUDIO_LINK_AiReadLength	 	= _AUDIO_BASIC_AiReadLength;
		_AUDIO_LINK_AiUpdate		 	= _AUDIO_BASIC_AiUpdate;
		_AUDIO_LINK_CloseDLL		 	= _AUDIO_BASIC_CloseDLL;
		//_AUDIO_LINK_DllAbout		 	= _AUDIO_BASIC_DllAbout;
		_AUDIO_LINK_DllConfig		 	= _AUDIO_BASIC_DllConfig;
		//_AUDIO_LINK_DllTest			 	= _AUDIO_BASIC_DllTest;
		_AUDIO_LINK_GetDllInfo		 	= _AUDIO_BASIC_GetDllInfo;
		_AUDIO_LINK_InitiateAudio	 	= _AUDIO_BASIC_InitiateAudio;
		_AUDIO_LINK_ProcessAList	 	= _AUDIO_BASIC_ProcessAList;
		_AUDIO_LINK_RomClosed		 	= _AUDIO_BASIC_RomClosed;
	}
	else if (g_iAudioPlugin == _AudioPluginJttl)
	{
		//freakdave - JttL 1.2
		_AUDIO_LINK_AiDacrateChanged 	= _AUDIO_JTTL_AiDacrateChanged;
		_AUDIO_LINK_AiLenChanged	 	= _AUDIO_JTTL_AiLenChanged;
		_AUDIO_LINK_AiReadLength	 	= _AUDIO_JTTL_AiReadLength;
		_AUDIO_LINK_AiUpdate		 	= _AUDIO_JTTL_AiUpdate;
		_AUDIO_LINK_CloseDLL		 	= _AUDIO_JTTL_CloseDLL;
		//_AUDIO_LINK_DllAbout		 	= _AUDIO_JTTL_DllAbout;
		_AUDIO_LINK_DllConfig		 	= _AUDIO_JTTL_DllConfig;
		//_AUDIO_LINK_DllTest			 	= _AUDIO_JTTL_DllTest;
		_AUDIO_LINK_GetDllInfo		 	= _AUDIO_JTTL_GetDllInfo;
		_AUDIO_LINK_InitiateAudio	 	= _AUDIO_JTTL_InitiateAudio;
		_AUDIO_LINK_ProcessAList	 	= _AUDIO_JTTL_ProcessAList;
		_AUDIO_LINK_RomClosed		 	= _AUDIO_JTTL_RomClosed;
		
		_AUDIO_LINK_AudioBoost		 	= _AUDIO_JTTL_AudioBoost;
	}
	else if (g_iAudioPlugin == _AudioPluginAzimer)
	{
		//freakdave - readded Azimer plugin from Surreal 1.0 and FDB
		_AUDIO_LINK_AiDacrateChanged 	= _AUDIO_AZIMER_AiDacrateChanged;
		_AUDIO_LINK_AiLenChanged	 	= _AUDIO_AZIMER_AiLenChanged;
		_AUDIO_LINK_AiReadLength	 	= _AUDIO_AZIMER_AiReadLength;
		_AUDIO_LINK_AiUpdate		 	= _AUDIO_AZIMER_AiUpdate;
		_AUDIO_LINK_CloseDLL		 	= _AUDIO_AZIMER_CloseDLL;
		//_AUDIO_LINK_DllAbout		 	= _AUDIO_AZIMER_DllAbout;
		_AUDIO_LINK_DllConfig		 	= _AUDIO_AZIMER_DllConfig;
		//_AUDIO_LINK_DllTest			 	= _AUDIO_AZIMER_DllTest;
		_AUDIO_LINK_GetDllInfo		 	= _AUDIO_AZIMER_GetDllInfo;
		_AUDIO_LINK_InitiateAudio	 	= _AUDIO_AZIMER_InitiateAudio;
		_AUDIO_LINK_ProcessAList	 	= _AUDIO_AZIMER_ProcessAList;
		_AUDIO_LINK_RomClosed		 	= _AUDIO_AZIMER_RomClosed;
		
		_AUDIO_LINK_AudioBoost		 	= _AUDIO_AZIMER_AudioBoost;
	}

#endif

	__try
	{
		_AUDIO_LINK_AudioBoost(g_bAudioBoost);
	}
	__except(NULL, EXCEPTION_EXECUTE_HANDLER)
	{
	}

	GetDllInfo = (void *)_AUDIO_LINK_GetDllInfo;
	if (GetDllInfo == NULL) { return FALSE; }

	GetDllInfo(&PluginInfo);
	if (!ValidPluginVersion(&PluginInfo) || PluginInfo.MemoryBswaped == FALSE) { return FALSE; }

	AiCloseDLL = (void *)_AUDIO_LINK_CloseDLL;
	if (AiCloseDLL == NULL) { return FALSE; }
	AiDacrateChanged = (void *)(int)_AUDIO_LINK_AiDacrateChanged;
	if (AiDacrateChanged == NULL) { return FALSE; }
	AiLenChanged = (void *)_AUDIO_LINK_AiLenChanged;
	if (AiLenChanged == NULL) { return FALSE; }
	AiReadLength = (DWORD ( *)(void))_AUDIO_LINK_AiReadLength;
	if (AiReadLength == NULL) { return FALSE; }
	InitiateAudio = (BOOL ( *)(AUDIO_INFO))_AUDIO_LINK_InitiateAudio;
	if (InitiateAudio == NULL) { return FALSE; }
	AiRomClosed = (void ( *)(void))_AUDIO_LINK_RomClosed;
	if (AiRomClosed == NULL) { return FALSE; }
	ProcessAList = (void ( *)(void))_AUDIO_LINK_ProcessAList;	
	if (ProcessAList == NULL) { return FALSE; }

	AiDllConfig = (void ( *)(HWND))_AUDIO_LINK_DllConfig;
	AiUpdate = (void ( *)(BOOL))_AUDIO_LINK_AiUpdate;
	
	{
		_AUDIO_LINK_AiDacrateChanged 	= NULL;
		_AUDIO_LINK_AiLenChanged	 	= NULL;
		_AUDIO_LINK_AiReadLength	 	= NULL;
		_AUDIO_LINK_AiUpdate		 	= NULL;
		_AUDIO_LINK_CloseDLL		 	= NULL;
		//_AUDIO_LINK_DllAbout		 	= NULL;
		_AUDIO_LINK_DllConfig		 	= NULL;
		//_AUDIO_LINK_DllTest			 	= NULL;
		_AUDIO_LINK_GetDllInfo		 	= NULL;
		_AUDIO_LINK_InitiateAudio	 	= NULL;
		_AUDIO_LINK_ProcessAList	 	= NULL;
		_AUDIO_LINK_RomClosed		 	= NULL;
		
		_AUDIO_LINK_AudioBoost		 	= NULL;
	}
	
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
	RumbleCommand = (BOOL ( *)(int,BOOL))_INPUT_SetRumble;
	return TRUE;
}

BOOL LoadGFXDll(char * RspDll) {
	PLUGIN_INFO PluginInfo;
		 
	hGfxDll = (HANDLE)1;
	if (hGfxDll == NULL) {  return FALSE; }


#if defined(_VIDEO_1964_11)
	GfxPluginVersion						= 0x0103;
	_VIDEO_InitiateGFX						= _VIDEO_1964_11_InitiateGFX;
	_VIDEO_ProcessDList						= _VIDEO_1964_11_ProcessDList;
	_VIDEO_ProcessDList_Count_Cycles		= _VIDEO_1964_11_ProcessDListCountCycles;
	_VIDEO_RomOpen							= _VIDEO_1964_11_RomOpen;
	_VIDEO_RomClosed						= _VIDEO_1964_11_RomClosed;
	_VIDEO_DllClose							= _VIDEO_1964_11_CloseDLL;
	_VIDEO_UpdateScreen						= _VIDEO_1964_11_UpdateScreen;
	_VIDEO_GetDllInfo						= _VIDEO_1964_11_GetDllInfo;
	//_VIDEO_ExtraChangeResolution			= 
	_VIDEO_DllConfig						= _VIDEO_1964_11_DllConfig;
	_VIDEO_Test								= _VIDEO_1964_11_DllTest;
	_VIDEO_About							= _VIDEO_1964_11_DllAbout;
	_VIDEO_MoveScreen						= _VIDEO_1964_11_MoveScreen;
	_VIDEO_DrawScreen						= _VIDEO_1964_11_DrawScreen;
	_VIDEO_ViStatusChanged					= _VIDEO_1964_11_ViStatusChanged;
	_VIDEO_ViWidthChanged					= _VIDEO_1964_11_ViWidthChanged;
	//_VIDEO_ChangeWindow					= _VIDEO_1964_11_ChangeWindow;
	
	_VIDEO_ChangeWindow_1_3					= _VIDEO_1964_11_ChangeWindow;
	//_VIDEO_CaptureScreen					= _VIDEO_1964_11_CaptureScreen;
	_VIDEO_ProcessRDPList					= _VIDEO_1964_11_ProcessRDPList;
	//_VIDEO_ShowCFB						= _VIDEO_1964_11_ShowCFB;
	
	_VIDEO_FrameBufferWrite					= _VIDEO_1964_11_FBWrite;
	_VIDEO_FrameBufferWriteList				= _VIDEO_1964_11_FBWList;
	_VIDEO_FrameBufferRead					= _VIDEO_1964_11_FBRead;
	_VIDEO_GetFrameBufferInfo				= _VIDEO_1964_11_FBGetFrameBufferInfo;
	//_VIDEO_SetOnScreenText				= _VIDEO_1964_11_SetOnScreenText;
	//_VIDEO_GetFullScreenStatus			= _VIDEO_1964_11_GetFullScreenStatus;
	
	_VIDEO_SetMaxTextureMem					= _VIDEO_1964_11_SetMaxTextureMem;

#elif defined(_VIDEO_RICE_510)

	
	//GfxPluginVersion						= 0x0102;
	_VIDEO_LINK_InitiateGFX						= _VIDEO_RICE_510_InitiateGFX;
	_VIDEO_LINK_ProcessDList						= _VIDEO_RICE_510_ProcessDList;
	//_VIDEO_LINK_ProcessDList_Count_Cycles		= _VIDEO_RICE_510_ProcessDListCountCycles;
	_VIDEO_LINK_RomOpen							= _VIDEO_RICE_510_RomOpen;
	_VIDEO_LINK_RomClosed						= _VIDEO_RICE_510_RomClosed;
	//_VIDEO_LINK_DllClose							= _VIDEO_RICE_510_CloseDLL;
	_VIDEO_LINK_UpdateScreen						= _VIDEO_RICE_510_UpdateScreen;
	_VIDEO_LINK_GetDllInfo						= _VIDEO_RICE_510_GetDllInfo;
	//_VIDEO_LINK_ExtraChangeResolution			= 
	_VIDEO_LINK_DllConfig						= _VIDEO_RICE_510_DllConfig;
	//_VIDEO_LINK_Test								= _VIDEO_RICE_510_DllTest;
	//_VIDEO_LINK_About							= _VIDEO_RICE_510_DllAbout;
	_VIDEO_LINK_MoveScreen						= _VIDEO_RICE_510_MoveScreen;
	_VIDEO_LINK_DrawScreen						= _VIDEO_RICE_510_DrawScreen;
	_VIDEO_LINK_ViStatusChanged					= _VIDEO_RICE_510_ViStatusChanged;
	_VIDEO_LINK_ViWidthChanged					= _VIDEO_RICE_510_ViWidthChanged;
	//_VIDEO_LINK_ChangeWindow					= _VIDEO_RICE_510_ChangeWindow;
	
	//_VIDEO_LINK_ChangeWindow_1_3					= _VIDEO_RICE_510_ChangeWindow;
	//_VIDEO_LINK_CaptureScreen					= _VIDEO_RICE_510_CaptureScreen;
	_VIDEO_LINK_ProcessRDPList					= _VIDEO_RICE_510_ProcessRDPList;
	//_VIDEO_LINK_ShowCFB						= _VIDEO_RICE_510_ShowCFB;
	
	//_VIDEO_LINK_FrameBufferWrite					= _VIDEO_RICE_510_FBWrite;
	//_VIDEO_LINK_FrameBufferWriteList				= _VIDEO_RICE_510_FBWList;
	//_VIDEO_LINK_FrameBufferRead					= _VIDEO_RICE_510_FBRead;
	//_VIDEO_LINK_GetFrameBufferInfo				= _VIDEO_RICE_510_FBGetFrameBufferInfo;
	//_VIDEO_LINK_SetOnScreenText				= _VIDEO_RICE_510_SetOnScreenText;
	//_VIDEO_LINK_GetFullScreenStatus			= _VIDEO_RICE_510_GetFullScreenStatus;
	
	_VIDEO_LINK_SetMaxTextureMem					= _VIDEO_RICE_510_SetMaxTextureMem;


#elif defined(_VIDEO_RICE_531)

	//GfxPluginVersion						= 0x0102;
	_VIDEO_LINK_InitiateGFX						= _VIDEO_RICE_531_InitiateGFX;
	_VIDEO_LINK_ProcessDList						= _VIDEO_RICE_531_ProcessDList;
	//_VIDEO_LINK_ProcessDList_Count_Cycles		= _VIDEO_RICE_531_ProcessDListCountCycles;
	_VIDEO_LINK_RomOpen							= _VIDEO_RICE_531_RomOpen;
	_VIDEO_LINK_RomClosed						= _VIDEO_RICE_531_RomClosed;
	//_VIDEO_LINK_DllClose							= _VIDEO_RICE_531_CloseDLL;
	_VIDEO_LINK_UpdateScreen						= _VIDEO_RICE_531_UpdateScreen;
	_VIDEO_LINK_GetDllInfo						= _VIDEO_RICE_531_GetDllInfo;
	//_VIDEO_LINK_ExtraChangeResolution			= 
	_VIDEO_LINK_DllConfig						= _VIDEO_RICE_531_DllConfig;
	//_VIDEO_LINK_Test								= _VIDEO_RICE_531_DllTest;
	//_VIDEO_LINK_About							= _VIDEO_RICE_531_DllAbout;
	_VIDEO_LINK_MoveScreen						= _VIDEO_RICE_531_MoveScreen;
	_VIDEO_LINK_DrawScreen						= _VIDEO_RICE_531_DrawScreen;
	_VIDEO_LINK_ViStatusChanged					= _VIDEO_RICE_531_ViStatusChanged;
	_VIDEO_LINK_ViWidthChanged					= _VIDEO_RICE_531_ViWidthChanged;
	//_VIDEO_LINK_ChangeWindow					= _VIDEO_RICE_531_ChangeWindow;
	
	//_VIDEO_LINK_ChangeWindow_1_3					= _VIDEO_RICE_531_ChangeWindow;
	//_VIDEO_LINK_CaptureScreen					= _VIDEO_RICE_531_CaptureScreen;
	_VIDEO_LINK_ProcessRDPList					= _VIDEO_RICE_531_ProcessRDPList;
	//_VIDEO_LINK_ShowCFB						= _VIDEO_RICE_531_ShowCFB;
	
	//_VIDEO_LINK_FrameBufferWrite					= _VIDEO_RICE_531_FBWrite;
	//_VIDEO_LINK_FrameBufferWriteList				= _VIDEO_RICE_531_FBWList;
	//_VIDEO_LINK_FrameBufferRead					= _VIDEO_RICE_531_FBRead;
	//_VIDEO_LINK_GetFrameBufferInfo				= _VIDEO_RICE_531_FBGetFrameBufferInfo;
	//_VIDEO_LINK_SetOnScreenText				= _VIDEO_RICE_531_SetOnScreenText;
	//_VIDEO_LINK_GetFullScreenStatus			= _VIDEO_RICE_531_GetFullScreenStatus;
	
	_VIDEO_LINK_SetMaxTextureMem					= _VIDEO_RICE_531_SetMaxTextureMem;

#elif defined(_VIDEO_RICE_560)

	//GfxPluginVersion						= 0x0103;
	_VIDEO_LINK_InitiateGFX						= _VIDEO_RICE_560_InitiateGFX;
	_VIDEO_LINK_ProcessDList						= _VIDEO_RICE_560_ProcessDList;
	//_VIDEO_LINK_ProcessDList_Count_Cycles		= _VIDEO_RICE_560_ProcessDListCountCycles;
	_VIDEO_LINK_RomOpen							= _VIDEO_RICE_560_RomOpen;
	_VIDEO_LINK_RomClosed						= _VIDEO_RICE_560_RomClosed;
	//_VIDEO_LINK_DllClose							= _VIDEO_RICE_560_CloseDLL;
	_VIDEO_LINK_UpdateScreen						= _VIDEO_RICE_560_UpdateScreen;
	_VIDEO_LINK_GetDllInfo						= _VIDEO_RICE_560_GetDllInfo;
	//_VIDEO_LINK_ExtraChangeResolution			= 
	_VIDEO_LINK_DllConfig						= _VIDEO_RICE_560_DllConfig;
	//_VIDEO_LINK_Test								= _VIDEO_RICE_560_DllTest;
	//_VIDEO_LINK_About							= _VIDEO_RICE_560_DllAbout;
	_VIDEO_LINK_MoveScreen						= _VIDEO_RICE_560_MoveScreen;
	_VIDEO_LINK_DrawScreen						= _VIDEO_RICE_560_DrawScreen;
	_VIDEO_LINK_ViStatusChanged					= _VIDEO_RICE_560_ViStatusChanged;
	_VIDEO_LINK_ViWidthChanged					= _VIDEO_RICE_560_ViWidthChanged;
	//_VIDEO_LINK_ChangeWindow					= _VIDEO_RICE_560_ChangeWindow;
	
	//_VIDEO_LINK_ChangeWindow_1_3					= _VIDEO_RICE_560_ChangeWindow;
	//_VIDEO_LINK_CaptureScreen					= _VIDEO_RICE_560_CaptureScreen;
	_VIDEO_LINK_ProcessRDPList					= _VIDEO_RICE_560_ProcessRDPList;
	//_VIDEO_LINK_ShowCFB						= _VIDEO_RICE_560_ShowCFB;
	
#ifdef CFB_READ
	_VIDEO_LINK_FrameBufferWrite					= _VIDEO_RICE_560_FBWrite;
	_VIDEO_LINK_FrameBufferWriteList				= _VIDEO_RICE_560_FBWList;
	_VIDEO_LINK_FrameBufferRead					= _VIDEO_RICE_560_FBRead;
	_VIDEO_LINK_GetFrameBufferInfo				= _VIDEO_RICE_560_FBGetFrameBufferInfo;
	//_VIDEO_LINK_SetOnScreenText				= _VIDEO_RICE_560_SetOnScreenText;
	//_VIDEO_LINK_GetFullScreenStatus			= _VIDEO_RICE_560_GetFullScreenStatus;
#endif
	_VIDEO_LINK_SetMaxTextureMem					= _VIDEO_RICE_560_SetMaxTextureMem;

#elif defined(_VIDEO_RICE_611)
	//GfxPluginVersion						= 0x0103;
	_VIDEO_LINK_InitiateGFX						= _VIDEO_RICE_611_InitiateGFX;
	_VIDEO_LINK_ProcessDList						= _VIDEO_RICE_611_ProcessDList;
	//_VIDEO_LINK_ProcessDList_Count_Cycles		= _VIDEO_RICE_611_ProcessDListCountCycles;
	_VIDEO_LINK_RomOpen							= _VIDEO_RICE_611_RomOpen;
	_VIDEO_LINK_RomClosed						= _VIDEO_RICE_611_RomClosed;
	//_VIDEO_LINK_DllClose							= _VIDEO_RICE_611_CloseDLL;
	_VIDEO_LINK_UpdateScreen						= _VIDEO_RICE_611_UpdateScreen;
	_VIDEO_LINK_GetDllInfo						= _VIDEO_RICE_611_GetDllInfo;
	//_VIDEO_LINK_ExtraChangeResolution			= 
	_VIDEO_LINK_DllConfig						= _VIDEO_RICE_611_DllConfig;
	//_VIDEO_LINK_Test								= _VIDEO_RICE_611_DllTest;
	//_VIDEO_LINK_About							= _VIDEO_RICE_611_DllAbout;
	_VIDEO_LINK_MoveScreen						= _VIDEO_RICE_611_MoveScreen;
	_VIDEO_LINK_DrawScreen						= _VIDEO_RICE_611_DrawScreen;
	_VIDEO_LINK_ViStatusChanged					= _VIDEO_RICE_611_ViStatusChanged;
	_VIDEO_LINK_ViWidthChanged					= _VIDEO_RICE_611_ViWidthChanged;
	//_VIDEO_LINK_ChangeWindow					= _VIDEO_RICE_611_ChangeWindow;

	//_VIDEO_LINK_ChangeWindow_1_3					= _VIDEO_RICE_611_ChangeWindow;
	//_VIDEO_LINK_CaptureScreen					= _VIDEO_RICE_611_CaptureScreen;
	_VIDEO_LINK_ProcessRDPList					= _VIDEO_RICE_611_ProcessRDPList;
	//_VIDEO_LINK_ShowCFB						= _VIDEO_RICE_611_ShowCFB;
	
#ifdef CFB_READ
	_VIDEO_LINK_FrameBufferWrite					= _VIDEO_RICE_611_FBWrite;
	_VIDEO_LINK_FrameBufferWriteList				= _VIDEO_RICE_611_FBWList;
	_VIDEO_LINK_FrameBufferRead					= _VIDEO_RICE_611_FBRead;
	_VIDEO_LINK_GetFrameBufferInfo				= _VIDEO_RICE_611_FBGetFrameBufferInfo;
#endif
	//_VIDEO_LINK_SetOnScreenText				= _VIDEO_RICE_611_SetOnScreenText;
	//_VIDEO_LINK_GetFullScreenStatus			= _VIDEO_RICE_611_GetFullScreenStatus;
	
	_VIDEO_LINK_SetMaxTextureMem					= _VIDEO_RICE_611_SetMaxTextureMem;

#elif defined(_VIDEO_RICE_612)
	//GfxPluginVersion						= 0x0103;
	_VIDEO_LINK_InitiateGFX						= _VIDEO_RICE_612_InitiateGFX;
	_VIDEO_LINK_ProcessDList						= _VIDEO_RICE_612_ProcessDList;
	//_VIDEO_LINK_ProcessDList_Count_Cycles		= _VIDEO_RICE_612_ProcessDListCountCycles;
	_VIDEO_LINK_RomOpen							= _VIDEO_RICE_612_RomOpen;
	_VIDEO_LINK_RomClosed						= _VIDEO_RICE_612_RomClosed;
	//_VIDEO_LINK_DllClose							= _VIDEO_RICE_612_CloseDLL;
	_VIDEO_LINK_UpdateScreen						= _VIDEO_RICE_612_UpdateScreen;
	_VIDEO_LINK_GetDllInfo						= _VIDEO_RICE_612_GetDllInfo;
	//_VIDEO_LINK_ExtraChangeResolution			= 
	_VIDEO_LINK_DllConfig						= _VIDEO_RICE_612_DllConfig;
	//_VIDEO_LINK_Test								= _VIDEO_RICE_612_DllTest;
	//_VIDEO_LINK_About							= _VIDEO_RICE_612_DllAbout;
	_VIDEO_LINK_MoveScreen						= _VIDEO_RICE_612_MoveScreen;
	_VIDEO_LINK_DrawScreen						= _VIDEO_RICE_612_DrawScreen;
	_VIDEO_LINK_ViStatusChanged					= _VIDEO_RICE_612_ViStatusChanged;
	_VIDEO_LINK_ViWidthChanged					= _VIDEO_RICE_612_ViWidthChanged;
	_VIDEO_LINK_ChangeWindow					= _VIDEO_RICE_612_ChangeWindow;
	
	//_VIDEO_LINK_ChangeWindow_1_3					= _VIDEO_RICE_612_ChangeWindow;
	//_VIDEO_LINK_CaptureScreen					= _VIDEO_RICE_612_CaptureScreen;
	_VIDEO_LINK_ProcessRDPList					= _VIDEO_RICE_612_ProcessRDPList;
	//_VIDEO_LINK_ShowCFB						= _VIDEO_RICE_612_ShowCFB;
	
#ifdef CFB_READ
	_VIDEO_LINK_FrameBufferWrite					= _VIDEO_RICE_612_FBWrite;
	_VIDEO_LINK_FrameBufferWriteList				= _VIDEO_RICE_612_FBWList;
	_VIDEO_LINK_FrameBufferRead					= _VIDEO_RICE_612_FBRead;
	_VIDEO_LINK_GetFrameBufferInfo				= _VIDEO_RICE_612_FBGetFrameBufferInfo;
#endif
	//_VIDEO_LINK_SetOnScreenText				= _VIDEO_RICE_612_SetOnScreenText;
	//_VIDEO_LINK_GetFullScreenStatus			= _VIDEO_RICE_612_GetFullScreenStatus;
	
	_VIDEO_LINK_SetMaxTextureMem					= _VIDEO_RICE_612_SetMaxTextureMem;

#else
	DisplayError("No Video Plugin Defined!");
#endif

	GetDllInfo = (void ( *)(PLUGIN_INFO *))_VIDEO_LINK_GetDllInfo;
	if (GetDllInfo == NULL) { return FALSE; }
	GetDllInfo(&PluginInfo);
	if (!ValidPluginVersion(&PluginInfo) || PluginInfo.MemoryBswaped == FALSE) { return FALSE; }
	//GFXCloseDLL = (void ( *)(void))_VIDEO_LINK_CloseDLL;
	//if (GFXCloseDLL == NULL) { return FALSE; }
	ChangeWindow = (void ( *)(void))_VIDEO_LINK_ChangeWindow;
	if (ChangeWindow == NULL) { return FALSE; }
	GFXDllConfig = (void ( *)(HWND))_VIDEO_LINK_DllConfig;
	DrawScreen = (void ( *)(void))_VIDEO_LINK_DrawScreen;
	if (DrawScreen == NULL) { return FALSE; }
	InitiateGFX = (BOOL ( *)(GFX_INFO))_VIDEO_LINK_InitiateGFX;
	if (InitiateGFX == NULL) { return FALSE; }
	MoveScreen = (void ( *)(int, int))_VIDEO_LINK_MoveScreen;
	if (MoveScreen == NULL) { return FALSE; }
	ProcessDList = (void ( *)(void))_VIDEO_LINK_ProcessDList;
	if (ProcessDList == NULL) { return FALSE; }
	GfxRomClosed = (void ( *)(void))_VIDEO_LINK_RomClosed;
	if (GfxRomClosed == NULL) { return FALSE; }
	GfxRomOpen = (void ( *)(void))_VIDEO_LINK_RomOpen;
	if (GfxRomOpen == NULL) { return FALSE; }
	UpdateScreen = (void ( *)(void))_VIDEO_LINK_UpdateScreen;
	if (UpdateScreen == NULL) { return FALSE; }
	ViStatusChanged = (void ( *)(void))_VIDEO_LINK_ViStatusChanged;
	if (ViStatusChanged == NULL) { return FALSE; }
	ViWidthChanged = (void ( *)(void))_VIDEO_LINK_ViWidthChanged;
	if (ViWidthChanged == NULL) { return FALSE; }
	
	if (PluginInfo.Version >= 0x0103 ){
		ProcessRDPList = (void ( *)(void))_VIDEO_LINK_ProcessRDPList;
		if (ProcessRDPList == NULL) { return FALSE; }
		//CaptureScreen = (void ( *)(char *))_VIDEO_LINK_CaptureScreen;
		//if (CaptureScreen == NULL) { return FALSE; }
		//ShowCFB = (void ( *)(void))_VIDEO_LINK_ShowCFB;
		//if (ShowCFB == NULL) { return FALSE; }
		//GetGfxDebugInfo = (void ( *)(GFXDEBUG_INFO *))_VIDEO_LINK_GetGfxDebugInfo;
		//InitiateGFXDebugger = (void ( *)(DEBUG_INFO))_VIDEO_LINK_InitiateGFXDebugger;
	} else {
		ProcessRDPList = NULL;
		CaptureScreen = NULL;
		ShowCFB = NULL;
		//GetGfxDebugInfo = NULL;
		//InitiateGFXDebugger = NULL;
	}
#ifdef CFB_READ
	FrameBufferRead = (void ( *)(DWORD))_VIDEO_LINK_FBRead;
	FrameBufferWrite = (void ( *)(DWORD, DWORD))_VIDEO_LINK_FBWrite;
#endif
	SetMaxTextureMem = (void ( *)(DWORD))_VIDEO_LINK_SetMaxTextureMem;
	SetMaxTextureMem(loaddwMaxVideoMem());

	{
		_VIDEO_LINK_CloseDLL		= NULL;
		_VIDEO_LINK_DllConfig		= NULL;
		_VIDEO_LINK_GetDllInfo		= NULL;
		_VIDEO_LINK_ChangeWindow	= NULL;
		_VIDEO_LINK_DrawScreen		= NULL;
		_VIDEO_LINK_FBRead			= NULL;
		_VIDEO_LINK_FBWrite		= NULL;
		_VIDEO_LINK_InitiateGFX	= NULL;
		_VIDEO_LINK_MoveScreen		= NULL;
		_VIDEO_LINK_ProcessDList	= NULL;
		_VIDEO_LINK_RomClosed		= NULL;
		_VIDEO_LINK_RomOpen		= NULL;
		_VIDEO_LINK_UpdateScreen	= NULL;
		_VIDEO_LINK_ViStatusChanged= NULL;
		_VIDEO_LINK_ViStatusChanged= NULL;
		_VIDEO_LINK_ViWidthChanged	= NULL;

		_VIDEO_LINK_ProcessRDPList	= NULL;
		_VIDEO_LINK_CaptureScreen	= NULL;
		_VIDEO_LINK_ShowCFB		= NULL;

		_VIDEO_LINK_SetMaxTextureMem= NULL;
	}




	
	GetDllInfo(&PluginInfo);
	if (!ValidPluginVersion(&PluginInfo) || PluginInfo.MemoryBswaped == FALSE) { return FALSE; }



	return TRUE;
}

BOOL LoadRSPDll(void) {
	PLUGIN_INFO PluginInfo;
 
	hRspDll = (HANDLE)1;
	if (hRspDll == NULL) {  return FALSE; }

	
	if(g_iRspPlugin == _RSPPluginLLE || g_iRspPlugin == _RSPPluginNone) // pj64 seems to break without rsp
	{
		_RSP_LINK_DoRspCycles 		= _RSP_DoRspCycles;
		_RSP_LINK_CloseDLL	 		= _RSP_CloseDLL;
		_RSP_LINK_RomClosed	 		= _RSP_RomClosed;
		_RSP_LINK_DllConfig		 	= _RSP_DllConfig;
		//_RSP_LINK_DllAbout		 	= NULL;
		_RSP_LINK_GetDllInfo		= _RSP_GetDllInfo;
		//_RSP_LINK_InitiateRSP_1_0	= _RSP_InitiateRSP;
		_RSP_LINK_InitiateRSP_1_1	= _RSP_InitiateRSP;
	}
	else if(g_iRspPlugin == _RSPPluginHLE)
	{
		_RSP_LINK_DoRspCycles 		= _RSP_HLE_DoRspCycles;
		_RSP_LINK_CloseDLL	 		= _RSP_HLE_CloseDLL;
		_RSP_LINK_RomClosed	 		= _RSP_HLE_RomClosed;
		_RSP_LINK_DllConfig		 	= _RSP_HLE_DllConfig;
		//_RSP_LINK_DllAbout		 	= NULL;
		_RSP_LINK_GetDllInfo		= _RSP_HLE_GetDllInfo;
		//_RSP_LINK_InitiateRSP_1_0	= _RSP_HLE_InitiateRSP;
		_RSP_LINK_InitiateRSP_1_1	= _RSP_HLE_InitiateRSP;
	}
	else if(g_iRspPlugin == _RSPPluginM64P)
	{
		_RSP_LINK_DoRspCycles 		= _RSP_M64p_DoRspCycles;
		_RSP_LINK_CloseDLL	 		= _RSP_M64p_CloseDLL;
		_RSP_LINK_RomClosed	 		= _RSP_M64p_RomClosed;
		_RSP_LINK_DllConfig		 	= _RSP_M64p_DllConfig;
		//_RSP_LINK_DllAbout		 	= NULL;
		_RSP_LINK_GetDllInfo		= _RSP_M64p_GetDllInfo;
		//_RSP_LINK_InitiateRSP_1_0	= _RSP_M64p_InitiateRSP;
		_RSP_LINK_InitiateRSP_1_1	= _RSP_M64p_InitiateRSP;
	}
	/*
	else if(g_iRspPlugin == _RSPPluginCXD4)
	{
		_RSP_LINK_DoRspCycles 		= _RSP_CXD4_DoRspCycles;
		_RSP_LINK_CloseDLL	 		= _RSP_CXD4_CloseDLL;
		_RSP_LINK_RomClosed	 		= _RSP_CXD4_RomClosed;
		_RSP_LINK_DllConfig		 	= _RSP_CXD4_DllConfig;
		//_RSP_LINK_DllAbout		 	= NULL;
		_RSP_LINK_GetDllInfo		= _RSP_CXD4_GetDllInfo;
		//_RSP_LINK_InitiateRSP_1_0	= _RSP_CXD4_InitiateRSP;
		_RSP_LINK_InitiateRSP_1_1	= _RSP_CXD4_InitiateRSP;
	}
	*/
	
	GetDllInfo = (void ( *)(PLUGIN_INFO *))_RSP_LINK_GetDllInfo;
	if (GetDllInfo == NULL) { return FALSE; }

	GetDllInfo(&PluginInfo);
	if (!ValidPluginVersion(&PluginInfo) || PluginInfo.MemoryBswaped == FALSE) { return FALSE; }
	RSPVersion = PluginInfo.Version;
	if (RSPVersion == 1) { RSPVersion = 0x0100; }

	DoRspCycles = (DWORD ( *)(DWORD))_RSP_LINK_DoRspCycles;
	if (DoRspCycles == NULL) { return FALSE; }
	InitiateRSP_1_0 = NULL;
	InitiateRSP_1_1 = NULL;
	if (RSPVersion == 0x100) {
		InitiateRSP_1_0 = (void *)_RSP_LINK_InitiateRSP_1_1;
		if (InitiateRSP_1_0 == NULL) { return FALSE; }
	}
	if (RSPVersion == 0x101) {
		InitiateRSP_1_1 = (void *)_RSP_LINK_InitiateRSP_1_1;
		if (InitiateRSP_1_1 == NULL) { return FALSE; }
	}
	RSPRomClosed = (void *)_RSP_LINK_RomClosed;
	if (RSPRomClosed == NULL) { return FALSE; }
	RSPCloseDLL = (void ( *)(void))_RSP_LINK_CloseDLL;
	if (RSPCloseDLL == NULL) { return FALSE; }
	
	RSPDllConfig = (void ( *)(HWND))_RSP_LINK_DllConfig;
	
	{
		_RSP_LINK_DoRspCycles 		= NULL;
		_RSP_LINK_CloseDLL	 		= NULL;
		_RSP_LINK_RomClosed	 		= NULL;
		_RSP_LINK_DllConfig		 	= NULL;
		//_RSP_LINK_DllAbout		 	= NULL;
		_RSP_LINK_GetDllInfo		= NULL;
		//_RSP_LINK_InitiateRSP_1_0	= NULL;
		_RSP_LINK_InitiateRSP_1_1	= NULL;
	}
	
	return TRUE;
}

void SetupPlugins (HWND hWnd) {
	 
	BOOL success = FALSE;

	ShutdownPlugins();
	GetCurrentDlls();

	//PluginsInitilized = TRUE;

	if (!LoadGFXDll("")) { 
		//DisplayError(GS(MSG_FAIL_INIT_GFX));
		//PluginsInitilized = FALSE;
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
			//DisplayError(GS(MSG_FAIL_INIT_GFX));
			//PluginsInitilized = FALSE;
		}
	}


	// plugin selection to replace bools: g_bUseBasicAudio = g_bUseLLERspPlugin;
	//if (g_bUseBasicAudio) success = LoadAudioBasicDll();
	//else success = LoadAudioJttLDll();
	success = LoadAudioDll();

	if (!success) {
		AiCloseDLL       = NULL;
		AiDacrateChanged = NULL;
		AiLenChanged     = NULL;
		AiReadLength     = NULL;
		AiUpdate         = NULL;
		InitiateAudio    = NULL;
		ProcessAList     = NULL;
		AiRomClosed      = NULL;
		//DisplayError(GS(MSG_FAIL_INIT_AUDIO));
		//PluginsInitilized = FALSE;
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
	
	if (!LoadRSPDll()) {
		//return;
		//PluginsInitialized = FALSE;
	}

	// control RSP with multi plugin options
	// pj64 seems to break without rsp
	{
		RSP_INFO_1_0 RspInfo10;
		RSP_INFO_1_1 RspInfo11;

		

		RspInfo10.CheckInterrupts = CheckInterrupts;
		RspInfo11.CheckInterrupts = CheckInterrupts;
		RspInfo10.ProcessDlist = ProcessDList;
		RspInfo11.ProcessDlist = ProcessDList;

		// going to use rsp audio plugin here instead of the use lle rsp bool
		if (g_bUseRspAudio) // g_bUseLLERspPlugin // g_iAudioPlugin != _AudioPluginLleRsp
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

		if(g_iAudioPlugin == _AudioPluginM64P)
		{
			//ProcessAList = RspInfo11.ProcessAlist;
		}
	}
	
#if (!defined(EXTERNAL_RELEASE))
	if (HaveDebugger) {
		DEBUG_INFO DebugInfo;

		if (GetRspDebugInfo != NULL) { GetRspDebugInfo(&RspDebug); }				
		if (GetGfxDebugInfo != NULL) { GetGfxDebugInfo(&GFXDebug); }
		
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
		//DisplayError(GS(MSG_FAIL_INIT_CONTROL));
		//PluginsInitilized = FALSE;
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
	//if (!PluginsInitilized) { ChangeSettings(hMainWindow); }
}

void SetupPluginScreen (HWND hDlg) {}

void ShutdownPlugins (void) {}

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
