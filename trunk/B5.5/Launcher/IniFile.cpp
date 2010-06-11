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

#include "IniFile.h"
#include "Util.h"

// Ez0n3 - determine if the current phys ram is greater than 100MB
extern bool has128ram;

IniFile g_iniFile;

IniFile::IniFile(void)
{
	// allocate 64k for the rom ini entry 'hash' table
	m_entryTable = new RomIniEntry *[0x10000];

	for (dword i = 0; i < 0x10000; i++)
	{
		m_entryTable[i] = NULL;
	}

	// create a default ini entry, this will be used for games that
	// dont yet have an entry and for games that have an entry but dont
	// use every ini option
	memset(&m_defaultEntry, 0, sizeof(RomIniEntry));
	m_defaultEntry.pbEmuSupported[_1964]		= true;
	m_defaultEntry.pbEmuSupported[_Project64]	= true;
	m_defaultEntry.pbEmuSupported[_UltraHLE]	= false;
	m_defaultEntry.iPreferredEmulator = _None; // = num of emulators + 1
	
	// Ez0n3 - default plugins per ini
	//m_defaultEntry.videoplugin = _VideoPluginMissing; // = num of video plugins + 1
	//m_defaultEntry.iAudioPlugin = _AudioPluginMissing; // = num of audio plugins + 1
	

	byte defaultConfig[72] 
		= {0x00,0x01,0x02,0x03,0x08,0x09,0x0A,0x0B,
		   0x04,0x05,0x06,0x07,0x0C,0x10,0x11,0x16,
		   0x0D,0x17,0x00,0x01,0x02,0x03,0x08,0x09,
		   0x0A,0x0B,0x04,0x05,0x06,0x07,0x0C,0x10,
		   0x11,0x16,0x0D,0x17,0x00,0x01,0x02,0x03,
		   0x08,0x09,0x0A,0x0B,0x04,0x05,0x06,0x07,
		   0x0C,0x10,0x11,0x16,0x0D,0x17,0x00,0x01,
		   0x02,0x03,0x08,0x09,0x0A,0x0B,0x04,0x05,
		   0x06,0x07,0x0C,0x10,0x11,0x16,0x0D,0x17};

	memcpy(&m_defaultEntry.pbyControllerConfig, defaultConfig, 72);

	m_defaultEntry.dw1964DynaMem = 8;
	m_defaultEntry.dw1964PagingMem = 4;
	
	// Ez0n3 - its 16 everywhere else, why buck the sys
	//m_defaultEntry.dwPJ64DynaMem = 8;
	m_defaultEntry.dwPJ64DynaMem = 16;
	
	m_defaultEntry.dwPJ64PagingMem = 4;
	m_defaultEntry.bUseLLERSP = false;
	m_defaultEntry.bUseBasicAudio = false;
	
	// Ez0n3 - reinstate max video mem
	m_defaultEntry.dwMaxVideoMem = 5;
	
	// some more defaults
	m_defaultEntry.iAudioPlugin = _AudioPluginJttl;
	m_defaultEntry.videoplugin = _VideoPluginRice560;
	
}

IniFile::~IniFile(void)
{
	for (dword i = 0; i < 0x10000; i++)
	{
		if (m_entryTable[i] != NULL)
		{
			RomIniEntry *delEntry = m_entryTable[i];
			RomIniEntry *nextEntry = delEntry->pNextEntry;
			delete delEntry;

			while (nextEntry != NULL)
			{
				delEntry = nextEntry;
				nextEntry = delEntry->pNextEntry;
				delete delEntry;
			}
		}
	}

	delete [] m_entryTable;
}

