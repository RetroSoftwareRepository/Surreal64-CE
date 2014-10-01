#include "SurrealConfig.h"
//#include "xbapp.h"
//#include "xbresource.h"
#include "simpleini.h"

int dw1964DynaMem			= default1964DynaMem;
int dw1964PagingMem			= default1964PagingMem;
int dwPJ64DynaMem			= defaultPJ64DynaMem;
int dwPJ64PagingMem			= defaultPJ64PagingMem;
int dwUltraCodeMem			= defaultUltraCodeMem;
int dwUltraGroupMem			= defaultUltraGroupMem;
int dwMaxVideoMem			= defaultMaxVideoMem;

int iVideoplugin			= defaultVideoplugin;
int iAudioPlugin			= defaultAudioPlugin;
int iRspPlugin				= defaultRspPlugin;
bool bUseRspAudio			= defaultUseRspAudio;

int iPagingMethod			= defaultPagingMethod;
bool bAudioBoost			= defaultAudioBoost;

bool bEnableController1		= defaultEnableController1;
bool bEnableController2		= defaultEnableController2;
bool bEnableController3		= defaultEnableController3;
bool bEnableController4		= defaultEnableController4;

int iDefaultPak				= defaultPak;
int iSensitivity			= defaultSensitivity;
float fDeadZone				= defaultDeadZone;

int iVSync					= defaultVSync;
int iAntiAliasMode			= defaultAntiAliasMode;
int iTextureMode 			= defaultTextureMode;
int iFlickerFilter			= defaultFlickerFilter;
int iSoftDisplayFilter		= defaultSoftDisplayFilter;
int iVertexMode				= defaultVertexMode;
bool bFrameSkip 			= defaultFrameSkip;

int iTVMode					= defaultTVMode;
int iTVResolution			= defaultTVResolution;
int iTVSystem				= defaultTVSystem;
bool bEnableHDTV			= defaultEnableHDTV;
bool bSupportsHDTV			= defaultSupportsHDTV;
bool IsHDTVSupported();

void ResetAppDefaults()
{

}

void ResetRomDefaults()
{
	dw1964DynaMem			= default1964DynaMem;
	dw1964PagingMem			= default1964PagingMem;
	dwPJ64DynaMem			= defaultPJ64DynaMem;
	dwPJ64PagingMem			= defaultPJ64PagingMem;
	dwUltraCodeMem			= defaultUltraCodeMem;
	dwUltraGroupMem			= defaultUltraGroupMem;
	dwMaxVideoMem			= defaultMaxVideoMem;

	iVideoplugin			= defaultVideoplugin;
	iAudioPlugin			= defaultAudioPlugin;
	iRspPlugin				= defaultRspPlugin;
	bUseRspAudio			= defaultUseRspAudio;
	
	iPagingMethod			= defaultPagingMethod;
	bAudioBoost				= defaultAudioBoost;
	
	bEnableController1		= defaultEnableController1;
	bEnableController2		= defaultEnableController2;
	bEnableController3		= defaultEnableController3;
	bEnableController4		= defaultEnableController4;
	
	iDefaultPak				= defaultPak;
	iSensitivity			= defaultSensitivity;
	fDeadZone				= defaultDeadZone;
	
	iVSync					= defaultVSync;
	iAntiAliasMode			= defaultAntiAliasMode;
	iTextureMode 			= defaultTextureMode;
	iFlickerFilter			= defaultFlickerFilter;
	iSoftDisplayFilter		= defaultSoftDisplayFilter;
	bFrameSkip 				= defaultFrameSkip;
}

int LoadAppSettings()
{
	return 0;
}

int SaveAppSettings()
{
	return 0;
}

int LoadRomSettings()
{
	return 0;
}

int SaveRomSettings()
{
	return 0;
}

int LoadRomHistory()
{
	return 0;
}

int SaveRomHistory()
{
	return 0;
}

void SetupTV()
{
	bool bHasHDPack = false;
	DWORD dwVideoFlags = XGetVideoFlags();

	if(XGetAVPack() == XC_AV_PACK_HDTV)
		bHasHDPack = true;

	if(bHasHDPack && (dwVideoFlags & XC_VIDEO_FLAGS_HDTV_1080i))
	{
		bSupportsHDTV = true;
		iTVResolution = _TVResolution1080i;
	}
	else if(bHasHDPack && (dwVideoFlags & XC_VIDEO_FLAGS_HDTV_720p))
	{
		bSupportsHDTV = true;
		iTVResolution = _TVResolution720p;
	}
	else if(bHasHDPack && (dwVideoFlags & XC_VIDEO_FLAGS_HDTV_480p))
	{
		bSupportsHDTV = true; //??
		iTVResolution = _TVResolution480p;
	}
	else
	{
		bSupportsHDTV = false;
		iTVResolution = _TVResolution480i;
	}
	
	if(XGetVideoStandard() == XC_VIDEO_STANDARD_PAL_I)
	{
		if(dwVideoFlags & XC_VIDEO_FLAGS_PAL_60Hz)
		{
			iTVSystem = _TVSystemMPAL;
		}
		else
		{
			iTVSystem = _TVSystemPAL;
		}
	}
	else
	{
		iTVSystem = _TVSystemNTSC;
	}
	
	if(dwVideoFlags & XC_VIDEO_FLAGS_WIDESCREEN)
	{
		iTVMode = _TVModeWide;
	}
	else if(dwVideoFlags & XC_VIDEO_FLAGS_LETTERBOX)
	{
		iTVMode = _TVModeLetterbox;
	}
	else
	{
		iTVMode = _TVModeNormal;
	}
}

#if defined(__cplusplus)
extern "C" {
#endif

void loadinis() {
	//Check for CD/DVD
	/*if(XGetDiskSectorSize("D:\\") == 2048){
		onhd = FALSE;
	}else{
		onhd = TRUE;
	}*/
	LoadAppSettings();
	SaveAppSettings();
	LoadRomHistory();
	LoadRomSettings();
	
	// just putting this here for now because it gets called right away
	SetupTV();
}

int loaddwPJ64PagingMem()	{ return dwPJ64PagingMem; }
int loaddwPJ64DynaMem()		{ return dwPJ64DynaMem; }
int loaddw1964PagingMem()	{ return dw1964PagingMem; }
int loaddw1964DynaMem()		{ return dw1964DynaMem; }
int loaddwUltraCodeMem()	{ return dwUltraCodeMem; }
int loaddwUltraGroupMem()	{ return dwUltraGroupMem; }
int loaddwMaxVideoMem()		{ return dwMaxVideoMem; }
int loadiRspPlugin()		{ return iRspPlugin; }
int loadiAudioPlugin()		{ return iAudioPlugin; }
int loadiPagingMethod()		{ return iPagingMethod; }
int loadbUseRspAudio()		{ return (bUseRspAudio ? 1 : 0); }
int loadbAudioBoost()		{ return (bAudioBoost ? 1 : 0); }

void GetPathSaves(char *pszPathSaves)
{
	//sprintf(pszPathSaves, "%s", szPathSaves);
	sprintf(pszPathSaves, "D:\\Saves\\");
}

#if defined(__cplusplus)
}
#endif
