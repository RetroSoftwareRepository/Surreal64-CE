#include "Panel.h"
#include "RomList.h"
#include <xbfont.h>
#include "musicmanager.h"

#define VERSION L"Surreal64 XXX CE B5.2"
extern CMusicManager  music;
extern int actualrom;
CPanel m_BgPanel;
CPanel m_BoxPanel;
CPanel m_PgPanel;
LPDIRECT3DTEXTURE8 bgTexture;
LPDIRECT3DTEXTURE8 PgTexture;
LPDIRECT3DTEXTURE8 BoxTexture;
extern int romcounter;
extern CXBFont	m_Font;					// Font	for	text display
extern LPDIRECT3DDEVICE8 g_pd3dDevice;
int nombre=0;
int defilement=540;
int endcredits=0;
extern bool onhd;
extern char skinname[32];

void InitLogo(void)
{
	char bgpath[256];
	sprintf(bgpath,"D:\\Skins\\%s\\bg.jpg",skinname);
	D3DXCreateTextureFromFileEx( g_pd3dDevice, bgpath,D3DX_DEFAULT, D3DX_DEFAULT,	1, 0, D3DFMT_LIN_A8R8G8B8 ,	D3DPOOL_MANAGED,D3DX_FILTER_NONE , D3DX_FILTER_NONE, 0x00000000,NULL, NULL,&bgTexture);		
	m_BgPanel.Create(g_pd3dDevice,	bgTexture, true);
}


// Ez0n3 - check if file exists
extern bool FileExists(char *szFilename);

void DrawLogo(bool Menu)
{
	DirectSoundDoWork();
	music.Process();

	m_BgPanel.Render(15,15);

	m_Font.Begin();
	// Title
	
	// Ez0n3 - update the version: unofficial "community edition"
	//m_Font.DrawText(320, 35, 0xFFFF7F7f, L"Surreal64 XXX B5", XBFONT_CENTER_X);
	//freakdave - Version is now defined
	m_Font.DrawText(320, 35, 0xFFFF7F7f, VERSION, XBFONT_CENTER_X);

	m_Font.End();

#ifdef DEBUG
	MEMORYSTATUS memStat;
	WCHAR szMemStatus[128];

	GlobalMemoryStatus(&memStat);
	swprintf(szMemStatus,L"%d Mb Free",(memStat.dwAvailPhys /1024 /1024));

	m_Font.Begin();

	m_Font.DrawText(60, 35, 0xFFFF7F7f, szMemStatus, XBFONT_LEFT);

	m_Font.End();
#endif

	if (Menu){
	m_Font.Begin();
	//freakdave - yeah, gimme some Action ^^
	m_Font.DrawText(480, 300, 0xFFEEEEEE, L"\400 Select / Next", XBFONT_LEFT);
	m_Font.DrawText(480, 325, 0xFFEEEEEE, L"\402 Select / Prev", XBFONT_LEFT);
	m_Font.DrawText(480, 350, 0xFFEEEEEE, L"\401 Back", XBFONT_LEFT);

	m_Font.End();}

	if (!Menu && (romcounter !=0)){
	int y = 40;
	WCHAR	   m_currentname[120];	// Game	name written to	a string
	
	

	char imagename[99];
	Rom *rom = g_romList.GetRomAt(actualrom);
	sprintf(imagename,"D:\\Media\\%s",rom->m_szBoxArtFilename.c_str());
	
	// Ez0n3 - make sure that there's actually an image - otherwise -> crash! (ie: boxart dir is empty = crash)
	if ( FileExists( imagename ) ) {
	
		m_BoxPanel.Destroy();
		D3DXCreateTextureFromFileEx( g_pd3dDevice, imagename,D3DX_DEFAULT, D3DX_DEFAULT,	1, 0, D3DFMT_LIN_A8R8G8B8 ,	D3DPOOL_MANAGED,D3DX_FILTER_NONE , D3DX_FILTER_NONE, 0x00000000,NULL, NULL,&BoxTexture);		
		m_BoxPanel.Create(g_pd3dDevice,	BoxTexture, true);
		m_BoxPanel.Render(450,50);
		
	}
	
	

    m_Font.Begin();

// rom size
		int romsize = (rom->m_romSize / 0x100000 * 8); 
		swprintf( m_currentname, L"Rom Size : %d Mbits", romsize);
		m_Font.DrawText( 60, 340, 0xFFEEEEEE, m_currentname, XBFONT_TRUNCATED,	530);
// country

		
		int country = rom->m_byCountry;
		char country2[10];
		
		// Ez0n3 - update the countries
		/*
		//calculate country (apparently 69=US, 4 and 74 = JAP, 80=PAL, no sure about that)
		switch (country) {
				case 4 : 
		sprintf( country2, "JAP" );
		break;
			case 69 : 
		sprintf( country2, "US" );
		break;
			case 74 : 
		sprintf( country2, "JAP" );
		break;
			case 80 :
		sprintf( country2, "EUR" );
		break;
			default :
		sprintf( country2, "Unknown" );
		break;
		}
		*/
		switch (country) {
			//case  : sprintf( country2, "Demo" ); break;
			//case  : sprintf( country2, "Beta" ); break;
			case 74 : sprintf( country2, "Japan" ); break; //0x4A
			case 65 : sprintf( country2, "USA/Japan" ); break; //0x41
			case 68 : sprintf( country2, "Germany" ); break; //0x44
			case 69 : sprintf( country2, "America" ); break; //0x45
			case 70 : sprintf( country2, "France" ); break; //0x46
			case 73 : sprintf( country2, "Italy" ); break; //0x49
			case 83 : sprintf( country2, "Spain" ); break; //0x53
			case 85 : //0x55
			case 89 : sprintf( country2, "Australia" ); break; //0x59
			case 32 : //0x20
			case 33 : //0x21 
			case 56 : //0x38 
			case 80 : //0x50 
			case 112 : //0x70 
			case 88 : sprintf( country2, "Europe" ); break; //0x58
			default : sprintf( country2, "Unknown" ); break;
		}

		
		swprintf( m_currentname, L"Country : %S", country2);
		m_Font.DrawText( 60, 360, 0xFFEEEEEE, m_currentname, XBFONT_TRUNCATED,	530);
// zip name
		char zipname[120];
		sprintf(zipname,rom->GetFileName().c_str());
		/* remove rom path */
		int counterbackslash;
		// find last backslash
		for (int i=0;i<120;i++){
			if (zipname[i]=='\\')  counterbackslash=i;
			if (zipname[i] == '\0') break;
		}
		// remove it
		for (int i=0;i<120;i++){
			for (int j= counterbackslash+1;j<120;j++){
				zipname[i]=zipname[j];
				i++;
			if (zipname[j] == '\0') break;
			}
			zipname[i+1] = '\0';
			break;
		}
		swprintf( m_currentname, L"Rom Name : %S", zipname );
		m_Font.DrawText( 60, 380, 0xFFEEEEEE, m_currentname, XBFONT_TRUNCATED,	530);

// rom counter
		swprintf( m_currentname, L"[%d Roms]", romcounter );
		m_Font.DrawText( 60, 400, 0xFFEEEEEE, m_currentname, XBFONT_TRUNCATED,	530);
		m_Font.DrawText( 450, 430, 0xFFEEEEEE, L"\403 Refresh");
		m_Font.DrawText( 260, 430, 0xFFEEEEEE, L"\402 Surreal Setup");
		m_Font.DrawText( 70, 430, 0xFFEEEEEE, L"\406 - \407 Fast Scroll");


        m_Font.End();
		}


	
}

