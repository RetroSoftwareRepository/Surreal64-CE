#include "RomList.h"
#include "Util.h"
#include "config.h"

RomList g_romList;
extern int romcounter;

bool RLessThan(Rom *elem1, Rom *elem2)
{
	return (elem1->GetProperName() < elem2->GetProperName());
}

RomList::RomList(void)
{
	m_romListMode = All;
	m_iBaseIndex = 0;
	m_bLoaded = false;
}

RomList::~RomList(void)
{
	Destroy();
}

void RomList::Load()
{
	ifstream cacheFile;

	cacheFile.open("T:\\RomlistCache.dat");

	// try and open the cache file, if it doesnt exist, generate the rom list
	if (!cacheFile.is_open())
	{
		Build();
		Save();
	}
	else
	{
		romcounter =0;
		while (!cacheFile.eof())
		{
			string szCrc1;
			string szCrc2;
			string szCountry;
			string szFilename;
			string szBoxArtFilename;
			string szRomSize;
			string szComments;

			dword crc1;						
			dword crc2;			
			dword country;
			dword romSize;
			dword comments;

			getline(cacheFile, szCrc1);
			getline(cacheFile, szCrc2);
			getline(cacheFile, szCountry);
			getline(cacheFile, szFilename);
			getline(cacheFile, szBoxArtFilename);
			getline(cacheFile, szRomSize);
			getline(cacheFile, szComments);

			if (Trim(szCrc1).length() == 0 ||
				Trim(szCrc2).length() == 0 ||
				Trim(szCountry).length() == 0)
			{
				continue;
			}

			Rom *rom = new Rom();

			// convert the crc1 string to a dword
			{
				stringstream conv(szCrc1);
				conv >> crc1;
			}
			// convert the crc2 string to a dword
			{
				stringstream conv(szCrc2);
				conv >> crc2;
			}
			// convert the country string to a dword
			{
				stringstream conv(szCountry);
				conv >> country;
			}
			// convert the country string to a dword
			{
				stringstream conv(szRomSize);
				conv >> romSize;
			}
			// convert the country string to a dword
			{
				stringstream conv(szComments);
				conv >> comments;
			}

			bool bSuccess = rom->LoadFromCache(crc1, crc2, static_cast<byte>(country),
											   szFilename, szBoxArtFilename, romSize, szComments);

			if (bSuccess)
				m_romList.push_back(rom);
			else
				delete rom;
			romcounter++;
		}
	
		cacheFile.close();
	}
}

void RomList::Save()
{
	vector<Rom *>::iterator i;
	ofstream cacheFile;

	// open/overwrite the rom cache
	cacheFile.open("T:\\RomlistCache.dat");

	for (i = m_romList.begin(); i != m_romList.end(); i++)
	{
		Rom *rom = *i;

		cacheFile << rom->GetCrc1() << endl;
		cacheFile << rom->GetCrc2() << endl;
		cacheFile << static_cast<dword>(rom->GetCountry()) << endl;
		cacheFile << rom->GetFileName() << endl;
		cacheFile << rom->GetBoxArtFilename() << endl;
		cacheFile << rom->GetRomSize() << endl;
		cacheFile << rom->GetComments() << endl;
	}

	cacheFile.close();

	ofstream favFile;
	favFile.open("T:\\Favourites.dat");

	for (i = m_favList.begin(); i != m_favList.end(); i++)
	{
		Rom *rom = *i;

		favFile << rom->GetCrc1() << endl;
		favFile << rom->GetCrc2() << endl;
		favFile << static_cast<dword>(rom->GetCountry()) << endl;
	}

	favFile.close();
}

void RomList::Refresh()
{
	Destroy();
	DeleteFile("T:\\RomlistCache.dat");
	DeleteFile("T:\\RomlistState.dat");
	Load();
}

bool RomList::IsLoaded()
{
	return m_bLoaded;
}

void RomList::SetRomListMode(int mode)
{
	m_iBaseIndex = 0;
	m_romListMode = mode;
}

int RomList::GetRomListMode()
{
	return m_romListMode;	
}

