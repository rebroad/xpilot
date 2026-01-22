/* $Id: shot.cpp,v 1.27 2004/05/04 21:07:17 dick Exp $
 *
 * XPilot, a multiplayer gravity war game.  Copyright (C) 1991-2002 by
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
 *  $Log: shot.cpp,v $
 *  Revision 1.27  2004/05/04 21:07:17  dick
 *  Need randommnt.h
 *
 *  Revision 1.26  2004/04/29 16:08:27  dick
 *  compile with SOUND enabled.
 *
 *  Revision 1.25  2004/02/24 07:01:35  dick
 *  Whitespace
 *
 *  Revision 1.24  2004/02/20 06:27:47  dick
 *  SendPlayMessage becomes BroadcastPlayMessage() because it sends a play message
 *  to all connections.
 *
 *  Revision 1.23  2004/02/18 01:22:14  dick
 *  Remove extraneous and obsolete #include "netserver.h"
 *
 *  Revision 1.22  2004/02/15 19:54:53  dick
 *  Just whitespace
 *
 *  Revision 1.21  2004/02/03 03:39:54  dick
 *  Add Windows precompiled header support.
 *
 *  Revision 1.20  2004/01/17 19:37:17  dick
 *  WildMap is now in the common library. Control and server use this,
 *  instead of each having it's own copy.
 *
 *  Revision 1.19  2002/09/11 16:42:04  dick
 *  Here endeth the server encapsulation task.
 *  theWorld is now an array of World(s) and get loaded dynamically.
 *
 *  Revision 1.18  2002/09/01 00:13:46  dick
 *  Remove obsolete windows #include "winServer.h"
 *
 *  Revision 1.17  2002/08/31 22:20:10  dick
 *  Remove more references to static world.
 *
 *  Revision 1.16  2002/08/01 14:29:32  dick
 *  Remove reference to theWorld.
 *
 *  Revision 1.15  2002/07/08 05:52:59  dick
 *  showtime() handles the space after itself
 *
 *  Revision 1.14  2002/06/30 06:43:23  dick
 *  SendMessage is a bad word.  It's a Windows function that is a macro and gets
 *  translated to SendMessageA or SendMessageW.  Although it works, renaming ours
 *  to SendPlayMessage makes browsing the source code easier.
 *
 *  Revision 1.13  2002/06/01 06:29:59  dick
 *  cannon.* becomes Cannon.*
 *
 *  Revision 1.12  2002/06/01 06:06:48  dick
 *  Encapsulate (almost) everything.  Get rid of (almost) all refs to theWorld.
 *
 *  Revision 1.11  2002/05/22 05:09:01  dick
 *  Update to XPilot r4_5_4alpha.
 *
 *  Revision 1.10  2002/05/20 07:32:13  dick
 *  Integration bug: missle firing
 *
 *  Revision 1.9  2002/05/18 22:34:15  dick
 *  Move showtime to common.
 *
 *  Revision 1.8  2002/05/18 20:55:39  dick
 *  Update to XPilot-4.5.1
 *
 *  Revision 1.7  2001/08/14 21:37:14  dick
 *  Array Obj[] becomes objs[].  We now have a class called Obj.
 *
 *  Revision 1.6  2001/08/09 07:42:59  dick
 *  SetMessage becomes member function of Player.
 *
 *  Revision 1.5  2001/07/24 09:04:00  dick
 *  Remove all global options.  Use World.options instead.
 *
 *  Revision 1.4  2001/07/23 21:08:37  dick
 *  Encapsulate ShipMass, ShotsMax, ShotsLife, ShotsMass, FPS.
 *
 *  Revision 1.3  2001/07/12 07:57:42  dick
 *  Use MineObject as our biggest object
 *
 *  Revision 1.2  2001/07/07 12:00:43  dick
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
#include "global.h"
#include "proto.h"
#include "showtime.h"
#include "score.h"
#include "saudio.h"
#include "Cannon.h"
#include "asteroid.h"
#include "objpos.h"
#include "error.h"
#include "commonproto.h"
#include "randommt.h"

char shot_version[] = VERSION;


#define MISSILE_POWER_SPEED_FACT	0.25
#define MISSILE_POWER_TURNSPEED_FACT	0.75
#define MINI_TORPEDO_SPREAD_TIME	6
#define MINI_TORPEDO_SPREAD_SPEED	20
#define MINI_TORPEDO_SPREAD_ANGLE	90
#define MINI_MINE_SPREAD_TIME		18
#define MINI_MINE_SPREAD_SPEED		8
#define MINI_MISSILE_SPREAD_ANGLE	45

#define CONFUSED_UPDATE_GRANULARITY	10
#define CONFUSED_TIME			3


/***********************
 * Functions for shots.
 */

void Player::PlaceMine()
{

	if (item[ITEM_MINE] <= 0
		|| (BIT(used, HAS_SHIELD|HAS_PHASING_DEVICE) && !world->options.shieldedMining->GetBool())) {
		return;
	}

	if (world->options.minMineSpeed->GetDouble() > 0) {
		PlaceMovingMine();
		return;
	}

	world->PlaceGeneralMine(Ind(), team, 0,
					   pos.x, pos.y, 0.0, 0.0, mods);
}


///////////////////////////////////////////////////////////////////////////////
void Player::PlaceMovingMine()
{
	DFLOAT		vx = vel.x;
	DFLOAT		vy = vel.y;

	if (item[ITEM_MINE] <= 0
		|| (BIT(used, HAS_SHIELD|HAS_PHASING_DEVICE) && !world->options.shieldedMining->GetBool())) {
		return;
	}

	if (world->options.minMineSpeed->GetDouble() > 0) {
		if (velocity < world->options.minMineSpeed->GetDouble()) {
			if (velocity >= 1) {
				vx *= (world->options.minMineSpeed->GetDouble() / velocity);
				vy *= (world->options.minMineSpeed->GetDouble() / velocity);
			}
			else {
				vx = world->options.minMineSpeed->GetDouble() * tcos(dir);
				vy = world->options.minMineSpeed->GetDouble() * tsin(dir);
			}
		}
	}

	world->PlaceGeneralMine(Ind(), team, GRAVITY,
					   pos.x, pos.y, vx, vy, mods);
}

