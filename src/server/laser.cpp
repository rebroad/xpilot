/* $Id: laser.cpp,v 1.21 2004/04/29 16:08:27 dick Exp $
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
 *  $Log: laser.cpp,v $
 *  Revision 1.21  2004/04/29 16:08:27  dick
 *  compile with SOUND enabled.
 *
 *  Revision 1.20  2004/02/23 02:06:09  dick
 *  robot.h becomes Robot.h
 *
 *  Revision 1.19  2004/02/20 06:27:47  dick
 *  SendPlayMessage becomes BroadcastPlayMessage() because it sends a play message
 *  to all connections.
 *
 *  Revision 1.18  2004/02/05 14:56:58  cppc
 *  First cut at integrating 'it' play mode.
 *
 *  Revision 1.17  2004/02/03 03:39:54  dick
 *  Add Windows precompiled header support.
 *
 *  Revision 1.16  2004/01/08 18:10:51  dick
 *  Update to XPilot 4.5.5beta
 *
 *  Revision 1.15  2002/09/11 16:42:04  dick
 *  Here endeth the server encapsulation task.
 *  theWorld is now an array of World(s) and get loaded dynamically.
 *
 *  Revision 1.14  2002/09/01 00:13:46  dick
 *  Remove obsolete windows #include "winServer.h"
 *
 *  Revision 1.13  2002/07/14 19:05:24  dick
 *  When talking to the ScoreServer about PCSTR nick, always mention the PlayerType too.
 *
 *  Revision 1.12  2002/06/30 07:00:44  dick
 *  Sprinkle ScoreServerScoreEvent()s around for every scoring event.
 *
 *  Revision 1.11  2002/06/25 05:04:24  dick
 *  Score_players takes an additional parameter: the ScoreType (describe the kill)
 *
 *  Revision 1.10  2002/06/01 06:06:48  dick
 *  Encapsulate (almost) everything.  Get rid of (almost) all refs to theWorld.
 *
 *  Revision 1.9  2002/05/22 05:09:01  dick
 *  Update to XPilot r4_5_4alpha.
 *
 *  Revision 1.8  2002/05/18 20:55:39  dick
 *  Update to XPilot-4.5.1
 *
 *  Revision 1.7  2001/08/14 21:37:14  dick
 *  Array Obj[] becomes objs[].  We now have a class called Obj.
 *
 *  Revision 1.6  2001/07/25 21:25:59  dick
 *  float to int warnings
 *
 *  Revision 1.5  2001/07/24 09:04:00  dick
 *  Remove all global options.  Use World.options instead.
 *
 *  Revision 1.4  2001/07/23 21:08:37  dick
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
#include <math.h>
#include <limits.h>

#include "version.h"
#include "config.h"
#include "serverconst.h"
#include "PacketCtl.h"
#include "list.h"
#include "global.h"
#include "proto.h"
#include "score.h"
#include "saudio.h"
#include "error.h"
#include "portability.h"
#include "objpos.h"
#include "asteroid.h"
#include "Robot.h"


char laser_version[] = VERSION;


/*
 * Type to hold info about a player
 * which might be hit by a laser pulse.
 */
class Victim
{
public:
    int			ind;		/* player index */
    position		pos;		/* current player position */
    DFLOAT		prev_dist;	/* distance at previous sample */
};


/*
 * Type to hold info about all players
 * which may be hit by a laser pulse.
 */
class Vicbuf
{
public:
    int			num_vic;	/* number of victims. */
    int			max_vic;	/* max number */
    Victim		*vic_ptr;	/* pointer to buffer for victims */
};


/*
 * Destroy one laser pulse.
 */
void World::LaserPulseDestroyOne(int pulse_index)
{
	int 				ind;
	Player				*pl;
	Pulse				*pulse_ptr;

	pulse_ptr = pulses[pulse_index];
	if (pulse_ptr->id != NO_ID) {
		ind = getInd[pulse_ptr->id];
		pl = players[ind];
		pl->num_pulses--;
	}

	free(pulse_ptr);

	if (--numPulses > pulse_index) {
		pulses[pulse_index] = pulses[numPulses];
	}
}


/*
 * Destroy all laser pulses.
 */
void World::LaserPulseDestroyAll()
{
	int 				p;

	for (p = numPulses - 1; p >= 0; --p) {
		LaserPulseDestroyOne(p);
	}
}


/*
 * Loop over all players and put the
 * ones which are close the pulse midpoint
 * in a vicbuf structure.
 */
