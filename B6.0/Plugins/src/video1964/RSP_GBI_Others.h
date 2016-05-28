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

// A few ucode used in DKR and Others Special games

uint32 GSBlkAddrSaves[100][2];

void RDP_GFX_DLInMem(Gfx *gfx)
{
	uint32 dwLimit = ((gfx->words.cmd0) >> 16) & 0xFF;
	uint32 dwPush = RSP_DLIST_PUSH; //((gfx->words.cmd0) >> 16) & 0xFF;
	uint32 dwAddr = 0x00000000 | (gfx->words.cmd1); //RSPSegmentAddr((gfx->words.cmd1));

	switch (dwPush)
	{
	case RSP_DLIST_PUSH:
		gDlistStackPointer++;
		gDlistStack[gDlistStackPointer].pc = dwAddr;
		gDlistStack[gDlistStackPointer].countdown = dwLimit;

		break;
	case RSP_DLIST_NOPUSH:
		gDlistStack[gDlistStackPointer].pc = dwAddr;
		gDlistStack[gDlistStackPointer].countdown = dwLimit;
		break;
	}

}


uint16 ConvertYUVtoR5G5B5X1(int y, int u, int v)
{
	float r = y + (1.370705f * (v-128));
	float g = y - (0.698001f * (v-128)) - (0.337633f * (u-128));
	float b = y + (1.732446f * (u-128));
	r *= 0.125f;
	g *= 0.125f;
	b *= 0.125f;

	//clipping the result
	if (r > 32) r = 32;
	if (g > 32) g = 32;
	if (b > 32) b = 32;
	if (r < 0) r = 0;
	if (g < 0) g = 0;
	if (b < 0) b = 0;

	uint16 c = (uint16)(((uint16)(r) << 11) |
		((uint16)(g) << 6) |
		((uint16)(b) << 1) | 1);
	return c;
}

void TexRectToN64FrameBuffer_YUV_16b(uint32 x0, uint32 y0, uint32 width, uint32 height)
{
	// Convert YUV image at TImg and Copy the texture into the N64 RDRAM framebuffer memory

	uint32 n64CIaddr = g_CI.dwAddr;
	uint32 n64CIwidth = g_CI.dwWidth;

	for (uint32 y = 0; y < height; y++)
	{
		uint32* pN64Src = (uint32*)(g_pRDRAMu8+(g_TI.dwAddr&(g_dwRamSize-1)))+y*(g_TI.dwWidth>>1);
		uint16* pN64Dst = (uint16*)(g_pRDRAMu8+(n64CIaddr&(g_dwRamSize-1)))+(y+y0)*n64CIwidth;

		for (uint32 x = 0; x < width; x+=2)
		{
			uint32 val = *pN64Src++;
			int y0 = (uint8)val&0xFF;
			int v  = (uint8)(val>>8)&0xFF;
			int y1 = (uint8)(val>>16)&0xFF;
			int u  = (uint8)(val>>24)&0xFF;

			pN64Dst[x+x0] = ConvertYUVtoR5G5B5X1(y0,u,v);
			pN64Dst[x+x0+1] = ConvertYUVtoR5G5B5X1(y1,u,v);
		}
	}
}

extern uObjMtxReal gObjMtxReal;
void DLParser_OgreBatter64BG(Gfx *gfx)
{
	uint32 dwAddr = RSPSegmentAddr((gfx->words.cmd1));
	uObjTxSprite *ptr = (uObjTxSprite*)(g_pRDRAMu8+dwAddr);
	//CRender::g_pRender->LoadObjSprite(*ptr,true);
	PrepareTextures();

	CTexture *ptexture = g_textures[0].m_pCTexture;
	TexRectToN64FrameBuffer_16b( (uint32)gObjMtxReal.X, (uint32)gObjMtxReal.Y, ptexture->m_dwWidth, ptexture->m_dwHeight, gRSP.curTile);

}

