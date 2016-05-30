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

// Texture related ucode

uint32 g_TmemFlag[16];
void SetTmemFlag(uint32 tmemAddr, uint32 size);
bool IsTmemFlagValid(uint32 tmemAddr);
uint32 GetValidTmemInfoIndex(uint32 tmemAddr);

CTexture* EnhanceTexture(TxtrCacheEntry *pEntry);
CTexture* MirrorTexture(uint32 tileno, TxtrCacheEntry *pEntry);

extern TMEMLoadMapInfo g_tmemInfo0;				// Info for Tmem=0
extern TMEMLoadMapInfo g_tmemInfo1;				// Info for Tmem=0x100

TmemType g_Tmem;

/************************************************************************/
/*                                                                      */
/************************************************************************/
uint32 sizeShift[4] = {2,1,0,0};
uint32 sizeIncr[4] = {3,1,0,0};
uint32 sizeBytes[4] = {0,1,2,4};

inline uint32 Txl2Words(uint32 width, uint32 size)
{
	if( size == TXT_SIZE_4b )
		return max(1, width/16);
	else
		return max(1, width*sizeBytes[size]/8);
}

inline uint32 CalculateImgSize(uint32 width, uint32 height, uint32 size)
{
	//(((width)*(height) + siz##_INCR) >> siz##_SHIFT) -1
	return (((width)*(height) + sizeIncr[size]) >> sizeShift[size]) -1;
}


inline uint32 CalculateDXT(uint32 txl2words)
{
	//#define CALC_DXT(width, b_txl)	((2048 + TXL2WORDS(width, b_txl) - 1) / TXL2WORDS(width, b_txl))
	if( txl2words == 0 ) return 1;
	else return (2048+txl2words-1)/txl2words;
}

inline uint32 ReverseDXT(uint32 val, uint32 lrs, uint32 width, uint32 size)
{
	//#define TXL2WORDS(txls, b_txl)	MAX(1, ((txls)*(b_txl)/8))
	if( val == 0x800 ) return 1;
	
	int low = 2047/val;
	if( CalculateDXT(low) > val )	low++;
	int high = 2047/(val-1);

	if( low == high )	return low;

	for( int i=low; i<=high; i++ )
	{
		if( Txl2Words(width, size) == i )
			return i;
	}

	return	(low+high)/2;	//dxt = 2047 / (dxt-1);
}

// The following inline assemble routines are borrowed from glN64, I am too tired to
// rewrite these routine by myself.
inline void UnswapCopy( void *src, void *dest, uint32 numBytes )
{
	__asm
	{
		mov		ecx, 0
		mov		esi, dword ptr [src]
		mov		edi, dword ptr [dest]

		mov		ebx, esi
		and		ebx, 3			// ebx = number of leading bytes

		cmp		ebx, 0
		jz		StartDWordLoop
		neg		ebx
		add		ebx, 4

		cmp		ebx, [numBytes]
		jle		NotGreater
		mov		ebx, [numBytes]
NotGreater:
		mov		ecx, ebx
			xor		esi, 3
LeadingLoop:				// Copies leading bytes, in reverse order (un-swaps)
		mov		al, byte ptr [esi]
		mov		byte ptr [edi], al
		sub		esi, 1
		add		edi, 1
		loop	LeadingLoop
		add		esi, 5

StartDWordLoop:
		mov		ecx, dword ptr [numBytes]
		sub		ecx, ebx		// Don't copy what's already been copied

		mov		ebx, ecx
		and		ebx, 3
		//		add		ecx, 3			// Round up to nearest dword
		shr		ecx, 2

		cmp		ecx, 0			// If there's nothing to do, don't do it
		jle		StartTrailingLoop

		// Copies from source to destination, bswap-ing first
DWordLoop:
		mov		eax, dword ptr [esi]
		bswap	eax
		mov		dword ptr [edi], eax
		add		esi, 4
		add		edi, 4
		loop	DWordLoop
StartTrailingLoop:
		cmp		ebx, 0
		jz		Done
		mov		ecx, ebx
		xor		esi, 3

TrailingLoop:
		mov		al, byte ptr [esi]
		mov		byte ptr [edi], al
		sub		esi, 1
		add		edi, 1
		loop	TrailingLoop
Done:
	}
}

inline void DWordInterleave( void *mem, uint32 numDWords )
{
	__asm {
		mov		esi, dword ptr [mem]
		mov		edi, dword ptr [mem]
		add		edi, 4
		mov		ecx, dword ptr [numDWords]
DWordInterleaveLoop:
		mov		eax, dword ptr [esi]
		mov		ebx, dword ptr [edi]
		mov		dword ptr [esi], ebx
		mov		dword ptr [edi], eax
		add		esi, 8
		add		edi, 8
		loop	DWordInterleaveLoop
	}
}

inline void QWordInterleave( void *mem, uint32 numDWords )
{
	__asm
	{
		// Interleave the line on the qword
		mov		esi, dword ptr [mem]
		mov		edi, dword ptr [mem]
		add		edi, 8
		mov		ecx, dword ptr [numDWords]
		shr		ecx, 1
QWordInterleaveLoop:
		mov		eax, dword ptr [esi]
		mov		ebx, dword ptr [edi]
		mov		dword ptr [esi], ebx
		mov		dword ptr [edi], eax
		add		esi, 4
		add		edi, 4
		mov		eax, dword ptr [esi]
		mov		ebx, dword ptr [edi]
		mov		dword ptr [esi], ebx
		mov		dword ptr [edi], eax
		add		esi, 12
		add		edi, 12
		loop	QWordInterleaveLoop
	}
}

inline uint32 swapdword( uint32 value )
{
	__asm
	{
		mov		eax, dword ptr [value]
		bswap	eax
	}
}

inline uint16 swapword( uint16 value )
{
	__asm
	{
		mov		ax, word ptr [value]
		xchg	ah, al
	}
}


void ComputeTileDimension(int mask, int clamp, int mirror, int width, uint32 &widthToCreate, uint32 &widthToLoad)
{
	int maskwidth = mask > 0 ? (1<<mask) : 0;
	widthToCreate = widthToLoad = width;

	if( mask > 0 )
	{
		if( width > maskwidth )
		{
			if( clamp == 0 )
			{
				// clamp is not used, so just use the dwTileMaskWidth as the real width
				widthToCreate = widthToLoad = maskwidth;
			}
			else
			{
				widthToLoad = maskwidth;
				//gti.WidthToCreate = dwTileWidth;
				// keep the current WidthToCreate, we will do mirror/wrap
				// during texture loading, not during rendering
			}
		}
		else if( width < maskwidth )
		{
			// dwTileWidth < dwTileMaskWidth

			if( clamp == 0 )
			{
				if( maskwidth%width == 0 )
				{
					if( (maskwidth/width)%2 == 0 || mirror == 0 )
					{
						// Do nothing
						// gti.WidthToLoad = gti.WidthToCreate = gRDP.tiles[tileno].dwWidth = dwTileWidth
					}
					else
					{
						widthToCreate = maskwidth;
					}
				}
				else
				{
					widthToCreate = maskwidth;
					//widthToLoad = maskwidth;
				}
			}
			else
			{
				widthToCreate = maskwidth;
				//widthToLoad = maskwidth;
			}
		}
		else // dwTileWidth == dwTileMaskWidth
		{
		}

		// Some hacks, to limit the image size
		if( mask >= 8 )
		{
			if( maskwidth / width >= 2 )
			{
				widthToCreate = width;
			}
		}
	}
}

bool conkerSwapHack=false;

