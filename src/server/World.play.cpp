/* $Id: World.play.cpp,v 1.4 2004/04/29 16:08:27 dick Exp $
 *
 * XPilot, a multiplayer gravity war game.  Copyright (C) 1991-2001 by
 *
 *      Bj�rn Stabell        <bjoern@xpilot.org>
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
 *  $Log: World.play.cpp,v $
 *  Revision 1.4  2004/04/29 16:08:27  dick
 *  compile with SOUND enabled.
 *
 *  Revision 1.3  2004/02/20 06:27:47  dick
 *  SendPlayMessage becomes BroadcastPlayMessage() because it sends a play message
 *  to all connections.
 *
 *  Revision 1.2  2004/02/03 03:39:54  dick
 *  Add Windows precompiled header support.
 *
 *  Revision 1.1  2002/09/12 19:46:21  dick
 *  play.cpp becomes World.play.cpp; World.server.cpp becomes World.main.cpp
 *
 *  Revision 1.13  2002/09/11 16:42:04  dick
 *  Here endeth the server encapsulation task.
 *  theWorld is now an array of World(s) and get loaded dynamically.
 *
 *  Revision 1.12  2002/09/01 00:13:46  dick
 *  Remove obsolete windows #include "winServer.h"
 *
 *  Revision 1.11  2002/07/17 23:51:33  dick
 *  FindPlayer() will match any PlayerType to PlayerAny.
 *
 *  Revision 1.10  2002/07/14 19:05:24  dick
 *  When talking to the ScoreServer about PCSTR nick, always mention the PlayerType too.
 *
 *  Revision 1.9  2002/06/30 06:43:24  dick
 *  SendMessage is a bad word.  It's a Windows function that is a macro and gets
 *  translated to SendMessageA or SendMessageW.  Although it works, renaming ours
 *  to SendPlayMessage makes browsing the source code easier.
 *
 *  Revision 1.8  2002/06/23 06:41:05  dick
 *  FindPlayer(PCSTR nick)
 *
 *  Revision 1.7  2002/06/01 06:06:48  dick
 *  Encapsulate (almost) everything.  Get rid of (almost) all refs to theWorld.
 *
 *  Revision 1.6  2002/05/18 20:55:39  dick
 *  Update to XPilot-4.5.1
 *
 *  Revision 1.5  2001/08/14 21:37:14  dick
 *  Array Obj[] becomes objs[].  We now have a class called Obj.
 *
 *  Revision 1.4  2001/07/24 09:04:00  dick
 *  Remove all global options.  Use World.options instead.
 *
 *  Revision 1.3  2001/07/23 21:08:37  dick
 *  Encapsulate ShipMass, ShotsMax, ShotsLife, ShotsMass, FPS.
 *
 *  Revision 1.2  2001/07/07 12:00:42  dick
 *  Rename classes to C++ "Style".  old World becomes theWorld.
 *
 */
#include "StdAfx.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>
#include <math.h>

#include "version.h"
#include "config.h"
#include "serverconst.h"
#include "global.h"
#include "proto.h"
#include "saudio.h"
#include "score.h"
#include "objpos.h"
#include "error.h"
#include "ConnectionPlayer.h"

char play_version[] = VERSION;


