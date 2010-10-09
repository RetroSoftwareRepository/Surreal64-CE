#include "config.h"
#include <xbapp.h>
#include <xbresource.h>

#ifdef DEBUG
bool showdebug = true;
#else
bool showdebug = false;
#endif

extern "C" void loadinis();
void LoadSkinFile();
extern "C" int loaddwPJ64PagingMem();
extern "C" int loaddwPJ64DynaMem();
extern "C" int loaddw1964PagingMem();
extern "C" int loaddw1964DynaMem();
void WriteSkinFile();

// Ez0n3 - use iAudioPlugin instead to determine which audio plugin is used
extern "C" int loadiAudioPlugin();

// Ez0n3 - reinstate max video mem
extern "C" int loaddwMaxVideoMem();

//freakdave
int dw1964DynaMem=8;
int dw1964PagingMem=4;
int dwPJ64DynaMem=16;
int dwPJ64PagingMem=4;

// Ez0n3 - use iAudioPlugin instead to determine which audio plugin is used
bool bUseLLERSP=false; // leave this in case it's set in ini
int iAudioPlugin = _AudioPluginJttl; // 2=JttL

// Ez0n3 - reinstate max free
int dwMaxVideoMem=5;
bool has128ram; // determine if the current phys ram is greater than 100MB: 128MB = true, 64MB = false

// Ez0n3 - launch screens enable / disable
bool HideLaunchScreens=0;

int videoplugin = _VideoPluginRice560; // Ez0n3 - 2=rice560
char romCRC[32];
int romcounter = 0;
bool onhd;
int FlickerFilter = 1;
bool SoftDisplayFilter = 0;
int TextureMode = 3;
int VertexMode = 1;
bool FrameSkip = FALSE;
float XBOX_CONTROLLER_DEAD_ZONE = 8600; // also change in SurrealMenu.cpp
float Deadzone = 26;
unsigned int Sensitivity = 10;
char skinname[32] = "default";

char romname[256];


int ControllerConfig[72] 
		= {0x00,0x01,0x02,0x03,0x08,0x09,0x0A,0x0B,
		   0x04,0x05,0x06,0x07,0x0C,0x10,0x12,0x16,
		   0x0D,0x17,0x00,0x01,0x02,0x03,0x08,0x09,
		   0x0A,0x0B,0x04,0x05,0x06,0x07,0x0C,0x10,
		   0x12,0x16,0x0D,0x17,0x00,0x01,0x02,0x03,
		   0x08,0x09,0x0A,0x0B,0x04,0x05,0x06,0x07,
		   0x0C,0x10,0x12,0x16,0x0D,0x17,0x00,0x01,
		   0x02,0x03,0x08,0x09,0x0A,0x0B,0x04,0x05,
		   0x06,0x07,0x0C,0x10,0x12,0x16,0x0D,0x17};

int ControllerReset[18]  = 
          {0x00,0x01,0x02,0x03,0x08,0x09,0x0A,0x0B,
		   0x04,0x05,0x06,0x07,0x0C,0x10,0x12,0x16,
		   0x0D,0x17};

enum Pak
{
NoPak,
MemPak,
RumblePak,
};
//Skin.ini Defaults
//Font Colors
DWORD dwTitleColor = 0xFF53B77F;
DWORD dwMenuItemColor = 0xCCEEEEEE;
DWORD dwMenuTitleColor = 0xFF8080FF;
DWORD dwRomListColor = 0xAAEEEEEE;
DWORD dwSelectedRomColor = 0xFFFF8000;
DWORD dwNullItemColor = 0xEE53B77F;

//Launcher XLMenu Coords
int iContMenuPosX = 60;
int iContMenuPosY = 80;
int iContSetMenuPosX = 60;
int iContSetMenuPosY = 80;
int iCreditsPosX = 60;
int iCreditsPosY = 80;
int iLaunchMenuPosX = 210;
int iLaunchMenuPosY = 160;
int iMainMenuPosX = 60;
int iMainMenuPosY = 80;
int iSettingsMenuPosX = 60;
int iSettingsMenuPosY = 80;
int iSkinMenuPosX = 60;
int iSkinMenuPosY = 80;
int iVidSetMenuPosX = 60;
int iVidSetMenuPosY = 80;

//IGM XLMenu Coords
int iIGMContPosX = 60;
int iIGMContPosY = 80;
int iIGMContSetPosX = 60;
int iIGMContSetPosY = 80;
int iIGMLoadPosX = 60;
int iIGMLoadPosY = 80;
int iIGMSavePosX = 60;
int iIGMSavePosY = 80;
int iIGMMainPosX = 60;
int iIGMMainPosY = 80;
int iIGMVidSetPosX = 60;
int iIGMVidSetPosY = 80;

//Launcher Coords
int iInfoPosX = 50;
int iInfoPosY = 310;
int iBoxPosX = 430;
int iBoxPosY = 30;
int iTitleX = 305;
int iTitleY = 20;

//IGM Coords
int iIGMTitleX = 305;
int iIGMTitleY = 20;
int iPanelX = 305;
int iPanelY = 65;
int iPanelNH = 267;
int iPanelNW = 200;

//RomList Coords
int iRomListPosX = 45;
int iRomListPosY = 45;
int GAMESEL_MaxWindowList = 12;
int RomListTrunc = 43;
int MenuTrunc = 256;

int DefaultPak = RumblePak;

//freakdave - now all controllers are enabled by default, users don't seem to know how to change it in Surreal's Menu
bool EnableController1 = true;
bool EnableController2 = true;
bool EnableController3 = true;
bool EnableController4 = true;

