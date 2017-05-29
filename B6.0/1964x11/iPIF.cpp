/*$T iPIF.c GC 1.136 03/09/02 17:42:34 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    Peripheral interface. Controller handled here, including mempak, flashram access, etc.
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


/*
 * 1964 Copyright (C) 1999-2004 Joel Middendorf, <schibo@emulation64.com> This
 * program is free software; you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option) any later
 * version. This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details. You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA. To contact the
 * authors: email: schibo@emulation64.com, rice1964@yahoo.com
 */
#ifndef USE_ICC_LIB
#ifndef _XBOX_ICC
#include "stdafx.h"
#else
#include <mytypes.h>
#include <memory.h>
#include "ipif.h"
#include "1964ini.h"
#include "registers.h"
#include "debug_option.h"
#include "plugins.h"
#include "emulator.h"
#include "gamesave.h"
#include "dynarec/opcodedebugger.h"
#include "hardware.h"
#endif
#include <time.h> // for RTC

extern void CONTROLLER_SetRumble(int cont, BOOL use);

#ifdef _XBOX_ICC
//extern MemoryState			gMemoryState;
extern void CONTROLLER_ControllerCommand(int device, unsigned char *cmd);
extern void	CONTROLLER_ReadController(int device, unsigned char *cmd);
extern void CONTROLLER_GetKeys(int device, BUTTONS *Keys);

extern void FileIO_LoadEEprom(void);
extern void FileIO_LoadMemPak(int device);
extern void FileIO_WriteEEprom(void);
extern void FileIO_WriteMemPak(int device);

extern CONTROL		Controls[4];
extern N64::CRegisters r;
extern INI_ENTRY currentromoptions;
extern uint32	current_rdram_size;
#endif

_u8 EEProm_Status_Byte = 0x00;

/*
 =======================================================================================================================
    Init PIF //
 =======================================================================================================================
 */
void Init_iPIF(void)
{
	gamesave.mempak_used[0] = FALSE;
	gamesave.mempak_used[1] = FALSE;
	gamesave.mempak_used[2] = FALSE;
	gamesave.mempak_used[3] = FALSE;
	gamesave.Sram_used = FALSE;
	gamesave.EEprom_used = FALSE;
	gamesave.firstusedsavemedia = 0;

	switch(currentromoptions.Eeprom_size)
	{
	case EEPROMSIZE_16KB:	
		EEProm_Status_Byte = 0xC0; 
		break;
	case EEPROMSIZE_4KB:	
		EEProm_Status_Byte = 0x80; 
		break;
	default:			 /* =EEPROMSIZE_NONE */
		EEProm_Status_Byte = 0x00; 
		break;
	};
}

/*
 =======================================================================================================================
    Close PIF //
 =======================================================================================================================
 */
void Close_iPIF(void)
{
	/*~~*/
	int i;
	/*~~*/

	/* write mempaks to file if it was in use */
	if
	(
		currentromoptions.Save_Type == MEMPAK_SAVETYPE
	||	(currentromoptions.Save_Type == FIRSTUSE_SAVETYPE && gamesave.firstusedsavemedia == MEMPAK_SAVETYPE)
	||	currentromoptions.Save_Type == ANYUSED_SAVETYPE
	)
	{
		for(i = 0; i < 4; i++)
		{
			if(gamesave.mempak_used[i] && gamesave.mempak_written[i])
			{
				FileIO_WriteMemPak(i);
				gamesave.mempak_used[i] = FALSE;
				gamesave.mempak_written[i] = FALSE;
			}
		}
	}

	/* write eeprom to file if it was in use */
	if
	(
		currentromoptions.Save_Type == EEPROM_SAVETYPE
	||	(currentromoptions.Save_Type == FIRSTUSE_SAVETYPE && gamesave.firstusedsavemedia == EEPROM_SAVETYPE)
	||	currentromoptions.Save_Type == ANYUSED_SAVETYPE
	)
	{
		if(gamesave.EEprom_used && gamesave.EEprom_written)
		{
			FileIO_WriteEEprom();
			gamesave.EEprom_used = FALSE;
			gamesave.EEprom_written = FALSE;
		}
	}

	if
	(
		currentromoptions.Save_Type == FLASHRAM_SAVETYPE
	||	currentromoptions.Save_Type == SRAM_SAVETYPE
	||	(currentromoptions.Save_Type == FIRSTUSE_SAVETYPE && gamesave.firstusedsavemedia == FLASHRAM_SAVETYPE)
	||	currentromoptions.Save_Type == ANYUSED_SAVETYPE
	)
	{
		//FileIO_WriteFLASHRAM();
	}
}

/*
 =======================================================================================================================
    Build CRC for PAK
 =======================================================================================================================
 */
void BuildCRC(_u8 *data, _u8 *crc)
{
	/*~~~~~~~~~~*/
	_u8 tmp, tmp2;
	int i, j;
	/*~~~~~~~~~~*/

	tmp = 0;
	tmp2 = 0;
	for(i = 0; i <= 32; i++)
	{
		for(j = 7; j >= 0; j--)
		{
			if(tmp & 0x80)
				tmp2 = 0x85;
			else
				tmp2 = 0x00;
			tmp <<= 1;
			if(i == 32)
				tmp |= 0;
			else if(data[i] & (1 << j))
				tmp |= 1;
			else
				tmp |= 0;
			tmp ^= tmp2;
		}
	}

	*crc = tmp;
}

/*
 =======================================================================================================================
    Read Data from MemPak
 =======================================================================================================================
 */
void ReadControllerPak(int device, unsigned char *cmd)
{
	uint8	crc;
	uint16	offset = *(_u16 *) &cmd[1];

	offset = (offset >> 8) | (offset << 8);
	offset = offset >> 5;

	if(offset <= 0x400)
	{
		if(!gamesave.mempak_used[device])
		{
			FileIO_LoadMemPak(device);
			gamesave.mempak_used[device] = TRUE;

			if(gamesave.firstusedsavemedia == 0)
			{
				gamesave.firstusedsavemedia = MEMPAK_SAVETYPE;
			}
		}
		memcpy(&cmd[3], &(gamesave.mempak[device][offset * 32]), 32);
	}

	BuildCRC(&cmd[3], &crc);	/* Build CRC of the Data */
	cmd[35] = crc;
}

/*
 =======================================================================================================================
    Write Data to MemPak
 =======================================================================================================================
 */
void WriteControllerPak(int device, char *cmd)
{
	uint8	crc;
	uint16	offset = *(_u16 *) &cmd[1];

	offset = (offset >> 8) | (offset << 8);
	offset = offset >> 5;

	if(offset <= 0x400)
	{
		if(!gamesave.mempak_used[device])
		{
			FileIO_LoadMemPak(device);
			gamesave.mempak_used[device] = TRUE;

			if(gamesave.firstusedsavemedia == 0)
			{
				gamesave.firstusedsavemedia = MEMPAK_SAVETYPE;
			}
		}

		/* Copy Data to Mempak */
		memcpy(&(gamesave.mempak[device][offset * 32]), &cmd[3], 32);
		gamesave.mempak_written[device] = TRUE;
		FileIO_WriteMemPak(device);		//Save the file to disk
	}

	BuildCRC((unsigned char*)&cmd[3], &crc);	/* Build CRC of the Data */
	cmd[35] = crc;
}

/*
 =======================================================================================================================
    Handles all Commands which are sent to a Controller
 =======================================================================================================================
 */
