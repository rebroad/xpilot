/* $Id: score.cpp,v 1.19 2004/05/22 14:49:24 dick Exp $
 *
 * XPilot, a multiplayer gravity war game.  Copyright (C) 1991-2001 by
 *
 *      Bjørn Stabell        <bjoern@xpilot.org>
 *      Ken Ronny Schouten   <ken@xpilot.org>
 *      Bert Gijsbers        <bert@xpilot.org>
 *      Dick Balaska         <dick@xpilot.org>
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
 *  $Log: score.cpp,v $
 *  Revision 1.19  2004/05/22 14:49:24  dick
 *  myEyes becomes a id instead of an ind
 *
 *  Revision 1.18  2004/05/07 04:27:41  dick
 *  _visibility becomes Visibility.  global updateScores becomes a member of World.
 *  Handle rank/rate data from the scoreserver and send it to the client.
 *
 *  Revision 1.17  2004/04/26 07:07:14  dick
 *  Define a list of pages that can be displayed on the ScoreTable in the client.
 *  Each page can be controlled by the server (and the server admin tool).
 *
 *  Revision 1.16  2004/02/18 01:22:14  dick
 *  Remove extraneous and obsolete #include "netserver.h"
 *
 *  Revision 1.15  2004/02/03 03:39:54  dick
 *  Add Windows precompiled header support.
 *
 *  Revision 1.14  2002/09/01 00:13:46  dick
 *  Remove obsolete windows #include "winServer.h"
 *
 *  Revision 1.13  2002/08/01 14:30:25  dick
 *  When receiving a player's score from the ScoreServer, check for a cookie.
 *
 *  Revision 1.12  2002/07/14 19:05:24  dick
 *  When talking to the ScoreServer about PCSTR nick, always mention the PlayerType too.
 *
 *  Revision 1.11  2002/07/08 18:13:29  dick
 *  Wrap debug with D()
 *
 *  Revision 1.10  2002/07/08 05:52:59  dick
 *  showtime() handles the space after itself
 *
 *  Revision 1.9  2002/06/30 06:44:47  dick
 *  Add some strings. s_br* for bracketed strings. s_brWall = "[Wall]";
 *
 *  Revision 1.8  2002/06/25 05:04:24  dick
 *  Score_players takes an additional parameter: the ScoreType (describe the kill)
 *
 *  Revision 1.7  2002/06/23 06:41:32  dick
 *  Send score changes to the ScoreServer
 *
 *  Revision 1.6  2002/06/04 22:21:18  dick
 *  Send score objects to anyone watching this player when he scores.
 *
 *  Revision 1.5  2002/06/01 06:06:48  dick
 *  Encapsulate (almost) everything.  Get rid of (almost) all refs to theWorld.
 *
 *  Revision 1.4  2002/05/18 20:55:39  dick
 *  Update to XPilot-4.5.1
 *
 *  Revision 1.3  2001/08/10 08:11:38  dick
 *  New class ConnectionPlayer subclasses Connection to provide player
 *  functions.  New class Frame subclasses ConnectionPlayer which provides
 *  Frame update functionality.
 *
 *  Revision 1.2  2001/07/07 12:00:43  dick
 *  Rename classes to C++ "Style".  old World becomes theWorld.
 *
 */
#include "StdAfx.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <limits.h>

#include "version.h"
#include "config.h"
#include "showtime.h"
#include "error.h"
#include "serverconst.h"
#include "global.h"
#include "proto.h"
#include "score.h"
#include "ConnectionPlayer.h"

char score_version[] = VERSION;

PCSTR	s_brAsteroid	= "[Asteroid]";
PCSTR	s_brBall		= "[Ball]";
PCSTR	s_brCannon		= "[Cannon]";
PCSTR	s_brExplosion	= "[Explosion]";
PCSTR	s_brSelfDestruct= "[SelfDestruct]";
PCSTR	s_brTarget		= "[Target]";
PCSTR	s_brTreasure	= "[Treasure]";
PCSTR	s_brWall		= "[Wall]";

///////////////////////////////////////////////////////////////////////////////
void SCORE(Player* pl, DFLOAT points, int x, int y, const char *msg)
{
	World*	w = pl->world;
	int		i;

	if (BIT(w->rules->mode, TEAM_PLAY)) {
		if (!w->options.teamShareScore->GetBool()) {
			pl->score += points;
		}
		TEAM_SCORE(w, pl->team, points);
	} else {
		if (pl->alliance != ALLIANCE_NOT_SET && w->options.teamShareScore->GetBool()) {
			Alliance_score(w, pl->alliance, points);
		} else {
			pl->score += points;
		}
	}
	if (pl->conn)
		pl->conn->SendScoreObject(points, x, y, msg);
	int id = pl->id;
	for (i=0; i<w->numPlayers; i++)
	{
		Player* p = w->players[i];
		if (p->myEyes == id && pl != p)
		{
			if (p->conn)
				p->conn->SendScoreObject(points, x, y, msg);
		}
	}
    w->updateScores = true;
}

