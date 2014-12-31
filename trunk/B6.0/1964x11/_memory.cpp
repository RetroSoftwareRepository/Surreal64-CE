//#define PAGE_NOACCESS PAGE_EXECUTE_READWRITE
/*
 * 1964 Copyright (C) 1999-2004 Joel Middendorf, <schibo@emulation64.com> This
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
 */
#include "stdafx.h"

void __cdecl	LogDyna(char *debug, ...);

uint32			rdram_sizes[3] = { MEMORY_SIZE_NO_EXPANSION, MEMORY_SIZE_NO_EXPANSION, MEMORY_SIZE_WITH_EXPANSION };

uint32			current_rdram_size = MEMORY_SIZE_NO_EXPANSION;
BOOL			rdram_is_at_0x20000000 = FALSE;

#include "globals.h"

uint8			*dynarommap[0x10000];
uint8			*sDWord[0x10000];
uint8			*TLB_sDWord[0x100000];

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void Init_R_AND_W(uint8 **sDWORD_R, uint8 *MemoryRange, uint32 startAddress, uint32 size)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	uint8	*pTmp = (uint8 *) MemoryRange;
	uint32	curSegment = ((startAddress) >> 16);
	uint32	endSegment = ((startAddress + size - 1) >> 16);
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	while(curSegment <= endSegment)
	{
		sDWORD_R[curSegment | 0x8000] = pTmp;
		sDWORD_R[curSegment | 0xA000] = pTmp;
		pTmp += 0x10000;
		curSegment++;
	}
}

/*
 =======================================================================================================================
    Init DynaMemory Lookup Table �
    Function is called from fileio.c �
    right after a new ROM image is loaded �
 =======================================================================================================================
 */
