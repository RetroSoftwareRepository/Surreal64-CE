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

#pragma once

#if defined(_WIN32) && !defined(_XBOX)
#include <windows.h>
#elif defined(_XBOX)
#include <xtl.h>
#else
#include <SDL/SDL.h>
#endif

/* strcpy() */
#include <string.h>

#include "common.h"
#include "AudioSpec.h"

#define SND_IS_NOT_EMPTY 0x4000000
#define SND_IS_FULL		 0x8000000

/* deprecated AI functions -- to be removed */
#if 1 && defined(_WIN32)
#define LEGACY_SOUND_DRIVER
#endif

#if !defined(_WIN32) && !defined(_XBOX)
#define UNREFERENCED_PARAMETER(msg)     msg
#endif

class SoundDriver
{
public:
	// Configuration variables
	// TODO: these may need to go elsewhere
	bool configAIEmulation;
	bool configSyncAudio;
	bool configForceSync;
	bool configMute;
	bool configHLE;
	bool configRSP;
	unsigned long configVolume;
	char configAudioLogFolder[500];
	char configDevice[100];

	// Setup and Teardown Functions
	virtual Boolean Initialize() = 0;
	virtual void DeInitialize() = 0;

	// Management functions
	virtual void AiUpdate(Boolean Wait) { UNREFERENCED_PARAMETER(Wait); }; // Optional
	virtual void StopAudio() = 0;							// Stops the Audio PlayBack (as if paused)
	virtual void StartAudio() = 0;							// Starts the Audio PlayBack (as if unpaused)
	virtual void SetFrequency(u32 Frequency) = 0; // Sets the Nintendo64 Game Audio Frequency

	// Deprecated
#ifdef LEGACY_SOUND_DRIVER
	virtual u32 GetReadStatus() = 0;                  // Returns the status on the read pointer
	virtual u32 AddBuffer(u8 *start, u32 length) = 0; // Uploads a new buffer and returns status
#endif

	// Audio Spec interface methods (new)
	void AI_SetFrequency(u32 Frequency);
	void AI_LenChanged(u8 *start, u32 length);
	u32 AI_ReadLength();
	void AI_Startup();
	void AI_Shutdown();
	void AI_ResetAudio();
	void AI_Update(Boolean Wait);

	// Buffer Management methods
	u32 LoadAiBuffer(u8 *start, u32 length); // Reads in length amount of audio bytes

	// Sound Driver Factory method
	static SoundDriver* SoundDriverFactory();

	virtual void SetVolume(u32 volume) { UNREFERENCED_PARAMETER(volume); }; // We could potentially do this ourselves within the buffer copy method
	virtual ~SoundDriver() {};

protected:
	// Temporary (to allow for incremental development)
	bool m_audioIsInitialized;

	// Mutex Handle
	HANDLE m_hMutex;

	// Variables for AI DMA emulation
	int m_AI_CurrentDMABuffer; // Currently playing AI Buffer
	int m_AI_WriteDMABuffer;   // Which set of registers will be written to
	u8 *m_AI_DMABuffer[2];    // Location in RDRAM containing buffer data
	u32 m_AI_DMARemaining[2]; // How much RDRAM buffer is left to read

	// Variables for Buffering audio samples from AI DMA
	static const int MAX_SIZE = 44100 * 2 * 2; // Max Buffer Size (44100Hz * 16bit * Stereo)
	static const int NUM_BUFFERS = 4; // Number of emulated buffers
	u32 m_MaxBufferSize;   // Variable size determined by Playback rate
	u32 m_CurrentReadLoc;   // Currently playing Buffer
	u32 m_CurrentWriteLoc;  // Currently writing Buffer
	u8 m_Buffer[MAX_SIZE]; // Emulated buffers
	u32 m_BufferRemaining; // Buffer remaining
	bool m_DMAEnabled;  // Sets to true when DMA is enabled

	SoundDriver(){
		m_audioIsInitialized = false;
		configAIEmulation = true;
		configSyncAudio = true;
		configForceSync = false;
		configMute = false;
		configHLE = true;
		configRSP = true;
		configVolume = 0;
		m_hMutex = NULL;
#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS) && !defined(_XBOX)
		strcpy_s(configAudioLogFolder, 500, "D:\\");
		strcpy_s(configDevice, 100, "");
#else
		strcpy(configAudioLogFolder, "D:\\");
		strcpy(configDevice, "");
#endif
	}
};
