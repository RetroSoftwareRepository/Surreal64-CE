/*
Copyright (C) 2001 CyRUS64 (http://www.boob.co.uk)

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

#include "stdafx.h"

#include <istream>
#include <fstream>

std::ifstream& getline( std::ifstream &is, char *str );


///////////////////////////////////////////////
//// Constructors / Deconstructors
///////////////////////////////////////////////

IniFile::IniFile(char *szFileName)
{
	sections.clear();
	bChanged = false;

	if (szFileName == NULL)
		strcpy(m_szFileName, "");
	else 
		strcpy(m_szFileName, szFileName);

}

IniFile::~IniFile()
{
	if (bChanged)
	{
		WriteIniFile(m_szFileName);
	}
}

char * left(char * src, int nchars)
{
	static char dst[300];			// BUGFIX (STRMNNRM)
	strncpy(dst,src,nchars);
	dst[nchars]=0;
	return dst;
}

char * right(char *src, int nchars)
{
	static char dst[300];			// BUGFIX (STRMNNRM)
	strncpy(dst, src + strlen(src) - nchars, nchars);
	dst[nchars]=0;
	return dst;
}

// (STRMNNRMN - Strip spaces from end of names)
char * tidy(char * s)
{
	char * p = s + lstrlen(s);

	p--;
	while (p >= s && *p == ' ')
	{
		*p = 0;
		p--;
	}
	return s;

}

extern void GetPluginDir( char * Directory );

BOOL IniFile::ReadIniFile()
{
	std::ifstream inifile;
	char readinfo[100];
	char tempreadinfo1[100];
	//char tempreadinfo2[100];
	char trim[]="{}"; //remove first and last character
	
	char filename[256];
	 
	strcpy(filename,m_szFileName);
	inifile.open(filename);
	
	if (inifile.fail())
	{
		return FALSE;
	}

	while (getline(inifile,readinfo)/*&&sectionno<999*/)
	{
		tidy(readinfo);			// STRMNNRMN - Strip spaces from end of lines

		// STRMNNRMN - comments
		if (readinfo[0] == '/')
			continue;

		if (!lstrcmpi(readinfo,"")==0)
		{
			if (readinfo[0] == '{') //if a section heading
			{
				section newsection;

				//StrTrim(readinfo,trim);
				 
				strcpy(tempreadinfo1,&readinfo[1]);
				int numToRead = strlen(tempreadinfo1) - 1;
				//strncpy(tempreadinfo2, tempreadinfo1, numToRead);
				//strcpy(readinfo, tempreadinfo2);
				tempreadinfo1[numToRead] = '\0';
				strcpy(readinfo, tempreadinfo1);


				strcpy(newsection.crccheck, readinfo);
				
				strcpy(newsection.comment, "");
				strcpy(newsection.info, "");
				newsection.ucode = 5;				// Most new games are ucode 5 - thanks CyRUS64
				newsection.bDisablePatches = false;
				newsection.bDisableTextureCRC = false;
				newsection.bDisableEeprom = false;
				newsection.bIncTexRectEdge = false;
				newsection.bTextureScaleHack = false;
				newsection.bDisableSpeedSync = false;
				newsection.bDisableDynarec = false;
				newsection.bFastLoadTile = false;
				newsection.bPrimaryDepthHack = false;
				newsection.bTexture1Hack = false;
				newsection.bExpansionPak = false;
				newsection.VIWidth = -1;
				newsection.VIHeight = -1;
				newsection.dwEepromSize = 2048;
				newsection.dwRescanCount = 0;

				newsection.dwFastTextureCRC = 0;
				newsection.dwForceScreenClear = 0;
				newsection.dwAccurateTextureMapping = 0;
				newsection.dwNormalBlender = 0;
				newsection.dwNormalCombiner = 0;
				newsection.dwEnableObjBG = 0;
				newsection.dwFrameBufferOption = 0;
				newsection.dwScreenUpdateSetting = 0;

				sections.push_back(newsection);
				
			}
			else
			{		
				int sectionno = sections.size() - 1;

				if (lstrcmpi(left(readinfo,5), "UCode")==0)
					sections[sectionno].ucode=strtol(right(readinfo,1),NULL,10);
				
				if (lstrcmpi(left(readinfo,7), "Comment")==0)
					strcpy(sections[sectionno].comment,right(readinfo,strlen(readinfo)-8));
				
				if (lstrcmpi(left(readinfo,4), "Info")==0)
					strcpy(sections[sectionno].info,right(readinfo,strlen(readinfo)-5));
				
				if (lstrcmpi(left(readinfo,4), "Name")==0)
					strcpy(sections[sectionno].name,right(readinfo,strlen(readinfo)-5));
				
				if (lstrcmpi(left(readinfo,14), "DisableDynarec")==0)
					sections[sectionno].bDisableDynarec=true;
				
				if (lstrcmpi(left(readinfo,14), "DisablePatches")==0)
					sections[sectionno].bDisablePatches=true;
				
				if (lstrcmpi(left(readinfo,20), "DisableTextureCRC")==0)
					sections[sectionno].bDisableTextureCRC=true;
				
				if (lstrcmpi(left(readinfo,13), "DisableEeprom")==0)
					sections[sectionno].bDisableEeprom=true;
				
				if (lstrcmpi(left(readinfo,16), "DisableSpeedSync")==0)
					sections[sectionno].bDisableSpeedSync=true;
				
				if (lstrcmpi(left(readinfo,16), "PrimaryDepthHack")==0)
					sections[sectionno].bPrimaryDepthHack=true;

				if (lstrcmpi(left(readinfo,16), "Texture1Hack")==0)
					sections[sectionno].bTexture1Hack=true;

				if (lstrcmpi(left(readinfo,16), "FastLoadTile")==0)
					sections[sectionno].bFastLoadTile=true;

				if (lstrcmpi(left(readinfo,14), "IncTexRectEdge")==0)
					sections[sectionno].bIncTexRectEdge=true;

				if (lstrcmpi(left(readinfo,16), "TexRectScaleHack")==0)
					sections[sectionno].bTextureScaleHack=true;

				if (lstrcmpi(left(readinfo,12), "ExpansionPak")==0)
					sections[sectionno].bExpansionPak = true;

				if (lstrcmpi(left(readinfo,10), "EepromSize")==0)
					sections[sectionno].dwEepromSize = strtol(right(readinfo,4),NULL,10);

				if (lstrcmpi(left(readinfo,7), "VIWidth")==0)
					sections[sectionno].VIWidth = strtol(right(readinfo,3),NULL,10);

				if (lstrcmpi(left(readinfo,8), "VIHeight")==0)
					sections[sectionno].VIHeight = strtol(right(readinfo,3),NULL,10);

				if (lstrcmpi(left(readinfo,13), "OSRescanCount")==0)
					sections[sectionno].dwRescanCount = strtol(right(readinfo,4),NULL,10);
				
				if (lstrcmpi(left(readinfo,11), "EnableObjBG")==0)
					sections[sectionno].dwEnableObjBG = strtol(right(readinfo,1),NULL,10);

				if (lstrcmpi(left(readinfo,16), "ForceScreenClear")==0)
					sections[sectionno].dwForceScreenClear = strtol(right(readinfo,1),NULL,10);

				if (lstrcmpi(left(readinfo,22), "AccurateTextureMapping")==0)
					sections[sectionno].dwAccurateTextureMapping = strtol(right(readinfo,1),NULL,10);

				if (lstrcmpi(left(readinfo,14), "FastTextureCRC")==0)
					sections[sectionno].dwFastTextureCRC = strtol(right(readinfo,1),NULL,10);

				if (lstrcmpi(left(readinfo,18), "NormalAlphaBlender")==0)
					sections[sectionno].dwNormalBlender = strtol(right(readinfo,1),NULL,10);

				if (lstrcmpi(left(readinfo,19), "NormalColorCombiner")==0)
					sections[sectionno].dwNormalCombiner = strtol(right(readinfo,1),NULL,10);

				if (lstrcmpi(left(readinfo,20), "FrameBufferEmulation")==0)
					sections[sectionno].dwFrameBufferOption = strtol(right(readinfo,1),NULL,10);

				if (lstrcmpi(left(readinfo,19), "ScreenUpdateSetting")==0)
					sections[sectionno].dwScreenUpdateSetting = strtol(right(readinfo,1),NULL,10);
			}
		}
	}
	inifile.close();

	return TRUE;
}

