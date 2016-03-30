/*$T Dll_Audio.c GC 1.136 03/09/02 17:41:09 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    Audio plugin interface functions
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


/*
 * 1964 Copyright (C) 1999-2004 Joel Middendorf, <schibo@emulation64.com> This
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
#include "../stdafx.h"
#include <float.h>

#ifdef _XBOX
int g_iAudioPlugin = _AudioPluginJttl; // set default to jttl
BOOL g_bAudioBoost = FALSE;
#endif //_XBOX

AUDIO_INFO	Audio_Info;

HINSTANCE	hinstLibAudio = NULL;
BOOL	CoreDoingAIUpdate = TRUE;

void (CALL *_AUDIO_RomClosed) (void) = NULL;
void (CALL *_AUDIO_DllClose) () = NULL;
void (CALL *_AUDIO_DllConfig) (HWND) = NULL;
void (CALL *_AUDIO_About) (HWND) = NULL;
void (CALL *_AUDIO_Test) (HWND) = NULL;
void (CALL *_AUDIO_GetDllInfo) (PLUGIN_INFO *) = NULL;
BOOL (CALL *_AUDIO_Initialize) (AUDIO_INFO) = NULL;
void (CALL *_AUDIO_End) () = NULL;
void (CALL *_AUDIO_PlaySnd) (unsigned __int8 *, unsigned __int32 *) = NULL;
_int32 (CALL *_AUDIO_TimeLeft) (unsigned char *) = NULL;
void (CALL *_AUDIO_AiDacrateChanged) (int) = NULL;
void (CALL *_AUDIO_AiLenChanged) (void) = NULL;
DWORD (CALL *_AUDIO_AiReadLength) (void) = NULL;
void (CALL *_AUDIO_AiUpdate) (BOOL) = NULL;
void (CALL *_AUDIO_ProcessAList) (void) = NULL;
DWORD (CALL *_AUDIO_ProcessAList_Count_Cycles) (void) = NULL;
BOOL (CALL *_AUDIO_IsMusyX) (void) = NULL;
/* Used when selecting plugins */
void (CALL *_AUDIO_Under_Selecting_About) (HWND) = NULL;
void (CALL *_AUDIO_Under_Selecting_Test) (HWND) = NULL;

#ifdef _XBOX
void (CALL *_AUDIO_AudioBoost)(BOOL Boost) = NULL; // increase audio db on xbox
BOOL (CALL *_AUDIO_AudioMute)(BOOL Mute) = NULL; // mute audio for pausing 1964audio on xbox
#endif //_XBOX