extern int actualrom;
int preferedemu=0;

void ResetDefaults()
{
preferedemu=0;
dw1964DynaMem=8;
dw1964PagingMem=4;
dwPJ64DynaMem=16;
dwPJ64PagingMem=4;

// Ez0n3 - use iAudioPlugin instead to determine if basic audio is used
bUseLLERSP=false; // leave this in case it's set in ini

// Ez0n3 - reinstate max free mem
dwMaxVideoMem=5;

DefaultPak = RumblePak;
EnableController1 = true;
EnableController2 = true;
EnableController3 = true;
EnableController4 = true;
XBOX_CONTROLLER_DEAD_ZONE = 8600;
Deadzone = 26;
Sensitivity = 10;
videoplugin = _VideoPluginRice560; // Ez0n3 - 2=rice560

//Ez0n3 - plugins
iAudioPlugin = _AudioPluginJttl; //2=jttl

for (int i=0;i<4;i++) {
for (int j=0;j<18;j++){
ControllerConfig[(i*18)+j]=ControllerReset[j];}}

}

// Read in the config file for the whole application
int ConfigAppLoad()
{
	char szLine[256];
	FILE *h;

	if (onhd){
	if ((h = fopen("D:\\ini\\surreal-xxx.ini", "rt")) == NULL) {
		return 1;
	}}
	else {
	if ((h = fopen("T:\\ini\\surreal-xxx.ini", "rt")) == NULL) {
		return 1;
	}}

	// Go through each line of the config file
	while (1) {
		int nLen;
		if (fgets(szLine, sizeof(szLine), h) == NULL) {	// End of config file
			break;
		}

		nLen = strlen(szLine);
		
		// Get rid of the linefeed at the end
		if (szLine[nLen - 1] == 10) {
			szLine[nLen - 1] = 0;
			nLen--;
		}

#define BOL(x) { char *szValue = LabelCheck(szLine,#x);		\
	if (szValue) x = strtol(szValue, NULL, 0) != 0; }
#define VAR(x) { char *szValue = LabelCheck(szLine,#x);		\
	if (szValue) x = strtol(szValue, NULL, 0); }
#define FLT(x) { char *szValue = LabelCheck(szLine,#x);		\
	if (szValue) x = atof(szValue); }
#define STR(x) { char *szValue = LabelCheck(szLine,#x " ");	\
	if (szValue) strcpy(x,szValue); }

        VAR(VertexMode);
		VAR(FlickerFilter);
		BOL(SoftDisplayFilter);
		VAR(TextureMode);
		VAR(romcounter);
		BOL(onhd);
		STR(skinname);

		// Ez0n3 - show launch screens
		BOL(HideLaunchScreens);

#undef STR
#undef FLT
#undef VAR
#undef BOL

	}
	fclose(h);
	return 0;
}

// Write out the config file for the whole application

int ConfigAppSave()
{
	FILE *h;

	if(onhd){
		if ((h = fopen("D:\\ini\\surreal-xxx.ini", "wt")) == NULL) return 1;}
	else{
		if ((h = fopen("T:\\ini\\surreal-xxx.ini", "wt")) == NULL) return 1;}

	// Write title
	fprintf(h,"// Surreal64 XXX Config File\n\n");
	fprintf(h,"// Don't edit this file manually unless you know what you're doing\n");
	fprintf(h,"// Surreal will restore default settings when this file is deleted\n");

#define BOL(x) fprintf(h, #x " %d\n", (int)(x != 0))
#define VAR(x) fprintf(h, #x " %d\n", x)
#define FLT(x) fprintf(h, #x " %f\n", x)
#define STR(x) fprintf(h, #x " %s\n", x)
	
	fprintf(h,"\n\n\n");
    
	VAR(VertexMode);
	VAR(FlickerFilter);
	BOL(SoftDisplayFilter);
	VAR(TextureMode);
	VAR(romcounter);
	BOL(onhd);
	STR(skinname);

	// Ez0n3 - show launch screens
	BOL(HideLaunchScreens);

#undef STR
#undef FLT
#undef VAR
#undef BOL

	fclose(h);
	return 0;
}


