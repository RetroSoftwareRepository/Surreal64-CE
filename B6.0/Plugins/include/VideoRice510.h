#pragma once

#if defined(__cplusplus)
extern "C" {
#endif

//void	_VIDEO_RICE_510_CaptureScreen ( char * Directory );
void	_VIDEO_RICE_510_ChangeWindow (void);
void	_VIDEO_RICE_510_CloseDLL (void);
void	_VIDEO_RICE_510_DllAbout (HWND hParent);
void	_VIDEO_RICE_510_DllConfig (HWND hParent);
void	_VIDEO_RICE_510_DllTest (HWND hParent);
void	_VIDEO_RICE_510_DrawScreen (void);
void	_VIDEO_RICE_510_GetDllInfo (PLUGIN_INFO * PluginInfo);
BOOL	_VIDEO_RICE_510_InitiateGFX (GFX_INFO Gfx_Info);
void	_VIDEO_RICE_510_MoveScreen (int xpos, int ypos);
void	_VIDEO_RICE_510_ProcessDList (void);
DWORD	_VIDEO_RICE_510_ProcessDListCountCycles (void);
void	_VIDEO_RICE_510_ProcessRDPList (void);
void	_VIDEO_RICE_510_RomClosed (void);
void	_VIDEO_RICE_510_RomOpen (void);
//void	_VIDEO_RICE_510_ShowCFB (void);
void	_VIDEO_RICE_510_UpdateScreen (void);
void	_VIDEO_RICE_510_ViStatusChanged (void);
void	_VIDEO_RICE_510_ViWidthChanged (void);
void	_VIDEO_RICE_510_FBWrite (DWORD, DWORD);
//void	_VIDEO_RICE_510_FBWList (FrameBufferModifyEntry *plist, DWORD size);
void	_VIDEO_RICE_510_FBRead (DWORD addr);
void	_VIDEO_RICE_510_FBGetFrameBufferInfo (void *pinfo);
//void	_VIDEO_RICE_510_GetFullScreenStatus (void);
//void	_VIDEO_RICE_510_SetOnScreenText (char *msg);

void	_VIDEO_RICE_510_SetMaxTextureMem (DWORD mem);

#if defined(__cplusplus)
}
#endif