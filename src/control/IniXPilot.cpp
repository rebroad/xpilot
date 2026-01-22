/* $Id: IniXPilot.cpp,v 1.8 2004/05/11 06:47:56 dick Exp $
 *
 * Class to provide the client side resource file handling.
 * Unix this is ~/.xpilotrc and Windows is XPilot.ini
 *
 * XPilot, a multiplayer gravity war game.  Copyright (C) 1991-2001 by
 *
 *      Bj�rn Stabell        <bjoern@xpilot.org>
 *      Ken Ronny Schouten   <ken@xpilot.org>
 *      Bert Gijsbers        <bert@xpilot.org>
 *      Dick Balaska         <dick@xpilot.org>
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
 * $Log: IniXPilot.cpp,v $
 * Revision 1.8  2004/05/11 06:47:56  dick
 * Load/Save the PrivateServer's servers.
 *
 * Revision 1.7  2004/05/10 12:43:30  dick
 * Handle Inet list and Private list
 *
 * Revision 1.6  2004/02/03 03:39:53  dick
 * Add Windows precompiled header support.
 *
 * Revision 1.5  2002/09/12 20:01:01  dick
 * Whitespace
 *
 * Revision 1.4  2002/09/07 17:53:55  dick
 * Add attribute - bool localServerAdvanced.
 *
 * Revision 1.3  2002/09/03 07:45:47  dick
 * Comment out unused code.
 *
 * Revision 1.2  2002/09/02 21:07:55  dick
 * Fix Linux ParseOptions
 *
 * Revision 1.1  2002/09/02 16:19:20  dick
 * Subclass Ini and provide configuration for XPilot.exe
 *
 */
#include "StdAfx.h"

#ifndef	_WINDOWS
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include "version.h"
#endif

#include "cstring.h"
#include "config.h"
#include "showtime.h"
#include "error.h"
#include "types.h"
#include "portability.h"

#include "IniXPilot.h"


PCSTR s_LocalServerAdvanced	= "LocalServerAdvanced";;
PCSTR s_ColumnServerName	= "ColumnServerName";
PCSTR s_ColumnMapName		= "ColumnMapName";
PCSTR s_ColumnMapAuthor		= "ColumnMapAuthor";
PCSTR s_ColumnMapSize		= "ColumnMapSize";
PCSTR s_ColumnNumPlayers	= "ColumnNumPlayers";
PCSTR s_ColumnNumBases		= "ColumnNumBases";
PCSTR s_ColumnLag			= "ColumnLag";
PCSTR s_ColumnNumTeams		= "ColumnNumTeams";
PCSTR s_ColumnFPS			= "ColumnFPS";
PCSTR s_ColumnVersion		= "ColumnVersion";
PCSTR s_ColumnSound			= "ColumnSound";
PCSTR s_ColumnUptime		= "ColumnUptime";
PCSTR s_ColumnIP			= "ColumnIP";
PCSTR s_ColumnRaceMode		= "ColumnRaceMode";
PCSTR s_ColumnPort			= "ColumnPort";
PCSTR s_ColWidthServerName	= "ColWidthServerName";
PCSTR s_ColWidthMapName		= "ColWidthMapName";
PCSTR s_ColWidthMapAuthor	= "ColWidthMapAuthor";
PCSTR s_ColWidthMapSize		= "ColWidthMapSize";
PCSTR s_ColWidthNumPlayers	= "ColWidthNumPlayers";
PCSTR s_ColWidthNumBases	= "ColWidthNumBases";
PCSTR s_ColWidthLag			= "ColWidthLag";
PCSTR s_ColWidthNumTeams	= "ColWidthNumTeams";
PCSTR s_ColWidthFPS			= "ColWidthFPS";
PCSTR s_ColWidthVersion		= "ColWidthVersion";
PCSTR s_ColWidthSound		= "ColWidthSound";
PCSTR s_ColWidthUptime		= "ColWidthUptime";
PCSTR s_ColWidthIP			= "ColWidthIP";
PCSTR s_ColWidthRaceMode	= "ColWidthRaceMode";
PCSTR s_ColWidthPort		= "ColWidthPort";

PCSTR s_PrivateServerCount	= "PrivateServerCount";
PCSTR s_PrivateServerStar	= "PrivateServer*";

