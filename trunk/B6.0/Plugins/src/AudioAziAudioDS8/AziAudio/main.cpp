/****************************************************************************
*                                                                           *
* Azimer's HLE Audio Plugin for Project64 Compatible N64 Emulators          *
* http://www.apollo64.com/                                                  *
* Copyright (C) 2000-2015 Azimer. All rights reserved.                      *
*                                                                           *
* License:                                                                  *
* GNU/GPLv2 http://www.gnu.org/licenses/gpl-2.0.html                        *
*                                                                           *
****************************************************************************/

#include "common.h"
#include "../Audio.h"
#ifdef USE_XAUDIO2
#include "XAudio2SoundDriver.h"
#else
#include "DirectSoundDriver.h"
#endif
#include "audiohle.h"
//#include "rsp/rsp.h"
#include <stdio.h>
#include <conio.h>
#include <fcntl.h>
#include <io.h>
#include <ios>
#include "resource.h"

using namespace std;

#if defined(XAUDIO_LIBRARIES_UNAVAILABLE) || !defined(USE_XAUDIO2)
DirectSoundDriver snd;// = AudioCode();
#else
XAudio2SoundDriver snd;// = AudioCode();
#endif

// Dialog Procedures
#if !defined(_XBOX)
INT_PTR CALLBACK ConfigProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
#endif

// Direct Sound selection
char DSoundDeviceName[10][100];
LPGUID DSoundGUID[10];
int DSoundCnt;
int SelectedDSound;


// RSP Test stuff

//RSP_INFO RSPInfo;


void RedirectIOToConsole();

// New Plugin Specification


// Old Plugin Specification


// Dialogs

HINSTANCE hInstance;
static bool	bAbortAiUpdate = false;

#ifdef __GNUC__
extern "C"
#endif
BOOL WINAPI DllMain(
  HINSTANCE hinstDLL,  // handle to DLL module
  DWORD fdwReason,     // reason for calling function
  LPVOID lpvReserved   // reserved
  ) {
	UNREFERENCED_PARAMETER(lpvReserved);
	hInstance = hinstDLL;
	if (fdwReason == DLL_PROCESS_DETACH)
	{ 
		bAbortAiUpdate = true;
		Sleep(100);
	}
	
	return TRUE;
}

 FUNC_TYPE(void) NAME_DEFINE(DllAbout) (HWND hParent) 
{
	MessageBoxA(hParent, "No About yet... ", "About Box", MB_OK);
}

FUNC_TYPE(void) NAME_DEFINE(DllConfig) (HWND hParent)
{
#if defined(_XBOX) || 0
	MessageBox(hParent, "We don't use config dialog... ", "", MB_OK);
#else
	DialogBox(hInstance, MAKEINTRESOURCE(IDD_CONFIG), hParent, ConfigProc);
#endif
}

FUNC_TYPE(void) NAME_DEFINE(DllTest) (HWND hParent)
{
	MessageBoxA(hParent, "Nothing to test yet... ", "Test Box", MB_OK);
}

// Initialization / Deinitalization Functions

// Note: We call CloseDLL just in case the audio plugin was already initialized...
AUDIO_INFO AudioInfo;
DWORD Dacrate = 0;

// TODO: Instead of checking for an initialized state, we should default to a no-sound audio processing state and give a warning
BOOL audioIsInitialized = FALSE;

//TODO: Do away with these from main.cpp.  They are only needed for HLE and available in AudioInfo
u8 * DMEM;
u8 * IMEM;
u8 * DRAM;

FUNC_TYPE(BOOL) NAME_DEFINE(InitiateAudio) (AUDIO_INFO Audio_Info) 
{

	//RedirectIOToConsole();
	Dacrate = 0;
	//CloseDLL ();
	DSoundCnt = 0;
	SelectedDSound = 0;
//	if ( (DirectSoundEnumerate(DSEnumProc, NULL)) != DS_OK ) { printf("Unable to enumerate DirectSound devices\n"); }

	// TODO: Move from SoundDriver to a configuration class
	snd.configAIEmulation = true;
	snd.configSyncAudio   = true;
	snd.configForceSync   = false;
	snd.configMute		  = false;
	snd.configHLE		  = true;
	snd.configRSP		  = true;
	safe_strcpy(snd.configAudioLogFolder, 499, "D:\\");

	//snd.configDevice = 0;
	snd.configVolume = 0;

	memcpy(&AudioInfo, &Audio_Info, sizeof(AUDIO_INFO));
	DRAM = Audio_Info.RDRAM;
	DMEM = Audio_Info.DMEM;
	IMEM = Audio_Info.IMEM;

	snd.AI_Startup(AudioInfo.hwnd);

	return TRUE;
}

FUNC_TYPE(void) NAME_DEFINE(CloseDLL) (void) 
{
	snd.AI_Shutdown();
}

