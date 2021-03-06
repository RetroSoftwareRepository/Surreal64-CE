/*
Copyright (C) 2005 Rice1964

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
#ifdef _RICE6FB
// ===========================================================================

#include "stdafx.h"
#include "UcodeDefs.h"
#include "RSP_Parser.h"

extern TMEMLoadMapInfo g_tmemLoadAddrMap[0x200];	// Totally 4KB TMEM;

// 0 keeps the most recent CI info
// 1 keeps the frame buffer CI info which is being displayed now
// 2 keeps the older frame buffer CI info. This can be used if we are using triple buffer


RecentCIInfo g_RecentCIInfo[5];
RecentCIInfo *g_uRecentCIInfoPtrs[5] =
{
	&g_RecentCIInfo[0],
	&g_RecentCIInfo[1],
	&g_RecentCIInfo[2],
	&g_RecentCIInfo[3],
	&g_RecentCIInfo[4],
};

int numOfRecentCIInfos = 5;

RecentViOriginInfo g_RecentVIOriginInfo[5];
uint32 dwBackBufferSavedAtFrame=0;

TextureBufferInfo gTextureBufferInfos[20];
int numOfTxtBufInfos = sizeof(gTextureBufferInfos)/sizeof(TextureBufferInfo);
TextureBufferInfo *g_pTxtBufferInfo = NULL;

FrameBufferManager* g_pFrameBufferManager = NULL;

bool LastCIIsNewCI=false;

FrameBufferManager::FrameBufferManager() :
	m_isRenderingToTexture(false),
	m_lastTextureBufferIndex(-1),
	m_curRenderTextureIndex(-1)
{
}

FrameBufferManager::~FrameBufferManager()
{
}

void FrameBufferManager::CloseUp()
{
	for( int i=0; i<numOfTxtBufInfos; i++ )
	{
		SAFE_DELETE(gTextureBufferInfos[i].pTxtBuffer);
	}
}

void FrameBufferManager::Initialize()
{
	m_isRenderingToTexture = false;
	m_lastTextureBufferIndex = -1;
	m_curRenderTextureIndex = -1;
	
	status.bCIBufferIsRendered = false;
	status.bN64IsDrawingTextureBuffer = false;
	status.bHandleN64TextureBuffer = false;
	status.bN64FrameBufferIsUsed = false;

	memset(&gTextureBufferInfos[0], 0, sizeof(TextureBufferInfo)*numOfTxtBufInfos);
}
// ===========================================================================

uint16 ConvertRGBATo555(uint8 r, uint8 g, uint8 b, uint8 a)
{
	uint8 ar = a>=0x20?1:0;
	return ((r>>3)<<RGBA5551_RedShift) | ((g>>3)<<RGBA5551_GreenShift) | ((b>>3)<<RGBA5551_BlueShift) | ar;//(a>>7);
}

uint16 ConvertRGBATo555(uint32 color32)
{
	return uint16((((color32>>19)&0x1F)<<RGBA5551_RedShift) | (((color32>>11)&0x1F)<<RGBA5551_GreenShift) | (((color32>>3)&0x1F)<<RGBA5551_BlueShift) | ((color32>>31)));;
}

void FrameBufferManager::UpdateRecentCIAddr(SetImgInfo &ciinfo)
{
	if( ciinfo.dwAddr == g_uRecentCIInfoPtrs[0]->dwAddr )
		return;

	RecentCIInfo *temp;

	int i;

	for( i=1; i<numOfRecentCIInfos; i++ )
	{
		if( ciinfo.dwAddr == g_uRecentCIInfoPtrs[i]->dwAddr )
		{
			temp = g_uRecentCIInfoPtrs[i];

			for( int j=i; j>0; j-- )
			{
				g_uRecentCIInfoPtrs[j] = g_uRecentCIInfoPtrs[j-1];
			}
			break;
		}
	}

	if( i >= numOfRecentCIInfos )
	{
		temp = g_uRecentCIInfoPtrs[4];
		g_uRecentCIInfoPtrs[4] = g_uRecentCIInfoPtrs[3];
		g_uRecentCIInfoPtrs[3] = g_uRecentCIInfoPtrs[2];
		g_uRecentCIInfoPtrs[2] = g_uRecentCIInfoPtrs[1];
		g_uRecentCIInfoPtrs[1] = g_uRecentCIInfoPtrs[0];
		temp->dwCopiedAtFrame = 0;
		temp->bCopied = false;
	}

	g_uRecentCIInfoPtrs[0] = temp;

	// Fix me here for Mario Tennis
	temp->dwLastWidth = windowSetting.uViWidth;
	temp->dwLastHeight = windowSetting.uViHeight;

	temp->dwFormat = ciinfo.dwFormat;
	temp->dwAddr = ciinfo.dwAddr;
	temp->dwSize = ciinfo.dwSize;
	temp->dwWidth = ciinfo.dwWidth;
	temp->dwHeight = gRDP.scissor.bottom;
	temp->dwMemSize = (temp->dwWidth*temp->dwHeight/2)<<temp->dwSize;
	temp->bCopied = false;
	temp->lastUsedFrame = status.gDlistCount;
	temp->lastSetAtUcode = status.gUcodeCount;
}


/************************************************************************/
/* Mark the ciinfo entry that the ciinfo is used by VI origin register  */
/* in another word, this is a real frame buffer, not a fake frame buffer*/
/* Fake frame buffers are never really used by VI origin				*/
/************************************************************************/
void FrameBufferManager::SetAddrBeDisplayed(uint32 addr)
{
	uint32 viwidth = *g_GraphicsInfo.VI_WIDTH_REG;
	addr &= (g_dwRamSize-1);

	int i;

	for( i=0; i<numOfRecentCIInfos; i++ )
	{
		if( g_uRecentCIInfoPtrs[i]->dwAddr+2*viwidth == addr )
		{
			g_uRecentCIInfoPtrs[i]->bUsedByVIAtFrame = status.gDlistCount;
		}
		else if( addr >= g_uRecentCIInfoPtrs[i]->dwAddr && addr < g_uRecentCIInfoPtrs[i]->dwAddr+0x1000 )
		{
			g_uRecentCIInfoPtrs[i]->bUsedByVIAtFrame = status.gDlistCount;
		}
	}

	for( i=0; i<numOfRecentCIInfos; i++ )
	{
		if( g_RecentVIOriginInfo[i].addr == addr )
		{
			g_RecentVIOriginInfo[i].FrameCount = status.gDlistCount;
			return;
		}
	}

	for( i=0; i<numOfRecentCIInfos; i++ )
	{
		if( g_RecentVIOriginInfo[i].addr == 0 )
		{
			// Never used
			g_RecentVIOriginInfo[i].addr = addr;
			g_RecentVIOriginInfo[i].FrameCount = status.gDlistCount;
			return;
		}
	}

	int index=0;
	uint32 minFrameCount = 0xffffffff;

	for( i=0; i<numOfRecentCIInfos; i++ )
	{
		if( g_RecentVIOriginInfo[i].FrameCount < minFrameCount )
		{
			index = i;
			minFrameCount = g_RecentVIOriginInfo[i].FrameCount;
		}
	}

	g_RecentVIOriginInfo[index].addr = addr;
	g_RecentVIOriginInfo[index].FrameCount = status.gDlistCount;
}

bool FrameBufferManager::HasAddrBeenDisplayed(uint32 addr, uint32 width)
{
	addr &= (g_dwRamSize-1);

	int i;

	for( i=0; i<numOfRecentCIInfos; i++ )
	{
		if( g_uRecentCIInfoPtrs[i]->dwAddr == 0 )
			continue;

		if( g_uRecentCIInfoPtrs[i]->dwAddr == addr )
		{
			if( status.gDlistCount-g_uRecentCIInfoPtrs[i]->bUsedByVIAtFrame < 20 )
				//if( g_uRecentCIInfoPtrs[i]->bUsedByVIAtFrame != 0 )
			{
				return true;
			}
			else
			{
				return false;
			}
		}
	}

	for( i=0; i<numOfRecentCIInfos; i++ )
	{
		if( g_RecentVIOriginInfo[i].addr != 0 )
		{
			if( g_RecentVIOriginInfo[i].addr > addr && 
				(g_RecentVIOriginInfo[i].addr - addr)%width == 0 &&
				(g_RecentVIOriginInfo[i].addr - addr)/width <= 4)
			{
				if( status.gDlistCount-g_RecentVIOriginInfo[i].FrameCount < 20 )
					//if( g_RecentVIOriginInfo[i].FrameCount != 0 )
				{
					return true;
				}
				else
				{
					return false;
				}
			}
		}
	}

	if( status.gDlistCount > 20 )
		return false;
	else
	{
		return true;
	}
}

int FrameBufferManager::FindRecentCIInfoIndex(uint32 addr)
{
	for( int i=0; i<numOfRecentCIInfos; i++ )
	{
		if( g_uRecentCIInfoPtrs[i]->dwAddr <= addr && addr < g_uRecentCIInfoPtrs[i]->dwAddr+g_uRecentCIInfoPtrs[i]->dwMemSize )
		{
			return i;
		}
	}
	return -1;
}

