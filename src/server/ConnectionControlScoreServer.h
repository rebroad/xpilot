/* $Id: ConnectionControlScoreServer.h,v 1.12 2004/05/31 18:17:25 dick Exp $
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
 *  $Log: ConnectionControlScoreServer.h,v $
 *  Revision 1.12  2004/05/31 18:17:25  dick
 *  virtual destructors so subclasses don't leak.
 *
 *  Revision 1.11  2004/05/07 04:27:41  dick
 *  _visibility becomes Visibility.  global updateScores becomes a member of World.
 *  Handle rank/rate data from the scoreserver and send it to the client.
 *
 *  Revision 1.10  2004/02/16 14:29:44  dick
 *  server/sched.cpp and friends become serverlib/NetServer
 *  which disconnects the udp listen and receive functionality from the game.
 *
 *  Revision 1.9  2004/02/05 04:56:29  dick
 *  Send back the servername in response to a ServerInfo request.
 *
 *  Revision 1.8  2002/07/14 19:05:24  dick
 *  When talking to the ScoreServer about PCSTR nick, always mention the PlayerType too.
 *
 *  Revision 1.7  2002/07/09 21:44:29  dick
 *  Send the player type (human,robot,tank) with the PlayerEvent to the ScoreServer.
 *
 *  Revision 1.6  2002/06/30 07:05:08  dick
 *  Define a proper SUPERCLASS
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

#ifndef	_CONNECTIONCONTROLSCORESERVER_H_
#define	_CONNECTIONCONTROLSCORESERVER_H_

#include "ConnectionControl.h"
#include "PacketCtl.h"

#define	ConnectionControlScoreServerSUPERCLASS	ConnectionControl
class ConnectionControlScoreServer : public ConnectionControlScoreServerSUPERCLASS
{
public:
	ConnectionControlScoreServer();
	virtual ~ConnectionControlScoreServer();

	virtual	void	DestroyConnection(PCSTR reason);

	bool	IsActive();
	int		RequestPlayerScore(Player* pl);
	int		RequestPlayerRanks(bool onOff);
	void	SendScoreEvent(PCSTR killer, PlayerType ptr, DFLOAT wscore,
						   PCSTR killee, PlayerType pte, DFLOAT lscore,
						   ScoreType st);
	void	SendPlayerEvent(Player* pl, ScorePlayerEvent pse);

	String	GetWebInfo();

//	int		ReceiveCtl();	// receive from server

//	int		SendOption(ServerOption* so);

protected:
	// overridden from Connection
//	virtual int	HandleSetup();

	// functions to handle each subcommand
//			int		ReceiveDownloadOptions();
//	virtual int		ReceiveSetOption();
	virtual	int		ReceiveServerActivate();
	virtual	int		ReceiveGetScore();
	virtual	int		ReceiveGetRanks();
	virtual	int		ReceivePlayerEvent();
	virtual	int		ReceiveSetScoreEvent();
	virtual	int		ReceiveWebInfo();

private:

	String	webHost;
	int		webPort;
	String	webMsg;
	bool	hasWebHost;

	bool	active;		// whether we can send events to the server
						// This is so we can wait until after the verify cycle.
						// This should probably be in ControlClient as all
						// controls should Activate after logging in.

};

inline	bool ConnectionControlScoreServer::IsActive() { return(active); };

#endif	// _CONNECTIONCONTROLSCORESERVER_H_
