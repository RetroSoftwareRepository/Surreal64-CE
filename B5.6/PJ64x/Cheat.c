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
//#include "../simpleini.h" // ini parser
//extern char romCRC[32];
//extern char szPathSaves[256];


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
int NoOfCodes;


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
void SaveCheat             ( char * CheatName, BOOL Active );
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
BOOL CheatActive (char * Name) {
/*
	CSimpleIniA ini;
	SI_Error rc;
	ini.SetUnicode(true);
	ini.SetMultiKey(true);
	ini.SetMultiLine(false);
	ini.SetSpaces(false); // spaces before and after =
	DWORD Active;

	char szIniFilename[64];
	sprintf(szIniFilename, "%s%s\\%s_cht.ini", szPathSaves, romCRC, romCRC);

	rc = ini.LoadFile(szIniFilename);
	if (rc < 0) 
	{
		OutputDebugStringA(" Failed to Load!\n");
		return FALSE;
	}
	OutputDebugStringA(" Successfully Loaded!\n");

	Active = ini.GetBoolValue("Cheat Activation", "Cheat Active", false);
	return Active;
/*
	char String[300], Identifier[100];
	HKEY hKeyResults = 0;
	long lResult;
	
	sprintf(Identifier,"%08X-%08X-C:%X",*(DWORD *)(&RomHeader[0x10]),*(DWORD *)(&RomHeader[0x14]),RomHeader[0x3D]);
	sprintf(String,"D:\\Cheats\\%s",Identifier);
	//lResult = RegOpenKeyEx( HKEY_CURRENT_USER,String,0, KEY_ALL_ACCESS,&hKeyResults); // check is game ID excists in registry
	//Open cheat ini, check if game exists.
	if (lResult == ERROR_SUCCESS) {
		DWORD Type, Bytes, Active;
		char GameName[300];

		Bytes = sizeof(GameName);
		//lResult = RegQueryValueEx(hKeyResults,"Name",0,&Type,(LPBYTE)GameName,&Bytes); // get gamename from registry
		//Load the Rom name...
		Bytes = sizeof(Active);
		lResult = RegQueryValueEx(hKeyResults,Name,0,&Type,(LPBYTE)(&Active),&Bytes); // get cheat-state from registry
		RegCloseKey(hKeyResults);
		if (lResult == ERROR_SUCCESS) { return Active; } // if no errors return active state
	}
	return FALSE;*/

	return FALSE;

    // Need to check an ini entry that says if cheats were/are active. 
	//sprintf(File, "%s%08X\\%08X-%08X-%02X.eep", g_szPathCheats, *((DWORD *)(RomHeader + 0x10)), *((DWORD *)(RomHeader + 0x10)), *((DWORD *)(RomHeader + 0x14)), *((BYTE *)(RomHeader + 0x3D)));

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
		SaveCheat(CheatName,FALSE);
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
	CSimpleIniA ini;
	SI_Error rc;
	ini.SetUnicode(true);
	ini.SetMultiKey(true);
	ini.SetMultiLine(false);
	ini.SetSpaces(false); // spaces before and after =

	char String[350];

	char szIniFilename[64];
	sprintf(szIniFilename, "%s%s\\%s_cht.ini", szPathSaves, romCRC, romCRC);

	rc = ini.LoadFile(szIniFilename);
	if (rc < 0) 
	{
		OutputDebugStringA(" Failed to Load!\n");
		return FALSE;
	}
	OutputDebugStringA(" Successfully Loaded!\n");

	sprintf(String,"%s.exten",CheatName);
	//CheatExt = ini.GetValue("Settings", String, "");
	return TRUE;
	

/*

	char String[350], Identifier[100];
	//HKEY hKeyResults = 0;
	long lResult;
	
	if (CheatName == NULL)
	{ 
		return FALSE;
	}

	sprintf(Identifier,"%08X-%08X-C:%X",*(DWORD *)(&RomHeader[0x10]),*(DWORD *)(&RomHeader[0x14]),RomHeader[0x3D]);
	sprintf(String,"Software\\N64 Emulation\\%s\\Cheats\\%s",AppName,Identifier);

	//lResult = RegOpenKeyEx( HKEY_CURRENT_USER,String,0, KEY_ALL_ACCESS,&hKeyResults);	 // open rom cheat ini
	if (lResult == ERROR_SUCCESS) {		
		DWORD Type, Bytes;

		sprintf(String,"%s.exten",CheatName);
		Bytes = MaxCheatExtLen;
		//lResult = RegQueryValueEx(hKeyResults,String,0,&Type,(LPBYTE)CheatExt,&Bytes); // load cheat ext	
		//RegCloseKey(hKeyResults);
		if (lResult == ERROR_SUCCESS) { return TRUE; }
	}
	return FALSE;
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
		if (!CheatActive (CheatName)) { continue; }
		ReadPos = strrchr(String,'"') + 2;
		LoadCode(CheatName, ReadPos);
	}
	if (String) { free(String); }
}

void SaveCheat(char * CheatName, BOOL Active) {
	/*
	char String[300], Identifier[100];
	DWORD Disposition = 0;
	//HKEY hKeyResults = 0;
	long lResult;
	
	sprintf(Identifier,"%08X-%08X-C:%X",*(DWORD *)(&RomHeader[0x10]),*(DWORD *)(&RomHeader[0x14]),RomHeader[0x3D]);
	sprintf(String,"Software\\N64 Emulation\\%s\\Cheats\\%s",AppName,Identifier);
	//lResult = RegCreateKeyEx( HKEY_CURRENT_USER, String,0,"", REG_OPTION_NON_VOLATILE,
	//	KEY_ALL_ACCESS,NULL, &hKeyResults,&Disposition);
	// Open ini file, check for rom name
	if (lResult == ERROR_SUCCESS) {		
		RegSetValueEx(hKeyResults,"Name",0,REG_SZ,(CONST BYTE *)RomName,strlen(RomName)); //save name							
		if (Active) {
			RegSetValueEx(hKeyResults,CheatName,0, REG_DWORD,(CONST BYTE *)(&Active),sizeof(DWORD));// save entry
		} else {
			RegDeleteValue(hKeyResults,CheatName); //delete entry
		}
		RegCloseKey(hKeyResults); //close ini
	}
	*/
}

/********************************************************************************************
  SaveCheatExt

  Purpose: 
  Parameters:
  Returns:

********************************************************************************************/
void SaveCheatExt(char * CheatName, char * CheatExt) {
	/*
	char String[300], Identifier[100];
	DWORD Disposition = 0;
	//HKEY hKeyResults = 0;
	long lResult;
	
	sprintf(Identifier,"%08X-%08X-C:%X",*(DWORD *)(&RomHeader[0x10]),*(DWORD *)(&RomHeader[0x14]),RomHeader[0x3D]);
	sprintf(String,"Software\\N64 Emulation\\%s\\Cheats\\%s",AppName,Identifier);
	//lResult = RegCreateKeyEx( HKEY_CURRENT_USER, String,0,"", REG_OPTION_NON_VOLATILE,
	//	KEY_ALL_ACCESS,NULL, &hKeyResults,&Disposition);
	//
	//if (lResult == ERROR_SUCCESS) {		
		sprintf(String,"%s.exten",CheatName);
		//RegSetValueEx(hKeyResults,"Name",0,REG_SZ,(CONST BYTE *)RomName,strlen(RomName));				 // save name
		//RegSetValueEx(hKeyResults,String,0,REG_SZ,(CONST BYTE *)CheatExt,strlen(CheatExt));			 // save cheat ext
		//RegCloseKey(hKeyResults);																		 // close file
	//}
	*/
}
