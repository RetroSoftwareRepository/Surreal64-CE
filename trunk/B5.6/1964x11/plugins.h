/*$T plugins.h GC 1.136 02/28/02 09:02:31 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifndef _PLUGINS_H__1964_
#define _PLUGINS_H__1964_

#define PLUGIN_TYPE_RSP			1
#define PLUGIN_TYPE_GFX			2
#define PLUGIN_TYPE_AUDIO		3
#define PLUGIN_TYPE_CONTROLLER	4

typedef struct
{
	uint16	Version;
	uint16	Type;
	char	Name[100];

	int		NormalMemory;
	int		MemoryBswaped;
} PLUGIN_INFO;

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


/* Note: BOOL, BYTE, WORD, DWORD, TRUE, FALSE are defined in windows.h */
//#define PLUGIN_TYPE_AUDIO	3 //again?

#ifdef _XBOX
#define EXPORT
#define CALL
#else //win32
#define EXPORT				__declspec(dllexport)
#define CALL				_cdecl
#endif //_XBOX

#define TV_SYSTEM_NTSC		1
#define TV_SYSTEM_PAL		0
#define TV_SYSTEM_MPAL		0

// pj64 and all audio plugins
#define SYSTEM_NTSC			0
#define SYSTEM_PAL			1
#define SYSTEM_MPAL			2

typedef struct
{
	HWND		hwnd;
	HINSTANCE	hinst;
	BOOL		MemoryBswaped;	/* If this is set to TRUE, then the memory has been pre bswapped on a dword (32 bits)
								 * boundry eg. the first 8 bytes are stored like this: 4 3 2 1 8 7 6 5 */
	BYTE		*HEADER;		/* This is the rom header (first 40h bytes of the rom */

	/* This will be in the same memory format as the rest of the memory. */
	BYTE		*__RDRAM;
	BYTE		*__DMEM;
	BYTE		*__IMEM;
	DWORD		*__MI_INTR_REG;
	DWORD		*__AI_DRAM_ADDR_REG;
	DWORD		*__AI_LEN_REG;
	DWORD		*__AI_CONTROL_REG;
	DWORD		*__AI_STATUS_REG;
	DWORD		*__AI_DACRATE_REG;
	DWORD		*__AI_BITRATE_REG;

	void (*CheckInterrupts) (void);
}
AUDIO_INFO;

/* Controller plugin's */
#define PLUGIN_NONE			1
#define PLUGIN_MEMPAK		2
#define PLUGIN_RUMBLE_PAK	3	/* not implemeted for non raw data */
#define PLUGIN_TANSFER_PAK	4	/* not implemeted for non raw data */
/*
typedef struct
{
	BOOL	Present;
	BOOL	RawData;
	int		Plugin;
} CONTROL;

typedef union
{
	DWORD	Value;
	struct
	{
		unsigned	R_DPAD : 1;
		unsigned	L_DPAD : 1;
		unsigned	D_DPAD : 1;
		unsigned	U_DPAD : 1;
		unsigned	START_BUTTON : 1;
		unsigned	Z_TRIG : 1;
		unsigned	B_BUTTON : 1;
		unsigned	A_BUTTON : 1;
		unsigned	R_CBUTTON : 1;
		unsigned	L_CBUTTON : 1;
		unsigned	D_CBUTTON : 1;
		unsigned	U_CBUTTON : 1;
		unsigned	R_TRIG : 1;
		unsigned	L_TRIG : 1;
		unsigned	Reserved1 : 1;
		unsigned	Reserved2 : 1;
		signed		Y_AXIS : 8;
		signed		X_AXIS : 8;
	};
} BUTTONS;
*/
typedef struct
{
	DWORD addr;
	DWORD val;
	DWORD size;				// 1 = BYTE, 2 = WORD, 4=DWORD
} FrameBufferModifyEntry;


#ifdef _XBOX
#include <SurrealPlugins.h>

