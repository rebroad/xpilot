/* $Id: InetServerList.cpp,v 1.3 2004/05/14 21:24:13 dick Exp $
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
 * $Log: InetServerList.cpp,v $
 * Revision 1.3  2004/05/14 21:24:13  dick
 * Attribute renaming
 *
 * Revision 1.2  2004/05/13 09:27:52  dick
 * stateText needs to be private
 *
 * Revision 1.1  2004/05/10 12:41:31  dick
 * InetServerList moves from common to control (where all his friends are)
 *
 * Revision 1.14  2004/02/03 03:39:52  dick
 * Add Windows precompiled header support.
 *
 * Revision 1.13  2004/02/03 00:41:13  dick
 * Don't try to process an fltk fd callback if the socket isn't ready for it.
 *
 * Revision 1.12  2004/01/26 16:56:53  dick
 * Add a couple of Fl::check() to refresh the display during slow networking
 *
 * Revision 1.11  2004/01/06 04:48:28  dick
 * Add support for building under Cygwin.  Still needs work and the output
 * is X11 and requires something like Cygwin's XWin.exe to run under.
 *
 * Revision 1.10  2002/09/10 04:06:19  dick
 * Fl::remove_fd() the socket whenever closing the socket.
 *
 * Revision 1.9  2002/09/05 17:55:35  dick
 * Always add the server to the recvList (don't shortcut it into the sorted list)
 *
 * Revision 1.8  2002/09/05 16:53:11  dick
 * Fix bug 17: XPWhere List crashes erratically since making list sortable.
 *
 * Revision 1.7  2002/09/01 03:49:18  dick
 * lists are ObjSortables in ProcessNewServers()
 *
 * Revision 1.6  2002/08/30 05:17:24  dick
 * Make the lists contain ObjSortable->ServerSite instead of ServerSite.
 *
 * Revision 1.5  2002/08/28 13:57:27  dick
 * Pass PCSTR through ...
 *
 * Revision 1.4  2002/08/28 13:40:47  dick
 * Use ErrMsgHandler to output error messages.
 *
 * Revision 1.3  2002/08/16 18:51:26  jlmiller
 * move MetaHost to InetServerList.h
 *
 * Revision 1.2  2002/08/14 19:39:24  dick
 * Use local fltk ( "" instead of <> )
 *
 * Revision 1.1  2002/08/02 13:58:26  jlmiller
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
#include "InetServerList.h"
#include "Fl_ServerList.h"
#include "sockerrs.h"

#if defined(_UNIX) || defined(_CYGWIN)
#include <unistd.h>
#include <sys/ioctl.h>
#endif


// The sort types
#define	SORT_NONE			0
#define	SORT_PLAYERCOUNT	1
//#define	SORT_DISTANCE		2



MetaHost	metaHosts[MAXMETAHOSTS] = {
	MetaHost("meta.xpilot.org", 4401),
	MetaHost("meta2.xpilot.org", 4401),
	MetaHost("bing.buckosoft.com", 4401),
};
/*
 * bing is a meta?
 * no.  In bing's /etc/inetd.conf i have this line:
 * 4401    stream  tcp nowait  nobody  /usr/sbin/tcpd  /bin/cat /home/dick/tmp/meta.txt
 * and i do telnet meta.xpilot.org 4401 >/home/dick/tmp/meta.txt
 * (and don't forget to delete the first 3 lines from meta.txt).
 * This gives me a fixed controllable data source for debugging.
 */

#ifndef	DEFAULTMETA
#define	DEFAULTMETA	0		// right now 2 is a flat file at bing
#endif

static PCSTR	stateText[] = {
	"Virgin",
	"Resolving meta name",
	"Initializing socket",
	"Receiving data",
	"Done",
	"Error"
};

///////////////////////////////////////////////////////////////////////////////
InetServerList::InetServerList()
{
	state = Virgin;
	currentMeta = DEFAULTMETA;
	sock = 0;
	inaddr.s_addr = 0;
	recvbuf = NULL;
	nosock = true;
	window = NULL;
	virgin = true;
}

///////////////////////////////////////////////////////////////////////////////
InetServerList::~InetServerList()
{
	if (recvbuf)
		delete recvbuf;
	Obj*	obj;
	while (GetHead())
	{
		obj = RemoveHead();
		delete obj;
	}
}

///////////////////////////////////////////////////////////////////////////////
void InetServerList::Refresh()
{
	switch (state)		// cleanup before restarting
	{
	case Fetching:
		Fl::remove_fd(sock);
		closesocket(sock);
		sock = 0;
		nosock = true;
		break;
	default:
		break;
	}
	SetState(Initializing);
}

