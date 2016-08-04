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
//#include <windowsx.h>
//#include <commctrl.h>
#include <stdio.h>
//#include <winuser.h>
#include <string.h>
#include "main.h"
#include "cheats.h"
#include "cpu.h"
#include "resource.h"
#ifdef _XBOX
#include "../Common/ini_manage.h" // ini parser
//extern char romCRC[32];
extern char szPathSaves[256];
#endif


//#define UM_CLOSE_CHEATS         (WM_USER + 132)
//#define UM_CHANGECODEEXTENSION  (WM_USER + 101)
//#define IDC_MYTREE				0x500

#define MaxCheats				500

//#define SelectCheat				1
//#define EditCheat				2
//#define NewCheat 				3 

//HWND hManageWindow = NULL;
//HWND hSelectCheat, hAddCheat, hCheatTree;
CHEAT_CODES Codes[MaxCheats];


/*******************************************************************************************
  Variables for Add Cheat
********************************************************************************************/
/*
char codestring[2048];
char optionsstring[8192];

BOOL validname;
BOOL validcodes;
BOOL validoptions;
BOOL nooptions;
int codeformat;

int numcodes;
int numoptions;

int ReadCodeString (HWND hDlg);
void ReadOptionsString(HWND hDlg);
*/
/********************************************************************************************/


BOOL CheatUsesCodeExtensions (char * CheatString);
void DeleteCheat           ( int CheatNo );
BOOL GetCheatName          ( int CheatNo, char * CheatName, int CheatNameLen );
BOOL LoadCheatExt          ( char * CheatName, char * CheatExt, int MaxCheatExtLen);
//void RefreshCheatManager   ( void );
void RenameCheat           ( int CheatNo );
void SaveCheat             ( int CheatNo, BOOL Active );
void SaveCheatExt          ( char * CheatName, char * CheatExt );
//int  _TreeView_GetCheckState(HWND hwndTreeView, HTREEITEM hItem);
//BOOL _TreeView_SetCheckState(HWND hwndTreeView, HTREEITEM hItem, int State);

//LRESULT CALLBACK ManageCheatsProc (HWND, UINT, WPARAM, LPARAM );
/*
enum Dialog_State {
	CONTRACTED,
	EXPANDED
} DialogState;

enum Cheat_Type {
	SIMPLE,
	OPTIONS,
	RANGE
} CheatType;*/



void AddCheatExtension(int CheatNo, char * CheatName, int CheatNameLen) {
	char *String = NULL, Identifier[100], CheatNumber[20];
	LPSTR IniFileName  = GetCheatIniFileName();

	sprintf(Identifier,"%08X-%08X-C:%X",*(DWORD *)(&RomHeader[0x10]),*(DWORD *)(&RomHeader[0x14]),RomHeader[0x3D]);
	sprintf(CheatNumber,"Cheat%d",CheatNo);

	_GetPrivateProfileString2(Identifier,CheatNumber,"",&String,IniFileName); //load cheat

	//Add cheat extension to the end
	if (CheatUsesCodeExtensions(String)) {
		char CheatExt[200];
		if (!LoadCheatExt(CheatName,CheatExt,sizeof(CheatExt))) { strcpy(CheatExt,"?"); }
		sprintf(CheatName,"%s (=> %s)",CheatName,CheatExt);
	}

	if (String) { free(String); }
}


void ApplyGSButton (void) {
	int count, count2;
	DWORD Address;

	for (count = 0; count < NoOfCodes; count++) {
		for (count2 = 0; count2 < MaxGSEntries; count2++) {
			switch (Codes[count].Code[count2].Command & 0xFF000000) {
			case 0x88000000:
				Address = 0x80000000 | (Codes[count].Code[count2].Command & 0xFFFFFF);
				r4300i_SB_VAddr(Address,(BYTE)Codes[count].Code[count2].Value);
				break;
			case 0x89000000:
				Address = 0x80000000 | (Codes[count].Code[count2].Command & 0xFFFFFF);
				r4300i_SH_VAddr(Address,Codes[count].Code[count2].Value);
				break;
			default:
				break;
			}
		}
	}
}

