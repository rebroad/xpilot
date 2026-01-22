/* $Id: ScoreServerCfg.cpp,v 1.12 2004/02/21 01:22:33 dick Exp $
 *
 * XPScoreServer - Who's on first?
 * Configuration handler
 *
 *      Copyright (C) 2002 by
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
 * $Log: ScoreServerCfg.cpp,v $
 * Revision 1.12  2004/02/21 01:22:33  dick
 * Windows ScoreServer minimizes to mini task tray.
 *
 * Revision 1.11  2004/02/03 03:39:53  dick
 * Add Windows precompiled header support.
 *
 * Revision 1.10  2004/02/02 18:48:54  dick
 * Cleanup parsing the configuration.
 *
 * Revision 1.9  2004/01/30 16:02:49  dick
 * Configure the scoreserver via the web interface.
 * Break out the engine configuration to it's own object; we don't want
 * to have the baggage of a big unused score engine loaded in memory if we
 * really are only interested in it's name and description.
 *
 * Revision 1.8  2004/01/28 00:00:21  dick
 * Correctly determine and use GetScoreServerDirectory() for our writable data.
 *
 * Revision 1.7  2004/01/07 04:35:16  dick
 * More CYGWIN changes
 *
 * Revision 1.6  2004/01/05 03:30:11  dick
 * gcc 3 does not like token pasting into a function name.  So unwind one level
 * of the xml macros, so we say ParseInt(attr) instead of ParseAttr(attr, Int).
 *
 * Revision 1.5  2002/07/09 21:57:27  dick
 * xpprintf needs showtime()
 *
 * Revision 1.4  2002/06/30 06:14:48  dick
 * Add sorting.
 *
 * Revision 1.3  2002/06/26 07:05:17  dick
 * Use xperrno instead of errno
 *
 * Revision 1.2  2002/06/26 04:40:21  dick
 * Wrap an HWND with #if _WINDOWS
 *
 * Revision 1.1  2002/06/25 05:15:12  dick
 * Provide ScoreServer configuration by subclassing CfgBase.
 *
 */
#include "StdAfx.h"

#if defined(_WINDOWS) && !defined(_CYGWIN)
#include <windows.h>
#include <direct.h>
#else
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#endif

#include <assert.h>

#include "config.h"
#include "ScoreServerCfg.h"
#include "ScoreServer.h"
#include "xpprintf.h"
#include "HttpScore.h"

//ScoreServerCfg		cfg;
int		defaultHttpPort = SCORE_SERVER_PORT;

#define	xarOBJECT	ScoreServerCfg
xarDECLARES;
xarDeclareToken(ScoreServerCfg);
xarDeclareToken(logLevel);
xarDeclareToken(logToFile);
xarDeclareToken(wantPlayMessages);
xarDeclareToken(webHost);
xarDeclareToken(webPort);
xarDeclareToken(webMsg);
xarDeclareToken(htmlDirectory);
xarDeclareToken(xpServerName);
xarDeclareToken(xpServerPort);
xarDeclareToken(xpServerPasswd);
xarDeclareToken(scoreEngine);

#if defined(_WINDOWS) && !defined(_CYGWIN)
xarDeclareToken(window);
xarDeclareToken(windowMode);
#endif

xarDefineToken(ScoreEngineSelector);

////////////////////////////////////////////////////////////////////////////////////////////
ScoreServerCfg::ScoreServerCfg()
{

	// reasonable defaults which can be overridden by the user
	helpDirectory = ".";		// this needs to be fixed for dev running
	logLevel = LOGMAX;
	logToFile = TRUE;
	webHost = "localhost";
	webPort = defaultHttpPort;
	wantPlayMessages = false;
	xpServerName = "localhost";
	xpServerPort = 15345;
	xpServerPasswd = "NOPASSWORD";
	scoreEngine = "Basic";

	startTime = time(NULL);

/*
	String	s(128);
	if (!getcwd(s.s, 128))
		xpprintf("%sFailed to get rootDirectory, errno=%d\n", showtime(), xperrno);
	else
	{
		if (s[s.GetLength()] != '\\' || s[s.GetLength()] != '/')
			s += "/";
		bootDirectory = s;
	}
*/
	bootDirectory = Conf_libdir();
	appname = APPNAME;
	htmlDirectory = Conf_libdir();
	htmlDirectory += "scoreserver/html";

	totalUpTimeAtStart = totalUpTime;

}

///////////////////////////////////////////////////////////////////////////////
void ScoreServerCfg::XMLOut(XMLArchive& xar)
{
	xar.WriteElementStart(ScoreServerCfg);
	xar.WriteIntAttr(logLevel);
	xar.WriteIntAttr(logToFile);
	xar.WriteBoolAttr(wantPlayMessages);
	xar.WriteStringAttr(webHost);
	xar.WriteIntAttr(webPort);
	xar.WriteStringAttr(webMsg);
	xar.WriteStringAttr(htmlDirectory);
	xar.WriteStringAttr(xpServerName);
	xar.WriteIntAttr(xpServerPort);
	xar.WriteStringAttr(xpServerPasswd);
	xar.WriteStringAttr(scoreEngine);
#if defined(_WINDOWS) && !defined(_CYGWIN)
	xar.WriteRectAttr(window);
	xar.WriteIntAttr(windowMode);
#endif

	theApp.selector.XMLOut(xar);		// go save the engine's cfg

	xar.WriteElementEnd(ScoreServerCfg);
}

///////////////////////////////////////////////////////////////////////////////
void ScoreServerCfg::XMLStartHandler(void *data, const char *el, const char **attr)
{
	ScoreServer* d = (ScoreServer*)data;
	TRACEXMLSTART(ScoreServerCfg);

#define	xarObjPREFIX	d->cfg
	xarParseInt(logLevel);
	xarParseInt(logToFile);
	xarParseBool(wantPlayMessages);
	xarParseString(webHost);
	xarParseInt(webPort);
	xarParseString(webMsg);
	xarParseString(htmlDirectory);
	xarParseString(xpServerName);
	xarParseInt(xpServerPort);
	xarParseString(xpServerPasswd);
	xarParseString(scoreEngine);
#if defined(_WINDOWS) && !defined(_CYGWIN)
	xarParseRect(window);
	xarParseInt(windowMode);
#endif
	xarParseObject(ScoreEngineSelector);

}

///////////////////////////////////////////////////////////////////////////////
void ScoreServerCfg::XMLEndHandler(void *data, const char *el)
{
	ScoreServer* d = (ScoreServer*)data;
	TRACEXMLEND(ScoreServerCfg);

	xarParseObjectEnd(ScoreServerCfg);
}

////////////////////////////////////////////////////////////////////////////////////////////
Obj*	ScoreServerCfg::ObjectFactory(const char* name)
{
	Obj* o;
/*	if (!strcmp(name, "sproject"))
		o = new sproject;
	else if (!strcmp(name, "bsacuser"))
		o = new bsacuser;
	else if (!strcmp(name, "user"))
		o = new bsacuser;
	else if (!strcmp(name, "renderobj"))
		o = new renderobj;
	else if (!strcmp(name, s_sclient))
		o = new sclient;
//	else if (!strcmp(name, s_engine))
//		o = new engine;
	else if ((o = engine::NewEngine(name)))
		;
	else
*/
		o = CfgBase::ObjectFactory(name);
	return(o);
}

////////////////////////////////////////////////////////////////////////////////////////////
Http*	ScoreServerCfg::HttpFactory()
{
	HttpScore* h = new HttpScore;
	return((Http*)h);
}

