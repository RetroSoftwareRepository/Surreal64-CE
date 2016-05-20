/*
Copyright (C) 2003-2009 Rice1964

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

#include "stdafx.h"
#include "float.h"
#include "VertexShaderConstantDef.h"

extern FiddledVtx * g_pVtxBase;

#define ENABLE_CLIP_TRI
#define X_CLIP_MAX	0x1
#define X_CLIP_MIN	0x2
#define Y_CLIP_MAX	0x4
#define Y_CLIP_MIN	0x8
#define Z_CLIP_MAX	0x10
#define Z_CLIP_MIN	0x20

//#define ENABLE_CLIP_TRI
#ifdef ENABLE_CLIP_TRI

inline void RSP_Vtx_Clipping(int i)
{
	g_clipFlag[i] = 0;
	g_clipFlag2[i] = 0;
	if( g_vecProjected[i].w > 0 )
	{
		/*
		if( gRSP.bRejectVtx )
		{
			if( g_vecProjected[i].x > 1 )	
			{
				g_clipFlag2[i] |= X_CLIP_MAX;
				if( g_vecProjected[i].x > gRSP.real_clip_ratio_posx )	
					g_clipFlag[i] |= X_CLIP_MAX;
			}

			if( g_vecProjected[i].x < -1 )	
			{
				g_clipFlag2[i] |= X_CLIP_MIN;
				if( g_vecProjected[i].x < gRSP.real_clip_ratio_negx )	
					g_clipFlag[i] |= X_CLIP_MIN;
			}

			if( g_vecProjected[i].y > 1 )	
			{
				g_clipFlag2[i] |= Y_CLIP_MAX;
				if( g_vecProjected[i].y > gRSP.real_clip_ratio_posy )	
					g_clipFlag[i] |= Y_CLIP_MAX;
			}

			if( g_vecProjected[i].y < -1 )	
			{
				g_clipFlag2[i] |= Y_CLIP_MIN;
				if( g_vecProjected[i].y < gRSP.real_clip_ratio_negy )	
					g_clipFlag[i] |= Y_CLIP_MIN;
			}

			//if( g_vecProjected[i].z > 1.0f )	
			//{
			//	g_clipFlag2[i] |= Z_CLIP_MAX;
			//	g_clipFlag[i] |= Z_CLIP_MAX;
			//}

			//if( gRSP.bNearClip && g_vecProjected[i].z < -1.0f )	
			//{
			//	g_clipFlag2[i] |= Z_CLIP_MIN;
			//	g_clipFlag[i] |= Z_CLIP_MIN;
			//}
		}
		else
		*/
		{
			if( g_vecProjected[i].x > 1 )	g_clipFlag2[i] |= X_CLIP_MAX;
			if( g_vecProjected[i].x < -1 )	g_clipFlag2[i] |= X_CLIP_MIN;
			if( g_vecProjected[i].y > 1 )	g_clipFlag2[i] |= Y_CLIP_MAX;
			if( g_vecProjected[i].y < -1 )	g_clipFlag2[i] |= Y_CLIP_MIN;
			//if( g_vecProjected[i].z > 1.0f )	g_clipFlag2[i] |= Z_CLIP_MAX;
			//if( gRSP.bNearClip && g_vecProjected[i].z < -1.0f )	g_clipFlag2[i] |= Z_CLIP_MIN;
		}

	}
}

#else
inline void RSP_Vtx_Clipping(int i) {}
#endif

/*
 *	Global variables
 */
RSP_Options gRSP;
RDP_Options gRDP;

#if _MSC_VER > 1200
__declspec(align(16)) static D3DXVECTOR4 g_normal;
#else
static D3DXVECTOR4 g_normal;
#endif
static int norms[3];

#if _MSC_VER > 1200
__declspec(align(16)) D3DXVECTOR4	g_vtxNonTransformed[MAX_VERTS];
__declspec(align(16)) D3DXVECTOR4	g_vecProjected[MAX_VERTS];
__declspec(align(16)) D3DXVECTOR4	g_vtxTransformed[MAX_VERTS];
#else
D3DXVECTOR4	g_vtxNonTransformed[MAX_VERTS];
D3DXVECTOR4	g_vecProjected[MAX_VERTS];
D3DXVECTOR4	g_vtxTransformed[MAX_VERTS];
#endif
float		g_vtxProjected5[1000][5];
float		g_vtxProjected5Clipped[2000][5];
//uint32		g_dwVtxFlags[MAX_VERTS];			// Z_POS Z_NEG etc
VECTOR2		g_fVtxTxtCoords[MAX_VERTS];
uint32		g_dwVtxDifColor[MAX_VERTS];
uint32		g_clipFlag[MAX_VERTS];
uint32		g_clipFlag2[MAX_VERTS];
RenderTexture g_textures[MAX_TEXTURES];
float		g_fFogCoord[MAX_VERTS];

EXTERNAL_VERTEX	g_vtxForExternal[MAX_VERTS];

TLITVERTEX			g_vtxBuffer[1000];
TLITVERTEX			g_clippedVtxBuffer[2000];
uint8				g_oglVtxColors[1000][4];
int					g_clippedVtxCount=0;
TLITVERTEX			g_texRectTVtx[4];
unsigned int		g_vtxIndex[1000];
unsigned int		g_minIndex, g_maxIndex;

float				gRSPfFogMin;
float				gRSPfFogMax;
float				gRSPfFogDivider;

uint32			gRSPnumLights;
Light	gRSPlights[16];
LightOld	gRSPlightsOld[16];
#if _MSC_VER > 1200
__declspec(align(16)) Matrix	gRSPworldProjectTransported;
__declspec(align(16)) Matrix	gRSPworldProject;
__declspec(align(16)) Matrix	gRSPmodelViewTop;
__declspec(align(16)) Matrix	gRSPmodelViewTopTranspose;
__declspec(align(16)) Matrix	dkrMatrixTransposed;
#else
Matrix	gRSPworldProjectTransported;
Matrix	gRSPworldProject;
Matrix	gRSPmodelViewTop;
Matrix	gRSPmodelViewTopTranspose;
Matrix	dkrMatrixTransposed;
#endif
N64LightOld		gRSPn64lightsOld[16];


void (*ProcessVertexData)(uint32 dwAddr, uint32 dwV0, uint32 dwNum)=NULL;

/*
 *	
 */


/*n.x = (g_normal.x * matWorld.m00) + (g_normal.y * matWorld.m10) + (g_normal.z * matWorld.m20);
n.y = (g_normal.x * matWorld.m01) + (g_normal.y * matWorld.m11) + (g_normal.z * matWorld.m21);
n.z = (g_normal.x * matWorld.m02) + (g_normal.y * matWorld.m12) + (g_normal.z * matWorld.m22);*/

// Multiply (x,y,z,0) by matrix m, then normalize
#define Vec3TransformNormal(vec, m) __asm					\
{														\
	__asm fld		dword ptr [vec + 0]							\
	__asm fmul	dword ptr [m + 0]			/* x m00*/		\
	__asm fld		dword ptr [vec + 0]							\
	__asm fmul	dword ptr [m + 4] 	/* x m01  x m00*/			\
	__asm fld		dword ptr [vec + 0]								\
	__asm fmul	dword ptr [m + 8] 	/* x m02  x m01  x m00*/	\
	\
	__asm fld		dword ptr [vec + 4]								\
	__asm fmul	dword ptr [m + 16] 	/* y m10  x m02  x m01  x m00*/	\
	__asm fld		dword ptr [vec + 4]									\
	__asm fmul	dword ptr [m + 20] 	/* y m11  y m10  x m02  x m01  x m00*/		\
	__asm fld		dword ptr [vec + 4]												\
	__asm fmul	dword ptr [m + 24]	/* y m12  y m11  y m10  x m02  x m01  x m00*/	\
	\
	__asm fxch	st(2)				/* y m10  y m11  y m12  x m02  x m01  x m00*/			\
	__asm faddp	st(5), st(0)		/* y m11  y m12  x m02  x m01  (x m00 + y m10)*/		\
	__asm faddp	st(3), st(0)		/* y m12  x m02  (x m01 + ym11)  (x m00 + y m10)*/	\
	__asm faddp	st(1), st(0)		/* (x m02 + y m12) (x m01 + ym11)  (x m00 + y m10)*/	\
	\
	__asm fld		dword ptr [vec + 8]														\
	__asm fmul	dword ptr [m + 32] /* z m20  (x m02 + y m12) (x m01 + ym11)  (x m00 + y m10)*/	\
	__asm fld		dword ptr [vec + 8]																\
	__asm fmul	dword ptr [m + 36] /* z m21  z m20  (x m02 + y m12) (x m01 + ym11)  (x m00 + y m10)*/				\
	__asm fld		dword ptr [vec + 8]																				\
	__asm fmul	dword ptr [m + 40] /* z m22  z m21  z m20  (x m02 + y m12) (x m01 + ym11)  (x m00 + y m10)*/		\
	\
	__asm fxch	st(2)				/* z m20  z m21  z m22  (x m02 + y m12) (x m01 + ym11)  (x m00 + y m10)*/		\
	__asm faddp	st(5), st(0)		/* z m21  z m22  (x m02 + y m12) (x m01 + ym11)  (x m00 + y m10 + z m20)*/	\
	__asm faddp	st(3), st(0)		/* z m22  (x m02 + y m12) (x m01 + ym11 + z m21)  (x m00 + y m10 + z m20)*/	\
	__asm faddp	st(1), st(0)		/* (x m02 + y m12 + z m 22) (x m01 + ym11 + z m21)  (x m00 + y m10 + z m20)*/	\
	\
	__asm fxch	st(2)				/* (x m00 + y m10 + z m20) (x m01 + ym11 + z m21) (x m02 + y m12 + z m 22) */	\
	\
	__asm fld1                      /* 1 x y z */ \
	__asm fld   st(1)				/* x 1 x y z */	\
	__asm fmul  st(0),st(0)			/* xx 1 x y z */  \
	__asm fld   st(3)               /* y xx 1 x y z */ \
	__asm fmul  st(0),st(0)			/* yy xx 1 x y z */ \
	__asm fld   st(5)				/* z yy xx 1 x y z */ \
	__asm fmul  st(0),st(0)			/* zz yy xx 1 x y z */ \
	\
	__asm fxch  st(2)				/* xx yy zz 1 x y z */ \
	\
	__asm faddp st(1),st(0)			/* (xx+yy) zz 1 x y z */ \
	__asm faddp st(1),st(0)			/* (xx+yy+zz) 1 x y z */ \
	\
	__asm ftst						/* Compare ST to 0	*/				\
	__asm fstsw	ax					/* Store FPU status word in a	*/	\
	__asm sahf						/* Transfer ax to flags register */	\
	__asm jz		l2				/* Skip if length is zero	*/		\
	\
	__asm fsqrt						/* l 1 x y z */ \
	\
	__asm fdivp st(1),st(0)			/* (1/l) x y z */ \
	\
	__asm fmul  st(3),st(0)		    /* f x y fz */										\
	__asm fmul  st(2),st(0)			/* f x fy fz */										\
	__asm fmulp st(1),st(0)			/* fx fy fz */										\
	\
	__asm fstp	dword ptr [vec + 0]	/* fy fz*/							\
	__asm fstp	dword ptr [vec + 4]	/* fz	*/			\
	__asm fstp	dword ptr [vec + 8]	/* done	*/			\
	__asm jmp   l3	\
__asm l2:	\
	__asm mov dword ptr [vec + 0], 0	\
	__asm mov dword ptr [vec + 4], 0	\
	__asm mov dword ptr [vec + 8], 0	\
__asm l3:	\
}		\


// Multiply (x,y,z,1) by matrix m and project onto w = 1
#define Vec3TransformCoord(vecout, vec, m) __asm			\
{														\
	__asm fld	dword ptr [vec + 0]							\
	__asm fmul	dword ptr [m + 0]			/* x m00*/		\
	__asm fld	dword ptr [vec + 0]							\
	__asm fmul	dword ptr [m + 4] 	/* x m01  x m00*/			\
	__asm fld	dword ptr [vec + 0]								\
	__asm fmul	dword ptr [m + 8] 	/* x m02  x m01  x m00*/	\
	\
	__asm fld	dword ptr [vec + 4]								\
	__asm fmul	dword ptr [m + 16] 	/* y m10  x m02  x m01  x m00*/	\
	__asm fld	dword ptr [vec + 4]									\
	__asm fmul	dword ptr [m + 20] 	/* y m11  y m10  x m02  x m01  x m00*/		\
	__asm fld	dword ptr [vec + 4]												\
	__asm fmul	dword ptr [m + 24]	/* y m12  y m11  y m10  x m02  x m01  x m00*/	\
	\
	__asm fxch	st(2)				/* y m10  y m11  y m12  x m02  x m01  x m00*/			\
	__asm faddp	st(5), st(0)		/* y m11  y m12  x m02  x m01  (x m00 + y m10)*/		\
	__asm faddp	st(3), st(0)		/* y m12  x m02  (x m01 + ym11)  (x m00 + y m10)*/	\
	__asm faddp	st(1), st(0)		/* (x m02 + y m12) (x m01 + ym11)  (x m00 + y m10)*/	\
	\
	__asm fld	dword ptr [vec + 8]														\
	__asm fmul	dword ptr [m + 32] /* z m20  (x m02 + y m12) (x m01 + ym11)  (x m00 + y m10)*/	\
	__asm fld	dword ptr [vec + 8]																\
	__asm fmul	dword ptr [m + 36] /* z m21  z m20  (x m02 + y m12) (x m01 + ym11)  (x m00 + y m10)*/				\
	__asm fld	dword ptr [vec + 8]																				\
	__asm fmul	dword ptr [m + 40] /* z m22  z m21  z m20  (x m02 + y m12) (x m01 + ym11)  (x m00 + y m10)*/		\
	\
	__asm fxch	st(2)				/* z m20  z m21  z m22  (x m02 + y m12) (x m01 + ym11)  (x m00 + y m10)*/		\
	__asm faddp	st(5), st(0)		/* z m21  z m22  (x m02 + y m12) (x m01 + ym11)  (x m00 + y m10 + z m20)*/	\
	__asm faddp	st(3), st(0)		/* z m22  (x m02 + y m12) (x m01 + ym11 + z m21)  (x m00 + y m10 + z m20)*/	\
	__asm faddp	st(1), st(0)		/* (x m02 + y m12 + z m 22) (x m01 + ym11 + z m21)  (x m00 + y m10 + z m20)*/	\
	\
	__asm fxch	st(2)				/* (x m00 + y m10 + z m20) (x m01 + ym11 + z m21) (x m02 + y m12 + z m 22) */	\
	__asm fadd	dword ptr [m + 48]	/* (xm00+ym10+zm20+m30)	 (xm01+ym11+zm21) (xm02+ym12+zm22) */		\
	__asm fxch	st(1)				/* (xm01+ym11+zm21) (xm00+ym10+zm20+m30)  (xm02+ym12+zm22) */		\
	__asm fadd	dword ptr [m + 52] 	/* (xm01+ym11+zm21+m31) (xm00+ym10+zm20+m30)  (xm02+ym12+zm22) */		\
	__asm fxch	st(2)				/* (xm02+ym12+zm22) (xm00+ym10+zm20+m30)(xm01+ym11+zm21+m31)   */	\
	__asm fadd	dword ptr [m + 56]	/* (xm02+ym12+zm22+m32) (xm00+ym10+zm20+m30)(xm01+ym11+zm21+m31)   */	\
	__asm fxch	st(1)				/* (xm00+ym10+zm20+m30)(xm02+ym12+zm22+m32) (xm01+ym11+zm21+m31)   */	\
	\
	__asm fld1						/* 1 abc */											\
	__asm fld	dword ptr [vec + 0]	/* x 1 abc*/										\
	__asm fmul  dword ptr [m + 12]  /* xm03 1 abc*/										\
	__asm fld	dword ptr [vec + 4]	/* y xm03 1 abc*/									\
	__asm fmul  dword ptr [m + 28]	/* ym13 xm03 1 abc*/								\
	__asm fld	dword ptr [vec + 8]	/* z ym13 xm03 1 abc*/								\
	__asm fmul  dword ptr [m + 44]  /* zm23 ym13 xm03 1 abc*/							\
	\
	__asm fxch	st(2)				/* xm03 ym13 zm23 1 abc*/							\
	__asm faddp st(1), st(0)		/* (xm03 + ym13) zm23 1 abc*/						\
	__asm faddp	st(1), st(0)		/* (xm03 + ym13 + zm23) 1 abc*/						\
	\
	__asm fadd  dword ptr [m + 60]	/* (xm03+ym13+zm23+m33) 1 abc*/						\
	\
	__asm fdivp  st(1), st(0)		/* 1.0 / (xm03+ym13+zm23+m33) abc*/					\
	\
	__asm fmul  st(3),st(0)		    /* f a b fc */										\
	__asm fmul  st(2),st(0)			/* f a fb fc */										\
	__asm fmulp st(1),st(0)			/* fx fz fy */										\
	\
	__asm fstp	dword ptr [vecout + 0]	/* (xm02+ym12+zm22+m32) (xm01+ym11+zm21+m31)*/	\
	__asm fstp	dword ptr [vecout + 8]	/* (xm01+ym11+zm21+m31)	*/						\
	__asm fstp	dword ptr [vecout + 4]	/* done	*/										\
}																						\

