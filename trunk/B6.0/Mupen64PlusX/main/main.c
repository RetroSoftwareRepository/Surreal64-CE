/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Mupen64plus - main.c                                                  *
 *   Mupen64Plus homepage: http://code.google.com/p/mupen64plus/           *
 *   Copyright (C) 2012 CasualJames                                        *
 *   Copyright (C) 2008-2009 Richard Goedeken                              *
 *   Copyright (C) 2008 Ebenblues Nmn Okaygo Tillin9                       *
 *   Copyright (C) 2002 Hacktarux                                          *
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

/* This is MUPEN64's main entry point. It contains code that is common
 * to both the gui and non-gui versions of mupen64. See
 * gui subdirectories for the gui-specific code.
 * if you want to implement an interface, you should look here
 */

#include <string.h>
#include <stdlib.h>

#include <stdio.h>
#include <stdarg.h>
#include "IOSupport.h"

#define M64P_CORE_PROTOTYPES 1
#include "../api/m64p_types.h"
#include "../api/callbacks.h"
#include "../api/config.h"
#include "../api/m64p_config.h"
#include "../api/debugger.h"
#include "../api/vidext.h"

#include "main.h"
#include "eventloop.h"
#include "rom.h"
#include "savestates.h"
#include "util.h"

#include "../memory/memory.h"
#include "../osal/preproc.h"
#include "../plugin/plugin.h"
#include "../r4300/r4300.h"
#include "../r4300/interupt.h"
#include "../r4300/reset.h"

#ifdef DBG
#include "../debugger/dbg_types.h"
#include "../debugger/debugger.h"
#endif

#ifdef _XBOX
#include "../plugin/Static_Video.h"
#include "../plugin/Static_Audio.h"
#include "../plugin/Static_Input.h"
//#include "../plugin/plugin.h"
#endif

/* version number for Core config section */
#define CONFIG_PARAM_VERSION 1.01

/** globals **/
m64p_handle g_CoreConfig = NULL;

m64p_frame_callback g_FrameCallback = NULL;

int         g_MemHasBeenBSwapped = 0;   // store byte-swapped flag so we don't swap twice when re-playing game
int         g_EmulatorRunning = 0;      // need separate boolean to tell if emulator is running, since --nogui doesn't use a thread

/** static (local) variables **/
static int   l_CurrentFrame = 0;         // frame counter
static int   l_FrameAdvance = 0;         // variable to check if we pause on next frame

/*********************************************************************************************************
* helper functions
*/

void main_message(m64p_msg_level level, unsigned int corner, const char *format, ...)
{
    va_list ap;
    char buffer[2049];
    va_start(ap, format);
    _vsnprintf(buffer, 2047, format, ap);
    buffer[2048]='\0';
    va_end(ap);

    /* send message to front-end */
    DebugMessage(level, "%s", buffer);
}


/*********************************************************************************************************
* global functions, for adjusting the core emulator behavior
*/