bool IniFile::Load(const string &szIniFilename)
{
	ifstream iniFile;

	// open the ini file
	iniFile.open(szIniFilename.c_str());

	if (!iniFile.is_open())
	{
		return false;
	}

	// iterate through and parse every line
	bool bUnderSettings = false;
	bool bUnderRomEntry = false;
	RomIniEntry *pCurrentEntry = NULL;
	
	while (!iniFile.eof())
	{
		string szLine;
		int iLineLength = 0;

		getline(iniFile, szLine);

		// remove all white spaces from the line
		szLine = Trim(szLine);

		// check for an empty line or invalid line
		iLineLength = szLine.length();
		if (iLineLength < 2)
			continue;

		// check for // at the start of the line
		if (szLine.find("//") == 0)
			continue;

		// look for [Settings]
		if (szLine.find("[Settings]") == 0)
		{
			bUnderSettings = true;
			bUnderRomEntry = false;

			continue; 
		}
		// look for a rom entry [xxxxxxxx-xxxxxxxx-C:xx]
		else if (szLine.find("[") == 0 && szLine.find("]") == 23)
		{
			// extract the crc1/crc2 and country values from the string
			dword dwCrc1		= strtoul(szLine.substr(1, 8).c_str(), NULL, 16);
			dword dwCrc2		= strtoul(szLine.substr(10, 8).c_str(), NULL, 16);
			byte  byCountry		= static_cast<byte>(strtoul(szLine.substr(21, 2).c_str(), NULL, 16));
			
			// create a new entry
			pCurrentEntry = CreateRomEntry(dwCrc1, dwCrc2, byCountry);

			bUnderSettings = false;
			bUnderRomEntry = true;

			continue;
		}

		if (bUnderSettings)
		{
			ParseSettingsEntry(szLine);
		}
		else if (bUnderRomEntry)
		{
			ParseRomEntry(pCurrentEntry, szLine);
		}
	}

	iniFile.close();

	return true;
}

