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
#ifdef _XBOX
#include <d3d8.h>
#include "../../../config.h"
#endif

#define RICEFVF_TEXRECTFVERTEX ( D3DFVF_XYZRHW | /*D3DFVF_DIFFUSE |*/ D3DFVF_TEX2 )
#define RICE_FVF_FILLRECTVERTEX ( D3DFVF_XYZRHW | D3DFVF_DIFFUSE  )

extern FiddledVtx * g_pVtxBase;
const int d3d_bias_factor = 4;

inline float round( float x )
{
	return (float)(s32)( x + 0.5f );
}

UVFlagMap DirectXUVFlagMaps[] =
{
	TEXTURE_UV_FLAG_WRAP,	D3DTADDRESS_WRAP,
	TEXTURE_UV_FLAG_MIRROR,	D3DTADDRESS_MIRROR,
	TEXTURE_UV_FLAG_CLAMP,	D3DTADDRESS_CLAMP,
};


//*****************************************************************************
// Creator function for singleton
//*****************************************************************************
D3DRender::D3DRender() 
{
	m_dwrsZEnable=D3DZB_FALSE;
	m_dwrsZWriteEnable=FALSE;
#ifdef _INIT_COMBINER
	memset(&m_D3DCombStages, 0, sizeof(D3DCombinerStage)*8);

	for (int i = 0; i < 8; i++)
	{
		m_D3DCombStages[i].dwColorOp = D3DTOP_DISABLE;
		m_D3DCombStages[i].dwColorArg1 = D3DTA_CURRENT;
		m_D3DCombStages[i].dwColorArg2 = D3DTA_CURRENT;
		m_D3DCombStages[i].dwColorArg0 = D3DTA_CURRENT;
		m_D3DCombStages[i].dwAlphaOp = D3DTOP_DISABLE;
		m_D3DCombStages[i].dwAlphaArg1 = D3DTA_CURRENT;
		m_D3DCombStages[i].dwAlphaArg2 = D3DTA_CURRENT;
		m_D3DCombStages[i].dwAlphaArg0 = D3DTA_CURRENT;

		m_D3DCombStages[i].dwMinFilter = D3DTEXF_NONE;
		m_D3DCombStages[i].dwMagFilter = D3DTEXF_NONE;

		m_D3DCombStages[i].dwAddressUMode = 0;
		m_D3DCombStages[i].dwAddressVMode = 0;
		m_D3DCombStages[i].dwAddressW = 0xFFFF;

		m_D3DCombStages[i].dwTexCoordIndex = 0xFFFF;
		m_D3DCombStages[i].pTexture = NULL;
	}
#endif
	m_Mux = 0;
	memset(&m_curCombineInfo, 0, sizeof( m_curCombineInfo) );

	m_dwrsZEnable = 0xEEEE;
	m_dwrsZWriteEnable = 0xEEEE;
	m_dwrsSrcBlend = 0xEEEE;
	m_dwrsDestBlend = 0xEEEE;
	m_dwrsAlphaBlendEnable = 0xEEEE;
	m_dwrsAlphaTestEnable = 0xEEEE;
	m_dwrsAlphaRef = 0xEEEE;
	m_dwrsZBias = 0xEEEE;
}

D3DRender::~D3DRender()
{	
	ClearDeviceObjects();
	gD3DDevWrapper.SetPixelShader(NULL);
}

bool D3DRender::ClearDeviceObjects()
{
	for( int i=0; i<MAX_TEXTURES; i++)
	{
		if (g_textures[i].m_lpsTexturePtr)		// We keep a reference to the most recently selected texture
		{
			MYLPDIRECT3DTEXTURE(g_textures[i].m_lpsTexturePtr)->Release();
			g_textures[i].m_lpsTexturePtr = NULL;
			gD3DDevWrapper.SetTexture( i, NULL );
		}
	}

	return true;
}
	

