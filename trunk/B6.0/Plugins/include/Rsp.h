/*
 * RSP Plugin Spec for Surreal64
 *
 * Original Plugin Spec written and maintained by:
 * zilmar (zilmar@emulation64.com)
 *
 *  
 * Permission to use, copy, modify and distribute Project64 in both binary and
 * source form, for non-commercial purposes, is hereby granted without fee,
 * providing that this license information and copyright notice appear with
 * all copies and any derived work.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event shall the authors be held liable for any damages
 * arising from the use of this software.
 */
#pragma once
#include "Common.h"
#if defined(__cplusplus)
extern "C" {
#endif
typedef struct {
	HINSTANCE hInst;
	BOOL MemoryBswaped;    /* If this is set to TRUE, then the memory has been pre
	                          bswap on a dword (32 bits) boundry */
	BYTE * RDRAM;
	BYTE * DMEM;
	BYTE * IMEM;
	DWORD * MI_INTR_RG;
	DWORD * SP_MEM_ADDR_RG;
	DWORD * SP_DRAM_ADDR_RG;
	DWORD * SP_RD_LEN_RG;
	DWORD * SP_WR_LEN_RG;
	DWORD * SP_STATUS_RG;
	DWORD * SP_DMA_FULL_RG;
	DWORD * SP_DMA_BUSY_RG;
	DWORD * SP_PC_RG;
	DWORD * SP_SEMAPHORE_RG;
	DWORD * DPC_START_RG;
	DWORD * DPC_END_RG;
	DWORD * DPC_CURRENT_RG;
	DWORD * DPC_STATUS_RG;
	DWORD * DPC_CLOCK_RG;
	DWORD * DPC_BUFBUSY_RG;
	DWORD * DPC_PIPEBUSY_RG;
	DWORD * DPC_TMEM_RG;
	void (*CheckInterrupts)( void );
	void (*ProcessDList)( void );
	void (*ProcessAList)( void );
	void (*ProcessRdpList)( void );
	void (*ShowCFB)( void );
} RSP_INFO;


void  _RSP_HLE_CloseDLL (void);
void  _RSP_HLE_DllAbout ( HWND hParent );
DWORD _RSP_HLE_DoRspCycles ( DWORD Cycles );
void  _RSP_HLE_GetDllInfo ( PLUGIN_INFO * PluginInfo );
void  _RSP_HLE_InitiateRSP ( RSP_INFO Rsp_Info, DWORD * CycleCount);
void  _RSP_HLE_RomClosed (void);
void  _RSP_HLE_DllConfig (HWND hWnd);void  _RSP_M64p_CloseDLL (void);
void  _RSP_M64p_DllAbout ( HWND hParent );
DWORD _RSP_M64p_DoRspCycles ( DWORD Cycles );
void  _RSP_M64p_GetDllInfo ( PLUGIN_INFO * PluginInfo );
void  _RSP_M64p_InitiateRSP ( RSP_INFO Rsp_Info, DWORD * CycleCount);
void  _RSP_M64p_RomClosed (void);
void  _RSP_M64p_DllConfig (HWND hWnd);
/*
void  _RSP_CXD4_CloseDLL (void);
void  _RSP_CXD4_DllAbout ( HWND hParent );
DWORD _RSP_CXD4_DoRspCycles ( DWORD Cycles );
void  _RSP_CXD4_GetDllInfo ( PLUGIN_INFO * PluginInfo );
void  _RSP_CXD4_InitiateRSP ( RSP_INFO Rsp_Info, DWORD * CycleCount);
void  _RSP_CXD4_RomClosed (void);
void  _RSP_CXD4_DllConfig (HWND hWnd);
*/
void   _RSP_CloseDLL (void);
void   _RSP_DllAbout ( HWND hParent );
DWORD  _RSP_DoRspCycles ( DWORD Cycles );
void   _RSP_GetDllInfo ( PLUGIN_INFO * PluginInfo );
void   _RSP_InitiateRSP ( RSP_INFO Rsp_Info, DWORD * CycleCount);
//void   InitiateRSPDebugger ( DEBUG_INFO Debug_Info);
void   _RSP_RomClosed (void);
void   _RSP_DllConfig (HWND hWnd);
#if defined(__cplusplus)
}
#endif