void World::LaserPulseFindVictims(
		Vicbuf* vicbuf,
		Pulse *pulse,
		DFLOAT midx,
		DFLOAT midy)
{
	int 		i;
	Player		*vic;
	DFLOAT		dist;

	vicbuf->num_vic = 0;
	for (i = 0; i < numPlayers; i++) {
		vic = players[i];
		if (BIT(vic->status, PLAYING|GAME_OVER|KILLED|PAUSE)
			!= PLAYING) {
			continue;
		}
		if (BIT(vic->used, HAS_PHASING_DEVICE)) {
			continue;
		}
		if (vic->id == pulse->id
			&& options.selfImmunity->GetBool()) {
			continue;
		}
		if (options.selfImmunity->GetBool() &&
			IS_TANK_PTR(vic) &&
			vic->lock.pl_id == pulse->id) {
			continue;
		}
		if (vic->TeamImmune(pulse->id)) {
			continue;
		}
		/* special case for cannon pulses */
		if (pulse->id == NO_ID
				&& options.teamImmunity->GetBool()
				&& BIT(rules->mode, TEAM_PLAY)
				&& pulse->team == vic->team) {
				continue;
		}
		if (vic->id == pulse->id && !pulse->refl) {
			continue;
		}
		dist = WrapLength(vic->pos.x - midx, vic->pos.y - midy);
		if (dist > pulse->len / 2 + SHIP_SZ) {
			continue;
		}
		if (vicbuf->max_vic == 0) {
			size_t victim_bufsize = numPlayers * sizeof(Victim);
			vicbuf->vic_ptr = (Victim *) malloc(victim_bufsize);
			if (vicbuf->vic_ptr == NULL) {
				break;
			}
			vicbuf->max_vic = numPlayers;
		}
		vicbuf->vic_ptr[vicbuf->num_vic].ind = i;
		vicbuf->vic_ptr[vicbuf->num_vic].pos.x = vic->pos.x;
		vicbuf->vic_ptr[vicbuf->num_vic].pos.y = vic->pos.y;
		vicbuf->vic_ptr[vicbuf->num_vic].prev_dist = 1e10;
		vicbuf->num_vic++;
	}
}


/*
 * Do what needs to be done when a laser pulse
 * actually hits a player.
 * If the pulse was reflected by a mirror
 * then set "refl" to true.
 */