bool IniFile::Save(const string &szIniFilename)
{
	ofstream iniFile;

	// open/overwrite the ini file
	iniFile.open(szIniFilename.c_str());

	if (!iniFile.is_open())
	{
		return false;
	}

	// write out all the settings
	iniFile << "[Settings]" << endl;

	iniFile << "Rom Path=" << m_szRomPath << endl;
	iniFile << "Media Path=" << m_szMediaPath << endl;
	
	// default controller config
	{
		iniFile << "Default Controller Config=";
		for (int i = 0; i < 72; i++)
		{
			char buf[3];
			sprintf(buf, "%02X", m_defaultEntry.pbyControllerConfig[i]);
			iniFile << buf;

			if (i < 71)
				iniFile << ",";
		}
		iniFile << endl;
	}

	// other rom defaults
	iniFile << "Default 1964 Dyna Mem=" << m_defaultEntry.dw1964DynaMem << endl;
	iniFile << "Default 1964 Paging Mem=" << m_defaultEntry.dw1964PagingMem << endl;
	iniFile << "Default PJ64 Dyna Mem=" << m_defaultEntry.dwPJ64DynaMem << endl;
	iniFile << "Default PJ64 Paging Mem=" << m_defaultEntry.dwPJ64PagingMem << endl;
	iniFile << "Default Use LLE RSP=" << m_defaultEntry.bUseLLERSP << endl;
	iniFile << "Default Use Basic Audio=" << m_defaultEntry.bUseBasicAudio << endl; 
	
	
	// Ez0n3 - reinstate max video mem
	iniFile << "Default Max Video Mem=" << m_defaultEntry.dwMaxVideoMem << endl;
	
	// some more defaults
	iniFile << "Default Video Plugin=" << m_defaultEntry.videoplugin << endl;
	iniFile << "Default Audio Plugin=" << m_defaultEntry.iAudioPlugin << endl; 
	

	iniFile << endl;

	// write out all the rom settings
	for (dword i = 0; i < 0x10000; i++)
	{
		if (m_entryTable[i] != NULL)
		{
			RomIniEntry *pCurrentEntry = m_entryTable[i];
			
			do
			{
				// output the rom's crc and country values
				char szRomCrcs[25];
				sprintf(szRomCrcs, "[%08X-%08X-C:%02X]", pCurrentEntry->dwCrc1, pCurrentEntry->dwCrc2, pCurrentEntry->byCountry);
				iniFile << szRomCrcs << endl;

				// always output the game name, alt title and comments
				iniFile << "Game Name=" << pCurrentEntry->szGameName << endl;
				iniFile << "Alternate Title=" << pCurrentEntry->szAltTitle << endl;
				iniFile << "Comments=" << pCurrentEntry->szComments << endl;
				
				// from now on, only output values that differ from the current default values
				
				// emulators supported
				if (pCurrentEntry->pbEmuSupported[_1964] != m_defaultEntry.pbEmuSupported[_1964] || 
					pCurrentEntry->pbEmuSupported[_Project64] != m_defaultEntry.pbEmuSupported[_Project64] ||
					pCurrentEntry->pbEmuSupported[_UltraHLE] != m_defaultEntry.pbEmuSupported[_UltraHLE])
				{
					iniFile << "Emulators Supported=";
					iniFile << pCurrentEntry->pbEmuSupported[_1964] << ",";
					iniFile << pCurrentEntry->pbEmuSupported[_Project64] << ",";
					iniFile << pCurrentEntry->pbEmuSupported[_UltraHLE] << endl;
				}

				// preferred emulator
				if (pCurrentEntry->iPreferredEmulator < _None) // Ez0n3 - should be equal to or greater?
				{
					iniFile << "Preferred Emulator=" << pCurrentEntry->iPreferredEmulator << endl;
				}
				
				
				//Ez0n3 - preferred video plugin
				if (pCurrentEntry->videoplugin < _VideoPluginMissing && pCurrentEntry->videoplugin != m_defaultEntry.videoplugin)
				{
					iniFile << "Preferred Video Plugin=" << pCurrentEntry->videoplugin << endl;
				}
				// preferred audio plugin
				if (pCurrentEntry->iAudioPlugin < _AudioPluginMissing && pCurrentEntry->iAudioPlugin != m_defaultEntry.iAudioPlugin)
				{
					iniFile << "Preferred Audio Plugin=" << pCurrentEntry->iAudioPlugin << endl;
				}
				

				// controller config
				if (memcmp(pCurrentEntry->pbyControllerConfig, m_defaultEntry.pbyControllerConfig, sizeof(m_defaultEntry.pbyControllerConfig)) != 0)
				{
					iniFile << "Controller Config=";
					for (int i = 0; i < 72; i++)
					{
						char buf[3];
						sprintf(buf, "%02X", pCurrentEntry->pbyControllerConfig[i]);
						iniFile << buf;

						if (i < 71)
							iniFile << ",";
					}
					iniFile << endl;
				}

				if (pCurrentEntry->dw1964DynaMem != m_defaultEntry.dw1964DynaMem)
				{
					iniFile << "1964 Dyna Mem=" << pCurrentEntry->dw1964DynaMem << endl;
				}

				if (pCurrentEntry->dw1964PagingMem != m_defaultEntry.dw1964PagingMem)
				{
					iniFile << "1964 Paging Mem=" << pCurrentEntry->dw1964PagingMem << endl;
				}

				if (pCurrentEntry->dwPJ64DynaMem != m_defaultEntry.dwPJ64DynaMem)
				{
					iniFile << "PJ64 Dyna Mem=" << pCurrentEntry->dwPJ64DynaMem << endl;
				}

				if (pCurrentEntry->dwPJ64PagingMem != m_defaultEntry.dwPJ64PagingMem)
				{
					iniFile << "PJ64 Paging Mem=" << pCurrentEntry->dwPJ64PagingMem << endl;
				}

				if (pCurrentEntry->bUseLLERSP != m_defaultEntry.bUseLLERSP)
				{
					iniFile << "Use LLE RSP=" << pCurrentEntry->bUseLLERSP << endl;
				}

				if (pCurrentEntry->bUseBasicAudio != m_defaultEntry.bUseBasicAudio)
				{
					iniFile << "Use Basic Audio=" << pCurrentEntry->bUseBasicAudio << endl;
				}
				
				// Ez0n3 - reinstate max video mem
				if (pCurrentEntry->dwMaxVideoMem != m_defaultEntry.dwMaxVideoMem)
				{
					iniFile << "Max Video Mem=" << pCurrentEntry->dwMaxVideoMem << endl;
				}

				iniFile << endl;
		
				pCurrentEntry = pCurrentEntry->pNextEntry;
			}
			while (pCurrentEntry != NULL);
		}
	}

	iniFile.close();

	return true;
}

