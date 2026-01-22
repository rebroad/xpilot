/* $Id: Cannon.cpp,v 1.15 2004/05/22 15:13:48 dick Exp $
 *
 * XPilot, a multiplayer gravity war game.  Copyright (C) 1991-2001 by
 *
 *      Bj�rn Stabell        <bjoern@xpilot.org>
 *      Ken Ronny Schouten   <ken@xpilot.org>
 *      Bert Gijsbers        <bert@xpilot.org>
 *      Dick Balaska         <dick@xpilot.org>
 *  	Guido Koopman        <guido@xpilot.org>
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
 */
/*
 *
 *  $Log: Cannon.cpp,v $
 *  Revision 1.15  2004/05/22 15:13:48  dick
 *  LASER_MODIFIERS becomes LASER_MODS
 *
 *  Revision 1.14  2004/05/04 21:07:17  dick
 *  Need randommnt.h
 *
 *  Revision 1.13  2004/04/29 16:08:27  dick
 *  compile with SOUND enabled.
 *
 *  Revision 1.12  2004/02/15 19:54:53  dick
 *  Just whitespace
 *
 *  Revision 1.11  2004/02/03 03:39:53  dick
 *  Add Windows precompiled header support.
 *
 *  Revision 1.10  2002/09/11 16:42:03  dick
 *  Here endeth the server encapsulation task.
 *  theWorld is now an array of World(s) and get loaded dynamically.
 *
 *  Revision 1.9  2002/09/01 00:13:45  dick
 *  Remove obsolete windows #include "winServer.h"
 *
 *  Revision 1.8  2002/08/01 14:41:23  dick
 *  deadwood delete, remove refs to theWorld, and retab.
 *
 *  Revision 1.7  2002/06/01 06:29:59  dick
 *  cannon.* becomes Cannon.*
 *
 *  Revision 1.6  2002/06/01 06:06:48  dick
 *  Encapsulate (almost) everything.  Get rid of (almost) all refs to theWorld.
 *
 *  Revision 1.5  2002/05/18 20:55:38  dick
 *  Update to XPilot-4.5.1
 *
 *  Revision 1.4  2001/08/14 21:37:14  dick
 *  Array Obj[] becomes objs[].  We now have a class called Obj.
 *
 *  Revision 1.3  2001/07/24 09:03:59  dick
 *  Remove all global options.  Use World.options instead.
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
#include <limits.h>

#include "version.h"
#include "config.h"
#include "serverconst.h"
#include "global.h"
#include "proto.h"
#include "bit.h"
#include "objpos.h"
#include "Cannon.h"
#include "saudio.h"
#include "commonproto.h"
#include "randommt.h"

char cannon_version[] = VERSION;


#ifdef	SOUND
#define IFSOUND(__x)	__x
#else
#define IFSOUND(__x)
#endif


/* the items that are useful to cannons.
   these are the items that cannon get 'for free' once in a while.
   cannons can get other items, but only by picking them up or
   stealing them from players. */
long CANNON_USE_ITEM = (ITEM_BIT_FUEL|ITEM_BIT_WIDEANGLE
						|ITEM_BIT_REARSHOT|ITEM_BIT_AFTERBURNER
						|ITEM_BIT_SENSOR|ITEM_BIT_TRANSPORTER
						|ITEM_BIT_TANK|ITEM_BIT_MINE
						|ITEM_BIT_ECM|ITEM_BIT_LASER
						|ITEM_BIT_EMERGENCY_THRUST|ITEM_BIT_ARMOR
						|ITEM_BIT_TRACTOR_BEAM|ITEM_BIT_MISSILE
						|ITEM_BIT_PHASING);

/* adds the given amount of an item to the cannon's inventory. the number of
   tanks is taken to be 1. amount is then the amount of fuel in that tank.
   fuel is given in 'units', but is stored in fuelpacks. */