BOOL ControllerCommand(_u8 *cmd, int device)
{
	//static int count=0;
	//if( count == 0 )
	//	MessageBox(gui.hwnd1964main,"Warning","Warning",MB_OK);
	//count++;

	emustatus.ControllerReadCount++;

	//KAILLERA_LOG(fprintf(ktracefile, "P%d cmd %d at VI %d\n", device, cmd[2], viTotalCount));


#ifndef _XBOX
	if( Kaillera_Is_Running == TRUE )
	{
		if( kailleraClientStatus[device] == FALSE )
		{
			cmd[1] |= 0x80;
			cmd[3] = 0xFF;
			cmd[4] = 0xFF;
			cmd[5] = 0xFF;
			//KAILLERA_LOG(fprintf(ktracefile, "P%d get status %08X%08X at VI %d\n", device, *(DWORD*)&cmd[0], *(DWORD*)&cmd[4], viTotalCount));
			return TRUE;
		}
	}
	else if( !Controls[device].Present )
#else //xbox
	if( !Controls[device].Present )
#endif
	{
		cmd[1] |= 0x80;
		cmd[3] = 0xFF;
		cmd[4] = 0xFF;
		cmd[5] = 0xFF;
		//KAILLERA_LOG(fprintf(ktracefile, "P%d get status %08X%08X at VI %d\n", device, *(DWORD*)&cmd[0], *(DWORD*)&cmd[4], viTotalCount));
		return TRUE;
	}

	switch(cmd[2])
	{
	/* Get Controller status */
	case 0xFF:				/* 0xFF could be something like Reset Controller and return the status */
	case 0x00:				/* 0x00 return the status */
		cmd[3] = 0x05;		/* for Adaptoid only */
		cmd[4] = 0x00;		/* for Adaptoid only */
#ifdef _XBOX
		if( currentromoptions.Save_Type == MEMPAK_SAVETYPE || currentromoptions.Save_Type == ANYUSED_SAVETYPE || Controls[device].Plugin & PLUGIN_RUMBLE_PAK ) 
#else //win32
		if( currentromoptions.Save_Type == MEMPAK_SAVETYPE || currentromoptions.Save_Type == ANYUSED_SAVETYPE ) 
#endif
			cmd[5] = 0x01;
		else
			cmd[5] = 0x00;	/* no mempak - reversed fir Adaptoid only (Bit 0x01 would be rumble-pack) */

		//KAILLERA_LOG(fprintf(ktracefile, "P%d get status %08X%08X at VI %d\n", device, *(DWORD*)&cmd[0], *(DWORD*)&cmd[4], viTotalCount));
		break;

	/* Read Controller Data ... need a DInput interface first =) */
	case 0x01:
		{
			BUTTONS Keys;

#ifndef _XBOX
			if(Kaillera_Is_Running )
			{
				KailleraGetPlayerKeyValuesFor1Player(&Keys, device);
			}
			else
#endif
			{
				CONTROLLER_GetKeys(device, &Keys);
			}

#ifndef _XBOX
			if( ktracefile )	fprintf(ktracefile, "P%d get key value %08X at VI %d\n", device, *(DWORD *) &Keys, viTotalCount);
#endif
			*(DWORD *) &cmd[3] = *(DWORD *) &Keys;
			DEBUG_CONTROLLER_TRACE(TRACE2("Read controller %d, return %X", device, *(DWORD *) &Keys););
		}

		//KAILLERA_LOG(fprintf(ktracefile, "P%d get Keys %08X%08X at VI %d\n", device, *(DWORD*)&cmd[0], *(DWORD*)&cmd[4], viTotalCount));
		break;

	/* Read Controller Pak */
	case 0x02:
#ifdef _XBOX
	{
		DWORD dwAddress = (cmd[3] << 8) + (cmd[4] & 0xE0);
		BYTE *Data = &cmd[5];
#endif
		switch(Controls[device].Plugin)
		{
		case PLUGIN_MEMPAK:
			ReadControllerPak(device, &cmd[2]);
			break;
		case PLUGIN_NONE:
			if( currentromoptions.Save_Type == MEMPAK_SAVETYPE || currentromoptions.Save_Type == ANYUSED_SAVETYPE )
			{
				// Always support mempak here, the flag "Controls[device].Plugin" is sometime reset by input plugin
				ReadControllerPak(device, &cmd[2]);
				break;
			}
		case PLUGIN_RUMBLE_PAK:
#ifdef _XBOX
		{
			if((dwAddress >= 0x8000) && (dwAddress < 0x9000))
				memset(Data,0x80,32);
				//FillMemory(Data, 32, 0x80);
			else
				memset(Data,0x00,32);
				//FillMemory(Data, 32, 0x00);
			BuildCRC(Data, &Data[32]);
			break;
		}
#endif
		case PLUGIN_TANSFER_PAK:
		default:
			break;
		}

		//KAILLERA_LOG(fprintf(ktracefile, "P%d get Pak %08X%08X at VI %d\n", device, *(DWORD*)&cmd[0], *(DWORD*)&cmd[4], viTotalCount));
		return FALSE;
		break;
#ifdef _XBOX
	}
#endif

	/* Write Controller Pak */
	case 0x03:
#ifdef _XBOX
	{
		DWORD dwAddress = (cmd[3] << 8) + (cmd[4] & 0xE0);
		//DWORD dwAddress = ((cmd[3] << 8) | cmd[4]);
		BYTE *Data = &cmd[5];
#endif
		switch(Controls[device].Plugin)
		{
		case PLUGIN_MEMPAK:
			WriteControllerPak(device, (char*)&cmd[2]);
			break;
		case PLUGIN_NONE:
			if( currentromoptions.Save_Type == MEMPAK_SAVETYPE || currentromoptions.Save_Type == ANYUSED_SAVETYPE )
			{
				// Always support mempak here, the flag "Controls[device].Plugin" is sometime reset by input plugin
				WriteControllerPak(device, (char*)&cmd[2]);
				break;
			}
		case PLUGIN_RUMBLE_PAK:
#ifdef _XBOX
		{
			// the rumble register
			if(dwAddress == 0xC000)
			{
				// check the first byte, if its 0x01 then enable the rumble
				if(*Data)
				{
					CONTROLLER_SetRumble(device, TRUE);
				}
				else
				{
					CONTROLLER_SetRumble(device, FALSE);
				}
			}

			BuildCRC(Data, &Data[32]);
			break;
		}
#endif
		case PLUGIN_TANSFER_PAK:
		default:
			break;
		}

		//KAILLERA_LOG(fprintf(ktracefile, "P%d write Pak %08X%08X at VI %d\n", device, *(DWORD*)&cmd[0], *(DWORD*)&cmd[4], viTotalCount));
		return FALSE;
		break;
#ifdef _XBOX
	}
#endif

	default:
		TRACE2("Unkown ControllerCommand %X, pc=%08X", cmd[2], gHWS_pc);
#ifdef _DEBUG
		{
			/*~~~~~~~~~~~~~~~~~~*/
			char	line[100];
			char	*pline = line;
			int		i;
			/*~~~~~~~~~~~~~~~~~~*/

			for(i = 0; i < 64; i++)
			{
				sprintf(pline, "%02X ", cmd[i]);
				pline += 3;
				if(i % 8 == 7)
				{
					pline = line;
					TRACE1("%s", pline);
				}
			}
		}
#endif
		DisplayError("Unknown ControllerCommand %x, pc=%08X, device=%d", cmd[2], gHWS_pc, device);

		/* Treat this as GET_STATUS */
		cmd[3] = 0x05;		/* for Adaptoid only */
		cmd[4] = 0x00;		/* for Adaptoid only */
		if( currentromoptions.Save_Type == MEMPAK_SAVETYPE || currentromoptions.Save_Type == ANYUSED_SAVETYPE )
			cmd[5] = 0x01;
		else
			cmd[5] = 0x00;	/* no mempak - for Adaptoid only (Bit 0x01 would be rumble-pack) */

		//KAILLERA_LOG(fprintf(ktracefile, "P%d get status %08X%08X at VI %d\n", device, *(DWORD*)&cmd[0], *(DWORD*)&cmd[4], viTotalCount));
		/* exit(IPIF_EXIT); */
		break;
	}

	return TRUE;
}

/*
 =======================================================================================================================
    Read EEprom (loads EEprom from file if it is present)
 =======================================================================================================================
 */
void ReadEEprom(unsigned char *dest, long offset)
{
#ifdef DEBUG_SI_EEPROM
	if(debugoptions.debug_si_eeprom)
	{
		TRACE0("Read from EEPROM");
	}
#endif

	if(!gamesave.EEprom_used)
	{
		FileIO_LoadEEprom();
		gamesave.EEprom_used = TRUE;

		if(gamesave.firstusedsavemedia == 0)
		{
			gamesave.firstusedsavemedia = EEPROM_SAVETYPE;
		}
	}

	memcpy(dest, &gamesave.EEprom[offset], 8);
}

/*
 =======================================================================================================================
    Writes to EEprom
 =======================================================================================================================
 */
void WriteEEprom(char *src, long offset)
{
#ifdef DEBUG_SI_EEPROM
	if(debugoptions.debug_si_eeprom)
	{
		TRACE0("Write to EEPROM");
	}
#endif

	if(!gamesave.EEprom_used)
	{
		FileIO_LoadEEprom();
		gamesave.EEprom_used = TRUE;

		if(gamesave.firstusedsavemedia == 0)
		{
			gamesave.firstusedsavemedia = EEPROM_SAVETYPE;
		}
	}

	memcpy(&gamesave.EEprom[offset], src, 8);
	FileIO_WriteEEprom();	//Write the changes to disk
	gamesave.EEprom_written = TRUE;
}

/*
 =======================================================================================================================
    Handles all Commands which are sent to the EEprom
 =======================================================================================================================
 */
// added by weinersch
unsigned char byte2bcd(int n)
{
	n %= 100;
	return ((n / 10) << 4) | (n % 10);
}

BOOL EEpromCommand(_u8 *cmd)
{
	// added by Shapyi
	time_t curtime_time;
    struct tm curtime;
	
	switch(cmd[2])
	{
	/* reporting eeprom state ... hmmm */
	case 0xFF:
	case 0x00:
#ifdef DEBUG_SI_EEPROM
		if(debugoptions.debug_si_eeprom)
		{
			TRACE0("Execute EEPROM GetStatus Commands");
		}
#endif
		cmd[3] = 0x00;
		cmd[4] = EEProm_Status_Byte;
		cmd[5] = 0x00;
		break;

	/* Read from Eeprom */
	case 0x04:
		ReadEEprom(&cmd[4], cmd[3] * 8);
		break;

	/* Write to Eeprom */
	case 0x05:
		WriteEEprom((char*)&cmd[4], cmd[3] * 8);
		break;
	
	// added by Shapyi
	/* RTC, credit: Mupen64 source */
	case 0x06:
		// RTC status query
	    cmd[3] = 0x00;
	    cmd[4] = 0x10;
	    cmd[5] = 0x00;
		break;
	case 0x07:
		// read RTC block
		switch (cmd[3]) { // block number
			case 0:
			    cmd[4] = 0x00;
			    cmd[5] = 0x02;
			    cmd[12] = 0x00;
			    break;
			case 1:
				//DebugMessage(M64MSG_ERROR, "RTC command in EepromCommand(): read block %d", Command[2]);
			    break;
			case 2:
				time(&curtime_time);
			    memcpy(&curtime, localtime(&curtime_time), sizeof(curtime)); // fd's fix
			    cmd[4] = byte2bcd(curtime.tm_sec);
			    cmd[5] = byte2bcd(curtime.tm_min);
			    cmd[6] = 0x80 + byte2bcd(curtime.tm_hour);
			    cmd[7] = byte2bcd(curtime.tm_mday);
			    cmd[8] = byte2bcd(curtime.tm_wday);
			    cmd[9] = byte2bcd(curtime.tm_mon + 1);
			    cmd[10] = byte2bcd(curtime.tm_year);
			    cmd[11] = byte2bcd(curtime.tm_year / 100);
			    cmd[12] = 0x00;	// status
			    break;
		}
		break;
	case 0x08:
		// write RTC block
		//DebugMessage(M64MSG_ERROR, "RTC write in EepromCommand(): %d not yet implemented", Command[2]);
		break;

	default:
		break;
	}

	return FALSE;
}