/********************************************************************************************
  ApplyCheats

  Purpose: Patch codes into memory
  Parameters: None
  Returns: None

********************************************************************************************/
int ApplyCheatEntry (GAMESHARK_CODE * Code, BOOL Execute ) {
	DWORD Address;
	WORD  Memory;

	switch (Code->Command & 0xFF000000) {
	case 0x50000000:													// Added by Witten (witten@pj64cheats.net)
		{
			int numrepeats = (Code->Command & 0x0000FF00) >> 8;
			int offset = Code->Command & 0x000000FF;
			int incr = Code->Value;
			int count;

			switch (Code[1].Command & 0xFF000000) {
			case 0x80000000:
				Address = 0x80000000 | (Code[1].Command & 0xFFFFFF);
				Memory = Code[1].Value;
				for (count=0; count<numrepeats; count++) {
					r4300i_SB_VAddr(Address, (BYTE)Memory);
					Address += offset;
					Memory += incr;
				}
				return 2;
			case 0x81000000:
				Address = 0x80000000 | (Code[1].Command & 0xFFFFFF);
				Memory = Code[1].Value;
				for (count=0; count<numrepeats; count++) {
					r4300i_SH_VAddr(Address, (WORD)Memory);
					Address += offset;
					Memory += incr;
				}
				return 2;
			default: return 1;
			}
		}
		break;
	case 0x80000000:
		Address = 0x80000000 | (Code->Command & 0xFFFFFF);
		if (Execute) { r4300i_SB_VAddr(Address,(BYTE)Code->Value); }
		break;
	case 0x81000000:
		Address = 0x80000000 | (Code->Command & 0xFFFFFF);
		if (Execute) { r4300i_SH_VAddr(Address,Code->Value); }
		break;
	case 0xA0000000:
		Address = 0xA0000000 | (Code->Command & 0xFFFFFF);
		if (Execute) { r4300i_SB_VAddr(Address,(BYTE)Code->Value);  }
		break;
	case 0xA1000000:
		Address = 0xA0000000 | (Code->Command & 0xFFFFFF);
		if (Execute) { r4300i_SH_VAddr(Address,Code->Value); }
		break;
	case 0xD0000000:													// Added by Witten (witten@pj64cheats.net)
		Address = 0x80000000 | (Code->Command & 0xFFFFFF);
		r4300i_LB_VAddr(Address, (BYTE*) &Memory);
		Memory &= 0x00FF;
		if (Memory != Code->Value) { Execute = FALSE; }
		return ApplyCheatEntry(&Code[1],Execute) + 1;
	case 0xD1000000:													// Added by Witten (witten@pj64cheats.net)
		Address = 0x80000000 | (Code->Command & 0xFFFFFF);
		r4300i_LH_VAddr(Address, (WORD*) &Memory);
		if (Memory != Code->Value) { Execute = FALSE; }
		return ApplyCheatEntry(&Code[1],Execute) + 1;
	case 0xD2000000:													// Added by Witten (witten@pj64cheats.net)
		Address = 0x80000000 | (Code->Command & 0xFFFFFF);
		r4300i_LB_VAddr(Address, (BYTE*) &Memory);
		Memory &= 0x00FF;
		if (Memory == Code->Value) { Execute = FALSE; }
		return ApplyCheatEntry(&Code[1],Execute) + 1;
	case 0xD3000000:													// Added by Witten (witten@pj64cheats.net)
		Address = 0x80000000 | (Code->Command & 0xFFFFFF);
		r4300i_LH_VAddr(Address, (WORD*) &Memory);
		if (Memory == Code->Value) { Execute = FALSE; }
		return ApplyCheatEntry(&Code[1],Execute) + 1;
	case 0: return MaxGSEntries; break;
	}
	return 1;
}

