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

// This is global in Rice6 for XBOX
#ifdef _XBOX
#include <xgraphics.h>
extern BYTE g_ucTempBuffer[1024*1024*4];

#endif

CDirectXTexture::CDirectXTexture(uint32 dwWidth, uint32 dwHeight, TextureUsage usage) :
	CTexture(dwWidth,dwHeight,usage)
{
	MYLPDIRECT3DTEXTURE pTxt;

	if (dwWidth < 1)
		dwWidth = 1;
	else if (dwWidth > 1024)
		dwWidth = 1024;

	if (dwHeight < 1)
		dwHeight = 1;
	else if (dwHeight > 1024)
		dwHeight = 1024;

	pTxt = (MYLPDIRECT3DTEXTURE)CreateTexture(dwWidth, dwHeight, usage);
	if (pTxt == NULL)
		return;

	// Copy from old surface to new surface
	if (m_pTexture != NULL)
	{
		MYLPDIRECT3DTEXTURE(m_pTexture)->Release();
		m_pTexture = NULL;
	}

	m_dwWidth = dwWidth;
	m_dwHeight = dwHeight;
	m_pTexture = pTxt;
}
	
CDirectXTexture::~CDirectXTexture()
{
	if (m_pTexture)
	{
		MYLPDIRECT3DTEXTURE(m_pTexture)->Release();
		m_pTexture = NULL;
	}
	m_dwWidth = 0;
	m_dwHeight = 0;
}


//////////////////////////////////////////////////
// Get information about the DIBitmap
// This locks the bitmap (and stops 
// it from being resized). Must be matched by a
// call to EndUpdate();

bool CDirectXTexture::StartUpdate(DrawInfo *di)
{
	if (m_pTexture == NULL)
		return false;
#ifndef _XBOX // XBOX goes not allow D3DLOCK_NOSYSLOCK in LockRect()
	D3DLOCKED_RECT d3d_lr;
	HRESULT hr = MYLPDIRECT3DTEXTURE(m_pTexture)->LockRect(0, &d3d_lr, NULL, D3DLOCK_NOSYSLOCK);
	if (SUCCEEDED(hr))
	{
		di->dwHeight = (uint16)m_dwHeight;
		di->dwWidth = (uint16)m_dwWidth;
		di->dwCreatedHeight = m_dwCreatedTextureHeight;
		di->dwCreatedWidth = m_dwCreatedTextureWidth;
		di->lpSurface = d3d_lr.pBits;
		di->lPitch    = d3d_lr.Pitch;
		return true;
	}
	else
	{
		return false;
	}

	if( pTempbuffer )
	{
		delete [] pTempbuffer;
		pTempbuffer = NULL;
	}

	pTempbuffer = new BYTE[m_dwCreatedTextureHeight*m_dwCreatedTextureWidth*GetPixelSize()];
	if( !pTempbuffer )
		return false;
	di->lpSurface = pTempbuffer;
#else
	di->lpSurface = g_ucTempBuffer; //pTempbuffer
	di->dwHeight = (uint16)m_dwHeight;
	di->dwWidth = (uint16)m_dwWidth;
	di->dwCreatedHeight = m_dwCreatedTextureHeight;
	di->dwCreatedWidth = m_dwCreatedTextureWidth;
	di->lPitch    = m_dwCreatedTextureWidth * GetPixelSize();;
#endif
	return true;
}

///////////////////////////////////////////////////
// This releases the DIB information, allowing it
// to be resized again
void CDirectXTexture::EndUpdate(DrawInfo *di)
{
	if (m_pTexture == NULL)
		return;
#ifdef _XBOX
	D3DLOCKED_RECT d3d_lr;
	HRESULT hr;

	hr = MYLPDIRECT3DTEXTURE(m_pTexture)->LockRect(0, &d3d_lr, NULL, 0);
	if (SUCCEEDED(hr))
	{
		XGSwizzleRect( g_ucTempBuffer, 0, NULL, d3d_lr.pBits,
			di->dwCreatedWidth, di->dwCreatedHeight, 
			NULL, GetPixelSize() );
		MYLPDIRECT3DTEXTURE(m_pTexture)->UnlockRect( 0 ); // dipset
	}
#else //#endif

	// Rice6 doesn't use this line for XBOX, but why
	// would you lock the rect and never unlock it?
	// We'll see what happens...
	// Also used in Rice 5.60
	MYLPDIRECT3DTEXTURE(m_pTexture)->UnlockRect( 0 );
#endif
}


