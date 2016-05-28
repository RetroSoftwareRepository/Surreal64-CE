/*
Copyright (C) 2002-2009 Rice1964

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
#ifndef RSP_PD_H__
#define RSP_PD_H__

uint32 dwPDCIAddr = 0;

void ProcessVertexDataPD(uint32 dwAddr, uint32 dwV0, uint32 dwNum);
void RSP_Vtx_PD(Gfx *gfx)
{
	SP_Timing(RSP_GBI0_Vtx);

	uint32 dwAddr = RSPSegmentAddr((gfx->words.cmd1));
	uint32 dwV0 =  ((gfx->words.cmd0)>>16)&0x0F;
	uint32 dwN  = (((gfx->words.cmd0)>>20)&0x0F)+1;
	uint32 dwLength = ((gfx->words.cmd0))&0xFFFF;

	ProcessVertexDataPD(dwAddr, dwV0, dwN);
	status.dwNumVertices += dwN;
}

void RSP_Set_Vtx_CI_PD(Gfx *gfx)
{
	// Color index buf address
	dwPDCIAddr = RSPSegmentAddr((gfx->words.cmd1));
}

void RSP_Tri4_PD(Gfx *gfx)
{
	uint32 cmd0 = gfx->words.cmd0;
	uint32 cmd1 = gfx->words.cmd1;

	status.primitiveType = PRIM_TRI2;

	// While the next command pair is Tri2, add vertices
	uint32 dwPC = gDlistStack[gDlistStackPointer].pc;

	BOOL bTrisAdded = FALSE;

	do {
		uint32 dwFlag = (cmd0>>16)&0xFF;

		BOOL bVisible;
		for( uint32 i=0; i<4; i++)
		{
			uint32 v0 = (cmd1>>(4+(i<<3))) & 0xF;
			uint32 v1 = (cmd1>>(  (i<<3))) & 0xF;
			uint32 v2 = (cmd0>>(  (i<<2))) & 0xF;
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

		cmd0			= *(uint32 *)(g_pRDRAMu8 + dwPC+0);
		cmd1			= *(uint32 *)(g_pRDRAMu8 + dwPC+4);
		dwPC += 8;

#ifdef _DEBUG
	} while (!(pauseAtNext && eventToPause==NEXT_TRIANGLE) && (cmd0>>24) == (uint8)RSP_TRI2);
#else
	} while ((cmd0>>24) == (uint8)RSP_TRI2);
#endif

	gDlistStack[gDlistStackPointer].pc = dwPC-8;

	if (bTrisAdded)	
	{
		CRender::g_pRender->DrawTriangles();
	}
}

#endif //RSP_PD_H__