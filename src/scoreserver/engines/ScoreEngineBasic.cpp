/* $Id: ScoreEngineBasic.cpp,v 1.15 2004/05/07 04:38:26 dick Exp $
 *
 * XPScoreServer - Who's on first?
 * ScoreEngineBasic - A simple score server Keeps Score/Kills/Deaths/Playtime
 *                    in memory.
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
 * $Log: ScoreEngineBasic.cpp,v $
 * Revision 1.15  2004/05/07 04:38:26  dick
 * Send rank data to the server.
 *
 * Revision 1.14  2004/02/03 03:39:53  dick
 * Add Windows precompiled header support.
 *
 * Revision 1.13  2004/01/30 16:02:49  dick
 * Configure the scoreserver via the web interface.
 * Break out the engine configuration to it's own object; we don't want
 * to have the baggage of a big unused score engine loaded in memory if we
 * really are only interested in it's name and description.
 *
 * Revision 1.12  2004/01/24 00:46:59  dick
 * Web configuration for the ScoreServer including which ScoreEngine the user has selected.
 *
 * Revision 1.11  2004/01/05 05:17:04  dick
 * Adjust the ranking formula, lower kills weight
 *
 * Revision 1.10  2002/09/06 19:41:07  dick
 * Add lastVisit as a player attribute
 *
 * Revision 1.9  2002/08/01 15:02:21  dick
 * Add support for cookies.
 *
 * Revision 1.8  2002/07/30 21:57:42  jlmiller
 *
 * must include <string.h>
 *
 * Revision 1.7  2002/07/18 00:00:26  dick
 * Check if a player's rank has changed and send a message to the server if it has.
 *
 * Revision 1.6  2002/07/14 19:09:05  dick
 * Whenever we're talking to the server about PCSTR nick, also mention the PlayerType.
 *
 * Revision 1.5  2002/07/09 21:57:02  dick
 * Send the player type (human,robot,tank) with the PlayerEvent
 *
 * Revision 1.4  2002/07/08 18:16:18  dick
 * Make public as a field the rating that was calculated.
 *
 * Revision 1.3  2002/06/30 06:14:48  dick
 * Add sorting.
 *
 * Revision 1.2  2002/06/26 04:35:45  dick
 * PlayerScoreEvent becomes ScorePlayerEvent
 *
 * Revision 1.1  2002/06/25 05:28:32  dick
 * A Basic Score Engine that tracks total score, kills, deaths and playing time.
 *
 */
#include "StdAfx.h"

#include <string.h>
#include "xpprintf.h"
#include "const.h"
#include "PacketCtl.h"
#include "ScoreEngineBasic.h"
#include "ScoreEngineBasicCfg.h"
#include "../ScoreServer.h"
#include "../HttpScore.h"

PCSTR ScoreEngineBasic::name = "Basic";
PCSTR ScoreEngineBasic::title  = "Basic";
PCSTR ScoreEngineBasic::description =
"This is a basic score server that keeps individual player statistics only "
"as long as it it running.  There is no persistence (no database, no file save)";

PCSTR	s_Basic		= ScoreEngineBasic::name;

PCSTR	s_messagetag = "[ScoreServer]";
PCSTR	s_score		= "score";
PCSTR	s_rank		= "rank";
PCSTR	s_rating	= "rating";
PCSTR	s_kills		= "kills";
PCSTR	s_deaths	= "deaths";
PCSTR	s_kdratio	= "kdratio";
PCSTR	s_sessions	= "sessions";
PCSTR	s_playtime	= "playtime";
PCSTR	s_lastvisit	= "lastvisit";

PCSTR	s_resetScores = "resetScores";

PCSTR	ScorePlayerEventText[] = {
	"Join",
	"Quit",
	"Pause",
	"Unpause",
	"ScorePlayerEventMAX"
};

PCSTR	PlayerTypeText[] = {
	"PlayerHuman",
	"PlayerTank",
	"PlayerRobot",
	"PlayerTypeMAX"
};


///////////////////////////////////////////////////////////////////////////////
ScoreEngineBasic::ScoreEngineBasic(ScoreEngineCfg* _cfg)
: ScoreEngine(_cfg)
{
	cfg = (ScoreEngineBasicCfg*)_cfg;
}

///////////////////////////////////////////////////////////////////////////////
ScoreEngineBasic::~ScoreEngineBasic()
{
}

///////////////////////////////////////////////////////////////////////////////
// static
ScoreEngineCfg* ScoreEngineBasic::Install()
{
	ScoreEngineCfg* def = new ScoreEngineBasicCfg;
	def->name = name;
	def->title = title;
	def->description = description;

	return(def);
}
///////////////////////////////////////////////////////////////////////////////
void ScoreEngineBasic::Startup()
{
	playerList.SortObjInto(rankList, PlayerSortByRank);
}