void IniFile::ParseSettingsEntry(const string &szLine)
{

	//Ez0n3 - set setting based on phys ram
	int maxVideoMem = 10;
	int maxDynaMem = 20;
	int maxPagingMem = 20;
	if (has128ram) {
		maxDynaMem = 32;
		maxPagingMem = 64;
	}

	if (szLine.find("Rom Path=") == 0)
	{
		m_szRomPath = FixPath(szLine.substr(9));
	}
	else if (szLine.find("Media Path=") == 0)
	{
		m_szMediaPath = FixPath(szLine.substr(11));
	}
	else if (szLine.find("Default Controller Config=") == 0)
	{
		if (szLine.size() >= 220)
		{
			for (int i = 0; i < 72; i++)
			{
				m_defaultEntry.pbyControllerConfig[i] 
					= static_cast<byte>(strtoul(szLine.substr(26 + (i*3), 2).c_str(), NULL, 16));
			}
		}
	}
	else if (szLine.find("Default 1964 Dyna Mem=") == 0)
	{
		stringstream conv(szLine.substr(22));
		conv >> m_defaultEntry.dw1964DynaMem;

		if (m_defaultEntry.dw1964DynaMem > maxDynaMem)
			m_defaultEntry.dw1964DynaMem = maxDynaMem;
	}
	else if (szLine.find("Default 1964 Paging Mem=") == 0)
	{
		stringstream conv(szLine.substr(24));
		conv >> m_defaultEntry.dw1964PagingMem;

		if (m_defaultEntry.dw1964PagingMem > maxPagingMem)
			m_defaultEntry.dw1964PagingMem = maxPagingMem;
	}
	else if (szLine.find("Default PJ64 Dyna Mem=") == 0)
	{
		stringstream conv(szLine.substr(22));
		conv >> m_defaultEntry.dwPJ64DynaMem;

		if (m_defaultEntry.dwPJ64DynaMem > maxDynaMem)
			m_defaultEntry.dwPJ64DynaMem = maxDynaMem;
	}
	else if (szLine.find("Default PJ64 Paging Mem=") == 0)
	{
		stringstream conv(szLine.substr(24));
		conv >> m_defaultEntry.dwPJ64PagingMem;

		if (m_defaultEntry.dwPJ64PagingMem > maxPagingMem)
			m_defaultEntry.dwPJ64PagingMem = maxPagingMem;
	}
	
	// Ez0n3 - these two are not used anymore in the emu's, but leave them to retain user setting between versions
	else if (szLine.find("Default Use LLE RSP=") == 0)
	{
		int val;
		stringstream conv(szLine.substr(20));
		conv >> val;

		m_defaultEntry.bUseLLERSP = (val != 0);
	}
	else if (szLine.find("Default Use Basic Audio=") == 0)
	{
		int val;
		stringstream conv(szLine.substr(24));
		conv >> val;

		m_defaultEntry.bUseBasicAudio = (val != 0);
	}
	
	// Ez0n3 - reinstate max video mem
	else if (szLine.find("Default Max Video Mem=") == 0)
	{
		stringstream conv(szLine.substr(22));
		conv >> m_defaultEntry.dwMaxVideoMem;

		if (m_defaultEntry.dwMaxVideoMem > maxVideoMem)
			m_defaultEntry.dwMaxVideoMem = maxVideoMem;
	}

	// some more defaults
	else if (szLine.find("Default Video Plugin=") == 0)
	{
		stringstream conv(szLine.substr(21));
		conv >> m_defaultEntry.videoplugin;

		if (m_defaultEntry.videoplugin >= _VideoPluginMissing)
			m_defaultEntry.videoplugin = (_VideoPluginMissing - 1);
	}
	else if (szLine.find("Default Audio Plugin=") == 0)
	{
		stringstream conv(szLine.substr(21));
		conv >> m_defaultEntry.iAudioPlugin;

		if (m_defaultEntry.iAudioPlugin >= _AudioPluginMissing)
			m_defaultEntry.iAudioPlugin = (_AudioPluginMissing - 1);
	}
	
	
}