///////////////////////////////////////////////////////////////////////////////
void InetServerList::PumpState()
{
	switch (state)
	{
	case Virgin:
		SetState(Resolving);
		break;;
	case Resolving:
		if (!ResolveHost())
			SetState(Error);
		else
			SetState(Initializing);
		break;
	case Initializing:
		if (nosock)
			InitializeSock();
		else
			emh(emhThis, EmWarning, "InetServerList:: PumpState to fetching with already a socket\n");
		SetState(Fetching);
		break;
	case Fetching:
		break;
	}
	Fl::check();
}

///////////////////////////////////////////////////////////////////////////////
void InetServerList::SetState(State newState)
{
	if (window)		// anybody wanna know?
	{
		window->ServerListStateChanged(stateText[newState]);
		Fl::check();
	}
	state = newState;
	PumpState();
}

///////////////////////////////////////////////////////////////////////////////
void InetServerList::FlCheckHandler()
{
	if (state == Virgin)
		PumpState();
}

///////////////////////////////////////////////////////////////////////////////
static void FdCallback(int fd, void* who)
{
	InetServerList* me = (InetServerList*)who;
	me->FlFdHandler();

}

///////////////////////////////////////////////////////////////////////////////
void InetServerList::FlFdHandler()
{
	char	data[1024];
	while (1)
	{
		memset(data,0, 1024);
		if (sock == 0)
		{
			return;
		}

		int ret = recv(sock, data, 1023, 0);
		if (ret == 0)
		{
			Fl::remove_fd(sock);
			closesocket(sock);
			sock = 0;
			nosock = true;
			SetState(Closed);

			return;
		}
		if (ret == SOCKET_ERROR)
		{
			if (h_errno == EWOULDBLOCK)
				return;
			emh(emhThis, EmError, "error while receiving. error=%d <%s>", h_errno, GetSockErrText(h_errno));
			SetState(Error);
			return;
		}
		data[ret] = '\0';
		ProcessRecvData(data);
	}
}

///////////////////////////////////////////////////////////////////////
void InetServerList::ProcessRecvData(char* text)
{
	char*			ns;
	long			index;
	ServerSite*		css;
	String			cs;

	if (recvbuf)
	{
		ns = new char[strlen(recvbuf)+strlen(text)+2];
		strcpy(ns, recvbuf);
		strcat(ns, text);
		delete recvbuf;
		recvbuf = ns;
		recvbuflen = strlen(recvbuf);
	}
	else
	{
		recvbuf = new char[strlen(text)+1];
		strcpy(recvbuf, text);
		recvbuflen = strlen(recvbuf);

	}

	for(index=0;;)
	{
		long	start = index;
		char*	eol;
		eol = strchr(&recvbuf[start], '\n');
		if (!eol)
		{
			if (index == recvbuflen)
			{
				delete recvbuf;
				recvbuf = NULL;
				break;
			}
			ns = new char[recvbuflen-index+1];
			strcpy(ns, &recvbuf[index]);
			delete recvbuf;
			recvbuf = ns;
			break;
		}
		else
		{
			*eol = '\0';
			cs = (PCSTR)&recvbuf[index];
			index = eol - recvbuf;
			index++;
			css = new ServerSite;
			css->ParseLine(cs);
			ObjSortable* o = new ObjSortable;
			o->o = css;
#ifdef	_DEBUG
			css->TRACEdump();
#endif
			recvList.Add(o);
		}
	}
	ProcessNewServers();
	SetState(Fetching);			// redraw
}
///////////////////////////////////////////////////////////////////////////////
bool InetServerList::VerifyExist(ServerSite* csi)
{
	ObjList*		slist = this;			// shortcut from the doc
	ServerSite*		csix;
	ObjSortable*	robj;
	bool		match = false;
	if (!slist->IsEmpty())
	{
		for (robj=(ObjSortable*)slist->GetHead(); robj != NULL; robj = (ObjSortable*)robj->GetNext())
		{
			csix = (ServerSite*)robj->o;
			if (csi->serverName == csix->serverName && csi->serverPort == csix->serverPort)
			{
				// we have a match
				match = true;
				csix->uptime = csi->uptime;
				break;
			}
		}
	}
	return(match);
}

///////////////////////////////////////////////////////////////////////////////
bool InetServerList::ResolveHost()
{
	HOSTENT*	he;
	if (inaddr.s_addr)
		return(true);
	he = gethostbyname(metaHosts[currentMeta].name);
	if (!he)
	{
		emh(emhThis, EmError, "Failed to resolve meta host \"%s\"",
			(PCSTR)metaHosts[currentMeta].name);
		return(false);
	}
	memcpy(&inaddr, he->h_addr_list[0], sizeof (IN_ADDR));
	return(true);
}

