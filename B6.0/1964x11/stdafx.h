// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#endif

// Windows Header Files:
#ifdef _XBOX
//#include <xtl.h>
#include <xbox_depp.h>

#undef USE_ROM_PAGING  // disable ROM Paging to cache whole ROM to memory, as is. Expensive.
//#define USE_ROM_PAGING	// enable ROM Paging using temporary file, breaks 007 and conker for 1.1

#if _MSC_VER > 1200
#include "xmmintrin.h" //SSE - needed?
#endif

#else
#include <windows.h>
//#define USE_ROM_PAGING 1 // enable rom paging using temporary file
//#define MEMORY_LIMIT 64 //128
//#define CACHE_ENTIRE_ROM 0 //1
#endif

// TODO: reference additional headers your program requires here
#include <process.h>
#ifndef _XBOX
#include <commdlg.h>
#include <direct.h>
#include <shlobj.h>
#include <shellapi.h>
#include <shlwapi.h>
#endif
#include "hardware.h"
#include "globals.h"
#include "debug_option.h"
#include "hle.h"
#include "emulator.h"
#include "r4300i.h"
#include "r4300i_32bit.h"
#include "n64rcp.h"
#include "interrupt.h"
#include "dma.h"
#include "compiler.h"
#include "timer.h"
#include "_memory.h"
#include "1964ini.h"
#include "iPif.h"
#include "dynarec/x86.h"
#include "dynarec/regcache.h"
#include "dynarec/dynaCPU.h"
#ifndef _XBOX
#include "dynarec/dynaLog.h"
#endif
#include "dynarec/dynarec.h"
#include "dynarec/xmm.h"
#include "win32/DLL_Video.h"
#include "win32/DLL_Input.h"
#include "win32/DLL_Rsp.h"
#include "win32/registry.h"

#ifdef _XBOX
#include "xbox/xboxgui.h"

#else //win32
#include "win32/windebug.h"
#include "win32/wingui.h"
#include "romlist.h"
#endif

#include "cheatcode.h"

#if defined(USE_ROM_PAGING) || defined(_XBOX) //_XBOX
#include "xbox/rompaging.h"
#endif

#include "gamesave.h"
#include "fileio.h"
#include "FrameBuffer.h"
#ifndef _XBOX
#include "kaillera/kaillera.h"
#endif

#ifdef _XBOX
#include "xbox/zlib/unzip.h"
#else //win32
#include "zlib/unzip.h"
#endif

#include "Registers.h"

#ifdef _XBOX
extern BOOL MessageBox(HWND hWnd, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType);
extern char *Get_Interrupt_Name(void);
#endif


#ifndef _XBOX //win32
#ifdef _DEBUG
//missing from source? see wingui.cpp
extern int DebuggerOpcodeTraceEnabled;
extern BOOL DebuggerActive;
extern void HandleBreakpoint(unsigned __int32 Instruction);
extern void OpenDebugger(void);
extern BOOL DebuggerBreakPointActive;

//linker helpers
extern void RefreshDebugger(void);
extern void WinDynDebugPrintInstruction(unsigned __int32 Instruction);
#endif
#endif

//linker helper
extern void RefreshOpList(char *opcode);

extern N64::CRegisters r;