int ConfigAppSave2()
{
	FILE *h;
	char szConfig[260];

	if(onhd)
	sprintf(szConfig, "D:\\ini\\games\\%s.ini", romCRC);
	else
	sprintf(szConfig, "T:\\ini\\games\\%s.ini", romCRC);

	if ((h = fopen(szConfig, "wt")) == NULL) return 1;

#define BOL(x) fprintf(h, #x " %d\n", (int)(x != 0))
#define VAR(x) fprintf(h, #x " %d\n", x)
#define FLT(x) fprintf(h, #x " %f\n", x)
#define STR(x) fprintf(h, #x " %s\n", x)
	
	fprintf(h,"\n\n\n");

	VAR(preferedemu);

	FLT(XBOX_CONTROLLER_DEAD_ZONE);
	FLT(Deadzone);
	VAR(Sensitivity);
	VAR(DefaultPak);
	BOL(EnableController1);
	BOL(EnableController2);
	BOL(EnableController3);
	BOL(EnableController4);

	VAR(dw1964DynaMem);
	VAR(dw1964PagingMem);
	VAR(dwPJ64DynaMem);
	VAR(dwPJ64PagingMem);
	BOL(bUseLLERSP); // Ez0n3 - use iAudioPlugin instead
	BOL(FrameSkip);
	
	// Ez0n3 - reinstate max video mem
	VAR(dwMaxVideoMem);
	VAR(videoplugin);
	VAR(iAudioPlugin);
	

	VAR(ControllerConfig[0]);
	VAR(ControllerConfig[1]);
	VAR(ControllerConfig[2]);
	VAR(ControllerConfig[3]);
	VAR(ControllerConfig[4]);
	VAR(ControllerConfig[5]);
	VAR(ControllerConfig[6]);
	VAR(ControllerConfig[7]);
	VAR(ControllerConfig[8]);
	VAR(ControllerConfig[9]);
	VAR(ControllerConfig[10]);
	VAR(ControllerConfig[11]);
	VAR(ControllerConfig[12]);
	VAR(ControllerConfig[13]);
	VAR(ControllerConfig[14]);
	VAR(ControllerConfig[15]);
	VAR(ControllerConfig[16]);
	VAR(ControllerConfig[17]);
	VAR(ControllerConfig[18]);
	VAR(ControllerConfig[19]);
	VAR(ControllerConfig[20]);
	VAR(ControllerConfig[21]);
	VAR(ControllerConfig[22]);
	VAR(ControllerConfig[23]);
	VAR(ControllerConfig[24]);
	VAR(ControllerConfig[25]);
	VAR(ControllerConfig[26]);
	VAR(ControllerConfig[27]);
	VAR(ControllerConfig[28]);
	VAR(ControllerConfig[29]);
	VAR(ControllerConfig[30]);
	VAR(ControllerConfig[31]);
	VAR(ControllerConfig[32]);
	VAR(ControllerConfig[33]);
	VAR(ControllerConfig[34]);
	VAR(ControllerConfig[35]);
	VAR(ControllerConfig[36]);
	VAR(ControllerConfig[37]);
	VAR(ControllerConfig[38]);
	VAR(ControllerConfig[39]);
	VAR(ControllerConfig[40]);
	VAR(ControllerConfig[41]);
	VAR(ControllerConfig[42]);
	VAR(ControllerConfig[43]);
	VAR(ControllerConfig[44]);
	VAR(ControllerConfig[45]);
	VAR(ControllerConfig[46]);
	VAR(ControllerConfig[47]);
	VAR(ControllerConfig[48]);
	VAR(ControllerConfig[49]);
	VAR(ControllerConfig[50]);
	VAR(ControllerConfig[51]);
	VAR(ControllerConfig[52]);
	VAR(ControllerConfig[53]);
	VAR(ControllerConfig[54]);
	VAR(ControllerConfig[55]);
	VAR(ControllerConfig[56]);
	VAR(ControllerConfig[57]);
	VAR(ControllerConfig[58]);
	VAR(ControllerConfig[59]);
	VAR(ControllerConfig[60]);
	VAR(ControllerConfig[61]);
	VAR(ControllerConfig[62]);
	VAR(ControllerConfig[63]);
	VAR(ControllerConfig[64]);
	VAR(ControllerConfig[65]);
	VAR(ControllerConfig[66]);
	VAR(ControllerConfig[67]);
	VAR(ControllerConfig[68]);
	VAR(ControllerConfig[69]);
	VAR(ControllerConfig[70]);
	VAR(ControllerConfig[71]);

#undef STR
#undef FLT
#undef VAR
#undef BOL
	fclose(h);
	return 0;
}


int ConfigAppLoad2()
{
	FILE *h;
	char szConfig[260];
	char szLine[256];

	if (onhd)
	sprintf(szConfig, "D:\\ini\\games\\%s.ini", romCRC);
	else
	sprintf(szConfig, "T:\\ini\\games\\%s.ini", romCRC);

	if ((h = fopen(szConfig, "rt")) == NULL) {
        ResetDefaults();	
		return 1;}

	// Go through each line of the config file
	while (1) {
		int nLen;
		if (fgets(szLine, sizeof(szLine), h) == NULL) {	// End of config file
			break;
		}

		nLen = strlen(szLine);
		
		// Get rid of the linefeed at the end
		if (szLine[nLen - 1] == 10) {
			szLine[nLen - 1] = 0;
			nLen--;
		}

#define BOL(x) { char *szValue = LabelCheck(szLine,#x);		\
	if (szValue) x = strtol(szValue, NULL, 0) != 0; }
#define VAR(x) { char *szValue = LabelCheck(szLine,#x);		\
	if (szValue) x = strtol(szValue, NULL, 0); }
#define FLT(x) { char *szValue = LabelCheck(szLine,#x);		\
	if (szValue) x = atof(szValue); }
