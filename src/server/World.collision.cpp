/* $Id: World.collision.cpp,v 1.11 2004/05/04 21:07:17 dick Exp $
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
 *  $Log: World.collision.cpp,v $
 *  Revision 1.11  2004/05/04 21:07:17  dick
 *  Need randommnt.h
 *
 *  Revision 1.10  2004/04/29 16:08:27  dick
 *  compile with SOUND enabled.
 *
 *  Revision 1.9  2004/04/26 07:07:14  dick
 *  Define a list of pages that can be displayed on the ScoreTable in the client.
 *  Each page can be controlled by the server (and the server admin tool).
 *
 *  Revision 1.8  2004/02/23 07:18:03  dick
 *  Retab
 *
 *  Revision 1.7  2004/02/23 02:06:09  dick
 *  robot.h becomes Robot.h
 *
 *  Revision 1.6  2004/02/20 06:27:47  dick
 *  SendPlayMessage becomes BroadcastPlayMessage() because it sends a play message
 *  to all connections.
 *
 *  Revision 1.5  2004/02/18 01:22:14  dick
 *  Remove extraneous and obsolete #include "netserver.h"
 *
 *  Revision 1.4  2004/02/05 14:56:58  cppc
 *  First cut at integrating 'it' play mode.
 *
 *  Revision 1.3  2004/02/03 03:39:54  dick
 *  Add Windows precompiled header support.
 *
 *  Revision 1.2  2004/01/08 18:10:51  dick
 *  Update to XPilot 4.5.5beta
 *
 *  Revision 1.1  2002/09/11 16:39:50  dick
 *  cell.cpp becomes World.cell.cpp; collision.cpp becomes World.collision.cpp
 *
 *  Revision 1.19  2002/09/01 00:13:46  dick
 *  Remove obsolete windows #include "winServer.h"
 *
 *  Revision 1.18  2002/07/14 19:05:24  dick
 *  When talking to the ScoreServer about PCSTR nick, always mention the PlayerType too.
 *
 *  Revision 1.17  2002/07/11 19:31:15  dick
 *  Repair a filesystem weird burp thing.  Better not happen again... :(
 *
 *  Revision 1.16  2002/07/08 05:52:59  dick
 *  showtime() handles the space after itself
 *
 *  Revision 1.15  2002/06/30 07:00:44  dick
 *  Sprinkle ScoreServerScoreEvent()s around for every scoring event.
 *
 *  Revision 1.14  2002/06/25 05:04:24  dick
 *  Score_players takes an additional parameter: the ScoreType (describe the kill)
 *
 *  Revision 1.13  2002/06/01 06:06:48  dick
 *  Encapsulate (almost) everything.  Get rid of (almost) all refs to theWorld.
 *
 *  Revision 1.12  2002/05/22 05:09:01  dick
 *  Update to XPilot r4_5_4alpha.
 *
 *  Revision 1.11  2002/05/20 07:33:23  dick
 *  Integration bugs: Player_collides_with_killing_shot()
 *
 *  Revision 1.10  2002/05/18 22:34:15  dick
 *  Move showtime to common.
 *
 *  Revision 1.9  2002/05/18 20:55:39  dick
 *  Update to XPilot-4.5.1
 *
 *  Revision 1.8  2001/08/14 21:37:14  dick
 *  Array Obj[] becomes objs[].  We now have a class called Obj.
 *
 *  Revision 1.7  2001/08/05 08:05:36  dick
 *  Add Tuners to the server.  ComputeSensorRange(), AddTank(), RemoveTank()
 *  become members of Player.  ComputeGravity*() become members of World.
 *
 *  Revision 1.6  2001/07/25 21:25:59  dick
 *  float to int warnings
 *
 *  Revision 1.5  2001/07/24 09:04:00  dick
 *  Remove all global options.  Use World.options instead.
 *
 *  Revision 1.4  2001/07/23 21:08:36  dick
 *  Encapsulate ShipMass, ShotsMax, ShotsLife, ShotsMass, FPS.
 *
 *  Revision 1.3  2001/07/10 08:10:08  dick
 *  Don't need (deleted) map.h because World.h is included in global.h
 *
 *  Revision 1.2  2001/07/07 12:00:42  dick
 *  Rename classes to C++ "Style".  old World becomes theWorld.
 *
 */
#include "StdAfx.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <time.h>
#include <math.h>
#include <limits.h>
#include <assert.h>

#include "version.h"
#include "config.h"
#include "serverconst.h"
#include "PacketCtl.h"
#include "list.h"
#include "global.h"
#include "proto.h"
#include "showtime.h"
#include "score.h"
#include "saudio.h"
#include "item.h"
#include "pack.h"
#include "error.h"
#include "portability.h"
#include "objpos.h"
#include "asteroid.h"
#include "commonproto.h"
#include "randommt.h"
#include "Robot.h"

char collision_version[] = VERSION;


/*
 * The very first "analytical" collision patch, XPilot 3.6.2
 * Faster than other patches and accurate below half warp-speed
 * Trivial common subexpressions are eliminated by any reasonable compiler,
 * and kept here for readability.
 * Written by Pontus (Rakk, Kepler) pontus@ctrl-c.liu.se Jan 1998
 * Kudos to Svenske and Mad Gurka for beta testing, and Murx for
 * invaluable insights.
 */
#if 0
static int in_range_acd(
		int p1x, int p1y, int p2x, int p2y,
		int q1x, int q1y, int q2x, int q2y,
		int r)
{
	long				fac1, fac2;
	double				tmin, fminx, fminy;
	long				top, bot;
	long				dpx, dpy, dqx, dqy;
	long				dx, dy, dox, doy;

	/*
	 * Get the wrapped coordinates straight
	 */
	if (BIT(World.rules->mode, WRAP_PLAY)) {
		if (ABS(p2x - p1x) > World.width / 2) {
			if (p1x < p2x)
				p1x += World.width;
			else
				p2x += World.width;
		}
		if (ABS(p2y - p1y) > World.height / 2) {
			if (p1y < p2y)
				p1y += World.height;
			else
				p2y += World.height;
		}
		if (ABS(q2x - q1x) > World.width / 2) {
			if (q1x < q2x)
				q1x += World.width;
			else
				q2x += World.width;
		}
		if (ABS(q2y - q1y) > World.height / 2) {
			if (q1y < q2y)
				q1y += World.height;
			else
				q2y += World.height;
		}
	}

	dx = WRAP_DX(q2x - p2x);
	dy = WRAP_DY(q2y - p2y);
	if (sqr(dx) + sqr(dy) < sqr(r))
		return 1;

	dox = WRAP_DX(p1x - q1x);
	doy = WRAP_DY(p1y - q1y);
	if (sqr(dox) + sqr(doy) < sqr(r))
		return 1;

	dpx = WRAP_DX(p2x - p1x);
	dpy = WRAP_DY(p2y - p1y);
	dqx = WRAP_DX(q2x - q1x);
	dqy = WRAP_DY(q2y - q1y);

	/*
	 * Do the detection
	 */
	fac1 = dpx - dqx;
	fac2 = dpy - dqy;
	top = -(fac1 * dx + fac2 * dy);
	bot = (fac1 * fac1 + fac2 * fac2);
	if (top < 0 || bot < 1 || top > bot)
		return 0;
	tmin = ((double)top) / ((double)bot);		/* BG: could make top&bot doubles. */
	fminx = dx + fac1 * tmin;
	fminy = dy + fac2 * tmin;
	if (fminx * fminx + fminy * fminy < r * r)
		return 1;
	else
		return 0;
}
#else
int World::InRangeAcd(
		int p1x, int p1y, int p2x, int p2y,
		int q1x, int q1y, int q2x, int q2y,
		int r)
{
	long		fac1, fac2;
	double		tmin, fminx, fminy;
	long		top, bot;
	bool		mpx, mpy, mqx, mqy;

	/*
	 * Get the wrapped coordinates straight
	 */
	if (BIT(rules->mode, WRAP_PLAY)) {
		if ((mpx = (ABS(p2x - p1x) > width / 2))) {
			if (p1x > p2x)
				p1x -= width;
			else
				p2x -= width;
		}
		if ((mpy = (ABS(p2y - p1y) > height / 2))) {
			if (p1y > p2y)
				p1y -= height;
			else
				p2y -= height;
		}
		if ((mqx = (ABS(q2x - q1x) > width / 2))) {
			if (q1x > q2x)
				q1x -= width;
			else
				q2x -= width;
		}
		if ((mqy = (ABS(q2y - q1y) > height / 2))) {
			if (q1y > q2y)
				q1y -= height;
			else
				q2y -= height;
		}

		if (mpx && !mqx && (q2x > width / 2 || q1x > width / 2)) {
			q1x -= width;
			q2x -= width;
		}

		if (mqy && !mpy && (q2y > height / 2 || q1y > height / 2)) {
			q1y -= height;
			q2y -= height;
		}

		if (mqx && !mpx && (p2x > width / 2 || p1x > width / 2)) {
			p1x -= width;
			p2x -= width;
		}

		if (mqy && !mpy && (p2y > height / 2 || p1y > height / 2)) {
			p1y -= height;
			p2y -= height;
		}
	}

	/*
	 * Do the detection
	 */
	if ((p2x - q2x) * (p2x - q2x) + (p2y - q2y) * (p2y - q2y) < r * r)
		return 1;
	fac1 = -p1x + p2x + q1x - q2x;
	fac2 = -p1y + p2y + q1y - q2y;
	top = -(fac1 * (-p2x + q2x) + fac2 * (-p2y + q2y));
	bot = (fac1 * fac1 + fac2 * fac2);
	if (top < 0 || bot < 1 || top > bot)
		return 0;
	tmin = ((double)top) / ((double)bot);
	fminx = -p2x + q2x + fac1 * tmin;
	fminy = -p2y + q2y + fac2 * tmin;
	if (fminx * fminx + fminy * fminy < r * r)
		return 1;
	else
		return 0;
}
#endif

