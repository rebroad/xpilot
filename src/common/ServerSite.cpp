/***************************************************************************\
*  ServerSite.cpp : Describe a single server                                *
*  $Id: ServerSite.cpp,v 1.16 2004/05/30 08:15:03 dick Exp $ 				*
*                                                                           *
*  Copyright� 1994-2001 by                                                  *
*      Dick Balaska         <dick@xpilot.org>                               *
*      Bert Gijsbers        <bert@xpilot.org>                               *
*      Ken Ronny Schouten   <ken@xpilot.org>                                *
*      Bj�rn Stabell        <bjoern@xpilot.org>                             *
*                                                                           *
* This program is free software; you can redistribute it and/or modify      *
* it under the terms of the GNU General Public License as published by      *
* the Free Software Foundation; either version 2 of the License, or         *
* (at your option) any later version.                                       *
*                                                                           *
* This program is distributed in the hope that it will be useful,           *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU General Public License for more details.                              *
*                                                                           *
* You should have received a copy of the GNU General Public License         *
* along with this program; if not, write to the Free Software               *
* Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.                 *
\***************************************************************************/
/*
 *  $Log: ServerSite.cpp,v $
 *  Revision 1.16  2004/05/30 08:15:03  dick
 *  Add support for teams in the ServerInfo (PrivatePanel) parsing.
 *
 *  Revision 1.15  2004/05/14 21:15:35  dick
 *  Handle PrivateServerList updates
 *
 *  Revision 1.14  2004/05/13 09:12:36  dick
 *  Parse a status message from the server
 *
 *  Revision 1.13  2004/05/10 06:26:47  dick
 *  Constructor clears some data
 *
 *  Revision 1.12  2004/05/07 04:29:15  dick
 *  Sort some columns backwards to be more useful
 *
 *  Revision 1.11  2004/02/10 18:19:32  dick
 *  Sort on any field
 *
 *  Revision 1.10  2004/02/03 03:39:52  dick
 *  Add Windows precompiled header support.
 *
 *  Revision 1.9  2002/08/30 05:34:36  dick
 *  Add Sort().  Currently only sorts by lag.
 *
 *  Revision 1.8  2002/08/28 13:31:54  dick
 *  Add attributes for measuring lag.
 *
 *  Revision 1.7  2002/08/15 18:46:19  jlmiller
 *  Fix ParseFreeBases typo.
 *
 *  Revision 1.6  2002/06/09 21:07:45  dick
 *  Must return a type.
 *
 *  Revision 1.4  2002/06/09 18:35:42  dick
 *  Parse players correctly.  Player and Watch become ServerSitePlayer and
 *  ServerSiteWatch.
 *
 *  Revision 1.3  2002/06/07 15:44:16  dick
 *  Fix parsing of the string from meta.
 *
 *  Revision 1.2  2001/07/29 23:40:49  dick
 *  Linux gcc tweaks.  The newer versions of g++ has stricter (and different)
 *  requirements.
 *
 *  Revision 1.1  2001/07/26 10:55:27  dick
 *  Describe a single server as known by meta
 *
 */
#include "StdAfx.h"

#include <string.h>
#include <stdlib.h>

#include "FL/Enumerations.H"

#include "Fl_XPColors.h"

#include "ServerSite.h"
#include "ServerListCfg.h"

ServerSite::ServerSite()
{
	pingTime = PING_UNKNOWN;
	playerCount = 0;
	uptime	= 0;
	teams	= 0;
	bases	= 0;
	fps		= 0;
	timing	= 0;
	color	= FL_WHITE;
	inactive = false;
	lastChecked = 0;
	memset(freebases, 0, MAX_TEAMS * sizeof(int));
}

///////////////////////////////////////////////////////////////////////////////
ServerSite::~ServerSite()
{
	/*
	Player*	cp;

	while (!playerlist.IsEmpty())
	{
		cp = (Player*)playerlist.RemoveHead();
		delete cp;
	}
	*/
}

