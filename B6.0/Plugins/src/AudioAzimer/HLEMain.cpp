#include "mytypes.h"
#include "audiohle.h"

// For pseudo-HLE code... :)
//u32 r0, at, v0, v1, a0, a1, a2, a3;
//u32 t0, t1, t2, t3, t4, t5, t6, t7;
//u32 s0, s1, s2, s3, s4, s5, s6, s7;
//u32 t8, t9, k0, k1, gp, sp, s8, ra;
u32 t9, k0;

u64 ProfileStartTimes[30];
u64 ProfileTimes[30];

// Audio UCode lists
//     Dummy UCode Handler for UCode detection... (Will always assume UCode1 until the nth list is executed)
extern void (*SafeABI[0x20])();
//---------------------------------------------------------------------------------------------
//
//     ABI 1 : Mario64, WaveRace USA, Golden Eye 007, Quest64, SF Rush
//				 60% of all games use this.  Distributed 3rd Party ABI
//
extern void (*ABI1[0x20])();
//---------------------------------------------------------------------------------------------
//
//     ABI 2 : WaveRace JAP, MarioKart 64, Mario64 JAP RumbleEdition, 
//				 Yoshi Story, Pokemon Games, Zelda64, Zelda MoM (miyamoto) 
//				 Most NCL or NOA games (Most commands)
extern void (*ABI2[0x20])();
//---------------------------------------------------------------------------------------------
//
//     ABI 3 : DK64, Perfect Dark, Banjo Kazooi, Banjo Tooie
//				 All RARE games except Golden Eye 007
//
extern void (*ABI3[0x20])();
//---------------------------------------------------------------------------------------------
//
//     ABI 5 : Factor 5 - MoSys/MusyX
//				 Rogue Squadron, Tarzan, Hydro Thunder, and TWINE
//				 Indiana Jones and Battle for Naboo (?)
//---------------------------------------------------------------------------------------------
//
//     ABI ? : Unknown or unsupported UCode
//
extern void (*ABIUnknown[0x20])();
//---------------------------------------------------------------------------------------------

void (*ABI[0x20])();
bool locklistsize = false;

//---------------------------------------------------------------------------------------------
// Set the Current ABI
void ChangeABI (int type) {
	switch (type) {
		case 0x0:
			//MessageBox (NULL, "ABI set to AutoDetect", "Audio ABI Changed", MB_OK);
			memcpy (ABI, SafeABI, 0x20*4);
		break;
		case 0x1:
			//MessageBox (NULL, "ABI set to ABI 1", "Audio ABI Changed", MB_OK);
			memcpy (ABI, ABI1, 0x20*4);
		break;
		case 0x2:
			//MessageBox (NULL, "ABI set to ABI 2", "Audio ABI Changed", MB_OK);
			//MessageBox (NULL, "Mario Kart, Zelda64, Zelda MoM, WaveRace JAP, etc. are not supported right now...", "Audio ABI Changed", MB_OK);
			//memcpy (ABI, ABIUnknown, 0x20*4);
			memcpy (ABI, ABI2, 0x20*4);
		break;
		case 0x3:
			//MessageBox (NULL, "ABI set to ABI 3", "Audio ABI Changed", MB_OK);
			//MessageBox (NULL, "DK64, Perfect Dark, Banjo Kazooi, Banjo Tooie, (RARE), not supported yet...", "Audio ABI Changed", MB_OK);
			//memcpy (ABI, ABIUnknown, 0x20*4);
			memcpy (ABI, ABI3, 0x20*4);
		break;
		/*case 0x4: // Mario Kart, Zelda64 (Demo Version)
			memcpy (ABI, ABI2, 0x20*4);
		break;*/
		case 0x5:
			 
			//MessageBox (NULL, "Rogue Squadron, Tarzan, Hydro Thunder, Indiana Jones and Battle for Naboo, and TWINE not supported yet...", "Audio ABI Not Supported", MB_OK);
//     ABI 5 : Factor 5 - MoSys/MusyX
//				 Rogue Squadron, Tarzan, Hydro Thunder, and TWINE
//				 Indiana Jones and Battle for Naboo (?)
			memcpy (ABI, ABIUnknown, 0x20*4);
			//memcpy (ABI, ABI3, 0x20*4);
		break;
		default:
			 
			memcpy (ABI, ABIUnknown, 0x20*4);
			return; // Quick out to prevent Dynarec from getting it...
	}
}

