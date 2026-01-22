/* $Id: ship.cpp,v 1.26 2007/01/14 22:10:15 dick Exp $
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
 *  $Log: ship.cpp,v $
 *  Revision 1.26  2007/01/14 22:10:15  dick
 *  Robot4 is the original xpilot 4 series robot.
 *  Robot4a is dick's playing around with making them better.
 *
 *  Revision 1.25  2004/05/07 04:27:41  dick
 *  _visibility becomes Visibility.  global updateScores becomes a member of World.
 *  Handle rank/rate data from the scoreserver and send it to the client.
 *
 *  Revision 1.24  2004/05/04 21:07:17  dick
 *  Need randommnt.h
 *
 *  Revision 1.23  2004/04/29 16:08:27  dick
 *  compile with SOUND enabled.
 *
 *  Revision 1.22  2004/04/26 07:07:14  dick
 *  Define a list of pages that can be displayed on the ScoreTable in the client.
 *  Each page can be controlled by the server (and the server admin tool).
 *
 *  Revision 1.21  2004/02/18 01:22:14  dick
 *  Remove extraneous and obsolete #include "netserver.h"
 *
 *  Revision 1.20  2004/02/03 03:39:54  dick
 *  Add Windows precompiled header support.
 *
 *  Revision 1.19  2004/01/26 19:11:28  dick
 *  Fix Bug#37 cloakedExhaust is backward.  Remove extra cloaked check in TurnThrust()
 *  because it was already filtered.
 *
 *  Revision 1.18  2002/09/11 16:42:04  dick
 *  Here endeth the server encapsulation task.
 *  theWorld is now an array of World(s) and get loaded dynamically.
 *
 *  Revision 1.17  2002/09/01 00:13:46  dick
 *  Remove obsolete windows #include "winServer.h"
 *
 *  Revision 1.16  2002/07/14 19:05:24  dick
 *  When talking to the ScoreServer about PCSTR nick, always mention the PlayerType too.
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
 *  Revision 1.9  2001/07/30 01:19:55  dick
 *  Encapsulate the last few options (Strings)
 *
 *  Revision 1.8  2001/07/24 09:04:00  dick
 *  Remove all global options.  Use World.options instead.
 *
 *  Revision 1.7  2001/07/23 21:08:37  dick
 *  Encapsulate ShipMass, ShotsMax, ShotsLife, ShotsMass, FPS.
 *
 *  Revision 1.6  2001/07/21 07:21:41  dick
 *  Slay some float to int warnings
 *
 *  Revision 1.5  2001/07/21 06:45:30  dick
 *  Hide the varInt varString, etc. and force access through the accessor funcs
 *
 *  Revision 1.4  2001/07/19 07:01:48  dick
 *  Integrate OptOrigin into our ServerOption.
 *  stringvar becomes varString, intvar becomes varInt, etc.
 *
 *  Revision 1.3  2001/07/19 05:00:02  dick
 *  There is no real reason to have optionsStruct, all of the ServerOption(s)
 *  are attributes of ServerOptions.  And rename Options.* to ServerOptions.*
 *  (They could be just Options, but when the client comes along with his
 *  different format options...)
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
#include "randommt.h"
#include "global.h"
#include "proto.h"
#include "saudio.h"
#include "error.h"
#include "objpos.h"
#include "commonproto.h"
#include "proto.h"
#include "ConnectionPlayer.h"

char ship_version[] = VERSION;



/******************************
 * Functions for ship movement.
 */