#define STR(x) { char *szValue = LabelCheck(szLine,#x " ");	\
	if (szValue) strcpy(x,szValue); }

	
		VAR(preferedemu);
		FLT(XBOX_CONTROLLER_DEAD_ZONE);
		FLT(Deadzone);
		VAR(Sensitivity);
		VAR(DefaultPak);
		BOL(EnableController1);
		BOL(EnableController2);
		BOL(EnableController3);
		BOL(EnableController4);

		VAR(dw1964DynaMem);
		VAR(dw1964PagingMem);
		VAR(dwPJ64DynaMem);
		VAR(dwPJ64PagingMem);
		BOL(bUseLLERSP); // Ez0n3 - use iAudioPlugin instead, but leave this in in case it's set in ini
		BOL(FrameSkip);

		// Ez0n3 - reinstate max video mem
		VAR(dwMaxVideoMem);
		VAR(videoplugin);
		VAR(iAudioPlugin);

        VAR(ControllerConfig[0]);
        VAR(ControllerConfig[1]);
        VAR(ControllerConfig[2]);
        VAR(ControllerConfig[3]);
        VAR(ControllerConfig[4]);
        VAR(ControllerConfig[5]);
        VAR(ControllerConfig[6]);
        VAR(ControllerConfig[7]);
        VAR(ControllerConfig[8]);
        VAR(ControllerConfig[9]);
        VAR(ControllerConfig[10]);
        VAR(ControllerConfig[11]);
        VAR(ControllerConfig[12]);
        VAR(ControllerConfig[13]);
        VAR(ControllerConfig[14]);
        VAR(ControllerConfig[15]);
        VAR(ControllerConfig[16]);
        VAR(ControllerConfig[17]);
        VAR(ControllerConfig[18]);
        VAR(ControllerConfig[19]);
        VAR(ControllerConfig[20]);
        VAR(ControllerConfig[21]);
        VAR(ControllerConfig[22]);
        VAR(ControllerConfig[23]);
        VAR(ControllerConfig[24]);
        VAR(ControllerConfig[25]);
        VAR(ControllerConfig[26]);
        VAR(ControllerConfig[27]);
        VAR(ControllerConfig[28]);
        VAR(ControllerConfig[29]);
        VAR(ControllerConfig[30]);
        VAR(ControllerConfig[31]);
        VAR(ControllerConfig[32]);
        VAR(ControllerConfig[33]);
        VAR(ControllerConfig[34]);
        VAR(ControllerConfig[35]);
        VAR(ControllerConfig[36]);
        VAR(ControllerConfig[37]);
        VAR(ControllerConfig[38]);
        VAR(ControllerConfig[39]);
        VAR(ControllerConfig[40]);
        VAR(ControllerConfig[41]);
        VAR(ControllerConfig[42]);
        VAR(ControllerConfig[43]);
        VAR(ControllerConfig[44]);
        VAR(ControllerConfig[45]);
        VAR(ControllerConfig[46]);
        VAR(ControllerConfig[47]);
        VAR(ControllerConfig[48]);
        VAR(ControllerConfig[49]);
        VAR(ControllerConfig[50]);
        VAR(ControllerConfig[51]);
        VAR(ControllerConfig[52]);
        VAR(ControllerConfig[53]);
        VAR(ControllerConfig[54]);
        VAR(ControllerConfig[55]);
        VAR(ControllerConfig[56]);
        VAR(ControllerConfig[57]);
        VAR(ControllerConfig[58]);
        VAR(ControllerConfig[59]);
        VAR(ControllerConfig[60]);
        VAR(ControllerConfig[61]);
        VAR(ControllerConfig[62]);
        VAR(ControllerConfig[63]);
        VAR(ControllerConfig[64]);
        VAR(ControllerConfig[65]);
        VAR(ControllerConfig[66]);
        VAR(ControllerConfig[67]);
        VAR(ControllerConfig[68]);
        VAR(ControllerConfig[69]);
        VAR(ControllerConfig[70]);
        VAR(ControllerConfig[71]);
	
#undef STR
#undef FLT
#undef VAR
#undef BOL
	}
	fclose(h);
	return 0;
}


int ConfigAppSaveTemp()
{
	FILE *h;

	if(onhd){
		if ((h = fopen("D:\\Temp.ini", "wt")) == NULL) return 1;}
	else {
		if ((h = fopen("T:\\Temp.ini", "wt")) == NULL) return 1;}

#define BOL(x) fprintf(h, #x " %d\n", (int)(x != 0))
#define VAR(x) fprintf(h, #x " %d\n", x)
#define FLT(x) fprintf(h, #x " %f\n", x)
#define STR(x) fprintf(h, #x " %s\n", x)
	
	fprintf(h,"\n\n\n");
    
    STR(romname);
	STR(romCRC);

#undef STR
#undef FLT
#undef VAR
#undef BOL
	fclose(h);
	return 0;
}


int ConfigAppLoadTemp()
{
	FILE *h;
	char szLine[256];

	if(onhd){
		if ((h = fopen("D:\\Temp.ini", "rt")) == NULL) return 1;}
	else {
		if ((h = fopen("T:\\Temp.ini", "rt")) == NULL) return 1;}

	// Go through each line of the config file
	while (1) {
		int nLen;
		if (fgets(szLine, sizeof(szLine), h) == NULL) {	// End of config file
			break;
		}

		nLen = strlen(szLine);
		
		// Get rid of the linefeed at the end
		if (szLine[nLen - 1] == 10) {
			szLine[nLen - 1] = 0;
			nLen--;
		}

#define BOL(x) { char *szValue = LabelCheck(szLine,#x);		\
	if (szValue) x = strtol(szValue, NULL, 0) != 0; }
#define VAR(x) { char *szValue = LabelCheck(szLine,#x);		\
	if (szValue) x = strtol(szValue, NULL, 0); }
#define FLT(x) { char *szValue = LabelCheck(szLine,#x);		\
	if (szValue) x = atof(szValue); }
#define STR(x) { char *szValue = LabelCheck(szLine,#x " ");	\
	if (szValue) strcpy(x,szValue); }

	STR(romname);
    STR(romCRC);

#undef STR
#undef FLT
#undef VAR
#undef BOL
	}

	fclose(h);
	return 0;
}

