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

#include "stdafx.h"
#include "glh_genext.h"

//========================================================================
DWORD DirectX_OGL_BlendFuncMaps [] =
{
	GL_SRC_ALPHA,		//Nothing
    GL_ZERO,			//D3DBLEND_ZERO               = 1,
    GL_ONE,				//D3DBLEND_ONE                = 2,
    GL_SRC_COLOR,		//D3DBLEND_SRCCOLOR           = 3,
    GL_ONE_MINUS_SRC_COLOR,		//D3DBLEND_INVSRCCOLOR        = 4,
    GL_SRC_ALPHA,				//D3DBLEND_SRCALPHA           = 5,
    GL_ONE_MINUS_SRC_ALPHA,		//D3DBLEND_INVSRCALPHA        = 6,
    GL_DST_ALPHA,				//D3DBLEND_DESTALPHA          = 7,
    GL_ONE_MINUS_DST_ALPHA,		//D3DBLEND_INVDESTALPHA       = 8,
    GL_DST_COLOR,				//D3DBLEND_DESTCOLOR          = 9,
    GL_ONE_MINUS_DST_COLOR,		//D3DBLEND_INVDESTCOLOR       = 10,
    GL_SRC_ALPHA_SATURATE,		//D3DBLEND_SRCALPHASAT        = 11,
    GL_SRC_ALPHA_SATURATE,		//D3DBLEND_BOTHSRCALPHA       = 12,	
    GL_SRC_ALPHA_SATURATE,		//D3DBLEND_BOTHINVSRCALPHA    = 13,
};

//========================================================================
COGLColorCombiner::COGLColorCombiner(CRender *pRender) :
	CColorCombiner(pRender),
	m_pOGLRender((OGLRender*)pRender),
	m_bSupportAdd(false), m_bSupportSubtract(false)
{
	m_pDecodedMux = new COGLDecodedMux;
	m_pDecodedMux->m_maxConstants = 0;
	m_pDecodedMux->m_maxTextures = 1;
}

COGLColorCombiner::~COGLColorCombiner()
{
	delete m_pDecodedMux;
	m_pDecodedMux = NULL;
}

bool COGLColorCombiner::Initialize(void)
{
	m_bSupportAdd = false;
	m_bSupportSubtract = false;
	m_supportedStages = 1;
	m_bSupportMultiTexture = false;

	COGLGraphicsContext *pcontext = (COGLGraphicsContext *)(CGraphicsContext::g_pGraphicsContext);
	if( pcontext->IsExtensionSupported("GL_ARB_texture_env_add") || pcontext->IsExtensionSupported("GL_EXT_texture_env_add") )
	{
		m_bSupportAdd = true;
	}

	if( pcontext->IsExtensionSupported("GL_EXT_blend_subtract") )
	{
		m_bSupportSubtract = true;
	}

	return true;
}

void COGLColorCombiner::DisableCombiner(void)
{
	m_pOGLRender->DisableMultiTexture();
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ZERO);
	
	if( m_bTexelsEnable )
	{
		COGLTexture* pTexture = g_textures[gRSP.curTile].m_pCOGLTexture;
		if( pTexture ) 
		{
			m_pOGLRender->EnableTexUnit(0,TRUE);
			m_pOGLRender->BindTexture(pTexture->m_dwTextureName, 0);
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
			m_pOGLRender->SetAllTexelRepeatFlag();
		}
#ifdef _DEBUG
		else
		{
			DebuggerAppendMsg("Check me, texture is NULL but it is enabled");
		}
#endif
	}
	else
	{
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		m_pOGLRender->EnableTexUnit(0,FALSE);
	}
}

void COGLColorCombiner::InitCombinerCycleCopy(void)
{
	m_pOGLRender->DisableMultiTexture();
	m_pOGLRender->EnableTexUnit(0,TRUE);
	COGLTexture* pTexture = g_textures[gRSP.curTile].m_pCOGLTexture;
	if( pTexture )
	{
		m_pOGLRender->BindTexture(pTexture->m_dwTextureName, 0);
		m_pOGLRender->SetTexelRepeatFlags(gRSP.curTile);
	}
#ifdef _DEBUG
	else
	{
		DebuggerAppendMsg("Check me, texture is NULL");
	}
#endif

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
}

void COGLColorCombiner::InitCombinerCycleFill(void)
{
	m_pOGLRender->DisableMultiTexture();
	m_pOGLRender->EnableTexUnit(0,FALSE);
}

