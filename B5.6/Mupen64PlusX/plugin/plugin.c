/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Mupen64plus - plugin.c                                                *
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

#include <stdlib.h>

#include "plugin.h"

#include "../api/callbacks.h"
#include "../api/m64p_common.h"
#include "../api/m64p_plugin.h"
#include "../api/m64p_types.h"

#include "../main/rom.h"
#include "../main/version.h"
#include "../memory/memory.h"

static unsigned int dummy;

/* local functions */
static void EmptyFunc(void)
{
}

/* local data structures and functions */
#if !defined(_STATIC_PLUGINS_)
#define DEFINE_GFX(X) \
    EXPORT m64p_error CALL X##PluginGetVersion(m64p_plugin_type *, int *, int *, const char **, int *); \
    EXPORT void CALL X##ChangeWindow(void); \
    EXPORT int  CALL X##InitiateGFX(GFX_INFO Gfx_Info); \
    EXPORT void CALL X##MoveScreen(int x, int y); \
    EXPORT void CALL X##ProcessDList(void); \
    EXPORT void CALL X##ProcessRDPList(void); \
    EXPORT void CALL X##RomClosed(void); \
    EXPORT int  CALL X##RomOpen(void); \
    EXPORT void CALL X##ShowCFB(void); \
    EXPORT void CALL X##UpdateScreen(void); \
    EXPORT void CALL X##ViStatusChanged(void); \
    EXPORT void CALL X##ViWidthChanged(void); \
    EXPORT void CALL X##ReadScreen2(void *dest, int *width, int *height, int front); \
    EXPORT void CALL X##SetRenderingCallback(void (*callback)(int)); \
    EXPORT void CALL X##ResizeVideoOutput(int width, int height); \
    EXPORT void CALL X##FBRead(unsigned int addr); \
    EXPORT void CALL X##FBWrite(unsigned int addr, unsigned int size); \
    EXPORT void CALL X##FBGetFrameBufferInfo(void *p); \
    \
    static const gfx_plugin_functions gfx_##X = { \
        X##PluginGetVersion, \
        X##ChangeWindow, \
        X##InitiateGFX, \
        X##MoveScreen, \
        X##ProcessDList, \
        X##ProcessRDPList, \
        X##RomClosed, \
        X##RomOpen, \
        X##ShowCFB, \
        X##UpdateScreen, \
        X##ViStatusChanged, \
        X##ViWidthChanged, \
        X##ReadScreen2, \
        X##SetRenderingCallback, \
        X##FBRead, \
        X##FBWrite, \
        X##FBGetFrameBufferInfo \
    }

DEFINE_GFX(rice);
DEFINE_GFX(gln64);
DEFINE_GFX(glide64);
#else
    //extern m64p_error CALL X##PluginGetVersion(m64p_plugin_type *, int *, int *, const char **, int *); // Not in RiceXDK
    //extern void _VIDEO_ChangeWindow(void);
    extern BOOL _VIDEO_InitiateGFX(GFX_INFO Gfx_Info);
    extern void _VIDEO_MoveScreen(int x, int y);
    extern void _VIDEO_ProcessDList(void);
    extern void _VIDEO_ProcessRDPList(void);
    extern void _VIDEO_RomClosed(void);
    extern int  _VIDEO_RomOpen(void);
    extern void _VIDEO_ShowCFB(void);
    extern void _VIDEO_UpdateScreen(void);
    extern void _VIDEO_ViStatusChanged(void);
    extern void _VIDEO_ViWidthChanged(void);
    extern void _VIDEO_ReadScreen2(void *dest, int *width, int *height, int front);
    extern void _VIDEO_SetRenderingCallback(void (*callback)(int));
    extern void _VIDEO_ResizeVideoOutput(int width, int height);
    extern void _VIDEO_FBRead(unsigned int addr);
    extern void _VIDEO_FBWrite(unsigned int addr, unsigned int size);
    extern void _VIDEO_FBGetFrameBufferInfo(void *p);

	static const gfx_plugin_functions gfx_VIDEO = {
        //_VIDEO_PluginGetVersion, 
        //_VIDEO_ChangeWindow,
        *_VIDEO_InitiateGFX,
        *_VIDEO_MoveScreen,
        *_VIDEO_ProcessDList,
        *_VIDEO_ProcessRDPList,
        *_VIDEO_RomClosed,
        *_VIDEO_RomOpen,
        *_VIDEO_ShowCFB,
        *_VIDEO_UpdateScreen,
        *_VIDEO_ViStatusChanged,
        *_VIDEO_ViWidthChanged,
        *_VIDEO_ReadScreen2,
        *_VIDEO_SetRenderingCallback,
        *_VIDEO_FBRead,
        *_VIDEO_FBWrite,
		*_VIDEO_FBGetFrameBufferInfo,
	};

