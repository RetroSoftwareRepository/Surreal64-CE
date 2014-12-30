/**
 * Surreal 64 Launcher (C) 2003
 * 
 * This program is free software; you can redistribute it and/or modify it under 
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option) any later
 * version. This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details. You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA. To contact the
 * authors: email: buttza@hotmail.com, lantus@lantus-x.com
 */
#ifndef _SURREAL_PLUGINS_H__COMMON_
#define _SURREAL_PLUGINS_H__COMMON_

#if _MSC_VER > 1000
#pragma once
#endif //_MSC_VER > 1000

//Video Plugins
enum VideoPlugins 
{
	_VideoPluginRice510,
	_VideoPluginRice531,
	_VideoPluginRice560,
	_VideoPluginRice611,
	_VideoPluginRice612,
	_VideoPluginVid1964,
	_VideoPluginMissing
};

//Audio Plugins
enum AudioPlugins 
{
	_AudioPluginNone,
	_AudioPluginLleRsp,
	_AudioPluginBasic,
	_AudioPluginJttl,
	_AudioPluginAzimer,
	_AudioPluginMusyX,
	_AudioPluginDaedalus,
	_AudioPluginMissing
};

enum AudioPluginsTest
{
	_AudioPluginM64p = 99 // mupen 1.5 audio
};

//RSP Plugins
enum RSPPlugins 
{
	_RSPPluginNone,
	_RSPPluginLLE,
	_RSPPluginHLE,
	_RSPPluginM64p,
	_RSPPluginMissing
};

//Rom Paging Methods
enum PagingMethods
{
	_PagingXXX, // Surreal XXX
	_PagingS10, // Surreal 1.0
	//_PagingVMM, // VMM maybe?
	_PagingMissing
};

#endif //_PLUGINS_H__COMMON_