void DynInit_R_AND_W(uint8 *MemoryRange, uint32 startAddress, uint32 endAddress)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	uint8	*pTmp = (uint8 *) MemoryRange;
	uint32	curSegment = ((startAddress & 0x1FFFFFFF) >> 16);	/* ????????? */
	uint32	endSegment = ((endAddress & 0x1FFFFFFF) >> 16);		/* ????????? */
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	while(curSegment <= endSegment)
	{
		gHardwareState.sDYN_PC_LOOKUP[curSegment | 0x8000] = pTmp;
		gHardwareState.sDYN_PC_LOOKUP[curSegment | 0xA000] = pTmp;
		pTmp += 0x10000;
		curSegment++;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void InitMemoryLookupTables1(uint8 **LUT, uint8 **LUT_2, MemoryState *gMemoryState)
{
	Init_R_AND_W(LUT, (uint8 *) gMemoryState->RDRAM, MEMORY_START_RDRAM, MEMORY_SIZE_RDRAM);
	Init_R_AND_W(LUT, (uint8 *) gMemoryState->ramRegs0, MEMORY_START_RAMREGS0, MEMORY_SIZE_RAMREGS0);
	Init_R_AND_W(LUT, (uint8 *) gMemoryState->ramRegs8, MEMORY_START_RAMREGS8, MEMORY_SIZE_RAMREGS8);
	Init_R_AND_W(LUT, (uint8 *) gMemoryState->SP_MEM, MEMORY_START_SPMEM, MEMORY_SIZE_SPMEM);
	Init_R_AND_W(LUT, (uint8 *) gMemoryState->SP_REG_1, MEMORY_START_SPREG_1, MEMORY_SIZE_SPREG_1);
	Init_R_AND_W(LUT, (uint8 *) gMemoryState->SP_REG_2, MEMORY_START_SPREG_2, MEMORY_SIZE_SPREG_2);
	Init_R_AND_W(LUT, (uint8 *) gMemoryState->DPC, MEMORY_START_DPC, MEMORY_SIZE_DPC);
	Init_R_AND_W(LUT, (uint8 *) gMemoryState->DPS, MEMORY_START_DPS, MEMORY_SIZE_DPS);
	Init_R_AND_W(LUT, (uint8 *) gMemoryState->MI, MEMORY_START_MI, MEMORY_SIZE_MI);
	Init_R_AND_W(LUT, (uint8 *) gMemoryState->VI, MEMORY_START_VI, MEMORY_SIZE_VI);
	Init_R_AND_W(LUT, (uint8 *) gMemoryState->AI, MEMORY_START_AI, MEMORY_SIZE_AI);
	Init_R_AND_W(LUT, (uint8 *) gMemoryState->PI, MEMORY_START_PI, MEMORY_SIZE_PI);
	Init_R_AND_W(LUT, (uint8 *) gMemoryState->RI, MEMORY_START_RI, MEMORY_SIZE_RI);
	Init_R_AND_W(LUT, (uint8 *) gMemoryState->SI, MEMORY_START_SI, MEMORY_SIZE_SI);
	Init_R_AND_W(LUT, (uint8 *) gMemoryState->C2A1, MEMORY_START_C2A1, MEMORY_SIZE_C2A1);
	Init_R_AND_W(LUT, (uint8 *) gMemoryState->C1A1, MEMORY_START_C1A1, MEMORY_SIZE_C1A1);
	Init_R_AND_W(LUT, (uint8 *) gMemoryState->C2A2, MEMORY_START_C2A2, MEMORY_SIZE_C2A2);

#ifndef USE_ROM_PAGING
	Init_R_AND_W(LUT, (uint8 *) gMemoryState->ROM_Image, MEMORY_START_ROM_IMAGE, gAllocationLength);
#endif
	Init_R_AND_W(LUT, (uint8 *) gMemoryState->GIO_REG, MEMORY_START_GIO, MEMORY_SIZE_GIO_REG);
	Init_R_AND_W(LUT, (uint8 *) gMemoryState->PIF, MEMORY_START_PIF, MEMORY_SIZE_PIF);
	Init_R_AND_W(LUT, (uint8 *) gMemoryState->C1A3, MEMORY_START_C1A3, MEMORY_SIZE_C1A3);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void InitMemoryLookupTables(void)
{
	int i;

	for(i = 0; i < 0x10000; i++)
	{
		sDWord[i] = gMemoryState.dummyNoAccess;

		gHardwareState.sDYN_PC_LOOKUP[i] = gMemoryState.dummyAllZero;
	}

	InitMemoryLookupTables1(sDWord, 0, &gMemoryState);

	for(i = 0; i < 0x100000; i++)
	{
		TLB_sDWord[i] = sDWord[i >> 4] + ((i & 0xf)<<12);
	}

	init_whole_mem_func_array();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void InitVirtualDynaMemory(void)
{
	if(dyna_RecompCode != NULL) VirtualFree(dyna_RecompCode, 0, MEM_RELEASE);
	if(RDRAM_Copy != NULL) VirtualFree(RDRAM_Copy, 0, MEM_RELEASE);

#ifdef _XBOX //_XBOX
	dyna_RecompCode = (uint8*)(double *) VirtualAlloc(NULL, g_dwRecompCodeSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);

#else //win32
	dyna_RecompCode = (uint8*)(double *) VirtualAlloc(NULL, RECOMPCODE_SIZE, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
#endif

	RDRAM_Copy = (uint8 *) VirtualAlloc(NULL, MEMORY_SIZE_WITH_EXPANSION, MEM_COMMIT, PAGE_EXECUTE_READWRITE);

	if(dyna_RecompCode == NULL)
	{
		MessageBox(0, "Can't alloc Mem for dyna_recompCode", "Exit", 0);
		exit(1);
	}

	if(RDRAM_Copy == NULL)
	{
		MessageBox(0, "Can't alloc RDRAM_Copy", "Exit", 0);
		exit(1);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void FreeVirtualDynaMemory(void)
{
	if(dyna_RecompCode != NULL) VirtualFree(dyna_RecompCode, 0, MEM_RELEASE);
}

#ifdef _XBOX //free mem for igm
extern "C" void ReInitVirtualDynaMemory(boolean charge)
{
	FILE *fp;

	if (!charge) {
		if(dyna_RecompCode != NULL){
			fp=fopen("Z:\\codetemp.dat","wb");
			//fp=fopen("T:\\Data\\codetemp.dat","wb");
			//fwrite(dyna_RecompCode,g_dwRecompCodeSize ,sizeof(char),fp);
			fwrite(dyna_RecompCode,g_dwRecompCodeSize ,sizeof(uint8),fp);
			VirtualFree(dyna_RecompCode, g_dwRecompCodeSize, MEM_DECOMMIT);
		    fclose(fp);}
	}
	else {
		fp=fopen("Z:\\codetemp.dat","rb");
		//fp=fopen("T:\\Data\\codetemp.dat","rb");
		VirtualFree(dyna_RecompCode, g_dwRecompCodeSize, MEM_DECOMMIT);
		//(double *) dyna_RecompCode = (double *) VirtualAlloc(dyna_RecompCode, g_dwRecompCodeSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
		dyna_RecompCode = (uint8*)(double *) VirtualAlloc(dyna_RecompCode, g_dwRecompCodeSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE); //surreal
        //fread(dyna_RecompCode,sizeof(char),g_dwRecompCodeSize,fp);
		fread(dyna_RecompCode,sizeof(uint8),g_dwRecompCodeSize,fp);
		fclose(fp);
		DeleteFile("Z:\\codetemp.dat");
		//DeleteFile("T:\\Data\\codetemp.dat");
	}
}
#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void InitVirtualMemory1(MemoryState *gMemoryState)
{


	/*
	 * We have to allocate the RDRAM and ExRDRAM atlocation 0x20000000, so then we can
	 * do faster �
	 * read/write_mem_rdram
	 */
#ifndef USE_ROM_PAGING //not _XBOX
	if(gMemoryState->RDRAM == NULL)
	{
		gMemoryState->RDRAM = (uint8*)(double *) VirtualAlloc
			(
				(double *) 0x20000000,
				MEMORY_SIZE_RDRAM + MEMORY_SIZE_EXRDRAM,
				MEM_RESERVE,
				PAGE_NOACCESS
			);
	}
#endif


	if(gMemoryState->dummyReadWrite == NULL) gMemoryState->dummyReadWrite = (uint8 *) malloc((MEMORY_SIZE_DUMMY * 15)+8);

#ifdef _XBOX
	if (gMemoryState->dummyReadWrite == NULL) OutputDebugString("dummyReadWrite malloc failed!"); //surreal
#endif

    //align data 64bits
    { int temp = (int)gMemoryState->dummyReadWrite+7;
        
        temp &= (~0x7);
        gMemoryState->dummyReadWrite = (unsigned char*)temp;
    }


	gMemoryState->SP_MEM = (uint32 *) (gMemoryState->dummyReadWrite + MEMORY_SIZE_DUMMY);
	gMemoryState->dummyAllZero = (uint8 *) (gMemoryState->dummyReadWrite + MEMORY_SIZE_DUMMY * 2);
	gMemoryState->SP_REG_1 = (uint32 *) (gMemoryState->dummyReadWrite + MEMORY_SIZE_DUMMY * 3);
	gMemoryState->SP_REG_2 = (uint32 *) (gMemoryState->dummyReadWrite + MEMORY_SIZE_DUMMY * 4);
	gMemoryState->DPC = (uint32 *) (gMemoryState->dummyReadWrite + MEMORY_SIZE_DUMMY * 5);
	gMemoryState->DPS = (uint32 *) (gMemoryState->dummyReadWrite + MEMORY_SIZE_DUMMY * 6);
	gMemoryState->MI = (uint32 *) (gMemoryState->dummyReadWrite + MEMORY_SIZE_DUMMY * 7);
	gMemoryState->VI = (uint32 *) (gMemoryState->dummyReadWrite + MEMORY_SIZE_DUMMY * 8);
	gMemoryState->AI = (uint32 *) (gMemoryState->dummyReadWrite + MEMORY_SIZE_DUMMY * 9);
	gMemoryState->PI = (uint32 *) (gMemoryState->dummyReadWrite + MEMORY_SIZE_DUMMY * 10);
	gMemoryState->RI = (uint32 *) (gMemoryState->dummyReadWrite + MEMORY_SIZE_DUMMY * 11);
	gMemoryState->SI = (uint32 *) (gMemoryState->dummyReadWrite + MEMORY_SIZE_DUMMY * 12);
	gMemoryState->PIF = (uint8 *) (gMemoryState->dummyReadWrite + MEMORY_SIZE_DUMMY * 13);
	gMemoryState->GIO_REG = (uint32 *) (gMemoryState->dummyReadWrite + MEMORY_SIZE_DUMMY * 14);

#ifdef USE_ROM_PAGING //_XBOX
	// MEMORY ISSUE MARKER
	//surreal
	rdram_is_at_0x20000000 = TRUE; //FALSE;

	(uint8 *) gMemoryState->RDRAM = (uint8 *) VirtualAlloc // surreal - oDD removed the doubles
		(
			(double*)0x20000000, //NULL,
			MEMORY_SIZE_RDRAM + MEMORY_SIZE_EXRDRAM,
			MEM_RESERVE,
			PAGE_NOACCESS
		);

#else // win32 - no paging
	rdram_is_at_0x20000000 = TRUE;

	if((uint32) gMemoryState->RDRAM != 0x20000000 || (debug_opcode == 1))
	{
		/*
		 * DisplayError("Cannot allocate 8MB RDRAM at fixed address. You may have some
		 * problems with memory settings in Windows. 1964 will still work without any
		 * problems, but speed will be affected a little bit.");
		 */
		VirtualFree(gMemoryState->RDRAM, 0, MEM_RELEASE);
		gMemoryState->RDRAM = (uint8*)(double *) VirtualAlloc
			(
				NULL,
				MEMORY_SIZE_RDRAM + MEMORY_SIZE_EXRDRAM,
				MEM_RESERVE,
				PAGE_NOACCESS
			);
		rdram_is_at_0x20000000 = FALSE;
	}
	else
	{
		rdram_is_at_0x20000000 = TRUE;
	}
#endif

	gMemoryState->RDRAM = (uint8 *) VirtualAlloc
		(
#ifdef USE_ROM_PAGING //_XBOX
			gMemoryState->RDRAM,
#else // win32 - no paging
			(void *) (gMemoryState->RDRAM),
#endif
			MEMORY_SIZE_RDRAM+8,
			MEM_COMMIT,
			PAGE_READWRITE
		);

    //align data 64bits
    { int temp = (int)gMemoryState->RDRAM+7;
        
        temp &= (~0x7);
        gMemoryState->RDRAM = (unsigned char*)temp;
    }

	if(gMemoryState->ramRegs0 == NULL)
	{
		gMemoryState->ramRegs0 = (uint32 *) VirtualAlloc(NULL, MEMORY_SIZE_DUMMY, MEM_RESERVE, PAGE_NOACCESS);
		gMemoryState->ramRegs0 = (uint32 *) VirtualAlloc
			(
				(void *) (gMemoryState->ramRegs0),
				MEMORY_SIZE_RAMREGS0,
				MEM_COMMIT,
				PAGE_READWRITE
			);
	}

	if(gMemoryState->ramRegs4 == NULL)
	{
		gMemoryState->ramRegs4 = (uint32 *) VirtualAlloc
			(
				(void *) (((uint8 *) gMemoryState->ramRegs0) + 0x4000),
				MEMORY_SIZE_RAMREGS4,
				MEM_COMMIT,
				PAGE_READWRITE
			);
	}

	if(gMemoryState->ramRegs8 == NULL)
	{
		gMemoryState->ramRegs8 = (uint32 *) VirtualAlloc(NULL, MEMORY_SIZE_RAMREGS8, MEM_RESERVE, PAGE_NOACCESS);
		gMemoryState->ramRegs8 = (uint32 *) VirtualAlloc
			(
				(void *) (gMemoryState->ramRegs8),
				MEMORY_SIZE_RAMREGS8,
				MEM_COMMIT,
				PAGE_READWRITE
			);
	}

	if(gMemoryState->C1A1 == NULL) gMemoryState->C1A1 = (uint32 *) gamesave.SRam;
	if(gMemoryState->C1A3 == NULL) gMemoryState->C1A3 = (uint32 *) gamesave.SRam;
	if(gMemoryState->C2A1 == NULL) gMemoryState->C2A1 = (uint32 *) gamesave.SRam;

	if(gMemoryState->dummyNoAccess == NULL)
	{
		gMemoryState->dummyNoAccess = (uint8 *) VirtualAlloc(NULL, MEMORY_SIZE_DUMMY+8, MEM_RESERVE, PAGE_NOACCESS);
		gMemoryState->dummyNoAccess = (uint8 *) VirtualAlloc
			(
				(void *) (gMemoryState->dummyNoAccess),
				MEMORY_SIZE_DUMMY+8,
				MEM_COMMIT,
				PAGE_NOACCESS
			);
	}

    //align data 64bits
    { int temp = (int)gMemoryState->dummyNoAccess+7;
        
        temp &= (~0x7);
        gMemoryState->dummyNoAccess = (unsigned char*)temp;
    }


	if(gMemoryState->C2A2 == NULL)
	{
		gMemoryState->C2A2 = (uint32 *) VirtualAlloc(NULL, MEMORY_SIZE_C2A2, MEM_RESERVE, PAGE_NOACCESS);
		gMemoryState->C2A2 = (uint32 *) VirtualAlloc
			(
				(void *) (gMemoryState->C2A2),
				MEMORY_SIZE_C2A2,
				MEM_COMMIT,
				PAGE_READWRITE
			);

		/* gMemoryState->C2A2 = (uint32*)gamesave.FlashRAM; //well, cannot do this */
	}

	memset(gMemoryState->dummyAllZero, 0, MEMORY_SIZE_DUMMY);
}

extern unsigned _int32	sync_valloc, sync_valloc2;
BOOL					opcode_debugger_memory_is_allocated = FALSE;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void InitVirtualMemory(void)
{
	InitVirtualMemory1(&gMemoryState);
	InitVirtualDynaMemory();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void FreeVirtualMemory1(MemoryState *gMemoryState)
{
	if(gMemoryState->dummyReadWrite != NULL) free((uint8 *) gMemoryState->dummyReadWrite);

	VirtualFree(gMemoryState->RDRAM, 0, MEM_RELEASE);
	VirtualFree(gMemoryState->ExRDRAM, 0, MEM_RELEASE);
	VirtualFree(gMemoryState->ramRegs0, 0, MEM_RELEASE);
	VirtualFree(gMemoryState->ramRegs4, 0, MEM_RELEASE);
	VirtualFree(gMemoryState->ramRegs8, 0, MEM_RELEASE);
	VirtualFree(gMemoryState->SP_MEM, 0, MEM_RELEASE);
	VirtualFree(gMemoryState->C2A1, 0, MEM_RELEASE);
	VirtualFree(gMemoryState->C1A1, 0, MEM_RELEASE);
	VirtualFree(gMemoryState->C2A2, 0, MEM_RELEASE);
	VirtualFree(gMemoryState->ROM_Image, 0, MEM_RELEASE);
	VirtualFree(gMemoryState->C1A3, 0, MEM_RELEASE);
	VirtualFree(gMemoryState->dummyNoAccess, 0, MEM_RELEASE);

	gMemoryState->RDRAM = NULL;
	gMemoryState->ExRDRAM = NULL;
	gMemoryState->C2A1 = NULL;
	gMemoryState->C1A1 = NULL;
	gMemoryState->C2A2 = NULL;
	gMemoryState->GIO_REG = NULL;
	gMemoryState->C1A3 = NULL;
	gMemoryState->PIF = NULL;
	gMemoryState->dummyNoAccess = NULL;
	gMemoryState->dummyReadWrite = NULL;
	gMemoryState->dummyAllZero = NULL;
	gMemoryState->ramRegs0 = NULL;
	gMemoryState->ramRegs4 = NULL;
	gMemoryState->ramRegs8 = NULL;
	gMemoryState->SP_MEM = NULL;
	gMemoryState->SP_REG_1 = NULL;
	gMemoryState->SP_REG_2 = NULL;
	gMemoryState->DPC = NULL;
	gMemoryState->DPS = NULL;
	gMemoryState->MI = NULL;
	gMemoryState->VI = NULL;
	gMemoryState->AI = NULL;
	gMemoryState->PI = NULL;
	gMemoryState->RI = NULL;
	gMemoryState->SI = NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void FreeVirtualMemory(void)
{
	FreeVirtualMemory1(&gMemoryState);
	FreeVirtualDynaMemory();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void InitVirtualRomMemory1(MemoryState *gMemoryState, uint32 memsize)
{
	FreeVirtualRomMemory();

    
    memsize = ((memsize + 0x1fffff) / 0x200000) * 0x200000; /* bring it up to a even 2MB. */

	gMemoryState->ROM_Image = (uint8 *) VirtualAlloc(NULL, memsize+8, MEM_RESERVE, PAGE_NOACCESS);
	gMemoryState->ROM_Image = (uint8 *) VirtualAlloc
		(
			(void *) (gMemoryState->ROM_Image),
			memsize+8,
			MEM_COMMIT,
			PAGE_READWRITE
		);
	TRACE1("Allocated memory for ROM image = %08X", (uint32) gMemoryState->ROM_Image);

    //align data 64bits
    { int temp = (int)gMemoryState->ROM_Image+7;
        
        temp &= (~0x7);
        gMemoryState->ROM_Image = (unsigned char*)temp;
    }


}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void InitVirtualRomMemory(uint32 memsize)
{
	InitVirtualRomMemory1(&gMemoryState, memsize);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void FreeVirtualRomMemory(void)
{
	/*~~*/
	int i;
	/*~~*/

	for(i = 0; i < 0x10000; i++)
	{
		if(dynarommap[i] != NULL)
		{
			/* VirtualFree(dynarommap[i], 0x10000, MEM_DECOMMIT); */
			VirtualFree(dynarommap[i], 0, MEM_RELEASE);
			dynarommap[i] = NULL;
		}
	}

	if(gMemoryState.ROM_Image != NULL)
	{
		/* if ( !(VirtualFree((void*)gMemoryState.ROM_Image, 64*1024*1024,MEM_DECOMMIT)) ) */
		if(!(VirtualFree((void *) gMemoryState.ROM_Image, 0, MEM_RELEASE)))
		{
			DisplayError("Failed to release virtual memory for ROM Image, error code is %ld", GetLastError());
		}
		else
		{
			gMemoryState.ROM_Image = NULL;
		}
	}

    //huh?
	for(i = 0x1000; i <= 0x1FFF; i++)
	{
		sDWord[i + 0x8000] = gMemoryState.dummyNoAccess;
		sDWord[i + 0xA000] = gMemoryState.dummyNoAccess;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL UnmappedMemoryExceptionHelper(uint32 addr)
{
	/*~~~~~~~~~~~~~~~~~~*/
	uint32	realpc = addr;
	uint32	offset;
	uint32	index;
	/*~~~~~~~~~~~~~~~~~~*/

    //MessageBox(0, "Unmapped Memory Exception", "", 0);
	//
	//  if( NOT_IN_KO_K1_SEG(realpc) )
	//  realpc = TranslateITLBAddress(realpc);
	// 
	offset = realpc & 0x1FFF0000;
	index = realpc >> 16;

	if (dynarommap[index] == NULL)
	dynarommap[index] = (unsigned char*)VirtualAlloc(NULL, 0x10000, MEM_COMMIT, PAGE_READWRITE);
	if(dynarommap[index] == NULL)
	{
		DisplayError("Unable to allocate memory to support dyna rom mapping, PC=%8X", gHWS_pc);
		return FALSE;
	}
	else
	{
        *(uint32*)(dynarommap[index]+(addr&0xffff)) = 0;

		/* Mapped the memory */
		if(IN_KO_K1_SEG(addr))
			DynInit_R_AND_W(dynarommap[index], offset, offset + 0x0000FFFF);
		else
			gHardwareState.sDYN_PC_LOOKUP[index] = dynarommap[index];

		/*
		 * TRACE3("Dyna memory mapped %08X, PC=%08X, Address=%08X", realpc, gHWS_pc,
		 * (uint32)dynarommap[index]);
		 */
		return TRUE;
	}
}

/*
 =======================================================================================================================
    Set RDRAM size
 =======================================================================================================================
 */
void ResetRdramSize(int setsize)
{
	/*~~~~~~~*/
	int retval;
	/*~~~~~~~*/

	if(rdram_sizes[setsize] != current_rdram_size)
	{
		if(setsize == RDRAMSIZE_4MB)	/* Need to turn off the expansion pack */
		{
			/*~~*/
			int i;
			/*~~*/

			for(i = 0x40; i < 0x80; i++)
			{
				sDWord[i | 0x8000] = gMemoryState.dummyNoAccess;
				sDWord[i | 0xA000] = gMemoryState.dummyNoAccess;
			}

			for(i = 0x400; i < 0x800; i++)
			{
				TLB_sDWord[i] = sDWord[i >> 4] + 0x1000 * (i & 0xf);
			}

			disable_exrdram_func_array();
			retval = VirtualFree(gMemoryState.ExRDRAM, MEMORY_SIZE_EXRDRAM, MEM_DECOMMIT);
			if(retval == 0)
			{
				DisplayError("Error to release the ExRDRAM");
			}
			else
			{
				gMemoryState.ExRDRAM = NULL;
			}
		}
		else	/* Need to turn on the expansion pack */
		{
			if(gMemoryState.ExRDRAM != NULL)
			{
				retval = VirtualFree(gMemoryState.ExRDRAM, MEMORY_SIZE_EXRDRAM, MEM_DECOMMIT);
			}

			gMemoryState.ExRDRAM = (uint8 *) VirtualAlloc
				(
					(((uint8 *) gMemoryState.RDRAM) + 0x400000),
					MEMORY_SIZE_EXRDRAM,
					MEM_COMMIT,
					PAGE_READWRITE
				);

			if((uint32) gMemoryState.ExRDRAM != (uint32) gMemoryState.RDRAM + 0x400000)
			{
				DisplayError
				(
					"Fix me in ResetRdramSize()!, RDRAM and ExRDRAM is not in contiguous memory address: RDRAM=%08X, ExRDRAM=%08X",
					(uint32) gMemoryState.RDRAM,
					(uint32) gMemoryState.ExRDRAM
				);
			}

			Init_R_AND_W(sDWord, (uint8 *) gMemoryState.ExRDRAM, MEMORY_START_EXRDRAM, MEMORY_SIZE_EXRDRAM);
			enable_exrdram_func_array();
		}

		current_rdram_size = rdram_sizes[setsize];
	}
	else if(setsize == RDRAMSIZE_8MB)	/* Need to use 8MB and we are on 8MB now */
	{
		/*~~*/
		int i;
		/*~~*/

		Init_R_AND_W(sDWord, (uint8 *) gMemoryState.ExRDRAM, MEMORY_START_EXRDRAM, MEMORY_SIZE_EXRDRAM);
		for(i = 0x400; i < 0x800; i++)
		{
			TLB_sDWord[0x80000 + i] = sDWord[(0x80000 + i) >> 4] + 0x1000 * (i & 0xf);
			TLB_sDWord[0xA0000 + i] = sDWord[(0xA0000 + i) >> 4] + 0x1000 * (i & 0xf);
		}

		enable_exrdram_func_array();
	}

	SetStatusBarText(3, setsize == RDRAMSIZE_4MB ? "4MB" : "8MB");
}

/*
 =======================================================================================================================
    These two CheckSum Checking routines are borrowed from Deadalus
 =======================================================================================================================
 */
#ifdef USE_ROM_PAGING //_XBOX
void ROM_CheckSumMario(uint8 *buf)
#else // win32 - no paging
void ROM_CheckSumMario(void)
#endif
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	DWORD	*rom;
	DWORD	addr1;
	DWORD	a1;
	DWORD	t7;
	DWORD	v1 = 0;
	DWORD	t0 = 0;
	DWORD	v0 = 0xF8CA4DDC;	/* (MARIO_BOOT_CIC * 0x5d588b65) + 1; */
	DWORD	a3 = 0xF8CA4DDC;
	DWORD	t2 = 0xF8CA4DDC;
	DWORD	t3 = 0xF8CA4DDC;
	DWORD	s0 = 0xF8CA4DDC;
	DWORD	a2 = 0xF8CA4DDC;
	DWORD	t4 = 0xF8CA4DDC;
	DWORD	t8, t6, a0;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

#ifdef USE_ROM_PAGING //_XBOX
	rom = (DWORD *)buf;
#else // win32 - no paging
	rom = (DWORD *) gMemoryState.ROM_Image;
#endif

	TRACE0("Checking CRC for this ROM");

	for(addr1 = 0; addr1 < 0x00100000; addr1 += 4)
	{
		v0 = rom[(addr1 + 0x1000) >> 2];
		v1 = a3 + v0;
		a1 = v1;
		if(v1 < a3) t2++;

		v1 = v0 & 0x001f;
		t7 = 0x20 - v1;
		t8 = (v0 >> (t7 & 0x1f));
		t6 = (v0 << (v1 & 0x1f));
		a0 = t6 | t8;

		a3 = a1;
		t3 ^= v0;
		s0 += a0;
		if(a2 < v0)
			a2 ^= a3 ^ v0;
		else
			a2 ^= a0;

		t0 += 4;
		t7 = v0 ^ s0;
		t4 += t7;
	}

	TRACE0("Finish CRC Checking");

	a3 ^= t2 ^ t3;				/* CRC1 */
	s0 ^= a2 ^ t4;				/* CRC2 */

	if(a3 != rom[0x10 >> 2] || s0 != rom[0x14 >> 2])
	{
		/* DisplayError("Warning, CRC values don't match, fixed"); */
		TRACE0("Warning, CRC values don't match, fixed");

		rom[0x10 >> 2] = a3;
		rom[0x14 >> 2] = s0;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
#ifdef USE_ROM_PAGING //_XBOX
void ROM_CheckSumZelda(uint8 *buf)
#else // win32 - no paging
void ROM_CheckSumZelda(void)
#endif
{
	/*~~~~~~~~~~~~~~~~~~~~*/
	DWORD	*rom;
	DWORD	addr1;
	DWORD	addr2;
	DWORD	t5 = 0x00000020;
	DWORD	a3 = 0xDF26F436;
	DWORD	t2 = 0xDF26F436;
	DWORD	t3 = 0xDF26F436;
	DWORD	s0 = 0xDF26F436;
	DWORD	a2 = 0xDF26F436;
	DWORD	t4 = 0xDF26F436;
	DWORD	v0, v1, a1, a0;
	/*~~~~~~~~~~~~~~~~~~~~*/

#ifdef USE_ROM_PAGING //_XBOX
	rom = (DWORD *)buf;
#else // win32 - no paging
	rom = (DWORD *) gMemoryState.ROM_Image;
#endif

	addr2 = 0;

	TRACE0("Checking CRC for this ROM");

	for(addr1 = 0; addr1 < 0x00100000; addr1 += 4)
	{
		v0 = rom[(addr1 + 0x1000) >> 2];
		v1 = a3 + v0;
		a1 = v1;

		if(v1 < a3) t2++;

		v1 = v0 & 0x1f;
		a0 = (v0 >> (t5 - v1)) | (v0 << v1);
		a3 = a1;
		t3 = t3 ^ v0;
		s0 += a0;
		if(a2 < v0)
			a2 ^= a3 ^ v0;
		else
			a2 ^= a0;

		t4 += rom[(addr2 + 0x750) >> 2] ^ v0;
		addr2 = (addr2 + 4) & 0xFF;
	}

	TRACE0("Finish CRC Checking");

	a3 ^= t2 ^ t3;
	s0 ^= a2 ^ t4;

	if(a3 != rom[0x10 >> 2] || s0 != rom[0x14 >> 2])
	{
		/* DisplayError("Warning, CRC values don't match, fixed"); */
		TRACE0("Warning, CRC values don't match, fixed");

		rom[0x10 >> 2] = a3;
		rom[0x14 >> 2] = s0;
	}

	TRACE2("Generating CRC [M%d / %d]", 0x00100000, 0x00100000);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void Debugger_Copy_Memory(MemoryState *target, MemoryState *source)
{
	memcpy(target->RDRAM, source->RDRAM, current_rdram_size);
	memcpy(target->ramRegs0, source->ramRegs0, MEMORY_SIZE_RAMREGS0);
	memcpy(target->ramRegs4, source->ramRegs4, MEMORY_SIZE_RAMREGS4);
	memcpy(target->ramRegs8, source->ramRegs8, MEMORY_SIZE_RAMREGS8);
	memcpy(target->SP_MEM, source->SP_MEM, MEMORY_SIZE_SPMEM);
	memcpy(target->SP_REG_1, source->SP_REG_1, MEMORY_SIZE_SPREG_1);
	memcpy(target->SP_REG_2, source->SP_REG_2, MEMORY_SIZE_SPREG_2);
	memcpy(target->DPC, source->DPC, MEMORY_SIZE_DPC);
	memcpy(target->DPS, source->DPS, MEMORY_SIZE_DPS);
	memcpy(target->MI, source->MI, MEMORY_SIZE_MI);
	memcpy(target->VI, source->VI, MEMORY_SIZE_VI);
	memcpy(target->AI, source->AI, MEMORY_SIZE_AI);
	memcpy(target->PI, source->PI, MEMORY_SIZE_PI);
	memcpy(target->RI, source->RI, MEMORY_SIZE_RI);
	memcpy(target->SI, source->SI, MEMORY_SIZE_SI);
	memcpy(target->GIO_REG, source->GIO_REG, MEMORY_SIZE_GIO_REG);
	memcpy(target->PIF, source->PIF, MEMORY_SIZE_PIF);
	memcpy(target->TLB, source->TLB, sizeof(tlb_struct) * MAXTLB);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL ProtectBlock(uint32 pc)
{
	if(IN_KO_K1_SEG(pc))
		pc = pc & 0xDFFFFFFF;
	else
		return FALSE;

	if(pc < 0x80000000 + current_rdram_size)
	{
		protect_memory_set_func_array(pc);
	}

	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL UnprotectBlock(uint32 pc)
{
	if(IN_KO_K1_SEG(pc))
		pc = pc & 0xDFFFFFFF;
	else
		return FALSE;

	if(pc >= 0x80000000 && pc < 0x80000000 + current_rdram_size)
	{
		unprotect_memory_set_func_array(pc);
	}

	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void UnprotectAllBlocks(void)
{
	/*~~~~~~*/
	uint32	i;
	/*~~~~~~*/

	for(i = 0; i < current_rdram_size / 0x1000; i++)
	{
		UnprotectBlock(0x80000000 + i * 0x1000);
	}

	PROTECT_MEMORY_TRACE(TRACE0("Unprotect All Blocks"));
}