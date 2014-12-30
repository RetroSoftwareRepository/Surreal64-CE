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
#include "../plugin/plugins.h"

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
    extern int   X##InitiateGFX(GFX_INFO Gfx_Info); \
    extern void  X##MoveScreen(int x, int y); \
    extern void  X##ProcessDList(void); \
    extern void  X##ProcessRDPList(void); \
    extern void  X##RomClosed(void); \
    extern int  X##RomOpen(void); \
    extern void  X##ShowCFB(void); \
    extern void  X##UpdateScreen(void); \
    extern void  X##ViStatusChanged(void); \
    extern void  X##ViWidthChanged(void); \
    extern void  X##ReadScreen2(void *dest, int *width, int *height, int front); \
    extern void  X##SetRenderingCallback(void (*callback)(int)); \
    extern void  X##ResizeVideoOutput(int width, int height); \
    extern void  X##FBRead(DWORD addr); \
    extern void  X##FBWrite(DWORD addr, DWORD size); \
    extern void  X##FBGetFrameBufferInfo(void *p); \
    \

DEFINE_GFX(VIDEO_);
//DEFINE_GFX(gln64);
//DEFINE_GFX(glide64);
#else
    //extern m64p_error CALL X##PluginGetVersion(m64p_plugin_type *, int *, int *, const char **, int *); // Not in RiceXDK
    //extern void VIDEO_ChangeWindow(void);
    extern BOOL VIDEO_InitiateGFX(GFX_INFO Gfx_Info);
    extern void VIDEO_MoveScreen(int x, int y);
    extern void VIDEO_ProcessDList(void);
    extern void VIDEO_ProcessRDPList(void);
    extern void VIDEO_RomClosed(void);
    extern int  VIDEO_RomOpen(void);
    extern void VIDEO_ShowCFB(void);
    extern void VIDEO_UpdateScreen(void);
    extern void VIDEO_ViStatusChanged(void);
    extern void VIDEO_ViWidthChanged(void);
    extern void VIDEO_ReadScreen2(void *dest, int *width, int *height, int front);
    extern void VIDEO_SetRenderingCallback(void (*callback)(int));
    extern void VIDEO_ResizeVideoOutput(int width, int height);
    extern void VIDEO_FBRead(unsigned int addr);
    extern void VIDEO_FBWrite(unsigned int addr, unsigned int size);
    extern void VIDEO_FBGetFrameBufferInfo(void *p);

#endif

//gfx_plugin_functions gfx;
//GFX_INFO	Gfx_Info;
//FrameBufferInfo FrameBufferInfos[6];


static m64p_error plugin_start_gfx(void)
{

    /* fill in the GFX_INFO data structure */
    Gfx_Info.HEADER = (unsigned char *) rom;
    Gfx_Info.RDRAM = (unsigned char *) rdram;
    Gfx_Info.DMEM = (unsigned char *) SP_DMEM;
    Gfx_Info.IMEM = (unsigned char *) SP_IMEM;
    Gfx_Info.MI_INTR_RG = (DWORD *)&(MI_register.mi_intr_reg);
    Gfx_Info.DPC_START_RG = (DWORD *)&(dpc_register.dpc_start);
    Gfx_Info.DPC_END_RG = (DWORD *)&(dpc_register.dpc_end);
    Gfx_Info.DPC_CURRENT_RG = (DWORD *)&(dpc_register.dpc_current);
    Gfx_Info.DPC_STATUS_RG = (DWORD *)&(dpc_register.dpc_status);
    Gfx_Info.DPC_CLOCK_RG = (DWORD *)&(dpc_register.dpc_clock);
    Gfx_Info.DPC_BUFBUSY_RG = (DWORD *)&(dpc_register.dpc_bufbusy);
    Gfx_Info.DPC_PIPEBUSY_RG = (DWORD *)&(dpc_register.dpc_pipebusy);
    Gfx_Info.DPC_TMEM_RG = (DWORD *)&(dpc_register.dpc_tmem);
    Gfx_Info.VI_STATUS_RG = (DWORD *)&(vi_register.vi_status);
    Gfx_Info.VI_ORIGIN_RG = (DWORD *)&(vi_register.vi_origin);
    Gfx_Info.VI_WIDTH_RG = (DWORD *)&(vi_register.vi_width);
    Gfx_Info.VI_INTR_RG = (DWORD *)&(vi_register.vi_v_intr);
    Gfx_Info.VI_V_CURRENT_LINE_RG = (DWORD *)&(vi_register.vi_current);
    Gfx_Info.VI_TIMING_RG = (DWORD *)&(vi_register.vi_burst);
    Gfx_Info.VI_V_SYNC_RG = (DWORD *)&(vi_register.vi_v_sync);
    Gfx_Info.VI_H_SYNC_RG = (DWORD *)&(vi_register.vi_h_sync);
    Gfx_Info.VI_LEAP_RG = (DWORD *)&(vi_register.vi_leap);
    Gfx_Info.VI_H_START_RG = (DWORD *)&(vi_register.vi_h_start);
    Gfx_Info.VI_V_START_RG = (DWORD *)&(vi_register.vi_v_start);
    Gfx_Info.VI_V_BURST_RG = (DWORD *)&(vi_register.vi_v_burst);
    Gfx_Info.VI_X_SCALE_RG = (DWORD *)&(vi_register.vi_x_scale);
    Gfx_Info.VI_Y_SCALE_RG = (DWORD *)&(vi_register.vi_y_scale);
    Gfx_Info.CheckInterrupts = EmptyFunc;

    /* call the audio plugin */
    if (!VIDEO_InitiateGFX(Gfx_Info))
        return M64ERR_PLUGIN_FAIL;

    return M64ERR_SUCCESS;
}

/* AUDIO */