//---------------------------------------------------------------------------------------------

void SPU () {
}

u32 UCData, UDataLen;

//#define ENABLELOG
#ifdef ENABLELOG
#pragma message ("Logging of Timing info is enabled!!!")
FILE *dfile = fopen ("T:\\Misc\\HLEInfo.txt", "wt");
#endif

extern u32 loopval;
extern u32 SEGMENTS[0x10];
u32 base, dmembase;

void HLEStart () {
	u32 List  = ((u32*)dmem)[0xFF0/4], ListLen = ((u32*)dmem)[0xFF4/4];
	u32 *HLEPtr= (u32 *)(rdram+List);
	
	UCData= ((u32*)dmem)[0xFD8/4];
	UDataLen= ((u32*)dmem)[0xFDC/4];
	base = ((u32*)dmem)[0xFD0/4];
	dmembase = ((u32*)dmem)[0xFD8/4];

	loopval = 0;
	memset (SEGMENTS, 0, 0x10 * 4);

	//memcpy (imem+0x80, rdram+((u32*)dmem)[0xFD0/4], ((u32*)dmem)[0xFD4/4]);

#ifdef ENABLELOG
	u32 times[0x20];
	u32 calls[0x20];
	u32 list=0;
	u32 total;

	//memcpy (dmem, rdram+UCData, UDataLen); // Load UCode Data (for RSP stuff)

	memset (times, 0, 0x20*4);
	memset (calls, 0, 0x20*4);
	u64 start, end;
#endif

	ListLen = ListLen >> 2;

	if (*(u32*)(rdram+UCData+0x30) == 0x0B396696) {
		ChangeABI (5); // This will be replaced with ProcessMusyX
		return;
	}


	for (u32 x=0; x < ListLen; x+=2) {
		k0 = HLEPtr[x  ];
		t9 = HLEPtr[x+1];
//		fprintf (dfile, "k0: %08X  t9: %08X\n", k0, t9);
#ifdef ENABLELOG
		__asm {
			rdtsc;
			mov dword ptr [start+0], eax;
			mov dword ptr [start+4], edx;
		}
#endif
		StartProfile (2 + (k0 >> 24));
		ABI[k0 >> 24]();
		EndProfile (2 + (k0 >> 24));
#ifdef ENABLELOG
		__asm {
			rdtsc;
			mov dword ptr [end+0], eax;
			mov dword ptr [end+4], edx;
		}
		calls[k0>>24]++;
		times[k0>>24]+=(u32)(end-start);
#endif
	}
#ifdef ENABLELOG
	fprintf (dfile, "List #%i\n", list++);
	total = 0;
	for (x=0; x < 0x20; x++)
		total += times[x];
	for (x=0; x < 0x20; x++) {
		if (calls[x] != 0)
			fprintf (dfile, 
			"Command: %02X - Calls: %3i - Total Time: %6i - Avg Time: %8.2f - Percent: %5.2f%%\n", 
			x, 
			calls[x], 
			times[x], 
			(float)times[x]/(float)calls[x],
			((float)times[x]/(float)total)*100.0);
	}
#endif

//	fclose (dfile);
//	__asm int 3;
}

void (*ABIUnknown [0x20])() = { // Unknown ABI
	SPU, SPU, SPU, SPU, SPU, SPU, SPU, SPU,
	SPU, SPU, SPU, SPU, SPU, SPU, SPU, SPU,
	SPU, SPU, SPU, SPU, SPU, SPU, SPU, SPU,
	SPU, SPU, SPU, SPU, SPU, SPU, SPU, SPU
};