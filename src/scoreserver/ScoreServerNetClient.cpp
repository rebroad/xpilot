/* $Id: ScoreServerNetClient.cpp,v 1.14 2004/05/07 04:38:25 dick Exp $
 *
 * ScoreServerNetClient - connect a ScoreServer to a server
 *
 *      Copyright (C) 2002 by
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
 *
 * See the file COPYRIGHT.TXT for current copyright information.
 *
 */
/*
 * $Log: ScoreServerNetClient.cpp,v $
 * Revision 1.14  2004/05/07 04:38:25  dick
 * Send rank data to the server.
 *
 * Revision 1.13  2004/02/05 04:55:43  dick
 * Download the ServerOptions from the server and when done, if the servername
 * is empty, then request the running host name from the server with a ServerInfo call.
 * (So it matches what we're reporting to meta).
 *
 * Revision 1.12  2004/02/03 03:39:53  dick
 * Add Windows precompiled header support.
 *
 * Revision 1.11  2004/01/30 16:02:49  dick
 * Configure the scoreserver via the web interface.
 * Break out the engine configuration to it's own object; we don't want
 * to have the baggage of a big unused score engine loaded in memory if we
 * really are only interested in it's name and description.
 *
 * Revision 1.10  2002/09/10 01:19:02  dick
 * Include <stdlib.h>
 *
 * Revision 1.9  2002/09/09 23:37:13  dick
 * Fix misleading commentary text
 *
 * Revision 1.8  2002/08/01 15:02:21  dick
 * Add support for cookies.
 *
 * Revision 1.7  2002/07/14 19:09:05  dick
 * Whenever we're talking to the server about PCSTR nick, also mention the PlayerType.
 *
 * Revision 1.6  2002/07/09 21:56:15  dick
 * Send the player type (human,robot,tank) with the PlayerEvent
 *
 * Revision 1.5  2002/06/30 06:14:12  dick
 * Receive and log Play messages (chat and system) from the server.
 * This feature was added to aid in debugging the ScoreEvents (to make sure
 * everything lines up) but it might be useful for other things too.
 *
 * Revision 1.4  2002/06/26 07:26:16  dick
 * Call it ServerActivate instead of Activate.
 *
 * Revision 1.3  2002/06/26 07:05:55  dick
 * Add an Activate command handler
 *
 * Revision 1.2  2002/06/26 04:43:01  dick
 * PlayerScoreEvent becomes ScorePlayerEvent
 *
 * Revision 1.1  2002/06/25 05:18:12  dick
 * The ScoreServer's network interface to the server.
 * I subclassed ServerOptionsNetClient instead of ControlClient directly.
 * I was thinking perhaps the ServerOptions could configure the ScoreServer
 * as well. (although most config was done because you already connected the
 * ScoreServer to the server :) .
 *
 */
#include "StdAfx.h"

#ifdef	_WINDOWS
#include <windows.h>
#else
#include <stdlib.h>
#endif

#include "config.h"
#include "const.h"
#include "error.h"
#include "xpprintf.h"
#include "packet.h"
#include "PacketCtl.h"
#include "ScoreServerNetClient.h"
#include "ScoreServer.h"

PCSTR	s_servername	= "servername";

PCSTR	s_ScoreServerNetClient = "ScoreServerNetClient";

PCSTR	playerEvents[] = {
	"Join",
	"Quit",
	"Pause",
	"Unpause",
	"ScorePlayerEventMAX"
};

PCSTR	scoreTypes[] = {
	"ScoreNone",
	"ScoreShot",
	"ScoreTorpedo",
	"ScoreSmart",
	"ScoreHeat",
	"ScoreCluster",
	"ScoreLaser",
	"ScoreTank",
	"ScoreRunover",
	"ScoreBall",
	"ScoreExplode",
	"ScoreShove",
	"ScoreCrash",
	"ScoreMine",
	"ScoreSelf",
	"ScoreSelfDestruct",
	"ScoreAsteroid",
	"ScoreUnknown",

	"ScoreTypeMAX"
};


///////////////////////////////////////////////////////////////////////////////
ScoreServerNetClient::ScoreServerNetClient()
{
	type = s_ScoreServerNetClient;
	sos = NULL;
	SetControlType("ScoreServer");
	if (NELEM(scoreTypes) != ScoreTypeMAX)
		error("scoreTypes table is wrong size");

}


///////////////////////////////////////////////////////////////////////////////
ScoreServerNetClient::~ScoreServerNetClient()
{
	Disconnect();
	delete sos;
}