static m64p_error plugin_start_audio(void)
{
	AUDIO_INFO Audio_Info;

    /* fill in the AUDIO_INFO data structure */
    Audio_Info.RDRAM = (unsigned char *) rdram;
    Audio_Info.DMEM = (unsigned char *) SP_DMEM;
    Audio_Info.IMEM = (unsigned char *) SP_IMEM;
    Audio_Info.MI_INTR_RG = (DWORD *)&(MI_register.mi_intr_reg);
    Audio_Info.AI_DRAM_ADDR_RG = (DWORD *)&(ai_register.ai_dram_addr);
    Audio_Info.AI_LEN_RG = (DWORD *)&(ai_register.ai_len);
    Audio_Info.AI_CONTROL_RG = (DWORD *)&(ai_register.ai_control);
    Audio_Info.AI_STATUS_RG = (DWORD *)&dummy;
    Audio_Info.AI_DACRATE_RG = (DWORD *)&(ai_register.ai_dacrate);
    Audio_Info.AI_BITRATE_RG = (DWORD *)&(ai_register.ai_bitrate);
    Audio_Info.CheckInterrupts = EmptyFunc;

    /* call the audio plugin */
    if (!AUDIO_InitiateAudio(Audio_Info))
        return M64ERR_PLUGIN_FAIL;

    return M64ERR_SUCCESS;
}

/* INPUT */
/*extern m64p_error inputPluginGetVersion(m64p_plugin_type *PluginType, int *PluginVersion,
                                              int *APIVersion, const char **PluginNamePtr, int *Capabilities);*/
extern void INPUT_InitiateControllers (HWND hMainWindow, CONTROL Controls[4]);
extern void INPUT_GetKeys(int Control, BUTTONS * Keys );
extern void INPUT_ControllerCommand(int Control, unsigned char *Command);
extern void INPUT_GetKeys(int Control, BUTTONS * Keys);
extern void INPUT_InitiateControllers(CONTROL_INFO ControlInfo);
extern void INPUT_ReadController(int Control, unsigned char *Command);
extern int INPUT_RomOpen(void);
extern void INPUT_RomClosed(void);


static CONTROL_INFO control_info;
CONTROL Controls[4];

static m64p_error plugin_start_input(void)
{
	HWND hwnd;
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
    INPUT_InitiateControllers(hwnd,Controls);

    return M64ERR_SUCCESS;
}

/* RSP */
extern m64p_error rspPluginGetVersion(m64p_plugin_type *PluginType, int *PluginVersion,
                                            int *APIVersion, const char **PluginNamePtr, int *Capabilities);
extern unsigned int RSP_DoRspCycles(unsigned int Cycles);
extern void RSP_InitiateRSP(RSP_INFO Rsp_Info, DWORD *CycleCount);
extern void RSP_RomClosed(void);

static RSP_INFO rsp_info;

static m64p_error plugin_start_rsp(void)
{
    /* fill in the RSP_INFO data structure */
    rsp_info.RDRAM = (unsigned char *) rdram;
    rsp_info.DMEM = (unsigned char *) SP_DMEM;
    rsp_info.IMEM = (unsigned char *) SP_IMEM;
    rsp_info.MI_INTR_RG = (DWORD *)&MI_register.mi_intr_reg;
    rsp_info.SP_MEM_ADDR_RG = (DWORD *)&sp_register.sp_mem_addr_reg;
    rsp_info.SP_DRAM_ADDR_RG = (DWORD *)&sp_register.sp_dram_addr_reg;
    rsp_info.SP_RD_LEN_RG = (DWORD *)&sp_register.sp_rd_len_reg;
    rsp_info.SP_WR_LEN_RG = (DWORD *)&sp_register.sp_wr_len_reg;
    rsp_info.SP_STATUS_RG = (DWORD *)&sp_register.sp_status_reg;
    rsp_info.SP_DMA_FULL_RG = (DWORD *)&sp_register.sp_dma_full_reg;
    rsp_info.SP_DMA_BUSY_RG = (DWORD *)&sp_register.sp_dma_busy_reg;
    rsp_info.SP_PC_RG = (DWORD *)&rsp_register.rsp_pc;
    rsp_info.SP_SEMAPHORE_RG = (DWORD *)&sp_register.sp_semaphore_reg;
    rsp_info.DPC_START_RG = (DWORD *)&dpc_register.dpc_start;
    rsp_info.DPC_END_RG = (DWORD *)&dpc_register.dpc_end;
    rsp_info.DPC_CURRENT_RG = (DWORD *)&dpc_register.dpc_current;
    rsp_info.DPC_STATUS_RG = (DWORD *)&dpc_register.dpc_status;
    rsp_info.DPC_CLOCK_RG = (DWORD *)&dpc_register.dpc_clock;
    rsp_info.DPC_BUFBUSY_RG = (DWORD *)&dpc_register.dpc_bufbusy;
    rsp_info.DPC_PIPEBUSY_RG = (DWORD *)&dpc_register.dpc_pipebusy;
    rsp_info.DPC_TMEM_RG = (DWORD *)&dpc_register.dpc_tmem;
    rsp_info.CheckInterrupts = EmptyFunc;
    rsp_info.ProcessDList = VIDEO_ProcessDList;
    rsp_info.ProcessAList = AUDIO_ProcessAList;
    rsp_info.ProcessRdpList = VIDEO_ProcessRDPList;
    rsp_info.ShowCFB = VIDEO_ShowCFB;

    /* call the RSP plugin  */
    RSP_InitiateRSP(rsp_info, NULL);

    return M64ERR_SUCCESS;
}

/* global functions */
void plugin_connect_all(enum gfx_plugin_type gfx_plugin)
{
#if defined(_XBOX)
	//gfx = gfx_VIDEO_;
#else
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