extern int gCheatActive[500];
void ApplyCheats (void) {
	int CurrentCheat, CurrentEntry;

	for (CurrentCheat = 0; CurrentCheat < NoOfCodes; CurrentCheat ++) {
		for (CurrentEntry = 0; CurrentEntry < MaxGSEntries;) {
			CurrentEntry += ApplyCheatEntry(&Codes[CurrentCheat].Code[CurrentEntry],TRUE);
		}
	}
}
/*
void ApplyCheats (void) {
	int count, count2, count3;
	DWORD Address;
	WORD Value;																	// Added by Witten (witten@pj64cheats.net)
	int numrepeats, offset, incr;												// Added by Witten (witten@pj64cheats.net)
	
	for (count = 0; count < NoOfCodes; count ++) {
		for (count2 = 0; count2 < MaxGSEntries; count2 ++) {
			switch (Codes[count].Code[count2].Command & 0xFF000000) {
			case 0x50000000:													// Added by Witten (witten@pj64cheats.net)
				numrepeats = (Codes[count].Code[count2].Command & 0x0000FF00) >> 8;
				offset = Codes[count].Code[count2].Command & 0x000000FF;
				incr = Codes[count].Code[count2].Value;
				count2++;
				switch (Codes[count].Code[count2].Command & 0xFF000000) {
				case 0x80000000:
					Address = 0x80000000 | (Codes[count].Code[count2].Command & 0xFFFFFF);
					Value = Codes[count].Code[count2].Value;
					for (count3=0; count3<numrepeats; count3++) {
						r4300i_SB_VAddr(Address, (BYTE)Value);
						Address += offset;
						Value += incr;
					}
					break;
				case 0x81000000:
					Address = 0x80000000 | (Codes[count].Code[count2].Command & 0xFFFFFF);
					Value = Codes[count].Code[count2].Value;
					for (count3=0; count3<numrepeats; count3++) {
						r4300i_SH_VAddr(Address, (WORD)Value);
						Address += offset;
						Value += incr;
					}
					break;
				default:
					break;
				}
				break;
			case 0x80000000:
				Address = 0x80000000 | (Codes[count].Code[count2].Command & 0xFFFFFF);
				r4300i_SB_VAddr(Address,(BYTE)Codes[count].Code[count2].Value);
				break;
			case 0x81000000:
				Address = 0x80000000 | (Codes[count].Code[count2].Command & 0xFFFFFF);
				r4300i_SH_VAddr(Address,Codes[count].Code[count2].Value);
				break;
			case 0xA0000000:
				Address = 0xA0000000 | (Codes[count].Code[count2].Command & 0xFFFFFF);
				r4300i_SB_VAddr(Address,(BYTE)Codes[count].Code[count2].Value);
				break;
			case 0xA1000000:
				Address = 0xA0000000 | (Codes[count].Code[count2].Command & 0xFFFFFF);
				r4300i_SH_VAddr(Address,Codes[count].Code[count2].Value);
				break;
			case 0xD0000000:													// Added by Witten (witten@pj64cheats.net)
				Address = 0x80000000 | (Codes[count].Code[count2].Command & 0xFFFFFF);
				r4300i_LB_VAddr(Address, (BYTE*) &Value);
				Value &= 0x00FF;
				if (Value == Codes[count].Code[count2].Value) {
					count2++;
					switch (Codes[count].Code[count2].Command & 0xFF000000) {
					case 0x80000000:
						Address = 0x80000000 | (Codes[count].Code[count2].Command & 0xFFFFFF);
						r4300i_SB_VAddr(Address,(BYTE)Codes[count].Code[count2].Value);
						break;
					case 0x81000000:
						Address = 0x80000000 | (Codes[count].Code[count2].Command & 0xFFFFFF);
						r4300i_SH_VAddr(Address,Codes[count].Code[count2].Value);
						break;
					default:
						break;
					}
				}
				else {
					count2++;
					break;
				}
				break;
			case 0xD1000000:													// Added by Witten (witten@pj64cheats.net)
				Address = 0x80000000 | (Codes[count].Code[count2].Command & 0xFFFFFF);
				r4300i_LH_VAddr(Address, (WORD*) &Value);
				if (Value == Codes[count].Code[count2].Value) {
					count2++;
					switch (Codes[count].Code[count2].Command & 0xFF000000) {
					case 0x80000000:
						Address = 0x80000000 | (Codes[count].Code[count2].Command & 0xFFFFFF);
						r4300i_SB_VAddr(Address,(BYTE)Codes[count].Code[count2].Value);
						break;
					case 0x81000000:
						Address = 0x80000000 | (Codes[count].Code[count2].Command & 0xFFFFFF);
						r4300i_SH_VAddr(Address,Codes[count].Code[count2].Value);
						break;
					default:
						break;
					}
				}
				else {
					count2++;
					break;
				}
				break;
			case 0xD2000000:													// Added by Witten (witten@pj64cheats.net)
				Address = 0x80000000 | (Codes[count].Code[count2].Command & 0xFFFFFF);
				r4300i_LB_VAddr(Address, (BYTE*) &Value);
				Value &= 0x00FF;
				if (Value != Codes[count].Code[count2].Value) {
					count2++;
					switch (Codes[count].Code[count2].Command & 0xFF000000) {
					case 0x80000000:
						Address = 0x80000000 | (Codes[count].Code[count2].Command & 0xFFFFFF);
						r4300i_SB_VAddr(Address,(BYTE)Codes[count].Code[count2].Value);
						break;
					case 0x81000000:
						Address = 0x80000000 | (Codes[count].Code[count2].Command & 0xFFFFFF);
						r4300i_SH_VAddr(Address,Codes[count].Code[count2].Value);
						break;
					default:
						break;
					}
				}
				else {
					count2++;
				}
				break;
			case 0xD3000000:													// Added by Witten (witten@pj64cheats.net)
				Address = 0x80000000 | (Codes[count].Code[count2].Command & 0xFFFFFF);
				r4300i_LH_VAddr(Address, (WORD*) &Value);
				if (Value != Codes[count].Code[count2].Value) {
					count2++;
					switch (Codes[count].Code[count2].Command & 0xFF000000) {
					case 0x80000000:
						Address = 0x80000000 | (Codes[count].Code[count2].Command & 0xFFFFFF);
						r4300i_SB_VAddr(Address,(BYTE)Codes[count].Code[count2].Value);
						break;
					case 0x81000000:
						Address = 0x80000000 | (Codes[count].Code[count2].Command & 0xFFFFFF);
						r4300i_SH_VAddr(Address,Codes[count].Code[count2].Value);
						break;
					default:
						break;
					}
				}
				else {
					count2++;
				}
				break;
			case 0: count2 = MaxGSEntries; break;
			}
		}
	} 
}
*/

