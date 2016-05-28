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
/*
void RSP_GBI1_SpNoop(Gfx *gfx)
{
	SP_Timing(RSP_GBI1_SpNoop);

	if( (gfx+1)->words.cmd == 0x00 && gRSP.ucode >= 17 )
	{
		RDP_GFX_PopDL();
	}
}
*/
void RSP_GBI0_Mtx(Gfx *gfx)
{	
	SP_Timing(RSP_GBI0_Mtx);

	uint32 addr = RSPSegmentAddr((gfx->mtx1.addr));

	if (addr + 64 > g_dwRamSize)
	{
		return;
	}

	LoadMatrix(addr);
	
	if (gfx->mtx1.projection)
	{
		CRender::g_pRender->SetProjection(matToLoad, gfx->mtx1.push, gfx->mtx1.load);
	}
	else
	{
		CRender::g_pRender->SetWorldView(matToLoad, gfx->mtx1.push, gfx->mtx1.load);
	}
}






void RSP_GBI0_Vtx(Gfx *gfx)
{
	SP_Timing(RSP_GBI0_Vtx);

	int n = gfx->vtx0.n + 1;
	int v0 = gfx->vtx0.v0;
	uint32 addr = RSPSegmentAddr((gfx->vtx0.addr));

	if ((v0 + n) > 80)
	{
		n = 32 - v0;
	}

	// Check that address is valid...
	if ((addr + n*16) > g_dwRamSize)
	{
	}
	else
	{
		ProcessVertexData(addr, v0, n);
		status.dwNumVertices += n;
		DisplayVertexInfo(addr, v0, n);
	}
}


void RSP_GBI0_DL(Gfx *gfx)
{	
	SP_Timing(RSP_GBI0_DL);

	uint32 addr = RSPSegmentAddr((gfx->dlist.addr)) & (g_dwRamSize-1);

	if( addr > g_dwRamSize )
	{
		addr &= (g_dwRamSize-1);
	}

	if( gfx->dlist.param == RSP_DLIST_PUSH )
		gDlistStackPointer++;

	gDlistStack[gDlistStackPointer].pc = addr;
	gDlistStack[gDlistStackPointer].countdown = MAX_DL_COUNT;

}



void RSP_GBI0_Tri4(Gfx *gfx)
{
	uint32 w0 = gfx->words.cmd0;
	uint32 w1 = gfx->words.cmd1;

	status.primitiveType = PRIM_TRI2;

	// While the next command pair is Tri2, add vertices
	uint32 dwPC = gDlistStack[gDlistStackPointer].pc;

	BOOL bTrisAdded = FALSE;

	do {
		uint32 dwFlag = (w0>>16)&0xFF;

		BOOL bVisible;
		for( int i=0; i<4; i++)
		{
			uint32 v0 = (w1>>(4+(i<<3))) & 0xF;
			uint32 v1 = (w1>>(  (i<<3))) & 0xF;
			uint32 v2 = (w0>>(  (i<<2))) & 0xF;
			bVisible = IsTriangleVisible(v0, v2, v1);
			if (bVisible)
			{
				if (!bTrisAdded && CRender::g_pRender->IsTextureEnabled())
				{
					PrepareTextures();
					InitVertexTextureConstants();
				}

				if( !bTrisAdded )
				{
					CRender::g_pRender->SetCombinerAndBlender();
				}

				bTrisAdded = true;
				PrepareTriangle(v0, v2, v1);
			}
		}
		
		w0			= *(uint32 *)(g_pRDRAMu8 + dwPC+0);
		w1			= *(uint32 *)(g_pRDRAMu8 + dwPC+4);
		dwPC += 8;

	} while (((w0)>>24) == (uint8)RSP_TRI2);


	gDlistStack[gDlistStackPointer].pc = dwPC-8;


	if (bTrisAdded)	
	{
		CRender::g_pRender->DrawTriangles();
	}
	
}

#ifndef _RICE6IM
void RSP_RDP_InsertMatrix(Gfx *gfx)
{
	 UpdateCombinedMatrix();

    int x = ((gfx->words.cmd0) & 0x1F) >> 1;
    int y = x >> 2;
    x &= 3;

	if ((gfx->words.cmd0) & 0x20)
    {
        gRSPworldProject.m[y][x]   = (float)(int)gRSPworldProject.m[y][x] + ((float)(gfx->words.cmd1 >> 16) / 65536.0f);
        gRSPworldProject.m[y][x+1] = (float)(int)gRSPworldProject.m[y][x+1] + ((float)(gfx->words.cmd1 & 0xFFFF) / 65536.0f);
    }
    else
    {
        gRSPworldProject.m[y][x] = (float)(short)(gfx->words.cmd1 >> 16);
        gRSPworldProject.m[y][x+1] = (float)(short)(gfx->words.cmd1 & 0xFFFF);
    }

    gRSP.bMatrixIsUpdated = false;
    gRSP.bCombinedMatrixIsUpdated = true;

}
#endif