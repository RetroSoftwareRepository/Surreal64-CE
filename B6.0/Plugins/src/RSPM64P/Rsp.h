/*
 * RSP Compiler plug in for Project 64 (A Nintendo 64 emulator).
 *
 * (c) Copyright 2001 jabo (jabo@emulation64.com) and
 * zilmar (zilmar@emulation64.com)
 *
 * pj64 homepage: www.pj64.net
 * 
 * Permission to use, copy, modify and distribute Project64 in both binary and
 * source form, for non-commercial purposes, is hereby granted without fee,
 * providing that this license information and copyright notice appear with
 * all copies and any derived work.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event shall the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Project64 is freeware for PERSONAL USE only. Commercial users should
 * seek permission of the copyright holders first. Commercial use includes
 * charging money for Project64 or software derived from Project64.
 *
 * The copyright holders request that bug fixes and improvements to the code
 * should be forwarded to them so if they want them.
 *
 */
#pragma once
#include "Common.h"//#include <xtl.h>
#if defined(__cplusplus)
extern "C" {
#endif
typedef struct {
	int hInst;
	int MemoryBswaped;    /* If this is set to TRUE, then the memory has been pre
	                          bswap on a dword (32 bits) boundry */
	unsigned char * RDRAM;
	unsigned char * DMEM;
	unsigned char * IMEM;
	unsigned long * MI_INTR_REG;

	unsigned long * SP_MEM_ADDR_REG;
	unsigned long * SP_DRAM_ADDR_REG;
	unsigned long * SP_RD_LEN_REG;
	unsigned long * SP_WR_LEN_REG;
	unsigned long * SP_STATUS_REG;
	unsigned long * SP_DMA_FULL_REG;
	unsigned long * SP_DMA_BUSY_REG;
	unsigned long * SP_PC_REG;
	unsigned long * SP_SEMAPHORE_REG;

	unsigned long * DPC_START_REG;
	unsigned long * DPC_END_REG;
	unsigned long * DPC_CURRENT_REG;
	unsigned long * DPC_STATUS_REG;
	unsigned long * DPC_CLOCK_REG;
	unsigned long * DPC_BUFBUSY_REG;
	unsigned long * DPC_PIPEBUSY_REG;
	unsigned long * DPC_TMEM_REG;
	void (*CheckInterrupts)( void );
	void (*ProcessDList)( void );
	void (*ProcessAList)( void );
	void (*ProcessRdpList)( void );
	void (*ShowCFB)( void );
} RSP_INFO;

void  _RSP_M64p_CloseDLL (void);
void  _RSP_M64p_DllAbout ( int hParent );
unsigned long _RSP_M64p_DoRspCycles ( unsigned long Cycles );
void  _RSP_M64p_GetDllInfo ( PLUGIN_INFO * PluginInfo );
void  _RSP_M64p_InitiateRSP ( RSP_INFO Rsp_Info, unsigned long * CycleCount);
void  _RSP_M64p_RomClosed (void);
void  _RSP_M64p_DllConfig (int hWnd);
#if defined(__cplusplus)
}
#endif