bool CalculateTileSizes_method_1(int tileno, TMEMLoadMapInfo *info, TxtrInfo &gti)
{
	Tile &tile = gRDP.tiles[tileno];
	Tile &loadtile = gRDP.tiles[RDP_TXT_LOADTILE];
	uint32 infoTmemAddr = tile.dwTMem;
	uint32 dwPitch;

	// Now Initialize the texture dimension
	int dwTileWidth;
	int dwTileHeight;
	if( info->bSetBy == CMD_LOADTILE )
	{
#ifdef _RICE560_CLAMP
			if( tile.dwMaskS == 0 || tile.bClampS )
			{
				dwTileWidth = tile.hilite_sh - tile.hilite_sl +1;
				if( dwTileWidth < tile.sh - tile.sl +1 )
					dwTileWidth = tile.sh - tile.sl +1;
			}
			else
			{
				if( tile.dwMaskS < 8 )
					dwTileWidth = (1 << tile.dwMaskS );
				else
				{
					dwTileWidth = info->dwWidth;	// From SetTImage
					dwTileWidth = dwTileWidth << info->dwSize >> tile.dwSize;
				}
			}

			if( tile.dwMaskT == 0 || tile.bClampT )
			{
				dwTileHeight= tile.hilite_th - tile.hilite_tl +1;
				if( dwTileHeight < tile.th - tile.tl +1 )
					dwTileHeight = tile.th - tile.tl +1;
			}
			else
			{
				if( tile.dwMaskT < 8 )
					dwTileHeight = (1 << tile.dwMaskT );
				else
				{
					if( tile.tl >= tile.th )
					{
						dwTileHeight= info->th - info->tl + 1;
					}
					else
					{
						dwTileHeight= tile.th - tile.tl + 1;
					}
				}
			}
#else
		if( tile.sl >= tile.sh )
		{
			dwTileWidth = info->dwWidth;	// From SetTImage
			dwTileWidth = dwTileWidth << info->dwSize >> tile.dwSize;
		}
		else
		{
			dwTileWidth= tile.sh - tile.sl + 1;
		}

		if( tile.tl >= tile.th )
		{
			dwTileHeight= info->th - info->tl + 1;
		}
		else
		{
			dwTileHeight= tile.th - tile.tl + 1;
		}
#endif
	}
	else
	{
		if( tile.dwMaskS == 0 || tile.bClampS )
		{
			dwTileWidth = tile.hilite_sh - tile.hilite_sl +1;
			if( dwTileWidth < tile.sh - tile.sl +1 )
				dwTileWidth = tile.sh - tile.sl +1;
			if( dwTileWidth <= 0 )
			{
			}
		}
		else
		{
			if( tile.dwMaskS < 8 )
				dwTileWidth = (1 << tile.dwMaskS );
			else if( tile.dwLine )
			{
				dwTileWidth = (tile.dwLine<<5)>>tile.dwSize;
			}
			else
			{
				if( tile.sl <= tile.sh )
				{
					dwTileWidth = tile.sh - tile.sl +1;
				}
				else if( loadtile.sl <= loadtile.sh )
				{
					dwTileWidth = loadtile.sh - loadtile.sl +1;
				}
				else
				{
					dwTileWidth = tile.sh - tile.sl +1;
				}
			}
		}

		if( tile.dwMaskT == 0 || tile.bClampT )
		{
			dwTileHeight= tile.hilite_th - tile.hilite_tl +1;
			if( dwTileHeight < tile.th - tile.tl +1 )
				dwTileHeight = tile.th - tile.tl +1;

			if( dwTileHeight <= 0 )
			{
			}
		}
		else
		{
			if( tile.dwMaskT < 8 )
				dwTileHeight = (1 << tile.dwMaskT );
			else if( tile.tl <= tile.th )
			{
				dwTileHeight = tile.th - tile.tl +1;
			}
			else if( loadtile.tl <= loadtile.th )
			{
				dwTileHeight = loadtile.th - loadtile.tl +1;
			}
			else
			{
				dwTileHeight = tile.th - tile.tl +1;
			}
		}
	}

	int dwTileMaskWidth = tile.dwMaskS > 0 ? (1 << tile.dwMaskS ) : 0;
	int dwTileMaskHeight = tile.dwMaskT > 0 ? (1 << tile.dwMaskT ) : 0;

	if( dwTileWidth < 0 || dwTileHeight < 0)
	{
		if( dwTileMaskWidth > 0 )
			dwTileWidth = dwTileMaskWidth;
		else if( dwTileWidth < 0 )
			dwTileWidth = -dwTileWidth;

		if( dwTileMaskHeight > 0 )
			dwTileHeight = dwTileMaskHeight;
		else if( dwTileHeight < 0 )
			dwTileHeight = -dwTileHeight;
	}



	if( dwTileWidth-dwTileMaskWidth == 1 && dwTileMaskWidth && dwTileHeight-dwTileMaskHeight == 1 && dwTileMaskHeight )
	{
		// Hack for Mario Kart
		dwTileWidth--;
		dwTileHeight--;
	}

	ComputeTileDimension(tile.dwMaskS, tile.bClampS,
		tile.bMirrorS, dwTileWidth, gti.WidthToCreate, gti.WidthToLoad);
	tile.dwWidth = gti.WidthToCreate;

	ComputeTileDimension(tile.dwMaskT, tile.bClampT,
		tile.bMirrorT, dwTileHeight, gti.HeightToCreate, gti.HeightToLoad);
	tile.dwHeight = gti.HeightToCreate;

	gti.bSwapped = info->bSwapped;

	if( info->bSetBy == CMD_LOADTILE )
	{
		// It was a tile - the pitch is set by LoadTile
		dwPitch = info->dwWidth<<(info->dwSize-1);

		if( dwPitch == 0 )
		{
			dwPitch = 1024;		// Hack for Bust-A-Move
		}
	}
	else	//Set by LoadBlock
	{
		// It was a block load - the pitch is determined by the tile size
		if (info->dxt == 0 || info->dwTmem != tile.dwTMem )
		{
			dwPitch = tile.dwLine << 3;
			gti.bSwapped = TRUE;
			if( info->dwTmem != tile.dwTMem && info->dxt != 0 && info->dwSize == TXT_SIZE_16b && tile.dwSize == TXT_SIZE_4b )
				conkerSwapHack = true;
		}
		else
		{
			uint32 DXT = info->dxt;
			if( info->dxt > 1 )
			{
				DXT = ReverseDXT(info->dxt, info->sh, dwTileWidth, tile.dwSize);
			}
			dwPitch = DXT << 3;
		}

		if (tile.dwSize == TXT_SIZE_32b)
			dwPitch = tile.dwLine << 4;
	}

	gti.Pitch = tile.dwPitch = dwPitch;

	if( (gti.WidthToLoad < gti.WidthToCreate || tile.bSizeIsValid == false) && tile.dwMaskS > 0 && gti.WidthToLoad != dwTileMaskWidth &&
		info->bSetBy == CMD_LOADBLOCK )
		//if( (gti.WidthToLoad < gti.WidthToCreate ) && tile.dwMaskS > 0 && gti.WidthToLoad != dwTileMaskWidth &&
		//	info->bSetBy == CMD_LOADBLOCK )
	{
		// We have got the pitch now, recheck the width_to_load
		uint32 pitchwidth = dwPitch<<1>>tile.dwSize;
		if( pitchwidth == dwTileMaskWidth )
		{
			gti.WidthToLoad = pitchwidth;
		}
	}
	if( (gti.HeightToLoad < gti.HeightToCreate  || tile.bSizeIsValid == false) && tile.dwMaskT > 0 && gti.HeightToLoad != dwTileMaskHeight &&
		info->bSetBy == CMD_LOADBLOCK )
		//if( (gti.HeightToLoad < gti.HeightToCreate  ) && tile.dwMaskT > 0 && gti.HeightToLoad != dwTileMaskHeight &&
		//	info->bSetBy == CMD_LOADBLOCK )
	{
		uint32 pitchwidth = dwPitch<<1>>tile.dwSize;
		uint32 pitchHeight = (info->dwTotalWords<<1)/dwPitch;
		if( pitchHeight == dwTileMaskHeight || gti.HeightToLoad == 1 )
		{
			gti.HeightToLoad = pitchHeight;
		}
	}
	if( gti.WidthToCreate < gti.WidthToLoad )	gti.WidthToCreate = gti.WidthToLoad;
	if( gti.HeightToCreate < gti.HeightToLoad )		gti.HeightToCreate = gti.HeightToLoad;

	if( info->bSetBy == CMD_LOADTILE )
	{
		gti.LeftToLoad = (info->sl<<info->dwSize)>>tile.dwSize;
		gti.TopToLoad = info->tl;
	}
	else
	{
		gti.LeftToLoad = (info->sl<<info->dwSize)>>tile.dwSize;
		gti.TopToLoad = (info->tl<<info->dwSize)>>tile.dwSize;
	}

	uint32 total64BitWordsToLoad = (gti.HeightToLoad*gti.WidthToLoad)>>(4-tile.dwSize);
	if( total64BitWordsToLoad + tile.dwTMem > 0x200 )
	{
		if( gti.WidthToLoad > gti.HeightToLoad )
		{
			uint32 newheight = (dwPitch << 1 )>> tile.dwSize;
			tile.dwWidth = gti.WidthToLoad = gti.WidthToCreate = min(newheight, (gti.WidthToLoad&0xFFFFFFFE));
			tile.dwHeight = gti.HeightToCreate = gti.HeightToLoad = ((0x200 - tile.dwTMem) << (4-tile.dwSize)) / gti.WidthToLoad;
		}
		else
		{
			tile.dwHeight = gti.HeightToCreate = gti.HeightToLoad = info->dwTotalWords / ((gti.WidthToLoad << tile.dwSize) >> 1);
		}
	}

	// Check the info
	if( (info->dwTotalWords>>2) < total64BitWordsToLoad+tile.dwTMem-info->dwTmem - 4 )
	{
		// Hack here
		if( (options.enableHackForGames == HACK_FOR_ZELDA||options.enableHackForGames == HACK_FOR_ZELDA_MM) && tileno != gRSP.curTile )
		{
			return false;
		}

		if( total64BitWordsToLoad+tile.dwTMem-info->dwTmem <= 0x200 )
		{
		}
	}

	//Check memory boundary
	if( gti.Address + gti.HeightToLoad*gti.Pitch >= g_dwRamSize )
	{
		gti.HeightToCreate = gti.HeightToLoad = tile.dwHeight = (g_dwRamSize-gti.Address)/gti.Pitch;
	}

	return true;
}

