/* $Id: ConnectionControlScoreServer.cpp,v 1.19 2007/02/03 09:20:55 dick Exp $
 *
 * Describe a server's connection to a control.
 *
 * XPilot, a multiplayer gravity war game.  Copyright (C) 1991-2001 by
 *
 *      Dick Balaska         <dick@xpilot.org>
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
 *  $Log: ConnectionControlScoreServer.cpp,v $
 *  Revision 1.19  2007/02/03 09:20:55  dick
 *  ScoreServer is deleted in superclass, don't double delete it
 *
 *  Revision 1.18  2007/02/03 05:36:33  dick
 *  hasWebHost becomes hasWebInfo and display the message even with no host
 *
 *  Revision 1.17  2007/01/10 18:14:47  dick
 *  All robot actions are now handled through RobotMan.
 *  There is one RobotMan per World.
 *
 *  Revision 1.16  2004/05/31 18:17:25  dick
 *  virtual destructors so subclasses don't leak.
 *
 *  Revision 1.15  2004/05/22 15:12:34  dick
 *  Remove #ifndef SILENT
 *
 *  Revision 1.14  2004/05/07 04:27:41  dick
 *  _visibility becomes Visibility.  global updateScores becomes a member of World.
 *  Handle rank/rate data from the scoreserver and send it to the client.
 *
 *  Revision 1.13  2004/02/16 14:29:44  dick
 *  server/sched.cpp and friends become serverlib/NetServer
 *  which disconnects the udp listen and receive functionality from the game.
 *
 *  Revision 1.12  2004/02/03 03:39:53  dick
 *  Add Windows precompiled header support.
 *
 *  Revision 1.11  2002/09/09 23:42:55  dick
 *  Handle connecting a LocalCtl to this server.
 *
 *  Revision 1.10  2002/08/01 14:38:30  dick
 *  Receive the player's cookie from the ScoreServer along with his score.
 *
 *  Revision 1.9  2002/07/26 08:27:38  dick
 *  Don't log the robot's score (to the log) if logRobots is false.
 *
 *  Revision 1.8  2002/07/14 19:05:24  dick
 *  When talking to the ScoreServer about PCSTR nick, always mention the PlayerType too.
 *
 *  Revision 1.7  2002/07/09 21:44:29  dick
 *  Send the player type (human,robot,tank) with the PlayerEvent to the ScoreServer.
 *
 *  Revision 1.6  2002/07/08 05:53:00  dick
 *  showtime() handles the space after itself
 *
 *  Revision 1.5  2002/06/26 17:40:28  dick
 *  Initialize attribute 'active'.
 *
 *  Revision 1.4  2002/06/26 07:26:17  dick
 *  Call it ServerActivate instead of Activate.
 *
 *  Revision 1.3  2002/06/26 06:59:50  dick
 *  Handle the Activate command from the ScoreServer, and don't send ScoreEvents
 *  unless we have been activated.
 *
 *  Revision 1.2  2002/06/25 04:48:29  dick
 *  Add SendScoreEvent() and send each score event to the ScoreServer.
 *
 *  Revision 1.1  2002/06/23 06:36:29  dick
 *  Handle connecting a server to a ScoreServer
 *
 */
#include "StdAfx.h"

#include <string.h>

#include "config.h"
#include "serverconst.h"
#include "global.h"
#include "proto.h"
#include "showtime.h"
#include "error.h"
#include "ConnectionPlayer.h"
#include "ConnectionControlScoreServer.h"
#include "packet.h"
#include "PacketCtl.h"
#include "World.h"

PCSTR	s_ScoreServer = "ScoreServer";

///////////////////////////////////////////////////////////////////////////////
ConnectionControlScoreServer::ConnectionControlScoreServer() {
	ctl = s_ScoreServer;
	hasWebInfo = false;
	active = false;
}

///////////////////////////////////////////////////////////////////////////////
ConnectionControlScoreServer::~ConnectionControlScoreServer() {
}

///////////////////////////////////////////////////////////////////////////////
void ConnectionControlScoreServer::DestroyConnection(PCSTR reason) {
	world->scoreServer = NULL;
	xpprintf("%sScoreServer disconnected\n", showtime());
	ConnectionControlScoreServerSUPERCLASS::DestroyConnection(reason);
}

///////////////////////////////////////////////////////////////////////////////
int ConnectionControlScoreServer::ReceiveServerActivate() {
	char	on;
	int		n;
    if ((n = cr.scanf("%c", &on)) <= 0)	{
		if (n == -1)
			DestroyConnection("read error, Activate needs 'On'");
		return n;
    }
	active = on ? true : false;
	if (active)	{
		world->ScoreServerRequestPlayerRanks();
	}
	return(TRUE);
}

