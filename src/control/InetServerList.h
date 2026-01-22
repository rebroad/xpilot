/* $Id: InetServerList.h,v 1.2 2004/05/13 09:28:31 dick Exp $
 *
 * XPwhere - Where in the world can i find people playing XPilot.
 *      Copyright (C) 2001 by
 *
 *      The XPilot Authors   <xpilot@xpilot.org>
 *      Dick Balaska         <dick@xpilot.org>
 *      Jarrod Miller        <jarrod@xpilot.org>
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
 *
 * See the file COPYRIGHT.TXT for current copyright information.
 *
 */
/*
 * $Log: InetServerList.h,v $
 * Revision 1.2  2004/05/13 09:28:31  dick
 * Describe FlCheckHandler() as virtual (redundant but programmer friendly)
 *
 * Revision 1.1  2004/05/10 12:41:31  dick
 * InetServerList moves from common to control (where all his friends are)
 *
 * Revision 1.5  2004/01/06 04:48:28  dick
 * Add support for building under Cygwin.  Still needs work and the output
 * is X11 and requires something like Cygwin's XWin.exe to run under.
 *
 * Revision 1.4  2002/09/05 17:55:03  dick
 * State enum is private.
 *
 * Revision 1.3  2002/08/28 13:40:47  dick
 * Use ErrMsgHandler to output error messages.
 *
 */
#ifndef	_INETSERVERLIST_H_
#define	_INETSERVERLIST_H_

#include "ServerList.h"
#include "portability.h"

#if defined(_WINDOWS) && !defined(_CYGWIN)
#include <winsock.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#endif

class MetaHost
{
public:
	MetaHost(PCSTR _n, int _p) { name = _n; port = _p; };
	String	name;
	int		port;
};

#define	MAXMETAHOSTS	3

class ServerSite;

class InetServerList : public ServerList
{
public:
	InetServerList();
	~InetServerList();

	void	Refresh();			// reload the list

	virtual void	FlCheckHandler();	// called once before idle
	void	FlFdHandler();		// socket handler

private:
	enum State {
		Virgin,
		Resolving,
		Initializing,
		Fetching,
		Closed,
		Error,
	};

	State		state;
	int			currentMeta;
	SOCKET		sock;
	bool		nosock;			// if the connection aborted, don't send
	IN_ADDR		inaddr;
	int			networkStatus;	// = 0 for OK or last socket error
	char*		recvbuf;
	long		recvbuflen;
	long		bytesread;
	ObjList		recvList;		// As they are coming in, they get put here
	ObjList		watchList;
	bool		virgin;			// first time ever reading the list

	void		PumpState();
	void		SetState(State newState);
	bool		ResolveHost();
	bool		InitializeSock();
	int			SetBlockingMode();
	int			SetNonBlockingMode();

	void		Prelude();
	void		ProcessRecvData(char* text);
	bool		VerifyExist(ServerSite*	csi);
	void		ProcessNewServers();
	void		PostMortem();
};

#endif	// _INETSERVERLIST_H_
