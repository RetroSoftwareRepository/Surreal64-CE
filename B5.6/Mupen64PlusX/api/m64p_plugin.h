/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Mupen64plus-core - m64p_plugin.h                                      *
 *   Mupen64Plus homepage: http://code.google.com/p/mupen64plus/           *
 *   Copyright (C) 2002 Hacktarux                                          *
 *   Copyright (C) 2009 Richard Goedeken                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.          *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#if !defined(M64P_PLUGIN_H)
#define M64P_PLUGIN_H

#include "m64p_types.h"

#include "../plugin/plugins.h"
#include "../plugin/Static_Audio.h"




#ifdef __cplusplus
extern "C" {
#endif
#ifndef _XBOX

//extern CONTROL		Controls[4];

/*** Controller plugin's ****/
#define PLUGIN_NONE                 1
#define PLUGIN_MEMPAK               2
#define PLUGIN_RUMBLE_PAK           3 /* not implemented for non raw data */
#define PLUGIN_TRANSFER_PAK         4 /* not implemented for non raw data */
#define PLUGIN_RAW                  5 /* the controller plugin is passed in raw data */

/***** Structures *****/
	/*
typedef struct {
    unsigned char * RDRAM;
    unsigned char * DMEM;
    unsigned char * IMEM;

    unsigned int * MI_INTR_REG;

    unsigned int * SP_MEM_ADDR_REG;
    unsigned int * SP_DRAM_ADDR_REG;
    unsigned int * SP_RD_LEN_REG;
    unsigned int * SP_WR_LEN_REG;
    unsigned int * SP_STATUS_REG;
    unsigned int * SP_DMA_FULL_REG;
    unsigned int * SP_DMA_BUSY_REG;
    unsigned int * SP_PC_REG;
    unsigned int * SP_SEMAPHORE_REG;

    unsigned int * DPC_START_REG;
    unsigned int * DPC_END_REG;
    unsigned int * DPC_CURRENT_REG;
    unsigned int * DPC_STATUS_REG;
    unsigned int * DPC_CLOCK_REG;
    unsigned int * DPC_BUFBUSY_REG;
    unsigned int * DPC_PIPEBUSY_REG;
    unsigned int * DPC_TMEM_REG;

    void (*CheckInterrupts)(void);
    void (*ProcessDlistList)(void);
    void (*ProcessAlistList)(void);
    void (*ProcessRdpList)(void);
    void (*ShowCFB)(void);
} RSP_INFO;
*/
/*
typedef struct {
	HWND hWnd;			
	HWND hStatusBar;   

	BOOL MemoryBswaped;    // If this is set to TRUE, then the memory has been pre
	                       //   bswap on a dword (32 bits) boundry 
						   //	eg. the first 8 bytes are stored like this:
	                       //        4 3 2 1   8 7 6 5

    BYTE * HEADER; 
    BYTE * RDRAM;
    BYTE * DMEM;
    BYTE * IMEM;

    DWORD * MI_INTR_REG;

    DWORD * DPC_START_REG;
    DWORD * DPC_END_REG;
    DWORD * DPC_CURRENT_REG;
    DWORD * DPC_STATUS_REG;
    DWORD * DPC_CLOCK_REG;
    DWORD * DPC_BUFBUSY_REG;
    DWORD * DPC_PIPEBUSY_REG;
    DWORD * DPC_TMEM_REG;

    DWORD * VI_STATUS_REG;
    DWORD * VI_ORIGIN_REG;
    DWORD * VI_WIDTH_REG;
    DWORD * VI_INTR_REG;
    DWORD * VI_V_CURRENT_LINE_REG;
    DWORD * VI_TIMING_REG;
    DWORD * VI_V_SYNC_REG;
    DWORD * VI_H_SYNC_REG;
    DWORD * VI_LEAP_REG;
    DWORD * VI_H_START_REG;
    DWORD * VI_V_START_REG;
    DWORD * VI_V_BURST_REG;
    DWORD * VI_X_SCALE_REG;
    DWORD * VI_Y_SCALE_REG;

    void (*CheckInterrupts)(void);
} GFX_INFO;
*/
/*
typedef struct {
	HWND hwnd;
	HINSTANCE hinst;
	BOOL MemoryBswaped;    // If this is set to TRUE, then the memory has been pre
	                       //   bswap on a dword (32 bits) boundry 
						   //	eg. the first 8 bytes are stored like this:
	                       //        4 3 2 1   8 7 6 5
	BYTE * HEADER;	// This is the rom header (first 40h bytes of the rom
					// This will be in the same memory format as the rest of the memory.
    unsigned char * RDRAM;
    unsigned char * DMEM;
    unsigned char * IMEM;

    unsigned int * MI_INTR_REG;

    unsigned int * AI_DRAM_ADDR_REG;
    unsigned int * AI_LEN_REG;
    unsigned int * AI_CONTROL_REG;
    unsigned int * AI_STATUS_REG;
    unsigned int * AI_DACRATE_REG;
    unsigned int * AI_BITRATE_REG;

    void (*CheckInterrupts)(void);
} AUDIO_INFO;
*/
/*
typedef struct {
    int Present;
    int RawData;
    int  Plugin;
} CONTROL;

typedef union {
    unsigned int Value;
    struct {
        unsigned R_DPAD       : 1;
        unsigned L_DPAD       : 1;
        unsigned D_DPAD       : 1;
        unsigned U_DPAD       : 1;
        unsigned START_BUTTON : 1;
        unsigned Z_TRIG       : 1;
        unsigned B_BUTTON     : 1;
        unsigned A_BUTTON     : 1;

        unsigned R_CBUTTON    : 1;
        unsigned L_CBUTTON    : 1;
        unsigned D_CBUTTON    : 1;
        unsigned U_CBUTTON    : 1;
        unsigned R_TRIG       : 1;
        unsigned L_TRIG       : 1;
        unsigned Reserved1    : 1;
        unsigned Reserved2    : 1;

        signed   X_AXIS       : 8;
        signed   Y_AXIS       : 8;
    };
} BUTTONS;
*/

//typedef struct {
 //  CONTROL *Controls;      /* A pointer to an array of 4 controllers .. eg:
  //                             CONTROL Controls[4]; */
//
//} CONTROL_INFO;

/* common plugin function pointer types */
typedef void (*ptr_RomClosed)(void);
typedef int  (*ptr_RomOpen)(void);
#if defined(M64P_PLUGIN_PROTOTYPES)
extern int   VIDEO_RomOpen(void);
extern void  VIDEO_RomClosed(void);
extern int   AUDIO_RomOpen(void);
extern void  AUDIO_RomClosed(void);
extern int   INPUT_RomOpen(void);
extern void  INPUT_RomClosed(void);
extern int   RSP_RomOpen(void);
extern void  RSP_RomClosed(void);
#endif

/* video plugin function pointer types */
typedef void (*ptr_ChangeWindow)(void);
typedef int  (*ptr_InitiateGFX)(GFX_INFO Gfx_Info);
typedef void (*ptr_MoveScreen)(int x, int y);
typedef void (*ptr_ProcessDList)(void);
typedef void (*ptr_ProcessRDPList)(void);
typedef void (*ptr_ShowCFB)(void);
typedef void (*ptr_UpdateScreen)(void);
typedef void (*ptr_ViStatusChanged)(void);
typedef void (*ptr_ViWidthChanged)(void);
typedef void (*ptr_ReadScreen2)(void *dest, int *width, int *height, int front);
typedef void (*ptr_SetRenderingCallback)(void (*callback)(int));
typedef void (*ptr_ResizeVideoOutput)(int width, int height);
#if defined(M64P_PLUGIN_PROTOTYPES)
/*extern void  VIDEO_ChangeWindow(void);
extern int   VIDEO_InitiateGFX(GFX_INFO Gfx_Info);
extern void  VIDEO_MoveScreen(int x, int y);
extern void  VIDEO_ProcessDList(void);
extern void  VIDEO_ProcessRDPList(void);
extern void  VIDEO_ShowCFB(void);
extern void  VIDEO_UpdateScreen(void);
extern void  VIDEO_ViStatusChanged(void);
extern void  VIDEO_ViWidthChanged(void);
extern void  VIDEO_ReadScreen2(void *dest, int *width, int *height, int front);
extern void  VIDEO_SetRenderingCallback(void (*callback)(int));
extern void  VIDEO_ResizeVideoOutput(int width, int height);*/
#endif

/* frame buffer plugin spec extension */
/*
typedef struct FrameBufferInfo2
{
   unsigned int addr;
   unsigned int size;
   unsigned int width;
   unsigned int height;
} FrameBufferInfo2;


typedef void (*ptr_FBRead)(DWORD addr);
typedef void (*ptr_FBWrite)(DWORD addr, DWORD size);
typedef void (*ptr_FBGetFrameBufferInfo)(void *p);
#if defined(M64P_PLUGIN_PROTOTYPES)
extern void  VIDEO_FBRead(DWORD addr);
extern void  VIDEO_FBWrite(DWORD addr, DWORD size);
extern void  VIDEO_FBGetFrameBufferInfo(void *p);
#endif
*/

/* audio plugin function pointers */
typedef void (*ptr_AiDacrateChanged)(int SystemType);
typedef void (*ptr_AiLenChanged)(void);
typedef int  (*ptr_InitiateAudio)(AUDIO_INFO Audio_Info);
typedef void (*ptr_ProcessAList)(void);
typedef void (*ptr_SetSpeedFactor)(int percent);
typedef void (*ptr_VolumeUp)(void);
typedef void (*ptr_VolumeDown)(void);
typedef int  (*ptr_VolumeGetLevel)(void);
typedef void (*ptr_VolumeSetLevel)(int level);
typedef void (*ptr_VolumeMute)(void);
typedef const char * (*ptr_VolumeGetString)(void);
#if defined(M64P_PLUGIN_PROTOTYPES)
/*void (*AUDIO_AiDacrateChanged)(int SystemType);
void (*AUDIO_AiLenChanged)(void);
int (*AUDIO_InitiateAudio)(AUDIO_INFO Audio_Info);
void (*AUDIO_ProcessAList)(void);
void(*AUDIO_SetSpeedFactor)(int percent);
void( *AUDIO_VolumeUp)(void);
void ( *AUDIO_VolumeDown)(void);
int ( *AUDIO_VolumeGetLevel)(void);
void ( *AUDIO_VolumeSetLevel)(int level);
void ( *AUDIO_VolumeMute)(void);
const char *  AUDIO_VolumeGetString(void);*/
#endif

/* input plugin function pointers */
typedef void (*ptr_ControllerCommand)(int Control, unsigned char *Command);
typedef void (*ptr_GetKeys)(int Control, BUTTONS *Keys);
typedef void (*ptr_InitiateControllers)(HWND hwnnd, CONTROL Controls[4]);
typedef void (*ptr_ReadController)(int Control, unsigned char *Command);
#if defined(M64P_PLUGIN_PROTOTYPES)
extern void  INPUT_ControllerCommand(int Control, unsigned char *Command);
extern void  INPUT_GetKeys(int Control, BUTTONS *Keys);
extern void  INPUT_InitiateControllers(HWND hwnnd, CONTROL Controls[4]);
extern void  INPUT_ReadController(int Control, unsigned char *Command);
#endif

/* RSP plugin function pointers */
typedef DWORD (*ptr_DoRspCycles)(DWORD Cycles);
typedef void (*ptr_InitiateRSP)(RSP_INFO Rsp_Info, DWORD *CycleCount);
#if defined(M64P_PLUGIN_PROTOTYPES)
extern DWORD  RSP_DoRspCycles(DWORD Cycles);
extern void  RSP_InitiateRSP(RSP_INFO Rsp_Info, DWORD *CycleCount);
#endif
#endif
#ifdef __cplusplus
}
#endif

#endif /* M64P_PLUGIN_H */