//Load Skin File 
void LoadSkinFile(){
	FILE* f;
	std::string PATH = "D:\\skins\\";
	PATH += skinname;
	PATH += "\\Skin.ini";
	f = fopen(PATH.c_str() , "r");


	//Build ini if nonexistant
	if(!f) {
		FILE* f;
		std::string PATH = "D:\\skins\\";
		PATH += skinname;
		PATH += "\\Skin.ini";
		f = fopen(PATH.c_str(), "w");
		fclose(f);

		WriteSkinFile();
		return;
	}
	if(f){
		char line[100];
		char szTitleColor[10];
		char tmpbuf[11];
		

		fgets(line,100, f);
		for(int i = 0; i < 100; i++) {
			if(line[i] == '=') {
				for(int j = 0; j < 10; j++){

					szTitleColor[j] = line[i+1+j]; 
				}
				memcpy(tmpbuf, szTitleColor, 10);
				tmpbuf[10] = '\0';
				dwTitleColor = strtoul(tmpbuf, NULL, 16);
				break;
			}
		}
		char line2[100];
		char szMenuTitleColor[10];

		fgets(line2,100, f);
		for(int i = 0; i < 100; i++) {
			if(line2[i] == '=') {
				for(int j = 0; j < 10; j++){

					szMenuTitleColor[j] = line2[i+1+j]; 
				}
				memcpy(tmpbuf, szMenuTitleColor, 10);
				tmpbuf[10] = '\0';
				dwMenuTitleColor = strtoul(tmpbuf, NULL, 16);
				break;
			}
		}
		char line3[100];
		char szRomListColor[10];

		fgets(line3,100, f);
		for(int i = 0; i < 100; i++) {
			if(line3[i] == '=') {
				for(int j = 0; j < 10; j++){

					szRomListColor[j] = line3[i+1+j]; 
				}
				memcpy(tmpbuf, szRomListColor, 10);
				tmpbuf[10] = '\0';
				dwRomListColor = strtoul(tmpbuf, NULL, 16);
				break;
			}
		}
		char line4[100];
		char szSelectedRomColor[10];

		fgets(line4,100, f);
		for(int i = 0; i < 100; i++) {
			if(line4[i] == '=') {
				for(int j = 0; j < 10; j++){
					
					szSelectedRomColor[j] = line4[i+1+j]; 
				}
				memcpy(tmpbuf, szSelectedRomColor, 10);
				tmpbuf[10] = '\0';
				dwSelectedRomColor = strtoul(tmpbuf, NULL, 16);
				break;
			}
		}
		char line5[100];
		char szMenuItemColor[10];

		fgets(line5,100, f);
		for(int i = 0; i < 100; i++) {
			if(line5[i] == '=') {
				for(int j = 0; j < 10; j++){

					szMenuItemColor[j] = line5[i+1+j]; 
				}

				memcpy(tmpbuf, szMenuItemColor, 10);
				tmpbuf[10] = '\0';
				dwMenuItemColor = strtoul(tmpbuf, NULL, 16);
				break;
			}
		}
		char line6[100];
		char szNullItemColor[10];

		fgets(line6,100, f);
		for(int i = 0; i < 100; i++) {
			if(line6[i] == '=') {
				for(int j = 0; j < 10; j++){

					szNullItemColor[j] = line6[i+1+j]; 
				}
				memcpy(tmpbuf, szNullItemColor, 10);
				tmpbuf[10] = '\0';
				dwNullItemColor = strtoul(tmpbuf, NULL, 16);
				break;
			}
		}
		//Load Coords
		char line7[100];
		fgets(line7,100, f);
		for(int i = 0; i < 100; i++) {
			if(line7[i] == '=') {
				iContMenuPosX = atoi(&line7[i+1]); 
				break;
			}
		}
		char line8[100];
		fgets(line8,100, f);
		for(int i = 0; i < 100; i++) {
			if(line8[i] == '=') {
				iContMenuPosY = atoi(&line8[i+1]); 
				break;
			}
		}
		char line9[100];
		fgets(line9,100, f);
		for(int i = 0; i < 100; i++) {
			if(line9[i] == '=') {
				iContSetMenuPosX = atoi(&line9[i+1]); 
				break;
			}
		}
		char line10[100];
		fgets(line10,100, f);
		for(int i = 0; i < 100; i++) {
			if(line10[i] == '=') {
				iContSetMenuPosY = atoi(&line10[i+1]); 
				break;
			}
		}
		char line11[100];
		fgets(line11,100, f);
		for(int i = 0; i < 100; i++) {
			if(line11[i] == '=') {
				iCreditsPosX = atoi(&line11[i+1]); 
				break;
			}
		}
		char line12[100];
		fgets(line12,100, f);
		for(int i = 0; i < 100; i++) {
			if(line12[i] == '=') {
				iCreditsPosY = atoi(&line12[i+1]); 
				break;
			}
		}
		char line13[100];
		fgets(line13,100, f);
		for(int i = 0; i < 100; i++) {
			if(line13[i] == '=') {
				iLaunchMenuPosX = atoi(&line13[i+1]); 
				break;
			}
		}
		char line14[100];
		fgets(line14,100, f);
		for(int i = 0; i < 100; i++) {
			if(line14[i] == '=') {
				iLaunchMenuPosY = atoi(&line14[i+1]); 
				break;
			}
		}
		char line15[100];
		fgets(line15,100, f);
		for(int i = 0; i < 100; i++) {
			if(line15[i] == '=') {
				iMainMenuPosX = atoi(&line15[i+1]); 
				break;
			}
		}
		char line16[100];
		fgets(line16,100, f);
		for(int i = 0; i < 100; i++) {
			if(line16[i] == '=') {
				iMainMenuPosY = atoi(&line16[i+1]); 
				break;
			}
		}
		char line17[100];
		fgets(line17,100, f);
		for(int i = 0; i < 100; i++) {
			if(line17[i] == '=') {
				iSettingsMenuPosX = atoi(&line17[i+1]); 
				break;
			}
		}
		char line18[100];
		fgets(line18,100, f);
		for(int i = 0; i < 100; i++) {
			if(line18[i] == '=') {
				iSettingsMenuPosY = atoi(&line18[i+1]); 
				break;
			}
		}
		char line19[100];
		fgets(line19,100, f);
		for(int i = 0; i < 100; i++) {
			if(line19[i] == '=') {
				iSkinMenuPosX = atoi(&line19[i+1]); 
				break;
			}
		}
		char line20[100];
		fgets(line20,100, f);
		for(int i = 0; i < 100; i++) {
			if(line20[i] == '=') {
				iSkinMenuPosY = atoi(&line20[i+1]); 
				break;
			}
		}
		char line21[100];
		fgets(line21,100, f);
		for(int i = 0; i < 100; i++) {
			if(line21[i] == '=') {
				iVidSetMenuPosX = atoi(&line21[i+1]); 
				break;
			}
		}
		char line22[100];
		fgets(line22,100, f);
		for(int i = 0; i < 100; i++) {
			if(line22[i] == '=') {
				iVidSetMenuPosY = atoi(&line22[i+1]); 
				break;
			}
		}
		char line23[100];
		fgets(line23,100, f);
		for(int i = 0; i < 100; i++) {
			if(line23[i] == '=') {
				iIGMContPosX = atoi(&line23[i+1]); 
				break;
			}
		}
		char line24[100];
		fgets(line24,100, f);
		for(int i = 0; i < 100; i++) {
			if(line24[i] == '=') {
				iIGMContPosY = atoi(&line24[i+1]); 
				break;
			}
		}
		char line25[100];
		fgets(line25,100, f);
		for(int i = 0; i < 100; i++) {
			if(line25[i] == '=') {
				iIGMContSetPosX  = atoi(&line25[i+1]); 
				break;
			}
		}
		char line26[100];
		fgets(line26,100, f);
		for(int i = 0; i < 100; i++) {
			if(line26[i] == '=') {
				iIGMContSetPosY  = atoi(&line26[i+1]); 
				break;
			}
		}
		char line27[100];
		fgets(line27,100, f);
		for(int i = 0; i < 100; i++) {
			if(line27[i] == '=') {
				iIGMLoadPosX  = atoi(&line27[i+1]); 
				break;
			}
		}
		char line28[100];
		fgets(line28,100, f);
		for(int i = 0; i < 100; i++) {
			if(line28[i] == '=') {
				iIGMLoadPosY  = atoi(&line28[i+1]); 
				break;
			}
		}
		char line29[100];
		fgets(line29,100, f);
		for(int i = 0; i < 100; i++) {
			if(line29[i] == '=') {
				iIGMSavePosX  = atoi(&line29[i+1]); 
				break;
			}
		}
		char line30[100];
		fgets(line30,100, f);
		for(int i = 0; i < 100; i++) {
			if(line30[i] == '=') {
				iIGMSavePosY  = atoi(&line30[i+1]); 
				break;
			}
		}
		char line31[100];
		fgets(line31,100, f);
		for(int i = 0; i < 100; i++) {
			if(line31[i] == '=') {
				iIGMMainPosX  = atoi(&line31[i+1]); 
				break;
			}
		}
		char line32[100];
		fgets(line32,100, f);
		for(int i = 0; i < 100; i++) {
			if(line32[i] == '=') {
				iIGMMainPosY  = atoi(&line32[i+1]); 
				break;
			}
		}
		char line33[100];
		fgets(line33,100, f);
		for(int i = 0; i < 100; i++) {
			if(line33[i] == '=') {
				iIGMVidSetPosX  = atoi(&line33[i+1]); 
				break;
			}
		}
		char line34[100];
		fgets(line34,100, f);
		for(int i = 0; i < 100; i++) {
			if(line34[i] == '=') {
				iIGMVidSetPosY  = atoi(&line34[i+1]); 
				break;
			}
		}
		char line35[100];
		fgets(line35,100, f);
		for(int i = 0; i < 100; i++) {
			if(line35[i] == '=') {
				iIGMVidSetPosY  = atoi(&line35[i+1]); 
				break;
			}
		}
		char line36[100];
		fgets(line36,100, f);
		for(int i = 0; i < 100; i++) {
			if(line36[i] == '=') {
				iIGMVidSetPosY  = atoi(&line36[i+1]); 
				break;
			}
		}
		char line37[100];
		fgets(line37,100, f);
		for(int i = 0; i < 100; i++) {
			if(line37[i] == '=') {
				iIGMVidSetPosY  = atoi(&line37[i+1]); 
				break;
			}
		}
		char line38[100];
		fgets(line38,100, f);
		for(int i = 0; i < 100; i++) {
			if(line38[i] == '=') {
				iIGMVidSetPosY  = atoi(&line38[i+1]); 
				break;
			}
		}
		char line39[100];
		fgets(line39,100, f);
		for(int i = 0; i < 100; i++) {
			if(line39[i] == '=') {
				iInfoPosX  = atoi(&line39[i+1]); 
				break;
			}
		}
		char line40[100];
		fgets(line40,100, f);
		for(int i = 0; i < 100; i++) {
			if(line40[i] == '=') {
				iInfoPosY  = atoi(&line40[i+1]); 
				break;
			}
		}
		char line41[100];
		fgets(line41,100, f);
		for(int i = 0; i < 100; i++) {
			if(line41[i] == '=') {
				iBoxPosX  = atoi(&line41[i+1]); 
				break;
			}
		}
		char line42[100];
		fgets(line42,100, f);
		for(int i = 0; i < 100; i++) {
			if(line42[i] == '=') {
				iBoxPosY  = atoi(&line42[i+1]); 
				break;
			}
		}
		char line43[100];
		fgets(line43,100, f);
		for(int i = 0; i < 100; i++) {
			if(line43[i] == '=') {
				iTitleX  = atoi(&line43[i+1]); 
				break;
			}
		}
		char line44[100];
		fgets(line44,100, f);
		for(int i = 0; i < 100; i++) {
			if(line44[i] == '=') {
				iTitleY  = atoi(&line44[i+1]); 
				break;
			}
		}
		char line45[100];
		fgets(line45,100, f);
		for(int i = 0; i < 100; i++) {
			if(line45[i] == '=') {
				iIGMTitleX  = atoi(&line45[i+1]); 
				break;
			}
		}
		char line46[100];
		fgets(line46,100, f);
		for(int i = 0; i < 100; i++) {
			if(line46[i] == '=') {
				iIGMTitleY  = atoi(&line46[i+1]); 
				break;
			}
		}
		char line47[100];
		fgets(line47,100, f);
		for(int i = 0; i < 100; i++) {
			if(line47[i] == '=') {
				iPanelX  = atoi(&line47[i+1]); 
				break;
			}
		}
		char line48[100];
		fgets(line48,100, f);
		for(int i = 0; i < 100; i++) {
			if(line48[i] == '=') {
				iPanelY  = atoi(&line48[i+1]); 
				break;
			}
		}
		char line49[100];
		fgets(line49,100, f);
		for(int i = 0; i < 100; i++) {
			if(line49[i] == '=') {
				iPanelNW  = atoi(&line49[i+1]); 
				break;
			}
		}
		char line50[100];
		fgets(line50,100, f);
		for(int i = 0; i < 100; i++) {
			if(line50[i] == '=') {
				iPanelNH  = atoi(&line50[i+1]); 
				break;
			}
		}
		char line51[100];
		fgets(line51,100, f);
		for(int i = 0; i < 100; i++) {
			if(line51[i] == '=') {
				iRomListPosX  = atoi(&line51[i+1]); 
				break;
			}
		}
		char line52[100];
		fgets(line52,100, f);
		for(int i = 0; i < 100; i++) {
			if(line52[i] == '=') {
				iRomListPosY  = atoi(&line52[i+1]); 
				break;
			}
		}
		char line53[100];
		fgets(line53,100, f);
		for(int i = 0; i < 100; i++) {
			if(line53[i] == '=') {
				GAMESEL_MaxWindowList  = atoi(&line53[i+1]); 
				break;
			}
		}
		char line54[100];
		fgets(line54,100, f);
		for(int i = 0; i < 100; i++) {
			if(line54[i] == '=') {
				RomListTrunc  = atoi(&line54[i+1]); 
				break;
			}
		}
		char line55[100];
		fgets(line55,100, f);
		for(int i = 0; i < 100; i++) {
			if(line55[i] == '=') {
				MenuTrunc = atoi(&line55[i+1]); 
				break;
			}
		}
		
			
		
		

	}
	fclose(f);
	return;


}

