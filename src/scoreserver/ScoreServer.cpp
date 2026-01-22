/* $Id: ScoreServer.cpp,v 1.30 2007/02/03 05:37:39 dick Exp $
 *
 * XPScoreServer - Who's on first?
 *
 * ScoreServer - The application object
 *
 *      Copyright (C) 2002 by
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
 * $Log: ScoreServer.cpp,v $
 * Revision 1.30  2007/02/03 05:37:39  dick
 * Note what we are parsing
 *
 * Revision 1.29  2007/01/29 04:51:19  dick
 * Set the engine to NULL after deleting it
 *
 * Revision 1.28  2005/03/17 22:12:14  kps
 * Get rid of warnings from makedepend about "non-portable whitespace".
 *
 * Revision 1.27  2004/05/16 00:58:45  dick
 * Comment out debug
 *
 * Revision 1.26  2004/05/07 05:52:48  dick
 * Turn off some debug
 *
 * Revision 1.25  2004/05/07 04:38:25  dick
 * Send rank data to the server.
 *
 * Revision 1.24  2004/05/04 21:07:38  dick
 * Need randommnt.h
 *
 * Revision 1.23  2004/04/22 05:57:17  dick
 * SetSchedIsLoop(false) to force the Sched() function to return after one network check
 *
 * Revision 1.22  2004/04/21 23:28:44  dick
 * Startup a UDP server (via NetServer) that allows xp5 to connect to us,
 * monitor our health, and shut us down.
 *
 * Revision 1.21  2004/04/16 21:07:59  dick
 * NetServer becomes a little UDP engine that can be plugged into different apps.
 *
 * Revision 1.20  2004/02/21 01:22:33  dick
 * Windows ScoreServer minimizes to mini task tray.
 *
 * Revision 1.19  2004/02/05 04:55:43  dick
 * Download the ServerOptions from the server and when done, if the servername
 * is empty, then request the running host name from the server with a ServerInfo call.
 * (So it matches what we're reporting to meta).
 *
 * Revision 1.18  2004/02/03 03:39:53  dick
 * Add Windows precompiled header support.
 *
 * Revision 1.17  2004/01/30 16:02:49  dick
 * Configure the scoreserver via the web interface.
 * Break out the engine configuration to it's own object; we don't want
 * to have the baggage of a big unused score engine loaded in memory if we
 * really are only interested in it's name and description.
 *
 * Revision 1.16  2004/01/28 00:00:21  dick
 * Correctly determine and use GetScoreServerDirectory() for our writable data.
 *
 * Revision 1.15  2004/01/26 17:07:06  dick
 * Add StartScoreEngine(), so we can change which engine is running on the fly.
 *
 * Revision 1.14  2004/01/24 00:46:59  dick
 * Web configuration for the ScoreServer including which ScoreEngine the user has selected.
 *
 * Revision 1.13  2004/01/09 21:57:51  dick
 * Use the default FirewallPortList for now, should be sufficient for a localhost connect.
 *
 * Revision 1.12  2004/01/09 17:35:15  dick
 * clientPortRange becomes firewallPortList
 *
 * Revision 1.11  2004/01/07 04:35:16  dick
 * More CYGWIN changes
 *
 * Revision 1.10  2002/08/01 15:02:21  dick
 * Add support for cookies.
 *
 * Revision 1.9  2002/07/17 23:58:20  dick
 * SendPlayerMessage hides details from callers about what is a netClient.
 *
 * Revision 1.8  2002/07/14 19:09:05  dick
 * Whenever we're talking to the server about PCSTR nick, also mention the PlayerType.
 *
 * Revision 1.7  2002/07/09 21:56:15  dick
 * Send the player type (human,robot,tank) with the PlayerEvent
 *
 * Revision 1.6  2002/07/09 04:48:05  dick
 * ScoreServer wants neither WANTHELPTEXT or WANTLABELS.
 *
 * Revision 1.5  2002/06/30 07:23:46  dick
 * Shouldn't check in disabling debug
 *
 * Revision 1.4  2002/06/30 06:14:48  dick
 * Add sorting.
 *
 * Revision 1.3  2002/06/26 04:43:01  dick
 * PlayerScoreEvent becomes ScorePlayerEvent
 *
 * Revision 1.2  2002/06/25 06:58:37  dick
 * Windows doesn't use MFC for new xpilot++ work.
 *
 * Revision 1.1  2002/06/25 05:13:13  dick
 * The main ScoreServer application object.
 *
 */
