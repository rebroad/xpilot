/* $Id: ServerList.cpp,v 1.21 2004/05/28 17:45:46 dick Exp $
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
 * $Log: ServerList.cpp,v $
 * Revision 1.21  2004/05/28 17:45:46  dick
 * Find(name,port) should not match a server if the string is empty.
 *
 * Revision 1.20  2004/05/14 21:15:35  dick
 * Handle PrivateServerList updates
 *
 * Revision 1.19  2004/05/13 09:11:29  dick
 * Need virtual FlCheckHandler() to prime the list update/display
 *
 * Revision 1.18  2004/05/11 06:32:54  dick
 * Add FindSortable() to get the node in the list
 *
 * Revision 1.17  2004/05/10 06:27:46  dick
 * Add Find(name,port)
 *
 * Revision 1.16  2004/02/03 03:39:52  dick
 * Add Windows precompiled header support.
 *
 * Revision 1.15  2002/08/02 13:58:26  jlmiller
 * ServerList gets subclassed to InetServerList.
 * InetServerlist does what Serverlist used to.
 *
 * Revision 1.14  2002/07/16 16:06:03  jlmiller
 * Basic filter of refreshed ServerList. Eliminates duplication/append of list.
 *
 * Revision 1.13  2002/07/12 19:30:55  dick
 * Set nosock when closing the socket
 *
 * Revision 1.12  2002/07/12 18:31:45  dick
 * Refresh sets state to Initializing
 *
 * Revision 1.11  2002/06/11 00:21:43  dick
 * Set the default to be meta.xpilot.org (not the dummy bing.buckosoft.com)
 *
 * Revision 1.10  2002/06/09 18:34:26  dick
 * Player and Watch become ServerSitePlayer and ServerSiteWatch.
 *
 * Revision 1.9  2002/02/12 17:05:13  dick
 * Set the default to be meta.xpilot.org (not the dummy bing.buckosoft.com)
 *
 * Revision 1.8  2001/09/03 00:51:20  dick
 * Move the metasimulator to bing
 *
 * Revision 1.7  2001/08/09 07:36:31  dick
 * Cleanup the ServerSite(s) when destroying the list.
 *
 * Revision 1.6  2001/08/05 19:54:04  dick
 * Allow DEFAULTMETA to be overriden from the default
 *
 * Revision 1.5  2001/07/27 06:33:31  dick
 * Commentality
 *
 * Revision 1.4  2001/07/26 16:34:09  dick
 * Fix Linux networking
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
#include "StdAfx.h"

#include <string.h>
#include "cstring.h"
#include "ServerSite.h"
#include "ServerList.h"

///////////////////////////////////////////////////////////////////////////////
ServerList::ServerList()
{
	window = NULL;
}

///////////////////////////////////////////////////////////////////////////////
ServerList::~ServerList()
{
}

///////////////////////////////////////////////////////////////////////////////
ServerSite* ServerList::Find(PCSTR name, int port)
{
	for (ObjSortable* os=(ObjSortable*)GetHead(); os; os=(ObjSortable*)os->GetNext())
	{
		ServerSite*	ss = (ServerSite*)os->o;
		if (ss->serverPort == port)
		{
			if (!ss->serverName.IsEmpty() && ss->serverName == name)
				return(ss);
			if (!ss->serverIP.IsEmpty() && ss->serverIP == name)
				return(ss);
		}
	}
	return(NULL);
}
///////////////////////////////////////////////////////////////////////////////
ObjSortable* ServerList::FindSortable(PCSTR name, int port)
{
	for (ObjSortable* os=(ObjSortable*)GetHead(); os; os=(ObjSortable*)os->GetNext())
	{
		ServerSite*	ss = (ServerSite*)os->o;
		if (ss->serverPort == port)
		{
			if (ss->serverName == name || ss->serverIP == name)
				return(os);
		}
	}
	return(NULL);
}

