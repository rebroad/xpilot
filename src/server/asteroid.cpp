/* $Id: asteroid.cpp,v 1.12 2004/04/29 16:08:27 dick Exp $
 *
 * XPilot, a multiplayer gravity war game.  Copyright (C) 1991-2002 by
 *
 *      Bj�rn Stabell        <bjoern@xpilot.org>
 *      Ken Ronny Schouten   <ken@xpilot.org>
 *      Bert Gijsbers        <bert@xpilot.org>
 *      Dick Balaska         <dick@xpilot.org>
 *  	Kimiko Koopman       <kimiko@xpilot.org>
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
 */
/*
 *  $Log: asteroid.cpp,v $
 *  Revision 1.12  2004/04/29 16:08:27  dick
 *  compile with SOUND enabled.
 *
 *  Revision 1.11  2004/02/03 03:39:54  dick
 *  Add Windows precompiled header support.
 *
 *  Revision 1.10  2004/01/08 18:10:51  dick
 *  Update to XPilot 4.5.5beta
 *
 *  Revision 1.9  2002/09/11 16:42:04  dick
 *  Here endeth the server encapsulation task.
 *  theWorld is now an array of World(s) and get loaded dynamically.
 *
 *  Revision 1.8  2002/09/01 00:13:46  dick
 *  Remove obsolete windows #include "winServer.h"
 *
 *  Revision 1.7  2002/08/01 14:31:20  dick
 *  Remove references to theWorld
 *
 *  Revision 1.6  2002/06/01 06:06:48  dick
 *  Encapsulate (almost) everything.  Get rid of (almost) all refs to theWorld.
 *
 *  Revision 1.5  2002/05/18 20:55:38  dick
 *  Update to XPilot-4.5.1
 *
 *  Revision 1.4  2001/07/24 09:03:59  dick
 *  Remove all global options.  Use World.options instead.
 *
 *  Revision 1.3  2001/07/21 07:21:41  dick
 *  Slay some float to int warnings
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
#include "list.h"
#include "global.h"
#include "proto.h"
#include "saudio.h"
#include "bit.h"
#include "objpos.h"
#include "asteroid.h"


char asteroid_version[] = VERSION;


/* list containing pointers to all asteroids */
static list_t	Asteroid_list = NULL;


/*
** Prototypes.
*/
static void MakeAsteroid(World* w, DFLOAT x, DFLOAT y,
						  int size, int dir,
						  DFLOAT speed);


/*
** Return the asteroid list.
*/
list_t Asteroid_get_list(void)
{
	return Asteroid_list;
}


static bool Asteroid_add_to_list(WireObject *ast)
{
	list_iter_t 		list_pos;
	bool				result = false;

	if (Asteroid_list == NULL) {
		Asteroid_list = List_new();
	}

	if (Asteroid_list != NULL) {
		list_pos = List_push_back(Asteroid_list, ast);
		if (list_pos != NULL) {
			result = true;
		}
	}

	return result;
}


static bool Asteroid_remove_from_list(WireObject *ast)
{
	list_iter_t 		list_pos;
	bool				result = false;

	if (Asteroid_list != NULL) {
		list_pos = List_find(Asteroid_list, ast);
		if (list_pos != List_end(Asteroid_list)) {
			List_erase(Asteroid_list, list_pos);
			result = true;
		}
	}

	return result;
}


/*
 * Breaks up an asteroid:
 * Asteroids of size > 1 break up into two asteroids of size n - 1
 * and some wreckage and debris.
 * Asteroids of size 1 break up into wreckage and debris only.
 * In both cases the sum of the masses of the smaller asteroids,
 * the wreckage and the debris should be about equal to the mass
 * of the original asteroid.
 */
