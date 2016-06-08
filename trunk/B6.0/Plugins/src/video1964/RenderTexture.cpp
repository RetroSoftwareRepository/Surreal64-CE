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

// ===========================================================================

#include "stdafx.h"
//#include "glh_genext.h"


CDXTextureBuffer::CDXTextureBuffer(int width, int height, TextureBufferInfo* pInfo, TextureUsage usage)
	: CTextureBuffer(width, height, pInfo, usage)
{
	m_pTexture = new CDirectXTexture(width, height, usage);
	if( m_pTexture )
	{
		m_width = width;
		m_height = height;
	}
	else
	{
		SAFE_DELETE(m_pTexture);
	}

	m_pColorBufferSave = NULL;
	m_pDepthBufferSave = NULL;
	m_beingRendered = false;
}

CDXTextureBuffer::~CDXTextureBuffer()
{
	if( m_beingRendered )
	{
#ifdef _RICE6FB
		g_pFrameBufferManager->CloseRenderTexture(false);
#else
		CGraphicsContext::g_pGraphicsContext->CloseTextureBuffer(false);
#endif
		SetAsRenderTarget(false);
	}

	SAFE_DELETE(m_pTexture);
	
	m_beingRendered = false;
}

bool CDXTextureBuffer::SetAsRenderTarget(bool enable)
{
	if( m_usage != AS_RENDER_TARGET )	return false;

	if( enable )
	{
		if( !m_beingRendered )
		{
			if(m_pTexture )
			{
				MYLPDIRECT3DSURFACE pColorBuffer;

				// Save the current back buffer
#if DX_VERSION == 8
				g_pD3DDev->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &m_pColorBufferSave);
#elif DX_VERSION == 9
				g_pD3DDev->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &m_pColorBufferSave);
#endif
				g_pD3DDev->GetDepthStencilSurface(&m_pDepthBufferSave);

				// Activate the texture buffer
				(MYLPDIRECT3DTEXTURE(m_pTexture->GetTexture()))->GetSurfaceLevel(0,&pColorBuffer);
#if DX_VERSION == 8
				HRESULT res = g_pD3DDev->SetRenderTarget(pColorBuffer, NULL);
#elif DX_VERSION == 9
				HRESULT res = g_pD3DDev->SetRenderTarget(0, pColorBuffer);
#endif
				SAFE_RELEASE(pColorBuffer);
				if( res != S_OK )
				{
					return false;
				}

				m_beingRendered = true;
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			return true;
		}
	}
	else
	{
		if( m_beingRendered )
		{
			if( m_pColorBufferSave && m_pDepthBufferSave )
			{
#if DX_VERSION == 8
				g_pD3DDev->SetRenderTarget(m_pColorBufferSave, m_pDepthBufferSave);
#elif DX_VERSION == 9
g_pD3DDev->SetRenderTarget(0, m_pColorBufferSave);
				g_pD3DDev->SetDepthStencilSurface(m_pDepthBufferSave);
#endif
				m_beingRendered = false;
				SAFE_RELEASE(m_pColorBufferSave);
				SAFE_RELEASE(m_pDepthBufferSave);
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			return true;
		}
	}
}

void CDXTextureBuffer::LoadTexture(TxtrCacheEntry* pEntry)
{
	bool IsBeingRendered = m_beingRendered;
	if( IsBeingRendered )
	{
		SetAsRenderTarget(false);
		//return;
	}

	CTexture *pSurf = pEntry->pTexture;
	TxtrInfo &ti = pEntry->ti;

	// Need to load texture from the saved back buffer

	MYLPDIRECT3DTEXTURE pNewTexture = MYLPDIRECT3DTEXTURE(pSurf->GetTexture());
	MYLPDIRECT3DSURFACE pNewSurface = NULL;
	HRESULT res = pNewTexture->GetSurfaceLevel(0,&pNewSurface);
	MYLPDIRECT3DSURFACE pSourceSurface = NULL;
	(MYLPDIRECT3DTEXTURE(m_pTexture->GetTexture()))->GetSurfaceLevel(0,&pSourceSurface);

	int left = (pEntry->ti.Address- m_pInfo->CI_Info.dwAddr )%m_pInfo->CI_Info.bpl + pEntry->ti.LeftToLoad;
	int top = (pEntry->ti.Address- m_pInfo->CI_Info.dwAddr)/m_pInfo->CI_Info.bpl + pEntry->ti.TopToLoad;
	RECT srcrect = {uint32(left*m_pInfo->scaleX) ,uint32(top*m_pInfo->scaleY), 
		uint32(min(m_width, left+(int)ti.WidthToLoad)*m_pInfo->scaleX), 
		uint32(min(m_height,top+(int)ti.HeightToLoad)*m_pInfo->scaleY) };

	if( pNewSurface != NULL && pSourceSurface != NULL )
	{
		if( left < m_width && top<m_height )
		{
			RECT dstrect = {0,0,ti.WidthToLoad,ti.HeightToLoad};
			HRESULT res = D3DXLoadSurfaceFromSurface(pNewSurface,NULL,&dstrect,pSourceSurface,NULL,&srcrect,D3DX_FILTER_POINT ,0xFF000000);
		}
	}

	if( IsBeingRendered )
	{
		SetAsRenderTarget(true);
	}

	pSurf->SetOthersVariables();
	SAFE_RELEASE(pSourceSurface);
}

