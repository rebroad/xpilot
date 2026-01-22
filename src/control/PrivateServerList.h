/* $Id: PrivateServerList.h,v 1.4 2004/05/14 21:56:16 dick Exp $
 *
 * XPwhere - Where in the world can i find people playing XPilot.
 *      Copyright (C) 2001 by
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
 * $Log: PrivateServerList.h,v $
 * Revision 1.4  2004/05/14 21:56:16  dick
 * Handle timeouts and retries.  If a server isn't alive try him again in 3 seconds.
 * If a server is alive, ping him in a minute.
 *
 * Revision 1.3  2004/05/13 09:23:30  dick
 * First pass at retrieving status from each server in the list
 *
 * Revision 1.2  2004/05/11 06:49:31  dick
 * Edit/Add/Change/Delete the entries in the PrivateServerList.
 *
 * Revision 1.1  2004/05/10 12:48:00  dick
 * Add a Private Server button to the main panel, which allows the user to
 * manually track servers that don't appear in meta.
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
#ifndef	_PRIVATESERVERLIST_H_
#define	_PRIVATESERVERLIST_H_

#include "ServerList.h"
#include "portability.h"

#define	MAXMETAHOSTS	3

class ServerSite;

#define	PrivateServerListSUPERCLASS	ServerList
class PrivateServerList : public PrivateServerListSUPERCLASS
{
public:
	PrivateServerList();
	~PrivateServerList();

	virtual void	SetErrMsgHandler(ErrMsgHandler _emh, void* _emhThis);

	void	SetIniServerList(ObjList* iniList);
	void	FlushToIni();

	void	MarkAllInactive();
	void	Refresh();			// reload the list

	virtual void	FlCheckHandler();	// called once before idle
	void	FlFdHandler();		// socket handler

	static void		TimerTickFl(void* v);
	static void		RbufSelectedFl(int fd, void* myThis);

private:

	enum State {
		psVirgin,
		psInitializing,
		psFetching,
		psClosed,
		psError,
	};
	State		state;

	void		PumpState();
	void		SetState(State newState);
	void		BuildWorkList();
	void		SendServerStatus();

	bool		OpenSock();
	void		CleanupSock();
	bool		ResolveHost(ServerSite* si);

	void		TimerTick();
	void		RbufSelected(int fd);
	void		SendStatusRequestToServer(ServerSite* wsi);

	ObjList*	iniList;
	ObjList		workList;
	Sock		sock;
	Sockbuf		sbuf;
	Sockbuf		rbuf;
	double		myTimeoutValue;

	bool		inRbufSelected;

};

#endif	// _PRIVATESERVERLIST_H_
