/* $Id: InetServerPanel.cpp,v 1.2 2004/05/14 21:24:13 dick Exp $
 *
 * InetServerPanel - Display the XPilot servers as reported by meta,
 *
 * XPilot5 - Manage all local XPilot activity.  Copyright (C) 2001 by
 *
 *      The XPilot Authors           <xpilot@xpilot.org>
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
 * $Log: InetServerPanel.cpp,v $
 * Revision 1.2  2004/05/14 21:24:13  dick
 * Attribute renaming
 *
 * Revision 1.1  2004/05/13 09:17:08  dick
 * XPWhereWindow becomes InetServerPanel, (which matches LocalServerPanel and PrivateServerPanel).
 * XPWhereConfig becomes InetServerConfig.
 *
 * Revision 1.46  2004/05/10 17:06:55  dick
 * Get rid of "type", which was supposed to be a switch between different configs.
 * Instead use different subclasses for the config.
 *
 * Revision 1.45  2004/05/10 12:45:45  dick
 * Move the buttons at the top of XPWhereWindow to their own class ServerButtonsGroup
 * so we can use them in other lists.
 *
 * Revision 1.44  2004/03/03 07:54:35  dick
 * Don't make pixmaps at load time; wait until runtime init, after Strings have
 * actually been inited.
 *
 * Revision 1.43  2004/02/29 00:12:37  dick
 * Draw background in xpBackColor and labels in xpLabelColor.
 *
 * Revision 1.42  2004/02/10 18:17:29  dick
 * Use the symbol COL_LAG, not a bad int constant
 *
 * Revision 1.41  2004/02/09 18:22:48  dick
 * Lots of whitespace changes to support fixing resizable columns.
 *
 * Revision 1.40  2004/02/05 04:45:08  dick
 * Don't crash when pinging servers.
 *
 * Revision 1.39  2004/02/03 03:39:53  dick
 * Add Windows precompiled header support.
 *
 * Revision 1.38  2004/01/26 03:25:53  jlmiller
 * Allow double-clicking a server to SERVERJOINGAME
 *
 * Revision 1.37  2004/01/06 04:48:28  dick
 * Add support for building under Cygwin.  Still needs work and the output
 * is X11 and requires something like Cygwin's XWin.exe to run under.
 *
 * Revision 1.36  2003/12/22 05:59:51  dick
 * Use png(s) instead of xpm(s).
 *
 * Revision 1.35  2002/09/18 14:49:22  jlmiller
 * Changes necessary for Main window to be resizable, and tiled.
 *
 * Revision 1.34  2002/09/09 23:48:52  dick
 * Use the global functions to PlayGame, AdminServer.
 * Swap the buttons around so "Join Game" is next to the "Join by team" buttons.
 *
 * Revision 1.33  2002/09/05 16:52:14  dick
 * Dim the Refresh button while refreshing, and the lag button while measuring.
 *
 * Revision 1.32  2002/09/03 22:58:00  jlmiller
 * TODO note was in wrong place.
 *
 * Revision 1.31  2002/09/02 16:29:08  dick
 * SetCfg() to the user's config
 *
 * Revision 1.30  2002/08/30 05:22:53  dick
 * Add SortByLag().  PingServers uses ServerList which now contains ObjSortables.
 *
 * Revision 1.29  2002/08/28 13:58:57  dick
 * Linux needs errno.h
 *
 * Revision 1.28  2002/08/28 13:44:23  dick
 * Add a measure lag utility.
 *
 * Revision 1.27  2002/08/22 12:28:40  dick
 * Get rid of flicker by using Fl_Double_Window
 *
 * Revision 1.26  2002/08/20 16:34:24  jlmiller
 * More usable pass at configurable XPWhere window.
 * Still a few bugs to work smash.
 *
 * Revision 1.25  2002/08/20 10:47:29  jlmiller
 * File & Class Renamings:
 * ViewWindow -> MapEditWindow
 * XpInput -> XPInput
 * xpeTypes -> xpcTypes
 *
 * Revision 1.24  2002/08/16 18:56:03  jlmiller
 * First pass as configurable XPWhereWindows. Very incomplete still.
 *
 * Revision 1.23  2002/08/15 20:58:56  jlmiller
 *
 * We use bool/true/false not BOOL/TRUE/FALSE
 *
 * Revision 1.22  2002/08/15 18:47:12  jlmiller
 * Update teambuttons dynamically.
 *
 * Revision 1.21  2002/08/06 14:51:20  jlmiller
 * change commandline generation so -team works. Remove
 * unused Constructor's, as they were out of date.
 *
 * Revision 1.20  2002/08/02 20:24:48  jlmiller
 * Add teamjoin buttons.
 *
 * Revision 1.19  2002/08/02 13:59:38  jlmiller
 * Create InetServerLists instead of ServerList.
 *
 * Revision 1.18  2002/08/01 20:27:22  dick
 * Better KDE detection.  Install man pages.
 *
 * Rename programs:
 * src/         Windows              Unix
 * control      XPilot.exe           xpilot
 * client       XPilotClient.exe     xpilotclient
 * server       XPilotServer.exe     xpilotserver
 * scoreserver  XPScoreServer.exe    xpscoreserver
 * replay       XPReplay.exe         xpreplay
 *
 * Revision 1.17  2002/07/30 19:32:57  dick
 * If there is a password, then just connect when admining a server.
 *
 * Revision 1.16  2002/07/25 01:25:52  dick
 * Build a correct command line to launch the client (xpilot++) from Linux.
 *
 * Revision 1.15  2002/07/24 07:33:37  dick
 * Build the Windows command line correctly to launch XPilot.
 *
 * Revision 1.14  2002/07/24 07:08:48  dick
 * Support the -port command line switch for Windows.
 *
 * Revision 1.13  2002/07/23 19:58:24  jlmiller
 * No, actually the #include "" was right the first time, so lets put it back.
 *
 * Revision 1.12  2002/07/23 18:04:15  dick
 * Fetch the password for the server we are connecting to.
 *
 * Revision 1.11  2002/07/23 13:24:12  jlmiller
 * #includes should be <> instead of ""
 *
 * Revision 1.10  2002/07/19 07:23:59  dick
 * Add a pane for the Local Server button.
 *
 * Revision 1.9  2002/07/15 19:13:23  jlmiller
 * Provide a (very) basic ClientConfig window.
 *
 * Revision 1.8  2002/07/15 04:04:38  dick
 * Bigger buttons/icons with some slightly more meaningful graphics.
 *
 * Revision 1.7  2002/07/14 23:04:36  jlmiller
 * Move status line to main window. Don't create ServerList's till
 * we need them.
 *
 * Revision 1.6  2002/07/12 19:03:10  jlmiller
 * First pass at serverlist refresh button
 *
 * Revision 1.5  2002/07/12 16:48:07  dick
 * Unix launches xpilot++ from the installed bin directory
 *
 * Revision 1.4  2002/07/12 15:00:06  jlmiller
 * Integrate tabbed main window, with clean up of globals.
 *
 * Revision 1.3  2002/07/10 04:05:27  dick
 * CreateXPWhereWin() becomes XPWhereWindow::Factory()
 */
