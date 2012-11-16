/**
 * Wii64 - gamehacks.c
 * Copyright (C) 2012 emu_kidid
 *
 * 
 * Game specific hacks to workaround core inaccuracy
 *
 * Wii64 homepage: http://www.emulatemii.com
 * email address: emukidid@gmail.com
 *
 *
 * This program is free software; you can redistribute it and/
 * or modify it under the terms of the GNU General Public Li-
 * cence as published by the Free Software Foundation; either
 * version 2 of the Licence, or any later version.
 *
 * This program is distributed in the hope that it will be use-
 * ful, but WITHOUT ANY WARRANTY; without even the implied war-
 * ranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public Licence for more details.
 *
**/

/**
 * Modified for use with Project64x/Surreal64CE by weinerschnitzel
 * Surreal64CE homepage: http://surreal64ce.sf.net
 * Email address: weinerschnitzel@gmail.com
**/
#include <xtl.h>
#include <stdio.h>
#include "main.h"
#include "cpu.h"
#include "Memory.h"
/*
extern "C" BOOL r4300i_LH_VAddr( DWORD VAddr, WORD * Value ); // Read Half Word from memory
extern "C" BOOL r4300i_SB_VAddr( DWORD VAddr, BYTE Value ); // Write Byte to memory
extern "C" BOOL r4300i_SH_VAddr( DWORD VAddr, WORD Value ); // Write Half Word to memory
*/
extern char RomHeader[0x1000];

void *game_specific_hack = 0;

// Pokemon Snap (U)
void hack_pkm_snap_u() {
	WORD Value = 0x0000;
	// Pass 1st Level and Controller Fix
	if(r4300i_LH_VAddr(0x80382D1C, Value)) {
		if( Value == 0x802C )
			r4300i_SB_VAddr(0x80382D0F, 0);
	}  
	// Make Picture selectable
	if(r4300i_LH_VAddr(0x801E3184, Value)) {
		if( Value == 0x2881 )
			r4300i_SH_VAddr(0x801E3184, 0x2001);
	}
	if(r4300i_LH_VAddr(0x801E3186, Value)) {
		if( Value == 0x0098 )
			r4300i_SH_VAddr(0x801E3186, 0x0001);
	}
}


// Pokemon Snap (A)
void hack_pkm_snap_a() {
	WORD Value = 0x0000;
	// Pass 1st Level and Controller Fix
	if(r4300i_LH_VAddr(0x80382D1C, Value)) {
		if( Value == 0x802C )
			r4300i_SB_VAddr(0x80382D0F, 0);
	} 
	//Make Picture selectable
	if(r4300i_LH_VAddr(0x801E3C44, Value)) {
		if( Value == 0x2881 )
			r4300i_SH_VAddr(0x801E3C44, 0x2001);
	}
	if(r4300i_LH_VAddr(0x801E3C46, Value)) {
		if( Value == 0x0098 )
			r4300i_SH_VAddr(0x801E3C46, 0x0001);
	}
}

// Pokemon Snap (E)
void hack_pkm_snap_e() {
	WORD Value = 0x0000;
	// Pass 1st Level and Controller Fix
	if(r4300i_LH_VAddr(0x80381BFC, Value)) {
		if( Value == 0x802C )
			r4300i_SB_VAddr(0x80381BEF, 0);
	} 
	//Make Picture selectable
	if(r4300i_LH_VAddr(0x801E3824, Value)) {
		if( Value == 0x2881 )
			r4300i_SH_VAddr(0x801E3824, 0x2001);
	}
	if(r4300i_LH_VAddr(0x801E3826, Value)) {
		if( Value == 0x0098 )
			r4300i_SH_VAddr(0x801E3826, 0x0001);
	}
}

// Pocket Monsters Snap (J)
void hack_pkm_snap_j() {
	WORD Value = 0x0000;
	// Pass 1st Level and Controller Fix
	if(r4300i_LH_VAddr(0x8036D22C, Value)) {
		if( Value == 0x802A )
			r4300i_SB_VAddr(0x8036D21F, 0);
	}
	//Make Picture selectable
	if(r4300i_LH_VAddr(0x801E1EC4, Value)) {
		if( Value == 0x2881 )
			r4300i_SH_VAddr(0x801E1EC4, 0x2001);
	}
	if(r4300i_LH_VAddr(0x801E1EC6, Value)) {
		if( Value == 0x0098 )
			r4300i_SH_VAddr(0x801E1EC6, 0x0001);
	}
}