#include "StdAfx.h"

#if defined(_WINDOWS) && !defined(_CYGWIN)
#	include <windows.h>
#	include <direct.h>
#else
#	include <unistd.h>
#	include <stdlib.h>
#	include <string.h>
#	include <errno.h>
#endif

#include "portability.h"
#include "commonproto.h"
#include "randommt.h"
#include "ScoreServer.h"
#include "ScoreServerCfg.h"
#include "xpprintf.h"
#include "error.h"
#include "Http.h"
#include "NetList.h"
#include "XMLArchive.h"
#include "FirewallPortList.h"
#include "ScoreServerNetClient.h"
#include "ScoreEngineSelector.h"
#include "engines/ScoreEngine.h"
#include "NetServerScoreServer.h"
#include "ConnectionLocalCtl.h"

ScoreServer	theApp;		// The one and only ScoreServer
NetServer*	netServer;

static Sock		localContactSocket;
static Sockbuf	localContactBuf;

///////////////////////////////////////////////////////////////////////////////
// XML support
PCSTR	scoreServerCfgFileName = "ScoreServerCfg.xml";
#define	xarOBJECT	ScoreServer
xarDECLARES;

///////////////////////////////////////////////////////////////////////////////
// Server Options initialization for XPScoreServer.
// Each application that uses a ServerOptions must include the init.cpp.h
// and provide a ServerOptionFactory
#define	HASTUNER		0
#define	WANTHELPTEXT	0
#define	WANTLABELS		0
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
ScoreServer::ScoreServer()
{
	scoreEngine = NULL;
	netClient = NULL;
	sendRanks = false;
}