bool FrameBufferManager::IsDIaRenderTexture()
{
	// Knowing g_CI and g_ZI

	//if( g_CI.dwWidth )

	bool foundSetScissor=false;
	bool foundFillRect=false;
	bool foundSetFillColor=false;
	bool foundSetCImg=false;
	bool foundTxtRect=false;
	int ucodeLength=10;
	int height;
	uint32 newFillColor;

	uint32 dwPC = gDlistStack[gDlistStackPointer].pc;		// This points to the next instruction

	for( int i=0; i<10; i++ )
	{
		uint32 w0 = *(uint32 *)(g_pRDRAMu8 + dwPC + i*8);
		uint32 w1 = *(uint32 *)(g_pRDRAMu8 + dwPC + 4 + i*8);

		if( (w0>>24) == RDP_SETSCISSOR )
		{
			height   = ((w1>>0 )&0xFFF)/4;
			foundSetScissor = true;
			continue;
		}

		if( (w0>>24) == RDP_SETFILLCOLOR )
		{
			height   = ((w1>>0 )&0xFFF)/4;
			foundSetFillColor = true;
			newFillColor = w1;
			continue;
		}

		if( (w0>>24) == RDP_FILLRECT )
		{
			uint32 x0   = ((w1>>12)&0xFFF)/4;
			uint32 y0   = ((w1>>0 )&0xFFF)/4;
			uint32 x1   = ((w0>>12)&0xFFF)/4;
			uint32 y1   = ((w0>>0 )&0xFFF)/4;

			if( x0 == 0 && y0 == 0 )
			{
				if( x1 == g_CI.dwWidth )
				{
					height = y1;
					foundFillRect = true;
					continue;
				}

				if( x1 == g_CI.dwWidth - 1 )
				{
					height = y1+1;
					foundFillRect = true;
					continue;
				}
			}
		}	

		if( (w0>>24) == RDP_TEXRECT )
		{
			foundTxtRect = true;
			break;
		}

		if( (w0>>24) == RDP_SETCIMG )
		{
			foundSetCImg = true;
			break;
		}
	}

	/*
	bool foundSetScissor=false;
	bool foundFillRect=false;
	bool foundSetFillColor=false;
	bool foundSetCImg=false;
	bool foundTxtRect=false;
	int ucodeLength=10;
	uint32 newFillColor;
	*/

	if( foundFillRect )
	{
		if( foundSetFillColor )
		{
			if( newFillColor != 0xFFFCFFFC )
				return true;	// this is a render_texture
			else
				return false;
		}

		if( gRDP.fillColor != 0x00FFFFF7 )
			return true;	// this is a render_texture
		else
			return false;	// this is a normal ZImg
	}
	else if( foundSetFillColor && newFillColor == 0xFFFCFFFC && foundSetCImg )
	{
		return false;
	}
	else
		return true;


	if( !foundSetCImg )
		return true;

	if( foundSetScissor )
		return true;
}

// Return -1 if the addr is not in recent frame buffer addr
// Return 1 if the addr is in the frame buffer which is currently being displayed
// Return 2 if the addr is in the 3rd frame buffer if we are using triple buffer
// this function will not return 0 which means the addr is within the current rendering frame buffer
//     this should not happen
extern BufferSettingInfo DirectXRenderBufferSettings[];
int FrameBufferManager::IsAddrInRecentFrameBuffers(uint32 addr)
{
	if( addr >= g_uRecentCIInfoPtrs[1]->dwAddr && addr < g_uRecentCIInfoPtrs[1]->dwAddr+g_uRecentCIInfoPtrs[1]->dwMemSize )
		return 1;
	else if( DirectXRenderBufferSettings[options.RenderBufferSetting].number > 1 &&
		addr >= g_uRecentCIInfoPtrs[2]->dwAddr && addr < g_uRecentCIInfoPtrs[2]->dwAddr+g_uRecentCIInfoPtrs[1]->dwMemSize )
	{
		return 2;
	}
	else if( addr >= g_uRecentCIInfoPtrs[0]->dwAddr && addr < g_uRecentCIInfoPtrs[0]->dwAddr+g_uRecentCIInfoPtrs[0]->dwMemSize && status.bHandleN64TextureBuffer )
	{
		return 1;
	}
	{
		return -1;
	}
}

int FrameBufferManager::CheckAddrInBackBuffers(uint32 addr, uint32 memsize, bool copyToRDRAM)
{
	int r = FindRecentCIInfoIndex(addr);

	if( r >= 0 )
	{
		// Also check if the address is overwritten by a recent render_texture
		//int t = CheckAddrInRenderTextures(addr,false);
		int t =-1;
		for( int i=0; i<numOfTxtBufInfos; i++ )
		{
			uint32 bufHeight = gTextureBufferInfos[i].knownHeight ? gTextureBufferInfos[i].N64Height : gTextureBufferInfos[i].maxUsedHeight;
			uint32 bufMemSize = gTextureBufferInfos[i].CI_Info.dwSize*gTextureBufferInfos[i].N64Width*bufHeight;
			if( addr >=gTextureBufferInfos[i].CI_Info.dwAddr && addr < gTextureBufferInfos[i].CI_Info.dwAddr+bufMemSize)
			{
				if( g_uRecentCIInfoPtrs[r]->lastSetAtUcode < gTextureBufferInfos[i].updateAtUcodeCount )
				{
					t = i;
					break;
				}
			}
		}

		if( t >= 0 )
			return -1;
	}

	if( r >= 0 && status.gDlistCount - g_uRecentCIInfoPtrs[r]->lastUsedFrame <= 3  && g_uRecentCIInfoPtrs[r]->bCopied == false )
	{
		SaveBackBuffer(r, NULL, true);
	}		

	return r;
}


uint8 CIFindIndex(uint16 val)
{
	for( int i=0; i<=0xFF; i++ )
	{
		if( val == g_wRDPTlut[i] )
		{
			return (uint8)i;
		}
	}
	return 0;
}


void TexRectToFrameBuffer_8b(uint32 dwXL, uint32 dwYL, uint32 dwXH, uint32 dwYH, float t0u0, float t0v0, float t0u1, float t0v1, uint32 dwTile)
{
	// Copy the texture into the N64 framebuffer memory
	// Used in Yoshi

	/*
	uint32 maxW = g_pTxtBufferInfo->CI_Info.dwWidth;
	uint32 maxH = maxW*3/4;
	if( status.dwTvSystem == TV_SYSTEM_PAL )
	{
	maxH = maxW*9/11;
	}
	*/

	uint32 maxW = g_pTxtBufferInfo->N64Width;
	uint32 maxH = g_pTxtBufferInfo->N64Height;

	uint32 maxOff = maxW*maxH;

	TMEMLoadMapInfo &info = g_tmemLoadAddrMap[gRDP.tiles[dwTile].dwTMem];
	uint32 dwWidth = dwXH-dwXL;
	uint32 dwHeight = dwYH-dwYL;

	float xScale = (t0u1-t0u0)/dwWidth;
	float yScale = (t0v1-t0v0)/dwHeight;

	uint8* dwSrc = g_pRDRAMu8 + info.dwLoadAddress;
	uint8* dwDst = g_pRDRAMu8 + g_pTxtBufferInfo->CI_Info.dwAddr;

	uint32 dwSrcPitch = gRDP.tiles[dwTile].dwPitch;
	uint32 dwDstPitch = g_pTxtBufferInfo->CI_Info.dwWidth;

	uint32 dwSrcOffX = gRDP.tiles[dwTile].hilite_sl;
	uint32 dwSrcOffY = gRDP.tiles[dwTile].hilite_tl;

	uint32 dwLeft = dwXL;
	uint32 dwTop = dwYL;

	dwWidth = min(dwWidth,maxW-dwLeft);
	dwHeight = min(dwHeight, maxH-dwTop);
	if( maxH <= dwTop )	return;

	for (uint32 y = 0; y < dwHeight; y++)
	{
		uint32 dwByteOffset = (uint32)(((y*yScale+dwSrcOffY) * dwSrcPitch) + dwSrcOffX);

		for (uint32 x = 0; x < dwWidth; x++)
		{
			if( (((y+dwTop)*dwDstPitch+x+dwLeft)^0x3) > maxOff )
			{
#ifdef _DEBUG
				TRACE0("Warning: Offset exceeds limit");
#endif
				continue;
			}
			dwDst[((y+dwTop)*dwDstPitch+x+dwLeft)^0x3] = dwSrc[(uint32)(dwByteOffset+x*xScale) ^ 0x3];
		}
	}
}

void TexRectToN64FrameBuffer_16b(uint32 x0, uint32 y0, uint32 width, uint32 height, uint32 dwTile)
{
	// Copy the texture into the N64 RDRAM framebuffer memory

	DrawInfo srcInfo;	
	if( g_textures[dwTile].m_pCTexture->StartUpdate(&srcInfo) == false )
	{
		DebuggerAppendMsg("Fail to lock texture:TexRectToN64FrameBuffer_16b" );
		return;
	}

	uint32 n64CIaddr = g_CI.dwAddr;
	uint32 n64CIwidth = g_CI.dwWidth;

	for (uint32 y = 0; y < height; y++)
	{
		uint32* pSrc = (uint32*)((uint8*)srcInfo.lpSurface + y * srcInfo.lPitch);
		uint16* pN64Buffer = (uint16*)(g_pRDRAMu8+(n64CIaddr&(g_dwRamSize-1)))+(y+y0)*n64CIwidth;

		for (uint32 x = 0; x < width; x++)
		{
			pN64Buffer[x+x0] = ConvertRGBATo555(pSrc[x]);
		}
	}

	g_textures[dwTile].m_pCTexture->EndUpdate(&srcInfo);
}

#define FAST_CRC_CHECKING_INC_X	13
#define FAST_CRC_CHECKING_INC_Y	11
#define FAST_CRC_MIN_Y_INC		2
#define FAST_CRC_MIN_X_INC		2
#define FAST_CRC_MAX_X_INC		7
#define FAST_CRC_MAX_Y_INC		3
extern uint32 dwAsmHeight;
extern uint32 dwAsmPitch;
extern uint32 dwAsmdwBytesPerLine;
extern uint32 dwAsmCRC;
extern uint8* pAsmStart;

