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

static BYTE Mempak[4][0x8000];
static HANDLE hMempakFile = NULL;

void CloseMempak (void) {
	if (hMempakFile) {
		CloseHandle(hMempakFile);
		hMempakFile = NULL;
	}
}

void LoadMempak (void) {
	char File[256], Directory[256];
	DWORD dwRead, count, count2;

	BYTE Initilize[] = { 
		0x81,0x01,0x02,0x03, 0x04,0x05,0x06,0x07, 0x08,0x09,0x0a,0x0b, 0x0C,0x0D,0x0E,0x0F,
		0x10,0x11,0x12,0x13, 0x14,0x15,0x16,0x17, 0x18,0x19,0x1A,0x1B, 0x1C,0x1D,0x1E,0x1F,
		0xFF,0xFF,0xFF,0xFF, 0x05,0x1A,0x5F,0x13, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,
		0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0x01,0xFF, 0x66,0x25,0x99,0xCD,
		0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,
		0xFF,0xFF,0xFF,0xFF, 0x05,0x1A,0x5F,0x13, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,
		0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0x01,0xFF, 0x66,0x25,0x99,0xCD,
		0xFF,0xFF,0xFF,0xFF, 0x05,0x1A,0x5F,0x13, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,
		0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0x01,0xFF, 0x66,0x25,0x99,0xCD,
		0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,
		0xFF,0xFF,0xFF,0xFF, 0x05,0x1A,0x5F,0x13, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,
		0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0x01,0xFF, 0x66,0x25,0x99,0xCD,
		0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,
		0x00,0x71,0x00,0x03, 0x00,0x03,0x00,0x03, 0x00,0x03,0x00,0x03, 0x00,0x03,0x00,0x03,
	};
	for (count = 0; count < 4; count ++) {
		for (count2 = 0; count2 < 0x8000; count2 += 2) {
			Mempak[count][count2] = 0x00;
			Mempak[count][count2 + 1] = 0x03;
		}
		memcpy(&Mempak[count][0],Initilize,sizeof(Initilize));
	}

	strcpy(Directory, g_szPathSaves);
	
	//sprintf(File,"%s%s.mpk",Directory,RomName);
	sprintf(File, "%s%08X\\%08X-%08X-%02X.mpk", g_szPathSaves, *((DWORD *)(RomHeader + 0x10)), *((DWORD *)(RomHeader + 0x10)), *((DWORD *)(RomHeader + 0x14)), *((BYTE *)(RomHeader + 0x3D)));
	
	hMempakFile = CreateFile(File,GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ,NULL,OPEN_ALWAYS,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS, NULL);
	if (hMempakFile == INVALID_HANDLE_VALUE) {
		switch (GetLastError()) {
		case ERROR_PATH_NOT_FOUND:
			CreateDirectory(Directory,NULL);
			hMempakFile = CreateFile(File,GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ,
				NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS, NULL);
			if (hMempakFile == INVALID_HANDLE_VALUE) {
				DisplayError("Failed to open mempak");
			}
			return;
			break;
		default:
			DisplayError("Failed to open mempak");
			return;
		}
	}
	SetFilePointer(hMempakFile,0,NULL,FILE_BEGIN);	
	ReadFile(hMempakFile,Mempak,sizeof(Mempak),&dwRead,NULL);
	WriteFile(hMempakFile,Mempak,sizeof(Mempak),&dwRead,NULL);
}

BYTE Mempacks_CalulateCrc(BYTE * DataToCrc) {
	DWORD Count;
	DWORD XorTap;

	int Length;
	BYTE CRC = 0;

	for (Count = 0; Count < 0x21; Count++) {
		for (Length = 0x80; Length >= 1; Length >>= 1) {
			XorTap = (CRC & 0x80) ? 0x85 : 0;
			CRC <<= 1;
			if (Count == 0x20) {
				CRC &= 0xFF;
			} else {
				if ((*DataToCrc & Length) != 0) {
					CRC |= 1;
				}
			}
			CRC ^= XorTap;
		}
		DataToCrc++;
	}

	return CRC;
}

void ReadFromMempak(int Control, int Address, BYTE * Buffer) {	
	if (Address == 0x8001) {
		memset(Buffer, 0, 0x20);
		Buffer[0x20] = Mempacks_CalulateCrc(Buffer);
		return;
	}
	Address &= 0xFFE0;

	if (Address <= 0x7FE0) {
		if (hMempakFile == NULL) {
			LoadMempak();
		}
		memcpy(Buffer, &Mempak[Control][Address], 0x20);
	} else {
		memset(Buffer, 0, 0x20);
		/* Rumble pack area */
	}

	Buffer[0x20] = Mempacks_CalulateCrc(Buffer);
}

void WriteToMempak(int Control, int Address, BYTE * Buffer) {
	DWORD dwWritten;
	
	if (Address == 0x8001) { Buffer[0x20] = Mempacks_CalulateCrc(Buffer); return; }

	Address &= 0xFFE0;
	if (Address <= 0x7FE0) {
		if (hMempakFile == NULL) {
			LoadMempak();
		}
		memcpy(&Mempak[Control][Address], Buffer, 0x20);

		SetFilePointer(hMempakFile,Control*0x8000,NULL,FILE_BEGIN);
		WriteFile(hMempakFile,&Mempak[Control][0],0x8000,&dwWritten,NULL);
	} else {
		/* Rumble pack area */
	}
	Buffer[0x20] = Mempacks_CalulateCrc(Buffer);
}