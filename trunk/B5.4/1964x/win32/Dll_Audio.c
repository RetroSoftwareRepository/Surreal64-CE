/*$T Dll_Audio.c GC 1.136 03/09/02 17:41:09 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    Audio plugin interface functions
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


/*
 * 1964 Copyright (C) 1999-2002 Joel Middendorf, <schibo@emulation64.com> This
 * program is free software; you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option) any later
 * version. This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details. You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA. To contact the
 * authors: email: schibo@emulation64.com, rice1964@yahoo.com
 */

#include <xtl.h>

#include "../globals.h"
#include "../n64rcp.h"
#include "../emulator.h"
#include "registry.h"
#include "DLL_Audio.h"
#include "DLL_Rsp.h"

AUDIO_INFO	Audio_Info;

BOOL	CoreDoingAIUpdate = TRUE;

// Ez0n3 - use iAudioPlugin instead to determine if basic audio is used
//BOOL	g_bUseBasicAudio = FALSE;
int g_iAudioPlugin = _AudioPluginJttl; // set default to jttl

// SET DEFAULT AUDIO PLUGIN - JttL
void (*_AUDIO_LINK_AiDacrateChanged)(int SystemType) = _AUDIO_AiDacrateChanged;
void (*_AUDIO_LINK_AiLenChanged)(void) = _AUDIO_AiLenChanged;
DWORD (*_AUDIO_LINK_AiReadLength)(void) = _AUDIO_AiReadLength;
void (*_AUDIO_LINK_AiUpdate)(BOOL Wait) = _AUDIO_AiUpdate;
void (*_AUDIO_LINK_CloseDLL)(void) = _AUDIO_CloseDLL;
void (*_AUDIO_LINK_DllAbout)(HWND hParent) = _AUDIO_DllAbout;
void (*_AUDIO_LINK_DllConfig)(HWND hParent) = _AUDIO_DllConfig;
void (*_AUDIO_LINK_DllTest)(HWND hParent) = _AUDIO_DllTest;
void (*_AUDIO_LINK_GetDllInfo)(PLUGIN_INFO *PluginInfo) = _AUDIO_GetDllInfo;
BOOL (*_AUDIO_LINK_InitiateAudio)(AUDIO_INFO Audio_Info) = _AUDIO_InitiateAudio;
void (*_AUDIO_LINK_ProcessAList)(void) = _AUDIO_ProcessAList;
void (*_AUDIO_LINK_RomClosed)(void) = _AUDIO_RomClosed;
DWORD (*_AUDIO_LINK_ProcessAListCountCycles)(void) = NULL;


