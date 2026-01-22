/* $Id: IniServer.h,v 1.1 2002/09/12 19:51:44 dick Exp $
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
 * $Log: IniServer.h,v $
 * Revision 1.1  2002/09/12 19:51:44  dick
 * IniServer handles the XPilotServer.ini file for XPilot (control).
 * We can't use this in the server because Ini uses X11 stuff.
 *
 */
#ifndef	_INISERVER_H_
#define	_INISERVER_H_

//#include <stdio.h>

#include "cstring.h"

#include "Ini.h"

extern	PCSTR s_World0;
extern	PCSTR s_World1;
extern	PCSTR s_World2;
extern	PCSTR s_World3;
extern	PCSTR s_World4;
extern	PCSTR s_World5;
extern	PCSTR s_World6;
extern	PCSTR s_World7;


class IniServer : public Ini
{
public:
	IniServer();
	~IniServer();

	virtual	void Save(FILE* fp);

	void	SetDefaultOptions();
	void	ParseOptions(int *argcp, char **argvp);
	void	ParseSwitches(int *argcp, char **argvp);

public:
	String	world0;
	String	world1;

private:
};

extern	IniServer	iniServer;	// each app has one instance of an IniServer

#endif		// _INISERVER_H_