void World::LaserPulseHitsPlayer(
		Pulse *pulse,
		Object *obj,
		DFLOAT x,
		DFLOAT y,
		Victim *victim,
		bool *refl)
{
	Player				*pl;
	Player				*vicpl;
	int 				ind;
	DFLOAT				sc;
	char				msg[MSG_LEN];

	if (pulse->id != NO_ID) {
		ind = getInd[pulse->id];
		pl = players[ind];
	} else {
		ind = -1;
		pl = NULL;
	}

	vicpl = players[victim->ind];
	vicpl->forceVisible++;
	if (BIT(vicpl->have, HAS_MIRROR)
		&& (rfrac() * (2 * vicpl->item[ITEM_MIRROR])) >= 1) {
		pulse->pos.x = x - tcos(pulse->dir) * 0.5
							* PULSE_SAMPLE_DISTANCE;
		pulse->pos.y = y - tsin(pulse->dir) * 0.5
							* PULSE_SAMPLE_DISTANCE;
		pulse->dir = (int)WrapFindDir(vicpl->pos.x - pulse->pos.x,
								  vicpl->pos.y - pulse->pos.y)
					 * 2 - RES / 2 - pulse->dir;
		pulse->dir = MOD2(pulse->dir, RES);
		pulse->life += vicpl->item[ITEM_MIRROR];
		pulse->len = PULSE_LENGTH;
		pulse->refl = true;
		*refl = true;
		return;
	}

	SoundPlaySensors(this, vicpl->pos.x, vicpl->pos.y,
					   PLAYER_EAT_LASER_SOUND);
	if (BIT(vicpl->used, (HAS_SHIELD|HAS_EMERGENCY_SHIELD))
		== (HAS_SHIELD|HAS_EMERGENCY_SHIELD))
		return;
	if (!BIT(obj->type, KILLING_SHOTS))
		return;
	if (BIT(pulse->mods.laser, STUN)
		|| (options.laserIsStunGun->GetBool() == true
			&& options.allowLaserModifiers->GetBool() == false)) {
		if (BIT(vicpl->used, HAS_SHIELD|HAS_LASER|HAS_SHOT)
			|| BIT(vicpl->status, THRUSTING)) {
			if (pl) {
				sprintf(msg,
					"%s got paralysed by %s's stun laser.",
					vicpl->name, pl->name);
				if (vicpl->id == pl->id)
					strcat(msg, " How strange!");
			} else {
				sprintf(msg,
					"%s got paralysed by a stun laser.",
					vicpl->name);
			}
			BroadcastPlayMessage(msg);
			CLR_BIT(vicpl->used,
					HAS_SHIELD|HAS_LASER|OBJ_SHOT);
			CLR_BIT(vicpl->status, THRUSTING);
			vicpl->stunned += 5;
		}
	} else if (BIT(pulse->mods.laser, BLIND)) {
		vicpl->damaged += (GetFPS() + 6);
		vicpl->forceVisible += (GetFPS() + 6);
		if (pl)
			Record_shove(vicpl, pl, frame_loops + GetFPS() + 6);
	} else {
		Add_fuel(&(vicpl->fuel), (long)ED_LASER_HIT);
		if (!BIT(vicpl->used, HAS_SHIELD)
			&& !BIT(vicpl->have, HAS_ARMOR)) {
			SET_BIT(vicpl->status, KILLED);
			if (pl) {
				sprintf(msg,
					"%s got roasted alive by %s's laser.",
					vicpl->name, pl->name);
				if (vicpl->id == pl->id) {
						sc = Rate(0, pl->score)
										* options.laserKillScoreMult->GetDouble()
										* options.selfKillScoreMult->GetDouble();
					SCORE(vicpl, -sc,
						  OBJ_X_IN_BLOCKS(vicpl),
						  OBJ_Y_IN_BLOCKS(vicpl),
						  vicpl->name);
					strcat(msg, " How strange!");
						ScoreServerScoreEvent(pl, 0, pl, -sc, ScoreLaser);
				} else {
					sc = Rate(pl->score, vicpl->score)
										* options.laserKillScoreMult->GetDouble();
					if (options.tagGame->GetBool())
						sc = HandleTag(sc, victim->ind, ind);
					Score_players(pl, sc, vicpl->name,
												  vicpl, -sc, pl->name, ScoreLaser);
				}
			} else {
				sc = Rate(CANNON_SCORE, vicpl->score) / 4;
				SCORE(vicpl, -sc,
					  OBJ_X_IN_BLOCKS(vicpl),
					  OBJ_Y_IN_BLOCKS(vicpl),
					  "Cannon");
				ScoreServerScoreEvent(s_brCannon, PlayerNone, 0, pl, -sc, ScoreLaser);
				if (BIT(rules->mode, TEAM_PLAY)
						&& vicpl->team != pulse->team)
				{
						TEAM_SCORE(this, pulse->team, sc);
				}
				sprintf(msg,
					"%s got roasted alive by cannonfire.",
					vicpl->name);
			}
			SoundPlaySensors(this, vicpl->pos.x, vicpl->pos.y,
							   PLAYER_ROASTED_SOUND);
			BroadcastPlayMessage(msg);
			if (pl && pl->id != vicpl->id) {
				pl->kills++;
				Robot_war(this, victim->ind, ind);
			}
		}
		if (!BIT(vicpl->used, HAS_SHIELD)
			&& BIT(vicpl->have, HAS_ARMOR)) {
			vicpl->HitArmor();
		}
	}
}


/*
 * Check a given pulse position against a list of players.
 * Do what needs to be done when on any pulse hits player event.
 * Return the number of hits.
 * When the pulse was reflected then "refl" will have been set to true.
 */
int World::LaserPulseCheckPlayerHits(
					Pulse *pulse,
					Object *obj,
					DFLOAT x,
					DFLOAT y,
					Vicbuf *vicbuf,
					bool *refl)
{
	int 				j;
	int 				hits = 0;
	int 				ind;
	DFLOAT				dist;
	Player				*pl;
	Victim				*victim;

	if (pulse->id != NO_ID) {
		ind = getInd[pulse->id];
		pl = players[ind];
	} else {
		ind = -1;
		pl = NULL;
	}

	for (j = vicbuf->num_vic - 1; j >= 0; --j) {
		victim = &(vicbuf->vic_ptr[j]);
		dist = WrapLength(x - victim->pos.x,
						   y - victim->pos.y);
		if (dist <= SHIP_SZ) {
			LaserPulseHitsPlayer(
						pulse,
						obj,
						x, y,
						victim,
						refl);
			hits++;
			/* stop at the first hit. */
			break;
		}
		else if (dist >= victim->prev_dist) {
			/* remove victim by copying the last victim over it */
			vicbuf->vic_ptr[j] = vicbuf->vic_ptr[--vicbuf->num_vic];
		} else {
			/* remember shortest distance from pulse to player */
			vicbuf->vic_ptr[j].prev_dist = dist;
		}
	}

	return hits;
}


