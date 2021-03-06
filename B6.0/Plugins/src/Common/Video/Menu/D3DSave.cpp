#include "..\stdafx_video.h"
#include "D3DSave.h"

D3DSave g_d3dSave;

//freakdave
extern int TextureMode;
extern bool bEnableHDTV;

D3DSave::D3DSave(void)
{	
}

D3DSave::~D3DSave(void)
{
}

void D3DSave::PushRenderState()
{
	int i = 0;

	g_pd3dDevice->GetRenderState(D3DRS_ALPHABLENDENABLE, &m_renderState[i++]);
	g_pd3dDevice->GetRenderState(D3DRS_ALPHAFUNC, &m_renderState[i++]);
	g_pd3dDevice->GetRenderState(D3DRS_ALPHAREF, &m_renderState[i++]);
	g_pd3dDevice->GetRenderState(D3DRS_ALPHATESTENABLE, &m_renderState[i++]);
	g_pd3dDevice->GetRenderState(D3DRS_AMBIENT, &m_renderState[i++]);
	g_pd3dDevice->GetRenderState(D3DRS_AMBIENTMATERIALSOURCE, &m_renderState[i++]);
	g_pd3dDevice->GetRenderState(D3DRS_BACKAMBIENT, &m_renderState[i++]);
	g_pd3dDevice->GetRenderState(D3DRS_BACKAMBIENTMATERIALSOURCE, &m_renderState[i++]);
	g_pd3dDevice->GetRenderState(D3DRS_BACKDIFFUSEMATERIALSOURCE, &m_renderState[i++]);
	g_pd3dDevice->GetRenderState(D3DRS_BACKEMISSIVEMATERIALSOURCE, &m_renderState[i++]);
	g_pd3dDevice->GetRenderState(D3DRS_BACKFILLMODE, &m_renderState[i++]);
	g_pd3dDevice->GetRenderState(D3DRS_BACKSPECULARMATERIALSOURCE, &m_renderState[i++]);
	g_pd3dDevice->GetRenderState(D3DRS_BLENDCOLOR, &m_renderState[i++]);
	g_pd3dDevice->GetRenderState(D3DRS_BLENDOP, &m_renderState[i++]);
	g_pd3dDevice->GetRenderState(D3DRS_COLORVERTEX, &m_renderState[i++]);
	g_pd3dDevice->GetRenderState(D3DRS_COLORWRITEENABLE, &m_renderState[i++]);
	g_pd3dDevice->GetRenderState(D3DRS_CULLMODE, &m_renderState[i++]);
	g_pd3dDevice->GetRenderState(D3DRS_DEPTHCLIPCONTROL, &m_renderState[i++]);
	g_pd3dDevice->GetRenderState(D3DRS_DESTBLEND, &m_renderState[i++]);
	g_pd3dDevice->GetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, &m_renderState[i++]);
	g_pd3dDevice->GetRenderState(D3DRS_DITHERENABLE, &m_renderState[i++]);
	g_pd3dDevice->GetRenderState(D3DRS_DONOTCULLUNCOMPRESSED, &m_renderState[i++]);
	g_pd3dDevice->GetRenderState(D3DRS_DXT1NOISEENABLE, &m_renderState[i++]);
	g_pd3dDevice->GetRenderState(D3DRS_EDGEANTIALIAS, &m_renderState[i++]);
	g_pd3dDevice->GetRenderState(D3DRS_EMISSIVEMATERIALSOURCE, &m_renderState[i++]);
	g_pd3dDevice->GetRenderState(D3DRS_FILLMODE, &m_renderState[i++]);
	g_pd3dDevice->GetRenderState(D3DRS_FOGCOLOR, &m_renderState[i++]);
	g_pd3dDevice->GetRenderState(D3DRS_FOGDENSITY, &m_renderState[i++]);
	g_pd3dDevice->GetRenderState(D3DRS_FOGENABLE, &m_renderState[i++]);
	g_pd3dDevice->GetRenderState(D3DRS_FOGEND, &m_renderState[i++]);
	g_pd3dDevice->GetRenderState(D3DRS_FOGSTART, &m_renderState[i++]);
	g_pd3dDevice->GetRenderState(D3DRS_FOGTABLEMODE, &m_renderState[i++]);
	g_pd3dDevice->GetRenderState(D3DRS_FRONTFACE, &m_renderState[i++]);
	g_pd3dDevice->GetRenderState(D3DRS_LIGHTING, &m_renderState[i++]);
	g_pd3dDevice->GetRenderState(D3DRS_LINEWIDTH, &m_renderState[i++]);
	g_pd3dDevice->GetRenderState(D3DRS_LOCALVIEWER, &m_renderState[i++]);
	g_pd3dDevice->GetRenderState(D3DRS_LOGICOP, &m_renderState[i++]);
	g_pd3dDevice->GetRenderState(D3DRS_MULTISAMPLEANTIALIAS, &m_renderState[i++]);
	g_pd3dDevice->GetRenderState(D3DRS_MULTISAMPLEMASK, &m_renderState[i++]);
	g_pd3dDevice->GetRenderState(D3DRS_MULTISAMPLEMODE, &m_renderState[i++]);
	g_pd3dDevice->GetRenderState(D3DRS_MULTISAMPLERENDERTARGETMODE, &m_renderState[i++]);
	g_pd3dDevice->GetRenderState(D3DRS_NORMALIZENORMALS, &m_renderState[i++]);
	g_pd3dDevice->GetRenderState(D3DRS_OCCLUSIONCULLENABLE, &m_renderState[i++]);
	g_pd3dDevice->GetRenderState(D3DRS_PATCHEDGESTYLE, &m_renderState[i++]);
	g_pd3dDevice->GetRenderState(D3DRS_PATCHSEGMENTS, &m_renderState[i++]);
	g_pd3dDevice->GetRenderState(D3DRS_POINTOFFSETENABLE, &m_renderState[i++]);
	g_pd3dDevice->GetRenderState(D3DRS_POINTSCALEENABLE, &m_renderState[i++]);
	g_pd3dDevice->GetRenderState(D3DRS_POINTSCALE_A, &m_renderState[i++]);
	g_pd3dDevice->GetRenderState(D3DRS_POINTSCALE_B, &m_renderState[i++]);
	g_pd3dDevice->GetRenderState(D3DRS_POINTSCALE_C, &m_renderState[i++]);
	g_pd3dDevice->GetRenderState(D3DRS_POINTSIZE, &m_renderState[i++]);
	g_pd3dDevice->GetRenderState(D3DRS_POINTSIZE_MAX, &m_renderState[i++]);
	g_pd3dDevice->GetRenderState(D3DRS_POINTSIZE_MIN, &m_renderState[i++]);
	g_pd3dDevice->GetRenderState(D3DRS_POINTSPRITEENABLE, &m_renderState[i++]);
	g_pd3dDevice->GetRenderState(D3DRS_POLYGONOFFSETZOFFSET, &m_renderState[i++]);
	g_pd3dDevice->GetRenderState(D3DRS_POLYGONOFFSETZSLOPESCALE, &m_renderState[i++]);
	g_pd3dDevice->GetRenderState(D3DRS_PRESENTATIONINTERVAL, &m_renderState[i++]);
	g_pd3dDevice->GetRenderState(D3DRS_RANGEFOGENABLE, &m_renderState[i++]);
	g_pd3dDevice->GetRenderState(D3DRS_ROPZCMPALWAYSREAD, &m_renderState[i++]);
	g_pd3dDevice->GetRenderState(D3DRS_ROPZREAD, &m_renderState[i++]);
	g_pd3dDevice->GetRenderState(D3DRS_SAMPLEALPHA, &m_renderState[i++]);
	g_pd3dDevice->GetRenderState(D3DRS_SHADEMODE, &m_renderState[i++]);
	g_pd3dDevice->GetRenderState(D3DRS_SHADOWFUNC, &m_renderState[i++]);
	g_pd3dDevice->GetRenderState(D3DRS_SOLIDOFFSETENABLE, &m_renderState[i++]);
	g_pd3dDevice->GetRenderState(D3DRS_SPECULARENABLE, &m_renderState[i++]);
	g_pd3dDevice->GetRenderState(D3DRS_SPECULARMATERIALSOURCE, &m_renderState[i++]);
	g_pd3dDevice->GetRenderState(D3DRS_SRCBLEND, &m_renderState[i++]);
	g_pd3dDevice->GetRenderState(D3DRS_STENCILCULLENABLE, &m_renderState[i++]);
	g_pd3dDevice->GetRenderState(D3DRS_STENCILENABLE, &m_renderState[i++]);
	g_pd3dDevice->GetRenderState(D3DRS_STENCILFAIL, &m_renderState[i++]);
	g_pd3dDevice->GetRenderState(D3DRS_STENCILFUNC, &m_renderState[i++]);
	g_pd3dDevice->GetRenderState(D3DRS_STENCILMASK, &m_renderState[i++]);
	g_pd3dDevice->GetRenderState(D3DRS_STENCILPASS, &m_renderState[i++]);
	g_pd3dDevice->GetRenderState(D3DRS_STENCILREF, &m_renderState[i++]);
	g_pd3dDevice->GetRenderState(D3DRS_STENCILWRITEMASK, &m_renderState[i++]);
	g_pd3dDevice->GetRenderState(D3DRS_STENCILZFAIL, &m_renderState[i++]);
	g_pd3dDevice->GetRenderState(D3DRS_STIPPLEENABLE, &m_renderState[i++]);
	g_pd3dDevice->GetRenderState(D3DRS_SWAPFILTER, &m_renderState[i++]);
	g_pd3dDevice->GetRenderState(D3DRS_SWATHWIDTH, &m_renderState[i++]);
	g_pd3dDevice->GetRenderState(D3DRS_TEXTUREFACTOR, &m_renderState[i++]);
	g_pd3dDevice->GetRenderState(D3DRS_TWOSIDEDLIGHTING, &m_renderState[i++]);
	g_pd3dDevice->GetRenderState(D3DRS_VERTEXBLEND, &m_renderState[i++]);
	g_pd3dDevice->GetRenderState(D3DRS_WIREFRAMEOFFSETENABLE, &m_renderState[i++]);
	g_pd3dDevice->GetRenderState(D3DRS_WRAP0, &m_renderState[i++]);
	g_pd3dDevice->GetRenderState(D3DRS_WRAP1, &m_renderState[i++]);
	g_pd3dDevice->GetRenderState(D3DRS_WRAP2, &m_renderState[i++]);
	g_pd3dDevice->GetRenderState(D3DRS_WRAP3, &m_renderState[i++]);
	g_pd3dDevice->GetRenderState(D3DRS_YUVENABLE, &m_renderState[i++]);
	g_pd3dDevice->GetRenderState(D3DRS_ZBIAS, &m_renderState[i++]);
	g_pd3dDevice->GetRenderState(D3DRS_ZENABLE, &m_renderState[i++]);
	g_pd3dDevice->GetRenderState(D3DRS_ZFUNC, &m_renderState[i++]);
	g_pd3dDevice->GetRenderState(D3DRS_ZWRITEENABLE, &m_renderState[i++]);

	int j = 0;

	g_pd3dDevice->GetTextureStageState(0, D3DTSS_ADDRESSU, &m_stageState[j++]);
	g_pd3dDevice->GetTextureStageState(0, D3DTSS_ADDRESSV, &m_stageState[j++]);
	g_pd3dDevice->GetTextureStageState(0, D3DTSS_ADDRESSW, &m_stageState[j++]);
	g_pd3dDevice->GetTextureStageState(0, D3DTSS_ALPHAARG0, &m_stageState[j++]);
	g_pd3dDevice->GetTextureStageState(0, D3DTSS_ALPHAARG1, &m_stageState[j++]);
	g_pd3dDevice->GetTextureStageState(0, D3DTSS_ALPHAARG2, &m_stageState[j++]);
	g_pd3dDevice->GetTextureStageState(0, D3DTSS_ALPHAKILL, &m_stageState[j++]);
	g_pd3dDevice->GetTextureStageState(0, D3DTSS_ALPHAOP, &m_stageState[j++]);
	g_pd3dDevice->GetTextureStageState(0, D3DTSS_BORDERCOLOR, &m_stageState[j++]);
	g_pd3dDevice->GetTextureStageState(0, D3DTSS_BUMPENVLOFFSET, &m_stageState[j++]);
	g_pd3dDevice->GetTextureStageState(0, D3DTSS_BUMPENVLSCALE, &m_stageState[j++]);
	g_pd3dDevice->GetTextureStageState(0, D3DTSS_BUMPENVMAT00, &m_stageState[j++]);
	g_pd3dDevice->GetTextureStageState(0, D3DTSS_BUMPENVMAT01, &m_stageState[j++]);
	g_pd3dDevice->GetTextureStageState(0, D3DTSS_BUMPENVMAT10, &m_stageState[j++]);
	g_pd3dDevice->GetTextureStageState(0, D3DTSS_BUMPENVMAT11, &m_stageState[j++]);
	g_pd3dDevice->GetTextureStageState(0, D3DTSS_COLORARG0, &m_stageState[j++]);
	g_pd3dDevice->GetTextureStageState(0, D3DTSS_COLORARG1, &m_stageState[j++]);
	g_pd3dDevice->GetTextureStageState(0, D3DTSS_COLORARG2, &m_stageState[j++]);
	g_pd3dDevice->GetTextureStageState(0, D3DTSS_COLORKEYCOLOR, &m_stageState[j++]);
	g_pd3dDevice->GetTextureStageState(0, D3DTSS_COLORKEYOP, &m_stageState[j++]);
	g_pd3dDevice->GetTextureStageState(0, D3DTSS_COLOROP, &m_stageState[j++]);
	g_pd3dDevice->GetTextureStageState(0, D3DTSS_COLORSIGN, &m_stageState[j++]);
	g_pd3dDevice->GetTextureStageState(0, D3DTSS_MAGFILTER, &m_stageState[j++]);
	g_pd3dDevice->GetTextureStageState(0, D3DTSS_MAXANISOTROPY, &m_stageState[j++]);
	g_pd3dDevice->GetTextureStageState(0, D3DTSS_MAXMIPLEVEL, &m_stageState[j++]);
	g_pd3dDevice->GetTextureStageState(0, D3DTSS_MINFILTER, &m_stageState[j++]);
	g_pd3dDevice->GetTextureStageState(0, D3DTSS_MIPFILTER, &m_stageState[j++]);
	g_pd3dDevice->GetTextureStageState(0, D3DTSS_MIPMAPLODBIAS, &m_stageState[j++]);
	g_pd3dDevice->GetTextureStageState(0, D3DTSS_RESULTARG, &m_stageState[j++]);
	g_pd3dDevice->GetTextureStageState(0, D3DTSS_TEXCOORDINDEX, &m_stageState[j++]);
	g_pd3dDevice->GetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, &m_stageState[j++]);

	g_pd3dDevice->GetTextureStageState(1, D3DTSS_ADDRESSU, &m_stageState[j++]);
	g_pd3dDevice->GetTextureStageState(1, D3DTSS_ADDRESSV, &m_stageState[j++]);
	g_pd3dDevice->GetTextureStageState(1, D3DTSS_ADDRESSW, &m_stageState[j++]);
	g_pd3dDevice->GetTextureStageState(1, D3DTSS_ALPHAARG0, &m_stageState[j++]);
	g_pd3dDevice->GetTextureStageState(1, D3DTSS_ALPHAARG1, &m_stageState[j++]);
	g_pd3dDevice->GetTextureStageState(1, D3DTSS_ALPHAARG2, &m_stageState[j++]);
	g_pd3dDevice->GetTextureStageState(1, D3DTSS_ALPHAKILL, &m_stageState[j++]);
	g_pd3dDevice->GetTextureStageState(1, D3DTSS_ALPHAOP, &m_stageState[j++]);
	g_pd3dDevice->GetTextureStageState(1, D3DTSS_BORDERCOLOR, &m_stageState[j++]);
	g_pd3dDevice->GetTextureStageState(1, D3DTSS_BUMPENVLOFFSET, &m_stageState[j++]);
	g_pd3dDevice->GetTextureStageState(1, D3DTSS_BUMPENVLSCALE, &m_stageState[j++]);
	g_pd3dDevice->GetTextureStageState(1, D3DTSS_BUMPENVMAT00, &m_stageState[j++]);
	g_pd3dDevice->GetTextureStageState(1, D3DTSS_BUMPENVMAT01, &m_stageState[j++]);
	g_pd3dDevice->GetTextureStageState(1, D3DTSS_BUMPENVMAT10, &m_stageState[j++]);
	g_pd3dDevice->GetTextureStageState(1, D3DTSS_BUMPENVMAT11, &m_stageState[j++]);
	g_pd3dDevice->GetTextureStageState(1, D3DTSS_COLORARG0, &m_stageState[j++]);
	g_pd3dDevice->GetTextureStageState(1, D3DTSS_COLORARG1, &m_stageState[j++]);
	g_pd3dDevice->GetTextureStageState(1, D3DTSS_COLORARG2, &m_stageState[j++]);
	g_pd3dDevice->GetTextureStageState(1, D3DTSS_COLORKEYCOLOR, &m_stageState[j++]);
	g_pd3dDevice->GetTextureStageState(1, D3DTSS_COLORKEYOP, &m_stageState[j++]);
	g_pd3dDevice->GetTextureStageState(1, D3DTSS_COLOROP, &m_stageState[j++]);
	g_pd3dDevice->GetTextureStageState(1, D3DTSS_COLORSIGN, &m_stageState[j++]);
	g_pd3dDevice->GetTextureStageState(1, D3DTSS_MAGFILTER, &m_stageState[j++]);
	g_pd3dDevice->GetTextureStageState(1, D3DTSS_MAXANISOTROPY, &m_stageState[j++]);
	g_pd3dDevice->GetTextureStageState(1, D3DTSS_MAXMIPLEVEL, &m_stageState[j++]);
	g_pd3dDevice->GetTextureStageState(1, D3DTSS_MINFILTER, &m_stageState[j++]);
	g_pd3dDevice->GetTextureStageState(1, D3DTSS_MIPFILTER, &m_stageState[j++]);
	g_pd3dDevice->GetTextureStageState(1, D3DTSS_MIPMAPLODBIAS, &m_stageState[j++]);
	g_pd3dDevice->GetTextureStageState(1, D3DTSS_RESULTARG, &m_stageState[j++]);
	g_pd3dDevice->GetTextureStageState(1, D3DTSS_TEXCOORDINDEX, &m_stageState[j++]);
	g_pd3dDevice->GetTextureStageState(1, D3DTSS_TEXTURETRANSFORMFLAGS, &m_stageState[j++]);

	g_pd3dDevice->GetTextureStageState(2, D3DTSS_ADDRESSU, &m_stageState[j++]);
	g_pd3dDevice->GetTextureStageState(2, D3DTSS_ADDRESSV, &m_stageState[j++]);
	g_pd3dDevice->GetTextureStageState(2, D3DTSS_ADDRESSW, &m_stageState[j++]);
	g_pd3dDevice->GetTextureStageState(2, D3DTSS_ALPHAARG0, &m_stageState[j++]);
	g_pd3dDevice->GetTextureStageState(2, D3DTSS_ALPHAARG1, &m_stageState[j++]);
	g_pd3dDevice->GetTextureStageState(2, D3DTSS_ALPHAARG2, &m_stageState[j++]);
	g_pd3dDevice->GetTextureStageState(2, D3DTSS_ALPHAKILL, &m_stageState[j++]);
	g_pd3dDevice->GetTextureStageState(2, D3DTSS_ALPHAOP, &m_stageState[j++]);
	g_pd3dDevice->GetTextureStageState(2, D3DTSS_BORDERCOLOR, &m_stageState[j++]);
	g_pd3dDevice->GetTextureStageState(2, D3DTSS_BUMPENVLOFFSET, &m_stageState[j++]);
	g_pd3dDevice->GetTextureStageState(2, D3DTSS_BUMPENVLSCALE, &m_stageState[j++]);
	g_pd3dDevice->GetTextureStageState(2, D3DTSS_BUMPENVMAT00, &m_stageState[j++]);
	g_pd3dDevice->GetTextureStageState(2, D3DTSS_BUMPENVMAT01, &m_stageState[j++]);
	g_pd3dDevice->GetTextureStageState(2, D3DTSS_BUMPENVMAT10, &m_stageState[j++]);
	g_pd3dDevice->GetTextureStageState(2, D3DTSS_BUMPENVMAT11, &m_stageState[j++]);
	g_pd3dDevice->GetTextureStageState(2, D3DTSS_COLORARG0, &m_stageState[j++]);
	g_pd3dDevice->GetTextureStageState(2, D3DTSS_COLORARG1, &m_stageState[j++]);
	g_pd3dDevice->GetTextureStageState(2, D3DTSS_COLORARG2, &m_stageState[j++]);
	g_pd3dDevice->GetTextureStageState(2, D3DTSS_COLORKEYCOLOR, &m_stageState[j++]);
	g_pd3dDevice->GetTextureStageState(2, D3DTSS_COLORKEYOP, &m_stageState[j++]);
	g_pd3dDevice->GetTextureStageState(2, D3DTSS_COLOROP, &m_stageState[j++]);
	g_pd3dDevice->GetTextureStageState(2, D3DTSS_COLORSIGN, &m_stageState[j++]);
	g_pd3dDevice->GetTextureStageState(2, D3DTSS_MAGFILTER, &m_stageState[j++]);
	g_pd3dDevice->GetTextureStageState(2, D3DTSS_MAXANISOTROPY, &m_stageState[j++]);
	g_pd3dDevice->GetTextureStageState(2, D3DTSS_MAXMIPLEVEL, &m_stageState[j++]);
	g_pd3dDevice->GetTextureStageState(2, D3DTSS_MINFILTER, &m_stageState[j++]);
	g_pd3dDevice->GetTextureStageState(2, D3DTSS_MIPFILTER, &m_stageState[j++]);
	g_pd3dDevice->GetTextureStageState(2, D3DTSS_MIPMAPLODBIAS, &m_stageState[j++]);
	g_pd3dDevice->GetTextureStageState(2, D3DTSS_RESULTARG, &m_stageState[j++]);
	g_pd3dDevice->GetTextureStageState(2, D3DTSS_TEXCOORDINDEX, &m_stageState[j++]);
	g_pd3dDevice->GetTextureStageState(2, D3DTSS_TEXTURETRANSFORMFLAGS, &m_stageState[j++]);

	g_pd3dDevice->GetTextureStageState(3, D3DTSS_ADDRESSU, &m_stageState[j++]);
	g_pd3dDevice->GetTextureStageState(3, D3DTSS_ADDRESSV, &m_stageState[j++]);
	g_pd3dDevice->GetTextureStageState(3, D3DTSS_ADDRESSW, &m_stageState[j++]);
	g_pd3dDevice->GetTextureStageState(3, D3DTSS_ALPHAARG0, &m_stageState[j++]);
	g_pd3dDevice->GetTextureStageState(3, D3DTSS_ALPHAARG1, &m_stageState[j++]);
	g_pd3dDevice->GetTextureStageState(3, D3DTSS_ALPHAARG2, &m_stageState[j++]);
	g_pd3dDevice->GetTextureStageState(3, D3DTSS_ALPHAKILL, &m_stageState[j++]);
	g_pd3dDevice->GetTextureStageState(3, D3DTSS_ALPHAOP, &m_stageState[j++]);
	g_pd3dDevice->GetTextureStageState(3, D3DTSS_BORDERCOLOR, &m_stageState[j++]);
	g_pd3dDevice->GetTextureStageState(3, D3DTSS_BUMPENVLOFFSET, &m_stageState[j++]);
	g_pd3dDevice->GetTextureStageState(3, D3DTSS_BUMPENVLSCALE, &m_stageState[j++]);
	g_pd3dDevice->GetTextureStageState(3, D3DTSS_BUMPENVMAT00, &m_stageState[j++]);
	g_pd3dDevice->GetTextureStageState(3, D3DTSS_BUMPENVMAT01, &m_stageState[j++]);
	g_pd3dDevice->GetTextureStageState(3, D3DTSS_BUMPENVMAT10, &m_stageState[j++]);
	g_pd3dDevice->GetTextureStageState(3, D3DTSS_BUMPENVMAT11, &m_stageState[j++]);
	g_pd3dDevice->GetTextureStageState(3, D3DTSS_COLORARG0, &m_stageState[j++]);
	g_pd3dDevice->GetTextureStageState(3, D3DTSS_COLORARG1, &m_stageState[j++]);
	g_pd3dDevice->GetTextureStageState(3, D3DTSS_COLORARG2, &m_stageState[j++]);
	g_pd3dDevice->GetTextureStageState(3, D3DTSS_COLORKEYCOLOR, &m_stageState[j++]);
	g_pd3dDevice->GetTextureStageState(3, D3DTSS_COLORKEYOP, &m_stageState[j++]);
	g_pd3dDevice->GetTextureStageState(3, D3DTSS_COLOROP, &m_stageState[j++]);
	g_pd3dDevice->GetTextureStageState(3, D3DTSS_COLORSIGN, &m_stageState[j++]);
	g_pd3dDevice->GetTextureStageState(3, D3DTSS_MAGFILTER, &m_stageState[j++]);
	g_pd3dDevice->GetTextureStageState(3, D3DTSS_MAXANISOTROPY, &m_stageState[j++]);
	g_pd3dDevice->GetTextureStageState(3, D3DTSS_MAXMIPLEVEL, &m_stageState[j++]);
	g_pd3dDevice->GetTextureStageState(3, D3DTSS_MINFILTER, &m_stageState[j++]);
	g_pd3dDevice->GetTextureStageState(3, D3DTSS_MIPFILTER, &m_stageState[j++]);
	g_pd3dDevice->GetTextureStageState(3, D3DTSS_MIPMAPLODBIAS, &m_stageState[j++]);
	g_pd3dDevice->GetTextureStageState(3, D3DTSS_RESULTARG, &m_stageState[j++]);
	g_pd3dDevice->GetTextureStageState(3, D3DTSS_TEXCOORDINDEX, &m_stageState[j++]);
	g_pd3dDevice->GetTextureStageState(3, D3DTSS_TEXTURETRANSFORMFLAGS, &m_stageState[j++]);

	int k = 0;

    g_pd3dDevice->GetTransform(D3DTS_VIEW, &m_transformState[k++]);
    g_pd3dDevice->GetTransform(D3DTS_PROJECTION, &m_transformState[k++]);
    g_pd3dDevice->GetTransform(D3DTS_TEXTURE0, &m_transformState[k++]);
    g_pd3dDevice->GetTransform(D3DTS_TEXTURE1, &m_transformState[k++]);
    g_pd3dDevice->GetTransform(D3DTS_TEXTURE2, &m_transformState[k++]);
    g_pd3dDevice->GetTransform(D3DTS_TEXTURE3, &m_transformState[k++]);
    g_pd3dDevice->GetTransform(D3DTS_WORLD, &m_transformState[k++]);
    g_pd3dDevice->GetTransform(D3DTS_WORLD1, &m_transformState[k++]);
    g_pd3dDevice->GetTransform(D3DTS_WORLD2, &m_transformState[k++]);
    g_pd3dDevice->GetTransform(D3DTS_WORLD3, &m_transformState[k++]);

	g_pd3dDevice->SetViewport(&m_viewport);

	LoadDefaultRenderState();
}

