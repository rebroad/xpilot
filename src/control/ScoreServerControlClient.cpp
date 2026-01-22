/* $Id: ScoreServerControlClient.cpp,v 1.1 2004/04/21 23:24:34 dick Exp $
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
 * $Log: ScoreServerControlClient.cpp,v $
 * Revision 1.1  2004/04/21 23:24:34  dick
 * The client that controls the ScoreServer
 *
 */
#include "StdAfx.h"

#include "FL/Fl.H"

#include "cstring.h"
#include "config.h"
#include "packet.h"

#include "LocalServerPanel.h"
#include "ScoreServerControlClient.h"

///////////////////////////////////////////////////////////////////////////////
ScoreServerControlClient::ScoreServerControlClient()
{
	SetControlType("localctl");
}

///////////////////////////////////////////////////////////////////////////////
ScoreServerControlClient::~ScoreServerControlClient()
{
	Disconnect();
}

///////////////////////////////////////////////////////////////////////////////
void ScoreServerControlClient::RbufSelectedFl(int /* fd */, void* myThis)
{
	int		ret;
	ScoreServerControlClient* cc = (ScoreServerControlClient*) myThis;
	//xpprintf("*");
	ret = cc->RbufSelected();
}

///////////////////////////////////////////////////////////////////////////////
void ScoreServerControlClient::TimerTickFl(void* myThis)
{
	int		ret;
	ScoreServerControlClient* cc = (ScoreServerControlClient*) myThis;
	ret = cc->TimerTick();
}

///////////////////////////////////////////////////////////////////////////////
void ScoreServerControlClient::FlSetup()
{
	Fl::add_fd(rbuf.sock.fd, &ScoreServerControlClient::RbufSelectedFl, this);
	Fl::add_timeout(TIMER_INTERVAL, &ScoreServerControlClient::TimerTickFl, this);
}

///////////////////////////////////////////////////////////////////////////////
bool ScoreServerControlClient::Disconnect()
{
	Fl::remove_timeout(&ScoreServerControlClient::TimerTickFl, this);
	if (rbuf.sock.fd != -1)
		Fl::remove_fd(rbuf.sock.fd);
	return(ScoreServerControlClientSUPERCLASS::Disconnect());
}

///////////////////////////////////////////////////////////////////////////////
void ScoreServerControlClient::Connected()
{
	ScoreServerControlClientSUPERCLASS::Connected();
}

///////////////////////////////////////////////////////////////////////////////
int ScoreServerControlClient::TimerTick()
{
	Fl::repeat_timeout(TIMER_INTERVAL, &ScoreServerControlClient::TimerTickFl, this);
	return(ScoreServerControlClientSUPERCLASS::TimerTick());
}

///////////////////////////////////////////////////////////////////////////////
bool ScoreServerControlClient::SendShutdown()
{
	cwbuf.printf("%c%c", PKT_CTL, Shutdown);
	SendReliable();
	wbuf.Flush();
	IHaveSpoken();
	return(true);
}