void DLParser_Bomberman2TextRect(Gfx *gfx)
{
	// Bomberman 64 - The Second Attack! (U) [!]
	// The 0x02 cmd, list a TexRect cmd

	if( options.enableHackForGames == HACK_FOR_OGRE_BATTLE && gRDP.tiles[7].dwFormat == TXT_FMT_YUV )
	{
		TexRectToN64FrameBuffer_YUV_16b( (uint32)gObjMtxReal.X, (uint32)gObjMtxReal.Y, 16, 16);
		//DLParser_OgreBatter64BG((gfx->words.cmd0), (gfx->words.cmd1));
		return;
	}

	uint32 dwAddr = RSPSegmentAddr((gfx->words.cmd1));
	uObjSprite *info = (uObjSprite*)(g_pRDRAMu8+dwAddr);

	uint32 dwTile	= gRSP.curTile;

	PrepareTextures();
	
	//CRender::g_pRender->SetCombinerAndBlender();

	uObjTxSprite drawinfo;
	memcpy( &(drawinfo.sprite), info, sizeof(uObjSprite));
	CRender::g_pRender->DrawSpriteR(drawinfo, false, dwTile, 0, 0, drawinfo.sprite.imageW/32, drawinfo.sprite.imageH/32);
}


void DLParser_Ucode8_0x0(Gfx *gfx)
{
	if( (gfx->words.cmd0) == 0 && (gfx->words.cmd1) )
	{
		uint32 newaddr = RSPSegmentAddr((gfx->words.cmd1));

		if( newaddr && newaddr < g_dwRamSize)
		{
			if( gDlistStackPointer < MAX_DL_STACK_SIZE-1 )
			{
				gDlistStackPointer++;
				gDlistStack[gDlistStackPointer].pc = newaddr+8;	// Always skip the first 2 entries
				gDlistStack[gDlistStackPointer].countdown = MAX_DL_COUNT;
			}
			else
			{
			}
		}
	}
	else
	{
		gDlistStack[gDlistStackPointer].pc += 8;
	}
}


void DLParser_Ucode8_EndDL(Gfx *gfx)
{
	
	

	uint32 dwPC = gDlistStack[gDlistStackPointer].pc-8;
	RDP_GFX_PopDL();
}

void DLParser_Ucode8_DL(Gfx *gfx)	// DL Function Call
{

	

	uint32 dwPC = gDlistStack[gDlistStackPointer].pc-8;

	uint32 dwAddr = RSPSegmentAddr((gfx->words.cmd1));
	uint32 dwCmd2 = *(uint32 *)(g_pRDRAMu8 + dwAddr);
	uint32 dwCmd3 = *(uint32 *)(g_pRDRAMu8 + dwAddr+4);

	if( dwAddr > g_dwRamSize )
	{
		dwAddr = (gfx->words.cmd1)&(g_dwRamSize-1);
	}

	if( gDlistStackPointer < MAX_DL_STACK_SIZE-1 )
	{
		gDlistStackPointer++;
		gDlistStack[gDlistStackPointer].pc = dwAddr+16;
		gDlistStack[gDlistStackPointer].countdown = MAX_DL_COUNT;
	}
	else
	{
		RDP_GFX_PopDL();
	}

	GSBlkAddrSaves[gDlistStackPointer][0]=GSBlkAddrSaves[gDlistStackPointer][1]=0;
	if( (dwCmd2>>24) == 0x80 )
	{
		GSBlkAddrSaves[gDlistStackPointer][0] = dwCmd2;
		GSBlkAddrSaves[gDlistStackPointer][1] = dwCmd3;
	}

}

void DLParser_Ucode8_JUMP(Gfx *gfx)	// DL Function Call
{
	
	

	if( ((gfx->words.cmd0)&0x00FFFFFF) == 0 )
	{
		uint32 dwPC = gDlistStack[gDlistStackPointer].pc-8;
		uint32 dwAddr = RSPSegmentAddr((gfx->words.cmd1));

		if( dwAddr > g_dwRamSize )
		{
			dwAddr = (gfx->words.cmd1)&(g_dwRamSize-1);
		}

		uint32 dwCmd2 = *(uint32 *)(g_pRDRAMu8 + dwAddr);
		uint32 dwCmd3 = *(uint32 *)(g_pRDRAMu8 + dwAddr+4);

		gDlistStack[gDlistStackPointer].pc = dwAddr+8;		// Jump to new address
	}
	else
	{
		uint32 dwPC = gDlistStack[gDlistStackPointer].pc-8;
	}
}