/********************************************************************************************
  CheatActive

  Purpose: Checks in registry if cheat is active
  Parameters: char*
    Name: name of cheat
  Returns: Boolean
    True: cheat is active
	False: cheat isn't active or cheat isn't found in registry

********************************************************************************************/
BOOL CheatActive (char * Name, int CheatNo) {
#ifdef CHEAT_INI
	char File[350];
	sprintf(File, "%s%08X\\%08X-%08X-%02X.cht", g_szPathSaves, *((DWORD *)(RomHeader + 0x10)), *((DWORD *)(RomHeader + 0x10)), *((DWORD *)(RomHeader + 0x14)), *((BYTE *)(RomHeader + 0x3D)));
	
	if(PathFileExists(File)) {
		FILE* f;
		f = fopen(File , "r+");
		if(f){
			//OutputDebugString("Cheat File Found!\n");
			char line[100];
			int i,j;
			for(i = 0; i < 500; i++){
				fgets(line,100, f);
				if(i == CheatNo){
					for(j = 0; j < 100; j++) {
						if(line[j] == '=')
							return atoi(&line[j+1]);
					}
				}
			}
			fclose(f);
		}
	}
#else
	char FilePath[350];
		sprintf(FilePath, "%s%08X\\Cheats\\%08X-%08X-%02X\\%d", g_szPathSaves, *((DWORD *)(RomHeader + 0x10)), *((DWORD *)(RomHeader + 0x10)), *((DWORD *)(RomHeader + 0x14)), *((BYTE *)(RomHeader + 0x3D)), CheatNo);	
		if(PathFileExists(FilePath))
			return TRUE;
		else
			return FALSE;
#endif
}


/********************************************************************************************
  CheatUsesCodeExtensions

  Purpose: 
  Parameters:
  Returns:

********************************************************************************************/
BOOL CheatUsesCodeExtensions (char * CheatString) {
	BOOL CodeExtension;
	DWORD count;
	DWORD len;

	char * ReadPos;

	if (strlen(CheatString) == 0){ return FALSE; }
	if (strchr(CheatString,'"') == NULL) { return FALSE; }
	len = strrchr(CheatString,'"') - strchr(CheatString,'"') - 1;

	ReadPos = strrchr(CheatString,'"') + 2;
	CodeExtension = FALSE;
	for (count = 0; count < MaxGSEntries && CodeExtension == FALSE; count ++) {
		if (strchr(ReadPos,' ') == NULL) { break; }
		ReadPos = strchr(ReadPos,' ') + 1;
		if (ReadPos[0] == '?' && ReadPos[1]== '?') { CodeExtension = TRUE; }
		if (ReadPos[2] == '?' && ReadPos[3]== '?') { CodeExtension = TRUE; }
		if (strchr(ReadPos,',') == NULL) { continue; }
		ReadPos = strchr(ReadPos,',') + 1;
	}
	return CodeExtension;
}

void DeleteCheat(int CheatNo) {
	char Identifier[100], Key[100], * Ext[] = {"", "_N", "_O", "_R" };
	LPSTR IniFileName;
	int type;

	IniFileName = GetCheatIniFileName();
	sprintf(Identifier,"%08X-%08X-C:%X",*(DWORD *)(&RomHeader[0x10]),*(DWORD *)(&RomHeader[0x14]),RomHeader[0x3D]);

	for (type = 0; type < (sizeof(Ext) / sizeof(char *)); type ++) {
		sprintf(Key,"Cheat%d%s",CheatNo,Ext[type]);
		_DeletePrivateProfileString(Identifier,Key,IniFileName); // Remove this cheat from ini
	}
	RenameCheat(CheatNo);

}