uint32 CalculateRDRAMCRC(void *pPhysicalAddress, uint32 left, uint32 top, uint32 width, uint32 height, uint32 size, uint32 pitchInBytes )
{

	dwAsmdwBytesPerLine = ((width<<size)+1)/2;
#if 1
    //Fast Hash used by default unless using a texturepack //Corn
    if( 1 )
    {
	    dwAsmCRC = (uint32)pPhysicalAddress;
		register uint32 *pStart = (uint32*)(pPhysicalAddress);
		register uint32 *pEnd = pStart;

		uint32 pitch = pitchInBytes>>2;
		pStart += (top * pitch) + (((left<<size)+1)>>3);
		pEnd += ((top+height) * pitch) + ((((left+width)<<size)+1)>>3);

		uint32 SizeInDWORD = (uint32)(pEnd-pStart);
        uint32 pinc = SizeInDWORD >> 2; 

		if( pinc < 1 ) pinc = 1;
        if( pinc > 23 ) pinc = 23;

        do
        {
            dwAsmCRC = ((dwAsmCRC << 1) | (dwAsmCRC >> 31)) ^ *pStart;	//This combines to a single instruction in ARM assembler EOR ...,ROR #31 :)
            pStart += pinc;
        }while(pStart < pEnd);
	}
#else
	if( currentRomOptions.bFastTexCRC  && (height>=32 || (dwAsmdwBytesPerLine>>2)>=16))
	{
		dwAsmCRC = 0;
		uint32 realWidthInDWORD = dwAsmdwBytesPerLine>>2;
		uint32 xinc = realWidthInDWORD / FAST_CRC_CHECKING_INC_X;	
		if( xinc < FAST_CRC_MIN_X_INC )
		{
			xinc = min(FAST_CRC_MIN_X_INC, width);
		}
		if( xinc > FAST_CRC_MAX_X_INC )
		{
			xinc = FAST_CRC_MAX_X_INC;
		}

		uint32 yinc = height / FAST_CRC_CHECKING_INC_Y;	
		if( yinc < FAST_CRC_MIN_Y_INC ) 
		{
			yinc = min(FAST_CRC_MIN_Y_INC, height);
		}
		if( yinc > FAST_CRC_MAX_Y_INC )
		{
			yinc = FAST_CRC_MAX_Y_INC;
		}

		uint32 pitch = pitchInBytes>>2;
		register uint32 *pStart = (uint32*)(pPhysicalAddress);
		pStart += (top * pitch) + (((left<<size)+1)>>3);

		/*
		uint32 x,y;
		for (y = 0; y < height; y+=yinc)		// Do every nth line?
		{
		for (x = 0; x < realWidthInDWORD; x+=xinc)
		{
		dwAsmCRC += *(pStart+x);
		dwAsmCRC ^= x;
		}
		pStart += pitch;
		dwAsmCRC ^= y;
		}
		*/


		__asm
		{
			push	esi;
			mov		esi, DWORD PTR [xinc]; 
			mov		ebx, DWORD PTR [pStart];
			mov		eax,0;	// EAX = the CRC
			mov		edx,0x0;
loop1:
			cmp		edx, height;
			jae		endloop1;
			mov		ecx, 0x0;
loop2:
			add		eax, ecx;
			cmp		ecx, DWORD PTR [realWidthInDWORD]
			jae		endloop2;

			rol		eax, 4
			add		eax, DWORD PTR [ebx][ecx*4];

			add		ecx, esi;
			jmp		loop2;
endloop2:
			xor		eax, edx;
			add		edx, DWORD PTR [yinc];
			add		ebx, DWORD PTR [pitch];
			jmp		loop1;
endloop1:
			mov		DWORD PTR [dwAsmCRC], eax;
			pop		esi;
		}
	}
#endif
	else
	{
		dwAsmCRC = 0;
		try{
			dwAsmdwBytesPerLine = ((width<<size)+1)/2;

			pAsmStart = (uint8*)(pPhysicalAddress);
			pAsmStart += (top * pitchInBytes) + (((left<<size)+1)>>1);

			dwAsmHeight = height - 1;
			dwAsmPitch = pitchInBytes;


			__asm 
			{
				push eax
				push ebx
				push ecx
				push edx
				push esi

				mov	ecx, pAsmStart;	// = pStart
				mov	edx, 0			// The CRC
				mov	eax, dwAsmHeight	// = y
l2:				mov	ebx, dwAsmdwBytesPerLine	// = x
				sub	ebx, 4
l1:				mov	esi, [ecx+ebx]
				xor esi, ebx
				rol edx, 4
				add edx, esi
				sub	ebx, 4
				jge l1
				xor esi, eax
				add edx, esi
				add ecx, dwAsmPitch
				dec eax
				jge l2

				mov	dwAsmCRC, edx

				pop esi
				pop edx
				pop ecx
				pop ebx
				pop	eax
			}
		}
		catch(...)
		{
		}
	}
	return dwAsmCRC;
}

BYTE CalculateMaxCI(void *pPhysicalAddress, uint32 left, uint32 top, uint32 width, uint32 height, uint32 size, uint32 pitchInBytes )
{
	uint32 x, y;
	BYTE *buf;
	BYTE val = 0;

	if( TXT_SIZE_8b == size )
	{
		for( y = 0; y<height; y++ )
		{
			buf = (BYTE*)pPhysicalAddress + left + pitchInBytes * (y+top);
			for( x=0; x<width; x++ )
			{
				if( buf[x] > val )	val = buf[x];
				if( val == 0xFF )
					return 0xFF;
			}
		}
	}
	else
	{
		BYTE val1,val2;
		left >>= 1;
		width >>= 1;
		for( y = 0; y<height; y++ )
		{
			buf = (BYTE*)pPhysicalAddress + left + pitchInBytes * (y+top);
			for( x=0; x<width; x++ )
			{
				val1 = buf[x]>>4;
				val2 = buf[x]&0xF;
				if( val1 > val )	val = val1;
				if( val2 > val )	val = val2;
				if( val == 0xF )
					return 0xF;
			}
		}
	}

	return val;
}

//
//uint32 CalculateRDRAMCRC2(void *pPhysicalAddress, uint32 left, uint32 top, uint32 width, uint32 height, uint32 size, uint32 pitchInBytes )
//{
//	dwAsmCRC = 0;
//	dwAsmdwBytesPerLine = ((width<<size)+1)/2;
//
//	if( currentRomOptions.bFastTexCRC && !options.bLoadHiResTextures && (height>=32 || (dwAsmdwBytesPerLine>>2)>=16))
//	{
//		uint32 realWidthInDWORD = dwAsmdwBytesPerLine>>2;
//		uint32 xinc = realWidthInDWORD / FAST_CRC_CHECKING_INC_X;	
//		if( xinc < FAST_CRC_MIN_X_INC )
//		{
//			xinc = min(FAST_CRC_MIN_X_INC, width);
//		}
//		if( xinc > FAST_CRC_MAX_X_INC )
//		{
//			xinc = FAST_CRC_MAX_X_INC;
//		}
//
//		uint32 yinc = height / FAST_CRC_CHECKING_INC_Y;	
//		if( yinc < FAST_CRC_MIN_Y_INC ) 
//		{
//			yinc = min(FAST_CRC_MIN_Y_INC, height);
//		}
//		if( yinc > FAST_CRC_MAX_Y_INC )
//		{
//			yinc = FAST_CRC_MAX_Y_INC;
//		}
//
//		uint32 pitch = pitchInBytes>>2;
//		register uint32 *pStart = (uint32*)(pPhysicalAddress);
//		pStart += (top * pitch) + (((left<<size)+1)>>3);
//
//		/*
//		uint32 x,y;
//		for (y = 0; y < height; y+=yinc)		// Do every nth line?
//		{
//		for (x = 0; x < realWidthInDWORD; x+=xinc)
//		{
//		dwAsmCRC += *(pStart+x);
//		dwAsmCRC ^= x;
//		}
//		pStart += pitch;
//		dwAsmCRC ^= y;
//		}
//		*/
//
//
//		__asm
//		{
//			push	esi;
//			mov		esi, DWORD PTR [xinc]; 
//			mov		ebx, DWORD PTR [pStart];
//			mov		eax,0;	// EAX = the CRC
//			mov		edx,0x0;
//loop1:
//			cmp		edx, height;
//			jae		endloop1;
//			mov		ecx, 0x0;
//loop2:
//			add		eax, ecx;
//			cmp		ecx, DWORD PTR [realWidthInDWORD]
//			jae		endloop2;
//
//			add		eax, DWORD PTR [ebx][ecx*4];
//
//			add		ecx, esi;
//			jmp		loop2;
//endloop2:
//			xor		eax, edx;
//			add		edx, DWORD PTR [yinc];
//			add		ebx, DWORD PTR [pitch];
//			jmp		loop1;
//endloop1:
//			mov		DWORD PTR [dwAsmCRC], eax;
//			pop		esi;
//		}
//	}
//	else
//	{
//		try{
//			dwAsmdwBytesPerLine = ((width<<size)+1)/2;
//
//			pAsmStart = (uint8*)(pPhysicalAddress);
//			pAsmStart += (top * pitchInBytes) + (((left<<size)+1)>>1);
//
//			dwAsmHeight = height - 1;
//			dwAsmPitch = pitchInBytes;
//
//
//			__asm 
//			{
//				push eax
//					push ebx
//					push ecx
//					push edx
//					push esi
//
//					mov	ecx, pAsmStart;	// = pStart
//				mov	edx, 0			// The CRC
//					mov	eax, dwAsmHeight	// = y
//l2:				mov	ebx, dwAsmdwBytesPerLine	// = x
//				sub	ebx, 4
//l1:				mov	esi, [ecx+ebx]
//				xor esi, ebx
//					add edx, esi
//					sub	ebx, 4
//					jge l1
//					xor esi, eax
//					add edx, esi
//					add ecx, dwAsmPitch
//					dec eax
//					jge l2
//
//					mov	dwAsmCRC, edx
//
//					pop esi
//					pop edx
//					pop ecx
//					pop ebx
//					pop	eax
//			}
//		}
//		catch(...)
//		{
//			TRACE0("Exception in texture CRC calculation");
//		}
//	}
//	return dwAsmCRC;
//}