///////////////////////////////////////////////////////////////////////////////
ServerSite& ServerSite::operator=(const ServerSite& _ss)
{
	version			= _ss.version;
	serverName		= _ss.serverName;
	serverPort		= _ss.serverPort;
	playerCount		= _ss.playerCount;
	mapName			= _ss.mapName;
	mapSize			= _ss.mapSize;
	author			= _ss.author;
	serverStatus	= _ss.serverStatus;
	bases			= _ss.bases;
	fps				= _ss.fps;
	sound			= _ss.sound;
	uptime			= _ss.uptime;
	teams			= _ss.teams;
	timing			= _ss.timing;
	serverIP		= _ss.serverIP;
	playersQueued	= _ss.playersQueued;

	status			= _ss.status;
	watched			= _ss.watched;

	playerCountChanged = _ss.playerCountChanged;
	mapChanged		= _ss.mapChanged;

	refreshed		= _ss.refreshed;

	color			= _ss.color;
	inactive		= _ss.inactive;

	//playerlist	= _ss.playerlist;
	playerList.Empty();
	ServerSitePlayer const* _ssp;
	const ObjList&	ol = _ss.playerList;
	_ssp = (ServerSitePlayer*)ol.GetHead();
	while (_ssp)
	{
		ServerSitePlayer* ssp = new ServerSitePlayer;
		*ssp = *_ssp;
		playerList.Add(ssp);
		_ssp = (ServerSitePlayer*)_ssp->GetNext();
	}

	int i;
	for (i=0; i<MAX_TEAMS; i++)
		freebases[i]		= _ss.freebases[i];

	return(*this);
}

///////////////////////////////////////////////////////////////////////////////
// The list is in the following format:
// version:hostname:port number:number of users:map name:map size:map author
// :server status:number of home bases:frames per second:players list:sound
// :server uptime:number of team bases:timing:ip number:free bases:players queued
bool	ServerSite::ParseLine(const String& line)
{
//	TRACE("ServerSite::ParseLine <%.250s>\n", (const char*)*line);

	String	par;
	int		i;

	watched = false;
	mapChanged = false;

	i = line.Find(':');
	version = line.Left(i);
	par = line.Mid(i+1);

	i = par.Find(':');
	serverName = par.Left(i);
	par = par.Mid(i+1);

	i = par.Find(':');
	serverPort = atoi((const char*)par.Left(i));
	par = par.Mid(i+1);

	i = par.Find(':');
	playerCount = atoi((const char*)par.Left(i));
	par = par.Mid(i+1);

	i = par.Find(':');
	mapName = par.Left(i);
	par = par.Mid(i+1);

	i = par.Find(':');
	mapSize = par.Left(i);
	par = par.Mid(i+1);

	i = par.Find(':');
	author = par.Left(i);
	par = par.Mid(i+1);

	i = par.Find(':');
	serverStatus = par.Left(i);
	par = par.Mid(i+1);

	i = par.Find(':');
	bases = atoi((const char*)par.Left(i));
	par = par.Mid(i+1);

	i = par.Find(':');
	fps = atoi((const char*)par.Left(i));
	par = par.Mid(i+1);

	i = par.Find(':');
	ParsePlayers(par.Left(i));
	par = par.Mid(i+1);

	i = par.Find(':');
	sound = par.Left(i);
	par = par.Mid(i+1);

	i = par.Find(':');
	uptime = atoi((const char*)par.Left(i));
	par = par.Mid(i+1);

	i = par.Find(':');
	teams = atoi((const char*)par.Left(i));
	par = par.Mid(i+1);

	i = par.Find(':');
	timing = atoi((const char*)par.Left(i));
	par = par.Mid(i+1);

	i = par.Find(':');
	serverIP = par.Left(i);
	par = par.Mid(i+1);

	i = par.Find(':');
	ParseFreeBases(par.Left(i));
	par = par.Mid(i+1);

	playersQueued = atoi((const char*)par.Left(i));
//	serverIP = par.Left(par.GetLength());
//	TRACE("ServerSite::ParseLine done\n");

	return(true);
}

///////////////////////////////////////////////////////////////////////////////
bool	ServerSite::ParseFreeBases(String line)
{
	int		i;
	int		freet, freeb;
	String	copy;
	char*	par;

	for (i=0; i<MAX_TEAMS; i++)
		freebases[i] = 0;
	if (version < "4.0")
	{
		if (teams)
			for (i=0; i<MAX_TEAMS; i++)
				freebases[i] = 1;
	}
	while (!line.IsEmpty())
	{
		par = strchr(line, '=');
		if (!par)
			break;
		*par++ = '\0';
		freet = atoi(line);
		if (freet > MAX_TEAMS)
		{
			break;
		}
		copy = par;
		par = strchr(copy, ',');
		if (!par)
		{
			freeb = atoi(copy);
			line = "";
		}
		else
		{
			*par++ = '\0';
			freeb = atoi(copy);
			line = par;
		}
		freebases[freet] = freeb;

	}
	return(true);
}

