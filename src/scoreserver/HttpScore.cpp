/* $Id: HttpScore.cpp,v 1.18 2005/03/17 22:12:14 kps Exp $
 *
 * XPScoreServer - Who's on first?
 * Subclass Http for the ScoreServer
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
 * $Log: HttpScore.cpp,v $
 * Revision 1.18  2005/03/17 22:12:14  kps
 * Get rid of warnings from makedepend about "non-portable whitespace".
 *
 * Revision 1.17  2004/02/05 05:19:35  dick
 * Tweak the HTML a little bit.
 *
 * Revision 1.16  2004/02/05 04:53:53  dick
 * TokenSubst() can take ServerOptions and put them on the web page.
 *
 * Revision 1.15  2004/02/03 03:39:53  dick
 * Add Windows precompiled header support.
 *
 * Revision 1.14  2004/01/30 16:02:49  dick
 * Configure the scoreserver via the web interface.
 * Break out the engine configuration to it's own object; we don't want
 * to have the baggage of a big unused score engine loaded in memory if we
 * really are only interested in it's name and description.
 *
 * Revision 1.13  2004/01/26 17:07:06  dick
 * Add StartScoreEngine(), so we can change which engine is running on the fly.
 *
 * Revision 1.12  2004/01/24 00:46:59  dick
 * Web configuration for the ScoreServer including which ScoreEngine the user has selected.
 *
 * Revision 1.11  2004/01/07 04:39:12  dick
 * More CYGWIN changes
 *
 * Revision 1.10  2002/09/06 19:41:07  dick
 * Add lastVisit as a player attribute
 *
 * Revision 1.9  2002/07/19 19:20:25  dick
 * Calculate the rank from all players, always including robots.
 *
 * Revision 1.8  2002/07/17 23:57:28  dick
 * Use SortSortableInto instead of SortInto
 *
 * Revision 1.7  2002/07/14 19:33:16  dick
 * Cast the String to PCSTR
 *
 * Revision 1.6  2002/07/14 19:07:45  dick
 * Toggle whether we're displaying robots in the list or not.
 *
 * Revision 1.5  2002/07/08 18:14:32  dick
 * Comment out some debug
 *
 * Revision 1.4  2002/06/30 06:14:48  dick
 * Add sorting.
 *
 * Revision 1.3  2002/06/26 04:42:49  dick
 * Use strcasecmp, not stricmp
 *
 * Revision 1.2  2002/06/25 06:58:37  dick
 * Windows doesn't use MFC for new xpilot++ work.
 *
 * Revision 1.1  2002/06/25 05:10:55  dick
 * Subclass the Http server to interface the ScoreServer to the WebServer.
 *
 */
#include "StdAfx.h"

#if defined(_WINDOWS) && !defined(_CYGWIN)
#	include <windows.h>
#	include <direct.h>
#	include <io.h>
#else
#	include <unistd.h>
#	include <string.h>
#endif

#include <fcntl.h>

#include "CfgBase.h"
#include "version.h"
#include "error.h"
#include "HttpScore.h"
#include "xpprintf.h"		// for liveLogEntries
#include "ScoreServerCfg.h"
#include "ScoreServer.h"
#include "ScoreServerNetClient.h"
#include "engines/ScoreEngine.h"
#include "engines/ScoreEngineCfg.h"

PCSTR	s_about			= "about";
PCSTR	s_logs			= "logs";
PCSTR	s_xpName		= "xpName";
PCSTR	s_xpPort		= "xpPort";
PCSTR	s_ssName		= "ssName";
PCSTR	s_ssPort		= "ssPort";
PCSTR	s_logmem		= "logmem";
PCSTR	s_loglevel		= "loglevel";
PCSTR	s_logToFile		= "logToFile";
PCSTR	s_es			= "es";			// engine selector
PCSTR	s_config		= "config";
PCSTR	s_admin			= "admin";
PCSTR	s_scorelist		= "scorelist";
PCSTR	s_playerline	= "playerline";
PCSTR	s_pl			= "pl";
PCSTR	s_cfgEngine		= "cfgEngine";
PCSTR	s_refreshRate	= "refreshRate";
PCSTR	s_welcomeHtml	= "welcomeHtml";
PCSTR	s_SELECTED		= "SELECTED";
PCSTR	s_CHECKED		= "CHECKED";
PCSTR	s_NULL			= "";
PCSTR	s_submit		= "submit";
PCSTR	s_type			= "type";
PCSTR	s_sort			= "sort";
PCSTR	s_robots		= "robots";
PCSTR	s_robotflag		= "robotflag";
PCSTR	s_robottoggle	= "robottoggle";
PCSTR	s_optprefix		= "opt-";