int World::PunishTeam(Player* pl, int t_destroyed, int t_target)
{
	String				msg;
	Treasure*	td = &treasures[t_destroyed];
	Treasure*	tt = &treasures[t_target];
	int 				i;
	DFLOAT				win_score = 0.0;
		DFLOAT			lose_score = 0.0;
	int 				win_team_members = 0, lose_team_members = 0;
	int 				somebody_flag = 0;
	DFLOAT				sc;
		DFLOAT			por;

	CheckTeamMembers(td->team);
	if (td->team == pl->team)
		return 0;

	if (BIT(rules->mode, TEAM_PLAY)) {
		for (i = 0; i < numPlayers; i++) {
			if (IS_TANK_IND(this, i)
				|| (BIT(players[i]->status, PAUSE)
					&& players[i]->count <= 0)
				|| (BIT(players[i]->status, GAME_OVER)
					&& players[i]->mychar == 'W'
					&& players[i]->score == 0)) {
				continue;
			}
			if (players[i]->team == td->team) {
				lose_score += players[i]->score;
				lose_team_members++;
				if (BIT(players[i]->status, GAME_OVER) == 0) {
					somebody_flag = 1;
				}
			}
			else if (players[i]->team == tt->team) {
				win_score += players[i]->score;
				win_team_members++;
			}
		}
	}

	SoundPlayAll(this, DESTROY_BALL_SOUND);
	msg.printf(" < %s's (%d) team has destroyed team %d treasure >",
			pl->name, pl->team, td->team);
	BroadcastPlayMessage(msg);

		if (!somebody_flag) {
				sc = Rate(pl->score, CANNON_SCORE)/2;
				SCORE(pl, sc, tt->pos.x, tt->pos.y, "Treasure:");
				ScoreServerScoreEvent(pl, sc, s_brTreasure, PlayerNone, 0, ScoreTreasure);
				return 0;
		}

	td->destroyed++;
	teams[td->team].treasuresLeft--;
	teams[tt->team].treasuresDestroyed++;


	sc	= 3 * Rate(win_score, lose_score);
	por = (sc * lose_team_members) / (2 * win_team_members + 1);

	for (i = 0; i < numPlayers; i++) {
		if (IS_TANK_IND(this, i)
			|| (BIT(players[i]->status, PAUSE)
				&& players[i]->count <= 0)
			|| (BIT(players[i]->status, GAME_OVER)
				&& players[i]->mychar == 'W'
				&& players[i]->score == 0)) {
			continue;
		}
		if (players[i]->team == td->team) {
			SCORE(players[i], -sc, tt->pos.x, tt->pos.y,
				  "Treasure: ");
			if (options.treasureKillTeam->GetBool())
				SET_BIT(players[i]->status, KILLED);
		}
		else if (players[i]->team == tt->team &&
				 (players[i]->team != TEAM_NOT_SET || players[i] == pl)) {
			SCORE(players[i], (players[i] == pl ? 3*por : 2*por), tt->pos.x, tt->pos.y,
				  "Treasure: ");
		}
	}

	if (options.treasureKillTeam->GetBool()) {
				pl->kills++;
	}

	updateScores = true;

	return 1;
}


/****************************
 * Functions for explosions.
 */

/* Create debris particles */
void World::MakeDebris(
	/* pos.x, pos.y   */ DFLOAT  x, 		 DFLOAT y,
	/* vel.x, vel.y   */ DFLOAT  velx,		 DFLOAT vely,
	/* owner id 	  */ int	id,
	/* owner team	  */ unsigned short team,
	/* type 		  */ int	type,
	/* mass 		  */ DFLOAT  mass,
	/* status		  */ long	status,
	/* color		  */ int	color,
	/* radius		  */ int	radius,
	/* min,max debris */ int	min_debris, int    max_debris,
	/* min,max dir	  */ int	min_dir,	int    max_dir,
	/* min,max speed  */ DFLOAT  min_speed,  DFLOAT  max_speed,
	/* min,max life   */ int	min_life,	int    max_life
)
{
	Object				*debris;
	int 				i, num_debris, life;
	modifiers			mods;

	if (BIT(rules->mode, WRAP_PLAY)) {
		if (x < 0) x += width;
		else if (x >= width) x -= width;
		if (y < 0) y += height;
		else if (y >= height) y -= height;
	}
	if (x < 0 || x >= width || y < 0 || y >= height) {
		return;
	}
	if (max_life < min_life)
		max_life = min_life;
	if (options.shotLife->GetInt() >= GetFPS()) {
		if (min_life > options.shotLife->GetInt()) {
			min_life = options.shotLife->GetInt();
			max_life = options.shotLife->GetInt();
		} else if (max_life > options.shotLife->GetInt()) {
			max_life = options.shotLife->GetInt();
		}
	}
	if (min_speed * max_life > hypotenuse)
		min_speed = hypotenuse / max_life;
	if (max_speed * min_life > hypotenuse)
		max_speed = hypotenuse / min_life;
	if (max_speed < min_speed)
		max_speed = min_speed;

	CLEAR_MODS(mods);

	if (type == OBJ_SHOT) {
		SET_BIT(mods.warhead, CLUSTER);
		if (!options.shotsGravity->GetBool()) {
			CLR_BIT(status, GRAVITY);
		}
	}

	num_debris = min_debris + (int)(rfrac() * (max_debris - min_debris));
	if (num_debris > MAX_TOTAL_SHOTS - numObjs) {
		num_debris = MAX_TOTAL_SHOTS - numObjs;
	}
	for (i = 0; i < num_debris; i++) {
		DFLOAT			speed, dx, dy, diroff;
		int 			dir, dirplus;

		if ((debris = Object_allocate(this)) == NULL)
				break;

		debris->color = color;
		debris->id = id;
		debris->team = team;
		Object_position_init_pixels(debris, x, y);
		dir = MOD2(min_dir + (int)(rfrac() * (max_dir - min_dir)), RES);
		dirplus = MOD2(dir + 1, RES);
		diroff = rfrac();
		dx = tcos(dir) + (tcos(dirplus) - tcos(dir)) * diroff;
		dy = tsin(dir) + (tsin(dirplus) - tsin(dir)) * diroff;
		speed = min_speed + rfrac() * (max_speed - min_speed);
		debris->vel.x = velx + dx * speed;
		debris->vel.y = vely + dy * speed;
		debris->acc.x = 0;
		debris->acc.y = 0;
		if (options.shotHitFuelDrainUsesKineticEnergy->GetBool()
				&& type == OBJ_SHOT)
		{
				/* compensate so that m*v^2 is constant */
				DFLOAT sp_shotsp = speed / options.shotSpeed->GetDouble();
				debris->mass = mass / (sp_shotsp * sp_shotsp);
		} else {
				debris->mass = mass;
		}
		debris->type = type;
		life = (int)(min_life + rfrac() * (max_life - min_life) + 1);
		if (life * speed > hypotenuse) {
			life = (long)(hypotenuse / speed);
		}
		debris->life = life;
		debris->fuselife = life;
		debris->pl_range = radius;
		debris->pl_radius = radius;
		debris->status = status;
		debris->mods = mods;
		CellAddObject(debris);
	}
}

