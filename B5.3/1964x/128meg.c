
#include <xtl.h>
#include "128meg.h"
#include <stdio.h>


// Ez0n3 - old method of rom paging
//extern unsigned short g_dwNumFrames;
//#define RP_PAGE_SIZE 0x10000


// freakdave - new method of rom paging
extern unsigned short nombreframes;
#define RP_PAGE_SIZE 0x40000



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
extern int loaddw1964PagingMem();

void Enable128MegCaching( void )
{
  LARGE_INTEGER regVal;
  FILE *fp;
  long filesize;

  // Verify that we have 128 megs available
  MEMORYSTATUS memStatus;
  GlobalMemoryStatus( &memStatus );
  if( memStatus.dwTotalPhys < (100 * 1024 * 1024) ){
  
    //Ez0n3 - wrong paging variable - should be 1964 not PJ64
   	//nombreframes = (loaddwPJ64PagingMem() * 1024 * 1024) / RP_PAGE_SIZE;
	nombreframes = (loaddw1964PagingMem() * 1024 * 1024) / RP_PAGE_SIZE; //need for 128mb users
	//g_dwNumFrames = (loaddw1964PagingMem() * 1024 * 1024) / RP_PAGE_SIZE; //old method
	
	return;}

    // Grab the existing default type
  READMSRREG( IA32_MTRR_DEF_TYPE, &regVal );
  
    // Set the default to WriteBack (0x06)
  regVal.LowPart = (regVal.LowPart & ~0xFF) | 0x06;
  WRITEMSRREG( IA32_MTRR_DEF_TYPE, regVal );

  fp = fopen("Z:\\TemporaryRom.dat","r");
  rewind(fp);
  fseek(fp, 0, SEEK_END);
  filesize = ftell(fp);
  fclose(fp);
  nombreframes = (filesize) / RP_PAGE_SIZE; //need this for 128mb users
  //g_dwNumFrames = (filesize) / RP_PAGE_SIZE; //old method
}