PCSTR s_inet				= "inet";
PCSTR s_priv				= "priv";

// The one and only ini for the control
IniXPilot	iniXPilot;

///////////////////////////////////////////////////////////////////////////////
IniXPilot::IniXPilot()
{
	SetDefaultOptions();
}

///////////////////////////////////////////////////////////////////////////////
IniXPilot::~IniXPilot()
{
}

///////////////////////////////////////////////////////////////////////////////
void IniXPilot::ParseOptions(int *argcp, char **argvp)
{

	XrmDatabase		argDB = 0;
	XrmDatabase		rDB = 0;
    int				i;

	D(xpprintf("%s Start parse\n", showtime());)
#ifdef _WINDOWS
	ParseSwitches(argcp, argvp);
#else
    char*				ptr;
	XrmOptionDescRec*	xopt;
	int					size;
    char				resValue[MAX(2*MSG_LEN, PATH_MAX + 1)];

	XrmInitialize();

	/*
	 * Construct a Xrm Option table from our options array.
	 */
	size = sizeof(*xopt) * numOptions;
	for (i = 0; i < numOptions; i++) {
		size += 2 * (strlen(options[i].name) + 2);
	}
	if ((ptr = (char *)malloc(size)) == NULL) {
		error("No memory for options");
		exit(1);
	}
	xopt = (XrmOptionDescRec *)ptr;
	ptr += sizeof(*xopt) * numOptions;
	for (i = 0; i < numOptions; i++) {
		options[i].hash = StringHash(options[i].name);
		xopt[i].option = ptr;
		xopt[i].option[0] = '-';
		strcpy(&xopt[i].option[1], options[i].name);
		size = strlen(options[i].name) + 2;
		ptr += size;
		xopt[i].specifier = ptr;
		xopt[i].specifier[0] = '.';
		strcpy(&xopt[i].specifier[1], options[i].name);
		ptr += size;
		if (options[i].noArg) {
			xopt[i].argKind = XrmoptionNoArg;
			xopt[i].value = (char *)options[i].noArg;
		}
		else {
			xopt[i].argKind = XrmoptionSepArg;
			xopt[i].value = NULL;
		}
	}

	XrmParseCommand(&argDB, xopt, numOptions, myName, argcp, argvp);

	/*
	 * Check for bad arguments.
	 */
	for (i = 1; i < *argcp; i++) {
		if (argvp[i][0] == '-' || argvp[i][0] == '+') {
			seterrno(0);
			error("Unknown or incomplete option '%s'", argvp[i]);
			error("Type: %s -help to see a list of options", argvp[0]);
			exit(1);
		}
		/* The rest of the arguments are hostnames of servers. */
	}
		curDB = argDB;
	if (GetResource("version", resValue, sizeof resValue) != 0) {
		puts(TITLE);
		exit(0);
	}

	GetFileDefaults(&rDB);

	XrmMergeDatabases(argDB, &rDB);
	curDB = rDB;

//	GetStringResource("geometry", resValue, sizeof resValue);
//	geometry = xp_strdup(resValue);
#endif

	GetBoolResource(s_LocalServerAdvanced, &localServerAdvanced);
	LoadPanelSettings(s_inet, inetSlCfg);
	LoadPanelSettings(s_priv, privateSlCfg);

	privateServersLoadedCount = 0;
	GetIntResource(s_PrivateServerCount, &privateServersLoadedCount);
	for (i=0; i<privateServersLoadedCount; i++)
	{
		String	s;
		String	res;
		s = i;
		GetStringResource(s_PrivateServerStar, &res, s);
		if (!res.GetLength())
			break;
		int index;
		index = res.Find(':');
		if (index == -1)
		{
			error("No : in name:port in PrivateServer in XPilot.ini\n"
				  "\"%s\"", (PCSTR)res);
			break;
		}
		IniPrivateServer* ips = new IniPrivateServer();
		ips->name = res.Left(index);
		s = res.Mid(index+1);
		index = s.Find(':');
		if (index != -1)
		{
			ips->ip = s.Mid(index+1);
			s = s.Left(index);
		}
		ips->port = s.atoi();
		privateServerList.Add(ips);
	}

#ifndef _WINDOWS
	XrmDestroyDatabase(rDB);

	free(xopt);
#endif
	D(xpprintf("%s End parse\n", showtime());)
}

