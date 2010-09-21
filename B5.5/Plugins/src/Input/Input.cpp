#include <xtl.h>
#include "Input.h"
#include "Configi.h"
#include <XbInput.h>
#include "../../../config.h"

static Config g_config;
static BOOL g_bIngameMenu = FALSE;
CONTROL *gControls;

DWORD dwLastChange = 0;

static void UpdateRumble(int Control);

void _INPUT_CloseDLL(){}

void _INPUT_ControllerCommand(int Control, BYTE * Command){}
void _INPUT_DllAbout(HWND hParent) {}
void _INPUT_DllConfig(HWND hParent){}

void _INPUT_GetDllInfo(PLUGIN_INFO *PluginInfo) 
{
	PluginInfo->Version = 0x0100;
	PluginInfo->Type	= PLUGIN_TYPE_CONTROLLER;
	PluginInfo->Name[0] = NULL;
}

void _INPUT_GetKeys(int Control, BUTTONS * Keys) 
{
	if (Keys == NULL)
		return;

	// retrieve the latest xbox controller state
	XBInput_GetInput();

	Keys->Y_AXIS		= g_config.GetN64ButtonValue(Control, N64ThumbRight)
								- g_config.GetN64ButtonValue(Control, N64ThumbLeft);
	Keys->X_AXIS		= g_config.GetN64ButtonValue(Control, N64ThumbUp) 
								- g_config.GetN64ButtonValue(Control, N64ThumbDown);
	Keys->L_DPAD		= g_config.GetN64ButtonValue(Control, N64DPadLeft);
	Keys->R_DPAD		= g_config.GetN64ButtonValue(Control, N64DPadRight);
	Keys->U_DPAD		= g_config.GetN64ButtonValue(Control, N64DPadUp);
	Keys->D_DPAD		= g_config.GetN64ButtonValue(Control, N64DPadDown);

	Keys->L_CBUTTON	= g_config.GetN64ButtonValue(Control, N64CButtonLeft);
	Keys->R_CBUTTON	= g_config.GetN64ButtonValue(Control, N64CButtonRight);
	Keys->U_CBUTTON	= g_config.GetN64ButtonValue(Control, N64CButtonUp);
	Keys->D_CBUTTON	= g_config.GetN64ButtonValue(Control, N64CButtonDown);

	Keys->START_BUTTON = g_config.GetN64ButtonValue(Control, N64Start);

	Keys->A_BUTTON		= g_config.GetN64ButtonValue(Control, N64A);
	Keys->B_BUTTON		= g_config.GetN64ButtonValue(Control, N64B);

	Keys->Z_TRIG		= g_config.GetN64ButtonValue(Control, N64ZTrigger);
	Keys->L_TRIG		= g_config.GetN64ButtonValue(Control, N64LeftTrigger);
	Keys->R_TRIG		= g_config.GetN64ButtonValue(Control, N64RightTrigger);


	// update the rumble strength for this control
	UpdateRumble(Control);

	if (g_config.GetXboxButtonValue(Control, XboxRightThumbButton))
		g_bIngameMenu = TRUE;

	if (g_config.GetXboxButtonValue(Control, XboxLeftThumbButton) && g_config.GetXboxButtonValue(Control, XboxLeftTrigger) && g_config.GetXboxButtonValue(Control, XboxRightTrigger) && GetTickCount() > dwLastChange + 1000)
	{
//		char msg[100];
		dwLastChange = GetTickCount();

		//Switch to MemPak if no Pak is detected
		if (gControls[Control].Plugin == PLUGIN_NONE)
		{
			gControls[Control].Plugin  = PLUGIN_MEMPAK;
//			sprintf(msg, "Controller %i switched to Mempak", Control+1);
		}

		//Switch to RumblePak if MemPak is already inserted
		if (gControls[Control].Plugin == PLUGIN_MEMPAK)
		{
			gControls[Control].Plugin  = PLUGIN_RUMBLE_PAK;
//			sprintf(msg, "Controller %i switched to Rumblepak", Control+1);
		}

		 //Remove Pak
		if (gControls[Control].Plugin == PLUGIN_RUMBLE_PAK)
		{
			gControls[Control].Plugin  = PLUGIN_NONE;
//			sprintf(msg, "Controller %i removed Pak", Control+1);
		}
		
		

	}
}
void _INPUT_InitiateControllers(HWND hMainWindow, CONTROL Controls[4]) 
{
	// init the xbox controllers
	XInitDevices(0, NULL);
	XBInput_CreateGamepads();

	//load Configs
	ConfigAppLoadTemp();
	ConfigAppLoad();
	ConfigAppLoad2();
	
	// init the Controls struct which tells the emulator
	// what controllers are plugged in
	Controls[0].Present = EnableController1;
	Controls[0].RawData = FALSE;
	Controls[0].Plugin = DefaultPak + 1;

	Controls[1].Present = EnableController2;
	Controls[1].RawData = FALSE;
	Controls[1].Plugin = DefaultPak + 1;

	Controls[2].Present = EnableController3;
	Controls[2].RawData = FALSE;
	Controls[2].Plugin = DefaultPak + 1;

	Controls[3].Present = EnableController4;
	Controls[3].RawData = FALSE;
	Controls[3].Plugin = DefaultPak + 1;

	gControls = Controls;
}
void _INPUT_ReadController(int Control, BYTE * Command) {}
void _INPUT_RomClosed() {}
void _INPUT_RomOpen() {}
void _INPUT_WM_KeyDown(WPARAM wParam, LPARAM lParam) {}
void _INPUT_WM_KeyUp(WPARAM wParam, LPARAM lParam) {}