void Cannon::AddItem(int item, int amount)
{
	switch (item) {
	case ITEM_TANK:
		items[ITEM_TANK]++;
		LIMIT(items[ITEM_TANK], 0, world->items[ITEM_TANK].limit);
		/* FALLTHROUGH */
	case ITEM_FUEL:
		items[ITEM_FUEL] += (int)(amount / (ENERGY_PACK_FUEL >> FUEL_SCALE_BITS)
							  + 0.5);
		LIMIT(items[ITEM_FUEL], 0, (int)(world->items[ITEM_FUEL].limit
									 / (ENERGY_PACK_FUEL >> FUEL_SCALE_BITS)
									 + 0.5));
		break;
	default:
		items[item] += amount;
		LIMIT(items[item], 0, world->items[item].limit);
		break;
	}
}

void Cannon::ThrowItems()
{
	int 		i, _dir;
		Object* obj;
		DFLOAT	velocity;

	for (i = 0; i < NUM_ITEMS; i++) {
		if (i == ITEM_FUEL)
			continue;
		items[i] -= world->items[i].initial;
		while (items[i] > 0) {
			int amount = world->items[i].max_per_pack
						 - (int)(rfrac() * (1 + world->items[i].max_per_pack
											- world->items[i].min_per_pack));
			LIMIT(amount, 0, items[i]);
			if (rfrac() < (world->options.dropItemOnKillProb->GetDouble() * CANNON_DROP_ITEM_PROB)
						&& (obj = Object_allocate(world)) != NULL) {

				obj->type = OBJ_ITEM;
				obj->info = i;
				obj->color = RED;
				obj->status = GRAVITY;
				_dir = (int)(dir
						   - (CANNON_SPREAD * 0.5)
						   + (rfrac() * CANNON_SPREAD));
				_dir = MOD2(_dir, RES);
				obj->id = NO_ID;
				obj->team = TEAM_NOT_SET;
				Object_position_init_pixels(obj, pix_pos.x, pix_pos.y);
				velocity = rfrac() * 6;
				obj->vel.x = tcos(_dir) * velocity;
				obj->vel.y = tsin(_dir) * velocity;
				obj->acc.x = 0;
				obj->acc.y = 0;
				obj->mass = 10;
				obj->life = 1500 + (int)(rfrac() * 512);
				obj->count = amount;
				obj->pl_range = ITEM_SIZE / 2;
				obj->pl_radius = ITEM_SIZE / 2;
				world->items[i].num++;
				world->CellAddObject(obj);
			}
			items[i] -= amount;
		}
	}
}

/* initializes the given cannon at startup or after death and gives it some
   items. */
void Cannon::Init()
{
	int 		i;

	last_change = frame_loops;
	for (i = 0; i < NUM_ITEMS; i++) {
		items[i] = 0;
		if (world->options.cannonsUseItems->GetBool())
			AddItem(i, (int)(rfrac() * (world->items[i].initial + 1)));
	}
	damaged = 0;
	tractor_target = -1;
	tractor_count = 0;
	tractor_is_pressor = false;
		used = 0;
		emergency_shield_left = 0;
		phasing_left = 0;
}

void Cannon::CheckDefense()
{
	int defense = SelectDefense();

	if (defense >= 0
				&& InDanger())
		{
				Defend(defense);
	}
}

void Cannon::CheckFire()
{
	int 		target = -1;
		int 			tdir = 0;
		int 			weapon = SelectWeapon();

	Aim(weapon, &target, &tdir);
	if (target != -1)
				Fire(weapon, target, tdir);
}

/* selects one of the available defenses. see cannon.h for descriptions. */
int Cannon::SelectDefense()
{

	if (world->options.cannonSmartness->GetInt() == 0)
		return -1;		/* mode 0 does not defend */
	if (BIT(used, HAS_EMERGENCY_SHIELD)
		|| BIT(used, HAS_PHASING_DEVICE))
		return -1;		/* still protected */
	if (items[ITEM_EMERGENCY_SHIELD])
		return CD_EM_SHIELD;
	if (items[ITEM_PHASING])
		return CD_PHASING;
	return -1;	/* no defense available */
}