/*
 * Globals
 */
static char msg[MSG_LEN];

///////////////////////////////////////////////////////////////////////////////
void World::CheckCollision()
{
		BallCollision();
		MineCollision();

	if (roundDelay == 0) {
				PlayerCollision();
	}

	LaserPulseCollision();
	AsteroidCollision();
}

///////////////////////////////////////////////////////////////////////////////
void World::PlayerCollision()
{
	int			i, j;
	DFLOAT		sc, sc2;
	Player*		pl;

	/* Player - player, checkpoint, treasure, object and wall */
	for (i=0; i<numPlayers; i++) {
		pl = players[i];
		if (BIT(pl->status, PLAYING|PAUSE|GAME_OVER|KILLED) != PLAYING)
			continue;

		if (pl->pos.x < 0 || pl->pos.y < 0
		 || pl->pos.x >= width
		 || pl->pos.y >= height) {
			SET_BIT(pl->status, KILLED);
			sprintf(msg, "%s left the known universe.", pl->name);
			BroadcastPlayMessage(msg);
			sc = Rate(WALL_SCORE, pl->score);
			SCORE(pl, -sc,
				  OBJ_X_IN_BLOCKS(pl),
				  OBJ_Y_IN_BLOCKS(pl),
				  pl->name);
			ScoreServerScoreEvent(s_brWall, PlayerNone, 0, pl, -sc, ScoreCrash);
			continue;
		}

		if (BIT(pl->used, HAS_PHASING_DEVICE))
			continue;

		/* Player - player */
		if (BIT(rules->mode, CRASH_WITH_PLAYER | BOUNCE_WITH_PLAYER)) {
			for (j=i+1; j<numPlayers; j++) {
				if (BIT(players[j]->status, PLAYING|PAUSE|GAME_OVER|KILLED)
				 != PLAYING) {
					continue;
				}
				if (BIT(players[j]->used, HAS_PHASING_DEVICE))
					continue;
				if (!InRangeAcd(pl->prevpos.x, pl->prevpos.y,
								  pl->pos.x, pl->pos.y,
								  players[j]->prevpos.x,
								  players[j]->prevpos.y,
								  players[j]->pos.x, players[j]->pos.y,
								  2*SHIP_SZ-6))
				{
					continue;
				}

				/*
				* Here we can add code to do more accurate player against
				* player collision detection.
				* A new algorithm could be based on the following idea:
				*
				* - If we can draw an uninterupted line between two players:
				*   - Then test for both ships:
				*     - For the three points which make up a ship:
				*       - If we can draw a line between its previous
				*         position and its current position which does not
				*         cross the first line.
				* Then the ships have not collided even though they may be
				* very close to one another.
				* The choosing of the first line may not be easy however.
				*/

				if (pl->TeamImmune(players[j]->id)
				 || PSEUDO_TEAM(i, j)) {
					continue;
				}
				SoundPlaySensors(this, pl->pos.x, pl->pos.y, PLAYER_HIT_PLAYER_SOUND);
				if (BIT(rules->mode, BOUNCE_WITH_PLAYER)) {
					if (BIT(pl->used, (HAS_SHIELD|HAS_EMERGENCY_SHIELD)) !=
					(HAS_SHIELD|HAS_EMERGENCY_SHIELD))
					{
						Add_fuel(&(pl->fuel), (long)ED_PL_CRASH);
						Item_damage(pl, options.destroyItemInCollisionProb->GetDouble());
					}
					if (BIT(players[j]->used, (HAS_SHIELD|HAS_EMERGENCY_SHIELD))
						!= (HAS_SHIELD|HAS_EMERGENCY_SHIELD)) {
							Add_fuel(&(players[j]->fuel), (long)ED_PL_CRASH);
							Item_damage(players[j], options.destroyItemInCollisionProb->GetDouble());
					}
					pl->forceVisible = 20;
					players[j]->forceVisible = 20;
					ObjRepel((Object *)pl, (Object *)players[j], 2*SHIP_SZ);
				}
				if (!BIT(rules->mode, CRASH_WITH_PLAYER)) {
					continue;
				}

				if (pl->fuel.sum <= 0
				  || (!BIT(pl->used, HAS_SHIELD)
				  && !BIT(pl->have, HAS_ARMOR)))
				{
					SET_BIT(pl->status, KILLED);
				}
				if (players[j]->fuel.sum <= 0
				 || (!BIT(players[j]->used, HAS_SHIELD)
				 && !BIT(players[j]->have, HAS_ARMOR)))
				{
					SET_BIT(players[j]->status, KILLED);
				}

				if (!BIT(pl->used, HAS_SHIELD)
				 && BIT(pl->have, HAS_ARMOR)) {
					pl->HitArmor();
				}
				if (!BIT(players[j]->used, HAS_SHIELD)
				 && BIT(players[j]->have, HAS_ARMOR)) {
					players[j]->HitArmor();
				}

				if (BIT(players[j]->status, KILLED)) {
					if (BIT(pl->status, KILLED)) {
						sprintf(msg, "%s and %s crashed.", pl->name, players[j]->name);
						BroadcastPlayMessage(msg);
						if (!IS_TANK_IND(this, i) && !IS_TANK_IND(this, j)) {
							sc = Rate(players[j]->score, pl->score)
										* options.crashScoreMult->GetDouble();
							sc2 = Rate(pl->score, players[j]->score)
										* options.crashScoreMult->GetDouble();
							Score_players(pl, -sc, players[j]->name,
										  players[j], -sc2, pl->name, ScoreCrash);
						} else if (IS_TANK_IND(this, i)) {
							int i_tank_owner = getInd[players[i]->lock.pl_id];
							sc = Rate(players[i_tank_owner]->score, players[j]->score)
										* options.tankKillScoreMult->GetDouble();
							if (options.tagGame->GetBool())
								sc = HandleTag(sc, j, i_tank_owner);
							Score_players(players[i_tank_owner], sc, players[j]->name,
										  players[j], -sc, pl->name, ScoreCrash);
						} else if (IS_TANK_IND(this, j)) {
							int j_tank_owner = getInd[players[j]->lock.pl_id];
							sc = Rate(players[j_tank_owner]->score,	pl->score)
										* options.tankKillScoreMult->GetDouble();
							if (options.tagGame->GetBool())
								sc = HandleTag(sc, i, j_tank_owner);
							Score_players(players[j_tank_owner], sc, pl->name,
										  pl, -sc, players[j]->name, ScoreCrash);
						} /* don't bother scoring two tanks */

					} else {
						int i_tank_owner = i;
						if (IS_TANK_IND(this, i)) {
							i_tank_owner = getInd[players[i]->lock.pl_id];
							if (i_tank_owner == j) {
								i_tank_owner = i;
							}
						}
						sprintf(msg, "%s ran over %s.",	pl->name, players[j]->name);
						BroadcastPlayMessage(msg);
						SoundPlaySensors(this,
										   players[j]->pos.x,
										   players[j]->pos.y,
										   PLAYER_RAN_OVER_PLAYER_SOUND);
						pl->kills++;
						if (IS_TANK_IND(this, i)) {
							sc = Rate(players[i_tank_owner]->score, players[j]->score)
									* options.tankKillScoreMult->GetDouble();
						} else {
							sc = Rate(pl->score, players[j]->score)
									* options.runoverKillScoreMult->GetDouble();
						}
						if (options.tagGame->GetBool())
							sc = HandleTag(sc, j, i_tank_owner);
						Score_players(players[i_tank_owner], sc, players[j]->name,
									  players[j], -sc, pl->name, ScoreRunover);
					}
				} else {
					if (BIT(pl->status, KILLED)) {
						ScoreType st = ScoreRunover;
						int j_tank_owner = j;
						if (IS_TANK_IND(this, j)) {
							j_tank_owner = getInd[players[j]->lock.pl_id];
							if (j_tank_owner == i) {
								j_tank_owner = j;
							}
							st = ScoreTank;
						}
						sprintf(msg, "%s ran over %s.",
						players[j]->name, pl->name);
						BroadcastPlayMessage(msg);
						SoundPlaySensors(this, pl->pos.x, pl->pos.y,
							PLAYER_RAN_OVER_PLAYER_SOUND);
						players[j]->kills++;
						if (IS_TANK_IND(this, j)) {
							sc = Rate(players[j_tank_owner]->score, pl->score)
							* options.tankKillScoreMult->GetDouble();
						} else {
							sc = Rate(players[j]->score, pl->score)
									* options.runoverKillScoreMult->GetDouble();
						}
						if (options.tagGame->GetBool())
							sc = HandleTag(sc, i, j_tank_owner);
						Score_players(players[j_tank_owner], sc, pl->name,
										pl, -sc, players[j]->name, st);
					}
				}

				if (BIT(players[j]->status, KILLED)) {
					if (IS_ROBOT_IND(this, j)
					 && Robot_war_on_player(this, j) == pl->id) {
						Robot_reset_war(this, j);
					}
				}

				if (BIT(pl->status, KILLED)) {
					if (IS_ROBOT_PTR(pl)
					 && Robot_war_on_player(this, i) == players[j]->id) {
						Robot_reset_war(this, i);
					}
					/* cannot crash with more than one player at the same time? */
					/* hmm, if 3 players meet at the same point at the same time? */
					/* break; */
				}
			}
		}

		/* Player picking up ball/treasure */
		if (!BIT(pl->used, HAS_CONNECTOR) || BIT(pl->used, HAS_PHASING_DEVICE)) {
			pl->ball = NULL;
		}
		else if (pl->ball != NULL) {
			BallObject *ball = pl->ball;
			if (ball->life <= 0 || ball->id != NO_ID)
			pl->ball = NULL;
			else {
				DFLOAT distance = WrapLength(pl->pos.x - ball->pos.x,
											  pl->pos.y - ball->pos.y);
				if (distance >= options.ballConnectorLength->GetDouble()) {
					ball->id = pl->id;
					/* this is only the team of the owner of the ball,
					   not the team the ball belongs to. the latter is
					   found through the ball's treasure */
					ball->team = pl->team;
					if (ball->owner == NO_ID)
						ball->life = LONG_MAX;  /* for frame counter */
					ball->owner = pl->id;
					ball->length = distance;
					SET_BIT(ball->status, GRAVITY);
					treasures[ball->treasure].have = false;
					SET_BIT(pl->have, HAS_BALL);
					pl->ball = NULL;
					SoundPlaySensors(this, pl->pos.x, pl->pos.y,
									   CONNECT_BALL_SOUND);
				}
			}
		}
		else
		{
			/*
			* We want a separate list of balls to avoid searching
			* the object list for balls.
			*/
			int dist;
			int mindist = options.ballConnectorLength->GetInt();
			for (j = 0; j < numObjs; j++) {
				if (BIT(objs[j]->type, OBJ_BALL) && objs[j]->id == NO_ID) {
					dist = (int)WrapLength(pl->pos.x - objs[j]->pos.x,
					pl->pos.y - objs[j]->pos.y);
					if (dist < mindist) {
						BallObject *ball = BALL_PTR(objs[j]);
						int bteam = treasures[ball->treasure].team;

						/*
						* The treasure's team cannot connect before
						* somebody else has owned the ball.
						* This was done to stop team members
						* taking and hiding with the ball... this was
						* considered bad gamesmanship.
						*/
						if (!BIT(rules->mode, TEAM_PLAY)
						 || ball->owner != NO_ID
						 || pl->team != bteam) {
							pl->ball = BALL_PTR(objs[j]);
							mindist = dist;
						}
					}
				}
			}
		}

		PlayerObjectCollision(i);

		/* Player checkpoint */
		if (BIT(rules->mode, TIMING)
		 && BIT(pl->status, PAUSE|GAME_OVER) == 0) {
			if (pl->round != 0) {
				pl->time++;
			}
			if (BIT(pl->status, PLAYING|KILLED) == PLAYING
			&& WrapLength(pl->pos.x - check[pl->check].x * BLOCK_SZ,
			pl->pos.y - check[pl->check].y * BLOCK_SZ)
			< options.checkpointRadius->GetDouble() * BLOCK_SZ
			&& !IS_TANK_PTR(pl)
			&& !options.ballRace->GetBool()) {
				PlayerPassCheckpoint(i);
			}
		}
	}
}