#endif

gfx_plugin_functions gfx;
static GFX_INFO gfx_info;

static m64p_error plugin_start_gfx(void)
{
    /* fill in the GFX_INFO data structure */
    gfx_info.HEADER = (unsigned char *) rom;
    gfx_info.RDRAM = (unsigned char *) rdram;
    gfx_info.DMEM = (unsigned char *) SP_DMEM;
    gfx_info.IMEM = (unsigned char *) SP_IMEM;
    gfx_info.MI_INTR_REG = &(MI_register.mi_intr_reg);
    gfx_info.DPC_START_REG = &(dpc_register.dpc_start);
    gfx_info.DPC_END_REG = &(dpc_register.dpc_end);
    gfx_info.DPC_CURRENT_REG = &(dpc_register.dpc_current);
    gfx_info.DPC_STATUS_REG = &(dpc_register.dpc_status);
    gfx_info.DPC_CLOCK_REG = &(dpc_register.dpc_clock);
    gfx_info.DPC_BUFBUSY_REG = &(dpc_register.dpc_bufbusy);
    gfx_info.DPC_PIPEBUSY_REG = &(dpc_register.dpc_pipebusy);
    gfx_info.DPC_TMEM_REG = &(dpc_register.dpc_tmem);
    gfx_info.VI_STATUS_REG = &(vi_register.vi_status);
    gfx_info.VI_ORIGIN_REG = &(vi_register.vi_origin);
    gfx_info.VI_WIDTH_REG = &(vi_register.vi_width);
    gfx_info.VI_INTR_REG = &(vi_register.vi_v_intr);
    gfx_info.VI_V_CURRENT_LINE_REG = &(vi_register.vi_current);
    gfx_info.VI_TIMING_REG = &(vi_register.vi_burst);
    gfx_info.VI_V_SYNC_REG = &(vi_register.vi_v_sync);
    gfx_info.VI_H_SYNC_REG = &(vi_register.vi_h_sync);
    gfx_info.VI_LEAP_REG = &(vi_register.vi_leap);
    gfx_info.VI_H_START_REG = &(vi_register.vi_h_start);
    gfx_info.VI_V_START_REG = &(vi_register.vi_v_start);
    gfx_info.VI_V_BURST_REG = &(vi_register.vi_v_burst);
    gfx_info.VI_X_SCALE_REG = &(vi_register.vi_x_scale);
    gfx_info.VI_Y_SCALE_REG = &(vi_register.vi_y_scale);
    gfx_info.CheckInterrupts = EmptyFunc;

    /* call the audio plugin */
    if (!gfx.initiateGFX(gfx_info))
        return M64ERR_PLUGIN_FAIL;

    return M64ERR_SUCCESS;
}

/* AUDIO */
extern void _AUDIO_M64P_AiDacrateChanged(int SystemType);
extern void _AUDIO_M64P_AiLenChanged(void);
extern int  _AUDIO_M64P_InitiateAudio(AUDIO_INFO Audio_Info);
extern void _AUDIO_M64P_ProcessAList(void);
extern int  _AUDIO_M64P_RomOpen(void);
extern void _AUDIO_M64P_RomClosed(void);
extern void _AUDIO_M64P_SetSpeedFactor(int percent);
extern void _AUDIO_M64P_VolumeUp(void);
extern void _AUDIO_M64P_VolumeDown(void);
extern int _AUDIO_M64P_VolumeGetLevel(void);
extern void _AUDIO_M64P_VolumeSetLevel(int level);
extern void _AUDIO_M64P_VolumeMute(void);
extern const char * _AUDIO_M64P_VolumeGetString(void);