void World::ComputeGameStatus()
{
	int 				i;
	Player				*pl;
	char				msg[MSG_LEN];

		if (roundDelaySend > 0) {
				roundDelaySend--;
		}
		if (roundDelay > 0) {
				if (!--roundDelay) {
						sprintf(msg, "%s starts now.",
						(BIT(rules->mode, TIMING) ? "Race" : "Round"));
						BroadcastPlayMessage(msg);
						roundTime = options.maxRoundTime->GetInt()
								* roundDelay + GetFPS();
						/* make sure players get the full 60 seconds of allowed idle time */
						for (i = 0; i < numPlayers; i++) {
								players[i]->frame_last_busy = frame_loops;
						}
				}
		}

		if (roundTime > 0) {
				roundTime--;
		}

	if (BIT(rules->mode, TIMING)) {
		/*
		 * We need a completely separate scoring system for race mode.
		 * I'm not sure how race mode should interact with team mode,
		 * so for the moment race mode takes priority.
		 *
		 * Race mode and limited lives mode interact. With limited lives on,
		 * race ends after all players have completed the course, or have died.
		 * With limited lives mode off, the race ends when the first player
		 * completes the course - all remaining players are then killed to
		 * reset them.
		 *
		 * In limited lives mode, where the race can be run to completion,
		 * points are awarded not just to the winner but to everyone who
		 * completes the course (with more going to the winner). These
		 * points are awarded as the player crosses the line. At the end
		 * of the race, a bonus is awarded to the player with the fastest lap.
		 *
		 * In unlimited lives mode, just the winner and the holder of the
		 * fastest lap get points.
		 */

		Player			*alive = NULL;
		int 			num_alive_players = 0,
						num_active_players = 0,
						num_finished_players = 0,
						num_race_over_players = 0,
						num_waiting_players = 0,
						position = 1,
						total_pts;
		DFLOAT	pts;

		/* First count the players */
		for (i = 0; i < numPlayers; i++)  {
			pl = players[i];
			if (BIT(pl->status, PAUSE)
				|| IS_TANK_PTR(pl)) {
				continue;
			}
			if (!BIT(pl->status, GAME_OVER)) {
				num_alive_players++;
			}
			else if (pl->mychar == 'W') {
				num_waiting_players++;
				continue;
			}

			if (BIT(pl->status, RACE_OVER)) {
				num_race_over_players++;
				position++;
			}
			else if (BIT(pl->status, FINISH)) {
				num_finished_players++;
			}
			else if (!BIT(pl->status, GAME_OVER)) {
				alive = pl;
			}

			/*
			 * An active player is one who is:
			 *	 still in the race.
			 *	 reached the finish line just now.
			 *	 has finished the race in a previous frame.
			 *	 died too often.
			 */
			num_active_players++;
		}
		if (num_active_players == 0 && num_waiting_players == 0) {
			return;
		}

		/* Now if any players are unaccounted for */
		if (num_finished_players > 0) {
			/*
			 * Ok, update positions. Everyone who finished the race in the last
			 * frame gets the current position.
			 */

			/* Only play the sound for the first person to cross the finish */
			if (position == 1)
			{
				SoundPlayAll(this, PLAYER_WIN_SOUND);
			}

			total_pts = 0;
			for (i = 0; i < num_finished_players; i++) {
				total_pts += (10 + 2 * num_active_players) >> (position - 1 + i);
			}
			pts = total_pts / num_finished_players;

			for (i = 0; i < numPlayers; i++)  {
				pl = players[i];
				if (BIT(pl->status, PAUSE)
					|| (BIT(pl->status, GAME_OVER) && pl->mychar == 'W')
					|| IS_TANK_PTR(pl)) {
					continue;
				}
				if (BIT(pl->status, FINISH)) {
					CLR_BIT(pl->status, FINISH);
					SET_BIT(pl->status, RACE_OVER);
					if (pts > 0) {
						sprintf(msg,
								"%s finishes %sin position %d "
								"scoring %.2f point%s.",
								pl->name,
								(num_finished_players == 1) ? "" : "jointly ",
								position, pts,
								(pts == 1) ? "" : "s");
						BroadcastPlayMessage(msg);
						sprintf(msg, "[Position %d%s]", position,
								(num_finished_players == 1) ? "" : " (jointly)");
						SCORE(pl, pts,
							  OBJ_X_IN_BLOCKS(pl),
							  OBJ_Y_IN_BLOCKS(pl),
							  msg);
					}
					else {
						sprintf(msg,
								"%s finishes %sin position %d.",
								pl->name,
								(num_finished_players == 1) ? "" : "jointly ",
								position);
						BroadcastPlayMessage(msg);
					}
				}
			}
		}

		/*
		 * If the maximum allowed time for this race is over, end it.
		 */
		if (options.maxRoundTime->GetInt() > 0 && roundTime == 0) {
			BroadcastPlayMessage("Timer expired. Race ends now.");
			RaceGameOver();
			return;
		}

		/*
		 * In limited lives mode, wait for everyone to die, except
		 * for the last player.
		 */
		if (BIT(rules->mode, LIMITED_LIVES)) {
			if (num_alive_players > 1) {
				return;
			}
			if (num_alive_players == 1) {
				if (num_finished_players + num_race_over_players == 0) {
					return;
				}
				if (!alive || alive->round == 0) {
					return;
				}
			}
		}
		else if (num_finished_players == 0) {
			return;
		}

		RaceGameOver();

	} else if (BIT(rules->mode, TEAM_PLAY)) {

		/* Do we have a winning team ? */

		enum TeamState {
			TeamEmpty,
			TeamDead,
			TeamAlive
		}		team_state[MAX_TEAMS];
		int 	num_dead_teams = 0;
		int 	num_alive_teams = 0;
		int 	winning_team = -1;

		for (i = 0; i < MAX_TEAMS; i++) {
			team_state[i] = TeamEmpty;
		}

		for (i = 0; i < numPlayers; i++) {
			if (IS_TANK_IND(this, i)) {
				/* Ignore tanks. */
				continue;
			}
			else if (BIT(players[i]->status, PAUSE)) {
				/* Ignore paused players. */
				continue;
			}
#if 0
			/* not all teammode maps have treasures. */
			else if (teams[players[i]->team].numTreasures == 0) {
				/* Ignore players with no treasure troves */
				continue;
			}
#endif
			else if (BIT(players[i]->status, GAME_OVER)) {
				if (team_state[players[i]->team] == TeamEmpty) {
					/* Assume all teammembers are dead. */
					num_dead_teams++;
					team_state[players[i]->team] = TeamDead;
				}
			}
			/*
			 * If the player is not paused and he is not in the
			 * game over mode and his team owns treasures then he is
			 * considered alive.
			 * But he may not be playing though if the rest of the team
			 * was genocided very quickly after game reset, while this
			 * player was still being transported back to his homebase.
			 */
			else if (team_state[players[i]->team] != TeamAlive) {
				if (team_state[players[i]->team] == TeamDead) {
					/* Oops!  Not all teammembers are dead yet. */
					num_dead_teams--;
				}
				team_state[players[i]->team] = TeamAlive;
				++num_alive_teams;
				/* Remember a team which was alive. */
				winning_team = players[i]->team;
			}
		}

		if (num_alive_teams > 1) {
			char		*bp;
			int 		teams_with_treasure = 0;
			int 		team_win[MAX_TEAMS];
			DFLOAT		team_score[MAX_TEAMS];
			int 		winners;
			int 		max_destroyed = 0;
			int 		max_left = 0;
			DFLOAT		max_score = 0;
			Team		*team_ptr;

			/*
			 * Game is not over if more than one team which have treasures
			 * still have one remaining in play.  Note that it is possible
			 * for max_destroyed to be zero, in the case where a team
			 * destroys some treasures and then all quit, and the remaining
			 * teams did not destroy any.
			 */
			for (i = 0; i < MAX_TEAMS; i++) {
				team_score[i] = 0;
				if (team_state[i] != TeamAlive) {
					team_win[i] = 0;
					continue;
				}
				team_win[i] = 1;
				team_ptr = &(teams[i]);
				if (team_ptr->treasuresDestroyed > max_destroyed) {
					max_destroyed = team_ptr->treasuresDestroyed;
				}
				if ((team_ptr->treasuresLeft > 0) ||
					(team_ptr->numTreasures == team_ptr->numEmptyTreasures)) {
					teams_with_treasure++;
				}
			}

			/*
			 * Game is not over if more than one team has treasure.
			 */
			if ((teams_with_treasure > 1 || !max_destroyed)
				&& (roundTime != 0 || options.maxRoundTime->GetInt() <= 0)) {
				return;
			}

			if (options.maxRoundTime->GetInt() > 0 && roundTime == 0) {
				BroadcastPlayMessage("Timer expired. Round ends now.");
			}

			/*
			 * Find the winning team;
			 *	Team destroying most number of treasures;
			 *	If drawn; the one with most saved treasures,
			 *	If drawn; the team with the most points,
			 *	If drawn; an overall draw.
			 */
			for (winners = i = 0; i < MAX_TEAMS; i++) {
				if (!team_win[i])
					continue;
				if (teams[i].treasuresDestroyed == max_destroyed) {
					if (teams[i].treasuresLeft > max_left)
						max_left = teams[i].treasuresLeft;
					winning_team = i;
					winners++;
				} else {
					team_win[i] = 0;
				}
			}
			if (winners == 1) {
				sprintf(msg, " by destroying %d treasures", max_destroyed);
				TeamGameOver(winning_team, msg);
				return;
			}

			for (i = 0; i < numPlayers; i++) {
				if (BIT(players[i]->status, PAUSE)
					|| IS_TANK_IND(this, i)) {
					continue;
				}
				team_score[players[i]->team] += players[i]->score;
			}

			for (winners = i = 0; i < MAX_TEAMS; i++) {
				if (!team_win[i])
					continue;
				if (teams[i].treasuresLeft == max_left) {
					if (team_score[i] > max_score)
						max_score = team_score[i];
					winning_team = i;
					winners++;
				} else {
					team_win[i] = 0;
				}
			}
			if (winners == 1) {
				sprintf(msg, " by destroying %d treasures"
										 " and successfully defending %d",
						max_destroyed, max_left);
				TeamGameOver(winning_team, msg);
				return;
			}

			for (winners = i = 0; i < MAX_TEAMS; i++) {
				if (!team_win[i])
					continue;
				if (team_score[i] == max_score) {
					winning_team = i;
					winners++;
				} else {
					team_win[i] = 0;
				}
			}
			if (winners == 1) {
				sprintf(msg, " by destroying %d treasures, saving %d, and "
						"scoring %.2f points",
						max_destroyed, max_left, max_score);
				TeamGameOver(winning_team, msg);
				return;
			}

			/* Highly unlikely */

			sprintf(msg, " between teams ");
			bp = msg + strlen(msg);
			for (i = 0; i < MAX_TEAMS; i++) {
				if (!team_win[i])
					continue;
				*bp++ = "0123456789"[i]; *bp++ = ','; *bp++ = ' ';
			}
			bp -= 2;
			*bp = '\0';
			TeamGameOver(-1, msg);

		}
		else if (num_dead_teams > 0) {
			if (num_alive_teams == 1)
				TeamGameOver(winning_team, " by staying alive");
			else
				TeamGameOver(-1, " as everyone died");
		}
		else {
			/*
			 * num_alive_teams <= 1 && num_dead_teams == 0
			 *
			 * There is a possibility that the game has ended because players
			 * quit, the game over state is needed to reset treasures.	We
			 * must count how many treasures are missing, if there are any
			 * the playing team (if any) wins.
			 */
			int i, treasures_destroyed;

			for (treasures_destroyed = i = 0; i < MAX_TEAMS; i++)
				treasures_destroyed += (teams[i].numTreasures
										- teams[i].numEmptyTreasures
										- teams[i].treasuresLeft);
			if (treasures_destroyed)
				TeamGameOver(winning_team, " by staying in the game");
		}

	} else {

	/* Do we have a winner ? (No team play) */
		int num_alive_players = 0;
		int num_active_players = 0;
		int num_alive_robots = 0;
		int num_active_humans = 0;
		int winner = -1;

		for (i=0; i<numPlayers; i++)  {
			if (BIT(players[i]->status, PAUSE)
				|| IS_TANK_IND(this, i)) {
				continue;
			}
			if (!BIT(players[i]->status, GAME_OVER)) {
				num_alive_players++;
				if (IS_ROBOT_IND(this, i)) {
					num_alive_robots++;
				}
				winner = i; 	/* Tag player that's alive */
			}
			else if (IS_HUMAN_IND(this, i)) {
				num_active_humans++;
			}
			num_active_players++;
		}

		if (num_alive_players == 1 && num_active_players > 1) {
			IndividualGameOver(winner);
		}
		else if (num_alive_players == 0 && num_active_players >= 1) {
			IndividualGameOver(-1);
		}
		else if (num_alive_robots > 1
			&& num_alive_players == num_alive_robots
			&& num_active_humans > 0) {
			IndividualGameOver(-2);
		}
		else if (options.maxRoundTime->GetInt() > 0 && roundTime == 0) {
			BroadcastPlayMessage("Timer expired. Round ends now.");
			IndividualGameOver(-1);
		}
	}
}

