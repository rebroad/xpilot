/* $Id: update.cpp,v 1.36 2005/03/17 22:12:14 kps Exp $
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
 *  $Log: update.cpp,v $
 *  Revision 1.36  2005/03/17 22:12:14  kps
 *  Get rid of warnings from makedepend about "non-portable whitespace".
 *
 *  Revision 1.35  2004/05/04 21:07:17  dick
 *  Need randommnt.h
 *
 *  Revision 1.34  2004/04/29 16:08:27  dick
 *  compile with SOUND enabled.
 *
 *  Revision 1.33  2004/02/23 02:06:09  dick
 *  robot.h becomes Robot.h
 *
 *  Revision 1.32  2004/02/20 06:27:47  dick
 *  SendPlayMessage becomes BroadcastPlayMessage() because it sends a play message
 *  to all connections.
 *
 *  Revision 1.31  2004/02/06 21:22:04  cppc
 *  Removed some ugly debug stuff...
 *
 *  Revision 1.30  2004/02/06 21:14:41  cppc
 *  Some changes to 'it'.
 *  * Pausing player loses 'it'.
 *  * Random 'it' selection will not choose paused player.
 *  ** Added World::RandomPlayerWithStatus(mask, bits) to find random player matching status.
 *  * Robots don't leave if they're 'it'.
 *
 *  Revision 1.29  2004/02/05 14:56:58  cppc
 *  First cut at integrating 'it' play mode.
 *
 *  Revision 1.28  2004/02/03 03:39:54  dick
 *  Add Windows precompiled header support.
 *
 *  Revision 1.27  2004/01/26 19:11:29  dick
 *  Fix Bug#37 cloakedExhaust is backward.  Remove extra cloaked check in TurnThrust()
 *  because it was already filtered.
 *
 *  Revision 1.26  2004/01/08 18:10:51  dick
 *  Update to XPilot 4.5.5beta
 *
 *  Revision 1.25  2002/09/16 21:47:37  dick
 *  Whitespace
 *
 *  Revision 1.24  2002/09/11 16:42:04  dick
 *  Here endeth the server encapsulation task.
 *  theWorld is now an array of World(s) and get loaded dynamically.
 *
 *  Revision 1.23  2002/09/01 00:13:46  dick
 *  Remove obsolete windows #include "winServer.h"
 *
 *  Revision 1.22  2002/08/23 12:49:20  bert
 *  Reduce paused player's score with 0.01 every frame.
 *
 *  Revision 1.21  2002/07/14 19:05:24  dick
 *  When talking to the ScoreServer about PCSTR nick, always mention the PlayerType too.
 *
 *  Revision 1.20  2002/07/11 16:36:03  dick
 *  Update to XPilot-4.5.4beta
 *
 *  Revision 1.19  2002/07/08 05:57:46  dick
 *  Whitespace
 *
 *  Revision 1.18  2002/06/30 06:28:52  dick
 *  Oh woe.  NT server has a crash.  Reindent the main loop so i can follow it,
 *  and add some sanity checks on objs[] to try to narrow down when it gets confused.
 *
 *  Revision 1.17  2002/06/01 06:29:59  dick
 *  cannon.* becomes Cannon.*
 *
 *  Revision 1.16  2002/06/01 06:06:48  dick
 *  Encapsulate (almost) everything.  Get rid of (almost) all refs to theWorld.
 *
 *  Revision 1.15  2002/05/22 16:03:34  dick
 *  Handle valSec for cannonDeadTime and maxPauseTime.
 *
 *  Revision 1.14  2002/05/22 05:09:01  dick
 *  Update to XPilot r4_5_4alpha.
 *
 *  Revision 1.13  2002/05/18 20:55:39  dick
 *  Update to XPilot-4.5.1
 *
 *  Revision 1.12  2001/08/14 21:37:14  dick
 *  Array Obj[] becomes objs[].  We now have a class called Obj.
 *
 *  Revision 1.11  2001/08/05 08:05:36  dick
 *  Add Tuners to the server.  ComputeSensorRange(), AddTank(), RemoveTank()
 *  become members of Player.  ComputeGravity*() become members of World.
 *
 *  Revision 1.10  2001/08/04 17:34:07  dick
 *  TransportToHome and DoAutopilot become members of Player.
 *  RECOVERY_DELAY needs * GetFPS().
 *
 *  Revision 1.9  2001/07/24 09:04:00  dick
 *  Remove all global options.  Use World.options instead.
 *
 *  Revision 1.8  2001/07/23 21:08:37  dick
 *  Encapsulate ShipMass, ShotsMax, ShotsLife, ShotsMass, FPS.
 *
 *  Revision 1.7  2001/07/21 07:21:41  dick
 *  Slay some float to int warnings
 *
 *  Revision 1.6  2001/07/18 20:15:33  dick
 *  The first pass at integrating xpilotedit's options with the server options.
 *  The server does not build yet.  xpilotedit works as before, with the new
 *  definitions.
 *
 *  Revision 1.5  2001/07/12 10:07:36  dick
 *  Go_home becomes GoHome
 *
 *  Revision 1.4  2001/07/10 08:10:08  dick
 *  Don't need (deleted) map.h because World.h is included in global.h
 *
 *  Revision 1.3  2001/07/08 06:40:14  dick
 *  C++ conversion stuff.
 *
 *  Revision 1.2  2001/07/07 12:00:43  dick
 *  Rename classes to C++ "Style".  old World becomes theWorld.
 *
 */
#include "StdAfx.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>

#include "version.h"
#include "config.h"
#include "serverconst.h"
#include "global.h"
#include "proto.h"
#include "score.h"
#include "bit.h"
#include "saudio.h"
#include "objpos.h"
#include "Cannon.h"
#include "asteroid.h"
#include "commonproto.h"
#include "randommt.h"
#include "ConnectionPlayer.h"
#include "Robot.h"

#define TURN_FUEL(acc)          (0.005*FUEL_SCALE_FACT*ABS(acc))
#define TURN_SPARKS(tf)         (5+((tf)>>((FUEL_SCALE_BITS)-6)))


