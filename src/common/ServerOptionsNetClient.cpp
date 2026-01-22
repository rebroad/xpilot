/* $Id: ServerOptionsNetClient.cpp,v 1.9 2007/02/03 09:22:31 dick Exp $
 *
 * ServerOptionsNetClient - connect a ServerOptions to a server
 *
 *      Copyright (C) 2001 by
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
 * $Log: ServerOptionsNetClient.cpp,v $
 * Revision 1.9  2007/02/03 09:22:31  dick
 * Remove the listeners before deleting the netclient
 *
 * Revision 1.8  2004/05/23 23:52:07  dick
 * src/common/IniClient.Defaults.cpp
 *
 * Revision 1.7  2004/05/14 21:15:35  dick
 * Handle PrivateServerList updates
 *
 * Revision 1.6  2004/02/05 04:47:01  dick
 * Add a callback when the options are finished downloading
 *
 * Revision 1.5  2004/02/03 03:39:52  dick
 * Add Windows precompiled header support.
 *
 * Revision 1.4  2002/07/12 15:43:29  dick
 * SetServer(PCSTR hostname, int contactPort) for when we don't have a ServerSite.
 *
 * Revision 1.3  2002/07/08 06:28:34  dick
 * If we are Idle, then just accept the received option, don't try to ask for
 * more options or display percent complete.
 *
 * Revision 1.2  2002/06/23 06:11:52  dick
 * Break the FLTK dependencies out of ServerOptionsNetClient by making a subclass
 * ServerOptionsNetClientFl, because sometimes you want a ServerOptionsNetClient
 * without the Fl baggage.
 *
 * Revision 1.1  2002/05/29 20:36:40  jlmiller
 * Move to common, since we're now used in both control and xpwhere
 *
 * Revision 1.13  2001/09/05 09:13:59  dick
 * Wrap debug with D()
 *
 * Revision 1.12  2001/09/03 00:51:50  dick
 * Tuners cleanup.
 *
 * Revision 1.11  2001/08/31 21:01:58  dick
 * Use outbound reliable buffer.
 *
 * Revision 1.10  2001/08/30 07:16:53  dick
 * Send and receive options between the server and xpilotedit.
 *
 * Revision 1.9  2001/08/26 10:56:12  dick
 * Linux cleanups
 *
 * Revision 1.8  2001/08/26 10:43:13  dick
 * Mark the time we last spoke to the server, so we can KEEPALIVE if its been
 * awhile.  ServerOptions* goes in ControlClient.
 *
 * Revision 1.7  2001/08/13 03:03:43  dick
 * SockbufClient becomes ControlClient
 *
 * Revision 1.6  2001/08/12 11:19:31  dick
 * A little bit closer to getting the control to work...
 *
 * Revision 1.5  2001/08/11 13:18:42  dick
 * work on ServerOptionsNetClient
 *
 * Revision 1.4  2001/08/11 09:40:49  dick
 * Use OpenUdp()
 *
 * Revision 1.3  2001/08/08 08:03:44  dick
 * deadwood delete
 *
 * Revision 1.2  2001/08/07 22:14:23  dick
 * usleep becomes microdelay.
 *
 * Revision 1.1  2001/08/07 20:56:01  dick
 * Connect a ServerOptions to a server.
 *
 */
#include "StdAfx.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include <signal.h>
#include <errno.h>

#include "config.h"
#include "commonproto.h"
#include "ServerSite.h"
#include "ServerOptionsNetClient.h"
#include "Sock.h"
#include "packet.h"
#include "pack.h"
#include "microdelay.h"
#include "PacketCtl.h"

#define	OPTS_PER_REQUEST	30

///////////////////////////////////////////////////////////////////////////////
ServerOptionsNetClient::ServerOptionsNetClient()
{
	state = Idle;
	noSend = false;
}

///////////////////////////////////////////////////////////////////////////////
ServerOptionsNetClient::~ServerOptionsNetClient()
{
	// Disconnect();
}

///////////////////////////////////////////////////////////////////////////////
void ServerOptionsNetClient::SetServer(const ServerSite& ss)
{
	serverName = ss.serverName;
	serverIP = ss.serverIP;
	serverPort = ss.serverPort;
	sos->connectedToName->Set(ss.serverName);
	sos->connectedToPort->Set(serverPort);
	serverVersion = ss.version;
}

///////////////////////////////////////////////////////////////////////////////
void ServerOptionsNetClient::SetServer(PCSTR hostname, PCSTR ip, int contactPort)
{
	serverName = hostname;
	serverIP = ip;
	sos->connectedToName->Set(hostname);
	sos->connectedToPort->Set(contactPort);
	serverPort = contactPort;
}

