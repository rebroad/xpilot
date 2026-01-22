/* $Id: ScoreEngineBasicXML.cpp,v 1.17 2004/02/03 03:39:53 dick Exp $
 *
 * XPScoreServer - Who's on first?
 * ScoreEngineBasicXML - A simple score server that extends ScoreEngineBasic
 *                       by keeping our data in an XML file.
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
 * $Log: ScoreEngineBasicXML.cpp,v $
 * Revision 1.17  2004/02/03 03:39:53  dick
 * Add Windows precompiled header support.
 *
 * Revision 1.16  2004/02/03 00:40:17  dick
 * Properly parse the Player objects again.
 *
 * Revision 1.15  2004/02/02 18:48:54  dick
 * Cleanup parsing the configuration.
 *
 * Revision 1.14  2004/01/30 16:17:26  dick
 * Use 0 instead of NULL for time_t = 0
 *
 * Revision 1.13  2004/01/30 16:02:49  dick
 * Configure the scoreserver via the web interface.
 * Break out the engine configuration to it's own object; we don't want
 * to have the baggage of a big unused score engine loaded in memory if we
 * really are only interested in it's name and description.
 *
 * Revision 1.12  2004/01/28 00:00:21  dick
 * Correctly determine and use GetScoreServerDirectory() for our writable data.
 *
 * Revision 1.11  2004/01/26 17:02:37  dick
 * XPilot writable data goes in the vardir
 *
 * Revision 1.10  2004/01/24 00:46:59  dick
 * Web configuration for the ScoreServer including which ScoreEngine the user has selected.
 *
 * Revision 1.9  2004/01/05 03:30:11  dick
 * gcc 3 does not like token pasting into a function name.  So unwind one level
 * of the xml macros, so we say ParseInt(attr) instead of ParseAttr(attr, Int).
 *
 * Revision 1.8  2002/09/06 19:41:07  dick
 * Add lastVisit as a player attribute
 *
 * Revision 1.7  2002/08/01 15:02:21  dick
 * Add support for cookies.
 *
 * Revision 1.6  2002/07/30 21:57:42  jlmiller
 *
 * must include <string.h>
 *
 * Revision 1.5  2002/07/18 00:01:10  dick
 * Call the superclass's Startup() after ours.
 *
 * Revision 1.4  2002/07/14 19:09:05  dick
 * Whenever we're talking to the server about PCSTR nick, also mention the PlayerType.
 *
 * Revision 1.3  2002/07/09 21:53:56  dick
 * xpprintf needs showtime()
 *
 * Revision 1.2  2002/06/26 04:35:45  dick
 * PlayerScoreEvent becomes ScorePlayerEvent
 *
 * Revision 1.1  2002/06/25 05:32:53  dick
 * This Score Engine inherits from ScoreEngineBasic and provides persistence
 * of data via XML.
 *
 */
#include "StdAfx.h"

#include <string.h>
#include "xpprintf.h"
#include "portability.h"
#include "const.h"
#include "config.h"
#include "error.h"
#include "XMLArchive.h"
#include "ScoreEngineBasicXML.h"
#include "ScoreEngineBasicXMLCfg.h"
#include "../ScoreServer.h"
#include "../HttpScore.h"

PCSTR	s_basicXMLFileName	= "ScoreEngineBasicData.xml";
PCSTR	s_flushDelay		= "flushDelay";

PCSTR ScoreEngineBasicXML::name = "BasicXML";
PCSTR ScoreEngineBasicXML::title  = "Basic XML";
PCSTR ScoreEngineBasicXML::description = 
"This engine extends the basic score server and keeps statistics in an "
"XML file that is read at startup and saved at shutdown.";

PCSTR	s_BasicXML	= ScoreEngineBasicXML::name;





///////////////////////////////////////////////////////////////////////////////
// Extend a ScoreEnginePlayer with some XML handling
class SEBPlayerXML	: public SEBPlayer
{
public:
	xarOPERATORS;

protected:
	xarHANDLERS;

};

///////////////////////////////////////////////////////////////////////////////
// XML handling for this file. (hmm, 3 xml objects in one file; most unusual)
#undef	xarOBJECT
#define	xarOBJECT	ScoreEngineBasicXML
xarDECLARES;

#undef	xarOBJECT
#define	xarOBJECT	SEBPlayerXML
xarDECLARES;

xarDeclareToken(PlayerData);
xarDeclareToken(Player);
xarDeclareToken(name);
xarDeclareToken(playerType);
xarDeclareToken(cookie);
xarDeclareToken(flags);
xarDeclareToken(score);
xarDeclareToken(kills);
xarDeclareToken(deaths);
xarDeclareToken(sessions);
xarDeclareToken(playTime);
xarDeclareToken(lastVisit);
xarDeclareToken(lastPlayTime);