void RomList::AddRomToList(Rom *rom, int mode)
{
	vector<Rom *> *pList;

	switch (mode)
	{
		case All:
			pList = &m_romList;
			break;
		case Favourite:
			pList = &m_favList;
			break;
		default:
			return;
	}

	// look to see if the rom is already in the list, we dont want duplicates
	for (int i = 0; i < static_cast<int>(pList->size()); i++)
	{
		if (rom == (*pList)[i])
			return;
	}

	pList->push_back(rom);
	sort(pList->begin(), pList->end(), RLessThan);
}

void RomList::RemoveRomFromList(Rom *rom, int mode)
{
	vector<Rom *> *pList;

	switch (mode)
	{
		case All:
			pList = &m_romList;
			break;
		case Favourite:
			pList = &m_favList;
			break;
		default:
			return;
	}

	vector<Rom *>::iterator i;

	// look to see if the rom is already in the list, we dont want duplicates
	for (i = pList->begin(); i != pList->end(); i++)
	{
		if (rom == *i)
		{
			pList->erase(i);
			return;
		}
	}
}

int RomList::GetBaseIndex()
{
	if (m_iBaseIndex > GetRomListSize() - 1)
		m_iBaseIndex = GetRomListSize() - 1;
	if (m_iBaseIndex < 0)
		m_iBaseIndex = 0;

	return m_iBaseIndex;
}

void RomList::SetBaseIndex(int index)
{
	if (index > GetRomListSize() - 1)
		index = GetRomListSize() - 1;
	if (index < 0)
		index = 0;

	m_iBaseIndex = index;
}

int RomList::GetRomListSize()
{
	switch (m_romListMode)
	{
		case All:
			return m_romList.size();
		case Favourite:
			return m_favList.size();
	}

	return 0;
}

Rom *RomList::GetRomAt(int index)
{
	switch (m_romListMode)
	{
		case All:
			return m_romList[index];
		case Favourite:
			return m_favList[index];
	}

	return 0;
}

int RomList::FindRom(Rom *rom, int mode)
{
	vector<Rom *> *pList;

	switch (mode)
	{
		case All:
			pList = &m_romList;
			break;
		case Favourite:
			pList = &m_favList;
			break;
		default:
			return -1;
	}

	for (int i = 0; i < static_cast<int>(pList->size()); i++)
	{
		if (rom == (*pList)[i])
			return i;
	}
	
	return -1;
}

void RomList::Build()
{
	romcounter=0;
	WIN32_FIND_DATA fd;

	HANDLE hFF = FindFirstFile((g_iniFile.GetRomPath() + "\\*.*").c_str(), &fd);

	do
	{
		char ext[_MAX_EXT];

		// get the filename extension
		_splitpath((g_iniFile.GetRomPath() + fd.cFileName).c_str(), 
				NULL, NULL, NULL, ext);

		if (
			stricmp(ext, ".rom") == 0
		||	stricmp(ext, ".v64") == 0
		||	stricmp(ext, ".z64") == 0
		||	stricmp(ext, ".usa") == 0
		||	stricmp(ext, ".n64") == 0
		||	stricmp(ext, ".bin") == 0
		||	stricmp(ext, ".zip") == 0
		||	stricmp(ext, ".j64") == 0
		||	stricmp(ext, ".pal") == 0
			)
		{
			Rom *rom = new Rom();
			bool bSuccess = rom->Load(g_iniFile.GetRomPath() + fd.cFileName);

			if (bSuccess){
				m_romList.push_back(rom);
				romcounter++;}
			else
				delete rom;
		}
	} while (FindNextFile(hFF, &fd));

	sort(m_romList.begin(), m_romList.end(), RLessThan);
    ConfigAppSave();

	m_bLoaded = true;
}

void RomList::Destroy()
{
	m_bLoaded = false;
	m_iBaseIndex = 0;

	vector<Rom *>::iterator i;

	for (i = m_romList.begin(); i != m_romList.end(); i++)
	{
		delete *i;
	}

	m_romList.clear();
	m_favList.clear();
}