bool D3DRender::InitDeviceObjects()
{
	// We never change these

	gD3DDevWrapper.SetRenderState( D3DRS_DITHERENABLE, TRUE ); // Re-enabled by Orkin - Makes 16-bit look nicer

	// We do our own culling
	gD3DDevWrapper.SetRenderState( D3DRS_CULLMODE,   D3DCULL_NONE );

	// We do our own lighting
	gD3DDevWrapper.SetRenderState(  D3DRS_AMBIENT, COLOR_RGBA(255,255,255,255) );
	gD3DDevWrapper.SetRenderState( D3DRS_LIGHTING,	  FALSE);


	gD3DDevWrapper.SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE );
	gD3DDevWrapper.SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	gD3DDevWrapper.SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);

	gD3DDevWrapper.SetRenderState( D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
	gD3DDevWrapper.SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	
#ifdef _XBOX

	if(AntiAliasMode>1){
		gD3DDevWrapper.SetRenderState( D3DRS_MULTISAMPLEANTIALIAS , TRUE);
	}else if(AntiAliasMode==1){
		gD3DDevWrapper.SetRenderState( D3DRS_MULTISAMPLEANTIALIAS , FALSE);
		gD3DDevWrapper.SetRenderState( D3DRS_ALPHABLENDENABLE , TRUE);
		gD3DDevWrapper.SetRenderState( D3DRS_EDGEANTIALIAS , TRUE);
	}else{
		gD3DDevWrapper.SetRenderState( D3DRS_MULTISAMPLEANTIALIAS , FALSE);
	}

#else

	if( ((CDXGraphicsContext*)CGraphicsContext::g_pGraphicsContext)->IsFSAAEnable() )
		gD3DDevWrapper.SetRenderState( D3DRS_MULTISAMPLEANTIALIAS , TRUE);
	else
		gD3DDevWrapper.SetRenderState( D3DRS_MULTISAMPLEANTIALIAS, FALSE);
#endif

	// Initialize all the renderstate to our defaults.
	SetShadeMode( gRSP.shadeMode );
	gD3DDevWrapper.SetRenderState( D3DRS_TEXTUREFACTOR, 0xFFFFFFFF );

	gD3DDevWrapper.SetRenderState( D3DRS_FOGENABLE, FALSE);
	float density = 1.0f;
	gD3DDevWrapper.SetRenderState(D3DRS_FOGDENSITY,   *(uint32 *)(&density));
	
#if 1
	if(options.bUseLinearFog)
		gD3DDevWrapper.SetRenderState( D3DRS_FOGTABLEMODE, D3DFOG_LINEAR );
	else
		gD3DDevWrapper.SetRenderState(D3DRS_RANGEFOGENABLE, TRUE);
#else
	//gD3DDevWrapper.SetRenderState( D3DRS_FOGTABLEMODE, D3DFOG_LINEAR );
#endif

	// Dafault is ZBuffer disabled
	gD3DDevWrapper.SetRenderState(D3DRS_ZENABLE, m_dwrsZEnable );

    gD3DDevWrapper.SetRenderState(D3DRS_ALPHATESTENABLE,TRUE );
    gD3DDevWrapper.SetRenderState(D3DRS_ALPHAREF,0x04 );
    gD3DDevWrapper.SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );

	m_dwrsZEnable=D3DZB_FALSE;
	m_dwrsZWriteEnable=FALSE;

#ifdef _INIT_COMBINER
	memset(&m_D3DCombStages, 0, sizeof(D3DCombinerStage)*8);

	for (int i = 0; i < 8; i++)
	{
		m_D3DCombStages[i].dwColorOp = D3DTOP_DISABLE;
		m_D3DCombStages[i].dwColorArg1 = D3DTA_CURRENT;
		m_D3DCombStages[i].dwColorArg2 = D3DTA_CURRENT;
		m_D3DCombStages[i].dwColorArg0 = D3DTA_CURRENT;
		m_D3DCombStages[i].dwAlphaOp = D3DTOP_DISABLE;
		m_D3DCombStages[i].dwAlphaArg1 = D3DTA_CURRENT;
		m_D3DCombStages[i].dwAlphaArg2 = D3DTA_CURRENT;
		m_D3DCombStages[i].dwAlphaArg0 = D3DTA_CURRENT;

		m_D3DCombStages[i].dwMinFilter = D3DTEXF_NONE;
		m_D3DCombStages[i].dwMagFilter = D3DTEXF_NONE;

		m_D3DCombStages[i].dwAddressUMode = 0xFFFF;
		m_D3DCombStages[i].dwAddressVMode = 0xFFFF;
		m_D3DCombStages[i].dwAddressW = 0xFFFF;

		m_D3DCombStages[i].dwTexCoordIndex = 0xFFFF;
	}
#endif
	m_Mux = 0;
	memset(&m_curCombineInfo, 0, sizeof( m_curCombineInfo) );

#ifndef _XBOX
	gD3DDevWrapper.Initalize();
#endif

	m_dwrsZEnable = 0xEEEE;
	m_dwrsZWriteEnable = 0xEEEE;
	m_dwrsSrcBlend = 0xEEEE;
	m_dwrsDestBlend = 0xEEEE;
	m_dwrsAlphaBlendEnable = 0xEEEE;
	m_dwrsAlphaTestEnable = 0xEEEE;
	m_dwrsAlphaRef = 0xEEEE;
	m_dwrsZBias = 0xEEEE;

#ifdef _XBOX
	for(int i = 0; i < 4; i++) 
#else
	for(int i = 0; i < 8; i++) //Crashes with this on XBOX
#endif
	{ 
		// Texturing stuff 
		D3DSetMinFilter( i, D3DTEXF_LINEAR ); 
		D3DSetMagFilter( i, D3DTEXF_LINEAR ); 

#if DX_VERSION == 8		
		gD3DDevWrapper.SetTextureStageState(i, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP ); 
		gD3DDevWrapper.SetTextureStageState(i, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP );
#elif DX_VERSION == 9
		gD3DDevWrapper.SetSamplerState(i, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP ); 
		gD3DDevWrapper.SetSamplerState(i, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP );
#endif 
		
		gD3DDevWrapper.SetTextureStageState( i, D3DTSS_COLORARG1, D3DTA_TEXTURE ); 
		gD3DDevWrapper.SetTextureStageState( i, D3DTSS_COLORARG2, D3DTA_DIFFUSE ); 
		gD3DDevWrapper.SetTextureStageState( i, D3DTSS_COLORARG0, D3DTA_DIFFUSE ); 
		gD3DDevWrapper.SetTextureStageState( i, D3DTSS_COLOROP, D3DTOP_MODULATE ); 
		gD3DDevWrapper.SetTextureStageState( i, D3DTSS_ALPHAARG1, D3DTA_TEXTURE ); 
		gD3DDevWrapper.SetTextureStageState( i, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE ); 
		gD3DDevWrapper.SetTextureStageState( i, D3DTSS_ALPHAARG0, D3DTA_DIFFUSE ); 
		gD3DDevWrapper.SetTextureStageState( i, D3DTSS_ALPHAOP, D3DTOP_MODULATE ); 

		gD3DDevWrapper.SetTexture( i, NULL );
	} 
	
	((CDirectXColorCombiner*)m_pColorCombiner)->Initialize();
