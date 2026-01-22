/* $Id: item.cpp,v 1.29 2007/01/19 07:14:49 dick Exp $
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
 *  $Log: item.cpp,v $
 *  Revision 1.29  2007/01/19 07:14:49  dick
 *  Whitespace
 *
 *  Revision 1.28  2007/01/17 23:18:36  dick
 *  Fix bug with cannon's firing ECMs in non-team games
 *
 *  Revision 1.27  2007/01/10 18:14:47  dick
 *  All robot actions are now handled through RobotMan.
 *  There is one RobotMan per World.
 *
 *  Revision 1.26  2006/09/21 04:55:49  dick
 *  TeamImmune takes an id, not an Ind() in DoTransporter
 *
 *  Revision 1.25  2004/04/29 16:08:27  dick
 *  compile with SOUND enabled.
 *
 *  Revision 1.24  2004/04/26 07:07:14  dick
 *  Define a list of pages that can be displayed on the ScoreTable in the client.
 *  Each page can be controlled by the server (and the server admin tool).
 *
 *  Revision 1.23  2004/04/21 21:43:57  dick
 *  Add CHECK_OBJS(), a debugging macro to do consistency checks on the objs array.
 *  Retab.
 *
 *  Revision 1.22  2004/02/23 02:06:09  dick
 *  robot.h becomes Robot.h
 *
 *  Revision 1.21  2004/02/20 06:27:47  dick
 *  SendPlayMessage becomes BroadcastPlayMessage() because it sends a play message
 *  to all connections.
 *
 *  Revision 1.20  2004/02/18 01:22:14  dick
 *  Remove extraneous and obsolete #include "netserver.h"
 *
 *  Revision 1.19  2004/02/03 03:39:54  dick
 *  Add Windows precompiled header support.
 *
 *  Revision 1.18  2002/09/11 16:42:04  dick
 *  Here endeth the server encapsulation task.
 *  theWorld is now an array of World(s) and get loaded dynamically.
 *
 *  Revision 1.17  2002/09/01 00:13:46  dick
 *  Remove obsolete windows #include "winServer.h"
 *
 *  Revision 1.16  2002/06/30 06:46:59  dick
 *  SendMessage is a bad word.  It's a Windows function that is a macro and gets
 *  translated to SendMessageA or SendMessageW.  Although it works, renaming ours
 *  to SendPlayMessage makes browsing the source code easier.
 *
 *  Revision 1.15  2002/06/01 06:06:48  dick
 *  Encapsulate (almost) everything.  Get rid of (almost) all refs to theWorld.
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
 *  Revision 1.11  2001/08/10 08:11:38  dick
 *  New class ConnectionPlayer subclasses Connection to provide player
 *  functions.  New class Frame subclasses ConnectionPlayer which provides
 *  Frame update functionality.
 *
 *  Revision 1.10  2001/08/05 08:05:36  dick
 *  Add Tuners to the server.  ComputeSensorRange(), AddTank(), RemoveTank()
 *  become members of Player.  ComputeGravity*() become members of World.
 *
 *  Revision 1.9  2001/07/24 09:04:00  dick
 *  Remove all global options.  Use World.options instead.
 *
 *  Revision 1.8  2001/07/23 21:08:37  dick
 *  Encapsulate ShipMass, ShotsMax, ShotsLife, ShotsMass, FPS.
 *
 *  Revision 1.7  2001/07/19 05:00:02  dick
 *  There is no real reason to have optionsStruct, all of the ServerOption(s)
 *  are attributes of ServerOptions.  And rename Options.* to ServerOptions.*
 *  (They could be just Options, but when the client comes along with his
 *  different format options...)
 *
 *  Revision 1.6  2001/07/18 20:15:33  dick
 *  The first pass at integrating xpilotedit's options with the server options.
 *  The server does not build yet.  xpilotedit works as before, with the new
 *  definitions.
 *
 *  Revision 1.5  2001/07/12 08:10:49  dick
 *  Comment out debug.
 *
 *  Revision 1.4  2001/07/09 06:57:19  dick
 *  Don't use Trace(). Use xpprintf
 *
 *  Revision 1.3  2001/07/08 06:40:14  dick
 *  C++ conversion stuff.
 *
 *  Revision 1.2  2001/07/07 12:00:42  dick
 *  Rename classes to C++ "Style".  old World becomes theWorld.
 *
 */
#include "StdAfx.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <errno.h>
#include <limits.h>
#include <assert.h>

#include "version.h"
#include "config.h"
#include "serverconst.h"
#include "global.h"
#include "proto.h"
#include "saudio.h"
#include "score.h"
#include "error.h"
#include "objpos.h"
#include "ConnectionPlayer.h"
#include "Cannon.h"
#include "RobotMan.h"

char item_version[] = VERSION;

#define CONFUSED_TIME	3

///////////////////////////////////////////////////////////////////////////////
/*
 * Debugging macro.  Turn this on to have the objs[] table consistency checked
 * at various places along the calculations.
 */
#if (0)
#define	CHECK_OBJS()							\
	D(	int	_i;									\
		for (_i=0; _i<numObjs; _i++) {			\
			assert(objs[_i]->type <= 0x2000);	\
		}										\
	)
#else
#define	CHECK_OBJS()
#endif