void DLParser_Ucode8_Unknown(Gfx *gfx)
{
	uint32 dwPC = gDlistStack[gDlistStackPointer].pc-8;
}

void DLParser_Unknown_Skip1(Gfx *gfx)
{
	uint32 dwPC = gDlistStack[gDlistStackPointer].pc-8;
	gfx++;
	dwPC+=8;
	gfx++;
	gDlistStack[gDlistStackPointer].pc += 8;
}

void DLParser_Unknown_Skip2(Gfx *gfx)
{
	uint32 dwPC = gDlistStack[gDlistStackPointer].pc-8;
	gfx++;
	dwPC+=8;
	gfx++;
	dwPC+=8;
	gfx++;
	gDlistStack[gDlistStackPointer].pc += 16;
}

void DLParser_Unknown_Skip3(Gfx *gfx)
{
	uint32 dwPC = gDlistStack[gDlistStackPointer].pc-8;
	gfx++;
	dwPC+=8;
	gfx++;
	dwPC+=8;
	gfx++;
	dwPC+=8;
	gfx++;
	gDlistStack[gDlistStackPointer].pc += 24;
}

void DLParser_Unknown_Skip4(Gfx *gfx)
{
	uint32 dwPC = gDlistStack[gDlistStackPointer].pc-8;
	gfx++;
	dwPC+=8;
	gfx++;
	dwPC+=8;
	gfx++;
	dwPC+=8;
	gfx++;
	dwPC+=8;
	gfx++;
	gDlistStack[gDlistStackPointer].pc += 32;
}

void DLParser_Ucode8_0x05(Gfx *gfx)
{
	// Be careful, 0x05 is variable length ucode
	/*
	0028E4E0: 05020088, 04D0000F - Reserved1
	0028E4E8: 6BDC0306, 00000000 - G_NOTHING
	0028E4F0: 05010130, 01B0000F - Reserved1
	0028E4F8: 918A01CA, 1EC5FF3B - G_NOTHING
	0028E500: 05088C68, F5021809 - Reserved1
	0028E508: 04000405, 00000000 - RSP_VTX
	0028E510: 102ECE60, 202F2AA0 - G_NOTHING
	0028E518: 05088C90, F5021609 - Reserved1
	0028E520: 04050405, F0F0F0F0 - RSP_VTX
	0028E528: 102ED0C0, 202F2D00 - G_NOTHING
	0028E530: B5000000, 00000000 - RSP_LINE3D
	0028E538: 8028E640, 8028E430 - G_NOTHING
	0028E540: 00000000, 00000000 - RSP_SPNOOP
	*/

	if( (gfx->words.cmd1) == 0 )
	{
		return;
	}
	else
	{
		DLParser_Unknown_Skip4(gfx);
	}
}

void DLParser_Ucode8_0xb4(Gfx *gfx)
{
	uint32 dwPC = gDlistStack[gDlistStackPointer].pc;

	if( ((gfx->words.cmd0)&0xFF) == 0x06 )
		DLParser_Unknown_Skip3(gfx);
	else if( ((gfx->words.cmd0)&0xFF) == 0x04 )
		DLParser_Unknown_Skip1(gfx);
	else if( ((gfx->words.cmd0)&0xFFF) == 0x600 )
		DLParser_Unknown_Skip3(gfx);
	else
	{
		DLParser_Unknown_Skip3(gfx);
	}
}

