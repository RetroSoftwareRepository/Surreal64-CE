/*
Copyright (C) 2002 Rice1964

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

#if !defined(AFX_DEBUGGER_H__63013E25_7F89_4826_880E_1B5D48CEA506__INCLUDED_)
#define AFX_DEBUGGER_H__63013E25_7F89_4826_880E_1B5D48CEA506__INCLUDED_

#ifdef _DEBUG

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Debugger.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDebugger dialog
#include "resource.h"

extern bool debuggerWinOpened;

extern BOOL logCombiners;
extern BOOL logTriangles;
extern BOOL logVertex;
extern BOOL logUnknownUcode;
extern BOOL logTextures;
extern BOOL logTextureBuffer;
extern BOOL logMatrix;
extern BOOL logToScreen;
extern BOOL logToFile;
extern BOOL logUcodes;
extern BOOL logMicrocode;


extern BOOL debuggerEnableTexture;
extern BOOL debuggerEnableZBuffer;
extern BOOL debuggerEnableCullFace;
extern BOOL debuggerEnableTestTris;
extern BOOL debuggerEnableAlphaTest;
extern BOOL debuggerContinueWithUnknown;

extern bool debuggerPause;
extern bool pauseAtNext;
extern int  eventToPause;
extern int  debuggerPauseCount;
extern int  countToPause;

extern bool debuggerDropCombiners;
extern bool debuggerDropDecodedMux;
extern bool debuggerDropCombinerInfos;

class CTexture;
extern CTexture *surfTlut;

enum {
	NEXT_FLUSH_TRI,
	NEXT_TEXTRECT,
	NEXT_FRAME,
	NEXT_SET_CIMG,
	NEXT_OBJ_TXT_CMD,
	NEXT_OBJ_BG,
	NEXT_SPRITE_2D,
	NEXT_FILLRECT,
	NEXT_TRIANGLE,
	NEXT_DLIST,
	NEXT_UCODE,
	NEXT_TEXTURE_BUFFER,
	NEXT_MATRIX_CMD,
	NEXT_VERTEX_CMD,
	NEXT_NEW_TEXTURE,
	NEXT_SET_TEXTURE,
	NEXT_MUX,
	NEXT_SET_LIGHT,
	NEXT_SET_MODE_CMD,
	NEXT_SET_PRIM_COLOR,
	NEXT_TEXTURE_CMD,
	NEXT_UNKNOWN_OP,
	NEXT_SCALE_IMG,
	NEXT_LOADTLUT,
	NEXT_SWITCH_UCODE,
};

void OpenDialogBox();
void CloseDialogBox();
void DebuggerPause();
void __cdecl DebuggerAppendMsg(const char * Message, ...);
void DumpHex(DWORD rdramAddr, int count);

void DumpMatrix(const Matrix &mtx, const char* prompt);

//Some common used macros
#define DEBUG_DUMP_VERTEXES(str, v0, v1, v2)	\
	if( (pauseAtNext && (eventToPause == NEXT_TRIANGLE || eventToPause == NEXT_FLUSH_TRI)) && logTriangles )	\
	{															\
		DebuggerAppendMsg("%s:%d(%08X),%d(%08X),%d(%08X)\n", (str),\
		(v0), GetVertexDiffuseColor((v0)),					\
			(v1), GetVertexDiffuseColor((v1)),					\
			(v2), GetVertexDiffuseColor((v2)));				\
	}

#define DEBUGGER_IF(op)		if(op)
#define DEBUGGER_PAUSE(op)	if(pauseAtNext && eventToPause == op){pauseAtNext = false;CGraphicsContext::Get()->UpdateFrame(); debuggerPause = true;}
extern void DEBUGGER_PAUSE_COUNT_N(DWORD event);
extern void DEBUGGER_PAUSE_COUNT_N_WITHOUT_UPDATE(DWORD val);
#define DebuggerPauseCountN DEBUGGER_PAUSE_COUNT_N
#define DEBUGGER_PAUSE_AND_DUMP(op,dumpfuc)		\
	if(pauseAtNext && eventToPause == op)	\
	{	pauseAtNext = false;debuggerPause = true; CGraphicsContext::Get()->UpdateFrame(); dumpfuc;}
#define DEBUGGER_PAUSE_AND_DUMP_NO_UPDATE(op,dumpfuc)		\
	if(pauseAtNext && eventToPause == op)	\
	{	pauseAtNext = false;debuggerPause = true; dumpfuc;}

#define DEBUGGER_PAUSE_AND_DUMP_COUNT_N(op,dumpfuc)		\
	if(pauseAtNext && eventToPause == op)	\
{	if( debuggerPauseCount > 0 ) debuggerPauseCount--; if( debuggerPauseCount == 0 ){pauseAtNext = false;debuggerPause = true; CGraphicsContext::Get()->UpdateFrame(); dumpfuc;}}

#define DEBUGGER_PAUSE_AT_COND_AND_DUMP_COUNT_N(cond,dumpfuc)		\
	if(pauseAtNext && (cond) )	\
{	if( debuggerPauseCount > 0 ) debuggerPauseCount--; if( debuggerPauseCount == 0 ){pauseAtNext = false;debuggerPause = true; CGraphicsContext::Get()->UpdateFrame(); dumpfuc;}}

void RDP_NOIMPL_Real(LPCTSTR op, uint32 word0, uint32 word1) ;
#define RSP_RDP_NOIMPL RDP_NOIMPL_Real
#define DEBUGGER_IF_DUMP(cond, dumpfunc)	{if(cond) {dumpfunc}}
#define DEBUGGER_ONLY_IF	DEBUGGER_IF_DUMP
#define DEBUGGER_ONLY(func)	{func}

#define TRACE0(arg0)			{DebuggerAppendMsg(arg0);}
#define TRACE1(arg0,arg1)		{DebuggerAppendMsg(arg0,arg1);}
#define TRACE2(arg0,arg1,arg2)	{DebuggerAppendMsg(arg0,arg1,arg2);}
#define TRACE3(arg0,arg1,arg2,arg3)	{DebuggerAppendMsg(arg0,arg1,arg2,arg3);}
#define TRACE4(arg0,arg1,arg2,arg3,arg4)	{DebuggerAppendMsg(arg0,arg1,arg2,arg3,arg4);}
#define TRACE5(arg0,arg1,arg2,arg3,arg4,arg5)	{DebuggerAppendMsg(arg0,arg1,arg2,arg3,arg4,arg5);}
#else
#define DEBUG_DUMP_VERTEXES(str, v0, v1, v2)
#define DEBUGGER_IF(op)
#define DEBUGGER_PAUSE(op)
#define DEBUGGER_PAUSE_COUNT_N(op)
#define DEBUGGER_PAUSE_AND_DUMP(op,dumpfuc)
#define DebuggerPauseCountN DEBUGGER_PAUSE_COUNT_N
#define DEBUGGER_PAUSE_AT_COND_AND_DUMP_COUNT_N(cond,dumpfuc)
#define DEBUGGER_PAUSE_AND_DUMP_COUNT_N(op,dumpfuc)	
#define DEBUGGER_PAUSE_COUNT_N_WITHOUT_UPDATE(op)
#define DEBUGGER_PAUSE_AND_DUMP_NO_UPDATE(op,dumpfuc)
#define RSP_RDP_NOIMPL(a,b,c)
void __cdecl DebuggerAppendMsg(const char * Message, ...);
#define DumpHex(rdramAddr, count)	
#define DEBUGGER_IF_DUMP(cond, dumpfunc)
#define DEBUGGER_ONLY_IF	DEBUGGER_IF_DUMP
#define DEBUGGER_ONLY(func)
#define DumpMatrix(a,b)

#define TRACE0(arg0)		{}
#define TRACE1(arg0,arg1)	{}
#define TRACE2(arg0,arg1,arg2)		{}
#define TRACE3(arg0,arg1,arg2,arg3)	{}
#define TRACE4(arg0,arg1,arg2,arg3,arg4)		{}
#define TRACE5(arg0,arg1,arg2,arg3,arg4,arg5)	{}

#endif

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DEBUGGER_H__63013E25_7F89_4826_880E_1B5D48CEA506__INCLUDED_)
