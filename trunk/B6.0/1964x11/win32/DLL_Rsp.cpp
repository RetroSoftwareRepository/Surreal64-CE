/*
 * 1964 Copyright (C) 1999-2004 Joel Middendorf, <schibo@emulation64.com> This
 * program is free software; you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option) any later
 * version. This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details. You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA. To contact the
 * authors: email: schibo@emulation64.com, rice1964@yahoo.com
 */

#include "../stdafx.h"

#ifdef _XBOX
BOOL g_bUseRspAudio = FALSE; // control a listing
int g_iRspPlugin = _RSPPluginHLE; // default hle
#endif //_XBOX

/********** RSP DLL: Functions *********************/
void	(CALL *_RSPCloseDLL)			( void ) = NULL;
void	(CALL *_RSPDllAbout)			( HWND hWnd ) = NULL;
void	(CALL *_RSPDllConfig)			( HWND hWnd ) = NULL;
void	(CALL *_RSPRomClosed)			( void ) = NULL;
DWORD	(CALL *_DoRspCycles)			( DWORD ) = NULL;
void	(CALL *_InitiateRSP_1_0)		( RSP_INFO_1_0 rspinfo, DWORD * cycles) = NULL;
void	(CALL *_InitiateRSP_1_1)		( RSP_INFO_1_1 rspinfo, DWORD * cycles) = NULL;
#ifndef _XBOX
void	(CALL *_RSP_GetDllInfo)			(PLUGIN_INFO *) = NULL;
#endif
void	(CALL *_RSP_Under_Selecting_DllAbout) (HWND _hWnd) = NULL;

#ifdef _DEBUG
static void __cdecl DebuggerMsgCallBack(char *msg)
{
	TRACE0(msg);
}
static void (__cdecl *_SetDebuggerCallBack)(void (_cdecl *DbgCallBackFun)(char *msg)) = NULL;
#endif

uint16		RSPVersion = 0;
DWORD		RspTaskValue = 0;
HINSTANCE	hRSPHandle = NULL;
BOOL		rsp_plugin_is_loaded = FALSE;


//
// ----------------------------------------------------------------------------
// Functions called by RSP Plugin
// ----------------------------------------------------------------------------
//
void CALL ProcessDList(void)
{
	VIDEO_ProcessDList();
}

void CALL ShowCFB(void)
{
	VIDEO_ShowCFB();
}

void CALL ProcessRDPList(void)
{
	VIDEO_ProcessRDPList();
}

void CALL ProcessAList(void)
{
	AUDIO_ProcessAList();
}

void CALL RspCheckInterrupts(void)
{
	CheckInterrupts();
}

//
// ----------------------------------------------------------------------------
// Wrapper functions from the RSP Plugin
// ----------------------------------------------------------------------------
//
void RSPCloseDLL( void )
{
	if( _RSPCloseDLL )
		_RSPCloseDLL();
}

void RSPDllAbout( HWND hWnd )
{
	if( _RSPDllAbout )
		_RSPDllAbout(hWnd);
}

void RSPDllConfig( HWND hWnd )
{
	if( _RSPDllConfig )
		_RSPDllConfig(hWnd);
}

void RSPRomClosed( void )
{
	if( _RSPRomClosed )
		_RSPRomClosed();
}

DWORD DoRspCycles( DWORD cycles )
{
	if( _DoRspCycles )
	{
		DWORD retval;
		DWORD sp_pc_save = SP_PC_REG;
		SP_PC_REG &= 0xFFC;
		retval = _DoRspCycles(cycles);
		SP_PC_REG = sp_pc_save;
		return retval;
	}
	else
		return 0;
}

void InitiateRSP_1_0( RSP_INFO_1_0 rspinfo, DWORD * cycles)
{
	if( _InitiateRSP_1_0 )
		_InitiateRSP_1_0(rspinfo, cycles );
}
void InitiateRSP_1_1( RSP_INFO_1_1 rspinfo, DWORD * cycles)
{
	if( _InitiateRSP_1_1 )
		_InitiateRSP_1_1(rspinfo, cycles );
}

//
// ----------------------------------------------------------------------------
// Supporting functions For the RSP Plugin
// ----------------------------------------------------------------------------
//