///////////////////////////////////////////////////////////////////////////////
void TEAM_SCORE(World* w, int team, DFLOAT points)
{
	if (team == TEAM_NOT_SET)	/* could happen if teamCannons is off */
		return;

	w->teams[team].score += points;
	if (w->options.teamShareScore->GetBool()) 
	{
		int i;
		DFLOAT share = w->teams[team].score / w->teams[team].numMembers;
		for (i = 0; i < w->numPlayers; i++)
		{
			if (w->players[i]->team == team) 
			{
				w->players[i]->score = share;
			}
		}
	}

	w->updateScores = true;
}

///////////////////////////////////////////////////////////////////////////////
void Alliance_score(World* w, int id, DFLOAT points)
{
	int 		i;
	int 		member_count = Get_alliance_member_count(w, id);
	DFLOAT		share = points / member_count;

	for (i = 0; i < w->numPlayers; i++) {
		if (w->players[i]->alliance == id) {
			w->players[i]->score += share;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
DFLOAT Rate(DFLOAT winner, DFLOAT loser)
{
	DFLOAT t;

	t = ((RATE_SIZE / 2) * RATE_RANGE) / (ABS(loser - winner) + RATE_RANGE);
	if (loser > winner)
		t = RATE_SIZE - t;
	return (t);
}

///////////////////////////////////////////////////////////////////////////////
/*
 * Cause `winner' to get `winner_score' points added with message
 * `winner_msg', and similarly with the `loser' and equivalent
 * variables.
 *
 * In general the winner_score should be positive, and the loser_score
 * negative, but this need not be true.
 *
 * If the winner and loser players are on the same team, the scores are
 * made negative, since you shouldn't gain points by killing team members,
 * or being killed by a team member (it is both players faults).
 *
 * KK 28-4-98: Same for killing your own tank.
 * KK 7-11-01: And for killing a member of your alliance
 */
void Score_players(Player* winner, DFLOAT winner_score, char *winner_msg,
		   Player* loser, DFLOAT loser_score, char *loser_msg, ScoreType st)
{
	if (TEAM(winner->world, winner->Ind(), loser->Ind())
		|| (winner->alliance != ALLIANCE_NOT_SET
			&& winner->alliance == loser->alliance)
		|| (IS_TANK_PTR(loser)
			&& loser->world->getInd[loser->lock.pl_id] == winner->Ind())) 
	{
		if (winner_score > 0)
			winner_score = -winner_score;
		if (loser_score > 0)
			loser_score = -loser_score;
	}
    SCORE(winner, winner_score,
	  OBJ_X_IN_BLOCKS(loser),
	  OBJ_Y_IN_BLOCKS(loser),
	  winner_msg);
    SCORE(loser, loser_score,
	  OBJ_X_IN_BLOCKS(loser),
	  OBJ_Y_IN_BLOCKS(loser),
	  loser_msg);
	winner->world->ScoreServerScoreEvent(winner, winner_score,
										 loser,  loser_score, st);
}

///////////////////////////////////////////////////////////////////////////////
// ScoreServer has said to set the player's score to this.
void World::SetPlayerScore(PCSTR nick, PlayerType pt, DFLOAT score, 
						   int kills, int deaths, int cookie)
{
	D(Trace("SetPlayerScore: %s to %.2f k:%d d:%d\n", nick, score, kills, deaths);)
	Player*	p = FindPlayer(nick, pt);
	if (p)
	{
		p->score = score;
		p->kills = kills;
		p->deaths = deaths;
		updateScores = true;
		if (p->conn)
			p->conn->SetCookie(cookie);
	}
	else
	{
		xpprintf("%sSetPlayerScore can't find player %s\n",
			showtime(), nick);
		D(
			Trace("numPlayers: %d\n", numPlayers);
			int		i;
			for (i=0; i<numPlayers; i++)
			{
				Trace("[%d] -%s-\n", i, players[i]->name);
			}
		)
	}
}

///////////////////////////////////////////////////////////////////////////////
// ScoreServer has said to set the player's rank to this.
void World::SetPlayerRank(PCSTR nick, PlayerType pt,
						  int rank, DFLOAT rate)
{
	Player*	p = FindPlayer(nick, pt);
	if (p)
	{
		p->rank = rank;
		p->rate = rate;
		updateScores = true;
	}
}