///////////////////////////////////////////////////////////////////////////////
bool	ServerSite::ParsePlayers(const String& _line)
{

	if (_line.IsEmpty())
		return(true);
	String line(_line);
	int		i;
	ServerSitePlayer* cp;

	while (1)
	{
		cp = new ServerSitePlayer;
		i = line.Find(',');
		if (i == -1)
			break;
		cp->name = line.Left(i);
		line = line.Mid(i+1);
		playerList.Add(cp);
	}
	cp->name = line;
	playerList.Add(cp);
//	TRACE("ServerSite::ParsePlayers done\n");
	return(true);
}

///////////////////////////////////////////////////////////////////////////////
/* Status request from a server looks like this:
SERVER VERSION...: 5.0.0a8
STATUS...........: ok
MAX SPEED........: 14 fps
WORLD ( 65x 51)..: Fishfight
      AUTHOR.....: Jon Moore
PLAYERS ( 5/16)..:
TEAM BASES.......: 1=3,2=3					<-- 5.0 only

NO:  TM: NAME:             LIFE:   SC:    PLAYER:
-------------------------------------------------
 1... *  C3PO               044    22     robot@xpilot.org
 2... R  Sparky (C3PO)      015  -119     robot@xpilot.org
 3... R  Dalt Wisney        009  -154     robot@xpilot.org
 4... R  R2D2               023  -175     robot@xpilot.org
 5... R  Azurion            011  -181     robot@xpilot.org
 2... R0 R2D2               002    -3     robot@xpilot.org       <-- robot on team 0
*/
///////////////////////////////////////////////////////////////////////////////
bool ServerSite::ParseStatus(const String& _line)
{
	String	line;
	String	s;
	String	w;
	int		x, y;
	int		i;

	line = _line;
	int		index;

	// SERVER VERSION...: 5.0.0a8
	index = line.Find('\n');
	if (index == -1)
		return(false);
	w = line.Left(index);
	line = line.Mid(index+1);
	if (w.GetLength() < 19)
		return(false);
	version = w.Mid(19);

	// STATUS...........: ok
	index = line.Find('\n');
	if (index == -1)
		return(false);
	w = line.Left(index);
	line = line.Mid(index+1);
	if (w.GetLength() < 19)
		return(false);
	serverStatus = w.Mid(19);
	if (serverStatus == "ok")
		SetActive();

	// MAX SPEED........: 14 fps
	index = line.Find('\n');
	if (index == -1)
		return(false);
	w = line.Left(index);
	line = line.Mid(index+1);
	if (w.GetLength() < 19)
		return(false);
	w = w.Mid(19);
	fps = w.atoi();

	// WORLD ( 65x 51)..: Fishfight
	index = line.Find('\n');
	if (index == -1)
		return(false);
	w = line.Left(index);
	line = line.Mid(index+1);
	if (w.GetLength() < 19)
		return(false);
	mapName = w.Mid(19);

	s = w.Mid(7,3);
	x = s.atoi();
	s = w.Mid(11,3);
	y = s.atoi();
	s.printf("%dx%d", x, y);
	mapSize = s;

	//       AUTHOR.....: Jon Moore
	index = line.Find('\n');
	if (index == -1)
		return(false);
	w = line.Left(index);
	line = line.Mid(index+1);
	if (w.GetLength() < 19)
		return(false);
	w = w.Mid(19);
	author = w;

	// PLAYERS ( 5/16)..:
	index = line.Find('\n');
	if (index == -1)
		return(false);
	w = line.Left(index);
	line = line.Mid(index+1);
	s = w.Mid(9,2);
	playerCount = s.atoi();
	s = w.Mid(12,2);
	bases = s.atoi();

	// TEAM BASES.......: 1=3,2=3					<-- 5.0 only
	index = line.Find('\n');
	if (index == -1)
		return(false);
	w = line.Left(index);
	line = line.Mid(index+1);
	if (!strncmp(w, "TEAM BASES", 10))	// if it's not TEAM BASES, then skip this line
	{
		w = w.Mid(19);
		index = w.Find(',');
		if (index == -1)
			return(false);
		s = w.Left(index);
		teams = s.atoi();
		w = w.Mid(index+1);
		ParseFreeBases(w);
		index = line.Find('\n');		// skip an extra line, cause we wanted 3 skips below
		if (index == -1)
			return(false);
		line = line.Mid(index+1);

	}

	//
	// NO:  TM: NAME:             LIFE:   SC:    PLAYER:
	// -------------------------------------------------
	index = line.Find('\n');	// skip 3 lines, no 2 lines one was skipped above at team bases
	if (index == -1)
		return(false);
	line = line.Mid(index+1);

	index = line.Find('\n');
	if (index == -1)
		return(false);
	line = line.Mid(index+1);


	//  1... *  C3PO               044    22     robot@xpilot.org
	playerList.Empty();
	ServerSitePlayer* cp;
	for (i=0; i<playerCount; i++)
	{
		index = line.Find('\n');
		if (index == -1)
			return(false);
		w = line.Left(index);
		line = line.Mid(index+1);

		cp = new ServerSitePlayer;
		s = w.Mid(9,18);
		s.TrimWhiteSpace();
		cp->name.printf("%s=%s", (PCSTR)s, (PCSTR)w.Mid(42));
		playerList.Add(cp);
	}

	return(true);
}

