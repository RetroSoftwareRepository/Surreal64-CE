#pragma once

#if defined(__cplusplus)
extern "C" {
#endif

//void	_VIDEO_1964_11_CaptureScreen ( char * Directory );
void	_VIDEO_1964_11_ChangeWindow (void);
void	_VIDEO_1964_11_CloseDLL (void);
void	_VIDEO_1964_11_DllAbout (HWND hParent);
void	_VIDEO_1964_11_DllConfig (HWND hParent);
void	_VIDEO_1964_11_DllTest (HWND hParent);
void	_VIDEO_1964_11_DrawScreen (void);
void	_VIDEO_1964_11_GetDllInfo (PLUGIN_INFO * PluginInfo);
BOOL	_VIDEO_1964_11_InitiateGFX (GFX_INFO Gfx_Info);
void	_VIDEO_1964_11_MoveScreen (int xpos, int ypos);
void	_VIDEO_1964_11_ProcessDList (void);
DWORD	_VIDEO_1964_11_ProcessDListCountCycles (void);
void	_VIDEO_1964_11_ProcessRDPList (void);
void	_VIDEO_1964_11_RomClosed (void);
void	_VIDEO_1964_11_RomOpen (void);
//void	_VIDEO_1964_11_ShowCFB (void);
void	_VIDEO_1964_11_UpdateScreen (void);
void	_VIDEO_1964_11_ViStatusChanged (void);
void	_VIDEO_1964_11_ViWidthChanged (void);
void	_VIDEO_1964_11_FBWrite (DWORD, DWORD);
void	_VIDEO_1964_11_FBWList (FrameBufferModifyEntry *plist, DWORD size);
void	_VIDEO_1964_11_FBRead (DWORD addr);
void	_VIDEO_1964_11_FBGetFrameBufferInfo (void *pinfo);
void	_VIDEO_1964_11_GetFullScreenStatus (void);
void	_VIDEO_1964_11_SetOnScreenText (char *msg);

void	_VIDEO_1964_11_SetMaxTextureMem (DWORD mem);

#if defined(__cplusplus)
}
#endif