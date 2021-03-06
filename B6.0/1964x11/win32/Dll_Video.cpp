/*$T Dll_Video.c GC 1.136 03/09/02 17:41:40 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    Video plugin interface functions
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

#if defined(_VIDEO_1964_11)
#include <Video1964.h>
#elif defined(_VIDEO_RICE_510)
#include <VideoRice510.h>
#elif defined(_VIDEO_RICE_531)
#include <VideoRice531.h>
#elif defined(_VIDEO_RICE_560)
#include <VideoRice560.h>
#elif defined(_VIDEO_RICE_611)
#include <VideoRice611.h>
#elif defined(_VIDEO_RICE_612)
#include <VideoRice612.h>
#endif

uint16		GfxPluginVersion;
HINSTANCE	hinstLibVideo = NULL;
GFX_INFO	Gfx_Info;
int			UsingInternalVideo=0;
BOOL		bRomIsOpened = FALSE;

BOOL ToCaptureScreen=FALSE;
char *CaptureScreenDirectory=NULL;

BOOL (CALL *_VIDEO_InitiateGFX) (GFX_INFO) = NULL;
void (CALL *_VIDEO_ProcessDList) (void) = NULL;
DWORD (CALL *_VIDEO_ProcessDList_Count_Cycles) (void) = NULL;
void (CALL *_VIDEO_RomOpen) (void) = NULL;
void (CALL *_VIDEO_RomClosed) (void) = NULL;
void (CALL *_VIDEO_DllClose) () = NULL;
void (CALL *_VIDEO_UpdateScreen) () = NULL;
void (CALL *_VIDEO_GetDllInfo) (PLUGIN_INFO *) = NULL;
void (CALL *_VIDEO_ExtraChangeResolution) (HWND, long, HWND) = NULL;
void (CALL *_VIDEO_DllConfig) (HWND hParent) = NULL;
void (CALL *_VIDEO_Test) (HWND) = NULL;
void (CALL *_VIDEO_About) (HWND) = NULL;
void (CALL *_VIDEO_MoveScreen) (int, int) = NULL;
void (CALL *_VIDEO_DrawScreen) (void) = NULL;
void (CALL *_VIDEO_ViStatusChanged) (void) = NULL;
void (CALL *_VIDEO_ViWidthChanged) (void) = NULL;
void (CALL *_VIDEO_ChangeWindow) (int) = NULL;

/* For spec 1.3 */
void (CALL *_VIDEO_ChangeWindow_1_3) (void) = NULL;
void (CALL *_VIDEO_CaptureScreen) (char *Directory) = NULL;
void (CALL *_VIDEO_ProcessRDPList) (void) = NULL;
void (CALL *_VIDEO_ShowCFB) (void) = NULL;

/* Used when selecting plugins */
void (CALL *_VIDEO_Under_Selecting_Test) (HWND) = NULL;
void (CALL *_VIDEO_Under_Selecting_About) (HWND) = NULL;

void (CALL *_VIDEO_FrameBufferWrite) (DWORD addr, DWORD size) = NULL;
//#ifndef _XBOX
void (CALL *_VIDEO_FrameBufferWriteList) (FrameBufferModifyEntry *plist, DWORD size) = NULL;
//#endif
void (CALL *_VIDEO_FrameBufferRead) (DWORD addr) = NULL;
void (CALL *_VIDEO_GetFrameBufferInfo) (void *pinfo) = NULL;
void (CALL *_VIDEO_SetOnScreenText) (char *msg) = NULL;
BOOL (CALL *_VIDEO_GetFullScreenStatus) (void) = NULL;