int main_set_core_defaults(void)
{
    float fConfigParamsVersion;
    int bSaveConfig = 0, bUpgrade = 0;

    if (ConfigGetParameter(g_CoreConfig, "Version", M64TYPE_FLOAT, &fConfigParamsVersion, sizeof(float)) != M64ERR_SUCCESS)
    {
        DebugMessage(M64MSG_WARNING, "No version number in 'Core' config section. Setting defaults.");
        ConfigDeleteSection("Core");
        ConfigOpenSection("Core", &g_CoreConfig);
        bSaveConfig = 1;
    }
    else if (((int) fConfigParamsVersion) != ((int) CONFIG_PARAM_VERSION))
    {
        DebugMessage(M64MSG_WARNING, "Incompatible version %.2f in 'Core' config section: current is %.2f. Setting defaults.", fConfigParamsVersion, (float) CONFIG_PARAM_VERSION);
        ConfigDeleteSection("Core");
        ConfigOpenSection("Core", &g_CoreConfig);
        bSaveConfig = 1;
    }
    else if ((CONFIG_PARAM_VERSION - fConfigParamsVersion) >= 0.0001f)
    {
        float fVersion = (float) CONFIG_PARAM_VERSION;
        ConfigSetParameter(g_CoreConfig, "Version", M64TYPE_FLOAT, &fVersion);
        DebugMessage(M64MSG_INFO, "Updating parameter set version in 'Core' config section to %.2f", fVersion);
        bUpgrade = 1;
        bSaveConfig = 1;
    }

    /* parameters controlling the operation of the core */
    ConfigSetDefaultFloat(g_CoreConfig, "Version", (float) CONFIG_PARAM_VERSION,  "Mupen64Plus Core config parameter set version number.  Please don't change this version number.");
    ConfigSetDefaultBool(g_CoreConfig, "OnScreenDisplay", 1, "Draw on-screen display if True, otherwise don't draw OSD");
#if defined(DYNAREC)
    ConfigSetDefaultInt(g_CoreConfig, "R4300Emulator", 2, "Use Pure Interpreter if 0, Cached Interpreter if 1, or Dynamic Recompiler if 2 or more");
#else
    ConfigSetDefaultInt(g_CoreConfig, "R4300Emulator", 1, "Use Pure Interpreter if 0, Cached Interpreter if 1, or Dynamic Recompiler if 2 or more");
#endif
    ConfigSetDefaultBool(g_CoreConfig, "NoCompiledJump", 0, "Disable compiled jump commands in dynamic recompiler (should be set to False) ");
    ConfigSetDefaultBool(g_CoreConfig, "DisableExtraMem", 0, "Disable 4MB expansion RAM pack. May be necessary for some games");
    ConfigSetDefaultBool(g_CoreConfig, "EnableDebugger", 0, "Activate the R4300 debugger when ROM execution begins, if core was built with Debugger support");

    if (bSaveConfig)
        ConfigSaveSection("Core");

    return 1;
}

static int main_is_paused(void)
{
    return (g_EmulatorRunning && rompause);
}

void main_toggle_pause(void)
{
    if (!g_EmulatorRunning)
        return;

    if (rompause)
    {
        DebugMessage(M64MSG_STATUS, "Emulation continued.");
        StateChanged(M64CORE_EMU_STATE, M64EMU_RUNNING);
    }
    else
    {
        DebugMessage(M64MSG_STATUS, "Emulation paused.");
        StateChanged(M64CORE_EMU_STATE, M64EMU_PAUSED);
    }

    rompause = !rompause;
    l_FrameAdvance = 0;
}

void main_advance_one(void)
{
    l_FrameAdvance = 1;
    rompause = 0;
    StateChanged(M64CORE_EMU_STATE, M64EMU_RUNNING);
}

m64p_error main_core_state_query(m64p_core_param param, int *rval)
{
    switch (param)
    {
        case M64CORE_EMU_STATE:
            if (!g_EmulatorRunning)
                *rval = M64EMU_STOPPED;
            else if (rompause)
                *rval = M64EMU_PAUSED;
            else
                *rval = M64EMU_RUNNING;
            break;
	case M64COREINPUT_GAMESHARK:
	    *rval = event_gameshark_active();
	    break;
        default:
            return M64ERRINPUT_INVALID;
    }

    return M64ERR_SUCCESS;
}

m64p_error main_core_state_set(m64p_core_param param, int val)
{
    switch (param)
    {
        case M64CORE_EMU_STATE:
            if (!g_EmulatorRunning)
                return M64ERR_INVALID_STATE;
            if (val == M64EMU_STOPPED)
            {        
                /* this stop function is asynchronous.  The emulator may not terminate until later */
                main_stop();
                return M64ERR_SUCCESS;
            }
            else if (val == M64EMU_RUNNING)
            {
                if (main_is_paused())
                    main_toggle_pause();
                return M64ERR_SUCCESS;
            }
            else if (val == M64EMU_PAUSED)
            {    
                if (!main_is_paused())
                    main_toggle_pause();
                return M64ERR_SUCCESS;
            }
            return M64ERRINPUT_INVALID;
	case M64COREINPUT_GAMESHARK:
            if (!g_EmulatorRunning)
                return M64ERR_INVALID_STATE;
	    event_set_gameshark(val);
	    return M64ERR_SUCCESS;
        default:
            return M64ERRINPUT_INVALID;
    }
}