#ifndef _OLDCLIPPER
	status.curScissor = UNKNOWN_SCISSOR;
#endif

#ifdef _XBOX
	g_pD3DDev->SetTextureStageState(0, D3DTSS_MINFILTER, TextureMode);
	g_pD3DDev->SetTextureStageState(0, D3DTSS_MAGFILTER, TextureMode);
	g_pD3DDev->SetFlickerFilter(FlickerFilter);
	g_pD3DDev->SetSoftDisplayFilter(SoftDisplayFilter);
#endif

#ifdef _DXCLIP
#if DX_VERSION == 8
	D3DCLIPSTATUS8 clippingstatus;
#else
	D3DCLIPSTATUS9 clippingstatus;
#endif
	g_pD3DDev->GetClipStatus(&clippingstatus);
	clippingstatus.ClipUnion = D3DCS_BACK | D3DCS_BOTTOM | D3DCS_FRONT | D3DCS_LEFT | D3DCS_RIGHT | D3DCS_TOP;
	clippingstatus.ClipIntersection = 0xFFFFFFFF;
	g_pD3DDev->SetClipStatus(&clippingstatus);
	g_pD3DDev->GetClipStatus(&clippingstatus);
	g_pD3DDev->SetRenderState(D3DRS_CLIPPING, TRUE);
#endif

	return true;	
}

bool D3DRender::RenderTexRect()
{
	gD3DDevWrapper.SetRenderState(D3DRS_ZBIAS,0);
	uint16 wIndices[2*3] = {1,0,2, 2,0,3};
	gD3DDevWrapper.SetVertexShader(RICE_FVF_TLITVERTEX);
	return S_OK == g_pD3DDev->DrawIndexedPrimitiveUP( D3DPT_TRIANGLELIST, 0, 4, 2, wIndices, D3DFMT_INDEX16, m_texRectTVtx, sizeof(TLITVERTEX));
}

bool D3DRender::RenderFillRect(uint32 dwColor, float depth)
{
	FILLRECTVERTEX frv[4] = {   {m_fillRectVtx[0].x, m_fillRectVtx[0].y, depth, 1, dwColor},
								{m_fillRectVtx[1].x, m_fillRectVtx[0].y, depth, 1, dwColor},
								{m_fillRectVtx[1].x, m_fillRectVtx[1].y, depth, 1, dwColor},
								{m_fillRectVtx[0].x, m_fillRectVtx[1].y, depth, 1, dwColor}  };
	static uint16 wIndices[2*3] = {1,0,2, 2,0,3};

	gD3DDevWrapper.SetRenderState(D3DRS_ZBIAS,0);
	gD3DDevWrapper.SetVertexShader(RICE_FVF_FILLRECTVERTEX);
	return S_OK == g_pD3DDev->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, 4, 2, wIndices, D3DFMT_INDEX16, frv, sizeof(FILLRECTVERTEX));
}

void ApplyZBias(uint32 bias)
{
#if DX_VERSION == 8
	gD3DDevWrapper.SetRenderState(D3DRS_ZBIAS,bias*d3d_bias_factor);
#else
	float f1 = bias > 0 ? -0.002f : 0.0f;
	float f2 = bias > 0 ? 1.0f : 0.0f;
	gD3DDevWrapper.SetRenderState(D3DRS_DEPTHBIAS,*(DWORD*)(&f1));
	gD3DDevWrapper.SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, *(DWORD*)(&f2));
#endif
}
void D3DRender::SetZBias(int bias)
{
	//if (m_dwZBias != bias)
	{
		//DEBUGGER_IF_DUMP(pauseAtNext, TRACE1("Set zbias = %d", bias));

		m_dwZBias = bias;
		ApplyZBias(m_dwZBias);
	}
}

bool D3DRender::RenderFlushTris()
{
	ApplyZBias(m_dwZBias);

	gD3DDevWrapper.SetVertexShader(RICE_FVF_TLITVERTEX);
	if( options.bForceSoftwareClipper )
	{
		ClipVertexes();
		if( g_clippedVtxCount > 0 )
			g_pD3DDev->DrawPrimitiveUP(D3DPT_TRIANGLELIST, g_clippedVtxCount/3, g_clippedVtxBuffer, sizeof(TLITVERTEX));
	}
	else
	{
		g_pD3DDev->DrawPrimitiveUP(D3DPT_TRIANGLELIST, gRSP.numVertices/3, g_vtxBuffer, sizeof(TLITVERTEX));
	}

	return true;
}