bool FrameBufferManager::FrameBufferInRDRAMCheckCRC()
{
	RecentCIInfo &p = *(g_uRecentCIInfoPtrs[0]);
	uint8 *pFrameBufferBase = (uint8*)(g_pRDRAMu8+p.dwAddr);
	uint32 pitch = (p.dwWidth << p.dwSize ) >> 1;
	uint32 crc = CalculateRDRAMCRC(pFrameBufferBase, 0, 0, p.dwWidth, p.dwHeight, p.dwSize, pitch);
	if( crc != p.dwCRC )
	{
		p.dwCRC = crc;
		return false;
	}
	else
	{
		return true;
	}
}

extern std::vector<uint32> frameWriteRecord;
void FrameBufferManager::FrameBufferWriteByCPU(uint32 addr, uint32 size)
{
	if( !frameBufferOptions.bProcessCPUWrite )	return;
	//WARNING(TRACE2("Frame Buffer Write, addr=%08X, CI Addr=%08X", addr, g_CI.dwAddr));
	status.frameWriteByCPU = TRUE;
	frameWriteRecord.push_back(addr&(g_dwRamSize-1));
}

extern RECT frameWriteByCPURect;
extern std::vector<RECT> frameWriteByCPURects;
extern RECT frameWriteByCPURectArray[20][20];
extern bool frameWriteByCPURectFlag[20][20];
#define FRAMEBUFFER_IN_BLOCK
bool FrameBufferManager::ProcessFrameWriteRecord()
{
	int size = frameWriteRecord.size();
	if( size == 0 ) return false;

	int index = FindRecentCIInfoIndex(frameWriteRecord[0]);
	if( index == -1 )
	{
		frameWriteRecord.clear();
		return false;
	}
	else
	{
		uint32 base = g_uRecentCIInfoPtrs[index]->dwAddr;
		uint32 uwidth = g_uRecentCIInfoPtrs[index]->dwWidth;
		uint32 uheight = g_uRecentCIInfoPtrs[index]->dwHeight;
		uint32 upitch = uwidth<<1;

		frameWriteByCPURect.left=uwidth-1;
		frameWriteByCPURect.top = uheight-1;

		frameWriteByCPURect.right=0;
		frameWriteByCPURect.bottom = 0;

		int x, y, off;

		for( int i=0; i<size; i++ )
		{
			off = frameWriteRecord[i]-base;
			if( off < (int)g_uRecentCIInfoPtrs[index]->dwMemSize )
			{
				y = off/upitch;
				x = (off - y*upitch)>>1;

#ifdef FRAMEBUFFER_IN_BLOCK
				int xidx=x/32;
				int yidx=y/24;

				RECT &rect = frameWriteByCPURectArray[xidx][yidx];

				if( !frameWriteByCPURectFlag[xidx][yidx] )
				{
					rect.left=rect.right=x;
					rect.top=rect.bottom=y;
					frameWriteByCPURectFlag[xidx][yidx]=true;
				}
				else
				{
					if( x < rect.left )	rect.left = x;
					if( x > rect.right ) rect.right = x;
					if( y < rect.top )	rect.top = y;
					if( y > rect.bottom ) rect.bottom = y;
				}
#else

				/*
				int index = -1;
				int rectsize = frameWriteByCPURects.size();

				if( rectsize == 0 )
				{
				RECT rect;
				rect.left=rect.right=x;
				rect.top=rect.bottom=y;
				frameWriteByCPURects.push_back(rect);
				continue;
				}

				for( int j=0; j<rectsize; j++ )
				{
				if( ( (x >= frameWriteByCPURects[j].left && (x<=frameWriteByCPURects[j].right || x-frameWriteByCPURects[j].left<=30)) ||
				(x < frameWriteByCPURects[j].left && frameWriteByCPURects[j].right-x <= 30) ) &&
				( (y >= frameWriteByCPURects[j].top && (x<=frameWriteByCPURects[j].bottom || x-frameWriteByCPURects[j].top<=30)) ||
				(y < frameWriteByCPURects[j].top && frameWriteByCPURects[j].bottom-y <= 30) ) )
				{
				index = j;
				break;
				}
				}

				if( index < 0 )
				{
				RECT rect;
				rect.left=rect.right=x;
				rect.top=rect.bottom=y;
				frameWriteByCPURects.push_back(rect);
				continue;
				}

				RECT &rect = frameWriteByCPURects[index];
				if( x < rect.left )	rect.left = x;
				if( x > rect.right ) rect.right = x;
				if( y < rect.top )	rect.top = y;
				if( y > rect.bottom ) rect.bottom = y;
				*/

				if( x < frameWriteByCPURect.left )	frameWriteByCPURect.left = x;
				if( x > frameWriteByCPURect.right ) frameWriteByCPURect.right = x;
				if( y < frameWriteByCPURect.top )	frameWriteByCPURect.top = y;
				if( y > frameWriteByCPURect.bottom ) frameWriteByCPURect.bottom = y;
#endif
			}
		}

		frameWriteRecord.clear();
		return true;
	}
}

void FrameBufferManager::FrameBufferReadByCPU( uint32 addr )
{
	///return;	// it does not work very well anyway


	if( !frameBufferOptions.bProcessCPURead )	return;

	addr &= (g_dwRamSize-1);
	int index = FindRecentCIInfoIndex(addr);
	if( index == -1 ) 
	{
		// Check if this is the depth buffer
		uint32 size = 2*g_RecentCIInfo[0].dwWidth*g_RecentCIInfo[0].dwHeight;
		addr &= 0x3FFFFFFF;

		if(!( addr >= g_ZI.dwAddr && addr < g_ZI.dwAddr + size ))
		{
			return;
		}
	}

	if( status.gDlistCount - g_uRecentCIInfoPtrs[index]->lastUsedFrame > 3 )
	{
		// Ok, we don't have this frame anymore
		return;
	}

	if( g_uRecentCIInfoPtrs[index]->bCopied )	return;
	//if( addr != g_uRecentCIInfoPtrs[index]->dwAddr )	return;

	uint32 size = 0x1000 - addr%0x1000;
	CheckAddrInBackBuffers(addr, size, true);
}



extern RECT frameWriteByCPURect;
extern std::vector<RECT> frameWriteByCPURects;
extern RECT frameWriteByCPURectArray[20][20];
extern bool frameWriteByCPURectFlag[20][20];
#define FRAMEBUFFER_IN_BLOCK

void FrameBufferManager::UpdateFrameBufferBeforeUpdateFrame()
{
	if( (frameBufferOptions.bProcessCPUWrite && status.frameWriteByCPU ) ||
		(frameBufferOptions.bLoadBackBufFromRDRAM && !FrameBufferInRDRAMCheckCRC() ) )		
		// Checks if frame buffer has been modified by CPU
		// Only happens to Dr. Mario
	{
		if( frameBufferOptions.bProcessCPUWrite )
		{
			if( ProcessFrameWriteRecord() )
			{
#ifdef FRAMEBUFFER_IN_BLOCK
				int i,j;
				for( i=0; i<20; i++)
				{
					for( j=0; j<20; j++ )
					{
						if( frameWriteByCPURectFlag[i][j] )
						{
							CRender::GetRender()->DrawFrameBuffer(false, frameWriteByCPURectArray[i][j].left, frameWriteByCPURectArray[i][j].top,
								frameWriteByCPURectArray[i][j].right-frameWriteByCPURectArray[i][j].left+1, frameWriteByCPURectArray[i][j].bottom-frameWriteByCPURectArray[i][j].top+1);
						}
					}
				}
				for( i=0; i<20; i++)
				{
					for( j=0; j<20; j++ )
					{
						if( frameWriteByCPURectFlag[i][j] )
						{
							ClearN64FrameBufferToBlack(frameWriteByCPURectArray[i][j].left, frameWriteByCPURectArray[i][j].top,
								frameWriteByCPURectArray[i][j].right-frameWriteByCPURectArray[i][j].left+1, frameWriteByCPURectArray[i][j].bottom-frameWriteByCPURectArray[i][j].top+1);
							frameWriteByCPURectFlag[i][j] = false;
						}
					}
				}
				//memset(frameWriteByCPURectArray, 0, sizeof(frameWriteByCPURectArray));
				//memset(frameWriteByCPURectFlag, 0, sizeof(frameWriteByCPURectFlag));
#else
				CRender::GetRender()->DrawFrameBuffer(false, frameWriteByCPURect.left, frameWriteByCPURect.top,
					frameWriteByCPURect.right-frameWriteByCPURect.left, frameWriteByCPURect.bottom-frameWriteByCPURect.top);
				ClearN64FrameBufferToBlack(frameWriteByCPURect.left, frameWriteByCPURect.top,
					frameWriteByCPURect.right-frameWriteByCPURect.left+1, frameWriteByCPURect.bottom-frameWriteByCPURect.top+1);

				/*
				int size = frameWriteByCPURects.size();
				for( int i=0; i<size; i++)
				{
				CRender::GetRender()->DrawFrameBuffer(false, frameWriteByCPURects[i].left, frameWriteByCPURects[i].top,
				frameWriteByCPURects[i].right-frameWriteByCPURects[i].left, frameWriteByCPURects[i].bottom-frameWriteByCPURects[i].top);
				ClearN64FrameBufferToBlack(frameWriteByCPURects[i].left, frameWriteByCPURects[i].top,
				frameWriteByCPURects[i].right-frameWriteByCPURects[i].left+1, frameWriteByCPURects[i].bottom-frameWriteByCPURects[i].top+1);
				}
				frameWriteByCPURects.clear();
				*/
#endif
			}
			status.frameWriteByCPU = FALSE;
		}
		else
		{
			if (CRender::IsAvailable())
			{
				RecentCIInfo &p = *(g_uRecentCIInfoPtrs[0]);
				CRender::GetRender()->DrawFrameBuffer(false, 0,0,p.dwWidth,p.dwHeight);
				ClearN64FrameBufferToBlack();
			}
		}
	}
}