list_t World::LaserPulseGetObjectList(
		list_t input_obj_list,
		Pulse *pulse,
		DFLOAT midx,
		DFLOAT midy)
{
	list_t				output_obj_list;
	list_t				ast_list;
	register DFLOAT 	dx, dy;
	int 				range;
	list_iter_t 		iter;
	Object				*ast;

	if (input_obj_list != NULL) {
		List_clear(input_obj_list);
	}
	output_obj_list = input_obj_list;

	ast_list = Asteroid_get_list();
	if (ast_list != NULL) {
		if (output_obj_list == NULL) {
			output_obj_list = List_new();
		}
		if (output_obj_list != NULL) {
			/* fill list with interesting objects
			 * which are close to our pulse. */
			for (iter = List_begin(ast_list);
				 iter != List_end(ast_list);
				 LI_FORWARD(iter))
			{
				ast = (Object *) LI_DATA(iter);
				dx = midx - ast->pos.x;
				dy = midy - ast->pos.y;
				dx = WRAP_DX(this, dx);
				dy = WRAP_DY(this, dy);
				range = ast->pl_radius + pulse->len / 2;
				if (sqr(dx) + sqr(dy) < sqr(range)) {
					List_push_back(output_obj_list, ast);
				}
			}
		}
	}

	return output_obj_list;
}


/*
 * For all existing laser pulse check
 * if they collide with ships or asteroids.
 */