__declspec( naked ) void  __fastcall SSEVec3Transform(int i)
{
#if _MSC_VER > 1200
	//SSEVec3Transform(g_vtxTransformed[i], g_vtxNonTransformed[i]);
	__asm
	{
		shl		ecx,4;		// ecx = i

		movaps	xmm1,	DWORD PTR g_vtxNonTransformed [ecx];		// xmm1 as original vector

		movaps	xmm4,	DWORD PTR gRSPworldProjectTransported;			// row1
		movaps	xmm5,	DWORD PTR gRSPworldProjectTransported[0x10];	// row2
		movaps	xmm6,	DWORD PTR gRSPworldProjectTransported[0x20];	// row3
		movaps	xmm7,	DWORD PTR gRSPworldProjectTransported[0x30];	// row4

		mulps	xmm4, xmm1;		// row 1
		mulps	xmm5, xmm1;		// row 2
		mulps	xmm6, xmm1;		// row 3
		mulps	xmm7, xmm1;		// row 4

		movhlps	xmm0, xmm4;		// xmm4 high to xmm0 low
		movlhps	xmm0, xmm5;		// xmm5 low to xmm0 high

		addps	xmm4, xmm0;		// result of add are in xmm4 low
		addps	xmm5, xmm0;		// result of add are in xmm5 high

		shufps	xmm0, xmm4, 0x44;	// move xmm4 low DWORDs to xmm0 high
		shufps	xmm4, xmm5, 0xe4;	// move xmm5 high DWORS to xmm4
		movhlps	xmm5, xmm0;			// xmm4, xmm5 are mirrored

		shufps	xmm4, xmm4, 0x08;	// move xmm4's 3rd uint32 to its 2nd uint32
		shufps	xmm5, xmm5, 0x0d;	// move xmm5's 4th uint32 to its 2nd uint32, 
									// and move its 2nd uint32 to its 1st uint32
		
		addps	xmm4, xmm5;		// results are in 1st and 2nd uint32


		movhlps	xmm0, xmm6;		// xmm6 high to xmm0 low
		movlhps	xmm0, xmm7;		// xmm7 low to xmm0 high

		addps	xmm6, xmm0;		// result of add are in xmm6 low
		addps	xmm7, xmm0;		// result of add are in xmm7 high

		shufps	xmm0, xmm6, 0x44;	// move xmm6 low DWORDs to xmm0 high
		shufps	xmm6, xmm7, 0xe4;	// move xmm7 high DWORS to xmm6
		movhlps	xmm7, xmm0;			// xmm6, xmm7 are mirrored

		shufps	xmm6, xmm6, 0x08;	// move xmm6's 3rd uint32 to its 2nd uint32
		shufps	xmm7, xmm7, 0x0d;	// move xmm7's 4th uint32 to its 2nd uint32, 
									// and move its 2nd uint32 to its 1st uint32
		
		addps	xmm6, xmm7;		// results are in 1st and 2nd uint32
		
		movlhps xmm4, xmm6;		// final result is in xmm4
		movaps  DWORD PTR g_vtxTransformed [ecx], xmm4;

		movaps	xmm0,xmm4;
		shufps	xmm0,xmm0,0xff;
		divps	xmm4,xmm0;
		rcpps	xmm0,xmm0;
		movhlps	xmm0,xmm4;
		shufps	xmm0,xmm0,0xe8;
		movlhps	xmm4,xmm0;

		movaps	DWORD PTR g_vecProjected [ecx], xmm4;

		emms;
		ret;
	}
#endif
}

// Only used by DKR
__declspec( naked ) void  __fastcall SSEVec3TransformDKR(D3DXVECTOR4 &pOut, const D3DXVECTOR4 &pV)
{
#if _MSC_VER > 1200
	__asm
	{
		movaps	xmm1,	DWORD PTR [edx];		// xmm1 as original vector

		movaps	xmm4,	DWORD PTR dkrMatrixTransposed;	// row1
		movaps	xmm5,	DWORD PTR dkrMatrixTransposed[0x10];	// row2
		movaps	xmm6,	DWORD PTR dkrMatrixTransposed[0x20];	// row3
		movaps	xmm7,	DWORD PTR dkrMatrixTransposed[0x30];	// row4

		mulps	xmm4, xmm1;		// row 1
		mulps	xmm5, xmm1;		// row 2
		mulps	xmm6, xmm1;		// row 3
		mulps	xmm7, xmm1;		// row 4

		movhlps	xmm0, xmm4;		// xmm4 high to xmm0 low
		movlhps	xmm0, xmm5;		// xmm5 low to xmm0 high

		addps	xmm4, xmm0;		// result of add are in xmm4 low
		addps	xmm5, xmm0;		// result of add are in xmm5 high

		shufps	xmm0, xmm4, 0x44;	// move xmm4 low DWORDs to xmm0 high
		shufps	xmm4, xmm5, 0xe4;	// move xmm5 high DWORS to xmm4
		movhlps	xmm5, xmm0;			// xmm4, xmm5 are mirrored

		shufps	xmm4, xmm4, 0x08;	// move xmm4's 3rd uint32 to its 2nd uint32
		shufps	xmm5, xmm5, 0x0d;	// move xmm5's 4th uint32 to its 2nd uint32, 
		// and move its 2nd uint32 to its 1st uint32

		addps	xmm4, xmm5;		// results are in 1st and 2nd uint32


		movhlps	xmm0, xmm6;		// xmm6 high to xmm0 low
		movlhps	xmm0, xmm7;		// xmm7 low to xmm0 high

		addps	xmm6, xmm0;		// result of add are in xmm6 low
		addps	xmm7, xmm0;		// result of add are in xmm7 high

		shufps	xmm0, xmm6, 0x44;	// move xmm6 low DWORDs to xmm0 high
		shufps	xmm6, xmm7, 0xe4;	// move xmm7 high DWORS to xmm6
		movhlps	xmm7, xmm0;			// xmm6, xmm7 are mirrored

		shufps	xmm6, xmm6, 0x08;	// move xmm6's 3rd uint32 to its 2nd uint32
		shufps	xmm7, xmm7, 0x0d;	// move xmm7's 4th uint32 to its 2nd uint32, 
		// and move its 2nd uint32 to its 1st uint32

		addps	xmm6, xmm7;		// results are in 1st and 2nd uint32

		movlhps xmm4, xmm6;		// final result is in xmm4
		movaps  DWORD PTR [ecx], xmm4;

		emms;
		ret;
	}
#endif
}

float real255 = 255.0f;
float real128 = 128.0f;

__declspec( naked ) void  __fastcall SSEVec3TransformNormal()
{
#if _MSC_VER > 1200
	__asm
	{
		mov		DWORD PTR [g_normal][12], 0;

		movaps	xmm4,	DWORD PTR gRSPmodelViewTopTranspose;	// row1
		movaps	xmm5,	DWORD PTR gRSPmodelViewTopTranspose[0x10];	// row2
		movaps	xmm1,	DWORD PTR [g_normal];		// xmm1 as the normal vector
		movaps	xmm6,	DWORD PTR gRSPmodelViewTopTranspose[0x20];	// row3

		mulps	xmm4, xmm1;		// row 1
		mulps	xmm5, xmm1;		// row 2
		mulps	xmm6, xmm1;		// row 3

		movhlps	xmm0, xmm4;		// xmm4 high to xmm0 low
		movlhps	xmm0, xmm5;		// xmm5 low to xmm0 high

		addps	xmm4, xmm0;		// result of add are in xmm4 low
		addps	xmm5, xmm0;		// result of add are in xmm5 high

		shufps	xmm0, xmm4, 0x44;	// move xmm4 low DWORDs to xmm0 high
		shufps	xmm4, xmm5, 0xe4;	// move xmm5 high DWORS to xmm4
		movhlps	xmm5, xmm0;			// xmm4, xmm5 are mirrored

		shufps	xmm4, xmm4, 0x08;	// move xmm4's 3rd uint32 to its 2nd uint32
		shufps	xmm5, xmm5, 0x0d;	// move xmm5's 4th uint32 to its 2nd uint32, 

		addps	xmm4, xmm5;		// results are in 1st and 2nd uint32

		movaps	xmm1,xmm4;
		mulps	xmm1,xmm1;	//square
		movlhps	xmm7, xmm1;
		shufps	xmm7, xmm7,0x03;
		addss	xmm7, xmm1;

		movhlps	xmm0, xmm6;		// xmm6 high to xmm0 low
		addps	xmm6, xmm0;		// result of add are in xmm6 low

		movlhps	xmm0, xmm6;
		shufps	xmm0, xmm0, 0x03;
		addss	xmm0, xmm6;		// result of add is at xmm0's 1st uint32

		movlhps	xmm4, xmm0;

		mulss	xmm0,xmm0;
		addss	xmm7,xmm0;		// xmm7 1st uint32 is the sum of squares

#ifdef _DEBUG
		movaps  DWORD PTR [g_normal], xmm4;
		movss  DWORD PTR [g_normal][12], xmm7;
#endif
		xorps	xmm0,xmm0;
		ucomiss	xmm0,xmm7;
		jz		l2

		rsqrtss	xmm7,xmm7;
		shufps	xmm7,xmm7,0;
#ifdef _DEBUG
		movss  DWORD PTR [g_normal][12], xmm7;
#endif
		mulps	xmm4,xmm7;

		movaps  DWORD PTR [g_normal], xmm4;		// Normalized
		mov		DWORD PTR [g_normal][12], 0;

		emms;
		ret;
l2:
		movss	DWORD PTR [g_normal], xmm0;
		movss	DWORD PTR [g_normal][12], xmm0;
		emms;
		ret;
	}
#endif
}

void NormalizeNormalVec()
{
	float w = 1/sqrtf(g_normal.x*g_normal.x + g_normal.y*g_normal.y + g_normal.z*g_normal.z);
	g_normal.x *= w;
	g_normal.y *= w;
	g_normal.z *= w;
}


void InitRenderBase()
{
	if( status.isVertexShaderEnabled || status.bUseHW_T_L )
	{
		ProcessVertexData = ProcessVertexDataExternal;
	}
	else if( !status.isSSEEnabled || g_curRomInfo.bPrimaryDepthHack || options.enableHackForGames == HACK_FOR_NASCAR)
	{
		ProcessVertexData = ProcessVertexDataNoSSE;
	}
	else
	{
		ProcessVertexData = ProcessVertexDataSSE;
	}

	gRSPfFogMin = gRSPfFogMax = 0.0f;
	windowSetting.fMultX = windowSetting.fMultY = 2.0f;
	windowSetting.vpLeftW = windowSetting.vpTopW = 0;
	windowSetting.vpRightW = windowSetting.vpWidthW = 640;
	windowSetting.vpBottomW = windowSetting.vpHeightW = 480;
	gRSP.maxZ = 0;
	gRSP.nVPLeftN = gRSP.nVPTopN = 0;
	gRSP.nVPRightN = 640;
	gRSP.nVPBottomN = 640;
	gRSP.nVPWidthN = 640;
	gRSP.nVPHeightN = 640;
	gRDP.scissor.left=gRDP.scissor.top=0;
	gRDP.scissor.right=gRDP.scissor.bottom=640;
	
	//gRSP.bLightingEnable = gRSP.bTextureGen = false;
	gRSP.curTile=gRSPnumLights=gRSP.ambientLightColor=gRSP.ambientLightIndex= 0;
	gRSP.fAmbientLightR=gRSP.fAmbientLightG=gRSP.fAmbientLightB=0;
	gRSP.projectionMtxTop = gRSP.modelViewMtxTop = 0;
	gRDP.fogColor = gRDP.primitiveColor = gRDP.envColor = gRDP.primitiveDepth = gRDP.primLODMin = gRDP.primLODFrac = gRDP.LODFrac = 0;
	gRDP.fPrimitiveDepth = 0;
	gRSP.numVertices = 0;
	gRSP.maxVertexID = 0;
	gRDP.bFogEnableInBlender=false;
	gRSP.shadeMode=SHADE_SMOOTH;
	gRDP.keyR=gRDP.keyG=gRDP.keyB=gRDP.keyA=gRDP.keyRGB=gRDP.keyRGBA = 0;
	gRDP.fKeyA = 0;

	gRSP.fTexScaleX = 1/32.0f;
	gRSP.fTexScaleY = 1/32.0f;
	gRSP.bTextureEnabled = FALSE;

	gRSP.clip_ratio_left = 0;
	gRSP.clip_ratio_top = 0;
	gRSP.clip_ratio_right = 640;
	gRSP.clip_ratio_bottom = 480;
	gRSP.clip_ratio_negx = 1;
	gRSP.clip_ratio_negy = 1;
	gRSP.clip_ratio_posx = 1;
	gRSP.clip_ratio_posy = 1;
	gRSP.real_clip_scissor_left = 0;
	gRSP.real_clip_scissor_top = 0;
	gRSP.real_clip_scissor_right = 640;
	gRSP.real_clip_scissor_bottom = 480;
#ifndef _OLDCLIPPER
	windowSetting.clipping.left = 0;
	windowSetting.clipping.top = 0;
	windowSetting.clipping.right = 640;
	windowSetting.clipping.bottom = 480;
	windowSetting.clipping.width = 640;
	windowSetting.clipping.height = 480;
	windowSetting.clipping.needToClip = false;
#endif
	gRSP.real_clip_ratio_negx = 1;
	gRSP.real_clip_ratio_negy = 1;
	gRSP.real_clip_ratio_posx = 1;
	gRSP.real_clip_ratio_posy = 1;

	gRDP.tnl._u32	= 0;
	gRDP.otherMode.L		= 0;
	gRDP.otherMode.H		= 0;
	gRDP.fillColor		= 0xFFFFFFFF;
	gRDP.originalFillColor	=0;

	gRSP.ucode		= 1;
	//gRSP.vertexMult = 10; //Do much?
	gRSP.bNearClip	= false;
	gRSP.bRejectVtx	= false;

	gRDP.texturesAreReloaded = false;
	gRDP.textureIsChanged = false;
	gRDP.colorsAreReloaded = false;

	memset(&gRDP.otherMode,0,sizeof(RDP_OtherMode));
	memset(&gRDP.tiles,0,sizeof(Tile)*8);

	int i;

	for( i=0; i<MAX_VERTS; i++ )
		g_clipFlag[i] = 0;
	for( i=0; i<MAX_VERTS; i++ )
		g_vtxNonTransformed[i].w = 1;

	memset(gRSPn64lightsOld, 0, sizeof(N64LightOld)*16);
}

void SetFogMinMax(float fMin, float fMax, float fMul, float fOffset)
{

	if( fMin > fMax )
	{
		float temp = fMin;
		fMin = fMax;
		fMax = temp;
	}

	{
		gRSPfFogMin = max(0,fMin/500-1);
		gRSPfFogMax = fMax/500-1;
	}

	gRSPfFogDivider = 255/(gRSPfFogMax-gRSPfFogMin); //CHECKME 255 replace with 220, Aristotle claimed to of improved fog rendering and this is the only real thing that has changed?
	CRender::g_pRender->SetFogMinMax(fMin, fMax);
/*
	gRSPfFogMin = max(0,-gRSP.fFogOffset/gRSP.fFogMul);
	//gRSPfFogMax = min(1,(255.0f-gRSP.fFogOffset)/gRSP.fFogMul);
	gRSPfFogMax = (255.0f-gRSP.fFogOffset)/gRSP.fFogMul;
	//gRSPfFogMax = (gRSP.fFogMul-gRSP.fFogOffset)/gRSP.fFogMul;
	gRSP.fFogMul = fMul;
	gRSP.fFogOffset = fOffset;

	gRSPfFogDivider = 255/(gRSPfFogMax-gRSPfFogMin);
*/
}

void InitVertexColors()
{
}

