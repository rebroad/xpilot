/* $Id: NetServer.h,v 1.5 2004/04/22 05:52:16 dick Exp $
 *
 * Drive the network for a server
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
 *  $Log: NetServer.h,v $
 *  Revision 1.5  2004/04/22 05:52:16  dick
 *  Attribute schedIsLoop is used to control the Sched() loop.  The Un*x server
 *  wants Sched() to be the main loop.  Windows server and all ScoreServers
 *  want Sched() to be pumped repeatedly by an outside source.
 *
 *  Revision 1.4  2004/04/16 21:07:59  dick
 *  NetServer becomes a little UDP engine that can be plugged into different apps.
 *
 *  Revision 1.3  2004/02/23 06:01:34  dick
 *  gcc3 warnings
 *
 *  Revision 1.2  2004/02/20 06:26:00  dick
 *  New virtual SendMetaUpdate() and BroadcastPlayMessage()
 *
 *  Revision 1.1  2004/02/16 14:29:44  dick
 *  server/sched.cpp and friends become serverlib/NetServer
 *  which disconnects the udp listen and receive functionality from the game.
 *
 */

#ifndef	_NETSERVER_H_
#define	_NETSERVER_H_

#include "types.h"
#include "Sock.h"
#include "Sockbuf.h"

typedef	void (*TimerFunc)(void* arg);
typedef	void (*InputFunc)(int fd, void* arg);

// Define a function for the receive tables
class Connection;
typedef	int	(Connection::*ConnectionReceiveFunc)();


class NetServer
{
public:
	NetServer();
	void	SetSchedIsLoop(bool b);

	int		mainLoops;
	Connection** conn;

	int		maxConnections;
	uint	timerResolution;
	int		loginInProgress;
	int		controlsConnected;
	int		numLogins;
	int		numLogouts;

	virtual bool	ContactInit() = 0;

	virtual int		GetFPS() = 0;
	virtual	void	SendMetaUpdate() {};
	virtual	void	BroadcastPlayMessage(PCSTR msg) {};

	int		Setup(int wantNumConnections);


	void	InstallTimerTick(TimerFunc func, int freq, void* world);
	void	InstallTimeout(void (*func)(void *), int offset, void *arg, void* world);
	void	RemoveTimeout(void (*func)(void *), void *arg);
	void	InstallInput(InputFunc func, int fd, void *arg);
	void	RemoveInput(int fd);
	void	Sched();
	void	StopSched();
	int		Input();

protected:
	bool	ContactInitPort(Sockbuf& ibuf, Sock& initSock, PCSTR addr, int port);

private:
	void	SetupTimer();
	bool	schedIsLoop;
};

inline void	NetServer::SetSchedIsLoop(bool b) { schedIsLoop = b; };

extern	NetServer*	netServer;	// each app has one NetServer

#ifdef	_WINDOWS
extern	void CALLBACK NetServerTimerProc(HWND hwnd, UINT uMsg, UINT idEvent, DWORD dwTime);
#endif

#endif	// _NETSERVER_H_
