/*
Copyright (C) 2002 Rice1964

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

// Some new GBI2 extension ucodes
void RSP_GBI2_DL_Count(uint32 word0, uint32 word1)
{
	SP_Timing(DP_Minimal);
	DP_Timing(DP_Minimal);

	// This cmd is likely to execute number of ucode at the given address
	uint32 dwAddr = RSPSegmentAddr(word1);
	{
		g_dwPCindex++;
		g_dwPCStack[g_dwPCindex].addr = dwAddr;
		g_dwPCStack[g_dwPCindex].limit = (word0&0xFFFF);
	}
}


void RSP_GBI2_0x8(uint32 word0, uint32 word1)
{
	if( (word0&0x00FFFFFF) == 0x2F && (word1&0xFF000000) == 0x80000000 )
	{
		// V-Rally 64
		RSP_S2DEX_SPObjLoadTxRectR(word0, word1);
	}
	else
	{
		RSP_RDP_Nothing(word0, word1);
	}
}