void RenameCheat(int CheatNo) {
	/*
	char *Input = NULL, *Data = NULL, * Pos = NULL, Identifier[100], CurrentSection[300];
	int DataLen = 0, DataLeft, result, count;
	static long Fpos = 0;
	LPSTR IniFileName;
	long WritePos;
	FILE * fInput;

#ifdef WIN32
char * LineFeed = "\r\n";
#else
char * LineFeed = "\n";
#endif

	IniFileName = GetCheatIniFileName();
	sprintf(Identifier,"%08X-%08X-C:%X",*(DWORD *)(&RomHeader[0x10]),*(DWORD *)(&RomHeader[0x14]),RomHeader[0x3D]);

	fInput = fopen(IniFileName,"r+b");
	if (fInput == NULL) { 
		fInput = fopen(IniFileName,"w+b");
		if (IniFileName == NULL) { return; }
	}
	CurrentSection[0] = 0;

	do {
		if (strcmp(Identifier,CurrentSection) != 0) { 
			Fpos = ftell(fInput) - DataLeft;
		}
		result = fGetString2(fInput,&Input,&Data,&DataLen,&DataLeft);
		if (result <= 1) { continue; }
		
		Pos = Input;
		while (Pos != NULL) {
			Pos = strchr(Pos,'/');
			if (Pos != NULL) {
				if (Pos[1] == '/') { Pos[0] = 0; } else { Pos += 1; }
			}
		}
		
		for (count = strlen(&Input[0]) - 1; count >= 0; count --) {
			if (Input[count] != ' ' && Input[count] != '\r') { break; }
			Input[count] = 0;
		}
		//stip leading spaces
		if (strlen(Input) <= 1) { continue; }
		if (Input[0] == '[') {
			if (Input[strlen(Input) - 1] != ']') { continue; }
			if (strcmp(Identifier,CurrentSection) == 0) { 
				result = -1;
				continue;
			}
			strcpy(CurrentSection,&Input[1]);
			CurrentSection[strlen(CurrentSection) - 1] = 0;
			WritePos = ftell(fInput) - DataLeft;
			continue;
		}
		if (strcmp(Identifier,CurrentSection) != 0) { 
			continue;
		}
		Pos = strchr(Input,'=');
		if (Pos == NULL) { continue; }
		if (strncmp(Input,"Cheat",5) != 0) { 
			WritePos = ftell(fInput) - DataLeft;
			continue;
		}
		if (atoi(&Input[5]) < CheatNo) {
			WritePos = ftell(fInput) - DataLeft;
			continue; 
		}
		if (strchr(Input,'_') > 0 && strchr(Input,'_') < Pos) { Pos = strchr(Input,'_'); }
		{
			long OldLen = strlen(Input) + strlen(LineFeed);
			int Newlen = strlen(Pos) + strlen(LineFeed);
			long CurrentPos = ftell(fInput);
			char Header[100];

			sprintf(Header,"Cheat%d",atoi(&Input[5]) - 1);
			Newlen += strlen(Header);

			if (OldLen != Newlen) {
				fInsertSpaces(fInput,WritePos,Newlen - OldLen);
				CurrentPos += Newlen - OldLen;
			}
			fseek(fInput,WritePos,SEEK_SET);
			fprintf(fInput,"%s%s%s",Header,Pos,LineFeed);
			fflush(fInput);
			fseek(fInput,CurrentPos,SEEK_SET);
		}
		WritePos = ftell(fInput) - DataLeft;
		continue;
		break;
	} while (result >= 0);
	fclose(fInput);
	if (Input) { free(Input);  Input = NULL; }
	if (Data) {  free(Data);  Data = NULL; }
	*/
}

void DisableAllCheats(void) {
	char CheatName[500];
	int count;

	for (count = 0; count < MaxCheats; count ++ ) {
		if (!GetCheatName(count,CheatName,sizeof(CheatName))) { break; }
		SaveCheat(count,FALSE);
	}
}

