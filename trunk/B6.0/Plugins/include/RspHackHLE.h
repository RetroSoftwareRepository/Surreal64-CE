#pragma once

#if defined(__cplusplus)
extern "C" {
#endif

void	_RSP_HLE_CloseDLL (void);
void	_RSP_HLE_DllAbout (HWND hParent);
DWORD	_RSP_HLE_DoRspCycles (DWORD Cycles);
void	_RSP_HLE_GetDllInfo (PLUGIN_INFO * PluginInfo);
void	_RSP_HLE_InitiateRSP (RSP_INFO_1_1 Rsp_Info, DWORD * CycleCount);
void	_RSP_HLE_RomClosed (void);
void	_RSP_HLE_DllConfig (HWND hWnd);
#if defined(__cplusplus)
}
#endif