void WriteSkinFile(){
	
	
	
	FILE* f;
		std::string PATH = "D:\\skins\\";
		PATH += skinname;
		PATH += "\\Skin.ini";
		f = fopen(PATH.c_str() , "w");
		if(f) {
			fprintf(f, "TitleColor=%ul\n",dwTitleColor);
			fprintf(f, "MenuTitleColor=%ul\n",dwMenuTitleColor);
			fprintf(f, "RomListColor=%ul\n",dwRomListColor);
			fprintf(f, "SelectedRomColor=%ul\n",dwSelectedRomColor);
			fprintf(f, "MenuItemColor=%ul\n",dwMenuItemColor);
			fprintf(f, "NullItemColor=%ul\n",dwNullItemColor);

			fprintf(f, "ContMenuPosX=%d\n",iContMenuPosX);
			fprintf(f, "ContMenuPosY=%d\n",iContMenuPosY);
			fprintf(f, "ContSetMenuPosX=%d\n",iContSetMenuPosX);
			fprintf(f, "ContSetMenuPosY=%d\n",iContSetMenuPosY);
			fprintf(f, "CreditsPosX=%d\n",iCreditsPosX);
			fprintf(f, "CreditsPosY=%d\n",iCreditsPosY);
			fprintf(f, "LaunchMenuPosX=%d\n",iLaunchMenuPosX);
			fprintf(f, "LaunchMenuPosY=%d\n",iLaunchMenuPosY);
			fprintf(f, "MainMenuPosX=%d\n",iMainMenuPosX);
			fprintf(f, "MainMenuPosY=%d\n",iMainMenuPosY);
			fprintf(f, "SettingsMenuPosX=%d\n",iSettingsMenuPosX);
			fprintf(f, "SettingsMenuPosY=%d\n",iSettingsMenuPosY);
			fprintf(f, "SkinMenuPosX=%d\n",iSkinMenuPosX);
			fprintf(f, "SkinMenuPosY=%d\n",iSkinMenuPosY);
			fprintf(f, "VidSetMenuPosX=%d\n",iVidSetMenuPosX);
			fprintf(f, "VidSetMenuPosY=%d\n",iVidSetMenuPosY);

			fprintf(f, "IGMContPosX=%d\n",iIGMContPosX);
			fprintf(f, "IGMContPosY=%d\n",iIGMContPosY);
			fprintf(f, "IGMContSetPosX=%d\n",iIGMContSetPosX);
			fprintf(f, "IGMContSetPosY=%d\n",iIGMContSetPosY);
			fprintf(f, "IGMLoadPosX=%d\n",iIGMLoadPosX);
			fprintf(f, "IGMLoadPosY=%d\n",iIGMLoadPosY);
			fprintf(f, "IGMSavePosX=%d\n",iIGMSavePosX);
			fprintf(f, "IGMSavePosY=%d\n",iIGMSavePosY);
			fprintf(f, "IGMMainPosX=%d\n",iIGMMainPosX);
			fprintf(f, "IGMMainPosY=%d\n",iIGMMainPosY);
			fprintf(f, "IGMVidSetPosX=%d\n",iIGMVidSetPosX);
			fprintf(f, "IGMVidSetPosY=%d\n",iIGMVidSetPosY);

			fprintf(f, "InfoPosX=%d\n",iInfoPosX);
			fprintf(f, "InfoPosY=%d\n",iInfoPosY);
			fprintf(f, "BoxPosX=%d\n",iBoxPosX);
			fprintf(f, "BoxPosY=%d\n",iBoxPosY);
			fprintf(f, "TitleX=%d\n",iTitleX);
			fprintf(f, "TitleY=%d\n",iTitleY);

			fprintf(f, "IGMTitleX=%d\n",iIGMTitleX);
			fprintf(f, "IGMTitleY=%d\n",iIGMTitleY);
			fprintf(f, "PanelX=%d\n",iPanelX);
			fprintf(f, "PanelY=%d\n",iPanelY);
			fprintf(f, "PanelNH=%d\n",iPanelNH);
			fprintf(f, "PanelNW=%d\n",iPanelNW);

			fprintf(f, "RomListPosX=%d\n",iRomListPosX);
			fprintf(f, "RomListPosY=%d\n",iRomListPosY);
			fprintf(f, "RomListSize=%d\n",GAMESEL_MaxWindowList);
			fprintf(f, "RomListCharacterLimit=%d\n",RomListTrunc);
			fprintf(f, "MenuCharacterLimit=%d\n",MenuTrunc);



		}
	fclose(f);
	
	

	return;

}