///////////////////////////////////////////////////////////////////////////////
int ScoreServerNetClient::TimerTick()
{
	return(ScoreServerNetClientSUPERCLASS::TimerTick());
}

///////////////////////////////////////////////////////////////////////////////
bool ScoreServerNetClient::Connect(PCSTR server, int port, PCSTR password)
{
	if (!ScoreServerNetClientSUPERCLASS::Connect(server, port, password))
		return(false);
	rbuf.sock.SetNonBlocking(1);
	Activate();
	return(true);
}

///////////////////////////////////////////////////////////////////////////////
bool ScoreServerNetClient::Activate(bool onoff)
{
	int	c = onoff;
	D(xpprintf("Activating ScoreServer\n");)
	cwbuf.printf("%c%c%c", PKT_CTL, ServerActivate, c);
	SendReliable();
	wbuf.Flush();
	IHaveSpoken();
	return(true);

}

///////////////////////////////////////////////////////////////////////////////
void ScoreServerNetClient::OptionsDownloaded()
{
	String	server;
	bool ret = sos->GetValue(s_servername, server);
	if (!server.GetLength())
		RequestServerInfo();
}

///////////////////////////////////////////////////////////////////////////////
bool ScoreServerNetClient::RequestServerInfo()
{
	D(xpprintf("Requesting ServerInfo\n");)
	cwbuf.printf("%c%c", PKT_CTL, ServerInfo);
	SendReliable();
	wbuf.Flush();
	IHaveSpoken();
	return(true);

}

///////////////////////////////////////////////////////////////////////////////
int ScoreServerNetClient::ReceiveServerInfo()
{
	char	name[MAX_CHARS];
	if (crbuf.scanf("%s", &name) <= 0)
	{
		emh(emhThis, EmError, "ReceiveServerInfo: Can't parse msg");
		return(-1);
	}
	xpprintf(LOGMIN, "%sReceiveServerInfo: %s\n", showtime(), name);
	sos->SetValue(s_servername, name, 1, OPT_COMMAND);	// Really should be OPT_SERVER
	return(1);
}

///////////////////////////////////////////////////////////////////////////////
bool ScoreServerNetClient::SendWebInfo(PCSTR webHost, int webPort, PCSTR webMsg)
{
	D(xpprintf("SendWebInfo: %s:%d\n", webHost, webPort);)
	cwbuf.printf("%c%c%s%d%s", PKT_CTL, WebInfo, webHost, webPort, webMsg);
	SendReliable();
	wbuf.Flush();
	IHaveSpoken();
	return(true);
}

///////////////////////////////////////////////////////////////////////////////
bool ScoreServerNetClient::SendScore(PCSTR nick, PlayerType pt, uint cookie,
									 DFLOAT score, int kills, int deaths)
{
	xpprintf(LOGMED, "Sending score %s %.2f k:%d d:%d\n", nick, score, kills, deaths);
	cwbuf.printf("%c%c%s%c%d%d%d%u", PKT_CTL, GetScore, nick, pt,
		(int)(score * 100 + (score > 0 ? 0.5 : 0.5)),
		kills, deaths, cookie);

	SendReliable();
	wbuf.Flush();
	IHaveSpoken();
	return(true);
}

///////////////////////////////////////////////////////////////////////////////
bool ScoreServerNetClient::SendRank(PCSTR nick, PlayerType pt, 
									int rank, DFLOAT rate)
{
	xpprintf(LOGMED, "Sending rank %s %d %.2f\n", nick, rank, rate);
	cwbuf.printf("%c%c%s%c%d%d", PKT_CTL, GetRanks, nick, pt,
		rank,
		(int)(rate * 100 + (rate > 0 ? 0.5 : 0.5)));

	SendReliable();
	wbuf.Flush();
	IHaveSpoken();
	return(true);
}


///////////////////////////////////////////////////////////////////////////////
int ScoreServerNetClient::ReceiveGetScore()
{
	char	name[MAX_CHARS];
	char	pt;
	uint	cookie;
	if (crbuf.scanf("%s%c%u", &name, &pt, &cookie) <= 0)
	{
		emh(emhThis, EmError, "ReceiveGetScore: Can't parse name");
		return(-1);
	}
	ss->ReceiveGetScore(name, (PlayerType)pt, cookie);
	return(1);
}

