/*
 * 1964 Copyright (C) 1999-2002 Joel Middendorf, <schibo@emulation64.com> This
 * program is free software; you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option) any later
 * version. This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details. You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA. To contact the
 * authors: email: schibo@emulation64.com, rice1964@yahoo.com
 *
 * 1964 modified for rom paging and paging algorithms by 
 * oDD buttza@hotmail.com July 2003
 */

#include <xtl.h>
#include <stdio.h>
#include <malloc.h>

#include "globals.h"
#include "rompaging.h"

#include "win32/wingui.h"
#include "fileio.h"
#include "r4300i.h"
#include "n64rcp.h"


/*---------------Doesnt work for 128mb-------------------
// Ez0n3 - old method of rom paging
DWORD g_dwNumFrames = 64; // default 4mb of memory
#define PAGE_NOT_IN_MEMORY	(g_dwNumFrames+1)
#define FRAME_FREE			0xFFFFFFFF
#define NEVER				0x0
#define NONE_FOUND			0xFFFFFFFF		
Frame *g_frameTable;
uint8 *g_memory;
uint8 *g_pageTable;
uint32 g_pageTableSize;

FILE	*g_temporaryRomFile = NULL;



uint8 *g_memory;
char    g_temporaryRomPath[260];
uint32	g_romSize;
*/

//---------------------------------------------------
//weinerschnitzel - reverted XXX B5 for 128mb users if freakdave's doesnt work
FILE	*g_temporaryRomFile = NULL;
char    g_temporaryRomPath[260];
uint32	g_romSize;

uint8 *g_memory;

char    g_temporaryRomPath[260];
uint32	g_romSize;

static BOOL indic[256];
static uint8 adress[256];
static uint8 adfix[256];

uint32 pagesize;  // 256*1024 -> uint32
uint16 nombreframes = 64; // 256 max -> uint8 = 255 max -> uint16
static uint8 plusgrand;
//---------------------------------------------------

/*
// freakdave - new method of rom paging
static BOOL indic[256];
static uint8 adress[256];
static uint8 adfix[256];
uint32 pagesize;  // 256*1024 -> uint32
uint16 nombreframes = 64; // 256 max -> uint8 = 255 max -> uint16
static uint8 plusgrand;
*/


BOOL InitVirtualRomData(char *rompath)
{
	InitMemoryLookupTables();
	InitTLB();

	Is_Reading_Rom_File = FALSE;
	
	strcpy(g_temporaryRomPath, "Z:\\TemporaryRom.dat");

	// open the temporary file for reading
	g_temporaryRomFile = fopen(g_temporaryRomPath, "rb");

	// get the size of the rom file
	rewind(g_temporaryRomFile);
	fseek(g_temporaryRomFile, 0, SEEK_END);
	g_romSize = ftell(g_temporaryRomFile);
	
	// read in the rom header
	fseek(g_temporaryRomFile, 0, SEEK_SET);
	fread((uint8 *) &rominfo.validation, sizeof(uint8), 0x40, g_temporaryRomFile);
	SwapRomHeader((uint8 *) &rominfo.validation);

	// read in boot code to SP_DMEM
	fseek(g_temporaryRomFile, 0, SEEK_SET);
	fread((uint8 *) &SP_DMEM, sizeof(uint8), 0x1000, g_temporaryRomFile);
	
	// read in rom name
	fseek(g_temporaryRomFile, 0x20, SEEK_SET);
	fread(rominfo.name, sizeof(uint8), 20, g_temporaryRomFile);
	SwapRomName(rominfo.name);
	
	/* weinerschnitzel - doesn't work for 128mb users
	// Ez0n3 - old method of rom paging
	fclose(g_temporaryRomFile);
	*/
	
	return TRUE;
}

void LoadVirtualRomData()
{
	/* weinerschnitzel - doesn't work for 128mb users
	// Ez0n3 - old method of rom paging
	g_temporaryRomFile = fopen(g_temporaryRomPath, "rb");
	*/
}

void CloseVirtualRomData()
{
	if (g_temporaryRomFile != NULL)
	{
		fclose(g_temporaryRomFile);
		g_temporaryRomFile = NULL;
		/* weinserschnitzel - doesn't work for 128mb users
		// Ez0n3 - old method of rom paging
		free(g_pageTable);
		*/

	}
}