#include "StdAfx.h"

#if defined(_UNIX) || defined(_CYGWIN)
# include <unistd.h>
# include <errno.h>
#else
# include <sys/types.h>
# include <sys/timeb.h>
# include "sockerrs.h"
#endif


#include "FL/Fl_Window.H"
#include "FL/Fl_PNG_Image.H"

#include "Fl_XPColors.h"
#include "FlEventNames.h"

#include "config.h"
#include "portability.h"
#include "cstring.h"
#include "controlconst.h"
#include "guiconst.h"
#include "proto.h"
#include "pack.h"
#include "xpcTypes.h"
#include "Sock.h"
#include "Sockbuf.h"
#include "InetServerPanel.h"
#include "XPilotControlWindow.h"
#include "InetServerConfig.h"
#include "IniXPilot.h"
#include "XPilotControl.h"
#include "InetServerList.h"
#include "ServerButtonsGroup.h"

#define SERVERLISTWIN_TOP		40
#define	SERVERLISTWIN_HEIGHT	(XPWHEREWIN_HEIGHT-SERVERLISTWIN_TOP)

//static void refreshlist_cb(Fl_Widget* w, void* v);
//static void joingame_cb(Fl_Widget* w, void* v);
//static void admingame_cb(Fl_Widget* w, void* v);
//static void measurelag_cb(Fl_Widget* w, void* v);
//static void xpwhereconfig_cb(Fl_Widget* w, void* v);

