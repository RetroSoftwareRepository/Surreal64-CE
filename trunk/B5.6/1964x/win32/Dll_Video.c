/*$T Dll_Video.c GC 1.136 03/09/02 17:41:40 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    Video plugin interface functions
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
#include "../memory.h"
#include "registry.h"
#include "DLL_Video.h"
#include "wingui.h"
#include "../emulator.h"

GFX_INFO	Gfx_Info;

extern int loaddwMaxVideoMem();

BOOL ( *_VIDEO_InitiateGFX) (GFX_INFO) = NULL;
void ( *_VIDEO_ProcessDList) (void) = NULL;
DWORD ( *_VIDEO_ProcessDList_Count_Cycles) (void) = NULL;
void ( *_VIDEO_RomOpen) (void) = NULL;
void ( *_VIDEO_RomClosed) (void) = NULL;
void ( *_VIDEO_DllClose) () = NULL;
void ( *_VIDEO_UpdateScreen) () = NULL;
void ( *_VIDEO_GetDllInfo) (PLUGIN_INFO *) = NULL;
void ( *_VIDEO_ExtraChangeResolution) (HWND, long, HWND) = NULL;
void ( *_VIDEO_DllConfig) (HWND hParent) = NULL;
void ( *_VIDEO_Test) (HWND) = NULL;
void ( *_VIDEO_About) (HWND) = NULL;
void ( *_VIDEO_MoveScreen) (int, int) = NULL;
void ( *_VIDEO_DrawScreen) (void) = NULL;
void ( *_VIDEO_ViStatusChanged) (void) = NULL;
void ( *_VIDEO_ViWidthChanged) (void) = NULL;
void ( *_VIDEO_ChangeWindow) (int) = NULL;

/* For spec 1.3 */
void ( *_VIDEO_ChangeWindow_1_3) (void) = NULL;
void ( *_VIDEO_CaptureScreen) (char *Directory) = NULL;
void ( *_VIDEO_ProcessRDPList) (void) = NULL;
void ( *_VIDEO_ShowCFB) (void) = NULL;

/* Used when selecting plugins */
void ( *_VIDEO_Under_Selecting_Test) (HWND) = NULL;
void ( *_VIDEO_Under_Selecting_About) (HWND) = NULL;

void ( *_VIDEO_FrameBufferWrite) (DWORD addr, DWORD size) = NULL;
//void ( *_VIDEO_FrameBufferWriteList) (FrameBufferModifyEntry *plist, DWORD size) = NULL;
void ( *_VIDEO_FrameBufferRead) (DWORD addr) = NULL;
void ( *_VIDEO_GetFrameBufferInfo) (void *pinfo) = NULL;
void ( *_VIDEO_SetOnScreenText) (char *msg) = NULL;
BOOL ( *_VIDEO_GetFullScreenStatus) (void) = NULL;

#ifdef _XBOX
void ( *_VIDEO_SetMaxTextureMem) (DWORD mem) = NULL;
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
	//_VIDEO_ShowCFB						= _VIDEO_1964_11_ShowCFB;
	
	_VIDEO_FrameBufferWrite					= _VIDEO_1964_11_FBWrite;
	_VIDEO_FrameBufferWriteList				= _VIDEO_1964_11_FBWList;
	_VIDEO_FrameBufferRead					= _VIDEO_1964_11_FBRead;
	_VIDEO_GetFrameBufferInfo				= _VIDEO_1964_11_FBGetFrameBufferInfo;
	//_VIDEO_SetOnScreenText				= _VIDEO_1964_11_SetOnScreenText;
	//_VIDEO_GetFullScreenStatus			= _VIDEO_1964_11_GetFullScreenStatus;
	
	_VIDEO_SetMaxTextureMem					= _VIDEO_1964_11_SetMaxTextureMem;

#elif defined(_VIDEO_RICE_510)

#elif defined(_VIDEO_RICE_531)

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
	_VIDEO_FrameBufferWriteList				= _VIDEO_RICE_560_FBWList;
	_VIDEO_FrameBufferRead					= _VIDEO_RICE_560_FBRead;
	_VIDEO_GetFrameBufferInfo				= _VIDEO_RICE_560_FBGetFrameBufferInfo;
	//_VIDEO_SetOnScreenText				= _VIDEO_RICE_560_SetOnScreenText;
	//_VIDEO_GetFullScreenStatus			= _VIDEO_RICE_560_GetFullScreenStatus;
	
	_VIDEO_SetMaxTextureMem					= _VIDEO_RICE_560_SetMaxTextureMem;