#ifdef _RICE6FB
void CDXTextureBuffer::StoreTextureBufferToRDRAM(int infoIdx)
#else
void CDXGraphicsContext::StoreTextureBufferToRDRAM(int infoIdx)
#endif
{
#ifndef _RICE6FB
	if( infoIdx < 0 )
		infoIdx = m_lastTextureBufferIndex;

	if( gTextureBufferInfos[infoIdx].pTxtBuffer && gTextureBufferInfos[infoIdx].pTxtBuffer->IsBeingRendered() )
	{
		return;
	}

	if( gTextureBufferInfos[infoIdx].pTxtBuffer )
#else

	if( !frameBufferOptions.bTextureBufferWriteBack )	return;

	RenderTextureInfo &info = gTextueBufferInfos[infoIdx];
	DXFrameBufferManager &FBmgr = *(DXFrameBufferManager*)g_pFrameBufferManager;
	if(1)
#endif
	{
		TextureBufferInfo &info = gTextureBufferInfos[infoIdx];

		uint32 fmt = info.CI_Info.dwFormat;
		if( frameBufferOptions.bTxtBufWriteBack )
		{
			CDXTextureBuffer* pTxtBuffer = (CDXTextureBuffer*)(info.pTxtBuffer);
			MYLPDIRECT3DSURFACE pSourceSurface = NULL;
			(MYLPDIRECT3DTEXTURE(pTxtBuffer->m_pTexture->GetTexture()))->GetSurfaceLevel(0,&pSourceSurface);

			// Ok, we are using texture render target right now
			// Need to copy content from the texture render target back to frame buffer
			// then reset the current render target

			// Here we need to copy the content from the texture frame buffer to RDRAM memory

			if( pSourceSurface )
			{
				uint32 width, height, bufWidth, bufHeight, memsize; 
				width = info.N64Width;
				height = info.N64Height;
				bufWidth = info.bufferWidth;
				bufHeight = info.bufferHeight;
				if( info.CI_Info.dwSize == TXT_SIZE_8b && fmt == TXT_FMT_CI )
				{
					info.CI_Info.dwFormat = TXT_FMT_I;
					height = info.knownHeight ? info.N64Height : info.maxUsedHeight;
					memsize = info.N64Width*height;
#ifdef _RICE6FB
					FBmgr.CopyD3DSurfaceToRDRAM(info.CI_Info.dwAddr, fmt, info.CI_Info.dwSize, width, height,
						bufWidth, bufHeight, info.CI_Info.dwAddr, memsize, info.N64Width, D3DFMT_A8R8G8B8, pSourceSurface);
#else
					CopyBackToRDRAM(info.CI_Info.dwAddr, fmt, info.CI_Info.dwSize, width, height,
						bufWidth, bufHeight, info.CI_Info.dwAddr, memsize, info.N64Width, D3DFMT_A8R8G8B8, pSourceSurface);
#endif
					info.CI_Info.dwFormat = TXT_FMT_CI;
				}
				else
				{
					if( info.CI_Info.dwSize == TXT_SIZE_8b )
					{
						height = info.knownHeight ? info.N64Height : info.maxUsedHeight;
						memsize = info.N64Width*height;
#ifdef _RICE6FB
						FBmgr.CopyD3DSurfaceToRDRAM(info.CI_Info.dwAddr, fmt, info.CI_Info.dwSize, width, height,
							bufWidth, bufHeight, info.CI_Info.dwAddr, memsize, info.N64Width, D3DFMT_A8R8G8B8, pSourceSurface);
#else
						CopyBackToRDRAM(info.CI_Info.dwAddr, fmt, info.CI_Info.dwSize, width, height,
							bufWidth, bufHeight, info.CI_Info.dwAddr, memsize, info.N64Width, D3DFMT_A8R8G8B8, pSourceSurface);
#endif
					}
					else
					{
						height = info.knownHeight ? info.N64Height : info.maxUsedHeight;
						memsize = g_pTxtBufferInfo->N64Width*height*2;
#ifdef _RICE6FB
						FBmgr.CopyD3DSurfaceToRDRAM(info.CI_Info.dwAddr, fmt, info.CI_Info.dwSize, width, height,
							bufWidth, bufHeight, info.CI_Info.dwAddr, memsize, info.N64Width, D3DFMT_X8R8G8B8, pSourceSurface);
#else
						CopyBackToRDRAM(info.CI_Info.dwAddr, fmt, info.CI_Info.dwSize, width, height,
							bufWidth, bufHeight, info.CI_Info.dwAddr, memsize, info.N64Width, D3DFMT_X8R8G8B8, pSourceSurface);
#endif
					}
				}
				SAFE_RELEASE(pSourceSurface);
			}
			else
			{
				DebuggerAppendMsg("Error, cannot lock the texture buffer");
			}
		}

	}

}

#ifdef _DEBUG
void CDXRenderTexture::Display(int idx)
{
	MYLPDIRECT3DSURFACE pSourceSurface = NULL;
	(MYLPDIRECT3DTEXTURE(m_pTexture->GetTexture()))->GetSurfaceLevel(0,&pSourceSurface);
	char filename[256];
	sprintf(filename,"\\DxTxtBuf%d",idx);
	((CDXGraphicsContext*)CGraphicsContext::g_pGraphicsContext)->SaveSurfaceToFile(filename, pSourceSurface);
	SAFE_RELEASE(pSourceSurface);
}
#endif