void DLParser_Ucode8_0xb5(Gfx *gfx)
{
	uint32 dwPC = gDlistStack[gDlistStackPointer].pc-8;

	uint32 dwCmd2 = *(uint32 *)(g_pRDRAMu8 + dwPC+8);
	uint32 dwCmd3 = *(uint32 *)(g_pRDRAMu8 + dwPC+12);

	//if( dwCmd2 == 0 && dwCmd3 == 0 )
	{
		DLParser_Ucode8_EndDL(gfx);	// Check me
		return;
	}

	gDlistStack[gDlistStackPointer].pc += 8;
	return;


	if( GSBlkAddrSaves[gDlistStackPointer][0] == 0 || GSBlkAddrSaves[gDlistStackPointer][1] == 0 )
	{
		DLParser_Ucode8_EndDL(gfx);	// Check me
		return;
	}

	if( ((dwCmd2>>24)!=0x80 && (dwCmd2>>24)!=0x00 ) || ((dwCmd3>>24)!=0x80 && (dwCmd3>>24)!=0x00 ) )
	{
		DLParser_Ucode8_EndDL(gfx);	// Check me
		return;
	}

	if( (dwCmd2>>24)!= (dwCmd3>>24) )
	{
		DLParser_Ucode8_EndDL(gfx);	// Check me
		return;
	}


	if( (dwCmd2>>24)==0x80 && (dwCmd3>>24)==0x80 )
	{
		if( dwCmd2 < dwCmd3  )
		{
			// All right, the next block is not ucode, but data
			DLParser_Ucode8_EndDL(gfx);	// Check me
			return;
		}

		uint32 dwCmd4 = *(uint32 *)(g_pRDRAMu8 + (dwCmd2&0x00FFFFFF));
		uint32 dwCmd5 = *(uint32 *)(g_pRDRAMu8 + (dwCmd2&0x00FFFFFF)+4);
		uint32 dwCmd6 = *(uint32 *)(g_pRDRAMu8 + (dwCmd3&0x00FFFFFF));
		uint32 dwCmd7 = *(uint32 *)(g_pRDRAMu8 + (dwCmd3&0x00FFFFFF)+4);
		if( (dwCmd4>>24) != 0x80 || (dwCmd5>>24) != 0x80 || (dwCmd6>>24) != 0x80 || (dwCmd7>>24) != 0x80 || dwCmd4 < dwCmd5 || dwCmd6 < dwCmd7 )
		{
			// All right, the next block is not ucode, but data
			DLParser_Ucode8_EndDL(gfx);	// Check me
			return;
		}

		gDlistStack[gDlistStackPointer].pc += 8;
		return;
	}
	else if( (dwCmd2>>24)==0x00 && (dwCmd3>>24)==0x00 )
	{
		DLParser_Ucode8_EndDL(gfx);	// Check me
		return;
	}
	else if( (dwCmd2>>24)==0x00 && (dwCmd3>>24)==0x00 )
	{
		dwCmd2 = *(uint32 *)(g_pRDRAMu8 + dwPC+16);
		dwCmd3 = *(uint32 *)(g_pRDRAMu8 + dwPC+20);
		if( (dwCmd2>>24)==0x80 && (dwCmd3>>24)==0x80 && dwCmd2 < dwCmd3 )
		{
			// All right, the next block is not ucode, but data
			DLParser_Ucode8_EndDL(gfx);	// Check me
			return;
		}
		else
		{
			gDlistStack[gDlistStackPointer].pc += 8;
			return;
		}
	}

	uint32 dwAddr1 = RSPSegmentAddr(dwCmd2);
	uint32 dwAddr2 = RSPSegmentAddr(dwCmd3);
	return;
}

void DLParser_Ucode8_0xbc(Gfx *gfx)
{
	
	

	if( ((gfx->words.cmd0)&0xFFF) == 0x58C )
	{
		DLParser_Ucode8_DL(gfx);
	}
	else
	{
		uint32 dwPC = gDlistStack[gDlistStackPointer].pc-8;
	}
}