// Top Gear Hyper-Bike (E) 
void hack_topgear_hb_e() {
	WORD Value = 0x0000;
	//Game Playable Fix (Gent)
	if(r4300i_LH_VAddr(0x800021EE, Value)) {
		if( Value == 0x0001 )
			r4300i_SB_VAddr(0x800021EE, 0);
	}
}

// Top Gear Hyper-Bike (J) 
void hack_topgear_hb_j() {
	WORD Value = 0x0000;
	//Game Playable Fix (Gent)
	if(r4300i_LH_VAddr(0x8000225A, Value)) {
		if( Value == 0x0001 )
			r4300i_SB_VAddr(0x8000225A, 0);
	}
}

// Top Gear Hyper-Bike (U) 
void hack_topgear_hb_u() {
	WORD Value = 0x0000;
	//Game Playable Fix (Gent)
	if(r4300i_LH_VAddr(0x800021EA, Value)) {
		if( Value == 0x0001 )
			r4300i_SB_VAddr(0x800021EA, 0);
	}
}

// Top Gear Overdrive (E) 
void hack_topgear_od_e() {
	WORD Value = 0x0000;
	//Game Playable Fix (Gent)
	if(r4300i_LH_VAddr(0x80001AB2, Value)) {
		if( Value == 0x0001 )
			r4300i_SB_VAddr(0x80001AB2, 0);
	}
}

// Top Gear Overdrive (J) 
void hack_topgear_od_j() {
	WORD Value = 0x0000;
	//Game Playable Fix (Gent)
	if(r4300i_LH_VAddr(0x80001B4E, Value)) {
		if( Value == 0x0001 )
			r4300i_SB_VAddr(0x80001B4E, 0);
	}
}

// Top Gear Overdrive (U) 
void hack_topgear_od_u() {
	WORD Value = 0x0000;
	//Game Playable Fix (Gent)
	if(r4300i_LH_VAddr(0x80001B4E, Value)) {
		if( Value == 0x0001 )
			r4300i_SB_VAddr(0x80001B4E, 0);
	}
}

// Return a pointer to the game specific hack or 0 if there isn't any
void *GetGameSpecificHack() {
	return game_specific_hack;
}

// Game specific hack detection via CRC
void GameSpecificHackSetup() {

	if((RomHeader + 0x10) == "0xCA12B547" && (RomHeader + 0x14) == "0x71FA4EE4") {
		game_specific_hack = &hack_pkm_snap_u;
	}
	else if((RomHeader + 0x10) == "0xEC0F690D" && (RomHeader + 0x14) == "0x32A7438C") {
		game_specific_hack = &hack_pkm_snap_j;
	}
	else if((RomHeader + 0x10) == "0x7BB18D40" && (RomHeader + 0x14) == "0x83138559") {
		game_specific_hack = &hack_pkm_snap_a;
	}
	else if((RomHeader + 0x10) == "0x4FF5976F" && (RomHeader + 0x14) == "0xACF559D8") {
		game_specific_hack = &hack_pkm_snap_e;
	}
	else if((RomHeader + 0x10) == "0x5F3F49C6" && (RomHeader + 0x14) == "0x0DC714B0") {
		game_specific_hack = &hack_topgear_hb_e;
	}
	else if((RomHeader + 0x10) == "0x845B0269" && (RomHeader + 0x14) == "0x57DE9502") {
		game_specific_hack = &hack_topgear_hb_j;
	}
	else if((RomHeader + 0x10) == "0x8ECC02F0" && (RomHeader + 0x14) == "0x7F8BDE81") {
		game_specific_hack = &hack_topgear_hb_u;
	}
	else if((RomHeader + 0x10) == "0xD09BA538" && (RomHeader + 0x14) == "0x1C1A5489") {
		game_specific_hack = &hack_topgear_od_e;
	}
	else if((RomHeader + 0x10) == "0x0578F24F" && (RomHeader + 0x14) == "0x9175BF17") {
		game_specific_hack = &hack_topgear_od_j;
	}
	else if((RomHeader + 0x10) == "0xD741CD80" && (RomHeader + 0x14) == "0xACA9B912") {
		game_specific_hack = &hack_topgear_od_u;
	}
	else {
		game_specific_hack = 0;
	}
}