///////////////////////////////////////////////////////////////////////////////
InetServerPanel::InetServerPanel(int x, int y, int w,int h,const char *l)
        : Fl_Double_Window(x,y,w,h,l)
{
	Fl_Box* bigbox = new Fl_Box(FL_DOWN_BOX, 0, 0, w, h, "");
	bigbox->color(xpBackColor);

	//Group everything so it all moves together right.
//	Fl_Group *o = new Fl_Group(0, 0, w, h, "");

	fsl = new Fl_ServerList(0, SERVERLISTWIN_TOP, w, h-SERVERLISTWIN_TOP, l);
	fsl->SetServerList(new InetServerList);
	fsl->SetCfg(&iniXPilot.inetSlCfg);
	fsl->ShowPlayers();

	serverButtonsGroup = new ServerButtonsGroup(0, 0, w, h, &InetServerPanel::ServerButtonsCallback, this);
	isConfig = NULL;
}

///////////////////////////////////////////////////////////////////////////////
void InetServerPanel::SetErrMsgHandler(ErrMsgHandler _emh, void* _emhThis)
{
	emh = _emh;
	emhThis = _emhThis;
	fsl->SetErrMsgHandler(emh, emhThis);
}

///////////////////////////////////////////////////////////////////////////////
void InetServerPanel::ServerButtonsCallback(int id, void* v)
{
	InetServerPanel* xpw = (InetServerPanel*)v;
	xpw->ServerButtonsCallback(id);
}

///////////////////////////////////////////////////////////////////////////////
void InetServerPanel::ServerButtonsCallback(int id)
{
	ServerSite	si;

	switch (id)
	{
	case IDM_REFRESH_LIST:
	{
		InetServerList*	sl = (InetServerList *) fsl->GetServerList();
		serverButtonsGroup->actionButtons[ButtonRefresh]->deactivate();
		Fl::flush();
		sl->Refresh();
		serverButtonsGroup->actionButtons[ButtonRefresh]->activate();
		break;
	}
	case IDM_JOIN_GAME:
	case IDM_JOINTEAM0:
	case IDM_JOINTEAM1:
	case IDM_JOINTEAM2:
	case IDM_JOINTEAM3:
	case IDM_JOINTEAM4:
	case IDM_JOINTEAM5:
	case IDM_JOINTEAM6:
	case IDM_JOINTEAM7:
	case IDM_JOINTEAM8:
	case IDM_JOINTEAM9:
	{
	//		int row = fsl->select_start_row();
		int	team = -1;
		bool b = fsl->GetSelected(si);
		if (b)
		{
			if (id >= IDM_JOINTEAM0 && id <= IDM_JOINTEAM9)
				team = id - IDM_JOINTEAM0;
			PlayGame(si.serverIP, si.serverPort, team);
		}
		break;
	}
	case IDM_ADMIN_GAME:
	{
		int row = fsl->select_start_row();
		bool b = fsl->GetSelected(si);
		if (b)
		{
			AdminServer(si.serverName, si.serverIP, si.serverPort);
		}
		break;
	}
	case IDM_MEASURE_LAG:
	{
		serverButtonsGroup->actionButtons[ButtonMeasureLag]->deactivate();
		Fl::flush();
		PingServers();
		SortByLag();
		redraw();
		serverButtonsGroup->actionButtons[ButtonMeasureLag]->activate();
		break;
	}
	case IDM_XPWHERE_CONFIG:
	{
		if (isConfig == NULL)
			isConfig = InetServerConfig::Factory();
		else
			isConfig->show();

		isConfig->isPanel = this;
		isConfig->SetupLocalColumns();
		isConfig->UpdateDisplay();
		break;
	}
	}
}

