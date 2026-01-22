/* $Id: ConnectionControlLocalCtl.cpp,v 1.7 2004/05/30 16:21:13 dick Exp $
 *
 * ConnectionControlLocalCtl - The Server side connection to a local XPilot control
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
 *  $Log: ConnectionControlLocalCtl.cpp,v $
 *  Revision 1.7  2004/05/30 16:21:13  dick
 *  Whitespace
 *
 *  Revision 1.6  2004/02/03 03:39:53  dick
 *  Add Windows precompiled header support.
 *
 *  Revision 1.5  2004/01/06 04:48:28  dick
 *  Add support for building under Cygwin.  Still needs work and the output
 *  is X11 and requires something like Cygwin's XWin.exe to run under.
 *
 *  Revision 1.4  2002/09/11 17:40:53  dick
 *  Linux needs to reference theWorld[0]->EndGame
 *
 *  Revision 1.3  2002/09/11 16:42:03  dick
 *  Here endeth the server encapsulation task.
 *  theWorld is now an array of World(s) and get loaded dynamically.
 *
 *  Revision 1.2  2002/09/09 23:59:13  dick
 *  cannot pass objects of type `String' through `...'
 *
 *  Revision 1.1  2002/09/09 23:38:24  dick
 *  New class to handle the server side of a LocalCtl connection.
 *
 */
#include "StdAfx.h"

#include "xpprintf.h"
#include "packet.h"
#include "global.h"

#include "ConnectionControlLocalCtl.h"

#if defined(_WINDOWS) && !defined(_CYGWIN)
#include "NT/ServerThreadW32.h"
#endif

///////////////////////////////////////////////////////////////////////////////
ConnectionControlLocalCtl::ConnectionControlLocalCtl()
{
}

///////////////////////////////////////////////////////////////////////////////
int ConnectionControlLocalCtl::ReceiveShutdown()
{
	xpprintf(LOGMED, "%sShutdown request received\n", showtime());
#if defined(_WINDOWS) && !defined(_CYGWIN)
	// server thread sends a WM_CLOSE to the main window
	ServerThreadW32NotifyMainShutdown();
#else
	// er, need a real Unix loop not part of theWorld.
	theWorld[0]->EndGame();
#endif
	return(0);
}

///////////////////////////////////////////////////////////////////////////////
int ConnectionControlLocalCtl::ReceiveRestartWorld()
{
	return(TRUE);
}

///////////////////////////////////////////////////////////////////////////////
int ConnectionControlLocalCtl::ReceiveWorldInfo()
{
	xpprintf(LOGMAX, "%sReceiveWorldInfo request\n", showtime());
	SendWorldInfo(world);
	return(1);
}

///////////////////////////////////////////////////////////////////////////////
void ConnectionControlLocalCtl::SendWorldInfo(World* w)
{
	cw.printf("%c%c%c%s", PKT_CTL, WorldInfo,
		1, (PCSTR)w->options.mapName->GetString());
}
