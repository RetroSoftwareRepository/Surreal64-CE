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

#ifndef _OGL_RENDER_H_
#define _OGL_RENDER_H_

#include "stdafx.h"
#include "Combiner.h"

#include "Unique.h"

class OGLRender : public CDaedalusRender
{
	friend class COGLColorCombiner;
	friend class COGLBlender;
	friend class OGLDeviceBuilder;
	
protected:
	OGLRender();

public:
	~OGLRender();
	void Initialize(void);

	bool RestoreDeviceObjects();
	bool InvalidateDeviceObjects();

	void CaptureScreen(char *filename);

	void ApplyTextureFilter();

	void SetShadeMode(RenderShadeMode mode);
	void ZBufferEnable(BOOL bZBuffer);
	void ClearBuffer(bool cbuffer, bool zbuffer);
	void SetZCompare(BOOL bZCompare);
	void SetZUpdate(BOOL bZUpdate);
	void SetZBias(int bias);
	void SetAlphaRef(DWORD dwAlpha);
	void ForceAlphaRef(DWORD dwAlpha);
	void SetFillMode(DaedalusRenderFillMode mode);
	void SetViewportRender();
	void RenderReset();
	void SetCullMode(bool bCullFront, bool bCullBack);
	void SetAlphaTestEnable(BOOL bAlphaTestEnable);
	void UpdateScissor();
	void ApplyRDPScissor();
	void ApplyScissorWithClipRatio();

	bool SetCurrentTexture(int tile, CTexture *handler,DWORD dwTileWidth, DWORD dwTileHeight);
	bool SetCurrentTexture(int tile, TextureEntry *pTextureEntry);
	void SetAddressUAllStages(DWORD dwTile, TextureUVFlag dwFlag);
	void SetAddressVAllStages(DWORD dwTile, TextureUVFlag dwFlag);
	void SetTextureUFlag(TextureUVFlag dwFlag, DWORD tile);
	void SetTextureVFlag(TextureUVFlag dwFlag, DWORD tile);
	virtual void BindTexture(GLuint texture, int unitno);
	virtual void DisBindTexture(GLuint texture, int unitno);
	virtual void TexCoord2f(float u, float v);
	virtual void TexCoord(TLITVERTEX &vtxInfo);

	void DrawSimple2DTexture(float x0, float y0, float x1, float y1, float u0, float v0, float u1, float v1, DaedalusColor dif, DaedalusColor spe, float z, float rhw);
	void DrawSimpleRect(LONG nX0, LONG nY0, LONG nX1, LONG nY1, DWORD dwColor, float depth, float rhw);
	void InitCombinerBlenderForSimpleRectDraw(DWORD tile=0);
	void DrawSpriteR_Render();
	void DrawObjBGCopy(uObjBg &info);
	void DrawText(const char* str, RECT *rect);

	void SetFogMinMax(float fMin, float fMax, float fMul, float fOffset);
	void SetFogEnable(BOOL bEnable);
	void TurnFogOnOff(BOOL flag);
	void SetFogColor(DWORD r, DWORD g, DWORD b, DWORD a);

	void DisableMultiTexture();
	void EnableMultiTexture() {m_bEnableMultiTexture=true;}
	void EndRendering(void);

	void glViewportWrapper(GLint x, GLint y, GLsizei width, GLsizei height, bool flag=true);
	virtual void EnableTexUnit(int unitno, BOOL flag);
	virtual void SetTexWrapS(int unitno,GLuint flag);
	virtual void SetTexWrapT(int unitno,GLuint flag);

public:
#ifdef _DEBUG
	bool DrawTexture(int tex, bool alphaOnly=false);
	void DrawTlutAsImage(WORD*);
#endif

protected:
	DaedalusColor PostProcessDiffuseColor(DaedalusColor curDiffuseColor);
	DaedalusColor PostProcessSpecularColor();

	// Basic render drawing functions
	bool RenderFlushTris();
	bool RenderTexRect();
	bool RenderFillRect(DWORD dwColor, float depth);
	bool RenderLine3D();

	bool m_bSupportFogCoordExt;
	bool m_bMultiTexture;
	bool m_bSupportClampToEdge;
	bool m_bClampS[2];
	bool m_bClampT[2];

	GLuint	m_curBoundTex[8];
	BOOL	m_texUnitEnabled[8];

	bool m_bEnableMultiTexture;
};

#endif


