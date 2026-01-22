/* $Id: NetServer.cpp,v 1.8 2006/09/24 05:49:17 dick Exp $
 *
 * Describe a server's connection to a client.
 *
 * XPilot, a multiplayer gravity war game.  Copyright (C) 1991-2003 by
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
 *  $Log: NetServer.cpp,v $
 *  Revision 1.8  2006/09/24 05:49:17  dick
 *  Better diagnostic when we can't create a socket
 *
 *  Revision 1.7  2004/04/23 17:43:15  dick
 *  Display the failed state as text as well as the hex value
 *
 *  Revision 1.6  2004/04/22 05:52:16  dick
 *  Attribute schedIsLoop is used to control the Sched() loop.  The Un*x server
 *  wants Sched() to be the main loop.  Windows server and all ScoreServers
 *  want Sched() to be pumped repeatedly by an outside source.
 *
 *  Revision 1.5  2004/04/16 21:07:59  dick
 *  NetServer becomes a little UDP engine that can be plugged into different apps.
 *
 *  Revision 1.4  2004/02/20 06:37:17  dick
 *  Unix needs declaration of PCSTR
 *
 *  Revision 1.3  2004/02/20 06:26:00  dick
 *  New virtual SendMetaUpdate() and BroadcastPlayMessage()
 *
 *  Revision 1.2  2004/02/18 01:24:33  dick
 *  Can't pass String through ...
 *
 *  Revision 1.1  2004/02/16 14:29:44  dick
 *  server/sched.cpp and friends become serverlib/NetServer
 *  which disconnects the udp listen and receive functionality from the game.
 *
 */
#include "StdAfx.h"

#if defined(_UNIX) || defined(_CYGWIN)
#include <stdlib.h>
#include <string.h>
#else
#endif


#include "const.h"
#include "error.h"
#include "xpprintf.h"
#include "showtime.h"
#include "Connection.h"
#include "NetServer.h"

#define MAX_SELECT_FD			(sizeof(int) * 8 - 1)

///////////////////////////////////////////////////////////////////////////////
NetServer::NetServer()
{
	mainLoops = 0;
	timerResolution = 0;
	loginInProgress = 0;
	controlsConnected = 0;
	numLogins = 0;
	numLogouts = 0;
	conn = NULL;
#ifdef _UNIX
	schedIsLoop = true;
#else
	schedIsLoop = false;
#endif
}

///////////////////////////////////////////////////////////////////////////////
bool NetServer::ContactInitPort(Sockbuf& ibuf, Sock& initSock, PCSTR addr, int port)
{
	int 		status;

	/*
	 * Create a socket which we can listen on.
	 */
	if ((status = initSock.OpenUdp(addr, port)) == -1) {
		error("Could not create Dgram initSock at %s:%d", addr, port);
		error("Perhaps program is already running?");
		return(false);
	}
	initSock.SetTimeout(0, 0);
	if (initSock.SetNonBlocking(1) == -1) {
		error("Can't make contact socket non-blocking");
		return(false);
	}
	if (ibuf.Init(&initSock, SERVER_SEND_SIZE,
					 SOCKBUF_READ | SOCKBUF_WRITE | SOCKBUF_DGRAM) == -1) {
		error("No memory for contact buffer");
		return(false);
	}
	return(TRUE);
}


///////////////////////////////////////////////////////////////////////////////
/*
 * Initialize the Connection structures.
 */
int NetServer::Setup(int wantNumConnections)
{
	size_t	size;

	Connection::InitReceiveTables();

	/*
	 * The number of connections is limited by the number of bases
	 * and the max number of possible file descriptors to use in
	 * the select(2) call minus those for stdin, stdout, stderr,
	 * the contact socket, and the socket for the resolver library routines.
	 */
	maxConnections = MIN(MAX_SELECT_FD - 5,wantNumConnections);
	size = maxConnections * sizeof(Connection*);

	if ((conn = (Connection**)malloc(size)) == NULL) {
		error("Cannot allocate memory for connections");
		return -1;
	}
	memset(conn, 0, size);

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
/*
 * Process a client packet.
 * The client may be in one of several states,
 * therefore we use function dispatch tables for easy processing.
 * Some functions may process requests from clients being
 * in different states.
 */
///////////////////////////////////////////////////////////////////////////////
int NetServer::Input()
{
	int			i,
	ind,
	num_reliable = 0,
	input_reliable[MAX_SELECT_FD];
	Connection*	connp;
	char		msg[MSG_LEN];

	for (i = 0; i < maxConnections; i++)
	{
		connp = conn[i];
		if (!connp || connp->state == CONN_FREE) 
			continue;
		if (connp->start + connp->timeout * GetFPS() < mainLoops)
		{
		   /*
			* Timeout this fellow if we have not heard a single thing
			* from him for a long time.
			*/
			if (connp->state & (CONN_PLAYING | CONN_READY))
			{
				sprintf(msg, "%s mysteriously disappeared!?", (PCSTR)connp->nick);
				BroadcastPlayMessage(msg);
				xpprintf("%s%s\n", showtime(), msg);
			}
			sprintf(msg, "timeout %02x %s", connp->state, (PCSTR)StateToText(connp->state));
			connp->DestroyConnection(msg);
			continue;
		}
		if (connp->state != CONN_PLAYING)
		{
			input_reliable[num_reliable++] = i;
			if (connp->state == CONN_SETUP) 
			{
				connp->HandleSetup();
				continue;
			}
		}
	}

	for (i = 0; i < num_reliable; i++)
	{
		ind = input_reliable[i];
		connp = conn[ind];
		if (connp && connp->state & (CONN_DRAIN | CONN_READY | CONN_SETUP
			| CONN_LOGIN | CONN_CTL))
		{
			if (connp->cw.len > 0)
			{
				if (connp->SendReliable() == -1)
				{
					continue;
				}
			}
		}
	}

	if (numLogins | numLogouts)
	{
		/* Tell the meta server */
		//D(xpprintf("%sCall MetaUpdate\n", showtime());)
		SendMetaUpdate();
		numLogins = 0;
		numLogouts = 0;
	}

    return loginInProgress;
}