uint32 FrameBufferManager::ComputeCImgHeight(SetImgInfo &info, uint32 &height)
{
	uint32 dwPC = gDlistStack[gDlistStackPointer].pc;		// This points to the next instruction

	for( int i=0; i<10; i++ )
	{
		uint32 w0 = *(uint32 *)(g_pRDRAMu8 + dwPC + i*8);
		uint32 w1 = *(uint32 *)(g_pRDRAMu8 + dwPC + 4 + i*8);

		if( (w0>>24) == RDP_SETSCISSOR )
		{
			height   = ((w1>>0 )&0xFFF)/4;
			return RDP_SETSCISSOR;
		}

		if( (w0>>24) == RDP_FILLRECT )
		{
			uint32 x0   = ((w1>>12)&0xFFF)/4;
			uint32 y0   = ((w1>>0 )&0xFFF)/4;
			uint32 x1   = ((w0>>12)&0xFFF)/4;
			uint32 y1   = ((w0>>0 )&0xFFF)/4;

			if( x0 == 0 && y0 == 0 )
			{
				if( x1 == info.dwWidth )
				{
					height = y1;
					return RDP_FILLRECT;
				}

				if( x1 == info.dwWidth - 1 )
				{
					height = y1+1;
					return RDP_FILLRECT;
				}
			}
		}	

		if( (w0>>24) == RDP_SETCIMG )
		{
			goto step2;
		}

		if( (w0>>24) == RDP_SETCIMG )
		{
			goto step2;
		}
	}

	if( gRDP.scissor.left == 0 && gRDP.scissor.top == 0 && gRDP.scissor.right == info.dwWidth )
	{
		height = gRDP.scissor.bottom;
		return RDP_SETSCISSOR+1;
	}

step2:
	height = info.dwWidth*3/4;
	if( status.dwTvSystem == TV_SYSTEM_PAL )
	{
		height = info.dwWidth*9/11;
	}

	if( gRDP.scissor.bottom < (int)height && gRDP.scissor.bottom != 0 )
	{
		height = gRDP.scissor.bottom;
	}

	if( info.dwAddr + height*info.dwWidth*info.dwSize >= g_dwRamSize )
	{
		height = info.dwWidth*3/4;
		if( status.dwTvSystem == TV_SYSTEM_PAL )
		{
			height = info.dwWidth*9/11;
		}

		if( gRDP.scissor.bottom < (int)height && gRDP.scissor.bottom != 0 )
		{
			height = gRDP.scissor.bottom;
		}

		if( info.dwAddr + height*info.dwWidth*info.dwSize >= g_dwRamSize )
		{
			height = ( g_dwRamSize - info.dwAddr ) / info.dwWidth;
		}
	}

	return 0;
}

int FrameBufferManager::CheckRenderTexturesWithNewCI(SetImgInfo &CIinfo, uint32 height, bool byNewTxtrBuf)
{
	int matchidx = -1;
	uint32 memsize = ((height*CIinfo.dwWidth)>>1)<<CIinfo.dwSize;

	for( int i=0; i<numOfTxtBufInfos; i++ )
	{
		TextureBufferInfo &info = gTextureBufferInfos[i];
		if( !info.isUsed )	continue;

		bool covered = false;

		if( info.CI_Info.dwAddr == CIinfo.dwAddr )
		{
			if( info.CI_Info.dwSize == CIinfo.dwSize &&
				info.CI_Info.dwWidth == CIinfo.dwWidth &&
				info.CI_Info.dwFormat == CIinfo.dwFormat &&
				info.N64Height == height 
				&& info.CI_Info.dwAddr == CIinfo.dwAddr 
				)
			{
				// This is the same texture at the same address
				if( byNewTxtrBuf )
				{
					matchidx = i;
					break;
				}
			}

			// At the same address, but not the same size
			//SAFE_DELETE(info.psurf);
			covered = true;
		}

		if( !covered )
		{
			uint32 memsize2 = ((info.N64Height*info.N64Width)>>1)<<info.CI_Info.dwSize;

			if( info.CI_Info.dwAddr > CIinfo.dwAddr && info.CI_Info.dwAddr < CIinfo.dwAddr + memsize)
				covered = true;
			else if( info.CI_Info.dwAddr+memsize2 > CIinfo.dwAddr && info.CI_Info.dwAddr+memsize2 < CIinfo.dwAddr + memsize)
				covered = true;
			else if( CIinfo.dwAddr > info.CI_Info.dwAddr && CIinfo.dwAddr < info.CI_Info.dwAddr + memsize2 )
				covered = true;
			else if( CIinfo.dwAddr+ memsize > info.CI_Info.dwAddr && CIinfo.dwAddr+ memsize < info.CI_Info.dwAddr + memsize2 )
				covered = true;
		}

		if( covered )
		{
			//SAFE_DELETE(info.psurf);
			info.isUsed = false;
			SAFE_DELETE(info.pTxtBuffer);
			info.txtEntry.pTexture = NULL;
			continue;
		}
	}

	return matchidx;
}

//RecentCIInfo *g_uRecentCIInfoPtrs[5];
TextureBufferInfo newTextureBufInfo;

int FrameBufferManager::FindASlot(void)
{
	int idx;

	// Find an empty slot
	bool found = false;
	for( int i=0; i<numOfTxtBufInfos; i++ )
	{
		if( !gTextureBufferInfos[i].isUsed && gTextureBufferInfos[i].updateAtFrame < status.gDlistCount )
		{
			found = true;
			idx = i;
			break;
		}
	}

	// If cannot find an empty slot, find the oldest slot and reuse the slot
	if( !found )
	{
		uint32 oldestCount=0xFFFFFFFF;
		uint32 oldestIdx = 0;
		for( int i=0; i<numOfTxtBufInfos; i++ )
		{
			if( gTextureBufferInfos[i].updateAtUcodeCount < oldestCount )
			{
				oldestCount = gTextureBufferInfos[i].updateAtUcodeCount;
				oldestIdx = i;
			}
		}

		idx = oldestIdx;
	}

	SAFE_DELETE(gTextureBufferInfos[idx].pTxtBuffer) ;

	return idx;
}


void FrameBufferManager::SetRenderTexture(void)
{
	memcpy(&(newTextureBufInfo.CI_Info), &g_CI, sizeof(SetImgInfo));

	newTextureBufInfo.N64Width = newTextureBufInfo.CI_Info.dwWidth;
	newTextureBufInfo.knownHeight = ComputeCImgHeight(g_CI, newTextureBufInfo.N64Height);

	status.bHandleN64TextureBuffer = true;
	newTextureBufInfo.maxUsedHeight = 0;

	if( defaultRomOptions.bInN64Resolution )
	{
		newTextureBufInfo.bufferWidth = newTextureBufInfo.N64Width;
		newTextureBufInfo.bufferHeight = newTextureBufInfo.N64Height;
	}
	else if( defaultRomOptions.bDoubleSizeForSmallTxtrBuf && newTextureBufInfo.N64Width<=128 && newTextureBufInfo.N64Height<=128)
	{
		newTextureBufInfo.bufferWidth = newTextureBufInfo.N64Width*2;
		newTextureBufInfo.bufferHeight = newTextureBufInfo.N64Height*2;
	}
	else
	{
		newTextureBufInfo.bufferWidth = newTextureBufInfo.N64Width;
		newTextureBufInfo.bufferHeight = newTextureBufInfo.N64Height;
	}

	newTextureBufInfo.scaleX = newTextureBufInfo.bufferWidth / float(newTextureBufInfo.N64Width);
	newTextureBufInfo.scaleY = newTextureBufInfo.bufferHeight / float(newTextureBufInfo.N64Height);

	status.bFrameBufferIsDrawn = false;
	status.bFrameBufferDrawnByTriangles = false;

	newTextureBufInfo.updateAtFrame = status.gDlistCount;
	newTextureBufInfo.updateAtUcodeCount = status.gUcodeCount;

	// Delay activation of the render_texture until the 1st rendering
}