bool CalculateTileSizes_method_2(int tileno, TMEMLoadMapInfo *info, TxtrInfo &gti)
{
	Tile &tile = gRDP.tiles[tileno];
	Tile &loadtile = gRDP.tiles[RDP_TXT_LOADTILE];

	// Now Initialize the texture dimension
	int loadwidth, loadheight;

	int maskwidth = tile.dwMaskS ? (1 << tile.dwMaskS ) : 0;
	int maskheight = tile.dwMaskT ? (1 << tile.dwMaskT ) : 0;
	int clampwidth = abs(tile.hilite_sh - tile.hilite_sl) +1;
	int clampheight = abs(tile.hilite_th - tile.hilite_tl) +1;
	int linewidth = tile.dwLine << (5 - tile.dwSize);

	gti.bSwapped = info->bSwapped;

	if( info->bSetBy == CMD_LOADTILE )
	{
		loadwidth = (abs(info->sh - info->sl) + 1) << info->dwSize >> tile.dwSize;
		loadheight = (abs(info->th - info->tl) + 1) << info->dwSize >> tile.dwSize;

		tile.dwPitch = info->dwWidth << info->dwSize >> 1;
		if( tile.dwPitch == 0 ) tile.dwPitch = 1024;		// Hack for Bust-A-Move

		gti.LeftToLoad = (info->sl<<info->dwSize)>>tile.dwSize;
		gti.TopToLoad = info->tl;
	}
	else
	{
		loadwidth = abs(tile.sh - tile.sl) +1;
		if( tile.dwMaskS )	
		{
#ifdef _VID1964_FIX
			if( !tile.bSizeIsValid )	
				loadwidth = maskwidth;
			else
				loadwidth = min(loadwidth, maskwidth);
#else
			loadwidth = maskwidth;
#endif
		}

		loadheight = abs(tile.th - tile.tl) +1;
		if( tile.dwMaskT )	
		{
#ifdef _VID1964_FIX
			if( !tile.bSizeIsValid )	
				loadheight = maskheight;
			else
				loadheight = min(loadheight, maskheight);
#else
			loadheight = maskheight;
#endif
		}


		// It was a block load - the pitch is determined by the tile size
		if (tile.dwSize == TXT_SIZE_32b)
			tile.dwPitch = tile.dwLine << 4;
		else if (info->dxt == 0 )
		{
			tile.dwPitch = tile.dwLine << 3;
			gti.bSwapped = TRUE;
			if( info->dwTmem != tile.dwTMem && info->dxt != 0 && info->dwSize == TXT_SIZE_16b && tile.dwSize == TXT_SIZE_4b )
				conkerSwapHack = true;
		}
		else
		{
			uint32 DXT = info->dxt;
			if( info->dxt > 1 )
			{
				DXT = ReverseDXT(info->dxt, info->sh, loadwidth, tile.dwSize);
			}
			tile.dwPitch = DXT << 3;
		}

		gti.LeftToLoad = (info->sl<<info->dwSize)>>tile.dwSize;
		gti.TopToLoad = (info->tl<<info->dwSize)>>tile.dwSize;
	}

	if( options.enableHackForGames == HACK_FOR_MARIO_KART )
	{
		if( loadwidth-maskwidth == 1 && tile.dwMaskS )
		{
			loadwidth--;
			if( loadheight%2 )	loadheight--;
		}

		if( loadheight-maskheight == 1 && tile.dwMaskT )
		{
			loadheight--;
			if(loadwidth%2)	loadwidth--;
		}

		if( loadwidth - ((g_TI.dwWidth<<g_TI.dwSize)>>tile.dwSize) == 1 )
		{
			loadwidth--;
			if( loadheight%2 )	loadheight--;
		}
	}

	// Limit the texture size
	if( g_curRomInfo.bUseSmallerTexture )
	{
#ifndef _RICE612_CLAMP
		if( tile.dwMaskS && tile.bClampS && !tile.bMirrorS )
		{
			if( clampwidth/maskwidth >= 2 )
			{
				clampwidth = maskwidth;
				tile.bForceWrapS = true;
			}
			else if( clampwidth && maskwidth/clampwidth >= 2 )
			{
				maskwidth = clampwidth;
				tile.bForceClampS = true;
			}
		}

		if( tile.dwMaskT && tile.bClampT && !tile.bMirrorT )
		{
			if( clampheight/maskheight >= 2 )
			{
				clampheight = maskheight;
				tile.bForceWrapT = true;
			}
			else if( clampheight && maskheight/clampheight >= 2 )
			{
				maskwidth = clampwidth;
				tile.bForceClampT = true;
			}
		}
#else
		if( tile.dwMaskS && tile.bClampS )
		{
			if( !tile.bMirrorS )
			{
				if( clampwidth/maskwidth >= 2 )
				{
					clampwidth = maskwidth;
					tile.bForceWrapS = true;
				}
				else if( clampwidth && maskwidth/clampwidth >= 2 )
				{
					maskwidth = clampwidth;
					tile.bForceClampS = true;
				}
			}
			else
			{
				if( clampwidth/maskwidth == 2 )
				{
					clampwidth = maskwidth*2;
					tile.bForceWrapS = false;
				}
				else if( clampwidth/maskwidth > 2 )
				{
					clampwidth = maskwidth*2;
					tile.bForceWrapS = true;
				}
			}
		}

		if( tile.dwMaskT && tile.bClampT )
		{
			if( !tile.bMirrorT )
			{
				if( clampheight/maskheight >= 2 )
				{
					clampheight = maskheight;
					tile.bForceWrapT = true;
				}
				else if( clampheight && maskheight/clampheight >= 2 )
				{
					maskwidth = clampwidth;
					tile.bForceClampT = true;
				}
			}
			else
			{
				if( clampheight/maskheight == 2 )
				{
					clampheight = maskheight*2;
					tile.bForceWrapT = false;
				}
				else if( clampheight/maskheight >= 2 )
				{
					clampheight = maskheight*2;
					tile.bForceWrapT = true;
				}
			}
		}
#endif
	}
	else
	{
		//if( clampwidth > linewidth )	clampwidth = linewidth;
		if( clampwidth > 512 && clampheight > 512 )
		{
			if( clampwidth > maskwidth && maskwidth && clampheight > 256 )	clampwidth = maskwidth;
			if( clampheight > maskheight && maskheight && clampheight > 256 )	clampheight = maskheight;
		}

		if( tile.dwMaskS > 8 && tile.dwMaskT > 8 )	
		{
			maskwidth = loadwidth;
			maskheight = loadheight;
		}
		else 
		{
			if( tile.dwMaskS > 10 )
				maskwidth = loadwidth;
			if( tile.dwMaskT > 10 )
				maskheight = loadheight;
		}
	}

	gti.Pitch = tile.dwPitch;

	if( tile.dwMaskS == 0 || tile.bClampS )
	{
		gti.WidthToLoad = linewidth ? min( linewidth, maskwidth ? min(clampwidth,maskwidth) : clampwidth ) : clampwidth;
		if( tile.dwMaskS && clampwidth < maskwidth )
			tile.dwWidth = gti.WidthToCreate = clampwidth;
		else
			tile.dwWidth = gti.WidthToCreate = max(clampwidth,maskwidth);
	}
	else
	{
		gti.WidthToLoad = loadwidth > 2 ? min(loadwidth,maskwidth) : maskwidth;
		if( linewidth ) gti.WidthToLoad = min( linewidth, (int)gti.WidthToLoad );
		tile.dwWidth = gti.WidthToCreate = maskwidth;
	}

	if( tile.dwMaskT == 0 || tile.bClampT )
	{
		gti.HeightToLoad = maskheight ? min(clampheight,maskheight) : clampheight;
		if( tile.dwMaskT && clampheight < maskheight )
			tile.dwHeight = gti.HeightToCreate = clampheight;
		else
			tile.dwHeight = gti.HeightToCreate = max(clampheight,maskheight);
	}
	else
	{
		gti.HeightToLoad = loadheight > 2 ? min(loadheight,maskheight) : maskheight;
		tile.dwHeight = gti.HeightToCreate = maskheight;
	}

	if( options.enableHackForGames == HACK_FOR_MARIO_KART )
	{
		if( gti.WidthToLoad - ((g_TI.dwWidth<<g_TI.dwSize)>>tile.dwSize) == 1 )
		{
			gti.WidthToLoad--;
			if( gti.HeightToLoad%2 )	gti.HeightToLoad--;
		}
	}

	// Double check
	uint32 total64BitWordsToLoad = (gti.HeightToLoad*gti.WidthToLoad)>>(4-tile.dwSize);
	if( total64BitWordsToLoad + tile.dwTMem > 0x200 )
	{
		//DebuggerAppendMsg("Warning: texture loading tmem is over range");
		if( gti.WidthToLoad > gti.HeightToLoad )
		{
			uint32 newheight = (tile.dwPitch << 1 )>> tile.dwSize;
			tile.dwWidth = gti.WidthToLoad = gti.WidthToCreate = min(newheight, (gti.WidthToLoad&0xFFFFFFFE));
			tile.dwHeight = gti.HeightToCreate = gti.HeightToLoad = ((0x200 - tile.dwTMem) << (4-tile.dwSize)) / gti.WidthToLoad;
		}
		else
		{
			tile.dwHeight = gti.HeightToCreate = gti.HeightToLoad = info->dwTotalWords / ((gti.WidthToLoad << tile.dwSize) >> 1);
		}
	}

	// Check the info
	if( (info->dwTotalWords>>2) < total64BitWordsToLoad+tile.dwTMem-info->dwTmem - 4 )
	{
		// Hack here
		if( (options.enableHackForGames == HACK_FOR_ZELDA||options.enableHackForGames == HACK_FOR_ZELDA_MM) && tileno != gRSP.curTile )
		{
			return false;
		}

		if( total64BitWordsToLoad+tile.dwTMem-info->dwTmem <= 0x200 )
		{
		}
	}

	//Check memory boundary
	if( gti.Address + gti.HeightToLoad*gti.Pitch >= g_dwRamSize )
	{
		gti.HeightToCreate = gti.HeightToLoad = tile.dwHeight = (g_dwRamSize-gti.Address)/gti.Pitch;
	}

	return true;
}