///////////////////////////////////////////////////////////////////////////////
static void Item_update_flags(Player *pl)
{
	if (pl->item[ITEM_CLOAK] <= 0
		&& BIT(pl->have, HAS_CLOAKING_DEVICE)) {
		CLR_BIT(pl->have, HAS_CLOAKING_DEVICE);
		pl->updateVisibility = 1;
	}
	if (pl->item[ITEM_MIRROR] <= 0)
		CLR_BIT(pl->have, HAS_MIRROR);
	if (pl->item[ITEM_DEFLECTOR] <= 0)
		CLR_BIT(pl->have, HAS_DEFLECTOR);
	if (pl->item[ITEM_AFTERBURNER] <= 0)
		CLR_BIT(pl->have, HAS_AFTERBURNER);
	if (pl->item[ITEM_PHASING] <=0
		&& !BIT(pl->used, HAS_PHASING_DEVICE)
		&& pl->phasing_left == 0)
		CLR_BIT(pl->have, HAS_PHASING_DEVICE);
	if (pl->item[ITEM_EMERGENCY_THRUST] <= 0
		&& !BIT(pl->used, HAS_EMERGENCY_THRUST)
		&& pl->emergency_thrust_left == 0)
		CLR_BIT(pl->have, HAS_EMERGENCY_THRUST);
	if (pl->item[ITEM_EMERGENCY_SHIELD] <= 0
		&& !BIT(pl->used, HAS_EMERGENCY_SHIELD)
		&& pl->emergency_shield_left == 0) {
		if (BIT(pl->have, HAS_EMERGENCY_SHIELD)) {
			CLR_BIT(pl->have, HAS_EMERGENCY_SHIELD);
			if (!BIT(DEF_HAVE, HAS_SHIELD) && pl->shield_time <= 0) {
				CLR_BIT(pl->have, HAS_SHIELD);
				CLR_BIT(pl->used, HAS_SHIELD);
			}
		}
	}
	if (pl->item[ITEM_TRACTOR_BEAM] <= 0)
		CLR_BIT(pl->have, HAS_TRACTOR_BEAM);
	if (pl->item[ITEM_AUTOPILOT] <= 0) {
		if (BIT(pl->used, HAS_AUTOPILOT))
			pl->Autopilot(false);
		CLR_BIT(pl->have, HAS_AUTOPILOT);
	}
	if (pl->item[ITEM_ARMOR] <= 0)
		CLR_BIT(pl->have, HAS_ARMOR);
}

///////////////////////////////////////////////////////////////////////////////
/*
 * Player loses some items after some event (collision, bounce).
 * The `prob' parameter gives the chance that items are lost
 * and, if they are lost, what percentage.
 */
void Item_damage(Player* pl, DFLOAT prob)
{
	if (prob < 1.0f) {
//		Player			*pl = Players[ind];
		int 			i;
		DFLOAT			loss;

		loss = prob;
		LIMIT(loss, 0.0f, 1.0f);

		for (i = 0; i < NUM_ITEMS; i++) {
			if (!BIT(1U << i, ITEM_BIT_FUEL|ITEM_BIT_TANK)) {
				if (pl->item[i]) {
					DFLOAT f = rfrac();
					if (f < loss) {
						pl->item[i] = (int)(pl->item[i] * loss + 0.5f);
					}
				}
			}
		}

		Item_update_flags(pl);
	}
}

///////////////////////////////////////////////////////////////////////////////
int World::ChooseRandomItem(void)
{
	int		i;
	DFLOAT	item_prob_sum = 0;

	for (i = 0; i < NUM_ITEMS; i++) {
		item_prob_sum += items[i].prob;
	}

	if (item_prob_sum > 0.0) {
		DFLOAT sum = item_prob_sum * rfrac();

		for (i = 0; i < NUM_ITEMS; i++) {
			sum -= items[i].prob;
			if (sum <= 0) {
			break;
			}
		}
		if (i >= NUM_ITEMS) {
			i = ITEM_FUEL;
		}
	}

	return i;
}