/* checks if a cannon is about to be hit by a hazardous object.
   mode 0 does not detect danger.
   modes 1 - 3 use progressively more accurate detection. */
int Cannon::InDanger()
{
	const int	range = 4 * BLOCK_SZ;
	const long	kill_shots = (KILLING_SHOTS) | OBJ_MINE | OBJ_SHOT
								| OBJ_PULSE | OBJ_SMART_SHOT | OBJ_HEAT_SHOT
								| OBJ_TORPEDO | OBJ_ASTEROID;
	Object*		shot;
	Object**	obj_list;
	const int	max_objs = 100;
	int 		obj_count, i, danger = false;
	int 		npx, npy, tdx, tdy;
	int 		cpx = (int)pix_pos.x, cpy = (int)pix_pos.y;
		int 			cannonSmartness = world->options.cannonSmartness->GetInt();

	if (cannonSmartness == 0)
		return false;

	world->CellGetObjects(blk_pos.x, blk_pos.y, range, max_objs,
						  &obj_list, &obj_count);

	for (i = 0; (i < obj_count) && !danger; i++) {
		shot = obj_list[i];

		if (shot->life <= 0)
			continue;
		if (!BIT(shot->type, kill_shots))
			continue;
		if (BIT(shot->status, FROMCANNON))
			continue;
		if (BIT(world->rules->mode, TEAM_PLAY)
			&& world->options.teamImmunity->GetBool()
			&& shot->team == team)
			continue;

		npx = shot->pos.x;
		npy = shot->pos.y;
		if (cannonSmartness > 1) {
			npx += (int)shot->vel.x;
			npy += (int)shot->vel.y;
			if (cannonSmartness > 2) {
				npx += (int)shot->acc.x;
				npy += (int)shot->acc.y;
			}
		}
		tdx = WRAP_DX(world, npx - cpx);
		tdy = WRAP_DY(world, npy - cpy);
		if (LENGTH(tdx, tdy) <= ((4.5 - cannonSmartness) * BLOCK_SZ)) {
			danger = true;
			break;
		}
	}

	return danger;
}

/* activates the selected defense. */
void Cannon::Defend(int defense)
{
	IFSOUND( int sound = -1; )

	switch (defense) {
	case CD_EM_SHIELD:
		emergency_shield_left += 4 * world->GetFPS();
		SET_BIT(used, HAS_EMERGENCY_SHIELD);
		items[ITEM_EMERGENCY_SHIELD]--;
		IFSOUND( sound = EMERGENCY_SHIELD_ON_SOUND; )
		break;
	case CD_PHASING:
		phasing_left += 4 * world->GetFPS();
		SET_BIT(used, HAS_PHASING_DEVICE);
		tractor_count = 0;
		items[ITEM_PHASING]--;
		IFSOUND( sound = PHASING_ON_SOUND; )
		break;
	}
	IFSOUND( if (sound != -1)
				 SoundPlaySensors(world, pix_pos.x, pix_pos.y, sound); )
}

/* selects one of the available weapons. see cannon.h for descriptions. */
int Cannon::SelectWeapon()
{
	if (items[ITEM_MINE]
		&& rfrac() < 0.5f)
		return CW_MINE;
	if (items[ITEM_MISSILE]
		&& rfrac() < 0.5f)
		return CW_MISSILE;
	if (items[ITEM_LASER]
		&& (int)(rfrac() * (items[ITEM_LASER] + 1)))
		return CW_LASER;
	if (items[ITEM_ECM]
		&& rfrac() < 0.333f)
		return CW_ECM;
	if (items[ITEM_TRACTOR_BEAM]
		&& rfrac() < 0.5f)
		return CW_TRACTORBEAM;
	if (items[ITEM_TRANSPORTER]
		&& rfrac() < 0.333f)
		return CW_TRANSPORTER;
	if ((items[ITEM_AFTERBURNER]
		 || items[ITEM_EMERGENCY_THRUST])
		&& items[ITEM_FUEL]
		&& (int)(rfrac() * ((items[ITEM_EMERGENCY_THRUST] ?
					  MAX_AFTERBURNER :
					  items[ITEM_AFTERBURNER]) + 3)) > 2)
		return CW_GASJET;
	return CW_SHOT;
}

