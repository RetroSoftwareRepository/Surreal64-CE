#include "InterpretedOpcodes.h"
	
#ifdef _XBOX
#include <xtl.h>
	
#else //win32
#include <windows.h>

extern int APIENTRY aWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow);
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow)
{
//	CInterpretedOpcodes* InterpretedOps = new CInterpretedOpcodes;
	
	aWinMain(hInstance, hPrevInstance, lpszCmdLine, nCmdShow);
}
#endif