void D3DSave::PopRenderState()
{
	int i = 0;

	g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, m_renderState[i++]);
	g_pd3dDevice->SetRenderState(D3DRS_ALPHAFUNC, m_renderState[i++]);
	g_pd3dDevice->SetRenderState(D3DRS_ALPHAREF, m_renderState[i++]);
	g_pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, m_renderState[i++]);
	g_pd3dDevice->SetRenderState(D3DRS_AMBIENT, m_renderState[i++]);
	g_pd3dDevice->SetRenderState(D3DRS_AMBIENTMATERIALSOURCE, m_renderState[i++]);
	g_pd3dDevice->SetRenderState(D3DRS_BACKAMBIENT, m_renderState[i++]);
	g_pd3dDevice->SetRenderState(D3DRS_BACKAMBIENTMATERIALSOURCE, m_renderState[i++]);
	g_pd3dDevice->SetRenderState(D3DRS_BACKDIFFUSEMATERIALSOURCE, m_renderState[i++]);
	g_pd3dDevice->SetRenderState(D3DRS_BACKEMISSIVEMATERIALSOURCE, m_renderState[i++]);
	g_pd3dDevice->SetRenderState(D3DRS_BACKFILLMODE, m_renderState[i++]);
	g_pd3dDevice->SetRenderState(D3DRS_BACKSPECULARMATERIALSOURCE, m_renderState[i++]);
	g_pd3dDevice->SetRenderState(D3DRS_BLENDCOLOR, m_renderState[i++]);
	g_pd3dDevice->SetRenderState(D3DRS_BLENDOP, m_renderState[i++]);
	g_pd3dDevice->SetRenderState(D3DRS_COLORVERTEX, m_renderState[i++]);
	g_pd3dDevice->SetRenderState(D3DRS_COLORWRITEENABLE, m_renderState[i++]);
	g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, m_renderState[i++]);
	g_pd3dDevice->SetRenderState(D3DRS_DEPTHCLIPCONTROL, m_renderState[i++]);
	g_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, m_renderState[i++]);
	g_pd3dDevice->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, m_renderState[i++]);
	g_pd3dDevice->SetRenderState(D3DRS_DITHERENABLE, m_renderState[i++]);
	g_pd3dDevice->SetRenderState(D3DRS_DONOTCULLUNCOMPRESSED, m_renderState[i++]);
	g_pd3dDevice->SetRenderState(D3DRS_DXT1NOISEENABLE, m_renderState[i++]);
	g_pd3dDevice->SetRenderState(D3DRS_EDGEANTIALIAS, m_renderState[i++]);
	g_pd3dDevice->SetRenderState(D3DRS_EMISSIVEMATERIALSOURCE, m_renderState[i++]);
	g_pd3dDevice->SetRenderState(D3DRS_FILLMODE, m_renderState[i++]);
	g_pd3dDevice->SetRenderState(D3DRS_FOGCOLOR, m_renderState[i++]);
	g_pd3dDevice->SetRenderState(D3DRS_FOGDENSITY, m_renderState[i++]);
	g_pd3dDevice->SetRenderState(D3DRS_FOGENABLE, m_renderState[i++]);
	g_pd3dDevice->SetRenderState(D3DRS_FOGEND, m_renderState[i++]);
	g_pd3dDevice->SetRenderState(D3DRS_FOGSTART, m_renderState[i++]);
	g_pd3dDevice->SetRenderState(D3DRS_FOGTABLEMODE, m_renderState[i++]);
	g_pd3dDevice->SetRenderState(D3DRS_FRONTFACE, m_renderState[i++]);
	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, m_renderState[i++]);
	g_pd3dDevice->SetRenderState(D3DRS_LINEWIDTH, m_renderState[i++]);
	g_pd3dDevice->SetRenderState(D3DRS_LOCALVIEWER, m_renderState[i++]);
	g_pd3dDevice->SetRenderState(D3DRS_LOGICOP, m_renderState[i++]);
	g_pd3dDevice->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, m_renderState[i++]);
	g_pd3dDevice->SetRenderState(D3DRS_MULTISAMPLEMASK, m_renderState[i++]);
	g_pd3dDevice->SetRenderState(D3DRS_MULTISAMPLEMODE, m_renderState[i++]);
	g_pd3dDevice->SetRenderState(D3DRS_MULTISAMPLERENDERTARGETMODE, m_renderState[i++]);
	g_pd3dDevice->SetRenderState(D3DRS_NORMALIZENORMALS, m_renderState[i++]);
	g_pd3dDevice->SetRenderState(D3DRS_OCCLUSIONCULLENABLE, m_renderState[i++]);
	g_pd3dDevice->SetRenderState(D3DRS_PATCHEDGESTYLE, m_renderState[i++]);
	g_pd3dDevice->SetRenderState(D3DRS_PATCHSEGMENTS, m_renderState[i++]);
	g_pd3dDevice->SetRenderState(D3DRS_POINTOFFSETENABLE, m_renderState[i++]);
	g_pd3dDevice->SetRenderState(D3DRS_POINTSCALEENABLE, m_renderState[i++]);
	g_pd3dDevice->SetRenderState(D3DRS_POINTSCALE_A, m_renderState[i++]);
	g_pd3dDevice->SetRenderState(D3DRS_POINTSCALE_B, m_renderState[i++]);
	g_pd3dDevice->SetRenderState(D3DRS_POINTSCALE_C, m_renderState[i++]);
	g_pd3dDevice->SetRenderState(D3DRS_POINTSIZE, m_renderState[i++]);
	g_pd3dDevice->SetRenderState(D3DRS_POINTSIZE_MAX, m_renderState[i++]);
	g_pd3dDevice->SetRenderState(D3DRS_POINTSIZE_MIN, m_renderState[i++]);
	g_pd3dDevice->SetRenderState(D3DRS_POINTSPRITEENABLE, m_renderState[i++]);
	g_pd3dDevice->SetRenderState(D3DRS_POLYGONOFFSETZOFFSET, m_renderState[i++]);
	g_pd3dDevice->SetRenderState(D3DRS_POLYGONOFFSETZSLOPESCALE, m_renderState[i++]);
	g_pd3dDevice->SetRenderState(D3DRS_PRESENTATIONINTERVAL, m_renderState[i++]);
	g_pd3dDevice->SetRenderState(D3DRS_RANGEFOGENABLE, m_renderState[i++]);
	g_pd3dDevice->SetRenderState(D3DRS_ROPZCMPALWAYSREAD, m_renderState[i++]);
	g_pd3dDevice->SetRenderState(D3DRS_ROPZREAD, m_renderState[i++]);
	g_pd3dDevice->SetRenderState(D3DRS_SAMPLEALPHA, m_renderState[i++]);
	g_pd3dDevice->SetRenderState(D3DRS_SHADEMODE, m_renderState[i++]);
	g_pd3dDevice->SetRenderState(D3DRS_SHADOWFUNC, m_renderState[i++]);
	g_pd3dDevice->SetRenderState(D3DRS_SOLIDOFFSETENABLE, m_renderState[i++]);
	g_pd3dDevice->SetRenderState(D3DRS_SPECULARENABLE, m_renderState[i++]);
	g_pd3dDevice->SetRenderState(D3DRS_SPECULARMATERIALSOURCE, m_renderState[i++]);
	g_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, m_renderState[i++]);
	g_pd3dDevice->SetRenderState(D3DRS_STENCILCULLENABLE, m_renderState[i++]);
	g_pd3dDevice->SetRenderState(D3DRS_STENCILENABLE, m_renderState[i++]);
	g_pd3dDevice->SetRenderState(D3DRS_STENCILFAIL, m_renderState[i++]);
	g_pd3dDevice->SetRenderState(D3DRS_STENCILFUNC, m_renderState[i++]);
	g_pd3dDevice->SetRenderState(D3DRS_STENCILMASK, m_renderState[i++]);
	g_pd3dDevice->SetRenderState(D3DRS_STENCILPASS, m_renderState[i++]);
	g_pd3dDevice->SetRenderState(D3DRS_STENCILREF, m_renderState[i++]);
	g_pd3dDevice->SetRenderState(D3DRS_STENCILWRITEMASK, m_renderState[i++]);
	g_pd3dDevice->SetRenderState(D3DRS_STENCILZFAIL, m_renderState[i++]);
	g_pd3dDevice->SetRenderState(D3DRS_STIPPLEENABLE, m_renderState[i++]);
	g_pd3dDevice->SetRenderState(D3DRS_SWAPFILTER, m_renderState[i++]);
	g_pd3dDevice->SetRenderState(D3DRS_SWATHWIDTH, m_renderState[i++]);
	g_pd3dDevice->SetRenderState(D3DRS_TEXTUREFACTOR, m_renderState[i++]);
	g_pd3dDevice->SetRenderState(D3DRS_TWOSIDEDLIGHTING, m_renderState[i++]);
	g_pd3dDevice->SetRenderState(D3DRS_VERTEXBLEND, m_renderState[i++]);
	g_pd3dDevice->SetRenderState(D3DRS_WIREFRAMEOFFSETENABLE, m_renderState[i++]);
	g_pd3dDevice->SetRenderState(D3DRS_WRAP0, m_renderState[i++]);
	g_pd3dDevice->SetRenderState(D3DRS_WRAP1, m_renderState[i++]);
	g_pd3dDevice->SetRenderState(D3DRS_WRAP2, m_renderState[i++]);
	g_pd3dDevice->SetRenderState(D3DRS_WRAP3, m_renderState[i++]);
	g_pd3dDevice->SetRenderState(D3DRS_YUVENABLE, m_renderState[i++]);
	g_pd3dDevice->SetRenderState(D3DRS_ZBIAS, m_renderState[i++]);
	g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, m_renderState[i++]);
	g_pd3dDevice->SetRenderState(D3DRS_ZFUNC, m_renderState[i++]);
	g_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, m_renderState[i++]);

	int j = 0;

	g_pd3dDevice->SetTextureStageState(0, D3DTSS_ADDRESSU, m_stageState[j++]);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_ADDRESSV, m_stageState[j++]);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_ADDRESSW, m_stageState[j++]);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG0, m_stageState[j++]);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, m_stageState[j++]);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, m_stageState[j++]);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAKILL, m_stageState[j++]);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, m_stageState[j++]);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_BORDERCOLOR, m_stageState[j++]);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_BUMPENVLOFFSET, m_stageState[j++]);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_BUMPENVLSCALE, m_stageState[j++]);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_BUMPENVMAT00, m_stageState[j++]);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_BUMPENVMAT01, m_stageState[j++]);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_BUMPENVMAT10, m_stageState[j++]);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_BUMPENVMAT11, m_stageState[j++]);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG0, m_stageState[j++]);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, m_stageState[j++]);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, m_stageState[j++]);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORKEYCOLOR, m_stageState[j++]);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORKEYOP, m_stageState[j++]);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, m_stageState[j++]);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORSIGN, m_stageState[j++]);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_MAGFILTER, m_stageState[j++]);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_MAXANISOTROPY, m_stageState[j++]);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_MAXMIPLEVEL, m_stageState[j++]);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_MINFILTER, m_stageState[j++]);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_MIPFILTER, m_stageState[j++]);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_MIPMAPLODBIAS, m_stageState[j++]);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_RESULTARG, m_stageState[j++]);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, m_stageState[j++]);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, m_stageState[j++]);

	g_pd3dDevice->SetTextureStageState(1, D3DTSS_ADDRESSU, m_stageState[j++]);
	g_pd3dDevice->SetTextureStageState(1, D3DTSS_ADDRESSV, m_stageState[j++]);
	g_pd3dDevice->SetTextureStageState(1, D3DTSS_ADDRESSW, m_stageState[j++]);
	g_pd3dDevice->SetTextureStageState(1, D3DTSS_ALPHAARG0, m_stageState[j++]);
	g_pd3dDevice->SetTextureStageState(1, D3DTSS_ALPHAARG1, m_stageState[j++]);
	g_pd3dDevice->SetTextureStageState(1, D3DTSS_ALPHAARG2, m_stageState[j++]);
	g_pd3dDevice->SetTextureStageState(1, D3DTSS_ALPHAKILL, m_stageState[j++]);
	g_pd3dDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, m_stageState[j++]);
	g_pd3dDevice->SetTextureStageState(1, D3DTSS_BORDERCOLOR, m_stageState[j++]);
	g_pd3dDevice->SetTextureStageState(1, D3DTSS_BUMPENVLOFFSET, m_stageState[j++]);
	g_pd3dDevice->SetTextureStageState(1, D3DTSS_BUMPENVLSCALE, m_stageState[j++]);
	g_pd3dDevice->SetTextureStageState(1, D3DTSS_BUMPENVMAT00, m_stageState[j++]);
	g_pd3dDevice->SetTextureStageState(1, D3DTSS_BUMPENVMAT01, m_stageState[j++]);
	g_pd3dDevice->SetTextureStageState(1, D3DTSS_BUMPENVMAT10, m_stageState[j++]);
	g_pd3dDevice->SetTextureStageState(1, D3DTSS_BUMPENVMAT11, m_stageState[j++]);
	g_pd3dDevice->SetTextureStageState(1, D3DTSS_COLORARG0, m_stageState[j++]);
	g_pd3dDevice->SetTextureStageState(1, D3DTSS_COLORARG1, m_stageState[j++]);
	g_pd3dDevice->SetTextureStageState(1, D3DTSS_COLORARG2, m_stageState[j++]);
	g_pd3dDevice->SetTextureStageState(1, D3DTSS_COLORKEYCOLOR, m_stageState[j++]);
	g_pd3dDevice->SetTextureStageState(1, D3DTSS_COLORKEYOP, m_stageState[j++]);
	g_pd3dDevice->SetTextureStageState(1, D3DTSS_COLOROP, m_stageState[j++]);
	g_pd3dDevice->SetTextureStageState(1, D3DTSS_COLORSIGN, m_stageState[j++]);
	g_pd3dDevice->SetTextureStageState(1, D3DTSS_MAGFILTER, m_stageState[j++]);
	g_pd3dDevice->SetTextureStageState(1, D3DTSS_MAXANISOTROPY, m_stageState[j++]);
	g_pd3dDevice->SetTextureStageState(1, D3DTSS_MAXMIPLEVEL, m_stageState[j++]);
	g_pd3dDevice->SetTextureStageState(1, D3DTSS_MINFILTER, m_stageState[j++]);
	g_pd3dDevice->SetTextureStageState(1, D3DTSS_MIPFILTER, m_stageState[j++]);
	g_pd3dDevice->SetTextureStageState(1, D3DTSS_MIPMAPLODBIAS, m_stageState[j++]);
	g_pd3dDevice->SetTextureStageState(1, D3DTSS_RESULTARG, m_stageState[j++]);
	g_pd3dDevice->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, m_stageState[j++]);
	g_pd3dDevice->SetTextureStageState(1, D3DTSS_TEXTURETRANSFORMFLAGS, m_stageState[j++]);

	g_pd3dDevice->SetTextureStageState(2, D3DTSS_ADDRESSU, m_stageState[j++]);
	g_pd3dDevice->SetTextureStageState(2, D3DTSS_ADDRESSV, m_stageState[j++]);
	g_pd3dDevice->SetTextureStageState(2, D3DTSS_ADDRESSW, m_stageState[j++]);
	g_pd3dDevice->SetTextureStageState(2, D3DTSS_ALPHAARG0, m_stageState[j++]);
	g_pd3dDevice->SetTextureStageState(2, D3DTSS_ALPHAARG1, m_stageState[j++]);
	g_pd3dDevice->SetTextureStageState(2, D3DTSS_ALPHAARG2, m_stageState[j++]);
	g_pd3dDevice->SetTextureStageState(2, D3DTSS_ALPHAKILL, m_stageState[j++]);
	g_pd3dDevice->SetTextureStageState(2, D3DTSS_ALPHAOP, m_stageState[j++]);
	g_pd3dDevice->SetTextureStageState(2, D3DTSS_BORDERCOLOR, m_stageState[j++]);
	g_pd3dDevice->SetTextureStageState(2, D3DTSS_BUMPENVLOFFSET, m_stageState[j++]);
	g_pd3dDevice->SetTextureStageState(2, D3DTSS_BUMPENVLSCALE, m_stageState[j++]);
	g_pd3dDevice->SetTextureStageState(2, D3DTSS_BUMPENVMAT00, m_stageState[j++]);
	g_pd3dDevice->SetTextureStageState(2, D3DTSS_BUMPENVMAT01, m_stageState[j++]);
	g_pd3dDevice->SetTextureStageState(2, D3DTSS_BUMPENVMAT10, m_stageState[j++]);
	g_pd3dDevice->SetTextureStageState(2, D3DTSS_BUMPENVMAT11, m_stageState[j++]);
	g_pd3dDevice->SetTextureStageState(2, D3DTSS_COLORARG0, m_stageState[j++]);
	g_pd3dDevice->SetTextureStageState(2, D3DTSS_COLORARG1, m_stageState[j++]);
	g_pd3dDevice->SetTextureStageState(2, D3DTSS_COLORARG2, m_stageState[j++]);
	g_pd3dDevice->SetTextureStageState(2, D3DTSS_COLORKEYCOLOR, m_stageState[j++]);
	g_pd3dDevice->SetTextureStageState(2, D3DTSS_COLORKEYOP, m_stageState[j++]);
	g_pd3dDevice->SetTextureStageState(2, D3DTSS_COLOROP, m_stageState[j++]);
	g_pd3dDevice->SetTextureStageState(2, D3DTSS_COLORSIGN, m_stageState[j++]);
	g_pd3dDevice->SetTextureStageState(2, D3DTSS_MAGFILTER, m_stageState[j++]);
	g_pd3dDevice->SetTextureStageState(2, D3DTSS_MAXANISOTROPY, m_stageState[j++]);
	g_pd3dDevice->SetTextureStageState(2, D3DTSS_MAXMIPLEVEL, m_stageState[j++]);
	g_pd3dDevice->SetTextureStageState(2, D3DTSS_MINFILTER, m_stageState[j++]);
	g_pd3dDevice->SetTextureStageState(2, D3DTSS_MIPFILTER, m_stageState[j++]);
	g_pd3dDevice->SetTextureStageState(2, D3DTSS_MIPMAPLODBIAS, m_stageState[j++]);
	g_pd3dDevice->SetTextureStageState(2, D3DTSS_RESULTARG, m_stageState[j++]);
	g_pd3dDevice->SetTextureStageState(2, D3DTSS_TEXCOORDINDEX, m_stageState[j++]);
	g_pd3dDevice->SetTextureStageState(2, D3DTSS_TEXTURETRANSFORMFLAGS, m_stageState[j++]);

	g_pd3dDevice->SetTextureStageState(3, D3DTSS_ADDRESSU, m_stageState[j++]);
	g_pd3dDevice->SetTextureStageState(3, D3DTSS_ADDRESSV, m_stageState[j++]);
	g_pd3dDevice->SetTextureStageState(3, D3DTSS_ADDRESSW, m_stageState[j++]);
	g_pd3dDevice->SetTextureStageState(3, D3DTSS_ALPHAARG0, m_stageState[j++]);
	g_pd3dDevice->SetTextureStageState(3, D3DTSS_ALPHAARG1, m_stageState[j++]);
	g_pd3dDevice->SetTextureStageState(3, D3DTSS_ALPHAARG2, m_stageState[j++]);
	g_pd3dDevice->SetTextureStageState(3, D3DTSS_ALPHAKILL, m_stageState[j++]);
	g_pd3dDevice->SetTextureStageState(3, D3DTSS_ALPHAOP, m_stageState[j++]);
	g_pd3dDevice->SetTextureStageState(3, D3DTSS_BORDERCOLOR, m_stageState[j++]);
	g_pd3dDevice->SetTextureStageState(3, D3DTSS_BUMPENVLOFFSET, m_stageState[j++]);
	g_pd3dDevice->SetTextureStageState(3, D3DTSS_BUMPENVLSCALE, m_stageState[j++]);
	g_pd3dDevice->SetTextureStageState(3, D3DTSS_BUMPENVMAT00, m_stageState[j++]);
	g_pd3dDevice->SetTextureStageState(3, D3DTSS_BUMPENVMAT01, m_stageState[j++]);
	g_pd3dDevice->SetTextureStageState(3, D3DTSS_BUMPENVMAT10, m_stageState[j++]);
	g_pd3dDevice->SetTextureStageState(3, D3DTSS_BUMPENVMAT11, m_stageState[j++]);
	g_pd3dDevice->SetTextureStageState(3, D3DTSS_COLORARG0, m_stageState[j++]);
	g_pd3dDevice->SetTextureStageState(3, D3DTSS_COLORARG1, m_stageState[j++]);
	g_pd3dDevice->SetTextureStageState(3, D3DTSS_COLORARG2, m_stageState[j++]);
	g_pd3dDevice->SetTextureStageState(3, D3DTSS_COLORKEYCOLOR, m_stageState[j++]);
	g_pd3dDevice->SetTextureStageState(3, D3DTSS_COLORKEYOP, m_stageState[j++]);
	g_pd3dDevice->SetTextureStageState(3, D3DTSS_COLOROP, m_stageState[j++]);
	g_pd3dDevice->SetTextureStageState(3, D3DTSS_COLORSIGN, m_stageState[j++]);
	g_pd3dDevice->SetTextureStageState(3, D3DTSS_MAGFILTER, m_stageState[j++]);
	g_pd3dDevice->SetTextureStageState(3, D3DTSS_MAXANISOTROPY, m_stageState[j++]);
	g_pd3dDevice->SetTextureStageState(3, D3DTSS_MAXMIPLEVEL, m_stageState[j++]);
	g_pd3dDevice->SetTextureStageState(3, D3DTSS_MINFILTER, m_stageState[j++]);
	g_pd3dDevice->SetTextureStageState(3, D3DTSS_MIPFILTER, m_stageState[j++]);
	g_pd3dDevice->SetTextureStageState(3, D3DTSS_MIPMAPLODBIAS, m_stageState[j++]);
	g_pd3dDevice->SetTextureStageState(3, D3DTSS_RESULTARG, m_stageState[j++]);
	g_pd3dDevice->SetTextureStageState(3, D3DTSS_TEXCOORDINDEX, m_stageState[j++]);
	g_pd3dDevice->SetTextureStageState(3, D3DTSS_TEXTURETRANSFORMFLAGS, m_stageState[j++]);

	int k = 0;

    g_pd3dDevice->SetTransform(D3DTS_VIEW, &m_transformState[k++]);
    g_pd3dDevice->SetTransform(D3DTS_PROJECTION, &m_transformState[k++]);
    g_pd3dDevice->SetTransform(D3DTS_TEXTURE0, &m_transformState[k++]);
    g_pd3dDevice->SetTransform(D3DTS_TEXTURE1, &m_transformState[k++]);
    g_pd3dDevice->SetTransform(D3DTS_TEXTURE2, &m_transformState[k++]);
    g_pd3dDevice->SetTransform(D3DTS_TEXTURE3, &m_transformState[k++]);
    g_pd3dDevice->SetTransform(D3DTS_WORLD, &m_transformState[k++]);
    g_pd3dDevice->SetTransform(D3DTS_WORLD1, &m_transformState[k++]);
    g_pd3dDevice->SetTransform(D3DTS_WORLD2, &m_transformState[k++]);
    g_pd3dDevice->SetTransform(D3DTS_WORLD3, &m_transformState[k++]);

	g_pd3dDevice->SetViewport(&m_viewport);
}