void Break_asteroid(World* w, WireObject* asteroid)
{
//	  WireObject		*asteroid = WIRE_IND(ind);
	DFLOAT		mass, mass3;
	DFLOAT		speed, speed1, speed2, radius;
	DFLOAT		velx1, vely1, velx2, vely2, velx3, vely3;
	int 		dir, dir1, dir2, split_dir;
	int 		x1, y1, x2, y2;

	if (asteroid->size == 1) {
		mass = asteroid->mass / 2;
		w->MakeWreckage(asteroid->pos.x, asteroid->pos.y,
					  asteroid->vel.x, asteroid->vel.y,
					  -1,
					  TEAM_NOT_SET,
					  mass / 20, mass / 3,
					  mass,
					  GRAVITY,
					  WHITE,
					  10,
					  0, RES-1,
					  5, 10,
					  3, 10);
		w->MakeDebris(asteroid->pos.x, asteroid->pos.y,
					asteroid->vel.x, asteroid->vel.y,
					-1,
					TEAM_NOT_SET,
					OBJ_DEBRIS,
					mass,
					GRAVITY,
					RED,
					8,
					20, 50,
					0, RES-1,
					5, 10,
					3, 10);
	} else {
		/* foo[12] refer to the mini-asteroids
		   foo3 refers to the wreckage and debris */
		speed = VECTOR_LENGTH(asteroid->vel);
		dir = (int)FindDir(asteroid->vel.x, asteroid->vel.y);
		mass3 = asteroid->mass * ASTEROID_DUST_FACT;
		mass = ASTEROID_MASS(w, asteroid->size - 1);
		dir1 = MOD2((int)(dir
						  - ASTEROID_DELTA_DIR / 4
						  - (rfrac() * ASTEROID_DELTA_DIR / 4)), RES);
		dir2 = MOD2((int)(dir
						  + ASTEROID_DELTA_DIR / 4
						  + (rfrac() * ASTEROID_DELTA_DIR / 4)), RES);
		speed1 = (speed * (1 - ASTEROID_DUST_FACT))
				 / tcos(ABS(dir - dir1));
		speed2 = (speed * (1 - ASTEROID_DUST_FACT))
				 / tcos(ABS(dir2 - dir));
		velx1 = tcos(dir1) * speed1;
		vely1 = tsin(dir1) * speed1;
		velx2 = tcos(dir2) * speed2;
		vely2 = tsin(dir2) * speed2;
		split_dir = MOD2(dir - RES/4, RES);
		radius = ASTEROID_RADIUS(asteroid->size - 1);
		x1 = (int)WRAP_XPIXEL(w, asteroid->pos.x + tcos(split_dir) * radius);
		y1 = (int)WRAP_YPIXEL(w, asteroid->pos.y + tsin(split_dir) * radius);
		x2 = (int)WRAP_XPIXEL(w, asteroid->pos.x - tcos(split_dir) * radius);
		y2 = (int)WRAP_YPIXEL(w, asteroid->pos.y - tsin(split_dir) * radius);
		velx3 = asteroid->vel.x;
		vely3 = asteroid->vel.y;
		MakeAsteroid(w, x1, y1, asteroid->size - 1, dir1, speed1);
		MakeAsteroid(w, x2, y2, asteroid->size - 1, dir2, speed2);
		w->MakeWreckage(asteroid->pos.x, asteroid->pos.y,
					  velx3, vely3,
					  -1,
					  TEAM_NOT_SET,
					  mass3 / 20, mass3 / 3,
					  mass3 / 2,
					  GRAVITY,
					  WHITE,
					  10,
					  0, RES-1,
					  5, 10,
					  3, 10);
		w->MakeDebris(asteroid->pos.x, asteroid->pos.y,
					velx3, vely3,
					-1,
					TEAM_NOT_SET,
					OBJ_DEBRIS,
					mass3 / 2,
					GRAVITY,
					RED,
					8,
					20, 50,
					0, RES-1,
					5, 10,
					3, 10);
	}
	if ((w->options.asteroidMaxItems->GetInt() > 0)
				&& (rfrac() < w->options.asteroidItemProb->GetDouble())) {
		int 	nitems = (int)(rfrac() * w->options.asteroidMaxItems->GetInt()) + 1;
		int 	i;
		int 	vx, vy;
		int 	item, item_dir, num_per_pack;
		DFLOAT	item_speed;
		long	status;

		for (i = 0; i < nitems; i++) {
			item = w->ChooseRandomItem();
			item_dir = (int)(rfrac() * RES);
			item_speed = rfrac() * 10;
			vx = (int)(asteroid->vel.x + item_speed * tcos(item_dir));
			vy = (int)(asteroid->vel.y + item_speed * tsin(item_dir));
			status = GRAVITY;
			if (rfrac() < w->options.randomItemProb->GetDouble())
				status |= RANDOM_ITEM;
			if (w->items[item].min_per_pack == w->items[item].max_per_pack) {
				num_per_pack = w->items[item].max_per_pack;
			} else {
				num_per_pack = w->items[item].min_per_pack
							 + (int)(rfrac() * (1 + w->items[item].max_per_pack
												  - w->items[item].min_per_pack));
			}

			w->MakeItem(asteroid->pos.x, asteroid->pos.y,
					  vx, vy,
					  item, num_per_pack,
					  status);
		}
	}
	SoundPlaySensors(w, asteroid->pos.x, asteroid->pos.y, ASTEROID_BREAK_SOUND);

	w->asteroids.num -= 1 << (asteroid->size - 1);

	Asteroid_remove_from_list(asteroid);
}