void InitVertexTextureConstants()
{
	float scaleX;
	float scaleY;

	RenderTexture &tex0 = g_textures[gRSP.curTile];
	CTexture *surf = tex0.m_pCTexture;
	Tile &tile0 = gRDP.tiles[gRSP.curTile];

	scaleX = gRSP.fTexScaleX;
	scaleY = gRSP.fTexScaleY;

	gRSP.tex0scaleX = scaleX * tile0.fShiftScaleS/tex0.m_fTexWidth;
	gRSP.tex0scaleY = scaleY * tile0.fShiftScaleT/tex0.m_fTexHeight;

	gRSP.tex0OffsetX = tile0.fhilite_sl/tex0.m_fTexWidth;
	gRSP.tex0OffsetY = tile0.fhilite_tl/tex0.m_fTexHeight;

	if( CRender::g_pRender->IsTexel1Enable() )
	{
		RenderTexture &tex1 = g_textures[(gRSP.curTile+1)&7];
		CTexture *surf = tex1.m_pCTexture;
		Tile &tile1 = gRDP.tiles[(gRSP.curTile+1)&7];

		gRSP.tex1scaleX = scaleX * tile1.fShiftScaleS/tex1.m_fTexWidth;
		gRSP.tex1scaleY = scaleY * tile1.fShiftScaleT/tex1.m_fTexHeight;

		gRSP.tex1OffsetX = tile1.fhilite_sl/tex1.m_fTexWidth;
		gRSP.tex1OffsetY = tile1.fhilite_tl/tex1.m_fTexHeight;
	}

	gRSP.texGenXRatio = tile0.fShiftScaleS;
	gRSP.texGenYRatio = gRSP.fTexScaleX/gRSP.fTexScaleY*tex0.m_fTexWidth/tex0.m_fTexHeight*tile0.fShiftScaleT;
}

void TexGen(float &s, float &t)
{
	if (gRDP.tnl.TexGenLin)
	{   
		s = acosf(g_normal.x) / 3.14f;
		t = acosf(g_normal.y) / 3.14f;
	}
	else
	{
		s = 0.5f * ( 1.0f + g_normal.x);
		t = 0.5f * ( 1.0f - g_normal.y);
	}
}

void ComputeLOD(bool openGL)
{
	TLITVERTEX &v0 = g_vtxBuffer[0];
	TLITVERTEX &v1 = g_vtxBuffer[1];
	RenderTexture &tex0 = g_textures[gRSP.curTile];

	float d,dt;
	if( openGL )
	{
		float x = g_vtxProjected5[0][0] / g_vtxProjected5[0][4] - g_vtxProjected5[1][0] / g_vtxProjected5[1][4];
		float y = g_vtxProjected5[0][1] / g_vtxProjected5[0][4] - g_vtxProjected5[1][1] / g_vtxProjected5[1][4];

		x = windowSetting.vpWidthW*x/windowSetting.fMultX/2;
		y = windowSetting.vpHeightW*y/windowSetting.fMultY/2;
		d = sqrtf(x*x+y*y);
	}
	else
	{
		float x = (v0.x - v1.x)/ windowSetting.fMultX;
		float y = (v0.y - v1.y)/ windowSetting.fMultY;
		d = sqrtf(x*x+y*y);
	}

	float s0 = v0.tcord[0].u * tex0.m_fTexWidth;
	float t0 = v0.tcord[0].v * tex0.m_fTexHeight;
	float s1 = v1.tcord[0].u * tex0.m_fTexWidth;
	float t1 = v1.tcord[0].v * tex0.m_fTexHeight;

	dt = sqrtf((s0-s1)*(s0-s1)+(t0-t1)*(t0-t1));

	float lod = dt/d;
	int ilod = (int)lod;
	double intptr;
	float frac = log10f(lod)/log10f(2.0f);
	//DEBUGGER_IF_DUMP(pauseAtNext,{DebuggerAppendMsg("LOD frac = %f", frac);});
	float lod_tile = min((log10f(lod)/log10f(2.0f)), gRSP.curTile + floorf(frac));
 	frac = max(modf(lod / pow(2.0f,lod_tile),(float *)&intptr), gRDP.primLODMin / 255.0f);
	//DEBUGGER_IF_DUMP(pauseAtNext,{DebuggerAppendMsg("LOD = %f, frac = %f", lod, frac);});
	gRDP.LODFrac = (uint32)(frac*255);
	CRender::g_pRender->SetCombinerAndBlender();
}

bool bHalfTxtScale=false;
extern uint32 lastSetTile;

void InitVertex(uint32 dwV, uint32 vtxIndex, bool bTexture)
{
	VTX_DUMP(TRACE2("Init vertex (%d) to vtx buf[%d]:", dwV, vtxIndex));

	TLITVERTEX &v = g_vtxBuffer[vtxIndex];

	VTX_DUMP(TRACE4("  Trans: x=%f, y=%f, z=%f, w=%f",  g_vtxTransformed[dwV].x,g_vtxTransformed[dwV].y,g_vtxTransformed[dwV].z,g_vtxTransformed[dwV].w));
	v.x = g_vecProjected[dwV].x*gRSP.vtxXMul+gRSP.vtxXAdd;
		v.y = g_vecProjected[dwV].y*gRSP.vtxYMul+gRSP.vtxYAdd;
		v.z = (g_vecProjected[dwV].z + 1.0f) * 0.5f;	// DirectX minZ=0, maxZ=1
		//v.z = g_vecProjected[dwV].z;	// DirectX minZ=0, maxZ=1
		v.rhw = g_vecProjected[dwV].w;
		VTX_DUMP(TRACE4("  Proj : x=%f, y=%f, z=%f, rhw=%f",  v.x,v.y,v.z,v.rhw));

	if( gRSP.bProcessSpecularColor )
	{
		v.dcSpecular = CRender::g_pRender->PostProcessSpecularColor();
		if( gRDP.tnl.Fog )
		{
			v.dcSpecular &= 0x00FFFFFF;
			uint32	fogFct = 0xFF-(uint8)((g_fFogCoord[dwV]-gRSPfFogMin)*gRSPfFogDivider);
			v.dcSpecular |= (fogFct<<24);
		}
	}
	else if( gRDP.tnl.Fog )
	{
		uint32	fogFct = 0xFF-(uint8)((g_fFogCoord[dwV]-gRSPfFogMin)*gRSPfFogDivider);
		v.dcSpecular = (fogFct<<24);
	}
	VTX_DUMP(TRACE2("  (U,V): %f, %f",  g_fVtxTxtCoords[dwV].x,g_fVtxTxtCoords[dwV].y));

	v.dcDiffuse = g_dwVtxDifColor[dwV];
	if( gRDP.otherMode.key_en )
	{
		v.dcDiffuse &= 0x00FFFFFF;
		v.dcDiffuse |= (gRDP.keyA<<24);
	}
	else if( gRDP.otherMode.aa_en && gRDP.otherMode.clr_on_cvg==0 )
	{
		v.dcDiffuse |= 0xFF000000;
	}

	if( gRSP.bProcessDiffuseColor )
	{
		v.dcDiffuse = CRender::g_pRender->PostProcessDiffuseColor(v.dcDiffuse);
	}
	if( options.bWinFrameMode )
	{
		v.dcDiffuse = g_dwVtxDifColor[dwV];
	}

	if( bTexture )
	{
		// If the vert is already lit, then there is no normal (and hence we can't generate tex coord)
		// Only scale if not generated automatically
		if (gRDP.tnl.TexGen && gRDP.tnl.Light)
		{
			// Correction for texGen result
			float u0,u1,v0,v1;
			RenderTexture &tex0 = g_textures[gRSP.curTile];
			u0 = g_fVtxTxtCoords[dwV].x * 32 * 1024 * gRSP.fTexScaleX / tex0.m_fTexWidth;
			v0 = g_fVtxTxtCoords[dwV].y * 32 * 1024 * gRSP.fTexScaleY / tex0.m_fTexHeight;
			u0 *= (gRDP.tiles[gRSP.curTile].fShiftScaleS);
			v0 *= (gRDP.tiles[gRSP.curTile].fShiftScaleT);

			if( CRender::g_pRender->IsTexel1Enable() )
			{
				RenderTexture &tex1 = g_textures[(gRSP.curTile+1)&7];
				u1 = g_fVtxTxtCoords[dwV].x * 32 * 1024 * gRSP.fTexScaleX / tex1.m_fTexWidth;
				v1 = g_fVtxTxtCoords[dwV].y * 32 * 1024 * gRSP.fTexScaleY / tex1.m_fTexHeight;
				u1 *= gRDP.tiles[(gRSP.curTile+1)&7].fShiftScaleS;
				v1 *= gRDP.tiles[(gRSP.curTile+1)&7].fShiftScaleT;
				CRender::g_pRender->SetVertexTextureUVCoord(v, u0, v0, u1, v1);
			}
			else
			{
				CRender::g_pRender->SetVertexTextureUVCoord(v, u0, v0);
			}
		}
		else
		{
			float tex0u = g_fVtxTxtCoords[dwV].x *gRSP.tex0scaleX - gRSP.tex0OffsetX ;
			float tex0v = g_fVtxTxtCoords[dwV].y *gRSP.tex0scaleY - gRSP.tex0OffsetY ;

			if( CRender::g_pRender->IsTexel1Enable() )
			{
				float tex1u = g_fVtxTxtCoords[dwV].x *gRSP.tex1scaleX - gRSP.tex1OffsetX ;
				float tex1v = g_fVtxTxtCoords[dwV].y *gRSP.tex1scaleY - gRSP.tex1OffsetY ;

				CRender::g_pRender->SetVertexTextureUVCoord(v, tex0u, tex0v, tex1u, tex1v);
				VTX_DUMP(TRACE2("  (tex0): %f, %f",  tex0u,tex0v));
				VTX_DUMP(TRACE2("  (tex1): %f, %f",  tex1u,tex1v));
			}
			else
			{
				CRender::g_pRender->SetVertexTextureUVCoord(v, tex0u, tex0v);
				VTX_DUMP(TRACE2("  (tex0): %f, %f",  tex0u,tex0v));
			}
		}

		// Check for txt scale hack
		if( !bHalfTxtScale && g_curRomInfo.bTextureScaleHack &&
			(gRDP.tiles[lastSetTile].dwSize == TXT_SIZE_32b || gRDP.tiles[lastSetTile].dwSize == TXT_SIZE_4b ) )
		{
			int width = ((gRDP.tiles[lastSetTile].sh-gRDP.tiles[lastSetTile].sl+1)<<1);
			int height = ((gRDP.tiles[lastSetTile].th-gRDP.tiles[lastSetTile].tl+1)<<1);
			if( g_fVtxTxtCoords[dwV].x*gRSP.fTexScaleX == width || g_fVtxTxtCoords[dwV].y*gRSP.fTexScaleY == height )
			{
				bHalfTxtScale=true;
			}
		}
	}

	if( g_curRomInfo.bEnableTxtLOD && vtxIndex == 1 && gRDP.otherMode.text_lod )
	{
		if( CRender::g_pRender->IsTexel1Enable() && CRender::g_pRender->m_pColorCombiner->m_pDecodedMux->isUsed(MUX_LODFRAC) )
		{
			ComputeLOD(0);
		}
		else
		{
			gRDP.LODFrac = 0;
		}
	}

	VTX_DUMP(TRACE2("  DIF(%08X), SPE(%08X)",   v.dcDiffuse, v.dcSpecular));
	VTX_DUMP(TRACE0(""));
}
uint32 LightVertOld(D3DXVECTOR4 & norm, int vidx)
{
	float fCosT;

	// Do ambient
	float r = gRSP.fAmbientLightR;
	float g = gRSP.fAmbientLightG;
	float b = gRSP.fAmbientLightB;

	if( options.enableHackForGames != HACK_FOR_ZELDA_MM )
	{
		for (register unsigned int l=0; l < gRSPnumLights; l++)
		{
			fCosT = norm.x*gRSPlightsOld[l].tx + norm.y*gRSPlightsOld[l].ty + norm.z*gRSPlightsOld[l].tz; 

			//fCosT = norm.x*gRSPlights[l].direction.x + norm.y*gRSPlights[l].direction.y + norm.z*gRSPlights[l].direction.z;

			if (fCosT > 0.0f)
			{
				r += gRSPlightsOld[l].fr * fCosT;
				g += gRSPlightsOld[l].fg * fCosT;
				b += gRSPlightsOld[l].fb * fCosT;
			}
		}
	}
	else
	{
		D3DXVECTOR4 v;
		bool transformed = false;

		for (register unsigned int l=0; l < gRSPnumLights; l++)
		{
			if( gRSPlightsOld[l].range == 0 )
			{
				// Regular directional light
				fCosT = norm.x*gRSPlightsOld[l].x + norm.y*gRSPlightsOld[l].y + norm.z*gRSPlightsOld[l].z; 
				//fCosT = norm.x*gRSPlights[l].direction.x + norm.y*gRSPlights[l].direction.y + norm.z*gRSPlights[l].direction.z;

				if (fCosT > 0 )
				{
					r += gRSPlightsOld[l].fr * fCosT;
					g += gRSPlightsOld[l].fg * fCosT;
					b += gRSPlightsOld[l].fb * fCosT;
				}
			}
			else //if( (gRSPlights[l].col&0x00FFFFFF) != 0x00FFFFFF )
			{
				// Point light
				if( !transformed )
				{
					D3DXVec3Transform(&v, (D3DXVECTOR3*)&g_vtxNonTransformed[vidx], &gRSPmodelViewTop);	// Convert to w=1
					transformed = true;
				}

				D3DXVECTOR3 dir(gRSPlightsOld[l].x - v.x, gRSPlightsOld[l].y - v.y, gRSPlightsOld[l].z - v.z);
				//D3DXVECTOR3 dir(v.x-gRSPlightsOld[l].x, v.y-gRSPlightsOld[l].y, v.z-gRSPlightsOld[l].z);
				float d2 = sqrtf(dir.x*dir.x+dir.y*dir.y+dir.z*dir.z);
				dir.x /= d2;
				dir.y /= d2;
				dir.z /= d2;

				fCosT = norm.x*dir.x + norm.y*dir.y + norm.z*dir.z; 

				if (fCosT > 0 )
				{
					//float f = d2/gRSPlights[l].range*50;
					float f = d2/15000*50;
					f = 1 - min(f,1);
					fCosT *= f*f;

					r += gRSPlightsOld[l].fr * fCosT;
					g += gRSPlightsOld[l].fg * fCosT;
					b += gRSPlightsOld[l].fb * fCosT;
				}

			}
		}
	}

	if (r > 255) r = 255;
	if (g > 255) g = 255;
	if (b > 255) b = 255;
	return ((0xff000000)|(((uint32)r)<<16)|(((uint32)g)<<8)|((uint32)b));
}


uint32 LightVert(D3DXVECTOR4 & norm)
{
	// Do ambient
	float r = gRSPlights[gRSP.ambientLightIndex].colour.r;
	float g = gRSPlights[gRSP.ambientLightIndex].colour.g;
	float b = gRSPlights[gRSP.ambientLightIndex].colour.b;

	for (unsigned int l=0; l < gRSPnumLights; l++)
	{
		// Regular directional light
		float fCosT = norm.x*gRSPlights[l].direction.x + norm.y*gRSPlights[l].direction.y + norm.z*gRSPlights[l].direction.z;

		if (fCosT > 0.0f)
		{
			r += gRSPlights[l].colour.fr * fCosT;
			g += gRSPlights[l].colour.fg * fCosT;
			b += gRSPlights[l].colour.fb * fCosT;
		}
	}

	if (r > 255) r = 255;
	if (g > 255) g = 255;
	if (b > 255) b = 255;
	return ((0xff000000)|(((uint32)r)<<16)|(((uint32)g)<<8)|((uint32)b));
}

uint32 LightPointVert(D3DXVECTOR4 & w)
{
	// Do ambient
	float r = gRSPlights[gRSP.ambientLightIndex].colour.r;
	float g = gRSPlights[gRSP.ambientLightIndex].colour.g;
	float b = gRSPlights[gRSP.ambientLightIndex].colour.b;

	for (unsigned int l = 0; l < gRSPnumLights; l++)
	{
		if (gRSPlights[l].SkipIfZero)
		{
			// Regular directional light
			D3DXVECTOR3 dir(gRSPlights[l].Position.x - w.x, gRSPlights[l].Position.y - w.y, gRSPlights[l].Position.z - w.z);

			float light_qlen = D3DXVec3LengthSq(&dir);
			float light_llen = sqrtf(light_qlen);

			float at = gRSPlights[l].ca + gRSPlights[l].la * light_llen + gRSPlights[l].qa * light_qlen;
			if (at > 0.0f)
			{
				float fCosT = 1.0f / at;
				r += gRSPlights[l].colour.fr * fCosT;
				g += gRSPlights[l].colour.fg * fCosT;
				b += gRSPlights[l].colour.fb * fCosT;
			}

		}
	}

	if (r > 255) r = 255;
	if (g > 255) g = 255;
	if (b > 255) b = 255;
	return ((0xff000000)|(((uint32)r)<<16)|(((uint32)g)<<8)|((uint32)b));
}