///////////////////////////////////////////////////////////////////////////////
void IniXPilot::ParseSwitches(int *argcp, char **argvp)
{
	int 			i;
	for (i = 1; i < *argcp; i++)
	{
/*		if (argvp[i][0] == '-' || argvp[i][0] == '+')
		{
			if (!strcmp(&argvp[i][1], "team"))
			{
				team = atoi(argvp[i+1]);
				SkipSwitches(argcp, argvp, i, 2);
			}
			if (!strcmp(&argvp[i][1], "port"))
			{
				serverport = atoi(argvp[i+1]);
				SkipSwitches(argcp, argvp, i, 2);
			}
		}
*/
	}
}

///////////////////////////////////////////////////////////////////////////////
void IniXPilot::Save(FILE* fp)
{
    SaveBool(fp, s_LocalServerAdvanced, localServerAdvanced);

	SavePanelSettings(fp, s_inet, inetSlCfg);
	SavePanelSettings(fp, s_priv, privateSlCfg);

	int	count = 0;
	IniPrivateServer* ips;
	for (ips=(IniPrivateServer*)privateServerList.GetHead();
		 ips;
		 ips=(IniPrivateServer*)ips->GetNext(), count++)
	{
		String	s;
		s = s_PrivateServerStar;
		s = s.Left(s.GetLength()-1);
		s += count;

		String	value;
		value.printf("%s:%d:%s", (PCSTR)ips->name, ips->port, (PCSTR)ips->ip);
		SaveResource(fp, s, value);
	}
	SaveInt(fp, s_PrivateServerCount, count);

}

///////////////////////////////////////////////////////////////////////////////
void IniXPilot::LoadPanelSettings(PCSTR prefix, ServerListCfg& cfg)
{
	GetBoolResource(s_ColumnServerName, &cfg.columns[COL_SERVERNAME], prefix);
	GetBoolResource(s_ColumnMapName, &cfg.columns[COL_MAPNAME], prefix);
	GetBoolResource(s_ColumnMapAuthor, &cfg.columns[COL_MAPAUTHOR], prefix);
	GetBoolResource(s_ColumnMapSize, &cfg.columns[COL_MAPSIZE], prefix);
	GetBoolResource(s_ColumnNumPlayers, &cfg.columns[COL_PLAYERCOUNT], prefix);
	GetBoolResource(s_ColumnLag, &cfg.columns[COL_LAG], prefix);
	GetBoolResource(s_ColumnNumBases, &cfg.columns[COL_BASES], prefix);
	GetBoolResource(s_ColumnNumTeams, &cfg.columns[COL_TEAMS], prefix);
	GetBoolResource(s_ColumnFPS, &cfg.columns[COL_FPS], prefix);
	GetBoolResource(s_ColumnVersion, &cfg.columns[COL_VERSION], prefix);
	GetBoolResource(s_ColumnSound, &cfg.columns[COL_SOUND], prefix);
	GetBoolResource(s_ColumnUptime, &cfg.columns[COL_UPTIME], prefix);
	GetBoolResource(s_ColumnIP, &cfg.columns[COL_IP], prefix);
	GetBoolResource(s_ColumnRaceMode, &cfg.columns[COL_RACE], prefix);
	GetBoolResource(s_ColumnPort, &cfg.columns[COL_PORT], prefix);
	GetIntResource(s_ColWidthServerName, &cfg.colSizes[COL_SERVERNAME], prefix);
	GetIntResource(s_ColWidthMapName, &cfg.colSizes[COL_MAPNAME], prefix);
	GetIntResource(s_ColWidthMapAuthor, &cfg.colSizes[COL_MAPAUTHOR], prefix);
	GetIntResource(s_ColWidthMapSize, &cfg.colSizes[COL_MAPSIZE], prefix);
	GetIntResource(s_ColWidthNumPlayers, &cfg.colSizes[COL_PLAYERCOUNT], prefix);
	GetIntResource(s_ColWidthLag, &cfg.colSizes[COL_LAG], prefix);
	GetIntResource(s_ColWidthNumBases, &cfg.colSizes[COL_BASES], prefix);
	GetIntResource(s_ColWidthNumTeams, &cfg.colSizes[COL_TEAMS], prefix);
	GetIntResource(s_ColWidthFPS, &cfg.colSizes[COL_FPS], prefix);
	GetIntResource(s_ColWidthVersion, &cfg.colSizes[COL_VERSION], prefix);
	GetIntResource(s_ColWidthSound, &cfg.colSizes[COL_SOUND], prefix);
	GetIntResource(s_ColWidthUptime, &cfg.colSizes[COL_UPTIME], prefix);
	GetIntResource(s_ColWidthIP, &cfg.colSizes[COL_IP], prefix);
	GetIntResource(s_ColWidthRaceMode, &cfg.colSizes[COL_RACE], prefix);
	GetIntResource(s_ColWidthPort, &cfg.colSizes[COL_PORT], prefix);
}