///////////////////////////////////////////////////////////////////////////////
bool InetServerList::InitializeSock()
{
	sock = socket(PF_INET, SOCK_STREAM, 0);
	nosock = false;
	if (sock == INVALID_SOCKET)
	{
		networkStatus = h_errno;
		emh(emhThis, EmError, "Failed to create socket.  error=%d <%s>\n", h_errno, GetSockErrText(h_errno));
		sock = 0;
		nosock = true;
		return(false);
	}

	SOCKADDR_IN	sin;
	memset(&sin, 0, sizeof(SOCKADDR_IN));

	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = INADDR_ANY;

	if (bind(sock, (SOCKADDR*)&sin, sizeof(SOCKADDR_IN)))
	{
		if (networkStatus != h_errno)
		{
			networkStatus = h_errno;
			emh(emhThis, EmError, "Failed to bind socket error=%d <%s>\n", h_errno, GetSockErrText(h_errno));
		}
		Fl::remove_fd(sock);
		closesocket(sock);
		sock = 0;
		nosock = true;
		return(false);
	}
	memset(&sin, 0, sizeof(SOCKADDR_IN));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(metaHosts[currentMeta].port);
	memcpy(&sin.sin_addr, &inaddr, sizeof(IN_ADDR));
	Fl::add_fd(sock, FdCallback, this);
	if (connect(sock, (SOCKADDR*)&sin, sizeof(SOCKADDR_IN)))
	{
		if (networkStatus != h_errno)
		{
			networkStatus = h_errno;
			emh(emhThis, EmError, "Failed to connect to %s:%d socket error=%d <%s>",
				(PCSTR)metaHosts[currentMeta].name, metaHosts[currentMeta].port,
				h_errno, GetSockErrText(h_errno));
		}
		Fl::remove_fd(sock);
		closesocket(sock);
		sock = 0;
		nosock = true;
		return(false);
	}
	bytesread = 0;
	return(true);
}