int FrameBufferManager::SetBackBufferAsRenderTexture(SetImgInfo &CIinfo, int ciInfoIdx)
{
	TextureBufferInfo tempTxtBufferInfo;

	memcpy(&(tempTxtBufferInfo.CI_Info), &CIinfo, sizeof(SetImgInfo));

	tempTxtBufferInfo.N64Width = g_uRecentCIInfoPtrs[ciInfoIdx]->dwLastWidth;
	tempTxtBufferInfo.N64Height = g_uRecentCIInfoPtrs[ciInfoIdx]->dwLastHeight;
	tempTxtBufferInfo.knownHeight = true;
	tempTxtBufferInfo.maxUsedHeight = 0;

	tempTxtBufferInfo.bufferWidth = windowSetting.uDisplayWidth;
	tempTxtBufferInfo.bufferHeight = windowSetting.uDisplayHeight;

	tempTxtBufferInfo.scaleX = tempTxtBufferInfo.bufferWidth / float(tempTxtBufferInfo.N64Width);
	tempTxtBufferInfo.scaleY = tempTxtBufferInfo.bufferHeight / float(tempTxtBufferInfo.N64Height);

	status.bFrameBufferIsDrawn = false;
	status.bFrameBufferDrawnByTriangles = false;

	tempTxtBufferInfo.updateAtFrame = status.gDlistCount;
	tempTxtBufferInfo.updateAtUcodeCount = status.gUcodeCount;

	// Checking against previous render_texture infos
	uint32 memsize = ((tempTxtBufferInfo.N64Height*tempTxtBufferInfo.N64Width)>>1)<<tempTxtBufferInfo.CI_Info.dwSize;
	int matchidx = CheckRenderTexturesWithNewCI(CIinfo,tempTxtBufferInfo.N64Height,false);
	int idxToUse = (matchidx >= 0) ? matchidx : FindASlot();

	if( gTextureBufferInfos[idxToUse].pTxtBuffer == NULL || matchidx < 0 )
	{
		if( CDeviceBuilder::m_deviceGeneralType == DIRECTX_DEVICE )
			gTextureBufferInfos[idxToUse].pTxtBuffer = 
				new CDXTextureBuffer(tempTxtBufferInfo.bufferWidth, tempTxtBufferInfo.bufferHeight, &gTextureBufferInfos[idxToUse], AS_BACK_BUFFER_SAVE);
#if 0 // disable for OGL right now
		else
			gTextureBufferInfos[idxToUse].pTxtBuffer = 
				new COGLRenderTexture(tempTxtBufferInfo.bufferWidth, tempTxtBufferInfo.bufferHeight, &gTextureBufferInfos[idxToUse], AS_BACK_BUFFER_SAVE);
#endif
	}

	// Need to set all variables for gTextureBufferInfos[idxToUse]
	CTextureBuffer *pTxtBuffer = gTextureBufferInfos[idxToUse].pTxtBuffer;
	memcpy(&gTextureBufferInfos[idxToUse], &tempTxtBufferInfo, sizeof(TextureBufferInfo) );
	gTextureBufferInfos[idxToUse].pTxtBuffer = pTxtBuffer;
	gTextureBufferInfos[idxToUse].isUsed = true;
	gTextureBufferInfos[idxToUse].txtEntry.pTexture = pTxtBuffer->m_pTexture;
	gTextureBufferInfos[idxToUse].txtEntry.txtrBufIdx = idxToUse+1;

	return idxToUse;
}

void FrameBufferManager::CloseRenderTexture(bool toSave)
{
	if( m_curRenderTextureIndex < 0 )	return;

	status.bHandleN64TextureBuffer = false;
	if( status.bDirectWriteIntoRDRAM )
	{
	}
	else
	{
		RestoreNormalBackBuffer();
		if( !toSave || !status.bFrameBufferIsDrawn || !status.bFrameBufferDrawnByTriangles )
		{
			SAFE_DELETE(gTextureBufferInfos[m_curRenderTextureIndex].pTxtBuffer);
			gTextureBufferInfos[m_curRenderTextureIndex].isUsed = false;
		}
		else
		{
			StoreRenderTextureToRDRAM();

			if( frameBufferOptions.bTxtBufWriteBack )
			{
				SAFE_DELETE(gTextureBufferInfos[m_curRenderTextureIndex].pTxtBuffer);
				gTextureBufferInfos[m_curRenderTextureIndex].isUsed = false;
			}
			else
			{
				g_pTxtBufferInfo->crcInRDRAM = ComputeRenderTextureCRCInRDRAM(m_curRenderTextureIndex);
				g_pTxtBufferInfo->crcCheckedAtFrame = status.gDlistCount;
			}
		}
	}

	SetScreenMult(windowSetting.uDisplayWidth/windowSetting.fViWidth, windowSetting.uDisplayHeight/windowSetting.fViHeight);
	CRender::g_pRender->UpdateClipRectangle();
	CRender::g_pRender->ApplyScissorWithClipRatio();
}

void FrameBufferManager::ClearN64FrameBufferToBlack(uint32 left, uint32 top, uint32 width, uint32 height)
{
	RecentCIInfo &p = *(g_uRecentCIInfoPtrs[0]);
	uint16 *frameBufferBase = (uint16*)(g_pRDRAMu8+p.dwAddr);
	uint32 pitch = p.dwWidth;

	if( width == 0 || height == 0 )
	{
		uint32 len = p.dwHeight*p.dwWidth*p.dwSize;
		if( p.dwSize == TXT_SIZE_4b ) len = (p.dwHeight*p.dwWidth)>>1;
		memset(frameBufferBase, 0, len);
	}
	else
	{
		for( uint32 y=0; y<height; y++)
		{
			for( uint32 x=0; x<width; x++ )
			{
				*(frameBufferBase+(y+top)*pitch+x+left) = 0;
			}
		}
	}
}

uint8 RevTlutTable[0x10000];
bool RevTlutTableNeedUpdate = false;
void InitTlutReverseLookup(void)
{
	if( RevTlutTableNeedUpdate )
	{
		memset(RevTlutTable, 0, 0x10000);
		for( int i=0; i<=0xFF; i++ )
		{
			RevTlutTable[g_wRDPTlut[i]] = uint8(i);
		}

		RevTlutTableNeedUpdate = false;
	}
}


void FrameBufferManager::CopyBackToFrameBufferIfReadByCPU(uint32 addr)
{
	int i = FindRecentCIInfoIndex(addr);
	if( i != -1 )
	{
		//if( i == 0 ) CGraphicsContext::Get()->UpdateFrame();
		RecentCIInfo *info = g_uRecentCIInfoPtrs[i];
		StoreBackBufferToRDRAM( info->dwAddr, info->dwFormat, info->dwSize, info->dwWidth, info->dwHeight, 
			windowSetting.uDisplayWidth, windowSetting.uDisplayHeight, addr, 0x1000-addr%0x1000);
	}
}
void FrameBufferManager::CheckRenderTextureCRCInRDRAM(void)
{
	for( int i=0; i<numOfTxtBufInfos; i++ )
	{
		if( !gTextureBufferInfos[i].isUsed )	
			continue;

		if( gTextureBufferInfos[i].pTxtBuffer->IsBeingRendered() )
			continue;

		if( gTextureBufferInfos[i].crcCheckedAtFrame < status.gDlistCount )
		{
			uint32 crc = ComputeRenderTextureCRCInRDRAM(i);
			if( gTextureBufferInfos[i].crcInRDRAM != crc )
			{
				// RDRAM has been modified by CPU core
				SAFE_DELETE(gTextureBufferInfos[i].pTxtBuffer);
				gTextureBufferInfos[i].isUsed = false;
				continue;
			}
			else
			{
				gTextureBufferInfos[i].crcCheckedAtFrame = status.gDlistCount;
			}
		}
	}
}

int	FrameBufferManager::CheckAddrInRenderTextures(uint32 addr, bool checkcrc)
{
	for( int i=0; i<numOfTxtBufInfos; i++ )
	{
		if( !gTextureBufferInfos[i].isUsed )	
			continue;

		if( gTextureBufferInfos[i].pTxtBuffer->IsBeingRendered() )
			continue;

		uint32 bufHeight = gTextureBufferInfos[i].knownHeight ? gTextureBufferInfos[i].N64Height : gTextureBufferInfos[i].maxUsedHeight;
		uint32 bufMemSize = gTextureBufferInfos[i].CI_Info.dwSize*gTextureBufferInfos[i].N64Width*bufHeight;
		if( addr >=gTextureBufferInfos[i].CI_Info.dwAddr && addr < gTextureBufferInfos[i].CI_Info.dwAddr+bufMemSize)
		{
			if(checkcrc)
			{
				// Check the CRC in RDRAM
				if( gTextureBufferInfos[i].crcCheckedAtFrame < status.gDlistCount )
				{
					uint32 crc = ComputeRenderTextureCRCInRDRAM(i);
					if( gTextureBufferInfos[i].crcInRDRAM != crc )
					{
						// RDRAM has been modified by CPU core
						SAFE_DELETE(gTextureBufferInfos[i].pTxtBuffer);
						gTextureBufferInfos[i].isUsed = false;
						continue;
					}
					else
					{
						gTextureBufferInfos[i].crcCheckedAtFrame = status.gDlistCount;
					}
				}
			}
			return i;
		}
	}

	return -1;
}

void FrameBufferManager::LoadTextureFromRenderTexture(TxtrCacheEntry* pEntry, int infoIdx)
{
	if( infoIdx < 0 || infoIdx >= numOfTxtBufInfos )
	{
		infoIdx = CheckAddrInRenderTextures(pEntry->ti.Address);
	}

	if( infoIdx >= 0 && gTextureBufferInfos[infoIdx].isUsed && gTextureBufferInfos[infoIdx].pTxtBuffer )
	{
		gTextureBufferInfos[infoIdx].pTxtBuffer->LoadTexture(pEntry);
	}
}

void FrameBufferManager::RestoreNormalBackBuffer()
{
	if( m_curRenderTextureIndex >= 0 && m_curRenderTextureIndex < numOfTxtBufInfos )
	{
		if( gTextureBufferInfos[m_curRenderTextureIndex].pTxtBuffer )
			gTextureBufferInfos[m_curRenderTextureIndex].pTxtBuffer->SetAsRenderTarget(false);
		m_isRenderingToTexture = false;
		m_lastTextureBufferIndex = m_curRenderTextureIndex;
	}

	if( !status.bFrameBufferIsDrawn || !status.bFrameBufferDrawnByTriangles )
	{
		gTextureBufferInfos[m_curRenderTextureIndex].isUsed = false;
		SAFE_DELETE(gTextureBufferInfos[m_curRenderTextureIndex].pTxtBuffer);
	}
}

uint32 FrameBufferManager::ComputeRenderTextureCRCInRDRAM(int infoIdx)
{
	if( infoIdx >= numOfTxtBufInfos || infoIdx < 0 || !gTextureBufferInfos[infoIdx].isUsed )
		return 0;

	TextureBufferInfo &info = gTextureBufferInfos[infoIdx];
	uint32 height = info.knownHeight ? info.N64Height : info.maxUsedHeight;
	uint8 *pAddr = (uint8*)(g_pRDRAMu8+info.CI_Info.dwAddr);
	uint32 pitch = (info.N64Width << info.CI_Info.dwSize ) >> 1;

	return CalculateRDRAMCRC(pAddr, 0, 0, info.N64Width, height, info.CI_Info.dwSize, pitch);
}