#elif defined(_VIDEO_RICE_611)

#elif defined(_VIDEO_RICE_612)
	//GfxPluginVersion						= 0x0103;
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
	//_VIDEO_ShowCFB						= _VIDEO_RICE_612_ShowCFB;
	
	_VIDEO_FrameBufferWrite					= _VIDEO_RICE_612_FBWrite;
	//_VIDEO_FrameBufferWriteList				= _VIDEO_RICE_612_FBWList;
	_VIDEO_FrameBufferRead					= _VIDEO_RICE_612_FBRead;
	_VIDEO_GetFrameBufferInfo				= _VIDEO_RICE_612_FBGetFrameBufferInfo;
	//_VIDEO_SetOnScreenText				= _VIDEO_RICE_612_SetOnScreenText;
	//_VIDEO_GetFullScreenStatus			= _VIDEO_RICE_612_GetFullScreenStatus;
	
	_VIDEO_SetMaxTextureMem					= _VIDEO_RICE_612_SetMaxTextureMem;
#else
	DisplayError("No Video Plugin Defined!");
#endif
	_VIDEO_SetMaxTextureMem(loaddwMaxVideoMem());
	return TRUE;
#endif
}

void VIDEO_GetDllInfo(PLUGIN_INFO *Plugin_Info)
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

/*
 =======================================================================================================================
 =======================================================================================================================
 */

BOOL VIDEO_InitiateGFX(GFX_INFO Gfx_Info)
{
	__try
	{
		_VIDEO_InitiateGFX(Gfx_Info);
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
void VIDEO_ProcessDList(void)
{
	_VIDEO_ProcessDList();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void VIDEO_RomOpen(void)
{
	__try
	{
		_VIDEO_RomOpen();
	}

	__except(NULL, EXCEPTION_EXECUTE_HANDLER)
	{
		DisplayError("Video RomOpen Failed.");
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void VIDEO_RomClosed(void)
{
	__try
	{
		_VIDEO_RomClosed();
	}

	__except(NULL, EXCEPTION_EXECUTE_HANDLER)
	{
		DisplayError("Video RomClosed Failed.");
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void VIDEO_ChangeWindow(int window)
{
	__try
	{
		_VIDEO_ChangeWindow(window);
		guistatus.IsFullScreen ^= 1;
	}

	__except(NULL, EXCEPTION_EXECUTE_HANDLER)
	{
		DisplayError("VIDEO ChangeWindow failed");
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void VIDEO_DllClose(void)
{
	__try
	{
		//_VIDEO_CloseDLL();
	}

	__except(NULL, EXCEPTION_EXECUTE_HANDLER)
	{
		DisplayError("VIDEO DllClose failed");
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
/*void CloseVideoPlugin(void)
{
	VIDEO_DllClose();
	DestroyGraphicsPlugin();
}*/

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void VIDEO_DllConfig(HWND hParent)
{
	_VIDEO_DllConfig(hParent);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void VIDEO_About(HWND hParent)
{
	//_VIDEO_DllAbout(hParent);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void VIDEO_Test(HWND hParent)
{
	//_VIDEO_DllTest(hParent);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void VIDEO_MoveScreen(int x, int y)
{
	_VIDEO_MoveScreen(x, y);
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

	__try
	{
		_VIDEO_UpdateScreen();
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
	__try
	{
		_VIDEO_DrawScreen();
	}

	__except(NULL, EXCEPTION_EXECUTE_HANDLER)
	{
		DisplayError("Video DrawScreen failed.");
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void VIDEO_ViStatusChanged(void)
{
	__try
	{
		_VIDEO_ViStatusChanged();
	}

	__except(NULL, EXCEPTION_EXECUTE_HANDLER)
	{
		DisplayError("Exception in ViStatusChanged");
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void VIDEO_ViWidthChanged(void)
{
	__try
	{
		_VIDEO_ViWidthChanged();
	}

	__except(NULL, EXCEPTION_EXECUTE_HANDLER)
	{
		DisplayError("Exception in ViWidthChanged");
	}
}

/*
 =======================================================================================================================
    changes for spec 1.3
 =======================================================================================================================
 */
void VIDEO_CaptureScreen(char *Directory)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void VIDEO_ProcessRDPList(void)
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

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void VIDEO_ShowCFB(void)
{
}
