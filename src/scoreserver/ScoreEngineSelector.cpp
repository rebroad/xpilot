/* $Id: ScoreEngineSelector.cpp,v 1.11 2007/02/03 08:03:33 dick Exp $
 *
 * XPScoreServer - Who's on first?
 * ScoreEngineSelector - Create a ScoreEngine based on which one we want
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
 * $Log: ScoreEngineSelector.cpp,v $
 * Revision 1.11  2007/02/03 08:03:33  dick
 * MYSQLFLAG becomes _MYSQLFLAG
 *
 * Revision 1.10  2007/01/30 06:56:04  dick
 * Optionally build the sql engine
 *
 * Revision 1.9  2007/01/29 04:46:04  dick
 * Add the SQL engine
 *
 * Revision 1.8  2004/07/07 19:29:34  dick
 * Delete dead secfg
 *
 * Revision 1.7  2004/02/03 03:39:53  dick
 * Add Windows precompiled header support.
 *
 * Revision 1.6  2004/02/02 18:48:54  dick
 * Cleanup parsing the configuration.
 *
 * Revision 1.5  2004/01/31 02:32:01  dick
 * Some Unix needs #include <string.h>
 *
 * Revision 1.4  2004/01/30 16:02:49  dick
 * Configure the scoreserver via the web interface.
 * Break out the engine configuration to it's own object; we don't want
 * to have the baggage of a big unused score engine loaded in memory if we
 * really are only interested in it's name and description.
 *
 * Revision 1.3  2004/01/26 17:07:06  dick
 * Add StartScoreEngine(), so we can change which engine is running on the fly.
 *
 * Revision 1.2  2004/01/24 00:46:59  dick
 * Web configuration for the ScoreServer including which ScoreEngine the user has selected.
 *
 * Revision 1.1  2002/06/25 05:12:26  dick
 * Select and install the ScoreEngine as configured by the user.
 *
 */
#include "StdAfx.h"

#if defined(_WINDOWS) && !defined(_CYGWIN)
#include <windows.h>
#endif
#if defined(_UNIX) || defined(_CYGWIN)
#include <string.h>
#endif

#include "cstring.h"

#include "portability.h"
#include "const.h"
#include "config.h"
#include "ScoreServer.h"
#include "ScoreEngineSelector.h"
#include "engines/ScoreEngine.h"
#include "engines/ScoreEngineCfg.h"
#include "engines/ScoreEngineBasic.h"
#include "engines/ScoreEngineBasicCfg.h"
#include "engines/ScoreEngineBasicXML.h"
#include "engines/ScoreEngineBasicXMLCfg.h"

#ifdef	_MYSQLFLAG
#include "engines/sql/ScoreEngineSoloSQL.h"
#include "engines/sql/ScoreEngineSoloSQLCfg.h"
#endif

extern PCSTR	s_Basic;
extern PCSTR	s_BasicXML;
extern PCSTR	s_SoloSQL;

#define	xarOBJECT	ScoreEngineSelector
xarDECLARES;
xarDeclareToken(ScoreEngineSelector);
xarDefineToken(ScoreEngineCfg);
xarDefineToken(ScoreEngineBasicCfg);
xarDefineToken(ScoreEngineBasicXMLCfg);
xarDefineToken(ScoreEngineSoloSQLCfg);


///////////////////////////////////////////////////////////////////////////////
ScoreEngineSelector::ScoreEngineSelector()
{
	selectedSECfg = NULL;
}

///////////////////////////////////////////////////////////////////////////////
ScoreEngineSelector::~ScoreEngineSelector()
{
}

///////////////////////////////////////////////////////////////////////////////
void ScoreEngineSelector::Init()
{
	list.Add((Obj*)ScoreEngine::Install());
	list.Add((Obj*)ScoreEngineBasic::Install());
	list.Add((Obj*)ScoreEngineBasicXML::Install());
#ifdef	_MYSQLFLAG
	list.Add((Obj*)ScoreEngineSoloSQL::Install());
#endif
}

///////////////////////////////////////////////////////////////////////////////
ScoreEngine* ScoreEngineSelector::Select(PCSTR engineName)
{
	xpprintf("Try selecting engine %s\n", engineName);
	selectedSECfg = (ScoreEngineCfg*)list.Find(engineName);

	if (!strcmp(engineName, s_Basic))
		return(new ScoreEngineBasic(selectedSECfg));
	if (!strcmp(engineName, s_BasicXML))
		return(new ScoreEngineBasicXML(selectedSECfg));
#ifdef	_MYSQLFLAG
	if (!strcmp(engineName, s_SoloSQL))
		return(new ScoreEngineSoloSQL(selectedSECfg));
#endif
	else
		return(new ScoreEngine(selectedSECfg));
}

///////////////////////////////////////////////////////////////////////////////
void ScoreEngineSelector::XMLOut(XMLArchive& xar)
{
	ScoreEngineCfg* secfg = (ScoreEngineCfg*)list.GetHead();

	xar.WriteElementStart(ScoreEngineSelector);

	while (secfg)
	{
		secfg->XMLOutVirtual(xar);
		secfg = (ScoreEngineCfg*)secfg->GetNext();
	}

	xar.WriteElementEnd(ScoreEngineSelector);
}

///////////////////////////////////////////////////////////////////////////////
void ScoreEngineSelector::XMLStartHandler(void *data, const char *el, const char **attr)
{
	ScoreServer* d = (ScoreServer*)data;
	TRACEXMLSTART(ScoreEngineSelector);

#define	xarObjPREFIX	d->selector
	xarParseObject(ScoreEngineCfg);
	xarParseObject(ScoreEngineBasicCfg);
	xarParseObject(ScoreEngineBasicXMLCfg);
#ifdef	_MYSQLFLAG
	xarParseObject(ScoreEngineSoloSQLCfg);
#endif
}

///////////////////////////////////////////////////////////////////////////////
void ScoreEngineSelector::XMLEndHandler(void *data, const char *el)
{
	ScoreServer* d = (ScoreServer*)data;
	TRACEXMLEND(ScoreEngineSelector);

	xarParseObjectEnd(ScoreEngineSelector);
}

///////////////////////////////////////////////////////////////////////////////
ScoreEngineCfg*	ScoreEngineSelector::Find(PCSTR _name)
{
	return((ScoreEngineCfg*)list.Find(_name));
}

///////////////////////////////////////////////////////////////////////////////
ScoreEngineCfg*	ScoreEngineSelector::GetSelectedSECfg()
{
	return(selectedSECfg);
}