void _INPUT_SetRumble(int Control, BOOL on)
{
	if (Control > 3)
		return;

	if (on)
	{
		g_Gamepads[Control].Feedback.Rumble.wRightMotorSpeed = 50000;
		g_Gamepads[Control].Feedback.Rumble.wLeftMotorSpeed = 50000;
	}
	else
	{
		g_Gamepads[Control].Feedback.Rumble.wRightMotorSpeed = 0;
		g_Gamepads[Control].Feedback.Rumble.wLeftMotorSpeed = 0;
	}
}

void _INPUT_LoadButtonMap(int *cfgData)
{
	g_config.Load(cfgData);
}

HANDLE _INPUT_GetXGamepadHandle(int controller)
{
	return g_Gamepads[controller].hDevice;
}

BOOL _INPUT_IsIngameMenuWaiting()
{
	if (g_bIngameMenu)
	{
		g_bIngameMenu = FALSE;
		return TRUE;
	}
	
	return FALSE;
}


BOOL _INPUT_UpdatePaks()
{
	gControls[0].Plugin  = DefaultPak + 1;
	gControls[1].Plugin  = DefaultPak + 1;
	gControls[2].Plugin  = DefaultPak + 1;
	gControls[3].Plugin  = DefaultPak + 1;

	return TRUE;
}


BOOL _INPUT_UpdateControllerStates()
{
	ConfigAppLoadTemp();//retrieve name of the current rom
	ConfigAppLoad2();//load its ini
	gControls[0].Present = EnableController1;
	gControls[1].Present = EnableController2;
	gControls[2].Present = EnableController3;
	gControls[3].Present = EnableController4;

	//freakdave FIXME
	//Disabling a Controller seems to work, however
	//re-enabling it doesn't work. This is strange..

	return TRUE;
}

void UpdateRumble(int Control)
{
	if(g_Gamepads[Control].hDevice)
	{
		if (g_Gamepads[Control].Feedback.Header.dwStatus != ERROR_IO_PENDING)
		{
			if (g_Gamepads[Control].Feedback.Rumble.wRightMotorSpeed > 10)
				g_Gamepads[Control].Feedback.Rumble.wRightMotorSpeed -= 10;
			else if (g_Gamepads[Control].Feedback.Rumble.wRightMotorSpeed > 0)
				g_Gamepads[Control].Feedback.Rumble.wRightMotorSpeed = 0;

			if (g_Gamepads[Control].Feedback.Rumble.wLeftMotorSpeed > 10)
				g_Gamepads[Control].Feedback.Rumble.wLeftMotorSpeed -= 10;
			else if (g_Gamepads[Control].Feedback.Rumble.wLeftMotorSpeed > 0)
				g_Gamepads[Control].Feedback.Rumble.wLeftMotorSpeed = 0;

			XInputSetState(g_Gamepads[Control].hDevice, &g_Gamepads[Control].Feedback);
		}
	}
}