void DrawCredits()
{

DirectSoundDoWork();
music.Process();//BGMProgress();

m_BgPanel.Render(15,15);
WCHAR	   m_currentname[99];	// Game	name written to	a string
char name[99];
FILE *fp;
if (defilement <= endcredits) defilement = 640;
int x=defilement;
if (onhd) fp=fopen("D:\\Credits.txt","r");
else fp=fopen("T:\\Credits.txt","r");
m_Font.Begin();

while(!feof(fp)){
fseek(fp,nombre,SEEK_SET);
if ((fgets(name,99,fp))!= NULL)
nombre=nombre+(strlen(name));
else 
{
sprintf(name," ");
nombre=0;
}
swprintf (m_currentname,L"%S",name);
m_Font.DrawText(320, x, 0xFFFFFFFF, m_currentname, XBFONT_CENTER_X);
x=x+15;
}

m_Font.DrawText(60, 405, 0xFFFFFFFF,L"\400 Fast Scroll" , XBFONT_LEFT);
m_Font.DrawText(60, 430, 0xFFFFFFFF,L"\401 Back" , XBFONT_LEFT);
m_Font.End();
fclose(fp);
}

void CalculateEndCredits()
{
char name[99];
FILE *fp;
if (onhd) fp=fopen("D:\\Credits.txt","r");
else fp=fopen("T:\\Credits.txt","r");
int x=0;

while(!feof(fp))
{
fseek(fp,nombre,SEEK_SET);
if ((fgets(name,99,fp))!= NULL)
{
nombre=nombre+(strlen(name));
}
x=x+15;
}

endcredits = -1*(x);
fclose(fp);
}

void CreateProgress()
{
	char progressname[256];
	sprintf(progressname,"D:\\Skins\\%s\\progress.png",skinname);
D3DXCreateTextureFromFileEx( g_pd3dDevice, progressname,D3DX_DEFAULT, D3DX_DEFAULT,	1, 0, D3DFMT_LIN_A8R8G8B8 ,	D3DPOOL_MANAGED,D3DX_FILTER_NONE , D3DX_FILTER_NONE, 0x00000000,NULL, NULL,&PgTexture);		
m_PgPanel.Create(g_pd3dDevice,	PgTexture, true);
}

void RenderProgress(int progress)
{
g_d3d.BeginRender();

m_BgPanel.Render(15,15);
m_Font.Begin();
m_Font.DrawText(320, 200, 0xFFFFFFFF,L"Loading Rom" , XBFONT_CENTER_X);
m_Font.End();
for (int i=0;i<progress;i++){
	m_PgPanel.Render(95+(i*4),230);}

g_d3d.EndRender();
}

extern bool compatible[3];
// 0=1964,1=pj64,2=ultrahle 
// not working apparently , ini not done
void display_compatible()
{
	char compatib[99];
	WCHAR compat[99];
	if (compatible[0]) {
		if (compatible[1]){

			if (compatible[2]){
				sprintf(compatib,"Compatible With all emulators"); }
			else {
				sprintf(compatib,"Compatible With 1964 and Project64"); }
		}
		else {

			if (compatible[2]){
				sprintf(compatib,"Compatible With 1964 and UltraHLE"); }
			else {
				sprintf(compatib,"Compatible With 1964"); }
		}
	}
	else{

		if (compatible[1]){

			if (compatible[2]){
				sprintf(compatib,"Compatible With Project64 and UltraHLE"); }
			else {
				sprintf(compatib,"Compatible With Project64"); }
		}
		else {

			if (compatible[2]){
				sprintf(compatib,"Compatible With UltraHLE"); }
			else {
				sprintf(compatib,"No information"); }
		}
	}
swprintf(compat,L"%S",compatib);
m_Font.Begin();
m_Font.DrawText(320, 100, 0xFFFFFFFF,compat , XBFONT_CENTER_X);
m_Font.End(); 

}