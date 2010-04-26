#include "Panel.h"
#include <xbfont.h>
#include <xbapp.h>
#include "XBInput.h"
#include "config.h"


extern CXBFont m_Font;
typedef unsigned char byte;
extern int controller;
extern int ControllerConfig[72];

WCHAR controlname[18][99] = {	
	L"Analog Left",			
	L"Analog Right",			
	L"Analog Up",				
	L"Analog Down",			

	L"DPad Left",				
	L"DPad Right",			
	L"DPad Up",				
	L"DPad Down",				

	L"CButton Left",			
	L"CButton Right",		
	L"CButton Up",			
	L"CButton Down",			

	L"\410\411",				

	L"\400",					
	L"\401",					

	L"Z Trigger",				
	L"\406",		
	L"\407",	};

WCHAR controlxboxname[24][99] = {
	L"Left Analog Left",
	L"Left Analog Right",		
	L"Left Analog Up",		
	L"Left Analog Down",		

	L"Right Analog Left",		
	L"Right Analog Right",		
	L"Right Analog Up",		
	L"Right Analog Down",		

	L"DPad Left",			
	L"DPad Right",			
	L"DPad Up",				
	L"DPad Down",			

	L"\410\411",				
	L"\412\413",				

	L"Left Analog Button",		
	L"Right Analog Button",	

	L"\400",					
	L"\401",					
	L"\402",					
	L"\403",					

	L"\405",				
	L"\404",				

	L"\406",			
	L"\407",	};

WCHAR controlxboxcorrect[18][99];

void Fillcorrectcontrols(){
	for (int i=0;i<18;i++){
		swprintf(controlxboxcorrect[i],controlxboxname[ControllerConfig[(controller*18)+i]]);
	}}

extern CPanel m_BgPanel;
int selectcontrol=1;
int selectedelement=0;
bool changebutton=false;

void Drawcontrol()
{
	m_BgPanel.Render(15,15);
	WCHAR m_currentname[99];
	Fillcorrectcontrols();
	m_Font.Begin();
	m_Font.DrawText(270, 20, 0x99FF7F7f,L"Nintendo 64" , XBFONT_RIGHT);
	m_Font.DrawText(370, 20, 0x99FF7F7f,L"XBOX" , XBFONT_LEFT);
	m_Font.DrawText(60, 380, 0x99EEEEEE,L"\402 Reset" , XBFONT_LEFT);
	m_Font.DrawText(60, 405, 0x99EEEEEE,L"\400 Change" , XBFONT_LEFT);
    m_Font.DrawText(60, 430, 0x99EEEEEE,L"\401 Back" , XBFONT_LEFT);
	m_Font.End();
	for (int j=0;j<3;j++){
		int y=40;
		int x,center;
	for (int i = 0;i<18;i++,y=y+23) {
		switch (j){
		case 0:  swprintf(m_currentname,L"%s",controlname[i]);
			     x=310;
				 center = XBFONT_RIGHT;
				 break;
		case 1:  swprintf(m_currentname,L":");
                 x=320;
				 center = XBFONT_CENTER_X;
				 break;
		case 2:  if ((i== selectedelement)&&(changebutton))
				 swprintf(m_currentname,L"Press a Button",controlname[i]);
		         else 
	             swprintf(m_currentname,L"%s",controlxboxcorrect[i]);
			     x=330;
				 center = XBFONT_LEFT;
				 break;}

	m_Font.Begin();
	if (i== selectedelement){
		m_Font.DrawText(x, y, 0xFFFF7F7f, m_currentname, center);}
	else {
	m_Font.DrawText(x, y, 0xFFEEEEEE, m_currentname, center);}
	m_Font.End();
	}}
}