///////////////////////////////////////////////////////////////////////////////
void ScoreServer::Startup()
{
	xpprintf("%sXPilotScoreServer starting up\n", showtime());
	ScoreServerCfg::SetCfg(&cfg);
	selector.Init();
	LoadCfg();
	logLevel = cfg.logLevel;
	

#if defined(_WINDOWS) && !defined(_CYGWIN)
	if (cfg.window.right > cfg.window.left && cfg.window.bottom > cfg.window.top)
	{
		MoveWindow(GetCfg()->hWnd, cfg.window.left, cfg.window.top,
			cfg.window.right - cfg.window.left, cfg.window.bottom - cfg.window.top,
			TRUE);
	}
#endif

#ifndef	NOTRANDOM
	srand(time((time_t *)0) * GetProcessId());
	seedMT(time((time_t *)0) * GetProcessId());
#else
	srand(1000);
	seedMT(1000);
#endif

	StartScoreEngine();

	InitConnectionLocalCtl();

	netServer = new NetServerScoreServer;
	netServer->ContactInit();
	netServer->Setup(2);
	netServer->SetSchedIsLoop(false);
	netServer->InstallTimerTick(NULL, 14, this);
	// InitConnectionControl();

	netClient = new ScoreServerNetClient;
	netClient->ss = this;
	netClient->SetErrMsgHandler(&ScoreServer::ErrHandler, this);

#if defined(_WINDOWS) && !defined(_CYGWIN)
	netClient->SetWindowMsg(GetCfg()->hWnd, WMB_UDP);
#endif

	// I'm not sure if we'll ever need a ServerOptions.
	// I thought we might want the score multipliers some day.
	ServerOptions* sos = new ServerOptions;
	delete sos->mapData;		// we don't need no stinky 9MB map blocks array
	sos->mapData = NULL;

	netClient->SetServerOptions(sos);
//	netClient->SetFirewallPortList((FirewallPortList&)netClient->sos->firewallPortList->GetList());
	netList.Add(netClient);


//#ifdef	CONNECTTOSERVER		// development may not want to connect when working on this module
	if (netClient->Connect(cfg.xpServerName, cfg.xpServerPort, cfg.xpServerPasswd))
	{
		netClient->SendWebInfo(cfg.webHost, cfg.webPort, cfg.webMsg);
		netClient->SendWantPlayMessages(cfg.wantPlayMessages);
	}
//#endif	// CONNECTTOSERVER

	HttpListen* n = new HttpListen;
	netList.Add(n);

	netClient->Download();

#ifdef	_WINDOWS
	if (cfg.windowMode == 1)
	{
		ShowWindow(GetCfg()->hWnd, SC_MINIMIZE);
		ShowWindow(GetCfg()->hWnd, SW_HIDE);
	}
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////
void ScoreServer::Shutdown()
{
	netClient->Disconnect();
	xpprintf("%sScoreServerShutdown:\n", showtime());
	SaveCfg();
	if (scoreEngine)
	{
		scoreEngine->Shutdown();
		delete scoreEngine;
		scoreEngine = NULL;
	}
}

/////////////////////////////////////////////////////////////////////////////
void ScoreServer::XMLStartHandler(void *data, const char *el, const char **attr)
{
	ScoreServer* d = (ScoreServer*)data;
		TRACEXML(
			TRACE("ScoreServer: start: %s ", el);
			for (int i=0; attr[i] != NULL; i++)
				TRACE("attr[%d]=%s ", i, attr[i]);
			TRACE("\n");
		)
	if (xarMatchToken(ScoreServerCfg))
		d->cfg.SetXMLParseHandlers(d->xmlp, d->XMLStartHandler, d->XMLEndHandler);

//	if (xarMatchToken(BuckoPicNodes))	// other types in this file?
//		node::SetXMLParseHandlers(d->xmlp, d->XMLStartHandler, d->XMLEndHandler);
}

///////////////////////////////////////////////////////////////////////////////
void ScoreServer::XMLEndHandler(void *data, const char *el)
{
	// nothing to pop because we are the top
}

///////////////////////////////////////////////////////////////////////////////
void ScoreServer::LoadCfg()
{
	ulong	fsize;
	ulong	read;
	Archive	ar;

	String	cs;
	cs = GetScoreServerDirectory();
	cs += scoreServerCfgFileName;
	ar.SetFileName(cs);
	fsize = ar.SeekToEnd();
	if (!fsize) {
		xpprintf("%sCan't open config file \"%s\"\n", showtime(), (PCSTR)cs);
		return;
	}
	xpprintf("%sParsing config file \"%s\"\n", showtime(), (PCSTR)cs);
	ar.SeekToBegin();
	read = ar.Read(cs, fsize);

	xmlp = XML_ParserCreate("ISO-8859-1");
	XML_SetUserData(xmlp, this);
	SetXMLParseHandlers(xmlp, NULL, NULL);
	int ret = XML_Parse(xmlp, cs, read, TRUE);
	if (!ret)
	{
		xpprintf("%sError parsing %s at line %d:\n%s\n", showtime(),
					scoreServerCfgFileName,
					XML_GetCurrentLineNumber(xmlp),
					XML_ErrorString(XML_GetErrorCode(xmlp)));
	}
	XML_ParserFree(xmlp);
}

///////////////////////////////////////////////////////////////////////////////
void ScoreServer::SaveCfg()
{
	XMLArchive	xar;
	String	cs;
	cs = GetScoreServerDirectory();
	cs += scoreServerCfgFileName;
	xar.SetFileName(cs);
	if (!xar.OpenCreate())
	{
		warn("Error opening %s for output\n", (PCSTR)cs);
		return;
	}
	xar.WriteHead();
	cfg.XMLOut(xar);
}

///////////////////////////////////////////////////////////////////////////////
void ScoreServer::XMLOut(XMLArchive& xar)
{
}

///////////////////////////////////////////////////////////////////////////////
String	ScoreServer::GetScoreServerDirectory()
{
	String	s;
	s = Conf_vardir();
	return(s);
}

///////////////////////////////////////////////////////////////////////////////
void ScoreServer::StartScoreEngine()
{
	if (scoreEngine)
	{
		scoreEngine->Shutdown();
		delete scoreEngine;
	}
	scoreEngine = selector.Select(cfg.scoreEngine);
	if (scoreEngine)
	{
		scoreEngine->Startup();
		xpprintf("%sEngine = %s\n", showtime(), scoreEngine->GetTitle());
	}
	else
	{
		xpprintf("%s!! No engine !!\n", showtime());
	}
}

///////////////////////////////////////////////////////////////////////////////
void ScoreServer::ErrHandler(void* myThis, ErrMsgType emt, PCSTR ctl, ...)
{
	ScoreServer* ss = (ScoreServer*)myThis;
    char u[1001];
	va_list marker;

    // Figure through the extra arguments.
	strcpy(u, showtime());
	va_start(marker, ctl);
	vsnprintf(&u[strlen(u)], 1000, ctl, marker);
    va_end(marker);
	strcat(u, "\n");

	xpprintf(ErrMsgTypeToLogLevel(emt), u);
}

///////////////////////////////////////////////////////////////////////////////
void ScoreServer::TimerTick()
{ 
	netList.TimerTick(); 
	netClient->TimerTick();
	scoreEngine->TimerTick();
#ifdef	_WINDOWS
	netServer->Sched();
#endif
	netServer->mainLoops++;
	netServer->Input();
}

///////////////////////////////////////////////////////////////////////////////
void ScoreServer::UdpSelected()
{
	netClient->RbufSelected();
}

///////////////////////////////////////////////////////////////////////////////
void WritePidFile()
{
#ifdef	_UNIX
	pid_t	myPid;
	myPid = getpid();
	PCSTR	s = VARRUN;
	String	f;
	FILE*	fp;
	f.printf("%s/%s.pid", (PCSTR)s, GetCfg()->appname);
	if (!(fp = fopen(f, "w")))
	{
		fprintf(stderr, "Can't open pid file <%s>\n", (PCSTR)f);
	}
	else
	{
		fprintf(fp, "%d\n", myPid);
		fclose(fp);
	}
#endif
}

///////////////////////////////////////////////////////////////////////////////
void ScoreServer::ReceiveGetScore(PCSTR nick, PlayerType pt, uint cookie)
{
	DFLOAT	score;
	int		kills;
	int		deaths;

	scoreEngine->GetInitialScore(nick, pt, cookie, score, kills, deaths);
	netClient->SendScore(nick, pt, cookie, score, kills, deaths);
}

///////////////////////////////////////////////////////////////////////////////
void ScoreServer::ReceiveSetScore(PCSTR nick, PlayerType pt, DFLOAT score, 
								  int kills, int deaths, uint cookie)
{
	scoreEngine->SetPlayerValues(nick, pt, cookie, score, kills, deaths);
}

///////////////////////////////////////////////////////////////////////////////
void ScoreServer::ReceivePlayerEvent(PCSTR name, PlayerType pt, ScorePlayerEvent pse,
									 PCSTR real, PCSTR host, PCSTR addr, uint cookie)
{
	scoreEngine->ReceivePlayerEvent(name, pt, pse, real, host, addr, cookie);
}

///////////////////////////////////////////////////////////////////////////////
void ScoreServer::ReceiveScoreEvent(PCSTR killer, PlayerType ptr, DFLOAT wscore,
									PCSTR killee, PlayerType pte, DFLOAT lscore, 
									ScoreType st)
{
	scoreEngine->ReceiveScoreEvent(killer, ptr, wscore, killee, pte, lscore, st);
}

///////////////////////////////////////////////////////////////////////////////
void ScoreServer::ReceiveGetRanks(bool onOff)
{
	sendRanks = onOff;
	scoreEngine->ReceiveGetRanks(onOff);
}

///////////////////////////////////////////////////////////////////////////////
void ScoreServer::SendPlayerMessage(PCSTR message, PCSTR player, PlayerType pt)
{
	if (!netClient)
		return;
	netClient->SendPlayerMessage(message, player, pt);
}

///////////////////////////////////////////////////////////////////////////////
void ScoreServer::SendPlayerRank(PCSTR player, PlayerType pt, int rank, DFLOAT rate)
{
	if (!netClient || !sendRanks)
		return;
	netClient->SendPlayerRank(player, pt, rank, rate);
	//D(xpprintf("SendPlayerRank: %s, %d %6.2f\n", player, rank, rate);)
}
