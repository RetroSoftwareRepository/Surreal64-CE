
//#include <xtl.h>
#include "..\stdafx.h"



#include "128meg.h"
#include <stdio.h>

// freakdave - new method of rom paging

extern DWORD g_dwNumFrames;
extern DWORD g_dwPageSize;
extern char g_temporaryRomPath[260];

// MTRR register addresses
#define IA32_MTRRCAP            0x00FE
#define IA32_MISCENABLE         0x01A0
#define IA32_MTRR_PHYSBASE0     0x0200
#define IA32_MTRR_PHYSMASK0     0x0201
#define IA32_MTRR_PHYSBASE1     0x0202
#define IA32_MTRR_PHYSMASK1     0x0203
#define IA32_MTRR_PHYSBASE2     0x0204
#define IA32_MTRR_PHYSMASK2     0x0205
#define IA32_MTRR_PHYSBASE3     0x0206
#define IA32_MTRR_PHYSMASK3     0x0207
#define IA32_MTRR_PHYSBASE4     0x0208
#define IA32_MTRR_PHYSMASK4     0x0209
#define IA32_MTRR_PHYSBASE5     0x020A
#define IA32_MTRR_PHYSMASK5     0x020B
#define IA32_MTRR_PHYSBASE6     0x020C
#define IA32_MTRR_PHYSMASK6     0x020D
#define IA32_MTRR_PHYSBASE7     0x020E
#define IA32_MTRR_PHYSMASK7     0x020F
#define IA32_MTRR_DEF_TYPE      0x02FF

void READMSRREG( UINT32 reg, LARGE_INTEGER *val ) 
{
  UINT32 lowPart, highPart;
  _asm {
    mov   ecx, reg
    rdmsr
    mov   lowPart, eax
    mov   highPart, edx
  };

  val->LowPart = lowPart;
  val->HighPart = highPart;
}

	//-------------------------------------------------------------------
  // WRITEMSRREG64
  //! \brief    Write to a 64 bit MSR register
	//-------------------------------------------------------------------
void WRITEMSRREG( UINT32 reg, LARGE_INTEGER val ) 
{
  _asm {
    mov   ecx, reg
    mov   eax, val.LowPart
    mov   edx, val.HighPart
    wrmsr
  };
}

//Ez0n3 - need this to get the paging value
#ifdef _XBOX
extern "C" int loaddw1964PagingMem();
#endif

void Enable128MegCaching( void )
{
  LARGE_INTEGER regVal;
    
  // Grab the existing default type
  READMSRREG( IA32_MTRR_DEF_TYPE, &regVal );
  
  // Set the default to WriteBack (0x06)
  regVal.LowPart = (regVal.LowPart & ~0xFF) | 0x06;
  WRITEMSRREG( IA32_MTRR_DEF_TYPE, regVal );

#ifdef USE_ROM_PAGING //_XBOX
  FILE *fp;
  long filesize;

#ifdef _XBOX
  fp = fopen(g_temporaryRomPath,"r");
  rewind(fp);
  fseek(fp, 0, SEEK_END);
  filesize = ftell(fp);
  fclose(fp);

  if(loaddw1964PagingMem() == 0)
	g_dwNumFrames = (DWORD)((filesize) / g_dwPageSize);
  else
	g_dwNumFrames = ((loaddw1964PagingMem() * 1024 * 1024) / g_dwPageSize); 

#else //win32
	g_dwNumFrames = ((4 * 1024 * 1024) / g_dwPageSize); // static 4MB
	return;
#endif
#endif
}
