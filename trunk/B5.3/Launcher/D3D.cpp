/**
 * Surreal 64 Launcher (C) 2003
 * 
 * This program is free software; you can redistribute it and/or modify it under 
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option) any later
 * version. This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details. You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA. To contact the
 * authors: email: buttza@hotmail.com, lantus@lantus-x.com
 */

#include "D3D.h"
#include <stdlib.h>
#include <io.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string.h>
#include <stdio.h>
#include <xbapp.h>

#include "../config.h"

D3D g_d3d;
HRESULT g_hResult;

D3D::D3D(void)
{
}

D3D::~D3D(void)
{
}

bool D3D::Create()
{
	// create the direct 3d object
	m_pD3D = Direct3DCreate8(D3D_SDK_VERSION);

	if (m_pD3D == NULL)
		return false;

    // set up the structure used to create the d3d device
    D3DPRESENT_PARAMETERS d3dpp;
    ZeroMemory(&d3dpp, sizeof(d3dpp));

    d3dpp.BackBufferWidth					= 640;
    d3dpp.BackBufferHeight					= 480;
    d3dpp.BackBufferFormat					= D3DFMT_A8R8G8B8;
    d3dpp.BackBufferCount					= 1;
    d3dpp.SwapEffect						= D3DSWAPEFFECT_DISCARD;
    d3dpp.FullScreen_PresentationInterval	= D3DPRESENT_INTERVAL_ONE_OR_IMMEDIATE;

/*	g_hResult = m_pD3D->CreateDevice(0, D3DDEVTYPE_HAL, NULL, 
									 D3DCREATE_HARDWARE_VERTEXPROCESSING,
									 &d3dpp, &m_pd3dDevice);*/

	if (FAILED(g_hResult))
		return false;

	m_pd3dDevice = g_pd3dDevice;

    // use an orthogonal matrix for the projection matrix
    D3DXMATRIX mat;
    D3DXMatrixOrthoOffCenterLH(&mat, 0, 640, 480, 0, 0.0f, 1.0f);
    m_pd3dDevice->SetTransform(D3DTS_PROJECTION, &mat);

	// use an identity matrix for the world and view matrices
	D3DXMatrixIdentity(&mat);
	m_pd3dDevice->SetTransform(D3DTS_WORLD, &mat);
	m_pd3dDevice->SetTransform(D3DTS_VIEW, &mat);

	// disable lighting
	m_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);


	return true;
}

void D3D::BeginRender()
{
    m_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, 
                      D3DCOLOR_XRGB(0,0,0), 
						1.0f, 0);

    m_pd3dDevice->BeginScene();
	m_pd3dDevice->SetFlickerFilter(FlickerFilter);
	m_pd3dDevice->SetSoftDisplayFilter(SoftDisplayFilter);
}

void D3D::EndRender()
{
	m_pd3dDevice->EndScene();

	m_pd3dDevice->Present(NULL, NULL, NULL, NULL);
}