TxtrCacheEntry* LoadTexture(uint32 tileno)
{
	TxtrCacheEntry *pEntry = NULL;
	TxtrInfo gti;

	Tile &tile = gRDP.tiles[tileno];

	// Retrieve the tile loading info
	uint32 infoTmemAddr = tile.dwTMem;
	TMEMLoadMapInfo *info = &g_tmemLoadAddrMap[infoTmemAddr];
	if( !IsTmemFlagValid(infoTmemAddr) )
	{
		infoTmemAddr =  GetValidTmemInfoIndex(infoTmemAddr);
		info = &g_tmemLoadAddrMap[infoTmemAddr];
	}

	if( info->dwFormat != tile.dwFormat )
	{
		// Check the tile format, hack for Zelda's road
		if( tileno != gRSP.curTile && tile.dwTMem == gRDP.tiles[gRSP.curTile].dwTMem &&
			tile.dwFormat != gRDP.tiles[gRSP.curTile].dwFormat )
		{
			return NULL;
		}
	}

	gti = tile;	// Copy tile info to textureInfo entry

	gti.TLutFmt = gRDP.otherMode.text_tlut <<RSP_SETOTHERMODE_SHIFT_TEXTLUT;
	if (gti.Format == TXT_FMT_CI && gti.TLutFmt == TLUT_FMT_NONE )
		gti.TLutFmt = TLUT_FMT_RGBA16;		// Force RGBA

	gti.PalAddress = (uint32)(&g_wRDPTlut[0]);
	if( !options.bUseFullTMEM && tile.dwSize == TXT_SIZE_4b )
		gti.PalAddress += 16  * 2 * tile.dwPalette; 

	gti.Address = (info->dwLoadAddress+(tile.dwTMem-infoTmemAddr)*8) & (g_dwRamSize-1) ;
	gti.pPhysicalAddress = ((uint8*)g_pRDRAMu32)+gti.Address;
	gti.tileNo = tileno;

	if( g_curRomInfo.bTxtSizeMethod2 )
	{
		if( !CalculateTileSizes_method_1(tileno, info, gti) )
		//if( !CalculateTileSizes_method_2(tileno, info, gti) )
			return NULL;
	}
	else
	{
		//if( !CalculateTileSizes_method_1(tileno, info, gti) )
		if( !CalculateTileSizes_method_2(tileno, info, gti) )
			return NULL;
	}

#ifdef _XBOX_HACK
	// Hack for XBOX, don't use too big textures
	if( gti.HeightToCreate/gti.HeightToLoad >= 2 && gti.HeightToCreate > 128 )//&& tile.bClampT )//&& tile.dwMaskT )
	{
		tile.bClampT = 0;
		gti.HeightToCreate = gti.HeightToLoad;
		tile.dwHeight = gti.HeightToLoad;
	}

	if( gti.WidthToCreate/gti.WidthToLoad >= 2 && gti.WidthToCreate > 128 )//&& tile.bClampS )//&& tile.dwMaskS )
	{
		tile.bClampS = 0;
		gti.WidthToCreate = gti.WidthToLoad;
		tile.dwWidth = gti.WidthToLoad;
	}
#endif

	// Option for faster loading tiles
#ifdef _VID1964_TILE
	if( g_curRomInfo.bFastLoadTile && status.primitiveType == PRIM_TEXTRECT && 
		info->bSetBy == CMD_LOADTILE && ((gti.Pitch<<1)>>gti.Size) > 128 &&
		((gti.Pitch<<1)>>gti.Size) <= 0x400  )
#else
	if( g_curRomInfo.bFastLoadTile && info->bSetBy == CMD_LOADTILE && ((gti.Pitch<<1)>>gti.Size) <= 0x400
		//&& ((gti.Pitch<<1)>>gti.Size) > 128 && status.primitiveType == PRIM_TEXTRECT
		)
#endif
	{
		uint32 idx = tileno-gRSP.curTile;
		status.LargerTileRealLeft[idx] = gti.LeftToLoad;
		gti.LeftToLoad=0;
		gti.WidthToLoad = gti.WidthToCreate = ((gti.Pitch<<1)>>gti.Size);
		status.UseLargerTile[idx]=true;
	}

	// Loading the textures by using texture cache manager
	return gTextureManager.GetTexture(&gti, true, true, true);	// Load the texture by using texture cache
}

