/*
* XPilotedit, the cross platform map editor for XPilot.  Copyright (C) 2001 by
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
 * $Log: XPilotControl.cpp,v $
 * Revision 1.47  2004/05/13 09:29:59  dick
 * Whitespace
 *
 * Revision 1.46  2004/03/03 07:54:35  dick
 * Don't make pixmaps at load time; wait until runtime init, after Strings have
 * actually been inited.
 *
 * Revision 1.45  2004/02/03 03:39:53  dick
 * Add Windows precompiled header support.
 *
 * Revision 1.44  2004/01/28 00:10:56  dick
 * PrefTab Server becomes TabServer
 *
 * Revision 1.43  2004/01/26 17:01:41  dick
 * LocalSimple panel only has Start Server, Play Game, and Change Map.
 * LocalAdvanced also has AdminServer, StartScoreServer, BrowseScoreServer
 * and dummy edit defaults and edit robots buttons.
 *
 * Revision 1.42  2004/01/20 06:13:53  dick
 * Add support for `make install` to a non-priv'd directory, like /home/dick/xpilot.
 * If we are doing a default installation (/usr/local/xpilot) then the VARDIR
 * becomes /var/xpilot .  If we are not default then use $prefix/var.
 *
 * Revision 1.41  2004/01/15 05:38:25  dick
 * Add a StartScoreServer button to the local panel
 *
 * Revision 1.40  2004/01/09 17:46:10  dick
 * Use normalFirewall for making local connections.
 *
 * Revision 1.39  2004/01/06 04:48:28  dick
 * Add support for building under Cygwin.  Still needs work and the output
 * is X11 and requires something like Cygwin's XWin.exe to run under.
 *
 * Revision 1.38  2002/09/16 21:43:23  dick
 * Use Stringized iniClient.ParseOptions()
 *
 * Revision 1.37  2002/09/12 19:59:50  dick
 * Add IniServer handling.
 *
 * Revision 1.36  2002/09/10 21:43:40  jlmiller
 *
 * These files suddenly need stdlib.h to compile on some linux machines??
 *
 * Revision 1.35  2002/09/10 05:25:50  dick
 * Use the proper linux string to launch the server
 *
 * Revision 1.34  2002/09/10 01:20:06  dick
 * Use passed parameters for Linux in PlayGame()
 *
 * Revision 1.33  2002/09/09 23:44:58  dick
 * Move StartLocalServer() AdminServer() and PlayGame() to a global location.
 *
 * Revision 1.32  2002/09/02 16:27:54  dick
 * Initialize iniClient and iniXPilot
 *
 * Revision 1.31  2002/07/23 19:58:24  jlmiller
 * No, actually the #include "" was right the first time, so lets put it back.
 *
 * Revision 1.30  2002/07/23 13:24:12  jlmiller
 * #includes should be <> instead of ""
 *
 * Revision 1.29  2002/07/15 07:02:55  dick
 * Need a dummy List_visuals()
 *
 * Revision 1.28  2002/07/15 06:54:48  dick
 * Add Ini to XPilotControl.
 *
 * Revision 1.27  2002/07/12 15:00:06  jlmiller
 * Integrate tabbed main window, with clean up of globals.
 *
 * Revision 1.26  2002/07/09 04:37:37  dick
 * Turn on WANTHELPTEXT and WANTLABELS
 *
 * Revision 1.25  2002/06/25 20:20:10  jlmiller
 * Massive restructure to clean up includes, and objectify code more.
 *
 * Revision 1.24  2002/06/23 06:27:32  dick
 * Each application that uses a ServerOptions must include the init.cpp.h
 * and provide a ServerOptionFactory
 *
 * Revision 1.23  2002/06/09 21:11:28  dick
 * App must include ServerOptions.init.cpp
 *
 * Revision 1.22  2002/06/02 22:44:05  jlmiller
 * Add undoMan to shipeditor.
 * Commentability.
 *
 * Revision 1.21  2002/05/30 14:19:26  jlmiller
 * Remove activeMapWindow global, which is unnecessay
 * MapWindow & ShipWindow store documents similarly now.
 *
 * Revision 1.20  2002/05/30 02:26:14  jlmiller
 * Eliminate unnecessary global pointer. Should help eliminate null pointer bugs.
 *
 * Revision 1.19  2002/05/28 19:29:03  jlmiller
 * lastTeamBase shouldn't be global. Hide the server tab from the preferences
 * while in the mapeditor, since it doesnt work through there anyway.
 *
 * Revision 1.18  2002/05/28 07:19:51  dick
 * Rename xpilotedit to XPilotControl
 *
 */