FUNC_TYPE(void) NAME_DEFINE(GetDllInfo) (PLUGIN_INFO * PluginInfo) 
{
	PluginInfo->MemoryBswaped = TRUE;
	PluginInfo->NormalMemory  = FALSE;
	safe_strcpy(PluginInfo->Name, 100, PLUGIN_VERSION);
	PluginInfo->Type = PLUGIN_TYPE_AUDIO;
	PluginInfo->Version = 0x0101; // Set this to retain backwards compatibility
}

FUNC_TYPE(void) NAME_DEFINE(ProcessAList) (void)
{
	if (snd.configHLE) 
	{
		HLEStart ();
	}
}

FUNC_TYPE(void) NAME_DEFINE(RomOpen) (void) 
{
	Dacrate = 0;
	snd.AI_ResetAudio();
}

FUNC_TYPE(void) NAME_DEFINE(RomClosed) (void) 
{
	Dacrate = 0;
	snd.AI_ResetAudio();
}

FUNC_TYPE(void) NAME_DEFINE(AiDacrateChanged) (int SystemType)
{
	DWORD Frequency, video_clock;

	if (Dacrate == *AudioInfo.AI_DACRATE_REG)
		return;

	Dacrate = *AudioInfo.AI_DACRATE_REG & 0x00003FFF;
#ifdef _DEBUG
	if (Dacrate != *AudioInfo.AI_DACRATE_REG)
		MessageBoxA(
			NULL,
			"Unknown/reserved bits in AI_DACRATE_REG set.",
			"Warning",
			MB_ICONWARNING
		);
#endif
	switch (SystemType) {
		default         :  MessageBoxA(NULL, "Invalid SystemType.", NULL, MB_ICONERROR);
		case SYSTEM_NTSC:  video_clock = 48681812; break;
		case SYSTEM_PAL :  video_clock = 49656530; break;
		case SYSTEM_MPAL:  video_clock = 48628316; break;
	}
	Frequency = video_clock / (Dacrate + 1);
	snd.AI_SetFrequency(Frequency);
}

FUNC_TYPE(void) NAME_DEFINE(AiLenChanged)(void) 
{
	snd.AI_LenChanged (
		(AudioInfo.RDRAM + (*AudioInfo.AI_DRAM_ADDR_REG & 0x00FFFFF8)), 
		*AudioInfo.AI_LEN_REG & 0x3FFF8);
}

FUNC_TYPE(DWORD) NAME_DEFINE(AiReadLength)(void) 
{
	if (audioIsInitialized == FALSE) return 0;
	*AudioInfo.AI_LEN_REG = snd.AI_ReadLength();
	return *AudioInfo.AI_LEN_REG;

}

// Deprecated Functions


FUNC_TYPE(void) NAME_DEFINE(AiUpdate) (BOOL Wait) 
{
	static int intCount = 0;
	if (Wait)
	{
		if (bAbortAiUpdate == true) 
		{
			if (intCount > 10) 
				ExitThread(0);
			intCount++;
			return;
		}
		Sleep(10);
	}
	return;
}