///////////////////////////////////////////////////////////////////////////////
int ConnectionControlScoreServer::ReceiveGetScore() {
	char		name[MAX_CHARS];
	char		ptc;
	int			score;
	int			kills;
	int			deaths;
	uint		cookie;
	PlayerType	pt;
	int			n;
    if ((n = cr.scanf("%s%c%d%d%d%u", &name, &ptc, &score, &kills, &deaths, &cookie)) <= 0)
	{
		if (n == -1)
			DestroyConnection("read error, GetScore needs name/score");
		return n;
    }
	pt = (PlayerType)ptc;
	if (!(pt == PlayerRobot) || world->options.logRobots->GetBool())
		xpprintf("%sPlayer %s starts with score %.2f\n", 
				 showtime(), name, (DFLOAT)(score/100.0));
	world->SetPlayerScore(name, pt, (DFLOAT)(score/100.0), kills, deaths, cookie);
	//world->SetPlayerRank(name, pt, 
	return(TRUE);
}

///////////////////////////////////////////////////////////////////////////////
int ConnectionControlScoreServer::ReceiveGetRanks() {
	char		name[MAX_CHARS];
	char		ptc;
	int			rank;
	int			rate;
	PlayerType	pt;
	int			n;
    if ((n = cr.scanf("%s%c%d%d", &name, &ptc, &rank, &rate)) <= 0)
	{
		if (n == -1)
			DestroyConnection("read error, GetRanks needs name/rank");
		return n;
    }
	pt = (PlayerType)ptc;
	world->SetPlayerRank(name, pt, rank, (DFLOAT)((double)rate/100.0));
	return(TRUE);
}

///////////////////////////////////////////////////////////////////////////////
int ConnectionControlScoreServer::ReceivePlayerEvent() {
	return(TRUE);
}

///////////////////////////////////////////////////////////////////////////////
int ConnectionControlScoreServer::ReceiveSetScoreEvent() {
	return(TRUE);
}

///////////////////////////////////////////////////////////////////////////////
int ConnectionControlScoreServer::ReceiveWebInfo() {
	char	name[MAX_CHARS];
	int		port;
	char	msg[MAX_CHARS];
	int		n;

    if ((n = cr.scanf("%s%d%s", &name, &port, &msg)) <= 0) {
		if (n == -1)
			DestroyConnection("read error, Webinfo needs host/port");
		return n;
    }
	webHost = name;
	webPort = port;
	webMsg = msg;
	hasWebInfo = true;
	return(TRUE);
}

///////////////////////////////////////////////////////////////////////////////
String ConnectionControlScoreServer::GetWebInfo() {
	String	s;
	if (!hasWebInfo)
		return(s);
	if (!webHost.IsEmpty()) {
		s.printf("http://%s:%d/", (PCSTR)webHost, webPort);
		if (!webMsg.IsEmpty())
			s += " - ";
	}
	if (!webMsg.IsEmpty())
		s += webMsg;
	return(s);
}

///////////////////////////////////////////////////////////////////////////////
int ConnectionControlScoreServer::RequestPlayerScore(Player* pl) {
	uint	cookie = 0;
	if (pl->conn)
		cookie = pl->conn->cookie;
	cw.printf("%c%c%s%c%u", PKT_CTL, GetScore, pl->name, pl->playerType, cookie);
	return(TRUE);
}

///////////////////////////////////////////////////////////////////////////////
int ConnectionControlScoreServer::RequestPlayerRanks(bool onOff) {
	cw.printf("%c%c%c", PKT_CTL, GetRanks, onOff ? 1 : 0);
	return(TRUE);
}

#if 0
///////////////////////////////////////////////////////////////////////////////
void ConnectionControlScoreServer::SendPlayerScore(PCSTR nick, DFLOAT score, int kills, int deaths)
{
	cw.printf("%c%c%s%d%d%d", PKT_CTL, SendScore, nick, 
		(int)(score * 100 + (score > 0 ? 0.5 : -0.5)),
		kills, deaths);
	return;
}
#endif

///////////////////////////////////////////////////////////////////////////////
void ConnectionControlScoreServer::SendScoreEvent(PCSTR killer, PlayerType ptr, DFLOAT wscore,
												  PCSTR killee, PlayerType pte, DFLOAT lscore, 
												  ScoreType st)
{
	cw.printf("%c%c%s%c%s%c%d%d%c", PKT_CTL, ScoreEvent,
		killer, ptr, killee, pte, 
		(int)(wscore * 100 + (wscore > 0 ? 0.5 : -0.5)),
		(int)(lscore * 100 + (lscore > 0 ? 0.5 : -0.5)),
		st);
}

///////////////////////////////////////////////////////////////////////////////
void ConnectionControlScoreServer::SendPlayerEvent(Player* pl, ScorePlayerEvent spe)
{
	Connection* conn = pl->conn;
	cw.printf("%c%c%s%c%c", PKT_CTL, PlayerEvent, pl->name, pl->playerType, spe);
	if (spe == Join && pl->playerType == PlayerHuman)
		cw.printf("%u%s%s%s", conn->cookie, (PCSTR)conn->real, (PCSTR)conn->host, (PCSTR)conn->addr);
}
