/* $Id: ServerList.h,v 1.11 2004/05/28 17:45:46 dick Exp $
 *
 * ServerList - Extend ObjList to manage a list of ServerSite(s).
 *
 * common - The common library for XPilot
 *
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
 * $Log: ServerList.h,v $
 * Revision 1.11  2004/05/28 17:45:46  dick
 * Find(name,port) should not match a server if the string is empty.
 *
 * Revision 1.10  2004/05/13 09:11:29  dick
 * Need virtual FlCheckHandler() to prime the list update/display
 *
 * Revision 1.9  2004/05/11 06:32:54  dick
 * Add FindSortable() to get the node in the list
 *
 * Revision 1.8  2004/05/10 06:27:46  dick
 * Add Find(name,port)
 *
 * Revision 1.7  2002/08/28 13:40:47  dick
 * Use ErrMsgHandler to output error messages.
 *
 * Revision 1.6  2002/08/02 13:58:26  jlmiller
 * ServerList gets subclassed to InetServerList.
 * InetServerlist does what Serverlist used to.
 *
 * Revision 1.5  2002/07/17 07:07:43  dick
 * declare forward reference for ServerSite
 *
 * Revision 1.4  2002/07/16 16:06:03  jlmiller
 * Basic filter of refreshed ServerList. Eliminates duplication/append of list.
 *
 * Revision 1.3  2001/07/26 13:03:05  dick
 * Add non-blocking to the socket.
 *
 * Revision 1.2  2001/07/26 12:13:11  dick
 * Linux XPwhere integration cleanup
 *
 * Revision 1.1  2001/07/26 10:56:36  dick
 * Manage a list of ServerSite(s) and the meta connection to fill it.
 *
 */

#ifndef	_SERVERLIST_H_
#define	_SERVERLIST_H_

#include "Obj.h"
#include "types.h"

class Fl_ServerList;
class ServerSite;

// A list of ObjSortable ServerSite
class ServerList : public ObjList
{
public:
	ServerList();
	~ServerList();

	virtual void	SetErrMsgHandler(ErrMsgHandler _emh, void* _emhThis);
	virtual void	FlCheckHandler() {}		// called once before idle

	void			SetWindow(Fl_ServerList* _w);

	ServerSite*		Find(PCSTR name, int port);
	ObjSortable*	FindSortable(PCSTR name, int port);

protected:
	Fl_ServerList* window;

protected:
	ErrMsgHandler	emh;
	void*			emhThis;
};

inline void ServerList::SetWindow(Fl_ServerList* _w) { window = _w; };
inline void ServerList::SetErrMsgHandler(ErrMsgHandler _emh, void* _emhThis)
										{ emh = _emh; emhThis = _emhThis; }
#endif	// _SERVERLIST_H_
