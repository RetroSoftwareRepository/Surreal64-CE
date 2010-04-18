#pragma once

#include "Launcher.h"
#include "Rom.h"

enum RomListMode
{
	All,
	Favourite
};

class RomList
{
public:
	RomList(void);
	virtual ~RomList(void);

	void Load();
	void Save();
	void Refresh();

	bool IsLoaded();

	void SetRomListMode(int mode);
	int GetRomListMode();

	void AddRomToList(Rom *rom, int mode);
	void RemoveRomFromList(Rom *rom, int mode);

	int GetBaseIndex();
	void SetBaseIndex(int index);

	int GetRomListSize();

	Rom *GetRomAt(int index);
	int FindRom(Rom *rom, int mode);

	int m_iBaseIndex;

private:
	void Build();
	void Destroy();

public:
	vector<Rom *> m_romList;
	vector<Rom *> m_favList;

	
	bool m_bLoaded;

	int m_romListMode;
};

extern RomList g_romList;