///////////////////////////////////////////////////////////////////////////////
void IniXPilot::SavePanelSettings(FILE* fp, PCSTR prefix, const ServerListCfg& cfg)
{
	String	pre;
	pre = prefix;

    SaveBool(fp, pre + s_ColumnServerName, cfg.columns[COL_SERVERNAME]);
    SaveBool(fp, pre + s_ColumnMapName, cfg.columns[COL_MAPNAME]);
    SaveBool(fp, pre + s_ColumnMapAuthor, cfg.columns[COL_MAPAUTHOR]);
    SaveBool(fp, pre + s_ColumnMapSize, cfg.columns[COL_MAPSIZE]);
    SaveBool(fp, pre + s_ColumnNumPlayers, cfg.columns[COL_PLAYERCOUNT]);
    SaveBool(fp, pre + s_ColumnLag, cfg.columns[COL_LAG]);
    SaveBool(fp, pre + s_ColumnNumBases, cfg.columns[COL_BASES]);
    SaveBool(fp, pre + s_ColumnNumTeams, cfg.columns[COL_TEAMS]);
    SaveBool(fp, pre + s_ColumnFPS, cfg.columns[COL_FPS]);
    SaveBool(fp, pre + s_ColumnVersion, cfg.columns[COL_VERSION]);
    SaveBool(fp, pre + s_ColumnSound, cfg.columns[COL_SOUND]);
    SaveBool(fp, pre + s_ColumnUptime, cfg.columns[COL_UPTIME]);
    SaveBool(fp, pre + s_ColumnIP, cfg.columns[COL_IP]);
    SaveBool(fp, pre + s_ColumnRaceMode, cfg.columns[COL_RACE]);
    SaveBool(fp, pre + s_ColumnPort, cfg.columns[COL_PORT]);

    SaveInt(fp, pre + s_ColWidthServerName, cfg.colSizes[COL_SERVERNAME]);
    SaveInt(fp, pre + s_ColWidthMapName, cfg.colSizes[COL_MAPNAME]);
    SaveInt(fp, pre + s_ColWidthMapAuthor, cfg.colSizes[COL_MAPAUTHOR]);
    SaveInt(fp, pre + s_ColWidthMapSize, cfg.colSizes[COL_MAPSIZE]);
    SaveInt(fp, pre + s_ColWidthNumPlayers, cfg.colSizes[COL_PLAYERCOUNT]);
    SaveInt(fp, pre + s_ColWidthLag, cfg.colSizes[COL_LAG]);
    SaveInt(fp, pre + s_ColWidthNumBases, cfg.colSizes[COL_BASES]);
    SaveInt(fp, pre + s_ColWidthNumTeams, cfg.colSizes[COL_TEAMS]);
    SaveInt(fp, pre + s_ColWidthFPS, cfg.colSizes[COL_FPS]);
    SaveInt(fp, pre + s_ColWidthVersion, cfg.colSizes[COL_VERSION]);
    SaveInt(fp, pre + s_ColWidthSound, cfg.colSizes[COL_SOUND]);
    SaveInt(fp, pre + s_ColWidthUptime, cfg.colSizes[COL_UPTIME]);
    SaveInt(fp, pre + s_ColWidthIP, cfg.colSizes[COL_IP]);
    SaveInt(fp, pre + s_ColWidthRaceMode, cfg.colSizes[COL_RACE]);
    SaveInt(fp, pre + s_ColWidthPort, cfg.colSizes[COL_PORT]);

}