///////////////////////////////////////////////////////////////////////////////
void ScoreEngineBasic::Shutdown()
{
}

///////////////////////////////////////////////////////////////////////////////
void ScoreEngineBasic::TimerTick()
{
}

///////////////////////////////////////////////////////////////////////////////
bool ScoreEngineBasic::OnAdminPrintTop(HttpScore* http)
{
	return(false);
}

///////////////////////////////////////////////////////////////////////////////
bool ScoreEngineBasic::OnAdminPrintTable(HttpScore* http)
{
	ScoreEngineBasicSUPERCLASS::OnAdminPrintTable(http);
	http->printf("<TR><TD>Reset Scores</TD>"
				 "<TD><INPUT TYPE=submit NAME=%s value=\"Reset Scores\"></TD>\n",
			s_resetScores);
	http->printf("<TD>Click this button to reset all of the scores now.</TD></TR>\n");
	return(false);
}

///////////////////////////////////////////////////////////////////////////////
bool ScoreEngineBasic::OnAdminSubmit(HttpScore* http)
{
	return(false);
}

///////////////////////////////////////////////////////////////////////////////
bool ScoreEngineBasic::GetInitialScore(PCSTR nick, PlayerType pt, uint& cookie,
									   DFLOAT& score, int& kills, int& deaths)
{
	String	s;
	SEBPlayer*	p = FindPlayer(nick, pt);
	if (!p)
	{
		xpprintf(LOGERR, "GetInitialScore error: player \"%s\" doesn't exist\n", nick);
		return(false);
	}
	if (p->cookie == cookie && cookie > COOKIESPECIAL)
		p->flags = (PlayerFlags)(p->flags | Authenticated);
	if (p->cookie != cookie)
	{
		if (cookie == COOKIENONE)	// he hasn't picked up his cookie yet.
		{
			cookie = p->cookie;
		}
		else if (p->flags & Authenticated)
		{
			// cookie has been authenticated at least once.
			// Don't allow non-authenticated players
			score = 0;
			kills = 0;
			deaths = 0;
			p->sessions--;
			p->startTime = 0;
			p->isPlaying = false;

			s.printf("Your cookie does not match the one assigned %s", s_messagetag);
			theApp.SendPlayerMessage(s, nick, pt);
			s.printf("You are not authorized to play as \"%s\" %s", nick, s_messagetag);
			theApp.SendPlayerMessage(s, nick, pt);
			s.printf("You may continue to play, but %s's score is preserved %s", nick, s_messagetag);
			theApp.SendPlayerMessage(s, nick, pt);
			return(false);
		}
		else if (cookie == COOKIENOSUP)
		{
			// version less than 5.0 doesn't support cookies
		}
		else
		{
			score = 0;
			kills = 0;
			deaths = 0;
			p->sessions--;
			p->startTime = 0;
			p->isPlaying = false;

			//if (p->cookie > COOKIENONE)
			{
				s.printf("Your cookie does not match the one assigned to you %s", s_messagetag);
				theApp.SendPlayerMessage(s, nick, pt);
				s.printf("You are not authorized to play as \"%s\" %s", nick, s_messagetag);
				theApp.SendPlayerMessage(s, nick, pt);
				s.printf("You may continue to play, but %s's score is preserved %s", nick, s_messagetag);
				theApp.SendPlayerMessage(s, nick, pt);
				return(false);
			}
			return(false);
		}
	}
	score = p->score;
	kills = p->kills;
	deaths = p->deaths;
	if (pt != PlayerRobot)
	{
		String	s;
		ObjSortable*	obj = rankList.FindSortable(nick, pt);
		int rank = rankList.Index(obj) + 1;

		s.printf("Player %s rank %d (kills %d, deaths %d has entered %s",
			nick, rank, kills, deaths, s_messagetag);
		theApp.SendPlayerMessage(s);
	}
	return(true);
}

///////////////////////////////////////////////////////////////////////////////
bool ScoreEngineBasic::SetPlayerValues(PCSTR nick, PlayerType pt, uint cookie,
									   DFLOAT  score, int  kills, int  deaths)
{
	SEBPlayer*	p = FindPlayer(nick, pt);
	if (!p)
	{
		xpprintf(LOGERR, "%sEngine: Can't find player '%s'\n", showtime(), nick);
		return(false);
	}
	p->score = score;
	p->kills = kills;
	p->deaths = deaths;
	return(true);
}

///////////////////////////////////////////////////////////////////////////////
SEBPlayer* ScoreEngineBasic::FindPlayer(PCSTR nick, PlayerType pt)
{
	SEBPlayer*	p = (SEBPlayer*)playerList.GetHead();
	while (p)
	{
		if (!strcmp(p->name, nick) && p->playerType == pt)
			return(p);
		p = (SEBPlayer*)p->GetNext();

	}
	return(NULL);
}