///////////////////////////////////////////////////////////////////////////////
void World::PlaceGeneralMine(int ind, unsigned short team, long status,
						DFLOAT x, DFLOAT y,
						DFLOAT vx, DFLOAT vy, modifiers mods)
{
	char				msg[MSG_LEN];
	Player* 			pl = (ind == -1 ? NULL : players[ind]);
	int 				used, life;
	long				drain;
	DFLOAT				mass;
	int 				i, minis;
	vector				mv;

	if (numObjs + mods.mini >= MAX_TOTAL_SHOTS)
		return;
	if (BIT(rules->mode, WRAP_PLAY)) {
		if (x < 0) x += width;
		else if (x >= width) x -= width;
		if (y < 0) y += height;
		else if (y >= height) y -= height;
	}
	if (x < 0 || x >= width || y < 0 || y >= height) {
		return;
	}

	if (pl && BIT(pl->status, KILLED)) {
		life = (int)(rfrac() * GetFPS());
	} else if (BIT(status, FROMCANNON)) {
		life = CANNON_SHOT_LIFE;
	} else {
		life = (options.mineLife->GetInt() 
				  ? options.mineLife->GetInt() : MINE_LIFETIME);
	}

	if (!BIT(mods.warhead, CLUSTER))
		mods.velocity = 0;
	if (!mods.mini)
		mods.spread = 0;

	if (options.nukeMinSmarts->GetInt() <= 0) {
		CLR_BIT(mods.nuclear, NUCLEAR);
	}
	if (BIT(mods.nuclear, NUCLEAR)) {
		if (pl) {
			used = (BIT(mods.nuclear, FULLNUCLEAR)
					? pl->item[ITEM_MINE]
					: options.nukeMinMines->GetInt());
			if (pl->item[ITEM_MINE] < options.nukeMinMines->GetInt()) {
				sprintf(msg, "You need at least %d mines to %s %s!",
						options.nukeMinMines->GetInt(),
						(BIT(status, GRAVITY) ? "throw" : "drop"),
						Describe_shot (OBJ_MINE, status, mods, 0));
				pl->SetMessage(msg);
				return;
			}
		} else {
			used = options.nukeMinMines->GetInt();
		}
		mass = MINE_MASS * used * NUKE_MASS_MULT;
	} else {
		mass = (BIT(status, FROMCANNON) ? MINE_MASS * 0.6 : MINE_MASS);
		used = 1;
	}

	if (pl) {
		drain = ED_MINE;
		if (BIT(mods.warhead, CLUSTER)) {
			drain += (long)(CLUSTER_MASS_DRAIN(this, mass));
		}
		if (pl->fuel.sum < -drain) {
			sprintf(msg, "You need at least %ld fuel units to %s %s!",
					(-drain) >> FUEL_SCALE_BITS,
					(BIT(status, GRAVITY) ? "throw" : "drop"),
					Describe_shot(OBJ_MINE, status, mods, 0));
			pl->SetMessage(msg);
			return;
		}
		if (options.baseMineRange->GetInt()) {
			for (i = 0; i < numPlayers; i++) {
				if (i != ind
					&& !pl->TeamImmune(players[i]->id)
					&& !IS_TANK_IND(this, i)) {
					int dx = (int)(x/BLOCK_SZ - bases[players[i]->home_base].pos.x);
					int dy = (int)(y/BLOCK_SZ - bases[players[i]->home_base].pos.y);
					if (sqr(dx) + sqr(dy) <= sqr(options.baseMineRange->GetInt())) {
						pl->SetMessage("No base mining!");
						return;
					}
				}
			}
		}
		Add_fuel(&(pl->fuel), drain);
		pl->item[ITEM_MINE] -= used;

		if (used > 1) {
			sprintf(msg, "%s has %s %s!", pl->name,
					(BIT(status, GRAVITY) ? "thrown" : "dropped"),
					Describe_shot(OBJ_MINE, status, mods, 0));
			BroadcastPlayMessage(msg);
			SoundPlayAll(this, NUKE_LAUNCH_SOUND);
		} else {
			SoundPlaySensors(this, pl->pos.x, pl->pos.y,
			  BIT(status, GRAVITY) ? DROP_MOVING_MINE_SOUND : DROP_MINE_SOUND);
		}
	}

	minis = (mods.mini + 1);
	SET_BIT(status, OWNERIMMUNE);

	for (i = 0; i < minis; i++) {
		MineObject* 	mine;
		if ((mine = MINE_PTR(Object_allocate(this))) == NULL) {
				break;
		}

		mine->type = OBJ_MINE;
		mine->color = BLUE;
		mine->info = options.mineFuseTime->GetInt();
		mine->status = status;
		mine->id = (pl ? pl->id : NO_ID);
		mine->team = team;
		mine->owner = mine->id;
		Object_position_init_pixels(OBJ_PTR(mine), x, y);
		if (minis > 1) {
			int 		space = RES/minis;
			int 		dir;
			DFLOAT		spread;

			spread = (DFLOAT)((unsigned)mods.spread + 1);
			/*
			 * Dir gives (S is ship upwards);
			 *
			 *						  o 			o	o
			 *	X2: o S o		X3:   S 		X4:   S
			 *						o	o			o	o
			 */
			dir = (i * space) + space/2 + (minis-2)*(RES/2) + (pl?pl->dir:0);
			dir += (int)((rfrac() - 0.5f) * space * 0.5f);
			dir = MOD2(dir, RES);
			mv.x = MINI_MINE_SPREAD_SPEED * tcos(dir) / spread;
			mv.y = MINI_MINE_SPREAD_SPEED * tsin(dir) / spread;
			/*
			 * This causes the added initial velocity to reduce to
			 * zero over the MINI_MINE_SPREAD_TIME.
			 */
			mine->spread_left = MINI_MINE_SPREAD_TIME;
			mine->acc.x = -mv.x / MINI_MINE_SPREAD_TIME;
			mine->acc.y = -mv.y / MINI_MINE_SPREAD_TIME;
		} else {
			mv.x = mv.y = mine->acc.x = mine->acc.y = 0.0;
			mine->spread_left = 0;
		}
		mine->vel = mv;
		mine->vel.x += vx * MINE_SPEED_FACT;
		mine->vel.y += vy * MINE_SPEED_FACT;
		mine->mass = mass / minis;
		mine->life = life / minis;
		mine->mods = mods;
		mine->pl_range = (int)(MINE_RANGE / minis);
		mine->pl_radius = MINE_RADIUS;
		CellAddObject(mine);
	}
}

///////////////////////////////////////////////////////////////////////////////
/*
 * Up to and including 3.2.6 it was:
 *	   Cause all of the given player's dropped/thrown mines to explode.
 * Since this caused a slowdown when many mines detonated it
 * is changed into:
 *	   Cause the mine which is closest to a player and owned
 *	   by that player to detonate.
 */
void Player::DetonateMines()
{
	int 				i;
	int 				closest = -1;
	DFLOAT				dist;
	DFLOAT				min_dist = world->hypotenuse + 1;

	if (BIT(used, HAS_PHASING_DEVICE))
		return;

	for (i = 0; i < world->numObjs; i++) {
		Object *mine = world->objs[i];

		if (! BIT(mine->type, OBJ_MINE))
			continue;
		/*
		 * Mines which have been ECM reprogrammed should only be detonatable
		 * by the reprogrammer, not by the original mine placer:
		 */
		if (mine->id == id) {
			dist = world->WrapLength(pos.x - mine->pos.x, pos.y - mine->pos.y);
			if (dist < min_dist) {
				min_dist = dist;
				closest = i;
			}
		}
	}
	if (closest != -1) {
				world->objs[closest]->life = 0;
	}

	return;
}

///////////////////////////////////////////////////////////////////////////////
void World::MakeTreasureBall(int treasure)
{
	BallObject *ball;
	Treasure *t = &(treasures[treasure]);
	DFLOAT		x = (t->pos.x + 0.5) * BLOCK_SZ,
				y = (t->pos.y * BLOCK_SZ) + 10;

	if (t->empty)
		return;
	if (t->have) {
		xpprintf ("%sFailed Make_treasure_ball(treasure=%d):\n",
				showtime(), treasure);
		xpprintf ("\ttreasure: destroyed = %d, team = %d, have = %d\n",
				t->destroyed, t->team, t->have);
		return;
	}

	if ((ball = BALL_PTR(Object_allocate(this))) == NULL) {
		return;
	}

	ball->length = options.ballConnectorLength->GetDouble();
	ball->life = LONG_MAX;
	ball->mass = options.ballMass->GetDouble();
	ball->vel.x = 0;			/* make the ball stuck a little */
	ball->vel.y = 0;			/* longer to the ground */
	ball->acc.x = 0;
	ball->acc.y = 0;
	Object_position_init_pixels(OBJ_PTR(ball), x, y);
	ball->id = NO_ID;
	ball->owner = NO_ID;
	ball->team = t->team;
	ball->type = OBJ_BALL;
	ball->color = WHITE;
	ball->count = 0;
	ball->pl_range = BALL_RADIUS;
	ball->pl_radius = BALL_RADIUS;
	CLEAR_MODS(ball->mods);
	ball->status = RECREATE;
	ball->treasure = treasure;
	CellAddObject(ball);

	t->have = true;
}


///////////////////////////////////////////////////////////////////////////////
/*
 * Describes shot of `type' which has `status' and `mods'.	If `hit' is
 * non-zero this description is part of a collision, otherwise its part
 * of a launch message.
 */
char *Describe_shot(int type, long status, modifiers mods, int hit)
{
	const char			*name, *howmany = "a ", *plural = "";
	static char 		msg[MSG_LEN];

	switch (type) {
	case OBJ_MINE:
		if (BIT(status, GRAVITY))
			name = "bomb";
		else
			name = "mine";
		break;
	case OBJ_SMART_SHOT:
		name = "smart missile";
		break;
	case OBJ_TORPEDO:
		name = "torpedo";
		break;
	case OBJ_HEAT_SHOT:
		name = "heatseeker";
		break;
	case OBJ_CANNON_SHOT:
		if (BIT(mods.warhead, CLUSTER)) {
				howmany = "";
				name = "flak";
		} else {
				name = "shot";
		}
		break;
	default:
		/*
		 * Cluster shots are actual debris from a cluster explosion
		 * so we describe it as "cluster debris".
		 */
		if (BIT(mods.warhead, CLUSTER)) {
			howmany = "";
			name = "debris";
		} else {
			name = "shot";
		}
		break;
	}

	if (mods.mini && !hit) {
		howmany = "some ";
		plural = (type == OBJ_TORPEDO) ? "es" : "s";
	}

	sprintf (msg, "%s%s%s%s%s%s%s%s%s",
			 howmany,
			 ((mods.velocity || mods.spread || mods.power) ? "modified " : ""),
			 (mods.mini ? "mini " : ""),
			 (BIT(mods.nuclear, FULLNUCLEAR) ? "full " : ""),
			 (BIT(mods.nuclear, NUCLEAR) ? "nuclear " : ""),
			 (BIT(mods.warhead, IMPLOSION) ? "imploding " : ""),
			 (BIT(mods.warhead, CLUSTER) ? "cluster " : ""),
			 name,
			 plural);

	return msg;
}

///////////////////////////////////////////////////////////////////////////////
void Player::FireMainShot(int type, int dir)
{
	DFLOAT x,
		  y;

	if (shots >= shot_max || BIT(used, HAS_SHIELD|HAS_PHASING_DEVICE))
		return;

	x = pos.x + ship->m_gun[dir].x;
	y = pos.y + ship->m_gun[dir].y;

	world->FireGeneralShot(Ind(), team, 0, x, y, type, dir, mods, -1);
}

///////////////////////////////////////////////////////////////////////////////
void Player::FireShot(int type, int dir)
{

	if (shots >= shot_max || BIT(used, HAS_SHIELD|HAS_PHASING_DEVICE))
		return;

	world->FireGeneralShot(Ind(), team, 0, pos.x, pos.y,
										  type, dir, mods, -1);
}

///////////////////////////////////////////////////////////////////////////////
void Player::FireLeftShot(int type, int dir, int gun)
{
	DFLOAT x,
		  y;

	if (shots >= shot_max || BIT(used, HAS_SHIELD|HAS_PHASING_DEVICE))
		return;

	x = pos.x + ship->l_gun[gun][dir].x;
	y = pos.y + ship->l_gun[gun][dir].y;

	world->FireGeneralShot(Ind(), team, 0, x, y, type, dir, mods, -1);

}

