/*
Copyright (C) 2003 Rice1964

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

#ifndef _TYPEDEFS_H_
#define _TYPEDEFS_H_

#define uint16 unsigned __int16
#define uint32 unsigned __int32
#define uint64 unsigned __int64

typedef unsigned char				uint8;

typedef signed char					s8;	
typedef long						s32;


//Fix me, these macro should not be used anymore in DirectX 8
#define RGBA_GETALPHA(rgb)      ((rgb) >> 24)
#define RGBA_GETRED(rgb)        (((rgb) >> 16) & 0xff)
#define RGBA_GETGREEN(rgb)      (((rgb) >> 8) & 0xff)
#define RGBA_GETBLUE(rgb)       ((rgb) & 0xff)

typedef D3DXMATRIX Matrix;
typedef D3DVECTOR Vector3;
typedef void* LPRICETEXTURE ;
typedef D3DLOCKED_RECT LockRectType;

#define COLOR_RGBA D3DCOLOR_RGBA

typedef struct 
{
	DWORD dwRGBA, dwRGBACopy;
	char x,y,z;			// Direction
	BYTE pad;
} N64Light;


typedef struct
{
	unsigned int	dwFormat:3;
	unsigned int	dwSize:2;
	unsigned int	dwWidth:10;
	uint32			dwAddr;
	uint32			bpl;
} SetImgInfo;

typedef struct 
{
	// Set by RDP_SetTile
	unsigned int dwFormat	:3;		// e.g. RGBA, YUV etc
	unsigned int dwSize		:2;		// e.g 4/8/16/32bpp
	unsigned int dwLine		:9;		// Ummm...
	unsigned int dwPalette	:4;		// 0..15 - a palette index?
	DWORD dwTMem;					// Texture memory location

	unsigned int bClampS	:1;
	unsigned int bClampT	:1;
	unsigned int bMirrorS	:1;
	unsigned int bMirrorT	:1;

	unsigned int dwMaskS	:4;
	unsigned int dwMaskT	:4;
	unsigned int dwShiftS	:4;
	unsigned int dwShiftT	:4;

	// Set by RDP_SetTileSize
	int sl;		// Upper left S		- 8:3
	int tl;		// Upper Left T		- 8:3
	int sh;		// Lower Right S
	int th;		// Lower Right T

	int   hilite_sl;
	int	  hilite_tl;
	int   hilite_sh;
	int	  hilite_th;

	float fsl;		// Upper left S		- 8:3
	float ftl;		// Upper Left T		- 8:3
	float fsh;		// Lower Right S
	float fth;		// Lower Right T

	float   fhilite_sl;
	float	fhilite_tl;
	float   fhilite_sh;
	float	fhilite_th;

	DWORD dwDXT;

	DWORD dwPitch;

	DWORD dwWidth;
	DWORD dwHeight;

	float fShiftScaleS;
	float fShiftScaleT;

	DWORD   lastTileCmd;
	bool  bSizeIsValid;
} Tile;


typedef struct
{
	float u;
	float v;
} TexCord;

typedef struct VECTOR2
{
	float x;
	float y;
	VECTOR2( float newx, float newy )	{x=newx; y=newy;}
	VECTOR2()	{x=0; y=0;}
} VECTOR2;

typedef struct
{
	short x;
	short y;
} IVector2;

typedef struct 
{
	short x;
	short y;
	short z;
} IVector3;

#define RICE_FVF_TLITVERTEX (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX2 ) 
typedef struct {
	float x,y,z;
	float rhw;
	union {
		D3DCOLOR  dcDiffuse;
		struct {
			BYTE b;
			BYTE g;
			BYTE r;
			BYTE a;
		};
	};
	D3DCOLOR  dcSpecular;
	TexCord tcord[2];
} TLITVERTEX, *LPTLITVERTEX;

#define RICE_FVF_UTLITVERTEX (D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX2 ) 
typedef struct {
	float x,y,z;
	union {
		D3DCOLOR  dcDiffuse;
		struct {
			BYTE b;
			BYTE g;
			BYTE r;
			BYTE a;
		};
	};
	D3DCOLOR  dcSpecular;
	TexCord tcord[2];
} UTLITVERTEX, *LPUTLITVERTEX;


#define RICE_FVF_LITVERTEX (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR ) 
typedef struct {
	float x,y,z;
	float rhw;
	union {
		D3DCOLOR  dcDiffuse;
		struct {
			BYTE b;
			BYTE g;
			BYTE r;
			BYTE a;
		};
	};
	D3DCOLOR  dcSpecular;
} LITVERTEX, *LPLITVERTEX;



typedef struct {
	float	x,y,z;
	float	rhw;
	D3DCOLOR dcDiffuse;
} FILLRECTVERTEX, *LPFILLRECTVERTEX;


typedef struct
{
	union {
		struct {
			float x;
			float y;
			float z;
			float dummy;
		};
#if _MSC_VER > 1200
		__m64 dir64[2];
#endif
	};

	union {
		struct {
			BYTE r;
			BYTE g;
			BYTE b;
			BYTE a;
		};
		DWORD col;
	};

	union {
		struct {
			float fr;
			float fg;
			float fb;
			float fa;
		};
		float fcolors[4];
#if _MSC_VER > 1200
		__m64 fcolor64[2];
#endif
	};
} Light;



typedef struct
{
	DWORD dwColorOp;
	DWORD dwColorArg0;
	DWORD dwColorArg1;
	DWORD dwColorArg2;
	DWORD dwAlphaOp;
	DWORD dwAlphaArg0;
	DWORD dwAlphaArg1;
	DWORD dwAlphaArg2;
	DWORD dwResultArg;
	
	DWORD dwMinFilter;
	DWORD dwMagFilter;

	DWORD dwAddressUMode;
	DWORD dwAddressVMode;
	DWORD dwAddressW;

	DWORD dwTexCoordIndex;
	LPDIRECT3DBASETEXTURE8 pTexture; 
} D3DCombinerStage;

typedef struct
{
	char na;
	char nz;	// b
	char ny;	//g
	char nx;	//r
}NormalStruct;

typedef struct
{
	short y;
	short x;
	
	short flag;
	short z;
	
	short tv;
	short tu;
	
	union {
		struct _rgba {
			BYTE a;
			BYTE b;
			BYTE g;
			BYTE r;
		} rgba;
		NormalStruct norma;
	};
} FiddledVtx;

typedef struct
{
	short y;
	short x;
	
	uint8 a;
	uint8 b;
	short z;
	
	uint8 g;
	uint8 r;
	
} FiddledVtxDKR;

typedef struct 
{
	short y;
	short	x;
	uint16	cidx;
	short z;
	short t;
	short s;
} N64VtxPD;

class CTexture;
class COGLTexture;
class CDirectXTexture;
struct TextureEntry;

typedef struct {
	LPRICETEXTURE m_lpsTexturePtr;
	union {
		CTexture *			m_pCTexture;
		CDirectXTexture *	m_pCDirectXTexture;
		COGLTexture *		m_pCOGLTexture;
	};
	
	DWORD m_dwTileWidth;
	DWORD m_dwTileHeight;
	float m_fTexWidth;
	float m_fTexHeight;		// Float to avoid converts when processing verts
	TextureEntry *pTextureEntry;
} RenderTexture;


typedef struct
{
	unsigned __int32	dwFormat;
	unsigned __int32	dwSize;
	unsigned __int32	dwWidth;
	unsigned __int32	dwAddr;

	unsigned __int32	dwLastWidth;
	unsigned __int32	dwLastHeight;

	unsigned __int32	dwHeight;
	unsigned __int32	dwMemSize;

	bool				bCopied;
	unsigned __int32	dwCopiedAtFrame;

	unsigned __int32	dwCRC;
	unsigned __int32	lastUsedFrame;
	unsigned __int32	bUsedByVIAtFrame;
} RecentCIInfo;

typedef struct
{
	DWORD		addr;
	DWORD		FrameCount;
} RecentViOriginInfo;

typedef enum {
	SHADE_DISABLED,
	SHADE_FLAT,
	SHADE_SMOOTH,
} RenderShadeMode;

typedef enum {
	TEXTURE_UV_FLAG_WRAP,
	TEXTURE_UV_FLAG_MIRROR,
	TEXTURE_UV_FLAG_CLAMP,
} TextureUVFlag;

typedef struct
{
	TextureUVFlag	N64flag;
	DWORD			realFlag;
} UVFlagMap;


//freakdave
typedef enum {
	FILTER_NONE,
	FILTER_POINT,
	FILTER_LINEAR,
	FILTER_ANISOTROPIC,
	FILTER_FLATCUBIC,
	FILTER_GAUSSIANCUBIC,
} TextureFilter;

typedef struct 
{
	TextureFilter	N64filter;
	DWORD					realFilter;
} TextureFilterMap;

typedef struct {
	char	*description;
	int		number;
	DWORD	setting;
} BufferSettingInfo;

typedef struct {
	char	*description;
	DWORD	setting;
} SettingInfo;

typedef union {
	BYTE	g_Tmem8bit[0x1000];
	__int16	g_Tmem16bit[0x800];
	uint32	g_Tmem32bit[0x300];
	uint64	g_Tmem64bit[0x200];
} TmemType;

#endif