int IsOffensiveItem(Items i)
{
    if (BIT(1 << i,
	    ITEM_BIT_WIDEANGLE |
	    ITEM_BIT_REARSHOT |
	    ITEM_BIT_MINE |
	    ITEM_BIT_MISSILE |
	    ITEM_BIT_LASER)) {
	return true;
    }
    return false;
}

int IsDefensiveItem(Items i)
{
	if (BIT(1 << i,
		ITEM_BIT_CLOAK |
		ITEM_BIT_ECM |
		ITEM_BIT_TRANSPORTER |
		ITEM_BIT_TRACTOR_BEAM |
		ITEM_BIT_EMERGENCY_SHIELD |
		ITEM_BIT_MIRROR |
		ITEM_BIT_DEFLECTOR |
		ITEM_BIT_HYPERJUMP |
		ITEM_BIT_PHASING |
		ITEM_BIT_TANK |
		ITEM_BIT_ARMOR)) {
			return true;
	}
	return false;
}

int CountOffensiveItems(Player *pl)
{
    return (pl->item[ITEM_WIDEANGLE] + pl->item[ITEM_REARSHOT] +
	    pl->item[ITEM_MINE] + pl->item[ITEM_MISSILE] +
	    pl->item[ITEM_LASER]);
}

int CountDefensiveItems(Player *pl)
{
	int count;

	count = pl->item[ITEM_CLOAK] + pl->item[ITEM_ECM] + pl->item[ITEM_ARMOR] +
			pl->item[ITEM_TRANSPORTER] + pl->item[ITEM_TRACTOR_BEAM] +
			pl->item[ITEM_EMERGENCY_SHIELD] + pl->fuel.num_tanks +
			pl->item[ITEM_DEFLECTOR] + pl->item[ITEM_HYPERJUMP] +
			pl->item[ITEM_PHASING] + pl->item[ITEM_MIRROR];
	if (pl->emergency_shield_left)
		count++;
	if (pl->phasing_left)
		count++;
	return count;
}


void World::PlayerObjectCollision(int ind)
{
	int			j, range, radius, hit, obj_count;
	Player*		pl = players[ind];
	Object*		obj;
	Object**	obj_list;


	D(	int	_i;
		for (_i=0; _i<MAX_TOTAL_SHOTS; _i++) {
			assert(objs[_i]->type <= 0x2000);
		}
	)
   /*
	* Collision between a player and an object.
	*/
	if (BIT(pl->status, PLAYING|PAUSE|GAME_OVER|KILLED) != PLAYING)
		return;

	CellGetObjects(OBJ_X_IN_BLOCKS(pl), OBJ_Y_IN_BLOCKS(pl),
				   4, 500,
				   &obj_list, &obj_count);

	for (j = 0; j < obj_count; j++) {
		obj = obj_list[j];
		if (obj->life <= 0) {
			continue;
		}

		range = SHIP_SZ + obj->pl_range;
		if (!InRangeAcd(pl->prevpos.x, pl->prevpos.y,
			pl->pos.x, pl->pos.y,
			obj->prevpos.x, obj->prevpos.y,
			obj->pos.x, obj->pos.y,
			range)) {
				continue;
		}

		if (obj->id != NO_ID) {
			if (obj->id == pl->id) {
				if (BIT(obj->type, OBJ_SPARK|OBJ_MINE)
				 && BIT(obj->status, OWNERIMMUNE)) {
					continue;
				}
				else if (options.selfImmunity->GetBool()) {
					continue;
				}
			} else if (options.selfImmunity->GetBool()
						&& IS_TANK_PTR(pl)
						&& (pl->lock.pl_id == obj->id)) {
				continue;
			} else if (pl->TeamImmune(obj->id)) {
				continue;
			} else if (BIT(players[getInd[obj->id]]->status, PAUSE)) {
				continue;
			}
		} else if (BIT(rules->mode, TEAM_PLAY)
					&& options.teamImmunity->GetBool()
					&& obj->team == pl->team
					/* allow players to destroy their team's unowned balls */
					&& obj->type != OBJ_BALL) {
			continue;
		}

		if (obj->type == OBJ_ITEM) {
			if (BIT(pl->used, HAS_SHIELD)
			  && !options.shieldedItemPickup->GetBool()) {
				SET_BIT(obj->status, GRAVITY);
				Delta_mv((Object*)pl, obj);
				continue;
			}
		}
		else if (BIT(obj->type, OBJ_HEAT_SHOT | OBJ_SMART_SHOT | OBJ_TORPEDO
								| OBJ_SHOT | OBJ_CANNON_SHOT)) {
			if (pl->id == obj->id && obj->life > obj->fuselife) {
			continue;
			}
		}
		else if (BIT(obj->type, OBJ_MINE)) {
			if (BIT(obj->status, CONFUSED)) {
				continue;
			}
		}
		else if (BIT(obj->type, OBJ_BALL) && obj->id != NO_ID) {
			if (BIT(players[getInd[obj->id]]->used, HAS_PHASING_DEVICE)) {
				continue;
			}
		}

		/*
		 * Objects actually only hit the player if they are really close.
		 */
		radius = SHIP_SZ + obj->pl_radius;
		if (radius >= range) {
			hit = 1;
		} else {
			hit = InRangeAcd(pl->prevpos.x, pl->prevpos.y,
							 pl->pos.x, pl->pos.y,
							 obj->prevpos.x, obj->prevpos.y,
							 obj->pos.x, obj->pos.y,
			range);
		}

		/*
		* Object collision.
		*/
		switch (obj->type) {
		case OBJ_BALL:
			if (! hit) {
				continue;
			}
			PlayerCollidesWithBall(ind, obj, radius);
			if (BIT(pl->status, KILLED)) {
				return;
			}
			continue;

		case OBJ_ITEM:
			PlayerCollidesWithItem(ind, obj);
			/* if life is non-zero then no collision occurred */
			if (obj->life != 0) {
				continue;
			}
			break;

		case OBJ_MINE:
			PlayerCollidesWithMine(ind, obj);
			break;

		case OBJ_WRECKAGE:
		case OBJ_DEBRIS:
			PlayerCollidesWithDebris(ind, obj);
			if (BIT(pl->status, KILLED)) {
				return;
			}
			break;

		case OBJ_ASTEROID:
			if (hit) {
				PlayerCollidesWithAsteroid(ind, WIRE_PTR(obj));
				Delta_mv_elastic((Object *)pl, obj);
			}
			if (BIT(pl->status, KILLED)) {
				return;
			}
			continue;
		case OBJ_CANNON_SHOT:
			/* don't explode cannon flak if it hits directly */
			CLR_BIT(obj->mods.warhead, CLUSTER);
			break;

		default:
			break;
		}

		obj->life = 0;

		if (BIT(obj->type, KILLING_SHOTS)) {
			PlayerCollidesWithKillingShot(ind, obj);
			if (BIT(pl->status, KILLED)) {
				return;
			}
		}
		if (hit) {
			Delta_mv((Object *)pl, obj);
		}
	}
}