#if defined(_XBOX)
static const audio_plugin_functions audio_M64P = {
    _AUDIO_M64P_AiDacrateChanged,
    _AUDIO_M64P_AiLenChanged,
    _AUDIO_M64P_InitiateAudio,
    _AUDIO_M64P_ProcessAList,
    _AUDIO_M64P_RomClosed,
    _AUDIO_M64P_RomOpen,
    _AUDIO_M64P_SetSpeedFactor,
	_AUDIO_M64P_VolumeUp,
    _AUDIO_M64P_VolumeDown,
    _AUDIO_M64P_VolumeGetLevel,
    _AUDIO_M64P_VolumeSetLevel,
    _AUDIO_M64P_VolumeMute,
    _AUDIO_M64P_VolumeGetString
};
#else
audio_plugin_functions audio = {
    (ptr_PluginGetVersion)EmptyFunc,
    audioAiDacrateChanged,
    audioAiLenChanged,
    audioInitiateAudio,
    audioProcessAList,
    audioRomClosed,
    audioRomOpen,
    audioSetSpeedFactor,
    audioVolumeUp,
    audioVolumeDown,
    audioVolumeGetLevel,
    audioVolumeSetLevel,
    audioVolumeMute,
    audioVolumeGetString
};
#endif

audio_plugin_functions audio;

static AUDIO_INFO audio_info;

static m64p_error plugin_start_audio(void)
{
    /* fill in the AUDIO_INFO data structure */
    audio_info.RDRAM = (unsigned char *) rdram;
    audio_info.DMEM = (unsigned char *) SP_DMEM;
    audio_info.IMEM = (unsigned char *) SP_IMEM;
    audio_info.MI_INTR_REG = &(MI_register.mi_intr_reg);
    audio_info.AI_DRAM_ADDR_REG = &(ai_register.ai_dram_addr);
    audio_info.AI_LEN_REG = &(ai_register.ai_len);
    audio_info.AI_CONTROL_REG = &(ai_register.ai_control);
    audio_info.AI_STATUS_REG = &dummy;
    audio_info.AI_DACRATE_REG = &(ai_register.ai_dacrate);
    audio_info.AI_BITRATE_REG = &(ai_register.ai_bitrate);
    audio_info.CheckInterrupts = EmptyFunc;

    /* call the audio plugin */
    if (!audio.initiateAudio(audio_info))
        return M64ERR_PLUGIN_FAIL;

    return M64ERR_SUCCESS;
}

/* INPUT */
/*extern m64p_error inputPluginGetVersion(m64p_plugin_type *PluginType, int *PluginVersion,
                                              int *APIVersion, const char **PluginNamePtr, int *Capabilities);*/
extern void _INPUT_InitiateControllers (CONTROL_INFO ControlInfo);
extern void _INPUT_GetKeys(int Control, BUTTONS * Keys );
extern void _INPUT_ControllerCommand(int Control, unsigned char *Command);
extern void _INPUT_GetKeys(int Control, BUTTONS * Keys);
extern void _INPUT_InitiateControllers(CONTROL_INFO ControlInfo);
extern void _INPUT_ReadController(int Control, unsigned char *Command);
extern int  _INPUT_RomOpen(void);
extern void _INPUT_RomClosed(void);

static const input_plugin_functions input_INPUT = {
   //_INPUT_PluginGetVersion,
    _INPUT_ControllerCommand,
    _INPUT_GetKeys,
    _INPUT_InitiateControllers,
    _INPUT_ReadController,
    _INPUT_RomClosed,
    _INPUT_RomOpen,
};

input_plugin_functions input;

static CONTROL_INFO control_info;
CONTROL Controls[4];