/*
 * Creates an asteroid with the given characteristics.
 */
static void MakeAsteroid(World* w, DFLOAT x, DFLOAT y,
						  int size, int dir,
						  DFLOAT speed)
{
	WireObject	*asteroid;
	DFLOAT		radius;
		int 			bx;
		int 			by;

	if (w->numObjs >= MAX_TOTAL_SHOTS) {
		return;
	}

	if (size < 1 || size > ASTEROID_MAX_SIZE) {
		return;
	}

	if (BIT(w->rules->mode, WRAP_PLAY)) {
		if (x < 0) x += w->width;
		else if (x >= w->width) x -= w->width;
		if (y < 0) y += w->height;
		else if (y >= w->height) y -= w->height;
	}
	if (x < 0 || x >= w->width || y < 0 || y >= w->height) {
		return;
	}
	bx = (int)(x / BLOCK_SZ);
		by = (int)(y / BLOCK_SZ);
	if (BIT(w->block[bx][by], FILLED_BIT|FUEL_BIT|TARGET_BIT|TREASURE_BIT)) {
		return;
	} else if (BIT(w->block[bx][by], REC_LU|REC_RU|REC_LD|REC_RD)) {
		DFLOAT	x_in_b = x - bx * BLOCK_SZ,
				y_in_b = y - by * BLOCK_SZ;
		switch (w->block[bx][by]) {
		case REC_LU:
			if (x_in_b < y_in_b)
				return;
			break;
		case REC_RU:
			if (x_in_b + y_in_b > BLOCK_SZ)
				return;
			break;
		case REC_LD:
			if (x_in_b + y_in_b < BLOCK_SZ)
				return;
			break;
		case REC_RD:
			if (x_in_b > y_in_b)
				return;
			break;
		}
	}

	//asteroid = WIRE_IND(NumObjs);
		asteroid = WIRE_PTR(Object_allocate(w));
		if (!asteroid)
				return;
	asteroid->color = WHITE;
	asteroid->id = NO_ID;
	asteroid->team = TEAM_NOT_SET;
	asteroid->type = OBJ_ASTEROID;

	/* Position */
	Object_position_init_pixels(OBJ_PTR(asteroid), x, y);

	asteroid->vel.x = tcos(dir) * speed;
	asteroid->vel.y = tsin(dir) * speed;
	asteroid->acc.x = asteroid->acc.y = 0;
	asteroid->mass = ASTEROID_MASS(w, size);
	asteroid->life = ASTEROID_LIFE;
	asteroid->turnspeed = 0.02 + rfrac() * 0.05;
	asteroid->rotation = (int)(rfrac() * RES);
	asteroid->size = size;
	asteroid->info = (int)(rfrac() * 256);
	radius = ASTEROID_RADIUS(size);
	asteroid->pl_range = (int)radius;
	asteroid->pl_radius = (int)radius;
	asteroid->fuselife = asteroid->life - 1;
	asteroid->status = GRAVITY;
	CLEAR_MODS(asteroid->mods);

		if (Asteroid_add_to_list(asteroid) == true) {
				w->asteroids.num += 1 << (size - 1);
				w->CellAddObject(OBJ_PTR(asteroid));
	}
		else {
				Object_free_ptr(OBJ_PTR(asteroid));
		}
}


/*
 * Tries to place a new asteroid on the map.
 * Calls MakeAsteroid() to actually create the new asteroid
 */