///////////////////////////////////////////////////////////////////////////////
#ifdef	_DEBUG
void	ServerSite::TRACEdump()
{
#if 0
	TRACE("ServerSite:: serverName=<%s> map=<%s> version=<%s>\n",
		(const char*)serverName, (const char*)mapName, (const char*)version);
	TRACE("ServerSite:: playerCount=%d, fps=%d serverIP=<%s>\n",
		playerCount, fps, (const char*)serverIP);
#endif
}
#endif


///////////////////////////////////////////////////////////////////////////////
bool ServerSite::Sort(ObjSortable* other, int priSort, int secSort)
{
	ServerSite* oss = (ServerSite*)other->o;

	switch (priSort)
	{
	case COL_SERVERNAME:
		return (strcmp(this->serverName, oss->serverName) > 0 ? true : false);
	case COL_MAPNAME:
		return (strcmp(this->mapName, oss->mapName) > 0 ? true : false);
	case COL_MAPAUTHOR:
		return (strcmp(this->author, oss->author) > 0 ? true : false);
	case COL_MAPSIZE:
		return (strcmp(this->mapSize, oss->mapSize) < 0 ? true : false);
	case COL_PLAYERCOUNT:
		return (this->playerCount < oss->playerCount);
	case COL_LAG:
		return (this->pingTime > oss->pingTime);
	case COL_BASES:
		return (this->bases < oss->bases);
	case COL_TEAMS:
		return (this->teams < oss->teams);
	case COL_FPS:
		return (this->fps > oss->fps);
	case COL_VERSION:
		return (strcmp(this->version, oss->version) < 0 ? true : false);
	case COL_SOUND:
		return (strcmp(this->sound, oss->sound) < 0 ? true : false);
	case COL_UPTIME:
		return (this->uptime < oss->uptime);
	case COL_IP:
		return (strcmp(this->serverIP, oss->serverIP) > 0 ? true : false);
	case COL_RACE:
		return (this->timing < oss->timing);
	case COL_PORT:
		return (this->serverPort > oss->serverPort);
	}
	return(ServerSiteSUPERCLASS::Sort(other, priSort, secSort));
}

///////////////////////////////////////////////////////////////////////////////
void ServerSite::SetActive(bool a)
{
	if (a)
	{
		inactive = false;
		color = xpwhite;
	}
	else
	{
		inactive = true;
		color = xpgrey;
		playerList.Empty();
		playerCount = 0;
		mapName.Empty();
		mapSize.Empty();
		author.Empty();
		serverStatus.Empty();
		version.Empty();
		bases = 0;
		timing = 0;
		fps = 0;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ServerSitePlayer& ServerSitePlayer::operator=(const ServerSitePlayer& ssp)
{
	name			= ssp.name;
	status			= ssp.status;
	drawn_status	= ssp.drawn_status;
	return(*this);
}

///////////////////////////////////////////////////////////////////////////////
ServerSitePlayer::ServerSitePlayer(const ServerSitePlayer& ssp)
{
	name = ssp.name;
	SetNew();
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ServerSiteWatch::ServerSiteWatch(ServerSite* csi)
{
	serverName = csi->serverName;
	serverPort = csi->serverPort;
}

///////////////////////////////////////////////////////////////////////////////
bool ServerSiteWatch::operator==(ServerSite& csi)
{
	if (serverName == csi.serverName && serverPort == csi.serverPort)
		return(true);
	return(false);
}