void PrepareTextures()
{
	if( gRDP.textureIsChanged || !currentRomOptions.bFastTexCRC ||
		CRender::g_pRender->m_pColorCombiner->m_pDecodedMux->m_ColorTextureFlag[0] ||
		CRender::g_pRender->m_pColorCombiner->m_pDecodedMux->m_ColorTextureFlag[1] )
	{
		status.UseLargerTile[0]=false;
		status.UseLargerTile[1]=false;

		int tilenos[2];
		if( CRender::g_pRender->IsTexel0Enable() || gRDP.otherMode.cycle_type  == CYCLE_TYPE_COPY )
			tilenos[0] = gRSP.curTile;
		else
			tilenos[0] = -1;

		if( gRSP.curTile<7 && CRender::g_pRender->IsTexel1Enable() )
			tilenos[1] = gRSP.curTile+1;
		else
			tilenos[1] = -1;


		for( int i=0; i<2; i++ )
		{
			if( tilenos[i] < 0 )	continue;

			if( CRender::g_pRender->m_pColorCombiner->m_pDecodedMux->m_ColorTextureFlag[i] )
			{
				TxtrCacheEntry *pEntry = gTextureManager.GetConstantColorTexture(CRender::g_pRender->m_pColorCombiner->m_pDecodedMux->m_ColorTextureFlag[i]);
				CRender::g_pRender->SetCurrentTexture( tilenos[i], pEntry->pTexture, 4, 4, pEntry);
			}
			else
			{
				TxtrCacheEntry *pEntry = LoadTexture(tilenos[i]);
				if (pEntry && pEntry->pTexture )
				{
#ifndef _XBOX
					if( pEntry->txtrBufIdx <= 0 )
					{
						if( pEntry->pEnhancedTexture == NULL )
						{
							MirrorTexture(tilenos[i], pEntry);;
						}

						if( pEntry->pEnhancedTexture == NULL || pEntry->dwEnhancementFlag != options.textureEnhancement )
						{
							EnhanceTexture(pEntry);
						}
					}
#endif

					CRender::g_pRender->SetCurrentTexture( tilenos[i], 
						(pEntry->pEnhancedTexture)?pEntry->pEnhancedTexture:pEntry->pTexture,
						pEntry->ti.WidthToLoad, pEntry->ti.HeightToLoad, pEntry);
				}
				else
				{
					pEntry = gTextureManager.GetBlackTexture();
					CRender::g_pRender->SetCurrentTexture( tilenos[i], pEntry->pTexture, 4, 4, pEntry);
				}

			}
		}

		gRDP.textureIsChanged = false;
	}
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
extern uint32 g_TxtLoadBy;;

void DLParser_LoadTLut(Gfx *gfx)
{
	gRDP.textureIsChanged = true;

	uint32 tileno	= gfx->loadtile.tile;
	uint32 uls		= gfx->loadtile.sl/4;
	uint32 ult		= gfx->loadtile.tl/4;
	uint32 lrs		= gfx->loadtile.sh/4;
	uint32 lrt		= gfx->loadtile.th/4;

	uint32 dwTLutFmt = (gRDP.otherModeH >> RSP_SETOTHERMODE_SHIFT_TEXTLUT)&0x3;

	uint32 dwCount;

	uint32 dwTMEMOffset = gRDP.tiles[tileno].dwTMem - 256;				// starting location in the palettes
	dwCount = ((uint16)((gfx->words.cmd1) >> 14) & 0x03FF) + 1;		// number to copy
	uint32 dwRDRAMOffset = 0;


	Tile &tile = gRDP.tiles[tileno];
	tile.bForceWrapS = tile.bForceWrapT = tile.bForceClampS = tile.bForceClampT = false;

	tile.hilite_sl = tile.sl = uls;
	tile.hilite_tl = tile.tl = ult;
	tile.sh = lrs;
	tile.th = lrt;
	tile.bSizeIsValid = true;

	tile.lastTileCmd = CMD_LOADTLUT;

	dwCount = (lrs - uls)+1;
	dwRDRAMOffset = (uls + ult*g_TI.dwWidth )*2;
	uint32 dwPalAddress = g_TI.dwAddr + dwRDRAMOffset;

	//Copy PAL to the PAL memory
	uint16 *srcPal = (uint16*)(g_pRDRAMu8 + (dwPalAddress& (g_dwRamSize-1)) );
	for (uint32 i=0; i<dwCount && i<0x100; i++)
	{
		g_wRDPTlut[(i+dwTMEMOffset)^1] = srcPal[i^1];
	}

	if( options.bUseFullTMEM )
	{
		for (uint32 i=0; i<dwCount && i+tile.dwTMem<0x200; i++)
		{
			*(uint16*)(&g_Tmem.g_Tmem64bit[tile.dwTMem+i]) = srcPal[i^1];
		}
	}

	extern bool RevTlutTableNeedUpdate;
	RevTlutTableNeedUpdate = true;
	g_TxtLoadBy = CMD_LOADTLUT;
}


void DLParser_LoadBlock(Gfx *gfx)
{
	gRDP.textureIsChanged = true;

	uint32 tileno	= gfx->loadtile.tile;
	uint32 uls		= gfx->loadtile.sl;
	uint32 ult		= gfx->loadtile.tl;
	uint32 lrs		= gfx->loadtile.sh;
	uint32 dxt		= gfx->loadtile.th;					// 1.11 fixed point

	Tile &tile = gRDP.tiles[tileno];
	tile.bForceWrapS = tile.bForceWrapT = tile.bForceClampS = tile.bForceClampT = false;

	uint32 size		= lrs+1;
	if( tile.dwSize == TXT_SIZE_32b )	size<<=1;

	SetTmemFlag(tile.dwTMem, size>>2);

	TMEMLoadMapInfo &info = g_tmemLoadAddrMap[tile.dwTMem];

	info.bSwapped = (dxt == 0? TRUE : FALSE);
#if defined(_RICE560_TILE) || defined(_RICE612_TILE)
	info.sl = tile.hilite_sl = tile.sl = uls;
	info.sh = tile.hilite_sh = tile.sh = lrs;
	info.tl = tile.tl = ult;
	info.th = tile.th = dxt;
	tile.bSizeIsValid = false;
#else // Below used in 1964video
	info.sl = tile.hilite_sl = tile.sl = uls;
	info.sh = tile.hilite_tl = tile.tl = lrs;
	info.tl = tile.sh = ult;
	info.th = tile.th = dxt;
	tile.bSizeIsValid = true;
#endif
	for( int i=0; i<8; i++ )
	{
		if( tile.dwTMem == tile.dwTMem )
			tile.lastTileCmd = CMD_LOADBLOCK;
	}

	info.dwLoadAddress = g_TI.dwAddr;
	info.bSetBy = CMD_LOADBLOCK;
	info.dxt = dxt;
	info.dwLine = tile.dwLine;

	info.dwFormat = g_TI.dwFormat;
	info.dwSize = g_TI.dwSize;
	info.dwWidth = g_TI.dwWidth;
	info.dwTotalWords = size;
	info.dwTmem = tile.dwTMem;

	if( gRDP.tiles[tileno].dwTMem == 0 )
	{
		if( size >= 1024 )
		{
			memcpy(&g_tmemInfo0, &info, sizeof(TMEMLoadMapInfo) );
			g_tmemInfo0.dwTotalWords = size>>2;
		}
		
		if( size == 2048 )
		{
			memcpy(&g_tmemInfo1, &info, sizeof(TMEMLoadMapInfo) );
			g_tmemInfo1.dwTotalWords = size>>2;
		}
	}
	else if( tile.dwTMem == 0x100 )
	{
		if( size == 1024 )
		{
			memcpy(&g_tmemInfo1, &info, sizeof(TMEMLoadMapInfo) );
			g_tmemInfo1.dwTotalWords = size>>2;
		}
	}

	g_TxtLoadBy = CMD_LOADBLOCK;


	if( options.bUseFullTMEM )
	{
		uint32 bytes = (lrs + 1) << tile.dwSize >> 1;
		uint32 address = g_TI.dwAddr + ult * g_TI.bpl + (uls << g_TI.dwSize >> 1);
		if ((bytes == 0) || ((address + bytes) > g_dwRamSize) || (((tile.dwTMem << 3) + bytes) > 4096))
		{
			return;
		}
		uint64* src = (uint64*)(g_pRDRAMu8+address);
		uint64* dest = &g_Tmem.g_Tmem64bit[tile.dwTMem];

		if( dxt > 0)
		{
			void (*Interleave)( void *mem, uint32 numDWords );

			uint32 line = (2047 + dxt) / dxt;
			uint32 bpl = line << 3;
			uint32 height = bytes / bpl;

			if (tile.dwSize == TXT_SIZE_32b)
				Interleave = QWordInterleave;
			else
				Interleave = DWordInterleave;

			for (uint32 y = 0; y < height; y++)
			{
				UnswapCopy( src, dest, bpl );
				if (y & 1) Interleave( dest, line );

				src += line;
				dest += line;
			}
		}
		else
			UnswapCopy( src, dest, bytes );
	}


}

void swap(int &a, int &b)
{
	int temp = a;
	a = b;
	b = temp;
}
void DLParser_LoadTile(Gfx *gfx)
{
	gRDP.textureIsChanged = true;

	uint32 tileno	= gfx->loadtile.tile;
	uint32 uls		= gfx->loadtile.sl/4;
	uint32 ult		= gfx->loadtile.tl/4;
	uint32 lrs		= gfx->loadtile.sh/4;
	uint32 lrt		= gfx->loadtile.th/4;

	Tile &tile = gRDP.tiles[tileno];
	tile.bForceWrapS = tile.bForceWrapT = tile.bForceClampS = tile.bForceClampT = false;

	if( lrt < ult )
	{
		uint32 temp = ult;
		ult = lrt;
		lrt = temp;
	}

	tile.hilite_sl = tile.sl = uls;
	tile.hilite_tl = tile.tl = ult;
	tile.hilite_sh = tile.sh = lrs;
	tile.hilite_th = tile.th = lrt;
	tile.bSizeIsValid = true;

	if( options.bUseFullTMEM )
	{
		void (*Interleave)( void *mem, uint32 numDWords );
		uint32 address, height, bpl, line, y;
		uint64 *dest;
		uint8 *src;

		if( g_TI.bpl == 0 )
		{
			if( options.enableHackForGames == HACK_FOR_BUST_A_MOVE )
			{
				g_TI.bpl = 1024;		// Hack for Bust-A-Move
			}
			else
			{
			}
		}

		address = g_TI.dwAddr + tile.tl * g_TI.bpl + (tile.sl << g_TI.dwSize >> 1);
		dest = &g_Tmem.g_Tmem64bit[tile.dwTMem];
		if( tile.sh < tile.sl )	swap(tile.sh, tile.sl);
		bpl = (tile.sh - tile.sl + 1) << tile.dwSize >> 1;
		height = tile.th - tile.tl + 1;
		src = &g_pRDRAMu8[address];

		if (((address + height * bpl) > g_dwRamSize) || (((tile.dwTMem << 3) + bpl * height) > 4096)) // Stay within TMEM
		{
			return;
		}

		// Line given for 32-bit is half what it seems it should since they split the
		// high and low words. I'm cheating by putting them together.
		if (tile.dwSize == TXT_SIZE_32b)
		{
			line = tile.dwLine << 1;
			Interleave = QWordInterleave;
		}
		else
		{
			line = tile.dwLine;
			Interleave = DWordInterleave;
		}

		if( tile.dwLine == 0 )
		{
			//tile.dwLine = 1;
			return;
		}

		for (y = 0; y < height; y++)
		{
			UnswapCopy( src, dest, bpl );
			if (y & 1) Interleave( dest, line );

			src += g_TI.bpl;
			dest += line;
		}
	}


	for( int i=0; i<8; i++ )
	{
		if( gRDP.tiles[i].dwTMem == tile.dwTMem )
			gRDP.tiles[i].lastTileCmd = CMD_LOADTILE;
	}

	uint32 size = tile.dwLine*(lrt-ult+1);
	if( tile.dwSize == TXT_SIZE_32b )	size<<=1;
	SetTmemFlag(tile.dwTMem,size );

	uint32 dwPitch = g_TI.dwWidth;
	if( tile.dwSize != TXT_SIZE_4b )
	{
		dwPitch = g_TI.dwWidth<<(tile.dwSize-1);
	}

	TMEMLoadMapInfo &info = g_tmemLoadAddrMap[tile.dwTMem];

	info.dwLoadAddress = g_TI.dwAddr;
	info.dwFormat = g_TI.dwFormat;
	info.dwSize = g_TI.dwSize;
	info.dwWidth = g_TI.dwWidth;

	info.sl = uls;
	info.sh = lrs;
	info.tl = ult;
	info.th = lrt;
	
	info.dxt = 0;
	info.dwLine = tile.dwLine;
	info.dwTmem = tile.dwTMem;
	info.dwTotalWords = size<<2;

	info.bSetBy = CMD_LOADTILE;
	info.bSwapped =FALSE;

	g_TxtLoadBy = CMD_LOADTILE;

	if( tile.dwTMem == 0 )
	{
		if( size >= 256 )
		{
			memcpy(&g_tmemInfo0, &info, sizeof(TMEMLoadMapInfo) );
			g_tmemInfo0.dwTotalWords = size;
		}

		if( size == 512 )
		{
			memcpy(&g_tmemInfo1, &info, sizeof(TMEMLoadMapInfo) );
			g_tmemInfo1.dwTotalWords = size;
		}
	}
	else if( tile.dwTMem == 0x100 )
	{
		if( size == 256 )
		{
			memcpy(&g_tmemInfo1, &info, sizeof(TMEMLoadMapInfo) );
			g_tmemInfo1.dwTotalWords = size;
		}
	}
}


static char *pszOnOff[2]     = {"Off", "On"};
uint32 lastSetTile;
void DLParser_SetTile(Gfx *gfx)
{
	gRDP.textureIsChanged = true;

	uint32 tileno		= gfx->settile.tile;
	Tile &tile = gRDP.tiles[tileno];
	tile.bForceWrapS = tile.bForceWrapT = tile.bForceClampS = tile.bForceClampT = false;

	lastSetTile = tileno;

	tile.dwFormat	= gfx->settile.fmt;
	tile.dwSize		= gfx->settile.siz;
	tile.dwLine		= gfx->settile.line;
	tile.dwTMem		= gfx->settile.tmem;

	tile.dwPalette	= gfx->settile.palette;
	tile.bClampT	= gfx->settile.ct;
	tile.bMirrorT	= gfx->settile.mt;
	tile.dwMaskT	= gfx->settile.maskt;
	tile.dwShiftT	= gfx->settile.shiftt;
	tile.bClampS	= gfx->settile.cs;
	tile.bMirrorS	= gfx->settile.ms;
	tile.dwMaskS	= gfx->settile.masks;
	tile.dwShiftS	= gfx->settile.shifts;

	tile.fShiftScaleS = 1.0f;
	if( tile.dwShiftS )
	{
		if( tile.dwShiftS > 10 )
		{
			tile.fShiftScaleS = (float)(1 << (16 - tile.dwShiftS));
		}
		else
		{
			tile.fShiftScaleS = (float)1.0f/(1 << tile.dwShiftS);
		}
	}

	tile.fShiftScaleT = 1.0f;
	if( tile.dwShiftT )
	{
		if( tile.dwShiftT > 10 )
		{
			tile.fShiftScaleT = (float)(1 << (16 - tile.dwShiftT));
		}
		else
		{
			tile.fShiftScaleT = (float)1.0f/(1 << tile.dwShiftT);
		}
	}

	// Hack for DK64
	/*
	if( tile.dwMaskS > 0 && tile.dwMaskT > 0 && tile.dwMaskS < 8 && tile.dwMaskT < 8 )
	{
		tile.sh = tile.sl + (1<<tile.dwMaskS);
		tile.th = tile.tl + (1<<tile.dwMaskT);
		tile.hilite_sl = tile.sl;
		tile.hilite_tl = tile.tl;
	}
	*/

	tile.lastTileCmd = CMD_SETTILE;
}

void DLParser_SetTileSize(Gfx *gfx)
{
	gRDP.textureIsChanged = true;

	uint32 tileno	= gfx->loadtile.tile;
	int sl		= gfx->loadtile.sl;
	int tl		= gfx->loadtile.tl;
	int sh		= gfx->loadtile.sh;
	int th		= gfx->loadtile.th;

	Tile &tile = gRDP.tiles[tileno];
	tile.bForceWrapS = tile.bForceWrapT = tile.bForceClampS = tile.bForceClampT = false;

	if( options.bUseFullTMEM )
	{
		tile.bSizeIsValid = true;
		tile.hilite_sl = tile.sl = sl / 4;
		tile.hilite_tl = tile.tl = tl / 4;
		tile.hilite_sh = tile.sh = sh / 4;
		tile.hilite_th = tile.th = th / 4;

		tile.fhilite_sl = tile.fsl = sl / 4.0f;
		tile.fhilite_tl = tile.ftl = tl / 4.0f;
		tile.fhilite_sh = tile.fsh = sh / 4.0f;
		tile.fhilite_th = tile.fth = th / 4.0f;

		tile.lastTileCmd = CMD_SETTILE_SIZE;
	}
	else
	{
		if( tile.lastTileCmd != CMD_SETTILE_SIZE )
		{
			tile.bSizeIsValid = true;
			if( sl/4 > sh/4 || tl/4 > th/4 || (sh == 0 && tile.dwShiftS==0 && th == 0 && tile.dwShiftT ==0 ) )
			{
				tile.bSizeIsValid = false;
			}
			tile.hilite_sl = tile.sl = sl / 4;
			tile.hilite_tl = tile.tl = tl / 4;
			tile.hilite_sh = tile.sh = sh / 4;
			tile.hilite_th = tile.th = th / 4;

			tile.fhilite_sl = tile.fsl = sl / 4.0f;
			tile.fhilite_tl = tile.ftl = tl / 4.0f;
			tile.fhilite_sh = tile.fsh = sh / 4.0f;
			tile.fhilite_th = tile.fth = th / 4.0f;

			tile.lastTileCmd = CMD_SETTILE_SIZE;
		}
		else
		{
			tile.fhilite_sh = tile.fsh;
			tile.fhilite_th = tile.fth;
			tile.fhilite_sl = tile.fsl = (sl>0x7ff ? (sl-0xfff) : sl)/4.0f;
			tile.fhilite_tl = tile.ftl = (tl>0x7ff ? (tl-0xfff) : tl)/4.0f;

			tile.hilite_sl = sl>0x7ff ? (sl-0xfff) : sl;
			tile.hilite_tl = tl>0x7ff ? (tl-0xfff) : tl;
			tile.hilite_sl /= 4;
			tile.hilite_tl /= 4;
			tile.hilite_sh = sh/4;
			tile.hilite_th = th/4;

			tile.lastTileCmd = CMD_SETTILE_SIZE;
		}
	}
}

extern char *pszImgFormat[8];// = {"RGBA", "YUV", "CI", "IA", "I", "?1", "?2", "?3"};
extern char *pszImgSize[4];// = {"4", "8", "16", "32"};
void DLParser_SetTImg(Gfx *gfx)
{
	gRDP.textureIsChanged = true;

	g_TI.dwFormat 	= gfx->img.fmt;
	g_TI.dwSize   	= gfx->img.siz;
	g_TI.dwWidth  	= gfx->img.width + 1;
	g_TI.dwAddr   	= RSPSegmentAddr((gfx->img.addr));
	g_TI.bpl		= g_TI.dwWidth << g_TI.dwSize >> 1;

}

void DLParser_TexRect(Gfx *gfx)
{
	Gtexrect *gtextrect = (Gtexrect *)gfx;

#ifdef _DISABLE_VID1964
	status.bCIBufferIsRendered = true;
#elif defined(_RICE6FB)
	if( !status.bCIBufferIsRendered ) g_pFrameBufferManager->ActiveTextureBuffer();
#else
	if( !status.bCIBufferIsRendered ) CGraphicsContext::g_pGraphicsContext->FirstDrawToNewCI();
#endif

	status.primitiveType = PRIM_TEXTRECT;

	// This command used 128bits, and not 64 bits. This means that we have to look one 
	// Command ahead in the buffer, and update the PC.
	uint32 dwPC = gDlistStack[gDlistStackPointer].pc;		// This points to the next instruction
	uint32 dwCmd2 = *(uint32 *)(g_pRDRAMu8 + dwPC+4);
	uint32 dwCmd3 = *(uint32 *)(g_pRDRAMu8 + dwPC+4+8);
	uint32 dwHalf1 = *(uint32 *)(g_pRDRAMu8 + dwPC);
	uint32 dwHalf2 = *(uint32 *)(g_pRDRAMu8 + dwPC+8);

	if( options.enableHackForGames == HACK_FOR_ALL_STAR_BASEBALL || options.enableHackForGames == HACK_FOR_MLB )
	{
		if( ((dwHalf1>>24) == 0xb4 || (dwHalf1>>24) == 0xb3 || (dwHalf1>>24) == 0xb2 || (dwHalf1>>24) == 0xe1) && 
			((dwHalf2>>24) == 0xb4 || (dwHalf2>>24) == 0xb3 || (dwHalf2>>24) == 0xb2 || (dwHalf2>>24) == 0xf1) )
		{
			// Increment PC so that it points to the right place
			gDlistStack[gDlistStackPointer].pc += 16;
		}
		else
		{
			// Hack for some games, All_Star_Baseball_2000
			gDlistStack[gDlistStackPointer].pc += 8;
			dwCmd3 = dwCmd2;
			//dwCmd2 = 0;

			// fix me here
			dwCmd2 = (((dwHalf1>>12)&0x03FF)<<17) | (((dwHalf1)&0x03FF)<<1);
		}	
	}
	else
	{
		gDlistStack[gDlistStackPointer].pc += 16;
	}


	// Hack for Mario Tennis
	if( !status.bHandleN64TextureBuffer && g_CI.dwAddr == g_ZI.dwAddr )
	{
		return;
	}


	uint32 dwXH		= (((gfx->words.cmd0)>>12)&0x0FFF)/4;
	uint32 dwYH		= (((gfx->words.cmd0)    )&0x0FFF)/4;
	uint32 tileno	= ((gfx->words.cmd1)>>24)&0x07;
	uint32 dwXL		= (((gfx->words.cmd1)>>12)&0x0FFF)/4;
	uint32 dwYL		= (((gfx->words.cmd1)    )&0x0FFF)/4;
	uint16 uS		= (uint16)(  dwCmd2>>16)&0xFFFF;
	uint16 uT		= (uint16)(  dwCmd2    )&0xFFFF;
	uint16  uDSDX 	= (uint16)((  dwCmd3>>16)&0xFFFF);
	uint16  uDTDY	    = (uint16)((  dwCmd3    )&0xFFFF);
	

	if( (int)dwXL >= gRDP.scissor.right || (int)dwYL >= gRDP.scissor.bottom || (int)dwXH < gRDP.scissor.left || (int)dwYH < gRDP.scissor.top )
	{
		// Clipping
		return;
	}

	short s16S = *(short*)(&uS);
	short s16T = *(short*)(&uT);

	short	 s16DSDX  = *(short*)(&uDSDX);
	short  s16DTDY	= *(short*)(&uDTDY);

	uint32 curTile = gRSP.curTile;
	ForceMainTextureIndex(tileno);

	float fS0 = s16S / 32.0f;
	float fT0 = s16T / 32.0f;

	if(options.enableHackTextureSeams)
	{
		if(s16DSDX<0) fS0 += 1.0f;	//Fix texture seams (California Speed)
		if(s16DTDY<0) fT0 += 1.0f;	//Fix texture seams (California Speed)
	}

	float fDSDX = s16DSDX / 1024.0f;
	float fDTDY = s16DTDY / 1024.0f;

	uint32 cycletype = gRDP.otherMode.cycle_type;

	if (cycletype == CYCLE_TYPE_COPY)
	{
		fDSDX /= 4.0f;	// In copy mode 4 pixels are copied at once.
		dwXH++;
		dwYH++;
	}
	else if (cycletype == CYCLE_TYPE_FILL)
	{
		dwXH++;
		dwYH++;
	}

	if( fDSDX == 0 )	fDSDX = 1;
	if( fDTDY == 0 )	fDTDY = 1;

	float fS1 = fS0 + (fDSDX * (dwXH - dwXL));
	float fT1 = fT0 + (fDTDY * (dwYH - dwYL));

	float t0u0 = (fS0-gRDP.tiles[tileno].hilite_sl) * gRDP.tiles[tileno].fShiftScaleS;
	float t0v0 = (fT0-gRDP.tiles[tileno].hilite_tl) * gRDP.tiles[tileno].fShiftScaleT;
	float t0u1 = t0u0 + (fDSDX * (dwXH - dwXL))*gRDP.tiles[tileno].fShiftScaleS;
	float t0v1 = t0v0 + (fDTDY * (dwYH - dwYL))*gRDP.tiles[tileno].fShiftScaleT;

	if( dwXL==0 && dwYL==0 && dwXH==windowSetting.fViWidth-1 && dwYH==windowSetting.fViHeight-1 &&
		t0u0 == 0 && t0v0==0 && t0u1==0 && t0v1==0 )
	{
		//Using TextRect to clear the screen
	}
	else
	{
		if( status.bHandleN64TextureBuffer && //status.bDirectWriteIntoRDRAM && 
			g_pTxtBufferInfo->CI_Info.dwFormat == gRDP.tiles[tileno].dwFormat && 
			g_pTxtBufferInfo->CI_Info.dwSize == gRDP.tiles[tileno].dwSize && 
			gRDP.tiles[tileno].dwFormat == TXT_FMT_CI && gRDP.tiles[tileno].dwSize == TXT_SIZE_8b )
		{
			if( options.enableHackForGames == HACK_FOR_YOSHI )
			{
				// Hack for Yoshi background image
				PrepareTextures();
				TexRectToFrameBuffer_8b(dwXL, dwYL, dwXH, dwYH, t0u0, t0v0, t0u1, t0v1, tileno);
			}
			else
			{
				if( frameBufferOptions.bUpdateCIInfo )
				{
					PrepareTextures();
					TexRectToFrameBuffer_8b(dwXL, dwYL, dwXH, dwYH, t0u0, t0v0, t0u1, t0v1, tileno);
				}

				if( !status.bDirectWriteIntoRDRAM )
				{
					CRender::g_pRender->TexRect(dwXL, dwYL, dwXH, dwYH, fS0, fT0, fDSDX, fDTDY);

					status.dwNumTrisRendered += 2;
				}
			}
		}
		else
		{
			CRender::g_pRender->TexRect(dwXL, dwYL, dwXH, dwYH, fS0, fT0, fDSDX, fDTDY);
			status.bFrameBufferDrawnByTriangles = true;

			status.dwNumTrisRendered += 2;
		}
	}

	if( status.bHandleN64TextureBuffer )	g_pTxtBufferInfo->maxUsedHeight = max(g_pTxtBufferInfo->maxUsedHeight,(int)dwYH);

	ForceMainTextureIndex(curTile);
}


void DLParser_TexRectFlip(Gfx *gfx)
{ 
	status.bCIBufferIsRendered = true;
	status.primitiveType = PRIM_TEXTRECTFLIP;

	// This command used 128bits, and not 64 bits. This means that we have to look one 
	// Command ahead in the buffer, and update the PC.
	uint32 dwPC = gDlistStack[gDlistStackPointer].pc;		// This points to the next instruction
	uint32 dwCmd2 = *(uint32 *)(g_pRDRAMu8 + dwPC+4);
	uint32 dwCmd3 = *(uint32 *)(g_pRDRAMu8 + dwPC+4+8);

	// Increment PC so that it points to the right place
	gDlistStack[gDlistStackPointer].pc += 16;

	uint32 dwXH		= (((gfx->words.cmd0)>>12)&0x0FFF)/4;
	uint32 dwYH		= (((gfx->words.cmd0)    )&0x0FFF)/4;
	uint32 tileno	= ((gfx->words.cmd1)>>24)&0x07;
	uint32 dwXL		= (((gfx->words.cmd1)>>12)&0x0FFF)/4;
	uint32 dwYL		= (((gfx->words.cmd1)    )&0x0FFF)/4;
	uint32 dwS		= (  dwCmd2>>16)&0xFFFF;
	uint32 dwT		= (  dwCmd2    )&0xFFFF;
	LONG  nDSDX 	= (LONG)(short)((  dwCmd3>>16)&0xFFFF);
	LONG  nDTDY	    = (LONG)(short)((  dwCmd3    )&0xFFFF);

	uint32 curTile = gRSP.curTile;
	ForceMainTextureIndex(tileno);
	
	float fS0 = (float)dwS / 32.0f;
	float fT0 = (float)dwT / 32.0f;

	if(options.enableHackTextureSeams)
	{
		if(nDSDX<0) fS0 += 1.0f;	//Fix texture seams (California Speed)
		if(nDTDY<0) fT0 += 1.0f;	//Fix texture seams (California Speed)
	}

	float fDSDX = (float)nDSDX / 1024.0f;
	float fDTDY = (float)nDTDY / 1024.0f;

	uint32 cycletype = gRDP.otherMode.cycle_type;

	if (cycletype == CYCLE_TYPE_COPY)
	{
		fDSDX /= 4.0f;	// In copy mode 4 pixels are copied at once.
		dwXH++;
		dwYH++;
	}
	else if (cycletype == CYCLE_TYPE_FILL)
	{
		dwXH++;
		dwYH++;
	}

	float fS1 = fS0 + (fDSDX * (dwYH - dwYL));
	float fT1 = fT0 + (fDTDY * (dwXH - dwXL));
	
	float t0u0 = (fS0) * gRDP.tiles[tileno].fShiftScaleS-gRDP.tiles[tileno].sl;
	float t0v0 = (fT0) * gRDP.tiles[tileno].fShiftScaleT-gRDP.tiles[tileno].tl;
	float t0u1 = t0u0 + (fDSDX * (dwYH - dwYL))*gRDP.tiles[tileno].fShiftScaleS;
	float t0v1 = t0v0 + (fDTDY * (dwXH - dwXL))*gRDP.tiles[tileno].fShiftScaleT;

	CRender::g_pRender->TexRectFlip(dwXL, dwYL, dwXH, dwYH, t0u0, t0v0, t0u1, t0v1);
	status.dwNumTrisRendered += 2;

	if( status.bHandleN64TextureBuffer )	g_pTxtBufferInfo->maxUsedHeight = max(g_pTxtBufferInfo->maxUsedHeight,int(dwYL+(dwXH-dwXL)));

	ForceMainTextureIndex(curTile);
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
/*
 *	TMEM emulation
 *  There are 0x200's 64bits entry in TMEM
 *  Usually, textures are loaded into TMEM at 0x0, and TLUT is loaded at 0x100
 *  of course, the whole TMEM can be used by textures if TLUT is not used, and TLUT
 *  can be at other address of TMEM.
 *
 *  We don't want to emulate TMEM by creating a block of memory for TMEM and load
 *  everything into the block of memory, this will be slow.
 */
typedef struct TmemInfoEntry{
	uint32 start;
	uint32 length;
	uint32 rdramAddr;
	TmemInfoEntry* next;
} TmemInfoEntry;

const int tmenMaxEntry=20;
TmemInfoEntry tmenEntryBuffer[20]={0};
TmemInfoEntry *g_pTMEMInfo=NULL;
TmemInfoEntry *g_pTMEMFreeList=tmenEntryBuffer;

void TMEM_Init()
{
	int i=0;
	
	g_pTMEMInfo=NULL;
	g_pTMEMFreeList=tmenEntryBuffer;
	for( i=0; i<tmenMaxEntry; i++ )
	{
		tmenEntryBuffer[i].start=0;
		tmenEntryBuffer[i].length=0;
		tmenEntryBuffer[i].rdramAddr=0;
		tmenEntryBuffer[i].next = &(tmenEntryBuffer[i+1]);
	}
	tmenEntryBuffer[i].next = NULL;
}

void TMEM_SetBlock(uint32 tmemstart, uint32 length, uint32 rdramaddr)
{
	TmemInfoEntry *p=g_pTMEMInfo;

	if( p == NULL )
	{
		// Move an entry from freelist and link it to the header
		p = g_pTMEMFreeList;
		g_pTMEMFreeList = g_pTMEMFreeList->next;

		p->start = tmemstart;
		p->length = length;
		p->rdramAddr = rdramaddr;
		p->next = NULL;
	}
	else
	{
		while ( tmemstart > (p->start+p->length) )
		{
			if( p->next != NULL ) {
				p = p->next;
				continue;
			}
			else {
				break;
			}
		}

		if ( p->start == tmemstart ) 
		{
			// need to replace the block of 'p'
			// or append a new block depend the block lengths
			if( length == p->length )
			{
				p->rdramAddr = rdramaddr;
				return;
			}
			else if( length < p->length )
			{
				TmemInfoEntry *newentry = g_pTMEMFreeList;
				g_pTMEMFreeList = g_pTMEMFreeList->next;

				newentry->length = p->length - length;
				newentry->next = p->next;
				newentry->rdramAddr = p->rdramAddr + p->length;
				newentry->start = p->start + p->length;

				p->length = length;
				p->next = newentry;
				p->rdramAddr = rdramaddr;
			}
		}
		else if( p->start > tmemstart )
		{
			// p->start > tmemstart, need to insert the new block before 'p'
			TmemInfoEntry *newentry = g_pTMEMFreeList;
			g_pTMEMFreeList = g_pTMEMFreeList->next;

			if( length+tmemstart < p->start+p->length )
			{
				newentry->length = p->length - length;
				newentry->next = p->next;
				newentry->rdramAddr = p->rdramAddr + p->length;
				newentry->start = p->start + p->length;

				p->length = length;
				p->next = newentry;
				p->rdramAddr = rdramaddr;
				p->start = tmemstart;
			}
			else if( length+tmemstart == p->start+p->length )
			{

			}
		}
		else
		{
		}
	}
}

uint32 TMEM_GetRdramAddr(uint32 tmemstart, uint32 length)
{
	return 0;
}


/*
 *  New implementation of texture loading
 */

bool IsTmemFlagValid(uint32 tmemAddr)
{
	uint32 index = tmemAddr>>5;
	uint32 bitIndex = (tmemAddr&0x1F);
	return ((g_TmemFlag[index] & (1<<bitIndex))!=0);
}

uint32 GetValidTmemInfoIndex(uint32 tmemAddr)
{
	return 0;
	uint32 index = tmemAddr>>5;
	uint32 bitIndex = (tmemAddr&0x1F);

	if ((g_TmemFlag[index] & (1<<bitIndex))!=0 )	//This address is valid
		return tmemAddr;
	else
	{
		for( uint32 i=index; i>=0; i-- )
		{
			if( g_TmemFlag[i] != 0 )
			{
				for( uint32 j=0x1F; j>=0; j-- )
				{
					if( (g_TmemFlag[i] & (1<<j)) != 0 )
					{
						return ((i<<5)+j);
					}
				}
			}
		}
		return 0;
	}
}


void SetTmemFlag(uint32 tmemAddr, uint32 size)
{
	uint32 index = tmemAddr>>5;
	uint32 bitIndex = (tmemAddr&0x1F);

	if( bitIndex == 0 )
	{
		uint32 i;
		for( i=0; i< (size>>5); i++ )
		{
			g_TmemFlag[index+i] = 0;
		}

		if( (size&0x1F) != 0 )
		{
			//ErrorMsg("Check me: tmemaddr=%X, size=%x", tmemAddr, size);
			g_TmemFlag[index+i] &= ~((1<<(size&0x1F))-1);
		}

		g_TmemFlag[index] |= 1;
	}
	else
	{
		if( bitIndex + size <= 0x1F )
		{
			uint32 val = g_TmemFlag[index];
			uint32 mask = (1<<(bitIndex))-1;
			mask |= ~((1<<(bitIndex + size))-1);
			val &= mask;
			val |= (1<<bitIndex);
			g_TmemFlag[index] = val;
		}
		else
		{
			//ErrorMsg("Check me: tmemaddr=%X, size=%x", tmemAddr, size);
			uint32 val = g_TmemFlag[index];
			uint32 mask = (1<<bitIndex)-1;
			val &= mask;
			val |= (1<<bitIndex);
			g_TmemFlag[index] = val;
			index++;
			size -= (0x20-bitIndex);

			uint32 i;
			for( i=0; i< (size>>5); i++ )
			{
				g_TmemFlag[index+i] = 0;
			}

			if( (size&0x1F) != 0 )
			{
				//ErrorMsg("Check me: tmemaddr=%X, size=%x", tmemAddr, size);
				g_TmemFlag[index+i] &= ~((1<<(size&0x1F))-1);
			}
		}
	}
}

