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

#ifndef _OGLEXT_COMBINER_H_
#define _OGLEXT_COMBINER_H_

#ifndef _XBOX

#include "OGLCombiner.h"
#include "GeneralCombiner.h"
#include "gl/gl.h"

typedef union 
{
	struct {
		BYTE	arg0;
		BYTE	arg1;
		BYTE	arg2;
	};
	BYTE args[3];
} OGLExt1CombType;

typedef struct {
	union {
		struct {
			GLenum	rgbOp;
			GLenum	alphaOp;
		};
		GLenum ops[2];
	};

	union {
		struct {
			BYTE	rgbArg0;
			BYTE	rgbArg1;
			BYTE	rgbArg2;
			BYTE	alphaArg0;
			BYTE	alphaArg1;
			BYTE	alphaArg2;
		};
		struct {
			OGLExt1CombType rgbComb;
			OGLExt1CombType alphaComb;
		};
		OGLExt1CombType Combs[2];
	};

	union {
		struct {
			GLint	rgbArg0gl;
			GLint	rgbArg1gl;
			GLint	rgbArg2gl;
		};
		GLint glRGBArgs[3];
	};

	union {
		struct {
			GLint	rgbFlag0gl;
			GLint	rgbFlag1gl;
			GLint	rgbFlag2gl;
		};
		GLint glRGBFlags[3];
	};

	union {
		struct {
			GLint	alphaArg0gl;
			GLint	alphaArg1gl;
			GLint	alphaArg2gl;
		};
		GLint glAlphaArgs[3];
	};

	union {
		struct {
			GLint	alphaFlag0gl;
			GLint	alphaFlag1gl;
			GLint	alphaFlag2gl;
		};
		GLint glAlphaFlags[3];
	};

	int		tex;
	bool	textureIsUsed;
	//float	scale;		//Will not be used
} OGLExtCombinerType;

typedef struct {
	DWORD	dwMux0;
	DWORD	dwMux1;
	OGLExtCombinerType units[8];
	int		numOfUnits;
	DWORD	constantColor;
} OGLExtCombinerSaveType;


//========================================================================
// OpenGL 1.4 combiner which support Texture Crossbar feature
class COGLColorCombiner4 : public COGLColorCombiner
{
public:
	bool Initialize(void);
protected:
	friend class OGLDeviceBuilder;
	void InitCombinerCycle12(void);
	void InitCombinerCycleFill(void);
	virtual void GenerateCombinerSetting(int index);
	virtual void GenerateCombinerSettingConstants(int index);
	virtual int ParseDecodedMux();

	COGLColorCombiner4(CDaedalusRender *pRender);
	~COGLColorCombiner4() {};

	bool m_bOGLExtCombinerSupported;		// Is this OGL extension combiner supported by the video card driver?
	bool m_bSupportModAdd_ATI;
	bool m_bSupportModSub_ATI;
	int m_maxTexUnits;
	int	m_lastIndex;
	DWORD m_dwLastMux0;
	DWORD m_dwLastMux1;

#ifdef _DEBUG
	void DisplaySimpleMuxString(void);
#endif

protected:
	virtual int SaveParsedResult(OGLExtCombinerSaveType &result);
	static GLint MapRGBArgFlags(BYTE arg);
	static GLint MapAlphaArgFlags(BYTE arg);
	std::vector<OGLExtCombinerSaveType>		m_vCompiledSettings;
	static GLint RGBArgsMap4[];
	static const char* GetOpStr(GLenum op);

private:
	virtual int ParseDecodedMux2Units();
	virtual int FindCompiledMux();

	virtual GLint MapRGBArgs(BYTE arg);
	virtual GLint MapAlphaArgs(BYTE arg);
};

//////////////////////////////////////////////////////////////////////////
// OpenGL 1.2, 1.3 combiner which does not support Texture Crossbar feature
class COGLColorCombiner2 : public COGLColorCombiner4, CGeneralCombiner
{
public:
	bool Initialize(void);

#ifdef _DEBUG
	void DisplaySimpleMuxString(void);
#endif

protected:
	friend class OGLDeviceBuilder;

	COGLColorCombiner2(CDaedalusRender *pRender);
	~COGLColorCombiner2() {};

private:
	virtual int ParseDecodedMux();
	virtual void GenerateCombinerSetting(int index);
	virtual void GenerateCombinerSettingConstants(int index);

	virtual GLint MapRGBArgs(BYTE arg);
	virtual GLint MapAlphaArgs(BYTE arg);
	static GLint RGBArgsMap2[];
};
#endif
#endif