///////////////////////////////////////////////////////////////////////////////
void InetServerPanel::UpdateButtons()
{
	ServerSite	si;
	bool	f = false;
	if (fsl->GetSelected(si))
		serverButtonsGroup->UpdateButtons(&si);
	else
		serverButtonsGroup->UpdateButtons(NULL);
}

///////////////////////////////////////////////////////////////////////////////
int InetServerPanel::handle(int event)
{
	//D(PCSTR ev = GetFlEventName(event);
	//			printf("InetServerPanel::handle: ev=%s\n", ev);)
    switch (event)
	{
	//When the user clicks a server, we get notified here.
    case SELECTSERVER :
		 UpdateButtons();
		 return TRUE;
	case SERVERJOINGAME :
		ServerButtonsCallback(IDM_JOIN_GAME);
		return TRUE;
	case FL_MOVE:
		fsl->handle(event);		// maybe setup resize cursor, ignore return value
	}

    return InetServerPanelSUPERCLASS::handle(event);
}

///////////////////////////////////////////////////////////////////////////////
// From Bert's welcome.c lag measure utility

#if defined(_WINDOWS) && !defined(_CYGWIN)
#define	_gettimeofday(__a, __b)	_ftime(__a)
#else
#define	_gettimeofday(__a, __b)	gettimeofday(__a, __b)
#endif