///////////////////////////////////////////////////////////////////////////////
void Player::FireRightShot(int type, int dir, int gun)
{
	DFLOAT x,
		  y;

	if (shots >= shot_max || BIT(used, HAS_SHIELD|HAS_PHASING_DEVICE))
		return;

	x = pos.x + ship->r_gun[gun][dir].x;
	y = pos.y + ship->r_gun[gun][dir].y;

	world->FireGeneralShot(Ind(), team, 0, x, y, type, dir, mods, -1);

}

///////////////////////////////////////////////////////////////////////////////
void Player::FireLeftRShot(int type, int dir, int gun)
{
	DFLOAT x,
		  y;

	if (shots >= shot_max || BIT(used, HAS_SHIELD|HAS_PHASING_DEVICE))
		return;

	x = pos.x + ship->l_rgun[gun][dir].x;
	y = pos.y + ship->l_rgun[gun][dir].y;

	world->FireGeneralShot(Ind(), team, 0, x, y, type, dir, mods, -1);

}

///////////////////////////////////////////////////////////////////////////////
void Player::FireRightRShot(int type, int dir, int gun)
{
	DFLOAT x,
		  y;

	if (shots >= shot_max || BIT(used, HAS_SHIELD|HAS_PHASING_DEVICE))
		return;

	x = pos.x + ship->r_rgun[gun][dir].x;
	y = pos.y + ship->r_rgun[gun][dir].y;

	world->FireGeneralShot(Ind(), team, 0, x, y, type, dir, mods, -1);

}

