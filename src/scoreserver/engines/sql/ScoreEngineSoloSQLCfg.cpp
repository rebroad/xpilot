/* $Id: ScoreEngineSoloSQLCfg.cpp,v 1.1 2007/01/29 04:52:19 dick Exp $
 *
 * XPScoreServer - Who's on first?
 * ScoreEngineSoloSQLCfg - Configure the SQL scoring engine.
 *
 *      Copyright (C) 2007 by
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
 * $Log: ScoreEngineSoloSQLCfg.cpp,v $
 * Revision 1.1  2007/01/29 04:52:19  dick
 * The first SQL Engine for the ScoreServer
 *
 */
#include "StdAfx.h"

#if defined(_WINDOWS) && !defined(_CYGWIN)
#include <windows.h>
#endif
#if defined(_UNIX) || defined(_CYGWIN)
#include <string.h>
#endif

#include "XMLArchive.h"
#include "ScoreEngineSoloSQLCfg.h"
#include "ScoreEngineSoloSQL.h"
#include "../../ScoreServer.h"


#define	xarOBJECT	ScoreEngineSoloSQLCfg
xarDECLARES;
xarDeclareToken(ScoreEngineSoloSQLCfg);
//xarDeclareToken(flushDelay);

///////////////////////////////////////////////////////////////////////////////
ScoreEngineSoloSQLCfg::ScoreEngineSoloSQLCfg()
{
//	flushDelay = (60*10);		// every 10 minutes
}

///////////////////////////////////////////////////////////////////////////////
void ScoreEngineSoloSQLCfg::XMLStartHandler(void *data, const char *el, const char **attr)
{
	ScoreServer* d = (ScoreServer*)data;
	TRACEXMLSTART(ScoreEngineDefBasicXML);

#define	xarObjPREFIX	(*(ScoreEngineSoloSQLCfg*)(d->selector.Find(ScoreEngineSoloSQL::name)))
	//xarParseInt(flushDelay);
}

///////////////////////////////////////////////////////////////////////////////
void ScoreEngineSoloSQLCfg::XMLEndHandler(void *data, const char *el)
{
	ScoreServer* d = (ScoreServer*)data;
	TRACEXMLEND(ScoreEngineSoloSQLCfg);

	xarParseObjectEnd(ScoreEngineSoloSQLCfg);
}

///////////////////////////////////////////////////////////////////////////////
void ScoreEngineSoloSQLCfg::XMLOutVirtual(XMLArchive& xar)
{
	xar.WriteElementStart(ScoreEngineSoloSQLCfg);
	//xar.WriteIntAttr(flushDelay);
	xar.WriteElementEnd(ScoreEngineSoloSQLCfg);
}

