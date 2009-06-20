/**
 * Surreal 64 Launcher (C) 2003
 * 
 * This program is free software; you can redistribute it and/or modify it under 
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option) any later
 * version. This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details. You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA. To contact the
 * authors: email: buttza@hotmail.com, lantus@lantus-x.com
 */

#pragma once

#include <xtl.h>


//Ez0n3 - Xport - NATEDOGG - CMD launch code
#define CUSTOM_LAUNCH_PARAMS_H
#include <XBApp.h>
#define CUSTOM_LAUNCH_MAGIC 0xEE456777
typedef struct _CUSTOM_LAUNCH_DATA
{
	DWORD magic; //populate this with CUSTOM_LAUNCH_MAGIC so we know we are using this special structure
	char szFilename[300]; //this is the path to the game to load upon startup
	//char szLaunchXBEOnExit[100] ;			//this is the XBE name that should be launched when exiting the emu  ( "FILE.XBE" )
	//char szRemap_D_As[350] ;				//this is what D drive should be mapped to in order to launch the XBE specified in szLaunchXBEOnExit  ( "\\Device\\Harddisk0\\Partition1\\GAMES" )
	//BYTE country ;							//country code to use
	//BYTE launchInsertedMedia ;				//should we auto-run the inserted CD/DVD ?
	BYTE executionType; //generic variable that determines how the emulator is run - for example, if you wish to run FMSXBOX as MSX1 or MSX2 or MSX2+
	char reserved[MAX_LAUNCH_DATA_SIZE-757]; //MAX_LAUNCH_DATA_SIZE is 3KB 

} CUSTOM_LAUNCH_DATA, *PCUSTOM_LAUNCH_DATA;


#include <fstream>
#include <string>
#include <sstream>
#include <list>
#include <vector>
#include <algorithm>
using namespace std;

#include "Common.h"
#include "D3D.h"

enum Emulators
{
	_1964,
	_Project64,
	_UltraHLE,
	_None
};


// Ez0n3
extern int preferedemu;
extern int videoplugin;
extern int iAudioPlugin;
int ConfigAppLoad3(); // load user pref func
bool PhysRam128(); // determine if the current phys ram is greater than 100MB
bool FileExists(char *szFilename); // check if file exists

extern bool g_bQuit;