void loadinis() {
	//Check for CD/DVD
	if(XGetDiskSectorSize("D:\\") == 2048){
		onhd = FALSE;
	}else{
		onhd = TRUE;
	}
	ConfigAppLoad();
	ConfigAppSave();
	ConfigAppLoadTemp();
	ConfigAppLoad2();
}

int loaddwPJ64PagingMem(){ return dwPJ64PagingMem;}
int loaddwPJ64DynaMem(){ return dwPJ64DynaMem;}
int loaddw1964PagingMem(){ return dw1964PagingMem;}
int loaddw1964DynaMem(){ return dw1964DynaMem;}

// Ez0n3 - use iAudioPlugin instead to determine if basic audio is used
int loadiAudioPlugin(){ return iAudioPlugin;}

// Ez0n3 - reinstate max video mem
int loaddwMaxVideoMem(){ return dwMaxVideoMem;}


char* LabelCheck(char *s,char *szLabel)
{
	int nLen;
	if (s == NULL) {
		return NULL;
	}
	if (szLabel == NULL) {
		return NULL;
	}
	nLen = strlen(szLabel);
	
	SKIP_WS(s);													// Skip whitespace

	if (strncmp(s, szLabel, nLen)){								// Doesn't match
		return NULL;
	}
	return s + nLen;
}