///////////////////////////////////////////////////////////////////////////////
void World::FireGeneralShot(int ind, unsigned short team, bool cannon,
							DFLOAT x, DFLOAT y,
							int type, int dir, 
							modifiers mods, int target)
{
	char		msg[MSG_LEN];
	Player* 	pl = (ind == -1 ? NULL : players[ind]);
	int 		used;
	int 		life = options.shotLife->GetInt();
	int 		fuse = 0;
	int 		lock = 0;
	int 		status = GRAVITY;
	int 		i;
	int 		ldir;
	int 		minis;
	int 		pl_range;
	int 		pl_radius;
	int 		rack_no = 0;
	int 		racks_left = 0;
	int 		r;
	int 		on_this_rack = 0;
	int 		side = 0;
	int 		fired = 0;
	long		drain;
	DFLOAT		mass = options.shotMass->GetDouble();
	DFLOAT		speed = options.shotSpeed->GetDouble();
	DFLOAT		turnspeed = 0;
	DFLOAT		max_speed = SPEED_LIMIT;
	DFLOAT		angle;
	DFLOAT		spread;
	vector		mv;
	position	shotpos;
	Object* 	mini_objs[MODS_MINI_MAX+1];


	if (numObjs >= MAX_TOTAL_SHOTS)
		return;

	if (!BIT(mods.warhead, CLUSTER))
		mods.velocity = 0;
	if (!mods.mini)
		mods.spread = 0;

		if (cannon) {
			mass = CANNON_SHOT_MASS;
			life = CANNON_SHOT_LIFE;
			SET_BIT(status, FROMCANNON);
		}

	switch (type) {
	default:
		return;

	case OBJ_SHOT:
				CLEAR_MODS(mods);		/* Shots can't be modified! */
				/* FALLTHROUGH */
	case OBJ_CANNON_SHOT:
		pl_range = pl_radius = 0;
		if (pl) {
			if (pl->fuel.sum < -ED_SHOT)
				return;
			Add_fuel(&(pl->fuel), (long)(ED_SHOT));
			SoundPlaySensors(this, pl->pos.x, pl->pos.y, FIRE_SHOT_SOUND);
			pl->shots++;
		}
		if (!options.shotsGravity->GetBool()) {
			CLR_BIT(status, GRAVITY);
		}
		break;

	case OBJ_SMART_SHOT:
	case OBJ_HEAT_SHOT:
		if ((type == OBJ_HEAT_SHOT) 
						? !options.allowHeatSeekers->GetBool() 
						: !options.allowSmartMissiles->GetBool()) {
			if (options.allowTorpedoes->GetBool()) {
				type = OBJ_TORPEDO;
			} else {
				return;
			}
		}
		/* FALLTHROUGH */
	case OBJ_TORPEDO:
		/*
		 * Make sure there are enough object entries for the mini shots.
		 */
		if (numObjs + mods.mini >= MAX_TOTAL_SHOTS)
			return;

		if (pl && pl->item[ITEM_MISSILE] <= 0)
			return;

		if (options.nukeMinSmarts->GetInt() <= 0) {
			CLR_BIT(mods.nuclear, NUCLEAR);
		}
		if (BIT(mods.nuclear, NUCLEAR)) {
			if (pl) {
				used = (BIT(mods.nuclear, FULLNUCLEAR)
						? pl->item[ITEM_MISSILE]
						: options.nukeMinSmarts->GetInt());
				if (pl->item[ITEM_MISSILE] < options.nukeMinSmarts->GetInt()) {
					sprintf(msg,
							"You need at least %d missiles to fire %s!",
							options.nukeMinSmarts->GetInt(),
							Describe_shot (type, status, mods, 0));
					pl->SetMessage(msg);
					return;
				}
			} else {
				used = options.nukeMinSmarts->GetInt();
			}
			mass = MISSILE_MASS * used * NUKE_MASS_MULT;
			pl_range = (type == OBJ_TORPEDO) ? (int)NUKE_RANGE : MISSILE_RANGE;
		} else {
			mass = MISSILE_MASS;
			used = 1;
			pl_range = (type == OBJ_TORPEDO) ? (int)TORPEDO_RANGE : MISSILE_RANGE;
		}
		pl_range /= mods.mini + 1;
		pl_radius = MISSILE_LEN;

		drain = used * ED_SMART_SHOT;
		if (BIT(mods.warhead, CLUSTER)) {
			if (pl)
				drain += (long)(CLUSTER_MASS_DRAIN(this, mass));
		}

		if (pl && BIT(pl->status, KILLED)) {
			life = (int)(rfrac() * GetFPS());
		} else if (!cannon) {
			life = (options.missileLife->GetInt()
						  ? options.missileLife->GetInt() : MISSILE_LIFETIME);
		}

		switch (type) {
		case OBJ_HEAT_SHOT:
#ifndef HEAT_LOCK
			lock = -1;
#else  /* HEAT_LOCK */
			if (pl == NULL) {
				lock = target;
			} else {
				if (!BIT(pl->lock.tagged, LOCK_PLAYER)
				|| ((pl->lock.distance > pl->sensor_range)
					&& BIT(rules->mode, LIMITED_VISIBILITY))) {
					lock = -1;
				} else {
					lock = pl->lock.pl_id;
				}
			}
#endif /* HEAT_LOCK */
			if (pl) {
				SoundPlaySensors(this, pl->pos.x, pl->pos.y, FIRE_HEAT_SHOT_SOUND);
			}
			max_speed = SMART_SHOT_MAX_SPEED * HEAT_SPEED_FACT;
			turnspeed = SMART_TURNSPEED * HEAT_SPEED_FACT;
			speed *= HEAT_SPEED_FACT;
			break;

		case OBJ_SMART_SHOT:
			if (pl == NULL) {
				lock = -1;
			} else {
				if (!BIT(pl->lock.tagged, LOCK_PLAYER)
				|| ((pl->lock.distance > pl->sensor_range)
					&& BIT(rules->mode, LIMITED_VISIBILITY))
				|| !pl->visibility[getInd[pl->lock.pl_id]].canSee)
					return;
				lock = pl->lock.pl_id;
				//D(
				//	if (type == OBJ_SMART_SHOT)
				//		xpprintf("%s %s (%d) fired a smart at %s (%d)\n", showtime(),
				//			pl->name, pl->Ind(), players[getInd[lock]]->name, getInd[lock]);
				//)
			}
			max_speed = SMART_SHOT_MAX_SPEED;
			turnspeed = SMART_TURNSPEED;
			break;

		case OBJ_TORPEDO:
			lock = -1;
			fuse = 8;
			break;
		}

		if (pl) {
			if (pl->fuel.sum < -drain) {
				sprintf(msg, "You need at least %ld fuel units to fire %s!",
						(-drain) >> FUEL_SCALE_BITS,
						Describe_shot(type, status, mods, 0));
				pl->SetMessage(msg);
				return;
			}
			Add_fuel(&(pl->fuel), drain);
			pl->item[ITEM_MISSILE] -= used;

			if (used > 1) {
				sprintf(msg, "%s has launched %s!", pl->name,
						Describe_shot(type, status, mods, 0));
				BroadcastPlayMessage(msg);
				SoundPlayAll(this, NUKE_LAUNCH_SOUND);
			} else if (type == OBJ_SMART_SHOT) {
				SoundPlaySensors(this, pl->pos.x, pl->pos.y, FIRE_SMART_SHOT_SOUND);
			} else if (type == OBJ_TORPEDO) {
				SoundPlaySensors(this, pl->pos.x, pl->pos.y, FIRE_TORPEDO_SOUND);
			}
		}
		break;
	}

	minis = (mods.mini + 1);
	speed *= (1 + (mods.power * MISSILE_POWER_SPEED_FACT));
	max_speed *= (1 + (mods.power * MISSILE_POWER_SPEED_FACT));
	turnspeed *= (1 + (mods.power * MISSILE_POWER_TURNSPEED_FACT));
	spread = (DFLOAT)((unsigned)mods.spread + 1);
	/*
	 * Calculate the maximum time it would take to cross one ships width,
	 * don't fuse the shot/missile/torpedo for the owner only until that
	 * time passes.  This is a hack to stop various odd missile and shot
	 * mounting points killing the player when they're firing.
	 */
	fuse += (int)((2.0 * (DFLOAT)SHIP_SZ) / speed + 1.0);

	/*
	 *					Missile Racks and Spread
	 *					------------------------
	 *
	 *				A short story by H. J. Thompson
	 *
	 * Once upon a time, back in the "good old days" of XPilot, it was
	 * relatively easy thing to remember the few keys needed to fly and shoot.
	 * It was the day of Sopwith Camels biplanes, albeit triangular ones,
	 * doing close to-the-death machine gun combat with other triangular
	 * Red Barons, the hard vacuum of space whistling silently by as only
	 * something that doesn't exist could do (this was later augmented by
	 * artificial aural feedback devices on certain advanced hardware).
	 *
	 * Eventually the weapon designers came up with "smart" missiles, and
	 * another key was added to the control board, causing one missile to
	 * launch straight forwards from the front of the triangular ship.
	 * Soon other types of missiles were added, including "heat" seekers,
	 * and fast straight travelling "torpedoes" (hark, is that the sonorous
	 * ping-ping-ping of sonar equipment I hear?).
	 *
	 * Then one day along came a certain fellow who thought, among other
	 * things, that it would be neat to fire up to four missiles with one
	 * key press, just so the enemy pilot would be scared witless by the
	 * sudden appearance of four missiles hot on their tail.  To make things
	 * fair these "mini" missiles would have the same total damage of a
	 * normal missile, but would travel at the speed of a normal missile.
	 *
	 * However this fellow mused that simply launching all the missiles in
	 * the same direction and from the same point would cause the missiles
	 * to appear on top of each other.	Thus he added code to "spread" the
	 * missiles out at various angular offsets from the ship.  Indeed the
	 * angular offsets could be controlled using a spread modifier, and yet
	 * more keys appeared on a now crowded control desk.
	 *
	 * Interestingly the future would see the same fellow adding a two seater
	 * variant of the standard single seater ship, allowing one person
	 * to concentrate on flying the ship, while another could flick through
	 * out-of-date manuals searching for the right key combinations on
	 * the now huge console which would launch four full nuclear slow-cluster
	 * imploding mini super speed close spread torpedoes at the currently
	 * targetted enemy, and then engage emergency thrust and shields before
	 * the ominous looking tri-winged dagger ship recoiled at high velocity
	 * into a rocky wall half way across the other side of the universe.
	 *
	 * Back to our story, and this same fellow was musing at the design of
	 * multiple "mini" missiles, and noted that the angle of launch would
	 * also require a different launch point on the ship (actually it was
	 * the same position as if the front of the ship was rotated to point in
	 * the direction of missile launch, mainly because it was easier to
	 * write the launch/guidance computer software that way).
	 *
	 * Later, some artistically (or sadistically) minded person decided that
	 * triangular ships just didn't look good (even though they were very
	 * spatially dynamic, cheap and easy to build), and wouldn't it be just
	 * fantastic if one could have a ship shaped like a banana!  Sensibly,
	 * however, he restricted missiles and guns to the normal single frontal
	 * launching point.
	 *
	 * A few weeks later, somebody else decided how visually pleasing it
	 * would be if one could design where missiles could be fired from by
	 * adding "missile rack" points on the ship.  Up to four racks were
	 * available, and missiles would fire from exactly these points on the
	 * ship.  Since one to four missiles could be fired in one go, the
	 * combinations with various ship designs were numerous (16).
	 *
	 * What would happen if somebody fired four missiles in one go, from a
	 * ship that only had three missile racks?	How about two missiles from
	 * one with four racks?  Sadly the missile launch software hadn't been
	 * designed to take this sort of thing into account, and incredibly the
	 * original programmer wasn't notified until after First Customer Ship
	 * [sic], the launch software only slightly modified by the ship
	 * designer, who didn't know the first thing about launch acceleration
	 * curves or electronic owner immunity fuse timers.
	 *
	 * Pilots found their missiles were being fired from random points and
	 * in sometimes very odd directions, occasionally even destroying the
	 * ship without trace, severely annoying the ship's owners and several
	 * insurance underwriters.	Not soon after several ship designers were
	 * mysteriously killed in a freak "accident" involving a stray nuclear
	 * cluster bomb, and the remaining ship designers became very careful
	 * to place missile racks and extra gun turrets well away from the
	 * ship's superstructure.
	 *
	 * The original programmer who invented multiple "mini" spreading
	 * missiles quickly decided to revisit his code before any "accidents"
	 * came his way, and spent a good few hours making sure one couldn't
	 * shoot oneself in the "foot", and that missiles where launched in some
	 * reasonable and sensible directions based on the position of the
	 * missile racks.
	 *
	 *					How It Actually Works
	 *					---------------------
	 *
	 * The first obstacle is getting the right number of missiles fired
	 * from each combination of missile rack configurations;
	 *
	 *
	 *			Minis	1		2		3		4
	 * Racks
	 *	1				1		2		3		4
	 *
	 *	2				1/- 	1/1 	2/1 	2/2
	 *					-/1 			1/2
	 *
	 *	3				1/-/-	1/1/-	1/1/1	2/1/1
	 *					-/1/-	-/1/1			1/2/1
	 *					-/-/1	1/-/1			1/1/2
	 *
	 *	4				1/-/-/- 1/1/-/- 1/1/1/- 1/1/1/1
	 *					-/1/-/- -/1/1/- -/1/1/1
	 *					-/-/1/- -/-/1/1 1/-/1/1
	 *					-/-/-/1 1/-/-/1 1/1/-/1
	 *
	 * To read; For example with 2 Minis and 3 Racks, the first round will
	 * fire 1/1/-, which is one missile from left and middle racks.  The
	 * next time fired will be -/1/1; middle and right, next fire is
	 * 1/-/1; left and right.  Next time goes to the beggining state.
	 *
	 *					Comment Point 1
	 *					---------------
	 *
	 * The *starting* rack number for each salvo cycles through the number
	 * of missiles racks.  This is stored in the player variable
	 * `pl->missile_rack', and is only incremented after each salvo (not
	 * after each mini missile is fired).  This value is used to initialise
	 * `rack_no', which stores the current rack that missiles are fired from.
	 *
	 * `on_this_rack' is computed to be the number of missiles that will be
	 * fired from `rack_no', and `r' is used as a counter to this value.
	 *
	 * `racks_left' count how many unused missiles racks are left on the ship
	 * in this mini missile salvo.
	 *
	 *					Comment Point 2
	 *					---------------
	 *
	 * When `r' reaches `on_this_rack' all the missiles have been fired for
	 * this rack, and the next rack should be used.  `rack_no' is incremented
	 * modulo the number of available racks, and `racks_left' is decremented.
	 * At this point `on_this_rack' is recomputed for the next rack, and `r'
	 * reset to zero.  Thus initially these two variables are both zero, and
	 * `rack_no' is one less, such that these variables can be computed inside
	 * the loop to make the code simpler.
	 *
	 * The computation of `on_this_rack' is as follows;  Given that there
	 * are M missiles and R racks remaining;
	 *
	 *	on_this_rack = int(M / R);		(ie. round down to lowest int)
	 *
	 * Then;
	 *
	 *	(M - on_this_rack) / (R - 1) < (M / R).
	 *
	 * That is, the number of missiles fired on the next rack will be
	 * more precise, and trivially can be seen that when R is 1, will
	 * give an exact number of missiles to fire on the last rack.
	 *
	 * In the code `M' is (minis - i), and `R' is racks_left.
	 *
	 *					Comment Point 3
	 *					---------------
	 *
	 * In order that multiple missiles fired from one rack do not conincide,
	 * each missile has to be "spread" based on the number of missiles
	 * fired from this rack point.
	 *
	 * This is computed similar to the wide shot code;
	 *
	 *	angle = (N - 1 - 2 * i) / (N - 1)
	 *
	 * Where N is the number of shots/missiles to be fired, and i is a counter
	 * from 0 .. N-1.
	 *
	 *			i		0		1		2		3
	 * N
	 * 1				0
	 * 2				1		-1
	 * 3				1		0		-1
	 * 4				1		0.333	-0.333	-1
	 *
	 * In this code `N' is `on_this_rack'.
	 *
	 * Also the position of the missile rack from the center line of the
	 * ship (stored in `side') has a linear effect on the angle, such that
	 * a point farthest from the center line contributes the largest angle;
	 *
	 * angle += (side / SHIP_SZ)
	 *
	 * Since the eventual `angle' value used in the code should be a
	 * percentage of the unmodified launch angle, it should be ranged between
	 * -1.00 and +1.00, and thus the first angle is reduced by 33% and the
	 * second by 66%.
	 *
	 * Contact: harveyt@sco.com
	 */

	if (pl && type != OBJ_SHOT) {
		/*
		 * Initialise missile rack spread variables. (See Comment Point 1)
		 */
		on_this_rack = 0;
		racks_left = pl->ship->num_m_rack;
		rack_no = pl->missile_rack - 1;
		if (++pl->missile_rack >= pl->ship->num_m_rack)
			pl->missile_rack = 0;
	}

	for (r = 0, i = 0; i < minis; i++, r++) {
		Object* shot;

		if ((shot = Object_allocate(this)) == NULL) {
				break;
		}
		shot->life		= life / minis;
		shot->fuselife	= shot->life - fuse;
		shot->mass		= mass / minis;
		shot->count 	= 0;
		shot->info		= lock;
		shot->type		= type;
		shot->id		= (pl ? pl->id : NO_ID);
		shot->team		= team;
		shot->color 	= (pl ? pl->color : WHITE);

		if (BIT(shot->type, OBJ_TORPEDO | OBJ_HEAT_SHOT | OBJ_SMART_SHOT)) {
			MISSILE_PTR(shot)->turnspeed = turnspeed;
			MISSILE_PTR(shot)->max_speed = max_speed;
		}

		shotpos.x		= x;
		shotpos.y		= y;
		if (pl && type != OBJ_SHOT) {
			if (r == on_this_rack) {
				/*
				 * We've fired all the mini missiles for the current rack,
				 * we now move onto the next one. (See Comment Point 2)
				 */
				on_this_rack = (minis - i) / racks_left--;
				if (on_this_rack < 1) on_this_rack = 1;
				if (++rack_no >= pl->ship->num_m_rack)
					rack_no = 0;
				r = 0;
			}
			shotpos.x += pl->ship->m_rack[rack_no][pl->dir].x;
			shotpos.y += pl->ship->m_rack[rack_no][pl->dir].y;
			side = (int)(pl->ship->m_rack[rack_no][0].y);
		}
		shotpos.x = WRAP_XPIXEL(this, shotpos.x);
		shotpos.y = WRAP_YPIXEL(this, shotpos.y);
		if (shotpos.x < 0 || shotpos.x >= width
			|| shotpos.y < 0 || shotpos.y >= height) {
			continue;
		}
		Object_position_init_pixels(shot, shotpos.x, shotpos.y);

		if (type == OBJ_SHOT || !pl) {
			angle = 0.0;
		} else {
			/*
			 * Calculate the percentage unmodified launch angle for missiles.
			 * (See Comment Point 3).
			 */
			if (on_this_rack <= 1) {
				angle = 0.0;
			} else {
				angle = (DFLOAT)(on_this_rack - 1 - 2 * r);
				angle /= (3.0 * (DFLOAT)(on_this_rack - 1));
			}
			angle += (DFLOAT)(2 * side) / (DFLOAT)(3 * SHIP_SZ);
		}

		/*
		 * Torpedoes spread like mines, except the launch direction
		 * is preset over the range +/- MINI_TORPEDO_SPREAD_ANGLE.
		 * (This is not modified by the spread, the initial velocity is)
		 *
		 * Other missiles are just launched in a different direction
		 * which varies over the range +/- MINI_MISSILE_SPREAD_ANGLE,
		 * which the spread modifier varies.
		 */
		switch (type) {
		case OBJ_TORPEDO:
			angle *= (MINI_TORPEDO_SPREAD_ANGLE / 360.0) * RES;
			ldir = MOD2(dir + (int)angle, RES);
			mv.x = MINI_TORPEDO_SPREAD_SPEED * tcos(ldir) / spread;
			mv.y = MINI_TORPEDO_SPREAD_SPEED * tsin(ldir) / spread;
			/*
			 * This causes the added initial velocity to reduce to
			 * zero over the MINI_TORPEDO_SPREAD_TIME.
			 * FIX: torpedoes should have the same speed
			 *		regardless of minification.
			 */
			TORP_PTR(shot)->spread_left = MINI_TORPEDO_SPREAD_TIME;
			shot->acc.x = -mv.x / MINI_TORPEDO_SPREAD_TIME;
			shot->acc.y = -mv.y / MINI_TORPEDO_SPREAD_TIME;
			ldir = dir;
			break;

		default:
			angle *= (MINI_MISSILE_SPREAD_ANGLE / 360.0) * RES / spread;
			ldir = MOD2(dir + (int)angle, RES);
			mv.x = mv.y = shot->acc.x = shot->acc.y = 0;
			break;
		}

		shot->vel.x 	= mv.x + (pl ? pl->vel.x : 0.0) + tcos(ldir) * speed;
		shot->vel.y 	= mv.y + (pl ? pl->vel.y : 0.0) + tsin(ldir) * speed;
		shot->status	= status;
		shot->missile_dir		= ldir;
		shot->mods		= mods;
		shot->pl_range	= pl_range;
		shot->pl_radius = pl_radius;
		CellAddObject(shot);
		mini_objs[fired] = shot;
		fired++;
	}

	/*
	 * Recoil must be done instantaneously otherwise ship moves back after
	 * firing each mini missile.
	 */
	if (pl) {
		for (i = 0; i < fired; i++)
			Recoil((Object *)pl, mini_objs[i]);
	}
}