/********************************************************************************************
  GetCheatIniFileName

  Purpose: 
  Parameters:
  Returns:

********************************************************************************************/
char * GetCheatIniFileName(void) {

	if(PathFileExists("T:\\Project64.cht")) {
		OutputDebugString("T:\\Project64.cht Check OK!\n");
		return "T:\\Project64.cht";
	}
	else {
		OutputDebugString("T:\\Project64.cht Failed to Load!\n");
		
		if(PathFileExists("D:\\Project64.cht"))
			OutputDebugString("D:\\Project64.cht Check OK!\n");
		else
			OutputDebugString("D:\\Project64.cht Failed to Load!\n");
			
		return "D:\\Project64.cht";
	}
}

/********************************************************************************************
  GetCheatName

  Purpose: 
  Parameters:
  Returns:

********************************************************************************************/
BOOL GetCheatName(int CheatNo, char * CheatName, int CheatNameLen) {
	char *String = NULL, Identifier[100];
	DWORD len;

	LPSTR IniFileName;
	IniFileName = GetCheatIniFileName();
	sprintf(Identifier,"%08X-%08X-C:%X",*(DWORD *)(&RomHeader[0x10]),*(DWORD *)(&RomHeader[0x14]),RomHeader[0x3D]);
	sprintf(CheatName,"Cheat%d",CheatNo);
	_GetPrivateProfileString2(Identifier,CheatName,"",&String,IniFileName);
	if (strlen(String) == 0) {
		memset(CheatName,0,CheatNameLen);
		if (String) { free(String); }
		return FALSE;		
	}
	len = strrchr(String,'"') - strchr(String,'"') - 1;
	memset(CheatName,0,CheatNameLen);
	strncpy(CheatName,strchr(String,'"') + 1,len);

	if (String) { free(String); }
	
	return TRUE;
}


/********************************************************************************************
  LoadCheatExt

  Purpose: 
  Parameters:
  Returns:

********************************************************************************************/
BOOL LoadCheatExt(char * CheatName, char * CheatExt, int MaxCheatExtLen) {
/*
	char *String = NULL, Identifier[100], FilePath[350];
	sprintf(FilePath, "%s%08X\\%08X-%08X-%02X.cht", g_szPathSaves, *((DWORD *)(RomHeader + 0x10)), *((DWORD *)(RomHeader + 0x10)), *((DWORD *)(RomHeader + 0x14)), *((BYTE *)(RomHeader + 0x3D)));
	sprintf(Identifier,"%08X-%08X-C:%X",*(DWORD *)(&RomHeader[0x10]),*(DWORD *)(&RomHeader[0x14]),RomHeader[0x3D]);
	if(PathFileExists(FilePath)) {
		OutputDebugString("Cheat Ext Exists!\n");
		sprintf(String,"%s.exten",CheatName);
		_GetPrivateProfileString2(Identifier,String,"",&CheatExt,FilePath);
		return TRUE;
	}
	else
	{
		return FALSE;
	}
*/
	return FALSE;
}

void LoadCode (LPSTR CheatName, LPSTR CheatString)
{
	char * ReadPos = CheatString;
	int count2;

	for (count2 = 0; count2 < MaxGSEntries; count2 ++) {
		char CheatExt[200];
		WORD Value;

		Codes[NoOfCodes].Code[count2].Command = AsciiToHex(ReadPos);
		if (strchr(ReadPos,' ') == NULL) { break; }
		ReadPos = strchr(ReadPos,' ') + 1;
		if (strncmp(ReadPos,"????",4) == 0) {
			if (LoadCheatExt(CheatName,CheatExt,sizeof(CheatExt))) {
				Value = CheatExt[0] == '$'?(WORD)AsciiToHex(&CheatExt[1]):(WORD)atol(CheatExt);
			} else {
				count2 = 0; break;
			}
			Codes[NoOfCodes].Code[count2].Value = Value;
		} else if (strncmp(ReadPos,"??",2) == 0) {
			Codes[NoOfCodes].Code[count2].Value = (BYTE)(AsciiToHex(ReadPos));
			if (LoadCheatExt(CheatName,CheatExt,sizeof(CheatExt))) {
				Value = CheatExt[0] == '$'?(BYTE)AsciiToHex(&CheatExt[1]):(BYTE)atol(CheatExt);
			} else {
				count2 = 0; break;
			}
			Codes[NoOfCodes].Code[count2].Value += (Value << 16);
		} else if (strncmp(&ReadPos[2],"??",2) == 0) {				
			Codes[NoOfCodes].Code[count2].Value = (WORD)(AsciiToHex(ReadPos) << 16);
			if (LoadCheatExt(CheatName,CheatExt,sizeof(CheatExt))) {
				Value = CheatExt[0] == '$'?(BYTE)AsciiToHex(&CheatExt[1]):(BYTE)atol(CheatExt);
			} else {
				count2 = 0; break;
			}
			Codes[NoOfCodes].Code[count2].Value += Value;
		} else {
			Codes[NoOfCodes].Code[count2].Value = (WORD)AsciiToHex(ReadPos);
		}
		if (strchr(ReadPos,',') == NULL) { continue; }
		ReadPos = strchr(ReadPos,',') + 1;
	}
	if (count2 == 0) { return; }
	if (count2 < MaxGSEntries) {
		Codes[NoOfCodes].Code[count2].Command = 0;
		Codes[NoOfCodes].Code[count2].Value   = 0;
	}
	NoOfCodes += 1;
}

