/*
Copyright (C) 2003 Rice1964

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

#include "stdafx.h"

extern uObjMtxReal gObjMtxReal;
extern Matrix gD3DMtxReal;
extern Matrix gD3DObjOffset;

static BOOL bSavedZEnabled = TRUE;
static DWORD dwSavedMinFilter = 0;
static DWORD dwSavedMagFilter = 0;


//========================================================================

void D3DRender::DrawSpriteR_Render()	// With Rotation
{
	WORD wIndices[2*3] = {1,0,2, 2,0,3};
	g_pD3DDev->SetVertexShader(RICE_FVF_TLITVERTEX);
	g_pD3DDev->DrawIndexedPrimitiveUP( D3DPT_TRIANGLELIST, 0, 4, 2, wIndices, D3DFMT_INDEX16, m_texRectTVtx, sizeof(TLITVERTEX));
}

void D3DRender::LoadTxtrBufFromRDRAM(void)
{
	LoadTxtrBufIntoTexture();
	LPD3DXSPRITE d3dSprite = InitSpriteDraw();
	if( d3dSprite != NULL )
	{
		D3DXVECTOR2 scales(1, 1);
		D3DXVECTOR2	trans(0,0);
		d3dSprite->Draw(LPDIRECT3DTEXTURE8(g_textures[0].m_lpsTexturePtr), NULL, &scales, NULL, 0, &trans, 0xFFFFFFFF);
#ifdef _DEBUG
		if( pauseAtNext && eventToPause == NEXT_TEXTURE_BUFFER )
		{
			LPDIRECT3DTEXTURE8 pD3DTexture = LPDIRECT3DTEXTURE8(g_textures[0].m_lpsTexturePtr);
			LPDIRECT3DSURFACE8 pD3DSurface = NULL;
			pD3DTexture->GetSurfaceLevel(0,&pD3DSurface);
			((CDXGraphicsContext*)CGraphicsContext::g_pGraphicsContext)->SaveSurfaceToFile(pD3DSurface, 25);
		}
#endif
	}
	FinishSpriteDraw(d3dSprite);
}


#ifdef _DEBUG

void D3DRender::SaveTextureToFile(int tex, bool alphaOnly)
{
	 
}

bool D3DRender::DrawTexture(int tex, bool alphaOnly)
{
	CTexture *pTexture = g_textures[tex].m_pCTexture;
	if( pTexture == NULL )
	{
		TRACE0("Can't draw null texture");
		return false;
	}

	SaveTextureToFile(tex, alphaOnly);	// Save to file instead of draw to screen
										// my Geforce2 crashes a lot if draw to screen
	DebuggerAppendMsg("Texture %d (CurTile:%d): W=%f, H=%f, Real W=%d, H=%d", tex, gRSP.curTile, 
		g_textures[tex].m_fTexWidth, g_textures[tex].m_fTexHeight, g_textures[tex].m_dwTileWidth, g_textures[tex].m_dwTileHeight);
	DebuggerAppendMsg("X scale: %f, Y scale: %f, %s", gRSP.fTexScaleX, gRSP.fTexScaleY, gRSP.bTextureEnabled?"Enabled":"Disabled");
	
	return true;
}

extern void ConvertRGBA16(CTexture *pSurf, WORD *pSrc, DWORD dwPitch,  DWORD dwSrcX, DWORD dwSrcY, DWORD dwWidth, DWORD dwHeight, BOOL bSwapped);
extern void ConvertRGBA16_16(CTexture *pSurf, WORD *pSrc, DWORD dwPitch,  DWORD dwSrcX, DWORD dwSrcY, DWORD dwWidth, DWORD dwHeight, BOOL bSwapped);
extern void ConvertIA16(CTexture *pSurf, WORD *pSrc, DWORD dwPitch,  DWORD dwSrcX, DWORD dwSrcY, DWORD dwWidth, DWORD dwHeight, BOOL bSwapped);
extern void ConvertIA16_16(CTexture *pSurf, WORD *pSrc, DWORD dwPitch,  DWORD dwSrcX, DWORD dwSrcY, DWORD dwWidth, DWORD dwHeight, BOOL bSwapped);

void D3DRender::DrawTlutAsImage(WORD* palAddr)
{
	/*	Fix me, this function does not work
	//For debugger purpose only, will create an 64x64 (16x4)x(16x4) image for current TLUT content
	if( surfTlut == NULL ) return;	//Out of memory

	HRESULT hr;
	TextureFmt dwType;
	dwType = surfTlut->GetSurfaceFormat();

	DWORD dwTLutFmt = gRDP.otherMode.text_tlut <<RSP_SETOTHERMODE_SHIFT_TEXTLUT;
	if(dwTLutFmt == TLUT_FMT_RGBA16)
	{
		if (dwType == TEXTURE_FMT_A8R8G8B8)
			ConvertRGBA16(surfTlut, g_wRDPTlut, 32, 0, 0, 16, 16, 16, 16,	FALSE);
		else
			ConvertRGBA16_16(surfTlut, g_wRDPTlut, 32, 0, 0, 16, 16,	16, 16, FALSE);
	}
	else	//=TLUT_FMT_IA16
	{
		if (dwType == TEXTURE_FMT_A8R8G8B8)
			ConvertIA16(surfTlut, g_wRDPTlut, 32, 0, 0, 16, 16, 16, 16,	FALSE);
		else
			ConvertIA16_16(surfTlut, g_wRDPTlut, 32, 0, 0, 16, 16, 16, 16,	FALSE);
	}

	
	
	// Save ZBuffer state
	BOOL bZEnabled = gRSP.bZBufferEnabled;
	DWORD dwMinFilter = m_D3DCombStages[0].dwMinFilter;
	DWORD dwMagFilter = m_D3DCombStages[0].dwMagFilter;
	TLITVERTEX trv[4];

	ZBufferEnable( FALSE );

	// Scale to Actual texture coords
	// The two cases are to handle the oversized textures hack on voodoos

	D3DSetColorOp( 1, D3DTOP_DISABLE );
	D3DSetAlphaOp( 1, D3DTOP_DISABLE );

	SetD3DRSAlphaBlendEnable(TRUE);
	SetD3DRSAlphaTestEnable(TRUE);
	SetNumStages(0);

	//Alpha = 1
	SetD3DRSSrcBlend( D3DBLEND_ONE);
	SetD3DRSDestBlend(D3DBLEND_ZERO);

	D3DSetColorOp( 0, D3DTOP_SELECTARG1 );
	D3DSetColorArg1( 0, D3DTA_TEXTURE );

	D3DSetAlphaOp( 0, D3DTOP_SELECTARG1 );
	D3DSetAlphaArg1( 0, D3DTA_TEXTURE );
	m_pD3DRender->D3DSetTexture( 0, surfTlut->GetTexture() );

	trv[0].x = 0;
	trv[0].y = 0;
	trv[0].tcord[0].u = 0;
	trv[0].tcord[0].v = 0;
	trv[0].dcDiffuse = 0xFFFFFFFF;
	trv[0].dcSpecular = 0xFFFFFFFF;


	trv[1].x = 128;
	trv[1].y = 0;
	trv[0].tcord[0].u = 1.0;
	trv[0].tcord[0].v = 0;
	trv[1].dcDiffuse = 0xFFFFFFFF;
	trv[1].dcSpecular = 0xFFFFFFFF;

	trv[2].x = 128;
	trv[2].y = 128;
	trv[0].tcord[0].u = 1.0;
	trv[0].tcord[0].v = 1.0;
	trv[2].dcDiffuse = 0xFFFFFFFF;
	trv[2].dcSpecular = 0xFFFFFFFF;

	trv[3].x = 0;
	trv[3].y = 128;
	trv[0].tcord[0].u = 0;
	trv[0].tcord[0].v = 1.0;
	trv[3].dcDiffuse = 0xFFFFFFFF;
	trv[3].dcSpecular = 0xFFFFFFFF;

	trv[0].z = trv[1].z = trv[2].z = trv[3].z = 1.0;
	trv[0].rhw = trv[1].rhw = trv[2].rhw = trv[3].rhw = 1.0;


	WORD wIndices[2*3] = {1,0,2, 2,0,3};

	g_pD3DDev->SetVertexShader(RICE_FVF_TLITVERTEX);
	hr = g_pD3DDev->DrawIndexedPrimitiveUP( D3DPT_TRIANGLELIST, 0, 4, 2, wIndices, D3DFMT_INDEX16, trv, sizeof(TLITVERTEX));

	// Restore state
	ZBufferEnable( bZEnabled );
	CGraphicsContext::Get()->UpdateFrame();

	TRACE0("Draw the Tlut As Image");
	*/
}
#endif