/* determines in which direction to fire.
   mode 0 fires straight ahead.
   mode 1 in a random direction.
   mode 2 aims at the current position of the closest player,
		  then limits that to the sector in front of the cannon,
		  then adds a small error.
   mode 3 calculates where the player will be when the shot reaches her,
		  checks if that position is within limits and selects the player
		  who will be closest in this way.
   the targeted player is also returned (for all modes).
   mode 0 always fires if it sees a player.
   modes 1 and 2 only fire if a player is within range of the selected weapon.
   mode 3 only fires if a player will be in range when the shot is expected to hit.
 */
//static void Cannon_aim(int ind, int weapon, int *target, int *dir)
void Cannon::Aim(int weapon, int* target, int* targetDir)
{
	int 		speed = world->options.shotSpeed->GetInt();
	int 		range = CANNON_SHOT_LIFE_MAX * speed;
	int 		cpx = (int)pix_pos.x;
	int 		cpy = (int)pix_pos.y;
	int 		visualrange = (int)(CANNON_DISTANCE
							  + 2 * items[ITEM_SENSOR] * BLOCK_SZ);
	bool		found = false, ready = false;
	int 		closest = range, i;
	int 		ddir;

	switch (weapon) {
	case CW_MINE:
		speed = (int)(speed * 0.5 + 0.1 * world->options.cannonSmartness->GetInt());
		range = (int)(range * 0.5 + 0.1 * world->options.cannonSmartness->GetInt());
		break;
	case CW_LASER:
		speed = PULSE_SPEED;
		range = (int)(PULSE_LIFE(CANNON_PULSES) * speed);
		break;
	case CW_ECM:
		/* smarter cannons wait a little longer before firing an ECM */
		if (world->options.cannonSmartness->GetInt() > 1) {
			range = (int)((ECM_DISTANCE / world->options.cannonSmartness->GetInt()
					 + (int)(rfrac() * (int)(ECM_DISTANCE
									   - ECM_DISTANCE / world->options.cannonSmartness->GetInt()))));
		} else {
			range = (int)ECM_DISTANCE;
		}
		break;
	case CW_TRACTORBEAM:
		range = TRACTOR_MAX_RANGE(items[ITEM_TRACTOR_BEAM]);
		break;
	case CW_TRANSPORTER:
		/* smarter cannons have a smaller chance of using a transporter when
		   target is out of range */
		if (world->options.cannonSmartness->GetInt() > 2
			|| (int)(rfrac() * sqr(world->options.cannonSmartness->GetInt() + 1)))
			range = (int)TRANSPORTER_DISTANCE;
		break;
	case CW_GASJET:
		if (items[ITEM_EMERGENCY_THRUST]) {
			speed *= 2;
			range *= 2;
		}
		break;
	}

	for (i = 0; i < world->numPlayers && !ready; i++) {
		Player* pl = world->players[i];
		int tdist, tdx, tdy;

		tdx = WRAP_DX(world, pl->pos.x - cpx);
		if (ABS(tdx) >= visualrange)
			continue;
		tdy = WRAP_DY(world, pl->pos.y - cpy);
		if (ABS(tdy) >= visualrange)
			continue;
		tdist = (int)LENGTH(tdx, tdy);
		if (tdist > visualrange)
			continue;

		/* mode 3 also checks if a player is using a phasing device */
		if (BIT(pl->status, PLAYING|GAME_OVER|PAUSE|KILLED) != PLAYING
			|| (BIT(world->rules->mode, TEAM_PLAY)
				&& pl->team == team)
			|| (!pl->forceVisible
				&& BIT(pl->used, HAS_CLOAKING_DEVICE)
				&& (int)(rfrac() * (pl->item[ITEM_CLOAK] + 1))
				   > (int)(rfrac() * (items[ITEM_SENSOR] + 1)))
			|| (world->options.cannonSmartness->GetInt() > 2
				&& BIT(pl->used, HAS_PHASING_DEVICE)))
			continue;

		switch (world->options.cannonSmartness->GetInt()) {
		case 0:
			ready = true;
			break;
		default:
		case 1:
			if (tdist < range)
				ready = true;
			break;
		case 2:
			if (tdist < closest) {
				*targetDir = (int)FindDir(tdx, tdy);
				found = true;
			}
			break;
		case 3:
			if (tdist < range) {
				DFLOAT time = tdist / speed;
				int npx = (int)(pl->pos.x
								+ pl->vel.x * time
								+ pl->acc.x * time * time);
				int npy = (int)(pl->pos.y
								+ pl->vel.y * time
								+ pl->acc.y * time * time);
				int tdir;

				tdx = WRAP_DX(world, npx - cpx);
				tdy = WRAP_DY(world, npy - cpy);
				tdir = (int)FindDir(tdx, tdy);
				ddir = MOD2(tdir - dir, RES);
				if ((ddir < (CANNON_SPREAD * 0.5)
					 || ddir > RES - (CANNON_SPREAD * 0.5))
					&& (int)LENGTH(tdx, tdy) < closest) {
					*targetDir = tdir;
					found = true;
				}
			}
			break;
		}
		if (found || ready) {
			closest = tdist;
			*target = i;
		}
	}
	if (!(found || ready)) {
		*target = -1;
		return;
	}

	switch (world->options.cannonSmartness->GetInt()) {
	case 0:
		*targetDir = dir;
		break;
	default:
	case 1:
		*targetDir = dir;
		*targetDir += (int)((rfrac() - 0.5f) * CANNON_SPREAD);
		break;
	case 2:
		ddir = MOD2(*targetDir - dir, RES);
		if (ddir > (CANNON_SPREAD * 0.5) && ddir < RES / 2) {
			*targetDir = (int)(dir + (CANNON_SPREAD * 0.5) + 3);
		} else if (ddir < RES - (CANNON_SPREAD * 0.5) && ddir > RES / 2) {
			*targetDir = (int)(dir - (CANNON_SPREAD * 0.5) - 3);
		}
		*targetDir += (int)(rfrac() * 7) - 3;
		break;
	case 3:
		/* nothing to be done for mode 3 */
		break;
	}
	*targetDir = MOD2(*targetDir, RES);
}