void World::CheckTeamMembers(int team)
{
	Player				*pl;
	int 				members, i;

	if (! BIT(rules->mode, TEAM_PLAY))
		return;

	for (members = i = 0; i < numPlayers; i++) {
		pl = players[i];
		if (pl->team != TEAM_NOT_SET
			&& !IS_TANK_PTR(pl)
			&& pl->team == team)
			members++;
	}
	if (teams[team].numMembers != members) {
		error ("Server has reset team %d members from %d to %d",
			   team, teams[team].numMembers, members);
		for (i = 0; i < numPlayers; i++) {
			pl = players[i];
			if (pl->team != TEAM_NOT_SET
				&& !IS_TANK_PTR(pl)
				&& pl->team == team)
				error ("Team %d currently has player %d: \"%s\"",
					   team, i+1, pl->name);
		}
		teams[team].numMembers = members;
	}
}

void World::TeamGameOver(int winning_team, const char *reason)
{
	int 				i, j;
	DFLOAT				average_score;
	int 				num_best_players;
	int 				*best_players;
	DFLOAT				best_ratio;
	char				msg[MSG_LEN];

	if (!(best_players = (int *)malloc(numPlayers * sizeof(int)))) {
		error("no mem");
		EndGame();
		return;
	}

	/* Figure out the average score and who has the best kill/death ratio */
	/* ratio for this round */
	ComputeEndOfRoundValues(&average_score,
								&num_best_players,
								&best_ratio,
								best_players);

	/* Print out the results of the round */
	if (winning_team != -1) {
		sprintf(msg, " < Team %d has won the game%s! >", winning_team,
				reason);
		SoundPlayAll(this, TEAM_WIN_SOUND);
	} else {
		sprintf(msg, " < We have a draw%s! >", reason);
		SoundPlayAll(this, TEAM_DRAW_SOUND);
	}
	BroadcastPlayMessage(msg);

	/* Give bonus to the best player */
	GiveBestPlayerBonus(average_score,
						   num_best_players,
						   best_ratio,
						   best_players);

	/* Give bonuses to the winning team */
	if (winning_team != -1) {
		for (i = 0; i < numPlayers; i++) {
			if (players[i]->team != winning_team) {
				continue;
			}
			if (IS_TANK_IND(this, i)
				|| (BIT(players[i]->status, PAUSE)
					&& players[i]->count <= 0)
				|| (BIT(players[i]->status, GAME_OVER)
					&& players[i]->mychar == 'W'
					&& players[i]->score == 0)) {
				continue;
			}
			for (j = 0; j < num_best_players; j++) {
				if (i == best_players[j]) {
					break;
				}
			}
			if (j == num_best_players) {
				GiveIndividualBonus(i, average_score);
			}
		}
	}

	ResetAllPlayers(this);

	CountRounds();

	free(best_players);
}