void World::PlayerCollidesWithBall(int ind, Object *obj, int radius)
{
	Player		*pl = players[ind];
	DFLOAT		sc;
	int 		killer;
	BallObject	*ball = BALL_PTR(obj);

	/*
	 * The ball is special, usually players bounce off of it with
	 * shields up, or die with shields down.  The treasure may
	 * be destroyed.
	 */
	ObjRepel((Object *)pl, obj, radius);
	if (BIT(pl->used, (HAS_SHIELD|HAS_EMERGENCY_SHIELD))
		!= (HAS_SHIELD|HAS_EMERGENCY_SHIELD)) {
		Add_fuel(&(pl->fuel), (long)ED_BALL_HIT);
		if (options.treasureCollisionDestroys->GetBool()) {
			ball->life = 0;
		}
	}
	if (pl->fuel.sum > 0) {
		if (!options.treasureCollisionMayKill->GetBool() || BIT(pl->used, HAS_SHIELD))
			return;
		if (!BIT(pl->used, HAS_SHIELD) && BIT(pl->have, HAS_ARMOR)) {
			pl->HitArmor();
			return;
		}
	}
	if (ball->owner == NO_ID) {
				sprintf(msg, "%s was killed by a ball.", pl->name);
				sc = Rate(0, pl->score)
								* options.ballKillScoreMult->GetDouble()
								* options.unownedKillScoreMult->GetDouble();
				SCORE(pl, -sc,
			  OBJ_X_IN_BLOCKS(pl),
			  OBJ_Y_IN_BLOCKS(pl),
			  "Ball");
				ScoreServerScoreEvent(s_brBall, PlayerNone, 0, pl, -sc, ScoreBall);
	} else {
		killer = getInd[ball->owner];

		sprintf(msg, "%s was killed by a ball owned by %s.",
				pl->name, players[killer]->name);

		if (killer == ind) {
			strcat(msg, "  How strange!");
				sc = Rate(0, pl->score)
								* options.ballKillScoreMult->GetDouble()
								* options.selfKillScoreMult->GetDouble();
			SCORE(pl, -sc,
				  OBJ_X_IN_BLOCKS(pl),
				  OBJ_Y_IN_BLOCKS(pl),
				  players[killer]->name);
				ScoreServerScoreEvent(pl, 0, pl, -sc, ScoreBall);
		} else {
			players[killer]->kills++;
			sc = Rate(players[killer]->score, pl->score)
					   * options.ballKillScoreMult->GetDouble();
				if (options.tagGame->GetBool())
						sc = HandleTag(sc, ind, killer);
			Score_players(players[killer], sc, pl->name,
						  pl, -sc, players[killer]->name, ScoreBall);
				ScoreServerScoreEvent(players[killer], sc, pl, -sc, ScoreBall);
			Robot_war(this, ind, killer);
		}
	}
	BroadcastPlayMessage(msg);
	SET_BIT(pl->status, KILLED);
}


