#include <xtl.h>
#include <stdio.h>
#include "main.h"
#include "stubs.h"
#include "cpu.h"

DWORD g_dwNumFrames = 64; // default 4mb of memory

//weinerschnitzel - check memory size for paging method
extern int	RAM_IS_128 = 0; //assume we are 64mb
extern BOOL PhysRam128(){
  MEMORYSTATUS memStatus;
  GlobalMemoryStatus( &memStatus );
  if( memStatus.dwTotalPhys < (100 * 1024 * 1024) ){ 
	  RAM_IS_128 = 0;
	  return FALSE;
  }else{
	  RAM_IS_128 = 1;
	  return TRUE;
  }
}



//---------------Doesnt work for 128mb-------------------
//weinerschnitzel - we'll keep this for 64mb users
// Ez0n3 - old method of rom paging
#define PAGE_NOT_IN_MEMORY	(g_dwNumFrames+1)
#define FRAME_FREE			0xFFFFFFFF
#define NEVER				0x0
#define NONE_FOUND			0xFFFFFFFF	

Frame *g_frameTable;


// Memory
//uint8 g_memory[RP_PAGE_SIZE_N*64];
//--------------------------------------------
uint8 *g_memory;

#define SHIFTER1_READ	2				/* Shifts off insignificant bits from memory_read_functions array size. The
										 * significant bits are 0xFFFC0000 (14 significant bits) because Check_LW needs
										 * to check SPREG_1, which is at 0xA4040000. So we only need 14bit instead of
										 * 16bit. */
#define SHIFTER1_WRITE	0				/* Shifts off insignificant bits from memory write functions array size. Set to
										 * zero because of protected memory in 0x1000 blocks. = All bits are
										 * significant. */

#define SHIFTER2_READ	(16 + SHIFTER1_READ)
#define SHIFTER2_WRITE	(12 + SHIFTER1_WRITE)


// Page table
// is your standard page table. each entry in the array contains a number indicating
// the frame that stores its page in memory

//---------------Doesnt work for 128mb-------------------
//weinerschnitsel - need to be declared for 64mb
// Ez0n3 - old method of rom pagin
uint8 *g_pageTable;
uint32 g_pageTableSize;
uint32 g_pageFunctionHits;
uint32 g_memFunctionHits;
uint32 g_pageHits;
uint32 g_pageMisses;
uint32 g_dynaHits;
//



// Rom file
// the original rom file is loaded, byte swapped and written to a temporary file pointed
// to below. the file is open for the duration of play and seeked and read when needed.

FILE	*g_temporaryRomFile = NULL;
char    g_temporaryRomPath[260];


BOOL	Is_Reading_Rom_File = FALSE;
BOOL	To_Stop_Reading_Rom_File = FALSE;

DWORD RomFileSize;
char RomHeader[64];
char RomName[MAX_PATH+1];

BOOL EnableMenuItem(HMENU hMenu, UINT uIDEnableItem, UINT uEnable)
{

	return TRUE;

}

HMENU GetSubMenu(HMENU hMenu, int nPos)
{
	return (HMENU)1;

}

BOOL TerminateThread(HANDLE hThread, DWORD dwExitCode)
{
	ExitThread(dwExitCode);
}

