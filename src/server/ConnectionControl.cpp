/* $Id: ConnectionControl.cpp,v 1.27 2004/09/14 00:25:42 dick Exp $
 *
 * Describe a server's connection to a control.
 *
 * XPilot, a multiplayer gravity war game.  Copyright (C) 1991-2001 by
 *
 *      Dick Balaska         <dick@xpilot.org>
 *      Bert Gijsbers        <bert@xpilot.org>
 *      Ken Ronny Schouten   <ken@xpilot.org>
 *      Bj�rn Stabell        <bjoern@xpilot.org>
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
 *  $Log: ConnectionControl.cpp,v $
 *  Revision 1.27  2004/09/14 00:25:42  dick
 *  SendPlayerMessage() needs to be a long message.
 *
 *  Revision 1.26  2004/05/31 18:17:25  dick
 *  virtual destructors so subclasses don't leak.
 *
 *  Revision 1.25  2004/05/07 04:27:41  dick
 *  _visibility becomes Visibility.  global updateScores becomes a member of World.
 *  Handle rank/rate data from the scoreserver and send it to the client.
 *
 *  Revision 1.24  2004/04/23 17:58:15  dick
 *  Move InitConnectionControl() to the top of the file
 *
 *  Revision 1.23  2004/02/20 06:27:47  dick
 *  SendPlayMessage becomes BroadcastPlayMessage() because it sends a play message
 *  to all connections.
 *
 *  Revision 1.22  2004/02/16 14:29:44  dick
 *  server/sched.cpp and friends become serverlib/NetServer
 *  which disconnects the udp listen and receive functionality from the game.
 *
 *  Revision 1.21  2004/02/05 04:56:29  dick
 *  Send back the servername in response to a ServerInfo request.
 *
 *  Revision 1.20  2004/02/03 03:39:53  dick
 *  Add Windows precompiled header support.
 *
 *  Revision 1.19  2002/09/09 23:42:55  dick
 *  Handle connecting a LocalCtl to this server.
 *
 *  Revision 1.18  2002/08/01 14:46:31  dick
 *  When asked to send a player message to a player, make sure the player exists first.
 *
 *  Revision 1.17  2002/07/17 23:50:37  dick
 *  ReceivePlayerMessage() from a control and either broadcast it (nick="")
 *  or send it to a player.
 *
 *  Revision 1.16  2002/07/14 19:01:44  dick
 *  PacketCtlTypesMax becomes PacketCtlTypesMAX
 *
 *  Revision 1.15  2002/07/08 06:17:07  dick
 *  Send the option's origin with the option
 *
 *  Revision 1.14  2002/07/07 04:51:09  dick
 *  DCC() debug macro wraps #if _DEBUG for debugging ConnectionControl
 *
 *  Revision 1.13  2002/06/30 06:52:35  dick
 *  If a Control wants Play messages, then send them to him.
 *
 *  Revision 1.12  2002/06/26 07:26:17  dick
 *  Call it ServerActivate instead of Activate.
 *
 *  Revision 1.11  2002/06/26 06:59:50  dick
 *  Handle the Activate command from the ScoreServer, and don't send ScoreEvents
 *  unless we have been activated.
 *
 *  Revision 1.10  2002/06/23 06:34:52  dick
 *  Tag the control with our type.
 *
 *  Revision 1.9  2002/06/01 06:06:47  dick
 *  Encapsulate (almost) everything.  Get rid of (almost) all refs to theWorld.
 *
 *  Revision 1.8  2002/05/18 22:34:15  dick
 *  Move showtime to common.
 *
 *  Revision 1.7  2002/05/18 20:55:38  dick
 *  Update to XPilot-4.5.1
 *
 *  Revision 1.6  2001/10/09 17:31:07  dick
 *  Add #include <string.h>
 *
 *  Revision 1.5  2001/09/03 00:45:00  dick
 *  New packet type Nop, used for a keepalive ping.
 *
 *  Revision 1.4  2001/08/31 20:58:13  dick
 *  Server needs to be able to receive reliable packets as well as send them.
 *  Use the new emh error handling.
 *
 *  Revision 1.3  2001/08/30 07:16:44  dick
 *  Send and receive options between the server and xpilotedit.
 *
 *  Revision 1.2  2001/08/26 10:34:26  dick
 *  Handle a connection from a control.
 *
 *  Revision 1.1  2001/08/11 09:38:20  dick
 *  new class ConnectionControl, a Connection for a control.
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
#include "NetServer.h"
#include "ConnectionControl.h"
#include "packet.h"
#include "PacketCtl.h"
#include "World.h"

typedef	int	(ConnectionControl::*ConnCtlReceiveFunc)();

ConnCtlReceiveFunc	ctlReceive[PacketCtlTypesMAX];

PCSTR	s_generic = "generic";

#if 0			// Debug ConnectionControl
#define	DCC(__d)	D(__d)
#else
#define	DCC(__d)
#endif

///////////////////////////////////////////////////////////////////////////////
void InitConnectionControl()
{
    for (int i = 0; i < PacketCtlTypesMAX; i++)
	{
		ctlReceive[i] = &ConnectionControl::ReceiveUndefined;
    }

    ctlReceive[Nop]					= &ConnectionControl::ReceiveNop;
    ctlReceive[DownloadOptions]		= &ConnectionControl::ReceiveDownloadOptions;
    ctlReceive[SetOption]			= &ConnectionControl::ReceiveSetOption;
    ctlReceive[OptionCount]			= &ConnectionControl::ReceiveRequestOptionCount;
    ctlReceive[WantPlayMessages]	= &ConnectionControl::ReceiveWantPlayMessages;
    ctlReceive[PlayerMessage]		= &ConnectionControl::ReceivePlayerMessage;
    ctlReceive[ServerActivate]		= &ConnectionControl::ReceiveServerActivate;
    ctlReceive[WebInfo]				= &ConnectionControl::ReceiveWebInfo;
    ctlReceive[GetScore]			= &ConnectionControl::ReceiveGetScore;
    ctlReceive[GetRanks]			= &ConnectionControl::ReceiveGetRanks;
    ctlReceive[Shutdown]			= &ConnectionControl::ReceiveShutdown;
    ctlReceive[RestartWorld]		= &ConnectionControl::ReceiveRestartWorld;
    ctlReceive[WorldInfo]			= &ConnectionControl::ReceiveWorldInfo;
    ctlReceive[ServerInfo]			= &ConnectionControl::ReceiveRequestServerInfo;
}

///////////////////////////////////////////////////////////////////////////////
ConnectionControl::ConnectionControl()
{
	ctl = s_generic;
	wantPlayMessages = false;
	SetErrMsgHandler(&World::ErrMsgHandler, this);
}

///////////////////////////////////////////////////////////////////////////////
ConnectionControl::~ConnectionControl()
{
}

///////////////////////////////////////////////////////////////////////////////
int ConnectionControl::HandleSetup()
{
	SetState(CONN_CTL, CONN_CTL);
	return(0);
}

///////////////////////////////////////////////////////////////////////////////
int ConnectionControl::ReceiveCtl()
{
	unsigned char	ch;
	unsigned char	command;
	char			value[MAX_CHARS];
	int				n;
    if ((n = cr.scanf("%c%c", &ch, &command)) <= 0)
	{
		if (n == -1)
			DestroyConnection("read error");
		return n;
    }
	start = netServer->mainLoops;
	DCC(if (command) xpprintf("ReceiveCtl: ch=%d command=%d\n", ch, command);)
	if (command >= PacketCtlTypesMAX)
	{
		sprintf(value, "bad Ctl command %d", command);
		DestroyConnection(value);
		return(-1);
	}
	if (ctlReceive[command] == &ConnectionControl::ReceiveUndefined)
	{
		xpprintf("Unhandled Ctl command %d\n", command);
		return(0);
	}
	return((this->*(ctlReceive[command]))());
}

///////////////////////////////////////////////////////////////////////////////
int ConnectionControl::ReceiveUndefined()
{
	xpprintf("Unhandled command\n");
	return(0);
}

///////////////////////////////////////////////////////////////////////////////
int ConnectionControl::ReceiveRequestOptionCount()
{
	cw.printf("%c%c%hd", PKT_CTL, OptionCount, world->options.numPrefs);
	if (SendReliable() <= 0)
		return(FALSE);
	return(TRUE);
}

///////////////////////////////////////////////////////////////////////////////
int ConnectionControl::ReceiveRequestServerInfo()
{
	cw.printf("%c%c%s", PKT_CTL, ServerInfo, (PCSTR)world->server.host);
	if (SendReliable() <= 0)
		return(FALSE);
	return(TRUE);
}

///////////////////////////////////////////////////////////////////////////////
int ConnectionControl::ReceiveWantPlayMessages()
{
	char	want;
	int	n;

    if ((n = cr.scanf("%c", &want)) <= 0)
	{
		if (n == -1)
			DestroyConnection("read error, WantPlayMessages needs want");
		return n;
    }
	wantPlayMessages = want ? true : false;
	return(TRUE);
}

///////////////////////////////////////////////////////////////////////////////
int ConnectionControl::ReceivePlayerMessage()
{
	char	message[MSG_LEN];
	char	player[MAX_CHARS];
	char	pt;
	int	n;

    if ((n = cr.scanf("%S%s%c", &message, &player, &pt)) <= 0)
	{
		if (n == -1)
			DestroyConnection("read error, ReceivePlayerMessage");
		return n;
    }
	if (!strlen(player))
		world->BroadcastPlayMessage(message);
	else
	{
		Player*	pl = world->FindPlayer(player, (PlayerType)pt);
		if (pl)
			pl->SetMessage(message);
	}
	return(TRUE);
}

///////////////////////////////////////////////////////////////////////////////
int ConnectionControl::ReceiveDownloadOptions()
{
	short	first;
	short	last;
	int	n;

    if ((n = cr.scanf("%hd%hd", &first, &last)) <= 0)
	{
		if (n == -1)
			DestroyConnection("read error, DownloadOptions needs first/last");
		return n;
    }
	for (int i=first; i<last; i++)
	{
		if (i >= world->options.numPrefs)
			break;
		SendOption(world->options.prefsArray[i]);
	}
	if (SendReliable() <= 0)
		return(FALSE);
	return(TRUE);
}

///////////////////////////////////////////////////////////////////////////////
int ConnectionControl::ReceiveSetOption()
{
	char	name[MAX_CHARS];
	char	value[MSG_LEN];
	String	valueOut;
	int	n;

    if ((n = cr.scanf("%s%S", &name, &value)) <= 0)
	{
		if (n == -1)
			DestroyConnection("read error, SetOption needs name/value");
		return n;
    }
	ServerOption*	so;
	so = world->options.FindOptionByName(name);
	if (!so)
	{
		xpprintf("%s%s@%s Tried to set unknown option <%s>\n",
				 showtime(), (PCSTR)real, (PCSTR)dpy, (PCSTR)name);
		return(FALSE);
	}
	int	i;
    i = world->options.TuneOption(name, value);
//	so->Set(value);
	so->Get(valueOut);
	String	s;
	if (valueOut == value)
		s.printf("%s@%s set option %s to <%s>",
				 (PCSTR)real, (PCSTR)dpy, (PCSTR)name, value);
	else
		s.printf("%s@%s set option %s to <%s> (limited from %s)",
				 (PCSTR)real, (PCSTR)dpy, (PCSTR)name, (PCSTR)valueOut, value);
	xpprintf("%s%s\n", showtime(), (PCSTR)s);
	if (world->options.notifyServerChanges->GetBool())
	{
		String t;
		t.printf(" <%s>", (PCSTR)s);
		world->BroadcastPlayMessage(t);
	}
	world->BroadcastOption(so);
	return(TRUE);
}

///////////////////////////////////////////////////////////////////////////////
int ConnectionControl::SendOption(ServerOption* so)
{
	DCC(xpprintf("SendOption %s:%s %s\n",
		so->name, (PCSTR)so->GetString(), (PCSTR)so->GetOrigin());)
	if (strlen(so->name))
		cw.printf("%c%c%s%c%S", PKT_CTL, SetOption,
			(PCSTR)so->name, (uchar)so->optOrigin, (PCSTR)so->GetString());
	else
		cw.printf("%c%c%s%c%S", PKT_CTL, SetOption,
			"", 0, "");
	return(TRUE);
}

///////////////////////////////////////////////////////////////////////////////
int ConnectionControl::SendPlayMessage(const char *msg)
{
    return cw.printf("%c%S", PKT_MESSAGE, msg);
}

///////////////////////////////////////////////////////////////////////////////
