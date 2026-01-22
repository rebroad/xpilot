/* $Id: IniXPilot.Defaults.cpp,v 1.4 2004/05/11 06:47:56 dick Exp $
 *
 * Class to provide the client side resource file handling.
 * Unix this is ~/.xpilotrc and Windows is XPilot.ini
 *
 * XPilot, a multiplayer gravity war game.  Copyright (C) 1991-2001 by
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
 * $Log: IniXPilot.Defaults.cpp,v $
 * Revision 1.4  2004/05/11 06:47:56  dick
 * Load/Save the PrivateServer's servers.
 *
 * Revision 1.3  2004/02/03 03:39:53  dick
 * Add Windows precompiled header support.
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
#include "IniXPilot.h"

///////////////////////////////////////////////////////////////////////////////
/*
 * Structure to store all the XPilot (control) options.
 * The most important field is the help field.
 * It is used to self-document the client to
 * the user when "xpilot -help" is issued.
 * Help lines can span multiple lines, but for
 * the key help window only the first line is used.
 */
Option xpilotOptions[] = {
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
		s_LocalServerAdvanced,
		"",
		"no",
		KEY_DUMMY,
		"Allow advanced configuration of the local server\n"
	},
	{
		s_ColumnServerName,
		"",
		"yes",
		KEY_DUMMY,
		"Display the server name column\n"
	},
	{
		s_ColumnMapName,
		"",
		"yes",
		KEY_DUMMY,
		"Display the map name column\n"
	},
	{
		s_ColumnMapAuthor,
		"",
		"yes",
		KEY_DUMMY,
		"Display the map author column\n"
	},
	{
		s_ColumnMapSize,
		"",
		"no",
		KEY_DUMMY,
		"Display the map size column\n"
	},
	{
		s_ColumnNumPlayers,
		"",
		"no",
		KEY_DUMMY,
		"Display the number of players column\n"
	},
	{
		s_ColumnLag,
		"",
		"yes",
		KEY_DUMMY,
		"Display the number of milliseconds to each server column\n"
	},
	{
		s_ColumnNumBases,
		"",
		"no",
		KEY_DUMMY,
		"Display the server name column\n"
	},
	{
		s_ColumnNumTeams,
		"",
		"yes",
		KEY_DUMMY,
		"Display the number of teams column\n"
	},
	{
		s_ColumnFPS,
		"",
		"yes",
		KEY_DUMMY,
		"Display the Frames Per Second column\n"
	},
	{
		s_ColumnVersion,
		"",
		"yes",
		KEY_DUMMY,
		"Display the server version column\n"
	},
	{
		s_ColumnSound,
		"",
		"no",
		KEY_DUMMY,
		"Display the server has sound column\n"
	},
	{
		s_ColumnUptime,
		"",
		"yes",
		KEY_DUMMY,
		"Display the server uptime column\n"
	},
	{
		s_ColumnIP,
		"",
		"no",
		KEY_DUMMY,
		"Display the server ip address column\n"
	},
	{
		s_ColumnRaceMode,
		"",
		"no",
		KEY_DUMMY,
		"Display the race mode column\n"
	},
	{
		s_ColumnPort,
		"",
		"no",
		KEY_DUMMY,
		"Display the server tcp/ip port column\n"
	},
	/////////////////
	{
		s_ColWidthServerName,
		"",
		"133",
		KEY_DUMMY,
		"Width of the server name column\n"
	},
	{
		s_ColWidthMapName,
		"",
		"95",
		KEY_DUMMY,
		"Width of the map name column\n"
	},
	{
		s_ColWidthMapAuthor,
		"",
		"76",
		KEY_DUMMY,
		"Width of the map author column\n"
	},
	{
		s_ColWidthMapSize,
		"",
		"57",
		KEY_DUMMY,
		"Width of the map size column\n"
	},
	{
		s_ColWidthNumPlayers,
		"",
		"38",
		KEY_DUMMY,
		"Width of the number of players column\n"
	},
	{
		s_ColWidthLag,
		"",
		"26",
		KEY_DUMMY,
		"Width of the number of milliseconds to each server column\n"
	},
	{
		s_ColWidthNumBases,
		"",
		"38",
		KEY_DUMMY,
		"Width of the server name column\n"
	},
	{
		s_ColWidthNumTeams,
		"",
		"19",
		KEY_DUMMY,
		"Width of the number of teams column\n"
	},
	{
		s_ColWidthFPS,
		"",
		"26",
		KEY_DUMMY,
		"Width of the Frames Per Second column\n"
	},
	{
		s_ColWidthVersion,
		"",
		"76",
		KEY_DUMMY,
		"Width of the server version column\n"
	},
	{
		s_ColWidthSound,
		"",
		"38",
		KEY_DUMMY,
		"Width of the server has sound column\n"
	},
	{
		s_ColWidthUptime,
		"",
		"68",
		KEY_DUMMY,
		"Width of the server uptime column\n"
	},
	{
		s_ColWidthIP,
		"",
		"114",
		KEY_DUMMY,
		"Width of the server ip address column\n"
	},
	{
		s_ColWidthRaceMode,
		"",
		"57",
		KEY_DUMMY,
		"Width of the race mode column\n"
	},
	{
		s_ColWidthPort,
		"",
		"57",
		KEY_DUMMY,
		"Width of the server tcp/ip port column\n"
	},
	{
		s_PrivateServerCount,
		"",
		"0",
		KEY_DUMMY,
		"The number of private servers in the list\n"
	},
	{
		s_PrivateServerStar,
		"",
		"",
		KEY_DUMMY,
		"A Private server (name:port) that you enter into the list of known private servers\n"
	},
};

///////////////////////////////////////////////////////////////////////////////
void IniXPilot::SetDefaultOptions()
{
	numOptions = NELEM(xpilotOptions);
	options = xpilotOptions;
}

