#include "../StdAfx.h"
#include "MenuMain.h"
#include "D3DSave.h"

static void SetupD3D()
{
	D3DXMATRIX mat;
	D3DXMatrixOrthoOffCenterLH(&mat, 0, 640, 480, 0, 0.0f, 1.0f);
	g_pD3DDev->SetTransform(D3DTS_PROJECTION, &mat);

	// use an identity matrix for the world and view matrices
	D3DXMatrixIdentity(&mat);
	g_pD3DDev->SetTransform(D3DTS_WORLD, &mat);
	g_pD3DDev->SetTransform(D3DTS_VIEW, &mat);

	// disable lighting
	g_pD3DDev->SetRenderState(D3DRS_LIGHTING, FALSE);
}

extern void MainMenu(void);

void RunIngameMenu()
{
	// save and init d3d
	g_d3dSave.PushRenderState();

	SetupD3D();

	MainMenu();
	
	g_d3dSave.PopRenderState();

}