///////////////////////////////////////////////////////////////////////////////
void World::PlaceItem(int item, int ind)
{
	int 				num_lose, num_per_pack,
						bx, by,
						place_count,
						dir, dist;
	long				grav, rand;
	int 				px, py;
	DFLOAT				vx, vy;
	ItemConcentrator	*con;
	Player				*pl = (ind == -1 ? NULL : players[ind]);

	if (numObjs >= MAX_TOTAL_SHOTS) {
		if (pl && !BIT(pl->status, KILLED)) {
			pl->item[item]--;
		}
		return;
	}

	if (pl) {
		if (BIT(pl->status, KILLED)) {
			num_lose = pl->item[item] - items[item].initial;
			if (num_lose <= 0) {
				return;
			}
			pl->item[item] -= num_lose;
			num_per_pack = (int)(num_lose * options.dropItemOnKillProb->GetDouble());
			if (num_per_pack < items[item].min_per_pack) {
				return;
			}
		}
		else {
			num_lose = pl->item[item];
			if (num_lose <= 0) {
				return;
			}
			if (items[item].min_per_pack == items[item].max_per_pack) {
				num_per_pack = items[item].max_per_pack;
			} else {
				num_per_pack = items[item].min_per_pack
							 + (int)(rfrac() * (1 + items[item].max_per_pack
												- items[item].min_per_pack));
			}
			if (num_per_pack > num_lose) {
				num_per_pack = num_lose;
			} else {
				num_lose = num_per_pack;
			}
			pl->item[item] -= num_lose;
		}
	} else {
		if (items[item].min_per_pack == items[item].max_per_pack) {
			num_per_pack = items[item].max_per_pack;
		} else {
			num_per_pack = items[item].min_per_pack
						 + (int)(rfrac() * (1 + items[item].max_per_pack
											- items[item].min_per_pack));
		}
	}

	if (pl) {
		grav = GRAVITY;
		rand = 0;
		px = pl->prevpos.x;
		py = pl->prevpos.y;
		if (!BIT(pl->status, KILLED)) {
			/*
			 * Player is dropping an item on purpose.
			 * Give the item some offset so that the
			 * player won't immediately pick it up again.
			 */
			if (pl->vel.x >= 0)
				px -= (BLOCK_SZ + (int)(rfrac() * 8));
			else
				px += (BLOCK_SZ + (int)(rfrac() * 8));
			if (pl->vel.y >= 0)
				py -= (BLOCK_SZ + (int)(rfrac() * 8));
			else
				py += (BLOCK_SZ + (int)(rfrac() * 8));
		}
		if (px < 0)
			px += width;
		else if (px >= width)
			px -= width;
		if (py < 0)
			py += height;
		else if (py >= height)
			py -= height;
		bx = px / BLOCK_SZ;
		by = py / BLOCK_SZ;
		if (!BIT(1U << block[bx][by], SPACE_BLOCKS)) {
			return;
		}
	} else {
		if (rfrac() < options.movingItemProb->GetDouble()) {
			grav = GRAVITY;
		} else {
			grav = 0;
		}
		if (rfrac() < options.randomItemProb->GetDouble()) {
			rand = RANDOM_ITEM;
		} else {
			rand = 0;
		}
		if (numItemConcentrators > 0 && rfrac() < options.itemConcentratorProb->GetDouble()) {
			con = &itemConcentrators[(int)(rfrac() * numItemConcentrators)];
		} else {
			con = NULL;
		}
		/*
		 * This will take very long (or forever) with maps
		 * that hardly have any (or none) spaces.
		 * So bail out after a few retries.
		 */
		for (place_count = 0; ; place_count++) {
			if (place_count >= 8) {
				return;
			}
			if (con) {
				dir = (int)(rfrac() * RES);
				dist = (int)(rfrac() * ((options.itemConcentratorRadius->GetInt()
						* BLOCK_SZ) + 1));
				px = (int)((con->pos.x + 0.5) * BLOCK_SZ + dist * tcos(dir));
				py = (int)((con->pos.y + 0.5) * BLOCK_SZ + dist * tsin(dir));
				if (BIT(rules->mode, WRAP_PLAY)) {
					if (px < 0) px += width;
					if (px >= width) px -= width;
					if (py < 0) py += height;
					if (py >= height) py -= height;
				}
				if (px < 0 || px >= width
					|| py < 0 || py >= height) {
					continue;
				}
				bx = px / BLOCK_SZ;
				by = py / BLOCK_SZ;
			}
			else {
				px = (int)(rfrac() * width);
				py = (int)(rfrac() * height);
				bx = px / BLOCK_SZ;
				by = py / BLOCK_SZ;
			}
			if (BIT(1U << block[bx][by], SPACE_BLOCKS|CANNON_BIT)) {
				break;
			}
		}
	}
	vx = vy = 0;
	if (grav) {
		if (pl) {
			vx += pl->vel.x;
			vy += pl->vel.y;
			if (!BIT(pl->status, KILLED)) {
				DFLOAT vl = LENGTH(vx, vy);
				int dvx = (int)(rfrac() * 8);
				int dvy = (int)(rfrac() * 8);
				const float drop_speed_factor = 0.75f;
				vx *= drop_speed_factor;
				vy *= drop_speed_factor;
				if (vl < 1.0f) {
					vx -= (pl->vel.x >= 0) ? dvx : -dvx;
					vy -= (pl->vel.y >= 0) ? dvy : -dvy;
				} else {
					vx -= dvx * (vx / vl);
					vy -= dvy * (vy / vl);
				}
				} else {
						DFLOAT vel = rfrac() * 6;
						int dir = (int)(rfrac() * RES);
						vx += tcos(dir) * vel;
						vy += tsin(dir) * vel;
				}
		} else {
			vx -= options.gravity->GetDouble() * gravity[bx][by].x;
			vy -= options.gravity->GetDouble() * gravity[bx][by].y;
			vx += (int)(rfrac() * 8)-3;
			vy += (int)(rfrac() * 8)-3;
		}
	}
		MakeItem(px, py,
						  (int)vx, (int)vy,
						  item, num_per_pack,
						  grav|rand);
}

///////////////////////////////////////////////////////////////////////////////
void World::MakeItem(int px, int py,
						   int vx, int vy,
						   int item, int num_per_pack,
						   long status)
{
		Object* obj;
		if (items[item].num >= items[item].max)
				return;
		if ((obj = Object_allocate(this)) == NULL)
				return;

	obj->type = OBJ_ITEM;
	obj->info = item;
//	xpprintf("info=%x\n", obj->info);
	obj->color = RED;
	obj->status = status;
	obj->id = NO_ID;
	obj->team = TEAM_NOT_SET;
	Object_position_init_pixels(obj, px, py);
	obj->vel.x = vx;
	obj->vel.y = vy;
	obj->acc.x =
	obj->acc.y = 0.0;
	obj->mass = 10.0;
	obj->life = 1500 + (int)(rfrac() * 512);
	obj->count = num_per_pack;
	obj->pl_range = ITEM_SIZE/2;
	obj->pl_radius = ITEM_SIZE/2;

	items[item].num++;
	CellAddObject(obj);
}

///////////////////////////////////////////////////////////////////////////////
void World::ThrowItems(int ind)
{
	int 				num_items_to_throw, remain, item;
	Player				*pl = (ind == -1 ? NULL : players[ind]);

	if (!pl)
	{
		xpprintf("Throw item player = null\n");
	}
	if (!options.dropItemOnKillProb->GetDouble() || !pl)
		return;

	for (item = 0; item < NUM_ITEMS; item++) {
		if (!BIT(1U << item, ITEM_BIT_FUEL | ITEM_BIT_TANK)) {
			do {
				num_items_to_throw = pl->item[item] - items[item].initial;
				if (num_items_to_throw <= 0) {
					break;
				}
				PlaceItem(item, ind);
				remain = pl->item[item] - items[item].initial;
			} while (remain > 0 && remain < num_items_to_throw);
		}
	}

	Item_update_flags(pl);
}