void LoadPermCheats (void) 
{
	LPSTR IniFileName;
	char * String = NULL;
	char Identifier[100];
	int count;

	IniFileName = GetIniFileName();
	sprintf(Identifier,"%08X-%08X-C:%X",*(DWORD *)(&RomHeader[0x10]),*(DWORD *)(&RomHeader[0x14]),RomHeader[0x3D]);
	
	for (count = 0; count < MaxCheats; count ++ ) 
	{
		char CheatName[300];
		
		sprintf(CheatName,"Cheat%d",count);
		_GetPrivateProfileString2(Identifier,CheatName,"",&String,IniFileName); // load cheat entry from ini
		if (strlen(String) == 0){ break; }
		LoadCode (NULL, String);
	}
	if (String) { free(String); }
}


/********************************************************************************************
  LoadCheats

  Purpose: 
  Parameters:
  Returns:

********************************************************************************************/
void LoadCheats (void) {
	DWORD len, count;
	LPSTR IniFileName;
	char * String = NULL;
	char Identifier[100];
	char CheatName[300];
	
	IniFileName = GetCheatIniFileName();
	sprintf(Identifier,"%08X-%08X-C:%X",*(DWORD *)(&RomHeader[0x10]),*(DWORD *)(&RomHeader[0x14]),RomHeader[0x3D]);
	NoOfCodes = 0;
	NoOfCodes2 = 0;

	LoadPermCheats();
	
	for (count = 0; count < MaxCheats; count ++ ) {
		char * ReadPos;

		sprintf(CheatName,"Cheat%d",count);
		_GetPrivateProfileString2(Identifier,CheatName,"",&String,IniFileName);
		if (strlen(String) == 0){ break; }
		if (strchr(String,'"') == NULL) { continue; }
		len = strrchr(String,'"') - strchr(String,'"') - 1;
		if ((int)len < 1) { continue; }
		memset(CheatName,0,sizeof(CheatName));
		strncpy(CheatName,strchr(String,'"') + 1,len);
		if (strlen(CheatName) == 0) { continue; }
		//if (strrchr(CheatName,'\\') != NULL) {
		//	strcpy(CheatName,strrchr(CheatName,'\\') + 1);
		//}	
		NoOfCodes2++;
		if (!CheatActive (CheatName,count)) { continue; }
		ReadPos = strrchr(String,'"') + 2;
		LoadCode(CheatName, ReadPos);
	}
	if (String) { free(String); }
}

