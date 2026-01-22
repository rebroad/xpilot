/* $Id: ScoreEngine.cpp,v 1.12 2004/05/04 21:07:38 dick Exp $
 *
 * XPScoreServer - Who's on first?
 * ScoreEngine - The base class for different scoring techniques
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
 * $Log: ScoreEngine.cpp,v $
 * Revision 1.12  2004/05/04 21:07:38  dick
 * Need randommnt.h
 *
 * Revision 1.11  2004/02/13 05:12:55  dick
 * Enbracket() the name to make it html friendly in case it has a '<' in it.
 *
 * Revision 1.10  2004/02/03 03:39:53  dick
 * Add Windows precompiled header support.
 *
 * Revision 1.9  2004/01/30 16:02:49  dick
 * Configure the scoreserver via the web interface.
 * Break out the engine configuration to it's own object; we don't want
 * to have the baggage of a big unused score engine loaded in memory if we
 * really are only interested in it's name and description.
 *
 * Revision 1.8  2004/01/26 17:02:55  dick
 * Really generate a random cookie
 *
 * Revision 1.7  2004/01/24 00:46:59  dick
 * Web configuration for the ScoreServer including which ScoreEngine the user has selected.
 *
 * Revision 1.6  2003/12/22 09:40:40  dick
 * Whitespace
 *
 * Revision 1.5  2002/08/01 15:02:21  dick
 * Add support for cookies.
 *
 * Revision 1.4  2002/07/17 23:59:47  dick
 * SEPlayerList derives from ObjList so we can add some specific sorting.
 *
 * Revision 1.3  2002/07/14 19:09:05  dick
 * Whenever we're talking to the server about PCSTR nick, also mention the PlayerType.
 *
 * Revision 1.2  2002/06/30 06:14:48  dick
 * Add sorting.
 *
 * Revision 1.1  2002/06/25 05:27:19  dick
 * The ScoreEngine Base class.  This file interfaces the ScoreServer to the user's
 * engine of choice.  I think i might move this class into the scoreserver main
 * directory.
 *
 */
#include "StdAfx.h"

#if defined(_WINDOWS) && !defined(_CYGWIN)
#include <windows.h>
#endif

#include <string.h>

#include "commonproto.h"
#include "randommt.h"
#include "ScoreEngine.h"
#include "ScoreEngineCfg.h"
#include "../ScoreServer.h"
#include "../ScoreEngineSelector.h"

PCSTR	s_nick		= "nick";
PCSTR	s_lineno	= "lineno";

PCSTR ScoreEngine::name = "Bare";
PCSTR ScoreEngine::title  = "Bare";
PCSTR ScoreEngine::description =
"This is the default/root score engine.  It keeps no scores and returns 0s when "
"asked for information.  If you are using this Engine, you might as well not "
"run the ScoreServer at all.";


///////////////////////////////////////////////////////////////////////////////
ScoreEngine::ScoreEngine(ScoreEngineCfg* _cfg)
{
	cfg = _cfg;
}

///////////////////////////////////////////////////////////////////////////////
ScoreEngine::~ScoreEngine()
{
}

///////////////////////////////////////////////////////////////////////////////
// static
ScoreEngineCfg* ScoreEngine::Install()
{
	ScoreEngineCfg* def = new ScoreEngineCfg;
	def->name = name;
	def->title = title;
	def->description = description;

	return(def);
}


///////////////////////////////////////////////////////////////////////////////
void ScoreEngine::Startup()
{
}

///////////////////////////////////////////////////////////////////////////////
void ScoreEngine::Shutdown()
{
}

///////////////////////////////////////////////////////////////////////////////
void ScoreEngine::TimerTick()
{
}

///////////////////////////////////////////////////////////////////////////////
bool ScoreEngine::OnAdminPrintTop(HttpScore* http)
{
	return(false);
}

///////////////////////////////////////////////////////////////////////////////
bool ScoreEngine::OnAdminPrintTable(HttpScore* http)
{
	return(false);
}

///////////////////////////////////////////////////////////////////////////////
bool ScoreEngine::OnAdminSubmit(HttpScore* http)
{
	return(false);
}

///////////////////////////////////////////////////////////////////////////////
bool ScoreEngine::GetInitialScore(PCSTR nick, PlayerType pt, uint& cookie,
								  DFLOAT& score, int& kills, int& deaths)
{
	score = 0;
	kills = 0;
	deaths = 0;
	cookie = 0;
	return(true);
}

///////////////////////////////////////////////////////////////////////////////
bool ScoreEngine::SetPlayerValues(PCSTR nick, PlayerType pt, uint  cookie,
								  DFLOAT  score, int  kills, int  deaths)
{
	return(true);
}


///////////////////////////////////////////////////////////////////////////////
String ScoreEngine::GetPlayerValue(SEPlayer* pl, const ObjList& opts)
{
	String	s;
	LString*	type = (LString*)opts.Find(s_type);
	if (!type)
	{
		s = " *No type for player field lookup* ";
		return(s);
	}
	s = GetValue(pl, type->s, opts);
	return(s);
}

///////////////////////////////////////////////////////////////////////////////
String ScoreEngine::GetValue(SEPlayer* pl, PCSTR type, const ObjList& opts)
{
	String	s;
	if (!strcmp(type, s_nick))
	{
		s = pl->GetName();
		s.Format(opts);
		s.Enbracket();
	}
	else if (!strcmp(type, s_lineno))
	{
		int i = playerList.Index(pl);
		s = i+1;		// line 0 is line 1
		s.Format(opts);
	}
	else
		s.printf("*Unknown player attribute '%s'*", type);
	return(s);
}

///////////////////////////////////////////////////////////////////////////////
uint ScoreEngine::MakeNewCookie()
{
	return(randomMT());
	// return(0x98765432);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool SEPlayer::Sort(ObjSortable* other, int priSort, int secSort)
{
	return(strcmp(name, ((SEPlayer*)other)->name) >= 0);
}

///////////////////////////////////////////////////////////////////////////////
String SEPlayer::GetName()
{
	String	s;
	s = name;
	if (playerType == PlayerRobot)
		s += " [R]";
	return(s);
}

///////////////////////////////////////////////////////////////////////////////
ObjSortable* SEPlayerList::FindSortable(PCSTR f, PlayerType pt) const
{
	ObjSortable*	o = (ObjSortable*)GetHead();
	while(o)
	{
		if (!strcmp(f, o->o->name)
			&& (((SEPlayer*)o->o)->playerType == pt
				|| ((SEPlayer*)o->o)->playerType == PlayerAny))
			return(o);
		o = (ObjSortable*)o->GetNext();
	}
	return(0);
}