char update_version[] = VERSION;


#define update_object_speed(o_)						\
    if (BIT((o_)->status, GRAVITY)) {					\
	(o_)->vel.x += (o_)->acc.x					\
		    + gravity[(o_)->pos.bx][(o_)->pos.by].x;	\
	(o_)->vel.y += (o_)->acc.y					\
		    + gravity[(o_)->pos.bx][(o_)->pos.by].y;	\
    } else {								\
	(o_)->vel.x += (o_)->acc.x;					\
	(o_)->vel.y += (o_)->acc.y;					\
    }

int	round_delay = 0;	/* delay until start of next round */
//int	round_delay_send = 0;	/* number of frames to send round_delay */
//int	roundtime = -1;		/* time left this round */

static char msg[MSG_LEN];


/*
 * Turn phasing on or off.
 */
void Player::Phasing (bool on)
{
	const int	phasing_time = 4 * world->GetFPS();

	if (on) {
		if (phasing_left <= 0) {
			phasing_left = phasing_time;
			phasing_max = phasing_time;
			item[ITEM_PHASING]--;
		}
		SET_BIT(used, HAS_PHASING_DEVICE);
		CLR_BIT(used, HAS_REFUEL);
		CLR_BIT(used, HAS_REPAIR);
		if (BIT(used, HAS_CONNECTOR))
			ball = NULL;
		CLR_BIT(used, HAS_TRACTOR_BEAM);
		CLR_BIT(status, GRAVITY);
		SoundPlaySensors(world, pos.x, pos.y, PHASING_ON_SOUND);
	} else {
		CLR_BIT(used, HAS_PHASING_DEVICE);
		if (phasing_left<= 0) {
			if (item[ITEM_PHASING] <= 0)
				CLR_BIT(have, HAS_PHASING_DEVICE);
		}
		SET_BIT(status, GRAVITY);
		SoundPlaySensors(world, pos.x, pos.y, PHASING_OFF_SOUND);
	}
}

/*
 * Turn cloak on or off.
 */
void Player::Cloak(bool on)
{

	if (on) {
		if (!BIT(used, HAS_CLOAKING_DEVICE) && item[ITEM_CLOAK] > 0) {
			if (!world->options.cloakedShield->GetBool()) {
				if (BIT(used, HAS_EMERGENCY_SHIELD)) {
					EmergencyShield(false);
				}
				if (BIT(used, HAS_DEFLECTOR)) {
					Deflector(false);
				}
				CLR_BIT(used, HAS_SHIELD);
				CLR_BIT(have, HAS_SHIELD);
			}
			SoundPlayPlayer(this, CLOAK_SOUND);
			updateVisibility = 1;
			SET_BIT(used, HAS_CLOAKING_DEVICE);
		}
	} else {
		if (BIT(used, HAS_CLOAKING_DEVICE)) {
			SoundPlayPlayer(this, CLOAK_SOUND);
			updateVisibility = 1;
			CLR_BIT(used, HAS_CLOAKING_DEVICE);
		}
		if (!item[ITEM_CLOAK]) {
			CLR_BIT(have, HAS_CLOAKING_DEVICE);
		}
		if (!world->options.cloakedShield->GetBool()) {
			if (BIT(have, HAS_EMERGENCY_SHIELD)) {
				SET_BIT(have, HAS_SHIELD);
				EmergencyShield(true);
			}
			if (BIT(DEF_HAVE, HAS_SHIELD) && !BIT(have, HAS_SHIELD)) {
				SET_BIT(have, HAS_SHIELD);
			}
			if (BITV_ISSET(last_keyv, KEY_SHIELD)) {
				SET_BIT(used, HAS_SHIELD);
			}
		}
	}
}

/*
 * Turn deflector on or off.
 */
void Player::Deflector(bool on)
{
	if (on) {
		if (!BIT(used, HAS_DEFLECTOR) && item[ITEM_DEFLECTOR] > 0) {
			/* only allow deflector when cloaked shielding or not cloaked */
			if (!world->options.cloakedShield->GetBool()
				|| !BIT(used, HAS_CLOAKING_DEVICE))
			{
				SET_BIT(used, HAS_DEFLECTOR);
				SoundPlayPlayer(this, DEFLECTOR_SOUND);
			}
		}
	} else {
		if (BIT(used, HAS_DEFLECTOR)) {
			CLR_BIT(used, HAS_DEFLECTOR);
			SoundPlayPlayer(this, DEFLECTOR_SOUND);
		}
		if (!item[ITEM_DEFLECTOR]) {
			CLR_BIT(have, HAS_DEFLECTOR);
		}
	}
}

/*
 * Turn emergency thrust on or off.
 */
void Player::EmergencyThrust(bool on)
{
	const int	emergency_thrust_time = 4 * world->GetFPS();

	if (on) {
		if (emergency_thrust_left <= 0) {
			emergency_thrust_left = emergency_thrust_time;
			emergency_thrust_max = emergency_thrust_time;
			item[ITEM_EMERGENCY_THRUST]--;
		}
		if (!BIT(used, HAS_EMERGENCY_THRUST)) {
			SET_BIT(used, HAS_EMERGENCY_THRUST);
			SoundPlaySensors(world, pos.x, pos.y, EMERGENCY_THRUST_ON_SOUND);
		}
	} else {
		if (BIT(used, HAS_EMERGENCY_THRUST)) {
			CLR_BIT(used, HAS_EMERGENCY_THRUST);
			SoundPlaySensors(world, pos.x, pos.y, EMERGENCY_THRUST_OFF_SOUND);
		}
		if (emergency_thrust_left <= 0) {
			if (item[ITEM_EMERGENCY_THRUST] <= 0)
				CLR_BIT(have, HAS_EMERGENCY_THRUST);
		}
	}
}

/*
 * Turn emergency shield on or off.
 */