void World::IndividualGameOver(int winner)
{
	int 				i, j;
	DFLOAT				average_score;
	int 				num_best_players;
	int 				*best_players;
	DFLOAT				best_ratio;
	char				msg[MSG_LEN];

	if (!(best_players = (int *)malloc(numPlayers * sizeof(int)))) {
		error("no mem");
		EndGame();
	}

	/* Figure out what the average score is and who has the best kill/death */
	/* ratio for this round */
	ComputeEndOfRoundValues(&average_score, &num_best_players,
								&best_ratio, best_players);

	/* Print out the results of the round */
	if (winner == -1) {
		BroadcastPlayMessage(" < We have a draw! >");
		SoundPlayAll(this, PLAYER_DRAW_SOUND);
	}
	else if (winner == -2) {
		BroadcastPlayMessage(" < The robots have won the game! >");
		/* Perhaps this should be a different sound? */
		SoundPlayAll(this, PLAYER_WIN_SOUND);
	} else {
		sprintf(msg, " < %s has won the game! >", players[winner]->name);
		BroadcastPlayMessage(msg);
		SoundPlayAll(this, PLAYER_WIN_SOUND);
	}

	/* Give bonus to the best player */
	GiveBestPlayerBonus(average_score,
						   num_best_players,
						   best_ratio,
						   best_players);

	/* Give bonus to the winning player */
	if (winner >= 0) {
		for (i = 0; i < num_best_players; i++) {
			if (winner == best_players[i]) {
				break;
			}
		}
		if (i == num_best_players) {
			GiveIndividualBonus(winner, average_score);
		}
	}
	else if (winner == -2) {
		for (j = 0; j < numPlayers; j++) {
			if (IS_ROBOT_IND(this, j)) {
				for (i = 0; i < num_best_players; i++) {
					if (j == best_players[i]) {
						break;
					}
				}
				if (i == num_best_players) {
					GiveIndividualBonus(j, average_score);
				}
			}
		}
	}

	ResetAllPlayers(this);

	free(best_players);
}

