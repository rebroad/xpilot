/* $Id: IniXPilot.h,v 1.6 2004/05/11 06:47:56 dick Exp $
 *
 * Class to provide the client side resource file handling.
 * Unix this is ~/.xpilotrc and Windows is XPilot.ini
 *
 * XPilot, a multiplayer gravity war game.  Copyright (C) 1991-2001 by
 *
 *      Bjørn Stabell        <bjoern@xpilot.org>
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
 * $Log: IniXPilot.h,v $
 * Revision 1.6  2004/05/11 06:47:56  dick
 * Load/Save the PrivateServer's servers.
 *
 * Revision 1.5  2004/05/10 12:43:30  dick
 * Handle Inet list and Private list
 *
 * Revision 1.4  2002/09/12 20:01:01  dick
 * Whitespace
 *
 * Revision 1.3  2002/09/07 17:53:55  dick
 * Add attribute - bool localServerAdvanced.
 *
 * Revision 1.2  2002/09/03 07:45:32  dick
 * Remove bad curDB (defined in the base class)
 *
 * Revision 1.1  2002/09/02 16:19:20  dick
 * Subclass Ini and provide configuration for XPilot.exe
 *
 */
#ifndef	_INIXPILOT_H_
#define	_INIXPILOT_H_

#include <stdio.h>

#include "cstring.h"
#include "Fl_ServerList.h"

#include "Ini.h"

extern	PCSTR s_LocalServerAdvanced;
extern	PCSTR s_ColumnServerName;
extern	PCSTR s_ColumnMapName;
extern	PCSTR s_ColumnMapAuthor;
extern	PCSTR s_ColumnMapSize;
extern	PCSTR s_ColumnNumPlayers;
extern	PCSTR s_ColumnNumBases;
extern	PCSTR s_ColumnLag;
extern	PCSTR s_ColumnNumTeams;
extern	PCSTR s_ColumnFPS;
extern	PCSTR s_ColumnVersion;
extern	PCSTR s_ColumnSound;
extern	PCSTR s_ColumnUptime;
extern	PCSTR s_ColumnIP;
extern	PCSTR s_ColumnRaceMode;
extern	PCSTR s_ColumnPort;
extern	PCSTR s_ColWidthServerName;
extern	PCSTR s_ColWidthMapName;
extern	PCSTR s_ColWidthMapAuthor;
extern	PCSTR s_ColWidthMapSize;
extern	PCSTR s_ColWidthNumPlayers;
extern	PCSTR s_ColWidthNumBases;
extern	PCSTR s_ColWidthLag;
extern	PCSTR s_ColWidthNumTeams;
extern	PCSTR s_ColWidthFPS;
extern	PCSTR s_ColWidthVersion;
extern	PCSTR s_ColWidthSound;
extern	PCSTR s_ColWidthUptime;
extern	PCSTR s_ColWidthIP;
extern	PCSTR s_ColWidthRaceMode;
extern	PCSTR s_ColWidthPort;

extern	PCSTR s_PrivateServerCount;
extern	PCSTR s_PrivateServerStar;

extern	PCSTR s_inet;
extern	PCSTR s_priv;

///////////////////////////////////////////////////////////////////////////////
class IniPrivateServer : public Obj
{
public:
	String	ip;
	int		port;
};

///////////////////////////////////////////////////////////////////////////////
class IniXPilot : public Ini
{
public:
	IniXPilot();
	~IniXPilot();


	virtual	void Save(FILE* fp);

	void	SetDefaultOptions();
	void	ParseOptions(int *argcp, char **argvp);
	void	ParseSwitches(int *argcp, char **argvp);

public:
	bool			localServerAdvanced;
	ServerListCfg	inetSlCfg;
	ServerListCfg	privateSlCfg;
	ObjList			privateServerList;
	int				privateServersLoadedCount;	// keep this in case we have to delete any when saving

private:
	void	LoadPanelSettings(PCSTR prefix, ServerListCfg& cfg);
	void	SavePanelSettings(FILE* fp, PCSTR prefix, const ServerListCfg& cfg);
};

extern	IniXPilot	iniXPilot;	// each app has one instance of an IniXPilot

#endif		// _INIXPILOT_H_
