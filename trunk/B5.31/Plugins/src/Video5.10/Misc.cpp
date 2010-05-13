//freakdave
#include "stdafx.h"
//#include "./Menu/MenuMain.h"
//#include "./Menu/IngameMenu.h"

BOOL g_bMenuWaiting = FALSE;
BOOL g_bTempMessage = FALSE;
DWORD g_dwTempMessageStart = 0;
char g_szTempMessage[100];
//XFONT *g_defaultTrueTypeFont = NULL;

extern BOOL _INPUT_IsIngameMenuWaiting();

void XboxCheckMenuAndDebugInfo()
{
/*	g_bMenuWaiting = _INPUT_IsIngameMenuWaiting();

	// output free mem
	if (g_showDebugInfo && !g_bMenuWaiting)
	{
		if (g_defaultTrueTypeFont == NULL)
		{
			XFONT_OpenDefaultFont(&g_defaultTrueTypeFont);
			g_defaultTrueTypeFont->SetBkMode(XFONT_OPAQUE);
			g_defaultTrueTypeFont->SetBkColor(D3DCOLOR_XRGB(0,0,0));
		}

		static DWORD lastTick = GetTickCount() / 1000;
		static int lastTickFPS = 0;
		static int frameCount = 0;

		if (lastTick != GetTickCount() / 1000)
		{
			lastTickFPS = frameCount;
			frameCount = 0;
			lastTick = GetTickCount() / 1000;
		}

		frameCount++;

		static MEMORYSTATUS stat;
		GlobalMemoryStatus(&stat);

		WCHAR buf[255];
		D3DSurface *pBackBuffer, *pFrontBuffer;

		g_pD3DDev->GetBackBuffer(-1, D3DBACKBUFFER_TYPE_MONO, &pFrontBuffer);
		g_pD3DDev->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer);

		swprintf(buf, L"%.2fMB free - %ifps", stat.dwAvailPhys/(1024.0f*1024.0f), lastTickFPS);

		g_defaultTrueTypeFont->TextOut(pFrontBuffer, buf, (unsigned)-1, 40, 40);
		g_defaultTrueTypeFont->TextOut(pBackBuffer, buf, (unsigned)-1, 40, 40);

		pFrontBuffer->Release();
		pBackBuffer->Release();
	}*/
}

void XboxDrawTemporaryMessage()
{
/*	if (g_bTempMessage)
	{
		if (g_defaultTrueTypeFont == NULL)
		{
			XFONT_OpenDefaultFont(&g_defaultTrueTypeFont);
			g_defaultTrueTypeFont->SetBkMode(XFONT_OPAQUE);
			g_defaultTrueTypeFont->SetBkColor(D3DCOLOR_XRGB(0,0,0));
		}

		if (GetTickCount() > g_dwTempMessageStart + 3000)
		{
			g_bTempMessage = FALSE;
		}

		WCHAR buf[200];
		D3DSurface *pBackBuffer, *pFrontBuffer;

		memset(buf, 0, sizeof(WCHAR) * 200);

		g_pD3DDev->GetBackBuffer(-1, D3DBACKBUFFER_TYPE_MONO, &pFrontBuffer);
		g_pD3DDev->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer);

		mbstowcs(buf, g_szTempMessage, strlen(g_szTempMessage));

		g_defaultTrueTypeFont->TextOut(pFrontBuffer, buf, (unsigned)-1, 40, 410);
		g_defaultTrueTypeFont->TextOut(pBackBuffer, buf, (unsigned)-1, 40, 410);

		pFrontBuffer->Release();
		pBackBuffer->Release();
	}*/
}