void InetServerPanel::PingServers()
{
	static int			serial; 		/* mark pings to identify stale reply */
	const int			interval = 1000 / 14;	/* assumes we can do 14fps of pings */
	const int			tries = 1;		/* at least 1 ping for ever server.
										 * in practice we get several */
	int 				maxwait = tries * interval * fsl->GetServerList()->GetCount();
	Sock				sock;
	fd_set				input_mask, readmask;
#if defined(_WINDOWS) && !defined(_CYGWIN)
	struct _timeb		start, end;
#else
	struct timeval		start, end;
#endif
	struct timeval		timeout;

	ObjSortable*		it;
	ObjSortable*		that;
	ServerSite*			it_sip;
	ServerList*			serverList = fsl->GetServerList();
	Sockbuf 			sbuf, rbuf;
	int 				ms;
	const char	*reply_ip;
	int 				reply_port;
	unsigned			reply_magic;
	unsigned char		reply_serial, reply_status;
	int 				outstanding;

	emh(emhThis, EmInfo, "Pinging servers (%d seconds)...", (maxwait + 500) / 1000);

	if (sock.OpenUdp(NULL, 0) == -1) {
		return;
	}
	if (sock.SetNonBlocking(1) == -1) {
		sock.Close();
		return;
	}
	if (sbuf.Init(&sock, CLIENT_RECV_SIZE,
					 SOCKBUF_WRITE | SOCKBUF_DGRAM) == -1) {
		sock.Close();
		return;
	}
	if (rbuf.Init(&sock, CLIENT_RECV_SIZE,
					 SOCKBUF_READ | SOCKBUF_DGRAM) == -1) {
		sbuf.Cleanup();
		sock.Close();
		return;
	}

	FD_ZERO(&input_mask);
	FD_SET(sock.fd, &input_mask);

	it = 0;
	outstanding = 0;
	ms = 0;
	_gettimeofday(&start, NULL);	// time(
	do {
		while (outstanding < (ms / interval + 1))
		{
			if (!it)
			{
				++serial;
				serial &= 0xFF;
				if (serial == 0)
					serial = 1;

				/*
				 * Send a packet to the contact port with
				 * a valid magic number but client version
				 * zero.  The server will reply to this
				 * so that the client can tell the user
				 * what version they need.
				 *
				 * Normally this would be a CONTACT_pack but
				 * we cheat and use the packet type field as
				 * a serial number, since the server is
				 * nice enough to send back whatever we send.
				 */
				sbuf.Clear();
				sbuf.printf("%u%s%hu%c",
							  MAGIC & 0xffff, "p",
							  sock.GetPort(), serial);

				/*
				 * Assuming sort order is the most to least
				 * desirable servers, give the interesting
				 * servers first crack at more pings, making
				 * their results more accurate.
				 */
				it = (ObjSortable*)serverList->GetHead();
			}
			// it_sip = SI_DATA(it);
			it_sip = (ServerSite*)it->o;
			sock.SendDest(it_sip->serverIP, it_sip->serverPort,
						   sbuf.buf, sbuf.len);
			_gettimeofday(&it_sip->start, NULL);
			/* if it has never been pinged (pung?) mark it now
			 * as "not responding" instead of just blank.
			 */
			if (it_sip->pingTime == PING_UNKNOWN)
			{
				it_sip->pingTime = PING_NORESP;
			}
			it_sip->serial = serial;
			outstanding++;
			it = (ObjSortable*)it->GetNext();
		}
		timeout.tv_sec = 0;
		timeout.tv_usec = (interval - (ms % interval)) * 1000;
		readmask = input_mask;
		if (select(sock.fd + 1, &readmask, 0, 0, &timeout) == -1
			&& xperrno != EINTR)
		{
			break;
		}
		_gettimeofday(&end, NULL);
#if defined(_WINDOWS) && !defined(_CYGWIN)
		ms = (end.time - start.time) * 1000
		   + (end.millitm - start.millitm);
#else
		ms = (end.tv_sec - start.tv_sec) * 1000
		   + (end.tv_usec - start.tv_usec) / 1000;
#endif
		rbuf.Clear();
		if ((rbuf.len = sock.ReceiveAny(rbuf.buf, rbuf.size)) < 4)
		{
			continue;
		}
		if (outstanding > 0)
		{
			--outstanding;
		}
		if (rbuf.scanf("%u%c%c",
						 &reply_magic, &reply_serial, &reply_status) <= 0)
		{
			continue;
		}
		reply_ip = sock.GetLastAddr();
		reply_port = sock.GetLastPort();
		for (that = (ObjSortable*)serverList->GetHead();
			 serverList && that;
			 that = (ObjSortable*)that->GetNext())
		{
			// it_sip = SI_DATA(that);
			it_sip = (ServerSite*)that->o;
			if (!strcmp(it_sip->serverIP, reply_ip)
				&& reply_port == it_sip->serverPort)
			{
				int n;

				if (reply_serial != it_sip->serial) {
					/* replied to an old ping, alive but
					 * slower than `interval' at least
					 */
					it_sip->pingTime = MIN(it_sip->pingTime, PING_SLOW);
				}
				else {
#if defined(_WINDOWS) && !defined(_CYGWIN)
					n = (end.time - it_sip->start.time) * 1000
					  + (end.millitm - it_sip->start.millitm);
#else
					n = (end.tv_sec - it_sip->start.tv_sec) * 1000
					  + (end.tv_usec - it_sip->start.tv_usec) / 1000;
#endif
					it_sip->pingTime = MIN(it_sip->pingTime, n);
				}
				break;
			}
		}
	} while (ms < maxwait);
	redraw();
	emh(emhThis, EmInfo, "Ready");
	sbuf.Cleanup();
	rbuf.Cleanup();
	sock.Close();
}

///////////////////////////////////////////////////////////////////////////////
void InetServerPanel::SortByLag()
{
	ServerList* sl = fsl->GetServerList();
	sl->Sort(COL_LAG);
}
///////////////////////////////////////////////////////////////////////////////