/* does the actual firing. also determines in which way to use weapons that
   have more than one possible use. */
void Cannon::Fire(int weapon, int target, int targetDir)
{
	Player		*pl = world->players[target];
	int 		cpx = (int)pix_pos.x;
		int 			cpy = (int)pix_pos.y;
	modifiers	mods;
	IFSOUND (int sound = CANNON_FIRE_SOUND;)
		int 			i;
	int 		speed = world->options.shotSpeed->GetInt();

	CLEAR_MODS(mods);
	switch (weapon) {
	case CW_MINE:
		if (BIT(world->rules->mode, ALLOW_CLUSTERS) && (rfrac() < 0.25f))
			SET_BIT(mods.warhead, CLUSTER);
		if (BIT(world->rules->mode, ALLOW_MODIFIERS)) {
			if (rfrac() >= 0.2f)
				SET_BIT(mods.warhead, IMPLOSION);
			mods.power = (int)(rfrac() * (MODS_POWER_MAX + 1));
			mods.velocity = (int)(rfrac() * (MODS_VELOCITY_MAX + 1));
		}
		if (rfrac() < 0.5f) {	/* place mine in front of cannon */
			world->PlaceGeneralMine(-1, team, FROMCANNON, cpx, cpy,
							   0, 0, mods);
			IFSOUND (sound = DROP_MINE_SOUND; )
		} else {				/* throw mine at player */
			if (BIT(world->rules->mode, ALLOW_MODIFIERS)) {
				mods.mini = (int)(rfrac() * MODS_MINI_MAX) + 1;
				mods.spread = (int)(rfrac() * (MODS_SPREAD_MAX + 1));
			}
			speed = (int)(speed * 0.5 + 0.1 * world->options.cannonSmartness->GetInt());
			world->PlaceGeneralMine(-1, team, GRAVITY|FROMCANNON, cpx, cpy,
							   tcos(dir) * speed, tsin(dir) * speed, mods);
			IFSOUND(sound = DROP_MOVING_MINE_SOUND;)
		}
		items[ITEM_MINE]--;
		break;
	case CW_MISSILE:
		if (BIT(world->rules->mode, ALLOW_CLUSTERS) && (rfrac() < 0.333f))
			SET_BIT(mods.warhead, CLUSTER);
		if (BIT(world->rules->mode, ALLOW_MODIFIERS)) {
			if (rfrac() >= 0.25f)
				SET_BIT(mods.warhead, IMPLOSION);
			mods.power = (int)(rfrac() * (MODS_POWER_MAX + 1));
			mods.velocity = (int)(rfrac() * (MODS_VELOCITY_MAX + 1));
			/* Because cannons don't have missile racks, all mini missiles
			   would be fired from the same point and appear to the players
			   as 1 missile (except heatseekers, which would appear to split
			   in midair because of navigation errors (see Move_smart_shot)).
			   Therefore, we don't minify cannon missiles.
			mods.mini = (int)(rfrac() * MODS_MINI_MAX) + 1;
			mods.spread = (int)(rfrac() * (MODS_SPREAD_MAX + 1));
			*/
		}
		/* smarter cannons use more advanced missile types */
		switch ((int)(rfrac() * (1 + world->options.cannonSmartness->GetInt()))) {
		default:
			if (world->options.allowSmartMissiles->GetBool()) {
				world->FireGeneralShot(-1, team, 1, cpx, cpy, OBJ_SMART_SHOT,
								  targetDir, mods, target);
				IFSOUND(sound = FIRE_SMART_SHOT_SOUND;)
				break;
			}
			/* FALLTHROUGH */
		case 1:
			if (world->options.allowHeatSeekers->GetBool()
				&& BIT(world->players[target]->status, THRUSTING)) {
				world->FireGeneralShot(-1, team, 1, cpx, cpy, OBJ_HEAT_SHOT,
								  targetDir, mods, target);
				IFSOUND(sound = FIRE_HEAT_SHOT_SOUND;)
				break;
			}
			/* FALLTHROUGH */
		case 0:
			world->FireGeneralShot(-1, team, 1, cpx, cpy, OBJ_TORPEDO,
							  targetDir, mods, -1);
			IFSOUND(sound = FIRE_TORPEDO_SOUND;)
			break;
		}
		items[ITEM_MISSILE]--;
		break;
	case CW_LASER:
		/* stun and blinding lasers are very dangerous,
		   so we don't use them often */
		if (BIT(world->rules->mode, ALLOW_LASER_MODS)
			&& (rfrac() * (8 - world->options.cannonSmartness->GetInt())) >= 1) {
			mods.laser = (int)(rfrac() * (MODS_LASER_MAX + 1));
		}
		world->FireGeneralLaser(-1, team, cpx, cpy, targetDir, mods);
		IFSOUND(sound = FIRE_LASER_SOUND;)
		break;
	case CW_ECM:
		world->FireGeneralEcm(NULL, team, cpx, cpy);
		items[ITEM_ECM]--;
		IFSOUND(sound = ECM_SOUND;)
		break;
	case CW_TRACTORBEAM:
		/* smarter cannons use tractors more often and also push/pull longer */
		tractor_is_pressor = (rfrac() * (world->options.cannonSmartness->GetInt() + 1) >= 1);
		tractor_target = pl->id;
		tractor_count = 11 + (int)(rfrac() * ((3 * world->options.cannonSmartness->GetInt()) + 1));
		IFSOUND(sound = -1;)
		break;
	case CW_TRANSPORTER:
		items[ITEM_TRANSPORTER]--;
		if ((int)world->WrapLength(pl->pos.x - cpx, pl->pos.y - cpy)
			< TRANSPORTER_DISTANCE) {
			int item = -1;
			long amount = 0;
			world->DoGeneralTransporter(NULL, cpx, cpy, target, &item, &amount);
			if (item != -1)
				AddItem(item, amount);
			IFSOUND(sound = -1;)
		} else {
			IFSOUND(sound = TRANSPORTER_FAIL_SOUND;)
		}
		break;
	case CW_GASJET:
		/* use emergency thrusts to make extra big jets */
		if ((rfrac() * (items[ITEM_EMERGENCY_THRUST] + 1)) >= 1) {
			world->MakeDebris(
				/* pos */		cpx, cpy,
				/* vel */		0, 0,
				/* id */		NO_ID,
				/* team */		team,
				/* type */		OBJ_SPARK,
				/* mass */		THRUST_MASS,
				/* status */	GRAVITY|FROMCANNON,
				/* color */ 	RED,
				/* radius */	8,
				/* number */	300, 700,
				/* dir */		targetDir - 4 * (4 - world->options.cannonSmartness->GetInt()),
								targetDir + 4 * (4 - world->options.cannonSmartness->GetInt()),
				/* speed */ 	0.1, speed * 4,
				/* life */		3, 20);
			items[ITEM_EMERGENCY_THRUST]--;
		} else {
			world->MakeDebris(
				/* pos */		cpx, cpy,
				/* vel */		0, 0,
				/* id */		NO_ID,
				/* team */		team,
				/* type */		OBJ_SPARK,
				/* mass */		THRUST_MASS,
				/* status */	GRAVITY|FROMCANNON,
				/* color */ 	RED,
				/* radius */	8,
				/* number */	150, 350,
				/* dir */		targetDir - 3 * (4 - world->options.cannonSmartness->GetInt()),
								targetDir + 3 * (4 - world->options.cannonSmartness->GetInt()),
				/* speed */ 	0.1, speed * 2,
				/* life */		3, 20);
		}
		items[ITEM_FUEL]--;
		IFSOUND(sound = THRUST_SOUND;)
		break;
	case CW_SHOT:
	default:
		if (world->options.cannonFlak->GetBool())
			mods.warhead = CLUSTER;
		/* smarter cannons fire more accurately and
		   can therefore narrow their bullet streams */
		for (i = 0; i < (1 + 2 * items[ITEM_WIDEANGLE]); i++) {
			int a_dir = targetDir
						+ (4 - world->options.cannonSmartness->GetInt())
						* (-items[ITEM_WIDEANGLE] +  i);
			a_dir = MOD2(a_dir, RES);
			world->FireGeneralShot(-1, team, 1, cpx, cpy, OBJ_SHOT,
							  a_dir, mods, -1);
		}
		/* I'm not sure cannons should use rearshots.
		   After all, they are restricted to 60 degrees when picking their
		   target. */
		for (i = 0; i < items[ITEM_REARSHOT]; i++) {
			int a_dir = (int)(dir + (RES / 2)
						+ (4 - world->options.cannonSmartness->GetInt())
						* (-((items[ITEM_REARSHOT] - 1) * 0.5) + i));
			a_dir = MOD2(a_dir, RES);
			world->FireGeneralShot(-1, team, 1, cpx, cpy, OBJ_SHOT,
							  a_dir, mods, -1);
		}
	}

	/* finally, play sound effect */
	IFSOUND(if (sound != -1)
				SoundPlaySensors(world, cpx, cpy, sound);)
}