void World::RaceGameOver(void)
{
	Player*		pl;
	int 		i,
				j,
				k,
				bestlap = 0,
				num_best_players = 0,
				num_active_players = 0,
				num_ordered_players = 0;
	int*		order;
	char		msg[MSG_LEN];

	/*
	 * Reassign players's starting posisitions based upon
	 * personal best lap times.
	 */
	if ((order = (int *)malloc(numPlayers * sizeof(int))) != NULL) {
		for (i = 0; i < numPlayers; i++) {
			pl = players[i];
			if (IS_TANK_PTR(pl)) {
				continue;
			}
			if (BIT(pl->status, PAUSE)
				|| (BIT(pl->status, GAME_OVER) && pl->mychar == 'W')
				|| pl->best_lap <= 0) {
				j = i;
			}
			else {
				for (j = 0; j < i; j++) {
					if (pl->best_lap < players[order[j]]->best_lap) {
						break;
					}
					if (BIT(players[order[j]]->status, PAUSE)
						|| (BIT(players[order[j]]->status, GAME_OVER)
							&& players[order[j]]->mychar == 'W')) {
						break;
					}
				}
			}
			for (k = i - 1; k >= j; k--) {
				order[k + 1] = order[k];
			}
			order[j] = i;
			num_ordered_players++;
		}
		for (i = 0; i < num_ordered_players; i++) {
			pl = players[order[i]];
			if (pl->home_base != baseorders[i].base_idx) {
				pl->home_base = baseorders[i].base_idx;
				for (j = 0; j < numPlayers; j++) {
					if (players[j]->conn ) {
								players[j]->conn->SendBase(pl->id, pl->home_base);
					}
				}
				if (BIT(pl->status, PAUSE)) {
					pl->GoHome();
				}
			}
		}
		free(order);
	}

	for (i = numPlayers - 1; i >= 0; i--)  {
		pl = players[i];
		CLR_BIT(pl->status, RACE_OVER | FINISH);
		if (BIT(pl->status, PAUSE)
			|| (BIT(pl->status, GAME_OVER) && pl->mychar == 'W')
			|| IS_TANK_PTR(pl)) {
			continue;
		}
		num_active_players++;

		/* Kill any remaining players */
		if (!BIT(pl->status, GAME_OVER))
			pl->Kill();
		else
			pl->DeathReset();
		if (pl != players[i]) {
			continue;
		}
		if ((pl->best_lap < bestlap || bestlap == 0) &&
			pl->best_lap > 0) {
			bestlap = pl->best_lap;
			num_best_players = 0;
		}
		if (pl->best_lap == bestlap)
			num_best_players++;
	}

	/* If someone completed a lap */
	if (bestlap > 0) {
		for (i = 0; i < numPlayers; i++)  {
			pl = players[i];
			if (BIT(pl->status, PAUSE)
				|| (BIT(pl->status, GAME_OVER) && pl->mychar == 'W')
				|| IS_TANK_PTR(pl)) {
				continue;
			}
			if (pl->best_lap == bestlap) {
				sprintf(msg,
						"%s %s the best lap time of %.2fs",
						pl->name,
						(num_best_players == 1) ? "had" : "shares",
						(DFLOAT) bestlap / roundDelay + GetFPS());
				BroadcastPlayMessage(msg);
				SCORE(pl, 5 + num_active_players,
					  OBJ_X_IN_BLOCKS(pl),
					  OBJ_Y_IN_BLOCKS(pl),
					  (num_best_players == 1) ? "[Fastest lap]" : "[Joint fastest lap]");
			}
		}

		updateScores = true;
	}
	else if (num_active_players > numRobots) {
		BroadcastPlayMessage("No-one even managed to complete one lap, you should be "
					"ashamed of yourselves.");
	}

	ResetAllPlayers(this);

	CountRounds();
}