void InitializeRSP (void) 
{
	RSP_INFO_1_0 RspInfo10;
	RSP_INFO_1_1 RspInfo11;

	RspInfo10.CheckInterrupts = RspCheckInterrupts;
	RspInfo11.CheckInterrupts = RspCheckInterrupts;
	RspInfo10.ProcessDlist = ProcessDList;
	RspInfo11.ProcessDlist = ProcessDList;
#ifdef _XBOX
	if (g_bUseRspAudio) {
		RspInfo10.ProcessAlist = NULL;
		RspInfo11.ProcessAlist = NULL;
	} else {
		RspInfo10.ProcessAlist = ProcessAList;
		RspInfo11.ProcessAlist = ProcessAList;
	}	
#else //win32
	RspInfo10.ProcessAlist = ProcessAList;
	RspInfo11.ProcessAlist = ProcessAList;
#endif //_XBOX
	RspInfo10.ProcessRdpList = ProcessRDPList;
	RspInfo11.ProcessRdpList = ProcessRDPList;
	RspInfo11.ShowCFB = ShowCFB;

	RspInfo10.hInst = gui.hInst;
	RspInfo11.hInst = gui.hInst;
	RspInfo10.RDRAM = gMS_RDRAM;
	RspInfo11.RDRAM = gMS_RDRAM;
	RspInfo10.DMEM = (uint8*)&SP_DMEM;
	RspInfo11.DMEM = (uint8*)&SP_DMEM;
	RspInfo10.IMEM = (uint8*)&SP_IMEM;
	RspInfo11.IMEM = (uint8*)&SP_IMEM;
	RspInfo10.MemoryBswaped = FALSE;
	RspInfo11.MemoryBswaped = FALSE;

	RspInfo10.MI__INTR_REG = (DWORD*)&MI_INTR_REG_R;
	RspInfo11.MI__INTR_REG = (DWORD*)&MI_INTR_REG_R;
		
	RspInfo10.SP__MEM_ADDR_REG = (DWORD*)&SP_MEM_ADDR_REG;
	RspInfo11.SP__MEM_ADDR_REG = (DWORD*)&SP_MEM_ADDR_REG;
	RspInfo10.SP__DRAM_ADDR_REG = (DWORD*)&SP_DRAM_ADDR_REG;
	RspInfo11.SP__DRAM_ADDR_REG = (DWORD*)&SP_DRAM_ADDR_REG;
	RspInfo10.SP__RD_LEN_REG = (DWORD*)&SP_RD_LEN_REG;
	RspInfo11.SP__RD_LEN_REG = (DWORD*)&SP_RD_LEN_REG;
	RspInfo10.SP__WR_LEN_REG = (DWORD*)&SP_WR_LEN_REG;
	RspInfo11.SP__WR_LEN_REG = (DWORD*)&SP_WR_LEN_REG;
	RspInfo10.SP__STATUS_REG = (DWORD*)&SP_STATUS_REG;
	RspInfo11.SP__STATUS_REG = (DWORD*)&SP_STATUS_REG;
	RspInfo10.SP__DMA_FULL_REG = (DWORD*)&SP_DMA_FULL_REG;
	RspInfo11.SP__DMA_FULL_REG = (DWORD*)&SP_DMA_FULL_REG;
	RspInfo10.SP__DMA_BUSY_REG = (DWORD*)&SP_DMA_BUSY_REG;
	RspInfo11.SP__DMA_BUSY_REG = (DWORD*)&SP_DMA_BUSY_REG;
	RspInfo10.SP__PC_REG = (DWORD*)&SP_PC_REG;
	RspInfo11.SP__PC_REG = (DWORD*)&SP_PC_REG;
	RspInfo10.SP__SEMAPHORE_REG = (DWORD*)&SP_SEMAPHORE_REG;
	RspInfo11.SP__SEMAPHORE_REG = (DWORD*)&SP_SEMAPHORE_REG;
		
	RspInfo10.DPC__START_REG = (DWORD*)&DPC_START_REG;
	RspInfo11.DPC__START_REG = (DWORD*)&DPC_START_REG;
	RspInfo10.DPC__END_REG = (DWORD*)&DPC_END_REG;
	RspInfo11.DPC__END_REG = (DWORD*)&DPC_END_REG;
	RspInfo10.DPC__CURRENT_REG = (DWORD*)&DPC_CURRENT_REG;
	RspInfo11.DPC__CURRENT_REG = (DWORD*)&DPC_CURRENT_REG;
	RspInfo10.DPC__STATUS_REG = (DWORD*)&DPC_STATUS_REG;
	RspInfo11.DPC__STATUS_REG = (DWORD*)&DPC_STATUS_REG;
	RspInfo10.DPC__CLOCK_REG = (DWORD*)&DPC_CLOCK_REG;
	RspInfo11.DPC__CLOCK_REG = (DWORD*)&DPC_CLOCK_REG;
	RspInfo10.DPC__BUFBUSY_REG = (DWORD*)&DPC_BUFBUSY_REG;
	RspInfo11.DPC__BUFBUSY_REG = (DWORD*)&DPC_BUFBUSY_REG;
	RspInfo10.DPC__PIPEBUSY_REG = (DWORD*)&DPC_PIPEBUSY_REG;
	RspInfo11.DPC__PIPEBUSY_REG = (DWORD*)&DPC_PIPEBUSY_REG;
	RspInfo10.DPC__TMEM_REG = (DWORD*)&DPC_TMEM_REG;
	RspInfo11.DPC__TMEM_REG = (DWORD*)&DPC_TMEM_REG;
	
	if (RSPVersion == 0x0100) 
	{ 
		InitiateRSP_1_0(RspInfo10, &RspTaskValue); 
	}
	else
	{ 
		InitiateRSP_1_1(RspInfo11, &RspTaskValue); 
	}

//    InitiateInternalRSP(RspInfo11, &RspTaskValue);
}