void FrameBufferManager::ActiveTextureBuffer(void)
{
	status.bCIBufferIsRendered = true;

	if( status.bHandleN64TextureBuffer )
	{
		// Checking against previous render_texture infos
		int matchidx = -1;

		uint32 memsize = ((newTextureBufInfo.N64Height*newTextureBufInfo.N64Width)>>1)<<newTextureBufInfo.CI_Info.dwSize;

		matchidx = CheckRenderTexturesWithNewCI(g_CI,newTextureBufInfo.N64Height,true);

		int idxToUse=-1;
		if( matchidx >= 0 )
		{
			// Reuse the matched slot
			idxToUse = matchidx;
		}
		else
		{
			idxToUse = FindASlot();
		}

		if( gTextureBufferInfos[idxToUse].pTxtBuffer == NULL || matchidx < 0 )
		{
			int w = newTextureBufInfo.bufferWidth;
			if( newTextureBufInfo.knownHeight == RDP_SETSCISSOR && newTextureBufInfo.CI_Info.dwAddr == g_ZI.dwAddr )
			{
				w = gRDP.scissor.right;
			}

			if( CDeviceBuilder::m_deviceGeneralType == DIRECTX_DEVICE )
				gTextureBufferInfos[idxToUse].pTxtBuffer = 
					new CDXTextureBuffer(w, newTextureBufInfo.bufferHeight, &gTextureBufferInfos[idxToUse], AS_RENDER_TARGET);
#if 0 // Disable for OGL
			else
				gTextureBufferInfos[idxToUse].pTxtBuffer = 
					new COGLRenderTexture(w, newTextureBufInfo.bufferHeight, &gTextureBufferInfos[idxToUse], AS_RENDER_TARGET);
#endif
		}

		// Need to set all variables for gTextureBufferInfos[idxToUse]
		CTextureBuffer *pTxtBuffer = gTextureBufferInfos[idxToUse].pTxtBuffer;
		memcpy(&gTextureBufferInfos[idxToUse], &newTextureBufInfo, sizeof(TextureBufferInfo) );
		gTextureBufferInfos[idxToUse].pTxtBuffer = pTxtBuffer;
		gTextureBufferInfos[idxToUse].isUsed = true;
		gTextureBufferInfos[idxToUse].txtEntry.pTexture = pTxtBuffer->m_pTexture;
		gTextureBufferInfos[idxToUse].txtEntry.txtrBufIdx = idxToUse+1;

		g_pTxtBufferInfo = &gTextureBufferInfos[idxToUse];

		// Active the render_texture
		if( m_curRenderTextureIndex >= 0 && gTextureBufferInfos[m_curRenderTextureIndex].isUsed && gTextureBufferInfos[m_curRenderTextureIndex].pTxtBuffer )
		{
			gTextureBufferInfos[m_curRenderTextureIndex].pTxtBuffer->SetAsRenderTarget(false);
			m_isRenderingToTexture = false;
		}

		if( gTextureBufferInfos[idxToUse].pTxtBuffer->SetAsRenderTarget(true) )
		{
			m_isRenderingToTexture = true;

			//Clear(CLEAR_COLOR_AND_DEPTH_BUFFER,0x80808080,1.0f);
			if( frameBufferOptions.bFillRectNextTextureBuffer )
				CGraphicsContext::g_pGraphicsContext->Clear(CLEAR_COLOR_BUFFER,gRDP.fillColor,1.0f);
			else if( options.enableHackForGames == HACK_FOR_MARIO_TENNIS && g_pTxtBufferInfo->N64Width > 64 && g_pTxtBufferInfo->N64Width < 300 )
			{
				CGraphicsContext::g_pGraphicsContext->Clear(CLEAR_COLOR_BUFFER,0,1.0f);
			}
			else if( options.enableHackForGames == HACK_FOR_MARIO_TENNIS && g_pTxtBufferInfo->N64Width < 64 && g_pTxtBufferInfo->N64Width > 32 )
			{
				CGraphicsContext::g_pGraphicsContext->Clear(CLEAR_COLOR_BUFFER,0,1.0f);
			}

			m_curRenderTextureIndex = idxToUse;

			status.bDirectWriteIntoRDRAM = false;

			SetScreenMult(gTextureBufferInfos[m_curRenderTextureIndex].scaleX, gTextureBufferInfos[m_curRenderTextureIndex].scaleY);
			CRender::g_pRender->UpdateClipRectangle();
			MYD3DVIEWPORT vp = {0,0,gTextureBufferInfos[idxToUse].bufferWidth,gTextureBufferInfos[idxToUse].bufferHeight,0,1};
			gD3DDevWrapper.SetViewport(&vp);
		}
	}
	else
	{
		UpdateRecentCIAddr(g_CI);
		CheckRenderTexturesWithNewCI(g_CI,gRDP.scissor.bottom,false);
	}
}

#define SAVE_CI	{g_CI.dwAddr = newCI.dwAddr;g_CI.dwFormat = newCI.dwFormat;g_CI.dwSize = newCI.dwSize;g_CI.dwWidth = newCI.dwWidth;g_CI.bpl=newCI.bpl;}

void FrameBufferManager::Set_CI_addr(SetImgInfo &newCI)
{
	bool wasDrawingTextureBuffer = status.bN64IsDrawingTextureBuffer;
	status.bN64IsDrawingTextureBuffer = ( newCI.dwSize != TXT_SIZE_16b || newCI.dwFormat != TXT_FMT_RGBA || newCI.dwWidth < 200 || ( newCI.dwAddr != g_ZI.dwAddr && newCI.dwWidth != 512 && !g_pFrameBufferManager->HasAddrBeenDisplayed(newCI.dwAddr, newCI.dwWidth)) );
	status.bN64FrameBufferIsUsed = status.bN64IsDrawingTextureBuffer;

	if( !wasDrawingTextureBuffer && g_CI.dwAddr == g_ZI.dwAddr && status.bCIBufferIsRendered )
	{
		if( options.enableHackForGames != HACK_FOR_CONKER && g_uRecentCIInfoPtrs[0]->bCopied == false )
		{
			// Conker is not actually using a backbuffer
			g_pFrameBufferManager->UpdateRecentCIAddr(g_CI);
			if( status.leftRendered != -1 && status.topRendered != -1 && status.rightRendered != -1 && status.bottomRendered != -1 )
			{
				RECT rect={status.leftRendered,status.topRendered,status.rightRendered,status.bottomRendered};
				g_pFrameBufferManager->SaveBackBuffer(0,&rect);
			}
			else
			{
				g_pFrameBufferManager->SaveBackBuffer(0,NULL);
			}
		}
	}

	frameBufferOptions.bFillRectNextTextureBuffer = false;
	if( g_CI.dwAddr == newCI.dwAddr && status.bHandleN64TextureBuffer && (g_CI.dwFormat != newCI.dwFormat || g_CI.dwSize != newCI.dwSize || g_CI.dwWidth != newCI.dwWidth ) )
	{
		// Mario Tennis player shadow
		g_pFrameBufferManager->CloseRenderTexture(true);
		if( options.enableHackForGames == HACK_FOR_MARIO_TENNIS )
			frameBufferOptions.bFillRectNextTextureBuffer = true;	// Hack for Mario Tennis
	}

	SAVE_CI;

	if( g_CI.dwAddr == g_ZI.dwAddr && !status.bN64IsDrawingTextureBuffer )
	{
		if( g_pFrameBufferManager->IsDIaRenderTexture() )
		{
			status.bN64IsDrawingTextureBuffer = true;
			status.bN64FrameBufferIsUsed = status.bN64IsDrawingTextureBuffer;
		}
	}

	status.bCIBufferIsRendered = false;
	status.leftRendered = status.topRendered = status.rightRendered = status.bottomRendered = -1;

	if( currentRomOptions.screenUpdateSetting==SCREEN_UPDATE_AT_CI_CHANGE && !status.bN64IsDrawingTextureBuffer )
	{
		if( status.curRenderBuffer == NULL )
		{
			status.curRenderBuffer = g_CI.dwAddr;
		}
		else if( status.curRenderBuffer != g_CI.dwAddr )
		{
			status.curDisplayBuffer = status.curRenderBuffer;
			CGraphicsContext::Get()->UpdateFrame();
			status.curRenderBuffer = g_CI.dwAddr;
		}
	}

	if( frameBufferOptions.bAtEachFrameUpdate && !status.bHandleN64TextureBuffer )
	{
		if( status.curRenderBuffer != g_CI.dwAddr )
		{
			if( status.gDlistCount%(currentRomOptions.N64FrameBufferWriteBackControl+1) == 0 )
			{
				g_pFrameBufferManager->StoreBackBufferToRDRAM(status.curRenderBuffer, 
					newCI.dwFormat, newCI.dwSize, windowSetting.uViWidth, windowSetting.uViHeight,
					windowSetting.uDisplayWidth, windowSetting.uDisplayHeight);
			}
		}

		//status.curDisplayBuffer = status.curRenderBuffer;
		status.curRenderBuffer = g_CI.dwAddr;
	}


	switch( currentRomOptions.N64RenderToTextureEmuType )
	{
	case TXT_BUF_NONE:
		if( status.bHandleN64TextureBuffer )
			g_pFrameBufferManager->CloseRenderTexture(false);
		status.bHandleN64TextureBuffer = false;	// Don't handle N64 render_texture stuffs
		if( !status.bN64IsDrawingTextureBuffer )
			g_pFrameBufferManager->UpdateRecentCIAddr(g_CI);
		break;
	default:
		if( status.bHandleN64TextureBuffer )
		{
#ifdef _DEBUG
			if( pauseAtNext && eventToPause == NEXT_RENDER_TEXTURE )
			{
				pauseAtNext = TRUE;
				eventToPause = NEXT_RENDER_TEXTURE;
			}
#endif
			g_pFrameBufferManager->CloseRenderTexture(true);
		}

		status.bHandleN64TextureBuffer = status.bN64IsDrawingTextureBuffer;
		if( status.bHandleN64TextureBuffer )
		{
			if( options.enableHackForGames != HACK_FOR_BANJO_TOOIE )
			{
				g_pFrameBufferManager->SetRenderTexture();
			}
		}
		else
		{
#ifdef _DEBUG
			if( g_CI.dwWidth == 512 && pauseAtNext && (eventToPause==NEXT_OBJ_BG || eventToPause==NEXT_SET_CIMG) )
			{
				DebuggerAppendMsg("Warning SetCImg: new Addr=0x%08X, fmt:%s size=%sb, Width=%d\n", 
					g_CI.dwAddr, pszImgFormat[newCI.dwFormat], pszImgSize[newCI.dwSize], newCI.dwWidth);
			}
#endif
			//g_pFrameBufferManager->UpdateRecentCIAddr(g_CI);		// Delay this until the CI buffer is actally drawn
		}
		break;
	}
}