///////////////////////////////////////////////////////////////////////////////
void ScoreEngineBasic::SendChangedRankMessage(PCSTR nick, PlayerType pt)
{
	if (pt == PlayerRobot)
		return;
	// Did his rank move?
	ObjSortable*	obj = rankList.FindSortable(nick, pt);
	int rankold = rankList.Index(obj) + 1;
	rankList.Sort(PlayerSortByRank);
	ObjSortable* o1 = rankList.FindSortable(nick, pt);
	int ranknew = rankList.Index(o1) + 1;
	if (rankold != ranknew)
	{
		String	s;
		s.printf("%s moves %s to rank %d %s", nick,
			ranknew < rankold ? "up" : "down",
			ranknew, s_messagetag);
		theApp.SendPlayerMessage(s);
	}
}

///////////////////////////////////////////////////////////////////////////////
void ScoreEngineBasic::ReceiveScoreEvent(PCSTR killer, PlayerType ptr, DFLOAT wscore,
										 PCSTR killee, PlayerType pte, DFLOAT lscore,
										 ScoreType st)
{
	SEBPlayer* kr = FindPlayer(killer, ptr);
	SEBPlayer* ke = FindPlayer(killee, pte);
	if (kr && kr->isPlaying)
	{
		if (st != ScoreMine)		// mines are the only nonkilling score event?
			kr->kills++;
		kr->score += wscore;
		kr->lastVisit = time(NULL);

		SendChangedRankMessage(killer, ptr);
		SendRank(kr);
		xpprintf(LOGLOTS, "EngineBasic: + %s = %.2f, k:%d\n", killer, wscore, kr->kills);
	}
	if (ke && ke->isPlaying)
	{
		if (st != ScoreMine)		// mines are the only nonkilling score event?
			ke->deaths++;
		ke->score += lscore;
		ke->lastVisit = time(NULL);

		SendChangedRankMessage(killee, pte);
		SendRank(ke);
		xpprintf(LOGLOTS, "EngineBasic: - %s = %.2f, d:%d\n", killee, lscore, ke->deaths);
	}
}

///////////////////////////////////////////////////////////////////////////////
// NOTE: real, host, addr and cookie are only valid for a Join event.
void ScoreEngineBasic::ReceivePlayerEvent(PCSTR name, PlayerType pt, ScorePlayerEvent spe,
										  PCSTR real, PCSTR host, PCSTR addr, uint cookie)
{
	SEBPlayer* pl = FindPlayer(name, pt);
	if (!pl && spe == Join)
	{
		pl = new SEBPlayer;
		pl->name = name;
		pl->playerType = pt;
		if (cookie < COOKIESPECIAL)
			pl->cookie = MakeNewCookie();
		else
			pl->cookie = cookie;
		playerList.Add(pl);
		xpprintf(LOGMIN, "%sEngine: PlayerEvent Join: Creating new player '%s'\n",
			showtime(), name);
		ObjSortable* os = new ObjSortable;
		os->o = pl;
		rankList.Add(os);
		rankList.Sort(PlayerSortByRank);
	}
	if (!pl)
	{
		xpprintf(LOGERR, "%sEngine: PlayerEvent %s Can't find player.%s type %s\n",
			showtime(),
			spe < ScorePlayerEventMAX ? ScorePlayerEventText[spe]
									  : ScorePlayerEventText[ScorePlayerEventMAX],
			name,
			pt < PlayerTypeMAX ? PlayerTypeText[pt]
							   : PlayerTypeText[PlayerTypeMAX]);
		return;
	}
	time_t now = time(NULL);
	pl->lastVisit = now;
	switch (spe)
	{
	case Join:
		pl->sessions++;
		pl->startTime = now;
		pl->isPlaying = true;
		xpprintf(LOGMED, "%sEngine: Player %s is joining for the %d time\n",
			showtime(), (PCSTR)pl->GetName(), pl->sessions);
		SendRank(pl);
		break;
	case Quit:
		if (!pl->isPlaying)
		{
			xpprintf(LOGERR, "%sEngine: Player %s quit and we didn't know he was playing\n",
				showtime(), (PCSTR)pl->GetName());
		}
		else
		{
			pl->playTime += (now - pl->startTime);
			pl->isPlaying = false;
			xpprintf(LOGMED, "%sEngine: Player %s left after playing for %d seconds\n",
				showtime(), (PCSTR)pl->GetName(), (now - pl->startTime));
		}
		break;
	case Pause:
	case Unpause:
		xpprintf(LOGERR, "%sNeed to handle pause/unpause\n", showtime());
		break;
	}
}

///////////////////////////////////////////////////////////////////////////////
void ScoreEngineBasic::ReceiveGetRanks(bool onOff)
{
	xpprintf("%sSet SendRanks: %s\n", showtime(), onOff ? "on" : "off");
	if (onOff)
		SendAllPlayingRanks();
}