void Player::Thrust()
{
	const int			min_dir = (int)(dir + RES/2 - RES*0.2 - 1);
	const int			max_dir = (int)(dir + RES/2 + RES*0.2 + 1);
	const DFLOAT		max_speed = 1 + (power * 0.14);
	const int			max_life = 3 + (int)(power * 0.35);
	static int			keep_rand;
	int 				this_rand = (((keep_rand >>= 2)
										? (keep_rand)
										: (keep_rand = randomMT())) & 0x03);
	int 				tot_sparks = (int)((power * 0.15) + this_rand + 1);
	DFLOAT				x = pos.x + ship->engine[dir].x;
	DFLOAT				y = pos.y + ship->engine[dir].y;
	int 				afterburners, alt_sparks;

	SoundPlaySensors(world, pos.x, pos.y, THRUST_SOUND);

	afterburners = (BIT(used, HAS_EMERGENCY_THRUST)
					? MAX_AFTERBURNER
					: item[ITEM_AFTERBURNER]);
	alt_sparks = afterburners
					? AFTER_BURN_SPARKS(tot_sparks-1, afterburners) + 1
					: 0;

	world->MakeDebris(
		/* pos.x, pos.y   */ x, y,
		/* vel.x, vel.y   */ vel.x, vel.y,
		/* owner id 	  */ id,
		/* owner team	  */ team,
		/* kind 		  */ OBJ_SPARK,
		/* mass 		  */ THRUST_MASS,
		/* status		  */ GRAVITY | OWNERIMMUNE,
		/* color		  */ RED,
		/* radius		  */ 8,
		/* min,max debris */ tot_sparks-alt_sparks, tot_sparks-alt_sparks,
		/* min,max dir	  */ min_dir, max_dir,
		/* min,max speed  */ 1.0, max_speed,
		/* min,max life   */ 3, max_life
		);

	world->MakeDebris(
		/* pos.x, pos.y   */ x, y,
		/* vel.x, vel.y   */ vel.x, vel.y,
		/* owner id 	  */ id,
		/* owner team	  */ team,
		/* kind 		  */ OBJ_SPARK,
		/* mass 		  */ THRUST_MASS * ALT_SPARK_MASS_FACT,
		/* status		  */ GRAVITY | OWNERIMMUNE,
		/* color		  */ BLUE,
		/* radius		  */ 8,
		/* min,max debris */ alt_sparks, alt_sparks,
		/* min,max dir	  */ min_dir, max_dir,
		/* min,max speed  */ 1.0, max_speed,
		/* min,max life   */ 3, max_life
		);
}


void Player::TurnThrust(int num_sparks)
{
	int 		x = pos.x + (int)ship->pts[0][dir].x;
	int 		y = pos.y + (int)ship->pts[0][dir].y;
	int 		_dir = dir + ((turnacc > 0.0) ? (RES/4) : (3*(RES/4)));

	world->MakeDebris(
		/* pos.x, pos.y   */ x, y,
		/* vel.x, vel.y   */ vel.x, vel.y,
		/* owner id 	  */ id,
		/* owner team		  */ team,
		/* kind 		  */ OBJ_SPARK,
		/* mass 		  */ THRUST_MASS,
		/* status		  */ GRAVITY | OWNERIMMUNE,
		/* color		  */ RED,
		/* radius		  */ 1,
		/* min,max debris */ num_sparks, num_sparks,
		/* min,max dir	  */ (int)(_dir - (RES*0.1) -1), (int)(_dir + (RES*0.1) + 1),
		/* min,max speed  */ 1, 3,
		/* min,max life   */ 1, 2*world->GetFPS()
		);
}

/* Calculates the recoil if a ship fires a shot */
void Recoil(Object *ship, Object *shot)
{
#if 0
/* old code, not used anymore. */
	ship->vel.x -= ((tcos(shot->dir) * ABS(shot->vel.x-ship->vel.x) *
		shot->mass) / ship->mass);
	ship->vel.y -= ((tsin(shot->dir) * ABS(shot->vel.y-ship->vel.y) *
		shot->mass) / ship->mass);
#else
/* new code thanks to Uoti Urpala. */
	ship->vel.x -= (((shot->vel.x - ship->vel.x) *
		shot->mass) / ship->mass);
	ship->vel.y -= (((shot->vel.y - ship->vel.y) *
		shot->mass) / ship->mass);
#endif
}

void Record_shove(Player *pl, Player *pusher, long time)
{
	Shove				*shove = &pl->shove_record[pl->shove_next];

	if (++pl->shove_next == MAX_RECORDED_SHOVES) {
		pl->shove_next = 0;
	}
	shove->pusher_id = pusher->id;
	shove->time = time;
}

