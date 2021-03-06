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

#ifndef _OGL_CONTEXT_H_
#define _OGL_CONTEXT_H_

class COGLGraphicsContext : public CGraphicsContext
{
	friend class OGLRender;
public:
	virtual ~COGLGraphicsContext();

	bool Initialize(HWND hWnd, HWND hWndStatus, DWORD dwWidth, DWORD dwHeight, BOOL bWindowed );
	void CleanUp();
	void Clear(DWORD dwFlags);

	void UpdateFrame(bool swaponly=false);
	int ToggleFullscreen();		// return 0 as the result is windowed

	void DumpScreenShot();
	bool IsExtensionSupported(const char* pExtName);
	bool IsWglExtensionSupported(const char* pExtName);

	void SetFrontBuffer(DWORD viorg);	// Change front buffer to the viorg addr
	void SetBackBuffer(DWORD ciaddr);	// Change back buffer to the ci addr
	void SwapBuffer();


protected:
	friend class OGLDeviceBuilder;
	COGLGraphicsContext();
	void InitState(void);
	void InitOGLExtension(void);
	bool SetFullscreenMode();
	bool SetWindowMode();

	HDC		m_hDC;
	HGLRC	m_hGLRC;
	
	// Important OGL extension features
	bool	m_bSupportMultiTexture;
	bool	m_bSupportTextureEnvCombine;
	bool	m_bSupportSeparateSpecularColor;
	bool	m_bSupportSecondColor;
	bool	m_bSupportFogCoord;
	bool	m_bSupportTextureObject;

	// Optional OGL extension features;
	bool	m_bSupportRescaleNormal;
	bool	m_bSupportLODBias;

	// Nvidia OGL only features
	bool	m_bSupportTextureMirrorRepeat;
	bool	m_bSupportTextureLOD;
	bool	m_bSupportNVRegisterCombiner;
	bool	m_bSupportBlendColor;
	bool	m_bSupportBlendSubtract;
	bool	m_bSupportNVTextureEnvCombine4;
	
	// Minimal requirements, I will even not check them at runtime
	//bool	m_bSupportTextureEnvAdd;
	//bool	m_bSupportVertexArray;


	const unsigned char*	m_pVendorStr;
	const unsigned char*	m_pRenderStr;
	const unsigned char*	m_pExtensionStr;
	const char*	m_pWglExtensionStr;
	const unsigned char*	m_pVersionStr;
};

#endif