static m64p_error plugin_start_input(void)
{
    int i;

    /* fill in the CONTROL_INFO data structure */
    control_info.Controls = Controls;
    for (i=0; i<4; i++)
      {
         Controls[i].Present = 0;
         Controls[i].RawData = 0;
         Controls[i].Plugin = PLUGIN_NONE;
      }

    /* call the input plugin */
    input.initiateControllers(control_info);

    return M64ERR_SUCCESS;
}

/* RSP */
extern m64p_error rspPluginGetVersion(m64p_plugin_type *PluginType, int *PluginVersion,
                                            int *APIVersion, const char **PluginNamePtr, int *Capabilities);
extern unsigned int _RSP_M64p_DoRspCycles(unsigned int Cycles);
extern void _RSP_M64p_InitiateRSP(RSP_INFO Rsp_Info, unsigned int *CycleCount);
extern void _RSP_M64p_RomClosed(void);

static const rsp_plugin_functions rsp_RSP_M64P = {
    //_RSP_M64p_PluginGetVersion,
    *_RSP_M64p_DoRspCycles,
    *_RSP_M64p_InitiateRSP,
    *_RSP_M64p_RomClosed
};

rsp_plugin_functions rsp;

static RSP_INFO rsp_info;

static m64p_error plugin_start_rsp(void)
{
    /* fill in the RSP_INFO data structure */
    rsp_info.RDRAM = (unsigned char *) rdram;
    rsp_info.DMEM = (unsigned char *) SP_DMEM;
    rsp_info.IMEM = (unsigned char *) SP_IMEM;
    rsp_info.MI_INTR_REG = &MI_register.mi_intr_reg;
    rsp_info.SP_MEM_ADDR_REG = &sp_register.sp_mem_addr_reg;
    rsp_info.SP_DRAM_ADDR_REG = &sp_register.sp_dram_addr_reg;
    rsp_info.SP_RD_LEN_REG = &sp_register.sp_rd_len_reg;
    rsp_info.SP_WR_LEN_REG = &sp_register.sp_wr_len_reg;
    rsp_info.SP_STATUS_REG = &sp_register.sp_status_reg;
    rsp_info.SP_DMA_FULL_REG = &sp_register.sp_dma_full_reg;
    rsp_info.SP_DMA_BUSY_REG = &sp_register.sp_dma_busy_reg;
    rsp_info.SP_PC_REG = &rsp_register.rsp_pc;
    rsp_info.SP_SEMAPHORE_REG = &sp_register.sp_semaphore_reg;
    rsp_info.DPC_START_REG = &dpc_register.dpc_start;
    rsp_info.DPC_END_REG = &dpc_register.dpc_end;
    rsp_info.DPC_CURRENT_REG = &dpc_register.dpc_current;
    rsp_info.DPC_STATUS_REG = &dpc_register.dpc_status;
    rsp_info.DPC_CLOCK_REG = &dpc_register.dpc_clock;
    rsp_info.DPC_BUFBUSY_REG = &dpc_register.dpc_bufbusy;
    rsp_info.DPC_PIPEBUSY_REG = &dpc_register.dpc_pipebusy;
    rsp_info.DPC_TMEM_REG = &dpc_register.dpc_tmem;
    rsp_info.CheckInterrupts = EmptyFunc;
    rsp_info.ProcessDlistList = gfx.processDList;
    rsp_info.ProcessAlistList = audio.processAList;
    rsp_info.ProcessRdpList = gfx.processRDPList;
    rsp_info.ShowCFB = gfx.showCFB;

    /* call the RSP plugin  */
    rsp.initiateRSP(rsp_info, NULL);

    return M64ERR_SUCCESS;
}

/* global functions */
void plugin_connect_all(enum gfx_plugin_type gfx_plugin)
{
#if !defined(_STATIC_PLUGINS_)
    switch (gfx_plugin)
    {
        case GFX_RICE:  gfx = gfx_rice; break;
        case GFX_GLN64: gfx = gfx_gln64; break;
        default:        gfx = gfx_glide64; break;
    }
#endif

    plugin_start_gfx();
    plugin_start_audio();
    plugin_start_input();
    plugin_start_rsp();
}
