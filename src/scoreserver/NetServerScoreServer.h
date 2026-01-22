/* $Id: NetServerScoreServer.h,v 1.2 2004/04/21 23:28:44 dick Exp $
 *
 * ScoreServer subclass of the network server
 * This object handles connections from the xpilot5 localctl
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
 *  $Log: NetServerScoreServer.h,v $
 *  Revision 1.2  2004/04/21 23:28:44  dick
 *  Startup a UDP server (via NetServer) that allows xp5 to connect to us,
 *  monitor our health, and shut us down.
 *
 *  Revision 1.1  2004/04/16 21:07:59  dick
 *  NetServer becomes a little UDP engine that can be plugged into different apps.
 *
 */

#ifndef	_NETSERVERSCORESERVER_H_
#define	_NETSERVERSCORESERVER_H_

#include "NetServer.h"

#define	NetServerScoreServerSUPERCLASS	NetServer
class NetServerScoreServer : public NetServerScoreServerSUPERCLASS
{
public:
	//NetServerServer();

	virtual int		GetFPS() { return(14); }
//	virtual	void	SendMetaUpdate() { MetaUpdate(theWorld[0], true); }
//	virtual	void	BroadcastPlayMessage(PCSTR msg) { theWorld[0]->BroadcastPlayMessage(msg); }

	void	ContactCleanup();
	virtual bool	ContactInit();

	void	Contact(Sockbuf& ibuf);
	int		SetupConnectionControl(PCSTR real, PCSTR nick, PCSTR dpy,
								   PCSTR addr, PCSTR host, int* port,
								   uint version, uint origVersion);

	static void ContactFromLocal(int fd, void* v);

private:
	Sock	localContactSocket;
	Sockbuf	localContactBuf;

};


#endif	// _NETSERVERSCORESERVER_H_