bool D3DRender::RenderLine3D()
{
	FILLRECTVERTEX frv[4] = {   {m_line3DVector[0].x, m_line3DVector[0].y, m_line3DVtx[0].z, m_line3DVtx[0].rhw, m_line3DVtx[0].dcDiffuse},
								{m_line3DVector[1].x, m_line3DVector[1].y, m_line3DVtx[0].z, m_line3DVtx[0].rhw, m_line3DVtx[0].dcDiffuse},
								{m_line3DVector[2].x, m_line3DVector[2].y, m_line3DVtx[0].z, m_line3DVtx[0].rhw, m_line3DVtx[1].dcDiffuse},
								{m_line3DVector[3].x, m_line3DVector[3].y, m_line3DVtx[0].z, m_line3DVtx[0].rhw, m_line3DVtx[1].dcDiffuse}  };

	static uint16 wIndices[2*3] = {1,0,2, 2,1,3};

	gD3DDevWrapper.SetRenderState(D3DRS_ZBIAS,0);
	gD3DDevWrapper.SetVertexShader(RICE_FVF_FILLRECTVERTEX);
	HRESULT hr = g_pD3DDev->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, 4, 2, wIndices, D3DFMT_INDEX16, frv, sizeof(FILLRECTVERTEX));

	return hr == S_OK;
}


MYLPDIRECT3DSURFACE g_pLockableBackBuffer=NULL;

void D3DRender::DrawSimple2DTexture(float x0, float y0, float x1, float y1, float u0, float v0, float u1, float v1, D3DCOLOR dif, D3DCOLOR spe, float z, float rhw)
{
	if( status.bVIOriginIsUpdated == true && currentRomOptions.screenUpdateSetting==SCREEN_UPDATE_AT_1ST_PRIMITIVE )
	{
		status.bVIOriginIsUpdated=false;
		CGraphicsContext::Get()->UpdateFrame();
	}

	StartDrawSimple2DTexture(x0, y0, x1, y1, u0, v0, u1, v1, dif, spe, z, rhw);
	
	uint16 wIndices[2*3] = {1,0,2, 2,0,3};
	gD3DDevWrapper.SetVertexShader(RICE_FVF_TLITVERTEX);
	g_pD3DDev->DrawIndexedPrimitiveUP( D3DPT_TRIANGLELIST, 0, 4, 2, wIndices, D3DFMT_INDEX16, m_texRectTVtx, sizeof(TLITVERTEX));
}

void D3DRender::DrawSimpleRect(LONG nX0, LONG nY0, LONG nX1, LONG nY1, uint32 dwColor, float depth, float rhw)
{
	StartDrawSimpleRect(nX0, nY0, nX1, nY1, dwColor, depth, rhw);

	// Divide by 2 instead of 4, because screen is magnified by two
	FILLRECTVERTEX frv[4] = {   {m_simpleRectVtx[0].x, m_simpleRectVtx[0].y, depth, rhw, dwColor},
								{m_simpleRectVtx[1].x, m_simpleRectVtx[0].y, depth, rhw, dwColor},
								{m_simpleRectVtx[1].x, m_simpleRectVtx[1].y, depth, rhw, dwColor},
								{m_simpleRectVtx[0].x, m_simpleRectVtx[1].y, depth, rhw, dwColor}  };

	static uint16 wIndices[2*3] = {1,0,2, 2,0,3};

	gD3DDevWrapper.SetVertexShader(RICE_FVF_FILLRECTVERTEX);
	g_pD3DDev->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, 4, 2, wIndices, D3DFMT_INDEX16, frv, sizeof(FILLRECTVERTEX));
}

void D3DRender::SetTextureUFlag(TextureUVFlag dwFlag, uint32 tile)
{
	TileUFlags[tile] = dwFlag;
	if( gRDP.otherMode.cycle_type  >= CYCLE_TYPE_COPY )
	{
#if DX_VERSION == 8
		gD3DDevWrapper.SetTextureStageState(0, D3DTSS_ADDRESSU, DirectXUVFlagMaps[dwFlag].realFlag );
#elif DX_VERSION == 9
		gD3DDevWrapper.SetSamplerState(0, D3DSAMP_ADDRESSU, DirectXUVFlagMaps[dwFlag].realFlag );
#endif
	}
	else
	{
		for( int i=0; i<m_curCombineInfo.nStages; i++ )
		{
			if( m_curCombineInfo.stages[i].dwTexture == tile-gRSP.curTile )
			{
#if DX_VERSION == 8
				gD3DDevWrapper.SetTextureStageState(i, D3DTSS_ADDRESSU, DirectXUVFlagMaps[dwFlag].realFlag );
#elif DX_VERSION == 9
				gD3DDevWrapper.SetSamplerState(i, D3DSAMP_ADDRESSU, DirectXUVFlagMaps[dwFlag].realFlag );
#endif
			}
		}
	}
}

void D3DRender::SetTextureVFlag(TextureUVFlag dwFlag, uint32 tile)
{
	TileVFlags[tile] = dwFlag;
	if( gRDP.otherMode.cycle_type  >= CYCLE_TYPE_COPY )
	{
#if DX_VERSION == 8
		gD3DDevWrapper.SetTextureStageState(0, D3DTSS_ADDRESSV, DirectXUVFlagMaps[dwFlag].realFlag );
#elif DX_VERSION == 9
		gD3DDevWrapper.SetSamplerState(0, D3DSAMP_ADDRESSV, DirectXUVFlagMaps[dwFlag].realFlag );
#endif
	}
	else
	{
		for( int i=0; i<m_curCombineInfo.nStages; i++ )
		{
			if( m_curCombineInfo.stages[i].dwTexture == tile-gRSP.curTile )
			{
#if DX_VERSION == 8
				gD3DDevWrapper.SetTextureStageState(i, D3DTSS_ADDRESSV, DirectXUVFlagMaps[dwFlag].realFlag );
#elif DX_VERSION == 9
				gD3DDevWrapper.SetSamplerState(i, D3DSAMP_ADDRESSV, DirectXUVFlagMaps[dwFlag].realFlag );
#endif
			}
		}
	}
}