#include "StdAfx.h"

#include <stdlib.h>
#include "FL/Fl.H"
#include "FL/fl_ask.H"

#include "XPilotControl.h"
#include "controlconst.h"
#include "config.h"
#include "proto.h"
#include "pack.h"
#include "password.h"
#include "IniXPilot.h"
#include "IniClient.h"
#include "IniServer.h"

// On Linux, 2.95.3 (glibc 2.2.3), somewhere, string.h defines index as a macro:
// #define index(s,c) (strchr((s),(c)))
// why this confuses Fl_Input from only this module i don't know
#undef	index
#include "ServerPrefsWindow.h"
#include "ServerOptionsNetClientFl.h"

///////////////////////////////////////////////////////////////////////////////
// Server Options initialization for XPilotControl.
// Each application that uses a ServerOptions must include the init.cpp.h
// and provide a ServerOptionFactory
#define	HASTUNER		0
#define	WANTHELPTEXT	1
#define	WANTLABELS		1
#include "ServerOptions.init.cpp.h"

ServerOption*	ServerOptionFactory(ServerOptions* _parent,
				 PCSTR _name, PCSTR _altname, PCSTR _label,
				 int _length, ValType _valType,
				 PCSTR _defval,
				 int _row, int _col, int _sheet, int _visibleto,
				 TunerFunc	_tuner,
				 PCSTR _help)
{
	return(new ServerOption(_parent,
				_name, _altname, _label, _length, _valType,
				_defval,
				_row, _col, _sheet, _visibleto,
				_tuner,
				_help));
}
///////////////////////////////////////////////////////////////////////////////

const char* szAppName= APPNAME;


///////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{
	int	ret;
#if defined(_WINDOWS) && !defined(_CYGWIN)
	WSADATA	wsaData;
	ret = WSAStartup(1, &wsaData);
	if (ret)
	{
		fl_alert("Failed to initialize Windows sockets");
		return(ret);
	}
#endif
	ConfigStartup();
	iniClient.SetFileName("XPilotClient");
	iniXPilot.ParseOptions(&argc, argv);
	String	dummyRealName;
	String	dummyNickName;
	String	dummyHostName;
	String	dummyDispName;
	iniClient.ParseOptions(&argc, argv, dummyRealName, 0,
			0, 0, 0,
			0, 0,
			dummyNickName, dummyDispName, dummyHostName,
			0);
	iniServer.SetFileName("XPilotServer");
	iniServer.ParseOptions(&argc, argv);
	ret = SetupMainWin(argc, argv);

	iniXPilot.SaveAll();
	iniServer.SaveAll();
#if defined(_WINDOWS) && !defined(_CYGWIN)
	WSACleanup();
#endif
	return(ret);
}

///////////////////////////////////////////////////////////////////////////////
void List_visuals()
{
	xpprintf("Not in XPilotContol!\n");
}