extern	PCSTR	s_lineno;
extern	PCSTR	s_nick;
extern	PCSTR	s_score;
extern	PCSTR	s_rank;
extern	PCSTR	s_kdratio;
extern	PCSTR	s_kills;
extern	PCSTR	s_deaths;
extern	PCSTR	s_sessions;
extern	PCSTR	s_playtime;
extern	PCSTR	s_lastvisit;

////////////////////////////////////////////////////////////////////////////////////////////
typedef void (HttpScore::*commfunc)();
typedef struct {
	const char*	name;
	commfunc	func;
} commfuncs;

////////////////////////////////////////////////////////////////////////////////////////////
void HttpScore::DoCommand()
{
	String	s;
	char*	ext;


	commfuncs	cfs[] = {
		{s_about,		&HttpScore::DoCommandAbout },
		{s_config,		&HttpScore::DoCommandConfig },
		{s_admin,		&HttpScore::DoCommandAdmin },
		{s_scorelist,	&HttpScore::DoCommandScoreList },
		{s_logs,		&HttpScore::DoCommandLogs },
		{s_logmem,		&HttpScore::DoCommandLogMem },
		{NULL}
	};

	LogHeaders(LOGMED);
	s = command;
	ext = strchr(s, '.');
	if (ext)
		*ext = '\0';

	commfuncs*	cf = cfs;

	while(cf->name)
	{
		if (!strcasecmp(cf->name, s))
		{
			commfunc c = cf->func;
			(this->*(cf->func))();
				return;
		}
		cf++;
	}
	printf("<HTML><BODY BGCOLOR=\"#C0A0A0\">Can't interprete command =%s=</BODY></HTML>\n", (PCSTR)s);
}

////////////////////////////////////////////////////////////////////////////////////////////
HttpScore::HttpScore()
{
	rankList = NULL;
	sortedList = NULL;
	playerList = NULL;
}

////////////////////////////////////////////////////////////////////////////////////////////
HttpScore::~HttpScore()
{
	if (rankList)
		delete rankList;
	if (sortedList)
		delete sortedList;
	if (playerList)
		delete playerList;
}
////////////////////////////////////////////////////////////////////////////////////////////
void HttpScore::PrintServer()
{
	printf("Server: XPScoreServer %s\n", VERSION);
}

////////////////////////////////////////////////////////////////////////////////////////////
void HttpScore::TokenSubst(String s)
{
	// Trace("Substituting: \"%s\"\n", (PCSTR)s);
	ObjList	opts;
	char*	paren;
	if ((paren = strchr(s, '?')) != NULL)
	{
		*paren = '\0';
		String	sopts;
		sopts = &paren[1];
		ParseGetOpts(opts, sopts);
		//Trace("sopts=<%s>\n", (PCSTR)sopts);
	}
	if (!strcmp(s, s_pl))
		HandlePlayer(opts);
	else if (!strcmp(s, s_xpName))
		printf("%s", GetServerName());
	else if (!strcmp(s, s_playerline))
		HandlePlayerLine();
	else if (!strcmp(s, s_robottoggle))
		HandleRobotToggle();
	else if (!strcmp(s, s_robotflag))
		HandleRobotFlag();
	else if (!strncmp(s, s_optprefix, sizeof(s_optprefix)))
		HandleServerOpt(opts, s.Mid(sizeof(s_optprefix)));
	else
		Http::TokenSubst(s);
}

////////////////////////////////////////////////////////////////////////////////////////////
void HttpScore::DoCommandAbout()
{
	Print200html();
	printf("<HTML><BODY BGCOLOR=\"#C0A0A0\">"
		   "The <I>XPilot5 Score Server</I><P>"
		   "Version" VERSION
//		   "<BR>Build Date" VERSION_BUILDDATE
		   "</BODY></HTML>");
}

////////////////////////////////////////////////////////////////////////////////////////////
void HttpScore::DoCommandScoreList()
{
	ProcessFile("/scorelist.html");
}

