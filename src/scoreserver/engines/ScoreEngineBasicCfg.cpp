/* $Id: ScoreEngineBasicCfg.cpp,v 1.3 2004/02/03 03:39:53 dick Exp $
 *
 * XPScoreServer - Who's on first?
 * ScoreEngineBasicCfg - The base class for different scoring techniques
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
 * $Log: ScoreEngineBasicCfg.cpp,v $
 * Revision 1.3  2004/02/03 03:39:53  dick
 * Add Windows precompiled header support.
 *
 * Revision 1.2  2004/01/31 02:32:02  dick
 * Some Unix needs #include <string.h>
 *
 * Revision 1.1  2004/01/30 16:02:49  dick
 * Configure the scoreserver via the web interface.
 * Break out the engine configuration to it's own object; we don't want
 * to have the baggage of a big unused score engine loaded in memory if we
 * really are only interested in it's name and description.
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
#include "ScoreEngineBasic.h"
#include "ScoreEngineBasicCfg.h"
#include "../ScoreServer.h"

#define	xarOBJECT	ScoreEngineBasicCfg
xarDECLARES;
xarDeclareToken(ScoreEngineBasicCfg);

///////////////////////////////////////////////////////////////////////////////
void ScoreEngineBasicCfg::XMLStartHandler(void *data, const char *el, const char **attr)
{
	ScoreServer* d = (ScoreServer*)data;
	TRACEXMLSTART(ScoreEngineBasicCfg);

#define	xarObjPREFIX	(*(ScoreEngineBasicCfg*)(d->selector.Find(ScoreEngineBasic::name)))
}

///////////////////////////////////////////////////////////////////////////////
void ScoreEngineBasicCfg::XMLEndHandler(void *data, const char *el)
{
	ScoreServer* d = (ScoreServer*)data;
	TRACEXMLEND(ScoreEngineBasicCfg);

	xarParseObjectEnd(ScoreEngineBasicCfg);
}

///////////////////////////////////////////////////////////////////////////////
void ScoreEngineBasicCfg::XMLOutVirtual(XMLArchive& xar)
{
	xar.WriteElementStart(ScoreEngineBasicCfg);
	xar.WriteElementEnd(ScoreEngineBasicCfg);
}