float zero = 0.0f;
float onef = 1.0f;
float fcosT;
#if _MSC_VER > 1200
__m128 cosT128;
__m64 icolor64;
__m128 icolor128;
#endif

__declspec( naked ) uint32  __fastcall SSELightVert()
{
#if _MSC_VER > 1200
	__asm
	{
		movaps		xmm3, DWORD PTR gRSP;	// loading Ambient colors, xmm3 is the result color
		movaps		xmm4, DWORD PTR [g_normal];	// xmm4 is the normal

		mov			ecx, 0;
loopback:
		cmp			ecx, DWORD PTR gRSPnumLights;
		jae			breakout;
		mov			eax,ecx;
		imul		eax,0x48;
		movups		xmm5, DWORD PTR gRSPlightsOld[eax];		// Light Dir
		movups		xmm1, DWORD PTR gRSPlightsOld[0x18][eax];	// Light color
		mulps       xmm5, xmm4;					// Lightdir * normals

		movlhps		xmm0,xmm5;
		addps		xmm0,xmm5;
		shufps		xmm5,xmm0,0xf0;			// xmm5 3rd float = xmm0 4th float
		addps		xmm0,xmm5;
		shufps		xmm0,xmm0,0x02;			// xmm0 1st float = xmm0 3rd float

		comiss		xmm0,zero;
		jc			endloop

		shufps      xmm0,xmm0,0;					// fcosT
		mulps       xmm1,xmm0; 
		addps       xmm3,xmm1; 
endloop:
		inc			ecx;
		jmp			loopback;
breakout:

		movss		xmm0,DWORD PTR real255;
		shufps      xmm0,xmm0,0;
		minps       xmm0,xmm3;

		// Without using a memory
		cvtss2si	eax,xmm0;	// move the 1st uint32 to eax
		shl			eax,10h;
		or			eax,0FF000000h;
		shufps		xmm0,xmm0,0E5h;	// move the 2nd uint32 to the 1st uint32
		cvtss2si	ecx,xmm0;	// move the 1st uint32 to ecx
		shl			ecx,8;
		or			eax,ecx;
		shufps		xmm0,xmm0,0E6h;	// Move the 3rd uint32 to the 1st uint32
		cvtss2si	ecx,xmm0;
		or          eax,ecx;

		ret;
	}
#else
	__asm ret;
#endif
}


__declspec( naked ) uint32  __fastcall SSELightVertNew()
{
#if _MSC_VER > 1200
	__asm
	{
		movaps		xmm3, DWORD PTR gRSP;	// loading Ambient colors, xmm3 is the result color
		movaps		xmm4, DWORD PTR [g_normal];	// xmm4 is the normal

		mov			ecx, 0;
loopback:
		cmp			ecx, DWORD PTR gRSPnumLights;
		jae			breakout;
		mov			eax,ecx;
		//imul		eax,0x28;
		//movups		xmm5, DWORD PTR gRSPlightsOld[eax];	// Light Dir
		imul		eax,0x48;
		movups		xmm5, DWORD PTR gRSPlightsOld[0x28][eax];	// Light Dir (transformed)
		movups		xmm1, DWORD PTR gRSPlightsOld[0x18][eax];	// Light color
		mulps       xmm5, xmm4;					// Lightdir * normals

		movlhps		xmm0,xmm5;
		addps		xmm0,xmm5;
		shufps		xmm5,xmm0,0xf0;			// xmm5 3rd float = xmm0 4th float
		addps		xmm0,xmm5;
		shufps		xmm0,xmm0,0x02;			// xmm0 1st float = xmm0 3rd float

		comiss		xmm0,zero;
		jc			endloop

		shufps      xmm0,xmm0,0;					// fcosT
		mulps       xmm1,xmm0; 
		addps       xmm3,xmm1; 
endloop:
		inc			ecx;
		jmp			loopback;
breakout:

		movss		xmm0,DWORD PTR real255;
		shufps      xmm0,xmm0,0;
		minps       xmm0,xmm3;

		// Without using a memory
		cvtss2si	eax,xmm0;	// move the 1st uint32 to eax
		shl			eax,10h;
		or			eax,0FF000000h;
		shufps		xmm0,xmm0,0E5h;	// move the 2nd uint32 to the 1st uint32
		cvtss2si	ecx,xmm0;	// move the 1st uint32 to ecx
		shl			ecx,8;
		or			eax,ecx;
		shufps		xmm0,xmm0,0E6h;	// Move the 3rd uint32 to the 1st uint32
		cvtss2si	ecx,xmm0;
		or          eax,ecx;

		ret;
	}
#else
	__asm ret;
#endif
}

inline void ReplaceAlphaWithFogFactor(int i)
{
	if( gRDP.tnl.Fog )
	{
		// Use fog factor to replace vertex alpha
		if( g_vecProjected[i].z > 1 )
			*(((uint8*)&(g_dwVtxDifColor[i]))+3) = 0xFF;
		if( g_vecProjected[i].z < 0 )
			*(((uint8*)&(g_dwVtxDifColor[i]))+3) = 0;
		else
			*(((uint8*)&(g_dwVtxDifColor[i]))+3) = (uint8)(g_vecProjected[i].z*255);	
	}
}


// Bits
// +-+-+-
// xxyyzz
#define Z_NEG  0x01
#define Z_POS  0x02
#define Y_NEG  0x04
#define Y_POS  0x08
#define X_NEG  0x10
#define X_POS  0x20

// Assumes dwAddr has already been checked!	
// Don't inline - it's too big with the transform macros

void ProcessVertexDataSSE(uint32 dwAddr, uint32 dwV0, uint32 dwNum)
{
	UpdateCombinedMatrix();

#if _MSC_VER > 1200


	// This function is called upon SPvertex
	// - do vertex matrix transform
	// - do vertex lighting
	// - do texture cooridinate transform if needed
	// - calculate normal vector

	// Output:  - g_vecProjected[i]				-> transformed vertex x,y,z
	//			- g_vecProjected[i].w						-> saved vertex 1/w
	//			- g_dwVtxFlags[i]				-> flags
	//			- g_dwVtxDifColor[i]			-> vertex color
	//			- g_fVtxTxtCoords[i]				-> vertex texture cooridinates

	FiddledVtx * pVtxBase = (FiddledVtx*)(g_pu8RamBase + dwAddr);
	g_pVtxBase = pVtxBase;

	uint32 i;
	for (i = dwV0; i < dwV0 + dwNum; i++)
	{
		SP_Timing(RSP_GBI0_Vtx);

		FiddledVtx & vert = pVtxBase[i - dwV0];

		g_vtxNonTransformed[i].x = (float)vert.x;
		g_vtxNonTransformed[i].y = (float)vert.y;
		g_vtxNonTransformed[i].z = (float)vert.z;

		SSEVec3Transform(i);

		if( gRDP.tnl.Fog )
		{
			g_fFogCoord[i] = g_vecProjected[i].z;
			if( g_vecProjected[i].w < 0 || g_vecProjected[i].z < 0 || g_fFogCoord[i] < gRSPfFogMin )
				g_fFogCoord[i] = gRSPfFogMin;
		}
		ReplaceAlphaWithFogFactor(i);

		/*if( gRSP.bFogEnabled ) // Rice 5.10
		{
			__asm {
				mov			eax, i;
				shl			eax, 2
				movss		xmm0, dword ptr g_vecProjected[eax*4][8];
				movss		xmm1, dword ptr g_vecProjected[eax*4][12];

				comiss		xmm1,zero;
				jc			step2;

				comiss		xmm0, dword ptr gRSPfFogMin;
				jc			step2;

				jmp			step3;
step2:
				movss		xmm0, dword ptr gRSPfFogMin;
step3:
				movss		dword ptr g_fFogCoord[eax], xmm0;
				mulss		xmm0, real255;
				cvtss2si	ecx,xmm0;	// move the 1st DWORD to ecx
				mov			byte ptr g_dwVtxDifColor[eax][3], cl; // Change to dwVtxDifColor to follow Rice 5.60
			}
		}*/


		VTX_DUMP( 
		{
			uint32 *dat = (uint32*)(&vert);
			DebuggerAppendMsg("vtx %d: %08X %08X %08X %08X", i, dat[0],dat[1],dat[2],dat[3]); 
			DebuggerAppendMsg("      : %f, %f, %f, %f", 
				g_vtxTransformed[i].x,g_vtxTransformed[i].y,g_vtxTransformed[i].z,g_vtxTransformed[i].w);
			DebuggerAppendMsg("      : %f, %f, %f, %f", 
				g_vecProjected[i].x,g_vecProjected[i].y,g_vecProjected[i].z,g_vecProjected[i].w);
		});

		RSP_Vtx_Clipping(i);

		if( gRDP.tnl.Light )
		{
			g_normal.x = (float)vert.norma.nx;
			g_normal.y = (float)vert.norma.ny;
			g_normal.z = (float)vert.norma.nz;

			//SSEVec3TransformNormal();
			//Vec3TransformNormal(g_normal, gRSPmodelViewTop);
			if (status.isSSEEnabled && ( options.enableHackForGames != HACK_FOR_ZELDA_MM ))
				//(( options.enableHackForGames != HACK_FOR_ZELDA_MM ) &&
				//( options.enableHackForGames != HACK_FOR_CONKER ))
			{
				SSEVec3TransformNormal();
				g_dwVtxDifColor[i] = SSELightVert();
			}
			else if(status.isSSEEnabled && options.enableHackForGames == HACK_FOR_ZELDA_MM )
			{
				SSEVec3TransformNormal();
				g_dwVtxDifColor[i] = LightVertOld(g_normal,i);
			}
			else
			{
				Vec3TransformNormal(g_normal, gRSPmodelViewTop);
				g_dwVtxDifColor[i] = LightVert(g_normal);
			}
			*(((uint8*)&(g_dwVtxDifColor[i]))+3) = vert.rgba.a;	// still use alpha from the vertex
		}
		else
		{
			if( (gRDP.tnl.Shade) == 0 && gRSP.ucode < 5 )	//Shade is disabled
			{
				//FLAT shade
				g_dwVtxDifColor[i] = gRDP.primitiveColor;
			}
			else
			{
				register IColor &color = *(IColor*)&g_dwVtxDifColor[i];
				color.b = vert.rgba.r;
				color.g = vert.rgba.g;
				color.r = vert.rgba.b;
				color.a = vert.rgba.a;
			}
		}

		if( options.bWinFrameMode )
		{
			g_dwVtxDifColor[i] = COLOR_RGBA(vert.rgba.r, vert.rgba.g, vert.rgba.b, vert.rgba.a);
		}

		// Update texture coords n.b. need to divide tu/tv by bogus scale on addition to buffer

		// If the vert is already lit, then there is no normal (and hence we
		// can't generate tex coord)
		if (gRDP.tnl.TexGen && gRDP.tnl.Light )
		{
			TexGen(g_fVtxTxtCoords[i].x, g_fVtxTxtCoords[i].y);
		}
		else
		{
			g_fVtxTxtCoords[i].x = (float)vert.tu;
			g_fVtxTxtCoords[i].y = (float)vert.tv; 
		}
	}

	VTX_DUMP(TRACE2("Setting Vertexes: %d - %d\n", dwV0, dwV0+dwNum-1));
	DEBUGGER_PAUSE_AND_DUMP(NEXT_VERTEX_CMD,{TRACE0("Paused at Vertex Cmd");});
#endif
}

void ProcessVertexDataNoSSE(uint32 dwAddr, uint32 dwV0, uint32 dwNum)
{

	UpdateCombinedMatrix();

	// This function is called upon SPvertex
	// - do vertex matrix transform
	// - do vertex lighting
	// - do texture cooridinate transform if needed
	// - calculate normal vector

	// Output:  - g_vecProjected[i]				-> transformed vertex x,y,z
	//			- g_vecProjected[i].w						-> saved vertex 1/w
	//			- g_dwVtxFlags[i]				-> flags
	//			- g_dwVtxDifColor[i]			-> vertex color
	//			- g_fVtxTxtCoords[i]				-> vertex texture cooridinates

	FiddledVtx * pVtxBase = (FiddledVtx*)(g_pu8RamBase + dwAddr);
	g_pVtxBase = pVtxBase;

	uint32 i;
	for (i = dwV0; i < dwV0 + dwNum; i++)
	{
		SP_Timing(RSP_GBI0_Vtx);

		FiddledVtx & vert = pVtxBase[i - dwV0];

		g_vtxNonTransformed[i].x = (float)vert.x;
		g_vtxNonTransformed[i].y = (float)vert.y;
		g_vtxNonTransformed[i].z = (float)vert.z;

		D3DXVec3Transform(&g_vtxTransformed[i], (D3DXVECTOR3*)&g_vtxNonTransformed[i], &gRSPworldProject);	// Convert to w=1

		g_vecProjected[i].w = 1.0f / g_vtxTransformed[i].w;
		g_vecProjected[i].x = g_vtxTransformed[i].x * g_vecProjected[i].w;
		g_vecProjected[i].y = g_vtxTransformed[i].y * g_vecProjected[i].w;

		if( (g_curRomInfo.bPrimaryDepthHack || options.enableHackForGames == HACK_FOR_NASCAR ) && gRDP.otherMode.depth_source )
		{
			g_vecProjected[i].z = gRDP.fPrimitiveDepth;
			g_vtxTransformed[i].z = gRDP.fPrimitiveDepth*g_vtxTransformed[i].w;
		}
		else
		{
			g_vecProjected[i].z = g_vtxTransformed[i].z * g_vecProjected[i].w;
		}

		if( gRDP.tnl.Fog )
		{
			g_fFogCoord[i] = g_vecProjected[i].z;
			if( g_vecProjected[i].w < 0 || g_vecProjected[i].z < 0 || g_fFogCoord[i] < gRSPfFogMin )
				g_fFogCoord[i] = gRSPfFogMin;
		}

		VTX_DUMP( 
		{
			uint32 *dat = (uint32*)(&vert);
			DebuggerAppendMsg("vtx %d: %08X %08X %08X %08X", i, dat[0],dat[1],dat[2],dat[3]); 
			DebuggerAppendMsg("      : %f, %f, %f, %f", 
				g_vtxTransformed[i].x,g_vtxTransformed[i].y,g_vtxTransformed[i].z,g_vtxTransformed[i].w);
			DebuggerAppendMsg("      : %f, %f, %f, %f", 
				g_vecProjected[i].x,g_vecProjected[i].y,g_vecProjected[i].z,g_vecProjected[i].w);
		});

		RSP_Vtx_Clipping(i);

		if( gRDP.tnl.Light )
		{
			g_normal.x = (float)vert.norma.nx;
			g_normal.y = (float)vert.norma.ny;
			g_normal.z = (float)vert.norma.nz;

			
			Vec3TransformNormal(g_normal, gRSPmodelViewTop);
			//else
			//{
				if (gRDP.tnl.PointLight && !status.isSSEEnabled)
				{
					g_dwVtxDifColor[i] = LightPointVert(D3DXVECTOR4(vert.x,vert.y,vert.z, 1.0f));
				}
				else
				{
					if(status.isSSEEnabled)
					//if((( options.enableHackForGames != HACK_FOR_ZELDA_MM ) &&
					//	( options.enableHackForGames != HACK_FOR_CONKER ))) //Hack for SSE & new lighting
					{
						g_dwVtxDifColor[i] = LightVertOld(g_normal , i);
					}
					else
					{
						//Vec3TransformNormal(g_normal, gRSPmodelViewTop);
						g_dwVtxDifColor[i] = LightVert(g_normal);
					}

				}
			//}
			*(((uint8*)&(g_dwVtxDifColor[i]))+3) = vert.rgba.a;	// still use alpha from the vertex
		}
		else
		{
			if( (gRDP.tnl.Shade) == 0 && gRSP.ucode < 5 )	//Shade is disabled
			{
				//FLAT shade
				g_dwVtxDifColor[i] = gRDP.primitiveColor;
			}
			else
			{
/*
				register IColor &color = *(IColor*)&g_dwVtxDifColor[i];
				color.b = vert.rgba.r;
				color.g = vert.rgba.g;
				color.r = vert.rgba.b;
				color.a = vert.rgba.a;
*/
			g_dwVtxDifColor[i] = COLOR_RGBA(vert.rgba.r, vert.rgba.g, vert.rgba.b, vert.rgba.a);
			}
		}

		if( options.bWinFrameMode )
		{
			g_dwVtxDifColor[i] = COLOR_RGBA(vert.rgba.r, vert.rgba.g, vert.rgba.b, vert.rgba.a);
		}

		ReplaceAlphaWithFogFactor(i);

		// Update texture coords n.b. need to divide tu/tv by bogus scale on addition to buffer

		// If the vert is already lit, then there is no normal (and hence we
		// can't generate tex coord)
		if (gRDP.tnl.TexGen && gRDP.tnl.Light )
		{
			TexGen(g_fVtxTxtCoords[i].x, g_fVtxTxtCoords[i].y);
		}
		else
		{
			g_fVtxTxtCoords[i].x = (float)vert.tu;
			g_fVtxTxtCoords[i].y = (float)vert.tv; 
		}
	}

	VTX_DUMP(TRACE2("Setting Vertexes: %d - %d\n", dwV0, dwV0+dwNum-1));
	DEBUGGER_PAUSE_AND_DUMP(NEXT_VERTEX_CMD,{TRACE0("Paused at Vertex Cmd");});
}

