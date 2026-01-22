/* $Id: ConnectionLocalCtl.cpp,v 1.4 2004/05/07 04:38:25 dick Exp $
 *
 * The Server side connection to a local XPilot control
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
 *  $Log: ConnectionLocalCtl.cpp,v $
 *  Revision 1.4  2004/05/07 04:38:25  dick
 *  Send rank data to the server.
 *
 *  Revision 1.3  2004/04/22 14:57:26  dick
 *  Handle terminating the Un*x ScoreServer
 *
 *  Revision 1.2  2004/04/22 05:58:10  dick
 *  Add an override for ReceiveUndefined() just so gcc will do a pointer compare.
 *
 *  Revision 1.1  2004/04/22 00:21:09  dick
 *  The Server side connection to a local XPilot control.
 *
 */
#include "StdAfx.h"

#include "xpprintf.h"
#include "packet.h"
#include "const.h"
#include "NetServer.h"
#include "ScoreServer.h"

#include "ConnectionLocalCtl.h"

#if defined(_WINDOWS) && !defined(_CYGWIN)
#include "ScoreServerW32.h"
#endif

#if 0			// Debug ConnectionControl
#define	DCC(__d)	D(__d)
#else
#define	DCC(__d)
#endif

typedef	int	(ConnectionLocalCtl::*ConnCtlReceiveFunc)();

ConnCtlReceiveFunc	ctlReceive[PacketCtlTypesMAX];

///////////////////////////////////////////////////////////////////////////////
void InitConnectionLocalCtl()
{
	for (int i = 0; i < PacketCtlTypesMAX; i++) 
	{
		ctlReceive[i] = &ConnectionLocalCtl::ReceiveUndefined;
	}

	ctlReceive[Nop]					= &ConnectionLocalCtl::ReceiveNop;
	ctlReceive[Shutdown]			= &ConnectionLocalCtl::ReceiveShutdown;
}

///////////////////////////////////////////////////////////////////////////////
ConnectionLocalCtl::ConnectionLocalCtl()
{
}

///////////////////////////////////////////////////////////////////////////////
int ConnectionLocalCtl::ReceiveUndefined()
{
	xpprintf("Unhandled command\n");
	return(0);
}

///////////////////////////////////////////////////////////////////////////////
int ConnectionLocalCtl::ReceiveCtl()
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
	if (ctlReceive[command] == &ConnectionLocalCtl::ReceiveUndefined)
	{
		xpprintf("Unhandled Ctl command %d\n", command);
		return(0);
	}
	return((this->*(ctlReceive[command]))());
}


///////////////////////////////////////////////////////////////////////////////
int ConnectionLocalCtl::HandleSetup()
{
	SetState(CONN_CTL, CONN_CTL);
	return(0);
}


///////////////////////////////////////////////////////////////////////////////
int ConnectionLocalCtl::ReceiveShutdown()
{
	xpprintf(LOGMED, "%sShutdown request received\n", showtime());
#if defined(_WINDOWS) && !defined(_CYGWIN)
	// server sends a WM_CLOSE to the main window
	scoreServerW32.SendWindowsShutdownMessage();
#else
	extern void TerminateScoreServer();
	TerminateScoreServer();
#endif
	return(0);
}