void Player::FireNormalShots()
{
	int 				i, shot_angle;

	if (frame_loops < shot_time + world->options.fireRepeatRate->GetInt()) {
		return;
	}
	shot_time = frame_loops;

	shot_angle = MODS_SPREAD_MAX - mods.spread;

	FireMainShot(OBJ_SHOT, dir);
	for (i = 0; i < item[ITEM_WIDEANGLE]; i++) {
		if (ship->num_l_gun > 0) {
			FireLeftShot(OBJ_SHOT, MOD2(dir + (1 + i) * shot_angle,
						   RES), i % ship->num_l_gun);
		}
		else {
			FireMainShot(OBJ_SHOT, MOD2(dir + (1 + i) * shot_angle,
						   RES));
		}
		if (ship->num_r_gun > 0) {
			FireRightShot(OBJ_SHOT, MOD2(dir - (1 + i) * shot_angle,
							RES), i % ship->num_r_gun);
		}
		else {
			FireMainShot(OBJ_SHOT, MOD2(dir - (1 + i) * shot_angle,
						   RES));
		}
	}
	for (i = 0; i < item[ITEM_REARSHOT]; i++) {
		if ((item[ITEM_REARSHOT] - 1 - 2 * i) < 0) {
			if (ship->num_l_rgun > 0) {
				FireLeftRShot(OBJ_SHOT, MOD2(dir + RES/2
					+ ((item[ITEM_REARSHOT] - 1 - 2 * i) * shot_angle) / 2,
						RES), (i - (item[ITEM_REARSHOT] + 1) / 2) % ship->num_l_rgun);
			}
			else {
				FireShot(OBJ_SHOT, MOD2(dir + RES/2
					+ ((item[ITEM_REARSHOT] - 1 - 2 * i) * shot_angle) / 2,
						RES));
			}
		}
		if ((item[ITEM_REARSHOT] - 1 - 2 * i) > 0) {
			if (ship->num_r_rgun > 0) {
				FireRightRShot(OBJ_SHOT, MOD2(dir + RES/2
					+ ((item[ITEM_REARSHOT] - 1 - 2 * i) * shot_angle) / 2,
						RES), (item[ITEM_REARSHOT] / 2 - i - 1) % ship->num_r_rgun);
			}
			else {
				FireShot(OBJ_SHOT, MOD2(dir + RES/2
					+ ((item[ITEM_REARSHOT] - 1 - 2 * i) * shot_angle) / 2,
						RES));
			}
		}
		if ((item[ITEM_REARSHOT] - 1 - 2 * i) == 0)
			 FireShot(OBJ_SHOT, MOD2(dir + RES/2
				+ ((item[ITEM_REARSHOT] - 1 - 2 * i) * shot_angle) / 2,
						RES));
	}
}