///////////////////////////////////////////////////////////////////////////////
void ScoreEngineBasic::SendAllPlayingRanks()
{
	SEBPlayer*	p = (SEBPlayer*)playerList.GetHead();
	while (p)
	{
		if (p->isPlaying)
			SendRank(p);
		p = (SEBPlayer*)p->GetNext();

	}
}

///////////////////////////////////////////////////////////////////////////////
void ScoreEngineBasic::SendRank(SEBPlayer* pl)
{
	ObjSortable*	obj = rankList.FindSortable(pl->name, pl->playerType);
	int rank = rankList.Index(obj) + 1;

	theApp.SendPlayerRank(pl->name, pl->playerType, rank, pl->GetRating());
}

///////////////////////////////////////////////////////////////////////////////
String ScoreEngineBasic::GetValue(SEPlayer* _pl, PCSTR type, const ObjList& opts)
{
	SEBPlayer* pl = (SEBPlayer*)_pl;
	String	s;
	if (!strcmp(type, s_score))
	{
		s.printf("%f", pl->score);
		s.Format(opts);
	}
	else if (!strcmp(type, s_rank))
	{
		int i = playerList.Index(pl);
		s = i+1;		// line 0 is line 1
		s.Format(opts);
	}
	else if (!strcmp(type, s_kills))
	{
		s = pl->kills;
		s.Format(opts);
	}
	else if (!strcmp(type, s_deaths))
	{
		s = pl->deaths;
		s.Format(opts);
	}
	else if (!strcmp(type, s_sessions))
	{
		s = pl->sessions;
		s.Format(opts);
	}
	else if (!strcmp(type, s_rating))
	{
		DFLOAT	rating;
		rating = pl->GetRating();
		s.printf("%f", rating);
		s.Format(opts);
	}
	else if (!strcmp(type, s_playtime))
	{
		int playtime = pl->playTime;
		if (pl->isPlaying)
		{
			time_t now = time(NULL);
			playtime += (now - pl->startTime);
		}

		int	secs, mins, hrs, days;
		secs = playtime % 60;
		mins = playtime / 60;
		mins = mins % 60;
		hrs = playtime / 3600;
		days = hrs / 24;
		hrs  = hrs % 24;
		s.printf("%d:%02d:%02d.%02d", days, hrs, mins, secs);

		s.Format(opts);
	}
	else if (!strcmp(type, s_lastvisit))
	{
		if (pl->lastVisit)
		{
			char	buf[32];
			struct tm* gmt;
			gmt = localtime(&pl->lastVisit);
			strftime(buf, 64, "%Y-%m-%d %H:%M:%S", gmt);
			s.printf(buf);
		}
		else
			s.printf("--");
		s.Format(opts);
	}
	else if (!strcmp(type, s_kdratio))
	{
		double ratio;
		if (!pl->deaths)
			ratio = pl->kills*2;
		else
			ratio = (double)pl->kills / (double)pl->deaths;
		s.printf("%f", ratio);;
		s.Format(opts);
	}

	else
		s = ScoreEngineBasicSUPERCLASS::GetValue(pl, type, opts);
	return(s);
}

///////////////////////////////////////////////////////////////////////////////
double	SEBPlayer::GetRating()
{
	double myRating;
	if (!deaths)
		myRating = kills*2;
	else
		myRating = (double)kills / (double)deaths;
	myRating = myRating + (score / 400) + ((double)kills/300.0);
	return(myRating);
}

///////////////////////////////////////////////////////////////////////////////
bool SEBPlayer::Sort(ObjSortable* _other, int priSort, int secSort)
{
	SEBPlayer*	other = (SEBPlayer*)_other->o;
	double	myRank;
	double	otherRank;

	switch (priSort)
	{
	case PlayerSortByRank:
		myRank = GetRating();
		otherRank = other->GetRating();
		return(myRank < otherRank);
	case PlayerSortByKDRatio:
		if (!deaths)
			myRank = kills*2;
		else
			myRank = (double)kills / (double)deaths;
		if (!other->deaths)
			otherRank = other->kills*2;
		else
			otherRank = (double)other->kills / (double)other->deaths;
		return(myRank < otherRank);
	case PlayerSortByScore:
		return(score < other->score);
	case PlayerSortByKills:
		return(kills < other->kills);
	case PlayerSortByDeaths:
		return(deaths < other->deaths);
	case PlayerSortBySessions:
		return(sessions < other->sessions);
	case PlayerSortByPlaytime:
		return(playTime < other->playTime);
	case PlayerSortByLastVisit:
		return(lastVisit < other->lastVisit);
	}
	return(SEBPlayerSUPERCLASS::Sort(other, priSort, secSort));
}