uint32 SrcCodeLUT[] = {
0x00040010, 0x00400000, 0x02000000, 0x00000001, //0
0x00040010, 0x00400001, 0x02000010, 0x00400001, //1
0x00040010, 0x00400002, 0x02000020, 0x00800001, //2
0x00040010, 0x00400003, 0x02000030, 0x00C00001, //3
0x00040010, 0x00410004, 0x02000040, 0x00000001, //4
0x00040010, 0x00410005, 0x02000050, 0x00400001, //5
0x00040010, 0x00410006, 0x02000060, 0x00800001, //6
0x00040010, 0x00410007, 0x02000070, 0x00C00001, //7
0x00040010, 0x00420008, 0x02000080, 0x00000001, //8
0x00040010, 0x00420009, 0x02000090, 0x00400001, //9
0x00040010, 0x0042000A, 0x020000A0, 0x00800001, //10
0x00040010, 0x0042000B, 0x020000B0, 0x00C00001, //11
0x00040010, 0x0043000C, 0x020000C0, 0x00000001, //12
0x00040010, 0x0043000D, 0x020000D0, 0x00400001, //13
0x00040010, 0x0043000E, 0x020000E0, 0x00800001, //14
0x00040010, 0x0043000F, 0x020000F0, 0x00C00001, //15
0x00040011, 0x00440010, 0x02000000, 0x00000001, //16
0x00040011, 0x00440011, 0x02000010, 0x00400001, //17
0x00040011, 0x00440012, 0x02000020, 0x00800001, //18
0x00040011, 0x00440013, 0x02000030, 0x00C00001, //19
0x00040011, 0x00450014, 0x02000040, 0x00000001, //20
0x00040011, 0x00450015, 0x02000050, 0x00400001, //21
0x00040011, 0x00450016, 0x02000060, 0x00800001, //22
0x00040011, 0x00450017, 0x02000070, 0x00C00001, //23
0x00040011, 0x00460018, 0x02000080, 0x00000001, //24
0x00040011, 0x00460019, 0x02000090, 0x00400001, //25
0x00040011, 0x0046001A, 0x020000A0, 0x00800001, //26
0x00040011, 0x0046001B, 0x020000B0, 0x00C00001, //27
0x00040011, 0x0047001C, 0x020000C0, 0x00000001, //28
0x00040011, 0x0047001D, 0x020000D0, 0x00400001, //29
0x00040011, 0x0047001E, 0x020000E0, 0x00800001, //30
0x00040011, 0x0047001F, 0x020000F0, 0x00C00001, //31
0x00040012, 0x00480020, 0x02000000, 0x00000001, //32
0x00040012, 0x00480021, 0x02000010, 0x00400001, //33
0x00040012, 0x00480022, 0x02000020, 0x00800001, //34
0x00040012, 0x00480023, 0x02000030, 0x00C00001, //35
0x00040012, 0x00490024, 0x02000040, 0x00000001, //36
0x00040012, 0x00490025, 0x02000050, 0x00400001, //37
0x00040012, 0x00490026, 0x02000060, 0x00800001, //38
0x00040012, 0x00490027, 0x02000070, 0x00C00001, //39
0x00040012, 0x004A0028, 0x02000080, 0x00000001, //40
0x00040012, 0x004A0029, 0x02000090, 0x00400001, //41
0x00040012, 0x004A002A, 0x020000A0, 0x00800001, //42
0x00040012, 0x004A002B, 0x020000B0, 0x00C00001, //43
0x00040012, 0x004B002C, 0x020000C0, 0x00000001, //44
0x00040012, 0x004B002D, 0x020000D0, 0x00400001, //45
0x00040012, 0x004B002E, 0x020000E0, 0x00800001, //46
0x00040012, 0x004B002F, 0x020000F0, 0x00C00001, //47
0x00040013, 0x004C0030, 0x02000000, 0x00000001, //48
0x00040013, 0x004C0031, 0x02000010, 0x00400001, //49
0x00040013, 0x004C0032, 0x02000020, 0x00800001, //50
0x00040013, 0x004C0033, 0x02000030, 0x00C00001, //51
0x00040013, 0x004D0034, 0x02000040, 0x00000001, //52
0x00040013, 0x004D0035, 0x02000050, 0x00400001, //53
0x00040013, 0x004D0036, 0x02000060, 0x00800001, //54
0x00040013, 0x004D0037, 0x02000070, 0x00C00001, //55
0x00040013, 0x004E0038, 0x02000080, 0x00000001, //56
0x00040013, 0x004E0039, 0x02000090, 0x00400001, //57
0x00040013, 0x004E003A, 0x020000A0, 0x00800001, //58
0x00040013, 0x004E003B, 0x020000B0, 0x00C00001, //59
0x00040013, 0x004F003C, 0x020000C0, 0x00000001, //60
0x00040013, 0x004F003D, 0x020000D0, 0x00400001, //61
0x00040013, 0x004F003E, 0x020000E0, 0x00800001, //62
0x00040013, 0x004F003F, 0x020000F0, 0x00C00001, //63
0x00050014, 0x00500040, 0x02000000, 0x00000001, //64
0x00050014, 0x00500041, 0x02000010, 0x00400001, //65
0x00050014, 0x00500042, 0x02000020, 0x00800001, //66
0x00050014, 0x00500043, 0x02000030, 0x00C00001, //67
0x00050014, 0x00510044, 0x02000040, 0x00000001, //68
0x00050014, 0x00510045, 0x02000050, 0x00400001, //69
0x00050014, 0x00510046, 0x02000060, 0x00800001, //70
0x00050014, 0x00510047, 0x02000070, 0x00C00001, //71
0x00050014, 0x00520048, 0x02000080, 0x00000001, //72
0x00050014, 0x00520049, 0x02000090, 0x00400001, //73
0x00050014, 0x0052004A, 0x020000A0, 0x00800001, //74
0x00050014, 0x0052004B, 0x020000B0, 0x00C00001, //75
0x00050014, 0x0053004C, 0x020000C0, 0x00000001, //76
0x00050014, 0x0053004D, 0x020000D0, 0x00400001, //77
0x00050014, 0x0053004E, 0x020000E0, 0x00800001, //78
0x00050014, 0x0053004F, 0x020000F0, 0x00C00001, //79
0x00050015, 0x00540050, 0x02000000, 0x00000001, //80
0x00050015, 0x00540051, 0x02000010, 0x00400001, //
0x00050015, 0x00540052, 0x02000020, 0x00800001, //
0x00050015, 0x00540053, 0x02000030, 0x00C00001, //
0x00050015, 0x00550054, 0x02000040, 0x00000001, //
0x00050015, 0x00550055, 0x02000050, 0x00400001, //
0x00050015, 0x00550056, 0x02000060, 0x00800001, //
0x00050015, 0x00550057, 0x02000070, 0x00C00001, //
0x00050015, 0x00560058, 0x02000080, 0x00000001, //
0x00050015, 0x00560059, 0x02000090, 0x00400001, //
0x00050015, 0x0056005A, 0x020000A0, 0x00800001, //
0x00050015, 0x0056005B, 0x020000B0, 0x00C00001, //
0x00050015, 0x0057005C, 0x020000C0, 0x00000001, //
0x00050015, 0x0057005D, 0x020000D0, 0x00400001, //
0x00050015, 0x0057005E, 0x020000E0, 0x00800001, //
0x00050015, 0x0057005F, 0x020000F0, 0x00C00001, //
0x00050016, 0x00580060, 0x02000000, 0x00000001, //
0x00050016, 0x00580061, 0x02000010, 0x00400001, //
0x00050016, 0x00580062, 0x02000020, 0x00800001, //
0x00050016, 0x00580063, 0x02000030, 0x00C00001, //
0x00050016, 0x00590064, 0x02000040, 0x00000001, //
0x00050016, 0x00590065, 0x02000050, 0x00400001, //
0x00050016, 0x00590066, 0x02000060, 0x00800001, //
0x00050016, 0x00590067, 0x02000070, 0x00C00001, //
0x00050016, 0x005A0068, 0x02000080, 0x00000001, //
0x00050016, 0x005A0069, 0x02000090, 0x00400001, //
0x00050016, 0x005A006A, 0x020000A0, 0x00800001, //
0x00050016, 0x005A006B, 0x020000B0, 0x00C00001, //
0x00050016, 0x005B006C, 0x020000C0, 0x00000001, //
0x00050016, 0x005B006D, 0x020000D0, 0x00400001, //
0x00050016, 0x005B006E, 0x020000E0, 0x00800001, //
0x00050016, 0x005B006F, 0x020000F0, 0x00C00001, //
0x00050017, 0x005C0070, 0x02000000, 0x00000001, //
0x00050017, 0x005C0071, 0x02000010, 0x00400001, //
0x00050017, 0x005C0072, 0x02000020, 0x00800001, //
0x00050017, 0x005C0073, 0x02000030, 0x00C00001, //
0x00050017, 0x005D0074, 0x02000040, 0x00000001, //
0x00050017, 0x005D0075, 0x02000050, 0x00400001, //
0x00050017, 0x005D0076, 0x02000060, 0x00800001, //
0x00050017, 0x005D0077, 0x02000070, 0x00C00001, //
0x00050017, 0x005E0078, 0x02000080, 0x00000001, //
0x00050017, 0x005E0079, 0x02000090, 0x00400001, //
0x00050017, 0x005E007A, 0x020000A0, 0x00800001, //
0x00050017, 0x005E007B, 0x020000B0, 0x00C00001, //
0x00050017, 0x005F007C, 0x020000C0, 0x00000001, //
0x00050017, 0x005F007D, 0x020000D0, 0x00400001, //
0x00050017, 0x005F007E, 0x020000E0, 0x00800001, //
0x00050017, 0x005F007F, 0x020000F0, 0x00C00001, //
0x00060018, 0x00600080, 0x02000000, 0x00000001, //
0x00060018, 0x00600081, 0x02000010, 0x00400001, //
0x00060018, 0x00600082, 0x02000020, 0x00800001, //
0x00060018, 0x00600083, 0x02000030, 0x00C00001, //
0x00060018, 0x00610084, 0x02000040, 0x00000001, //
0x00060018, 0x00610085, 0x02000050, 0x00400001, //
0x00060018, 0x00610086, 0x02000060, 0x00800001, //
0x00060018, 0x00610087, 0x02000070, 0x00C00001, //
0x00060018, 0x00620088, 0x02000080, 0x00000001, //
0x00060018, 0x00620089, 0x02000090, 0x00400001, //
0x00060018, 0x0062008A, 0x020000A0, 0x00800001, //
0x00060018, 0x0062008B, 0x020000B0, 0x00C00001, //
0x00060018, 0x0063008C, 0x020000C0, 0x00000001, //
0x00060018, 0x0063008D, 0x020000D0, 0x00400001, //
0x00060018, 0x0063008E, 0x020000E0, 0x00800001, //
0x00060018, 0x0063008F, 0x020000F0, 0x00C00001, //
0x00060019, 0x00640090, 0x02000000, 0x00000001, //
0x00060019, 0x00640091, 0x02000010, 0x00400001, //
0x00060019, 0x00640092, 0x02000020, 0x00800001, //
0x00060019, 0x00640093, 0x02000030, 0x00C00001, //
0x00060019, 0x00650094, 0x02000040, 0x00000001, //
0x00060019, 0x00650095, 0x02000050, 0x00400001, //
0x00060019, 0x00650096, 0x02000060, 0x00800001, //
0x00060019, 0x00650097, 0x02000070, 0x00C00001, //
0x00060019, 0x00660098, 0x02000080, 0x00000001, //
0x00060019, 0x00660099, 0x02000090, 0x00400001, //
0x00060019, 0x0066009A, 0x020000A0, 0x00800001, //
0x00060019, 0x0066009B, 0x020000B0, 0x00C00001, //
0x00060019, 0x0067009C, 0x020000C0, 0x00000001, //
0x00060019, 0x0067009D, 0x020000D0, 0x00400001, //
0x00060019, 0x0067009E, 0x020000E0, 0x00800001, //
0x00060019, 0x0067009F, 0x020000F0, 0x00C00001, //
0x0002000A, 0x002800A0, 0x02000000, 0x00000000, //
0x0002000A, 0x002800A1, 0x02000010, 0x00400000, //
0x0002000A, 0x002800A2, 0x02000020, 0x00800000, //
0x0002000A, 0x002800A3, 0x02000030, 0x00C00000, //
0x0002000A, 0x002900A4, 0x02000040, 0x00000000, //
0x0002000A, 0x002900A5, 0x02000050, 0x00400000, //
0x0002000A, 0x002900A6, 0x02000060, 0x00800000, //
0x0002000A, 0x002900A7, 0x02000070, 0x00C00000, //
0x0002000A, 0x002A00A8, 0x02000080, 0x00000000, //
0x0002000A, 0x002A00A9, 0x02000090, 0x00400000, //
0x0002000A, 0x002A00AA, 0x020000A0, 0x00800000, //
0x0002000A, 0x002A00AB, 0x020000B0, 0x00C00000, //
0x0002000A, 0x002B00AC, 0x020000C0, 0x00000000, //
0x0002000A, 0x002B00AD, 0x020000D0, 0x00400000, //
0x0002000A, 0x002B00AE, 0x020000E0, 0x00800000, //
0x0002000A, 0x002B00AF, 0x020000F0, 0x00C00000, //
0x0002000B, 0x002C00B0, 0x02000000, 0x00000000, //
0x0002000B, 0x002C00B1, 0x02000010, 0x00400000, //
0x0002000B, 0x002C00B2, 0x02000020, 0x00800000, //
0x0002000B, 0x002C00B3, 0x02000030, 0x00C00000, //
0x0002000B, 0x002D00B4, 0x02000040, 0x00000000, //
0x0002000B, 0x002D00B5, 0x02000050, 0x00400000, //
0x0002000B, 0x002D00B6, 0x02000060, 0x00800000, //
0x0002000B, 0x002D00B7, 0x02000070, 0x00C00000, //
0x0002000B, 0x002E00B8, 0x02000080, 0x00000000, //
0x0002000B, 0x002E00B9, 0x02000090, 0x00400000, //
0x0002000B, 0x002E00BA, 0x020000A0, 0x00800000, //
0x0002000B, 0x002E00BB, 0x020000B0, 0x00C00000, //
0x0002000B, 0x002F00BC, 0x020000C0, 0x00000000, //
0x0002000B, 0x002F00BD, 0x020000D0, 0x00400000, //
0x0002000B, 0x002F00BE, 0x020000E0, 0x00800000, //
0x0002000B, 0x002F00BF, 0x020000F0, 0x00C00000, //
0x0003000C, 0x003000C0, 0x02000000, 0x00000000, //
0x0003000C, 0x003000C1, 0x02000010, 0x00400000, //
0x0003000C, 0x003000C2, 0x02000020, 0x00800000, //
0x0003000C, 0x003000C3, 0x02000030, 0x00C00000, //
0x0003000C, 0x003100C4, 0x02000040, 0x00000000, //
0x0003000C, 0x003100C5, 0x02000050, 0x00400000, //
0x0003000C, 0x003100C6, 0x02000060, 0x00800000, //
0x0003000C, 0x003100C7, 0x02000070, 0x00C00000, //
0x0003000C, 0x003200C8, 0x02000080, 0x00000000, //
0x0003000C, 0x003200C9, 0x02000090, 0x00400000, //
0x0003000C, 0x003200CA, 0x020000A0, 0x00800000, //
0x0003000C, 0x003200CB, 0x020000B0, 0x00C00000, //
0x0003000C, 0x003300CC, 0x020000C0, 0x00000000, //
0x0003000C, 0x003300CD, 0x020000D0, 0x00400000, //
0x0003000C, 0x003300CE, 0x020000E0, 0x00800000, //
0x0003000C, 0x003300CF, 0x020000F0, 0x00C00000, //
0x0003000D, 0x003400D0, 0x02000000, 0x00000000, //
0x0003000D, 0x003400D1, 0x02000010, 0x00400000, //
0x0003000D, 0x003400D2, 0x02000020, 0x00800000, //
0x0003000D, 0x003400D3, 0x02000030, 0x00C00000, //
0x0003000D, 0x003500D4, 0x02000040, 0x00000000, //
0x0003000D, 0x003500D5, 0x02000050, 0x00400000, //
0x0003000D, 0x003500D6, 0x02000060, 0x00800000, //
0x0003000D, 0x003500D7, 0x02000070, 0x00C00000, //
0x0003000D, 0x003600D8, 0x02000080, 0x00000000, //
0x0003000D, 0x003600D9, 0x02000090, 0x00400000, //
0x0003000D, 0x003600DA, 0x020000A0, 0x00800000, //
0x0003000D, 0x003600DB, 0x020000B0, 0x00C00000, //
0x0003000D, 0x003700DC, 0x020000C0, 0x00000000, //
0x0003000D, 0x003700DD, 0x020000D0, 0x00400000, //
0x0003000D, 0x003700DE, 0x020000E0, 0x00800000, //
0x0003000D, 0x003700DF, 0x020000F0, 0x00C00000, //
0x0003000E, 0x003800E0, 0x02000000, 0x00000000, //
0x0003000E, 0x003800E1, 0x02000010, 0x00400000, //
0x0003000E, 0x003800E2, 0x02000020, 0x00800000, //
0x0003000E, 0x003800E3, 0x02000030, 0x00C00000, //
0x0003000E, 0x003900E4, 0x02000040, 0x00000000, //
0x0003000E, 0x003900E5, 0x02000050, 0x00400000, //
0x0003000E, 0x003900E6, 0x02000060, 0x00800000, //
0x0003000E, 0x003900E7, 0x02000070, 0x00C00000, //
0x0003000E, 0x003A00E8, 0x02000080, 0x00000000, //
0x0003000E, 0x003A00E9, 0x02000090, 0x00400000, //
0x0003000E, 0x003A00EA, 0x020000A0, 0x00800000, //
0x0003000E, 0x003A00EB, 0x020000B0, 0x00C00000, //
0x0003000E, 0x003B00EC, 0x020000C0, 0x00000000, //
0x0003000E, 0x003B00ED, 0x020000D0, 0x00400000, //
0x0003000E, 0x003B00EE, 0x020000E0, 0x00800000, //
0x0003000E, 0x003B00EF, 0x020000F0, 0x00C00000, //
0x0003000F, 0x003C00F0, 0x02000000, 0x00000000, //
0x0003000F, 0x003C00F1, 0x02000010, 0x00400000, //
0x0003000F, 0x003C00F2, 0x02000020, 0x00800000, //
0x0003000F, 0x003C00F3, 0x02000030, 0x00C00000, //
0x0003000F, 0x003D00F4, 0x02000040, 0x00000000, //
0x0003000F, 0x003D00F5, 0x02000050, 0x00400000, //
0x0003000F, 0x003D00F6, 0x02000060, 0x00800000, //
0x0003000F, 0x003D00F7, 0x02000070, 0x00C00000, //
0x0003000F, 0x003E00F8, 0x02000080, 0x00000000, //
0x0003000F, 0x003E00F9, 0x02000090, 0x00400000, //
0x0003000F, 0x003E00FA, 0x020000A0, 0x00800000, //
0x0003000F, 0x003E00FB, 0x020000B0, 0x00C00000, //
0x0003000F, 0x003F00FC, 0x020000C0, 0x00000000, //
0x0003000F, 0x003F00FD, 0x020000D0, 0x00400000, //
0x0003000F, 0x003F00FE, 0x020000E0, 0x00800000, //
0x0003000F, 0x003F00FF, 0x020000F0, 0x00C00000, //
0x0006001A, 0x006800A0, 0x02000000, 0x00000001, //
0x0006001A, 0x006800A1, 0x02000010, 0x00400001, //
0x0006001A, 0x006800A2, 0x02000020, 0x00800001, //
0x0006001A, 0x006800A3, 0x02000030, 0x00C00001, //
0x0006001A, 0x006900A4, 0x02000040, 0x00000001, //
0x0006001A, 0x006900A5, 0x02000050, 0x00400001, //
0x0006001A, 0x006900A6, 0x02000060, 0x00800001, // //
0x0006001A, 0x006900A7, 0x02000070, 0x00C00001, //
0x0006001A, 0x006A00A8, 0x02000080, 0x00000001, //
0x0006001A, 0x006A00A9, 0x02000090, 0x00400001, //
0x0006001A, 0x006A00AA, 0x020000A0, 0x00800001, //
0x8FBB1DB8, 0x76B63CEC, 0x025BEAED, 0xEC803A6B,
};