/* Removes shot from array */
void World::DeleteShot(int ind)
{
	Object* 			shot = objs[ind];		/* Used when swapping places */
	BallObject* ball;
	Player* 			pl;
	int 				addMine = 0;
	int 				addHeat = 0;
	int 				addBall = 0;
	modifiers			mods;
	long				status;
	int 				i;
	int 				intensity;
	int 				type, color;
	DFLOAT				modv, speed_modv, life_modv, num_modv;
	DFLOAT				mass;

	switch (shot->type) {

	case OBJ_SPARK:
	case OBJ_DEBRIS:
	case OBJ_WRECKAGE:
		break;

	case OBJ_ASTEROID:
		Break_asteroid(this, (WireObject*)shot);
		break;

	case OBJ_BALL:
		ball = BALL_PTR(shot);
		if (ball->id != NO_ID)
			players[getInd[ball->id]]->DetachBall(ind);
		else {
			/*
			 * Maybe some player is still busy trying to connect to this ball.
			 */
			for (i = 0; i < numPlayers; i++) {
				if (players[i]->ball == ball) {
					players[i]->ball = NULL;
				}
			}
		}
		if (ball->owner == NO_ID) {
			/*
			 * If the ball has never been owned, the only way it could
			 * have been destroyed is by being knocked out of the goal.
			 * Therefore we force the ball to be recreated.
			 */
			treasures[ball->treasure].have = false;
			SET_BIT(ball->status, RECREATE);
		}
		if (BIT(ball->status, RECREATE)) {
			addBall = 1;
			if (BIT(ball->status, NOEXPLOSION))
				break;
			SoundPlaySensors(this, ball->pos.x, ball->pos.y, EXPLODE_BALL_SOUND);
			MakeDebris(
				/* pos.x, pos.y   */ ball->prevpos.x, ball->prevpos.y,
				/* vel.x, vel.y   */ ball->vel.x, ball->vel.y,
				/* owner id 	  */ ball->id,
				/* owner team	  */ ball->team,
				/* kind 		  */ OBJ_DEBRIS,
				/* mass 		  */ DEBRIS_MASS,
				/* status		  */ GRAVITY,
				/* color		  */ RED,
				/* radius		  */ 8,
				/* min,max debris */ 10, 20,
				/* min,max dir	  */ 0, RES-1,
				/* min,max speed  */ 10, 50,
				/* min,max life   */ 10, 2*(GetFPS()+15)
				);

		}
		break;
		/* Shots related to a player. */

	case OBJ_MINE:
	case OBJ_HEAT_SHOT:
	case OBJ_TORPEDO:
	case OBJ_SMART_SHOT:
	case OBJ_CANNON_SHOT:
		if (shot->mass == 0) {
			break;
		}

		status = GRAVITY;
		if (shot->type == OBJ_MINE) {
			status |= COLLISIONSHOVE;
		}
		if (BIT(shot->status, FROMCANNON)) {
			status |= FROMCANNON;
		}

		if (BIT(shot->mods.nuclear, NUCLEAR)) {
			SoundPlayAll(this, NUKE_EXPLOSION_SOUND);
		}
		else if (BIT(shot->type, OBJ_MINE)) {
			SoundPlaySensors(this, shot->pos.x, shot->pos.y, MINE_EXPLOSION_SOUND);
		} else {
			SoundPlaySensors(this, shot->pos.x, shot->pos.y, OBJECT_EXPLOSION_SOUND);
		}

		if (BIT(shot->mods.warhead, CLUSTER)) {
			type = OBJ_SHOT;
			if (shot->id != NO_ID) {
				Player *pl = players[getInd[shot->id]];
				color = pl->color;
			}
			else {
				color = WHITE;
			}
				mass = options.shotMass->GetDouble();
			mass *= 3;
			modv = 1 << shot->mods.velocity;
			num_modv = 4;
			if (BIT(shot->mods.nuclear, NUCLEAR)) {
				modv *= 4.0f;
				num_modv = 1;
			}
			life_modv = modv * 0.20f;
			speed_modv = 1.0f / modv;
			intensity = (int)CLUSTER_MASS_SHOTS(this, shot->mass);
		} else {
			type = OBJ_DEBRIS;
			color = RED;
			mass = DEBRIS_MASS;
			modv = 1;
			num_modv = 1;
			life_modv = modv;
			speed_modv = modv;
			if (shot->type == OBJ_MINE) {
				intensity = 512;
			} else {
				intensity = 32;
			}
			/*
			 * Writing it like this:
			 *	 num_modv /= (shot->mods.mini + 1);
			 * triggers a bug in HP C A.09.19.
			 */
			num_modv = num_modv / ((DFLOAT)(unsigned)shot->mods.mini + 1.0f);
		}

		if (BIT(shot->mods.nuclear, NUCLEAR)) {
			DFLOAT nuke_factor;
			if (shot->type == OBJ_MINE) {
				nuke_factor = NUKE_MINE_EXPL_MULT * shot->mass / MINE_MASS;
			} else {
				nuke_factor = NUKE_SMART_EXPL_MULT * shot->mass / MISSILE_MASS;
			}
			nuke_factor = (nuke_factor * (shot->mods.mini + 1)) / SHOT_MULT(shot);
			intensity = (int)(intensity * nuke_factor);
		}

		if (BIT(shot->mods.warhead, IMPLOSION)) {
			/*intensity >>= 1;*/
			mass = -mass;
		}

		if (BIT(shot->type, OBJ_TORPEDO|OBJ_HEAT_SHOT|OBJ_SMART_SHOT)) {
			intensity /= (1 + shot->mods.power);
		}

		MakeDebris(
			/* pos.x, pos.y   */ shot->prevpos.x, shot->prevpos.y,
			/* vel.x, vel.y   */ shot->vel.x, shot->vel.y,
			/* owner id 	  */ shot->id,
			/* owner team	  */ shot->team,
			/* kind 		  */ type,
			/* mass 		  */ mass,
			/* status		  */ status,
			/* color		  */ color,
			/* radius		  */ 6,
			/* min,max debris */ (int)(0.20f * intensity * num_modv),
								 (int)(0.30f * intensity * num_modv),
			/* min,max dir	  */ 0, RES-1,
			/* min,max speed  */ 20 * speed_modv,
								 (intensity >> 2) * speed_modv,
			/* min,max life   */ (int)(8 * life_modv),
								 (int)((intensity >> 1) * life_modv)
			);
		break;

	case OBJ_SHOT:
		if (shot->id == NO_ID
			|| BIT(shot->status, FROMCANNON)
			|| BIT(shot->mods.warhead, CLUSTER)) {
			break;
		}
		pl = players[getInd[shot->id]];
		if (--pl->shots <= 0) {
			pl->shots = 0;
		}
		break;

		/* Special items. */
	case OBJ_ITEM:

		switch (shot->info) {

		case ITEM_MISSILE:
			if (shot->life == 0 && shot->color != WHITE) {
				shot->color = WHITE;
				shot->life	= GetFPS() * WARN_TIME;
				return;
			}
			if (shot->life == 0 && rfrac() < options.rogueHeatProb->GetDouble()) {
				addHeat = 1;
			}
			break;

		case ITEM_MINE:
			if (!shot->life && shot->color != WHITE) {
				shot->color = WHITE;
				shot->life	= GetFPS() * WARN_TIME;
				return;
			}
			if (shot->life == 0 && rfrac() < options.rogueMineProb->GetDouble()) {
				addMine = 1;
			}
			break;
		}

		items[shot->info].num--;

		break;

	default:
		xpprintf("%sDelete_shot(): Unkown shot type %d.\n",
				 showtime(), shot->type);
		break;
	}

	CellRemoveObject(shot);
	shot->life = 0;
	shot->type = 0;
	shot->mass = 0;

	/* This should be the only place in the code which decrements NumObjs. */
		Object_free_ind(this, ind);

	if (addMine | addHeat) {
		CLEAR_MODS(mods);
		if (BIT(rules->mode, ALLOW_CLUSTERS) && (rfrac() <= 0.333f)) {
			SET_BIT(mods.warhead, CLUSTER);
		}
		if (BIT(rules->mode, ALLOW_MODIFIERS) && (rfrac() <= 0.333f)) {
			SET_BIT(mods.warhead, IMPLOSION);
		}
		if (BIT(rules->mode, ALLOW_MODIFIERS)) {
			mods.velocity = (int)(rfrac() * (MODS_VELOCITY_MAX + 1));
		}
		if (BIT(rules->mode, ALLOW_MODIFIERS)) {
			mods.power = (int)(rfrac() * (MODS_POWER_MAX + 1));
		}
		if (addMine) {
			long gravity_status = ((rfrac() < 0.5f) ? GRAVITY : 0);
			PlaceGeneralMine(-1, TEAM_NOT_SET, gravity_status,
							   shot->pos.x, shot->pos.y,
							   0.0, 0.0, mods);
		}
		else if (addHeat) {
			FireGeneralShot(-1, TEAM_NOT_SET, 0,
							  shot->pos.x, shot->pos.y,
							  OBJ_HEAT_SHOT, (int)(rfrac() * RES),
							  mods, -1);
		}
	}
	else if (addBall) {
		ball = BALL_PTR(shot);
		MakeTreasureBall(ball->treasure);
	}
}

void Player::FireLaser()
{
	DFLOAT		x, y;
	
	if (item[ITEM_LASER] > num_pulses
		&& velocity < PULSE_SPEED - PULSE_SAMPLE_DISTANCE) {
		if (fuel.sum <= -ED_LASER) {
			CLR_BIT(used, HAS_LASER);
		} else {
			x = pos.x + ship->m_gun[dir].x + vel.x;
			y = pos.y + ship->m_gun[dir].y + vel.y;
			x = WRAP_XPIXEL(world, x);
			y = WRAP_YPIXEL(world, y);
			if (x >= 0 && x < world->width && y >= 0 && y < world->height) {
				world->FireGeneralLaser(Ind(), team, x, y, dir, mods);
			}
		}
	}
}

void Player::FireEcm()
{
	if (item[ITEM_ECM] == 0
		|| fuel.sum <= -ED_ECM
		|| ecmcount >= MAX_PLAYER_ECMS
		|| BIT(used, HAS_PHASING_DEVICE))
		return;

	world->FireGeneralEcm(this, team, pos.x, pos.y);
}


