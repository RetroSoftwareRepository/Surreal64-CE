
#pragma once

#define uint8  unsigned __int8
#define uint16 unsigned __int16
#define uint32 unsigned __int32
#define uint64 unsigned __int64

BOOL ByteSwap2(uint32 Size, uint8 *Image, uint8 bswaptype);
void CloseROM(void);
void SwapRomName(uint8 *name);
void SwapRomHeader(uint8 *romheader);
BOOL InitVirtualRomData(char *rompath);
void InitPageAndFrameTables();
void LoadVirtualRomData();
uint32 ReadUWORDFromROM(uint32 location);

__int32 ReadSWORDFromROM(uint32 location);
__int32 ReadSWORDFromROM_EAXOnly(uint32 location);
uint8 ReadUBYTEFromROM(uint32 location);
BOOL CheckIfInRom(uint32 location);


// Ez0n3 - old method of rom paging
#define RP_PAGE_SIZE		0x10000		// 64k pages
// freakdave - new method of rom paging
//#define RP_PAGE_SIZE		0x40000		// 64k pages


extern DWORD g_dwNumFrames;

typedef struct
{
	LARGE_INTEGER	lastUsed;
	uint32			pageNum;
} Frame;

extern Frame *g_frameTable;
extern uint8 *g_memory;