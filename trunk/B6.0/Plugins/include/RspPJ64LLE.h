#pragma once

#if defined(__cplusplus)
extern "C" {
#endif
void	_RSP_CloseDLL (void);
void	_RSP_DllAbout (HWND hParent);
DWORD	_RSP_DoRspCycles (DWORD Cycles);
void	_RSP_GetDllInfo (PLUGIN_INFO * PluginInfo);
void	_RSP_InitiateRSP (RSP_INFO_1_1 Rsp_Info, DWORD * CycleCount);
void	_RSP_RomClosed (void);
void	_RSP_DllConfig (HWND hWnd);

//void   InitiateRSPDebugger ( DEBUG_INFO Debug_Info);
#if defined(__cplusplus)
}
#endif