void World::ComputeEndOfRoundValues(DFLOAT* average_score,
										int* num_best_players,
										DFLOAT* best_ratio,
										int best_players[])
{
	int 				i;
	DFLOAT				ratio;

	/* Initialize everything */
	*average_score = 0;
	*num_best_players = 0;
	*best_ratio = -1.0;

	/* Figure out what the average score is and who has the best kill/death */
	/* ratio for this round */
	for (i = 0; i < numPlayers; i++) {
		if (IS_TANK_IND(this, i)
			|| (BIT(players[i]->status, PAUSE)
			   && players[i]->count <= 0)) {
			continue;
		}
		*average_score += players[i]->score;
		ratio = (DFLOAT) players[i]->kills / (players[i]->deaths + 1);
		if (ratio > *best_ratio) {
			*best_ratio = ratio;
			best_players[0] = i;
			*num_best_players = 1;
		}
		else if (ratio == *best_ratio) {
			best_players[(*num_best_players)++] = i;
		}
	}
	*average_score /= numPlayers;
}


void World::GiveBestPlayerBonus(DFLOAT average_score,
								   int num_best_players,
								   DFLOAT best_ratio,
								   int best_players[])
{
	int 				i;
	DFLOAT				points;
	char				msg[MSG_LEN];


	if (best_ratio == 0) {
		sprintf(msg, "There is no Deadly Player");
	}
	else if (num_best_players == 1) {
		Player *bp = players[best_players[0]];

		sprintf(msg,
				"%s is the Deadliest Player with a kill ratio of %d/%d.",
				bp->name,
				bp->kills, bp->deaths);
		points = best_ratio * Rate(bp->score, average_score);
		SCORE(bp, points,
			  OBJ_X_IN_BLOCKS(bp),
			  OBJ_Y_IN_BLOCKS(bp),
			  "[Deadliest]");
	}
	else {
		msg[0] = '\0';
		for (i = 0; i < num_best_players; i++) {
			Player		*bp = players[best_players[i]];
			DFLOAT		ratio = Rate(bp->score, average_score);
			DFLOAT		score = (ratio + num_best_players)
								/ num_best_players;

			if (msg[0]) {
				if (i == num_best_players - 1)
					strcat(msg, " and ");
				else
					strcat(msg, ", ");
			}
			if (strlen(msg) + 8 + strlen(bp->name) >= sizeof(msg)) {
				BroadcastPlayMessage(msg);
				msg[0] = '\0';
			}
			strcat(msg, bp->name);
			points = (int) (best_ratio * score);
			SCORE(bp, points,
				  OBJ_X_IN_BLOCKS(bp),
				  OBJ_Y_IN_BLOCKS(bp),
				  "[Deadly]");
		}
		if (strlen(msg) + 64 >= sizeof(msg)) {
			BroadcastPlayMessage(msg);
			msg[0] = '\0';
		}
		sprintf(msg + strlen(msg),
				" are the Deadly Players with kill ratios of %d/%d.",
				players[best_players[0]]->kills,
				players[best_players[0]]->deaths);
	}
	BroadcastPlayMessage(msg);
}