#ifdef _DEBUG
static void __cdecl DebuggerMsgCallBack(char *msg)
{
	TRACE0(msg);
}
static void (__cdecl *_SetDebuggerCallBack)(void (_cdecl *DbgCallBackFun)(char *msg)) = NULL;
#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL _AUDIO_AZIAUD_IsMusyX(){ return 1;}
BOOL LoadAudioPlugin(char *libname)
{
#ifdef _XBOX
#if defined(_AUDIO_SOLO)
#if defined(_AUDIO_AZIMER)
	g_iAudioPlugin = _AudioPluginAzimer;
#elif defined(_AUDIO_AZIAUD)
	g_iAudioPlugin = _AudioPluginAziDS8;
#elif defined(_AUDIO_BASIC)
	g_iAudioPlugin = _AudioPluginBasic;
#elif defined(_AUDIO_DAEDALUS)
	g_iAudioPlugin = _AudioPluginDaedalus;
#elif defined(_AUDIO_JTTL)
	g_iAudioPlugin = _AudioPluginJttl;
#elif defined(_AUDIO_M64P)
	g_iAudioPlugin = _AudioPluginM64P;
#elif defined(_AUDIO_MUSYX)
	g_iAudioPlugin = _AudioPluginMusyX;
#elif defined(_AUDIO_NONE)
	g_iAudioPlugin = _AudioPluginNone;
#else
	DisplayError("No Audio Plugin Defined! (%i)", g_iAudioPlugin);
#endif
#endif //_AUDIO_SOLO

	switch (g_iAudioPlugin)
	{
#if defined(_AUDIO_AZIMER)
	case _AudioPluginAzimer :
		_AUDIO_RomClosed		 			= _AUDIO_AZIMER_RomClosed;
		_AUDIO_DllClose		 				= _AUDIO_AZIMER_CloseDLL;
		//_AUDIO_DllConfig		 			= _AUDIO_AZIMER_DllConfig;
		_AUDIO_About		 				= _AUDIO_AZIMER_DllAbout;
		//_AUDIO_Test			 			= _AUDIO_AZIMER_DllTest;
		_AUDIO_GetDllInfo		 			= _AUDIO_AZIMER_GetDllInfo;
		_AUDIO_Initialize	 				= _AUDIO_AZIMER_InitiateAudio;
		_AUDIO_AiDacrateChanged 			= _AUDIO_AZIMER_AiDacrateChanged;
		_AUDIO_AiLenChanged	 				= _AUDIO_AZIMER_AiLenChanged;
		_AUDIO_AiReadLength	 				= _AUDIO_AZIMER_AiReadLength;
		_AUDIO_AiUpdate		 				= _AUDIO_AZIMER_AiUpdate;
		_AUDIO_ProcessAList	 				= _AUDIO_AZIMER_ProcessAList;
		//_AUDIO_ProcessAList_Count_Cycles	= _AUDIO_AZIMER_ProcessAListCountCycles;
		//_AUDIO_IsMusyX					= _AUDIO_AZIMER_IsMusyX;
		_AUDIO_AudioBoost		 			= _AUDIO_AZIMER_AudioBoost;
			break;
#endif //_AUDIO_AZIMER

#if defined(_AUDIO_AZIAUD)

	case _AudioPluginAziDS8 :
		_AUDIO_RomClosed		 			= _AUDIO_AZIAUD_RomClosed;
		_AUDIO_DllClose		 				= _AUDIO_AZIAUD_CloseDLL;
		//_AUDIO_DllConfig		 			= _AUDIO_AZIAUD_DllConfig;
		//_AUDIO_About		 				= _AUDIO_AZIAUD_DllAbout;
		//_AUDIO_Test			 			= _AUDIO_AZIAUD_DllTest;
		_AUDIO_GetDllInfo		 			= _AUDIO_AZIAUD_GetDllInfo;
		_AUDIO_Initialize	 				= _AUDIO_AZIAUD_InitiateAudio;
		_AUDIO_AiDacrateChanged 			= _AUDIO_AZIAUD_AiDacrateChanged;
		_AUDIO_AiLenChanged	 				= _AUDIO_AZIAUD_AiLenChanged;
		_AUDIO_AiReadLength	 				= _AUDIO_AZIAUD_AiReadLength;
		_AUDIO_AiUpdate		 				= _AUDIO_AZIAUD_AiUpdate;
		_AUDIO_ProcessAList	 				= _AUDIO_AZIAUD_ProcessAList;
		//_AUDIO_ProcessAList_Count_Cycles	= _AUDIO_AZIAUD_ProcessAListCountCycles;
		_AUDIO_IsMusyX					=  _AUDIO_AZIAUD_IsMusyX;
		//_AUDIO_AudioBoost		 			= _AUDIO_AZIAUD_AudioBoost;
			break;
#endif //_AUDIO_AZIAUD

#if defined(_AUDIO_BASIC)
	case _AudioPluginBasic :
		_AUDIO_RomClosed		 			= _AUDIO_BASIC_RomClosed;
		_AUDIO_DllClose		 				= _AUDIO_BASIC_CloseDLL;
		//_AUDIO_DllConfig		 			= _AUDIO_BASIC_DllConfig;
		_AUDIO_About		 				= _AUDIO_BASIC_DllAbout;
		//_AUDIO_Test			 			= _AUDIO_BASIC_DllTest;
		_AUDIO_GetDllInfo		 			= _AUDIO_BASIC_GetDllInfo;
		_AUDIO_Initialize	 				= _AUDIO_BASIC_InitiateAudio;
		_AUDIO_AiDacrateChanged 			= _AUDIO_BASIC_AiDacrateChanged;
		_AUDIO_AiLenChanged	 				= _AUDIO_BASIC_AiLenChanged;
		_AUDIO_AiReadLength	 				= _AUDIO_BASIC_AiReadLength;
		_AUDIO_AiUpdate		 				= _AUDIO_BASIC_AiUpdate;
		_AUDIO_ProcessAList	 				= _AUDIO_BASIC_ProcessAList;
		//_AUDIO_ProcessAList_Count_Cycles	= _AUDIO_BASIC_ProcessAListCountCycles;
		//_AUDIO_IsMusyX					= _AUDIO_BASIC_IsMusyX;
		//_AUDIO_AudioBoost		 			= _AUDIO_BASIC_AudioBoost;
			break;
#endif //_AUDIO_BASIC

#if defined(_AUDIO_DAEDALUS)
	case _AudioPluginDaedalus :
		_AUDIO_RomClosed		 			= _AUDIO_DAEDALUS_RomClosed;
		_AUDIO_DllClose		 				= _AUDIO_DAEDALUS_CloseDLL;
		//_AUDIO_DllConfig		 			= _AUDIO_DAEDALUS_DllConfig;
		_AUDIO_About		 				= _AUDIO_DAEDALUS_DllAbout;
		//_AUDIO_Test			 			= _AUDIO_DAEDALUS_DllTest;
		_AUDIO_GetDllInfo		 			= _AUDIO_DAEDALUS_GetDllInfo;
		_AUDIO_Initialize	 				= _AUDIO_DAEDALUS_InitiateAudio;
		_AUDIO_AiDacrateChanged 			= _AUDIO_DAEDALUS_AiDacrateChanged;
		_AUDIO_AiLenChanged	 				= _AUDIO_DAEDALUS_AiLenChanged;
		_AUDIO_AiReadLength	 				= _AUDIO_DAEDALUS_AiReadLength;
		_AUDIO_AiUpdate		 				= _AUDIO_DAEDALUS_AiUpdate;
		_AUDIO_ProcessAList	 				= _AUDIO_DAEDALUS_ProcessAList;
		//_AUDIO_AudioBoost		 			= _AUDIO_DAEDALUS_AudioBoost;
			break;
#endif //_AUDIO_DAEDALUS

#if defined(_AUDIO_JTTL)
	case _AudioPluginJttl :
		_AUDIO_RomClosed		 			= _AUDIO_JTTL_RomClosed;
		_AUDIO_DllClose		 				= _AUDIO_JTTL_CloseDLL;
		//_AUDIO_DllConfig		 			= _AUDIO_JTTL_DllConfig;
		_AUDIO_About		 				= _AUDIO_JTTL_DllAbout;
		//_AUDIO_Test			 			= _AUDIO_JTTL_DllTest;
		_AUDIO_GetDllInfo		 			= _AUDIO_JTTL_GetDllInfo;
		_AUDIO_Initialize	 				= _AUDIO_JTTL_InitiateAudio;
		_AUDIO_AiDacrateChanged 			= _AUDIO_JTTL_AiDacrateChanged;
		_AUDIO_AiLenChanged	 				= _AUDIO_JTTL_AiLenChanged;
		_AUDIO_AiReadLength	 				= _AUDIO_JTTL_AiReadLength;
		_AUDIO_AiUpdate		 				= _AUDIO_JTTL_AiUpdate;
		_AUDIO_ProcessAList	 				= _AUDIO_JTTL_ProcessAList;
		//_AUDIO_ProcessAList_Count_Cycles	= _AUDIO_JTTL_ProcessAListCountCycles;
		//_AUDIO_IsMusyX					= _AUDIO_JTTL_IsMusyX;
		_AUDIO_AudioBoost		 			= _AUDIO_JTTL_AudioBoost;
			break;
#endif //_AUDIO_JTTL

#if defined(_AUDIO_M64P)
	case _AudioPluginM64P :
		_AUDIO_RomClosed		 			= _AUDIO_M64P_RomClosed;
		//_AUDIO_DllClose		 				= _AUDIO_M64P_CloseDLL;
		//_AUDIO_DllConfig		 			= _AUDIO_M64P_DllConfig;
		//_AUDIO_About		 				= _AUDIO_M64P_DllAbout;
		//_AUDIO_Test			 			= _AUDIO_M64P_DllTest;
		//_AUDIO_GetDllInfo		 			= _AUDIO_M64P_GetDllInfo;
		_AUDIO_Initialize	 				= _AUDIO_M64P_InitiateAudio;
		_AUDIO_AiDacrateChanged 			= _AUDIO_M64P_AiDacrateChanged;
		_AUDIO_AiLenChanged	 				= _AUDIO_M64P_AiLenChanged;
		//_AUDIO_AiReadLength	 				= _AUDIO_M64P_AiReadLength;
		//_AUDIO_AiUpdate		 				= _AUDIO_M64P_AiUpdate;
		_AUDIO_ProcessAList	 				= _AUDIO_M64P_ProcessAList;
		//_AUDIO_ProcessAList_Count_Cycles	= _AUDIO_M64P_ProcessAListCountCycles;
		_AUDIO_IsMusyX					= _AUDIO_M64P_IsMusyX;
		_AUDIO_AudioBoost		 			= _AUDIO_M64P_AudioBoost;
			break;
#endif //_AUDIO_M64P

#if defined(_AUDIO_MUSYX)
	case _AudioPluginMusyX :
		_AUDIO_RomClosed		 			= _AUDIO_MUSYX_RomClosed;
		_AUDIO_DllClose		 				= _AUDIO_MUSYX_CloseDLL;
		//_AUDIO_DllConfig		 			= _AUDIO_MUSYX_DllConfig;
		_AUDIO_About		 				= _AUDIO_MUSYX_DllAbout;
		//_AUDIO_Test			 			= _AUDIO_MUSYX_DllTest;
		_AUDIO_GetDllInfo		 			= _AUDIO_MUSYX_GetDllInfo;
		_AUDIO_Initialize	 				= _AUDIO_MUSYX_InitiateAudio;
		_AUDIO_AiDacrateChanged 			= _AUDIO_MUSYX_AiDacrateChanged;
		_AUDIO_AiLenChanged	 				= _AUDIO_MUSYX_AiLenChanged;
		_AUDIO_AiReadLength	 				= _AUDIO_MUSYX_AiReadLength;
		_AUDIO_AiUpdate		 				= _AUDIO_MUSYX_AiUpdate;
		_AUDIO_ProcessAList	 				= _AUDIO_MUSYX_ProcessAList;
		_AUDIO_ProcessAList_Count_Cycles	= _AUDIO_MUSYX_ProcessAListCountCycles;
		_AUDIO_IsMusyX						= _AUDIO_MUSYX_IsMusyX;
		_AUDIO_AudioBoost		 			= _AUDIO_MUSYX_AudioBoost;
		_AUDIO_AudioMute		 			= _AUDIO_MUSYX_AudioMute;
			break;
#endif //_AUDIO_MUSYX

#if defined(_AUDIO_NONE)
	case _AudioPluginNone :
		_AUDIO_RomClosed		 			= _AUDIO_NONE_RomClosed;
		_AUDIO_DllClose		 				= _AUDIO_NONE_CloseDLL;
		//_AUDIO_DllConfig		 			= _AUDIO_NONE_DllConfig;
		_AUDIO_About		 				= _AUDIO_NONE_DllAbout;
		//_AUDIO_Test			 			= _AUDIO_NONE_DllTest;
		_AUDIO_GetDllInfo		 			= _AUDIO_NONE_GetDllInfo;
		_AUDIO_Initialize	 				= _AUDIO_NONE_InitiateAudio;
		_AUDIO_AiDacrateChanged 			= _AUDIO_NONE_AiDacrateChanged;
		_AUDIO_AiLenChanged	 				= _AUDIO_NONE_AiLenChanged;
		_AUDIO_AiReadLength	 				= _AUDIO_NONE_AiReadLength;
		_AUDIO_AiUpdate		 				= _AUDIO_NONE_AiUpdate;
		_AUDIO_ProcessAList	 				= _AUDIO_NONE_ProcessAList;
		//_AUDIO_ProcessAList_Count_Cycles	= _AUDIO_NONE_ProcessAListCountCycles;
		//_AUDIO_IsMusyX					= _AUDIO_NONE_IsMusyX;
		//_AUDIO_AudioBoost		 			= _AUDIO_NONE_AudioBoost;
			break;
#endif //_AUDIO_NONE

	default :
		DisplayError("No Audio Plugin Specified! (%i)", g_iAudioPlugin);
			break;
	}

	if(_AUDIO_AudioBoost != NULL) __try
	{
		_AUDIO_AudioBoost(g_bAudioBoost);
		_AUDIO_AudioBoost = NULL;
	}
	__except(NULL, EXCEPTION_EXECUTE_HANDLER)
	{
	}

	return TRUE;

#else //win32
	if(hinstLibAudio != NULL)
	{
		FreeLibrary(hinstLibAudio);
	}

	/* Load the Audio DLL */
	hinstLibAudio = LoadLibrary(libname);
	Audio_Info.hinst = hinstLibAudio; //bug fix. The other plugins don't use an hInstance...weird spec.

	if(hinstLibAudio != NULL)	/* Check if load DLL successfully */
	{
		/* Get the function address AUDIO_GetDllInfo in the audio DLL file */
		_AUDIO_GetDllInfo = (void(__cdecl *) (PLUGIN_INFO *)) GetProcAddress(hinstLibAudio, "GetDllInfo");

		if(_AUDIO_GetDllInfo != NULL)
		{
			/*~~~~~~~~~~~~~~~~~~~~*/
			PLUGIN_INFO Plugin_Info;
			/*~~~~~~~~~~~~~~~~~~~~*/

			ZeroMemory(&Plugin_Info, sizeof(Plugin_Info));

			AUDIO_GetDllInfo(&Plugin_Info);

			if(Plugin_Info.Type == PLUGIN_TYPE_AUDIO)
			{
				/* if(Plugin_Info.Version == 1) */
				{
					_AUDIO_AiDacrateChanged = (void(__cdecl *) (int)) GetProcAddress(hinstLibAudio, "AiDacrateChanged");
					_AUDIO_AiLenChanged = (void(__cdecl *) (void)) GetProcAddress(hinstLibAudio, "AiLenChanged");
					_AUDIO_AiReadLength = (DWORD(__cdecl *) (void)) GetProcAddress(hinstLibAudio, "AiReadLength");
					_AUDIO_AiUpdate = (void(__cdecl *) (BOOL)) GetProcAddress(hinstLibAudio, "AiUpdate");
					_AUDIO_DllClose = (void(__cdecl *) (void)) GetProcAddress(hinstLibAudio, "CloseDLL");

					_AUDIO_About = (void(__cdecl *) (HWND)) GetProcAddress(hinstLibAudio, "DllAbout");
					_AUDIO_DllConfig = (void(__cdecl *) (HWND)) GetProcAddress(hinstLibAudio, "DllConfig");
					_AUDIO_Test = (void(__cdecl *) (HWND)) GetProcAddress(hinstLibAudio, "DllTest");
					_AUDIO_Initialize = (BOOL(__cdecl *) (AUDIO_INFO)) GetProcAddress(hinstLibAudio, "InitiateAudio");
					_AUDIO_ProcessAList = (void(__cdecl *) (void)) GetProcAddress(hinstLibAudio, "ProcessAList");
					_AUDIO_ProcessAList_Count_Cycles = (DWORD(__cdecl *) (void)) GetProcAddress(hinstLibAudio, "ProcessAListCountCycles");
					_AUDIO_IsMusyX = (BOOL(__cdecl *) (void)) GetProcAddress(hinstLibAudio, "IsMusyX");

					_AUDIO_RomClosed = (void(__cdecl *) (void)) GetProcAddress(hinstLibAudio, "RomClosed");

					if( strstr(Plugin_Info.Name, "Jabo") != NULL || strstr(Plugin_Info.Name, "jabo") != NULL )
					{
						CoreDoingAIUpdate = FALSE;
						if( emuoptions.UsingRspPlugin == FALSE )
						{
							DisplayError("Warning, Jabo DirectSound Plugin is selected and loaded, but RSP Plugin is not "\
										"selected, Jabo DirectSound Plugin does not produce sound without RSP plugin. You can activate "\
										"RSP Plugin in plugin setting, or you can change to other audio plugins");
						}
					}
					else if( strstr(Plugin_Info.Name, "0.40 Beta 2") != NULL )
					{
						CoreDoingAIUpdate = FALSE;
					}
					else
					{
						CoreDoingAIUpdate = TRUE;
					}

#ifdef _DEBUG
					_SetDebuggerCallBack = (void (__cdecl *)(void (_cdecl *DbgCallBackFun)(char *msg))) 
						GetProcAddress(hinstControllerPlugin, "SetDebuggerCallBack");
					if( _SetDebuggerCallBack )
					{
						_SetDebuggerCallBack(DebuggerMsgCallBack);
					}
#endif

					return(TRUE);
				}
			}
		}
	}

	return FALSE;
#endif //_XBOX
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AUDIO_GetDllInfo(PLUGIN_INFO *Plugin_Info)
{
	if(_AUDIO_GetDllInfo != NULL) __try
	{
		_AUDIO_GetDllInfo(Plugin_Info);
	}

	__except(NULL, EXCEPTION_EXECUTE_HANDLER)
	{
	}
}

BOOL AUDIO_IsMusyX(void)
{
	BOOL IsMusyX = FALSE;
	
	if(_AUDIO_IsMusyX != NULL) __try
	{
		IsMusyX = _AUDIO_IsMusyX();
	}

	__except(NULL, EXCEPTION_EXECUTE_HANDLER)
	{	
		IsMusyX = FALSE;
	}

	return(IsMusyX);
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL AUDIO_Initialize(AUDIO_INFO Audio_Info)
{
	if(_AUDIO_Initialize != NULL) __try
	{
		_AUDIO_Initialize(Audio_Info);
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
DWORD AUDIO_ProcessAList(void)
{
	int k = 1600;

	/* try/except is handled from the call to this function */
	__try
	{
		_control87(_RC_NEAR|_PC_64, _MCW_RC|_MCW_PC);

		if(_AUDIO_ProcessAList_Count_Cycles != NULL && currentromoptions.RSP_RDP_Timing)
		{
			k = _AUDIO_ProcessAList_Count_Cycles();
		}
		else if(_AUDIO_ProcessAList != NULL) 
			_AUDIO_ProcessAList();

        RestoreOldRoundingMode(((uint32) cCON31 & 0x00000003) << 8);
	}

	__except(NULL, EXCEPTION_EXECUTE_HANDLER)
	{
		/* Some people won't have a sound card. No error. */
		//MessageBox(0, "Exception", "", 0);
	}

	return k;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AUDIO_RomClosed(void)
{
	if(_AUDIO_RomClosed != NULL)
	{
		__try
		{
			_AUDIO_RomClosed();
		}

		__except(NULL, EXCEPTION_EXECUTE_HANDLER)
		{
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AUDIO_DllClose(void)
{
	if(_AUDIO_DllClose != NULL)
	{
		__try
		{
			_AUDIO_DllClose();
		}

		__except(NULL, EXCEPTION_EXECUTE_HANDLER)
		{
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void CloseAudioPlugin(void)
{
#ifndef _XBOX
	/* AUDIO_End(); */
	AUDIO_RomClosed();
	AUDIO_DllClose();

	if(hinstLibAudio)
	{
		FreeLibrary(hinstLibAudio);
		hinstLibAudio = NULL;
	}

	_AUDIO_DllClose = NULL;
	_AUDIO_DllConfig = NULL;
	_AUDIO_About = NULL;
	_AUDIO_Test = NULL;
	_AUDIO_GetDllInfo = NULL;
	_AUDIO_Initialize = NULL;
	_AUDIO_End = NULL;
	_AUDIO_PlaySnd = NULL;
	_AUDIO_TimeLeft = NULL;
#endif //_XBOX
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AUDIO_DllConfig(HWND hParent)
{
	if(_AUDIO_DllConfig != NULL)
	{
		_AUDIO_DllConfig(hParent);
	}
	else
	{
		DisplayError("%s cannot be configured.", "Audio Plugin");
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AUDIO_About(HWND hParent)
{
	if(_AUDIO_About != NULL)
	{
		_AUDIO_About(hParent);
	}
	else
	{
		/*
		 * DisplayError("%s: About information is not available for this plug-in.", "Audio
		 * Plugin");
		 */
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AUDIO_Test(HWND hParent)
{
	if(_AUDIO_Test != NULL)
	{
		_AUDIO_Test(hParent);
	}
	else
	{
		/* DisplayError("%s: Test box is not available for this plug-in.", "Audio Plugin"); */
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AUDIO_AiDacrateChanged(int SystemType)
{
	if(_AUDIO_AiDacrateChanged != NULL)
	{
		_AUDIO_AiDacrateChanged(SystemType);
	}
	else
	{
		/* DisplayError("%s: Test box is not available for this plug-in.", "Audio Plugin"); */
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AUDIO_AiLenChanged(void)
{
	if(_AUDIO_AiLenChanged != NULL)
	{
		_AUDIO_AiLenChanged();
	}
	else
	{
		/* DisplayError("%s: Test box is not available for this plug-in.", "Audio Plugin"); */
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
DWORD AUDIO_AiReadLength(void)
{
	if(_AUDIO_AiReadLength != NULL)
	{
		return _AUDIO_AiReadLength();
	}
	else
	{
		/*
		 * DisplayError("%s: Test box is not available for this plug-in.", "Audio
		 * Plugin"); ?		 * return AI_LEN_REG;
		 */
		return 0;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AUDIO_AiUpdate(BOOL update)
{
	if(_AUDIO_AiUpdate != NULL)
	{
		_AUDIO_AiUpdate(update);
	}
	else
	{
		/* DisplayError("%s: Test box is not available for this plug-in.", "Audio Plugin"); */
	}

#ifndef _XBOX
	if( Kaillera_Is_Running )
	{
		Trigger_AIInterrupt();
	}
#endif //_XBOX
}

BOOL AUDIO_AudioMute(BOOL Mute)
{
	return _AUDIO_AudioMute(Mute);
}

/*
 =======================================================================================================================
    Used when selecting plugins
 =======================================================================================================================
 */
void AUDIO_Under_Selecting_About(HWND hParent)
{
	if(_AUDIO_Under_Selecting_About != NULL)
	{
		_AUDIO_Under_Selecting_About(hParent);
	}
	else
	{
		/*
		 * DisplayError("%s: About information is not available for this plug-in.", "Audio
		 * Plugin");
		 */
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AUDIO_Under_Selecting_Test(HWND hParent)
{
	if(_AUDIO_Under_Selecting_Test != NULL)
	{
		_AUDIO_Under_Selecting_Test(hParent);
	}
	else
	{
		/* DisplayError("%s: Test box is not available for this plug-in.", "Audio Plugin"); */
	}
}