// #define SUPPORT_ZBUFFER_IMG		//Doesn't work good enough, still need lockable zbuffer
//#define SUPPORT_LOCKABLE_ZBUFFER
void D3DRender::DrawObjBGCopy(uObjBg &info)
{
	if( IsUsedAsDI(g_CI.dwAddr) )
	{
#ifndef SUPPORT_LOCKABLE_ZBUFFER
#ifndef SUPPORT_ZBUFFER_IMG
		g_pD3DDev->Clear(0, NULL, D3DCLEAR_ZBUFFER, 0, 1.0, 0);	//Check me
		LOG_DL("    Clearing ZBuffer by using ZeldaBG");
#else
		if( g_CI.dwWidth == 0x200 && info.imageFmt == g_CI.dwFormat && info.imageSiz == g_CI.dwSize &&
				info.frameW == 0x800 )
		{
			InitCombinerBlenderForSimpleTextureDraw();
			ZBufferEnable( TRUE );
			D3DSetColorArg1( 0, D3DTA_TFACTOR );
			D3DSetAlphaArg1( 0, D3DTA_TFACTOR );
			SetTextureFactor(0);
			//SetTextureFactor(0x80808080);
			//SetD3DRSSrcBlend( D3DBLEND_SRCCOLOR);
			//SetD3DRSDestBlend(D3DBLEND_INVSRCALPHA);
			
			// Resident Evil2
			DWORD width = *g_GraphicsInfo.VI_WIDTH_REG & 0xFFF;
			DWORD height = (info.frameW/4*info.frameH/4)/width;
			DWORD pitch = width;

			//LoadObjBGCopy(info);

			// We don't support lockable Zbuffer, but we can simular it by splitting the image
			// to small pieces and render the piece with depth from the depth image

			WORD *Base = (WORD*)(g_pRDRAMu8+RSPSegmentAddr(info.imagePtr));
			float depth;

			const DWORD inc=10;

			for( DWORD i=0; i<height; i+=inc )
			{
				WORD *pSrc = Base + i * pitch;
				depth = (*pSrc + 1.0f ) / 0x10000;
				for( DWORD j=0; j<width; j+=inc)
				{
					DrawSimpleRect(j, i, j+inc, i+inc, gRDP.primitiveColor, depth, 1/depth);

#ifdef _DEBUG
					if( pauseAtNext && eventToPause == NEXT_TRIANGLE )
					{
						debuggerPause = true;
						TRACE0("Pause after drawing a rect for Z buffer");
						DebuggerPause();
					}
#endif
				}
			}
		}
#endif
#else
		if( g_pLockableBackBuffer == NULL && status.bHandleN64TextureBuffer == false )
		{
			if( IsResultGood(g_pD3DDev->CreateDepthStencilSurface(windowSetting.uDisplayWidth, windowSetting.uDisplayHeight, D3DFMT_D16_LOCKABLE, D3DMULTISAMPLE_NONE, &g_pLockableBackBuffer)) && g_pLockableBackBuffer )
			{
				g_pD3DDev->SetRenderTarget(NULL, g_pLockableBackBuffer);
				TRACE0("Created and use lockable depth buffer");
			}
			else
			{
				if( g_pLockableBackBuffer )
				{
					g_pLockableBackBuffer->Release();
					g_pLockableBackBuffer = NULL;
				}
				TRACE0("Can not create lockable depth buffer");
			}
		}

		DEBUGGER_IF_DUMP((pauseAtNext&& (eventToPause==NEXT_OBJ_TXT_CMD||eventToPause==NEXT_OBJ_BG)), 
		{TRACE0("Using BG to update Z buffer has not been implemented yet");});

		// I can not finish this function because Z buffer is not lockable
		// and lockable zbuffer does not work
		LPDIRECT3DSURFACE8 pDepthBufferSurf = NULL;
		g_pD3DDev->GetDepthStencilSurface(&pDepthBufferSurf);

		D3DLOCKED_RECT dlre;
		ZeroMemory( &dlre, sizeof(D3DLOCKED_RECT) );
		//int pixSize = GetPixelSize();
		DWORD width = *g_GraphicsInfo.VI_WIDTH_REG & 0xFFF;
		DWORD height = (info.frameW/4*info.frameH/4)/width;
		DWORD pitch = width;

		if (IsResultGood(pDepthBufferSurf->LockRect(&dlre, NULL, D3DLOCK_NOSYSLOCK)))
		{
			WORD *Base = (WORD*)(g_pRDRAMu8+RSPSegmentAddr(g_ZI.dwAddr));
			int x, y;
			for( int i=0; i<windowSetting.uDisplayHeight; i++ )
			{
				y = i*height/windowSetting.uDisplayHeight;

				WORD *pSrc = Base + y * pitch;
				WORD *pDst = (WORD*)((BYTE *)dlre.pBits + i * dlre.Pitch);

				for( int j=0; j<windowSetting.uDisplayWidth; j++ )
				{
					x = j*width/windowSetting.uDisplayWidth;
					pDst[j] = pSrc[x];
				}
			}
			pDepthBufferSurf->UnlockRect();
		}
		
		pDepthBufferSurf->Release();
#endif
		return;
	}
	else
	{	
		CRender::LoadObjBGCopy(info);
		CRender::DrawObjBGCopy(info);
	}
}