///////////////////////////////////////////////////////////////////////////////
int ScoreServerNetClient::ReceiveSetScore()
{
	char	name[MAX_CHARS];
	char	pt;
	int		score;
	int		kills;
	int		deaths;
	uint	cookie;

	if (crbuf.scanf("%s%c%u%d%d%d", &name, &pt, &cookie, 
									&score, &kills, &deaths) <= 0)
	{
		emh(emhThis, EmError, "ReceiveSetScore: Can't parse name");
		return(-1);
	}
	xpprintf(LOGMIN, "Recv score %s %.2f k:%d d:%d\n",
		name, (DFLOAT)(score/100.0), kills, deaths);
	ss->ReceiveSetScore(name, (PlayerType)pt, (DFLOAT)(score/100.0), 
						kills, deaths, cookie);
	return(1);
}

///////////////////////////////////////////////////////////////////////////////
int ScoreServerNetClient::ReceiveGetRanks()
{
	unsigned char	command;
	int				n;

	if ((n = crbuf.scanf("%c", &command)) <= 0)
	{
		emh(emhThis, EmError, "ReceiveGetRanks: Can't parse command");
		return(-1);
	}
	ss->ReceiveGetRanks(command == 1 ? true : false);
	return(1);
}
///////////////////////////////////////////////////////////////////////////////
int ScoreServerNetClient::ReceivePlayerEvent()
{
	char	nick[MAX_CHARS];	// bucko
	char	real[MAX_CHARS];	// dick
	char	host[MAX_CHARS];	// frank.buckosoft.com
	char	addr[MAX_CHARS];	// 204.249.240.117
	uint	cookie;				// as sent from the client to the server (and relayed here)

	char	cpt;
	char	cspe;

	ScorePlayerEvent	spe;
	PlayerType			pt;

	if (crbuf.scanf("%s%c%c", &nick, &cpt, &cspe) <= 0)
	{
		emh(emhThis, EmError, "ReceivePlayerEvent: Can't parse name/spe");
		return(-1);
	}
	spe = (ScorePlayerEvent)cspe;
	pt = (PlayerType)cpt;

	if (spe == Join && pt == PlayerHuman)
	{
		if (crbuf.scanf("%u%s%s%s", &cookie, &real, &host, &addr) <= 0)
		{
			emh(emhThis, EmError, "ReceivePlayerEvent: Can't parse extended info");
			return(-1);
		}
	}
	xpprintf(LOGMIN, "%sPlayerEvent %s%s %s\n",	showtime(), nick,
		pt == PlayerRobot ? "(R)" : "",
		spe < ScorePlayerEventMAX ? playerEvents[spe] : playerEvents[ScorePlayerEventMAX]);
	ss->ReceivePlayerEvent(nick, pt, spe, real, host, addr, cookie);
	return(1);

}

///////////////////////////////////////////////////////////////////////////////
int ScoreServerNetClient::ReceiveScoreEvent()
{
	char		killer[MAX_CHARS];
	char		ptr;
	char		killee[MAX_CHARS];
	char		pte;
	int			wscore;
	int			lscore;
	char		sType;
	ScoreType	st;

	if (crbuf.scanf("%s%c%s%c%d%d%c", &killer, &ptr, &killee, &pte, 
		&wscore, &lscore, &sType) <= 0)
	{
		emh(emhThis, EmError, "ReceiveSetScoreEvent: Can't parse all");
		return(-1);
	}
	st = (ScoreType)sType;
	xpprintf(LOGMED, "%sScoreEvent %s / %s %.2f/%.2f %s\n", showtime(),
		killer, killee, (DFLOAT)(wscore/100.0), (DFLOAT)(lscore/100.0), 
		(st >= 0 && st < ScoreTypeMAX) ? scoreTypes[st] : scoreTypes[ScoreTypeMAX]);
	ss->ReceiveScoreEvent(killer, (PlayerType)ptr, (DFLOAT)(wscore/100.0), 
						  killee, (PlayerType)pte, (DFLOAT)(lscore/100.0), st);
	return(1);
	
}

///////////////////////////////////////////////////////////////////////////////
int ScoreServerNetClient::ReceivePlayMessage()
{
	char	msg[MSG_LEN];
	char	pkt_msg;
	if (crbuf.scanf("%c%S", &pkt_msg, &msg) <= 0)
	{
		emh(emhThis, EmError, "ReceivePlayMessage: Can't parse msg");
		return(-1);
	}
	xpprintf(LOGMIN, "%sPlayMessage: %s\n", showtime(), msg);
	return(1);
}