void World::GiveIndividualBonus(int ind, DFLOAT average_score)
{
	DFLOAT				ratio;
	DFLOAT						points;

	ratio = (DFLOAT) players[ind]->kills / (players[ind]->deaths + 1);
	points = ratio * Rate(players[ind]->score, average_score);
	SCORE(players[ind], points,
		  OBJ_X_IN_BLOCKS(players[ind]),
		  OBJ_Y_IN_BLOCKS(players[ind]),
		  "[Winner]");
}


void World::CountRounds()
{
	char				msg[MSG_LEN];

	if (!options.roundsToPlay->GetInt()) {
		return;
	}

	++roundsPlayed;

	sprintf(msg, " < Round %d out of %d completed. >",
			roundsPlayed, options.roundsToPlay->GetInt());
	BroadcastPlayMessage(msg);
	if (roundsPlayed >= options.roundsToPlay->GetInt()) {
		GameOver();
	}
}

Player* World::FindPlayer(PCSTR nick, PlayerType pt)
{
	int 			i;

	for (i=0; i<numPlayers; i++)
	{
		if (!strcmp(players[i]->name, nick)
			&& (players[i]->playerType == pt
			|| players[i]->playerType == PlayerAny))
		{
				return(players[i]);
		}
	}
	return(NULL);
}