void D3DSave::SaveRenderState()
{
	/*
	HANDLE hStateFile;
	char FileName[255];
	DWORD dwWritten;
	int n;

	sprintf(FileName, "Z:\\%08X-%08X-%02X.tmp", *((DWORD *)(RomHeader + 0x10)), *((DWORD *)(RomHeader + 0x14)), *((BYTE *)(RomHeader + 0x3D)));

	hStateFile = CreateFile(FileName,GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ,NULL,OPEN_ALWAYS,
			FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS, NULL);

	SetFilePointer(hStateFile,0,NULL,FILE_BEGIN);
	for (n=0 ; n<100 ; n++)
	{
		WriteFile( hStateFile,&m_renderState[n],sizeof(m_renderState[n]),&dwWritten,NULL);
	}
	for (n=0 ; n<150 ; n++)
	{
		WriteFile( hStateFile,&m_stageState[n],sizeof(m_stageState[n]),&dwWritten,NULL);
	}
	for (n=0 ; n<15 ; n++)
	{
		WriteFile( hStateFile,&m_transformState[n],sizeof(m_transformState[n]),&dwWritten,NULL);
	}

	CloseHandle(hStateFile);
	*/
}

void D3DSave::LoadRenderState()
{
	/*
	HANDLE hStateFile;
	char FileName[255];
	DWORD dwRead;
	int n;
	

	sprintf(FileName, "Z:\\%08X-%08X-%02X.tmp", *((DWORD *)(RomHeader + 0x10)), *((DWORD *)(RomHeader + 0x14)), *((BYTE *)(RomHeader + 0x3D)));

	hSaveFile = CreateFile(FileName,GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ,NULL,OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS, NULL);

	SetFilePointer(hSaveFile,0,NULL,FILE_BEGIN);
	for (n=0 ; n<100 ; n++)
	{
		ReadFile( hStateFile,&m_renderState[n],sizeof(m_renderState[n]),&dwRead,NULL);
	}
	for (n=0 ; n<150 ; n++)
	{
		ReadFile( hStateFile,&m_stageState[n],sizeof(m_stageState[n]),&dwRead,NULL);
	}
	for (n=0 ; n<15 ; n++)
	{
		ReadFile( hStateFile,&m_transformState[n],sizeof(m_transformState[n]),&dwRead,NULL);
	}

	CloseHandle(hStateFile);
	*/
}

