#pragma once

#if defined(__cplusplus)
extern "C" {
#endifvoid	_RSP_M64p_CloseDLL (void);
void	_RSP_M64p_DllAbout (HWND hParent);
DWORD	_RSP_M64p_DoRspCycles (DWORD Cycles);
void	_RSP_M64p_GetDllInfo (PLUGIN_INFO * PluginInfo);
void	_RSP_M64p_InitiateRSP (RSP_INFO_1_1 Rsp_Info, DWORD * CycleCount);
void	_RSP_M64p_RomClosed (void);
void	_RSP_M64p_DllConfig (HWND hWnd);
#if defined(__cplusplus)
}
#endif