void D3DRender::SetAddressUAllStages(uint32 dwTile, TextureUVFlag dwFlag)
{
	SetTextureUFlag(dwFlag, dwTile);
}

void D3DRender::SetAddressVAllStages(uint32 dwTile, TextureUVFlag dwFlag)
{
	SetTextureVFlag(dwFlag, dwTile);
}

#define RSP_ZELDA_FOG 0x00010000
void D3DRender::ZBufferEnable(BOOL bZBuffer)
{
	if( g_curRomInfo.bForceDepthBuffer )
		bZBuffer = TRUE;

	SetZCompare(bZBuffer);
	SetZUpdate(bZBuffer);
}

#define RSP_ZELDA_ZBUFFER 0x00000001 
void D3DRender::SetZCompare(BOOL bZCompare)
{
	if( g_curRomInfo.bForceDepthBuffer )
		bZCompare = TRUE;

	gRSP.bZBufferEnabled = bZCompare;
	m_bZCompare = bZCompare;
	gD3DDevWrapper.SetRenderState(D3DRS_ZENABLE, bZCompare ? D3DZB_TRUE : D3DZB_FALSE );
}

void D3DRender::SetZUpdate(BOOL bZUpdate)
{
	if( g_curRomInfo.bForceDepthBuffer )
		bZUpdate = TRUE;

	m_bZUpdate = bZUpdate;
	if( bZUpdate )	
	{
		gD3DDevWrapper.SetRenderState(D3DRS_ZENABLE,  D3DZB_TRUE );
	}
	gD3DDevWrapper.SetRenderState(D3DRS_ZWRITEENABLE, bZUpdate );
}

//freakdave
TextureFilterMap DXTexFilterMap[6]=
{
	{FILTER_NONE, D3DTEXF_NONE},
	{FILTER_POINT, D3DTEXF_POINT},
	{FILTER_LINEAR, D3DTEXF_LINEAR},
	{FILTER_ANISOTROPIC, D3DTEXF_ANISOTROPIC},
	//{FILTER_FLATCUBIC, D3DTEXF_FLATCUBIC}, //wtf is going on here ??
	{FILTER_FLATCUBIC, 4}, //it works this way though,hm,strange...
	{FILTER_GAUSSIANCUBIC, D3DTEXF_GAUSSIANCUBIC},
};

void D3DRender::ApplyTextureFilter()
{
	if( gRDP.otherMode.cycle_type  >= CYCLE_TYPE_COPY )
	{
		D3DSetMinFilter( 0, DXTexFilterMap[m_dwMinFilter].realFilter );
		D3DSetMagFilter( 0, DXTexFilterMap[m_dwMagFilter].realFilter );
	}
	else
	{
		for( int i=0; i<m_curCombineInfo.nStages; i++ )
		{
			D3DSetMinFilter( i, DXTexFilterMap[m_dwMinFilter].realFilter );
			D3DSetMagFilter( i, DXTexFilterMap[m_dwMagFilter].realFilter );
		}
	}
}

void D3DRender::SetShadeMode(RenderShadeMode mode)
{
	if (gRSP.shadeMode != mode)
	{
		gRSP.shadeMode = mode;
		if( mode == SHADE_DISABLED || mode == SHADE_FLAT )	//Shade is disabled, use Primitive color for flat shade
		{
			gD3DDevWrapper.SetRenderState( D3DRS_SHADEMODE, D3DSHADE_FLAT );
		}
		else
			gD3DDevWrapper.SetRenderState( D3DRS_SHADEMODE, D3DSHADE_GOURAUD );
	}
}

void D3DRender::SetAlphaRef(uint32 dwAlpha)
{
	if (m_dwAlpha != dwAlpha)
	{
		m_dwAlpha = dwAlpha;
		gD3DDevWrapper.SetRenderState(D3DRS_ALPHAREF,dwAlpha);
	}
}

void D3DRender::ForceAlphaRef(uint32 dwAlpha)
{
	//gD3DDevWrapper.SetRenderState(D3DRS_ALPHAREF,dwAlpha);
	m_dwrsAlphaRef = dwAlpha;
	gD3DDevWrapper.SetRenderState( D3DRS_ALPHAREF, dwAlpha );	
}

bool D3DRender::SetCurrentTexture(int tile, CTexture *handler, uint32 dwTileWidth, uint32 dwTileHeight, TxtrCacheEntry *pTextureEntry)
{
	RenderTexture &texture = g_textures[tile];
	texture.pTextureEntry = pTextureEntry;

	if( handler != NULL  && texture.m_lpsTexturePtr != handler->GetTexture() )
	{
		if( texture.m_lpsTexturePtr )
		{
			MYLPDIRECT3DTEXTURE(texture.m_lpsTexturePtr)->Release();
			texture.m_lpsTexturePtr = NULL;
		}

		texture.m_lpsTexturePtr = MYLPDIRECT3DTEXTURE(handler->GetTexture());
		texture.m_pCTexture = handler;

		if (texture.m_lpsTexturePtr != NULL)
		{
			MYLPDIRECT3DTEXTURE(texture.m_lpsTexturePtr)->AddRef();
		}
			
		texture.m_dwTileWidth = dwTileWidth;
		texture.m_dwTileHeight = dwTileHeight;

		texture.m_fTexWidth = (float)handler->m_dwCreatedTextureWidth;
		texture.m_fTexHeight = (float)handler->m_dwCreatedTextureHeight;
	}

	return true;
}