bool PrepareTriangle(uint32 dwV0, uint32 dwV1, uint32 dwV2)
{
	bool textureFlag = (CRender::g_pRender->IsTextureEnabled() || gRSP.ucode == 6 );

	InitVertex(dwV0, gRSP.numVertices, textureFlag);
	InitVertex(dwV1, gRSP.numVertices+1, textureFlag);
	InitVertex(dwV2, gRSP.numVertices+2, textureFlag);

	gRSP.numVertices += 3;
	status.dwNumTrisRendered++;
	return true;
}

bool AddTri(u32 v0, u32 v1, u32 v2, bool bTri4)
{
	if(bTri4 && v0 == v1)
		return false; // Cull empty tris

	if (IsTriangleVisible(v0, v1, v2))
	{
		if (CRender::g_pRender->IsTextureEnabled())
		{
			PrepareTextures();
			InitVertexTextureConstants();
		}

		CRender::g_pRender->SetCombinerAndBlender();

		PrepareTriangle(v0, v1, v2);
		return true;
	}
	return false;
}

// Returns TRUE if it thinks the triangle is visible
// Returns FALSE if it is clipped
bool IsTriangleVisible(uint32 dwV0, uint32 dwV1, uint32 dwV2)
{
	DEBUGGER_ONLY_IF( (!debuggerEnableTestTris || !debuggerEnableCullFace), {return TRUE;});
    
#ifdef _DEBUG
	// Check vertices are valid!
	if (dwV0 >= MAX_VERTS || dwV1 >= MAX_VERTS || dwV2 >= MAX_VERTS)
		return false;
#endif

	// Here we AND all the flags. If any of the bits is set for all
	// 3 vertices, it means that all three x, y or z lie outside of
	// the current viewing volume.
	// Currently disabled - still seems a bit dodgy
	if ((gRDP.tnl.TriCull || gRDP.tnl.CullBack) && gRDP.otherMode.zmode != 3)
	{
		D3DXVECTOR4 & v0 = g_vecProjected[dwV0];
		D3DXVECTOR4 & v1 = g_vecProjected[dwV1];
		D3DXVECTOR4 & v2 = g_vecProjected[dwV2];

		// Only try to clip if the tri is onscreen. For some reason, this
		// method doesnt' work well when the z value is outside of screenspace
		//if (v0.z < 1 && v1.z < 1 && v2.z < 1)
		{
			float V1 = v2.x - v0.x;
			float V2 = v2.y - v0.y;
			
			float W1 = v2.x - v1.x;
			float W2 = v2.y - v1.y;

			float fDirection = (V1 * W2) - (V2 * W1);
			fDirection = fDirection * v1.w * v2.w * v0.w;
			//float fDirection = v0.x*v1.y-v1.x*v0.y+v1.x*v2.y-v2.x*v1.y+v2.x*v0.y-v0.x*v2.y;
			/*
			*/

			if (fDirection < 0 && gRDP.tnl.CullBack)
			{
				status.dwNumTrisClipped++;
				return false;
			}
			else if (fDirection > 0 && gRDP.tnl.TriCull)
			{
				status.dwNumTrisClipped++;
				return false;
			}
		}
	}
	
#ifdef ENABLE_CLIP_TRI
	//if( gRSP.bRejectVtx && (g_clipFlag[dwV0]|g_clipFlag[dwV1]|g_clipFlag[dwV2]) )	
	//	return;
	if( g_clipFlag2[dwV0]&g_clipFlag2[dwV1]&g_clipFlag2[dwV2] )
	{
		//DebuggerAppendMsg("Clipped");
		return false;
	}
#endif

	return true;
}


void SetPrimitiveColor(uint32 dwCol, uint32 LODMin, uint32 LODFrac)
{
	gRDP.colorsAreReloaded = true;
	gRDP.primitiveColor = dwCol;
	gRDP.primLODMin = LODMin;
	gRDP.primLODFrac = LODFrac;
	if( gRDP.primLODFrac < gRDP.primLODMin )
	{
		gRDP.primLODFrac = gRDP.primLODMin;
	}

	gRDP.fvPrimitiveColor[0] = ((dwCol>>16)&0xFF)/255.0f;		//r
	gRDP.fvPrimitiveColor[1] = ((dwCol>>8)&0xFF)/255.0f;		//g
	gRDP.fvPrimitiveColor[2] = ((dwCol)&0xFF)/255.0f;			//b
	gRDP.fvPrimitiveColor[3] = ((dwCol>>24)&0xFF)/255.0f;		//a
}

void SetPrimitiveDepth(uint32 z, uint32 dwDZ)
{
	gRDP.primitiveDepth = z&0x7FFF;
	//if( gRDP.primitiveDepth != 0 && gRDP.primitiveDepth != 0xFFFF ) 
	{
		gRDP.fPrimitiveDepth = (float)(gRDP.primitiveDepth)/(float)0x8000;

		//gRDP.fPrimitiveDepth = gRDP.fPrimitiveDepth*2-1;	
		/*
		z=0xFFFF	->	1	the farest
		z=0			->	-1	the nearest
		*/
	}

	//how to use dwDZ?

#ifdef _DEBUG
	if( (pauseAtNext && (eventToPause == NEXT_VERTEX_CMD || eventToPause == NEXT_FLUSH_TRI )) )//&& logTriangles ) 
	{
		DebuggerAppendMsg("Set prim Depth: %f, (%08X, %08X)", gRDP.fPrimitiveDepth, z, dwDZ); 
	}
#endif
}

void SetVertexXYZ(uint32 vertex, float x, float y, float z)
{
	g_vecProjected[vertex].x = x;
	g_vecProjected[vertex].y = y;
	g_vecProjected[vertex].z = z;

	g_vtxTransformed[vertex].x = x*g_vtxTransformed[vertex].w;
	g_vtxTransformed[vertex].y = y*g_vtxTransformed[vertex].w;
	g_vtxTransformed[vertex].z = z*g_vtxTransformed[vertex].w;
}

void ModifyVertexInfo(uint32 where, uint32 vertex, uint32 val)
{
	switch (where)
	{
	case RSP_MV_WORD_OFFSET_POINT_RGBA:		// Modify RGBA
		{
			uint32 r = (val>>24)&0xFF;
			uint32 g = (val>>16)&0xFF;
			uint32 b = (val>>8)&0xFF;
			uint32 a = val&0xFF;
			g_dwVtxDifColor[vertex] = COLOR_RGBA(r, g, b, a);
			LOG_UCODE("Modify vert %d color, 0x%08x", vertex, g_dwVtxDifColor[vertex]);
		}
		break;
	case RSP_MV_WORD_OFFSET_POINT_XYSCREEN:		// Modify X,Y
		{
			uint16 x = (uint16)((val>>16) / 4.0f);
			uint16 y = (uint16)((val & 0xFFFF) / 4.0f);
/*
			uint16 nX = (uint16)(val>>16);
			short x = *((short*)&nX);
			x /= 4;

			uint16 nY = uint16(val&0xFFFF);
			short y = *((short*)&nY);
			y /= 4;
*/
			// Should do viewport transform

			x -= windowSetting.uViWidth/2;
			y = windowSetting.uViHeight/2-y;

			if( options.bEnableHacks && ((*g_GraphicsInfo.VI_X_SCALE_REG)&0xF) != 0 )
			{
				// Tarzan
				// I don't know why Tarzan is different
				SetVertexXYZ(vertex, x/windowSetting.fViWidth, y/windowSetting.fViHeight, g_vecProjected[vertex].z);
			}
			else
			{
				// Toy Story 2 and other games
				SetVertexXYZ(vertex, x*2/windowSetting.fViWidth, y*2/windowSetting.fViHeight, g_vecProjected[vertex].z);
			}

			LOG_UCODE("Modify vert %d: x=%d, y=%d", vertex, x, y);
			VTX_DUMP(TRACE3("Modify vert %d: (%d,%d)", vertex, x, y));
		}
		break;
	case RSP_MV_WORD_OFFSET_POINT_ZSCREEN:		// Modify C
		{
			int z = val>>16;

			SetVertexXYZ(vertex, g_vecProjected[vertex].x, g_vecProjected[vertex].y, (((float)z/0x03FF)+0.5f)/2.0f );
			LOG_UCODE("Modify vert %d: z=%d", vertex, z);
			VTX_DUMP(TRACE2("Modify vert %d: z=%d", vertex, z));
		}
		break;
	case RSP_MV_WORD_OFFSET_POINT_ST:		// Texture
		{
			short tu = short(val>>16);
			short tv = short(val & 0xFFFF);
			float ftu = tu / 32.0f;
			float ftv = tv / 32.0f;
			LOG_UCODE("      Setting vertex %d tu/tv to %f, %f", vertex, (float)tu, (float)tv);
			CRender::g_pRender->SetVtxTextureCoord(vertex, ftu/gRSP.fTexScaleX, ftv/gRSP.fTexScaleY);
		}
		break;
	default:
		RSP_RDP_NOIMPL("RSP_GBI1_ModifyVtx: Setting unk value: 0x%02x, 0x%08x", dwWhere, dwValue);
		break;
	}
	DEBUGGER_PAUSE_AND_DUMP(NEXT_VERTEX_CMD,{TRACE0("Paused at ModVertex Cmd");});
}

extern u32 gDKRCMatrixIndex;
extern u32 gDKRVtxCount;
extern bool gDKRBillBoard;

void ProcessVertexDataDKR(uint32 dwAddr, uint32 dwV0, uint32 dwNum)
{
	UpdateCombinedMatrix();

	uint32 pVtxBase = uint32(g_pu8RamBase + dwAddr);
	g_pVtxBase = (FiddledVtx*)pVtxBase;

	Matrix &matWorldProject(gRSP.DKRMatrixes[gDKRCMatrixIndex]);

	uint32 i;
	LONG nOff;

	bool addbase=false;
	if ((!gDKRBillBoard) || (gDKRCMatrixIndex != 2) )
		addbase = false;
	else
		addbase = true;

	if( addbase && gDKRVtxCount == 0 && dwNum > 1 )
	{
		gDKRVtxCount++;
	}

	LOG_UCODE("    ProcessVertexDataDKR, CMatrix = %d, Add base=%s", gDKRCMatrixIndex, gDKRBillBoard?"true":"false");
	VTX_DUMP(TRACE2("DKR Setting Vertexes\nCMatrix = %d, Add base=%s", gDKRCMatrixIndex, gDKRBillBoard?"true":"false"));

	nOff = 0;
	uint32 end = dwV0 + dwNum;
	for (i = dwV0; i < end; i++)
	{
		D3DXVECTOR3 w;

		g_vtxNonTransformed[i].x = (float)*(short*)((pVtxBase+nOff + 0) ^ 2);
		g_vtxNonTransformed[i].y = (float)*(short*)((pVtxBase+nOff + 2) ^ 2);
		g_vtxNonTransformed[i].z = (float)*(short*)((pVtxBase+nOff + 4) ^ 2);

		//if( status.isSSEEnabled )
		//	SSEVec3TransformDKR(g_vtxTransformed[i], g_vtxNonTransformed[i]);
		//else
		D3DXVec3Transform(&g_vtxTransformed[i], (D3DXVECTOR3*)&g_vtxNonTransformed[i], &matWorldProject);	// Convert to w=1

		if( gDKRVtxCount == 0 && dwNum==1 )
		{
			gRSP.DKRBaseVec.x = g_vtxTransformed[i].x;
			gRSP.DKRBaseVec.y = g_vtxTransformed[i].y;
			gRSP.DKRBaseVec.z = g_vtxTransformed[i].z;
			gRSP.DKRBaseVec.w = g_vtxTransformed[i].w;
		}
		else if( addbase )
		{
			g_vtxTransformed[i].x += gRSP.DKRBaseVec.x;
			g_vtxTransformed[i].y += gRSP.DKRBaseVec.y;
			g_vtxTransformed[i].z += gRSP.DKRBaseVec.z;
			g_vtxTransformed[i].w  = gRSP.DKRBaseVec.w;
		}

		g_vecProjected[i].w = 1.0f / g_vtxTransformed[i].w;
		g_vecProjected[i].x = g_vtxTransformed[i].x * g_vecProjected[i].w;
		g_vecProjected[i].y = g_vtxTransformed[i].y * g_vecProjected[i].w;
		g_vecProjected[i].z = g_vtxTransformed[i].z * g_vecProjected[i].w;

		gDKRVtxCount++;

		VTX_DUMP(TRACE5("vtx %d: %f, %f, %f, %f", i, 
			g_vtxTransformed[i].x,g_vtxTransformed[i].y,g_vtxTransformed[i].z,g_vtxTransformed[i].w));

		if( gRDP.tnl.Fog )
		{
			g_fFogCoord[i] = g_vecProjected[i].z;
			if( g_vecProjected[i].w < 0 || g_vecProjected[i].z < 0 || g_fFogCoord[i] < gRSPfFogMin )
				g_fFogCoord[i] = gRSPfFogMin;
		}

		RSP_Vtx_Clipping(i);

		short wA = *(short*)((pVtxBase+nOff + 6) ^ 2);
		short wB = *(short*)((pVtxBase+nOff + 8) ^ 2);

		s8 r = (s8)(wA >> 8);
		s8 g = (s8)(wA);
		s8 b = (s8)(wB >> 8);
		s8 a = (s8)(wB);

		if (gRDP.tnl.Light)
		{
			g_normal.x = (char)r; //norma.nx;
			g_normal.y = (char)g; //norma.ny;
			g_normal.z = (char)b; //norma.nz;

				Vec3TransformNormal(g_normal, gRSPmodelViewTop);
				if(status.isSSEEnabled)
					//(( options.enableHackForGames != HACK_FOR_ZELDA_MM ) &&
					//( options.enableHackForGames != HACK_FOR_CONKER ))
				{
				//SSEVec3TransformNormal();
				g_dwVtxDifColor[i] = SSELightVert();
				}
				else{
				//Vec3TransformNormal(g_normal, matWorldProject)
				g_dwVtxDifColor[i] = LightVert(g_normal);
				}
		}
		else
		{
			LONG nR, nG, nB, nA;

			nR = r;
			nG = g;
			nB = b;
			nA = a;
			// Assign true vert colour after lighting/fogging
			g_dwVtxDifColor[i] = COLOR_RGBA(nR, nG, nB, nA);
		}

		ReplaceAlphaWithFogFactor(i);

		g_fVtxTxtCoords[i].x = g_fVtxTxtCoords[i].y = 1;

		nOff += 10;
	}


	DEBUGGER_PAUSE_AND_DUMP(NEXT_VERTEX_CMD,{DebuggerAppendMsg("Paused at DKR Vertex Cmd, v0=%d, vn=%d, addr=%08X", dwV0, dwNum, dwAddr);});
}

