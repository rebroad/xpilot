/* $Id: IniServer.Defaults.cpp,v 1.2 2004/02/03 03:39:52 dick Exp $
 *
 * Class to provide the server side resource file handling.
 * Unix this is ~/.xpilot/XPilotServer.ini and Windows is XPilotServer.ini
 *
 * XPilot, a multiplayer gravity war game.  Copyright (C) 2002 by
 *
 *      The XPilot Authors   <xpilot@xpilot.org>
 *      Dick Balaska         <dick@xpilot.org>
 *      Jarrod Miller        <jarrod@xpilot.org>
 *      Bert Gijsbers        <bert@xpilot.org>
 *      Ken Ronny Schouten   <ken@xpilot.org>
 *      Bjørn Stabell        <bjoern@xpilot.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */
/*
 * $Log: IniServer.Defaults.cpp,v $
 * Revision 1.2  2004/02/03 03:39:52  dick
 * Add Windows precompiled header support.
 *
 * Revision 1.1  2002/09/12 19:51:44  dick
 * IniServer handles the XPilotServer.ini file for XPilot (control).
 * We can't use this in the server because Ini uses X11 stuff.
 *
 * Revision 1.2  2002/09/07 17:53:55  dick
 * Add attribute - bool localServerAdvanced.
 *
 * Revision 1.1  2002/09/02 16:19:20  dick
 * Subclass Ini and provide configuration for XPilot.exe
 *
 */
#include "StdAfx.h"

#include "pack.h"
#include "IniServer.h"

///////////////////////////////////////////////////////////////////////////////
/*
 * Structure to store all the XPilotServer (control) options.
 */
Option serverOptions[] = {
    {
		"help",
		"Yes",
		"",
		KEY_DUMMY,
		"Display this help message.\n"
    },
    {
		"version",
		"Yes",
		"",
		KEY_DUMMY,
		"Show the source code version.\n"
    },
	{
		s_World0,
		"",
		"",
		KEY_DUMMY,
		"Arguments to pass to world 0\n"
	},
	{
		s_World1,
		"",
		"",
		KEY_DUMMY,
		"Arguments to pass to world 1\n"
	},
	{
		s_World2,
		"",
		"",
		KEY_DUMMY,
		"Arguments to pass to world 2\n"
	},
	{
		s_World3,
		"",
		"",
		KEY_DUMMY,
		"Arguments to pass to world 3\n"
	},
	{
		s_World4,
		"",
		"",
		KEY_DUMMY,
		"Arguments to pass to world 4\n"
	},
	{
		s_World5,
		"",
		"",
		KEY_DUMMY,
		"Arguments to pass to world 5\n"
	},
	{
		s_World6,
		"",
		"",
		KEY_DUMMY,
		"Arguments to pass to world 6\n"
	},
	{
		s_World7,
		"",
		"",
		KEY_DUMMY,
		"Arguments to pass to world 7\n"
	},

};

///////////////////////////////////////////////////////////////////////////////
void IniServer::SetDefaultOptions()
{
	numOptions = NELEM(serverOptions);
	options = serverOptions;
}