////////////////////////////////////////////////////////////////////////////////////////////
void HttpScore::HandlePlayerLine()
{
	LString*	sort = (LString*)args.Find(s_sort);
	LString*	robots = (LString*)args.Find(s_robots);
	int	sortType = PlayerSortByRank;
	if (sort)
	{
		if (!strcmp(sort->s, s_nick))
			sortType = PlayerSortByNick;
		else if (!strcmp(sort->s, s_score))
			sortType = PlayerSortByScore;
		else if (!strcmp(sort->s, s_kdratio))
			sortType = PlayerSortByKDRatio;
		else if (!strcmp(sort->s, s_kills))
			sortType = PlayerSortByKills;
		else if (!strcmp(sort->s, s_deaths))
			sortType = PlayerSortByDeaths;
		else if (!strcmp(sort->s, s_sessions))
			sortType = PlayerSortBySessions;
		else if (!strcmp(sort->s, s_playtime))
			sortType = PlayerSortByPlaytime;
		else if (!strcmp(sort->s, s_lastvisit))
			sortType = PlayerSortByLastVisit;
	}
	rankList = new ObjList;
	sortedList = new ObjList;
	playerList = new ObjList;
	
	if (robots && robots->s[0] == 'n')
	{
		ObjSortable* o = (ObjSortable*)theApp.scoreEngine->playerList.GetHead();
		while (o)
		{
			SEPlayer* sep = (SEPlayer*)o;
			if (sep->playerType != PlayerRobot)
			{
				ObjSortable*	newo = new ObjSortable;
				newo->o = o;

				playerList->Add(newo);
			}
			o = (ObjSortable*)o->GetNext();
		}
		
	}
	else
	{
		ObjSortable* o = (ObjSortable*)theApp.scoreEngine->playerList.GetHead();
		while (o)
		{
			ObjSortable*	newo = new ObjSortable;
			newo->o = o;
			playerList->Add(newo);
			o = (ObjSortable*)o->GetNext();
		}
	}
	theApp.scoreEngine->playerList.SortObjInto(*rankList, PlayerSortByRank);
	playerList->SortSortableInto(*sortedList, sortType);
	currentLine = (ObjSortable*)sortedList->GetHead();
	currentLineNumber = 0;
	while (currentLine)
	{
		currentPlayer = (SEPlayer*)currentLine->o;
		if (currentPlayer->isPlaying)
			printf("<FONT COLOR=\"00FF00\">");
		ProcessFile("/playerline.html");
		if (currentPlayer->isPlaying)
			printf("</FONT>");
		currentLine = (ObjSortable*)currentLine->GetNext();
		currentLineNumber++;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
void HttpScore::HandlePlayer(const ObjList& opts)
{
	String	value;
	LString*	type = (LString*)opts.Find(s_type);
	if (!type)
	{
		value = " *No type for player field lookup* ";
		printf(value);
		return;
	}
	if (!strcmp(type->s, s_lineno))
	{
		value = currentLineNumber + 1;
		value.Format(opts);
	}
	else if (!strcmp(type->s, s_rank))
	{
		ObjSortable*	obj = (ObjSortable*)rankList->FindSortable(currentPlayer->name);
		value = rankList->Index(obj) + 1;
		value.Format(opts);
	}
	else
		value = theApp.scoreEngine->GetPlayerValue(currentPlayer, opts);

	printf(value);
}

////////////////////////////////////////////////////////////////////////////////////////////
void HttpScore::HandleServerOpt(const ObjList& opts, const String opt)
{
	String	value;
	bool ret = theApp.netClient->GetServerOptions()->GetValue(opt, value);
	if (!ret)
	{
		value.printf("***Can't get value for opt- \"%s\" ***", (PCSTR)opt);
		printf(value);
		return;
	}
	value.Format(opts);
	printf(value);
}


////////////////////////////////////////////////////////////////////////////////////////////
void HttpScore::HandleRobotToggle()
{
	LString*	robots = (LString*)args.Find(s_robots);
	LString*	sort = (LString*)args.Find(s_sort);
	String	robout;
	String	sortout = "?sort=rank";
	if (robots && robots->s[0] == 'n')
		robout.printf("&%s=y", s_robots);
	else
		robout.printf("&%s=n", s_robots);
	if (sort)
		sortout.printf("?sort=%s", (PCSTR)sort->s);

	printf("_%s%s%s", (PCSTR)command, (PCSTR)sortout, (PCSTR)robout);
}

////////////////////////////////////////////////////////////////////////////////////////////
void HttpScore::HandleRobotFlag()
{
	LString*	robots = (LString*)args.Find(s_robots);
	if (robots)
		printf("&%s=%s", s_robots, (PCSTR)robots->s);
}

////////////////////////////////////////////////////////////////////////////////////////////
void HttpScore::PrintConfigHead(PCSTR which)
{

	printf("<HTML>\n"
		   "<HEAD><TITLE>%s</TITLE>\n"
		   "<LINK rel=\"stylesheet\" href=\"/g/styles.css\" type=\"text/css\">\n"
		   "<BODY>\n"
		   "<CENTER><H1>%s</H1><HR></CENTER>\n",
		which, which);
}


////////////////////////////////////////////////////////////////////////////////////////////
void HttpScore::DoCommandConfig()
{
	Print200html();
	PrintConfigHead("XPScoreServer Config");

	bool	helpText = true;
	bool	mustRestart = false;
	// handle any any inbound strings with the GET
	LString*	l;
	String		s;
	int			i;

	// check for new values on submit
	if (l = (LString*)args.Find(s_submit))
	{
		if (!AuthOK(authAdmin))
			return;
		////////////////////////////////////////////////
		if (l = (LString*)args.Find(s_xpName))
		{
			s = l->s;
			s.Detox();
			if (theApp.cfg.xpServerName != s)
			{
				theApp.cfg.xpServerName = s;
				mustRestart = true;
			}
		}
		////////////////////////////////////////////////
		if (l = (LString*)args.Find(s_xpPort))
		{
			sscanf(l->s, "%d", &i);
			if (theApp.cfg.xpServerPort != i)
			{
				theApp.cfg.xpServerPort = i;
				mustRestart = true;
			}
		}
		////////////////////////////////////////////////
		if (l = (LString*)args.Find(s_ssName))
		{
			s = l->s;
			s.Detox();
			if (theApp.cfg.webHost != s)
			{
				theApp.cfg.webHost = s;
				mustRestart = true;
			}
		}
		////////////////////////////////////////////////
		if (l = (LString*)args.Find(s_ssPort))
		{
			sscanf(l->s, "%d", &i);
			if (theApp.cfg.webPort != i)
			{
				theApp.cfg.webPort = i;
				mustRestart = true;
			}
		}
		////////////////////////////////////////////////
		if (l = (LString*)args.Find(s_loglevel))
			sscanf(l->s, "%d", &theApp.cfg.logLevel);
		logLevel = theApp.cfg.logLevel;
		////////////////////////////////////////////////
		if (l = (LString*)args.Find(s_logToFile))
			theApp.cfg.logToFile = TRUE;
		else
			theApp.cfg.logToFile = FALSE;
		////////////////////////////////////////////////
		if (l = (LString*)args.Find(s_welcomeHtml))
		{
			theApp.cfg.welcomeHtml = l->s;
			theApp.cfg.welcomeHtml.Detox();
		}
		////////////////////////////////////////////////
		if (l = (LString*)args.Find(s_es))
		{
			s = l->s;
			s.Detox();
			if (theApp.selector.Find(s) != theApp.selector.GetSelectedSECfg())
			{
				theApp.cfg.scoreEngine = s;
				theApp.StartScoreEngine();
			}
		}

		theApp.SaveCfg();
	}
	if (mustRestart)
		printf("<FONT SIZE=\"+2\" COLOR=\"#FF0000\">You must restart "
			   "your ScoreServer for these changes to take effect<P>\n");
	/////////////////////////

	printf("<A HREF=\"/\">Up to ScoreServer Home page</A><P>\n");
	// Do the form
	printf("<FORM METHOD=GET ACTION=_config><CENTER><TABLE BORDER=1>\n");

	/////////////////////////
	printf("<TR><TD>XPilot server name</TD><TD><INPUT TYPE=text NAME=%s size=20 value=%s></TD>\n",
		s_xpName, (PCSTR)theApp.cfg.xpServerName);
	if (helpText)
		printf("<TD>The name of the machine that is running the XPilot Server "
			   " we want to connect to.  Usually <I>localhost</I> is good. "
			   "</TD>");
	printf("</TR>\n");

	/////////////////////////
	printf("<TR><TD>XPilot server port</TD><TD><INPUT TYPE=text NAME=%s size=20 value=%d></TD>\n",
		s_xpPort, (PCSTR)theApp.cfg.xpServerPort);
	if (helpText)
		printf("<TD>The UDP port that the XPilot server is running on. "
			   " Usually <I>15345</I> is good."
			   "</TD>");
	printf("</TR>\n");

	/////////////////////////
	printf("<TR><TD>ScoreServer server name</TD><TD><INPUT TYPE=text NAME=%s size=20 value=%s></TD>\n",
		s_ssName, (PCSTR)theApp.cfg.webHost);
	if (helpText)
		printf("<TD>The name of the machine that is running the XPilot ScoreServer"
			   " (our name).  You probably don't want <I>localhost</I>"
			   " because that would prevent outsiders from viewing the ScoreServer"
			   " web pages"
			   "</TD>");
	printf("</TR>\n");

	/////////////////////////
	printf("<TR><TD>ScoreServer HTTP port<TD><INPUT TYPE=text NAME=%s size=5 value=%d></TD>\n",
		s_ssPort, theApp.cfg.webPort);
	if (helpText)
		printf("<TD>The HTTP port that the ScoreServer listens on for web requests."
			   " The default is <I>15346</I>"
			   "</TD>");
	printf("</TR>\n");

	/////////////////////////
	printf("<TR><TD ROWSPAN=2>Logging</TD><TD><SELECT NAME=%s>\n"
		   "<OPTION value=0 %s>None\n"
		   "<OPTION value=1 %s>Errors Only\n"
		   "<OPTION value=2 %s>Minimum Logging\n"
		   "<OPTION value=3 %s>Medium\n"
		   "<OPTION value=4 %s>Lots\n"
		   "<OPTION value=5 %s>Maximum</SELECT></TD>\n",
		s_loglevel,
		theApp.cfg.logLevel == LOGNONE ? s_SELECTED : "",
		theApp.cfg.logLevel == LOGERR ? s_SELECTED : "",
		theApp.cfg.logLevel == LOGMIN ? s_SELECTED : "",
		theApp.cfg.logLevel == LOGMED ? s_SELECTED : "",
		theApp.cfg.logLevel == LOGLOTS ? s_SELECTED : "",
		theApp.cfg.logLevel == LOGMAX ? s_SELECTED : "");
	if (helpText)
		printf("<TD ROWSPAN=2>System logging options</TD>");
	printf("</TR>\n");

	/////////////////////////
	printf("<TR><TD><INPUT TYPE=CHECKBOX name=%s %s>Write logs to File\n",
		s_logToFile, theApp.cfg.logToFile ? s_CHECKED : "");

	/////////////////////////
	printf(" <SELECT NAME=logType>"
		   "<OPTION value=0 %s>Append"
		   "<OPTION value=1 %s>Overwrite</SELECT></TD>\n",
		theApp.cfg.logType == LOGAPPEND ? s_SELECTED : "",
		theApp.cfg.logType == LOGOVERWRITE ? s_SELECTED : "");
	printf("</TR>\n");

	/////////////////////////
	printf("<TR><TD>Welcome Page</TD>"
		   "<TD><INPUT TYPE=text NAME=%s size=20 value=%s></TD>\n",
			s_welcomeHtml, (PCSTR)theApp.cfg.welcomeHtml);
	if (helpText)
		printf("<TD>Name of the main welcome frame (if different than default)</TD>");
	printf("</TR>\n");

	/////////////////////////
	printf("</TABLE>\n");

	////////////////////////////////////////////////////////////////////////////
	// Print the Engine Selector list
	printf("<P><TABLE BORDER=1>\n");
	printf("<TR><TD COLSPAN=3><CENTER><FONT SIZE=+2>Score Engine Selector</FONT></CENTER></TD></TR>\n");
	printf("<TR><TD>Name</TD><TD>Select</TD><TD>Description</TD></TR>\n");
	ScoreEngineCfg* secfg = (ScoreEngineCfg*)theApp.selector.list.GetHead();
	while (secfg)
	{
		printf("<TR><TD>%s</TD><TD><INPUT TYPE=radio name=%s value=\"%s\" %s></TD><TD>%s</TD></TR>\n",
			(PCSTR)secfg->name, s_es,
			(PCSTR)secfg->name,
			(secfg == theApp.selector.GetSelectedSECfg()) ? s_CHECKED : s_NULL,
			secfg->description);
		secfg = (ScoreEngineCfg*)secfg->GetNext();
	}
	printf("</TABLE>\n");
//	if (AuthenticateUser(authAdmin))
	if (AuthOK(authAdmin))
	{
		printf("<INPUT TYPE=RESET Value=\"Reset Values\">"
			   "<input type=submit width=100 name=%s Value=\"Commit Changes\">", s_submit);
	}
	printf("</CENTER></FORM>\n");
	printf("</BODY></HTML>\n");
}

////////////////////////////////////////////////////////////////////////////////////////////
void HttpScore::DoCommandAdmin()
{
	Print200html();
	String	s;
	s.printf("XPScoreServer %s Score Engine Administration",
		theApp.scoreEngine->GetTitle());
	PrintConfigHead(s);

	bool	helpText = true;
	bool	mustRestart = false;
	// handle any any inbound strings with the GET
	LString*	l;

	// check for new values on submit
	if (l = (LString*)args.Find(s_submit))
	{
		if (!AuthOK(authAdmin))
			return;
		mustRestart = theApp.scoreEngine->OnAdminSubmit(this);
		theApp.SaveCfg();
	}
	if (mustRestart)
		printf("<FONT SIZE=\"+2\" COLOR=\"#FF0000\">You must restart "
			   "your ScoreServer for these changes to take effect<P>\n");
	/////////////////////////

	printf("<A HREF=\"/\">Up to ScoreServer Home page</A><P>\n");
	printf("<CENTER><TABLE WIDTH=\"70%%\"><TR><TD>%s</TD><TR></TABLE></CENTER>\n",
		theApp.scoreEngine->GetDescription());
	printf("<P>\n");
	theApp.scoreEngine->OnAdminPrintTop(this);
	// Do the form
	printf("<FORM METHOD=GET ACTION=_admin><CENTER><TABLE BORDER=1>\n");

	theApp.scoreEngine->OnAdminPrintTable(this);

	/////////////////////////
	printf("</TABLE>\n");

	if (AuthOK(authAdmin))
	{
		printf("<BR><INPUT TYPE=RESET Value=\"Reset Values\">"
			   "<input type=submit width=100 name=%s Value=\"Commit Changes\">", s_submit);
	}
	printf("</CENTER></FORM>\n");
	printf("</BODY></HTML>\n");

}

////////////////////////////////////////////////////////////////////////////////////////////
void HttpScore::PrintLogHead(const char* which)
{

	printf("<HTML>"
		"<BODY BACKGROUND=\"/g/xpback.png\" BGCOLOR=\"#000000\" TEXT=\"#FFFFFF\""
		"VLINK=\"#bbaa99\" LINK=\"#ffaa00\" ALINK=\"#ff0000\" RGB=\"#000000\">"
		   "<CENTER><FONT SIZE=+3>Logs%s%s</FONT><HR></CENTER>\n",
		*which ? " - " : "", which);

}

////////////////////////////////////////////////////////////////////////////////////////////
void HttpScore::DoCommandLogs()
{
	Print200html();
	PrintLogHead("");
	printf("</BODY></HTML>\n");
}

////////////////////////////////////////////////////////////////////////////////////////////
void HttpScore::DoCommandLogMem()
{
	char	s[64];
	sprintf(s, "Last %d", GetCfg()->liveLogEntries);
	Print200html();
	PrintLogHead(s);

	printf("<PRE>");
	LString*	ls = (LString*)logEntries.GetHead();
	while (ls)
	{
		String s;
		int		l = ls->s.GetLength();
		int		i;
		for (i=0; i<l; i++)
		{
			if (ls->s[i] == '<')
				s += "&lt;";
			else if (ls->s[i] == '>')
				s += "&gt;";
			else
				s += ls->s[i];
		}
		send((PCSTR)s, s.GetLength());
		ls = (LString*)ls->GetNext();
	}
	printf("</PRE></BODY></HTML>\n");
}

#if 0
////////////////////////////////////////////////////////////////////////////////////////////
void HttpScore::DoCommandLogAll()
{
	PrintLogHead("");
	printf("</BODY></HTML>\n");
}
#endif
