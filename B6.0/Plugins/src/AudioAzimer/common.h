#ifndef _COMMON_DOT_H_
#define _COMMON_DOT_H_
#ifdef _XBOX
#include <xtl.h>
#else
#include <windows.h>
#endif
#include <stdio.h>

//#define ENABLEPROFILING

#include "mytypes.h"

typedef struct {
	u16 Version;
	u32 BufferSize;
	BOOL doAIHACK;
	BOOL syncAudio;
	BOOL fillAudio;
	BOOL oldStyle;
	BOOL Reserved2;
	BOOL Reserved3;
	u32  Reserved4;
	u32  Reserved5;
	u32  Reserved6;
} rSettings;
extern rSettings RegSettings;
#endif

#define AUDIOCODE 0
#define HLECODE   1
#define CPUCODE   2

unsigned long GenerateCRC (unsigned char *data, int size);

#ifdef ENABLEPROFILING

	extern u64 ProfileStartTimes[30];
	extern u64 ProfileTimes[30];

	inline void StartProfile (int profile) {
		u64 start;
		__asm {
			rdtsc;
			mov dword ptr [start+0], eax;
			mov dword ptr [start+4], edx;
		}
		ProfileStartTimes[profile] = start;
	}

	inline void EndProfile (int profile) {
		u64 end;
		__asm {
			rdtsc;
			mov dword ptr [end+0], eax;
			mov dword ptr [end+4], edx;
		}
		ProfileTimes[profile] = ProfileTimes[profile] + (end - ProfileStartTimes[profile]);
	}
	inline void PrintProfiles () {
		FILE *dfile = fopen ("T:\\Misc\\profile.txt", "wt");
		u64 totalTimes = 0;
		for (int x = 0; x < 30; x++) {
			if (ProfileTimes[x] != 0) {
				fprintf (dfile, "Times for %i is: %08X %08X\n", x, (u32)(ProfileTimes[x] >> 32), (u32)ProfileTimes[x]);
				totalTimes += ProfileTimes[x];
			}
		}
		for (x = 0; x < 30; x++) {
			if (ProfileTimes[x] != 0) {
				fprintf (dfile, "Percent Time for %i is: %i%%\n", x, (u32)((ProfileTimes[x]*100) / totalTimes));			
			}
		}
		fclose (dfile);
	}
	inline void ClearProfiles () {
		for (int x = 0; x < 30; x++) {
			ProfileTimes[x] = 0;
		}
	}
#else
#	define StartProfile(profile) //
#	define EndProfile(profile) //
#	define PrintProfiles() //
#	define ClearProfiles()//
#endif