void FrameBufferManager::StoreRenderTextureToRDRAM(int infoIdx)
{
	if( !frameBufferOptions.bTxtBufWriteBack )	return;

	if( infoIdx < 0 )
		infoIdx = m_lastTextureBufferIndex;

	if( !gTextureBufferInfos[infoIdx].pTxtBuffer )
		return;

	if( gTextureBufferInfos[infoIdx].pTxtBuffer->IsBeingRendered() )
	{
		return;
	}

	gTextureBufferInfos[infoIdx].pTxtBuffer->StoreTextureBufferToRDRAM(infoIdx);
}


void FrameBufferManager::CopyBufferToRDRAM(uint32 addr, uint32 fmt, uint32 siz, uint32 width, uint32 height, uint32 bufWidth, uint32 bufHeight, uint32 startaddr, uint32 memsize, uint32 pitch, TextureFmt bufFmt, void *buffer, uint32 bufPitch)
{
	uint32 startline=0;
	if( startaddr == 0xFFFFFFFF )	startaddr = addr;

	startline = (startaddr-addr)/siz/pitch;
	if( startline >= height )
	{
		//TRACE0("Warning: check me");
		startline = height;
	}

	uint32 endline = height;
	if( memsize != 0xFFFFFFFF )
	{
		endline = (startaddr+memsize-addr)/siz;
		if( endline % pitch == 0 )
			endline /= pitch;
		else
			endline = endline/pitch+1;
	}
	if( endline > height )
	{
		endline = height;
	}

	int indexes[600];
	{
		float sx;
		int sx0;
		float ratio = bufWidth/(float)width;
		for( uint32 j=0; j<width; j++ )
		{
			sx = j*ratio;
			sx0 = int(sx+0.5);
			indexes[j] = 4*sx0;
		}
	}

	if( siz == TXT_SIZE_16b )
	{
		uint16 *frameBufferBase = (uint16*)(g_pRDRAMu8+addr);

		if( bufFmt==TEXTURE_FMT_A8R8G8B8 )
		{
			int  sy0;
			float ratio = bufHeight/(float)height;

			for( uint32 i=startline; i<endline; i++ )
			{
				sy0 = int(i*ratio+0.5);

				uint16 *pD = frameBufferBase + i * pitch;
				uint8 *pS0 = (uint8 *)buffer + sy0 * bufPitch;
				char arr[0x12C000];
				int iterate = 4;

				for( uint32 j=0; j<width; j++ )
				{
					// Point
					//uint8 r = pS0[indexes[j]+2];
					//uint8 g = pS0[indexes[j]+1];
					//uint8 b = pS0[indexes[j]+0];
					//uint8 a = pS0[indexes[j]+3];
					// Liner
					//*(pD+(j^1)) = ConvertRGBATo555( r, g, b, a);
					uint32 rgba = *(uint32 *)&pS0[indexes[j]];
					//uint32 rgba2 = *(uint32 *)&pS0[indexes[j+1]];
					//uint32 rgba3 = *(uint32 *)&pS0[indexes[j+2]];
					//uint32 rgba4 = *(uint32 *)&pS0[indexes[j+3]];

					//*(pD+(j^1)) = _mm_set_ps(ConvertRGBATo555( rgba1),ConvertRGBATo555( rgba2),ConvertRGBATo555( rgba3),ConvertRGBATo555( rgba4));
					//__m128 _mm_store_ps(float * p, __m128 a);
					*(pD+(j^1)) = ConvertRGBATo555( rgba);


				}
			}
		}
	}
	else if( siz == TXT_SIZE_8b && fmt == TXT_FMT_CI )
	{
		uint8 *frameBufferBase = (uint8*)(g_pRDRAMu8+addr);

		if( bufFmt==TEXTURE_FMT_A8R8G8B8 )
		{
			uint16 tempword;
			InitTlutReverseLookup();

			for( uint32 i=startline; i<endline; i++ )
			{
				uint8 *pD = frameBufferBase + i * width;
				uint8 *pS = (uint8 *)buffer + i*bufHeight/height * bufPitch;
				for( uint32 j=0; j<width; j++ )
				{
					int pos = 4*(j*bufWidth/width);
					tempword = ConvertRGBATo555((pS[pos+2]),		// Red
						(pS[pos+1]),		// G
						(pS[pos+0]),		// B
						(pS[pos+3]));		// Alpha
					//*pD = CIFindIndex(tempword);
					*(pD+(j^3)) = RevTlutTable[tempword];
				}
			}
		}
	}
	else if( siz == TXT_SIZE_8b && fmt == TXT_FMT_I )
	{
		uint8 *frameBufferBase = (uint8*)(g_pRDRAMu8+addr);

		if( bufFmt==TEXTURE_FMT_A8R8G8B8 )
		{
			int sy0;
			float ratio = bufHeight/(float)height;

			for( uint32 i=startline; i<endline; i++ )
			{
				sy0 = int(i*ratio+0.5);

				uint8 *pD = frameBufferBase + i * width;
				uint8 *pS0 = (uint8 *)buffer + sy0 * bufPitch;

				for( uint32 j=0; j<width; j++ )
				{
					// Point
					uint32 r = pS0[indexes[j]+2];
					uint32 g = pS0[indexes[j]+1];
					uint32 b = pS0[indexes[j]+0];

					// Liner
					*(pD+(j^3)) = (uint8)((r+b+g)/3);

				}
			}
		}
	}
}


#ifdef _DEBUG
void FrameBufferManager::DisplayRenderTexture(int infoIdx)
{
	if( infoIdx < 0 )
		infoIdx = m_lastTextureBufferIndex;

	if( gTextureBufferInfos[infoIdx].pTxtBuffer )
	{
		if( gTextureBufferInfos[infoIdx].pTxtBuffer->IsBeingRendered() )
		{
			TRACE1("Render texture %d is being rendered, cannot display", infoIdx);
		}
		else
		{
			gTextureBufferInfos[infoIdx].pTxtBuffer->Display(infoIdx);
			TRACE1("Texture buffer %d:", infoIdx);
			TRACE1("Addr=%08X", gTextureBufferInfos[infoIdx].CI_Info.dwAddr);
			TRACE2("Width=%d, Created Height=%d", gTextureBufferInfos[infoIdx].N64Width,gTextureBufferInfos[infoIdx].N64Height);
			TRACE2("Fmt=%d, Size=%d", gTextureBufferInfos[infoIdx].CI_Info.dwFormat,gTextureBufferInfos[infoIdx].CI_Info.dwSize);
		}
	}
	else
	{
		TRACE1("Texture buffer %d is not used", infoIdx);
	}
}
#endif



void FrameBufferManager::SaveBackBuffer(int ciInfoIdx, RECT* pSrcRect, bool forceToSaveToRDRAM)
{
	RecentCIInfo &ciInfo = *g_uRecentCIInfoPtrs[ciInfoIdx];

	if( ciInfoIdx == 1 )	// to save the current front buffer
	{
		CGraphicsContext::g_pGraphicsContext->UpdateFrame(true);
	}

	if( frameBufferOptions.bWriteBackBufToRDRAM || forceToSaveToRDRAM )
	{
		uint32 width = ciInfo.dwWidth;
		uint32 height = ciInfo.dwHeight;

		if( ciInfo.dwWidth == *g_GraphicsInfo.VI_WIDTH_REG && ciInfo.dwWidth != windowSetting.uViWidth )
		{
			width = windowSetting.uViWidth;
			height = windowSetting.uViHeight;
		}

		StoreBackBufferToRDRAM( ciInfo.dwAddr, ciInfo.dwFormat, ciInfo.dwSize, width, height, 
			windowSetting.uDisplayWidth, windowSetting.uDisplayHeight);

		g_uRecentCIInfoPtrs[ciInfoIdx]->bCopied = true;
		if( ciInfoIdx == 1 )	// to save the current front buffer
		{
			CGraphicsContext::g_pGraphicsContext->UpdateFrame(true);
		}
		return;
	}


	SetImgInfo tempinfo;
	tempinfo.dwAddr = ciInfo.dwAddr;
	tempinfo.dwFormat = ciInfo.dwFormat;
	tempinfo.dwSize = ciInfo.dwSize;
	tempinfo.dwWidth = ciInfo.dwWidth;

	int idx = SetBackBufferAsRenderTexture(tempinfo, ciInfoIdx);

	CopyBackBufferToRenderTexture(idx, ciInfo, pSrcRect);

	gTextureBufferInfos[idx].crcCheckedAtFrame = status.gDlistCount;
	gTextureBufferInfos[idx].crcInRDRAM = ComputeRenderTextureCRCInRDRAM(idx);

	g_uRecentCIInfoPtrs[ciInfoIdx]->bCopied = true;
}
#endif