void IniFile::ParseRomEntry(RomIniEntry *pCurrentEntry, const string &szLine)
{
	//Ez0n3 - set setting based on phys ram
	int maxDynaMem = 20;
	int maxPagingMem = 20;
	if (has128ram) {
		maxDynaMem = 32;
		maxPagingMem = 64;
	}

	if (szLine.find("Game Name=") == 0)
	{
		strncpy(pCurrentEntry->szGameName,				// copy to szGameName in entry
				szLine.substr(10).c_str(),				// grab everything after GN=
				sizeof(pCurrentEntry->szGameName) - 1);	// dont overrun the buffer
	}
	else if (szLine.find("Alternate Title=") == 0)
	{
		strncpy(pCurrentEntry->szAltTitle, 
				szLine.substr(16).c_str(), 
				sizeof(pCurrentEntry->szAltTitle) - 1);
	}
	else if (szLine.find("Comments=") == 0)
	{
		strncpy(pCurrentEntry->szComments, 
				szLine.substr(9).c_str(), 
				sizeof(pCurrentEntry->szAltTitle) - 1);
	}
	else if (szLine.find("Emulators Supported=") == 0)
	{
		// theres no way that it can be shorter than 25 characters
		if (szLine.size() >= 25)
		{
			for (int i = 0; i < 3; i++)
			{
				int val;
				stringstream conv(szLine.substr(20 + (i*2), 1));
				conv >> val;

				pCurrentEntry->pbEmuSupported[i] = (val != 0);
			}
		}
	}
	else if (szLine.find("Preferred Emulator=") == 0)
	{
		stringstream conv(szLine.substr(19));
		conv >> pCurrentEntry->iPreferredEmulator;

		if (pCurrentEntry->iPreferredEmulator > _None)
			pCurrentEntry->iPreferredEmulator = _None;
	}
	
	
	//Ez0n3 - preferred video plugin
	else if (szLine.find("Preferred Video Plugin=") == 0)
	{
		stringstream conv(szLine.substr(23)); // num chars to subtract to get val
		conv >> pCurrentEntry->videoplugin;

		if (pCurrentEntry->videoplugin > _VideoPluginMissing)
			pCurrentEntry->videoplugin = _VideoPluginMissing;
	}
	// preferred audio plugin
	else if (szLine.find("Preferred Audio Plugin=") == 0)
	{
		stringstream conv(szLine.substr(23));
		conv >> pCurrentEntry->iAudioPlugin;

		if (pCurrentEntry->iAudioPlugin > _AudioPluginMissing)
			pCurrentEntry->iAudioPlugin = _AudioPluginMissing;
	}
	
	
	
	else if (szLine.find("Controller Config=") == 0)
	{
		// theres no way that it can be shorter than 220 characters
		if (szLine.size() >= 220)
		{
			for (int i = 0; i < 72; i++)
			{
				pCurrentEntry->pbyControllerConfig[i] 
					= static_cast<byte>(strtoul(szLine.substr(18 + (i*3), 2).c_str(), NULL, 16));
			}
		}
	}
	else if (szLine.find("1964 Dyna Mem=") == 0)
	{
		stringstream conv(szLine.substr(14));
		conv >> pCurrentEntry->dw1964DynaMem;

		if (pCurrentEntry->dw1964DynaMem > maxDynaMem)
			pCurrentEntry->dw1964DynaMem = maxDynaMem;
	}
	else if (szLine.find("1964 Paging Mem=") == 0)
	{
		stringstream conv(szLine.substr(16));
		conv >> pCurrentEntry->dw1964PagingMem;

		if (pCurrentEntry->dw1964PagingMem > maxPagingMem)
			pCurrentEntry->dw1964PagingMem = maxPagingMem;
	}
	else if (szLine.find("PJ64 Dyna Mem=") == 0)
	{
		stringstream conv(szLine.substr(14));
		conv >> pCurrentEntry->dwPJ64DynaMem;

		if (pCurrentEntry->dwPJ64DynaMem > maxDynaMem)
			pCurrentEntry->dwPJ64DynaMem = maxDynaMem;
	}
	else if (szLine.find("PJ64 Paging Mem=") == 0)
	{
		stringstream conv(szLine.substr(16));
		conv >> pCurrentEntry->dwPJ64PagingMem;

		if (pCurrentEntry->dwPJ64PagingMem > maxPagingMem)
			pCurrentEntry->dwPJ64PagingMem = maxPagingMem;
	}
	else if (szLine.find("Use LLE RSP=") == 0)
	{
		int val;
		stringstream conv(szLine.substr(12));
		conv >> val;

		pCurrentEntry->bUseLLERSP = (val != 0);
	}
	else if (szLine.find("Use Basic Audio=") == 0)
	{
		int val;
		stringstream conv(szLine.substr(16));
		conv >> val;

		pCurrentEntry->bUseBasicAudio = (val != 0);
	}
	
	//Ez0n3 - reinstate max video mem
	else if (szLine.find("Max Video Mem=") == 0)
	{
		stringstream conv(szLine.substr(14));
		conv >> pCurrentEntry->dwMaxVideoMem;

		if (pCurrentEntry->dwMaxVideoMem > 10)
			pCurrentEntry->dwMaxVideoMem = 10;
	}
	
}