void ChangeControl()
{
	bool quitcontrol=false;
	Sleep(500);
	XBInput_GetInput();
    float tempdeadzone = XBOX_CONTROLLER_DEAD_ZONE;
//	XBOX_CONTROLLER_DEAD_ZONE = 15000;
	while (!quitcontrol){
	XBInput_GetInput();
	if (g_Gamepads[controller].sThumbLX < -15000)
	    {
        ControllerConfig[(controller*18)+selectedelement]=0;
		quitcontrol=true;}
	if (g_Gamepads[controller].sThumbLX > 15000)
	    {
        ControllerConfig[(controller*18)+selectedelement]=1;
		quitcontrol=true;}
	if (g_Gamepads[controller].sThumbLY > 15000)
	    {
        ControllerConfig[(controller*18)+selectedelement]=2;
		quitcontrol=true;}
	if (g_Gamepads[controller].sThumbLY < -15000)
	    {
        ControllerConfig[(controller*18)+selectedelement]=3;
		quitcontrol=true;}
	if (g_Gamepads[controller].sThumbRX < -15000)
	    {
        ControllerConfig[(controller*18)+selectedelement]=4;
		quitcontrol=true;}
	if (g_Gamepads[controller].sThumbRX > 15000)
	    {
        ControllerConfig[(controller*18)+selectedelement]=5;
		quitcontrol=true;}
	if (g_Gamepads[controller].sThumbRY > 15000)
	    {
        ControllerConfig[(controller*18)+selectedelement]=6;
		quitcontrol=true;}
	if (g_Gamepads[controller].sThumbRY < -15000)
	    {
        ControllerConfig[(controller*18)+selectedelement]=7;
		quitcontrol=true;}
	if (g_Gamepads[controller].wButtons & XINPUT_GAMEPAD_DPAD_LEFT)
	    {
        ControllerConfig[(controller*18)+selectedelement]=8;
		quitcontrol=true;}
	if (g_Gamepads[controller].wButtons & XINPUT_GAMEPAD_DPAD_RIGHT)
	    {
        ControllerConfig[(controller*18)+selectedelement]=9;
		quitcontrol=true;}
	if (g_Gamepads[controller].wButtons & XINPUT_GAMEPAD_DPAD_UP)
	    {
        ControllerConfig[(controller*18)+selectedelement]=10;
		quitcontrol=true;}
	if (g_Gamepads[controller].wButtons & XINPUT_GAMEPAD_DPAD_DOWN)
	    {
        ControllerConfig[(controller*18)+selectedelement]=11;
		quitcontrol=true;}
	if (g_Gamepads[controller].wButtons & XINPUT_GAMEPAD_START)
	    {
        ControllerConfig[(controller*18)+selectedelement]=12;
		quitcontrol=true;}
	if (g_Gamepads[controller].wButtons & XINPUT_GAMEPAD_BACK)
	    {
        ControllerConfig[(controller*18)+selectedelement]=13;
		quitcontrol=true;}
	if (g_Gamepads[controller].wButtons & XINPUT_GAMEPAD_LEFT_THUMB)
	    {
        ControllerConfig[(controller*18)+selectedelement]=14;
		quitcontrol=true;}
	if (g_Gamepads[controller].wButtons & XINPUT_GAMEPAD_RIGHT_THUMB)
	    {
        ControllerConfig[(controller*18)+selectedelement]=15;
		quitcontrol=true;}
	if(g_Gamepads[controller].bAnalogButtons[XINPUT_GAMEPAD_A])
	    {
        ControllerConfig[(controller*18)+selectedelement]=16;
		quitcontrol=true;}
	if(g_Gamepads[controller].bAnalogButtons[XINPUT_GAMEPAD_B])
	    {
        ControllerConfig[(controller*18)+selectedelement]=17;
		quitcontrol=true;}
	if(g_Gamepads[controller].bAnalogButtons[XINPUT_GAMEPAD_X])
	    {
        ControllerConfig[(controller*18)+selectedelement]=18;
		quitcontrol=true;}
	if(g_Gamepads[controller].bAnalogButtons[XINPUT_GAMEPAD_Y])
	    {
        ControllerConfig[(controller*18)+selectedelement]=19;
		quitcontrol=true;}
	if(g_Gamepads[controller].bAnalogButtons[XINPUT_GAMEPAD_BLACK])
	    {
        ControllerConfig[(controller*18)+selectedelement]=20;
		quitcontrol=true;}
	if(g_Gamepads[controller].bAnalogButtons[XINPUT_GAMEPAD_WHITE])
	    {
        ControllerConfig[(controller*18)+selectedelement]=21;
		quitcontrol=true;}
	if(g_Gamepads[controller].bAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER])
	    {
        ControllerConfig[(controller*18)+selectedelement]=22;
		quitcontrol=true;}
	if(g_Gamepads[controller].bAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER])
	    {
        ControllerConfig[(controller*18)+selectedelement]=23;
	    quitcontrol=true;}
	}
	changebutton=false;
	//XBOX_CONTROLLER_DEAD_ZONE = tempdeadzone;
}

void ControllerReset(){
ControllerConfig[(controller*18)+0]=0x00;
ControllerConfig[(controller*18)+1]=0x01;
ControllerConfig[(controller*18)+2]=0x02;
ControllerConfig[(controller*18)+3]=0x03;
ControllerConfig[(controller*18)+4]=0x08;
ControllerConfig[(controller*18)+5]=0x09;
ControllerConfig[(controller*18)+6]=0x0A;
ControllerConfig[(controller*18)+7]=0x0B;
ControllerConfig[(controller*18)+8]=0x04;
ControllerConfig[(controller*18)+9]=0x05;
ControllerConfig[(controller*18)+10]=0x06;
ControllerConfig[(controller*18)+11]=0x07;
ControllerConfig[(controller*18)+12]=0x0C;
ControllerConfig[(controller*18)+13]=0x10;
ControllerConfig[(controller*18)+14]=0x12;
ControllerConfig[(controller*18)+15]=0x16;
ControllerConfig[(controller*18)+16]=0x0D;
ControllerConfig[(controller*18)+17]=0x17;
}