extern uint32 dwPDCIAddr;
void ProcessVertexDataPD(uint32 dwAddr, uint32 dwV0, uint32 dwNum)
{
	UpdateCombinedMatrix();

	N64VtxPD * pVtxBase = (N64VtxPD*)(g_pu8RamBase + dwAddr);
	g_pVtxBase = (FiddledVtx*)pVtxBase;	// Fix me

	for (uint32 i = dwV0; i < dwV0 + dwNum; i++)
	{
		N64VtxPD &vert = pVtxBase[i - dwV0];

		g_vtxNonTransformed[i].x = (float)vert.x;
		g_vtxNonTransformed[i].y = (float)vert.y;
		g_vtxNonTransformed[i].z = (float)vert.z;

		if( status.isSSEEnabled )
			SSEVec3Transform(i);
		else
		{
			D3DXVec3Transform(&g_vtxTransformed[i], (D3DXVECTOR3*)&g_vtxNonTransformed[i], &gRSPworldProject);	// Convert to w=1
			g_vecProjected[i].w = 1.0f / g_vtxTransformed[i].w;
			g_vecProjected[i].x = g_vtxTransformed[i].x * g_vecProjected[i].w;
			g_vecProjected[i].y = g_vtxTransformed[i].y * g_vecProjected[i].w;
			g_vecProjected[i].z = g_vtxTransformed[i].z * g_vecProjected[i].w;
		}

		g_fFogCoord[i] = g_vecProjected[i].z;
		if( g_vecProjected[i].w < 0 || g_vecProjected[i].z < 0 || g_fFogCoord[i] < gRSPfFogMin )
			g_fFogCoord[i] = gRSPfFogMin;


		RSP_Vtx_Clipping(i);

		uint8 *addr = g_pu8RamBase+dwPDCIAddr+ (vert.cidx&0xFF);
		uint32 a = addr[0];
		uint32 r = addr[3];
		uint32 g = addr[2];
		uint32 b = addr[1];

		if( gRDP.tnl.Light )
		{
			g_normal.x = (char)r;
			g_normal.y = (char)g;
			g_normal.z = (char)b;

			//Vec3TransformNormal(g_normal, gRSPmodelViewTop);

			if(status.isSSEEnabled)
				//(( options.enableHackForGames != HACK_FOR_ZELDA_MM ) &&
				//( options.enableHackForGames != HACK_FOR_CONKER ))
			{
				SSEVec3TransformNormal();
				g_dwVtxDifColor[i] = SSELightVert();
			}
			else
			{
				Vec3TransformNormal(g_normal, gRSPmodelViewTop);
				g_dwVtxDifColor[i] = LightVert(g_normal);
			}
			*(((uint8*)&(g_dwVtxDifColor[i]))+3) = (uint8)a;	// still use alpha from the vertex
		}
		else
		{
			if( (gRDP.tnl.Shade) == 0 && gRSP.ucode < 5 )	//Shade is disabled
			{
				g_dwVtxDifColor[i] = gRDP.primitiveColor;
			}
			else	//FLAT shade
			{
				g_dwVtxDifColor[i] = COLOR_RGBA(r, g, b, a);
			}
		}

		if( options.bWinFrameMode )
		{
			g_dwVtxDifColor[i] = COLOR_RGBA(r, g, b, a);
		}

		ReplaceAlphaWithFogFactor(i);

		VECTOR2 & t = g_fVtxTxtCoords[i];
		if (gRDP.tnl.TexGen && gRDP.tnl.Light )
		{
			// Not sure if we should transform the normal here
			//Matrix & matWV = gRSP.projectionMtxs[gRSP.projectionMtxTop];
			//Vec3TransformNormal(g_normal, matWV);

			TexGen(g_fVtxTxtCoords[i].x, g_fVtxTxtCoords[i].y);
		}
		else
		{
			t.x = vert.s;
			t.y = vert.t; 
		}


		VTX_DUMP( 
		{
			uint32 *dat = (uint32*)(&vert);
			DebuggerAppendMsg("vtx %d: %d %d %d", i, vert.x,vert.y,vert.z); 
			DebuggerAppendMsg("      : %f, %f, %f, %f", 
				g_vtxTransformed[i].x,g_vtxTransformed[i].y,g_vtxTransformed[i].z,g_vtxTransformed[i].w);
			DebuggerAppendMsg("      : %X, %X, %X, %X", r,g,b,a);
			DebuggerAppendMsg("      : u=%f, v=%f", t.x, t.y);
		});
	}

	VTX_DUMP(TRACE2("Setting Vertexes: %d - %d\n", dwV0, dwV0+dwNum-1));
	DEBUGGER_PAUSE_AND_DUMP(NEXT_VERTEX_CMD,{TRACE0("Paused at Vertex Cmd");});
}

extern uint32 dwConkerVtxZAddr;
void ProcessVertexDataConker(uint32 dwAddr, uint32 dwV0, uint32 dwNum)
{
	UpdateCombinedMatrix();

	FiddledVtx * pVtxBase = (FiddledVtx*)(g_pu8RamBase + dwAddr);
	g_pVtxBase = pVtxBase;
	
	short *vertexColoraddr = (short*)(g_pu8RamBase+dwConkerVtxZAddr);

	uint32 i;
	for (i = dwV0; i < dwV0 + dwNum; i++)
	{
		SP_Timing(RSP_GBI0_Vtx);

		FiddledVtx & vert = pVtxBase[i - dwV0];

		g_vtxNonTransformed[i].x = (float)vert.x;
		g_vtxNonTransformed[i].y = (float)vert.y;
		g_vtxNonTransformed[i].z = (float)vert.z;

		if( status.isSSEEnabled )
			SSEVec3Transform(i);
		else
		{
			D3DXVec3Transform(&g_vtxTransformed[i], (D3DXVECTOR3*)&g_vtxNonTransformed[i], &gRSPworldProject);	// Convert to w=1
			g_vecProjected[i].w = 1.0f / g_vtxTransformed[i].w;
			g_vecProjected[i].x = g_vtxTransformed[i].x * g_vecProjected[i].w;
			g_vecProjected[i].y = g_vtxTransformed[i].y * g_vecProjected[i].w;
			g_vecProjected[i].z = g_vtxTransformed[i].z * g_vecProjected[i].w;
		}

		g_fFogCoord[i] = g_vecProjected[i].z;
		if( g_vecProjected[i].w < 0 || g_vecProjected[i].z < 0 || g_fFogCoord[i] < gRSPfFogMin )
			g_fFogCoord[i] = gRSPfFogMin;

		VTX_DUMP( 
		{
			uint32 *dat = (uint32*)(&vert);
			DebuggerAppendMsg("vtx %d: %08X %08X %08X %08X", i, dat[0],dat[1],dat[2],dat[3]); 
			DebuggerAppendMsg("      : %f, %f, %f, %f", 
				g_vtxTransformed[i].x,g_vtxTransformed[i].y,g_vtxTransformed[i].z,g_vtxTransformed[i].w);
			DebuggerAppendMsg("      : %f, %f, %f, %f", 
				g_vecProjected[i].x,g_vecProjected[i].y,g_vecProjected[i].z,g_vecProjected[i].w);
		});

		RSP_Vtx_Clipping(i);

		if( gRDP.tnl.Light )
		{
			{
				uint32 r= 0;
				uint32 g= 0;
				uint32 b= 0;
				if(status.isSSEEnabled)
				{
					r= ((gRSP.ambientLightColor>>16)&0xFF);
					g= ((gRSP.ambientLightColor>> 8)&0xFF);
					b= ((gRSP.ambientLightColor    )&0xFF);

					for( uint32 k=1; k<=gRSPnumLights; k++)
					{
						r += gRSPlightsOld[k].r;
						g += gRSPlightsOld[k].g;
						b += gRSPlightsOld[k].b;
					}
				}
				else
				{
					r= gRSPlights[gRSP.ambientLightIndex].colour.r;
					g= gRSPlights[gRSP.ambientLightIndex].colour.g;
					b= gRSPlights[gRSP.ambientLightIndex].colour.b;

					for( uint32 k=1; k<=gRSPnumLights; k++)
					{
						r += gRSPlights[k].colour.r;
						g += gRSPlights[k].colour.g;
						b += gRSPlights[k].colour.b;
					}
				}
				if( r>255 ) 
					r=255;
				if( g>255 ) 
					g=255;
				if( b>255 )
					b=255;
				r *= vert.rgba.r ;
				g *= vert.rgba.g ;
				b *= vert.rgba.b ;
				r >>= 8;
				g >>= 8;
				b >>= 8;
				g_dwVtxDifColor[i] = 0xFF000000;
				g_dwVtxDifColor[i] |= (r<<16);
				g_dwVtxDifColor[i] |= (g<< 8);
				g_dwVtxDifColor[i] |= (b    );			
			}

			*(((uint8*)&(g_dwVtxDifColor[i]))+3) = vert.rgba.a;	// still use alpha from the vertex
//TEXTURE
			g_vtxTransformed[i].x = (float)vert.tu;
			g_vtxTransformed[i].y = (float)vert.tv;

		}
		else
		{
			if( (gRDP.tnl.Shade) == 0 && gRSP.ucode < 5 )	//Shade is disabled
			{
				g_dwVtxDifColor[i] = gRDP.primitiveColor;
			}
			else	//FLAT shade
			{
				g_dwVtxDifColor[i] = COLOR_RGBA(vert.rgba.r, vert.rgba.g, vert.rgba.b, vert.rgba.a);
			}
		}

		if( options.bWinFrameMode )
		{
			//g_vecProjected[i].z = 0;
			g_dwVtxDifColor[i] = COLOR_RGBA(vert.rgba.r, vert.rgba.g, vert.rgba.b, vert.rgba.a);
		}

		ReplaceAlphaWithFogFactor(i);

		// Update texture coords n.b. need to divide tu/tv by bogus scale on addition to buffer
		VECTOR2 & t = g_fVtxTxtCoords[i];

		// If the vert is already lit, then there is no normal (and hence we
		// can't generate tex coord)
		if (gRDP.tnl.TexGen && gRDP.tnl.Light )
		{
				g_normal.x = (float)*(char*)(g_pu8RamBase+ (((i<<1)+0)^3)+dwConkerVtxZAddr);
				g_normal.y = (float)*(char*)(g_pu8RamBase+ (((i<<1)+1)^3)+dwConkerVtxZAddr);
				g_normal.z = (float)*(char*)(g_pu8RamBase+ (((i<<1)+2)^3)+dwConkerVtxZAddr);
				Vec3TransformNormal(g_normal, gRSPmodelViewTop);
				TexGen(g_fVtxTxtCoords[i].x, g_fVtxTxtCoords[i].y);
		}
		else
		{
			g_fVtxTxtCoords[i].x = (float)vert.tu;
			g_fVtxTxtCoords[i].y = (float)vert.tv; 
		}
	}

	VTX_DUMP(TRACE2("Setting Vertexes: %d - %d\n", dwV0, dwV0+dwNum-1));
	DEBUGGER_PAUSE_AND_DUMP(NEXT_VERTEX_CMD,{DebuggerAppendMsg("Paused at Vertex Cmd");});
}


typedef struct{
	short y;
	short x;
	short flag;
	short z;
} RS_Vtx_XYZ;

typedef union {
	struct {
		uint8 a;
		uint8 b;
		uint8 g;
		uint8 r;
	};
	struct {
		char na;
		char nz;	//b
		char ny;	//g
		char nx;	//r
	};
} RS_Vtx_Color;


void ProcessVertexData_Rogue_Squadron(uint32 dwXYZAddr, uint32 dwColorAddr, uint32 dwXYZCmd, uint32 dwColorCmd)
{
	UpdateCombinedMatrix();

	uint32 dwV0 = 0;
	uint32 dwNum = (dwXYZCmd&0xFF00)>>10;

	RS_Vtx_XYZ * pVtxXYZBase = (RS_Vtx_XYZ*)(g_pu8RamBase + dwXYZAddr);
	RS_Vtx_Color * pVtxColorBase = (RS_Vtx_Color*)(g_pu8RamBase + dwColorAddr);

	uint32 i;
	for (i = dwV0; i < dwV0 + dwNum; i++)
	{
		RS_Vtx_XYZ & vertxyz = pVtxXYZBase[i - dwV0];
		RS_Vtx_Color & vertcolors = pVtxColorBase[i - dwV0];

		g_vtxNonTransformed[i].x = (float)vertxyz.x;
		g_vtxNonTransformed[i].y = (float)vertxyz.y;
		g_vtxNonTransformed[i].z = (float)vertxyz.z;

		if( status.isSSEEnabled )
			SSEVec3Transform(i);
		else
		{
			D3DXVec3Transform(&g_vtxTransformed[i], (D3DXVECTOR3*)&g_vtxNonTransformed[i], &gRSPworldProject);	// Convert to w=1
			g_vecProjected[i].w = 1.0f / g_vtxTransformed[i].w;
			g_vecProjected[i].x = g_vtxTransformed[i].x * g_vecProjected[i].w;
			g_vecProjected[i].y = g_vtxTransformed[i].y * g_vecProjected[i].w;
			g_vecProjected[i].z = g_vtxTransformed[i].z * g_vecProjected[i].w;
		}

		VTX_DUMP( 
		{
			DebuggerAppendMsg("      : %f, %f, %f, %f", 
				g_vtxTransformed[i].x,g_vtxTransformed[i].y,g_vtxTransformed[i].z,g_vtxTransformed[i].w);
			DebuggerAppendMsg("      : %f, %f, %f, %f", 
				g_vecProjected[i].x,g_vecProjected[i].y,g_vecProjected[i].z,g_vecProjected[i].w);
		});

		g_fFogCoord[i] = g_vecProjected[i].z;
		if( g_vecProjected[i].w < 0 || g_vecProjected[i].z < 0 || g_fFogCoord[i] < gRSPfFogMin )
			g_fFogCoord[i] = gRSPfFogMin;

		RSP_Vtx_Clipping(i);

		if( gRDP.tnl.Light )
		{
			g_normal.x = (float)vertcolors.nx;
			g_normal.y = (float)vertcolors.ny;
			g_normal.z = (float)vertcolors.nz;

			//Vec3TransformNormal(g_normal, gRSPmodelViewTop);
			
			if(status.isSSEEnabled)
				//( options.enableHackForGames != HACK_FOR_ZELDA_MM ) &&
				//( options.enableHackForGames != HACK_FOR_CONKER ))
			{
				SSEVec3TransformNormal();
				g_dwVtxDifColor[i] = SSELightVert();
			}
			else
			{
				Vec3TransformNormal(g_normal, gRSPmodelViewTop);
				g_dwVtxDifColor[i] = LightVert(g_normal);
			}
			*(((uint8*)&(g_dwVtxDifColor[i]))+3) = vertcolors.a;	// still use alpha from the vertex
		}
		else
		{
			if( (gRDP.tnl.Shade) == 0 && gRSP.ucode < 5 )	//Shade is disabled
			{
				g_dwVtxDifColor[i] = gRDP.primitiveColor;
			}
			else	//FLAT shade
			{
				g_dwVtxDifColor[i] = COLOR_RGBA(vertcolors.r, vertcolors.g, vertcolors.b, vertcolors.a);
			}
		}

		if( options.bWinFrameMode )
		{
			g_dwVtxDifColor[i] = COLOR_RGBA(vertcolors.r, vertcolors.g, vertcolors.b, vertcolors.a);
		}

		ReplaceAlphaWithFogFactor(i);

		/*
		// Update texture coords n.b. need to divide tu/tv by bogus scale on addition to buffer
		VECTOR2 & t = g_fVtxTxtCoords[i];

		// If the vert is already lit, then there is no normal (and hence we
		// can't generate tex coord)
		if (gRDP.tnl.TexGen && gRDP.tnl.Light && g_textures[gRSP.curTile].m_bTextureEnable )
		{
			TexGen(g_fVtxTxtCoords[i].x, g_fVtxTxtCoords[i].y);
		}
		else
		{
			t.x = (float)vert.tu;
			t.y = (float)vert.tv; 
		}
		*/
	}

	VTX_DUMP(TRACE2("Setting Vertexes: %d - %d\n", dwV0, dwV0+dwNum-1));
	DEBUGGER_PAUSE_AND_DUMP(NEXT_VERTEX_CMD,{TRACE0("Paused at Vertex Cmd");});
}

void SetLightColOld(uint32 dwLight, uint32 dwCol)
{
	gRSPlightsOld[dwLight].r = (uint8)((dwCol >> 24)&0xFF);
	gRSPlightsOld[dwLight].g = (uint8)((dwCol >> 16)&0xFF);
	gRSPlightsOld[dwLight].b = (uint8)((dwCol >>  8)&0xFF);
	gRSPlightsOld[dwLight].a = 255;	// Ignore light alpha
	gRSPlightsOld[dwLight].fr = (float)gRSPlightsOld[dwLight].r;
	gRSPlightsOld[dwLight].fg = (float)gRSPlightsOld[dwLight].g;
	gRSPlightsOld[dwLight].fb = (float)gRSPlightsOld[dwLight].b;
	gRSPlightsOld[dwLight].fa = 255;	// Ignore light alpha

	//TRACE1("Set light %d color", dwLight);
//	LIGHT_DUMP(TRACE2("Set Light %d color: %08X", dwLight, dwCol));
}