//read a line from the ini file
std::ifstream & getline(std::ifstream & is, char *str)
{
    char buf[100];
	
    is.getline(buf,100);
    strcpy( str,buf);
    return is;
}

void IniFile::WriteIniFile(LPCTSTR szFileName)
{
	TCHAR szFileNameOut[MAX_PATH+1];
	TCHAR szFileNameDelete[MAX_PATH+1];
	TCHAR filename[MAX_PATH+1];
	DWORD i;
	FILE * fhIn;
	FILE * fhOut;
	TCHAR szBuf[1024+1];
	char trim[]="{}\n\r"; //remove first and last character

	GetPluginDir(szFileNameOut);
	GetPluginDir(szFileNameDelete);
	wsprintf(filename, "%s.tmp", szFileName);
	strcat(szFileNameOut, filename);
	wsprintf(filename, "%s.del", szFileName);
	strcat(szFileNameDelete, filename);

	GetPluginDir(filename);
	strcat(filename,m_szFileName);
	fhIn = fopen(filename, "r");
	if (fhIn == NULL)
		return;

	fhOut = fopen(szFileNameOut, "w");
	if (fhOut == NULL)
	{
		fclose(fhIn);
		return;
	}

	// Mark all sections and needing to be written
	for (i = 0; i < sections.size(); i++)
	{
		sections[i].bOutput = false;
	}


	while (fgets(szBuf, 1024, fhIn))
	{
		if (szBuf[0] == '{')
		{
			BOOL bFound = FALSE;
			// Start of section
			//StrTrim(szBuf,trim);


			for (i = 0; i < sections.size(); i++)
			{
				if (sections[i].bOutput)
					continue;

				if (lstrcmpi(szBuf, sections[i].crccheck) == 0)
				{
					// Output this CRC
					OutputSectionDetails(i, fhOut);
					sections[i].bOutput = true;
					bFound = TRUE;
					break;
				}
			}
			if (!bFound)
			{
				// Do what? This should never happen, unless the user
				// replaces the inifile while Daedalus is running!
			}
		}
		else if (szBuf[0] == '/')
		{
			// Comment
			fputs(szBuf, fhOut);
			continue;
		}

	}

	// Input buffer done-  process any new entries!
	for (i = 0; i < sections.size(); i++)
	{
		// Skip any that have not been done.
		if (sections[i].bOutput)
			continue;
		// Output this CRC
		// Removed at request of Genueix :)
		//fprintf(fhOut, "// Automatically generated entry - may need editing\n");
		OutputSectionDetails(i, fhOut);
		sections[i].bOutput = true;
	}

	fclose(fhOut);
	fclose(fhIn);

	// Create the new file
	/*
	MoveFile(filename, szFileNameDelete);
	MoveFile(szFileNameOut, filename);
	DeleteFile(szFileNameDelete);
	*/
	DeleteFile(filename);
	MoveFile(szFileNameOut, filename);

	bChanged = false;
}