void Player::EmergencyShield (bool on)
{
	const int	emergency_shield_time = 4 * world->GetFPS();	/* 8 -> 4 */

	if (on) {
		if (BIT(have, HAS_EMERGENCY_SHIELD)) {
			if (emergency_shield_left <= 0) {
				emergency_shield_left = emergency_shield_time;
				emergency_shield_max = emergency_shield_time;
				item[ITEM_EMERGENCY_SHIELD]--;
			}
			if (world->options.cloakedShield->GetBool()
				|| !BIT(used, HAS_CLOAKING_DEVICE))
			{
				SET_BIT(have, HAS_SHIELD);
				if (!BIT(used, HAS_EMERGENCY_SHIELD)) {
					SET_BIT(used, HAS_EMERGENCY_SHIELD);
					SoundPlaySensors(world, pos.x, pos.y, EMERGENCY_SHIELD_ON_SOUND);
				}

			}
		}
	} else {
		if (emergency_shield_left <= 0) {
			if (item[ITEM_EMERGENCY_SHIELD] <= 0)
				CLR_BIT(have, HAS_EMERGENCY_SHIELD);
		}
		if (!BIT(DEF_HAVE, HAS_SHIELD)) {
			CLR_BIT(have, HAS_SHIELD);
			CLR_BIT(used, HAS_SHIELD);
		}
		if (BIT(used, HAS_EMERGENCY_SHIELD)) {
			CLR_BIT(used, HAS_EMERGENCY_SHIELD);
			SoundPlaySensors(world, pos.x, pos.y, EMERGENCY_SHIELD_OFF_SOUND);
		}
	}
}

/*
 * Turn autopilot on or off.  This always clears the thrusting bit.  During
 * automatic pilot mode any changes to the current power, turnacc, turnspeed
 * and turnresistance settings will be temporary.
 */
void Player::Autopilot(bool on)
{

	CLR_BIT(status, THRUSTING);
	if (on) {
		auto_power_s = power;
		auto_turnspeed_s = turnspeed;
		auto_turnresistance_s = turnresistance;
		SET_BIT(used, HAS_AUTOPILOT);
		power = (MIN_PLAYER_POWER+MAX_PLAYER_POWER)/2.0;
		turnspeed = (MIN_PLAYER_TURNSPEED+MAX_PLAYER_TURNSPEED)/2.0;
		turnresistance = 0.2;
		SoundPlaySensors(world, pos.x, pos.y, AUTOPILOT_ON_SOUND);
	} else {
		power = auto_power_s;
		turnacc = 0.0;
		turnspeed = auto_turnspeed_s;
		turnresistance = auto_turnresistance_s;
		CLR_BIT(used, HAS_AUTOPILOT);
		SoundPlaySensors(world, pos.x, pos.y, AUTOPILOT_OFF_SOUND);
	}
}


/********** **********
 * Updating objects and the like.
 */