void World::LaserPulseCollision()
{
	int 						ind, i;
	int 						p;
	int 						max;
		int 							hits;
	int 						objnum = -1;
	bool						refl;
	Vicbuf						vicbuf;
	DFLOAT						x, y, x1, x2, y1, y2;
	DFLOAT						dx, dy;
	DFLOAT						midx, midy;
	Player						*pl;
	Pulse						*pulse;
	Object						*obj = NULL, *ast = NULL;
	list_t						obj_list = NULL;
	list_iter_t 				iter;

	/*
	 * Allocate one object with which we will
	 * do pulse wall bounce checking.
	 */
	if ((obj = Object_allocate(this)) == NULL) {
		/* overload.  we can't do bounce checking. */
		LaserPulseDestroyAll();
		return;
	}

	/* init vicbuf */
	vicbuf.num_vic = 0;
	vicbuf.max_vic = 0;
	vicbuf.vic_ptr = NULL;

	for (p = numPulses - 1; p >= 0; --p) {
		pulse = pulses[p];

		/* check for end of pulse life */
		if (--pulse->life < 0 || pulse->len < PULSE_LENGTH) {
			LaserPulseDestroyOne(p);
			continue;
		}

		if (pulse->id != NO_ID) {
			ind = getInd[pulse->id];
			pl = players[ind];
		} else {
			ind = -1;
			pl = NULL;
		}

		pulse->pos.x += tcos(pulse->dir) * PULSE_SPEED;
		pulse->pos.y += tsin(pulse->dir) * PULSE_SPEED;
		if (BIT(rules->mode, WRAP_PLAY)) {
			if (pulse->pos.x < 0) {
				pulse->pos.x += width;
			}
			else if (pulse->pos.x >= width) {
				pulse->pos.x -= width;
			}
			if (pulse->pos.y < 0) {
				pulse->pos.y += height;
			}
			else if (pulse->pos.y >= height) {
				pulse->pos.y -= height;
			}
			x1 = pulse->pos.x;
			y1 = pulse->pos.y;
			x2 = x1 + tcos(pulse->dir) * pulse->len;
			y2 = y1 + tsin(pulse->dir) * pulse->len;
		} else {
			x1 = pulse->pos.x;
			y1 = pulse->pos.y;
			if (x1 < 0 || x1 >= width
				|| y1 < 0 || y1 >= height) {
				pulse->len = 0;
				continue;
			}
			x2 = x1 + tcos(pulse->dir) * pulse->len;
			if (x2 < 0) {
				pulse->len = (int)(pulse->len * (0 - x1) / (x2 - x1));
				x2 = x1 + tcos(pulse->dir) * pulse->len;
			}
			if (x2 >= width) {
				pulse->len = (int)(pulse->len * (width - 1 - x1)
					/ (x2 - x1));
				x2 = x1 + tcos(pulse->dir) * pulse->len;
			}
			y2 = y1 + tsin(pulse->dir) * pulse->len;
			if (y2 < 0) {
				pulse->len = (int)(pulse->len * (0 - y1) / (y2 - y1));
				x2 = x1 + tcos(pulse->dir) * pulse->len;
				y2 = y1 + tsin(pulse->dir) * pulse->len;
			}
			if (y2 > height) {
				pulse->len = (int)(pulse->len * (height - 1 - y1)
					/ (y2 - y1));
				x2 = x1 + tcos(pulse->dir) * pulse->len;
				y2 = y1 + tsin(pulse->dir) * pulse->len;
			}
			if (pulse->len <= 0) {
				pulse->len = 0;
				continue;
			}
		}

		/* calculate delta x and y for pulse start and end position. */
		dx = x2 - x1;
		dy = y2 - y1;
		dx = WRAP_DX(this, dx);
		dy = WRAP_DY(this, dy);

		/* max is the highest absolute delta length of either x or y. */
		max = (int)MAX(ABS(dx), ABS(dy));
		if (max == 0) {
			continue;
		}

		/* calculate the midpoint of the new laser pulse position. */
		midx = x1 + (dx * 0.5);
		midy = y1 + (dy * 0.5);
		midx = WRAP_XPIXEL(this, midx);
		midy = WRAP_YPIXEL(this, midy);

		if (roundDelay == 0) {
			/* assemble a shortlist of players which might get hit. */
			LaserPulseFindVictims(&vicbuf, pulse, midx, midy);
		}

		obj_list = LaserPulseGetObjectList(obj_list, pulse, midx, midy);

		obj->type = OBJ_PULSE;
		obj->life = 1;
		obj->id = pulse->id;
		obj->team = pulse->team;
		obj->count = 0;
		obj->status = 0;
		if (pulse->id == NO_ID) {
			obj->status = FROMCANNON;
		}
		Object_position_init_pixels(obj, x1, y1);

		refl = false;

		for (i = hits = 0; i <= max; i += PULSE_SAMPLE_DISTANCE) {
			x = x1 + (i * dx) / max;
			y = y1 + (i * dy) / max;
			obj->vel.x = (x - CLICK_TO_FLOAT(obj->pos.cx));
			obj->vel.y = (y - CLICK_TO_FLOAT(obj->pos.cy));
			/* changed from = x - obj->pos.x to make lasers disappear
			   less frequently when wrapping. There's still a small
			   chance of it happening though. */
			MoveObject(obj);
			if (obj->life == 0) {
				break;
			}
			if (BIT(rules->mode, WRAP_PLAY)) {
				if (x < 0) {
					x += width;
					x1 += width;
				}
				else if (x >= width) {
					x -= width;
					x1 -= width;
				}
				if (y < 0) {
					y += height;
					y1 += height;
				}
				else if (y >= height) {
					y -= height;
					y1 -= height;
				}
			}

			/* check for collision with objects. */
			if (obj_list != NULL) {
				for (iter = List_begin(obj_list);
					 iter != List_end(obj_list);
					 LI_FORWARD(iter))
				{
					DFLOAT adx, ady;
					ast = (Object*)LI_DATA(iter);
					adx = x - ast->pos.x;
					ady = y - ast->pos.y;
					adx = WRAP_DX(this, adx);
					ady = WRAP_DX(this, ady);
					if (sqr(adx) + sqr(ady) <= sqr(ast->pl_radius)) {
						obj->life = 0;
						ast->life += (long)ASTEROID_FUEL_HIT(ED_LASER_HIT,
													   WIRE_PTR(ast)->size);
						if (ast->life < 0)
							ast->life = 0;
						if (ast->life == 0
							&& ind != -1
							&& options.asteroidPoints->GetBool()
								&& players[ind]->score <= options.asteroidMaxScore->GetDouble())
						{
							SCORE(players[ind], options.asteroidPoints->GetDouble(),
								  OBJ_X_IN_BLOCKS(ast), OBJ_Y_IN_BLOCKS(ast),
								  "");
								ScoreServerScoreEvent(players[ind], options.asteroidPoints->GetDouble(),
										s_brAsteroid, PlayerNone, 0, ScoreAsteroid);
						}
						break;
					}
				}
			}

			if (obj->life == 0) {
				/* pulse hit asteroid */
				continue;
			}

			if (roundDelay > 0) {
				/* at round delay no hits are possible */
				continue;
			}

			hits = LaserPulseCheckPlayerHits(
							pulse, obj,
							x, y,
							&vicbuf,
							&refl);

			if (hits > 0) {
				break;
			}
		}

		if (i < max && refl == false) {
			pulse->len = (pulse->len * i) / max;
		}
	}
	if (vicbuf.max_vic > 0 && vicbuf.vic_ptr != NULL) {
		free(vicbuf.vic_ptr);
	}
		obj->type = OBJ_DEBRIS;
		obj->life = 0;
		CellAddObject(obj);

	if (obj_list != NULL) {
		List_delete(obj_list);
	}
}