void IniFile::OutputSectionDetails(DWORD i, FILE * fh)
{
	fprintf(fh, "{%s}\n", sections[i].crccheck);
	
	fprintf(fh, "Name=%s\n", sections[i].name);
	//fprintf(fh, "UCode=%d\n", sections[i].ucode);

	if (lstrlen(sections[i].comment) > 0)
		fprintf(fh, "Comment=%s\n", sections[i].comment);

	if (lstrlen(sections[i].info) > 0)
		fprintf(fh, "Info=%s\n", sections[i].info);

	if (sections[i].bDisableDynarec)
		fprintf(fh, "DisableDynarec\n");

	if (sections[i].bDisablePatches)
		fprintf(fh, "DisablePatches\n");

	if (sections[i].bDisableTextureCRC)
		fprintf(fh, "DisableTextureCRC\n");

	if (sections[i].bDisableEeprom)
		fprintf(fh, "DisableEeprom\n");

	if (sections[i].bDisableSpeedSync)
		fprintf(fh, "DisableSpeedSync\n");

	if (sections[i].bPrimaryDepthHack)
		fprintf(fh, "PrimaryDepthHack\n");

	if (sections[i].bTexture1Hack)
		fprintf(fh, "Texture1Hack\n");

	if (sections[i].bFastLoadTile)
		fprintf(fh, "FastLoadTile\n");

	if (sections[i].bIncTexRectEdge)
		fprintf(fh, "IncTexRectEdge\n");

	if (sections[i].bTextureScaleHack)
		fprintf(fh, "TexRectScaleHack\n");

	if (sections[i].bExpansionPak)
		fprintf(fh, "ExpansionPak\n");

	if (sections[i].dwEepromSize != 2048)
		fprintf(fh, "EepromSize=%d\n", sections[i].dwEepromSize);

	if (sections[i].VIWidth > 0)
		fprintf(fh, "VIWidth=%d\n", sections[i].VIWidth);

	if (sections[i].VIHeight > 0)
		fprintf(fh, "VIHeight=%d\n", sections[i].VIHeight);

	if (sections[i].dwRescanCount != 0)
		fprintf(fh, "OSRescanCount=%d\n", sections[i].dwRescanCount);

	if (sections[i].dwEnableObjBG != 0)
		fprintf(fh, "EnableObjBG=%d\n", sections[i].dwEnableObjBG);

	if (sections[i].dwForceScreenClear != 0)
		fprintf(fh, "ForceScreenClear=%d\n", sections[i].dwForceScreenClear);

	if (sections[i].dwAccurateTextureMapping != 0)
		fprintf(fh, "AccurateTextureMapping=%d\n", sections[i].dwAccurateTextureMapping);

	if (sections[i].dwFastTextureCRC != 0)
		fprintf(fh, "FastTextureCRC=%d\n", sections[i].dwFastTextureCRC);

	if (sections[i].dwNormalBlender != 0)
		fprintf(fh, "NormalAlphaBlender=%d\n", sections[i].dwNormalBlender);

	if (sections[i].dwNormalCombiner != 0)
		fprintf(fh, "NormalColorCombiner=%d\n", sections[i].dwNormalCombiner);

	if (sections[i].dwFrameBufferOption != 0)
		fprintf(fh, "FrameBufferEmulation=%d\n", sections[i].dwFrameBufferOption);

	if (sections[i].dwScreenUpdateSetting != 0)
		fprintf(fh, "ScreenUpdateSetting=%d\n", sections[i].dwScreenUpdateSetting);

	fprintf(fh, "\n");			// Spacer
}