///////////////////////////////////////////////////////////////////////////////
ScoreEngineBasicXML::ScoreEngineBasicXML(ScoreEngineCfg* _cfg)
: ScoreEngineBasicXMLSUPERCLASS(_cfg)
{
	cfg = (ScoreEngineBasicXMLCfg*)_cfg;
	dirtyTime = 0;
}

///////////////////////////////////////////////////////////////////////////////
ScoreEngineBasicXML::~ScoreEngineBasicXML()
{
}

///////////////////////////////////////////////////////////////////////////////
// static
ScoreEngineCfg* ScoreEngineBasicXML::Install()
{
	ScoreEngineCfg* def = new ScoreEngineBasicXMLCfg;
	def->name = name;
	def->title = title;
	def->description = description;

	return(def);
}

///////////////////////////////////////////////////////////////////////////////
void ScoreEngineBasicXML::Startup()
{
	LoadData();
	ScoreEngineBasicXMLSUPERCLASS::Startup();
}

///////////////////////////////////////////////////////////////////////////////
void ScoreEngineBasicXML::Shutdown()
{
	SaveData();
}

///////////////////////////////////////////////////////////////////////////////
void ScoreEngineBasicXML::TimerTick()
{
	ScoreEngineBasicXMLSUPERCLASS::TimerTick();
	if (!dirtyTime)
		return;
	time_t now = time(NULL);
	if (dirtyTime + cfg->flushDelay < now)
	{
		SaveData();
		dirtyTime = 0;
	}
}

///////////////////////////////////////////////////////////////////////////////
bool ScoreEngineBasicXML::OnAdminPrintTop(HttpScore* http)
{
	return(false);
}

///////////////////////////////////////////////////////////////////////////////
bool ScoreEngineBasicXML::OnAdminPrintTable(HttpScore* http)
{
	ScoreEngineBasicXMLSUPERCLASS::OnAdminPrintTable(http);
	http->printf("<TR><TD>Flush Cycle</TD>"
				 "<TD><INPUT TYPE=text NAME=%s size=5 value=%d></TD>\n",
			s_flushDelay, cfg->flushDelay);
	http->printf("<TD>How long to delay between disk saves</TD></TR>\n");
	return(false);
}

///////////////////////////////////////////////////////////////////////////////
bool ScoreEngineBasicXML::OnAdminSubmit(HttpScore* http)
{
	LString*	l;

	////////////////////////////////////////////////
	if (l = (LString*)http->args.Find(s_flushDelay))
		sscanf(l->s, "%d", &cfg->flushDelay);
	return(false);
}

///////////////////////////////////////////////////////////////////////////////
void ScoreEngineBasicXML::LoadData()
{
	uint	fsize;
	uint	read;
	Archive	ar;

	String	cs;
	cs = theApp.GetScoreServerDirectory();
	cs += s_basicXMLFileName;
	ar.SetFileName(cs);
	fsize = ar.SeekToEnd();
	if (!fsize)
		return;
	ar.SeekToBegin();
	read = ar.Read(cs, fsize);

	xmlp = XML_ParserCreate("ISO-8859-1");
	XML_SetUserData(xmlp, this);
	SetXMLParseHandlers(xmlp, NULL, NULL);
	int ret = XML_Parse(xmlp, cs, read, TRUE);
	if (!ret)
	{
		xpprintf("%sError parsing %s at line %d:\n%s\n", showtime(),
					s_basicXMLFileName,
					XML_GetCurrentLineNumber(xmlp),
					XML_ErrorString(XML_GetErrorCode(xmlp)));
	}
	XML_ParserFree(xmlp);
}

///////////////////////////////////////////////////////////////////////////////
void ScoreEngineBasicXML::SaveData()
{
	XMLArchive	xar;
	String	cs;
	cs = theApp.GetScoreServerDirectory();
	cs += s_basicXMLFileName;
	xar.SetFileName(cs);
	if (!xar.OpenCreate())
	{
		xpprintf("%sError opening %s for output\n", showtime(), (PCSTR)cs);
		return;
	}
	xar.WriteHead();
	XMLOut(xar);
	xpprintf(LOGMIN, "%sScoreEngineBasicXML: saved %s\n", showtime(), (PCSTR)cs);
}

///////////////////////////////////////////////////////////////////////////////
void ScoreEngineBasicXML::XMLOut(XMLArchive& xar) 
{
	xar.WriteElementStart(PlayerData);
	SEBPlayerXML*	p = (SEBPlayerXML*)playerList.GetHead();
	while (p)
	{
		p->XMLOut(xar);
		p = (SEBPlayerXML*)p->GetNext();
	}
	xar.WriteElementEnd(PlayerData);
}