void InitPageAndFrameTables()
{
	/* weinerschnitzel - doesn't work for 128mb users
	// Ez0n3 - old method of rom paging
	uint32 i;

	// get the size of the page table
	g_pageTableSize = g_romSize / RP_PAGE_SIZE;

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
	*/
	
//-------weinerschnitzel -- reverted for 128mb users------
// freakdave - new method of rom paging

	uint16 i;
	pagesize = 256*1024;
	g_memory = (uint8 *) malloc ( sizeof(*g_memory)  *  RP_PAGE_SIZE * nombreframes );
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
//--------------------------------------------------------

}

uint32 ReadUWORDFromROM(uint32 location)
{
	/*-------doesn't work for 128mb users------------------
	// Ez0n3 - old method of rom paging
	uint32 i;
	uint32 pageNumberOfLocation;
	uint32 offsetFromPage;

	g_pageFunctionHits++;

	location &= 0x7ffffff;

	if (location > g_romSize)
		return 0;

	// calculate what page the location is in and how far from the start of
	// the page the location is
	pageNumberOfLocation = location / RP_PAGE_SIZE;
	offsetFromPage = location - (pageNumberOfLocation*RP_PAGE_SIZE);

	// if the page required is in memory, use it
	if (g_pageTable[pageNumberOfLocation] != PAGE_NOT_IN_MEMORY)
	{
		g_pageHits++;

		if (offsetFromPage <= (RP_PAGE_SIZE-sizeof(uint32)))
		{
			return *(uint32 *)(g_memory + (g_pageTable[pageNumberOfLocation]*RP_PAGE_SIZE) + offsetFromPage);
		}
		else // if the location isn't a multiple of 4 this may happen? hopefully it doesn't ever
		{
			g_memFunctionHits++;
			// fix me
			return *(uint32 *)(g_memory + (g_pageTable[pageNumberOfLocation]*RP_PAGE_SIZE) + offsetFromPage);
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
			fseek(g_temporaryRomFile, pageNumberOfLocation*RP_PAGE_SIZE, SEEK_SET);

			// read in the page
			fread(&g_memory[frameToUse*RP_PAGE_SIZE], sizeof(char), RP_PAGE_SIZE, g_temporaryRomFile);

			// update the page table
			g_pageTable[pageNumberOfLocation] = frameToUse;

			return *(uint32 *)(g_memory + (g_pageTable[pageNumberOfLocation]*RP_PAGE_SIZE) + offsetFromPage);
		}
	}

*/
//-----------------------------------------------------

//weinerschnitzel reverting for 128mb users------------
// freakdave - new method of rom paging

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
//--------------------------------------------------------
}

uint16 ReadUHALFFromROM(uint32 location)
{

	/*-------doesn't work for 128mb users------------------
	// Ez0n3 - old method of rom paging
	uint32 i;
	uint32 pageNumberOfLocation;
	uint32 offsetFromPage;

	g_pageFunctionHits++;

	location &= 0x7ffffff;

	if (location > g_romSize)
		return 0;

	// calculate what page the location is in and how far from the start of
	// the page the location is
	pageNumberOfLocation = location / RP_PAGE_SIZE;
	offsetFromPage = location - (pageNumberOfLocation*RP_PAGE_SIZE);

	// if the page required is in memory, use it
	if (g_pageTable[pageNumberOfLocation] != PAGE_NOT_IN_MEMORY)
	{
		g_pageHits++;

		if (offsetFromPage <= (RP_PAGE_SIZE-sizeof(uint16)))
		{
			return *(uint16 *)(g_memory + (g_pageTable[pageNumberOfLocation]*RP_PAGE_SIZE) + offsetFromPage);
		}
		else // if the location isn't a multiple of 4 this may happen? hopefully it doesn't ever
		{
			g_memFunctionHits++;
			// fix me
			return *(uint16 *)(g_memory + (g_pageTable[pageNumberOfLocation]*RP_PAGE_SIZE) + offsetFromPage);
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
			fseek(g_temporaryRomFile, pageNumberOfLocation*RP_PAGE_SIZE, SEEK_SET);

			// read in the page
			fread(&g_memory[frameToUse*RP_PAGE_SIZE], sizeof(char), RP_PAGE_SIZE, g_temporaryRomFile);

			// update the page table
			g_pageTable[pageNumberOfLocation] = frameToUse;

			return *(uint16 *)(g_memory + (g_pageTable[pageNumberOfLocation]*RP_PAGE_SIZE) + offsetFromPage);
		}
	}
*/
//------------------------------------------------------------

//weinerschnitzel - reverting for 128mb users
// freakdave - new method of rom paging

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
//--------------------------------------
	
}