void COGLColorCombiner::InitCombinerCycle12(void)
{
	m_pOGLRender->DisableMultiTexture();
	if( !m_bTexelsEnable )
	{
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		m_pOGLRender->EnableTexUnit(0,FALSE);
		return;
	}

	DWORD mask = 0x1f;
	COGLTexture* pTexture = g_textures[gRSP.curTile].m_pCOGLTexture;
	if( pTexture )
	{
		m_pOGLRender->EnableTexUnit(0,TRUE);
		m_pOGLRender->BindTexture(pTexture->m_dwTextureName, 0);
		m_pOGLRender->SetAllTexelRepeatFlag();
	}
#ifdef _DEBUG
	else
	{
		DebuggerAppendMsg("Check me, texture is NULL");
	}
#endif

	bool texIsUsed = m_pDecodedMux->isUsed(MUX_TEXEL0);
	bool shadeIsUsed = m_pDecodedMux->isUsed(MUX_SHADE);
	bool shadeIsUsedInColor = m_pDecodedMux->isUsedInCycle(MUX_SHADE, 0, COLOR_CHANNEL);
	bool texIsUsedInColor = m_pDecodedMux->isUsedInCycle(MUX_TEXEL0, 0, COLOR_CHANNEL);

	if( texIsUsed )
	{
		// Parse the simplified the mux, because the OGL 1.1 combiner function is so much
		// limited, we only parse the 1st N64 combiner setting and only the RGB part

		N64CombinerType & comb = m_pDecodedMux->m_n64Combiners[0];
		switch( m_pDecodedMux->mType )
		{
		case CM_FMT_TYPE_NOT_USED:
		case CM_FMT_TYPE_D:				// = A
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
			break;
		case CM_FMT_TYPE_A_ADD_D:			// = A+D
			if( shadeIsUsedInColor && texIsUsedInColor )
			{
				if( m_bSupportAdd )
					glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_ADD);
				else
					glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND);
			}
			else if( texIsUsedInColor )
			{
				glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
			}
			else
				glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
			break;
		case CM_FMT_TYPE_A_SUB_B:			// = A-B
			if( shadeIsUsedInColor && texIsUsedInColor )
			{
				if( m_bSupportSubtract )
					glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_SUBTRACT_ARB);
				else
					glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND);
			}
			else if( texIsUsedInColor )
			{
				glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
			}
			else
				glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
			break;
		case CM_FMT_TYPE_A_MOD_C:			// = A*C
		case CM_FMT_TYPE_A_MOD_C_ADD_D:	// = A*C+D
			if( shadeIsUsedInColor && texIsUsedInColor )
			{
				if( ((comb.c & mask) == MUX_SHADE && !(comb.c&MUX_COMPLEMENT)) || 
					((comb.a & mask) == MUX_SHADE && !(comb.a&MUX_COMPLEMENT)) )
					glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
				else
					glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
			}
			else if( texIsUsedInColor )
			{
				glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
			}
			else
				glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
			break;
		case CM_FMT_TYPE_A_LERP_B_C:	// = A*C+D
			if( (comb.b&mask) == MUX_SHADE && (comb.c&mask)==MUX_TEXEL0 && ((comb.a&mask)==MUX_PRIM||(comb.a&mask)==MUX_ENV))
			{
				float *fv;
				if( (comb.a&mask)==MUX_PRIM )
				{
					fv = GetPrimitiveColorfv();
				}
				else
				{
					fv = GetEnvColorfv();
				}

				glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR,fv);
				glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND);
				break;
			}
		default:		// = (A-B)*C+D
			if( shadeIsUsedInColor )
			{
				if( ((comb.c & mask) == MUX_SHADE && !(comb.c&MUX_COMPLEMENT)) || 
					((comb.a & mask) == MUX_SHADE && !(comb.a&MUX_COMPLEMENT)) )
					glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
				else
					glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
			}
			else
			{
				glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
			}
			break;
		}
	}
	else
	{
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	}
}

void COGLBlender::NormalAlphaBlender(void)
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void COGLBlender::DisableAlphaBlender(void)
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ZERO);
}


void COGLBlender::BlendFunc(DWORD srcFunc, DWORD desFunc)
{
	glBlendFunc(DirectX_OGL_BlendFuncMaps[srcFunc], DirectX_OGL_BlendFuncMaps[desFunc]);
}

void COGLBlender::Enable()
{
	glEnable(GL_BLEND);
}

void COGLBlender::Disable()
{
	glDisable(GL_BLEND);
}

void COGLColorCombiner::InitCombinerBlenderForSimpleTextureDraw(DWORD tile)
{
	m_pOGLRender->DisableMultiTexture();
	if( g_textures[tile].m_pCTexture )
	{
		m_pOGLRender->EnableTexUnit(0,TRUE);
		glBindTexture(GL_TEXTURE_2D, ((COGLTexture*)(g_textures[tile].m_pCTexture))->m_dwTextureName);
	}
	m_pOGLRender->SetAllTexelRepeatFlag();

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);	// Linear Filtering
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);	// Linear Filtering

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	m_pOGLRender->SetAlphaTestEnable(FALSE);
}

#ifdef _DEBUG
extern const char *translatedCombTypes[];
void COGLColorCombiner::DisplaySimpleMuxString(void)
{
	TRACE0("\nSimplified Mux\n");
	m_pDecodedMux->DisplaySimpliedMuxString("Used");
}
#endif


COGLFragmentShaderCombiner::COGLFragmentShaderCombiner(CRender *pRender)
	: COGLColorCombiner(pRender)
{
	m_bShaderIsSupported = false;
}
COGLFragmentShaderCombiner::~COGLFragmentShaderCombiner()
{
}

