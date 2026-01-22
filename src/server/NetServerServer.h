/* $Id: NetServerServer.h,v 1.4 2004/04/16 21:07:59 dick Exp $
 *
 * Server subclass of the network server
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
 *  $Log: NetServerServer.h,v $
 *  Revision 1.4  2004/04/16 21:07:59  dick
 *  NetServer becomes a little UDP engine that can be plugged into different apps.
 *
 *  Revision 1.3  2004/02/23 06:01:34  dick
 *  gcc3 warnings
 *
 *  Revision 1.2  2004/02/20 06:27:47  dick
 *  SendPlayMessage becomes BroadcastPlayMessage() because it sends a play message
 *  to all connections.
 *
 *  Revision 1.1  2004/02/16 14:29:44  dick
 *  server/sched.cpp and friends become serverlib/NetServer
 *  which disconnects the udp listen and receive functionality from the game.
 *
 */

#ifndef	_NETSERVERSERVER_H_
#define	_NETSERVERSERVER_H_

#include "NetServer.h"
#include "metaserver.h"

class NetServerServer : public NetServer
{
public:
	//NetServerServer();

	virtual int		GetFPS() { return(theWorld[0]->options.framesPerSecond->GetInt()); }
	virtual	void	SendMetaUpdate() { MetaUpdate(theWorld[0], true); }
	virtual	void	BroadcastPlayMessage(PCSTR msg) { theWorld[0]->BroadcastPlayMessage(msg); }
	virtual bool	ContactInit() { return(theWorld[0]->ContactInit()); }
};


#endif	// _NETSERVERSERVER_H_