void SaveCheat(int CheatNo, BOOL Active) {

HANDLE hFile;
#ifdef CHEAT_INI
	FILE* f;
	int LastActive[500], i, j;
	char FilePath[350], DirPath[350];
	sprintf(FilePath, "%s%08X\\%08X-%08X-%02X.cht", g_szPathSaves, *((DWORD *)(RomHeader + 0x10)), *((DWORD *)(RomHeader + 0x10)), *((DWORD *)(RomHeader + 0x14)), *((BYTE *)(RomHeader + 0x3D)));
	sprintf(DirPath, "%s%08X\\", g_szPathSaves, *((DWORD *)(RomHeader + 0x10)), *((DWORD *)(RomHeader + 0x10)), *((DWORD *)(RomHeader + 0x14)), *((BYTE *)(RomHeader + 0x3D)));

	if(PathFileExists(FilePath))
	{
		f = fopen(FilePath , "r");
		if(f){
			char line[100];
			for(i = 0; i < 500; i++){
				fgets(line,100, f);
				if(i == CheatNo){
					for(j = 0; j < 100; j++) {
						if(line[j] == '=')
							LastActive[i] = atoi(&line[j+1]);
					}
				}
			}
		fclose(f);
		}
		f = fopen(FilePath , "w");
		if(f) {
			for(i=0;i<500;i++){
				if(i==CheatNo)
					fprintf(f, "Cheat%d.Active=%d\n", CheatNo, Active);
				else
					fprintf(f, "Cheat%d.Active=0\n", i);
			}
			fclose(f);
		}
	}
	else
	{
		hFile = CreateFile(FilePath,GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ,
				NULL,CREATE_NEW,FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS, NULL);
		CloseHandle(hFile);
		if(!PathFileExists(FilePath)){
			CreateDirectory(DirPath,NULL);
			hFile = CreateFile(FilePath,GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ,
				NULL,CREATE_NEW,FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS, NULL);
		}

		f = fopen(FilePath , "w");
		if(f) {
			for(i=0;i<500;i++){
				if(i==CheatNo)
					fprintf(f, "Cheat%d.Active=%d\n", CheatNo, Active);
				else
					fprintf(f, "Cheat%d.Active=0\n", i);
			}
			fclose(f);
		}
	}

	OutputDebugString("Cheat Saved!\n");
#else
	char FilePath[350], DirPath[350];
	sprintf(FilePath, "%s%08X\\Cheats\\%08X-%08X-%02X\\%d", g_szPathSaves, *((DWORD *)(RomHeader + 0x10)), *((DWORD *)(RomHeader + 0x10)), *((DWORD *)(RomHeader + 0x14)), *((BYTE *)(RomHeader + 0x3D)), CheatNo);
	
	if(!PathFileExists(FilePath)&& Active){
		sprintf(DirPath, "%s%08X\\Cheats\\", g_szPathSaves, *((DWORD *)(RomHeader + 0x10)), *((DWORD *)(RomHeader + 0x10)), *((DWORD *)(RomHeader + 0x14)), *((BYTE *)(RomHeader + 0x3D)));	
		CreateDirectory(DirPath,NULL);		
		sprintf(DirPath, "%s%08X\\Cheats\\%08X-%08X-%02X\\", g_szPathSaves, *((DWORD *)(RomHeader + 0x10)), *((DWORD *)(RomHeader + 0x10)), *((DWORD *)(RomHeader + 0x14)), *((BYTE *)(RomHeader + 0x3D)));	
		CreateDirectory(DirPath,NULL);
		hFile = CreateFile(FilePath,GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ,
			NULL,CREATE_NEW,FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS, NULL);
		CloseHandle(hFile);
	}
	else if(PathFileExists(FilePath) && !Active){
		DeleteFile(FilePath);
	}

#endif
}

/********************************************************************************************
  SaveCheatExt

  Purpose: 
  Parameters:
  Returns:

********************************************************************************************/
void SaveCheatExt(char * CheatName, char * CheatExt) {

	/*char *String = NULL, Identifier[100], FilePath[350];// ActiveString[500];
	sprintf(FilePath, "%s%08X%08X-%08X-%02X.cht", g_szPathSaves, *((DWORD *)(RomHeader + 0x10)), *((DWORD *)(RomHeader + 0x10)), *((DWORD *)(RomHeader + 0x14)), *((BYTE *)(RomHeader + 0x3D)));
	sprintf(Identifier,"%08X-%08X-C:%X",*(DWORD *)(&RomHeader[0x10]),*(DWORD *)(&RomHeader[0x14]),RomHeader[0x3D]);
	if(PathFileExists(FilePath)) {
		OutputDebugString("T:\\Cheat Ext File Found!\n");
		sprintf(String,"%s.exten",CheatName);
		//sprintf(ActiveString,"%s",CheatExt);
		_WritePrivateProfileString(Identifier,String,CheatExt,FilePath);
		//_WritePrivateProfileString(Identifier,String,(CONST BYTE *)(&CheatExt),FilePath);
		OutputDebugString("Cheat Ext Saved!\n");
	}
	else
	{
		HANDLE hFile;
		hFile = CreateFile(FilePath,GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ,
				NULL,CREATE_NEW,FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			OutputDebugString("Cannot Create T:\\Cheat Ext File!\n");
		}
		OutputDebugString("T:\\Cheat Ext File Created!\n");
		sprintf(String,"%s.exten",CheatName);
		//sprintf(ActiveString,"%s",CheatExt);
		_WritePrivateProfileString(Identifier,String,CheatExt,FilePath);
		//_WritePrivateProfileString(Identifier,String,(CONST BYTE *)(&CheatExt),FilePath);
		OutputDebugString("Cheat Ext Saved!\n");
	}*/
}