bool COGLFragmentShaderCombiner::Initialize(void)
{
	if( !COGLColorCombiner::Initialize() )
		return false;

	COGLGraphicsContext *pcontext = (COGLGraphicsContext *)(CGraphicsContext::g_pGraphicsContext);
	if( pcontext->IsExtensionSupported("GL_ARB_fragment_shader") )
	{
		m_bShaderIsSupported = true;
	}

	return true;
}

void COGLFragmentShaderCombiner::InitCombinerCycle12(void)
{
	/*
	GLcharARB *vertexShader;
	GLcharARB *fragmentShader;

	const GLcharARB    *pInfoLog;
	GLboolean compiled = GL_FALSE;
	GLboolean linked   = GL_FALSE;
	GLint	  length;

	//
	// Create shader and program objects.
	//
	ProgramObject	   = glCreateProgramObjectARB();
	VertexShaderObject = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
	FragmentShaderObject = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);


	length = strlen(vertexShader);
	glShaderSourceARB(VertexShaderObject, 1, &vertexShader, &length);
	glShaderSourceARB(FragmentShaderObject, 1, &fragmentShader, NULL);

	//
	// OpenGL made a copy of the shaders, we can free our copy
	//
	free(vertexShader);
	free(fragmentShader);

	//
	// Compile the vertex and fragment shader, and print out the
	// compiler log file.
	//
	glCompileShaderARB(VertexShaderObject);
	glGetObjectParameterivARB(VertexShaderObject,
		GL_OBJECT_COMPILE_STATUS_ARB, &compiled);

	glGetObjectParamterivARB(VertexShaderObject,
		GL_OBJECT_INFO_LOG_LENGTH_ARB, &maxLength);
	pInfoLog = (GLcharARB *) malloc(maxLength * sizeof(GLcharARB));
	glGetInfoLogARB(VertexShaderObject, maxLength, &length, pInfoLog);
	printf("%s", pInfoLog);
	free(pInfoLog);

	if (!compiled) {
		printf("Compile failed\n");
		return GL_FALSE;
	}

	glCompileShaderARB(FragmentShaderObject);
	glGetObjectParameterivARB(FragmentShaderObject,
		GL_OBJECT_COMPILE_STATUS_ARB, &compiled);

	glGetObjectParamterivARB(FragmentShaderObject,
		GL_OBJECT_INFO_LOG_LENGTH_ARB, &maxLength);
	pInfoLog = (GLcharARB *) malloc(maxLength * sizeof(GLcharARB));
	glGetInfoLogARB(FragmentShaderObject, maxLength, NULL, pInfoLog);
	printf("%s", pInfoLog);
	free(pInfoLog);

	if (!compiled) {
		printf("Compile failed\n");
		return GL_FALSE;
	}

	//
	// Populate the program object with the compiled shadera
	//
	glAttachObjectARB(ProgramObject, VertexShaderObject);
	glAttachObjectARB(ProgramObject, FragmentShaderObject);

	//
	// We want the shader objects to go away as soon as it is detached
	// from the program object it is attached to. We can simply call
	// delete now to achieve that. Note that calling delete on a program
	// object will result in all shaders attached to that program object
	// to be detached. If delete has been called for the shader objects,
	// calling delete on the program object will result in the shader
	// objects being deleted as well.
	//
	glDeleteObjectARB(VertexShaderObject);
	glDeleteObjectARB(FragmentShaderObject);

	//
	// Link the program object and print out the linker log file
	//
	glLinkProgramARB(ProgramObject);
	glGetObjectParameterivARB(ProgramObject, GL_OBJECT_LINK_STATUS_ARB,
		&linked);

	glGetObjectParamterivARB(ProgramObject,
		GL_OBJECT_INFO_LOG_LENGTH_ARB, &maxLength);
	pInfoLog = (GLcharARB *) malloc(maxLength * sizeof(GLcharARB));
	glGetInfoLogARB(ProgramObject, maxLength, NULL, pInfoLog);

	printf("%s\n", pInfoLog);
	free(pInfoLog);

	//
	// If all went well, make the program object part of the current state
	//
	if (linked) {
		glUseProgramObjectARB(ProgramObject);
		return GL_TRUE;
	} else {
		return GL_FALSE;
	}
	*/
}
void COGLFragmentShaderCombiner::DisableCombiner(void)
{
	COGLColorCombiner::DisableCombiner();
}

void COGLFragmentShaderCombiner::InitCombinerCycleCopy(void)
{
	COGLColorCombiner::InitCombinerCycleCopy();
}

void COGLFragmentShaderCombiner::InitCombinerCycleFill(void)
{
	COGLColorCombiner::InitCombinerCycleFill();
}
void COGLFragmentShaderCombiner::InitCombinerBlenderForSimpleTextureDraw(DWORD tile)
{
	COGLColorCombiner::InitCombinerBlenderForSimpleTextureDraw(tile);
}

#ifdef _DEBUG
void COGLFragmentShaderCombiner::DisplaySimpleMuxString(void)
{
	COGLColorCombiner::DisplaySimpleMuxString();
}
#endif

