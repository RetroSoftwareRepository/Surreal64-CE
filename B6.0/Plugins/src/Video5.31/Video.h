/**********************************************************************************
Common gfx plugin spec, version #1.2 maintained by zilmar (zilmar@emulation64.com)

All questions or suggestions should go through the mailing list.
http://www.egroups.com/group/Plugin64-Dev
***********************************************************************************

Notes:
------

Setting the approprate bits in the MI_INTR_RG and calling CheckInterrupts which 
are both passed to the DLL in _VIDEO_InitiateGFX will generate an Interrupt from with in 
the plugin.

The Setting of the RSP flags and generating an SP interrupt  should not be done in
the plugin

**********************************************************************************/
#pragma once

#include "Common.h"

#if defined(__cplusplus)
extern "C" {
#endif

#define CALL
#define EXPORT

/***** Structures *****/
typedef struct {
	HWND hWnd;			/* Render window */
	HWND hStatusBar;    /* if render window does not have a status bar then this is NULL */

	BOOL MemoryBswaped;    // If this is set to TRUE, then the memory has been pre
	                       //   bswap on a dword (32 bits) boundry 
						   //	eg. the first 8 bytes are stored like this:
	                       //        4 3 2 1   8 7 6 5

	BYTE * HEADER;	// This is the rom header (first 40h bytes of the rom
					// This will be in the same memory format as the rest of the memory.
	BYTE * RDRAM;
	BYTE * DMEM;
	BYTE * IMEM;

	DWORD * MI_INTR_RG;

	DWORD * DPC_START_RG;
	DWORD * DPC_END_RG;
	DWORD * DPC_CURRENT_RG;
	DWORD * DPC_STATUS_RG;
	DWORD * DPC_CLOCK_RG;
	DWORD * DPC_BUFBUSY_RG;
	DWORD * DPC_PIPEBUSY_RG;
	DWORD * DPC_TMEM_RG;

	DWORD * VI_STATUS_RG;
	DWORD * VI_ORIGIN_RG;
	DWORD * VI_WIDTH_RG;
	DWORD * VI_INTR_RG;
	DWORD * VI_V_CURRENT_LINE_RG;
	DWORD * VI_TIMING_RG;
	DWORD * VI_V_SYNC_RG;
	DWORD * VI_H_SYNC_RG;
	DWORD * VI_LEAP_RG;
	DWORD * VI_H_START_RG;
	DWORD * VI_V_START_RG;
	DWORD * VI_V_BURST_RG;
	DWORD * VI_X_SCALE_RG;
	DWORD * VI_Y_SCALE_RG;

	void (*CheckInterrupts)( void );
} GFX_INFO;

typedef struct
{
	DWORD addr;
	DWORD val;
	DWORD size;				// 1 = BYTE, 2 = WORD, 4=DWORD
} FrameBufferModifyEntry;

/******************************************************************
  Function: _VIDEO_RICE_531_CloseDLL
  Purpose:  This function is called when the emulator is closing
            down allowing the dll to de-Initialize.
  input:    none
  output:   none
*******************************************************************/ 
 void  _VIDEO_RICE_531_CloseDLL (void);

/******************************************************************
  Function: _VIDEO_RICE_531_ChangeWindow
  Purpose:  to change the window between fullscreen and window 
            mode. If the window was in fullscreen this should 
			change the screen to window mode and vice vesa.
  input:    none
  output:   none
*******************************************************************/ 
 void  _VIDEO_RICE_531_ChangeWindow (void);

/******************************************************************
  Function: _VIDEO_RICE_531_DllAbout
  Purpose:  This function is optional function that is provided
            to give further information about the DLL.
  input:    a handle to the window that calls this function
  output:   none
*******************************************************************/ 
 void  _VIDEO_RICE_531_DllAbout ( HWND hParent );

/******************************************************************
  Function: _VIDEO_RICE_531_DllConfig
  Purpose:  This function is optional function that is provided
            to allow the user to configure the dll
  input:    a handle to the window that calls this function
  output:   none
*******************************************************************/ 
 void  _VIDEO_RICE_531_DllConfig ( HWND hParent );

/******************************************************************
  Function: _VIDEO_RICE_531_DllTest
  Purpose:  This function is optional function that is provided
            to allow the user to test the dll
  input:    a handle to the window that calls this function
  output:   none
*******************************************************************/ 
 void  _VIDEO_RICE_531_DllTest ( HWND hParent );

/******************************************************************
  Function: _VIDEO_RICE_531_DrawScreen
  Purpose:  This function is called when the emulator receives a
            WM_PAINT message. This allows the gfx to fit in when
			it is being used in the desktop.
  input:    none
  output:   none
*******************************************************************/ 
 void  _VIDEO_RICE_531_DrawScreen (void);

/******************************************************************
  Function: _VIDEO_RICE_531_GetDllInfo
  Purpose:  This function allows the emulator to gather information
            about the dll by filling in the PluginInfo structure.
  input:    a pointer to a PLUGIN_INFO stucture that needs to be
            filled by the function. (see def above)
  output:   none
*******************************************************************/ 
 void  _VIDEO_RICE_531_GetDllInfo ( PLUGIN_INFO * PluginInfo );

/******************************************************************
  Function: _VIDEO_RICE_531_InitiateGFX
  Purpose:  This function is called when the DLL is started to give
            information from the emulator that the n64 graphics
			uses. This is not called from the emulation thread.
  Input:    Gfx_Info is passed to this function which is defined
            above.
  Output:   TRUE on success
            FALSE on failure to Initialize
             
  ** note on interrupts **:
  To generate an interrupt set the appropriate bit in MI_INTR_RG
  and then call the function CheckInterrupts to tell the emulator
  that there is a waiting interrupt.
*******************************************************************/ 
 BOOL  _VIDEO_RICE_531_InitiateGFX (GFX_INFO Gfx_Info);

/******************************************************************
  Function: _VIDEO_RICE_531_MoveScreen
  Purpose:  This function is called in response to the emulator
            receiving a WM_MOVE passing the xpos and ypos passed
			from that message.
  input:    xpos - the x-coordinate of the upper-left corner of the
            client area of the window.
			ypos - y-coordinate of the upper-left corner of the
			client area of the window. 
  output:   none
*******************************************************************/ 
 void  _VIDEO_RICE_531_MoveScreen (int xpos, int ypos);

/******************************************************************
  Function: _VIDEO_RICE_531_ProcessDList
  Purpose:  This function is called when there is a Dlist to be
            processed.
  input:    none
  output:   none
*******************************************************************/ 
 void  _VIDEO_RICE_531_ProcessDList(void);
 DWORD  _VIDEO_RICE_531_ProcessDListCountCycles(void);
 void  _VIDEO_RICE_531_ProcessRDPList(void);

/******************************************************************
  Function: _VIDEO_RICE_531_RomClosed
  Purpose:  This function is called when a rom is closed.
  input:    none
  output:   none
*******************************************************************/ 
 void  _VIDEO_RICE_531_RomClosed (void);

/******************************************************************
  Function: _VIDEO_RICE_531_RomOpen
  Purpose:  This function is called when a rom is open. (from the 
            emulation thread)
  input:    none
  output:   none
*******************************************************************/ 
 void  _VIDEO_RICE_531_RomOpen (void);

/******************************************************************
  Function: _VIDEO_RICE_531_UpdateScreen
  Purpose:  This function is called in response to a vsync of the
            screen were the VI bit in MI_INTR_RG has already been
			set
  input:    none
  output:   none
*******************************************************************/ 
 void  _VIDEO_RICE_531_UpdateScreen (void);

/******************************************************************
  Function: _VIDEO_RICE_531_ViStatusChanged
  Purpose:  This function is called to notify the dll that the
            ViStatus registers value has been changed.
  input:    none
  output:   none
*******************************************************************/ 
 void  _VIDEO_RICE_531_ViStatusChanged (void);

/******************************************************************
  Function: _VIDEO_RICE_531_ViWidthChanged
  Purpose:  This function is called to notify the dll that the
            ViWidth registers value has been changed.
  input:    none
  output:   none
*******************************************************************/ 
 void  _VIDEO_RICE_531_ViWidthChanged (void);



/******************************************************************
  Function: FrameBufferWrite
  Purpose:  This function is called to notify the dll that the
            frame buffer has been modified by CPU at the given address.
  input:    addr		rdram address
			val			val
			size		1 = BYTE, 2 = WORD, 4 = DWORD
  output:   none
*******************************************************************/ 
 void  _VIDEO_RICE_531_FBWrite(DWORD, DWORD);

/******************************************************************
  Function: FrameBufferWriteList
  Purpose:  This function is called to notify the dll that the
            frame buffer has been modified by CPU at the given address.
  input:    FrameBufferModifyEntry *plist
			size = size of the plist, max = 1024
  output:   none
*******************************************************************/ 
 void  _VIDEO_RICE_531_FBWList(FrameBufferModifyEntry *plist, DWORD size);

/******************************************************************
  Function: FrameBufferRead
  Purpose:  This function is called to notify the dll that the
            frame buffer memory is beening read at the given address.
			DLL should copy content from its render buffer to the frame buffer
			in N64 RDRAM
			DLL is responsible to maintain its own frame buffer memory addr list
			DLL should copy 4KB block content back to RDRAM frame buffer.
			Emulator should not call this function again if other memory
			is read within the same 4KB range
  input:    addr		rdram address
			val			val
			size		1 = BYTE, 2 = WORD, 4 = DWORD
  output:   none
*******************************************************************/ 
 void  _VIDEO_RICE_531_FBRead(DWORD addr);
 
 /************************************************************************
Function: FBGetFrameBufferInfo
Purpose:  This function is called by the emulator core to retrieve depth
buffer information from the video plugin in order to be able
to notify the video plugin about CPU depth buffer read/write
operations

size:
= 1		byte
= 2		word (16 bit) <-- this is N64 default depth buffer format
= 4		dword (32 bit)

when depth buffer information is not available yet, set all values
in the FrameBufferInfo structure to 0

input:    FrameBufferInfo *pinfo
pinfo is pointed to a FrameBufferInfo structure which to be
filled in by this function
output:   Values are return in the FrameBufferInfo structure
/************************************************************************/
void _VIDEO_RICE_531_FBGetFrameBufferInfo(void *pinfo);

/******************************************************************
  Function: ShowCFB
  Purpose:  Useally once Dlists are started being displayed, cfb is
            ignored. This function tells the dll to start displaying
			them again.
  input:    none
  output:   none
*******************************************************************/ 
EXPORT void CALL _VIDEO_RICE_531_ShowCFB (void);

/******************************************************************
  Function: CaptureScreen
  Purpose:  This function dumps the current frame to a file
  input:    pointer to the directory to save the file to
  output:   none
*******************************************************************/ 
EXPORT void CALL _VIDEO_RICE_531_CaptureScreen ( char * Directory );

//void InitGraphicsPlugin(HINSTANCE hInstance);
//void DestroyGraphicsPlugin();

// Ez0n3 - reinstate max video mem until freakdave finishes this
void	_VIDEO_RICE_531_SetMaxTextureMem(DWORD mem);


#if defined(__cplusplus)
}
#endif