/*$T wingui.h GC 1.136 02/28/02 07:53:40 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
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
#ifndef _XBOXGUI_H__1964_
#define _XBOXGUI_H__1964_
#include <stdlib.h>

/* Functions in the wingui.c */
void __cdecl		DisplayError(char *Message, ...);
void __cdecl		DisplayCriticalMessage(char *Message, ...);

BOOL				WinLoadRom(void);

BOOL				
__cdecl 
WinLoadRomStep2(char *szFileName);

void				Resume(void);
void				Kill(void);

void
__cdecl
Play(BOOL WithFullScreen);

void				Stop(void);
void				CloseROM(void);
void				SaveState(void);
void				LoadState(void);
void				PrepareBeforePlay(int IsFullScreen);
void				KillCPUThread(void);
void				SetCounterFactor(int);
void				SetCodeCheckMethod(int);
void				InitPluginData(void);
void				Set_1964_Directory(void);

void				StateSetNumber(int number);
void				Exit1964(void);

#define MAXFILENAME 256					/* maximum length of file pathname */

struct EMU1964GUI
{
	char		*szBaseWindowTitle;
	HINSTANCE	hInst;
	HWND		hwnd1964main;			/* handle to main window */
	HWND		hClientWindow;			/* Window handle of the client child window */
	char	szWindowTitle[80];
};

extern struct EMU1964GUI	gui;

struct GUISTATUS
{
	int		clientwidth;				/* Client window width */
	int		clientheight;				/* Client window height */
	RECT	window_position;			/* 1964 main window location */
	BOOL	WindowIsMaximized;
	BOOL	window_is_maximized;
	BOOL	window_is_minimized;
	BOOL	block_menu;					/* Block all menu command while 1964 is busy */
	int		IsFullScreen;
};
extern struct GUISTATUS guistatus;

extern void	__cdecl		SetStatusBarText(int, char * );

struct DIRECTORIES
{
	char	main_directory[_MAX_PATH];
	char	state_save_directory_to_use[_MAX_PATH];
	char	save_directory_to_use[_MAX_PATH];
};

extern struct DIRECTORIES	directories;

extern char					game_country_name[10];
extern int					game_country_tvsystem;

enum { LOAD_ALL_PLUGIN, LOAD_VIDEO_PLUGIN, LOAD_AUDIO_PLUGIN, LOAD_INPUT_PLUGIN, LOAD_RSP_PLUGIN };

BOOL LoadPlugins();

enum { SAVE_STATE_1964_FORMAT, SAVE_STATE_PJ64_FORMAT, SAVE_STATE_1964_085_FORMAT};

enum { LOAD_1964_CREATED_PJ64_STATE, LOAD_PJ64_CREATED_PJ64_STATE, SAVE_1964_CREATED_PJ64_STATE, DO_NOT_DO_PJ64_STATE};

extern "C" void __EMU_GetStateFilename(int index, char *filename, int mode);
extern "C" void __EMU_Get1964StateFilename(int index, char *filename, int mode);
extern "C" void __EMU_GetPJ64StateFilename(int index, char *filename, int mode);

/*#define INVALID_FILE_ATTRIBUTES ((DWORD)-1)
extern BOOL PathFileExists(const char *pszPath);
extern BOOL TerminateThread(HANDLE hThread, DWORD dwExitCode);*/

extern char g_szPathSaves[256];


#define VirtualSafeFree(ptr) {if(ptr) {VirtualFree((ptr),0,MEM_RELEASE);(ptr)=NULL;}}
#endif //_XBOXGUI_H__1964_