///////////////////////////////////////////////////////////////////////////////
void InetServerList::ProcessNewServers()
{
	ObjList*		rlist = &recvList;
	ObjList*		slist = this;
	ObjList*		wlist = &watchList;
	ObjSortable*	robj;
	ServerSite*		rsi;
	ObjSortable*	sobj;
	ServerSite*		ssi;
	Obj*			wobj;
	int				sorttype = SORT_PLAYERCOUNT;
	bool			wflag;

	if (sorttype == SORT_NONE)
		return;

	while(!rlist->IsEmpty())
	{
		bool	ins_flag = false;
		robj = (ObjSortable*)rlist->RemoveHead();
		rsi = (ServerSite*)robj->o;
//		TRACE("SortList: doing <%s> pcount=%d\n",
//			(const char*)csi->servername, csi->playercount);

// See if we can find it already in the list
		bool		match = false;
		if (!slist->IsEmpty())
		{
			for (sobj=(ObjSortable*)slist->GetHead();
				 sobj != NULL;
				 sobj = (ObjSortable*)sobj->GetNext())
			{
				ssi = (ServerSite*)sobj->o;
				if (rsi->serverName == ssi->serverName
				 && rsi->serverPort == ssi->serverPort)
				{
					// we have a match
					match = true;
					ssi->uptime = rsi->uptime;
					break;
				}
			}
		}
		if (!match)
		{
			if (!virgin)
				rsi->SetNew();
		}
		else
		{
			// ssi = already in list
			// csi = new one
			Obj*				pobj;
			Obj*				qobj;
			ServerSitePlayer*	acp;
			ServerSitePlayer*	ncp;
			bool				pmatch;

			// check the map attributes for change
			ssi->mapChanged = false;
			if (ssi->version		!= rsi->version		 ||
				ssi->mapName		!= rsi->mapName		 ||
				ssi->mapSize		!= rsi->mapSize		 ||
				ssi->author			!= rsi->author		 ||
				ssi->serverStatus	!= rsi->serverStatus ||
				ssi->bases			!= rsi->bases		 ||
				ssi->teams			!= rsi->teams		 ||
				ssi->fps			!= rsi->fps			 ||
				ssi->timing			!= rsi->timing)
			{
				ssi->mapChanged = true;
				ssi->version		= rsi->version;
				ssi->mapName		= rsi->mapName;
				ssi->mapSize		= rsi->mapSize;
				ssi->author			= rsi->author;
				ssi->serverStatus	= rsi->serverStatus;
				ssi->bases			= rsi->bases;
				ssi->teams			= rsi->teams;
				ssi->fps			= rsi->fps;
				ssi->timing			= rsi->timing;
				for (int i=0; i<MAX_TEAMS; i++)
					ssi->freebases[i]		= rsi->freebases[i];
			}

			ssi->playerCountChanged = false;
			ssi->SetNormal();
			if (ssi->playerCount != rsi->playerCount)
			{
				// check each player for a new one.
				for (pobj=rsi->playerList.GetHead(); pobj; pobj=pobj->GetNext())
				{
					ncp = (ServerSitePlayer*)pobj;
					pmatch = false;
					for (qobj=ssi->playerList.GetHead(); qobj != NULL; qobj=qobj->GetNext())
					{
						acp = (ServerSitePlayer*)qobj;
						if (acp->name == ncp->name)
						{
							pmatch = true;
							break;
						}
					}
					if (!pmatch)	// he's a newbie
					{
						ServerSitePlayer* xcp = new ServerSitePlayer(*ncp);
						ssi->playerList.Add(xcp);
//							if (pDoc->IsDing(csi))
//								pDoc->AddDings(csi, xcp);
					}
				}
				// check each player for departures
				for (pobj=ssi->playerList.GetHead(); pobj; pobj=pobj->GetNext())
				{
					acp = (ServerSitePlayer*)pobj;
					pmatch = false;
					for (qobj=rsi->playerList.GetHead(); qobj; qobj=qobj->GetNext())
					{
						ncp = (ServerSitePlayer*)qobj;
						if (acp->name == ncp->name)
						{
							pmatch = true;
							break;
						}
					}
					if (!pmatch)	// he left us
					{
						acp->SetDel();
					}
				}
				if (ssi->playerCount < rsi->playerCount)
					ssi->playerCountChanged = CHANGEDLESS;
				else
					ssi->playerCountChanged = CHANGEDMORE;
				ssi->playerCount = rsi->playerCount;
				// remove this guy from the list so we can resort him.
				slist->Remove(sobj);
				delete robj;
				rsi = ssi;
				robj = sobj;
			}
			else
			{
				ssi->refreshed = true;
//					TRACE("Sortlist: refreshed A <%s>\n", (const char*)csix->servername);
				delete rsi;	// we don't need him anymore
				continue;
			}
		}
		rsi->refreshed = true;
		// TRACE("Sortlist: refreshed B <%s>\n", (const char*)csi->servername);

		switch (sorttype)
		{
		case SORT_PLAYERCOUNT:
			if (slist->IsEmpty())
			{
				slist->Add(robj);
				ins_flag = true;
//					TRACE("SortList: IsEmpty(), addtail()\n");
				break;
			}
			wflag = false;
			bool swflag;
			for (wobj=wlist->GetHead(); wobj; wobj=wobj->GetNext())
			{
				ServerSiteWatch* csw= (ServerSiteWatch*)wobj;
				if (*csw == *rsi)
				{
					wflag = true;
					break;
				}
			}
			for (sobj=(ObjSortable*)slist->GetHead();
				 sobj;
				 sobj=(ObjSortable*)sobj->GetNext())
			{
				ssi = (ServerSite*)sobj->o;
				swflag = false;
				for (wobj=wlist->GetHead(); wobj; wobj=wobj->GetNext())
				{
					ServerSiteWatch* csw= (ServerSiteWatch*)wobj;
					if (*csw == *ssi)
					{
						swflag = true;
						break;
					}
				}
				if (swflag && !wflag)
					continue;
//					TRACE("SortList: compare to <%s> pcount=%d\n",
//						(const char*)csix->servername, csix->playercount);
				if ((wflag && !swflag) ||
					(rsi->playerCount >= ssi->playerCount))
				{
//						if (pos)
//							slist->GetPrev(pos);		// rewind
//						else
//							pos=slist->GetTailPosition();
					slist->InsertBefore(sobj, robj);
					ins_flag = true;
//						TRACE("SortList: insert before\n");
					break;
				}
			}
			break;
		default:
			break;
		}
		if (!ins_flag)
		{
			slist->Add(robj);
//				TRACE("SortList: AddTail\n");
		}
	}
	SetState(state);
}

