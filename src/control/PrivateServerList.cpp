/* $Id: PrivateServerList.cpp,v 1.5 2006/09/24 04:18:43 dick Exp $
 *
 * PrivateServerList - Manage a list of servers that get manually entered.
 *      Copyright (C) 2004 by
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
 */
/*
 * $Log: PrivateServerList.cpp,v $
 * Revision 1.5  2006/09/24 04:18:43  dick
 * Don't remove the socket if it's -1
 *
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
 */
#include "StdAfx.h"

#if defined(_UNIX) || defined(_CYGWIN)
//#include <unistd.h>
//#include <sys/ioctl.h>
#endif

#include <string.h>

#include "cstring.h"
#include "pack.h"
#include "sockerrs.h"
#include "IniXPilot.h"
#include "IniClient.h"
#include "ServerSite.h"
#include "microdelay.h"

#include "PrivateServerList.h"
#include "Fl_ServerList.h"

static PCSTR	stateText[] = {
	"Virgin",
	"Initializing socket",
	"Receiving data",
	"Done",
	"Error"
};


///////////////////////////////////////////////////////////////////////////////
PrivateServerList::PrivateServerList()
{
	state = psVirgin;
	window = NULL;
	iniList = NULL;
	inRbufSelected = false;
	Fl::add_timeout(3.0, &TimerTickFl, this);
}

///////////////////////////////////////////////////////////////////////////////
PrivateServerList::~PrivateServerList()
{
	Fl::remove_timeout(TimerTickFl, this);
}

///////////////////////////////////////////////////////////////////////////////
//virtual
void PrivateServerList::SetErrMsgHandler(ErrMsgHandler _emh, void* _emhThis)
{
	PrivateServerListSUPERCLASS::SetErrMsgHandler(_emh, _emhThis);
	rbuf.SetErrMsgHandler(_emh, _emhThis);
	sbuf.SetErrMsgHandler(_emh, _emhThis);
}

///////////////////////////////////////////////////////////////////////////////
void PrivateServerList::SetIniServerList(ObjList* il)
{
	iniList = il;
	IniPrivateServer* ips;
	for (ips=(IniPrivateServer*)iniList->GetHead(); ips; ips=(IniPrivateServer*)ips->GetNext())
	{
		ObjSortable* os = new ObjSortable();
		ServerSite* ss = new ServerSite();
		os->o = ss;
		ss->serverName = ips->name;
		ss->serverIP = ips->ip;
		ss->serverPort = ips->port;
		Add(os);
	}
}

///////////////////////////////////////////////////////////////////////////////
void PrivateServerList::FlushToIni()
{
	ObjSortable*		os;
	IniPrivateServer*	ips;
	ServerSite*			ss;

	iniList->Empty();
	for (os=(ObjSortable*)GetHead(); os; os=(ObjSortable*)os->GetNext())
	{
		ss = (ServerSite*)os->o;
		ips = new IniPrivateServer();
		ips->name = ss->serverName;
		ips->ip   = ss->serverIP;
		ips->port = ss->serverPort;
		iniList->Add(ips);
	}
}

///////////////////////////////////////////////////////////////////////////////
void PrivateServerList::Refresh()
{
	switch (state)		// cleanup before restarting
	{
	case psFetching:
//		Fl::remove_fd(sock);
//		closesocket(sock);
//		sock = 0;
//		nosock = true;
		break;
	default:
		break;
	}
	SetState(psInitializing);
}

///////////////////////////////////////////////////////////////////////////////
void PrivateServerList::PumpState()
{
	switch (state)
	{
	case psVirgin:
		SetState(psInitializing);
		break;;
	case psInitializing:
		BuildWorkList();
		SetState(psFetching);
		break;
	case psFetching:
		SendServerStatus();
		break;
	case psClosed:
		CleanupSock();
		break;
	}
	Fl::check();
}

///////////////////////////////////////////////////////////////////////////////
void PrivateServerList::SetState(State newState)
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
void PrivateServerList::FlCheckHandler()
{
	if (state == psVirgin)
		PumpState();
}


///////////////////////////////////////////////////////////////////////////////
void PrivateServerList::BuildWorkList()
{
	ObjSortable*		os;
	ServerSite*			ss;
	ServerSite*			ni;

	workList.Empty();
	for (os=(ObjSortable*)GetHead(); os; os=(ObjSortable*)os->GetNext())
	{
		ss = (ServerSite*)os->o;
		ni = new ServerSite();
		*ni = *ss;
		workList.Add(ni);
	}
}
///////////////////////////////////////////////////////////////////////////////

void PrivateServerList::SendServerStatus()
{
	ServerSite*		wsi;

	if (workList.IsEmpty())
	{
		SetState(psClosed);
		return;
	}
	if (!OpenSock())
	{
		SetState(psError);
		return;
	}
	wsi=(ServerSite*)workList.GetHead();
	if (!ResolveHost(wsi))
	{
		workList.RemoveHead();		// we can't talk to this guy
		delete wsi;					// so remove him from the list
		SetState(psFetching);		// and go see if there are any others
		return;
	}
	SendStatusRequestToServer(wsi);
}

///////////////////////////////////////////////////////////////////////////////
bool PrivateServerList::ResolveHost(ServerSite* si)
{
	if (!si->serverIP.IsEmpty())
		return(true);
	si->serverIP = sock_get_addr_by_name(si->serverName);
	if (si->serverIP.IsEmpty())
	{
		emh(emhThis, EmError, "Failed to resolve host \"%s\"",
			(PCSTR)si->serverName);
		return(false);
	}
	return(true);
}


