/*
 * Project 64 - A Nintendo 64 emulator.
 *
 * (c) Copyright 2001 zilmar (zilmar@emulation64.com) and 
 * Jabo (jabo@emulation64.com).
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
#include "main.h"
#include "CPU.h"

static HANDLE hSramFile = NULL;

void CloseSram (void) {
	if (hSramFile) {
		CloseHandle(hSramFile);
		hSramFile = NULL;
	}
}

BOOL LoadSram (void) {
	char File[50], Directory[10];
	 

	strcpy(Directory,"T:\\");
	//sprintf(File,"%s%s.sra",Directory,RomName);
	sprintf(File, "T:\\%08X-%08X-%02X.sra", *((DWORD *)(RomHeader + 0x10)), *((DWORD *)(RomHeader + 0x14)), *((BYTE *)(RomHeader + 0x3D)));
	
	hSramFile = CreateFile(File,GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ,NULL,OPEN_ALWAYS,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS, NULL);
	if (hSramFile == INVALID_HANDLE_VALUE) {
		switch (GetLastError()) {
		case ERROR_PATH_NOT_FOUND:
			CreateDirectory(Directory,NULL);
			hSramFile = CreateFile(File,GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ,
				NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS, NULL);
			if (hSramFile == INVALID_HANDLE_VALUE) {
 
				return FALSE;
			}
			break;
		default:
		 
			return FALSE;
		}
	}
	return TRUE;
}

void DmaFromSram(BYTE * dest, int StartOffset, int len) {
	DWORD dwRead;

	if (hSramFile == NULL) {
		if (!LoadSram()) {
			return;
		}
	}
	SetFilePointer(hSramFile,StartOffset,NULL,FILE_BEGIN);	
	ReadFile(hSramFile,dest,len,&dwRead,NULL);

}

void DmaToSram(BYTE * Source, int StartOffset, int len) {
	DWORD dwWritten;

	if (hSramFile == NULL) {
		if (!LoadSram()) {
			return;
		}
	}
	SetFilePointer(hSramFile,StartOffset,NULL,FILE_BEGIN);	
	WriteFile(hSramFile,Source,len,&dwWritten,NULL);
}