/////////////////////////////////////////////////////////////////////////////
void ScoreEngineBasicXML::XMLStartHandler(void *data, const char *el, const char **attr)
{
	ScoreEngineBasicXML* d = (ScoreEngineBasicXML*)data;
		TRACEXML(
			Trace("ScoreServer: start: %s ", el);
			for (int i=0; attr[i] != NULL; i++)
				Trace("attr[%d]=%s ", i, attr[i]);
			Trace("\n");
		)
	if (xarMatchToken(PlayerData))
		SEBPlayerXML::SetXMLParseHandlers(d->xmlp, d->XMLStartHandler, d->XMLEndHandler);
}

///////////////////////////////////////////////////////////////////////////////
void ScoreEngineBasicXML::XMLEndHandler(void *data, const char *el)
{
	// nothing to pop because we are the top
//	ScoreEngineBasicXML* d = (ScoreEngineBasicXML*)data;
}

///////////////////////////////////////////////////////////////////////////////
bool ScoreEngineBasicXML::GetInitialScore(PCSTR nick, PlayerType pt, uint& cookie,
								 DFLOAT& score, int& kills, int& deaths)
{
	if (!dirtyTime)
		dirtyTime = time(NULL);
	return(ScoreEngineBasicXMLSUPERCLASS::GetInitialScore(nick, pt, cookie, 
														  score, kills, deaths));
}

///////////////////////////////////////////////////////////////////////////////
bool ScoreEngineBasicXML::SetPlayerValues(PCSTR nick, PlayerType pt, uint cookie,
								 DFLOAT  score, int  kills, int  deaths)
{
	if (!dirtyTime)
		dirtyTime = time(NULL);
	return(ScoreEngineBasicXMLSUPERCLASS::SetPlayerValues(nick, pt, cookie,
														  score, kills, deaths));
}

///////////////////////////////////////////////////////////////////////////////
void ScoreEngineBasicXML::ReceivePlayerEvent(PCSTR name, PlayerType pt, ScorePlayerEvent spe,
									PCSTR real, PCSTR host, PCSTR addr, uint cookie)
{
	if (!dirtyTime)
		dirtyTime = time(NULL);
	ScoreEngineBasicXMLSUPERCLASS::ReceivePlayerEvent(name, pt, spe, 
													  real, host, addr, cookie);
}
///////////////////////////////////////////////////////////////////////////////
void ScoreEngineBasicXML::ReceiveScoreEvent(PCSTR killer, PlayerType ptr, DFLOAT wscore,
								   PCSTR killee, PlayerType pte, DFLOAT lscore, 
								   ScoreType st)
{
	if (!dirtyTime)
		dirtyTime = time(NULL);
	ScoreEngineBasicXMLSUPERCLASS::ReceiveScoreEvent(killer, ptr, wscore,
													 killee, pte, lscore,
													 st);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

SEBPlayerXML*	newPlayer;
///////////////////////////////////////////////////////////////////////////////
void SEBPlayerXML::XMLStartHandler(void *data, const char *el, const char **attr)
{
	ScoreEngineBasicXML* d = (ScoreEngineBasicXML*)data;
	TRACEXMLSTART(SEBPlayerXML);
#undef	xarObjPREFIX
#define	xarObjPREFIX	(*newPlayer)
	if (xarMatchToken(Player))
	{
		newPlayer = new SEBPlayerXML;
		d->playerList.Add(newPlayer);
	}
	//xarParseAttr(name, String);
	xarParseString(name);
	xarParseIntType(playerType, PlayerType);
	xarParseInt(cookie);
	xarParseInt(flags);
	xarParseFloat(score);
	xarParseInt(kills);
	xarParseInt(deaths);
	xarParseInt(sessions);
	xarParseInt(playTime);
	xarParseInt(lastVisit);
}

///////////////////////////////////////////////////////////////////////////////
void SEBPlayerXML::XMLEndHandler(void *data, const char *el)
{
	ScoreEngineBasicXML* d = (ScoreEngineBasicXML*)data;
	TRACEXMLEND(SEBPlayerXML);

	xarParseObjectEnd(PlayerData);
}


///////////////////////////////////////////////////////////////////////////////
void SEBPlayerXML::XMLOut(XMLArchive& xar) 
{
	xar.WriteElementStart(Player);
	xar.WriteStringAttr(name);
	xar.WriteIntAttr(playerType);
	xar.WriteIntAttr(cookie);
	xar.WriteIntAttr(flags);
	xar.WriteFloatAttr(score);
	xar.WriteIntAttr(kills);
	xar.WriteIntAttr(deaths);
	xar.WriteIntAttr(sessions);
	xar.WriteIntAttr(playTime);
	xar.WriteIntAttr(lastVisit);
	xar.WriteElementEnd(Player);
}