#ifdef _XBOX
void (CALL *_VIDEO_SetMaxTextureMem) (DWORD mem) = NULL;
#endif //_XBOX

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL LoadVideoPlugin(char *libname)
{
#ifdef _XBOX

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
	_VIDEO_ShowCFB							= _VIDEO_1964_11_ShowCFB;
	
	_VIDEO_FrameBufferWrite					= _VIDEO_1964_11_FBWrite;
	_VIDEO_FrameBufferWriteList				= _VIDEO_1964_11_FBWList;
	_VIDEO_FrameBufferRead					= _VIDEO_1964_11_FBRead;
	_VIDEO_GetFrameBufferInfo				= _VIDEO_1964_11_FBGetFrameBufferInfo;
	//_VIDEO_SetOnScreenText				= _VIDEO_1964_11_SetOnScreenText;
	//_VIDEO_GetFullScreenStatus			= _VIDEO_1964_11_GetFullScreenStatus;
	
	_VIDEO_SetMaxTextureMem					= _VIDEO_1964_11_SetMaxTextureMem;

#elif defined(_VIDEO_RICE_510)

	GfxPluginVersion						= 0x0102;
	_VIDEO_InitiateGFX						= _VIDEO_RICE_510_InitiateGFX;
	_VIDEO_ProcessDList						= _VIDEO_RICE_510_ProcessDList;
	_VIDEO_ProcessDList_Count_Cycles		= _VIDEO_RICE_510_ProcessDListCountCycles;
	_VIDEO_RomOpen							= _VIDEO_RICE_510_RomOpen;
	_VIDEO_RomClosed						= _VIDEO_RICE_510_RomClosed;
	_VIDEO_DllClose							= _VIDEO_RICE_510_CloseDLL;
	_VIDEO_UpdateScreen						= _VIDEO_RICE_510_UpdateScreen;
	_VIDEO_GetDllInfo						= _VIDEO_RICE_510_GetDllInfo;
	//_VIDEO_ExtraChangeResolution			= 
	_VIDEO_DllConfig						= _VIDEO_RICE_510_DllConfig;
	_VIDEO_Test								= _VIDEO_RICE_510_DllTest;
	_VIDEO_About							= _VIDEO_RICE_510_DllAbout;
	_VIDEO_MoveScreen						= _VIDEO_RICE_510_MoveScreen;
	_VIDEO_DrawScreen						= _VIDEO_RICE_510_DrawScreen;
	_VIDEO_ViStatusChanged					= _VIDEO_RICE_510_ViStatusChanged;
	_VIDEO_ViWidthChanged					= _VIDEO_RICE_510_ViWidthChanged;
	//_VIDEO_ChangeWindow					= _VIDEO_RICE_510_ChangeWindow;
	
	_VIDEO_ChangeWindow_1_3					= _VIDEO_RICE_510_ChangeWindow;
	//_VIDEO_CaptureScreen					= _VIDEO_RICE_510_CaptureScreen;
	_VIDEO_ProcessRDPList					= _VIDEO_RICE_510_ProcessRDPList;
	//_VIDEO_ShowCFB						= _VIDEO_RICE_510_ShowCFB;
	
	_VIDEO_FrameBufferWrite					= _VIDEO_RICE_510_FBWrite;
	//_VIDEO_FrameBufferWriteList				= _VIDEO_RICE_510_FBWList;
	_VIDEO_FrameBufferRead					= _VIDEO_RICE_510_FBRead;
	//_VIDEO_GetFrameBufferInfo				= _VIDEO_RICE_510_FBGetFrameBufferInfo;
	//_VIDEO_SetOnScreenText				= _VIDEO_RICE_510_SetOnScreenText;
	//_VIDEO_GetFullScreenStatus			= _VIDEO_RICE_510_GetFullScreenStatus;
	
	_VIDEO_SetMaxTextureMem					= _VIDEO_RICE_510_SetMaxTextureMem;


#elif defined(_VIDEO_RICE_531)

	GfxPluginVersion						= 0x0102;
	_VIDEO_InitiateGFX						= _VIDEO_RICE_531_InitiateGFX;
	_VIDEO_ProcessDList						= _VIDEO_RICE_531_ProcessDList;
	_VIDEO_ProcessDList_Count_Cycles		= _VIDEO_RICE_531_ProcessDListCountCycles;
	_VIDEO_RomOpen							= _VIDEO_RICE_531_RomOpen;
	_VIDEO_RomClosed						= _VIDEO_RICE_531_RomClosed;
	_VIDEO_DllClose							= _VIDEO_RICE_531_CloseDLL;
	_VIDEO_UpdateScreen						= _VIDEO_RICE_531_UpdateScreen;
	_VIDEO_GetDllInfo						= _VIDEO_RICE_531_GetDllInfo;
	//_VIDEO_ExtraChangeResolution			= 
	_VIDEO_DllConfig						= _VIDEO_RICE_531_DllConfig;
	_VIDEO_Test								= _VIDEO_RICE_531_DllTest;
	_VIDEO_About							= _VIDEO_RICE_531_DllAbout;
	_VIDEO_MoveScreen						= _VIDEO_RICE_531_MoveScreen;
	_VIDEO_DrawScreen						= _VIDEO_RICE_531_DrawScreen;
	_VIDEO_ViStatusChanged					= _VIDEO_RICE_531_ViStatusChanged;
	_VIDEO_ViWidthChanged					= _VIDEO_RICE_531_ViWidthChanged;
	//_VIDEO_ChangeWindow					= _VIDEO_RICE_531_ChangeWindow;
	
	_VIDEO_ChangeWindow_1_3					= _VIDEO_RICE_531_ChangeWindow;
	//_VIDEO_CaptureScreen					= _VIDEO_RICE_531_CaptureScreen;
	_VIDEO_ProcessRDPList					= _VIDEO_RICE_531_ProcessRDPList;
	//_VIDEO_ShowCFB						= _VIDEO_RICE_531_ShowCFB;
	
	_VIDEO_FrameBufferWrite					= _VIDEO_RICE_531_FBWrite;
	//_VIDEO_FrameBufferWriteList				= _VIDEO_RICE_531_FBWList;
	_VIDEO_FrameBufferRead					= _VIDEO_RICE_531_FBRead;
	//_VIDEO_GetFrameBufferInfo				= _VIDEO_RICE_531_FBGetFrameBufferInfo;
	//_VIDEO_SetOnScreenText				= _VIDEO_RICE_531_SetOnScreenText;
	//_VIDEO_GetFullScreenStatus			= _VIDEO_RICE_531_GetFullScreenStatus;
	
	_VIDEO_SetMaxTextureMem					= _VIDEO_RICE_531_SetMaxTextureMem;


#elif defined(_VIDEO_RICE_560)
	GfxPluginVersion						= 0x0103;
	_VIDEO_InitiateGFX						= _VIDEO_RICE_560_InitiateGFX;
	_VIDEO_ProcessDList						= _VIDEO_RICE_560_ProcessDList;
	_VIDEO_ProcessDList_Count_Cycles		= _VIDEO_RICE_560_ProcessDListCountCycles;
	_VIDEO_RomOpen							= _VIDEO_RICE_560_RomOpen;
	_VIDEO_RomClosed						= _VIDEO_RICE_560_RomClosed;
	_VIDEO_DllClose							= _VIDEO_RICE_560_CloseDLL;
	_VIDEO_UpdateScreen						= _VIDEO_RICE_560_UpdateScreen;
	_VIDEO_GetDllInfo						= _VIDEO_RICE_560_GetDllInfo;
	//_VIDEO_ExtraChangeResolution			= 
	_VIDEO_DllConfig						= _VIDEO_RICE_560_DllConfig;
	_VIDEO_Test								= _VIDEO_RICE_560_DllTest;
	_VIDEO_About							= _VIDEO_RICE_560_DllAbout;
	_VIDEO_MoveScreen						= _VIDEO_RICE_560_MoveScreen;
	_VIDEO_DrawScreen						= _VIDEO_RICE_560_DrawScreen;
	_VIDEO_ViStatusChanged					= _VIDEO_RICE_560_ViStatusChanged;
	_VIDEO_ViWidthChanged					= _VIDEO_RICE_560_ViWidthChanged;
	//_VIDEO_ChangeWindow					= _VIDEO_RICE_560_ChangeWindow;
	
	_VIDEO_ChangeWindow_1_3					= _VIDEO_RICE_560_ChangeWindow;
	//_VIDEO_CaptureScreen					= _VIDEO_RICE_560_CaptureScreen;
	_VIDEO_ProcessRDPList					= _VIDEO_RICE_560_ProcessRDPList;
	//_VIDEO_ShowCFB						= _VIDEO_RICE_560_ShowCFB;
	
	_VIDEO_FrameBufferWrite					= _VIDEO_RICE_560_FBWrite;
	//_VIDEO_FrameBufferWriteList				= _VIDEO_RICE_560_FBWList;
	_VIDEO_FrameBufferRead					= _VIDEO_RICE_560_FBRead;
	_VIDEO_GetFrameBufferInfo				= _VIDEO_RICE_560_FBGetFrameBufferInfo;
	//_VIDEO_SetOnScreenText				= _VIDEO_RICE_560_SetOnScreenText;
	//_VIDEO_GetFullScreenStatus			= _VIDEO_RICE_560_GetFullScreenStatus;
	
	_VIDEO_SetMaxTextureMem					= _VIDEO_RICE_560_SetMaxTextureMem;

#elif defined(_VIDEO_RICE_611)
	GfxPluginVersion						= 0x0103;
	_VIDEO_InitiateGFX						= _VIDEO_RICE_611_InitiateGFX;
	_VIDEO_ProcessDList						= _VIDEO_RICE_611_ProcessDList;
	_VIDEO_ProcessDList_Count_Cycles		= _VIDEO_RICE_611_ProcessDListCountCycles;
	_VIDEO_RomOpen							= _VIDEO_RICE_611_RomOpen;
	_VIDEO_RomClosed						= _VIDEO_RICE_611_RomClosed;
	_VIDEO_DllClose							= _VIDEO_RICE_611_CloseDLL;
	_VIDEO_UpdateScreen						= _VIDEO_RICE_611_UpdateScreen;
	_VIDEO_GetDllInfo						= _VIDEO_RICE_611_GetDllInfo;
	//_VIDEO_ExtraChangeResolution			= 
	_VIDEO_DllConfig						= _VIDEO_RICE_611_DllConfig;
	_VIDEO_Test								= _VIDEO_RICE_611_DllTest;
	_VIDEO_About							= _VIDEO_RICE_611_DllAbout;
	_VIDEO_MoveScreen						= _VIDEO_RICE_611_MoveScreen;
	_VIDEO_DrawScreen						= _VIDEO_RICE_611_DrawScreen;
	_VIDEO_ViStatusChanged					= _VIDEO_RICE_611_ViStatusChanged;
	_VIDEO_ViWidthChanged					= _VIDEO_RICE_611_ViWidthChanged;
	//_VIDEO_ChangeWindow					= _VIDEO_RICE_611_ChangeWindow;
	
	_VIDEO_ChangeWindow_1_3					= _VIDEO_RICE_611_ChangeWindow;
	//_VIDEO_CaptureScreen					= _VIDEO_RICE_611_CaptureScreen;
	_VIDEO_ProcessRDPList					= _VIDEO_RICE_611_ProcessRDPList;
	//_VIDEO_ShowCFB						= _VIDEO_RICE_611_ShowCFB;
	
	_VIDEO_FrameBufferWrite					= _VIDEO_RICE_611_FBWrite;
	//_VIDEO_FrameBufferWriteList				= _VIDEO_RICE_611_FBWList;
	_VIDEO_FrameBufferRead					= _VIDEO_RICE_611_FBRead;
	_VIDEO_GetFrameBufferInfo				= _VIDEO_RICE_611_FBGetFrameBufferInfo;
	//_VIDEO_SetOnScreenText				= _VIDEO_RICE_611_SetOnScreenText;
	//_VIDEO_GetFullScreenStatus			= _VIDEO_RICE_611_GetFullScreenStatus;
	
	_VIDEO_SetMaxTextureMem					= _VIDEO_RICE_611_SetMaxTextureMem;

#elif defined(_VIDEO_RICE_612)
	GfxPluginVersion						= 0x0103;
	_VIDEO_InitiateGFX						= _VIDEO_RICE_612_InitiateGFX;
	_VIDEO_ProcessDList						= _VIDEO_RICE_612_ProcessDList;
	_VIDEO_ProcessDList_Count_Cycles		= _VIDEO_RICE_612_ProcessDListCountCycles;
	_VIDEO_RomOpen							= _VIDEO_RICE_612_RomOpen;
	_VIDEO_RomClosed						= _VIDEO_RICE_612_RomClosed;
	_VIDEO_DllClose							= _VIDEO_RICE_612_CloseDLL;
	_VIDEO_UpdateScreen						= _VIDEO_RICE_612_UpdateScreen;
	_VIDEO_GetDllInfo						= _VIDEO_RICE_612_GetDllInfo;
	//_VIDEO_ExtraChangeResolution			= 
	_VIDEO_DllConfig						= _VIDEO_RICE_612_DllConfig;
	_VIDEO_Test								= _VIDEO_RICE_612_DllTest;
	_VIDEO_About							= _VIDEO_RICE_612_DllAbout;
	_VIDEO_MoveScreen						= _VIDEO_RICE_612_MoveScreen;
	_VIDEO_DrawScreen						= _VIDEO_RICE_612_DrawScreen;
	_VIDEO_ViStatusChanged					= _VIDEO_RICE_612_ViStatusChanged;
	_VIDEO_ViWidthChanged					= _VIDEO_RICE_612_ViWidthChanged;
	//_VIDEO_ChangeWindow					= _VIDEO_RICE_612_ChangeWindow;
	
	_VIDEO_ChangeWindow_1_3					= _VIDEO_RICE_612_ChangeWindow;
	//_VIDEO_CaptureScreen					= _VIDEO_RICE_612_CaptureScreen;
	_VIDEO_ProcessRDPList					= _VIDEO_RICE_612_ProcessRDPList;
	_VIDEO_ShowCFB							= _VIDEO_RICE_612_ShowCFB;
	
	_VIDEO_FrameBufferWrite					= _VIDEO_RICE_612_FBWrite;
	_VIDEO_FrameBufferWriteList				= _VIDEO_RICE_612_FBWList;
	_VIDEO_FrameBufferRead					= _VIDEO_RICE_612_FBRead;
	_VIDEO_GetFrameBufferInfo				= _VIDEO_RICE_612_FBGetFrameBufferInfo;
	//_VIDEO_SetOnScreenText				= _VIDEO_RICE_612_SetOnScreenText;
	//_VIDEO_GetFullScreenStatus			= _VIDEO_RICE_612_GetFullScreenStatus;
	
	_VIDEO_SetMaxTextureMem					= _VIDEO_RICE_612_SetMaxTextureMem;
#else
	DisplayError("No Video Plugin Defined!");
#endif

	return TRUE;

#else //win32
	/* Release the video plug-in if it has already been loaded */
	if(hinstLibVideo != NULL)
	{
		FreeLibrary(hinstLibVideo);
	}

	hinstLibVideo = LoadLibrary(libname);

	if(hinstLibVideo != NULL)						/* Here the library is loaded successfully */
	{
		/* Get the VIDEO_GetDllInfo function address in the loaded DLL file */
		_VIDEO_GetDllInfo = (void(__cdecl *) (PLUGIN_INFO *)) GetProcAddress(hinstLibVideo, "GetDllInfo");

		if(_VIDEO_GetDllInfo != NULL)
		{
			/*~~~~~~~~~~~~~~~~~~~~*/
			PLUGIN_INFO Plugin_Info;
			/*~~~~~~~~~~~~~~~~~~~~*/

			ZeroMemory(&Plugin_Info, sizeof(Plugin_Info));

			VIDEO_GetDllInfo(&Plugin_Info);
			GfxPluginVersion = Plugin_Info.Version;

			if(Plugin_Info.Type == PLUGIN_TYPE_GFX) /* Check if this is a video plugin */
			{
				_VIDEO_DllClose = (void(__cdecl *) (void)) GetProcAddress(hinstLibVideo, "CloseDLL");
				_VIDEO_ExtraChangeResolution = (void(__cdecl *) (HWND, long, HWND)) GetProcAddress
					(
						hinstLibVideo,
						"ChangeWinSize"
					);
				_VIDEO_Test = (void(__cdecl *) (HWND)) GetProcAddress(hinstLibVideo, "DllTest");
				_VIDEO_About = (void(__cdecl *) (HWND)) GetProcAddress(hinstLibVideo, "DllAbout");
				_VIDEO_DllConfig = (void(__cdecl *) (HWND)) GetProcAddress(hinstLibVideo, "DllConfig");
				_VIDEO_MoveScreen = (void(__cdecl *) (int, int)) GetProcAddress(hinstLibVideo, "MoveScreen");
				_VIDEO_DrawScreen = (void(__cdecl *) (void)) GetProcAddress(hinstLibVideo, "DrawScreen");
				_VIDEO_ViStatusChanged = (void(__cdecl *) (void)) GetProcAddress(hinstLibVideo, "ViStatusChanged");
				_VIDEO_ViWidthChanged = (void(__cdecl *) (void)) GetProcAddress(hinstLibVideo, "ViWidthChanged");
				_VIDEO_InitiateGFX = (BOOL(__cdecl *) (GFX_INFO)) GetProcAddress(hinstLibVideo, "InitiateGFX");
				_VIDEO_RomOpen = (void(__cdecl *) (void)) GetProcAddress(hinstLibVideo, "RomOpen");
				_VIDEO_RomClosed = (void(__cdecl *) (void)) GetProcAddress(hinstLibVideo, "RomClosed");
				_VIDEO_ProcessDList = (void(__cdecl *) (void)) GetProcAddress(hinstLibVideo, "ProcessDList");
				_VIDEO_ProcessDList_Count_Cycles = (DWORD(__cdecl *) (void)) GetProcAddress(hinstLibVideo, "ProcessDListCountCycles");
				_VIDEO_UpdateScreen = (void(__cdecl *) (void)) GetProcAddress(hinstLibVideo, "UpdateScreen");
				_VIDEO_ChangeWindow = (void(__cdecl *) (int)) GetProcAddress(hinstLibVideo, "ChangeWindow");

				/* for spec 1.3 */
				_VIDEO_ChangeWindow_1_3 = (void(__cdecl *) (void)) GetProcAddress(hinstLibVideo, "ChangeWindow");
				_VIDEO_CaptureScreen = (void(__cdecl *) (char *)) GetProcAddress(hinstLibVideo, "CaptureScreen");
				_VIDEO_ProcessRDPList = (void(__cdecl *) (void)) GetProcAddress(hinstLibVideo, "ProcessRDPList");
				_VIDEO_ShowCFB = (void(__cdecl *) (void)) GetProcAddress(hinstLibVideo, "ShowCFB");

				_VIDEO_FrameBufferWriteList = (void(__cdecl *) (FrameBufferModifyEntry *, DWORD)) GetProcAddress(hinstLibVideo, "FBWList");
				_VIDEO_FrameBufferRead = (void(__cdecl *) (DWORD)) GetProcAddress(hinstLibVideo, "FBRead");
				_VIDEO_FrameBufferWrite = (void(__cdecl *) (DWORD, DWORD)) GetProcAddress(hinstLibVideo, "FBWrite");
				_VIDEO_GetFrameBufferInfo = (void(__cdecl *) (void *pinfo)) GetProcAddress(hinstLibVideo, "FBGetFrameBufferInfo");
				_VIDEO_GetFullScreenStatus = (BOOL (__cdecl *) (void)) GetProcAddress(hinstLibVideo, "GetFullScreenStatus");
				_VIDEO_SetOnScreenText = (void (__cdecl *)(char *msg)) GetProcAddress(hinstLibVideo, "SetOnScreenText");

				if( _VIDEO_FrameBufferRead == NULL && _VIDEO_FrameBufferWrite == NULL )
				{
					emustatus.VideoPluginSupportingFrameBuffer = FALSE;
				}
				else
				{
					emustatus.VideoPluginSupportingFrameBuffer = TRUE;
				}

				if( _VIDEO_GetFrameBufferInfo == NULL )
				{
					emustatus.VideoPluginProvideFrameBufferInfo = FALSE;
				}
				else
				{
					emustatus.VideoPluginProvideFrameBufferInfo = TRUE;
				}

				return(TRUE);
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
void VIDEO_GetDllInfo(PLUGIN_INFO *Plugin_Info)
{
	if(_VIDEO_GetDllInfo != NULL)
	{
		__try
		{
			_VIDEO_GetDllInfo(Plugin_Info);
		}

		__except(NULL, EXCEPTION_EXECUTE_HANDLER)
		{
			DisplayError("GettDllInfo Failed.");
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */

#ifndef _XBOX
g_GFX_PluginRECT GFX_PluginRECT;

// If the plugin wants to resize the window, respect its wishes,
// but not until after the rom is loaded.
void GetPluginsResizeRequest(LPRECT lpRect)
{
	RECT RequestRect;
	GetWindowRect(gui.hwnd1964main, &RequestRect);
	
	
	if ( (RequestRect.right  != lpRect->right)  || 
	   (RequestRect.left   != lpRect->left)   ||
	   (RequestRect.top    != lpRect->top)    || 
	   (RequestRect.bottom != lpRect->bottom) )

		if ( ((RequestRect.right - RequestRect.left) > 300) && 
			 ((RequestRect.bottom - RequestRect.top) > 200) )
		{
			GFX_PluginRECT.rect.left   = RequestRect.left;
			GFX_PluginRECT.rect.right  = RequestRect.right;
			GFX_PluginRECT.rect.top    = RequestRect.top;
			GFX_PluginRECT.rect.bottom = RequestRect.bottom;
			GFX_PluginRECT.UseThis     = TRUE;
		}
}
#endif

BOOL VIDEO_InitiateGFX(GFX_INFO Gfx_Info)
{
#ifndef _XBOX
	RECT Rect;
	
	GFX_PluginRECT.UseThis = FALSE;
	bRomIsOpened = FALSE;
#endif

	__try
	{
		if (!UsingInternalVideo) //Make this a guistatus or emustatus or something.
		{
#ifdef _XBOX
			_VIDEO_InitiateGFX(Gfx_Info);

#else //win32
			GetWindowRect(gui.hwnd1964main, &Rect);
			_VIDEO_InitiateGFX(Gfx_Info);
			GetPluginsResizeRequest(&Rect);
#endif
		}
		else
		{
		//	Internal_VIDEO_InitiateGFX(Gfx_Info);
		}
	}
	__except(NULL, EXCEPTION_EXECUTE_HANDLER)
	{
		/* DisplayError("Cannot Initialize Graphics"); */
	}

	return(1);	/* why not for now.. */
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
DWORD VIDEO_ProcessDList(void)
{
	int k = 100;

    _control87(_RC_NEAR|_PC_64, _MCW_RC|_MCW_PC);    

    /* try/except is handled from the call */
	
#ifndef _DEBUG
	__try
#endif
	{
		if(_VIDEO_ProcessDList_Count_Cycles != NULL && currentromoptions.RSP_RDP_Timing)
			k = _VIDEO_ProcessDList_Count_Cycles();
		else if(_VIDEO_ProcessDList != NULL) 
			_VIDEO_ProcessDList();
	}
#ifndef _DEBUG
	__except(NULL, EXCEPTION_EXECUTE_HANDLER)
	{
		DisplayError("Exception in VIDEO_ProcessDList");
		Trigger_DPInterrupt();
	}
#endif

    RestoreOldRoundingMode(((uint32) cCON31 & 0x00000003) << 8);

#ifndef _XBOX
	if( ToCaptureScreen && CaptureScreenDirectory != NULL )
	{
		__try
		{
			_VIDEO_CaptureScreen(CaptureScreenDirectory);
		}

		__except(NULL, EXCEPTION_EXECUTE_HANDLER)
		{
			DisplayError("Exception in Capture Screen");
		}
		ToCaptureScreen = FALSE;
	}
#endif

	return k;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void VIDEO_RomOpen(void)
{
	if(_VIDEO_RomOpen != NULL)
	{
		__try
		{
			if (!UsingInternalVideo)
			{
#ifdef _XBOX
				_VIDEO_RomOpen();

#else //win32
				RECT Rect;
				GetWindowRect(gui.hwnd1964main, &Rect);
				_VIDEO_RomOpen();
				bRomIsOpened = TRUE;
				GetPluginsResizeRequest(&Rect);
#endif
			}
			else
			{
//				Internal_VIDEO_RomOpen();
			}
		}

		__except(NULL, EXCEPTION_EXECUTE_HANDLER)
		{
			DisplayError("Video RomOpen Failed.");
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void VIDEO_RomClosed(void)
{
	if(_VIDEO_RomClosed != NULL)
	{
		__try
		{
#ifdef _XBOX
			_VIDEO_RomClosed();

#else //win32
			if( bRomIsOpened )
			{
				bRomIsOpened = FALSE;
				_VIDEO_RomClosed();
			}
#endif
		}

		__except(NULL, EXCEPTION_EXECUTE_HANDLER)
		{
			DisplayError("Video RomClosed Failed.");
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void VIDEO_ChangeWindow(int window)
{
	int passed = 0;

	if(GfxPluginVersion == 0x0103)
	{
		if(_VIDEO_ChangeWindow_1_3 != NULL)
		{
			__try
			{
				_VIDEO_ChangeWindow_1_3();
				guistatus.IsFullScreen ^= 1;
				passed = 1;
			}

			__except(NULL, EXCEPTION_EXECUTE_HANDLER)
			{
				DisplayError("VIDEO ChangeWindow failed");
				passed = 0;
			}
		}
	}
	else
	{
		if(_VIDEO_ChangeWindow != NULL)
		{
			__try
			{
				_VIDEO_ChangeWindow(window);
				guistatus.IsFullScreen ^= 1;
				passed = 1;
			}

			__except(NULL, EXCEPTION_EXECUTE_HANDLER)
			{
				DisplayError("VIDEO ChangeWindow failed");
				passed = 0;
			}
		}
	}
	
#ifndef _XBOX //win32
	if( guistatus.IsFullScreen && (passed==1))
	{
		EnableWindow(gui.hToolBar, FALSE);
		ShowWindow(gui.hToolBar, SW_HIDE);
		EnableWindow(gui.hReBar, FALSE);
		ShowWindow(gui.hReBar, SW_HIDE);
		EnableWindow((HWND)gui.hMenu1964main, FALSE);
		ShowWindow((HWND)gui.hMenu1964main, FALSE);
		ShowWindow(gui.hStatusBar, SW_HIDE);
		ShowCursor(FALSE);
	}
	else
	{
		ShowWindow(gui.hReBar, SW_SHOW);
		EnableWindow(gui.hReBar, TRUE);
		EnableWindow(gui.hToolBar, TRUE);
		EnableWindow((HWND)gui.hMenu1964main, TRUE);
		ShowWindow(gui.hToolBar, SW_SHOW);
		ShowWindow(gui.hStatusBar, SW_SHOW);
		ShowWindow((HWND)gui.hMenu1964main, TRUE);
		ShowCursor(TRUE);
		DockStatusBar();
	}
#endif //_XBOX //win32
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void VIDEO_DllClose(void)
{
	if(_VIDEO_DllClose != NULL)
	{
		__try
		{
			if (!UsingInternalVideo)
			_VIDEO_DllClose();
		}

		__except(NULL, EXCEPTION_EXECUTE_HANDLER)
		{
			DisplayError("VIDEO DllClose failed");
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void CloseVideoPlugin(void)
{
#ifndef _XBOX
	VIDEO_DllClose();

	if(hinstLibVideo) FreeLibrary(hinstLibVideo);

	hinstLibVideo = NULL;

	_VIDEO_InitiateGFX = NULL;
	_VIDEO_ProcessDList = NULL;
	_VIDEO_ProcessDList_Count_Cycles = NULL;
	_VIDEO_RomOpen = NULL;
	_VIDEO_DllClose = NULL;
	_VIDEO_DllConfig = NULL;
	_VIDEO_GetDllInfo = NULL;
	_VIDEO_UpdateScreen = NULL;
	_VIDEO_ExtraChangeResolution = NULL;

	_VIDEO_ChangeWindow = NULL;
	_VIDEO_Test = NULL;
	_VIDEO_About = NULL;
	_VIDEO_MoveScreen = NULL;
#endif
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void VIDEO_DllConfig(HWND hParent)
{
#ifndef _XBOX
	RECT Rect;

	if(_VIDEO_DllConfig != NULL)
	{
		GetWindowRect(gui.hwnd1964main, &Rect);
		_VIDEO_DllConfig(hParent);
		GetPluginsResizeRequest(&Rect);
		if (Rom_Loaded == FALSE)
		SetWindowPos(gui.hwnd1964main, NULL, Rect.left, Rect.top, 
			Rect.right-Rect.left, 
			Rect.bottom-Rect.top,
			SWP_NOZORDER | SWP_SHOWWINDOW);
	}
	else
	{
		DisplayError("%s cannot be configured.", "Video Plugin");
	}
#endif
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void VIDEO_About(HWND hParent)
{
	if(_VIDEO_About != NULL)
	{
		if (!UsingInternalVideo)
		_VIDEO_About(hParent);
	}
	else
	{
		DisplayError("%s: About information is not available for this plug-in.", "Video Plugin");
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void VIDEO_Test(HWND hParent)
{
	if(_VIDEO_Test != NULL)
	{
		if (!UsingInternalVideo)
			_VIDEO_Test(hParent);
	}
	else
	{
		DisplayError("%s: Test function is not available for this plug-in.", "Video Plugin");
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void VIDEO_MoveScreen(int x, int y)
{
	if(_VIDEO_MoveScreen != NULL)
	{
		if (!UsingInternalVideo)
			_VIDEO_MoveScreen(x, y);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
#include "../n64rcp.h"
void VIDEO_UpdateScreen(void)
{
	//static int recall = 0x04000000+307200*2;
	//static int k=0;


	if(_VIDEO_UpdateScreen != NULL) __try
	{
		if (!UsingInternalVideo)
		{
			_VIDEO_UpdateScreen();
		}
	}
	__except(NULL, EXCEPTION_EXECUTE_HANDLER)
	{
		DisplayError("Video UpdateScreen failed.");
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void VIDEO_DrawScreen(void)
{
	if(_VIDEO_DrawScreen != NULL) __try
	{
		if (!UsingInternalVideo)
			_VIDEO_DrawScreen();
	}

	__except(NULL, EXCEPTION_EXECUTE_HANDLER)
	{
		DisplayError("Video DrawScreen failed.");
	}

	//VIDEO_UpdateScreen();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void VIDEO_ViStatusChanged(void)
{
	if(_VIDEO_ViStatusChanged != NULL)
	{
		__try
		{
			if (!UsingInternalVideo)
				_VIDEO_ViStatusChanged();
		}

		__except(NULL, EXCEPTION_EXECUTE_HANDLER)
		{
			DisplayError("Exception in ViStatusChanged");
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void VIDEO_ViWidthChanged(void)
{
	if(_VIDEO_ViWidthChanged != NULL)
	{
		__try
		{
			if (!UsingInternalVideo)
				_VIDEO_ViWidthChanged();
		}

		__except(NULL, EXCEPTION_EXECUTE_HANDLER)
		{
			DisplayError("Exception in ViWidthChanged");
		}
	}
}

/*
 =======================================================================================================================
    changes for spec 1.3
 =======================================================================================================================
 */
void VIDEO_CaptureScreen(char *Directory)
{
	// Call from GUI thread
#ifndef _XBOX
	if(_VIDEO_CaptureScreen != NULL)
	{
		ToCaptureScreen = TRUE;
		CaptureScreenDirectory = Directory;
	}
#endif
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void VIDEO_ProcessRDPList(void)
{
	if(_VIDEO_ProcessRDPList != NULL)
	{
		__try
		{
			_VIDEO_ProcessRDPList();
		}

		__except(NULL, EXCEPTION_EXECUTE_HANDLER)
		{
			DisplayError("Exception in Processing RDP List");
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void VIDEO_ShowCFB(void)
{
//#ifndef _XBOX
	if(_VIDEO_ShowCFB != NULL)
	{
		__try
		{
			_VIDEO_ShowCFB();
		}

		__except(NULL, EXCEPTION_EXECUTE_HANDLER)
		{
			DisplayError("Exception in VIDEO_ShowCFB");
		}
	}
//#endif
}

/*
 =======================================================================================================================
    Used when selecting plugins
 =======================================================================================================================
 */
#ifndef _XBOX
void VIDEO_Under_Selecting_About(HWND hParent)
{
	if(_VIDEO_Under_Selecting_About != NULL)
	{
		_VIDEO_Under_Selecting_About(hParent);
	}
	else
	{
		DisplayError("%s: About information is not available for this plug-in.", "Video Plugin");
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void VIDEO_Under_Selecting_Test(HWND hParent)
{
	if(_VIDEO_Under_Selecting_Test != NULL)
	{
		_VIDEO_Under_Selecting_Test(hParent);
	}
	else
	{
		DisplayError("%s: Test function is not available for this plug-in.", "Video Plugin");
	}
}
#endif


/******************************************************************
  Function: FrameBufferWrite
  Purpose:  This function is called to notify the dll that the
            frame buffer has been modified by CPU at the given address.
  input:    addr		rdram address
			val			val
			size		1 = BYTE, 2 = WORD, 4 = DWORD
  output:   none
*******************************************************************/ 
void VIDEO_FrameBufferWrite(DWORD addr, DWORD size)
{
	if( _VIDEO_FrameBufferWrite != NULL  )
	{
		_VIDEO_FrameBufferWrite( addr, size);
	}
	//TRACE0("Write into frame buffer");
}

/******************************************************************
  Function: FrameBufferWriteList
  Purpose:  This function is called to notify the dll that the
            frame buffer has been modified by CPU at the given address.
  input:    FrameBufferModifyEntry *plist
			size = size of the plist, max = 1024
  output:   none
*******************************************************************/ 
void VIDEO_FrameBufferWriteList(FrameBufferModifyEntry *plist, DWORD size);

/******************************************************************
  Function: FrameBufferRead
  Purpose:  This function is called to notify the dll that the
            frame buffer memory is beening read at the given address.
			DLL should copy content from its render buffer to the frame buffer
			in N64 RDRAM
			DLL is responsible to maintain its own frame buffer memory addr list
			DLL should copy 4KB block content back to RDRAM frame buffer.
			Emulator should not call this function again if other memory
			is read within the same 4KB range
  input:    addr		rdram address
			val			val
			size		1 = BYTE, 2 = WORD, 4 = DWORD
  output:   none
*******************************************************************/
uint32 lastFBAddr;
void VIDEO_FrameBufferRead(DWORD addr)
{
	if( _VIDEO_FrameBufferRead != NULL )
	{
		if(((float)lastFBAddr - (float)addr) >= 1024*4)
		{
			_VIDEO_FrameBufferRead( addr );
			lastFBAddr = addr;
		}
	}
	//TRACE0("Read from frame buffer");
}


/************************************************************************
Function: FBGetFrameBufferInfo
Purpose:  This function is called by the emulator core to retrieve frame
buffer information from the video plugin in order to be able
to notify the video plugin about CPU frame buffer read/write
operations

size:
= 1		byte
= 2		word (16 bit) <-- this is N64 default depth buffer format
= 4		dword (32 bit)

when frame buffer information is not available yet, set all values
in the FrameBufferInfo structure to 0

input:		FrameBufferInfo *pinfo
			pinfo is pointed to a FrameBufferInfo structure array which to be
filled in by this function
			Upto to 6 FrameBufferInfo structure
output:		Values are return in the FrameBufferInfo structure
/************************************************************************/

void VIDEO_GetFrameBufferInfo(void *pinfo)
{
	if( _VIDEO_GetFrameBufferInfo != NULL )
	{
		_VIDEO_GetFrameBufferInfo(pinfo);
	}
}



BOOL VIDEO_FrameBufferSupportRead()
{
	return _VIDEO_FrameBufferRead != NULL;
}

BOOL VIDEO_FrameBufferSupportWrite()
{
	return _VIDEO_FrameBufferWrite != NULL;
}

BOOL VIDEO_FrameBufferSupport()
{
	return VIDEO_FrameBufferSupportRead() || VIDEO_FrameBufferSupportWrite();
}

#ifndef _XBOX
/******************************************************************
Function: GetFullScreenStatus
Purpose:  
Input:    
Output:   TRUE if current display is in full screen
FALSE if current display is in windowed mode

Attention: After the CPU core call the ChangeWindow function to request
the video plugin to switch between full screen and window mode,
the plugin may not carry out the request at the function call.
The video plugin may want to delay and do the screen switching later.

*******************************************************************/ 
int VIDEO_GetFullScreenStatus(void)
{
	if( _VIDEO_GetFullScreenStatus )
	{
		return _VIDEO_GetFullScreenStatus();
	}
	else
		return -1;
}


/******************************************************************
Function: SetOnScreenText
Purpose:  
Input:    char *msg
Output:   

*******************************************************************/ 
void VIDEO_SetOnScreenText(char *msg)
{
	if( _VIDEO_SetOnScreenText )
	{
		_VIDEO_SetOnScreenText(msg);
	}
}
#endif