bool D3DRender::SetCurrentTexture(int tile, TxtrCacheEntry *pEntry)
{
	if (pEntry != NULL && pEntry->pTexture != NULL)
	{	
		SetCurrentTexture( tile, pEntry->pTexture,pEntry->ti.WidthToCreate, pEntry->ti.HeightToCreate, pEntry);
		return true;
	}
	else
	{
		SetCurrentTexture( tile, NULL, 64, 64, NULL );
		return false;
	}
}


void D3DRender::SetFillMode(FillMode mode)
{
	switch( mode )
	{
	case RICE_FILLMODE_WINFRAME:
		gD3DDevWrapper.SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME );
		break;
	case RICE_FILLMODE_SOLID:
		gD3DDevWrapper.SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID );
		break;
	}
}

void D3DRender::SetAlphaTestEnable(BOOL bAlphaTestEnable)
{
	gD3DDevWrapper.SetRenderState(D3DRS_ALPHATESTENABLE,(bAlphaTestEnable));
}

extern float HackZ(float z);
void D3DRender::SetFogMinMax(float fMin, float fMax)
{
	if( g_curRomInfo.bZHack )
	{
		float minf = HackZ(gRSPfFogMin);
		float maxf = HackZ(gRSPfFogMax);
		gD3DDevWrapper.SetRenderState(D3DRS_FOGSTART, *(uint32 *)(&minf));
		gD3DDevWrapper.SetRenderState(D3DRS_FOGEND,   *(uint32 *)(&maxf));
	}
	else
	{
		gD3DDevWrapper.SetRenderState(D3DRS_FOGSTART, *(uint32 *)(&gRSPfFogMin));
		gD3DDevWrapper.SetRenderState(D3DRS_FOGEND,   *(uint32 *)(&gRSPfFogMax));
	}
}

void D3DRender::TurnFogOnOff(bool flag)
{
	gD3DDevWrapper.SetRenderState( D3DRS_FOGENABLE, flag?TRUE:FALSE);
}

#define RSP_ZELDA_CULL_FRONT 0x00000400
void D3DRender::SetFogEnable(bool bEnable)
{
	if( options.enableHackForGames == HACK_FOR_TWINE && gRSP.bFogEnabled == FALSE && bEnable == FALSE && (gRDP.geometryMode & RSP_ZELDA_CULL_FRONT) )
	{
		g_pD3DDev->Clear(1, NULL, D3DCLEAR_ZBUFFER, 0xFF000000, 1.0, 0);
	}



	gRSP.bFogEnabled = bEnable&&options.bEnableFog;

	if( gRSP.bFogEnabled )
	{
		gD3DDevWrapper.SetRenderState( D3DRS_FOGENABLE, TRUE);
		gD3DDevWrapper.SetRenderState(D3DRS_FOGCOLOR, gRDP.fogColor);
#if 1
		if(!options.bUseLinearFog)
			gD3DDevWrapper.SetRenderState(D3DRS_RANGEFOGENABLE, TRUE);
#endif
		
		if( g_curRomInfo.bZHack )
		{
			float minf = HackZ(gRSPfFogMin);
			float maxf = HackZ(gRSPfFogMax);
			gD3DDevWrapper.SetRenderState(D3DRS_FOGSTART, *(uint32 *)(&minf));
			gD3DDevWrapper.SetRenderState(D3DRS_FOGEND,   *(uint32 *)(&maxf));
		}
		else
		{
			gD3DDevWrapper.SetRenderState(D3DRS_FOGSTART, *(uint32 *)(&gRSPfFogMin));
			gD3DDevWrapper.SetRenderState(D3DRS_FOGEND,   *(uint32 *)(&gRSPfFogMax));
		}
	}
	else
	{
		gD3DDevWrapper.SetRenderState( D3DRS_FOGENABLE, FALSE);
	}
}

void D3DRender::SetFogColor(uint32 r, uint32 g, uint32 b, uint32 a)
{
	gRDP.fogColor = COLOR_RGBA(r, g, b, a); 
    gD3DDevWrapper.SetRenderState(D3DRS_FOGCOLOR, gRDP.fogColor);
}

void D3DRender::ClearBuffer(bool cbuffer, bool zbuffer)
{
	float depth = ((gRDP.originalFillColor&0xFFFF)>>2)/(float)0x3FFF;
	if( cbuffer ) g_pD3DDev->Clear(0, NULL, D3DCLEAR_TARGET, 0xFF000000, depth, 0);
	if( zbuffer ) g_pD3DDev->Clear(0, NULL, D3DCLEAR_ZBUFFER, 0xFF000000, depth, 0);
}

void D3DRender::ClearZBuffer(float depth)
{
	g_pD3DDev->Clear(0, NULL, D3DCLEAR_ZBUFFER, 0xFF000000, depth, 0);
}

void D3DRender::ClearBuffer(bool cbuffer, bool zbuffer, D3DRECT &rect)
{
	float depth = ((gRDP.originalFillColor&0xFFFF)>>2)/(float)0x3FFF;
	if( cbuffer ) g_pD3DDev->Clear(1, &rect, D3DCLEAR_TARGET, 0xFF000000, depth, 0);
	if( zbuffer ) g_pD3DDev->Clear(1, &rect, D3DCLEAR_ZBUFFER, 0xFF000000, depth, 0);
}