#if !defined(_XBOX)
INT_PTR CALLBACK ConfigProc(
	HWND hDlg,  // handle to dialog box
	UINT uMsg,     // message
	WPARAM wParam, // first message parameter
	LPARAM lParam  // second message parameter
	) {
	UNREFERENCED_PARAMETER(lParam);
	int x;
	switch (uMsg) {
	case WM_INITDIALOG:
		SendMessage(GetDlgItem(hDlg, IDC_DEVICE), CB_RESETCONTENT, 0, 0);
		for (x = 0; x < DSoundCnt; x++) {
			SendMessage(GetDlgItem(hDlg, IDC_DEVICE), CB_ADDSTRING, 0, (long)DSoundDeviceName[x]);
		}
		SendMessage(GetDlgItem(hDlg, IDC_DEVICE), CB_SETCURSEL, SelectedDSound, 0);
		SendMessage(GetDlgItem(hDlg, IDC_OLDSYNC), BM_SETCHECK, snd.configForceSync ? BST_CHECKED : BST_UNCHECKED, 0);
		SendMessage(GetDlgItem(hDlg, IDC_AUDIOSYNC), BM_SETCHECK, snd.configSyncAudio ? BST_CHECKED : BST_UNCHECKED, 0);
		SendMessage(GetDlgItem(hDlg, IDC_AI), BM_SETCHECK, snd.configAIEmulation ? BST_CHECKED : BST_UNCHECKED, 0);
		SendMessage(GetDlgItem(hDlg, IDC_VOLUME), TBM_SETRANGEMIN, FALSE, 0);
		SendMessage(GetDlgItem(hDlg, IDC_VOLUME), TBM_SETRANGEMAX, FALSE, 100);
		SendMessage(GetDlgItem(hDlg, IDC_MUTE), BM_SETCHECK, snd.configMute ? BST_CHECKED : BST_UNCHECKED, 0);
		if (snd.configMute) SendMessage(GetDlgItem(hDlg, IDC_VOLUME), TBM_SETPOS, FALSE, 100);
		else SendMessage(GetDlgItem(hDlg, IDC_VOLUME), TBM_SETPOS, FALSE, snd.configVolume);
		SendMessage(GetDlgItem(hDlg, IDC_VOLUME), TBM_SETTICFREQ, 20, 0);
		SendMessage(GetDlgItem(hDlg, IDC_HLE), BM_SETCHECK, snd.configHLE ? BST_CHECKED : BST_UNCHECKED, 0);
		SendMessage(GetDlgItem(hDlg, IDC_RSP), BM_SETCHECK, snd.configRSP ? BST_CHECKED : BST_UNCHECKED, 0);
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK:
			snd.configForceSync = SendMessage(GetDlgItem(hDlg, IDC_OLDSYNC), BM_GETSTATE, 0, 0) == BST_CHECKED ? true : false;
			snd.configSyncAudio = SendMessage(GetDlgItem(hDlg, IDC_AUDIOSYNC), BM_GETSTATE, 0, 0) == BST_CHECKED ? true : false;
			snd.configAIEmulation = SendMessage(GetDlgItem(hDlg, IDC_AI), BM_GETSTATE, 0, 0) == BST_CHECKED ? true : false;
			snd.configHLE = SendMessage(GetDlgItem(hDlg, IDC_HLE), BM_GETSTATE, 0, 0) == BST_CHECKED ? true : false;
			snd.configRSP = SendMessage(GetDlgItem(hDlg, IDC_RSP), BM_GETSTATE, 0, 0) == BST_CHECKED ? true : false;
			SelectedDSound = (int)SendMessage(GetDlgItem(hDlg, IDC_DEVICE), CB_GETCURSEL, 0, 0);
			safe_strcpy(snd.configDevice, 99, DSoundDeviceName[SelectedDSound]);
			EndDialog(hDlg, 0);
			break;
		case IDCANCEL:
			EndDialog(hDlg, 0);
			break;
		case IDC_MUTE:
			if (IsDlgButtonChecked(hDlg, IDC_MUTE))
			{
				snd.SetVolume(100);
				snd.configMute = true;
				SendMessage(GetDlgItem(hDlg, IDC_VOLUME), TBM_SETPOS, TRUE, 100);
			}
			else {
				snd.SetVolume(snd.configVolume);
				snd.configMute = false;
				SendMessage(GetDlgItem(hDlg, IDC_VOLUME), TBM_SETPOS, TRUE, snd.configVolume);
			}
			break;
		}
		break;
	case WM_KEYDOWN:
		break;
	case WM_VSCROLL:
		short int userReq = LOWORD(wParam);
		if (userReq == TB_ENDTRACK || userReq == TB_THUMBTRACK)
		{
			LRESULT position;
			DWORD dwPosition;

			position = SendMessage(GetDlgItem(hDlg, IDC_VOLUME), TBM_GETPOS, 0, 0);
			dwPosition = (position > 100) ? 100 : (DWORD)position;
			snd.SetVolume(dwPosition);
			if (!snd.configMute)
			{
				snd.configVolume = dwPosition;
			}
			else if (dwPosition != 100)
			{
				SendMessage(GetDlgItem(hDlg, IDC_MUTE), BM_SETCHECK, BST_UNCHECKED, 0);
				snd.configMute = false;
				snd.configVolume = dwPosition;
			}
			if (dwPosition == 100)
			{
				SendMessage(GetDlgItem(hDlg, IDC_MUTE), BM_SETCHECK, BST_CHECKED, 0);
				snd.configMute = true;
			}
		}
		break;
	}

	return FALSE;

}
#endif

// TODO: I think this can safely be removed
BOOL CALLBACK DSEnumProc(LPGUID lpGUID, LPCTSTR lpszDesc, LPCTSTR lpszDrvName, LPVOID lpContext)
{
	UNREFERENCED_PARAMETER(lpszDrvName);
	UNREFERENCED_PARAMETER(lpContext);
	//HWND hDlg = (HWND)lpContext;
	safe_strcpy(DSoundDeviceName[DSoundCnt], 99, lpszDesc);
	DSoundGUID[DSoundCnt] = lpGUID;
	if (strcmp(lpszDesc, snd.configDevice) == 0)
	{
		SelectedDSound = DSoundCnt;
	}
	DSoundCnt++;

	return TRUE;
}

int safe_strcpy(char* dst, size_t limit, const char* src)
{
#if (_MSC_VER > 1400)
    return strcpy_s(dst, limit, src);
#else
    size_t bytes;
    int failure;

    if (dst == NULL || src == NULL)
        return (failure = 22); /* EINVAL, from MSVC <errno.h> */

    bytes = strlen(src) + 1; /* strlen("abc") + 1 == 4 bytes */
    failure = 34; /* ERANGE, from MSVC <errno.h> */
    if (bytes > limit)
        bytes = limit;
    else
        failure = 0;

    memcpy(dst, src, bytes);
    dst[limit - 1] = '\0'; /* in case of ERANGE, may not be null-terminated */
    return (failure);
#endif
}
