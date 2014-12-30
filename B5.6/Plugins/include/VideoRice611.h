#pragma once

#if defined(__cplusplus)
extern "C" {
#endif

//void	_VIDEO_RICE_611_CaptureScreen ( char * Directory );
void	_VIDEO_RICE_611_ChangeWindow (void);
void	_VIDEO_RICE_611_CloseDLL (void);
void	_VIDEO_RICE_611_DllAbout (HWND hParent);
void	_VIDEO_RICE_611_DllConfig (HWND hParent);
void	_VIDEO_RICE_611_DllTest (HWND hParent);
void	_VIDEO_RICE_611_DrawScreen (void);
void	_VIDEO_RICE_611_GetDllInfo (PLUGIN_INFO * PluginInfo);
BOOL	_VIDEO_RICE_611_InitiateGFX (GFX_INFO Gfx_Info);
void	_VIDEO_RICE_611_MoveScreen (int xpos, int ypos);
void	_VIDEO_RICE_611_ProcessDList (void);
DWORD	_VIDEO_RICE_611_ProcessDListCountCycles (void);
void	_VIDEO_RICE_611_ProcessRDPList (void);
void	_VIDEO_RICE_611_RomClosed (void);
void	_VIDEO_RICE_611_RomOpen (void);
//void	_VIDEO_RICE_611_ShowCFB (void);
void	_VIDEO_RICE_611_UpdateScreen (void);
void	_VIDEO_RICE_611_ViStatusChanged (void);
void	_VIDEO_RICE_611_ViWidthChanged (void);
void	_VIDEO_RICE_611_FBWrite (DWORD, DWORD);
//void	_VIDEO_RICE_611_FBWList (FrameBufferModifyEntry *plist, DWORD size);
void	_VIDEO_RICE_611_FBRead (DWORD addr);
void	_VIDEO_RICE_611_FBGetFrameBufferInfo (void *pinfo);
//void	_VIDEO_RICE_611_GetFullScreenStatus (void);
//void	_VIDEO_RICE_611_SetOnScreenText (char *msg);

void	_VIDEO_RICE_611_SetMaxTextureMem (DWORD mem);

#if defined(__cplusplus)
}
#endif