void D3DSave::LoadDefaultRenderState()
{
	g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
	g_pd3dDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);
	g_pd3dDevice->SetRenderState(D3DRS_ALPHAREF, 0);
	g_pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, false);
	g_pd3dDevice->SetRenderState(D3DRS_AMBIENT, 0);
	g_pd3dDevice->SetRenderState(D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_MATERIAL);
	g_pd3dDevice->SetRenderState(D3DRS_BACKAMBIENT, 0);
	g_pd3dDevice->SetRenderState(D3DRS_BACKAMBIENTMATERIALSOURCE, D3DMCS_MATERIAL);
	g_pd3dDevice->SetRenderState(D3DRS_BACKDIFFUSEMATERIALSOURCE, D3DMCS_COLOR1);
	g_pd3dDevice->SetRenderState(D3DRS_BACKEMISSIVEMATERIALSOURCE, D3DMCS_MATERIAL);
	g_pd3dDevice->SetRenderState(D3DRS_BACKFILLMODE, D3DFILL_SOLID);
	g_pd3dDevice->SetRenderState(D3DRS_BACKSPECULARMATERIALSOURCE, D3DMCS_COLOR2);
	g_pd3dDevice->SetRenderState(D3DRS_BLENDCOLOR, 0);
	g_pd3dDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	g_pd3dDevice->SetRenderState(D3DRS_COLORVERTEX, true);
	g_pd3dDevice->SetRenderState(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_ALL);
	g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	g_pd3dDevice->SetRenderState(D3DRS_DEPTHCLIPCONTROL, D3DDCC_CULLPRIMITIVE);
	g_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);
	g_pd3dDevice->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_COLOR1);
	g_pd3dDevice->SetRenderState(D3DRS_DITHERENABLE, false);
	g_pd3dDevice->SetRenderState(D3DRS_DONOTCULLUNCOMPRESSED, false);
	g_pd3dDevice->SetRenderState(D3DRS_DXT1NOISEENABLE, true);
	g_pd3dDevice->SetRenderState(D3DRS_EDGEANTIALIAS, false);
	g_pd3dDevice->SetRenderState(D3DRS_EMISSIVEMATERIALSOURCE, D3DMCS_MATERIAL);
	g_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	g_pd3dDevice->SetRenderState(D3DRS_FOGCOLOR, 0);
	g_pd3dDevice->SetRenderState(D3DRS_FOGDENSITY, 1);
	g_pd3dDevice->SetRenderState(D3DRS_FOGENABLE, false);
	g_pd3dDevice->SetRenderState(D3DRS_FOGEND, 1);
	g_pd3dDevice->SetRenderState(D3DRS_FOGSTART, 0);
	g_pd3dDevice->SetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_NONE);
	g_pd3dDevice->SetRenderState(D3DRS_FRONTFACE, D3DFRONT_CW);
	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, true);
	g_pd3dDevice->SetRenderState(D3DRS_LINEWIDTH, 1);
	g_pd3dDevice->SetRenderState(D3DRS_LOCALVIEWER, true);
	g_pd3dDevice->SetRenderState(D3DRS_LOGICOP, D3DLOGICOP_NONE);
	g_pd3dDevice->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, true);
	g_pd3dDevice->SetRenderState(D3DRS_MULTISAMPLEMASK, 0xffffffff);
	g_pd3dDevice->SetRenderState(D3DRS_MULTISAMPLEMODE, D3DMULTISAMPLEMODE_1X);
	g_pd3dDevice->SetRenderState(D3DRS_MULTISAMPLERENDERTARGETMODE, D3DMULTISAMPLEMODE_1X);
	g_pd3dDevice->SetRenderState(D3DRS_NORMALIZENORMALS, false);
	g_pd3dDevice->SetRenderState(D3DRS_OCCLUSIONCULLENABLE, true);
	g_pd3dDevice->SetRenderState(D3DRS_PATCHEDGESTYLE, D3DPATCHEDGE_DISCRETE);
	g_pd3dDevice->SetRenderState(D3DRS_PATCHSEGMENTS, 1065353216);
	g_pd3dDevice->SetRenderState(D3DRS_POINTOFFSETENABLE, false);
	g_pd3dDevice->SetRenderState(D3DRS_POINTSCALEENABLE, false);
	g_pd3dDevice->SetRenderState(D3DRS_POINTSCALE_A, 1);
	g_pd3dDevice->SetRenderState(D3DRS_POINTSCALE_B, 0);
	g_pd3dDevice->SetRenderState(D3DRS_POINTSCALE_C, 0);
	g_pd3dDevice->SetRenderState(D3DRS_POINTSIZE, 1);
	g_pd3dDevice->SetRenderState(D3DRS_POINTSIZE_MAX, 64);
	g_pd3dDevice->SetRenderState(D3DRS_POINTSIZE_MIN, 0);
	g_pd3dDevice->SetRenderState(D3DRS_POINTSPRITEENABLE, false);
	g_pd3dDevice->SetRenderState(D3DRS_POLYGONOFFSETZOFFSET, 0);
	g_pd3dDevice->SetRenderState(D3DRS_POLYGONOFFSETZSLOPESCALE, 0);
	g_pd3dDevice->SetRenderState(D3DRS_PRESENTATIONINTERVAL, D3DPRESENT_INTERVAL_DEFAULT | D3DPRESENT_INTERVAL_ONE | D3DPRESENT_INTERVAL_IMMEDIATE);
	g_pd3dDevice->SetRenderState(D3DRS_RANGEFOGENABLE, false);
	g_pd3dDevice->SetRenderState(D3DRS_ROPZCMPALWAYSREAD, false);
	g_pd3dDevice->SetRenderState(D3DRS_ROPZREAD, false);
	g_pd3dDevice->SetRenderState(D3DRS_SAMPLEALPHA, 0);
	g_pd3dDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
	g_pd3dDevice->SetRenderState(D3DRS_SHADOWFUNC, D3DCMP_NEVER);
	g_pd3dDevice->SetRenderState(D3DRS_SOLIDOFFSETENABLE, false);
	g_pd3dDevice->SetRenderState(D3DRS_SPECULARENABLE, false);
	g_pd3dDevice->SetRenderState(D3DRS_SPECULARMATERIALSOURCE, D3DMCS_COLOR2);
	g_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
	g_pd3dDevice->SetRenderState(D3DRS_STENCILCULLENABLE, true);
	g_pd3dDevice->SetRenderState(D3DRS_STENCILENABLE, false);
	g_pd3dDevice->SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP);
	g_pd3dDevice->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_ALWAYS);
	g_pd3dDevice->SetRenderState(D3DRS_STENCILMASK, 0xffffffff);
	g_pd3dDevice->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_KEEP);
	g_pd3dDevice->SetRenderState(D3DRS_STENCILREF, 0);
	g_pd3dDevice->SetRenderState(D3DRS_STENCILWRITEMASK, 0xffffffff);
	g_pd3dDevice->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP);
	g_pd3dDevice->SetRenderState(D3DRS_STIPPLEENABLE, false);
	g_pd3dDevice->SetRenderState(D3DRS_SWAPFILTER, D3DTEXF_LINEAR);
	g_pd3dDevice->SetRenderState(D3DRS_SWATHWIDTH, D3DSWATH_128);
	g_pd3dDevice->SetRenderState(D3DRS_TEXTUREFACTOR, 0xffffffff);
	g_pd3dDevice->SetRenderState(D3DRS_TWOSIDEDLIGHTING, false);
	g_pd3dDevice->SetRenderState(D3DRS_VERTEXBLEND, D3DVBF_DISABLE);
	g_pd3dDevice->SetRenderState(D3DRS_WIREFRAMEOFFSETENABLE, false);
	g_pd3dDevice->SetRenderState(D3DRS_WRAP0, 0);
	g_pd3dDevice->SetRenderState(D3DRS_WRAP1, 0);
	g_pd3dDevice->SetRenderState(D3DRS_WRAP2, 0);
	g_pd3dDevice->SetRenderState(D3DRS_WRAP3, 0);
	g_pd3dDevice->SetRenderState(D3DRS_YUVENABLE, false);
	g_pd3dDevice->SetRenderState(D3DRS_ZBIAS, 0);
	g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
	g_pd3dDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
	g_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, true);

	g_pd3dDevice->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_ADDRESSW, D3DTADDRESS_WRAP);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG0, D3DTA_CURRENT);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_CURRENT);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAKILL, D3DTALPHAKILL_DISABLE);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_BORDERCOLOR, 0);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_BUMPENVLOFFSET, 0);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_BUMPENVLSCALE, 0);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_BUMPENVMAT00, 0);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_BUMPENVMAT01, 0);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_BUMPENVMAT10, 0);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_BUMPENVMAT11, 0);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG0, D3DTA_CURRENT);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CURRENT);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORKEYCOLOR, 0);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORKEYOP, D3DTCOLORKEYOP_DISABLE);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORSIGN, 0);
