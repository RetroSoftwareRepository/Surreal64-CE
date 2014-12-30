#include <stdlib.h>
#include <io.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string.h>
#include <stdio.h>
#include "SurrealPlugins.h"
//#include "simpleini.h"
#include "xbapp.h"
#include "xbresource.h"


enum ControllerPaks
{
	_PakNone = 1,
	_PakMemory,
	_PakRumble,
	_PakMissing
};

enum Emulators
{
	_1964,
	_Project64,
	_UltraHLE,
	_None
};

enum TVResolutions
{
	_TVResolution480i,
	_TVResolution480p,
	_TVResolution720i,
	_TVResolution720p,
	_TVResolution1080i,
	_TVResolution1080p,
	_TVResolutionMissing
};

enum TVSystems
{
	_TVSystemNTSC,
	_TVSystemPAL,
	_TVSystemMPAL,
	_TVSystemMissing
};

enum TVModes
{
	_TVModeNormal,
	_TVModeWide,
	_TVModeLetterbox,
	_TVModeMissing
};

enum D3D_PresentationIntervals
{
	INTERVAL_IMMEDIATE,	//screen tearing possible
	INTERVAL_ONE,	//VSync
	//INTERVAL_TWO,	//VSync
	//INTERVAL_THREE,	//VSync
	//INTERVAL_FOUR,	//not supported on XBOX
	//INTERVAL_DEFAULT,	//VSync
	INTERVAL_ONE_OR_IMMEDIATE //see SDK docs
	//INTERVAL_TWO_OR_IMMEDIATE, //see SDK docs
	//INTERVAL_THREE_OR_IMMEDIATE, //see SDK docs*/
};

#define default1964DynaMem			4 //8
#define default1964PagingMem		4
#define defaultPJ64DynaMem			10 //16
#define defaultPJ64PagingMem		4
#define defaultUltraCodeMem			5
#define defaultUltraGroupMem		10
#define defaultMaxVideoMem			10

#define defaultVideoplugin			_VideoPluginRice560 /*_VideoPluginVid1964*/
#define defaultAudioPlugin			_AudioPluginJttl /*_AudioPluginMusyX*/
#define defaultRspPlugin			_RSPPluginLLE /*_RSPPluginHLE*/
#define defaultUseRspAudio			1 /*0*/

#define defaultPagingMethod			_PagingXXX
#define defaultAudioBoost			0

#define defaultEnableController1	1
#define defaultEnableController2	1
#define defaultEnableController3	1
#define defaultEnableController4	1

#define defaultPak					_PakRumble
#define defaultSensitivity			10
#define defaultDeadZone				8600

#define defaultVSync				INTERVAL_IMMEDIATE //see D3D_PresentParameters (PresentationIntervals)
#define defaultAntiAliasMode		0
#define defaultTextureMode			3
#define defaultFlickerFilter		1
#define defaultSoftDisplayFilter	0
#define defaultVertexMode			3 //hardware
#define defaultFrameSkip			0

#define defaultTVMode				_TVModeNormal
#define defaultTVResolution			_TVResolution480i
#define defaultTVSystem				_TVSystemNTSC
#define defaultEnableHDTV			0
#define defaultSupportsHDTV			0


extern int dw1964DynaMem;
extern int dw1964PagingMem;
extern int dwPJ64DynaMem;
extern int dwPJ64PagingMem;
extern int dwUltraCodeMem;
extern int dwUltraGroupMem;
extern int dwMaxVideoMem;

extern int iVideoplugin;
extern int iAudioPlugin;
extern int iRspPlugin;
extern bool bUseRspAudio;

extern bool bEnableController1;
extern bool bEnableController2;
extern bool bEnableController3;
extern bool bEnableController4;

extern int iDefaultPak;
extern int iSensitivity;
extern float fDeadZone;

extern int iVSync;
extern int iAntiAliasMode;
extern int iTextureMode;
extern int iFlickerFilter;
extern int iSoftDisplayFilter;
extern int iVertexMode;
extern bool bFrameSkip;

extern int iTVMode;
extern int iTVResolution;
extern int iTVSystem;
extern bool bEnableHDTV;
extern bool bSupportsHDTV;

extern void SetupTV();

extern void ResetAppDefaults();
extern void ResetRomDefaults();

extern int LoadAppSettings();
extern int SaveAppSettings();
extern int LoadRomSettings();
extern int SaveRomSettings();
extern int LoadRomHistory();
extern int SaveRomHistory();