void D3DRender::UpdateScissor()
{
	if( options.bEnableHacks && g_CI.dwWidth == 0x200 && gRDP.scissor.right == 0x200 && g_CI.dwWidth>(*g_GraphicsInfo.VI_WIDTH_REG & 0xFFF) )
	{
		// Hack for RE2
		uint32 width = *g_GraphicsInfo.VI_WIDTH_REG & 0xFFF;
		uint32 height = (gRDP.scissor.right*gRDP.scissor.bottom)/width;
		MYD3DVIEWPORT vp = {0, 0, (uint32)(width*windowSetting.fMultX), (uint32)(height*windowSetting.fMultY), 0, 1};
		if( !gRSP.bNearClip )
			vp.MinZ = -10000;
#ifndef _OLDCLIPPER
		if( vp.Width+vp.X > (DWORD)windowSetting.uDisplayWidth-1) vp.Width = windowSetting.uDisplayWidth-1-vp.X;
		if( vp.Height+vp.Y > (DWORD)windowSetting.uDisplayHeight-1) vp.Height = windowSetting.uDisplayHeight-1-vp.Y;
#endif
		gD3DDevWrapper.SetViewport(&vp);
	}
	else
	{
		UpdateScissorWithClipRatio();
	}
}

void D3DRender::ApplyRDPScissor(bool force)
{
#ifndef _OLDCLIPPER
	if( !force && status.curScissor == RDP_SCISSOR )	return;
#endif
	if( options.bEnableHacks && g_CI.dwWidth == 0x200 && gRDP.scissor.right == 0x200 && g_CI.dwWidth>(*g_GraphicsInfo.VI_WIDTH_REG & 0xFFF) )
	{
		// Hack for RE2
		uint32 width = *g_GraphicsInfo.VI_WIDTH_REG & 0xFFF;
		uint32 height = (gRDP.scissor.right*gRDP.scissor.bottom)/width;
		MYD3DVIEWPORT vp = {0, 0, (uint32)(width*windowSetting.fMultX), (uint32)(height*windowSetting.fMultY), 0, 1};
		if( !gRSP.bNearClip )
			vp.MinZ = -10000;
#ifndef _OLDCLIPPER
		if( vp.Width+vp.X > (DWORD)windowSetting.uDisplayWidth-1) vp.Width = windowSetting.uDisplayWidth-1-vp.X;
		if( vp.Height+vp.Y > (DWORD)windowSetting.uDisplayHeight-1) vp.Height = windowSetting.uDisplayHeight-1-vp.Y;
#endif
		gD3DDevWrapper.SetViewport(&vp);
	}
	else
	{
		MYD3DVIEWPORT vp = {
			(uint32)(gRDP.scissor.left*windowSetting.fMultX), 
				(uint32)(gRDP.scissor.top*windowSetting.fMultY), 
				(uint32)((gRDP.scissor.right-gRDP.scissor.left+1)*windowSetting.fMultX), 
				(uint32)((gRDP.scissor.bottom-gRDP.scissor.top+1)*windowSetting.fMultY), 0, 1
		};
		if( !gRSP.bNearClip )
			vp.MinZ = -10000;
#ifndef _OLDCLIPPER
		if( vp.Width+vp.X > (DWORD)windowSetting.uDisplayWidth-1) vp.Width = windowSetting.uDisplayWidth-1-vp.X;
		if( vp.Height+vp.Y > (DWORD)windowSetting.uDisplayHeight-1) vp.Height = windowSetting.uDisplayHeight-1-vp.Y;
#endif
		gD3DDevWrapper.SetViewport(&vp);
	}
#ifndef _OLDCLIPPER
	status.curScissor = RDP_SCISSOR;
#endif
}

void D3DRender::ApplyScissorWithClipRatio(bool force)
{
#ifndef _OLDCLIPPER
	if( !force && status.curScissor == RSP_SCISSOR )	return;
#endif

	MYD3DVIEWPORT vp = {
		(uint32)(gRSP.real_clip_scissor_left*windowSetting.fMultX), 
			(uint32)(gRSP.real_clip_scissor_top*windowSetting.fMultY), 
			(uint32)((gRSP.real_clip_scissor_right-gRSP.real_clip_scissor_left+1)*windowSetting.fMultX), 
			(uint32)((gRSP.real_clip_scissor_bottom-gRSP.real_clip_scissor_top+1)*windowSetting.fMultY), 0, 1
	};
	if( !gRSP.bNearClip )
		vp.MinZ = -10000;
#ifndef _OLDCLIPPER
	if( vp.Width+vp.X > (DWORD)windowSetting.uDisplayWidth-1) vp.Width = windowSetting.uDisplayWidth-1-vp.X;
	if( vp.Height+vp.Y > (DWORD)windowSetting.uDisplayHeight-1) vp.Height = windowSetting.uDisplayHeight-1-vp.Y;
#endif
	gD3DDevWrapper.SetViewport(&vp);
	
#ifndef _OLDCLIPPER
	status.curScissor = RSP_SCISSOR;
#endif
}

