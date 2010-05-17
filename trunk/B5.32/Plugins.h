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

#pragma once

// Ez0n3 - use this file to assign plugins numerical values
// names set in ./Launcher/SurrealMenu.cpp
// both .c and .cpp use this

//give each video plugin a numerical value for use with Preferred Video Plugin
enum VideoPlugins 
{
	_VideoPluginRice510,
	_VideoPluginRice531,
	_VideoPluginRice560,
	_VideoPluginRice612,
	_VideoPluginMissing
};


// give each audio plugin a numerical value for use with Preferred Audio Plugin
enum AudioPlugins 
{
	_AudioPluginNone,
	_AudioPluginLleRsp,
	_AudioPluginBasic,
	_AudioPluginJttl,
	//_AudioPluginMusyX,	//freakdave - new MusyX audio plugin
	_AudioPluginAzimer,
	_AudioPluginMissing
};

