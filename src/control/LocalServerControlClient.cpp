/* $Id: LocalServerControlClient.cpp,v 1.4 2004/02/03 03:39:53 dick Exp $
 *
 * A network client that controls the local server.
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
 * $Log: LocalServerControlClient.cpp,v $
 * Revision 1.4  2004/02/03 03:39:53  dick
 * Add Windows precompiled header support.
 *
 * Revision 1.3  2004/02/01 15:51:48  dick
 * Fix tooltips.  LocalServerPanel caused the widgets to be updated every
 * two seconds (even when this window is hidden), which resets the tooltips timer.
 * Use forceUpdate in combination with current settings to determine whether
 * to actually alter the widgets.  (forceUpdate is needed for initial settings)
 * I don't really like the forceUpdate technique, it's a runaway stack waiting
 * to happen, but it works.
 *
 * Revision 1.2  2002/09/10 04:56:35  dick
 * Only remove a valid socket from Fl::
 *
 * Revision 1.1  2002/09/09 23:46:09  dick
 * New LocalServerControlClient handles the client side connection of a LocalCtl.
 *
 */
#include "StdAfx.h"

#include "FL/Fl.H"

#include "cstring.h"
#include "config.h"
#include "packet.h"

#include "LocalServerPanel.h"
#include "LocalServerControlClient.h"

///////////////////////////////////////////////////////////////////////////////
LocalServerControlClient::LocalServerControlClient()
{
	SetControlType("localctl");
}

///////////////////////////////////////////////////////////////////////////////
LocalServerControlClient::~LocalServerControlClient()
{
	Disconnect();
}

///////////////////////////////////////////////////////////////////////////////
void LocalServerControlClient::RbufSelectedFl(int /* fd */, void* myThis)
{
	int		ret;
	LocalServerControlClient* cc = (LocalServerControlClient*) myThis;
	//xpprintf("*");
	ret = cc->RbufSelected();
}

///////////////////////////////////////////////////////////////////////////////
void LocalServerControlClient::TimerTickFl(void* myThis)
{
	int		ret;
	LocalServerControlClient* cc = (LocalServerControlClient*) myThis;
	ret = cc->TimerTick();
}

///////////////////////////////////////////////////////////////////////////////
void LocalServerControlClient::FlSetup()
{
	Fl::add_fd(rbuf.sock.fd, &LocalServerControlClient::RbufSelectedFl, this);
	Fl::add_timeout(TIMER_INTERVAL, &LocalServerControlClient::TimerTickFl, this);
}

///////////////////////////////////////////////////////////////////////////////
bool LocalServerControlClient::Disconnect()
{
	Fl::remove_timeout(&LocalServerControlClient::TimerTickFl, this);
	if (rbuf.sock.fd != -1)
		Fl::remove_fd(rbuf.sock.fd);
	return(LocalServerControlClientSUPERCLASS::Disconnect());
}

///////////////////////////////////////////////////////////////////////////////
void LocalServerControlClient::Connected()
{
	LocalServerControlClientSUPERCLASS::Connected();
	SendRequestWorldInfo();
}

///////////////////////////////////////////////////////////////////////////////
int LocalServerControlClient::TimerTick()
{
	Fl::repeat_timeout(TIMER_INTERVAL, &LocalServerControlClient::TimerTickFl, this);
	return(LocalServerControlClientSUPERCLASS::TimerTick());
}

///////////////////////////////////////////////////////////////////////////////
bool LocalServerControlClient::SendShutdown()
{
	cwbuf.printf("%c%c", PKT_CTL, Shutdown);
	SendReliable();
	wbuf.Flush();
	IHaveSpoken();
	return(true);
}

///////////////////////////////////////////////////////////////////////////////
bool LocalServerControlClient::SendRestartWorld(int world)
{
	cwbuf.printf("%c%c", PKT_CTL, RestartWorld);
	SendReliable();
	wbuf.Flush();
	IHaveSpoken();
	return(true);
}

///////////////////////////////////////////////////////////////////////////////
bool LocalServerControlClient::SendRequestWorldInfo()
{
	cwbuf.printf("%c%c", PKT_CTL, WorldInfo);
	SendReliable();
	wbuf.Flush();
	IHaveSpoken();
	return(true);
}

///////////////////////////////////////////////////////////////////////////////
int LocalServerControlClient::ReceiveWorldInfo()
{
	int		i;
	char	worldCount;
	char	mapName[MAX_CHARS];

    if (crbuf.scanf("%c", &worldCount) <= 0)
	{
		emh(emhThis, EmError, "ReceiveWorldInfo: Can't parse msg");
		return(-1);
    }
	worldCount = MIN(worldCount, MAX_WORLDS);
	for (i=0; i<worldCount; i++)
	{
		if (crbuf.scanf("%s", &mapName) <=0)
		{
			emh(emhThis, EmError, "ReceiveWorldInfo: Can't parse world info");
			return(-1);
		}
		lsp->UpdateWorldInfo(i, mapName, true);
	}
	return(1);
}

