#include <stdlib.h>
#include <io.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string.h>
#include <stdio.h>

// Ez0n3 - plugins
#include "Plugins.h"


#define QUOTE_MAX (128)						// Maximum length of "quoted strings"
int QuoteRead(char** pszQuote, char** pszEnd, char* szSrc);					// Read a quoted string from szSrc and point to the end

char *LabelCheck(char* s, char* szLabel);
// Skin
char *LabelCheckNew( char* s, char* szLabel );
//
#define SKIP_WS(s) for (;;) { if (*s != ' ' && *s != '\t') break; s++; }	// skip whitespace
int ConfigAppLoad();				// char* lpszName = NULL
int ConfigAppSave();
int ConfigAppLoad2();				
int ConfigAppSave2();
int ConfigAppLoadTemp();				
int ConfigAppSaveTemp();
//freakdave
extern int dw1964DynaMem;
extern int dw1964PagingMem;
extern int dwPJ64DynaMem;
extern int dwPJ64PagingMem;
extern bool bUseLLERSP; // Ez0n3 - use iAudioPlugin instead, but leave this in case it's set in ini

// Ez0n3 - use iAudioPlugin instead to determine if basic audio is used
extern int iAudioPlugin;

// Ez0n3 - reinstate max video mem until freakdave finishes this
//extern int dwFreeMem;
extern int dwMaxVideoMem;



extern int FlickerFilter;
extern bool SoftDisplayFilter;
extern int TextureMode;
extern int VertexMode;
extern float XBINPUT_DEADZONE;
extern float XBINPUT_SUMMAND;
extern float XBOX_CONTROLLER_DEAD_ZONE;
extern int Sensitivity;
extern int DefaultPak;
extern bool EnableController1;
extern bool EnableController2;
extern bool EnableController3;
extern bool EnableController4;
extern int preferedemu;
extern int videoplugin;