m64p_error main_read_screen(void *pixels, int bFront)
{
//    int width_trash, height_trash;
    //VIDEO_ReadScreen2(pixels, &width_trash, &height_trash, bFront);
    return M64ERR_SUCCESS;
}

m64p_error main_reset(int do_hard_reset)
{
    if (do_hard_reset)
        reset_hard_job |= 1;
    else
        reset_soft();
    return M64ERR_SUCCESS;
}

/*********************************************************************************************************
* global functions, callbacks from the r4300 core or from other plugins
*/

static void video_plugin_render_callback(int bScreenRedrawn)
{
}

void new_frame(void)
{
    if (g_FrameCallback != NULL)
        (*g_FrameCallback)(l_CurrentFrame);

    /* advance the current frame */
    l_CurrentFrame++;

    if (l_FrameAdvance) {
        rompause = 1;
        l_FrameAdvance = 0;
        StateChanged(M64CORE_EMU_STATE, M64EMU_PAUSED);
    }
}

/*********************************************************************************************************
* emulation thread - runs the core
*/
m64p_error main_run(void)
{
    /* take the r4300 emulator mode from the config file at this point and cache it in a global variable */
    r4300emu = ConfigGetParamInt(g_CoreConfig, "R4300Emulator");

    /* set some other core parameters based on the config file values */
    no_compiled_jump = ConfigGetParamBool(g_CoreConfig, "NoCompiledJump");

    // initialize memory, and do byte-swapping if it's not been done yet
    if (g_MemHasBeenBSwapped == 0)
    {
        init_memory(1);
        g_MemHasBeenBSwapped = 1;
    }
    else
    {
        init_memory(0);
    }

    // Attach rom to plugins
	VIDEO_RomOpen();
	//AUDIO_RomOpen();
	INPUT_RomOpen();
    /*if (!VIDEO_RomOpen())
    {
        free_memory(); return M64ERR_PLUGIN_FAIL;
    }
    if (!AUDIO_RomOpen())
    {
        VIDEO_RomClosed(); free_memory(); return M64ERR_PLUGIN_FAIL;
    }
    if (!input.romOpen())
    {
       AUDIO_RomClosed(); VIDEO_RomClosed(); free_memory(); return M64ERR_PLUGIN_FAIL;
    }*/

    // setup rendering callback from video plugin to the core, for screenshots and On-Screen-Display
    //VIDEO_SetRenderingCallback(video_plugin_render_callback);

#ifdef DBG
    if (ConfigGetParamBool(g_CoreConfig, "EnableDebugger"))
        init_debugger();
#endif

    g_EmulatorRunning = 1;
    StateChanged(M64CORE_EMU_STATE, M64EMU_RUNNING);

    /* call r4300 CPU core and run the game */
    r4300_reset_hard();
    r4300_reset_soft();
    r4300_execute();

    /* now begin to shut down */
#ifdef DBG
    if (g_DebuggerActive)
        destroy_debugger();
#endif

   /* rsp.romClosed();
    input.romClosed();
    AUDIO_RomClosed();
    VIDEO_RomClosed();
    */
	free_memory();

    // clean up
    g_EmulatorRunning = 0;
    StateChanged(M64CORE_EMU_STATE, M64EMU_STOPPED);

    return M64ERR_SUCCESS;
}

void main_stop(void)
{
    /* note: this operation is asynchronous.  It may be called from a thread other than the
       main emulator thread, and may return before the emulator is completely stopped */
    if (!g_EmulatorRunning)
        return;

    DebugMessage(M64MSG_STATUS, "Stopping emulation.");
    if (rompause)
    {
        rompause = 0;
        StateChanged(M64CORE_EMU_STATE, M64EMU_RUNNING);
    }
    stop = 1;
#ifdef DBG
    if(g_DebuggerActive)
    {
        debugger_step();
    }
#endif        
}
char g_szPathSaves[256] = "D:\\Saves\\";
extern void GetPathSaves(char *pszPathSaves);

