#include "configi.h"
#include "xbinput.h"

#include <stdlib.h>
#include <io.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string.h>
#include <stdio.h>
#include "../../../config.h"



using namespace std;

//float XBOX_CONTROLLER_DEAD_ZONE;

Config::Config(void)
{
	// create default mappings
	for (int i = 0; i < NUMBER_OF_CONTROLLERS; i++)
	{
		m_buttonMap[i][N64ThumbLeft]	= XboxLeftThumbLeft;
		m_buttonMap[i][N64ThumbRight]	= XboxLeftThumbRight;
		m_buttonMap[i][N64ThumbUp]		= XboxLeftThumbUp;
		m_buttonMap[i][N64ThumbDown]	= XboxLeftThumbDown;

		m_buttonMap[i][N64DPadLeft]		= XboxDPadLeft;
		m_buttonMap[i][N64DPadRight]	= XboxDPadRight;
		m_buttonMap[i][N64DPadUp]		= XboxDPadUp;
		m_buttonMap[i][N64DPadDown]		= XboxDPadDown;

		m_buttonMap[i][N64CButtonLeft]	= XboxRightThumbLeft;
		m_buttonMap[i][N64CButtonRight]	= XboxRightThumbRight;
		m_buttonMap[i][N64CButtonUp]	= XboxRightThumbUp;
		m_buttonMap[i][N64CButtonDown]	= XboxRightThumbDown;

		m_buttonMap[i][N64Start]		= XboxStart;

		m_buttonMap[i][N64A]			= XboxA;
		m_buttonMap[i][N64B]			= XboxB;

		m_buttonMap[i][N64ZTrigger]		= XboxLeftTrigger;
		m_buttonMap[i][N64LeftTrigger]	= XboxBack;
		m_buttonMap[i][N64RightTrigger]	= XboxRightTrigger;
	}

	for (int i = 0; i < NUMBER_OF_CONTROLLERS; i++)
	{
		m_cur64ThumbVal[i][0] = 0;
		m_cur64ThumbVal[i][1] = 0;
		m_cur64ThumbVal[i][2] = 0;
		m_cur64ThumbVal[i][3] = 0;
	}
}

Config::~Config(void)
{
}

bool Config::Load(int *cfgData)
{
	for (int i=0;i<4;i++) {
		for (int j=0;j<18;j++){
             m_buttonMap[i][j]=cfgData[(i*18)+j];
		}}
	
	return true;
}

bool Config::Load(string &szFilename)
{
	fstream cfgFile;

	cfgFile.open(szFilename.c_str(), fstream::in | fstream::binary);

	// fail if the file doesnt exist
	if (!cfgFile.is_open())
	{
		return false;
	}

	for (DWORD i = 0; i < NUMBER_OF_CONTROLLERS; i++)
	{
		for (DWORD j = 0; j < NUMBER_OF_N64_BUTTONS; j++)
		{
			char temp;
			cfgFile.read(&temp, sizeof(char));
			m_buttonMap[i][j] = static_cast<XboxButton>(temp);
		}
	}

	cfgFile.close();

	return true;
}

bool Config::Save(string &szFilename)
{
	fstream cfgFile;

	cfgFile.open(szFilename.c_str(), fstream::out | fstream::binary);

	// fail if the cant be written to
	if (!cfgFile.is_open())
	{
		return false;
	}

	for (DWORD i = 0; i < NUMBER_OF_CONTROLLERS; i++)
	{
		for (DWORD j = 0; j < NUMBER_OF_N64_BUTTONS; j++)
		{
			char temp = m_buttonMap[i][j];
			cfgFile.write(&temp, sizeof(char));
		}
	}

	cfgFile.close();

	return true;
}

/**
 * Simply maps a n64 button to an xbox button.
 */
void Config::MapN64ToXbox(DWORD controller, byte xboxButton, byte n64Button)
{
	if (controller < NUMBER_OF_CONTROLLERS)
	{
		m_buttonMap[controller][n64Button] = xboxButton;
	}
}

/**
 * Returns the value (state) of a xbox button. If its an axis 
 * then the return value will be between 0 and 32768. If its
 * a button then the return value will be 0 or 1.
 */