static void PlaceAsteroid(World* w)
{
	int 		place_count;
	int 		px = 0;
		int 			py = 0;
	int 		bx, by;
		int 			dir;
		int 			dist;
	unsigned	space;
	int 		okay;
		AsteroidConcentrator*	con;

	space = SPACE_BLOCKS;
	space &= ~(BASE_BIT | WORMHOLE_BIT);
	space |= FRICTION_BIT;
	/* would be dubious: space |= CANNON_BIT; */

		if (w->numAsteroidConcs > 0
				&& rfrac() < w->options.asteroidConcentratorProb->GetDouble())
		{
				con = &w->asteroidConcs[(int)(rfrac() * w->numAsteroidConcs)];
	} else {
				con = NULL;
	}

   /* we bail out after 8 unsuccessful attempts to avoid wasting
	 * too much time on crowded maps */
	okay = false;
	for (place_count = 0; okay != true; place_count++) {
		if (place_count >= 10) {
			return;
		}
		if (con) {
			dir = (int)(rfrac() * RES);
			dist = (int)(rfrac() * ((w->options.asteroidConcentratorRadius->GetInt() * BLOCK_SZ) + 1));
			px = (int)((con->pos.x + 0.5) * BLOCK_SZ + dist * tcos(dir));
			py = (int)((con->pos.y + 0.5) * BLOCK_SZ + dist * tsin(dir));

			if (BIT(w->rules->mode, WRAP_PLAY)) {
				if (px < 0) px += w->width;
				if (py < 0) py += w->height;
				if (px > w->width) px -= w->width;
				if (py > w->height) py -= w->height;
			}
			if (px < 0 || px >= w->width
				|| py < 0 || py >= w->height) {
				continue;
			}
		} else {
			px = (int)(rfrac() * w->width);
			py = (int)(rfrac() * w->height);
		}
		bx = px / BLOCK_SZ;
		by = py / BLOCK_SZ;

		if (BIT(1U << w->block[bx][by], space)) {
			int i, dpx, dpy, ox, oy;

			okay = true;

			for (i = 0; i < w->numPlayers; i++) {
				if (IS_HUMAN_IND(w, i)) {
					ox = OBJ_X_IN_PIXELS(w->players[i]);
					oy = OBJ_Y_IN_PIXELS(w->players[i]);
					dpx = WRAP_DX(w, px - ox);
					dpy = WRAP_DY(w, py - oy);
					if (sqr(dpx) + sqr(dpy) < sqr(ASTEROID_MIN_DIST)) {
						/* too close to player */
						okay = false;
						break;
					}
				}
			}
		}
	}
	if (okay == true) {
		MakeAsteroid(w, px, py,
					  (int)(1 + rfrac() * ASTEROID_MAX_SIZE),
					  (int)(rfrac() * RES),
					  (DFLOAT)ASTEROID_START_SPEED);
	}
}


static void AsteroidMove(World* w, WireObject *wireobj)
{
	w->MoveObject((Object *) wireobj);
}


static void Asteroid_rotate(WireObject *wireobj)
{
	wireobj->rotation =
		(wireobj->rotation + (int) (wireobj->turnspeed * RES)) % RES;
}


/*
** Called once each frame update to update everything
** related to asteroids including creation, destruction,
** rotation and movement.
*/
void Asteroid_update(World* w)
{
	int 		num;
	list_t		list;
	list_iter_t iter;
	WireObject	*asteroid;

	list = Asteroid_get_list();
	if (list) {
		/* if there are more asteroids than are wanted, mark
		 * all asteroids to be removed (by Delete_shot()),
		 * until enough of size 1 have been removed
		 * (only breaking of size 1 actually lowers the
		 * total number of asteroids)
		 * one iteration may not remove enough asteroids
		 * the rest are left until the next frame then
		 * */
		num = w->asteroids.num;
		if (num > w->asteroids.max) {
			for (iter = List_begin(list);
				 iter != List_end(list);
				 LI_FORWARD(iter)) {
				asteroid = (WireObject *) LI_DATA(iter);
				if (asteroid->life > 0) {
					asteroid->life = 0;
					if (asteroid->size == 1) {
						num--;
					}
				}
				if (num <= w->asteroids.max) {
					break;
				}
			}
		}

		/* rotate asteroids */
		for (iter = List_begin(list);
			 iter != List_end(list);
			 LI_FORWARD(iter)) {
			asteroid = (WireObject *) LI_DATA(iter);
			if (asteroid->life > 0) {
				Asteroid_rotate(asteroid);
			}
		}

		/* move asteroids */
		for (iter = List_begin(list);
			 iter != List_end(list);
			 LI_FORWARD(iter)) {
			asteroid = (WireObject *) LI_DATA(iter);
			if (asteroid->life > 0) {
				AsteroidMove(w, asteroid);
			}
		}
	}

	/* place new asteroid if room left */
	if (w->asteroids.chance > 0) {
		int incr = (1 << (ASTEROID_MAX_SIZE - 1));
		if (w->asteroids.num + incr < w->asteroids.max) {
			if ((rfrac() * w->asteroids.chance) < 1.0f) {
				PlaceAsteroid(w);
			}
		}
	}
}