void World::PlayerCollidesWithItem(int ind, Object *obj)
{
	Player* 	pl = players[ind];
	int 		old_have;
	Items		item_index;

	if (IsOffensiveItem((Items) obj->info)) {
		int off_items = CountOffensiveItems(pl);
		if (off_items >= options.maxOffensiveItems->GetInt()) {
			/* Set_player_message(pl, "No space left for offensive items."); */
			Delta_mv((Object *)pl, obj);
			return;
		}
		else if (obj->count > 1
				 && off_items + obj->count > options.maxOffensiveItems->GetInt()) {
			obj->count = options.maxOffensiveItems->GetInt() - off_items;
		}
	}
	else if (IsDefensiveItem((Items) obj->info)) {
		int def_items = CountDefensiveItems(pl);
		if (def_items >= options.maxDefensiveItems->GetInt()) {
			/* Set_player_message(pl, "No space for left for defensive items."); */
			Delta_mv((Object *)pl, obj);
			return;
		}
		else if (obj->count > 1
				 && def_items + obj->count > options.maxDefensiveItems->GetInt()) {
			obj->count = options.maxDefensiveItems->GetInt() - def_items;
		}
	}

	item_index = (Items) obj->info;

	switch (item_index) {
	case ITEM_WIDEANGLE:
		pl->item[item_index] += obj->count;
		LIMIT(pl->item[item_index], 0, items[item_index].limit);
		SoundPlaySensors(this, pl->pos.x, pl->pos.y,
						   WIDEANGLE_SHOT_PICKUP_SOUND);
		break;
	case ITEM_ECM:
		pl->item[item_index] += obj->count;
		LIMIT(pl->item[item_index], 0, items[item_index].limit);
		SoundPlaySensors(this, pl->pos.x, pl->pos.y, ECM_PICKUP_SOUND);
		break;
	case ITEM_ARMOR:
		pl->item[item_index]++;
		LIMIT(pl->item[item_index], 0, items[item_index].limit);
		if (pl->item[item_index] > 0)
			SET_BIT(pl->have, HAS_ARMOR);
		SoundPlaySensors(this, pl->pos.x, pl->pos.y,
						   ARMOR_PICKUP_SOUND);
		break;
	case ITEM_TRANSPORTER:
		pl->item[item_index] += obj->count;
		LIMIT(pl->item[item_index], 0, items[item_index].limit);
		SoundPlaySensors(this, pl->pos.x, pl->pos.y, TRANSPORTER_PICKUP_SOUND);
		break;
	case ITEM_MIRROR:
		pl->item[ITEM_MIRROR] += obj->count;
		LIMIT(pl->item[item_index], 0, items[item_index].limit);
		if (pl->item[item_index] > 0)
			SET_BIT(pl->have, HAS_MIRROR);
		SoundPlaySensors(this, pl->pos.x, pl->pos.y, MIRROR_PICKUP_SOUND);
		break;
	case ITEM_DEFLECTOR:
		pl->item[ITEM_DEFLECTOR] += obj->count;
		LIMIT(pl->item[item_index], 0, items[item_index].limit);
		if (pl->item[item_index] > 0)
			SET_BIT(pl->have, HAS_DEFLECTOR);
		SoundPlaySensors(this, pl->pos.x, pl->pos.y, DEFLECTOR_PICKUP_SOUND);
		break;
	case ITEM_HYPERJUMP:
		pl->item[item_index] += obj->count;
		LIMIT(pl->item[item_index], 0, items[item_index].limit);
		SoundPlaySensors(this, pl->pos.x, pl->pos.y, HYPERJUMP_PICKUP_SOUND);
		break;
	case ITEM_PHASING:
		pl->item[item_index] += obj->count;
		LIMIT(pl->item[item_index], 0, items[item_index].limit);
		if (pl->item[item_index] > 0)
			SET_BIT(pl->have, HAS_PHASING_DEVICE);
		SoundPlaySensors(this, pl->pos.x, pl->pos.y, PHASING_DEVICE_PICKUP_SOUND);
		break;
	case ITEM_SENSOR:
		pl->item[item_index] += obj->count;
		LIMIT(pl->item[item_index], 0, items[item_index].limit);
		pl->updateVisibility = 1;
		SoundPlaySensors(this, pl->pos.x, pl->pos.y, SENSOR_PACK_PICKUP_SOUND);
		break;
	case ITEM_AFTERBURNER:
		pl->item[item_index] += obj->count;
		LIMIT(pl->item[item_index], 0, items[item_index].limit);
		if (pl->item[item_index] > 0)
			SET_BIT(pl->have, HAS_AFTERBURNER);
		SoundPlaySensors(this, pl->pos.x, pl->pos.y, AFTERBURNER_PICKUP_SOUND);
		break;
	case ITEM_REARSHOT:
		pl->item[item_index] += obj->count;
		LIMIT(pl->item[item_index], 0, items[item_index].limit);
		SoundPlaySensors(this, pl->pos.x, pl->pos.y, BACK_SHOT_PICKUP_SOUND);
		break;
	case ITEM_MISSILE:
		pl->item[item_index] += obj->count;
		LIMIT(pl->item[item_index], 0, items[item_index].limit);
		SoundPlaySensors(this, pl->pos.x, pl->pos.y, ROCKET_PACK_PICKUP_SOUND);
		break;
	case ITEM_CLOAK:
		pl->item[item_index] += obj->count;
		LIMIT(pl->item[item_index], 0, items[item_index].limit);
		if (pl->item[item_index] > 0)
			SET_BIT(pl->have, HAS_CLOAKING_DEVICE);
		pl->updateVisibility = 1;
		SoundPlaySensors(this, pl->pos.x, pl->pos.y, CLOAKING_DEVICE_PICKUP_SOUND);
		break;
	case ITEM_FUEL:
		Add_fuel(&(pl->fuel), ENERGY_PACK_FUEL);
		SoundPlaySensors(this, pl->pos.x, pl->pos.y, ENERGY_PACK_PICKUP_SOUND);
		break;
	case ITEM_MINE:
		pl->item[item_index] += obj->count;
		LIMIT(pl->item[item_index], 0, items[item_index].limit);
		SoundPlaySensors(this, pl->pos.x, pl->pos.y, MINE_PACK_PICKUP_SOUND);
		break;
	case ITEM_LASER:
		pl->item[item_index] += obj->count;
		LIMIT(pl->item[item_index], 0, items[item_index].limit);
		SoundPlaySensors(this, pl->pos.x, pl->pos.y, LASER_PICKUP_SOUND);
		break;
	case ITEM_EMERGENCY_THRUST:
		pl->item[item_index] += obj->count;
		LIMIT(pl->item[item_index], 0, items[item_index].limit);
		if (pl->item[item_index] > 0)
			SET_BIT(pl->have, HAS_EMERGENCY_THRUST);
		SoundPlaySensors(this, pl->pos.x, pl->pos.y, EMERGENCY_THRUST_PICKUP_SOUND);
		break;
	case ITEM_EMERGENCY_SHIELD:
		old_have = pl->have;
		pl->item[item_index] += obj->count;
		LIMIT(pl->item[item_index], 0, items[item_index].limit);
		if (pl->item[item_index] > 0)
			SET_BIT(pl->have, HAS_EMERGENCY_SHIELD);
		SoundPlaySensors(this, pl->pos.x, pl->pos.y, EMERGENCY_SHIELD_PICKUP_SOUND);
		/*
		 * New feature since 3.2.7:
		 * If we're playing in a map where shields are not allowed
		 * and a player picks up her first emergency shield item
		 * then we'll immediately turn on emergency shield.
		 */
		if (!BIT(old_have, HAS_SHIELD | HAS_EMERGENCY_SHIELD)
			&& pl->item[ITEM_EMERGENCY_SHIELD] == 1) {
			pl->EmergencyShield(true);
		}
		break;
	case ITEM_TRACTOR_BEAM:
		pl->item[item_index] += obj->count;
		LIMIT(pl->item[item_index], 0, items[item_index].limit);
		if (pl->item[item_index] > 0)
			SET_BIT(pl->have, HAS_TRACTOR_BEAM);
		SoundPlaySensors(this, pl->pos.x, pl->pos.y, TRACTOR_BEAM_PICKUP_SOUND);
		break;
	case ITEM_AUTOPILOT:
		pl->item[item_index] += obj->count;
		LIMIT(pl->item[item_index], 0, items[item_index].limit);
		if (pl->item[item_index] > 0)
			SET_BIT(pl->have, HAS_AUTOPILOT);
		SoundPlaySensors(this, pl->pos.x, pl->pos.y, AUTOPILOT_PICKUP_SOUND);
		break;

	case ITEM_TANK:
		if (pl->fuel.num_tanks < items[ITEM_TANK].limit) {
			pl->AddTank(TANK_FUEL(pl->fuel.num_tanks + 1));
		} else {
			Add_fuel(&(pl->fuel), TANK_FUEL(MAX_TANKS));
		}
		SoundPlaySensors(this, pl->pos.x, pl->pos.y, TANK_PICKUP_SOUND);
		break;
	case NUM_ITEMS:
		/* impossible */
		break;
	}

	obj->life = 0;
}


void World::PlayerCollidesWithMine(int ind, Object *obj)
{
	Player		*pl = players[ind];
	DFLOAT		sc;
	int 		killer;
	MineObject	*mine = MINE_PTR(obj);

	SoundPlaySensors(this, pl->pos.x, pl->pos.y, PLAYER_HIT_MINE_SOUND);
	killer = -1;
	if (mine->id == NO_ID && mine->owner == NO_ID) {
		sprintf(msg, "%s hit %s.",
				pl->name,
				Describe_shot(mine->type, mine->status, mine->mods, 1));
	}
	else if (mine->owner == mine->id) {
		killer = getInd[mine->owner];
		sprintf(msg, "%s hit %s %s by %s.", pl->name,
				Describe_shot(mine->type, mine->status, mine->mods,1),
				BIT(mine->status, GRAVITY) ? "thrown " : "dropped ",
				players[killer]->name);
	}
	else if (mine->owner == NO_ID) {
		const char *reprogrammer_name = "some jerk";
		if (mine->id != NO_ID) {
			killer = getInd[mine->id];
			reprogrammer_name = players[killer]->name;
		}
		sprintf(msg, "%s hit %s reprogrammed by %s.",
				pl->name,
				Describe_shot(mine->type, mine->status, mine->mods, 1),
				reprogrammer_name);
	}
	else {
		const char *reprogrammer_name = "some jerk";
		if (mine->id != NO_ID) {
			killer = getInd[mine->id];
			reprogrammer_name = players[killer]->name;
		}
		sprintf(msg, "%s hit %s %s by %s and reprogrammed by %s.",
				pl->name,
				Describe_shot(mine->type, mine->status, mine->mods,1),
				BIT(mine->status, GRAVITY) ? "thrown " : "dropped ",
				players[getInd[mine->owner]]->name,
				reprogrammer_name);
	}
	if (killer != -1) {
		/*
		 * Question with this is if we want to give the same points for
		 * a high-scored-player hitting a low-scored-player's mine as
		 * for a low-scored-player hitting a high-scored-player's mine.
		 * Maybe not.
		 */
		sc = Rate(players[killer]->score, pl->score)
				   * options.mineScoreMult->GetDouble();
		Score_players(players[killer], sc, pl->name,
					  pl, -sc, players[killer]->name, ScoreMine);
	}
	BroadcastPlayMessage(msg);
}


