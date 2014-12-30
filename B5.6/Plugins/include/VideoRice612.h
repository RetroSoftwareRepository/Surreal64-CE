#pragma once

#if defined(__cplusplus)
extern "C" {
#endif

//void	_VIDEO_RICE_612_CaptureScreen ( char * Directory );
void	_VIDEO_RICE_612_ChangeWindow (void);
void	_VIDEO_RICE_612_CloseDLL (void);
void	_VIDEO_RICE_612_DllAbout (HWND hParent);
void	_VIDEO_RICE_612_DllConfig (HWND hParent);
void	_VIDEO_RICE_612_DllTest (HWND hParent);
void	_VIDEO_RICE_612_DrawScreen (void);
void	_VIDEO_RICE_612_GetDllInfo (PLUGIN_INFO * PluginInfo);
BOOL	_VIDEO_RICE_612_InitiateGFX (GFX_INFO Gfx_Info);
void	_VIDEO_RICE_612_MoveScreen (int xpos, int ypos);
void	_VIDEO_RICE_612_ProcessDList (void);
DWORD	_VIDEO_RICE_612_ProcessDListCountCycles (void);
void	_VIDEO_RICE_612_ProcessRDPList (void);
void	_VIDEO_RICE_612_RomClosed (void);
void	_VIDEO_RICE_612_RomOpen (void);
//void	_VIDEO_RICE_612_ShowCFB (void);
void	_VIDEO_RICE_612_UpdateScreen (void);
void	_VIDEO_RICE_612_ViStatusChanged (void);
void	_VIDEO_RICE_612_ViWidthChanged (void);
void	_VIDEO_RICE_612_FBWrite (DWORD, DWORD);
//void	_VIDEO_RICE_612_FBWList (FrameBufferModifyEntry *plist, DWORD size);
void	_VIDEO_RICE_612_FBRead (DWORD addr);
void	_VIDEO_RICE_612_FBGetFrameBufferInfo (void *pinfo);
//void	_VIDEO_RICE_612_GetFullScreenStatus (void);
//void	_VIDEO_RICE_612_SetOnScreenText (char *msg);

void	_VIDEO_RICE_612_SetMaxTextureMem (DWORD mem);

#if defined(__cplusplus)
}
#endif