void DLParser_Ucode8_0xbd(Gfx *gfx)
{
	
	

	/*
	00359A68: BD000000, DB5B0077 - RSP_POPMTX
	00359A70: C8C0A000, 00240024 - RDP_TriFill
	00359A78: 01000100, 00000000 - RSP_MTX
	00359A80: BD000501, DB5B0077 - RSP_POPMTX
	00359A88: C8C0A000, 00240024 - RDP_TriFill
	00359A90: 01000100, 00000000 - RSP_MTX
	00359A98: BD000A02, DB5B0077 - RSP_POPMTX
	00359AA0: C8C0A000, 00240024 - RDP_TriFill
	00359AA8: 01000100, 00000000 - RSP_MTX
	00359AB0: BD000F04, EB6F0087 - RSP_POPMTX
	00359AB8: C8C0A000, 00280028 - RDP_TriFill
	00359AC0: 01000100, 00000000 - RSP_MTX
	00359AC8: BD001403, DB5B0077 - RSP_POPMTX
	00359AD0: C8C0A000, 00240024 - RDP_TriFill
	00359AD8: 01000100, 00000000 - RSP_MTX
	00359AE0: B5000000, 00000000 - RSP_LINE3D
	00359AE8: 1A000000, 16000200 - G_NOTHING
	 */

	if( (gfx->words.cmd1) != 0 )
	{
		DLParser_Unknown_Skip2(gfx);
		return;
	}

	uint32 dwPC = gDlistStack[gDlistStackPointer].pc;
}

void DLParser_Ucode8_0xbf(Gfx *gfx)
{
	
	

	if( ((gfx->words.cmd0)&0xFF) == 0x02 )
		DLParser_Unknown_Skip3(gfx);
	else
		DLParser_Unknown_Skip1(gfx);
}

void PD_LoadMatrix_0xb4(uint32 addr)
{
	int i, j;

	uint32 data[16];
	data[0] =  *(uint32*)(g_pRDRAMu8+addr+4+ 0);
	data[1] =  *(uint32*)(g_pRDRAMu8+addr+4+ 8);
	data[2] =  *(uint32*)(g_pRDRAMu8+addr+4+16);
	data[3] =  *(uint32*)(g_pRDRAMu8+addr+4+24);

	data[8] =  *(uint32*)(g_pRDRAMu8+addr+4+32);
	data[9] =  *(uint32*)(g_pRDRAMu8+addr+4+40);
	data[10] = *(uint32*)(g_pRDRAMu8+addr+4+48);
	data[11] = *(uint32*)(g_pRDRAMu8+addr+4+56);

	data[4] =  *(uint32*)(g_pRDRAMu8+addr+4+ 0+64);
	data[5] =  *(uint32*)(g_pRDRAMu8+addr+4+ 8+64);
	data[6] =  *(uint32*)(g_pRDRAMu8+addr+4+16+64);
	data[7] =  *(uint32*)(g_pRDRAMu8+addr+4+24+64);

	data[12] = *(uint32*)(g_pRDRAMu8+addr+4+32+64);
	data[13] = *(uint32*)(g_pRDRAMu8+addr+4+40+64);
	data[14] = *(uint32*)(g_pRDRAMu8+addr+4+48+64);
	data[15] = *(uint32*)(g_pRDRAMu8+addr+4+56+64);


	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 4; j++) 
		{
			int     hi = *(short *)((BYTE*)data + (((i<<3)+(j<<1)     )^0x2));
			uint16  lo = *(uint16*)((BYTE*)data + (((i<<3)+(j<<1) + 32)^0x2));
			matToLoad.m[i][j] = (float)((hi<<16) | (lo))/ 65536.0f;
		}
	}
}	

void DLParser_RSP_DL_WorldDriver(Gfx *gfx)
{
	uint32 dwAddr = RSPSegmentAddr((gfx->words.cmd1));
	if( dwAddr > g_dwRamSize )
	{
		dwAddr &= (g_dwRamSize-1);
	}

	gDlistStackPointer++;
	gDlistStack[gDlistStackPointer].pc = dwAddr;
	gDlistStack[gDlistStackPointer].countdown = MAX_DL_COUNT;
}

void DLParser_RSP_Pop_DL_WorldDriver(Gfx *gfx)
{
	
	

	RDP_GFX_PopDL();
}