void World::PlayerCollidesWithDebris(int ind, Object *obj)
{
	Player				*pl = players[ind];
	DFLOAT				v = VECTOR_LENGTH(obj->vel);
	long				tmp = (long) (2 * obj->mass * v);
	long				cost = ABS(tmp);
	int 				killer;
	DFLOAT				sc;

	if (BIT(pl->used, (HAS_SHIELD|HAS_EMERGENCY_SHIELD))
		!= (HAS_SHIELD|HAS_EMERGENCY_SHIELD))
		Add_fuel(&pl->fuel, - cost);
	if (pl->fuel.sum == 0
		|| (obj->type == OBJ_WRECKAGE
			&& options.wreckageCollisionMayKill->GetBool()
			&& !BIT(pl->used, HAS_SHIELD)
			&& !BIT(pl->have, HAS_ARMOR))) {
		SET_BIT(pl->status, KILLED);
		sprintf(msg, "%s succumbed to an explosion.", pl->name);
		killer = -1;
		if (obj->id != NO_ID) {
			killer = getInd[obj->id];
			sprintf(msg + strlen(msg) - 1, " from %s.",
					players[killer]->name);
			if (obj->id == pl->id) {
				sprintf(msg + strlen(msg), "  How strange!");
			}
		}
		BroadcastPlayMessage(msg);
		if (killer == -1 || killer == ind) {
				PCSTR	killername = pl->name;
				if (killer == -1)
						killername = s_brExplosion;
				sc = Rate(0, pl->score)
								* options.explosionKillScoreMult->GetDouble()
								* options.selfKillScoreMult->GetDouble();
			SCORE(pl, -sc,
				  OBJ_X_IN_BLOCKS(pl),
				  OBJ_Y_IN_BLOCKS(pl),
				  killername);
				ScoreServerScoreEvent(killername, PlayerNone, 0, pl, -sc, ScoreExplode);
		} else {
			players[killer]->kills++;
			sc = Rate(players[killer]->score, pl->score)
					   * options.explosionKillScoreMult->GetDouble();
				if (options.tagGame->GetBool())
					sc = HandleTag(sc, ind, killer);
			Score_players(players[killer], sc, pl->name,
						  pl, -sc, players[killer]->name, ScoreExplode);
			ScoreServerScoreEvent(players[killer], sc, pl, -sc, ScoreExplode);
		}
		obj->life = 0;
		return;
	}
	if (obj->type == OBJ_WRECKAGE
		&& options.wreckageCollisionMayKill->GetBool()
		&& !BIT(pl->used, HAS_SHIELD)
		&& BIT(pl->have, HAS_ARMOR))
	{
			pl->HitArmor();
	}
}


void World::PlayerCollidesWithAsteroid(int ind, WireObject *ast)
{
    Player	*pl = players[ind];
    DFLOAT	v = VECTOR_LENGTH(ast->vel);
    long	tmp = (long) (2 * ast->mass * v);
    long	cost = ABS(tmp);

    ast->life += (long)ASTEROID_FUEL_HIT(ED_PL_CRASH, ast->size);
    if (ast->life < 0)
	ast->life = 0;
    if (ast->life == 0
		&& options.asteroidPoints->GetInt() > 0
		&& options.asteroidMaxScore->GetInt())
	{
		SCORE(pl, options.asteroidPoints->GetInt(),
			OBJ_X_IN_BLOCKS(ast),
			OBJ_Y_IN_BLOCKS(ast), "");
		ScoreServerScoreEvent(pl, options.asteroidPoints->GetInt(),
			s_brAsteroid, PlayerNone, 0, ScoreAsteroid);
    }
    if (BIT(pl->used, (HAS_SHIELD|HAS_EMERGENCY_SHIELD))
	!= (HAS_SHIELD|HAS_EMERGENCY_SHIELD)) {
	Add_fuel(&pl->fuel, -cost);
    }
	if (options.asteroidCollisionMayKill->GetBool()
		&& (pl->fuel.sum == 0
			|| (!BIT(pl->used, HAS_SHIELD)
			&& !BIT(pl->have, HAS_ARMOR))))
	{
		DFLOAT	sc;
		SET_BIT(pl->status, KILLED);
		if (pl->velocity > v) {
			/* player moves faster than asteroid */
			sprintf(msg, "%s smashed into an asteroid.", pl->name);
		} else {
			sprintf(msg, "%s was hit by an asteroid.", pl->name);
		}
		BroadcastPlayMessage(msg);
		sc = Rate(0, pl->score) * options.unownedKillScoreMult->GetDouble();
		SCORE(pl, -sc,
			  OBJ_X_IN_BLOCKS(pl),
			  OBJ_Y_IN_BLOCKS(pl),
			  s_brAsteroid);
		ScoreServerScoreEvent(s_brAsteroid, PlayerNone, 0, pl, -sc, ScoreAsteroid);
		if (IS_TANK_PTR(pl)
			&& options.asteroidPoints->GetInt() > 0)
		{
			int owner = getInd[pl->lock.pl_id];
			if (players[owner]->score <= options.asteroidMaxScore->GetDouble())
			{
				SCORE(players[owner], options.asteroidPoints->GetInt(),
					OBJ_X_IN_BLOCKS(ast), OBJ_Y_IN_BLOCKS(ast),	"");
				ScoreServerScoreEvent(players[owner], options.asteroidPoints->GetInt(),
					s_brAsteroid, PlayerNone, 0, ScoreAsteroid);
			}
		}
		return;
    }
	if (options.asteroidCollisionMayKill->GetBool()
		&& !BIT(pl->used, HAS_SHIELD)
		&& BIT(pl->have, HAS_ARMOR))
	{
		pl->HitArmor();
    }
}


void World::PlayerCollidesWithKillingShot(int ind, Object *obj)
{
    Player*	pl = players[ind];
    DFLOAT	sc;
    DFLOAT  drainfactor;
    long	drain;
    int		killer = NO_ID;
	ScoreType	st;

    /*
     * Player got hit by a potentially deadly object.
     *
     * When a player has shields up, and not enough fuel
     * to `absorb' the shot then shields are lowered.
     * This is not very logical, rather in this case
     * the shot should be considered to be deadly too.
     *
     * Sound effects are missing when shot is deadly.
     */

    if (BIT(pl->used, HAS_SHIELD)
	|| BIT(pl->have, HAS_ARMOR)
	|| (obj->type == OBJ_TORPEDO
	    && BIT(obj->mods.nuclear, NUCLEAR)
	    && (int)(rfrac() >= 0.25f)))
	{
		switch (obj->type)
		{
		case OBJ_TORPEDO:
			SoundPlaySensors(this, pl->pos.x, pl->pos.y,
					   PLAYER_EAT_TORPEDO_SHOT_SOUND);
			break;
		case OBJ_HEAT_SHOT:
			SoundPlaySensors(this, pl->pos.x, pl->pos.y,
					   PLAYER_EAT_HEAT_SHOT_SOUND);
			break;
		case OBJ_SMART_SHOT:
			SoundPlaySensors(this, pl->pos.x, pl->pos.y,
					   PLAYER_EAT_SMART_SHOT_SOUND);
			break;
		}

		switch(obj->type)
		{
		case OBJ_TORPEDO:
		case OBJ_HEAT_SHOT:
		case OBJ_SMART_SHOT:
			if (obj->id == NO_ID)
				sprintf(msg, "%s ate %s.", pl->name,
					Describe_shot(obj->type, obj->status,
							  obj->mods, 1));
			else
				sprintf(msg, "%s ate %s from %s.", pl->name,
					Describe_shot(obj->type, obj->status,
							  obj->mods, 1),
					players[ killer=getInd[obj->id] ]->name);
			drain = (long)(ED_SMART_SHOT_HIT /
			((obj->mods.mini + 1) * (obj->mods.power + 1)));
			if (BIT(pl->used, (HAS_SHIELD|HAS_EMERGENCY_SHIELD))
				!= (HAS_SHIELD|HAS_EMERGENCY_SHIELD))
				Add_fuel(&(pl->fuel), drain);
			pl->forceVisible += 2;
			BroadcastPlayMessage(msg);
			break;

		case OBJ_SHOT:
		case OBJ_CANNON_SHOT:
			SoundPlaySensors(this, pl->pos.x, pl->pos.y,
					   PLAYER_EAT_SHOT_SOUND);
			if (BIT(pl->used, (HAS_SHIELD|HAS_EMERGENCY_SHIELD))
			!= (HAS_SHIELD|HAS_EMERGENCY_SHIELD)) {
				if (options.shotHitFuelDrainUsesKineticEnergy->GetBool())
				{
					DFLOAT rel_velocity = LENGTH(pl->vel.x - obj->vel.x,
												 pl->vel.y - obj->vel.y);
					drainfactor = (rel_velocity * rel_velocity * ABS(obj->mass))
								  / (options.shotSpeed->GetDouble()
								   * options.shotSpeed->GetDouble()
								   * options.shotMass->GetDouble());
				} else {
					drainfactor = 1.0f;
				}
			drain = (long)(ED_SHOT_HIT * drainfactor * SHOT_MULT(obj));
			Add_fuel(&(pl->fuel), drain);
			}
			pl->forceVisible = (int)(pl->forceVisible + SHOT_MULT(obj));
			break;

		default:
			xpprintf("%sYou were hit by what?\n", showtime());
			st = ScoreUnknown;
			break;
		}
		if (pl->fuel.sum <= 0) {
			CLR_BIT(pl->used, HAS_SHIELD);
		}
		if (!BIT(pl->used, HAS_SHIELD) && BIT(pl->have, HAS_ARMOR)) {
			pl->HitArmor();
		}
    } else
	{
		DFLOAT factor;
		switch (obj->type)
		{
		case OBJ_TORPEDO:
		case OBJ_SMART_SHOT:
		case OBJ_HEAT_SHOT:
		case OBJ_SHOT:
		case OBJ_CANNON_SHOT:
			if (BIT(obj->status, FROMCANNON)) {
				SoundPlaySensors(this, pl->pos.x, pl->pos.y,
						   PLAYER_HIT_CANNONFIRE_SOUND);
				sprintf(msg, "%s was hit by cannonfire.", pl->name);
				sc = Rate(CANNON_SCORE, pl->score)/4;
			} else if (obj->id == NO_ID) {
				sprintf(msg, "%s was killed by %s.", pl->name,
					Describe_shot(obj->type, obj->status,
							  obj->mods, 1));
				sc = Rate(0, pl->score) * options.unownedKillScoreMult->GetDouble();

			} else {
				sprintf(msg, "%s was killed by %s from %s.",
					pl->name,
					Describe_shot(obj->type, obj->status,
								  obj->mods, 1),
					players[killer=getInd[obj->id]]->name);
				if (killer == ind) {
					SoundPlaySensors(this, pl->pos.x, pl->pos.y,
							   PLAYER_SHOT_THEMSELF_SOUND);
					strcat(msg, "  How strange!");
					sc = Rate(0, pl->score) * options.selfKillScoreMult->GetDouble();
				} else {
					players[killer]->kills++;
					sc = Rate(players[killer]->score, pl->score);
				}
			}
			switch (obj->type)
			{
			case OBJ_SHOT:
				if (BIT(obj->mods.warhead, CLUSTER)) {
					factor = options.clusterKillScoreMult->GetDouble();
				} else {
					factor = options.shotKillScoreMult->GetDouble();
				}
				st = ScoreShot;
				break;
			case OBJ_TORPEDO:
				factor = options.torpedoKillScoreMult->GetDouble();
				st = ScoreTorpedo;
				break;
			case OBJ_SMART_SHOT:
				factor = options.smartKillScoreMult->GetDouble();
				st = ScoreSmart;
				break;
			case OBJ_HEAT_SHOT:
				factor = options.heatKillScoreMult->GetDouble();
				st = ScoreHeat;
				break;
			default:
				factor = options.shotKillScoreMult->GetDouble();
				break;
			}
			sc *= factor;
			if (BIT(obj->status, FROMCANNON)) {
				SCORE(pl, -sc,
					OBJ_X_IN_BLOCKS(pl),
					OBJ_Y_IN_BLOCKS(pl),
					"Cannon");
				ScoreServerScoreEvent(s_brCannon, PlayerNone, 0, pl, -sc, st);

				if (BIT(rules->mode, TEAM_PLAY)
					&& pl->team != obj->team)
				{
					TEAM_SCORE(this, obj->team, sc);
				}
			}
			else if (obj->id == NO_ID || killer == ind) {
				PCSTR killername = obj->id == NO_ID ? "" : pl->name;
				SCORE(pl, -sc,
					OBJ_X_IN_BLOCKS(pl),
					OBJ_Y_IN_BLOCKS(pl),
					killername);
				ScoreServerScoreEvent(killername, PlayerNone, 0, pl, -sc, st);
			}
			else {
				if (options.tagGame->GetBool())
					sc = HandleTag(sc, ind, killer);

				Score_players(players[killer], sc, pl->name,
							  pl, -sc, players[killer]->name, st);
				Robot_war(this, ind, killer);
			}

			BroadcastPlayMessage(msg);
			SET_BIT(pl->status, KILLED);
			return;

		default:
			break;
		}
    }
}