/* Calculates the effect of a collision between two objects */
/* This calculates a completely inelastic collision. Ie. the
 * objects remain stuck together (same velocity and direction.
 * Use this function if one of the objects will die in the
 * collision. */
void Delta_mv(Object *ship, Object *obj)
{
	DFLOAT		vx, vy, m;

	m = ship->mass + ABS(obj->mass);
	vx = (ship->vel.x * ship->mass + obj->vel.x * obj->mass) / m;
	vy = (ship->vel.y * ship->mass + obj->vel.y * obj->mass) / m;
	if (ship->type == OBJ_PLAYER
		&& obj->id != NO_ID
		&& BIT(obj->status, COLLISIONSHOVE)) {
		Player *pl = (Player *)ship;
		Player *pusher = pl->world->players[pl->world->getInd[obj->id]];
		if (pusher != pl) {
			Record_shove(pl, pusher, frame_loops);
		}
	}
	ship->vel.x = vx;
	ship->vel.y = vy;
	obj->vel.x = vx;
	obj->vel.y = vy;
}

/* Calculates the effect of a collision between two objects */
/* And now for a completely elastic collision. Ie. the objects
 * will bounce off of eachother. Use this function if both
 * objects stay alive after the collision. */
void Delta_mv_elastic(Object *obj1, Object *obj2)
{
	DFLOAT		m1 = (DFLOAT)obj1->mass,
				m2 = (DFLOAT)obj2->mass,
				ms = m1 + m2;
	DFLOAT		v1x = obj1->vel.x,
				v1y = obj1->vel.y,
				v2x = obj2->vel.x,
				v2y = obj2->vel.y;

	obj1->vel.x = (m1 - m2) / ms * v1x
				  + 2 * m2 / ms * v2x;
	obj1->vel.y = (m1 - m2) / ms * v1y
				  + 2 * m2 / ms * v2y;
	obj2->vel.x = 2 * m1 / ms * v1x
				  + (m2 - m1) / ms * v2x;
	obj2->vel.y = 2 * m1 / ms * v1y
				  + (m2 - m1) / ms * v2y;
	if (obj1->type == OBJ_PLAYER
		&& obj2->id != NO_ID
		&& BIT(obj2->status, COLLISIONSHOVE)) {
		Player *pl = (Player *)obj1;
		Player *pusher = pl->world->players[pl->world->getInd[obj2->id]];
		if (pusher != pl) {
			Record_shove(pl, pusher, frame_loops);
		}
	}
}


/*
 * Add fuel to fighter's tanks.
 * Maybe use more than one of tank to store the fuel.
 */
void Add_fuel(PlayerFuel* ft, long fuel)
{
	if (ft->sum + fuel > ft->max)
		fuel = ft->max - ft->sum;
	else if (ft->sum + fuel < 0)
		fuel = -ft->sum;
	ft->sum += fuel;
	ft->tank[ft->current] += fuel;
}


/*
 * Move fuel from add-on tanks to main tank,
 * handle over and underflow of tanks.
 */