uint32 ResCodeLUT[] = {
 0xA862ECC6, 0x71D39FBF, 0x00F9F9F9, 0xF9F9F9CB, //0
 0x71939FBF, 0x8C2AE6B4, 0x00171737, 0xC6A8EC76, //1
 0xA862ECC6, 0x71D399B9, 0x00171717, 0x7171F9CB, //2
 0x71939FBF, 0x8C2AC6BE, 0x00555555, 0x8C2AEC76, //3
 0xF91B1737, 0xE6B471B3, 0x00ECC6A8, 0xECC66EFE, //4
 0xA862ECE6, 0x9F2FC6B8, 0x00ECC6A8, 0x1737F9CB, //5
 0x71939FBF, 0x8C5A37BD, 0x00C66E4E, 0xECE6EC76, //6
 0x8CCA6EEC, 0x9909A8B2, 0x00C66E4E, 0x17377163, //7
 0xA862ECE6, 0x9F5F17B7, 0x00717171, 0xF9F9F9CB, //8
 0xF91B1737, 0xC60E6EBC, 0x005555B5, 0xE6046EFE, //9
 0xA862ECE6, 0x9F5F11B1, 0x009F9F9F, 0x7171F9CB, //10
 0x55151111, 0xA8A2ECB6, 0x007171D3, 0x8C2A8C5A, //11
 0xF91B1737, 0xC69855BB, 0x006E8C2A, 0xECC66EFE, //12
 0x4EE0C66E, 0x17A74EB0, 0x00ECC6A8, 0x9FBF7163, //13
 0xF91B1737, 0xC69855B5, 0x006E8C2A, 0x6E4E6EFE, //14
 0x4EE0C66E, 0x17A78CBA, 0x006EECC6, 0x17377163, //15
 0x8CCA6E3C, 0xC62EE604, 0x00717171, 0x71717163, //16
 0x55757163, 0x17D79909, 0x00171737, 0xC6A88C5A, //17
 0x8CCA6E3C, 0xC62EC60E, 0x009F9F9F, 0xF9F97163, //18
 0x37FDF9CB, 0x71B37103, 0x009F9FBF, 0x6E8C6E1C, //19
 0x37FDF9CB, 0x7123C608, 0x00ECC6A8, 0xECC66E1C, //20
 0x8CCA6E3C, 0xC65E370D, 0x006E8C2A, 0x9FBF7163, //21
 0x55757163, 0x370DA802, 0x00C66E4E, 0xECE68C5A, //22
 0x8CCA6E3C, 0xC65E1707, 0x00C66E4E, 0x17377163, //23
 0xA862EC16, 0xA8026E0C, 0x00717171, 0xF9F9F9CB, //24
 0x37FDF9CB, 0x71531101, 0x005555B5, 0xE6046E1C, //25
 0x4EE0E6B4, 0xECA6EC06, 0x00171717, 0xF9F97163, //26
 0x55757163, 0x1797550B, 0x007171D3, 0x8C2A8C5A, //27
 0x55757143, 0xF9AB4E00, 0x00ECC6A8, 0xC66E8C5A, //28
 0x8CCA6E1C, 0x6E9C5505, 0x00ECC6A8, 0x9FBF7163, //29
 0x55757143, 0xF9AB8C0A, 0x006EECC6, 0xECE68C5A, //30
 0x4EE0E6B4, 0xECD69F0F, 0x006EECC6, 0x17377163, //31
 0x4EE0C668, 0x11915555, 0x00717171, 0x71717163, //32
 0x71939FBF, 0x4EA08C5A, 0x00171737, 0xC6A8EC76, //33
 0x4EE0C648, 0xF9DB9F5F, 0x009F9F9F, 0xF9F97163, //34
 0x9959551B, 0xA822E654, 0x007171D3, 0xC6A8C69E, //35
 0x71939FBF, 0x4ED09959, 0x006E8C2A, 0xC66EEC76, //36
 0x4EE0C668, 0x1727C65E, 0x006E8C2A, 0x9FBF7163, //37
 0x55757111, 0x6EBC7153, 0x00C66E4E, 0xECE68C5A, //38
 0x4EE0C668, 0x1727C658, 0x00C66E4E, 0x17377163, //39
 0x4EE0C668, 0x1757375D, 0x00F9F9F9, 0x71717163, //40
 0x379D99B9, 0xEC06A852, 0x005555B5, 0xE6046E1C, //41
 0x4EE0C668, 0x17571757, 0x00171717, 0xF9F97163, //42
 0x379D99B9, 0xEC066E5C, 0x00171737, 0x6E8C6E1C, //43
 0x55757111, 0x6E5C1151, 0x00ECC6A8, 0xC66E8C5A, //44
 0xA8624EE0, 0x9FAFEC56, 0x006E8C2A, 0x1737F9CB, //45
 0x71939FBF, 0x4E90555B, 0x006EECC6, 0xECE6EC76, //45
 0xA8624EE0, 0x9FAF4E50, 0x00C66E4E, 0x9FBFF9CB, //47
 0x4EE0C6BE, 0xEC26C6A8, 0x00717171, 0x71717163, //48
 0xF91B376D, 0x115137AD, 0x00171737, 0xE6046EFE, //49
 0x4EE0E6B4, 0x4E00A8A2, 0x009F9F9F, 0xF9F97163, //50
 0xF91B376D, 0x115117A7, 0x009F9FBF, 0x6E8C6EFE, //51
 0xF91B17A7, 0x55056EAC, 0x00ECC6A8, 0xECC66EFE, //52
 0x4EE0E6B4, 0x4EB071A1, 0x006E8C2A, 0x9FBF7163, //53
 0xF91B374D, 0xF9A9ECA6, 0x00ECC6A8, 0x6E4E6EFE, //54
 0xA862EC36, 0xC638F9AB, 0x006EECC6, 0x9FBFF9CB, //55
 0xA862EC16, 0x6EAE4EA0, 0x00717171, 0xF9F9F9CB, //56
 0x995955E5, 0x9F9F55A5, 0x00171737, 0x8C2AC69E, //57
 0xA862EC16, 0x6EAE8CAA, 0x009F9F9F, 0x7171F9CB, //58
 0xF91B374D, 0xF9D99FAF, 0x00171737, 0x6E8C6EFE, //59
 0xF91B17A7, 0x5525E6A4, 0x006E8C2A, 0xECC66EFE, //60
 0xA8624E10, 0xA832F9A9, 0x006E8C2A, 0x1737F9CB, //61
 0x995955E5, 0x9F0F6EAE, 0x00C66E4E, 0xC66EC69E, //62
 0x4EE0C6BE, 0x8CBA71A3, 0x006EECC6, 0x17377163, //63
 0xA8F2F91B, 0x6E8CF9F9, 0x00F9F9F9, 0xF9F9F9CB, //64
 0x9FCF6EAC, 0x55556EFE, 0x009F9FBF, 0x8C2AE694, //65
 0xC65E553B, 0xE60471F3, 0x009F9F9F, 0xF9F971E1, //66
 0x17678CCA, 0x7171C6F8, 0x009F9FBF, 0x6E8C6E1C, //67
 0x9FCFA862, 0x37AD37FD, 0x00ECE604, 0x6EECE694, //68
 0x8C7A71B3, 0x8C5AA8F2, 0x006E8C2A, 0x9FBF7163, //69
 0x9FCFA862, 0x37AD17F7, 0x006EECC6, 0xC66EE694, //70
 0x8C7A71B3, 0x8C5A6EFC, 0x00C66E4E, 0x17377163, //71
 0xC65E553B, 0xC60E71F1, 0x00F9F9F9, 0x717171E1, //72
 0x17678CCA, 0x71F3ECF6, 0x005555B5, 0xE6046E1C, //73
 0x8C7A71B3, 0xEC86F9FB, 0x00171717, 0xF9F97163, //74
 0x17678CCA, 0x71F34EF0, 0x00171737, 0x6E8C6E1C, //75
 0x9FCFA842, 0xF9EB55F5, 0x00C66E8C, 0x6EECE694, //76
 0xC65E553B, 0xC6F88CFA, 0x00ECC6A8, 0x9FBF71E1, //77
 0x17674EE0, 0x99299FFF, 0x006E8C2A, 0x6E4E6E1C, //78
 0xA8F2F9B9, 0x4E70E6F4, 0x00C66E4E, 0x9FBFF9CB, //79
 0xC65E55E5, 0x99596E4E, 0x00717171, 0x717171E1,
 0x17674E30, 0xC6087143, 0x00171737, 0xE6046E1C,
 0x8C7A71A1, 0x5575C648, 0x009F9F9F, 0xf9f97163,
 0x17674E10, 0x6EAC374D, 0x009F9FBF, 0x6E8C6E1C,
 0x17674E30, 0xC65EA842, 0x00ECC6A8, 0xECC66E1C,
 0xC6989FCF, 0x71A11747, 0x006E8C2A, 0x9FBF71E1,
 0x17674E30, 0xC65E6E4C, 0x00ECC6A8, 0x6E4E6E1C,
 0x4E101767, 0x370D7141, 0x00C66E4E, 0x17377163,
 0x4E101767, 0x17F7EC46, 0x00F9F9F9, 0x71717163,
 0x55E5C6B8, 0x8C8AF94B, 0x005555B5, 0xC6A88C5A,
 0x4E101767, 0x17F74E40, 0x00171717, 0xF9F97163,
 0x55E5C6B8, 0x8CEA5545, 0x007171D3, 0x8C2A8C5A,
 0x17674E10, 0x6EFC8C4A, 0x006E8C2A, 0xECC66E1C,
 0x4E101767, 0x372D9F4F, 0x00ECC6A8, 0x9FBF7163,
 0x17674E30, 0xE674E644, 0x006E8C2A, 0x6E4E6E1C,
 0xC6989FEF, 0x9F8FF949, 0x006EECC6, 0x173771E1,
 0xC6989F1F, 0xA8229F9F, 0x00717171, 0x717171E1,
 0x9FCF6E4C, 0xF979E694, 0x009F9FBF, 0x8C2AE694,
 0xC65E5535, 0xE684F999, 0x009F9F9F, 0xF9F971E1,
 0x9FCF6E6C, 0x375D6E9E, 0x007171D3, 0xC6A8E694,
 0x9FCF6E6C, 0x11017193, 0x00ECE604, 0x6EECE694,
 0xC65E5515, 0x6E7EC698, 0x006E8C2A, 0x9FBF71E1,
 0x1767ECA6, 0x55A5379D, 0x00ECC6A8, 0x6E4E6E1C,
 0xC6989F1F, 0xA852A892, 0x00C66E4E, 0x173771E1,
 0x4E1037BD, 0x8CAA1797, 0x00F9F9F9, 0x71717163,
 0x9FCF6E6C, 0x17576E9C, 0x00171737, 0x8C2AE694,
 0x4E1037BD, 0xEC067191, 0x00171717, 0xF9F97163,
 0x9FCF6E4C, 0xF9FBEC96, 0x00555555, 0xC6A8E694,
 0x55E5E6C4, 0x7183F99B, 0x00ECC6A8, 0xC66E8C5A,
 0x8C7A71B3, 0x4EF04E90, 0x00ECC6A8, 0x9FBF7163,
 0x55E5E6C4, 0x71E35595, 0x006EECC6, 0xECE68C5A,
 0x8C7A71B3, 0x4EF08C9A, 0x006EECC6, 0x17377163,
 0xA8F2F9AB, 0x55F5ECE6, 0x00F9F9F9, 0xF9F9F9CB,
 0x17678C3A, 0xC6E855EB, 0x00171737, 0xE6046E1C,
 0xA8F2F9AB, 0x55F54EE0, 0x00171717, 0x7171F9CB,
 0x17678C3A, 0xC6E855E5, 0x009F9FBF, 0x6E8C6E1C,
 0x9FCFA8B2, 0x8CFA8CEA, 0x00ECE604, 0x6EECE694,
 0xA8F2F9AB, 0x552B9FEF, 0x00ECC6A8, 0x1737F9CB,
 0x55E5E614, 0xA872E6E4, 0x00C66E4E, 0xECE68C5A,
 0xA8F2F9AB, 0x552B99E9, 0x006EECC6, 0x9FBFF9CB,
 0x8C1A1161, 0x1777C6EE, 0x00F9F9F9, 0x71717163,
 0x9FCFA8B2, 0x8C0A71E3, 0x00171737, 0x8C2AE694,
 0x8C1A1161, 0x1777C6E8, 0x00171717, 0xF9F97163,
 0x17678C1A, 0x6EAE37ED, 0x00171737, 0x6E8C6E1C,
 0x55E5C6B8, 0x4E50A8E2, 0x00ECC6A8, 0xC66E8C5A,
 0xA8F2F9C9, 0x71A317E7, 0x006E8C2A, 0x1737F9CB,
 0x55E5C6B8, 0x4E506EEC, 0x006EECC6, 0xECE68C5A,
 0xABF2F9C9, 0x71A311E1, 0x00C66E4E, 0x9FBFF9CB,
 0x6E6CEC46, 0xF9F91737, 0x00F9F9F9, 0xF9F9F9CB,
 0x379D9F3F, 0xC6A86E3C, 0x00171737, 0xE6046E1C,
 0x6E6CEC46, 0xF9F91131, 0x00171717, 0x7171F9CB,
 0x379D9F1F, 0x6E4EEC36, 0x009F9FBF, 0x6E8C6E1C,
 0x379D9F1F, 0x6E3C553B, 0x00ECC6A8, 0xECC66E1C,
 0xE6C46ECE, 0x71434E30, 0x006E8C2A, 0x9FBF71E1,
 0x379D9F1F, 0x6E3C5535, 0x00ECC6A8, 0x6E4E6E1C,
 0xE6C46ECE, 0x71438C3A, 0x00C66E4E, 0x173771E1,
 0x6E6CEC46, 0xF97B9F3F, 0x00717171, 0xF9F9F9CB,
 0x551537BD, 0x4EC0E634, 0x005555B5, 0xC6A88C5A,
 0x6E6CEC46, 0xF97B9939, 0x009F9F9F, 0x7171F9CB,
 0x551537BD, 0x4EC0C63E, 0x007171D3, 0x8C2A8C5A,
 0x379D9F3F, 0xE6547133, 0x006E8C2A, 0xECC66E1C,
 0x6E6C4EA0, 0x55C5C638, 0x006E8C2A, 0x1737F9CB,
 0x379D9F3F, 0xC6F8373D, 0x006E8C2A, 0x6E4E6E1C,
 0x6E6CEC66, 0x17A7A832, 0x00C66E4E, 0x9FBFF9CB,
 0x8CEAE614, 0x6EAC6E8C, 0x00717171, 0x71717163,
 0x7153556B, 0x37FD1181, 0x00171737, 0xC6A8EC76,
 0x8CEAC61E, 0xA842EC86, 0x009F9F9F, 0xF9F97163,
 0x715355A5, 0x553B558B, 0x00555555, 0x8C2AEC76,
 0x7153556B, 0x17474E80, 0x006E8C2A, 0xC66EEC76,
 0xE6C4A8B2, 0x4E305585, 0x006E8C2A, 0x9FBF71E1,
 0x7153556B, 0x17478C8A, 0x00C66E4E, 0xECE6EC76,
 0xE6C46EBC, 0x8C7A9F8F, 0x00C66E4E, 0x173771E1,
 0x8CEAE634, 0xE6C4E684, 0x00F9F9F9, 0x71717163,
 0x9F7F71E3, 0x9F7F9989, 0x00171737, 0x8C2AE694,
 0x8CEAE634, 0xE6C4C68E, 0x00171717, 0xF9F97163,
 0x9F7F71C3, 0x71537183, 0x00555555, 0xC6A8E694,
 0x17F7F94B, 0xF9C9C688, 0x006E8C2A, 0xECC66E1C,
 0x8CEAE614, 0x6EFC378D, 0x00ECC6A8, 0x9FBF7163,
 0x17F7F94B, 0xF9ABA882, 0x006E8C2A, 0x6E4E6E1C,
 0x8CEAE614, 0x6EFC1787, 0x006EECC6, 0x17377163,
 0xF97B7191, 0x9F1F71D3, 0x00F9F9F9, 0xF9F9F9F9,
 0x4EC06EEE, 0xE684C6D8, 0x009F9FBF, 0x8C2AC66E,
 0x71F3F919, 0x17B737DD, 0x009F9F9F, 0xF9F97171,
 0xC6684E60, 0xEC66A8D2, 0x009F9FBF, 0x6E8C6EEC,
 0xECE6C6C8, 0x6EBC17D7, 0x00ECE604, 0x6EECC66E,
 0x71111757, 0x556B6EDC, 0x006E8C2A, 0x9FBF7171,
 0x4EC06EEE, 0xE61471D1, 0x006EECC6, 0xC66EC66E,
 0xF97B7113, 0x1101ECD6, 0x006EECC6, 0x9FBFF9F9,
 0xF97B7173, 0x7191F9DB, 0x00717171, 0xF9F9F9F9,
 0xC6684E60, 0xEC064ED0, 0x005555B5, 0xE6046EEC,
 0x71F3F9FB, 0xF9FB55D5, 0x00171717, 0xF9F97171,
 0xC6684E60, 0xEC068CDA, 0x00171737, 0x6E8C6EEC,
 0x4EC06EEE, 0xC63E9FDF, 0x00C66E8C, 0x6EECC66E,
 0xF97B7173, 0x7183E6D4, 0x006E8C2A, 0x1737F9F9,
 0xC6684E60, 0xEC96F9D9, 0x006E8C2A, 0x6E4E6EEC,
 0x71111757, 0x55656EDE, 0x006EECC6, 0x17377171,
 0x9F5F5565, 0x4E008C2A, 0x009F9F9F, 0x9F9F9F9F,
 0xECE6C678, 0x71339F2F, 0x009F9FBF, 0x8C2AC66E,
 0xF97B71E3, 0xC688E624, 0x99171717, 0x7171F9F9,
 0xECE6C678, 0x71339929, 0x007171D3, 0xC6A8C66E,
 0x4EC06E1E, 0x378DC62E, 0x00ECE604, 0x6EECC66E,
 0xF97B71E3, 0xC6187123, 0x00ECC6A8, 0x1737F9F9,
 0x4EC06E1E, 0x378DC628, 0x006EECC6, 0xC66EC66E,
 0x9F5F5565, 0x4EB0372D, 0x00C66E8C, 0x55559F9F,
 0xF97B71E3, 0xE664A822, 0x00717171, 0xF9F9F9F9,
 0x4EC06E5C, 0x55B51727, 0x00171737, 0x8C2AC66E,
 0xF97B71E3, 0xE6646E2C, 0x009F9F9F, 0x7171F9F9,
 0xC6684EF0, 0xF9BB1121, 0x00171737, 0x6E8C6EEC,
 0xC668EC96, 0x9F0FEC26, 0x006E8C2A, 0xECC66EEC,
 0xF97B71E3, 0xE6F4552B, 0x006E8C2A, 0x1737F9F9,
 0xC668EC96, 0x9F0F4E20, 0x006E8C2A, 0x6E4E6EEC,
 0xF97B71E3, 0xE6F45525, 0x00C66E4E, 0x9FBFF9F9,
 0xF9EBC6E8, 0xC66E7171, 0x00F9F9F9, 0xF9F9F9F9,
 0x4E101151, 0x5555EC76, 0x009F9FBF, 0x8C2AC66E,
 0x376D8C6A, 0xECE6F97B, 0x00171737, 0x9F9F1717,
 0x4E101151, 0x55554E70, 0x007171D3, 0xC6A8C66E,
 0xEC16377D, 0x71435575, 0x00ECE604, 0x6EECC66E,
 0x376D8C6A, 0x8C5A8C7A, 0x00C66EEC, 0x55551717,
 0xEC16371D, 0x11819F7F, 0x006EECC6, 0xC66EC66E,
 0xF9EBC6E8, 0xC6D8E674, 0x006EECC6, 0x9FBFF9F9,
 0x376D8C6A, 0x4EE0F979, 0x009F9F9F, 0x17171717,
 0xE6545595, 0x99B96E7E, 0x005555B5, 0xE6046EEC,
 0xF9EBC6E8, 0xC6687173, 0x009F9F9F, 0x7171F9F9,
 0x4E707113, 0x37DDC678, 0x00555555, 0xC6A8C66E,
 0xEC16377D, 0x7103377D, 0x00C66E8C, 0x6EECC66E,
 0xF9EBC6E8, 0xE6B4A872, 0x006E8C2A, 0x1737F9F9,
 0xEC16377D, 0x71031777, 0x00C66E4E, 0xC66EC66E,
 0xF9EBC6E8, 0xC6BE6E7C, 0x00C66E4E, 0x9FBFF9F9,
 0x376D8C9A, 0x9959ECC6, 0x00171717, 0x17171717,
 0x6E9E9F6F, 0x8CEAF9CB, 0x00171737, 0xC6A8ECC6,
 0x376D8C9A, 0x99594EC0, 0x00171737, 0x9F9F1717,
 0xEC1617C7, 0xA84255C5, 0x007171D3, 0xC6A8C66E,
 0xEC1637ED, 0xC65E8CCA, 0x00ECE604, 0x6EECC66E,
 0x376DEC56, 0x55859FCF, 0x00C66EEC, 0x55551717,
 0xEC1617C7, 0x6EDCE6C4, 0x006EECC6, 0xC66EC66E,
 0x376D4E90, 0x9FEFF9C9, 0x00ECC6A8, 0x71D31717,
 0xF9EBC618, 0x37BD6ECE, 0x00717171, 0xF9F9F9F9,
 0xEC1617C7, 0x6E6C71C3, 0x00171737, 0x8C2AC66E,
 0x55C5A872, 0x71D1C6C8, 0x009F9F9F, 0x71D35555,
 0xEC1617C7, 0xA80237CD, 0x00555555, 0xC6A8C66E,
 0xEC1637ED, 0xC6B8A8C2, 0x00C66E8C, 0x6EECC66E,
 0xF9EBC618, 0x170717C7, 0x006E8C2A, 0x1737F9F9,
 0xEC1617C7, 0xA8B26ECC, 0x00C66E4E, 0xC66EC66E,
 0x55C5A812, 0x116171C1, 0x006EECC6, 0x11115555,
 0x55C56ECC, 0x6E0C1717, 0x00555555, 0x55555555,
 0xEC16371D, 0x17B76E1C, 0x009F9FBF, 0x8C2AC66E,
 0xF9EBE664, 0xEC667111, 0x00171717, 0x7171F9F9,
 0xEC16371D, 0x375DEC16, 0x007171D3, 0xC6A8C66E,
 0xEC161777, 0x71E3F91B, 0x00ECE604, 0x6EECC66E,
 0x55C56ECC, 0xA8524E10, 0x006E8C2A, 0x99995555,
 0x6E9E9FFF, 0xF94B5515, 0x00C66E4E, 0xECE6ECC6,
 0x55C56ECC, 0xA8528C1A, 0x00C66E4E, 0x11115555,
 0x55C5A8E2, 0xC68E9F1F, 0x007171D3, 0x55555555,
 0xE654555B, 0x55D5E614, 0x005555B5, 0xE6046EEC,
 0x55C5A8E2, 0xC6E8F919, 0x009F9F9F, 0x71D35555,
 0xEC161717, 0x11B16E1E, 0x00555555, 0xC6A8C66E,
 0xE654555B, 0x55657113, 0x006E8C2A, 0xECC66EEC,
 0xF9EBE664, 0x8CDAC618, 0x006E8C2A, 0x1737F9F9,
 0xE654555B, 0x550B371D, 0x006E8C2A, 0x6E4E6EEC,
 0x55C56ECC, 0x6EBCA812, 0x006EECC6, 0x11115555,
 0x376D8C9A, 0x9FDFC66E, 0x00171717, 0x17171717,
 0xC6989969, 0x8C6A7163, 0x00171737, 0xE6046EEC,
 0x376D8C9A, 0x9FDFC668, 0x00171737, 0x9F9F1717,
 0xC6989969, 0x4E00376D, 0x009F9FBF, 0x6E8C6EEC,
 0xC6989969, 0xECB6A862, 0x00ECC6A8, 0xECC66EEC,
 0x376D4E50, 0x55051767, 0x00C66EEC, 0x55551717,
 0xC6989969, 0x4EB06E6C, 0x00ECC6A8, 0x6E4E6EEC,
 0x376D4E50, 0x55051161, 0x00ECC6A8, 0x71D31717,
 0x376D4E50, 0x555BEC66, 0x009F9F9F, 0x17171717,
 0xEC1617E7, 0xE644556B, 0x00171737, 0x8C2AC66E,
 0x376D4E50, 0x555B4E60, 0x009F9FBF, 0x9F9F1717,
 0xEC1617E7, 0xE6445565, 0x00555555, 0xC6A8C66E,
 0xC6989969, 0x4E508C6A, 0x006E8C2A, 0xECC66EEC,
 0x55C5A812, 0x17879F6F, 0x00ECC6A8, 0x99995555,
 0x4E7071C1, 0x6EDEE664, 0x00C66E4E, 0xC66EC66E,
 0x55C5A812, 0x17879969, 0x006EECC6, 0x11115555,
 0x6E6C4E60, 0x375D71D3, 0x00F9F9F9, 0xF9F9F9CB,
 0x17F7F919, 0x6ECEC6D8, 0x00171737, 0xE6046E1C,
 0x8CEAC6C8, 0x71F137DD, 0x009F9F9F, 0xF9F97163,
 0x551517B7, 0x4EA0A8D2, 0x00555555, 0x8C2A8C5A,
 0x9F1F11B1, 0xECF617D7, 0x00ECE604, 0x6EECE694,
 0x6E6C8CAA, 0x55A56EDC, 0x00ECC6A8, 0x1737F9CB,
 0x9F7F7113, 0xA85271D1, 0x006EECC6, 0xC66EE694,
 0x6E6C4E40, 0xF94BECD6, 0x006EECC6, 0x9FBFF9CB,
 0x6E6C8CAA, 0x55D5F9DB, 0x00717171, 0xF9F9F9CB,
 0x551517B7, 0x4E404ED0, 0x005555B5, 0xC6A88C5A,
 0xE6C46EEE, 0x993955D5, 0x00171717, 0xF9F971E1,
 0xDEB04FDB, 0x4CF76A13, 0x000B73E7, 0x4AC64045,
	};