void D3DRender::BeginRendering(void) 
{
	if( CRender::gRenderReferenceCount == 0 )
	{
		g_pD3DDev->BeginScene();
		D3DXMATRIX mat;
		D3DXMatrixIdentity(&mat);
		g_pD3DDev->SetTransform( D3DTS_WORLD, &mat );
		g_pD3DDev->SetTransform( D3DTS_PROJECTION, &mat );
		g_pD3DDev->SetTransform( D3DTS_VIEW, &mat );
	}

	CRender::gRenderReferenceCount++; 
}

void D3DRender::CaptureScreen(char *filename)
{
	RECT rect={0,windowSetting.toolbarHeight,windowSetting.uDisplayWidth,windowSetting.uDisplayHeight+windowSetting.toolbarHeight};

	MYLPDIRECT3DSURFACE surface;

// Copy the screen to a surface
#if DX_VERSION == 8
	g_pD3DDev->GetRenderTarget(&surface);
#elif DX_VERSION == 9
	g_pD3DDev->GetRenderTarget(0, &surface);
#endif

// Save the Surface to a file
#ifdef _XBOX
	XGWriteSurfaceToFile(surface, filename);
#else
	//D3DXSaveSurfaceToFile(filename,D3DXIFF_BMP,surface,NULL,NULL);
	//D3DXSaveSurfaceToFile(filename,D3DXIFF_BMP,surface,NULL,&rect);
#endif

	surface->Release();
}

void D3DRender::SetCullMode(bool bCullFront, bool bCullBack)
{
	CRender::SetCullMode(bCullFront, bCullBack);
	/*
	if( bCullFront && bCullBack )
	{
		gD3DDevWrapper.SetRenderState( D3DRS_CULLMODE,   D3DCULL_CW );
		gD3DDevWrapper.SetRenderState( D3DRS_CULLMODE,   D3DCULL_CCW );
	}
	else if( bCullFront )
	{
		gD3DDevWrapper.SetRenderState( D3DRS_CULLMODE,   D3DCULL_CW );
	}
	else if( bCullBack )
	{
		gD3DDevWrapper.SetRenderState( D3DRS_CULLMODE,   D3DCULL_CCW );
	}
	else
	{
		gD3DDevWrapper.SetRenderState( D3DRS_CULLMODE,   D3DCULL_NONE );
	}
	*/
}

#if DX_VERSION == 8
void D3DRender::D3DSetMinFilter(uint32 dwStage, uint32 filter)
{
	if( filter == D3DTEXF_LINEAR && options.DirectXAnisotropyValue > 0 )
	{
		// Use Anisotropy filter instead of LINEAR filter
		gD3DDevWrapper.SetTextureStageState( dwStage, D3DTSS_MINFILTER, D3DTEXF_ANISOTROPIC  );
		gD3DDevWrapper.SetTextureStageState( dwStage, D3DTSS_MAXANISOTROPY, min(options.DirectXAnisotropyValue, (uint32)CGraphicsContext::m_maxAnisotropy) );
	}
	else
		gD3DDevWrapper.SetTextureStageState( dwStage, D3DTSS_MINFILTER, filter );
}
#elif DX_VERSION == 9
void D3DRender::D3DSetMinFilter(uint32 dwSampler, uint32 filter)
{
	if( filter == D3DTEXF_LINEAR && options.DirectXAnisotropyValue > 0 )
	{
		// Use Anisotropy filter instead of LINEAR filter
		gD3DDevWrapper.SetSamplerState( dwSampler, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC  );
		gD3DDevWrapper.SetSamplerState( dwSampler, D3DSAMP_MAXANISOTROPY, min(options.DirectXAnisotropyValue, (uint32)CGraphicsContext::m_maxAnisotropy) );
	}
	else
		gD3DDevWrapper.SetSamplerState( dwSampler, D3DSAMP_MINFILTER, filter );
}
#endif

#if DX_VERSION == 8
void D3DRender::D3DSetMagFilter(uint32 dwStage, uint32 filter)
{
	if( filter == D3DTEXF_LINEAR && options.DirectXAnisotropyValue > 0 )
	{
		// Use Anisotropy filter instead of LINEAR filter
		gD3DDevWrapper.SetTextureStageState( dwStage, D3DTSS_MAGFILTER, D3DTEXF_ANISOTROPIC  );
		gD3DDevWrapper.SetTextureStageState( dwStage, D3DTSS_MAXANISOTROPY, min(options.DirectXAnisotropyValue, (uint32)CGraphicsContext::m_maxAnisotropy) );
	}
	else
		gD3DDevWrapper.SetTextureStageState( dwStage, D3DTSS_MAGFILTER, filter );
}
#elif DX_VERSION == 9
void D3DRender::D3DSetMagFilter(uint32 dwSampler, uint32 filter)
{
	if( filter == D3DTEXF_LINEAR && options.DirectXAnisotropyValue > 0 )
	{
		// Use Anisotropy filter instead of LINEAR filter
		gD3DDevWrapper.SetSamplerState( dwSampler, D3DSAMP_MAGFILTER, D3DTEXF_ANISOTROPIC  );
		gD3DDevWrapper.SetSamplerState( dwSampler, D3DSAMP_MAXANISOTROPY, min(options.DirectXAnisotropyValue, (uint32)CGraphicsContext::m_maxAnisotropy) );
	}
	else
		gD3DDevWrapper.SetSamplerState( dwSampler, D3DSAMP_MAGFILTER, filter );
}
#endif