///////////////////////////////////////////////////////////////////////
void InetServerList::Prelude()
{
#if 0
	XPwhoDoc* 		pDoc = GetDoc();
	POSITION		pos;
	POSITION		ppos;
	CObList*		slist= &pDoc->serverlist;	// shortcut from the doc
	ServerSite*	csi;
	Player*		cp;

	// now go through the list again clear refresh flags
	for (pos=slist->GetHeadPosition(); pos != NULL;)
	{
		csi = (ServerSite*)slist->GetNext(pos);
		csi->refreshed = false;
		if (csi->GetNew())
			csi->SetNormal();
		for (ppos=csi->playerList.GetHeadPosition(); ppos != NULL;)
		{
			cp = (Player*)csi->playerList.GetNext(ppos);
			if (cp->GetDel())
			{
				if (ppos)
					csi->playerList.GetPrev(ppos);		// rewind
				else
					ppos=csi->playerList.GetTailPosition();
				csi->playerList.RemoveAt(ppos);
				TRACE("Prelude: deleting player <%s>\n", (const char*)cp->name);
				delete cp;
				ppos=csi->playerList.GetHeadPosition();	// start over
			}
			else
			{
				cp->SetNormal();
			}
		}
	}
	pDoc->ClearDings();
#endif
}

///////////////////////////////////////////////////////////////////////////////
void InetServerList::PostMortem()
{
#if 0
	XPwhoDoc* 		pDoc = GetDoc();
	POSITION		pos;
	CObList*		slist= &pDoc->serverlist;	// shortcut from the doc
	ServerSite*	csi;
	bool			changed = false;
	// delete anybody who wants to get deleted
	for (pos=slist->GetHeadPosition(); pos != NULL;)
	{
		csi = (ServerSite*)slist->GetNext(pos);
		if (!csi->refreshed)
		{
			if (csi->GetDel())
			{
				if (pos)
					slist->GetPrev(pos);		// rewind
				else
					pos=slist->GetTailPosition();
				slist->RemoveAt(pos);
				pos=slist->GetHeadPosition();
				if (selection == csi)
					selection = NULL;
				TRACE("Postmortem: deleting server <%s>\n", (const char*)csi->serverName);
				delete csi;
				changed = true;
			}
		}
	}
	// now go through the list again check for deletion
	// and count the total number of players
	int	playerCount = 0;
	for (pos=slist->GetHeadPosition(); pos != NULL;)
	{
		csi = (ServerSite*)slist->GetNext(pos);
		playerCount += csi->playerCount;
		if (!csi->refreshed)
		{
			TRACE("Prelude: SetDel() server <%s>\n", (const char*)csi->serverName);
			csi->SetDel();
			changed = true;
		}
	}
	if (changed)
	{
		RedrawWindow();
	}
	char	s[50];
	sprintf(s, "Servers: %d", slist->GetCount());
	SetStatusServerCount(s);
	sprintf(s, "Players: %d", playerCount);
	SetStatusPlayerCount(s);
	if (pDoc->HasDings())
		ProcessDings();
#endif
	virgin = false;		// can't be a virgin no more.
}

///////////////////////////////////////////////////////////////////////////////
int InetServerList::SetBlockingMode()
{
	int		ret = 0;
	u_long	argp = 0;
#if defined(_WINDOWS) && !defined(_CYGWIN)
//	if (GetCfg()->cWnd && GetCfg()->cWnd->m_hWnd)
//		ret = WSAAsyncSelect(sock, GetCfg()->cWnd->m_hWnd,
//							 wMsg, NULL);
	ret = ioctlsocket(sock, FIONBIO, &argp);			// set blocking mode
#else
	ret = ioctl(sock, FIONBIO, &argp);			// set blocking mode
#endif
	if (ret)
		printf("network: can't set %d to be a blocking socket %d (%s)\n", sock, h_errno, GetSockErrText(h_errno));
//	mode = nm_blocking;
	return ret;

}

///////////////////////////////////////////////////////////////////////////////
int InetServerList::SetNonBlockingMode()
{
	int		ret = 0;
	u_long	argp = 1;
#if defined(_WINDOWS) && !defined(_CYGWIN)
//	SOTRACE("SetNonBlockingMode: socket %d\n", sock);
//	ret = WSAAsyncSelect(sock, GetCfg()->cWnd->m_hWnd,
//						 wMsg, FD_ACCEPT|FD_READ|FD_CLOSE);
	ret = ioctlsocket(sock, FIONBIO, &argp);			// set blocking mode
	if (ret)
	{
		printf("network: WSAAsyncSelect failed %d <%s>\n",h_errno, GetSockErrText(h_errno));
		return(ret);
	}
#else
	ret = ioctl(sock, FIONBIO, &argp);			// set blocking mode
	if (ret)
		printf("network: can't set %d to be a nonblocking socket %d (%s)\n", sock, h_errno, GetSockErrText(h_errno));
#endif
//	mode = nm_nonblocking;
	return(ret);
}