Matrix spriteMtx(1.0, 0, 0, 0, 0, 1.0, 0, 0, 0, 0, 1.0, 0, 0, 0, 0, 1.0);

extern uObjTxtrTLUT *gObjTlut;
extern void TH_RemoveTextureEntry(TextureEntry * pEntry);


void D3DRender::FinishSpriteDraw(LPD3DXSPRITE d3dSprite)
{
	if( d3dSprite != NULL )
	{
		d3dSprite->End();
		d3dSprite->Release();
	}
}



LPD3DXSPRITE D3DRender::InitSpriteDraw(void)
{
	D3DSetColorOp( 1, D3DTOP_DISABLE );
	D3DSetAlphaOp( 1, D3DTOP_DISABLE );

	((CDirectXColorCombiner*)m_pColorCombiner)->SetNumStages(0);

	D3DSetTexture( 0, LPDIRECT3DTEXTURE8(g_textures[gRSP.curTile].m_lpsTexturePtr) );
	D3DSetColorOp( 0, D3DTOP_SELECTARG1 );
	D3DSetColorArg1( 0, D3DTA_TEXTURE );

	D3DSetAlphaOp( 0, D3DTOP_SELECTARG1 );
	D3DSetAlphaArg1( 0, D3DTA_TFACTOR );
	SetTextureFactor(0xFFFFFFFF);

	LPD3DXSPRITE d3dSprite;
	HRESULT hr = D3DXCreateSprite(g_pD3DDev, &d3dSprite);
	if( hr == S_OK )
	{
		d3dSprite->Begin();
		return d3dSprite;
	}
	else
	{
		if( d3dSprite != NULL )
			d3dSprite->Release();
		return NULL;
	}
}


void D3DRender::DrawText(const char* str, RECT *rect)
{
	return;

	 
}
