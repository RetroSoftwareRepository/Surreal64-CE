#pragma once

#include "Launcher.h"

struct BoxArtEntry
{
	dword dwCrc1;
	char szPath[_MAX_PATH];

	BoxArtEntry *pNextEntry;
};

class BoxArtTable
{
public:
	BoxArtTable(void);
	~BoxArtTable(void);

	void Build();
	string GetBoxArtFilename(dword crc1);

private:
	BoxArtEntry *CreateBoxArtEntry(dword crc1, const string &pathName);

private:
	BoxArtEntry **m_entryTable;
};

extern BoxArtTable g_boxArtTable;