///////////////////////////////////////////////////////////////////////////////
void PrivateServerList::SendStatusRequestToServer(ServerSite* wsi)
{
	String	name;
	GetLoginName(name);

	sbuf.Clear();
	sbuf.printf("%u%s%hu%c", MAGIC,
		      (PCSTR)name, sbuf.sock.GetPort(), REPORT_STATUS_pack);
	sock.SendDest(wsi->serverIP, wsi->serverPort, sbuf.buf, sbuf.len);

	ServerSite* si;
	si = Find(wsi->serverName, wsi->serverPort);
	if (si)
		si->lastChecked = time(NULL);
}


///////////////////////////////////////////////////////////////////////////////
bool PrivateServerList::OpenSock()
{
	if (sock.IsValidSock())
		return(true);
	if (sock.OpenUdp(NULL, 0) == -1)
		return(false);

	if (sock.SetNonBlocking(1) == -1) {
		sock.Close();
		return(false);
	}
	if (sbuf.Init(&sock, CLIENT_RECV_SIZE,
					 SOCKBUF_WRITE | SOCKBUF_DGRAM) == -1) {
		sock.Close();
		return(false);
	}
	if (rbuf.Init(&sock, CLIENT_RECV_SIZE,
					 SOCKBUF_READ | SOCKBUF_DGRAM) == -1) {
		sbuf.Cleanup();
		sock.Close();
		return(false);
	}
	Fl::add_fd(rbuf.sock.fd, &PrivateServerList::RbufSelectedFl, this);
	return(true);
}

///////////////////////////////////////////////////////////////////////////////
void PrivateServerList::CleanupSock()
{
	if (rbuf.sock.fd != -1)
		Fl::remove_fd(rbuf.sock.fd);
	sbuf.Cleanup();
	rbuf.Cleanup();
	if (sock.IsValidSock())
		sock.Close();
}

///////////////////////////////////////////////////////////////////////////////
void PrivateServerList::RbufSelectedFl(int fd, void* myThis)
{
	PrivateServerList* psp = (PrivateServerList*) myThis;
	//xpprintf("*");
	psp->RbufSelected(fd);
}

///////////////////////////////////////////////////////////////////////////////
void PrivateServerList::RbufSelected(int fd)
{
	unsigned			reply_magic;
	unsigned char		reply_serial, reply_status;
	String				s;
	ServerSite*			si;
	ServerSite*			wsi;

	if (inRbufSelected)
		return;
	inRbufSelected = true;
	if (workList.IsEmpty())
		return;

	wsi=(ServerSite*)workList.GetHead();
	si = Find(wsi->serverName, wsi->serverPort);

	rbuf.Clear();
	microdelay(500);
	if ((rbuf.len = sock.ReceiveAny(rbuf.buf, rbuf.size)) < 4
	 || rbuf.scanf("%u%c%c",
					 &reply_magic, &reply_serial, &reply_status) <= 0)
	{
		// emh(emhThis, EmError, "PrivateServerList: Incomplete packet");
		workList.RemoveHead();
		delete wsi;
		si->SetActive(false);
		SetState(psFetching);
		inRbufSelected = false;
		return;
	}

	if (*rbuf.ptr != '\0')
	{
		if (rbuf.len < rbuf.size)
			rbuf.buf[rbuf.len] = '\0';
		else
			rbuf.buf[rbuf.size - 1] = '\0';
		s = rbuf.ptr;
//		if (rbuf.ptr[strlen(rbuf.ptr) - 1] != '\n') {
//			printf("\n");
//		}
	}
	si->ParseStatus(s);
	workList.RemoveHead();
	delete wsi;

	SetState(psFetching);
	inRbufSelected = false;
}

///////////////////////////////////////////////////////////////////////////////
//static
void PrivateServerList::TimerTickFl(void* v)
{
	PrivateServerList* me = (PrivateServerList*)v;
	me->TimerTick();
}

///////////////////////////////////////////////////////////////////////////////
void PrivateServerList::TimerTick()
{
	ObjSortable*		os;
	ServerSite*			si;
	ServerSite*			wsi;

	time_t	now = time(NULL);

	if (!workList.IsEmpty())
	{
		wsi = (ServerSite*)workList.GetHead();
		si = Find(wsi->serverName, wsi->serverPort);
		if (si)
		{
			if (si->lastChecked < now-3)	// dead server?
			{
				workList.RemoveHead();
				delete wsi;
				si->SetActive(false);
				SetState(psFetching);
			}
		}
		Fl::repeat_timeout(2.0, TimerTickFl, this);
		return;
	}

	for (os=(ObjSortable*)GetHead(); os; os=(ObjSortable*)os->GetNext())
	{
		si = (ServerSite*)os->o;
		if (si->inactive)
		{
			if (si->lastChecked < now-3)
			{
				wsi = new ServerSite();
				*wsi = *si;
				workList.Add(wsi);
			}
		}
		else
		{
			if (si->lastChecked < now-60)
			{
				wsi = new ServerSite();
				*wsi = *si;
				workList.Add(wsi);
			}
		}
	}
	if (!workList.IsEmpty())
	{
		SetState(psFetching);
		Fl::repeat_timeout(3.0, TimerTickFl, this);
	}
	else
		Fl::repeat_timeout(6.0, TimerTickFl, this);

}