void World::FireGeneralLaser(int ind, unsigned short team, DFLOAT x, DFLOAT y,
						int dir, modifiers mods)
{
	Player				*pl = ((ind == -1) ? NULL : players[ind]);
	Pulse				*pulse;
	int 				life;

	if (pl) {
		Add_fuel(&(pl->fuel), (long)ED_LASER);
		SoundPlaySensors(this, x, y, FIRE_LASER_SOUND);
		life = (int)PULSE_LIFE(pl->item[ITEM_LASER]);
	} else {
		life = (int)PULSE_LIFE(CANNON_PULSES);
	}

	if (numPulses >= MAX_TOTAL_PULSES) {
		return;
	}
	pulses[numPulses] = (Pulse *)malloc(sizeof(Pulse));
	if (pulses[numPulses] == NULL) {
		return;
	}

	pulse = pulses[numPulses];
	pulse->id = (pl ? pl->id : NO_ID);
	pulse->team = team;
	pulse->dir = dir;
	pulse->len = PULSE_LENGTH;
	pulse->life = life;
	pulse->mods = mods;
	pulse->refl = false;
	pulse->pos.x = x - PULSE_SPEED * tcos(dir);
	pulse->pos.y = y - PULSE_SPEED * tsin(dir);
	numPulses++;
	if (pl)
		pl->num_pulses++;
}


void World::MoveBall(int ind)
{
#ifdef ORIGINAL_BALL

	/*
	 * This is the original ball code from XPilot versions 2.0 till 3.3.1.
	 * The `feature' which some people got dissatisfied with
	 * is that trying to connect to a fast moving ball may result
	 * in being launched with high speed into a wall.
	 * Some like that feature reasoning that making everything
	 * easy is boring.	Hence keeping the old code around.
	 * It can be enabled by adding -DORIGINAL_BALL to the compilation flags.
	 */

	ballobject			*ball = BALL_IND(this, ind);
	player				*pl = players[ GetInd[ball->id] ];
	vector				F;
	const DFLOAT				k = 10.0,
						a = 0.01,
						l = Wrap_length(pl->pos.x - ball->pos.x,
										pl->pos.y - ball->pos.y),
						c = k * (1.0 - ballConnectorLength / l)
							- a * ABS(ball->length - l) * (ball->length - l);

	if (l > ballConnectorLength * (1.00 + maxBallConnectorRatio)
		|| l < ballConnectorLength * (1.00 - maxBallConnectorRatio)) {
		Detach_ball(GetInd[ball->id], ind);
		return;
	}

	F.x = WRAP_DX(pl->pos.x - ball->pos.x) * c;
	F.y = WRAP_DY(pl->pos.y - ball->pos.y) * c;

	pl->vel.x -= F.x/pl->mass;
	pl->vel.y -= F.y/pl->mass;

	ball->vel.x += F.x/ball->mass;
	ball->vel.y += F.y/ball->mass;

	ball->length = l;

#else	/* ORIGINAL_BALL */

	/*
	 * The new ball movement code since XPilot version 3.4.0 as made
	 * by Bretton Wade.  The code was submitted in context diff format
	 * by Mark Boyns.  Here is a an excerpt from a post in
	 * rec.games.computer.xpilot by Bretton Wade dated 27 Jun 1995:
	 *
	 *	   If I'm not mistaken (not having looked very closely at the code
	 *	   because I wasn't sure what it was trying to do), the original move_ball
	 *	   routine was trying to model a Hook's law spring, but squared the
	 *	   deformation term, which would lead to exagerated behavior as the spring
	 *	   stretched too far. Not really a divide by zero, but effectively producing
	 *	   large numbers.
	 *
	 *	   When I coded up the spring myself, I found that I could recreate the
	 *	   effect by using a VERY strong spring. This can be defeated, however, by
	 *	   damping. Specifically, If you compute the critical damping factor, then
	 *	   you could have the cable always be the correct length. This makes me
	 *	   wonder how to decide when the cable snaps.
	 *
	 *	   I chose a relatively strong spring, and a small damping factor, to make
	 *	   for a nice realistic bounce when you grab at the treasure. It also gives a
	 *	   fairley close approximation to the "normal" feel of the treasure.
	 *
	 *	   I modeled the cable as having zero mass, or at least insignificant mass as
	 *	   compared to the ship and ball. This greatly simplifies the math, and leads
	 *	   to the conclusion that there will be no change in velocity when the cable
	 *	   breaks. You can check this by integrating the momentum along the cable,
	 *	   and the ship or ball.
	 *
	 *	   If you assume that the cable snaps in the middle, then half of the
	 *	   potential energy goes to each object attached. However, as you said, the
	 *	   total momentum of the system cannot change. Because the weight of the
	 *	   cable is small, the vast majority of the potential energy will become
	 *	   heat. I've had two physicists verify this for me, and they both worked
	 *	   really hard on the problem because they found it interesting.
	 *
	 * End of post.
	 *
	 * Changes since then:
	 *
	 * Comment from people was that the string snaps too soon.
	 * Changed the value (max_spring_ratio) at which the string snaps
	 * from 0.25 to 0.30.  Not sure if that helps enough, or too much.
	 */

	BallObject			*ball = BALL_IND(this, ind);
	Player				*pl = players[getInd[ball->id]];
	vector				D;
	DFLOAT				length, force, ratio, accell, cosine;
	DFLOAT				pl_damping, ball_damping;
	/* const DFLOAT 			k = 1500.0, b = 2.0; */
	/* const DFLOAT 			max_spring_ratio = 0.30; */

	/* compute the normalized vector between the ball and the player */
	D.x = WRAP_DX(this, pl->pos.x - ball->pos.x);
	D.y = WRAP_DY(this, pl->pos.y - ball->pos.y);
	length = VECTOR_LENGTH(D);
	if (length > 0.0) {
		D.x /= length;
		D.y /= length;
	}
	else
		D.x = D.y = 0.0;

	/* compute the ratio for the spring action */
	ratio = (options.ballConnectorLength->GetDouble() - length) 
						/ options.ballConnectorLength->GetDouble();

	/* compute force by spring for this length */
	force = options.ballConnectorSpringConstant->GetDouble() * ratio;

		/* If we have string-style connectors, then it is allowed to be
		 * shorter than its natural length. */
		if (options.connectorIsString->GetBool()
				&& ratio > 0.0) {
				return;
		}

	/* if the tether is too long or too short, release it */
	if (ABS(ratio) > options.maxBallConnectorRatio->GetDouble()) {
				players[getInd[ball->id]]->DetachBall(ind);
		return;
	}
	ball->length = length;

	/* compute damping for player */
	cosine = (pl->vel.x * D.x) + (pl->vel.y * D.y);
	pl_damping = -options.ballConnectorDamping->GetDouble() * cosine;

	/* compute damping for ball */
	cosine = (ball->vel.x * -D.x) + (ball->vel.y * -D.y);
	ball_damping = -options.ballConnectorDamping->GetDouble() * cosine;

	/* compute accelleration for player, assume t = 1 */
	accell = (force + pl_damping + ball_damping) / pl->mass;
	pl->vel.x += D.x * accell;
	pl->vel.y += D.y * accell;

	/* compute accelleration for ball, assume t = 1 */
	accell = (force + ball_damping + pl_damping) / ball->mass;
	ball->vel.x += -D.x * accell;
	ball->vel.y += -D.y * accell;

#endif	/* ORIGINAL_BALL */
}