/*freakdave
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_MAXANISOTROPY, 1);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_MAXMIPLEVEL, 0);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
	*/
	//g_pd3dDevice->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_MAGFILTER, TextureMode);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_MAXANISOTROPY, 1);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_MAXMIPLEVEL, 0);
	//g_pd3dDevice->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_MINFILTER, TextureMode);

	g_pd3dDevice->SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTEXF_NONE);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_MIPMAPLODBIAS, 0);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_RESULTARG, D3DTA_CURRENT);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU | 0);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);

	g_pd3dDevice->SetTextureStageState(1, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP);
	g_pd3dDevice->SetTextureStageState(1, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP);
	g_pd3dDevice->SetTextureStageState(1, D3DTSS_ADDRESSW, D3DTADDRESS_WRAP);
	g_pd3dDevice->SetTextureStageState(1, D3DTSS_ALPHAARG0, D3DTA_CURRENT);
	g_pd3dDevice->SetTextureStageState(1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	g_pd3dDevice->SetTextureStageState(1, D3DTSS_ALPHAARG2, D3DTA_CURRENT);
	g_pd3dDevice->SetTextureStageState(1, D3DTSS_ALPHAKILL, D3DTALPHAKILL_DISABLE);
	g_pd3dDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
	g_pd3dDevice->SetTextureStageState(1, D3DTSS_BORDERCOLOR, 0);
	g_pd3dDevice->SetTextureStageState(1, D3DTSS_BUMPENVLOFFSET, 0);
	g_pd3dDevice->SetTextureStageState(1, D3DTSS_BUMPENVLSCALE, 0);
	g_pd3dDevice->SetTextureStageState(1, D3DTSS_BUMPENVMAT00, 0);
	g_pd3dDevice->SetTextureStageState(1, D3DTSS_BUMPENVMAT01, 0);
	g_pd3dDevice->SetTextureStageState(1, D3DTSS_BUMPENVMAT10, 0);
	g_pd3dDevice->SetTextureStageState(1, D3DTSS_BUMPENVMAT11, 0);
	g_pd3dDevice->SetTextureStageState(1, D3DTSS_COLORARG0, D3DTA_CURRENT);
	g_pd3dDevice->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	g_pd3dDevice->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_CURRENT);
	g_pd3dDevice->SetTextureStageState(1, D3DTSS_COLORKEYCOLOR, 0);
	g_pd3dDevice->SetTextureStageState(1, D3DTSS_COLORKEYOP, D3DTCOLORKEYOP_DISABLE);
	g_pd3dDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
	g_pd3dDevice->SetTextureStageState(1, D3DTSS_COLORSIGN, 0);
	g_pd3dDevice->SetTextureStageState(1, D3DTSS_MAGFILTER, D3DTEXF_POINT);
	g_pd3dDevice->SetTextureStageState(1, D3DTSS_MAXANISOTROPY, 1);
	g_pd3dDevice->SetTextureStageState(1, D3DTSS_MAXMIPLEVEL, 0);
	g_pd3dDevice->SetTextureStageState(1, D3DTSS_MINFILTER, D3DTEXF_POINT);
	g_pd3dDevice->SetTextureStageState(1, D3DTSS_MIPFILTER, D3DTEXF_NONE);
	g_pd3dDevice->SetTextureStageState(1, D3DTSS_MIPMAPLODBIAS, 0);
	g_pd3dDevice->SetTextureStageState(1, D3DTSS_RESULTARG, D3DTA_CURRENT);
	g_pd3dDevice->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU | 1);
	g_pd3dDevice->SetTextureStageState(1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);

	g_pd3dDevice->SetTextureStageState(2, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP);
	g_pd3dDevice->SetTextureStageState(2, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP);
	g_pd3dDevice->SetTextureStageState(2, D3DTSS_ADDRESSW, D3DTADDRESS_WRAP);
	g_pd3dDevice->SetTextureStageState(2, D3DTSS_ALPHAARG0, D3DTA_CURRENT);
	g_pd3dDevice->SetTextureStageState(2, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	g_pd3dDevice->SetTextureStageState(2, D3DTSS_ALPHAARG2, D3DTA_CURRENT);
	g_pd3dDevice->SetTextureStageState(2, D3DTSS_ALPHAKILL, D3DTALPHAKILL_DISABLE);
	g_pd3dDevice->SetTextureStageState(2, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
	g_pd3dDevice->SetTextureStageState(2, D3DTSS_BORDERCOLOR, 0);
	g_pd3dDevice->SetTextureStageState(2, D3DTSS_BUMPENVLOFFSET, 0);
	g_pd3dDevice->SetTextureStageState(2, D3DTSS_BUMPENVLSCALE, 0);
	g_pd3dDevice->SetTextureStageState(2, D3DTSS_BUMPENVMAT00, 0);
	g_pd3dDevice->SetTextureStageState(2, D3DTSS_BUMPENVMAT01, 0);
	g_pd3dDevice->SetTextureStageState(2, D3DTSS_BUMPENVMAT10, 0);
	g_pd3dDevice->SetTextureStageState(2, D3DTSS_BUMPENVMAT11, 0);
	g_pd3dDevice->SetTextureStageState(2, D3DTSS_COLORARG0, D3DTA_CURRENT);
	g_pd3dDevice->SetTextureStageState(2, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	g_pd3dDevice->SetTextureStageState(2, D3DTSS_COLORARG2, D3DTA_CURRENT);
	g_pd3dDevice->SetTextureStageState(2, D3DTSS_COLORKEYCOLOR, 0);
	g_pd3dDevice->SetTextureStageState(2, D3DTSS_COLORKEYOP, D3DTCOLORKEYOP_DISABLE);
	g_pd3dDevice->SetTextureStageState(2, D3DTSS_COLOROP, D3DTOP_DISABLE);
	g_pd3dDevice->SetTextureStageState(2, D3DTSS_COLORSIGN, 0);
	g_pd3dDevice->SetTextureStageState(2, D3DTSS_MAGFILTER, D3DTEXF_POINT);
	g_pd3dDevice->SetTextureStageState(2, D3DTSS_MAXANISOTROPY, 1);
	g_pd3dDevice->SetTextureStageState(2, D3DTSS_MAXMIPLEVEL, 0);
	g_pd3dDevice->SetTextureStageState(2, D3DTSS_MINFILTER, D3DTEXF_POINT);
	g_pd3dDevice->SetTextureStageState(2, D3DTSS_MIPFILTER, D3DTEXF_NONE);
	g_pd3dDevice->SetTextureStageState(2, D3DTSS_MIPMAPLODBIAS, 0);
	g_pd3dDevice->SetTextureStageState(2, D3DTSS_RESULTARG, D3DTA_CURRENT);
	g_pd3dDevice->SetTextureStageState(2, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU | 1);
	g_pd3dDevice->SetTextureStageState(2, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);

	g_pd3dDevice->SetTextureStageState(3, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP);
	g_pd3dDevice->SetTextureStageState(3, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP);
	g_pd3dDevice->SetTextureStageState(3, D3DTSS_ADDRESSW, D3DTADDRESS_WRAP);
	g_pd3dDevice->SetTextureStageState(3, D3DTSS_ALPHAARG0, D3DTA_CURRENT);
	g_pd3dDevice->SetTextureStageState(3, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	g_pd3dDevice->SetTextureStageState(3, D3DTSS_ALPHAARG2, D3DTA_CURRENT);
	g_pd3dDevice->SetTextureStageState(3, D3DTSS_ALPHAKILL, D3DTALPHAKILL_DISABLE);
	g_pd3dDevice->SetTextureStageState(3, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
	g_pd3dDevice->SetTextureStageState(3, D3DTSS_BORDERCOLOR, 0);
	g_pd3dDevice->SetTextureStageState(3, D3DTSS_BUMPENVLOFFSET, 0);
	g_pd3dDevice->SetTextureStageState(3, D3DTSS_BUMPENVLSCALE, 0);
	g_pd3dDevice->SetTextureStageState(3, D3DTSS_BUMPENVMAT00, 0);
	g_pd3dDevice->SetTextureStageState(3, D3DTSS_BUMPENVMAT01, 0);
	g_pd3dDevice->SetTextureStageState(3, D3DTSS_BUMPENVMAT10, 0);
	g_pd3dDevice->SetTextureStageState(3, D3DTSS_BUMPENVMAT11, 0);
	g_pd3dDevice->SetTextureStageState(3, D3DTSS_COLORARG0, D3DTA_CURRENT);
	g_pd3dDevice->SetTextureStageState(3, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	g_pd3dDevice->SetTextureStageState(3, D3DTSS_COLORARG2, D3DTA_CURRENT);
	g_pd3dDevice->SetTextureStageState(3, D3DTSS_COLORKEYCOLOR, 0);
	g_pd3dDevice->SetTextureStageState(3, D3DTSS_COLORKEYOP, D3DTCOLORKEYOP_DISABLE);
	g_pd3dDevice->SetTextureStageState(3, D3DTSS_COLOROP, D3DTOP_DISABLE);
	g_pd3dDevice->SetTextureStageState(3, D3DTSS_COLORSIGN, 0);
	g_pd3dDevice->SetTextureStageState(3, D3DTSS_MAGFILTER, D3DTEXF_POINT);
	g_pd3dDevice->SetTextureStageState(3, D3DTSS_MAXANISOTROPY, 1);
	g_pd3dDevice->SetTextureStageState(3, D3DTSS_MAXMIPLEVEL, 0);
	g_pd3dDevice->SetTextureStageState(3, D3DTSS_MINFILTER, D3DTEXF_POINT);
	g_pd3dDevice->SetTextureStageState(3, D3DTSS_MIPFILTER, D3DTEXF_NONE);
	g_pd3dDevice->SetTextureStageState(3, D3DTSS_MIPMAPLODBIAS, 0);
	g_pd3dDevice->SetTextureStageState(3, D3DTSS_RESULTARG, D3DTA_CURRENT);
	g_pd3dDevice->SetTextureStageState(3, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU | 1);
	g_pd3dDevice->SetTextureStageState(3, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);

	D3DXMATRIX mat;
	D3DXMatrixIdentity(&mat);
    g_pd3dDevice->SetTransform(D3DTS_VIEW, &mat);
    g_pd3dDevice->SetTransform(D3DTS_PROJECTION, &mat);
    g_pd3dDevice->SetTransform(D3DTS_TEXTURE0, &mat);
    g_pd3dDevice->SetTransform(D3DTS_TEXTURE1, &mat);
    g_pd3dDevice->SetTransform(D3DTS_TEXTURE2, &mat);
    g_pd3dDevice->SetTransform(D3DTS_TEXTURE3, &mat);
    g_pd3dDevice->SetTransform(D3DTS_WORLD, &mat);
    g_pd3dDevice->SetTransform(D3DTS_WORLD1, &mat);
    g_pd3dDevice->SetTransform(D3DTS_WORLD2, &mat);
    g_pd3dDevice->SetTransform(D3DTS_WORLD3, &mat);


	D3DVIEWPORT8 vp;
	vp.X = 0;
	vp.Y = 0;
	if(bEnableHDTV){
		vp.Width = 1280;
		vp.Height = 720;
	}else{
		vp.Width = 640;
		vp.Height = 480;
	}
	vp.MinZ = 0.0;
	vp.MaxZ = 1.0;
	g_pd3dDevice->SetViewport(&vp);
}