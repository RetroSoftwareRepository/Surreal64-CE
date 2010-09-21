#include "config.h"
#include <xbapp.h>
#include <xbresource.h>

#ifdef DEBUG
bool showdebug = true;
#else
bool showdebug = false;
#endif

extern "C" void loadinis();
extern "C" int loaddwPJ64PagingMem();
extern "C" int loaddwPJ64DynaMem();
extern "C" int loaddw1964PagingMem();
extern "C" int loaddw1964DynaMem();

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
unsigned int Sensitivity = 5;
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

int DefaultPak = MemPak;

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

DefaultPak = MemPak;
EnableController1 = true;
EnableController2 = true;
EnableController3 = true;
EnableController4 = true;
XBOX_CONTROLLER_DEAD_ZONE = 8600;
Deadzone = 26;
Sensitivity = 5;
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

	// On passe à la suite
	s += nLen;

	// On saute les blancs
	SKIP_WS( s );

	// On cherche le "="
	if ( *s != '=' )
		return NULL;

	// On passe à la suite
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