/* Used when selecting plugins */
//void (__cdecl *_AUDIO_Under_Selecting_About) (HWND) = NULL;
//void (__cdecl *_AUDIO_Under_Selecting_Test) (HWND) = NULL;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL LoadAudioPlugin()
{ 
	// Ez0n3 - plugin selection to replace bools: g_bUseBasicAudio = g_bUseLLERspPlugin;
	if (g_iAudioPlugin == _AudioPluginNone)
	{
		// Ez0n3 - No Audio (declared in plugin.h include)
		_AUDIO_LINK_AiDacrateChanged 	= _AUDIO_NONE_AiDacrateChanged;
		_AUDIO_LINK_AiLenChanged	 	= _AUDIO_NONE_AiLenChanged;
		_AUDIO_LINK_AiReadLength	 	= _AUDIO_NONE_AiReadLength;
		_AUDIO_LINK_AiUpdate		 	= _AUDIO_NONE_AiUpdate;
		_AUDIO_LINK_CloseDLL		 	= _AUDIO_NONE_CloseDLL;
		_AUDIO_LINK_DllAbout		 	= _AUDIO_NONE_DllAbout;
		_AUDIO_LINK_DllConfig		 	= _AUDIO_NONE_DllConfig;
		_AUDIO_LINK_DllTest			 	= _AUDIO_NONE_DllTest;
		_AUDIO_LINK_GetDllInfo		 	= _AUDIO_NONE_GetDllInfo;
		_AUDIO_LINK_InitiateAudio	 	= _AUDIO_NONE_InitiateAudio;
		_AUDIO_LINK_ProcessAList	 	= _AUDIO_NONE_ProcessAList;
		_AUDIO_LINK_RomClosed		 	= _AUDIO_NONE_RomClosed;
	}
	else if (g_iAudioPlugin == _AudioPluginLleRsp)
	{
		//
	}
	else if (g_iAudioPlugin == _AudioPluginBasic)
	{
		_AUDIO_LINK_AiDacrateChanged 	= _AUDIO_BASIC_AiDacrateChanged;
		_AUDIO_LINK_AiLenChanged	 	= _AUDIO_BASIC_AiLenChanged;
		_AUDIO_LINK_AiReadLength	 	= _AUDIO_BASIC_AiReadLength;
		_AUDIO_LINK_AiUpdate		 	= _AUDIO_BASIC_AiUpdate;
		_AUDIO_LINK_CloseDLL		 	= _AUDIO_BASIC_CloseDLL;
		_AUDIO_LINK_DllAbout		 	= _AUDIO_BASIC_DllAbout;
		_AUDIO_LINK_DllConfig		 	= _AUDIO_BASIC_DllConfig;
		_AUDIO_LINK_DllTest			 	= _AUDIO_BASIC_DllTest;
		_AUDIO_LINK_GetDllInfo		 	= _AUDIO_BASIC_GetDllInfo;
		_AUDIO_LINK_InitiateAudio	 	= _AUDIO_BASIC_InitiateAudio;
		_AUDIO_LINK_ProcessAList	 	= _AUDIO_BASIC_ProcessAList;
		_AUDIO_LINK_RomClosed		 	= _AUDIO_BASIC_RomClosed;
	}
	else if (g_iAudioPlugin == _AudioPluginJttl)
	{
		//freakdave - JttL 1.2
		_AUDIO_LINK_AiDacrateChanged 	= _AUDIO_AiDacrateChanged;
		_AUDIO_LINK_AiLenChanged	 	= _AUDIO_AiLenChanged;
		_AUDIO_LINK_AiReadLength	 	= _AUDIO_AiReadLength;
		_AUDIO_LINK_AiUpdate		 	= _AUDIO_AiUpdate;
		_AUDIO_LINK_CloseDLL		 	= _AUDIO_CloseDLL;
		_AUDIO_LINK_DllAbout		 	= _AUDIO_DllAbout;
		_AUDIO_LINK_DllConfig		 	= _AUDIO_DllConfig;
		_AUDIO_LINK_DllTest			 	= _AUDIO_DllTest;
		_AUDIO_LINK_GetDllInfo		 	= _AUDIO_GetDllInfo;
		_AUDIO_LINK_InitiateAudio	 	= _AUDIO_InitiateAudio;
		_AUDIO_LINK_ProcessAList	 	= _AUDIO_ProcessAList;
		_AUDIO_LINK_RomClosed		 	= _AUDIO_RomClosed;
	}
	else if (g_iAudioPlugin == _AudioPluginAzimer)
	{
		// Ez0n3 - old Azimer plugin from Surreal 1.0 and FDB
		//freakdave - readded Azimer
		_AUDIO_LINK_AiDacrateChanged 	= _AUDIO_AZIMER_AiDacrateChanged;
		_AUDIO_LINK_AiLenChanged	 	= _AUDIO_AZIMER_AiLenChanged;
		_AUDIO_LINK_AiReadLength	 	= _AUDIO_AZIMER_AiReadLength;
		_AUDIO_LINK_AiUpdate		 	= _AUDIO_AZIMER_AiUpdate;
		_AUDIO_LINK_CloseDLL		 	= _AUDIO_AZIMER_CloseDLL;
		_AUDIO_LINK_DllAbout		 	= _AUDIO_AZIMER_DllAbout;
		_AUDIO_LINK_DllConfig		 	= _AUDIO_AZIMER_DllConfig;
		_AUDIO_LINK_DllTest			 	= _AUDIO_AZIMER_DllTest;
		_AUDIO_LINK_GetDllInfo		 	= _AUDIO_AZIMER_GetDllInfo;
		_AUDIO_LINK_InitiateAudio	 	= _AUDIO_AZIMER_InitiateAudio;
		_AUDIO_LINK_ProcessAList	 	= _AUDIO_AZIMER_ProcessAList;
		_AUDIO_LINK_RomClosed		 	= _AUDIO_AZIMER_RomClosed;
	}
	
	else if (g_iAudioPlugin == _AudioPluginMusyX)
	{
		// freakdave - new MusyX Audio plugin
		_AUDIO_LINK_AiDacrateChanged 	= _AUDIO_MUSYX_AiDacrateChanged;
		_AUDIO_LINK_AiLenChanged	 	= _AUDIO_MUSYX_AiLenChanged;
		_AUDIO_LINK_AiReadLength	 	= _AUDIO_MUSYX_AiReadLength;
		_AUDIO_LINK_AiUpdate		 	= _AUDIO_MUSYX_AiUpdate;
		_AUDIO_LINK_CloseDLL		 	= _AUDIO_MUSYX_CloseDLL;
		_AUDIO_LINK_DllAbout		 	= _AUDIO_MUSYX_DllAbout;
		_AUDIO_LINK_DllConfig		 	= _AUDIO_MUSYX_DllConfig;
		_AUDIO_LINK_DllTest			 	= _AUDIO_MUSYX_DllTest;
		_AUDIO_LINK_GetDllInfo		 	= _AUDIO_MUSYX_GetDllInfo;
		_AUDIO_LINK_InitiateAudio	 	= _AUDIO_MUSYX_InitiateAudio;
		_AUDIO_LINK_ProcessAList	 	= _AUDIO_MUSYX_ProcessAList;
		_AUDIO_LINK_RomClosed		 	= _AUDIO_MUSYX_RomClosed;
		_AUDIO_LINK_ProcessAListCountCycles = _AUDIO_MUSYX_ProcessAListCountCycles;
	}
	

	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AUDIO_GetDllInfo(PLUGIN_INFO *Plugin_Info)
{
	__try
	{
		_AUDIO_LINK_GetDllInfo(Plugin_Info);
	}

	__except(NULL, EXCEPTION_EXECUTE_HANDLER)
	{
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL AUDIO_Initialize(AUDIO_INFO Audio_Info)
{ 
	__try
	{
		_AUDIO_LINK_InitiateAudio(Audio_Info);
	}

	__except(NULL, EXCEPTION_EXECUTE_HANDLER)
	{
		/* Some people won't have a soud card. No error. */
	}

	return(1);	/* for now.. */
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AUDIO_ProcessAList(void)
{
	__try
	{
		_AUDIO_LINK_ProcessAList();
	}

	__except(NULL, EXCEPTION_EXECUTE_HANDLER)
	{
		/* Some people won't have a soud card. No error. */
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AUDIO_RomClosed(void)
{
	__try
	{
		_AUDIO_LINK_RomClosed();
	}

	__except(NULL, EXCEPTION_EXECUTE_HANDLER)
	{
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AUDIO_DllClose(void)
{
	__try
	{
		_AUDIO_LINK_CloseDLL();
	}

	__except(NULL, EXCEPTION_EXECUTE_HANDLER)
	{
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
/*void CloseAudioPlugin(void)
{
	// AUDIO_End();
	AUDIO_RomClosed();
	AUDIO_DllClose();
}*/

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AUDIO_DllConfig(HWND hParent)
{
	//_AUDIO_LINK_DllConfig(hParent);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AUDIO_About(HWND hParent)
{
	_AUDIO_LINK_DllAbout(hParent);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AUDIO_Test(HWND hParent)
{
	//_AUDIO_LINK_DllTest(hParent);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AUDIO_AiDacrateChanged(int SystemType)
{
	_AUDIO_LINK_AiDacrateChanged(SystemType);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AUDIO_AiLenChanged(void)
{
	_AUDIO_LINK_AiLenChanged();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
DWORD AUDIO_AiReadLength(void)
{
	return _AUDIO_LINK_AiReadLength();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AUDIO_AiUpdate(BOOL update)
{
	_AUDIO_LINK_AiUpdate(update);
}

DWORD AUDIO_ProcessAListCountCycles(void)
{
	_AUDIO_LINK_ProcessAListCountCycles();
}

/*
 =======================================================================================================================
    Used when selecting plugins
 =======================================================================================================================
 */
/*void AUDIO_Under_Selecting_About(HWND hParent)
{
	if(_AUDIO_LINK_Under_Selecting_About != NULL)
	{
		_AUDIO_LINK_Under_Selecting_About(hParent);
	}
	else
	{

	}
}*/

/*
 =======================================================================================================================
 =======================================================================================================================
 */
/*void AUDIO_Under_Selecting_Test(HWND hParent)
{
	if(_AUDIO_LINK_Under_Selecting_Test != NULL)
	{
		_AUDIO_LINK_Under_Selecting_Test(hParent);
	}
	else
	{
	}
}*/