// Find the entry corresponding to the specified rom. 
// If the rom is not found, a new entry is created
// The resulting value is returned
void __cdecl DebuggerAppendMsg (const char * Message, ...);
int IniFile::FindEntry(DWORD dwCRC1, DWORD dwCRC2, BYTE nCountryID, LPCTSTR szName)
{
	DWORD i;
	CHAR szCRC[50+1];
	
	// Generate the CRC-ID for this rom:
	wsprintf(szCRC, "%08x%08x-%02x", dwCRC1, dwCRC2, nCountryID);

	for (i = 0; i < sections.size(); i++)
	{
		if (lstrcmpi(szCRC, sections[i].crccheck) == 0)
		{
			TRACE2("Found Ini Entry: %s, Ucode=%d", szCRC, sections[i].ucode);
			return i;
		}
	}
	
	// Add new entry!!!
	section newsection;

	strcpy(newsection.crccheck, szCRC);
	
	///MsgInfo("Can not find this rom in INI, use ucode=5 as default, %s", szCRC);

	lstrcpyn(newsection.name, szName, 50);
	newsection.ucode = 5;				// Most new games are ucode 5 - thanks CyRUS64
	strcpy(newsection.comment, "");
	strcpy(newsection.info, "");
	newsection.bDisablePatches = false;
	newsection.bDisableTextureCRC = false;
	newsection.bDisableEeprom = false;
	newsection.bIncTexRectEdge = false;
	newsection.bTextureScaleHack = false;
	newsection.bDisableSpeedSync = false;
	newsection.bDisableDynarec = false;
	newsection.bExpansionPak = false;
	newsection.bFastLoadTile = false;
	newsection.bPrimaryDepthHack = false;
	newsection.bTexture1Hack = false;
	newsection.dwEepromSize = 2048;
	newsection.dwRescanCount = 0;
	newsection.VIWidth = -1;
	newsection.VIHeight = -1;

	newsection.dwFastTextureCRC = 0;
	newsection.dwForceScreenClear = 0;
	newsection.dwAccurateTextureMapping = 0;
	newsection.dwNormalBlender = 0;
	newsection.dwNormalCombiner = 0;
	newsection.dwEnableObjBG = 0;
	newsection.dwFrameBufferOption = 0;
	newsection.dwScreenUpdateSetting = 0;

	sections.push_back(newsection);

	bChanged = true;				// Flag to indicate we should be updated
	return sections.size()-1;			// -1 takes into account increment
}