void SetLightCol(uint32 dwLight, u8 r, u8 g, u8 b)
{
	
	gRSPlights[dwLight].SkipIfZero = (r + g + b);
	gRSPlights[dwLight].colour.r = r;
	gRSPlights[dwLight].colour.g = g;
	gRSPlights[dwLight].colour.b = b;
	gRSPlights[dwLight].colour.a = 255;	// Ignore light alpha
	gRSPlights[dwLight].colour.fr = (float)gRSPlights[dwLight].colour.r;
	gRSPlights[dwLight].colour.fg = (float)gRSPlights[dwLight].colour.g;
	gRSPlights[dwLight].colour.fb = (float)gRSPlights[dwLight].colour.b;
	gRSPlights[dwLight].colour.fa = 255;	// Ignore light alpha

	//TRACE1("Set light %d color", dwLight);
//	LIGHT_DUMP(TRACE2("Set Light %d color: %08X", dwLight, dwCol));
}

void SetLightDirection(uint32 dwLight, float x, float y, float z, float range)
{
	if(status.isSSEEnabled)
	{
		float w = range == 0 ? (float)sqrt(x*x+y*y+z*z) : 1;

		gRSPlightsOld[dwLight].x = x/w;
		gRSPlightsOld[dwLight].y = y/w;
		gRSPlightsOld[dwLight].z = z/w;
		gRSPlightsOld[dwLight].range = range;
	}
	else
	{

		float w =  sqrt(x*x + y*y + z*z);

		gRSPlights[dwLight].direction.x = x/w;
		gRSPlights[dwLight].direction.y = y/w;
		gRSPlights[dwLight].direction.z = z/w;
		gRSPlights[dwLight].direction.range = range;
	}

	DEBUGGER_PAUSE_AND_DUMP(NEXT_SET_LIGHT,TRACE4("Set Light %d dir: %.4f, %.4f, %.4f, %.4f", dwLight, x, y, z, range));
}

void SetLightPosition(uint32 dwLight, float x, float y, float z, float w)
{
	gRSPlights[dwLight].Position.x = x;
	gRSPlights[dwLight].Position.y = y;
	gRSPlights[dwLight].Position.z = z;
	gRSPlights[dwLight].Position.w = w;
}

void SetLightCBFD(uint32 dwLight, short nonzero)
{
	gRSPlights[dwLight].Iscale = (float)(nonzero << 12);
	gRSPlights[dwLight].SkipIfZero = gRSPlights[dwLight].SkipIfZero&&nonzero;
}


void SetLightEx(uint32 dwLight, float ca, float la, float qa)
{
	gRSPlights[dwLight].ca = ca / 16.0f;
	gRSPlights[dwLight].la = la / 65535.0f;
	gRSPlights[dwLight].qa = qa / (8.0f*65535.0f);
}

static float maxS0, maxT0;
static float maxS1, maxT1;
static bool validS0, validT0;
static bool validS1, validT1;

void LogTextureCoords(float fTex0S, float fTex0T, float fTex1S, float fTex1T)
{
	if( validS0 )
	{
		if( fTex0S<0 || fTex0S>maxS0 )	validS0 = false;
	}
	if( validT0 )
	{
		if( fTex0T<0 || fTex0T>maxT0 )	validT0 = false;
	}
	if( validS1 )
	{
		if( fTex1S<0 || fTex1S>maxS1 )	validS1 = false;
	}
	if( validT1 )
	{
		if( fTex1T<0 || fTex1T>maxT1 )	validT1 = false;
	}
}

bool CheckTextureCoords(int tex)
{
	if( tex==0 )
	{
		return validS0&&validT0;
	}
	else
	{
		return validS1&&validT1;
	}
}

void ResetTextureCoordsLog(float maxs0, float maxt0, float maxs1, float maxt1)
{
	maxS0 = maxs0;
	maxT0 = maxt0;
	maxS1 = maxs1;
	maxT1 = maxt1;
	validS0 = validT0 = true;
	validS1 = validT1 = true;
}

void ForceMainTextureIndex(int dwTile) 
{
	if( dwTile == 1 && !(CRender::g_pRender->IsTexel0Enable()) && CRender::g_pRender->IsTexel1Enable() )
	{
		// Hack
		gRSP.curTile = 0;
	}
	else
		gRSP.curTile = dwTile;
}

float HackZ2(float z)
{
	z = (z+9)/10;
	return z;
}

float HackZ(float z)
{
	return HackZ2(z);

	if( z < 0.1 && z >= 0 )
		z = (.1f+z)/2;
	else if( z < 0 )
		//return (10+z)/100;
		z = (expf(z)/20);
	return z;
}

void HackZ(std::vector<D3DXVECTOR3>& points)
{
	int size = points.size();
	for( int i=0; i<size; i++)
	{
		D3DXVECTOR3 &v = points[i];
		v.z = (float)HackZ(v.z);
	}
}

void HackZAll()
{
	for( uint32 i=0; i<gRSP.numVertices; i++)
	{
		g_vtxBuffer[i].z = HackZ(g_vtxBuffer[i].z);
	}
}


extern D3DXMATRIX reverseXY;
extern D3DXMATRIX reverseY;

void UpdateCombinedMatrix()
{
	if( gRSP.bMatrixIsUpdated )
	{
		gRSPworldProject = gRSP.modelviewMtxs[gRSP.modelViewMtxTop] * gRSP.projectionMtxs[gRSP.projectionMtxTop];
		gRSP.bMatrixIsUpdated = false;
		gRSP.bCombinedMatrixIsUpdated = true;
	}

	if( gRSP.bCombinedMatrixIsUpdated )
	{
		if( options.enableHackForGames == HACK_REVERSE_XY_COOR )
		{
			gRSPworldProject = gRSPworldProject * reverseXY;
			gRSPmodelViewTop = gRSPmodelViewTop * reverseXY;
		}
		if( options.enableHackForGames == HACK_REVERSE_Y_COOR )
		{
			gRSPworldProject = gRSPworldProject * reverseY;
			gRSPmodelViewTop = gRSPmodelViewTop * reverseY;
		}

		if( status.isSSEEnabled )
		{
			D3DXMatrixTranspose(&gRSPworldProjectTransported, &gRSPworldProject);
			D3DXMatrixTranspose(&gRSPmodelViewTopTranspose, &gRSPmodelViewTop);
		}

		gRSP.bCombinedMatrixIsUpdated = false;
	}

	//if( gRSP.bWorldMatrixIsUpdated || gRSP.bLightIsUpdated )
	//{
	//	// Update lights with transported world matrix
	//	for( unsigned int l=0; l<gRSPnumLights; l++)
	//	{
	//		D3DXVec3TransformCoord(&gRSPlights[l].td, &gRSPlights[l].od, &gRSPmodelViewTopTranspose);
	//		D3DXVec3Normalize(&gRSPlights[l].td,&gRSPlights[l].td);
	//	}

	//	gRSP.bWorldMatrixIsUpdated = false;
	//	gRSP.bLightIsUpdated = false;
	//}
}
#ifdef _OLDCLIPPER
//////////////////////////////////////////////////////////////////////////
/*
 *	Manual vertex clipper Rice 5.60
 */
//////////////////////////////////////////////////////////////////////////
typedef struct {
	double d;
	double x;
	double y;
} LineEuqationType;
LineEuqationType lines[3];
double EvaLine(LineEuqationType &li, double x, double y)
{
	return li.x*x+li.y*y+li.d;
}
inline D3DXVECTOR3 Split( D3DXVECTOR3 &a, D3DXVECTOR3 &b, LineEuqationType &line  )
{
	double aDot = (a.x*line.x + a.y*line.y);
	double bDot = (b.x*line.x + b.y*line.y);

	double scale = ( - line.d - aDot) / ( bDot - aDot );

	return a + ((b - a) * (float)scale );
}
// Clipping using the Sutherland-Hodgeman algorithm
bool ClipFor1LineXY( std::vector<D3DXVECTOR3> &in, std::vector<D3DXVECTOR3> &out, LineEuqationType &line )
{
	int insize = in.size();
	int thisInd=insize-1;
	int nextInd=0;

	double thisRes = EvaLine( line, in[thisInd].x, in[thisInd].y );
	double nextRes;

	out.clear();

	for( nextInd=0; nextInd<insize; nextInd++ )
	{
		nextRes = EvaLine( line, in[nextInd].x, in[nextInd].y );

		if( thisRes >= 0 )
		{
			// Add the point
			out.push_back(in[thisInd]);
		}

		if( ( thisRes < 0 && nextRes >= 0 ) || ( thisRes >= 0 && nextRes < 0 ) )
		{
			// Add the split point
			out.push_back( Split(in[thisInd], in[nextInd], line ));
		}

		thisInd = nextInd;
		thisRes = nextRes;
	}
	if( (int)out.size() >= insize )
	{
		return true;
	}
	return false;
}
// Clipping using the Sutherland-Hodgeman algorithm
bool ClipFor1LineZ( std::vector<D3DXVECTOR3> &in, std::vector<D3DXVECTOR3> &out, bool nearplane )
{
	const float nearz = 1e-4f;
	const float farz = 1-1e-4f;

	int insize = in.size();
	int thisInd=insize-1;
	int nextInd=0;

	bool thisRes = nearplane ? (in[thisInd].z >= nearz) : (in[thisInd].z <= farz) ;
	bool nextRes;

	out.clear();

	for( nextInd=0; nextInd<insize; nextInd++ )
	{
		nextRes = nearplane ? (in[nextInd].z >= nearz) : (in[nextInd].z <= farz) ;

		if( thisRes )
		{
			// Add the point
			out.push_back(in[thisInd]);
		}

		if( ( !thisRes && nextRes ) || ( thisRes && !nextRes ) )
		{
			// Add the split point
			D3DXVECTOR3 newvtx;
			D3DXVECTOR3 &v1 = in[thisInd];
			D3DXVECTOR3 &v2 = in[nextInd];

			newvtx.z = nearplane ? nearz : farz;

			float r = (v1.z - newvtx.z )/(v1.z-v2.z);
			if( r != r )
			{
				r = (v1.z - newvtx.z )/(v1.z-v2.z);
			}
			newvtx.x = v1.x - r*(v1.x-v2.x);
			newvtx.y = v1.y - r*(v1.y-v2.y);
			out.push_back( newvtx );
		}

		thisInd = nextInd;
		thisRes = nextRes;
	}
	if( (int)out.size() >= insize )
	{
		return true;
	}
	return false;
}
void Create1LineEq(LineEuqationType &l, TLITVERTEX &v1, TLITVERTEX &v2, TLITVERTEX &v3)
{
	// Line between (x1,y1) to (x2,y2)
	l.x = v2.y-v1.y;
	l.y = v1.x-v2.x;
	l.d = -(l.x*v2.x+(l.y)*v2.y);
	if( EvaLine(l,v3.x,v3.y)*v3.rhw<0 )
	{
		l.x = -l.x ;
		l.y = -l.y ;
		l.d = -l.d;
	}
}

void CopyVertexData(int oriidx, TLITVERTEX *oribuf, int dstidx, TLITVERTEX *dstbuf)
{
	memcpy(dstbuf+dstidx,oribuf+oriidx, sizeof(TLITVERTEX));
}

#define interp(a,b,r)	(((a)-(r)*(b))/	(1-(r)))
void SetVtx(TLITVERTEX &v, TLITVERTEX &v1, TLITVERTEX &v2, float r)
{
	v.x = interp(v1.x,v2.x,r);
	v.y = interp(v1.y,v2.y,r);
	//v.z = (v1.z-r*v2.z)/(1-r);
	v.dcSpecular = v2.dcSpecular; //fix me here
	v.r = (BYTE)(interp((int)v1.r,(int)v2.r,r));
	v.g = (BYTE)(interp((int)v1.g,(int)v2.g,r));
	v.b = (BYTE)(interp((int)v1.b,(int)v2.b,r));
	v.a = (BYTE)(interp((int)v1.a,(int)v2.a,r));

	for( int i=0; i<2; i++ )
	{
		v.tcord[i].u = interp(v1.tcord[i].u,v2.tcord[i].u,r);
		v.tcord[i].v = interp(v1.tcord[i].v,v2.tcord[i].v,r);
	}
}



void SwapVertexPos(int firstidx)
{
	TLITVERTEX &v1 = g_vtxBuffer[firstidx];
	TLITVERTEX &v2 = g_vtxBuffer[firstidx+1];
	TLITVERTEX &v3 = g_vtxBuffer[firstidx+2];

	if( v1.rhw >= v2.rhw && v1.rhw >= v3.rhw ) return;

	TLITVERTEX tempv;
	memcpy(&tempv,&g_vtxBuffer[firstidx], sizeof(TLITVERTEX));

	if( v2.rhw > v1.rhw && v2.rhw >= v3.rhw )
	{
		// v2 is the largest one
		memcpy(&g_vtxBuffer[firstidx],&g_vtxBuffer[firstidx+1], sizeof(TLITVERTEX));
		memcpy(&g_vtxBuffer[firstidx+1],&g_vtxBuffer[firstidx+2], sizeof(TLITVERTEX));
		memcpy(&g_vtxBuffer[firstidx+2],&tempv, sizeof(TLITVERTEX));
	}
	else
	{
		memcpy(&g_vtxBuffer[firstidx],&g_vtxBuffer[firstidx+2], sizeof(TLITVERTEX));
		memcpy(&g_vtxBuffer[firstidx+2],&g_vtxBuffer[firstidx+1], sizeof(TLITVERTEX));
		memcpy(&g_vtxBuffer[firstidx+1],&tempv, sizeof(TLITVERTEX));
	}
}
/************************************************************************/
/* Manually clipping vertexes                                           */
/* DirectX won't clip transformed vertex, unless the vertex is transfor-*/
/* med by DirectX, so we have to do vertex clipping ourself. Otherwise  */
/* this plugin works very bad on newest video cards, like ATI Radeons.  */
/************************************************************************/
bool Clip1TriangleForNegW(TLITVERTEX &v1, TLITVERTEX &v2, TLITVERTEX &v3, int &dstidx);
bool Clip1TriangleForZ(TLITVERTEX &v1, TLITVERTEX &v2, TLITVERTEX &v3, int &dstidx);

void ClipVertexes()
{
	// transverse the list of transformed vertex for each triangles
	// - If z values of all 3 vertexes of the triangle are greater than 1, then ignore this triangle
	// - If non z value of all 3 vertexes of the triangle is greater than 1, then this triangle is ok
	// - If one or two z values are greater than 1, then split this triangle to 2 triangles

	int dstidx = 0;

	for( uint32 i=0; i<gRSP.numVertices/3; i++)
	{
		int firstidx = i*3;
		TLITVERTEX &v1 = g_vtxBuffer[firstidx];
		TLITVERTEX &v2 = g_vtxBuffer[firstidx+1];
		TLITVERTEX &v3 = g_vtxBuffer[firstidx+2];

		if( v1.rhw < 0 && v2.rhw < 0 && v3.rhw < 0 )
			continue;	// Skip this triangle

		if( v1.rhw >= 0 && v2.rhw >= 0 && v3.rhw >= 0 )	
		//if( v1.rhw >= 0 && v2.rhw >= 0 && v3.rhw >= 0 &&
		//	v1.x>=0 && v1.x < windowSetting.uViWidth && v1.y>=0 && v1.y<windowSetting.uViHeight &&
		//	v2.x>=0 && v2.x < windowSetting.uViWidth && v2.y>=0 && v2.y<windowSetting.uViHeight &&
		//	v3.x>=0 && v3.x < windowSetting.uViWidth && v3.y>=0 && v3.y<windowSetting.uViHeight )	
		{
			if( v1.z < 0 || v2.z < 0 || v3.z < 0 )
			{
				if( !gRDP.otherMode.z_cmp )
				{
					// Do nothing about this triangle
					CopyVertexData(firstidx, g_vtxBuffer, dstidx++, g_clippedVtxBuffer);
					CopyVertexData(firstidx+1, g_vtxBuffer, dstidx++, g_clippedVtxBuffer);
					CopyVertexData(firstidx+2, g_vtxBuffer, dstidx++, g_clippedVtxBuffer);
					continue;
				}
				else if( v1.z < 0 && v2.z < 0 && v3.z < 0 )
					continue;
				else
				{
					// Clipping this triangle for z
#ifdef _DEBUG
					if( pauseAtNext && logTriangles ) 
					{
						TRACE1("Clip triangle %d against near plane", i);
						TRACE3("Old triangle z values: %f, %f, %f", v1.z, v2.z, v3.z);
					}
#endif
					Clip1TriangleForZ(v1, v2, v3, dstidx);
					continue;
				}
			}
			else if( v1.z > 1 || v2.z > 1 || v3.z > 1 )
			{
				if( v1.z > 1 && v2.z > 1 && v3.z > 1 )
					continue;
				else
				{
					// Clipping this triangle for z
#ifdef _DEBUG
					if( pauseAtNext && logTriangles ) 
					{
						TRACE1("Clip triangle %d against far plane", i);
						TRACE3("Old triangle z values: %f, %f, %f", v1.z, v2.z, v3.z);
					}
#endif
					Clip1TriangleForZ(v1, v2, v3, dstidx);
					continue;
				}
			}
			else
			{
				// Do nothing about this triangle
				CopyVertexData(firstidx, g_vtxBuffer, dstidx++, g_clippedVtxBuffer);
				CopyVertexData(firstidx+1, g_vtxBuffer, dstidx++, g_clippedVtxBuffer);
				CopyVertexData(firstidx+2, g_vtxBuffer, dstidx++, g_clippedVtxBuffer);
			}
			continue;
		}

#ifdef _DEBUG
		if( pauseAtNext && logTriangles ) 
		{
			TRACE1("Clip triangle %d for negative w", i);
			TRACE3("Old triangle w values: %f, %f, %f", v1.rhw, v2.rhw, v3.rhw);
		}
#endif
		SwapVertexPos(firstidx);

#ifdef _DEBUG
		if( pauseAtNext && logTriangles ) 
		{
			TRACE3("Old triangle w values: %f, %f, %f", v1.rhw, v2.rhw, v3.rhw);
		}
#endif

		Clip1TriangleForNegW(v1, v2, v3, dstidx);
	}

	g_clippedVtxCount = dstidx;

}

