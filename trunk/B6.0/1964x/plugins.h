/*$T plugins.h GC 1.136 02/28/02 09:02:31 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#pragma once

//#include <Common.h>
// original 1964
//controls tv sys only - similar to rice
#define TV_SYSTEM_NTSC		1
#define TV_SYSTEM_PAL		0
#define TV_SYSTEM_MPAL		0

// pj64 and all audio plugins
#define SYSTEM_NTSC			0
#define SYSTEM_PAL			1
#define SYSTEM_MPAL			2

#include <RSP.h>

#if defined(USE_MUSYX)
#include <AudioMusyX.h>
#elif defined(_AUDIO_AZIAUD)
#include <AudioAziAudioDS8.h>
#else
//#include <AudioM64Plus.h>
#include <AudioJttL.h>
#include <AudioBasic.h>
#include <AudioNone.h>
#include <AudioAzimer.h>
#endif

#include "../Plugins.h"

#include <Input.h>
typedef struct
{
	HWND	hWnd;
	HWND	hStatusBar;
	int		MemoryBswaped;
	_int8	*HEADER;
	_int8	*RDRAM;
	_int8	*DMEM;
	_int8	*IMEM;
	uint32	*MI_INTR_RG;
	uint32	*DPC_START_RG;
	uint32	*DPC_END_RG;
	uint32	*DPC_CURRENT_RG;
	uint32	*DPC_STATUS_RG;
	uint32	*DPC_CLOCK_RG;
	uint32	*DPC_BUFBUSY_RG;
	uint32	*DPC_PIPEBUSY_RG;
	uint32	*DPC_TMEM_RG;
	uint32	*VI_STATUS_RG;
	uint32	*VI_ORIGIN_RG;
	uint32	*VI_WIDTH_RG;
	uint32	*VI_INTR_RG;
	uint32	*VI_V_CURRENT_LINE_RG;
	uint32	*VI_TIMING_RG;
	uint32	*VI_V_SYNC_RG;
	uint32	*VI_H_SYNC_RG;
	uint32	*VI_LEAP_RG;
	uint32	*VI_H_START_RG;
	uint32	*VI_V_START_RG;
	uint32	*VI_V_BURST_RG;
	uint32	*VI_X_SCALE_RG;
	uint32	*VI_Y_SCALE_RG;
	void (*CheckInterrupts) (void);
}
GFX_INFO;

typedef struct
{
	uint32	addr;
	uint32	size;
	uint32	width;
	uint32	height;
} FrameBufferInfo;

//#include <Video.h>
#if defined(_VIDEO_1964_11)
#include <Video1964.h>
#elif defined(_VIDEO_RICE_510)
#include <VideoRice510.h>
#elif defined(_VIDEO_RICE_531)
#include <VideoRice531.h>
#elif defined(_VIDEO_RICE_560)
#include <VideoRice560.h>
#elif defined(_VIDEO_RICE_611)
#include <VideoRice611.h>
#elif defined(_VIDEO_RICE_612)
#include <VideoRice612.h>
#endif

extern GFX_INFO		Gfx_Info;
extern AUDIO_INFO	Audio_Info;
extern CONTROL		Controls[4];