void World::PlayerPassCheckpoint(int ind)
{
    Player*	pl = players[ind];
    int		j;

    if (pl->check == 0) {
	pl->round++;
	pl->last_lap_time = pl->time - pl->last_lap;
	if ((pl->best_lap > pl->last_lap_time
	     || pl->best_lap == 0)
	    && pl->time != 0
	    && pl->round != 1) {
	    pl->best_lap = pl->last_lap_time;
	}
	pl->last_lap = pl->time;
	if (pl->round > options.raceLaps->GetInt()) {
	    if (options.ballRace->GetBool()) {
		/* Balls are made unowned when their owner finishes the race
		   This way, they can be reused by other players */
		for (j = 0; j < numObjs; j++) {
		    if (objs[j]->type == OBJ_BALL) {
			BallObject* ball = BALL_PTR(objs[j]);

			if (ball->owner == pl->id)
			    ball->owner = NO_ID;
		    }
		}
	    }
	    pl->DeathReset();
	    pl->mychar = 'D';
	    SET_BIT(pl->status, GAME_OVER|FINISH);
	    sprintf(msg,
		    "%s finished the race. Last lap time: %.2fs. "
		    "Personal race best lap time: %.2fs.",
		    pl->name,
		    (DFLOAT) pl->last_lap_time / GetFPS(),
		    (DFLOAT) pl->best_lap / GetFPS());
	} else if (pl->round > 1) {
	    sprintf(msg,
		    "%s completes lap %d in %.2fs. "
		    "Personal race best lap time: %.2fs.",
		    pl->name,
		    pl->round-1,
		    (DFLOAT) pl->last_lap_time / GetFPS(),
		    (DFLOAT) pl->best_lap / GetFPS());
	} else {
	    sprintf(msg, "%s starts lap 1 of %d", pl->name,
			options.raceLaps->GetInt());
	}
	BroadcastPlayMessage(msg);
    }

    if (++pl->check == numChecks)
	pl->check = 0;
    pl->last_check_dir = pl->dir;

    updateScores = true;
}


void World::AsteroidCollision(void)
{
    int		j, radius, obj_count;
    Object	*ast;
    Object	*obj = NULL, **obj_list;
    list_t	list;
    list_iter_t	iter;
    DFLOAT	damage = 0;
    bool	sound = false;

    list = Asteroid_get_list();
    if (!list) {
	return;
    }

    for (iter = List_begin(list); iter != List_end(list); LI_FORWARD(iter)) {
	ast = (Object*)LI_DATA(iter);

	assert(BIT(ast->type, OBJ_ASTEROID));

	if (ast->life <= 0) {
	    continue;
	}

	assert(OBJ_X_IN_BLOCKS(ast) >= 0);
	assert(OBJ_X_IN_BLOCKS(ast) < blockWidth);
	assert(OBJ_Y_IN_BLOCKS(ast) >= 0);
	assert(OBJ_Y_IN_BLOCKS(ast) < blockHeight);

	CellGetObjects(OBJ_X_IN_BLOCKS(ast), OBJ_Y_IN_BLOCKS(ast),
			 ast->pl_radius / BLOCK_SZ + 1, 300,
			 &obj_list, &obj_count);

	for (j = 0; j < obj_count; j++) {
	    obj = obj_list[j];
	    assert(obj != NULL);
	    if (obj->life <= 0)
		continue;

	    /* asteroids don't hit these objects */
	    if (BIT(obj->type, OBJ_ITEM|OBJ_DEBRIS|OBJ_SPARK|OBJ_WRECKAGE)
		&& obj->id == NO_ID
		&& !BIT(obj->status, FROMCANNON))
		continue;
	    /* don't collide while still overlapping  after breaking */
	    if (obj->type == OBJ_ASTEROID && ast->life > ast->fuselife)
		continue;
	    /* don't collide with self */
	    if (obj == ast)
		continue;
		/* don't collide with phased balls */
		if (BIT(obj->type, OBJ_BALL)
			&& obj->id != NO_ID
			&& BIT(players[getInd[obj->id]]->used, HAS_PHASING_DEVICE))
		{
			continue;
		}

	    radius = ast->pl_radius + obj->pl_radius;
	    if (!InRangeAcd(ast->prevpos.x, ast->prevpos.y,
			      ast->pos.x, ast->pos.y,
			      obj->prevpos.x, obj->prevpos.y,
			      obj->pos.x, obj->pos.y,
			      radius)) {
		continue;
	    }

	    switch (obj->type) {
	    case OBJ_BALL:
		ObjRepel(ast, obj, radius);
		if (options.treasureCollisionDestroys->GetBool())
		    obj->life = 0;
		damage = ED_BALL_HIT;
		sound = true;
		break;
	    case OBJ_ASTEROID:
		obj->life -= (long)ASTEROID_FUEL_HIT(ABS(2 * ast->mass
					           * VECTOR_LENGTH(ast->vel)),
				               WIRE_PTR(obj)->size);
		damage = -ABS(2 * obj->mass * VECTOR_LENGTH(obj->vel));
		Delta_mv_elastic(ast, obj);
		/* avoid doing collision twice */
		obj->fuselife = obj->life - 1;
		sound = true;
		break;
	    case OBJ_SPARK:
		obj->life = 0;
		Delta_mv(ast, obj);
		damage = 0;
		break;
	    case OBJ_DEBRIS:
	    case OBJ_WRECKAGE:
		obj->life = 0;
		damage = -ABS(2 * obj->mass * VECTOR_LENGTH(obj->vel));
		Delta_mv(ast, obj);
		break;
	    case OBJ_MINE:
		if (!BIT(obj->status, CONFUSED))
		    obj->life = 0;
		break;
	    case OBJ_SHOT:
		case OBJ_CANNON_SHOT:
		obj->life = 0;
		Delta_mv(ast, obj);
		damage = ED_SHOT_HIT;
		sound = true;
		break;
	    case OBJ_SMART_SHOT:
	    case OBJ_TORPEDO:
	    case OBJ_HEAT_SHOT:
		obj->life = 0;
		Delta_mv(ast, obj);
		damage = ED_SMART_SHOT_HIT
			 / ((obj->mods.mini + 1) * (obj->mods.power + 1));
		sound = true;
		break;
	    default:
		Delta_mv(ast, obj);
		damage = 0;
		break;
	    }

	    if (ast->life > 0) {
		if (ast->life <= ast->fuselife) {
		    ast->life += (long)ASTEROID_FUEL_HIT(damage, WIRE_PTR(ast)->size);
		}
		if (sound) {
		    SoundPlaySensors(this, ast->pos.x, ast->pos.y,
				       ASTEROID_HIT_SOUND);
		}
		if (ast->life < 0) {
		    ast->life = 0;
		}
		if (ast->life == 0) {
		    if (options.asteroidPoints->GetBool()
			&& (obj->id != NO_ID
			    || (obj->type == OBJ_BALL
				&& BALL_PTR(obj)->owner != NO_ID))) {
			int owner_id = ((obj->type == OBJ_BALL)
					? BALL_PTR(obj)->owner
					: obj->id);
			int ind = getInd[owner_id];
			if (players[ind]->score <= options.asteroidMaxScore->GetDouble())
			{
				SCORE(players[ind], options.asteroidPoints->GetDouble(),
					  OBJ_X_IN_BLOCKS(ast),
					  OBJ_Y_IN_BLOCKS(ast),
					  "");
				ScoreServerScoreEvent(players[ind], options.asteroidPoints->GetDouble(),
					s_brAsteroid, PlayerNone, 0, ScoreAsteroid);
		    }
			}
		    /* break; */
		}
	    }
	}
    }
}