/*
 =======================================================================================================================
    Decode the PIF Rom Data
 =======================================================================================================================
 */
extern void __cdecl error(char *Message, ...);
void __cdecl iPifCheck(void)
{
	/*~~~~~~~~~~~~~~~~~*/
	int i, count, device;
	_u8 bufin[64];
	/*~~~~~~~~~~~~~~~~~*/
	uint8* PIF_Ram_Phys = &gMS_PIF[PIF_RAM_PHYS];
	for(i = 0; i < 64; i++)
	{
		bufin[i] = PIF_Ram_Phys[i ^ 3];
	}

#ifdef LOG_PIF
	LogPIFData(bufin, TRUE);
#endif
	count = 0;
	device = 0;
	{
		int cIdx;
		uint32 *tmp = (uint32*)bufin;
		if ((tmp[0] == 0xffffffff) && 
			(tmp[1] == 0xffffffff) &&
			(tmp[2] == 0xffffffff) &&
			(tmp[3] == 0xffffffff))
		{
			for(cIdx = 0; cIdx < sizeof(SrcCodeLUT) / sizeof(_u32);cIdx += 4)
			{
				if ((tmp[13] == SrcCodeLUT[cIdx]) && 
					(tmp[12] == SrcCodeLUT[cIdx+1]) &&
					(tmp[15] == SrcCodeLUT[cIdx+2]) &&
					(tmp[14] == SrcCodeLUT[cIdx+3]))
				{				
					tmp[13] = ResCodeLUT[cIdx];
					tmp[12] = ResCodeLUT[cIdx+1];
					tmp[15] = ResCodeLUT[cIdx+2];
					tmp[14] = ResCodeLUT[cIdx+3];

					tmp[11] = 0xffff;

					//bufin[63] = 0;	// Set the last bit is 0 as successfully return
					//error("Decrypt %08X %08X %08X %08X", tmp[13], tmp[12], tmp[15], tmp[14]);
					for(i = 0; i < 64; i++)
					{
						PIF_Ram_Phys[i ^ 3] = bufin[i];
					}
					return;
				}
			}
			return;
		}
	}
	while(count < 64)
	{
		/*~~~~~~~~~~~~~~~~~~~~~*/
		_u8 *cmd = &bufin[count];
		/*~~~~~~~~~~~~~~~~~~~~~*/

		/* Command Block is ready */
		if(cmd[0] == 0xFE)
		{
			count = 0x40;
			break;
		}

		/*
		 * no-op Commands ?		 * FD is from Command and Conquer
		 */
		if((cmd[0] == 0xFF) || (cmd[0] == 0xFD))
		{
			count++;
			continue;
		}
		if((cmd[0] ==  0xB4) || (cmd[0] == 0x56) || (cmd[0] == 0xB8))
		{ //???
			count++;
			continue;
		}

		/* Next Device */
		if(cmd[0] == 0x00)
		{
			count++;
			device++;
			continue;
		}

		/* Device Channel to large (0-3 = Controller, 4 = EEprom) */
		if(device > 4)
		{
			device+=0;
			break;
		}

		/* We get a Real Command now =) */
		switch(device)
		{
		/* Controler Command */
		case 0:
		case 1:
		case 2:
		case 3:
#ifdef _XBOX
			if(Controls[device].RawData)
#else //win32
			if(Controls[device].RawData && Kaillera_Is_Running == FALSE )
#endif
			{
				CONTROLLER_ControllerCommand(device, cmd);
				CONTROLLER_ReadController(device, cmd);
				break;
			}
			else
			{
				if(!ControllerCommand(cmd, device))
				{
					count = 64;
				}
			}
			break;

		/* EEprom Command */
		case 4:
			if(!EEpromCommand(cmd))
			{
				count = 64;
			}
			break;

		default:
			DisplayError("Unknown Command for unknwon Device %x", device);
			return;
		}

#ifdef _DEBUG
		switch(cmd[2])
		{
		case 0x00:
		case 0xFF:
			if(debugoptions.debug_si_controller)
			{
				sprintf(tracemessage, "Get Status %d: %02X %02X %02X %02X %02X %02X %02X", device,
						cmd[0], cmd[1], cmd[2], cmd[3], cmd[4], cmd[5], cmd[6]); 
				RefreshOpList(tracemessage);
			}
			break;
		case 0x01:
			if(debugoptions.debug_si_controller)
			{
				sprintf(tracemessage, "Read Controller %d: %02X %02X %02X %02X %02X %02X %02X", device,
						cmd[0], cmd[1], cmd[2], cmd[3], cmd[4], cmd[5], cmd[6]); 
				RefreshOpList(tracemessage);
			}
			break;
		case 0x02:
			if(debugoptions.debug_si_mempak)
			{
				sprintf(tracemessage, "Read Mempak %d: %02X %02X %02X %02X %02X %02X %02X", device,
						cmd[0], cmd[1], cmd[2], cmd[3], cmd[4], cmd[5], cmd[6]); 
				RefreshOpList(tracemessage);
			}
			break;
		case 0x03:
			if(debugoptions.debug_si_mempak)
			{
				sprintf(tracemessage, "Write Mempak %d: %02X %02X %02X %02X %02X %02X %02X", device,
						cmd[0], cmd[1], cmd[2], cmd[3], cmd[4], cmd[5], cmd[6]); 
				RefreshOpList(tracemessage);
			}
			break;
		}
#endif

		/*
		 * increase count ?		 * i think that the maximum command size is 12 bytes long ?		 * if it is longer than 12 bytes i cut it 12 Bytes ?		 * i think so because normally the formula ?		 * (size of Command-Bytes + size of Answer-Bytes + 2 for the 2 size Bytes) for the ?		 * command size works pretty fine, but it doesn't work together with the EEprom
		 * Write CMD
		 */
		device++;	/* only one command per controller =) */
		count += cmd[0] +
		(cmd[1] & 0x3f) +
		2;			/* size of Command-Bytes + size of Answer-Bytes + 2 for the 2 size Bytes */
	}

#ifdef _XBOX
	if(Controls[0].RawData)
#else //win32
	if(Controls[0].RawData && Kaillera_Is_Running == FALSE )
#endif
	{
		CONTROLLER_ControllerCommand(-1, bufin);	/* 1 signalling end of processing the pif ram. */
	}

	/*
	 * write answer packet to pi_ram ?	 * bufin[63] = 1;
	 */
	bufin[63] = 0;	/* Set the last bit is 0 as successfully return */
	for(i = 0; i < 64; i++)
	{
		PIF_Ram_Phys[i ^ 3] = bufin[i];
	}

#ifdef LOG_PIF
	LogPIFData(bufin, FALSE);
#endif
}

/*
 =======================================================================================================================
    Log PIF Data
 =======================================================================================================================
 */
void LogPIFData(char *data, BOOL input)
{
	FILE *stream = fopen("c:/pif_data.txt", "at");
	if(stream != NULL)
	{
		int				i, j;
		unsigned char	*p = (unsigned char*)data;

		if(input)
		{
			fprintf(stream, "\nIncoming\n");
		}
		else
		{
			fprintf(stream, "\nOutgoing\n");
		}

		for(i = 0; i < 8; i++)
		{
			for(j = 0; j < 8; j++)
			{
				fprintf(stream, "%02x ", *p);
				p++;
			}

			fprintf(stream, "\n");
		}

		fclose(stream);
	}
}
#endif //USE_ICC_LIB