LPRICETEXTURE CDirectXTexture::CreateTexture(uint32 dwWidth, uint32 dwHeight, TextureUsage usage)
{
	HRESULT hr;
	MYLPDIRECT3DTEXTURE lpSurf = NULL;
	unsigned int dwNumMaps = 1;

#ifdef _XBOX
	D3DFORMAT pf = D3DFMT_A4R4G4B4;
#else
	D3DFORMAT pf = ((CDXGraphicsContext*)(CGraphicsContext::g_pGraphicsContext))->GetFormat();
#endif
	switch( pf )
	{
	case D3DFMT_R5G6B5:
	case D3DFMT_X1R5G5B5:
	case D3DFMT_A1R5G5B5:
	case D3DFMT_A4R4G4B4:
#ifndef _XBOX
	case D3DFMT_X4R4G4B4:
#endif
		switch(usage)
		{
		case AS_BACK_BUFFER_SAVE:
#ifdef _XBOX
			pf = D3DFMT_A4R4G4B4;
#else
			pf = D3DFMT_X4R4G4B4;
#endif
			break;
		case AS_RENDER_TARGET:
#ifdef _XBOX
			pf = D3DFMT_A4R4G4B4;
#else
			pf = D3DFMT_X4R4G4B4;
#endif
			break;
		default:
			if( options.textureQuality == TXT_QUALITY_32BIT )
				pf = D3DFMT_A8R8G8B8;
			else
				pf = D3DFMT_A4R4G4B4;
			break;
		}
		break;
	default:
		switch(usage)
		{
		case AS_BACK_BUFFER_SAVE:
			pf = D3DFMT_X8R8G8B8;
			break;
		case AS_RENDER_TARGET:
#ifdef _XBOX
			pf = D3DFMT_LIN_X8R8G8B8;
#else
			pf = D3DFMT_A8R8G8B8;
#endif
			break;
		default:
			if( options.textureQuality == TXT_QUALITY_16BIT )
				pf = D3DFMT_A4R4G4B4;
			else
				pf = D3DFMT_A8R8G8B8;
			break;
		}
		break;
	}

	m_dwCreatedTextureWidth = dwWidth;
	m_dwCreatedTextureHeight = dwHeight;

	m_dwWidth = dwWidth;
	m_dwHeight = dwHeight;

	if( m_Usage == AS_RENDER_TARGET)
	{
#ifdef _XBOX_HACK2
		IDirect3D8 *d3d;
		g_pD3DDev->GetDirect3D(&d3d);
		hr = d3d->CheckDeviceFormat(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, D3DFMT_X8R8G8B8, D3DUSAGE_RENDERTARGET, D3DRTYPE_SURFACE, pf);

		if (SUCCEEDED(hr))
		{
			D3DXCheckTextureRequirements(g_pD3DDev, &m_dwCreatedTextureWidth, &m_dwCreatedTextureHeight, &dwNumMaps, D3DUSAGE_RENDERTARGET, &pf, D3DPOOL_DEFAULT);
			hr = D3DXCreateTexture(g_pD3DDev, m_dwCreatedTextureWidth, m_dwCreatedTextureHeight, 1, D3DUSAGE_RENDERTARGET, pf, D3DPOOL_DEFAULT  , &lpSurf);
		}
#else
		D3DXCheckTextureRequirements(g_pD3DDev, &m_dwCreatedTextureWidth, &m_dwCreatedTextureHeight, &dwNumMaps, D3DUSAGE_RENDERTARGET, &pf, D3DPOOL_DEFAULT);
		hr = D3DXCreateTexture(g_pD3DDev, m_dwCreatedTextureWidth, m_dwCreatedTextureHeight, 1, D3DUSAGE_RENDERTARGET, pf, D3DPOOL_DEFAULT  , &lpSurf);
#endif
	}
	else
	{
		D3DXCheckTextureRequirements(g_pD3DDev, &m_dwCreatedTextureWidth, &m_dwCreatedTextureHeight, &dwNumMaps, 0, &pf, D3DPOOL_MANAGED);
		hr = D3DXCreateTexture(g_pD3DDev, m_dwCreatedTextureWidth, m_dwCreatedTextureHeight, 1, 0, pf, D3DPOOL_MANAGED  , &lpSurf);
	}

	m_fYScale = (float)m_dwCreatedTextureHeight/(float)m_dwHeight;
	m_fXScale = (float)m_dwCreatedTextureWidth/(float)m_dwWidth;

	// D3D likes texture w/h to be a power of two, so the condition below
	// will almost always hold. 
	// D3D should usually create textures large enough (on nVidia cards anyway),
	// and so there will usually be some "slack" left in the texture (blank space
	// that isn't used). The D3DRender code takes care of this (mostly)
	// Voodoo cards are limited to 256x256 and so often textures will be
	// created that are too small for the required dimensions. We compensate for
	// this by passing in a dummy area of memory when the surface is locked,
	// and copying across pixels to the real surface when the surface is unlocked.
	// In this case there will be no slack and D3DRender takes this into account.
	// 

	// HACK - we should only assign this when m_pTexture is assigned!
	
	if( pf == D3DFMT_A8R8G8B8 || pf == D3DFMT_X8R8G8B8  || pf == D3DFMT_LIN_X8R8G8B8 || pf == D3DFMT_LIN_A8R8G8B8) // Add Linear modes for XBOX
		m_dwTextureFmt = TEXTURE_FMT_A8R8G8B8;
	else
		m_dwTextureFmt = TEXTURE_FMT_A4R4G4B4;

	if( !CDXGraphicsContext::IsResultGood(hr))
	{
		return NULL;
	}
	
	return lpSurf;		
}