typedef struct {
	HINSTANCE hInst;
	BOOL MemoryBswaped;    // If this is set to TRUE, then the memory has been pre
	                       //  bswap on a dword (32 bits) boundry 
	BYTE * RDRAM;
	BYTE * DMEM;
	BYTE * IMEM;

	DWORD * MI__INTR_REG;

	DWORD * SP__MEM_ADDR_REG;
	DWORD * SP__DRAM_ADDR_REG;
	DWORD * SP__RD_LEN_REG;
	DWORD * SP__WR_LEN_REG;
	DWORD * SP__STATUS_REG;
	DWORD * SP__DMA_FULL_REG;
	DWORD * SP__DMA_BUSY_REG;
	DWORD * SP__PC_REG;
	DWORD * SP__SEMAPHORE_REG;

	DWORD * DPC__START_REG;
	DWORD * DPC__END_REG;
	DWORD * DPC__CURRENT_REG;
	DWORD * DPC__STATUS_REG;
	DWORD * DPC__CLOCK_REG;
	DWORD * DPC__BUFBUSY_REG;
	DWORD * DPC__PIPEBUSY_REG;
	DWORD * DPC__TMEM_REG;

	void (CALL *CheckInterrupts)( void );
	void (CALL *ProcessDlist)( void );
	void (CALL *ProcessAlist)( void );
	void (CALL *ProcessRdpList)( void );
} RSP_INFO_1_0;

typedef struct {
	HINSTANCE hInst;
	BOOL MemoryBswaped;    // If this is set to TRUE, then the memory has been pre
	                       //  bswap on a dword (32 bits) boundry 
	BYTE * RDRAM;
	BYTE * DMEM;
	BYTE * IMEM;

	DWORD * MI__INTR_REG;

	DWORD * SP__MEM_ADDR_REG;
	DWORD * SP__DRAM_ADDR_REG;
	DWORD * SP__RD_LEN_REG;
	DWORD * SP__WR_LEN_REG;
	DWORD * SP__STATUS_REG;
	DWORD * SP__DMA_FULL_REG;
	DWORD * SP__DMA_BUSY_REG;
	DWORD * SP__PC_REG;
	DWORD * SP__SEMAPHORE_REG;

	DWORD * DPC__START_REG;
	DWORD * DPC__END_REG;
	DWORD * DPC__CURRENT_REG;
	DWORD * DPC__STATUS_REG;
	DWORD * DPC__CLOCK_REG;
	DWORD * DPC__BUFBUSY_REG;
	DWORD * DPC__PIPEBUSY_REG;
	DWORD * DPC__TMEM_REG;

	void (CALL *CheckInterrupts)( void );
	void (CALL *ProcessDlist)( void );
	void (CALL *ProcessAlist)( void );
	void (CALL *ProcessRdpList)( void );
	void (CALL *ShowCFB)( void );
} RSP_INFO_1_1;


//#include <RSP.h>
// Rsp Plugins
#if defined(_RSP_HACK_HLE)
#include <RspHackHLE.h>
#endif //_RSP_HACK_HLE

#if defined(_RSP_M64P_HLE)
#include <RspM64PHLE.h>
#endif //_RSP_M64P_HLE

#if defined(_RSP_PJ64_LLE)
#include <RspPJ64LLE.h>
#endif //_RSP_PJ64_LLE

// Audio Plugins
#if defined(_AUDIO_AZIMER)
#include <AudioAzimer.h>
#endif //_AUDIO_AZIMER

#if defined(_AUDIO_BASIC)
#include <AudioBasic.h>
#endif //_AUDIO_BASIC

#if defined(_AUDIO_DAEDALUS)
#include <AudioDaedalus.h>
#endif //_AUDIO_DAEDALUS

#if defined(_AUDIO_JTTL)
#include <AudioJttL.h>
#endif //_AUDIO_JTTL

#if defined(_AUDIO_M64P)
#include <AudioM64Plus.h>
#endif //_AUDIO_M64P

#if defined(_AUDIO_MUSYX)
#include <AudioMusyX.h>
#endif //_AUDIO_MUSYX

#if defined(_AUDIO_NONE)
#include <AudioNone.h>
#endif //_AUDIO_NONE

#include <Input.h>

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

#endif //_XBOX


extern GFX_INFO		Gfx_Info;
extern AUDIO_INFO	Audio_Info;
extern CONTROL		Controls[4];
#endif //_PLUGINS_H__1964_