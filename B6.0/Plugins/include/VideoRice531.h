#pragma once

#if defined(__cplusplus)
extern "C" {
#endif

//void	_VIDEO_RICE_531_CaptureScreen ( char * Directory );
void	_VIDEO_RICE_531_ChangeWindow (void);
void	_VIDEO_RICE_531_CloseDLL (void);
void	_VIDEO_RICE_531_DllAbout (HWND hParent);
void	_VIDEO_RICE_531_DllConfig (HWND hParent);
void	_VIDEO_RICE_531_DllTest (HWND hParent);
void	_VIDEO_RICE_531_DrawScreen (void);
void	_VIDEO_RICE_531_GetDllInfo (PLUGIN_INFO * PluginInfo);
BOOL	_VIDEO_RICE_531_InitiateGFX (GFX_INFO Gfx_Info);
void	_VIDEO_RICE_531_MoveScreen (int xpos, int ypos);
void	_VIDEO_RICE_531_ProcessDList (void);
DWORD	_VIDEO_RICE_531_ProcessDListCountCycles (void);
void	_VIDEO_RICE_531_ProcessRDPList (void);
void	_VIDEO_RICE_531_RomClosed (void);
void	_VIDEO_RICE_531_RomOpen (void);
//void	_VIDEO_RICE_531_ShowCFB (void);
void	_VIDEO_RICE_531_UpdateScreen (void);
void	_VIDEO_RICE_531_ViStatusChanged (void);
void	_VIDEO_RICE_531_ViWidthChanged (void);
void	_VIDEO_RICE_531_FBWrite (DWORD, DWORD);
//void	_VIDEO_RICE_531_FBWList (FrameBufferModifyEntry *plist, DWORD size);
void	_VIDEO_RICE_531_FBRead (DWORD addr);
void	_VIDEO_RICE_531_FBGetFrameBufferInfo (void *pinfo);
//void	_VIDEO_RICE_531_GetFullScreenStatus (void);
//void	_VIDEO_RICE_531_SetOnScreenText (char *msg);

void	_VIDEO_RICE_531_SetMaxTextureMem (DWORD mem);

#if defined(__cplusplus)
}
#endif