///////////////////////////////////////////////////////////////////////////////
/*
 * Cause some remaining mines or missiles to be launched in
 * a random direction with a small life time (ie. magazine has
 * gone off).
 */
void World::DetonateItems(Player* pl)
{
	int 				i;
	modifiers			mods;
	int 				owner_ind;

	if (!BIT(pl->status, KILLED))
		return;

	/* ZE: Detonated items on tanks should belong to the tank's owner. */
	if (IS_TANK_PTR(pl)) {
		owner_ind = getInd[pl->lock.pl_id];
	} else {
		owner_ind = pl->Ind();
	}

	/*
	 * These are always immune to detonation.
	 */
	if ((pl->item[ITEM_MINE] -= items[ITEM_MINE].initial) < 0)
		pl->item[ITEM_MINE] = 0;
	if ((pl->item[ITEM_MISSILE] -= items[ITEM_MISSILE].initial) < 0)
		pl->item[ITEM_MISSILE] = 0;

	/*
	 * Drop shields in order to launch mines and missiles.
	 */
	CLR_BIT(pl->used, HAS_SHIELD);

	/*
	 * Mines are always affected by gravity and are sent in random directions
	 * slowly out from the ship (velocity relative).
	 */
	for (i = 0; i < pl->item[ITEM_MINE]; i++) {
		if (rfrac() < options.detonateItemOnKillProb->GetDouble()) {
			int dir = (int)(rfrac() * RES);
			DFLOAT vel = rfrac() * 4.0f;

			mods = pl->mods;
			if (BIT(mods.nuclear, NUCLEAR)
				&& pl->item[ITEM_MINE] < options.nukeMinMines->GetInt()) {
				CLR_BIT(mods.nuclear, NUCLEAR);
			}
			PlaceGeneralMine(owner_ind, pl->team, GRAVITY,
							   pl->pos.x, pl->pos.y,
							   pl->vel.x + vel * tcos(dir),
							   pl->vel.y + vel * tsin(dir),
							   mods);
		}
	}
	for (i = 0; i < pl->item[ITEM_MISSILE]; i++) {
		if (rfrac() < options.detonateItemOnKillProb->GetDouble()) {
			int type;

			if (pl->shots >= pl->shot_max)
				break;

			/*
			 * Missiles are random type at random players, which could
			 * mean a misfire.
			 */
			SET_BIT(pl->lock.tagged, LOCK_PLAYER);
			pl->lock.pl_id = players[(int)(rfrac() * numPlayers)]->id;

			switch ((int)(rfrac() * 3)) {
			case 0: 	type = OBJ_TORPEDO; 	break;
			case 1: 	type = OBJ_HEAT_SHOT;	break;
			default:	type = OBJ_SMART_SHOT;	break;
			}

			mods = pl->mods;
			if (BIT(mods.nuclear, NUCLEAR)
				&& pl->item[ITEM_MISSILE] < options.nukeMinSmarts->GetInt()) {
				CLR_BIT(mods.nuclear, NUCLEAR);
			}
			FireGeneralShot(owner_ind, 0, pl->team != 0, pl->pos.x, pl->pos.y,
							  type, (int)(rfrac() * RES), mods, -1);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
void World::TractorBeam(Player* pl)
{
	DFLOAT		maxdist, percent;
	long		cost;

	maxdist = TRACTOR_MAX_RANGE(pl->item[ITEM_TRACTOR_BEAM]);
	if (BIT(pl->lock.tagged, LOCK_PLAYER|LOCK_VISIBLE)
		!= (LOCK_PLAYER|LOCK_VISIBLE)
		|| BIT(players[getInd[pl->lock.pl_id]]->status,
			   PLAYING|PAUSE|KILLED|GAME_OVER) != PLAYING
		|| pl->lock.distance >= maxdist
		|| BIT(pl->used, HAS_PHASING_DEVICE)
		|| BIT(players[getInd[pl->lock.pl_id]]->used, HAS_PHASING_DEVICE)) {
		CLR_BIT(pl->used, HAS_TRACTOR_BEAM);
		return;
	}
	percent = TRACTOR_PERCENT(pl->lock.distance, maxdist);
	cost = (long)TRACTOR_COST(percent);
	if (pl->fuel.sum < -cost) {
		CLR_BIT(pl->used, HAS_TRACTOR_BEAM);
		return;
	}
	GeneralTractorBeam(pl, pl->pos.x, pl->pos.y,
						 pl->item[ITEM_TRACTOR_BEAM],
						 getInd[pl->lock.pl_id], pl->tractor_is_pressor);
}

///////////////////////////////////////////////////////////////////////////////
void World::GeneralTractorBeam(Player* pl, DFLOAT x, DFLOAT y,
						  int items, int target, bool pressor)
{
		Player* victim = players[target];
	DFLOAT		maxdist = TRACTOR_MAX_RANGE(items),
				maxforce = TRACTOR_MAX_FORCE(items),
				percent, force, dist;
	long		cost;
	int 		theta;

	dist = WrapLength(x - victim->pos.x, y - victim->pos.y);
	if (dist > maxdist)
		return;
	percent = TRACTOR_PERCENT(dist, maxdist);
	cost = (long)TRACTOR_COST(percent);
	force = TRACTOR_FORCE(pressor, percent, maxforce);
	
	SoundPlaySensors(this, x, y,
					   (pressor ? PRESSOR_BEAM_SOUND : TRACTOR_BEAM_SOUND));

	if (pl)
		Add_fuel(&(pl->fuel), cost);

	theta = (int)WrapFindDir(x - victim->pos.x, y - victim->pos.y);

	if (pl) {
		pl->vel.x += tcos(theta) * (force / pl->mass);
		pl->vel.y += tsin(theta) * (force / pl->mass);
		Record_shove(pl, victim, frame_loops);
		Record_shove(victim, pl, frame_loops);
	}
	victim->vel.x -= tcos(theta) * (force / victim->mass);
	victim->vel.y -= tsin(theta) * (force / victim->mass);
}


///////////////////////////////////////////////////////////////////////////////
void World::DoDeflector(Player* pl)
{
	DFLOAT		range = (pl->item[ITEM_DEFLECTOR] * 0.5 + 1) * BLOCK_SZ;
	DFLOAT		maxforce = pl->item[ITEM_DEFLECTOR] * 0.2;
	Object		*obj, **obj_list;
	int 		i, obj_count;
	long		dist, dx, dy;

	if (pl->fuel.sum < -ED_DEFLECTOR) {
				if (BIT(pl->used, HAS_DEFLECTOR)) {
						pl->Deflector(false);
				}
		return;
	}
	Add_fuel(&(pl->fuel), (long)ED_DEFLECTOR);

	CellGetObjects(OBJ_X_IN_BLOCKS(pl), OBJ_Y_IN_BLOCKS(pl),
					 (int)(range / BLOCK_SZ + 1), 200,
					 &obj_list, &obj_count);
	
	for (i = 0; i < obj_count; i++) {
		obj = obj_list[i];

		if (obj->life <= 0)
			continue;

		if (obj->id == pl->id) {
			if (BIT(obj->status, OWNERIMMUNE)
				|| obj->fuselife < obj->life
				|| options.selfImmunity->GetBool())
				continue;
		} else {
			if (pl->TeamImmune(obj->id))
				continue;
		}

		/* don't push balls out of treasure boxes */
		if (BIT(obj->type, OBJ_BALL)
			&& !BIT(obj->status, GRAVITY))
			continue;

		dx = (obj->pos.x - pl->pos.x);
		dy = (obj->pos.y - pl->pos.y);
		dx = WRAP_DX(this, dx);
		dy = WRAP_DY(this, dy);
		
		dist = (long)(LENGTH(dx, dy) - SHIP_SZ);
		if (dist < range
			&& dist > 0) {
			int dir = (int)FindDir(dx, dy);
			int idir = MOD2((int)(dir - FindDir(obj->vel.x, obj->vel.y)), RES);

			if (idir > RES * 0.25
				&& idir < RES * 0.75) {
				DFLOAT force = ((DFLOAT)(range - dist) / range)
								* ((DFLOAT)(range - dist) / range)
								* maxforce
								* ((RES * 0.25) - ABS(idir - RES * 0.5))
								/ (RES * 0.25);
				DFLOAT dv = force / ABS(obj->mass);

				obj->vel.x += tcos(dir) * dv;
				obj->vel.y += tsin(dir) * dv;
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
void World::DoTransporter(Player* pl)
{
	Player*		p;
	int 		i, target = -1;
	DFLOAT		dist, closest = TRANSPORTER_DISTANCE;

	/* if not available, fail silently */
	if (!pl->item[ITEM_TRANSPORTER]
		|| pl->fuel.sum < -ED_TRANSPORTER
		|| BIT(pl->used, HAS_PHASING_DEVICE))
		return;

	/* find victim */
	for (i = 0; i < numPlayers; i++) {
		p = players[i];
		if (p == pl
			|| BIT(p->status, PLAYING|PAUSE|GAME_OVER) != PLAYING
			|| p->TeamImmune(pl->id)
			|| IS_TANK_PTR(p)
			|| BIT(p->used, HAS_PHASING_DEVICE))
			continue;
		dist = WrapLength(pl->pos.x - p->pos.x, pl->pos.y - p->pos.y);
		if (dist < closest) {
			closest = dist;
			target = i;
		}
	}

	/* no victims in range */
	if (target == -1) {
		SoundPlaySensors(this, pl->pos.x, pl->pos.y, TRANSPORTER_FAIL_SOUND);
		Add_fuel(&(pl->fuel), ED_TRANSPORTER);
		pl->item[ITEM_TRANSPORTER]--;
		return;
	}

	/* victim found */
	DoGeneralTransporter(pl, pl->pos.x, pl->pos.y, target, NULL, NULL);
}

///////////////////////////////////////////////////////////////////////////////
void World::DoGeneralTransporter(Player* pl, DFLOAT x, DFLOAT y, int target,
							int *itemp, long *amountp)
{
	Player* 	victim = players[target];
	char		msg[MSG_LEN];
	PCSTR		what = NULL;
	int 		i;
	int 		item = ITEM_FUEL;
	long		amount;

	/* choose item type to steal */
	for (i = 0; i < 50; i++) {
		item = (int)(rfrac() * NUM_ITEMS);
		if (victim->item[item]
			|| (item == ITEM_TANK && victim->fuel.num_tanks)
			|| (item == ITEM_FUEL && victim->fuel.sum))
			break;
	}

	if (i == 50) {
		/* you can't pluck from a bald chicken.. */
		SoundPlaySensors(this, x, y, TRANSPORTER_FAIL_SOUND);
		if (!pl) {
			*amountp = 0;
			*itemp = -1;
		}
		return;
	} else {
		SoundPlaySensors(this, x, y, TRANSPORTER_SUCCESS_SOUND);
		if (numTransporters < MAX_TOTAL_TRANSPORTERS) {
			transporters[numTransporters] = (Transporter *)malloc(sizeof(Transporter));
			if (transporters[numTransporters] != NULL) {
				transporters[numTransporters]->pos.x = x;
				transporters[numTransporters]->pos.y = y;
				transporters[numTransporters]->target = victim->id;
				transporters[numTransporters]->id = (pl ? pl->id : NO_ID);
				transporters[numTransporters]->count = 5;
				numTransporters++;
			}
		}
	}

	/* remove loot from victim */
	amount = 1;
	if (!(item == ITEM_MISSILE
		  || item == ITEM_FUEL
		  || item == ITEM_TANK))
		victim->item[item]--;

	/* describe loot and update victim */
	msg[0] = '\0';
	switch (item) {
	case ITEM_AFTERBURNER:
		what = "an afterburner";
		if (victim->item[item] == 0)
			CLR_BIT(victim->have, HAS_AFTERBURNER);
		break;
	case ITEM_MISSILE:
		amount = MIN(victim->item[item], 3);
		if (amount == 1) {
			sprintf(msg, "%s stole a missile from %s.",
					(pl ? pl->name : "A cannon"), victim->name);
		} else {
			sprintf(msg, "%s stole %ld missiles from %s",
					(pl ? pl->name : "A cannon"), amount, victim->name);
		}
		break;
	case ITEM_CLOAK:
		what = "a cloaking device";
		victim->updateVisibility = 1;
		if (!victim->item[item]) {
				victim->Cloak(false);
		}
		break;
	case ITEM_WIDEANGLE:
		what = "a wideangle";
		break;
	case ITEM_REARSHOT:
		what = "a rearshot";
		break;
	case ITEM_MINE:
		what = "a mine";
		break;
	case ITEM_SENSOR:
		what = "a sensor";
		victim->updateVisibility = 1;
		break;
	case ITEM_ECM:
		what = "an ECM";
		break;
	case ITEM_ARMOR:
		what = "an armor";
		if (!victim->item[item])
			CLR_BIT(victim->have, HAS_ARMOR);
		break;
	case ITEM_TRANSPORTER:
		what = "a transporter";
		break;
	case ITEM_MIRROR:
		what = "a mirror";
		if (!victim->item[item])
			CLR_BIT(victim->have, HAS_MIRROR);
		break;
	case ITEM_DEFLECTOR:
		what = "a deflector";
		if (!victim->item[item]) {
				victim->Deflector(false);
		}
		break;
	case ITEM_HYPERJUMP:
		what = "a hyperjump";
		break;
	case ITEM_PHASING:
		what = "a phasing device";
		if (!victim->item[item]) {
			if (BIT(victim->used, HAS_PHASING_DEVICE))
				victim->Phasing(0);
			CLR_BIT(victim->have, HAS_PHASING_DEVICE);
		}
		break;
	case ITEM_LASER:
		what = "a laser";
		break;
	case ITEM_EMERGENCY_THRUST:
		what = "an emergency thrust";
		if (!victim->item[item]) {
			if (BIT(victim->used, HAS_EMERGENCY_THRUST))
				victim->EmergencyThrust(0);
			CLR_BIT(victim->have, HAS_EMERGENCY_THRUST);
		}
		break;
	case ITEM_EMERGENCY_SHIELD:
		what = "an emergency shield";
		if (!victim->item[item]) {
			if (BIT(victim->used, HAS_EMERGENCY_SHIELD))
				victim->EmergencyShield(false);
			CLR_BIT(victim->have, HAS_EMERGENCY_SHIELD);
			if (!BIT(DEF_HAVE, HAS_SHIELD)) {
				CLR_BIT(victim->have, HAS_SHIELD);
				CLR_BIT(victim->used, HAS_SHIELD);
			}
		}
		break;
	case ITEM_TRACTOR_BEAM:
		what = "a tractor beam";
		if (!victim->item[item])
			CLR_BIT(victim->have, HAS_TRACTOR_BEAM);
		break;
	case ITEM_AUTOPILOT:
		what = "an autopilot";
		if (!victim->item[item]) {
			if (BIT(victim->used, HAS_AUTOPILOT))
				victim->Autopilot(false);
			CLR_BIT(victim->have, HAS_AUTOPILOT);
		}
		break;
	case ITEM_TANK:
		/* for tanks, amount is the amount of fuel in the stolen tank */
		what = "a tank";
		i = (int)(rfrac() * victim->fuel.num_tanks) + 1;
		amount = victim->fuel.tank[i];
		victim->RemoveTank(i);
		break;
	case ITEM_FUEL:
		{
			/* choose percantage between 10 and 50. */
			DFLOAT percent = 10.0f + 40.0f * rfrac();
			amount = (long)(victim->fuel.sum * percent / 100);
			sprintf(msg, "%s stole %ld units (%d%%) of fuel from %s.",
					(pl ? pl->name : "A cannon"),
					amount >> FUEL_SCALE_BITS,
					(int)(percent + 0.5),
					victim->name);
		}
		Add_fuel(&(victim->fuel), -amount);
		break;
	}

	/* inform the world about the robbery */
	if (!msg[0]) {
		sprintf(msg, "%s stole %s from %s.", (pl ? pl->name : "A cannon"),
				what, victim->name);
	}
	BroadcastPlayMessage(msg);

	/* cannons take care of themselves */
	if (!pl) {
		*itemp = item;
		*amountp = amount;
		if (item == ITEM_FUEL || item == ITEM_TANK)
			*amountp >>= FUEL_SCALE_BITS;
		return;
	}

	/* don't forget the penalty for robbery */
	pl->item[ITEM_TRANSPORTER]--;
	Add_fuel(&(pl->fuel), ED_TRANSPORTER);

	/* update thief */
	if (!(item == ITEM_FUEL
		  || item == ITEM_TANK))
		pl->item[item] += amount;
	switch(item) {
	case ITEM_AFTERBURNER:
		SET_BIT(pl->have, HAS_AFTERBURNER);
		LIMIT(pl->item[item], 0, MAX_AFTERBURNER);
		break;
	case ITEM_CLOAK:
		SET_BIT(pl->have, HAS_CLOAKING_DEVICE);
		pl->updateVisibility = 1;
		break;
	case ITEM_SENSOR:
		pl->updateVisibility = 1;
		break;
	case ITEM_MIRROR:
		SET_BIT(pl->have, HAS_MIRROR);
		break;
	case ITEM_ARMOR:
		SET_BIT(pl->have, HAS_ARMOR);
		break;
	case ITEM_DEFLECTOR:
		SET_BIT(pl->have, HAS_DEFLECTOR);
		break;
	case ITEM_PHASING:
		SET_BIT(pl->have, HAS_PHASING_DEVICE);
		break;
	case ITEM_EMERGENCY_THRUST:
		SET_BIT(pl->have, HAS_EMERGENCY_THRUST);
		break;
	case ITEM_EMERGENCY_SHIELD:
		SET_BIT(pl->have, HAS_EMERGENCY_SHIELD);
		break;
	case ITEM_TRACTOR_BEAM:
		SET_BIT(pl->have, HAS_TRACTOR_BEAM);
		break;
	case ITEM_AUTOPILOT:
		SET_BIT(pl->have, HAS_AUTOPILOT);
		break;
	case ITEM_TANK:
		/* for tanks, amount is the amount of fuel in the stolen tank */
		if (pl->fuel.num_tanks < MAX_TANKS)
			pl->AddTank(amount);
		break;
	case ITEM_FUEL:
		Add_fuel(&(pl->fuel), amount);
		break;
	default:
		break;
	}

	LIMIT(pl->item[item], 0, items[item].limit);
}

///////////////////////////////////////////////////////////////////////////////
void World::DoLoseItem(Player* pl)
{
	int		item;

	if (!pl)
		return;
	item = pl->lose_item;
	if (item < 0 || item >= NUM_ITEMS) {
		error("BUG: DoLoseItem %d", item);
		return;
	}
	if (BIT(1U << pl->lose_item, ITEM_BIT_FUEL | ITEM_BIT_TANK)) {
		return;
	}
	if (pl->item[item] <= 0) {
		return;
	}

	if (options.loseItemDestroys->GetBool() == false && !BIT(pl->used, HAS_PHASING_DEVICE)) {
		PlaceItem(item, pl->Ind());
	}
	else {
		pl->item[item]--;
	}

	Item_update_flags(pl);
}


///////////////////////////////////////////////////////////////////////////////
void World::FireGeneralEcm(Player* pl, unsigned short team, DFLOAT x, DFLOAT y)
{
	Object*			shot;
	Player*			p;
	MineObject*		closest_mine = NULL;
	SmartObject*	smart;
	MineObject*		mine;
	DFLOAT			closest_mine_range = hypotenuse;
	int				i, j, owner;
	DFLOAT			range, perim, damage;
	Ecm*			ecm;

	if (numEcms >= MAX_TOTAL_ECMS) {
		return;
	}
	ecms[numEcms] = (Ecm *)malloc(sizeof(Ecm));
	if (ecms[numEcms] == NULL) {
		return;
	}
	ecm = ecms[numEcms];
	ecm->pos.x = x;
	ecm->pos.y = y;
	ecm->id = (pl ? pl->id : NO_ID);
	ecm->size = (int)ECM_DISTANCE;
	numEcms++;
	if (pl) {
		pl->ecmcount++;
		pl->item[ITEM_ECM]--;
		Add_fuel(&(pl->fuel), ED_ECM);
		SoundPlaySensors(this, x, y, ECM_SOUND);
	}

	for (i = 0; i < numObjs; i++) {
		shot = objs[i];

		if (! BIT(shot->type, OBJ_SMART_SHOT|OBJ_MINE))
			continue;
		if ((range = WrapLength(x - shot->pos.x, y - shot->pos.y)) > ECM_DISTANCE)
			continue;

		/*
		 * Ignore mines owned by yourself which you are immune to,
		 * or missiles owned by you which are after somebody else.
		 *
		 * Ignore any object not owned by you which are owned by
		 * team members if team immunity is on.
		 */
		if (shot->id != NO_ID) {
			owner = getInd[shot->id];
			if (pl && pl->Ind() == owner) {
				if (shot->type == OBJ_MINE) {
					if (BIT(shot->status, OWNERIMMUNE)) {
						continue;
					}
				}
				if (shot->type == OBJ_SMART_SHOT) {
					if (shot->info != owner) {
						continue;
					}
				}
			} else if ((pl && pl->TeamImmune(owner))
			  || (BIT(rules->mode, TEAM_PLAY)
			  && shot->team == team)) {
				continue;
			}
		}

		switch (shot->type) {
		case OBJ_SMART_SHOT:
			/*
			 * See Move_smart_shot() for re-lock probablities after confusion
			 * ends.
			 */
			smart = SMART_PTR(shot);
			SET_BIT(smart->status, CONFUSED);
			smart->ecm_range = range;
			smart->count = CONFUSED_TIME;
			CHECK_OBJS();
			if (pl
			  && BIT(pl->lock.tagged, LOCK_PLAYER)
			  && (pl->lock.distance <= pl->sensor_range
			  || !BIT(rules->mode, LIMITED_VISIBILITY))
			  && pl->visibility[getInd[pl->lock.pl_id]].canSee)
				smart->new_info = pl->lock.pl_id;
			else
				smart->new_info = players[(int)(rfrac() * numPlayers)]->id;
			CHECK_OBJS();
			/* Can't redirect missiles to team mates. */
			/* So let the missile keep on following this unlucky player. */
			/*-BA Why not redirect missiles to team mates?
			 *-BA It's not ideal, but better them than me...
			 *if (TEAM_IMMUNE(ind, GetInd[smart->new_info])) {
			 *	smart->new_info = ind;
			 * }
			 */
			break;

		case OBJ_MINE:
			mine = MINE_PTR(shot);
			mine->ecm_range = range;

			/*
			 * perim is distance from the mine to its detonation perimeter
			 *
			 * range is the proportion from the mine detontation perimeter
			 * to the maximum ecm range.
			 * low values of range mean the mine is close
			 *
			 * remember the closest unconfused mine -- it gets reprogrammed
			 */
			perim = MINE_RANGE / (mine->mods.mini+1);
			range = (range - perim) / (ECM_DISTANCE - perim);

			/*
			 * range%		explode%	confuse time (seconds)
			 * 100				 5		 2
			 *  50				10		 6
			 *	 0 (closest)	15		10
			 */
			if (range <= 0 || (int)(rfrac() * 100.0f) < ((int)(10*(1-range)) + 5)) {
				mine->life = 0;
				break;
			}
			mine->count = ((int)(8*(1-range)) + 2) * GetFPS();
			if (   !BIT(mine->status, CONFUSED)
			  && (closest_mine == NULL || range < closest_mine_range)) {
				closest_mine = mine;
				closest_mine_range = range;
			}
			SET_BIT(mine->status, CONFUSED);
			if (mine->count <= 0)
				CLR_BIT(mine->status, CONFUSED);
			break;
		}
	}

	CHECK_OBJS();
	/*
	 * range%		reprogram%
	 * 100				50
	 *  50				75
	 *	 0 (closest)   100
	 */
	if (options.ecmsReprogramMines->GetBool() && closest_mine != NULL) {
		range = closest_mine_range;
		if (range <= 0 || (int)(rfrac() * 100.0f) < (100 - (int)(50*range)))
			closest_mine->id = (pl ? pl->id : NO_ID);
		closest_mine->team = team;
	}

	/* in non-team mode cannons are immune to cannon ECMs */
	if (BIT(rules->mode, TEAM_PLAY) || pl) {
		for (i = 0; i < numCannons; i++) {
			Cannon *c = cannons + i;
			if (BIT(rules->mode, TEAM_PLAY)
			  && c->team == team)
				continue;
			range = WrapLength(x - c->pix_pos.x, y - c->pix_pos.y);
			if (range > ECM_DISTANCE)
				continue;
			damage = (ECM_DISTANCE - range) / ECM_DISTANCE;
			if (c->items[ITEM_LASER]) {
				c->items[ITEM_LASER] -= (int)(damage * c->items[ITEM_LASER] + 0.5);
			}
			c->damaged += (int)(24 * range * pow(0.75, c->items[ITEM_SENSOR]));
		}
	}

	for (i = 0; i < numPlayers; i++) 
	{
		p = players[i];
		if (p == pl)
			continue;
		/*
		* Team members are always immune from ECM effects from other
		* team members.  Its too nasty otherwise.
		*/
		if (BIT(rules->mode, TEAM_PLAY) && p->team == team)
			continue;

		if (pl && ALLIANCE(this, pl->Ind(), i))
			continue;

		if (BIT(p->used, HAS_PHASING_DEVICE))
			continue;

		if (BIT(p->status, PLAYING|GAME_OVER|PAUSE) == PLAYING) 
		{
			range = WrapLength(x - p->pos.x,
			y - p->pos.y);
			if (range > ECM_DISTANCE)
			continue;

			/* range is how close the player is to the center of ecm */
			range = ((ECM_DISTANCE - range) / ECM_DISTANCE);

			CHECK_OBJS();
			/*
			 * range%	damage (sec)	laser destroy%	reprogram%	drop%
			 * 100		4				75				100			25
			 * 50		2				50				75			15
			 * 0		0				25				50			5
			*/

			/*
			* should this be FPS dependant: damage = 4.0f * FPS * range; ?
			* No, i think.
			*/
			damage = 24.0f * range;

			if (p->item[ITEM_CLOAK] <= 1) {
				p->forceVisible += (int)damage;
			} else {
				p->forceVisible += (int)(damage
									* pow(0.75, (p->item[ITEM_CLOAK]-1)));
			}

			/* ECM may cause balls to detach. */
			if (BIT(p->have, HAS_BALL)) {
				for (j = 0; j < numObjs; j++) {
					shot = objs[j];
					if (BIT(shot->type, OBJ_BALL)) {
						BallObject *ball = BALL_PTR(shot);
						if (ball->owner == p->id) {
							if ((int)(rfrac() * 100.0f) < ((int)(20*range)+5)) {
								p->DetachBall(j);
							}
						}
					}
				}
			}
			CHECK_OBJS();

			/* ECM damages sensitive equipment like lasers */
			if (p->item[ITEM_LASER] > 0) {
				p->item[ITEM_LASER] -= (int)(range * p->item[ITEM_LASER] + 0.5);
			}

			CHECK_OBJS();
			if (!IS_ROBOT_PTR(p) || !options.ecmsReprogramRobots->GetBool() || !pl) {
				/* player is blinded by light flashes. */
				long duration = (int)(damage * pow(0.75, p->item[ITEM_SENSOR]));
				p->damaged += duration;
				if (pl)
					Record_shove(p, pl, frame_loops + duration);
			} else {
				if (BIT(pl->lock.tagged, LOCK_PLAYER)
				&& (pl->lock.distance < pl->sensor_range
				|| !BIT(rules->mode, LIMITED_VISIBILITY))
				&& pl->visibility[getInd[pl->lock.pl_id]].canSee
				&& pl->lock.pl_id != p->id
				/*&& !TEAM_IMMUNE(ind, GetInd[pl->lock.pl_id])*/) {

					/*
					* Player programs robot to seek target.
					*/
					CHECK_OBJS();
					this->robotMan->Program(i, pl->lock.pl_id);
					CHECK_OBJS();
					for (j = 0; j < numPlayers; j++) {
						if (players[j]->conn) {
							players[j]->conn->SendSeek(pl->id, p->id, pl->lock.pl_id);
						}
					}
				}
			}
		}
	}
}