// Skin

char* LabelCheckNew( char *s, char *szLabel )
{
	char	*returnvalue = NULL;
	int		nLen;

	if ( s == NULL )
		return NULL;

	if ( szLabel == NULL )
		return NULL;

	// Longueur de ce que l'on recherche
	nLen = strlen( szLabel );
	
	// On saute les blancs
	SKIP_WS( s );

	// On ne tiens pas compte de la casse
	if ( _strnicmp( s, szLabel, nLen ) )
		return NULL;

	// On passe � la suite
	s += nLen;

	// On saute les blancs
	SKIP_WS( s );

	// On cherche le "="
	if ( *s != '=' )
		return NULL;

	// On passe � la suite
	s++;

	// On saute les blancs
	SKIP_WS( s );

	// On lis ce qu'il y a entre les guillemets
	if ( QuoteRead( &returnvalue, NULL, s ) ) return NULL;
	// On retourne la valeur
	return returnvalue;
}


int QuoteRead(char** pszQuote, char** pszEnd, char* szSrc)		// Read a quoted string from szSrc and point to the end
{
	static char szQuote[QUOTE_MAX];
	char *s = szSrc;
	int i;
	
	SKIP_WS(s);													// Skip whitespace

	if (*s != '\"')	{											// Didn't start with a quote
		return 1;
	}
	s++;

	// Copy the characters into szQuote
	for (i = 0; i < QUOTE_MAX - 1; i++, s++) {
		if (*s == '\"') {
			s++;
			break;
		}
		if (*s == 0) {											// Stop copying if end of line or end of string
			break;
		}
		szQuote[i] = *s;										// Copy character to szQuote
	}
	szQuote[i] = 0;												// Zero-terminate szQuote

	if (pszQuote) {
		*pszQuote = szQuote;
	}
	if (pszEnd)	{
		*pszEnd = s;
	}
	
	return 0;
}