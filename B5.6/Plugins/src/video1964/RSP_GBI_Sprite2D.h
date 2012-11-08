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

// Sprite2D Ucodes


Sprite2DInfo g_Sprite2DInfo;
uint32 g_SavedUcode=1;
 
void RSP_GBI_Sprite2DBase(Gfx *gfx)
{
	
	

	uint32 dwAddr = RSPSegmentAddr((gfx->words.w1));
	dwAddr &= (g_dwRamSize-1);

	g_Sprite2DInfo.spritePtr = (SpriteStruct *)(g_pRDRAMs8+dwAddr);

}

typedef struct{
	uint32 SourceImagePointer;
	uint32 TlutPointer;

	short SubImageWidth;
	short Stride;

	char  SourceImageBitSize;
	char  SourceImageType;
	short SubImageHeight;

	short scaleY;
	short scaleX;

	short SourceImageOffsetS;
	char  dummy1[2]; 

	short px;
	short SourceImageOffsetT;

	char  dummy2[2]; 
	short py;

} PuzzleMasterSprite;

void RSP_GBI_Sprite2D_PuzzleMaster64(Gfx *gfx)
{
	
	

	uint32 dwAddr = RSPSegmentAddr((gfx->words.w1));
	dwAddr &= (g_dwRamSize-1);

	g_Sprite2DInfo.spritePtr = (SpriteStruct *)(g_pRDRAMs8+dwAddr);

	g_Sprite2DInfo.flipX = 0;
	g_Sprite2DInfo.flipY = 0;
	g_Sprite2DInfo.px = 0;
	g_Sprite2DInfo.py = 0;

	SpriteStruct tempInfo;
	memcpy(&tempInfo, g_Sprite2DInfo.spritePtr, sizeof(SpriteStruct));
	PuzzleMasterSprite info;
	memcpy(&info, g_Sprite2DInfo.spritePtr, sizeof(PuzzleMasterSprite));

	g_Sprite2DInfo.px = info.px>>2;
	g_Sprite2DInfo.py = info.py>>2;
	g_Sprite2DInfo.scaleX = info.scaleX / 1024.0f;
	g_Sprite2DInfo.scaleY = info.scaleY / 1024.0f;

	tempInfo.SourceImageOffsetS = info.SourceImageOffsetS;
	tempInfo.SourceImageOffsetT = info.SourceImageOffsetT;
	g_Sprite2DInfo.spritePtr = &tempInfo;

	CRender::g_pRender->DrawSprite2D(g_Sprite2DInfo, 1);
}


void RSP_GBI1_Sprite2DDraw(Gfx *gfx)
{
	
	

	// This ucode is shared by PopMtx and gSPSprite2DDraw
	g_Sprite2DInfo.px = (short)(((gfx->words.w1)>>16)&0xFFFF)/4;
	g_Sprite2DInfo.py = (short)((gfx->words.w1)&0xFFFF)/4;

	CRender::g_pRender->DrawSprite2D(g_Sprite2DInfo, 1);

	LoadedUcodeMap[RSP_SPRITE2D_SCALEFLIP] = &RSP_GBI1_CullDL;
	LoadedUcodeMap[RSP_SPRITE2D_DRAW] = &RSP_GBI1_PopMtx;
	LoadedUcodeMap[RSP_SPRITE2D_BASE] = &RSP_GBI1_Sprite2DBase;

}

void RSP_GBI0_Sprite2DDraw(Gfx *gfx)
{
	
	

	// This ucode is shared by PopMtx and gSPSprite2DDraw
	g_Sprite2DInfo.px = (short)(((gfx->words.w1)>>16)&0xFFFF)/4;
	g_Sprite2DInfo.py = (short)((gfx->words.w1)&0xFFFF)/4;

	CRender::g_pRender->DrawSprite2D(g_Sprite2DInfo, 0);
}


void RSP_GBI1_Sprite2DScaleFlip(Gfx *gfx)
{

	

	g_Sprite2DInfo.scaleX = (((gfx->words.w1)>>16)&0xFFFF)/1024.0f;
	g_Sprite2DInfo.scaleY = ((gfx->words.w1)&0xFFFF)/1024.0f;

	if( ((gfx->words.w1)&0xFFFF) < 0x100 )
	{
		g_Sprite2DInfo.scaleY = g_Sprite2DInfo.scaleX;
	}

	g_Sprite2DInfo.flipX = (uint8)(((gfx->words.w0)>>8)&0xFF);
	g_Sprite2DInfo.flipY = (uint8)((gfx->words.w0)&0xFF);
}



void RSP_GBI1_Sprite2DBase(Gfx *gfx)
{
	
	

	if( !status.bUseModifiedUcodeMap )
	{
		memcpy( &LoadedUcodeMap, &ucodeMap1, sizeof(UcodeMap));
		status.bUseModifiedUcodeMap = true;
	}

	LoadedUcodeMap[RSP_SPRITE2D_BASE] = &RSP_GBI_Sprite2DBase;
	LoadedUcodeMap[RSP_SPRITE2D_SCALEFLIP] = &RSP_GBI1_Sprite2DScaleFlip;
	LoadedUcodeMap[RSP_SPRITE2D_DRAW] = &RSP_GBI1_Sprite2DDraw;

	RSP_GBI_Sprite2DBase(gfx);
}



void RSP_GBI0_Sprite2DBase(Gfx *gfx)
{
	
	

	//Weired, this ucode 0 game is using ucode 1, but sprite2D cmd is working differently from
	//normal ucode1 sprite2D game

	RSP_GBI_Sprite2DBase(gfx);
}

