/***************************************************************************\
*  ServerSite.h : Describe a single server                                  *
*  $Id: ServerSite.h,v 1.14 2005/03/17 22:12:13 kps Exp $   				*
*                                                                           *
*  Copyright© 1994-2001 by                                                  *
*      Dick Balaska         <dick@xpilot.org>                               *
*      Jarrod Miller        <jarrod@xpilot.org>                             *
*      Bert Gijsbers        <bert@xpilot.org>                               *
*      Ken Ronny Schouten   <ken@xpilot.org>                                *
*      Bjørn Stabell        <bjoern@xpilot.org>                             *
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
 *  $Log: ServerSite.h,v $
 *  Revision 1.14  2005/03/17 22:12:13  kps
 *  Get rid of warnings from makedepend about "non-portable whitespace".
 *
 *  Revision 1.13  2004/05/14 21:15:35  dick
 *  Handle PrivateServerList updates
 *
 *  Revision 1.12  2004/05/13 09:58:24  dick
 *  gcc3 doesn't allow forward (empty) enums
 *
 *  Revision 1.11  2004/05/13 09:12:36  dick
 *  Parse a status message from the server
 *
 *  Revision 1.10  2004/05/10 06:26:47  dick
 *  Constructor clears some data
 *
 *  Revision 1.9  2004/01/06 04:48:28  dick
 *  Add support for building under Cygwin.  Still needs work and the output
 *  is X11 and requires something like Cygwin's XWin.exe to run under.
 *
 *  Revision 1.8  2002/08/30 05:18:02  dick
 *  New ServerSite::Sort, currently only supports lag (column 6)
 *
 *  Revision 1.7  2002/08/28 13:58:38  dick
 *  Linux needs sys/time.h
 *
 *  Revision 1.6  2002/08/28 13:31:54  dick
 *  Add attributes for measuring lag.
 *
 *  Revision 1.5  2002/06/09 21:01:13  dick
 *  ANSI C++ forbids declaration `operator =' with no type
 *
 *  Revision 1.4  2002/06/09 18:35:42  dick
 *  Parse players correctly.  Player and Watch become ServerSitePlayer and
 *  ServerSiteWatch.
 *
 *  Revision 1.3  2002/06/07 15:44:16  dick
 *  Fix parsing of the string from meta.
 *
 *  Revision 1.2  2001/07/26 16:22:07  dick
 *  White space.
 *
 *  Revision 1.1  2001/07/26 10:55:27  dick
 *  Describe a single server as known by meta
 *
 */

#ifndef	_SERVERSITE_H_
#define	_SERVERSITE_H_

#if defined(_WINDOWS) && !defined(_CYGWIN)
#	include <sys/types.h>
#	include <sys/timeb.h>
#else
#	include <sys/time.h>
#endif

#include "FL/Enumerations.H"

#include "cstring.h"
#include "Obj.h"
#include "Sockbuf.h"

#define	MAX_TEAMS	10
class Watch;

#define PING_UNKNOWN	10000	/* never transmitted a ping to it */	
#define PING_NORESP		 9999	/* never responded to our ping */
#define PING_SLOW		 9998	/* responded to first ping after
								 * we had already retried (ie slow!) */

#define	ServerSiteSUPERCLASS	Obj
class ServerSite  : public ServerSiteSUPERCLASS
{
public:
	ServerSite();
	~ServerSite();
	ServerSite& operator=(const ServerSite& _ss);
	virtual	bool	Sort(ObjSortable* other, int priSort = 0, int secSort = 0);


	bool	ParseLine(const String& line);		// parse a line from the meta server
	bool	ParseStatus(const String& line);	// parse a status report from a server
#ifdef	_DEBUG
	void	TRACEdump();
#endif
// attributes from the meta server
// 3.4.100:gotland.gmd.de:15345:2:TheBreeder's Bouncy Tank Party 3:100x100:Breeder & Hass:ok:6:14:Warp=andy@baltrum.gmd.de,TheBrave=dirk@sark.gmd.de:no:64:0:0:129.26.12.138
// 4.0.0beta-NT12:duke.buckosoft.com:15345:0:Wacky 3.8.0:100x100:Bucko:ok:14:12::no:61193:3:0:204.249.240.106:1=3,2=3:0
	String		version;
	String		serverName;
	String		serverIP;
	int			serverPort;
	int			playerCount;
	String		mapName;
	String		mapSize;
	String		author;
	String		serverStatus;
	int			bases;
	int			fps;
	ObjList		playerList;
	String		sound;
	long		uptime;
	int			teams;
	int			timing;
	int			freebases[MAX_TEAMS];
	int			playersQueued;

// watch attributes
	bool		watched;

// draw attribute
	Fl_Color	color;

// attributes for PrivateServerList
	bool		inactive;
	void		SetActive(bool active=true);
	time_t		lastChecked;

// attributes for InetServerList
	int			playerCountChanged;	// please draw highlighting
	bool		mapChanged;

#define	CHANGEDNONE		0
#define	CHANGEDLESS		1
#define	CHANGEDMORE		2

	bool		refreshed;				// RecvData has touched this one
//	bool		drawn_refreshed;		// draw has displayed this in highlights

	int			status;
	bool		GetNew() { return(status == 2); };
	void		SetNew() { status = 2; };
	bool		GetDel() { return(status == 1); };
	void		SetDel() { status = 1; };
	bool		GetNormal() { return(status == 0); };
	void		SetNormal() { status = 0; };

// attributes for ping time handling
#if defined(_WINDOWS) && !defined(_CYGWIN)
	struct _timeb		start;
#else
	struct timeval		start;
#endif
	int			pingTime;
	uchar		serial;

private:
	bool		ParsePlayers(const String& line);
	bool		ParseFreeBases(String line);

};

///////////////////////////////////////////////////////////////////////////////
class ServerSitePlayer  : public Obj
{
public:
	ServerSitePlayer() {};
	ServerSitePlayer(const ServerSitePlayer&);
	ServerSitePlayer& operator=(const ServerSitePlayer&);
	
	String	name;

	int		status;
	bool	drawn_status;
	bool	GetNew() { return(status == 2); };
	void	SetNew() { status = 2; };
	bool	GetDel() { return(status == 1); };
	void	SetDel() { status = 1; };
	bool	GetNormal() { return(status == 0); };
	void	SetNormal() { status = 0; };
};

///////////////////////////////////////////////////////////////////////////////
class ServerSiteWatch  : public Obj
{
public:
	ServerSiteWatch() {};
	ServerSiteWatch(ServerSite* csi);
	bool		operator==(ServerSite& csi);

	String		serverName;
	int			serverPort;
//	StringList	players;		// used by dings 
};


#endif