///////////////////////////////////////////////////////////////////////////////
void PlayGame(PCSTR serverIP, int port, int team)
{
	String	ts;
	if (team != -1)
		ts.printf("-team %d", team);
#if defined(_WINDOWS) && !defined(_CYGWIN)
	String	cs;
	cs.printf("XPilotClient  %s -port %d %s", (PCSTR)ts.s, port, serverIP);
	WinExec((const char*)cs, SW_SHOWDEFAULT);
#else
	extern char** environ;

	pid_t pid;
	pid = fork();
	if (pid == -1)
		return;
	if (pid == 0)
	{
		String	exe;
		exe = Conf_bindir();
		exe += "xpilotclient";

		String	cs;
		cs.printf("%s -join %s -port %d %s", (PCSTR)exe, (PCSTR)ts.s, port, (PCSTR)serverIP);
		char* argv[7];
		argv[0] = "sh";
		argv[1] = "-c";
		argv[2] = (char*)(PCSTR)cs;
		argv[3] = 0;
		execve("/bin/sh", argv, environ);
		exit(127);
	}
#endif
}

///////////////////////////////////////////////////////////////////////////////
void StartLocalServer()
{
#if defined(_WINDOWS) && !defined(_CYGWIN)
	String	cs;
	cs.printf("XPilotServer");
	WinExec((const char*)cs, SW_SHOWDEFAULT);
#else
	extern char** environ;

	pid_t pid;
	pid = fork();
	if (pid == -1)
		return;
	if (pid == 0)
	{
		String	exe;
		exe = Conf_bindir();
		exe += "xpilotserver";

		char* argv[7];
		argv[0] = "sh";
		argv[1] = "-c";
		argv[2] = (char*)(PCSTR)exe;
		argv[3] = 0;
		execve("/bin/sh", argv, environ);
		exit(127);
	}
#endif
}

///////////////////////////////////////////////////////////////////////////////
void AdminServer(PCSTR serverName, PCSTR serverIP, int port)
{
	ServerPrefsWindow* spw = CreateServerPrefsWindow(NULL, TabServer);
	ServerOptionsNetClientFl*	sonc = new ServerOptionsNetClientFl;
	sonc->SetServerOptions(spw->sos);
	spw->sonc = sonc;
	//*(ServerOptionsNetClient*)sonc = si;
	sonc->SetServer(serverName, serverIP, port);
	sonc->SetWindow(spw);
	sonc->SetFirewallPortList(normalFirewall);
	String password = GetPassword(serverName, port);
	spw->sos->connectedToPassword->Set(password);
	if (password.GetLength())
		spw->sos->isConnected->Set(true);
}

///////////////////////////////////////////////////////////////////////////////
void StartScoreServer()
{
#if defined(_WINDOWS) && !defined(_CYGWIN)
	String	cs;
	cs.printf("XPScoreServer");
	WinExec((const char*)cs, SW_SHOWDEFAULT);
#else
	extern char** environ;

	pid_t pid;
	pid = fork();
	if (pid == -1)
		return;
	if (pid == 0)
	{
		String	exe;
		exe = Conf_bindir();
		exe += "xpscoreserver";

		char* argv[7];
		argv[0] = "sh";
		argv[1] = "-c";
		argv[2] = (char*)(PCSTR)exe;
		argv[3] = 0;
		execve("/bin/sh", argv, environ);
		exit(127);
	}
#endif
}

///////////////////////////////////////////////////////////////////////////////
void BrowseScoreServer()
{
	String	s;
	s.printf("http://localhost:15346/");

#if defined(_WINDOWS) && !defined(_CYGWIN)
	BOOL	ret;

	SHELLEXECUTEINFO	si;
	memset(&si, 0, sizeof(SHELLEXECUTEINFO));
	si.cbSize = sizeof(SHELLEXECUTEINFO);
	si.lpVerb = "open";
	si.lpFile = s;
	si.nShow = SW_SHOW;
	si.fMask = SEE_MASK_NOCLOSEPROCESS;

	ret = ShellExecuteEx(&si);

#else
	extern char** environ;

	pid_t pid;
	pid = fork();
	if (pid == -1)
		return;
	if (pid == 0)
	{
		String	exe;
		exe = Conf_browser();
		exe += " ";
		exe += s;

		char* argv[7];
		argv[0] = "sh";
		argv[1] = "-c";
		argv[2] = (char*)(PCSTR)exe;
		argv[3] = 0;
		execve("/bin/sh", argv, environ);
		exit(127);
	}
#endif
}