void Update_tanks(PlayerFuel *ft)
{
	if (ft->num_tanks) {
		int  t, check;
		long low_level;
		long fuel;
		long *f;

		/* Set low_level to minimum fuel in each tank */
		low_level = ft->sum / (ft->num_tanks + 1) - 1;
		if (low_level < 0)
			low_level = 0;
		if (TANK_REFILL_LIMIT < low_level)
			low_level = TANK_REFILL_LIMIT;

		t = ft->num_tanks;
		check = MAX_TANKS<<2;
		fuel = 0;
		f = ft->tank + t;

		while (t>=0 && check--) {
			long m = TANK_CAP(t);

			/* Add the previous over/underflow and do a new cut */
			*f += fuel;
			if (*f > m) {
				fuel = *f - m;
				*f = m;
			} else if (*f < 0) {
				fuel = *f;
				*f = 0;
			} else
				fuel = 0;

			/* If there is no over/underflow, let the fuel run to main-tank */
			if (!fuel) {
				if (t
					&& t != ft->current
					&& *f >= low_level + REFUEL_RATE
					&& *(f-1) <= TANK_CAP(t-1) - REFUEL_RATE) {

					*f -= REFUEL_RATE;
					fuel = REFUEL_RATE;
				} else if (t && *f < low_level) {
					*f += REFUEL_RATE;
					fuel = -REFUEL_RATE;
				}
			}
			if (fuel && t == 0) {
			   t = ft->num_tanks;
			   f = ft->tank + t;
			} else {
				t--;
				f--;
			}
		}
		if (!check) {
			error("fuel problem");
			fuel = ft->sum;
			ft->sum =
			ft->max = 0;
			t = 0;
			while (t <= ft->num_tanks) {
				if (fuel) {
					if (fuel>TANK_CAP(t)) {
						ft->tank[t] = TANK_CAP(t);
						fuel -= TANK_CAP(t);
					} else {
						ft->tank[t] = fuel;
						fuel = 0;
					}
					ft->sum += ft->tank[t];
				} else
					ft->tank[t] = 0;
				ft->max += TANK_CAP(t);
				t++;
			}
		}
	} else
		ft->tank[0] = ft->sum;
}


/*
 * Use current tank as dummy target for heat seeking missles.
 */
void Tank_handle_detach(Player *pl)
{
		World*			w = pl->world;
	Player				*dummy;
	int 				i, ct;

	if (BIT(pl->used, HAS_PHASING_DEVICE))
		return;

	/* Return, if no more players or no tanks */
	if (pl->fuel.num_tanks == 0
		|| w->numPseudoPlayers == MAX_PSEUDO_PLAYERS
		|| peek_ID() == 0) {
		return;
	}

	/* If current tank is main, use another one */
	if ((ct = pl->fuel.current) == 0)
		ct = pl->fuel.num_tanks;

	Update_tanks(&(pl->fuel));
	/* Fork the current player */
	dummy				= w->players[w->numPlayers];
	/*
	 * MWAAH: this was ... naieve at least:
	 * *dummy			   = *pl;
	 * Player structures contain pointers to dynamic memory...
	 */

	w->players[w->numPlayers]->Init((w->options.allowShipShapes->GetBool())
							? Parse_shape_str(w->options.tankShipShape->GetString())
							: NULL);
	/* Released tanks don't have tanks... */
	while (dummy->fuel.num_tanks > 0) {
				dummy->RemoveTank(dummy->fuel.num_tanks);
	}
		dummy->playerType = PlayerTank;
	Player_position_init_pixels(dummy, pl->pos.x, pl->pos.y);
	dummy->vel			= pl->vel;
	dummy->acc			= pl->acc;
	dummy->dir			= pl->dir;
	dummy->turnspeed	= pl->turnspeed;
	dummy->velocity 	= pl->velocity;
	dummy->float_dir	= pl->float_dir;
	dummy->turnresistance = pl->turnresistance;
	dummy->turnvel		= pl->turnvel;
	dummy->oldturnvel	= pl->oldturnvel;
	dummy->turnacc		= pl->turnacc;
	dummy->power		= pl->power;

	strlcpy(dummy->name, pl->name, MAX_CHARS);
	strlcat(dummy->name, "'s tank", MAX_CHARS);
	strlcpy(dummy->realname, w->options.tankRealName->GetString(), MAX_CHARS);
	strlcpy(dummy->hostname, w->options.tankHostName->GetString(), MAX_CHARS);
	dummy->home_base	= pl->home_base;
	dummy->team 		= pl->team;
	dummy->pseudo_team	= pl->pseudo_team;
		dummy->alliance 		= ALLIANCE_NOT_SET;
		dummy->invite			= NO_ID;
	dummy->mychar		= 'T';
	dummy->score		= pl->score - w->options.tankScoreDecrement->GetInt();

	/* Fuel is the one from choosen tank */
	dummy->fuel.sum 	=
	dummy->fuel.tank[0] = pl->fuel.tank[ct];
	dummy->fuel.max 	= TANK_CAP(ct);
	dummy->fuel.current = 0;
	dummy->fuel.num_tanks = 0;


	/* Mass is only tank + fuel */
	dummy->mass = (dummy->emptymass = w->options.shipMass->GetDouble()) + FUEL_MASS(dummy->fuel.sum);
	dummy->power *= TANK_THRUST_FACT;

	/* Reset visibility. */
	dummy->updateVisibility = 1;
	for (i = 0; i <= w->numPlayers; i++) {
		dummy->visibility[i].lastChange = 0;
		w->players[i]->visibility[w->numPlayers].lastChange = 0;
	}

	/* Remember whose tank this is */
	dummy->lock.pl_id = pl->id;

	request_ID();
	w->numPlayers++;
	w->numPseudoPlayers++;
	w->updateScores = true;

		/* Possibly join alliance. */
		if (pl->alliance != ALLIANCE_NOT_SET) {
				pl->JoinAlliance(w->getInd[dummy->id]);
		}

	SoundPlaySensors(w, pl->pos.x, pl->pos.y, TANK_DETACH_SOUND);

	/* The tank uses shield and thrust */
	dummy->status = (DEF_BITS & ~KILL_BITS) | PLAYING | GRAVITY | THRUSTING;
	dummy->have = DEF_HAVE;
	dummy->used = (DEF_USED & ~USED_KILL & pl->have) | HAS_SHIELD;
	if (w->options.allowShields->GetBool() == false) {
		dummy->shield_time = 30 * w->GetFPS();
		dummy->have |= HAS_SHIELD;
	}

	/* Maybe heat-seekers to retarget? */
	for (i=0; i < w->numObjs; i++) {
		if (w->objs[i]->type == OBJ_HEAT_SHOT
				&& w->objs[i]->info > 0
				&& w->players[ w->getInd[w->objs[i]->info] ] == pl) {
						w->objs[i]->info = w->numPlayers - 1;
		}
		}

	/* Remove tank, fuel and mass from myself */
		pl->RemoveTank(ct);

	for (i = 0; i < w->numPlayers - 1; i++) {
		if (w->players[i]->conn) {
			w->players[i]->conn->SendPlayer(dummy);
			w->players[i]->conn->SendScore(dummy);
		}
	}
}