BOOL LoadRSPPlugin(char * libname) 
{
	PLUGIN_INFO RSPPluginInfo;
	
#ifdef _XBOX
#if defined(_RSP_SOLO)
#if defined(_RSP_HACK_HLE)
	g_iRspPlugin = _RSPPluginHLE;
	g_bUseRspAudio = FALSE;
#elif defined(_RSP_M64P_HLE)
	g_iRspPlugin = _RSPPluginM64P;
#elif defined(_RSP_PJ64_LLE)
	g_iRspPlugin = _RSPPluginLLE;
#else
	g_iRspPlugin = _RSPPluginNone;
	DisplayError("No Rsp Plugin Defined! (%i)", g_iRspPlugin);
#endif
#endif //_RSP_SOLO

	switch (g_iRspPlugin)
	{
#if defined(_RSP_HACK_HLE)
	case _RSPPluginHLE :
		RSPVersion							= 0x0101;
		_RSPCloseDLL						= _RSP_HLE_CloseDLL;
		_DoRspCycles						= _RSP_HLE_DoRspCycles;
		_InitiateRSP_1_1					= _RSP_HLE_InitiateRSP;
		_RSPRomClosed						= _RSP_HLE_RomClosed;
		//_RSP_GetDllInfo					= _RSP_HLE_GetDllInfo;
			break;
#endif //_RSP_HACK_HLE

#if defined(_RSP_M64P_HLE)
	case _RSPPluginM64P :
		RSPVersion							= 0x0101;
		//_RSPCloseDLL						= _RSP_M64p_CloseDLL;
		_DoRspCycles						= _RSP_M64p_DoRspCycles;
		_InitiateRSP_1_1					= _RSP_M64p_InitiateRSP;
		_RSPRomClosed						= _RSP_M64p_RomClosed;
		//_RSP_GetDllInfo					= _RSP_M64p_GetDllInfo;
			break;
#endif //_RSP_M64P_HLE

#if defined(_RSP_PJ64_LLE)
	case _RSPPluginLLE :
		RSPVersion							= 0x0101;
		_RSPCloseDLL						= _RSP_CloseDLL;
		_DoRspCycles						= _RSP_DoRspCycles;
		_InitiateRSP_1_1					= _RSP_InitiateRSP;
		_RSPRomClosed						= _RSP_RomClosed;
		//_RSP_GetDllInfo					= _RSP_GetDllInfo;
			break;
#endif //_RSP_PJ64_LLE

	case _RSPPluginNone :
		g_bUseRspAudio = FALSE;
			return TRUE;
		
	default :
		DisplayError("No Rsp Plugin Specified! (%i)", g_iRspPlugin);
			break;
	}
	
	//_RSP_GetDllInfo(&RSPPluginInfo);
	//DisplayError("RSP PLUGIN INFO: Version=%04X, Type=%04X (RSP=%04X), Name=%s, NormalMemory=%i, MemoryBswaped=%i", RSPPluginInfo.Version, RSPPluginInfo.Type, PLUGIN_TYPE_RSP, RSPPluginInfo.Name, (RSPPluginInfo.NormalMemory ? 1:0), (RSPPluginInfo.MemoryBswaped ? 1:0));

	return TRUE;

#else //win32
	hRSPHandle = LoadLibrary(libname);
	if (hRSPHandle == NULL) 
	{  
		return FALSE; 
	}

	_RSP_GetDllInfo = (void (__cdecl *)(PLUGIN_INFO *))GetProcAddress( hRSPHandle, "GetDllInfo" );
	if( _RSP_GetDllInfo == NULL) 
	{ 
		return FALSE; 
	}

	_RSP_GetDllInfo(&RSPPluginInfo);
	RSPVersion = RSPPluginInfo.Version;

	if(RSPPluginInfo.Type == PLUGIN_TYPE_RSP) /* Check if this is a video plugin */
	{
		if (RSPVersion == 1) 
		{ 
			RSPVersion = 0x0100; 
		}

		if (RSPVersion == 0x100) {
			_InitiateRSP_1_0 = (void (__cdecl *)(RSP_INFO_1_0,DWORD *))GetProcAddress( hRSPHandle, "InitiateRSP" );
		}
		else
		{
			_InitiateRSP_1_1 = (void (__cdecl *)(RSP_INFO_1_1,DWORD *))GetProcAddress( hRSPHandle, "InitiateRSP" );
		}

		_DoRspCycles = (DWORD (__cdecl *)(DWORD))GetProcAddress( hRSPHandle, "DoRspCycles" );
		_RSPRomClosed = (void (__cdecl *)(void))GetProcAddress( hRSPHandle, "RomClosed" );
		_RSPCloseDLL = (void (__cdecl *)(void))GetProcAddress( hRSPHandle, "CloseDLL" );
		_RSPDllConfig = (void (__cdecl *)(HWND))GetProcAddress( hRSPHandle, "DllConfig" );
		_RSPDllAbout = (void (__cdecl *)(HWND))GetProcAddress( hRSPHandle, "DllAbout" );

#ifdef _DEBUG
		_SetDebuggerCallBack = (void (__cdecl *)(void (_cdecl *DbgCallBackFun)(char *msg))) 
			GetProcAddress(hinstControllerPlugin, "SetDebuggerCallBack");
		if( _SetDebuggerCallBack )
		{
			_SetDebuggerCallBack(DebuggerMsgCallBack);
		}
#endif

		if( _DoRspCycles == NULL	||
			_RSPRomClosed == NULL	||
			_RSPCloseDLL == NULL	||
			_RSPDllConfig == NULL	||
			( _InitiateRSP_1_0 == NULL && _InitiateRSP_1_1 == NULL ) )
		{
			return FALSE;
		}
		else
		{
			return TRUE;
		}
	}
	else
	{
		return FALSE;
	}
#endif //_XBOX
}

void CloseRSPPlugin (void) 
{
#ifndef _XBOX //win32
	if (hRSPHandle != NULL) 
	{ 
		RSPCloseDLL();

		_DoRspCycles = NULL;
		_RSPRomClosed = NULL;
		_RSPCloseDLL =  NULL;
		_RSPDllConfig = NULL;
		_RSPDllAbout =  NULL;

		FreeLibrary(hRSPHandle);
		hRSPHandle = NULL;
	}
#endif //_XBOX //win32
}

void RSP_Under_Selecting_DllAbout(HWND _hWnd)
{
	if(_RSP_Under_Selecting_DllAbout != NULL)
	{
		__try
		{
			_RSP_Under_Selecting_DllAbout(_hWnd);
		}

		__except(NULL, EXCEPTION_EXECUTE_HANDLER)
		{
		}
	}
}