void World::MoveMissile(int ind)
{
	MissileObject		*shot = MISSILE_IND(this, ind);
	Player* 			pl;
	int 				angle, theta;
	DFLOAT				range = 0.0;
	DFLOAT				acc;
	DFLOAT				x_dif = 0.0;
	DFLOAT				y_dif = 0.0;
	DFLOAT				shot_speed;

	if (shot->type == OBJ_TORPEDO) {
		TorpObject *torp = TORP_PTR(shot);
		if (BIT(torp->mods.nuclear, NUCLEAR)) {
			acc = (torp->info++ < NUKE_SPEED_TIME*GetFPS()) ? NUKE_ACC*GetFPS() : 0.0;
		} else {
			acc = (torp->info++ < TORPEDO_SPEED_TIME*GetFPS())
				? TORPEDO_ACC/(TORPEDO_SPEED_TIME*GetFPS())
				: 0.0;
		}
		acc *= (1 + (torp->mods.power * MISSILE_POWER_SPEED_FACT));
		if (torp->spread_left-- <= 0) {
			torp->acc.x = 0;
			torp->acc.y = 0;
		}
		torp->vel.x += acc * tcos(torp->missile_dir);
		torp->vel.y += acc * tsin(torp->missile_dir);
		return;
	}

	acc = SMART_SHOT_ACC;

	if (shot->type == OBJ_HEAT_SHOT) {
		acc = SMART_SHOT_ACC * HEAT_SPEED_FACT;
		if (shot->info >= 0) {
			/* Get player and set min to distance */
			pl = players[getInd[shot->info]];
			range = WrapLength(pl->pos.x + pl->ship->engine[pl->dir].x
								- shot->pos.x,
								pl->pos.y + pl->ship->engine[pl->dir].y
								- shot->pos.y);
		} else {
			/* No player. Number of moves so that new target is searched */
			pl = 0;
			shot->count = (int)(HEAT_WIDE_TIMEOUT * GetFPS() + HEAT_WIDE_ERROR);
		}
		if (pl && BIT(pl->status, THRUSTING)) {
			/*
			 * Target is thrusting,
			 * set number to moves to correct error value
			 */
			if (range < HEAT_CLOSE_RANGE) {
				shot->count = HEAT_CLOSE_ERROR;
			} else if (range < HEAT_MID_RANGE) {
				shot->count = HEAT_MID_ERROR;
			} else {
				shot->count = HEAT_WIDE_ERROR;
			}
		} else {
			shot->count++;
			/* Look for new target */
			if ((range < HEAT_CLOSE_RANGE
				 && shot->count > HEAT_CLOSE_TIMEOUT*GetFPS() + HEAT_CLOSE_ERROR)
				|| (range < HEAT_MID_RANGE
					&& shot->count > HEAT_MID_TIMEOUT*GetFPS() + HEAT_MID_ERROR)
				|| shot->count > HEAT_WIDE_TIMEOUT*GetFPS() + HEAT_WIDE_ERROR) {
				DFLOAT l;
				int i;

				range = HEAT_RANGE * (shot->count/(HEAT_CLOSE_TIMEOUT*GetFPS()));
				for (i=0; i<numPlayers; i++) {
					Player *p = players[i];

					if (!BIT(p->status, THRUSTING))
						continue;

					l = WrapLength(p->pos.x + p->ship->engine[p->dir].x
									- shot->pos.x,
									p->pos.y + p->ship->engine[p->dir].y
									- shot->pos.y);
					/*
					 * After burners can be detected easier;
					 * so scale the length:
					 */
					l *= MAX_AFTERBURNER + 1 - p->item[ITEM_AFTERBURNER];
					l /= MAX_AFTERBURNER + 1;
					if (BIT(p->have, HAS_AFTERBURNER))
						l *= 16 - p->item[ITEM_AFTERBURNER];
					if (l < range) {
						shot->info = players[i]->id;
						range = l;
						shot->count =
							l < HEAT_CLOSE_RANGE ?
								HEAT_CLOSE_ERROR : l < HEAT_MID_RANGE ?
									HEAT_MID_ERROR : HEAT_WIDE_ERROR;
						pl = p;
					}
				}
			}
		}
		if (shot->info < 0)
			return;
		/*
		 * Heat seekers cannot fly exactly, if target is far away or thrust
		 * isn't active.  So simulate the error:
		 */
		x_dif = (int)(rfrac() * 4 * shot->count);
		y_dif = (int)(rfrac() * 4 * shot->count);

	}
	else if (shot->type == OBJ_SMART_SHOT) {
		SmartObject *smart = SMART_PTR(shot);

		if (BIT(smart->status, CONFUSED)
			&& (!(frame_loops % CONFUSED_UPDATE_GRANULARITY)
				|| smart->count == CONFUSED_TIME)) {

			if (smart->count) {
				smart->info = players[(int)(rfrac() * numPlayers)]->id;
				smart->count--;
			} else {
				CLR_BIT(smart->status, CONFUSED);

				/* range is percentage from center to periphery of ecm burst */
				range = (ECM_DISTANCE - smart->ecm_range) / ECM_DISTANCE;
				range *= 100.0;

				/*
				 * range%		lock%
				 * 100			100
				 *	50			75
				 *	 0			50
				 */
				if ((int)(rfrac() * 100) <= ((int)(range/2)+50)) {
					smart->info = smart->new_info;
				}
			}
		}
		pl = players[getInd[shot->info]];
	}
	else {
		/*NOTREACHED*/
		return;
	}

	/*
	 * Use a little look ahead to fly more exact
	 */
	acc *= (1 + (shot->mods.power * MISSILE_POWER_SPEED_FACT));
	if ((shot_speed = VECTOR_LENGTH(shot->vel)) < 1) shot_speed = 1;
	range = WrapLength(pl->pos.x - shot->pos.x, pl->pos.y - shot->pos.y);
	x_dif += pl->vel.x * (range / shot_speed);
	y_dif += pl->vel.y * (range / shot_speed);
	theta = (int)WrapFindDir(pl->pos.x + x_dif - shot->pos.x,
												 pl->pos.y + y_dif - shot->pos.y);

	{
		DFLOAT x, y, vx, vy;
		int i, xi, yi, j, freemax, k, foundw;
		static struct {
			int dx, dy;
		} sur[8] = {
			{1,0}, {1,1}, {0,1}, {-1,1}, {-1,0}, {-1,-1}, {0,-1}, {1,-1}
		};

#define BLOCK_PARTS 2
		vx = shot->vel.x;
		vy = shot->vel.y;
		x = shot_speed / (BLOCK_SZ*BLOCK_PARTS);
		vx /= x; vy /= x;
		x = shot->pos.x; y = shot->pos.y;
		foundw = 0;

		for (i = SMART_SHOT_LOOK_AH; i > 0 && foundw == 0; i--) {
			xi = (int)((x += vx) / BLOCK_SZ);
			yi = (int)((y += vy) / BLOCK_SZ);
			if (BIT(rules->mode, WRAP_PLAY)) {
				if (xi < 0) xi += blockWidth;
				else if (xi >= blockWidth) xi -= blockWidth;
				if (yi < 0) yi += blockHeight;
				else if (yi >= blockHeight) yi -= blockHeight;
			}
			if (xi < 0 || xi >= blockWidth || yi < 0 || yi >= blockHeight)
				break;

			switch(block[xi][yi]) {
			case TARGET:
			case TREASURE:
			case FUEL:
			case FILLED:
			case REC_LU:
			case REC_RU:
			case REC_LD:
			case REC_RD:
			case CANNON:
				if (range > (SMART_SHOT_LOOK_AH-i)*(BLOCK_SZ/BLOCK_PARTS)) {
					if (shot_speed > SMART_SHOT_MIN_SPEED)
						shot_speed -= acc * (SMART_SHOT_DECFACT+1);
				}
				foundw = 1;
			}
		}

		i = ((int)(shot->missile_dir * 8 / RES)&7) + 8;
		xi = OBJ_X_IN_BLOCKS(shot);
		yi = OBJ_Y_IN_BLOCKS(shot);

		for (j=2, angle=-1, freemax=0; j>=-2; --j) {
			int si, xt, yt;

			for (si=1, k=0; si >= -1; --si) {
				xt = xi + sur[(i+j+si)&7].dx;
				yt = yi + sur[(i+j+si)&7].dy;

				if (xt >= 0 && xt < blockWidth && yt >= 0 && yt < blockHeight)
					switch (block[xt][yt]) {
					case TARGET:
					case TREASURE:
					case FUEL:
					case FILLED:
					case REC_LU:
					case REC_RU:
					case REC_LD:
					case REC_RD:
					case CANNON:
						if (!si)
							k = -32;
						break;
					default:
						++k;
						break;
					}
			}
			if (k > freemax
				|| (k == freemax
					&& ((j == -1 && (rfrac() < 0.5f)) || j == 0 || j == 1))) {
				freemax = k > 2 ? 2 : k;
				angle = i + j;
			}

			if (k == 3 && !j) {
				angle = -1;
				break;
			}
		}

		if (angle >= 0) {
			i = angle&7;
			theta = (int)WrapFindDir((yi + sur[i].dy) * BLOCK_SZ
							- (shot->pos.y + 2 * shot->vel.y),
							(xi + sur[i].dx) * BLOCK_SZ
							- (shot->pos.x - 2 * shot->vel.x));
#ifdef SHOT_EXTRA_SLOWDOWN
			if (!foundw && range > (SHOT_LOOK_AH-i) * BLOCK_SZ) {
				if (shot_speed
					> (SMART_SHOT_MIN_SPEED + SMART_SHOT_MAX_SPEED)/2)
					shot_speed -= SMART_SHOT_DECC+SMART_SHOT_ACC;
			}
#endif
		}
	}
	angle = theta;

	if (angle < 0)
		angle += RES;
	angle %= RES;

	if (angle < shot->missile_dir)
		angle += RES;
	angle = angle - shot->missile_dir - RES/2;

	if (angle < 0) {
		shot->missile_dir += (u_byte)(((-angle < shot->turnspeed)
										? -angle
										: shot->turnspeed));
	}
	else {
		shot->missile_dir -= (u_byte)(((angle < shot->turnspeed)
										? angle
										: shot->turnspeed));
	}

	shot->missile_dir = MOD2(shot->missile_dir, RES); /* NOTE!!!! */

	if (shot_speed < shot->max_speed)
		shot_speed += acc;

	/*	shot->velocity = MIN(shot->velocity, shot->max_speed);	*/

	shot->vel.x = tcos(shot->missile_dir) * shot_speed;
	shot->vel.y = tsin(shot->missile_dir) * shot_speed;
}

void World::MoveMine(int ind)
{
	MineObject	*mine = MINE_IND(this, ind);

	if (BIT(mine->status, CONFUSED) && --mine->count <= 0)
		CLR_BIT(mine->status, CONFUSED);

	if (BIT(mine->status, OWNERIMMUNE) && mine->info) {
		if (--mine->info <= 0)
			CLR_BIT(mine->status, OWNERIMMUNE);
	}

	if (mine->mods.mini && mine->spread_left-- <= 0) {
		mine->acc.x = 0;
		mine->acc.y = 0;
	}
}
