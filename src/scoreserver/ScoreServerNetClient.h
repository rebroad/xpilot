/* $Id: ScoreServerNetClient.h,v 1.8 2004/05/07 04:38:25 dick Exp $
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
 * $Log: ScoreServerNetClient.h,v $
 * Revision 1.8  2004/05/07 04:38:25  dick
 * Send rank data to the server.
 *
 * Revision 1.7  2004/02/05 04:55:43  dick
 * Download the ServerOptions from the server and when done, if the servername
 * is empty, then request the running host name from the server with a ServerInfo call.
 * (So it matches what we're reporting to meta).
 *
 * Revision 1.6  2002/09/03 08:17:54  bert
 * fix gcc3 compile error: ScoreServerNetClient.h:70: friend declaration requires class-key, i.e. `friend
 *    class ScoreServer'
 *
 * Revision 1.5  2002/08/01 15:02:21  dick
 * Add support for cookies.
 *
 * Revision 1.4  2002/07/14 19:09:05  dick
 * Whenever we're talking to the server about PCSTR nick, also mention the PlayerType.
 *
 * Revision 1.3  2002/06/30 06:14:12  dick
 * Receive and log Play messages (chat and system) from the server.
 * This feature was added to aid in debugging the ScoreEvents (to make sure
 * everything lines up) but it might be useful for other things too.
 *
 * Revision 1.2  2002/06/26 07:05:55  dick
 * Add an Activate command handler
 *
 * Revision 1.1  2002/06/25 05:18:12  dick
 * The ScoreServer's network interface to the server.
 * I subclassed ServerOptionsNetClient instead of ControlClient directly.
 * I was thinking perhaps the ServerOptions could configure the ScoreServer
 * as well. (although most config was done because you already connected the
 * ScoreServer to the server :) .
 *
 */

#ifndef	_SCORESERVERNETCLIENT_H_
#define	_SCORESERVERNETCLIENT_H_

#include "Obj.h"
#include "ServerOptionsNetClient.h"

extern PCSTR s_ScoreServerNetClient;

class ScoreServer;

#define	ScoreServerNetClientSUPERCLASS	ServerOptionsNetClient
class ScoreServerNetClient : public ScoreServerNetClientSUPERCLASS
{
public:
	friend	class ScoreServer;
	ScoreServerNetClient();
	~ScoreServerNetClient();

	bool	Connect(PCSTR server, int port, PCSTR password);
	bool	SendWebInfo(PCSTR webHost, int webPort, PCSTR webMsg);
	bool	SendScore(PCSTR nick, PlayerType pt, uint cookie,
					  DFLOAT score, int kills, int deaths);
	bool	SendRank(PCSTR nick, PlayerType pt, 
					 int rank, DFLOAT rate);
	bool	RequestServerInfo();

	int	TimerTick();

	// Control (PKT_CTL) receive overrides
protected:
	virtual	int		ReceiveGetScore();
	virtual	int		ReceiveGetRanks();
	virtual int		ReceiveSetScore();
	virtual int		ReceivePlayerEvent();
	virtual int		ReceiveScoreEvent();
	virtual int		ReceivePlayMessage();
	virtual	int		ReceiveServerInfo();

	virtual	void	OptionsDownloaded();

private:
	ScoreServer*	ss;

	bool	Activate(bool on = true);
};

#endif	// _SCORESERVERNETCLIENT_H_
