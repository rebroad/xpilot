/* $Id: ConnectionControl.h,v 1.14 2004/05/31 18:17:25 dick Exp $
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
 *  $Log: ConnectionControl.h,v $
 *  Revision 1.14  2004/05/31 18:17:25  dick
 *  virtual destructors so subclasses don't leak.
 *
 *  Revision 1.13  2004/05/07 04:27:41  dick
 *  _visibility becomes Visibility.  global updateScores becomes a member of World.
 *  Handle rank/rate data from the scoreserver and send it to the client.
 *
 *  Revision 1.12  2004/02/16 14:29:44  dick
 *  server/sched.cpp and friends become serverlib/NetServer
 *  which disconnects the udp listen and receive functionality from the game.
 *
 *  Revision 1.11  2004/02/05 04:56:29  dick
 *  Send back the servername in response to a ServerInfo request.
 *
 *  Revision 1.10  2002/09/09 23:42:55  dick
 *  Handle connecting a LocalCtl to this server.
 *
 *  Revision 1.9  2002/07/17 23:50:37  dick
 *  ReceivePlayerMessage() from a control and either broadcast it (nick="")
 *  or send it to a player.
 *
 *  Revision 1.8  2002/06/30 07:02:44  dick
 *  Send each ConnectionControl the Play messages if he wants them.
 *
 *  Revision 1.7  2002/06/26 07:26:17  dick
 *  Call it ServerActivate instead of Activate.
 *
 *  Revision 1.6  2002/06/26 06:59:50  dick
 *  Handle the Activate command from the ScoreServer, and don't send ScoreEvents
 *  unless we have been activated.
 *
 *  Revision 1.5  2002/06/23 06:34:28  dick
 *  Add the virtual jump functions for the ScoreServer
 *
 *  Revision 1.4  2001/09/03 00:45:00  dick
 *  New packet type Nop, used for a keepalive ping.
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

#ifndef	_CONNECTIONCONTROL_H_
#define	_CONNECTIONCONTROL_H_

#include "Connection.h"

class ServerOption;

class ConnectionControl : public Connection
{
public:
	ConnectionControl();
	virtual ~ConnectionControl();

	bool	wantPlayMessages;		// Does the client want to receive play messages?

	int		SendOption(ServerOption* so);
	int		SendPlayMessage(PCSTR msg);

	// overridden from Connection
	virtual int	HandleSetup();

	// Receive a PKT_CTL command from the client
	int		ReceiveCtl();

	// functions to handle each subcommand
			int		ReceiveUndefined();
			int		ReceiveDownloadOptions();
			int		ReceiveWantPlayMessages();
			int		ReceivePlayerMessage();
	virtual int		ReceiveSetOption();
			int		ReceiveRequestOptionCount();
			int		ReceiveRequestServerInfo();
	virtual	int		ReceiveServerActivate()	{ return(ReceiveUndefined()); }
	virtual	int		ReceiveGetScore()		{ return(ReceiveUndefined()); }
	virtual	int		ReceiveGetRanks()		{ return(ReceiveUndefined()); }
	virtual	int		ReceivePlayerEvent()	{ return(ReceiveUndefined()); }
	virtual	int		ReceiveSetScoreEvent()	{ return(ReceiveUndefined()); }
	virtual	int		ReceiveWebInfo()		{ return(ReceiveUndefined()); }
	virtual int		ReceiveShutdown()		{ return(ReceiveUndefined()); }
	virtual int		ReceiveRestartWorld()	{ return(ReceiveUndefined()); }
	virtual int		ReceiveWorldInfo()		{ return(ReceiveUndefined()); }

	World*		world;				/* which world this connection belongs to */
};

void	InitConnectionControl();

#endif	// _CONNECTIONCONTROL_H_