LRESULT SendMessage(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{

	return 0;
}

BOOL SetMenuItemInfo(HMENU a, UINT b, BOOL c, LPMENUITEMINFO d)
{
	return TRUE;

}

BOOL PeekMessage(PMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax, UINT wRemoveMsg)
{

	return TRUE;

}

BOOL MessageBox(HWND hWnd, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType)
{

	return 0;

}

 

int GetWindowText(HWND hWnd, LPTSTR lpString, int nMaxCount)
{
	return 1;
}

HMENU GetMenu(HWND hWnd)
{

	return 0;

}

BOOL SetWindowText(HWND hWnd, LPCTSTR lpString)
{
	return TRUE;
}

BOOL ByteSwap2(uint32 Size, uint8 *Image, uint8 bswaptype)
{
	/*~~~~~~*/
	uint32	k;
	/*~~~~~~*/

	switch (bswaptype)
	{
		case 1:
		{
			for(k = 0; k < Size; k += 4)
			{
				/* Converts 40123780 to 0x80371240 */
				_asm
				{
					mov edx, Image
					add edx, k
					mov eax, dword ptr[edx]
					bswap eax
					mov dword ptr[edx], eax
				}
			}

			return TRUE;
		}
		case 2:
		{
			/* Converts 0x12408037 to 0x80371240 */
			k = Size;
			_asm
			{
				mov edx, Image
				mov ebx, dword ptr[k]
				jmp _LABEL3
			}

			_LABEL2:
			_asm
			{
				sub ebx, 8
				test ebx, ebx
				jz _LABELEXIT
			}

			_LABEL3:
			_asm
			{
				/* Yup i copied this asm routine twice..to cut down on the looping by 50% */
				mov eax, dword ptr[edx]
				mov ecx, eax
				shr ecx, 0x10
				shl eax, 0x10
				or ecx, eax
				mov dword ptr[edx], ecx
				add edx, 4

				mov eax, dword ptr[edx]
				mov ecx, eax
				shr ecx, 0x10
				shl eax, 0x10
				or ecx, eax
				mov dword ptr[edx], ecx
				add edx, 4
				jmp _LABEL2
			}

			_LABELEXIT: ;
			return TRUE;
		}
		case 0:
			return TRUE;
		default:
			return FALSE;
	}
}


BOOL InitVirtualRomData(char *rompath)
{
	
	//---------------Doesnt work for 128mb-------------------
	//weinerschnitzel - enable this in 64mb condition
	// Ez0n3 - old method of rom paging
	if(RAM_IS_128 == 0){
	//FILE *temporaryFile = NULL; //lets be consistent here...

	strcpy(g_temporaryRomPath, "Z:\\TemporaryRom.dat");

	// open the temporary file again for reading
	g_temporaryRomFile = fopen(g_temporaryRomPath, "rb");

	// get the size of the original rom file
	rewind(g_temporaryRomFile);
	fseek(g_temporaryRomFile, 0, SEEK_END);
	RomFileSize = ftell(g_temporaryRomFile);

	// read in the rom header
	fseek(g_temporaryRomFile, 0, SEEK_SET);
	fread((uint8 *) &RomHeader, sizeof(uint8), 0x40, g_temporaryRomFile);

	// read in rom name
	fseek(g_temporaryRomFile, 0x20, SEEK_SET);
	fread(RomName, sizeof(uint8), 20, g_temporaryRomFile);

	fclose(g_temporaryRomFile);

	return TRUE;
	}
	
// freakdave - new method of rom paging
//weinerschnitzel - this needs to be in 128mb condition
	if(RAM_IS_128 == 1){
		FILE *temporaryFile = NULL;

		strcpy(g_temporaryRomPath, "Z:\\TemporaryRom.dat");

		// open the temporary file again for reading
		temporaryFile = fopen(g_temporaryRomPath, "rb");

		// get the size of the original rom file
		rewind(temporaryFile);
		fseek(temporaryFile, 0, SEEK_END);
		RomFileSize = ftell(temporaryFile);

		// read in the rom header
		fseek(temporaryFile, 0, SEEK_SET);
		fread((uint8 *) &RomHeader, sizeof(uint8), 0x40, temporaryFile);

		
		// read in rom name
		fseek(temporaryFile, 0x20, SEEK_SET);
		fread(RomName, sizeof(uint8), 20, temporaryFile);

		fclose(temporaryFile);

		return TRUE;
	}
	
}

void CloseROM(void)
{

}

void SwapRomName(uint8 *name)
{
	/*~~~~~~*/
	int		i;
	uint8	c;
	/*~~~~~~*/

	for(i = 0; i < 20; i += 4)
	{
		c = name[i];
		name[i] = name[i + 3];
		name[i + 3] = c;

		c = name[i + 2];
		name[i + 2] = name[i + 1];
		name[i + 1] = c;
	}

	for(i = 19; i >= 0; i--)
	{
		if(name[i] != ' ') break;
	}

	name[i + 1] = '\0';

	/*
	 * for( ; i>=0; i-- ) £
	 * { £
	 * if( name[i] == ':' ) £
	 * name[i] = '-'; £
	 * }
	 */
}

void SwapRomHeader(uint8 *romheader)
{
	/*~~~~~~*/
	int		i;
	uint8	c;
	/*~~~~~~*/

	for(i = 0; i < 0x40; i += 4)
	{
		c = romheader[i];
		romheader[i] = romheader[i + 3];
		romheader[i + 3] = c;

		c = romheader[i + 2];
		romheader[i + 2] = romheader[i + 1];
		romheader[i + 1] = c;
	}
}


void LoadVirtualRomData()
{
	g_temporaryRomFile = fopen(g_temporaryRomPath, "rb");
}

void CloseVirtualRomData()
{
	if (g_temporaryRomFile != NULL)
	{
		fclose(g_temporaryRomFile);
		g_temporaryRomFile = NULL;

		
		//---------------Doesnt work for 128mb-------------------
		// weinerschnitzel - enable in 64mb condition
		// Ez0n3 - old method of rom paging
		if(RAM_IS_128 == 0){
			free(g_pageTable);
		}
		//
		
	}
}


// freakdave - new method of rom paging
// weinerschnitzel - declare these for 128mb
static BOOL indic[256];
static uint8 adress[256];
static uint8 adfix[256];
uint32 pagesize;  // 256*1024 -> uint32
uint16 nombreframes; // 256 max -> uint8 = 255 max -> uint16
static uint8 plusgrand;
uint32 poubelle = 0;
BOOL pause = 0;

void InitPageAndFrameTables()
{
	//---------------Doesnt work for 128mb-------------------
	// weinerschnitzel - 64 mb condition
	// Ez0n3 - old method of rom paging
	if(RAM_IS_128 == 0){
	uint32 i;

	// get the size of the page table
	g_pageTableSize = RomFileSize / RP_PAGE_SIZE_O;

	// allocate memory for the page table
	g_pageTable = (uint8 *)malloc(g_pageTableSize);

	// initialize it
	for (i = 0; i < g_pageTableSize; i++)
	{
		g_pageTable[i] = PAGE_NOT_IN_MEMORY;
	}

	// initialize the frame table
	for (i = 0; i < g_dwNumFrames; i++)
	{
		g_frameTable[i].pageNum				= FRAME_FREE;
		g_frameTable[i].lastUsed.QuadPart	= NEVER;
	}
	}
	//


// freakdave - new method of rom paging
//weinerschnitzel - 128mb condition
if(RAM_IS_128 == 1){

	uint16 i;
	pagesize = 256*1024;
	nombreframes = g_dwNumFrames;
	g_memory = (uint8 *) malloc ( sizeof(*g_memory)  *  RP_PAGE_SIZE_N * nombreframes );
//	g_temp = CreateFile(g_PageFileName,GENERIC_READ,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_READONLY,NULL);

	// all indication frame to false
	for (i=0;i<256;i++){ 
    indic[i]=0;
	}
	// copy first frames
	for (i=0;i<nombreframes;i++){  
	//SetFilePointer(g_temporaryRomFile,(i*pagesize),NULL,FILE_BEGIN);
	//ReadFile(g_temporaryRomFile,(g_memory+(i*pagesize)),(pagesize),&dwread,NULL);
	fseek(g_temporaryRomFile, (i*pagesize), SEEK_SET);
	fread((g_memory+(i*pagesize)), sizeof(*g_memory), (pagesize), g_temporaryRomFile);

    indic[i] = 1;
	adress[i] = i;
	adfix[i] = i;
	} 
	plusgrand = nombreframes-1;
	}

}

uint32 ReadUWORDFromROM(uint32 location)
{
	//---------------Doesnt work for 128mb-------------------
	//weinerschnitzel - 64mb condition
	// Ez0n3 - old method of rom paging
	if(RAM_IS_128 == 0){
	uint32 i;
	uint32 pageNumberOfLocation;
	uint32 offsetFromPage;

	g_pageFunctionHits++;

	location &= 0x7ffffff;

	if (location > RomFileSize)
		return 0;

	// calculate what page the location is in and how far from the start of
	// the page the location is
	pageNumberOfLocation = location / RP_PAGE_SIZE_O;
	offsetFromPage = location - (pageNumberOfLocation*RP_PAGE_SIZE_O);

	// if the page required is in memory, use it
	if (g_pageTable[pageNumberOfLocation] != PAGE_NOT_IN_MEMORY)
	{
		g_pageHits++;

		if (offsetFromPage <= (RP_PAGE_SIZE_O-sizeof(uint32)))
		{
			return *(uint32 *)(g_memory + (g_pageTable[pageNumberOfLocation]*RP_PAGE_SIZE_O) + offsetFromPage);
		}
		else // if the location isn't a multiple of 4 this may happen? hopefully it doesn't ever
		{
			g_memFunctionHits++;
			// fix me
			return *(uint32 *)(g_memory + (g_pageTable[pageNumberOfLocation]*RP_PAGE_SIZE_O) + offsetFromPage);
		}
	}
	else
	{
		uint32  freeFrame		 = NONE_FOUND;
		__int64 oldestFrameTime	 = 0x7FFFFFFFFFFFFFFF;
		uint32  oldestFrameIndex = NONE_FOUND;

		g_pageMisses++;

		// find an empty frame, or the oldest frame
		for (i = 0; i < g_dwNumFrames; i++)
		{
			if (g_frameTable[i].pageNum == FRAME_FREE)
			{
				freeFrame = i;
				break;
			}
			else if (g_frameTable[i].lastUsed.QuadPart < oldestFrameTime)
			{
				oldestFrameTime  = g_frameTable[i].lastUsed.QuadPart;
				oldestFrameIndex = i;
			}
		}

		{
			uint32 frameToUse = 0;
			LARGE_INTEGER currentTime;
		
			if (freeFrame == NONE_FOUND)
			{
				frameToUse = oldestFrameIndex;
				
				// set the old page as now not being in memory
				g_pageTable[g_frameTable[frameToUse].pageNum] = PAGE_NOT_IN_MEMORY;
			}
			else
			{
				frameToUse = freeFrame;
			}

			QueryPerformanceCounter(&currentTime);

			// update the frame table
			g_frameTable[frameToUse].pageNum = pageNumberOfLocation;
			g_frameTable[frameToUse].lastUsed = currentTime;

			// seek to the start of the page in the temporary file
			fseek(g_temporaryRomFile, pageNumberOfLocation*RP_PAGE_SIZE_O, SEEK_SET);

			// read in the page
			fread(&g_memory[frameToUse*RP_PAGE_SIZE_O], sizeof(char), RP_PAGE_SIZE_O, g_temporaryRomFile);

			// update the page table
			g_pageTable[pageNumberOfLocation] = frameToUse;

			return *(uint32 *)(g_memory + (g_pageTable[pageNumberOfLocation]*RP_PAGE_SIZE_O) + offsetFromPage);
		}
	}
	}
	//end ram check

// freakdave - new method of rom paging
//weinerschnitzel - 128mb condition
if(RAM_IS_128 == 1){

	uint32 location2 = location & 0x7ffffff;

	uint8 numero = (location2/pagesize);
	uint32 adresstemp = (location2 - ((numero) *(pagesize)));
// see if it is on RAM or in the file
	if (indic[numero] == 0) {
		// adress of frame to change (FILO stack)
		adress[numero] = plusgrand;
		// change indication and adressfix
		indic[adfix[plusgrand]] = 0;
		indic[numero]=1;
		adfix[plusgrand]=numero;
	    // copy
		//SetFilePointer(g_temporaryRomFile,(numero*pagesize),NULL,FILE_BEGIN);
	    //ReadFile(g_temporaryRomFile,(g_memory+(plusgrand*pagesize)),(pagesize),&dwread,NULL);
		fseek(g_temporaryRomFile, (numero*pagesize), SEEK_SET);
	    fread((g_memory+(plusgrand*pagesize)), sizeof(*g_memory), (pagesize), g_temporaryRomFile);
        // action sur plusgrand - boucle décrémentation
		plusgrand--; if(plusgrand > (nombreframes-1)) plusgrand = nombreframes-1;
    //
	}
	// return information
	return *(uint32*) (g_memory + (adress[numero] * pagesize) + adresstemp);
}
	
}

uint16 ReadUHALFFromROM(uint32 location)
{

	//---------------Doesnt work for 128mb-------------------
	//weinerschnitzel - 64mb condition
	// Ez0n3 - old method of rom paging
	if(RAM_IS_128 == 0){
	uint32 i;
	uint32 pageNumberOfLocation;
	uint32 offsetFromPage;

	g_pageFunctionHits++;

	location &= 0x7ffffff;

	if (location > RomFileSize)
		return 0;

	// calculate what page the location is in and how far from the start of
	// the page the location is
	pageNumberOfLocation = location / RP_PAGE_SIZE_O;
	offsetFromPage = location - (pageNumberOfLocation*RP_PAGE_SIZE_O);

	// if the page required is in memory, use it
	if (g_pageTable[pageNumberOfLocation] != PAGE_NOT_IN_MEMORY)
	{
		g_pageHits++;

		if (offsetFromPage <= (RP_PAGE_SIZE_O-sizeof(uint16)))
		{
			return *(uint16 *)(g_memory + (g_pageTable[pageNumberOfLocation]*RP_PAGE_SIZE_O) + offsetFromPage);
		}
		else // if the location isn't a multiple of 4 this may happen? hopefully it doesn't ever
		{
			g_memFunctionHits++;
			// fix me
			return *(uint16 *)(g_memory + (g_pageTable[pageNumberOfLocation]*RP_PAGE_SIZE_O) + offsetFromPage);
		}
	}
	else
	{
		uint32  freeFrame		 = NONE_FOUND;
		__int64 oldestFrameTime	 = 0x7FFFFFFFFFFFFFFF;
		uint32  oldestFrameIndex = NONE_FOUND;

		g_pageMisses++;

		// find an empty frame, or the oldest frame
		for (i = 0; i < g_dwNumFrames; i++)
		{
			if (g_frameTable[i].pageNum == FRAME_FREE)
			{
				freeFrame = i;
				break;
			}
			else if (g_frameTable[i].lastUsed.QuadPart < oldestFrameTime)
			{
				oldestFrameTime  = g_frameTable[i].lastUsed.QuadPart;
				oldestFrameIndex = i;
			}
		}

		{
			uint32 frameToUse = 0;
			LARGE_INTEGER currentTime;
		
			if (freeFrame == NONE_FOUND)
			{
				frameToUse = oldestFrameIndex;
				
				// set the old page as now not being in memory
				g_pageTable[g_frameTable[frameToUse].pageNum] = PAGE_NOT_IN_MEMORY;
			}
			else
			{
				frameToUse = freeFrame;
			}

			QueryPerformanceCounter(&currentTime);

			// update the frame table
			g_frameTable[frameToUse].pageNum = pageNumberOfLocation;
			g_frameTable[frameToUse].lastUsed = currentTime;

			// seek to the start of the page in the temporary file
			fseek(g_temporaryRomFile, pageNumberOfLocation*RP_PAGE_SIZE_O, SEEK_SET);

			// read in the page
			fread(&g_memory[frameToUse*RP_PAGE_SIZE_O], sizeof(char), RP_PAGE_SIZE_O, g_temporaryRomFile);

			// update the page table
			g_pageTable[pageNumberOfLocation] = frameToUse;

			return *(uint16 *)(g_memory + (g_pageTable[pageNumberOfLocation]*RP_PAGE_SIZE_O) + offsetFromPage);
		}
	}
}

// freakdave - new method of rom paging
// weinerschnitzel - 128mb condition
if(RAM_IS_128 == 1){

	uint32 location2 = location & 0x7ffffff;

	uint8 numero = (location2/pagesize);
	uint32 adresstemp = (location2 - ((numero) *(pagesize)));
// see if it is on RAM or in the file
	if (indic[numero] == 0) {
		// adress of frame to change (FILO stack)
		adress[numero] = plusgrand;
		// change indication and adressfix
		indic[adfix[plusgrand]] = 0;
		indic[numero]=1;
		adfix[plusgrand]=numero;
	    // copy
		fseek(g_temporaryRomFile, (numero*pagesize), SEEK_SET);
	    fread((g_memory+(plusgrand*pagesize)), sizeof(*g_memory), (pagesize), g_temporaryRomFile);
        // action sur plusgrand - boucle décrémentation
		plusgrand--; if(plusgrand > (nombreframes-1)) plusgrand = nombreframes-1;
    //
	}
	// return information
	return *(uint16*) (g_memory + (adress[numero] * pagesize) + adresstemp);
}
	
}

uint8 ReadUBYTEFromROM(uint32 location)
{
	
	//---------------Doesnt work for 128mb-------------------
	//weinerschnitzel - 64mb condition
	// Ez0n3 - old method of rom paging
	if(RAM_IS_128 == 0){
	uint32 i;
	uint32 pageNumberOfLocation;
	uint32 offsetFromPage;

	g_pageFunctionHits++;

	location &= 0x7ffffff;

	if (location > RomFileSize)
		return 0;

	// calculate what page the location is in and how far from the start of
	// the page the location is
	pageNumberOfLocation = location / RP_PAGE_SIZE_O;
	offsetFromPage = location - (pageNumberOfLocation*RP_PAGE_SIZE_O);

	// if the page required is in memory, use it
	if (g_pageTable[pageNumberOfLocation] != PAGE_NOT_IN_MEMORY)
	{
		g_pageHits++;

		if (offsetFromPage <= (RP_PAGE_SIZE_O-sizeof(uint8)))
		{
			return *(uint8 *)(g_memory + (g_pageTable[pageNumberOfLocation]*RP_PAGE_SIZE_O) + offsetFromPage);
		}
		else // if the location isn't a multiple of 4 this may happen? hopefully it doesn't ever
		{
			g_memFunctionHits++;
			// fix me
			return *(uint8 *)(g_memory + (g_pageTable[pageNumberOfLocation]*RP_PAGE_SIZE_O) + offsetFromPage);
		}
	}
	else
	{
		uint32  freeFrame		 = NONE_FOUND;
		__int64 oldestFrameTime	 = 0x7FFFFFFFFFFFFFFF;
		uint32  oldestFrameIndex = NONE_FOUND;

		g_pageMisses++;

		// find an empty frame, or the oldest frame
		for (i = 0; i < g_dwNumFrames; i++)
		{
			if (g_frameTable[i].pageNum == FRAME_FREE)
			{
				freeFrame = i;
				break;
			}
			else if (g_frameTable[i].lastUsed.QuadPart < oldestFrameTime)
			{
				oldestFrameTime  = g_frameTable[i].lastUsed.QuadPart;
				oldestFrameIndex = i;
			}
		}

		{
			uint32 frameToUse = 0;
			LARGE_INTEGER currentTime;
		
			if (freeFrame == NONE_FOUND)
			{
				frameToUse = oldestFrameIndex;
				
				// set the old page as now not being in memory
				g_pageTable[g_frameTable[frameToUse].pageNum] = PAGE_NOT_IN_MEMORY;
			}
			else
			{
				frameToUse = freeFrame;
			}

			QueryPerformanceCounter(&currentTime);

			// update the frame table
			g_frameTable[frameToUse].pageNum = pageNumberOfLocation;
			g_frameTable[frameToUse].lastUsed = currentTime;

			// seek to the start of the page in the temporary file
			fseek(g_temporaryRomFile, pageNumberOfLocation*RP_PAGE_SIZE_O, SEEK_SET);

			// read in the page
			fread(&g_memory[frameToUse*RP_PAGE_SIZE_O], sizeof(char), RP_PAGE_SIZE_O, g_temporaryRomFile);

			// update the page table
			g_pageTable[pageNumberOfLocation] = frameToUse;

			return *(uint8 *)(g_memory + (g_pageTable[pageNumberOfLocation]*RP_PAGE_SIZE_O) + offsetFromPage);
		}
	}
}

// freakdave - new method of rom paging
//weinerschnitzel - 128mb condition
if(RAM_IS_128 == 1){

	uint32 location2 = location & 0x7ffffff;

	uint8 numero = (location2/pagesize);
	uint32 adresstemp = (location2 - ((numero) *(pagesize)));
// see if it is on RAM or in the file
	if (indic[numero] == 0) {
		// adress of frame to change (FILO stack)
		adress[numero] = plusgrand;
		// change indication and adressfix
		indic[adfix[plusgrand]] = 0;
		indic[numero]=1;
		adfix[plusgrand]=numero;
	    // copy
		fseek(g_temporaryRomFile, (numero*pagesize), SEEK_SET);
	    fread((g_memory+(plusgrand*pagesize)), sizeof(*g_memory), (pagesize), g_temporaryRomFile);
        // action sur plusgrand - boucle décrémentation
		plusgrand--; if(plusgrand > (nombreframes-1)) plusgrand = nombreframes-1;
    //
	}
	// return information
	return *(uint8*) (g_memory + (adress[numero] * pagesize) + adresstemp);

}	
}

__int32 ReadSWORDFromROM(uint32 location)
{

	//---------------Doesnt work for 128mb-------------------
	// Ez0n3 - old method of rom paging
	//weinerschnitzel - 64mb condition
	if(RAM_IS_128 == 0){
	uint32 i;
	uint32 pageNumberOfLocation;
	uint32 offsetFromPage;
	uint32 retVal;

	g_pageFunctionHits++;

	location &= 0x7ffffff;

	if (location > RomFileSize)
		return 0;

	// calculate what page the location is in and how far from the start of
	// the page the location is
	pageNumberOfLocation = location / RP_PAGE_SIZE_O;
	offsetFromPage = location - (pageNumberOfLocation*RP_PAGE_SIZE_O);

	// if the page required is in memory, use it
	if (g_pageTable[pageNumberOfLocation] != PAGE_NOT_IN_MEMORY)
	{
		g_pageHits++;

		if (offsetFromPage <= (RP_PAGE_SIZE_O-sizeof(uint16)))
		{
			retVal = *(__int32 *)(g_memory + (g_pageTable[pageNumberOfLocation]*RP_PAGE_SIZE_O) + offsetFromPage);
		}
		else // if the location isn't a multiple of 4 this may happen? hopefully it doesn't ever
		{
			g_memFunctionHits++;
			// fix me
			retVal = *(__int32 *)(g_memory + (g_pageTable[pageNumberOfLocation]*RP_PAGE_SIZE_O) + offsetFromPage);
		}
	}
	else
	{
		uint32  freeFrame		 = NONE_FOUND;
		__int64 oldestFrameTime	 = 0x7FFFFFFFFFFFFFFF;
		uint32  oldestFrameIndex = NONE_FOUND;

		g_pageMisses++;

		// find an empty frame, or the oldest frame
		for (i = 0; i < g_dwNumFrames; i++)
		{
			if (g_frameTable[i].pageNum == FRAME_FREE)
			{
				freeFrame = i;
				break;
			}
			else if (g_frameTable[i].lastUsed.QuadPart < oldestFrameTime)
			{
				oldestFrameTime  = g_frameTable[i].lastUsed.QuadPart;
				oldestFrameIndex = i;
			}
		}

		{
			uint32 frameToUse = 0;
			LARGE_INTEGER currentTime;
		
			if (freeFrame == NONE_FOUND)
			{
				frameToUse = oldestFrameIndex;
				
				// set the old page as now not being in memory
				g_pageTable[g_frameTable[frameToUse].pageNum] = PAGE_NOT_IN_MEMORY;
			}
			else
			{
				frameToUse = freeFrame;
			}

			QueryPerformanceCounter(&currentTime);

			// update the frame table
			g_frameTable[frameToUse].pageNum = pageNumberOfLocation;
			g_frameTable[frameToUse].lastUsed = currentTime;

			// seek to the start of the page in the temporary file
			fseek(g_temporaryRomFile, pageNumberOfLocation*RP_PAGE_SIZE_O, SEEK_SET);

			// read in the page
			fread(&g_memory[frameToUse*RP_PAGE_SIZE_O], sizeof(char), RP_PAGE_SIZE_O, g_temporaryRomFile);

			// update the page table
			g_pageTable[pageNumberOfLocation] = frameToUse;

			retVal = *(__int32 *)(g_memory + (g_pageTable[pageNumberOfLocation]*RP_PAGE_SIZE_O) + offsetFromPage);
		}
	}

	return retVal;
}

// freakdave - new method of rom paging
// weinerschnitzel 128mb condition                                                                                                                                                                                                                                                                                                           
if(RAM_IS_128 == 1){
	uint32 location2 = location & 0x7ffffff;

	uint8 numero = (location2/pagesize);
	uint32 adresstemp = (location2 - ((numero) *(pagesize)));
// see if it is on RAM or in the file
	if (indic[numero] == 0) {
		// adress of frame to change (FILO stack)
		adress[numero] = plusgrand;
		// change indication and adressfix
		indic[adfix[plusgrand]] = 0;
		indic[numero]=1;
		adfix[plusgrand]=numero;
	    // copy
		fseek(g_temporaryRomFile, (numero*pagesize), SEEK_SET);
	    fread((g_memory+(plusgrand*pagesize)), sizeof(*g_memory), (pagesize), g_temporaryRomFile);
        // action sur plusgrand - boucle décrémentation
		plusgrand--; if(plusgrand > (nombreframes-1)) plusgrand = nombreframes-1;
    //
	}
	// return information
	return *(__int32*) (g_memory + (adress[numero] * pagesize) + adresstemp);
}
	
}

BOOL CheckIfInRom(uint32 location)
{
	//return (((addr)&0x1FFFFFFF) >= MEMORY_START_ROM_IMAGE && ((addr)&0x1FFFFFFF) <= 0x14000000);
	register uint32 locShifted = location >> SHIFTER2_READ;

	return ((locShifted >= 0x2400 && locShifted <= 0x24FF) \
		 || (locShifted >= 0x2C00 && locShifted <= 0x2CFF));
}


 __int32 ReadSWORDFromROM_EAXOnly(uint32 location)
{
	{
		__int32 temp;
		__asm
		{
			pushad
			mov ecx, location
			call ReadSWORDFromROM
			mov temp, eax
			popad
			mov eax, temp
		}
	}
}

extern int SaveStateIndex;
extern CPU_ACTION CPU_Action;

void __EMU_SaveState(int index)
{
	SaveStateIndex = index;
	CPU_Action.DoSomething = TRUE;
	CPU_Action.SaveState = TRUE;
}

void __EMU_LoadState(int index)
{
	SaveStateIndex = index;
	CPU_Action.DoSomething = TRUE;
	CPU_Action.RestoreState = TRUE;
}

// GogoAckman - free up 4Mb for the menu
void ReInitVirtualDynaMemory(boolean charge)
{

	// Ez0n3 - i don't think this should free up g_dwNumFrames (PJ64PagingMem) for menu - what if it's a 128MB sys and g_dwNumFrames = entire rom??!!
	// this is how much memory it should free up for the in-game menu to popup, doesn't seem right to me that it should try to free so much up for PJ64 and not 1964 if using 128MB
	// 1964 uses loaddw1964DynaMem default 8 MB for this setting, so this should be loaddwPJ64DynaMem value instead of g_dwNumFrames
	// g_dwNumFrames = loaddw1964PagingMem default 4MB on 64MB sys and entire rom on 128MB sys!!!! that's 20-60MB decommit for menu!!??
	// loaddw1964DynaMem default = 8
	// loaddwPJ64DynaMem default = 16
	// PJ64 dyna mem is twice as much (dumps dyna mem to file to free mem for menu, on menu exit - reloads dyna mem from file)
	// have no choice but to free up 16MB by default for the menu by using dwPJ64DynaMem, but i think it's better than the alternative
	// see what happens...
	
	FILE *fp;

	if (!charge) {
		if(XBOXRecompCode != NULL){
			fp=fopen("Z:\\codetemp.dat","wb");
			fwrite(XBOXRecompCode,g_dwNormalCompileBufferSize ,sizeof(char),fp);
			VirtualFree(XBOXRecompCode, g_dwNormalCompileBufferSize, MEM_DECOMMIT);
		    fclose(fp);}
	}
	else {
		fp=fopen("Z:\\codetemp.dat","rb");
		VirtualFree(XBOXRecompCode, g_dwNormalCompileBufferSize, MEM_DECOMMIT);
		(double *) XBOXRecompCode = (double *) VirtualAlloc(XBOXRecompCode, g_dwNormalCompileBufferSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
        fread(XBOXRecompCode,sizeof(char),g_dwNormalCompileBufferSize,fp);
		fclose(fp);
		DeleteFile("Z:\\codetemp.dat");
	}
	
/*
	FILE *fp;

	if (!charge) {
		if(g_memory != NULL){
			fp=fopen("Z:\\codetemp.dat","wb");
			fwrite(g_memory,RP_PAGE_SIZE_N * g_dwNumFrames ,sizeof(char),fp);
			free(g_memory);
		    fclose(fp);
			pause = 1;
            }
	}
	else {
		fp=fopen("Z:\\codetemp.dat","rb");
       	g_memory = (uint8 *) malloc ( sizeof(*g_memory)  *  RP_PAGE_SIZE_N * nombreframes );
		fread(g_memory,sizeof(char),RP_PAGE_SIZE_N * g_dwNumFrames,fp);
		fclose(fp);
		DeleteFile("Z:\\codetemp.dat");
		pause = 0;
	}
*/
}