float inline interp3p(float a, float b, float c, double r1, double r2)
{
	return (float)((a)+(((b)+((c)-(b))*(r2))-(a))*(r1));
}

void Interp1PtZ(D3DXVECTOR3 &v, TLITVERTEX &v1, TLITVERTEX &v2, TLITVERTEX &v3)
{
	if( abs(v.x-v1.x)<1 && abs(v.y-v1.y) < 1 )
	{
		v.z = v1.z;
		return;
	}
	if( abs(v.x-v2.x)<1 && abs(v.y-v2.y) < 1 )
	{
		v.z = v2.z;
		return;
	}
	if( abs(v.x-v3.x)<1 && abs(v.y-v3.y) < 1 )
	{
		v.z = v3.z;
		return;
	}

	LineEuqationType line;
	Create1LineEq(line, v2, v3, v1);

	D3DXVECTOR3 tempv1(v1.x, v1.y, 0);
	double aDot = (v.x*line.x + v.y*line.y);
	double bDot = (v1.x*line.x + v1.y*line.y);

	double scale1 = ( - line.d - aDot) / ( bDot - aDot );

	D3DXVECTOR3 tempv;
	tempv = v + ((float)scale1 * (tempv1 - v));

	double s1 = (v.x-v1.x)/(tempv.x-v1.x);
	if( !_finite(s1) )
	{
		s1 = (v.y-v1.y)/(tempv.y-v1.y);
	}
	double s2 = (tempv.x-v2.x)/(v3.x-v2.x);
	if( !_finite(s2) )
	{
		s2 = (tempv.y-v2.y)/(v3.y-v2.y);
	}

	v.z = interp3p(v1.z,v2.z,v3.z,s1,s2);

#ifdef _DEBUG
	if( !_finite(v.z) )
	{
		TRACE0("Z value is #INF, check me");
	}

	if( abs(v.z) > 10 )
	{
		TRACE0("Large Z");
	}
#endif
}

bool Interp1Pt(D3DXVECTOR3 &v, TLITVERTEX &v1, TLITVERTEX &v2, TLITVERTEX &v3, TLITVERTEX &out)
{
	LineEuqationType line;
	Create1LineEq(line, v2, v3, v1);

	D3DXVECTOR3 tempv1(v1.x, v1.y, 0);

	double aDot = (v.x*line.x + v.y*line.y);
	double bDot = (v1.x*line.x + v1.y*line.y);

	double scale1 = ( - line.d - aDot) / ( bDot - aDot );

	D3DXVECTOR3 tempv;
	tempv = v + ((float)scale1 * (tempv1 - v));

	double s1 = (v.x-v1.x)/(tempv.x-v1.x);
	if( !_finite(s1) )
	{
		s1 = (v.y-v1.y)/(tempv.y-v1.y);
	}
	double s2 = (tempv.x-v2.x)/(v3.x-v2.x);

	if( !_finite(s2) )
	{
		s2 = (tempv.y-v2.y)/(v3.y-v2.y);
	}

	if( !_finite(s1) || !_finite(s2) )
	{
		memcpy(&out, &v3, sizeof(TLITVERTEX) );
		TRACE0("s1 or s2 is still infinite, it is a coplaner triangle");
		return false;
	}

	//out.x = interp3p(v1.x,v2.x,v3.x,s1,s2);
	out.x = v.x;
	out.y = v.y;
	//out.y = interp3p(v1.y,v2.y,v3.y,s1,s2);
	out.z = interp3p(v1.z,v2.z,v3.z,s1,s2);
	out.rhw = interp3p(v1.rhw,v2.rhw,v3.rhw,s1,s2);
#ifdef _DEBUG
	if( !_finite(out.z) || !_finite(out.rhw) )
	{
		TRACE0("Z or RHW value is #INF, check me");
	}
#endif


#ifdef _DEBUG
	if( out.rhw < 0 )	
	{
		DebuggerAppendMsg("Warning: rhw<0");
		DebuggerAppendMsg("nx=[%f %f %f %f];\n",v1.x, v2.x, v3.x, v1.x);
		DebuggerAppendMsg("ny=[%f %f %f %f];\n",v1.y, v2.y, v3.y, v1.y);
		DebuggerAppendMsg("nz=[%f %f %f %f];\n",v1.z, v2.z, v3.z, v1.z);
		DebuggerAppendMsg("newx=[%f %f %f];\n",v1.x, v.x, tempv.x);
		DebuggerAppendMsg("newy=[%f %f %f];\n",v1.y, v.y, tempv.y);
		DebuggerAppendMsg("newz=[%f %f %f];\n",v1.z, v.z, tempv.z);
	}
	if( out.z < 0 )	
		DebuggerAppendMsg("Warning: z<0");
	if( out.z > 1 )	
		DebuggerAppendMsg("Warning: z>1");
#endif



	out.dcSpecular = v2.dcSpecular; //fix me here
	if( gRDP.tnl.Fog )
	{
		float f1 = (v1.dcSpecular>>24)*v1.rhw;
		float f2 = (v2.dcSpecular>>24)*v2.rhw;
		float f3 = (v3.dcSpecular>>24)*v3.rhw;
		float f = interp3p(f1,f2,f3,s1,s2)/out.rhw;	
		if( f < 0 )	f = 0;
		if( f > 255 ) f = 255;
		DWORD fb = (BYTE)f;
		out.dcSpecular &= 0x00FFFFFF;
		out.dcSpecular |= (fb<<24);
	}

	float r = interp3p(v1.r*v1.rhw,v2.r*v2.rhw,v3.r*v3.rhw,s1,s2)/out.rhw;	
	if( r<0 )	r=0; 
	if( r>255 )	r=255;
	out.r = (BYTE)r;
	float g = interp3p(v1.g*v1.rhw,v2.g*v2.rhw,v3.g*v3.rhw,s1,s2)/out.rhw;
	if( g<0 )	g=0; 
	if( g>255 )	g=255;
	out.g = (BYTE)g;
	float b = interp3p(v1.b*v1.rhw,v2.b*v2.rhw,v3.b*v3.rhw,s1,s2)/out.rhw;
	if( b<0 )	b=0; 
	if( b>255 )	b=255;
	out.b = (BYTE)b;
	float a = interp3p(v1.a*v1.rhw,v2.a*v2.rhw,v3.a*v3.rhw,s1,s2)/out.rhw;
	if( a<0 )	a=0; 
	if( a>255 )	a=255;
	out.a = (BYTE)a;

	for( int i=0; i<2; i++ )
	{
		out.tcord[i].u = interp3p(v1.tcord[i].u*v1.rhw,v2.tcord[i].u*v2.rhw,v3.tcord[i].u*v3.rhw,s1,s2)/out.rhw;
		out.tcord[i].v = interp3p(v1.tcord[i].v*v1.rhw,v2.tcord[i].v*v2.rhw,v3.tcord[i].v*v3.rhw,s1,s2)/out.rhw;
	}

	if( out.rhw < 0 )	
	{
		return false;
	}
	else
		return true;
}

bool Clip1TriangleForNegW(TLITVERTEX &v1, TLITVERTEX &v2, TLITVERTEX &v3, int &dstidx)
{
#ifdef _DEBUG
	if( pauseAtNext && logTriangles ) 
	{
		TRACE0("To clip triangle for negative w:");
		TRACE4("Vtx 1: %f, %f, %f, %f", v1.x, v1.y, v1.z, 1/v1.rhw);
		TRACE4("Vtx 2: %f, %f, %f, %f", v2.x, v2.y, v2.z, 1/v2.rhw);
		TRACE4("Vtx 3: %f, %f, %f, %f", v3.x, v3.y, v3.z, 1/v3.rhw);
	}
#endif

	int lno = 0;
	if( v1.rhw >= 0 || v2.rhw >= 0 )	Create1LineEq(lines[lno++],v1,v2,v3);
	if( abs(EvaLine(lines[0],v3.x, v3.y) ) < 1 )	// v1, v2, v3 are linear
	{
		return false;
	}

	if( v1.rhw >= 0 || v3.rhw >= 0 )	Create1LineEq(lines[lno++],v1,v3,v2);
	if( v2.rhw >= 0 || v3.rhw >= 0 )	Create1LineEq(lines[lno++],v2,v3,v1);

	std::vector<D3DXVECTOR3> pts[2];

	D3DXVECTOR3 p;
	p.x = 0;
	p.y = 0;
	pts[0].push_back(p);
	p.x = windowSetting.uDisplayWidth;
	p.y = 0;
	pts[0].push_back(p);
	p.x = windowSetting.uDisplayWidth;
	p.y = windowSetting.uDisplayHeight;
	pts[0].push_back(p);
	p.x = 0;
	p.y = windowSetting.uDisplayHeight;
	pts[0].push_back(p);


	for( int j=0; j<lno; j++ )
	{
		ClipFor1LineXY(pts[j%2], pts[(j+1)%2], lines[j]);
		if( pts[(j+1)%2].size() < 3 )
			return false;
	}

	std::vector<D3DXVECTOR3> &ps = pts[lno%2];
	int size = ps.size();

	if( gRDP.otherMode.z_cmp )
	{
		for( int k=0; k<size; k++ )
		{
			Interp1PtZ(ps[k],v1,v2,v3);
		}

		ClipFor1LineZ(pts[lno%2], pts[(lno+1)%2], true);	// Clip for near plane
		if( pts[(lno+1)%2].size() < 3 )
			return false;
		//ClipFor1LineZ(pts[(lno+1)%2], pts[lno%2], false);	// clip for far plane
		//if( pts[lno%2].size() < 3 )
		//	return false;

		size = ps.size();
	}

	// now compute z and rhw values
	int i;
	TLITVERTEX  *newvertexes = new TLITVERTEX[size];

	for( i=0; i<size; i++ )
	{
		D3DXVECTOR3 &p = ps[i];
		if( abs(p.x-v1.x)<1 && abs(p.y-v1.y)<1 )
		{
			memcpy(&newvertexes[i], &v1, sizeof(TLITVERTEX) );
			continue;
		}
		else if( abs(p.x-v2.x)<1 && abs(p.y-v2.y)<1 )
		{
			memcpy(&newvertexes[i], &v2, sizeof(TLITVERTEX) );
			continue;
		}
		if( abs(p.x-v3.x)<1 && abs(p.y-v3.y)<1 )
		{
			memcpy(&newvertexes[i], &v3, sizeof(TLITVERTEX) );
			continue;
		}

		TLITVERTEX newvtx;
        if( Interp1Pt(ps[i],v1,v2,v3,newvtx) )
		{
			memcpy(&newvertexes[i], &newvtx, sizeof(TLITVERTEX) );
		}
		else
		{
			// negative w
			memcpy(&newvertexes[i], &newvtx, sizeof(TLITVERTEX) );
			//return false;	
		}
	}

#ifdef _DEBUG
	if( pauseAtNext && logTriangles ) 
	{
		TRACE0("Clipped x and y:");
		for( i=0; i<size; i++ )
		{
			DebuggerAppendMsg("Vtx %d: %f, %f", i, ps[i].x, ps[i].y);
		}
		TRACE0("Generated new vertexes after clipping for W:");
		for( i=0; i<size; i++ )
		{
			DebuggerAppendMsg("Vtx %d: %f, %f, %f, %f", i, newvertexes[i].x, newvertexes[i].y, newvertexes[i].z, 1/newvertexes[i].rhw);
		}
	}
#endif

	for( i=0; i<size-2; i++ )
	{
		CopyVertexData(0,   newvertexes, dstidx++, g_clippedVtxBuffer);
		CopyVertexData(i+1, newvertexes, dstidx++, g_clippedVtxBuffer);
		CopyVertexData(i+2, newvertexes, dstidx++, g_clippedVtxBuffer);
	}

	delete [] newvertexes;

	return true;
}


bool Clip1TriangleForZ(TLITVERTEX &v1, TLITVERTEX &v2, TLITVERTEX &v3, int &dstidx)
{
#ifdef _DEBUG
	if( pauseAtNext && logTriangles ) 
	{
		TRACE0("To clip triangle for z:");
		TRACE4("Vtx 1: %f, %f, %f, %f", v1.x, v1.y, v1.z, 1/v1.rhw);
		TRACE4("Vtx 2: %f, %f, %f, %f", v2.x, v2.y, v2.z, 1/v2.rhw);
		TRACE4("Vtx 3: %f, %f, %f, %f", v3.x, v3.y, v3.z, 1/v3.rhw);
	}
#endif

	std::vector<D3DXVECTOR3> pts[2];

	D3DXVECTOR3 p;
	p.x = v1.x;
	p.y = v1.y;
	p.z = v1.z;
	pts[0].push_back(p);
	p.x = v2.x;
	p.y = v2.y;
	p.z = v2.z;
	pts[0].push_back(p);
	p.x = v3.x;
	p.y = v3.y;
	p.z = v3.z;
	pts[0].push_back(p);


	ClipFor1LineZ(pts[0], pts[1], true);	// Clip for near plane
	if( pts[1].size() < 3 )
	return false;
	ClipFor1LineZ(pts[1], pts[0], false);	// clip for far plane
	if( pts[0].size() < 3 )
	return false;

	std::vector<D3DXVECTOR3> &ps = pts[0];
	int size = ps.size();

	// now compute x, y and rhw values
	int i;
	TLITVERTEX  *newvertexes = new TLITVERTEX[size];

	for( i=0; i<size; i++ )
	{
		D3DXVECTOR3 &p = ps[i];
		if( abs(p.x-v1.x)<1 && abs(p.y-v1.y)<1 )
		{
			memcpy(&newvertexes[i], &v1, sizeof(TLITVERTEX) );
			continue;
		}
		else if( abs(p.x-v2.x)<1 && abs(p.y-v2.y)<1 )
		{
			memcpy(&newvertexes[i], &v2, sizeof(TLITVERTEX) );
			continue;
		}
		if( abs(p.x-v3.x)<1 && abs(p.y-v3.y)<1 )
		{
			memcpy(&newvertexes[i], &v3, sizeof(TLITVERTEX) );
			continue;
		}

		TLITVERTEX newvtx;
		Interp1Pt(ps[i],v1,v2,v3,newvtx);
		memcpy(&newvertexes[i], &newvtx, sizeof(TLITVERTEX) );
	}

#ifdef _DEBUG
	if( pauseAtNext && logTriangles ) 
	{
		TRACE0("Clipped z:");
		for( i=0; i<size; i++ )
		{
			DebuggerAppendMsg("Vtx %d: %f, %f, %f", i, ps[i].x, ps[i].y, ps[i].z);
		}
		TRACE0("Generated new vertexes after clipping for z:");
		for( i=0; i<size; i++ )
		{
			DebuggerAppendMsg("Vtx %d: %f, %f, %f, %f", i, newvertexes[i].x, newvertexes[i].y, newvertexes[i].z, 1/newvertexes[i].rhw);
		}
	}
#endif

	for( i=0; i<size-2; i++ )
	{
		CopyVertexData(0,   newvertexes, dstidx++, g_clippedVtxBuffer);
		CopyVertexData(i+1, newvertexes, dstidx++, g_clippedVtxBuffer);
		CopyVertexData(i+2, newvertexes, dstidx++, g_clippedVtxBuffer);
	}

	delete [] newvertexes;

	return true;
}
#endif