CDirectXTexture *CDirectXTexture::DuplicateTexture()
{
	CDirectXTexture *dsttxtr = new CDirectXTexture(m_dwWidth, m_dwHeight, AS_NORMAL);
	if( dsttxtr )
	{
		MYLPDIRECT3DSURFACE pSrc;
		MYLPDIRECT3DSURFACE pDst;
		(MYLPDIRECT3DTEXTURE(GetTexture()))->GetSurfaceLevel(0,&pSrc);
		(MYLPDIRECT3DTEXTURE(dsttxtr->GetTexture()))->GetSurfaceLevel(0,&pDst);
#if DX_VERSION == 8
		g_pD3DDev->CopyRects(pSrc,NULL,0,pDst,NULL);
#elif DX_VERSION == 9
		g_pD3DDev->UpdateSurface(pSrc,NULL,pDst,NULL);
#endif
		pSrc->Release();
		pDst->Release();
	}

	return dsttxtr;
}

void CDirectXTexture::RestoreAlphaChannel(void)
{
	DrawInfo di;

	if ( StartUpdate(&di) )
	{
		EndUpdate(&di);
		CTexture::RestoreAlphaChannel();
		return;
	}
	else
	{
		CDirectXTexture *duptxtr = DuplicateTexture();
		if( duptxtr )
		{
			duptxtr->CTexture::RestoreAlphaChannel();

			MYLPDIRECT3DSURFACE pSrc;
			MYLPDIRECT3DSURFACE pDst;
			(MYLPDIRECT3DTEXTURE(GetTexture()))->GetSurfaceLevel(0,&pDst);
			(MYLPDIRECT3DTEXTURE(duptxtr->GetTexture()))->GetSurfaceLevel(0,&pSrc);
			HRESULT res;
#if DX_VERSION == 8
			res = g_pD3DDev->CopyRects(pSrc,NULL,0,pDst,NULL);
#elif DX_VERSION == 9
			res = g_pD3DDev->UpdateSurface(pSrc,NULL,pDst,NULL);
#endif
			//res = D3DXLoadSurfaceFromSurface(pDst,NULL,NULL,pSrc,NULL,NULL,D3DX_FILTER_POINT,0xFF000000);
			pSrc->Release();
			pDst->Release();

			if( res != S_OK )
			{
			}

			delete duptxtr;
		}
	}
}