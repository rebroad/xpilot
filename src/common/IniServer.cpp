/* $Id: IniServer.cpp,v 1.2 2004/02/03 03:39:52 dick Exp $
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
 * $Log: IniServer.cpp,v $
 * Revision 1.2  2004/02/03 03:39:52  dick
 * Add Windows precompiled header support.
 *
 * Revision 1.1  2002/09/12 19:51:44  dick
 * IniServer handles the XPilotServer.ini file for XPilot (control).
 * We can't use this in the server because Ini uses X11 stuff.
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

#include "IniServer.h"


PCSTR s_World0		= "World0";
PCSTR s_World1		= "World1";
PCSTR s_World2		= "World2";
PCSTR s_World3		= "World3";
PCSTR s_World4		= "World4";
PCSTR s_World5		= "World5";
PCSTR s_World6		= "World6";
PCSTR s_World7		= "World7";

// The one and only iniServer
IniServer	iniServer;

///////////////////////////////////////////////////////////////////////////////
IniServer::IniServer()
{
	SetDefaultOptions();
}

///////////////////////////////////////////////////////////////////////////////
IniServer::~IniServer()
{
}

///////////////////////////////////////////////////////////////////////////////
void IniServer::ParseOptions(int *argcp, char **argvp)
{

	XrmDatabase		argDB = 0;
	XrmDatabase		rDB = 0;

	D(xpprintf("%s Start parse\n", showtime());)
#ifdef _WINDOWS
	ParseSwitches(argcp, argvp);
#else
    int					i;
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

#endif

	GetStringResource(s_World0, &world0);
	GetStringResource(s_World1, &world1);


#ifndef _WINDOWS
	XrmDestroyDatabase(rDB);

	free(xopt);
#endif
	D(xpprintf("%s End parse\n", showtime());)
}

///////////////////////////////////////////////////////////////////////////////
void IniServer::ParseSwitches(int *argcp, char **argvp)
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
void IniServer::Save(FILE* fp)
{
	SaveResource(fp, s_World0, world0);
	SaveResource(fp, s_World1, world1);
}