BOOL PathFileExists(const char *pszPath)
{   
    return GetFileAttributes(pszPath) != INVALID_FILE_ATTRIBUTES;   
}
char g_temporaryRomPath[260];
extern "C" char emuname[256];
char emuname[256];
void _cdecl main(){
	// mount the common drives
	Mount("A:","cdrom0");
	Mount("E:","Harddisk0\\Partition1");
	//Mount("C:","Harddisk0\\Partition2");
	//Mount("X:","Harddisk0\\Partition3");
	//Mount("Y:","Harddisk0\\Partition4");
	//Mount("Z:","Harddisk0\\Partition5");
	Mount("F:","Harddisk0\\Partition6");
	Mount("G:","Harddisk0\\Partition7");
	
	// utility shoud be mounted automatically
	if(XGetDiskSectorSize("Z:\\") == 0)
		Mount("Z:","Harddisk0\\Partition5");
	
	// make sure there's a temp rom
	if (PathFileExists("Z:\\TemporaryRom.dat")) {
		OutputDebugString("Z:\\TemporaryRom.dat File Found!\n");
		strcpy(g_temporaryRomPath, "Z:\\TemporaryRom.dat");
	}
	else {
		OutputDebugString("Z:\\TemporaryRom.dat File Not Found!\n");
		
		// if debugging, a temp rom can be placed in T to skip the launcher
		if (PathFileExists("T:\\Data\\TemporaryRom.dat")) {
			OutputDebugString("T:\\Data\\TemporaryRom.dat File Found!\n");
			strcpy(g_temporaryRomPath, "T:\\Data\\TemporaryRom.dat");
		}
		else {
			OutputDebugString("T:\\Data\\TemporaryRom.dat File Not Found!\n");
			Sleep(100);
			XLaunchNewImage("D:\\default.xbe", NULL);
		}
	}
 /* get the length of the ROM, allocate memory buffer, load it from disk */
    FILE *fPtr = fopen(g_temporaryRomPath, "rb");
	long romlength = 0;
    fseek(fPtr, 0L, SEEK_END);
    romlength = ftell(fPtr);
    fseek(fPtr, 0L, SEEK_SET);
    unsigned char *ROM_buffer = (unsigned char *) malloc(romlength);
    if (ROM_buffer == NULL)
    {
        DebugMessage(M64MSG_ERROR, "couldn't allocate %li-byte buffer for ROM image file '%s'.", romlength, g_temporaryRomPath);
        fclose(fPtr);
        (*CoreShutdown)();
        //DetachCoreLib();
        //return 8;
    }
    else if (fread(ROM_buffer, 1, romlength, fPtr) != romlength)
    {
        DebugMessage(M64MSG_ERROR, "couldn't read %li bytes from ROM image file '%s'.", romlength, g_temporaryRomPath);
        free(ROM_buffer);
        fclose(fPtr);
        (*CoreShutdown)();
        //DetachCoreLib();
        //return 9;
    }
    fclose(fPtr);

    /* Try to load the ROM image into the core */
    if (open_rom((const unsigned char *) ROM_buffer, (int) romlength) != M64ERR_SUCCESS)
    {
        DebugMessage(M64MSG_ERROR, "core failed to open ROM image file '%s'.", g_temporaryRomPath);
        free(ROM_buffer);
        (*CoreShutdown)();
        //DetachCoreLib();
        //return 10;
    }
    free(ROM_buffer); /* the core copies the ROM image, so we can release this buffer immediately */

	//attach plugins
	//plugin_link_gfx();
	/* run the game */
    main_run();
}

void __cdecl DisplayError (char * Message, ...)
{
//#ifdef DEBUG
	char Msg[400];
	va_list ap;

	va_start( ap, Message );
	vsprintf( Msg, Message, ap );
	va_end( ap );
	
	OutputDebugString(Msg);
/*#else
	OutputDebugString(Message);
#endif*/
	OutputDebugString("\n");
}

