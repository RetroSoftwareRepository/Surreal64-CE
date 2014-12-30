#pragma once

#if defined(__cplusplus)
extern "C" {
#endif

//void	_VIDEO_RICE_560_CaptureScreen ( char * Directory );
void	_VIDEO_RICE_560_ChangeWindow (void);
void	_VIDEO_RICE_560_CloseDLL (void);
void	_VIDEO_RICE_560_DllAbout (HWND hParent);
void	_VIDEO_RICE_560_DllConfig (HWND hParent);
void	_VIDEO_RICE_560_DllTest (HWND hParent);
void	_VIDEO_RICE_560_DrawScreen (void);
void	_VIDEO_RICE_560_GetDllInfo (PLUGIN_INFO * PluginInfo);
BOOL	_VIDEO_RICE_560_InitiateGFX (GFX_INFO Gfx_Info);
void	_VIDEO_RICE_560_MoveScreen (int xpos, int ypos);
void	_VIDEO_RICE_560_ProcessDList (void);
DWORD	_VIDEO_RICE_560_ProcessDListCountCycles (void);
void	_VIDEO_RICE_560_ProcessRDPList (void);
void	_VIDEO_RICE_560_RomClosed (void);
void	_VIDEO_RICE_560_RomOpen (void);
//void	_VIDEO_RICE_560_ShowCFB (void);
void	_VIDEO_RICE_560_UpdateScreen (void);
void	_VIDEO_RICE_560_ViStatusChanged (void);
void	_VIDEO_RICE_560_ViWidthChanged (void);
void	_VIDEO_RICE_560_FBWrite (DWORD, DWORD);
//void	_VIDEO_RICE_560_FBWList (FrameBufferModifyEntry *plist, DWORD size);
void	_VIDEO_RICE_560_FBRead (DWORD addr);
void	_VIDEO_RICE_560_FBGetFrameBufferInfo (void *pinfo);
//void	_VIDEO_RICE_560_GetFullScreenStatus (void);
//void	_VIDEO_RICE_560_SetOnScreenText (char *msg);

void	_VIDEO_RICE_560_SetMaxTextureMem (DWORD mem);

#if defined(__cplusplus)
}
#endif