uint8 ReadUBYTEFromROM(uint32 location)
{

	/*-------doesn't work for 128mb users------------------
	// Ez0n3 - old method of rom paging
	uint32 i;
	uint32 pageNumberOfLocation;
	uint32 offsetFromPage;

	g_pageFunctionHits++;

	location &= 0x7ffffff;

	if (location > g_romSize)
		return 0;

	// calculate what page the location is in and how far from the start of
	// the page the location is
	pageNumberOfLocation = location / RP_PAGE_SIZE;
	offsetFromPage = location - (pageNumberOfLocation*RP_PAGE_SIZE);

	// if the page required is in memory, use it
	if (g_pageTable[pageNumberOfLocation] != PAGE_NOT_IN_MEMORY)
	{
		g_pageHits++;

		if (offsetFromPage <= (RP_PAGE_SIZE-sizeof(uint8)))
		{
			return *(uint8 *)(g_memory + (g_pageTable[pageNumberOfLocation]*RP_PAGE_SIZE) + offsetFromPage);
		}
		else // if the location isn't a multiple of 4 this may happen? hopefully it doesn't ever
		{
			g_memFunctionHits++;
			// fix me
			return *(uint8 *)(g_memory + (g_pageTable[pageNumberOfLocation]*RP_PAGE_SIZE) + offsetFromPage);
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
			fseek(g_temporaryRomFile, pageNumberOfLocation*RP_PAGE_SIZE, SEEK_SET);

			// read in the page
			fread(&g_memory[frameToUse*RP_PAGE_SIZE], sizeof(char), RP_PAGE_SIZE, g_temporaryRomFile);

			// update the page table
			g_pageTable[pageNumberOfLocation] = frameToUse;

			return *(uint8 *)(g_memory + (g_pageTable[pageNumberOfLocation]*RP_PAGE_SIZE) + offsetFromPage);
		}
	}
*/

//-----------------------------------------------------
//weinerschnitzel - reverting for 128mb users
// freakdave - new method of rom paging

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
//-------------------------------------------------
	
}

__int32 ReadSWORDFromROM(uint32 location)
{
	
	/*-------doesn't work for 128mb users------------------
	// Ez0n3 - old method of rom paging
	uint32 i;
	uint32 pageNumberOfLocation;
	uint32 offsetFromPage;
	uint32 retVal;

	g_pageFunctionHits++;

	location &= 0x7ffffff;

	if (location > g_romSize)
		return 0;

	// calculate what page the location is in and how far from the start of
	// the page the location is
	pageNumberOfLocation = location / RP_PAGE_SIZE;
	offsetFromPage = location - (pageNumberOfLocation*RP_PAGE_SIZE);

	// if the page required is in memory, use it
	if (g_pageTable[pageNumberOfLocation] != PAGE_NOT_IN_MEMORY)
	{
		g_pageHits++;

		if (offsetFromPage <= (RP_PAGE_SIZE-sizeof(uint16)))
		{
			retVal = *(__int32 *)(g_memory + (g_pageTable[pageNumberOfLocation]*RP_PAGE_SIZE) + offsetFromPage);
		}
		else // if the location isn't a multiple of 4 this may happen? hopefully it doesn't ever
		{
			g_memFunctionHits++;
			// fix me
			retVal = *(__int32 *)(g_memory + (g_pageTable[pageNumberOfLocation]*RP_PAGE_SIZE) + offsetFromPage);
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
			fseek(g_temporaryRomFile, pageNumberOfLocation*RP_PAGE_SIZE, SEEK_SET);

			// read in the page
			fread(&g_memory[frameToUse*RP_PAGE_SIZE], sizeof(char), RP_PAGE_SIZE, g_temporaryRomFile);

			// update the page table
			g_pageTable[pageNumberOfLocation] = frameToUse;

			retVal = *(__int32 *)(g_memory + (g_pageTable[pageNumberOfLocation]*RP_PAGE_SIZE) + offsetFromPage);
		}
	}

	return retVal;
*/
//------------------------------------------

//weinerschnitzel - reverting for 128mb users
// freakdave - new method of rom paging

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
//-----------------------------------------------------------------
	
}

BOOL CheckIfInRom(uint32 location)
{
	//return (((addr)&0x1FFFFFFF) >= MEMORY_START_ROM_IMAGE && ((addr)&0x1FFFFFFF) <= 0x14000000);
	register uint32 locShifted = location >> SHIFTER2_READ;

	return ((locShifted >= 0x2400 && locShifted <= 0x24FF) \
		 || (locShifted >= 0x2C00 && locShifted <= 0x2CFF));
}