void World::UpdateObjects()
{
	int i, j;
	Player *pl;
	Object *obj;

	/*
	* Update robots.
	*/
	Robot_update(this);

	/*
	* Autorepeat fire, must unfortunately be done here, not in
	* the player loop below, because of collisions between the shots
	* and the auto-firing player that would otherwise occur.
	*/
	if (options.fireRepeatRate->GetInt() > 0) {
		for (i = 0; i < numPlayers; i++) {
			pl = players[i];
			if (BIT(pl->used, HAS_SHOT)) {
				pl->FireNormalShots();
			}
		}
	}

	/*
	* Special items.
	*/
	for (i=0; i<NUM_ITEMS; i++)
		if (items[i].num < items[i].max &&
			items[i].chance > 0 &&
			items[i].chance * rfrac() < 1.0f)
		{
		PlaceItem(i, -1);
		}
	D(for (i = numObjs - 1; i >= 0; i--)
	{
		assert(objs[i]->type);
	})

	/*
	* Let the fuel stations regenerate some fuel.
	*/
	if (numPlayers > 0) {
		int fuel = (int)(numPlayers * STATION_REGENERATION);
		int frames_per_update = MAX_STATION_FUEL / (fuel * BLOCK_SZ);
		for (i=0; i<numFuels; i++) {
			if (fuels[i].fuel == MAX_STATION_FUEL) {
				continue;
			}
			if ((fuels[i].fuel += fuel) >= MAX_STATION_FUEL) {
				fuels[i].fuel = MAX_STATION_FUEL;
			}
			else if (fuels[i].last_change + frames_per_update > frame_loops) {
				/*
				* We don't send fuelstation info to the clients every frame
				* if it wouldn't change their display.
				*/
				continue;
			}
			fuels[i].conn_mask = 0;
			fuels[i].last_change = frame_loops;
		}
	}
	D(for (i = numObjs - 1; i >= 0; i--)
	{
		assert(objs[i]->type);
	})

	/*
	 * Update shots.
	 */
	for (i = 0; i < numObjs; i++) {
		obj = objs[i];
		//	xpprintf("i=%d type=%d\n", i, obj->type);
		//	if (i == 344)
		//	   xpprintf("i==88\n");

		if (BIT(obj->type, OBJ_MINE))
		MoveMine(i);

		else if (BIT(obj->type, OBJ_SMART_SHOT|OBJ_HEAT_SHOT|OBJ_TORPEDO))
		MoveMissile(i);

		else if (BIT(obj->type, OBJ_BALL)) {
			if (obj->id != NO_ID)
			MoveBall(i);
		}

		else if (BIT(obj->type, OBJ_WRECKAGE)) {
			WireObject *wireobj = WIRE_PTR(obj);
			wireobj->rotation =
				(wireobj->rotation + (int) (wireobj->turnspeed * RES)) % RES;
		}

		//	update_object_speed(obj);
		if (BIT((obj)->status, GRAVITY)) {
			(obj)->vel.x += (obj)->acc.x
			+ gravity[(obj)->pos.bx][(obj)->pos.by].x;
			(obj)->vel.y += (obj)->acc.y
			+ gravity[(obj)->pos.bx][(obj)->pos.by].y;
		} else {
			(obj)->vel.x += (obj)->acc.x;
			(obj)->vel.y += (obj)->acc.y;
		}

		if (!BIT(obj->type, OBJ_ASTEROID)) {
			MoveObject(obj);
		}
	}

	D(for (i = numObjs - 1; i >= 0; i--)
	{
		assert(objs[i]->type);
	})
    /*
     * Asteroids.
     */
    Asteroid_update(this);

	/*
	 * Update ECM blasts
	 */
	for (i = 0; i < numEcms; i++) {
		if ((ecms[i]->size >>= 1) == 0) {
			if (ecms[i]->id != NO_ID)
				players[getInd[ecms[i]->id]]->ecmcount--;
			free(ecms[i]);
			--numEcms;
			ecms[i] = ecms[numEcms];
			i--;
		}
	}

	/*
	* Update transporters
	*/
	for (i = 0; i < numTransporters; i++) {
		if (--transporters[i]->count <= 0) {
			free(transporters[i]);
			--numTransporters;
			transporters[i] = transporters[numTransporters];
			i--;
		}
	}

	/*
	 * Updating cannons, maybe a little bit of fireworks too?
	 */
	for (i = 0; i < numCannons; i++) {
		Cannon *cannon = cannons + i;
		if (cannon->dead_time > 0) {
			if (!--cannon->dead_time) {
				block[cannon->blk_pos.x][cannon->blk_pos.y] = CANNON;
				cannon->conn_mask = 0;
				cannon->last_change = frame_loops;
			}
			continue;
		} else {
			/* don't check too often, because this gets quite expensive
			on maps with many cannons with defensive items */
			if (options.cannonsUseItems->GetBool()
			&& options.cannonsDefend->GetBool()
			&& rfrac() < 0.65) {
				cannon->CheckDefense();
			}
			if (!BIT(cannon->used, HAS_EMERGENCY_SHIELD)
				&& !BIT(cannon->used, HAS_PHASING_DEVICE)
				&& !cannon->damaged
				&& !cannon->tractor_count
				&& rfrac() * 16 < 1)
			{
				cannon->CheckFire();
			}
			else if (options.cannonsUseItems->GetBool()
					&& options.itemProbMult->GetDouble() > 0
					&& options.cannonItemProbMult->GetDouble() > 0)
			{
				int item = (int)(rfrac() * NUM_ITEMS);
				/* this gives the cannon an item about once every minute */
				if (items[item].cannonprob > 0
					&& options.cannonItemProbMult->GetDouble() > 0
					&& (int)(rfrac() * (60 * GetFPS()))
					< (options.cannonItemProbMult->GetDouble() *
					items[item].cannonprob))
				{
					cannon->AddItem(item, (item == ITEM_FUEL ?
									ENERGY_PACK_FUEL >> FUEL_SCALE_BITS
									: 1));
				}
			}
		}
		if (cannon->damaged > 0) {
			cannon->damaged--;
		}
		if (cannon->tractor_count > 0) {
			int ind = getInd[cannon->tractor_target];
			if (WrapLength(players[ind]->pos.x - cannon->pix_pos.x,
				players[ind]->pos.y - cannon->pix_pos.y)
					< TRACTOR_MAX_RANGE(cannon->items[ITEM_TRACTOR_BEAM])
				&& BIT(players[ind]->status, PLAYING|GAME_OVER|KILLED|PAUSE)
					== PLAYING)
			{
				GeneralTractorBeam(NULL, cannon->pix_pos.x, cannon->pix_pos.y,
				cannon->items[ITEM_TRACTOR_BEAM], ind,
				cannon->tractor_is_pressor);
				cannon->tractor_count--;
			} else {
				cannon->tractor_count = 0;
			}
		}
		if (cannon->emergency_shield_left > 0) {
			if (--cannon->emergency_shield_left <= 0) {
				CLR_BIT(cannon->used, HAS_EMERGENCY_SHIELD);
				SoundPlaySensors(this, cannon->pix_pos.x, cannon->pix_pos.y,
					EMERGENCY_SHIELD_OFF_SOUND);
			}
		}
		if (cannon->phasing_left > 0) {
			if (--cannon->phasing_left <= 0) {
				CLR_BIT(cannon->used, HAS_PHASING_DEVICE);
				SoundPlaySensors(this, cannon->pix_pos.x, cannon->pix_pos.y,
					PHASING_OFF_SOUND);
			}
		}
	}

    /*
     * Update targets
     */
	for (i = 0; i < numTargets; i++) {
		if (targets[i].dead_time > 0) {
			if (!--targets[i].dead_time) {
				block[targets[i].pos.x][targets[i].pos.y] = TARGET;
				targets[i].conn_mask = 0;
				targets[i].update_mask = (unsigned)-1;
				targets[i].last_change = frame_loops;

				if (options.targetSync->GetBool()) {
					unsigned short team = targets[i].team;

					for (j = 0; j < numTargets; j++) {
						if (targets[j].team == team) {
							block[targets[j].pos.x][targets[j].pos.y] = TARGET;
							targets[j].conn_mask = 0;
							targets[j].update_mask = (unsigned)-1;
							targets[j].last_change = frame_loops;
							targets[j].dead_time = 0;
							targets[j].damage = TARGET_DAMAGE;
						}
					}
				}
			}
			continue;
		}
		else if (targets[i].damage == TARGET_DAMAGE) {
			continue;
		}
		targets[i].damage += TARGET_REPAIR_PER_FRAME;
		if (targets[i].damage >= TARGET_DAMAGE) {
			targets[i].damage = TARGET_DAMAGE;
		}
		else if (targets[i].last_change + TARGET_UPDATE_DELAY < frame_loops) {
			/*
			 * We don't send target info to the clients every frame
			 * if the latest repair wouldn't change their display.
			 */
			continue;
		}
		targets[i].conn_mask = 0;
		targets[i].last_change = frame_loops;
	}

    D(for (i = numObjs - 1; i >= 0; i--)
	{
		assert(objs[i]->type);
	})

	/* * * * * *
	 *
	 * Player loop. Computes miscellaneous updates.
	 *
	 */
	for (i=0; i<numPlayers; i++) {
		long tf = 0;

		pl = players[i];

		/* Limits. */
		LIMIT(pl->power, MIN_PLAYER_POWER, MAX_PLAYER_POWER);
		LIMIT(pl->turnspeed, MIN_PLAYER_TURNSPEED, MAX_PLAYER_TURNSPEED);
		LIMIT(pl->turnresistance, MIN_PLAYER_TURNRESISTANCE, MAX_PLAYER_TURNRESISTANCE);

		if (pl->damaged > 0)
		pl->damaged--;

		if (pl->count > 0) {
			pl->count--;
			if (!BIT(pl->status, PLAYING)) {
				pl->TransportToHome();
				MovePlayer(i);
				continue;
			}
		}

		if (pl->count == 0) {
			pl->count = -1;

			if (!BIT(pl->status, PLAYING)) {
				SET_BIT(pl->status, PLAYING);
				pl->GoHome();
			}
			if (BIT(pl->status, SELF_DESTRUCT)) {
				if (options.selfDestructScoreMult->GetBool()) {
					DFLOAT sc = Rate(0, pl->score) * options.selfDestructScoreMult->GetDouble();
					SCORE(pl, -sc,
						OBJ_X_IN_BLOCKS(pl),
						OBJ_Y_IN_BLOCKS(pl),
						"Self-Destruct");
					ScoreServerScoreEvent(s_brSelfDestruct, PlayerNone, 0,
										  pl, -sc, ScoreSelfDestruct);
				}
				SET_BIT(pl->status, KILLED);
				sprintf(msg, "%s has committed suicide.", pl->name);
				BroadcastPlayMessage(msg);
				ThrowItems(i);
				pl->Kill();
				updateScores = true;
			}
		}


		if (BIT(pl->status, PLAYING|GAME_OVER|PAUSE) != PLAYING) {
			if (BIT(pl->status, PAUSE)) {
				/* reduce paused player's score a little. */
				pl->score -= options.pauseScoreReduce->GetDouble();
				/* only update paused score every 4 or 5 seconds. */
				if ((frame_loops & 63) == 0) {
					updateScores = true;
				}
			}
			continue;
		}

		if (round_delay > 0)
			continue;

		if (pl->stunned > 0) {
			pl->stunned--;
			CLR_BIT(pl->used, HAS_SHIELD|HAS_LASER|HAS_SHOT);
			CLR_BIT(pl->status, THRUSTING);
		}

		if (pl->shield_time > 0) {
			if (--pl->shield_time == 0) {
				if (!BIT(pl->used, HAS_EMERGENCY_SHIELD)) {
					CLR_BIT(pl->used, HAS_SHIELD);
				}
			}
			if (BIT(pl->used, HAS_SHIELD) == 0) {
				/* BG 95/06/03: change test on "have" to "used". */
				if (!BIT(pl->used, HAS_EMERGENCY_SHIELD)) {
					CLR_BIT(pl->have, HAS_SHIELD);
				}
				pl->shield_time = 0;
			}
		}

		if (BIT(pl->used, HAS_PHASING_DEVICE)) {
			if (--pl->phasing_left <= 0) {
				if (pl->item[ITEM_PHASING]) {
					pl->Phasing(true);
				} else {
					pl->Phasing(false);
				}
			}
		}

		if (BIT(pl->used, HAS_EMERGENCY_THRUST)) {
			if (pl->fuel.sum > 0
				&& BIT(pl->status, THRUSTING)
				&& --pl->emergency_thrust_left <= 0)
			{
				if (pl->item[ITEM_EMERGENCY_THRUST]) {
					pl->EmergencyThrust(true);
				} else {
					pl->EmergencyThrust(false);
				}
			}
		}

		if (BIT(pl->used, HAS_EMERGENCY_SHIELD)) {
			if (pl->fuel.sum > 0
				&& BIT(pl->used, HAS_SHIELD)
				&& --pl->emergency_shield_left <= 0)
			{
				if (pl->item[ITEM_EMERGENCY_SHIELD]) {
					pl->EmergencyShield(true);
				} else {
					pl->EmergencyShield(false);
				}
			}
		}

		if (BIT(pl->used, HAS_LASER)) {
			if (pl->item[ITEM_LASER] <= 0
				|| BIT(pl->used, HAS_PHASING_DEVICE))
			{
				CLR_BIT(pl->used, HAS_LASER);
			} else {
				pl->FireLaser();
			}
		}

		if (BIT(pl->used, HAS_DEFLECTOR)) {
			DoDeflector(pl);
		}

		/*
		 * Only do autopilot code if switched on and player is not
		 * damaged (ie. can see).
		 */
		if (   (BIT(pl->used, HAS_AUTOPILOT))
			|| (BIT(pl->status, HOVERPAUSE) && !pl->damaged))
		{
			pl->DoAutopilot();
		}

		/*
		 * Compute turn
		 */
		pl->turnvel	+= pl->turnacc;

		/*
		 * turnresistance is zero: client requests linear turning behaviour
		 * when playing with pointer control.
		 */
		if (pl->turnresistance) {
			pl->turnvel *= pl->turnresistance;
		}

		if (options.turnThrust->GetBool()) {
			tf = (long)(pl->oldturnvel - pl->turnvel);
			tf = (long)TURN_FUEL(tf);
			if (pl->fuel.sum <= tf) {
				tf = 0;
				pl->turnacc = 0.0;
				pl->turnvel = pl->oldturnvel;
			} else {
				Add_fuel(&(pl->fuel),-tf);
				pl->oldturnvel = pl->turnvel;
			}
		}


		pl->float_dir	+= pl->turnvel;

		while (pl->float_dir < 0)
			pl->float_dir += RES;
		while (pl->float_dir >= RES)
			pl->float_dir -= RES;

		/*
		 * turnresistance is zero: client requests linear turning behaviour
		 * when playing with pointer control.
		 */
		if (!pl->turnresistance) {
			pl->turnvel = 0;
		}

		TurnPlayer(i);


		/*
		 * Compute energy drainage
		 */
		if (BIT(pl->used, HAS_SHIELD))
			Add_fuel(&(pl->fuel), (long)ED_SHIELD);

		if (BIT(pl->used, HAS_PHASING_DEVICE))
			Add_fuel(&(pl->fuel), (long)ED_PHASING_DEVICE);

		if (BIT(pl->used, HAS_CLOAKING_DEVICE))
			Add_fuel(&(pl->fuel), (long)ED_CLOAKING_DEVICE);

#		define UPDATE_RATE 100

		for (j = 0; j < numPlayers; j++) {
			if (pl->forceVisible)
				players[j]->visibility[i].canSee = 1;

			if (i == j || !BIT(players[j]->used, HAS_CLOAKING_DEVICE))
				pl->visibility[j].canSee = 1;
			else if (pl->updateVisibility
					|| players[j]->updateVisibility
					|| (int)(rfrac() * UPDATE_RATE)
						< ABS(frame_loops - pl->visibility[j].lastChange))
			{

				pl->visibility[j].lastChange = frame_loops;
				pl->visibility[j].canSee
					= (rfrac() * (pl->item[ITEM_SENSOR] + 1))
					> (rfrac() * (players[j]->item[ITEM_CLOAK] + 1));
			}
		}

		if (BIT(pl->used, HAS_REFUEL)) {
			if ((WrapLength(pl->pos.x - fuels[pl->fs].pix_pos.x,
							 pl->pos.y - fuels[pl->fs].pix_pos.y) > 90.0)
				|| (pl->fuel.sum >= pl->fuel.max)
				|| (block[fuels[pl->fs].blk_pos.x][fuels[pl->fs].blk_pos.y] != FUEL)
				|| BIT(pl->used, HAS_PHASING_DEVICE)
				|| (BIT(rules->mode, TEAM_PLAY)
					&& options.teamFuel->GetBool()
					&& fuels[pl->fs].team != pl->team))
			{
				CLR_BIT(pl->used, HAS_REFUEL);
			} else {
				int i = pl->fuel.num_tanks;
				int ct = pl->fuel.current;

				do {
					if (fuels[pl->fs].fuel > REFUEL_RATE) {
						fuels[pl->fs].fuel -= REFUEL_RATE;
						fuels[pl->fs].conn_mask = 0;
						fuels[pl->fs].last_change = frame_loops;
						Add_fuel(&(pl->fuel), REFUEL_RATE);
					} else {
						Add_fuel(&(pl->fuel), fuels[pl->fs].fuel);
						fuels[pl->fs].fuel = 0;
						fuels[pl->fs].conn_mask = 0;
						fuels[pl->fs].last_change = frame_loops;
						CLR_BIT(pl->used, HAS_REFUEL);
						break;
					}
					if (pl->fuel.current == pl->fuel.num_tanks)
						pl->fuel.current = 0;
					else
						pl->fuel.current += 1;
				} while (i--);
				pl->fuel.current = ct;
			}
		}

		/* target repair */
		if (BIT(pl->used, HAS_REPAIR)) {
			Target *targ = &targets[pl->repair_target];
			DFLOAT x = (targ->pos.x + 0.5) * BLOCK_SZ;
			DFLOAT y = (targ->pos.y + 0.5) * BLOCK_SZ;
			if (WrapLength(pl->pos.x - x, pl->pos.y - y) > 90.0
			|| targ->damage >= TARGET_DAMAGE
			|| targ->dead_time > 0
			|| BIT(pl->used, HAS_PHASING_DEVICE)) {
			CLR_BIT(pl->used, HAS_REPAIR);
			} else {
				int i = pl->fuel.num_tanks;
				int ct = pl->fuel.current;

				do {
					if (pl->fuel.tank[pl->fuel.current] > REFUEL_RATE) {
						targ->damage += TARGET_FUEL_REPAIR_PER_FRAME;
						targ->conn_mask = 0;
						targ->last_change = frame_loops;
						Add_fuel(&(pl->fuel), -REFUEL_RATE);
						if (targ->damage > TARGET_DAMAGE) {
							targ->damage = TARGET_DAMAGE;
							break;
						}
					} else {
						CLR_BIT(pl->used, HAS_REPAIR);
					}
					if (pl->fuel.current == pl->fuel.num_tanks)
						pl->fuel.current = 0;
					else
						pl->fuel.current += 1;
				} while (i--);
				pl->fuel.current = ct;
			}
		}

		if (pl->fuel.sum <= 0) {
			CLR_BIT(pl->used, HAS_SHIELD|HAS_CLOAKING_DEVICE|HAS_DEFLECTOR);
			CLR_BIT(pl->status, THRUSTING);
		}
		if (pl->fuel.sum > (pl->fuel.max-REFUEL_RATE))
			CLR_BIT(pl->used, HAS_REFUEL);

		/*
		 * Update acceleration vector etc.
		 */
		if (BIT(pl->status, THRUSTING)) {
			DFLOAT power = pl->power;
			DFLOAT f = pl->power * 0.0008;	/* 1/(FUEL_SCALE*MIN_POWER) */
			int a = (BIT(pl->used, HAS_EMERGENCY_THRUST)
					? MAX_AFTERBURNER
					: pl->item[ITEM_AFTERBURNER]);
			DFLOAT inert = pl->mass;

			if (a) {
				power = AFTER_BURN_POWER(power, a);
				f = AFTER_BURN_FUEL(f, a);
			}
			pl->acc.x = power * tcos(pl->dir) / inert;
			pl->acc.y = power * tsin(pl->dir) / inert;
			Add_fuel(&(pl->fuel), (long)(-f * FUEL_SCALE_FACT)); /* Decrement fuel */
		} else {
			pl->acc.x = pl->acc.y = 0.0;
		}

		pl->SetMass();

		if (BIT(pl->status, WARPING)) {
			position w;
			int wx, wy, proximity,
			nearestFront, nearestRear,
			proxFront, proxRear;

			if (pl->wormHoleHit >= numWormholes) {
				/* could happen if the player hit a temporary wormhole
				that was removed while the player was warping */
				CLR_BIT(pl->status, WARPING);
				break;
			}

			if (pl->wormHoleHit != -1) {

				if (wormHoles[pl->wormHoleHit].countdown > 0) {
					j = wormHoles[pl->wormHoleHit].lastdest;
				} else if (rfrac() < 0.10f) {
					do
						j = (int)(rfrac() * numWormholes);
					while (wormHoles[j].type == WORM_IN
						|| pl->wormHoleHit == j
						|| wormHoles[j].temporary);
				} else {
					nearestFront = nearestRear = -1;
					proxFront = proxRear = 10000000;

					for (j = 0; j < numWormholes; j++) {
						if (j == pl->wormHoleHit
							|| wormHoles[j].type == WORM_IN
							|| wormHoles[j].temporary)
							continue;

						wx = (wormHoles[j].pos.x -
							wormHoles[pl->wormHoleHit].pos.x) * BLOCK_SZ;
						wy = (wormHoles[j].pos.y -
							wormHoles[pl->wormHoleHit].pos.y) * BLOCK_SZ;
						wx = WRAP_DX(this, wx);
						wy = WRAP_DX(this, wy);

						proximity = (int)(pl->vel.y * wx + pl->vel.x * wy);
						proximity = ABS(proximity);

						if (pl->vel.x * wx + pl->vel.y * wy < 0) {
						if (proximity < proxRear) {
							nearestRear = j;
							proxRear = proximity;
						}
						} else if (proximity < proxFront) {
							nearestFront = j;
							proxFront = proximity;
						}
					}

#				define RANDOM_REAR_WORM
#				ifndef RANDOM_REAR_WORM
					j = nearestFront < 0 ? nearestRear : nearestFront;
#				else /* RANDOM_REAR_WORM */
					if (nearestFront >= 0) {
						j = nearestFront;
					} else {
						do
							j = (int)(rfrac() * numWormholes);
						while (wormHoles[j].type == WORM_IN
								|| j == pl->wormHoleHit);
					}
#				endif /* RANDOM_REAR_WORM */
				}

				SoundPlaySensors(this, pl->pos.x, pl->pos.y, WORM_HOLE_SOUND);

				w.x = (wormHoles[j].pos.x + 0.5) * BLOCK_SZ;
				w.y = (wormHoles[j].pos.y + 0.5) * BLOCK_SZ;

			} else { /* wormHoleHit == -1 */
				int counter;
				for (counter = 20; counter > 0; counter--) {
					w.x = (int)(rfrac() * width);
					w.y = (int)(rfrac() * height);
					if (BIT(1U << block[(int)(w.x/BLOCK_SZ)]
					[(int)(w.y/BLOCK_SZ)],
					SPACE_BLOCKS)) {
						break;
					}
				}
				if (!counter) {
					w.x = OBJ_X_IN_PIXELS(pl);
					w.y = OBJ_Y_IN_PIXELS(pl);
				}
				if (counter
					&& options.wormTime->GetInt()
					&& BIT(1U << block[OBJ_X_IN_BLOCKS(pl)]
						[OBJ_Y_IN_BLOCKS(pl)],
						SPACE_BIT)
					&& BIT(1U << block[(int)(w.x/BLOCK_SZ)]
						[(int)(w.y/BLOCK_SZ)],
						SPACE_BIT))
				{
					AddTempWormholes(OBJ_X_IN_BLOCKS(pl),
					OBJ_Y_IN_BLOCKS(pl),
					(int)(w.x/BLOCK_SZ),
					(int)(w.y/BLOCK_SZ));
				}
				j = -2;
				SoundPlaySensors(this, pl->pos.x, pl->pos.y, HYPERJUMP_SOUND);
			}

			/*
			* Don't connect to balls while warping.
			*/
			if (BIT(pl->used, HAS_CONNECTOR))
			pl->ball = NULL;

			if (BIT(pl->have, HAS_BALL)) {
				/*
				 * Take every ball associated with player through worm hole.
				 * NB. the connector can cross a wall boundary this is
				 * allowed, so long as the ball itself doesn't collide.
				 */
				int k;
				for (k = 0; k < numObjs; k++) {
					Object *b = objs[k];
					if (BIT(b->type, OBJ_BALL) && b->id == pl->id) {
						position ballpos;
						ballpos.x = b->pos.x + (w.x - pl->pos.x);
						ballpos.y = b->pos.y + (w.y - pl->pos.y);
						ballpos.x = WRAP_XPIXEL(this, ballpos.x);
						ballpos.y = WRAP_YPIXEL(this, ballpos.y);
						if (ballpos.x < 0 || ballpos.x >= width
						|| ballpos.y < 0 || ballpos.y >= height) {
							b->life = 0;
						} else {
							Object_position_set_pixels(b, ballpos.x, ballpos.y);
							Object_position_remember(b);
							b->vel.x *= WORM_BRAKE_FACTOR;
							b->vel.y *= WORM_BRAKE_FACTOR;
							CellAddObject(b);
						}
					}
				}
			}

			pl->wormHoleDest = j;
			Player_position_init_pixels(pl, w.x, w.y);
			pl->vel.x *= WORM_BRAKE_FACTOR;
			pl->vel.y *= WORM_BRAKE_FACTOR;
			pl->forceVisible += 15;

			if ((j != pl->wormHoleHit) && (pl->wormHoleHit != -1)) {
				wormHoles[pl->wormHoleHit].lastdest = j;
				if (!wormHoles[j].temporary) {
					wormHoles[pl->wormHoleHit].countdown =
						(options.wormTime->GetInt()
						? options.wormTime->GetInt()
						: WORMCOUNT);
				}
			}

			CLR_BIT(pl->status, WARPING);
			SET_BIT(pl->status, WARPED);

			SoundPlaySensors(this, pl->pos.x, pl->pos.y, WORM_HOLE_SOUND);
		}

		if (!BIT(pl->status, PAUSE)) {
			update_object_speed(pl);	    /* New position */
			MovePlayer(i);
		}

		if ((!BIT(pl->used, HAS_CLOAKING_DEVICE) || !options.cloakedExhaust->GetBool())
			&& !BIT(pl->used, HAS_PHASING_DEVICE))
		{
			if (BIT(pl->status, THRUSTING))
				pl->Thrust();
			if (tf && options.turnThrust->GetBool())
				pl->TurnThrust(TURN_SPARKS(tf));
		}

		pl->ComputeSensorRange();

		pl->used &= pl->have;
	}

	for (i = numWormholes - 1; i >= 0; i--) {
		if (wormHoles[i].countdown > 0) {
			wormHoles[i].countdown--;
		}
		if (wormHoles[i].temporary
			&& wormHoles[i].countdown <= 0)
		{
			RemoveTempWormhole(i);
		}
	}


	D(for (i = numObjs - 1; i >= 0; i--)
	{
		assert(objs[i]->type);
	})


	for (i = 0; i < numPlayers; i++) {
		Player *pl = players[i];

		pl->updateVisibility = 0;

		if (pl->forceVisible) {
			pl->forceVisible--;

		if (!pl->forceVisible)
			pl->updateVisibility = 1;
		}

		if (BIT(pl->used, HAS_TRACTOR_BEAM))
			TractorBeam(pl);

		if (BIT(pl->lock.tagged, LOCK_PLAYER)) {
			pl->lock.distance =
				WrapLength(pl->pos.x - players[getInd[pl->lock.pl_id]]->pos.x,
			pl->pos.y - players[getInd[pl->lock.pl_id]]->pos.y);
		}
	}

	/*
	 * Checking for collision, updating score etc. (see collision.c)
	 */
	CheckCollision();


	/*
	 * Update tanks, Kill players that ought to be killed.
	 */
	for (i = numPlayers - 1; i >= 0; i--) {
		Player *pl = players[i];

		if (BIT(pl->status, PLAYING|PAUSE|GAME_OVER|KILLED) == PLAYING)
		Update_tanks(&(pl->fuel));
		if (BIT(pl->status, KILLED)) {
			ThrowItems(i);

			DetonateItems(pl);

			pl->Kill();

			if (IS_HUMAN_PTR(pl)) {
				if (frame_loops - pl->frame_last_busy > 60 * GetFPS()) {
					if ((numPlayers - numRobots - numPseudoPlayers) > 1) {
						PausePlayer(pl, 1);
					}
				}
			}
		}
		if (options.maxPauseTime->GetSec() > 0
			&& IS_HUMAN_PTR(pl)
			&& BIT(pl->status, PAUSE)
			&& frame_loops - pl->frame_last_busy > options.maxPauseTime->GetSec())
		{
			sprintf(msg, "%s was auto-kicked for pausing too long [*Server notice*]",
					pl->name);
			BroadcastPlayMessage(msg);
			players[i]->conn->DestroyConnection("auto-kicked: paused too long");
		}
	}

	/*
	 * Kill shots that ought to be dead.
	 */
	for (i = numObjs - 1; i >= 0; i--)
	{
#ifdef	_DEBUG
		Object* o = objs[i];
#endif
		assert(objs[i]->type);
		if (--(objs[i]->life) <= 0)
		    DeleteShot(i);
	}

	//
	// In a tag game, check if no one is it, and tag someone
	//
	if (options.tagGame->GetBool()
		&& numPlayers > 1
		&& tag == NO_ID)
	{
        if ((tag = RandomPlayerWithStatus(PAUSE, 0L)) != NO_ID)
        {
		    sprintf(msg, " < %s is now 'it' > ", players[getInd[tag]]->name);
		    BroadcastPlayMessage(msg);
        }
	}

	/*
	 * Compute general game status, do we have a winner?
	 * (not called after Game_Over() )
	 */
	if (options.gameDuration->GetDouble() >= 0.0
		|| options.maxRoundTime->GetInt() > 0)
	{
		ComputeGameStatus();
	}

	/*
	 * Now update labels if need be.
	 */
	if (updateScores && frame_loops % UPDATE_SCORE_DELAY == 0)
		UpdateScoreTable(this);
}

