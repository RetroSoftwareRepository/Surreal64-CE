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

#include <xtl.h>
#include <stdio.h>
#include "rsp.h"
#include "CPU.h"
#include "breakpoint.h"

void Add_BPoint ( void ) {
 
}

int AddRSP_BPoint( DWORD Location, int Confirm ) {
 
	return TRUE;
}

int CheckForRSPBPoint ( DWORD Location ) {
	return TRUE;
}

void CreateBPPanel ( HWND hDlg, RECT rcBox ) {
	 return;
}

void HideBPPanel ( void ) {
	 
}

 

void ShowBPPanel ( void ) {
 
}

void RefreshBpoints ( HWND hList ) {
 
}

void RemoveAllBpoint ( void ) {
	 
}

void RemoveBpoint ( HWND hList, int index ) {
 
}

void RemoveRSPBreakPoint (DWORD Location) {
 
}