void World::MakeWreckage(
	/* pos.x, pos.y 	*/ DFLOAT x,			DFLOAT y,
	/* vel.x, vel.y 	*/ DFLOAT velx, 		DFLOAT vely,
	/* owner id 		*/ int	  id,
	/* owner team		*/ unsigned short team,
	/* min,max mass 	*/ DFLOAT min_mass, 	DFLOAT max_mass,
	/* total mass		*/ DFLOAT total_mass,
	/* status			*/ long   status,
	/* color			*/ int	  color,
	/* max wreckage 	*/ int	  max_wreckage,
	/* min,max dir		*/ int	  min_dir,		int    max_dir,
	/* min,max speed	*/ DFLOAT min_speed,	DFLOAT max_speed,
	/* min,max life 	*/ int	  min_life, 	int    max_life
)
{
	WireObject			*wreckage;
	int 				i, life, size;
	modifiers			mods;
	DFLOAT				mass, sum_mass = 0.0;

	if (!options.useWreckage->GetBool()) {
		return;
	}
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
		}
	}

	if (min_speed * max_life > hypotenuse)
		min_speed = hypotenuse / max_life;
	if (max_speed * min_life > hypotenuse)
		max_speed = hypotenuse / min_life;
	if (max_speed < min_speed)
		max_speed = min_speed;

	if (max_wreckage > MAX_TOTAL_SHOTS - numObjs) {
		max_wreckage = MAX_TOTAL_SHOTS - numObjs;
	}

	CLEAR_MODS(mods);

	for (i = 0; i < max_wreckage && sum_mass < total_mass; i++) {
		DFLOAT			speed;
		int 			dir, radius;

//		wreckage = WIRE_IND(NumObjs);
		/* Calculate mass */
		mass = min_mass + rfrac() * (max_mass - min_mass);
		if (sum_mass + mass > total_mass) {
				mass = total_mass - sum_mass;
		}
		if (mass < min_mass) {
				/* not enough mass available. */
				break;
		}

		/* Allocate object */
		if ((wreckage = WIRE_PTR(Object_allocate(this))) == NULL) {
				break;
		}

		wreckage->color = color;
		wreckage->id = id;
		wreckage->team = team;
		wreckage->type = OBJ_WRECKAGE;

		/* Position */
		Object_position_init_pixels(OBJ_PTR(wreckage), x, y);

		/* Direction */
		dir = MOD2(min_dir + (int)(rfrac() * MOD2(max_dir - min_dir, RES)), RES);

		/* Velocity and acceleration */
		speed = min_speed + rfrac() * (max_speed - min_speed);
		wreckage->vel.x = velx + tcos(dir) * speed;
		wreckage->vel.y = vely + tsin(dir) * speed;
		wreckage->acc.x = 0;
		wreckage->acc.y = 0;

		/* Mass */
		wreckage->mass = mass;
		sum_mass += mass;

		/* Lifespan  */
		life = (int)(min_life + rfrac() * (max_life - min_life) + 1);
		if (life * speed > hypotenuse) {
			life = (long)(hypotenuse / speed);
		}
		wreckage->life = life;
		wreckage->fuselife = wreckage->life;

		/* Wreckage type, rotation, and size */
		wreckage->turnspeed = 0.02 + rfrac() * 0.35;
		wreckage->rotation = (int)(rfrac() * RES);
		size = (int) ( 256.0 * 1.5 * mass / total_mass );
		if ( size > 255 )
			size = 255;
		wreckage->size = size;
		wreckage->info = (int)(rfrac() * 256);

		radius = wreckage->size * 16 / 256;
		if ( radius < 8 ) radius = 8;

		wreckage->pl_range = radius;
		wreckage->pl_radius = radius;
		wreckage->status = status;
		wreckage->mods = mods;
		CellAddObject(wreckage);
	}
}

