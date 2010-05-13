/**
 * Surreal 64 Launcher (C) 2003
 * 
 * This program is free software; you can redistribute it and/or modify it under 
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option) any later
 * version. This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details. You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA. To contact the
 * authors: email: buttza@hotmail.com, lantus@lantus-x.com
 */

#pragma once

#include "Launcher.h"

// Ez0n3 - plugins
#include "../Plugins.h"

struct RomIniEntry
{
	// crc 1 & 2 and the country
	dword dwCrc1;
	dword dwCrc2;
	byte byCountry;

	// game title and comments
	char szGameName[40];
	char szAltTitle[51];
	char szComments[80];

	// what emulators support this game
	bool pbEmuSupported[3];
	int iPreferredEmulator;
	
	//Ez0n3 - preferred plugins
	//int iPreferredVideoPlugin; 
	//int iPreferredAudioPlugin; 
	

	// controller config
	byte pbyControllerConfig[72];

	// 1964 settings
	dword dw1964DynaMem;
	dword dw1964PagingMem;
	
	// Pj64 settings
	dword dwPJ64DynaMem;
	dword dwPJ64PagingMem;

	// Ez0n3 - leave these for now incase they are set in ini file
	// common settings
	bool bUseLLERSP;
	bool bUseBasicAudio;
	
	// Ez0n3 - reinstate max video mem
	dword dwMaxVideoMem;
	int iAudioPlugin;
	int videoplugin;
	

	RomIniEntry *pNextEntry;
};

class IniFile
{
public:
	IniFile(void);
	~IniFile(void);

	bool Load(const string &szIniFilename);
	bool Save(const string &szIniFilename);

	// Settings
	string GetRomPath();
	void SetRomPath(const string &romPath);
	string GetMediaPath();
	void SetMediaPath(const string &mediaPath);

	// Rom Entries
	RomIniEntry *CreateRomEntry(dword crc1, dword crc2, byte country);
	RomIniEntry *GetRomEntry(dword crc1, dword crc2, byte country);
	RomIniEntry *GetDefaultRomEntry();

private:
	void ParseSettingsEntry(const string &szLine);
	void ParseRomEntry(RomIniEntry *pCurrentEntry, const string &szLine);

private:
	/**
	 * All variables to do with setting
	 */
	string m_szRomPath;
	string m_szMediaPath;
	RomIniEntry m_defaultEntry;

	/**
	 * A chaining hash table used to store the ini entries
	 */
	RomIniEntry **m_entryTable;
};

extern IniFile g_iniFile;