#define TMASK(w, m, b) ((w & m) == b)

int World::RandomPlayerWithStatus(long mask, long bits)
{
    int   choice;

    choice = players[(int)(rfrac() * numPlayers)]->id;

    //
    // Don't pick an ineligible player.
    //
    if (!TMASK(players[getInd[choice]]->status, mask, bits))
    {
        //
        // If only two players, try the other one.
        //
        if (numPlayers == 2)
        {
            int picked = (getInd[choice] == 1) ? 0 : 1;
            if (TMASK(players[picked]->status, mask, bits))
                choice = players[picked]->id;
            else
                choice = NO_ID;
        }
        //
        // More than one other player, pick at random until we find
        // an eligible player, or we run out of options.
        //
        else
        {
            int ind,                    // The currently chosen index
                *picked = (int *)malloc(numPlayers * sizeof(int));     // Player picked flags
            int left = numPlayers - 1;  // How many players remain unchecked

            for (int i = 0; i < numPlayers; i++) picked[i] = 0;     // Set all flags to false.
            picked[getInd[choice]] = 1;                            // Except for the known ineligible one.

            choice = NO_ID;
            //
            // Choose players at random, marking those ineligible, until
            // an eligible player is found, or there is only one left to check.
            //
            while (left > 1)
            {
                ind = (int)(rfrac() * numPlayers);
                if (!picked[ind])
                {
                    if (TMASK(players[ind]->status, mask, bits))
                    {
                        choice = players[ind]->id;
                        break;
                    }
                    else
                    {
                        picked[ind] = 1;
                        --left;
                    }
                }
            }
            //
            // If we haven't chosen a player, then there is only one player
            // left, and we can stop the random picking.
            // Find the leftover, and check to see if he's eligible.
            //
            if (choice == NO_ID && left > 0)
                for (int i = 0; i < numPlayers; i++)
                    if (!picked[i])
                        if (TMASK(players[i]->status, mask, bits))
                            choice = players[i]->id;
            free(picked);
        }
	}
    return choice;
}