/* Explode a fighter */
void Player::ExplodeFighter()
{
	int min_debris, max_debris;

	SoundPlaySensors(world, pos.x, pos.y, PLAYER_EXPLOSION_SOUND);

	min_debris = (int)(1 + (fuel.sum / (8.0 * FUEL_SCALE_FACT)));
	max_debris = (int)(min_debris + (mass * 2.0));
	/* reduce debris since we also create wreckage objects */
	min_debris >>= 1;
	max_debris >>= 1;

	world->MakeDebris(
		/* pos.x, pos.y   */ pos.x, pos.y,
		/* vel.x, vel.y   */ vel.x, vel.y,
		/* owner id 	  */ id,
		/* owner team	  */ team,
		/* kind 		  */ OBJ_DEBRIS,
		/* mass 		  */ 3.5,
		/* status		  */ GRAVITY,
		/* color		  */ RED,
		/* radius		  */ 8,
		/* min,max debris */ min_debris, max_debris,
		/* min,max dir	  */ 0, RES-1,
		/* min,max speed  */ 20.0, 20 + (((int)(mass))>>1),
		/* min,max life   */ 5, (int)(5 + (mass * 1.5))
		);

	if ( !BIT(status, KILLED) )
		return;
	world->MakeWreckage(
		/* pos.x, pos.y 	*/ pos.x, pos.y,
		/* vel.x, vel.y 	*/ vel.x, vel.y,
		/* owner id 		*/ id,
		/* owner team		*/ team,
		/* min,max mass 	*/ MAX(mass/8.0, 0.33), mass,
		/* total mass		*/ 2.0 * mass,
		/* status			*/ GRAVITY,
		/* color			*/ WHITE,
		/* max wreckage 	*/ 10,
		/* min,max dir		*/ 0, RES-1,
		/* min,max speed	*/ 10.0, 10 + (((int)(mass))>>1),
		/* min,max life 	*/ 5, (int)(5 + (mass * 1.5))
		);

}