/* do ball - object and ball - checkpoint collisions */
void World::BallCollision()
{
    int         i, j, obj_count;
    int			ignored_object_types;
    Object**	obj_list;
    Object*		obj;
    BallObject*	ball;

    /*
     * These object types ignored;
     * some are handled by other code,
     * some don't interact.
     */
    ignored_object_types = OBJ_PLAYER | OBJ_ASTEROID | OBJ_MINE | OBJ_ITEM;
    if (!options.ballSparkCollisions->GetBool()) {
	ignored_object_types |= OBJ_SPARK;
    }

    for (i = 0; i < numObjs; i++) {
	ball = BALL_IND(this, i);

	/* ignore if: */
	if (ball->type != OBJ_BALL ||	/* not a ball */
	    ball->life <= 0 ||		/* dying ball */
	    (ball->id != NO_ID
	     && BIT(players[getInd[ball->id]]->used, HAS_PHASING_DEVICE)) ||
					/* phased ball */
	    treasures[ball->treasure].have) {
					/* safe in a treasure */
	    continue;
	}

	/* Ball - checkpoint */
	if (BIT(rules->mode, TIMING)
	    && options.ballRace->GetBool()
	    && ball->owner != NO_ID) {
	    int owner_ind = getInd[ball->owner];
	    Player *owner = players[owner_ind];

	    if (!options.ballRaceConnected->GetBool()
			|| ball->id == owner->id) {
		if (WrapLength(ball->pos.x
				 - check[owner->check].x * BLOCK_SZ,
				ball->pos.y
				 - check[owner->check].y * BLOCK_SZ)
		    < options.checkpointRadius->GetDouble() * BLOCK_SZ) {
		    PlayerPassCheckpoint(owner_ind);
		}
	    }
	}

	/* Ball - object */
	if (!options.ballCollisions->GetBool())
	    continue;

	CellGetObjects(OBJ_X_IN_BLOCKS(ball), OBJ_Y_IN_BLOCKS(ball),
			 4, 300,
			 &obj_list, &obj_count);

	for (j = 0; j < obj_count; j++) {
	    obj = obj_list[j];

	    if (BIT(obj->type, ignored_object_types))
		continue;

	    if (obj->life <= 0)
		continue;

	    /* have we already done this ball pair? */
	    if (obj->type == OBJ_BALL && obj <= OBJ_PTR(ball)) {
		continue;
	    }

	    if (!InRangeAcd(ball->prevpos.x, ball->prevpos.y,
			      ball->pos.x, ball->pos.y,
			      obj->prevpos.x, obj->prevpos.y,
			      obj->pos.x, obj->pos.y,
			      ball->pl_radius + obj->pl_radius)) {
		continue;
	    }

	    /* bang! */

	    switch (obj->type) {
	    case OBJ_BALL:
		/* Balls bounce off other balls that aren't safe in
		 * the treasure: */
		{
		    BallObject *b2 = BALL_PTR(obj);
		    if (treasures[b2->treasure].have) {
			break;
		    }
		    if (b2->id != NO_ID
			&& BIT(players[getInd[b2->id]]->used, HAS_PHASING_DEVICE)) {
			break;
		    }
		}

		/* if the collision was too violent, destroy ball and object */
		if ((sqr(ball->vel.x - obj->vel.x) +
		     sqr(ball->vel.y - obj->vel.y)) >
		    sqr(options.maxObjectWallBounceSpeed->GetDouble())) {
		    ball->life = 0;
		    obj->life  = 0;
		} else {
		    /* they bounce */
		    ObjRepel((Object*)ball, obj,
			      ball->pl_radius + obj->pl_radius);
		}
		break;

	    /* balls absorb and destroy all other objects: */
	    case OBJ_SPARK:
	    case OBJ_TORPEDO:
	    case OBJ_SMART_SHOT:
	    case OBJ_HEAT_SHOT:
	    case OBJ_SHOT:
	    case OBJ_CANNON_SHOT:
	    case OBJ_DEBRIS:
	    case OBJ_WRECKAGE:
		Delta_mv(OBJ_PTR(ball), obj);
		obj->life = 0;
		break;
	    }
	}
    }
}


/* do mine - object collisions */
void World::MineCollision()
{
    int			i, j, obj_count;
    Object**	obj_list;
    Object*		obj;
    MineObject*	mine;
    int			collide_object_types;

    if (!options.mineShotDetonateDistance->GetBool())
	return;

    /*
     * These object types ignored;
     * some are handled by other code,
     * some don't interact.
     */
    collide_object_types = OBJ_SHOT |
			   OBJ_TORPEDO |
			   OBJ_SMART_SHOT |
			   OBJ_HEAT_SHOT |
			   OBJ_CANNON_SHOT;

    for (i = 0; i < numObjs; i++) {
	mine = MINE_IND(this, i);

	/* ignore if: */
	if (mine->type != OBJ_MINE ||	/* not a mine */
	    mine->life <= 0) {		/* dying mine */
	    continue;
	}

	CellGetObjects(OBJ_X_IN_BLOCKS(mine), OBJ_Y_IN_BLOCKS(mine),
			 4, 300,
			 &obj_list, &obj_count);

	for (j = 0; j < obj_count; j++) {
	    obj = obj_list[j];

	    if (!BIT(obj->type, collide_object_types))
		continue;

	    if (obj->life <= 0)
		continue;

	    if (!InRangeAcd(mine->prevpos.x, mine->prevpos.y,
			      mine->pos.x, mine->pos.y,
			      obj->prevpos.x, obj->prevpos.y,
			      obj->pos.x, obj->pos.y,
			      options.mineShotDetonateDistance->GetInt()
					+ obj->pl_radius)) {
		continue;
	    }

	    /* bang! */
	    obj->life = 0;
	    mine->life = 0;
	    break;
	}
    }
}


void World::ObjRepel(Object* obj1, Object* obj2, int repel_dist)
{
	DFLOAT				xd, yd,
						force, dm,
						dvx1, dvy1,
						dvx2, dvy2;
	int 				obj_theta;

	xd = WRAP_DX(this, obj2->pos.x - obj1->pos.x);
	yd = WRAP_DY(this, obj2->pos.y - obj1->pos.y);
	force = (repel_dist - LENGTH(xd, yd));

	if (force <= 0)
		return;

	force = MIN(force, 10);

	obj_theta = (int)FindDir(xd, yd);

	dm = obj1->mass / obj2->mass;
	dvx2 = tcos(obj_theta) * force * dm;
	dvy2 = tsin(obj_theta) * force * dm;

	dvx1 = -(tcos(obj_theta) * force / dm);
	dvy1 = -(tsin(obj_theta) * force / dm);

	if (obj1->type == OBJ_PLAYER && obj2->id != NO_ID) {
		Player *pl = (Player *)obj1;
		Player *pusher = players[getInd[obj2->id]];
		if (pusher != pl) {
			Record_shove(pl, pusher, frame_loops);
		}
	}

	if (obj2->type == OBJ_PLAYER && obj1->id != NO_ID) {
		Player *pl = (Player *)obj2;
		Player *pusher = players[getInd[obj1->id]];
		if (pusher != pl) {
			Record_shove(pl, pusher, frame_loops);
		}
	}

	obj1->vel.x += dvx1;
	obj1->vel.y += dvy1;

	obj2->vel.x += dvx2;
	obj2->vel.y += dvy2;
}