///////////////////////////////////////////////////////////////////////////////
void ServerOptionsNetClient::SetServerOptions(ServerOptions* _sos)
{
	if (sos)
		warn("ServerOptionsNetClient already has an so\n");
	sos = _sos;
	InitListeners();
}

///////////////////////////////////////////////////////////////////////////////
void ServerOptionsNetClient::RemoveServerOptions()
{
	RemoveListeners();
	sos = NULL;
}

///////////////////////////////////////////////////////////////////////////////
void ServerOptionsNetClient::InitListeners()
{
	for (int i=0; i<sos->numPrefs; i++)	{
		sos->prefsArray[i]->AddListener(&ServerOptionsNetClient::HandleListener, NULL, this);
	}
}

///////////////////////////////////////////////////////////////////////////////
void ServerOptionsNetClient::RemoveListeners()
{
	for (int i=0; i<sos->numPrefs; i++)	{
		sos->prefsArray[i]->RemoveListener(&ServerOptionsNetClient::HandleListener, this);
	}
}

///////////////////////////////////////////////////////////////////////////////
void ServerOptionsNetClient::HandleListener(void* myThis, ServerOption* so)
{
	ServerOptionsNetClient* sonc = (ServerOptionsNetClient*)myThis;
	sonc->Listener(so);
}

///////////////////////////////////////////////////////////////////////////////
void ServerOptionsNetClient::Listener(ServerOption* so)
{
	// named option?  we should send it, unless it is not ours (masked by noSend)
	// and we are connected
	if (connected && strlen(so->name) && !noSend)
	{
		SendOption(so);
	}

}

///////////////////////////////////////////////////////////////////////////////
bool ServerOptionsNetClient::Disconnect()
{
	bool ret = ServerOptionsNetClientSUPERCLASS::Disconnect();
	if (sos)
		sos->isConnected->Set(false);
	return(ret);
}

///////////////////////////////////////////////////////////////////////////////
int ServerOptionsNetClient::TimerTick()
{
	int ret = ServerOptionsNetClientSUPERCLASS::TimerTick();
	if (state != Idle)
	{
		// need to rerequest?
	}
	return(ret);
}

///////////////////////////////////////////////////////////////////////////////
bool ServerOptionsNetClient::Download()
{
	D(xpprintf("Requesting Option count\n");)
	cwbuf.printf("%c%c", PKT_CTL, OptionCount);
	SendReliable();
	wbuf.Flush();
	state = WaitCount;
	IHaveSpoken();
	return(true);
}

///////////////////////////////////////////////////////////////////////////////
int ServerOptionsNetClient::ReceiveOptionCount()
{
	if (crbuf.scanf("%hd", &numOptions) <= 0)
	{
		emh(emhThis, EmError, "ReceiveOptionCount: parse error");
		return(-1);
	}
	D(printf("ReceiveOptionCount: %d\n", numOptions);)
	curOptions = 0;
	RequestOptions();
	return(1);
}

///////////////////////////////////////////////////////////////////////////////
void ServerOptionsNetClient::RequestOptions()
{
	optsRequested = OPTS_PER_REQUEST;
	if (curOptions + optsRequested > numOptions)
		optsRequested = numOptions - curOptions;
	cwbuf.printf("%c%c%hd%hd", PKT_CTL, DownloadOptions, 
							  curOptions, curOptions+optsRequested);
	xpprintf(LOGLOTS, "Requesting options %d -> %d\n", curOptions, curOptions+optsRequested);
	wbuf.Flush();
	state = WaitOptions;
	partialReceive = 0;
	IHaveSpoken();
}

///////////////////////////////////////////////////////////////////////////////
int ServerOptionsNetClient::ReceiveSetOption()
{
	noSend = true;
	int ret = ServerOptionsNetClientSUPERCLASS::ReceiveSetOption();
	noSend = false;
	if (ret != 1)
		return(ret);
	if (state == Idle)
		return(1);
	if (++partialReceive < optsRequested)
		return(1);
//	emh(emhThis, EmAppend, ".");
	curOptions += OPTS_PER_REQUEST;
	emh(emhThis, EmDebug, "Received %d%%%%", curOptions*100/numOptions);
	if (curOptions >= numOptions)
	{
		state = Idle;
		emh(emhThis, EmInfo, "Options Downloaded");
		OptionsDownloaded();
		
		return(1);
	}
	RequestOptions();
	return(1);
}

///////////////////////////////////////////////////////////////////////////////
int ServerOptionsNetClient::SendOption(ServerOption* so)
{
	if (!strlen(so->name))
		return(0);
	D(xpprintf("SendOption: %s <%s>\n", so->name, (PCSTR)so->GetString());)
	cwbuf.printf("%c%c%s%S", PKT_CTL, SetOption, so->name, (PCSTR)so->GetString());
	wbuf.Flush();
	IHaveSpoken();

	return(1);
}