DWORD Config::GetXboxButtonValue(DWORD controller, byte xboxButton)
{
	// buttons fixed by GogoAckman
	if (controller > NUMBER_OF_CONTROLLERS-1)
		return 0;

	switch (xboxButton)
	{
		// Left Thumb Stick
		case XboxLeftThumbLeft:
		{
			if (g_Gamepads[controller].sThumbLX < -XBOX_CONTROLLER_DEAD_ZONE)
				return abs(g_Gamepads[controller].sThumbLX);
			else
				return 0;
		}
		case XboxLeftThumbRight:
		{
			if (g_Gamepads[controller].sThumbLX > XBOX_CONTROLLER_DEAD_ZONE)
				return abs(g_Gamepads[controller].sThumbLX);
			else
				return 0;
		}
		case XboxLeftThumbUp:
		{
			if (g_Gamepads[controller].sThumbLY > XBOX_CONTROLLER_DEAD_ZONE)
				return abs(g_Gamepads[controller].sThumbLY);
			else
				return 0;
		}
		case XboxLeftThumbDown:
		{
			if (g_Gamepads[controller].sThumbLY < -XBOX_CONTROLLER_DEAD_ZONE)
				return abs(g_Gamepads[controller].sThumbLY);
			else
				return 0;
		}

		// Right Thumb Stick
		case XboxRightThumbLeft:
		{
			if (g_Gamepads[controller].sThumbRX < -XBOX_CONTROLLER_DEAD_ZONE)
				return abs(g_Gamepads[controller].sThumbRX);
			else
				return 0;
		}
		case XboxRightThumbRight:
		{
			if (g_Gamepads[controller].sThumbRX > XBOX_CONTROLLER_DEAD_ZONE)
				return abs(g_Gamepads[controller].sThumbRX);
			else
				return 0;
		}
		case XboxRightThumbUp:
		{
			if (g_Gamepads[controller].sThumbRY > XBOX_CONTROLLER_DEAD_ZONE)
				return abs(g_Gamepads[controller].sThumbRY);
			else
				return 0;
		}
		case XboxRightThumbDown:
		{
			if (g_Gamepads[controller].sThumbRY < -XBOX_CONTROLLER_DEAD_ZONE)
				return abs(g_Gamepads[controller].sThumbRY);
			else
				return 0;
		}

		// Directional Pad
		case XboxDPadLeft:
		{
			return ((g_Gamepads[controller].wButtons & XINPUT_GAMEPAD_DPAD_LEFT)? 1:0);
		}
		case XboxDPadRight:
		{
			return ((g_Gamepads[controller].wButtons & XINPUT_GAMEPAD_DPAD_RIGHT)? 1:0);
		}	
		case XboxDPadUp:
		{
			return ((g_Gamepads[controller].wButtons & XINPUT_GAMEPAD_DPAD_UP)? 1:0);
		}
		case XboxDPadDown:
		{
			return ((g_Gamepads[controller].wButtons & XINPUT_GAMEPAD_DPAD_DOWN)? 1:0);
		}

		// Start and Back Buttons
		case XboxStart:
		{
			return ((g_Gamepads[controller].wButtons & XINPUT_GAMEPAD_START)? 1:0);
		}
		case XboxBack:
		{
			return ((g_Gamepads[controller].wButtons & XINPUT_GAMEPAD_BACK)? 1:0);
		}

		// Thumb Buttons
		case XboxLeftThumbButton:
		{
			return ((g_Gamepads[controller].wButtons & XINPUT_GAMEPAD_LEFT_THUMB)? 1:0);
		}
		case XboxRightThumbButton:
		{
			return ((g_Gamepads[controller].wButtons & XINPUT_GAMEPAD_RIGHT_THUMB)? 1:0);
		}

		// A, B, X and Y Buttons
		case XboxA:
		{
			return (g_Gamepads[controller].bAnalogButtons[XINPUT_GAMEPAD_A]?1:0 );
		}
		case XboxB:
		{
			return (g_Gamepads[controller].bAnalogButtons[XINPUT_GAMEPAD_B]?1:0 );
		}
		case XboxX:
		{
			return (g_Gamepads[controller].bAnalogButtons[XINPUT_GAMEPAD_X]?1:0 );
		}
		case XboxY:
		{
			return (g_Gamepads[controller].bAnalogButtons[XINPUT_GAMEPAD_Y]?1:0 );
		}

		// Black and White Buttons
		case XboxBlack:
		{
			return (g_Gamepads[controller].bAnalogButtons[XINPUT_GAMEPAD_BLACK]?1:0 );
		}
		case XboxWhite:
		{
			return (g_Gamepads[controller].bAnalogButtons[XINPUT_GAMEPAD_WHITE]?1:0 );
		}

		// Left and Right Triggers
		case XboxLeftTrigger:
		{
			return (g_Gamepads[controller].bAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER]?1:0 );
		}
		case XboxRightTrigger:
		{
			return (g_Gamepads[controller].bAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER]?1:0 );
		}
	}

	return 0;
}

/**
 * Returns the value (state) of a n64 button. If its an axis 
 * then the return value will be between 0 and 127. If its
 * a button then the return value will be 0 or 1.
 */
DWORD Config::GetN64ButtonValue(DWORD controller, byte n64Button)
{
	if (controller > NUMBER_OF_CONTROLLERS-1)
		return 0;

	byte xboxButton = m_buttonMap[controller][n64Button];

	// if its an n64 axis
	if (IS_N64_AXIS(n64Button))
	{
		// if we are mapping this axis to an xbox axis
		if (IS_XBOX_AXIS(xboxButton))
		{
			// diving by 128 reduces 0-32768 to 0-127 which the n64 uses
			// GogoAckman s note : it was divising by 258 :? , I setted 256 -> 32768/256=128
			return GetXboxButtonValue(controller, xboxButton) / 256;
		}
		else // if we are mapping this axis to an xbox button
		{
			// slowly increment the axis when the button is down
			if (GetXboxButtonValue(controller, xboxButton))
			{
				m_cur64ThumbVal[controller][n64Button] += 1;

				if (m_cur64ThumbVal[controller][n64Button] > 115)
					m_cur64ThumbVal[controller][n64Button] = 115;
			}
			else // reduce the axis by half when the button is released
			{
				//m_cur64ThumbVal[controller][n64Button] /= 2;
                m_cur64ThumbVal[controller][n64Button] = 0;
			}

			return m_cur64ThumbVal[controller][n64Button];
		}
	}
	else // if its a n64 button
	{
		// if we are mapping this button to an xbox axis
		if (IS_XBOX_AXIS(xboxButton))
		{
			// if the axis is moved across greater than 30000 then its
			// considered a button push
			return (GetXboxButtonValue(controller, xboxButton) > 30000);
		}
		else // if we are mapping this button to an xbox button
		{
			return ((GetXboxButtonValue(controller, xboxButton))? 1:0);
		}
	}
}
