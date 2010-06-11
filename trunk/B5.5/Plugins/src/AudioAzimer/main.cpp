
#include <xtl.h>
#include "Audio.h"
#include "AudioCode.h"
#include "audiohle.h"

#define PLUGIN_VERSION "0.55.1 Alpha"

// New Plugin Specification


// Old Plugin Specification


// Dialogs

void   _AUDIO_AZIMER_DllAbout ( HWND hParent ){

}

void   _AUDIO_AZIMER_DllConfig ( HWND hParent ){
	 
}

void   _AUDIO_AZIMER_DllTest ( HWND hParent ){
 
}

// Initialization / Deinitalization Functions

// Note: We call CloseDLL just in case the audio plugin was already initialized...
//AUDIO_INFO AudioInfo;
AudioCode  snd;
//DWORD Dacrate = 0;
extern DWORD Dacrate; //freakdave - FIXME: split up in multiple projects/binaries
void AiCallBack (DWORD Status);

 BOOL   _AUDIO_AZIMER_InitiateAudio (AUDIO_INFO Audio_Info){
	 _AUDIO_AZIMER_CloseDLL ();
	memcpy (&AudioInfo, &Audio_Info, sizeof(AUDIO_INFO));
	snd.Initialize (AiCallBack, AudioInfo.hwnd);
	return TRUE;
}

 void   _AUDIO_AZIMER_CloseDLL (void){
	ChangeABI (0);
	snd.DeInitialize ();
}

 void   _AUDIO_AZIMER_GetDllInfo ( PLUGIN_INFO * PluginInfo ){
	PluginInfo->MemoryBswaped = TRUE;
	PluginInfo->NormalMemory  = FALSE;
	strcpy (PluginInfo->Name, "Azimer's HLE Audio v");
	strcat (PluginInfo->Name, PLUGIN_VERSION);
	PluginInfo->Type = PLUGIN_TYPE_AUDIO;
	PluginInfo->Version = 0x0101; // Set this to retain backwards compatibility
}

 void   _AUDIO_AZIMER_ProcessAList(void){
	HLEStart ();
}

 void   _AUDIO_AZIMER_RomClosed (void){
	ChangeABI (0);
	snd.StopAudio ();
	Dacrate = 0;
	snd.Initialize(AiCallBack, AudioInfo.hwnd);
}

 void   _AUDIO_AZIMER_AiDacrateChanged (int  SystemType) {
	DWORD Frequency;
	if (Dacrate != *AudioInfo.AI_DACRATE_RG) {
		Dacrate = *AudioInfo.AI_DACRATE_RG;
		switch (SystemType) {
			case 0: Frequency = 48681812 / (Dacrate + 1); break;
			case 1:  Frequency = 49656530 / (Dacrate + 1); break;
			case 2: Frequency = 48628316 / (Dacrate + 1); break;
		}
		snd.SetFrequency (Frequency);
	}
}

 void   _AUDIO_AZIMER_AiLenChanged (void){
	DWORD retVal;
	retVal = snd.AddBuffer (
		(AudioInfo.RDRAM + (*AudioInfo.AI_DRAM_ADDR_RG & 0x00FFFFF8)), 
		*AudioInfo.AI_LEN_RG & 0x3FFF8);
	//if (retVal & SND_IS_FULL)
	//	*AudioInfo.AI_STATUS_RG |= AI_STATUS_FIFO_FULL;
	//*AudioInfo.AI_STATUS_RG |= AI_STATUS_DMA_BUSY;
}

 DWORD   _AUDIO_AZIMER_AiReadLength (void){
	*AudioInfo.AI_LEN_RG = snd.GetReadStatus ();
	return *AudioInfo.AI_LEN_RG;
}

// Deprecated Functions

 void   _AUDIO_AZIMER_AiUpdate (BOOL Wait){
	//if (Wait)
	//	WaitMessage ();
}