RomIniEntry *IniFile::CreateRomEntry(dword crc1, dword crc2, byte country)
{
	// create a new ini entry struct for this entry 
	// and copy the default ini entry into it
	RomIniEntry *newEntry = new RomIniEntry;
	memcpy(newEntry, &m_defaultEntry, sizeof(RomIniEntry));
	
	newEntry->dwCrc1 = crc1;
	newEntry->dwCrc2 = crc2;
	newEntry->byCountry = country;
	newEntry->pNextEntry = NULL;

	// place the new entry in the table according to the first 16
	// bits of its crc1. if the entry is already used, iterate
	// until the end of the chain and place the new entry there.
	word tableLocation = static_cast<word>(crc1 >> 16);

	if (m_entryTable[tableLocation] == NULL)
	{
		m_entryTable[tableLocation] = newEntry;
	}
	else
	{
		RomIniEntry *tmpEntry = m_entryTable[tableLocation];

		// keep looping till we reach the end of the chain
		while (tmpEntry->pNextEntry != NULL)
		{					
			tmpEntry = tmpEntry->pNextEntry;
		}
		
		tmpEntry->pNextEntry = newEntry;
	}

	return newEntry;
}

RomIniEntry *IniFile::GetRomEntry(dword crc1, dword crc2, byte country)
{
	// look up the table using the first 16 bits of crc1
	word tableLocation = static_cast<word>(crc1 >> 16);

	RomIniEntry *curEntry = m_entryTable[tableLocation];

	// search through the chain
	while (curEntry != NULL)
	{
		if (curEntry->dwCrc1 == crc1 &&
			curEntry->dwCrc2 == crc2 &&
			curEntry->byCountry == country)
		{
			// a match was found
			return curEntry;
		}

		curEntry = curEntry->pNextEntry;
	}

	// no entry was found that matches
	return NULL;
}

RomIniEntry *IniFile::GetDefaultRomEntry()
{
	return &m_defaultEntry;
}

string IniFile::GetRomPath()
{
	return m_szRomPath;
}

void IniFile::SetRomPath(const string &romPath)
{
	m_szRomPath = romPath;
}

string IniFile::GetMediaPath()
{
	return m_szMediaPath;
}

void IniFile::SetMediaPath(const string &mediaPath)
{
	m_szMediaPath = mediaPath;
}

