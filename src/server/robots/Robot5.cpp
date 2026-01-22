/* $Id: Robot5.cpp,v 1.10 2007/02/12 07:53:34 dick Exp $
 *
 * The XPilot4 series default robot driver
 * with some enhancements
 *
 * XPilot, a multiplayer gravity war game.  Copyright (C) 1991-2007 by
 *
 *      Dick Balaska         <dick@xpilot.org>
 *      Bert Gijsbers        <bert@xpilot.org>
 *      Bj�rn Stabell        <bjoern@xpilot.org>
 *      Ken Ronny Schouten   <ken@xpilot.org>
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
 *  $Log: Robot5.cpp,v $
 *  Revision 1.10  2007/02/12 07:53:34  dick
 *  Robot5s are little item pigs now.
 *  Still playing with the shields/phasing.  I think i made it worse...
 *
 *  Revision 1.9  2007/01/20 19:28:07  dick
 *  Comment out unused debug
 *
 *  Revision 1.8  2007/01/19 20:29:25  dick
 *  Rename Robot4a to Robot5 to promote the xp5 brand.
 *
 *  Revision 1.7  2007/01/19 20:07:12  dick
 *  There is a lot of shield wasting by the robots.
 *  Tune the wastage based on the defense attribute. (less defense = less wasting.)
 *
 *  Revision 1.6  2007/01/19 07:13:00  dick
 *  Shields work.
 *  Rename RM_* to RM4_* because i keep editing Robot4.cpp by mistake because
 *  the symbol browser takes me there. :)
 *
 *  Revision 1.5  2007/01/18 21:19:09  dick
 *  Work on CheckRobotNavigate
 *
 *  Revision 1.4  2007/01/17 23:11:10  dick
 *  More Watch()ing
 *
 *  Revision 1.3  2007/01/17 21:39:01  dick
 *  Fix the Watch strings
 *
 *  Revision 1.2  2007/01/17 09:16:04  dick
 *  Use the Watch() function to gather robot brain data
 *
 *  Revision 1.1  2007/01/14 22:10:15  dick
 *  Robot4 is the original xpilot 4 series robot.
 *  Robot4a is dick's playing around with making them better.
 *
 */
/*
 * Robot code originally submitted by Maurice Abraham.
 */
#include "StdAfx.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <errno.h>
#include <limits.h>
#include <assert.h>

#ifndef _WINDOWS
# include <unistd.h>
#endif

#include "version.h"
#include "config.h"
#include "serverconst.h"
#include "global.h"
#include "randommt.h"
#include "proto.h"
#include "showtime.h"
#include "score.h"
#include "bit.h"
#include "saudio.h"
#include "pack.h"
#include "error.h"
#include "portability.h"
#include "commonproto.h"
#include "Cannon.h"

#include "Robot5.h"

#ifdef	_DEBUG
#include "NetServer.h"
#endif

// Debug object table via robots
#if 0
#define	DR(__x)	D(__x)
#else
#define	DR(__x)
#endif
#define DRT()											\
	DR(	int	_i;											\
		for (_i=0; _i<w->numObjs; _i++) {				\
			assert(w->objs[_i]->type <= 0x2000);		\
		}												\
	)													\
														\

char robot4a_version[] = VERSION;

#define ROB_LOOK_AH 			2

#define WITHIN(NOW,THEN,DIFF) (NOW<=THEN && (THEN-NOW)<DIFF)

/*
 * Flags for the default robots being in different modes (or moods).
 */
#define RM5_ROBOT_IDLE			(1 << 2)	// 0004	   4
#define RM5_EVADE_LEFT			(1 << 3)	// 0008	   8
#define RM5_EVADE_RIGHT			(1 << 4)	// 0010	  16
#define RM5_ROBOT_CLIMB			(1 << 5)	// 0020	  32
#define RM5_HARVEST				(1 << 6)	// 0040	  64
#define RM5_ATTACK				(1 << 7)	// 0080	 128
#define RM5_TAKE_OFF 			(1 << 8)	// 0100	 256
#define RM5_CANNON_KILL			(1 << 9)	// 0200	 512
#define RM5_REFUEL				(1 << 10)	// 0400	1024
#define RM5_NAVIGATE 			(1 << 11)	// 0800	2048

/* long term modes */
#define FETCH_TREASURE			(1 << 0)
#define TARGET_KILL 			(1 << 1)
#define NEED_FUEL				(1 << 2)

/*
 * Map objects a robot can fly through without damage.
 */
#define EMPTY_SPACE(s)	BIT(1 << (s), SPACE_BLOCKS)

PCSTR	itemTypeText[] = {"ITEM_FUEL", "WIDEANGLE", "REARSHOT", "AFTERBURNER",
						  "CLOAK", "SENSOR", "TRANSPORTER", "TANK",
						  "MINE", "MISSILE", "ECM", "LASER",
						  "ETHRUST", "TRACTOR_BEAM", "AUTOPILOT", "ESHIELD",
						  "DEFLECTOR", "HYPERJUMP", "PHASING", "MIRROR",
						  "ARMOR" };


///////////////////////////////////////////////////////////////////////////////
// static
Robot* Robot5::Factory() {
	Robot5* r = new Robot5();
	return(r);
}

///////////////////////////////////////////////////////////////////////////////
Robot5::Robot5() {
}

///////////////////////////////////////////////////////////////////////////////
Robot5::~Robot5() {
}


///////////////////////////////////////////////////////////////////////////////
/*
 * A robot is created.
 */
void Robot5::Create(char *str) {
	//Trace("Robot5::Create %s\n", str);

	robot_mode 	 = RM5_TAKE_OFF;
	robot_count	 = 0;
	robot_lock 	 = LOCK_NONE;
	robot_lock_id	 = 0;

	if (str != NULL
		&& *str != '\0'
		&& sscanf(str, " %d %d", &attack, &defense) != 2) {
		if (str && *str) {
			xpprintf("%sinvalid parameters for Robot5: \"%s\"\n", showtime(), str);
			attack = (int)(rfrac() * 99.5f);
			defense = 100 - attack;
		}
		LIMIT(attack, 1, 99);
		LIMIT(defense, 1, 99);
	}
	/*
	 * some parameters which may be changed to be dependent upon
	 * the `attack' and `defense' settings of this robot.
	 */
	if (BIT(w->rules->mode, TIMING)) {
		robot_normal_speed = 10.0;
		robot_attack_speed = 25.0 + (attack / 10);
		robot_max_speed = 50.0 + (attack / 20) - (defense / 50);
	} else {
		robot_normal_speed = 6.0;
		robot_attack_speed = 15.0 + (attack / 25);
		robot_max_speed = 30.0 + (attack / 50) - (defense / 50);
	}

	pl->fuel.l3 += defense - attack + (int)((rfrac() - 0.5f) * 20);
	pl->fuel.l2 += 2 * (defense - attack) / 5 + (int)((rfrac() - 0.5f) * 8);
	pl->fuel.l1 += (defense - attack) / 5 + (int)((rfrac() - 0.5f) * 4);

	last_used_ecm		= 0;
	last_dropped_mine	= 0;
	last_fired_missile	= 0;
	last_thrown_ball	= 0;

	longterm_mode		= 0;
	lastDeltaDir		= 0;
}

///////////////////////////////////////////////////////////////////////////////
/*
 * A default robot is placed on its homebase.
 */
void Robot5::GoHome() {
	robot_mode 	 = RM5_TAKE_OFF;
	longterm_mode	 = 0;
}

///////////////////////////////////////////////////////////////////////////////
/*
 * A default robot is declaring war (or resetting war).
 */
void Robot5::SetWar(int victim_id) {
	if (victim_id == NO_ID) {
		CLR_BIT(robot_lock, LOCK_PLAYER);
	} else {
		robot_lock_id = victim_id;
		SET_BIT(robot_lock, LOCK_PLAYER);
	}
}

///////////////////////////////////////////////////////////////////////////////
/*
 * Return the id of the player a default robot has war against (or NO_ID).
 */
int Robot5::WarOnPlayer() {
	if (BIT(robot_lock, LOCK_PLAYER)) {
		return robot_lock_id;
	} else {
		return NO_ID;
	}
}

///////////////////////////////////////////////////////////////////////////////
/*
 * A default robot receives a message.
 */
void Robot5::Message(const char *message) {
#if 0
	int 	len;
	char*	ptr;
	char	sender_name[MAX_NAME_LEN];
	char	msg[MSG_LEN];

	/*
	 * Extract the message body and the sender name from the message string.
	 */

	ptr = strrchr(message, ']');/* get end of message */
	if (!ptr) {
		return; 		/* Make sure to ignore server messages, oops! */
	}
	while (*--ptr != '[');		/* skip back over receiver name */
	while (*--ptr != '[');		/* skip back over sender name */
	strlcpy(sender_name, ptr + 1, sizeof sender_name);	/* copy sender */
	len = ptr - message;		/* find read message length */
	if (message[len] == ' ') {	/* ignore the readability space */
		len--;
	}
	strlcpy(msg, message, len); /* copy real message */
	if ((ptr = strchr(sender_name, ']')) != NULL) {
		*ptr = '\0';	/* remove the ']' separator */
	}
	printf("%s got message \"%s\" from \"%s\"\n", pl->name, msg, sender_name);
#endif
}

///////////////////////////////////////////////////////////////////////////////
/*
 * A robot is destroyed.
 */
void Robot5::Destroy() {
}

///////////////////////////////////////////////////////////////////////////////
/*
 * A robot is asked to join an alliance
 */
void Robot5::Invite(int inv_ind) {
	Player* 		inviter = w->players[inv_ind];
	int 				war_id = WarOnPlayer();
	int 				i;
	DFLOAT				limit;
	int 				accept = 1; 	/* accept by default */

	if (pl->alliance != ALLIANCE_NOT_SET) {
		/* if there is a human in our alliance, they should decide
		   let robots refuse in this case */
		for (i = 0; i < w->numPlayers; i++) {
			if (IS_HUMAN_IND(w, i) && ALLIANCE(w, pl->Ind(), i)) {
				accept = 0;
				break;
			}
		}
		if (!accept) {
			pl->RefuseAlliance(inv_ind);
			return;
		}
	}
	limit = MAX(ABS(pl->score / MAX((attack / 10), 10)),
				defense);
	if (inviter->alliance == ALLIANCE_NOT_SET) {
		/* don't accept players we are at war with */
		if (inviter->id == war_id)
			accept = 0;
		/* don't accept players who are not active */
		if (BIT(inviter->status, PLAYING|GAME_OVER|PAUSE) != PLAYING)
			accept = 0;
		/* don't accept players with scores substantially lower than ours */
		else if (inviter->score < (pl->score - limit))
			accept = 0;
	}
	else {
		DFLOAT	avg_score = 0;
		int 	member_count = Get_alliance_member_count(w, inviter->alliance);

		for (i = 0; i < w->numPlayers; i++) {
			if (w->players[i]->alliance == inviter->alliance) {
				if (w->players[i]->id == war_id) {
					accept = 0;
					break;
				}
				avg_score += w->players[i]->score;
			}
		}
		if (accept) {
			avg_score = avg_score / member_count;
			if (avg_score < (pl->score - limit)) {
				accept = 0;
			}
		}
	}
	if (accept) {
		pl->AcceptAlliance(inv_ind);
	}
	else {
		pl->RefuseAlliance(inv_ind);
	}
}

///////////////////////////////////////////////////////////////////////////////
// This function is used all the way through (causes action)
// only if we are really really close to a wall.
bool Robot5::CheckNavigate(bool * isEvade)
{
	int 		i, j, k;
	int 		area_val[10][10];
	int 		calc_val[10][10];
	int 		locn_block;
	long		dx, dy;
	int 		di, dj;
	bool		found_wall;
	bool		found_grav;
	bool		near_wall;
	int 		best_val;
	int 		best_i, best_j;
	DFLOAT		best_vx, best_vy;
	int 		best_dir;
	int 		delta_dir;

#if 0
	Move this down below so we can watch the calculations
	if (pl->velocity > 2.0 || ABS(pl->vel.x) > 1.5) {
		Watch(WATCH_NAV, "pl->vel=%f   vel.x=%f\n", pl->velocity, ABS(pl->vel.x) > 1.5);
		return false;
	}
#endif
	for (i = 0; i < 10; i++) {
		for (j = 0; j < 10; j++) {
			area_val[i][j] = 0;
		}
	}
	Watch(WATCH_NAV, "dir= %2d, power=%f\n", pl->dir, pl->power);
	found_wall = false;
	found_grav = false;

	for (i = 0; i < 10; i += 2) {
		for (j = 0; j < 10; j += 2) {

			dx = OBJ_X_IN_BLOCKS(pl) + (i / 2) - 2;
			dy = OBJ_Y_IN_BLOCKS(pl) + (j / 2) - 2;

			if (BIT(w->rules->mode, WRAP_PLAY)) {
				if (dx < 0) dx += w->blockWidth;
				else if (dx >= w->blockWidth) dx -= w->blockWidth;
				if (dy < 0) dy += w->blockHeight;
				else if (dy >= w->blockHeight) dy -= w->blockHeight;
			}
			if (dx < 0 || dx >= w->blockWidth || dy < 0 || dy >= w->blockHeight)
				locn_block = FILLED;
			else
				locn_block = w->block[dx][dy];

			switch (locn_block) {

			case SPACE:
			case BASE:
			case CHECK:
			case ITEM_CONCENTRATOR:
				area_val[i][j] = 1;
				area_val[i + 1][j] = 1;
				area_val[i + 1][j + 1] = 1;
				area_val[i][j + 1] = 1;
				break;

			case WORMHOLE:
				k = w->itemID[dx][dy];
				if (w->wormHoles[k].type == WORM_OUT
					|| !w->options.wormholeVisible->GetBool()) {
					area_val[i][j] = 1;
					area_val[i + 1][j] = 1;
					area_val[i + 1][j + 1] = 1;
					area_val[i][j + 1] = 1;
				} else {
					found_wall = true;
				}
				break;

			case REC_LU:
				area_val[i + 1][j] = 1;
				found_wall = true;
				break;

			case REC_LD:
				area_val[i + 1][j + 1] = 1;
				found_wall = true;
				break;

			case REC_RU:
				area_val[i][j] = 1;
				found_wall = true;
				break;

			case REC_RD:
				area_val[i][j + 1] = 1;
				found_wall = true;
				break;

			case POS_GRAV:
			case NEG_GRAV:
			case CWISE_GRAV:
			case ACWISE_GRAV:
			case UP_GRAV:
			case DOWN_GRAV:
			case RIGHT_GRAV:
			case LEFT_GRAV:
				found_grav = true;
				break;

			case TARGET:
				k = w->itemID[dx][dy];
				if (!w->options.targetTeamCollision->GetBool()
					&& BIT(w->rules->mode, TEAM_PLAY)
					&& w->targets[k].team == pl->team) {
					area_val[i][j] = 1;
					area_val[i + 1][j] = 1;
					area_val[i + 1][j + 1] = 1;
					area_val[i][j + 1] = 1;
				} else {
					found_wall = true;
				}
				break;

			case CANNON:
				k = w->itemID[dx][dy];
				if (BIT(w->rules->mode, TEAM_PLAY)
					&& w->options.teamImmunity->GetBool()
					&& w->cannons[k].team == pl->team) {
					area_val[i][j] = 1;
					area_val[i + 1][j] = 1;
					area_val[i + 1][j + 1] = 1;
					area_val[i][j + 1] = 1;
				} else {
					found_wall = true;
				}
				break;

			default:
				found_wall = true;
				break;
			}
		}
	}

	if (this->IsWatch(WATCH_NAV)) {
		String	s;
		char z[32];
		for (j=9; j>=0; j--) {
			sprintf(z, "a%d:", j);
			s = z;
			for (i=0; i<10; i++) {
				sprintf(z, "  %c ", area_val[i][j] == 0 ? 'X' : ' ');
				s += z;
			}
			//Watch(WATCH_NAV, s);
		}
	}
	if (found_grav || !found_wall)
		return false;

	/* iterate twice for weighting, central 6x6 square should be accurate */

	for (k = 0; k < 2; k++) {
		for (i = 0; i < 10; i++) {
			for (j = 0; j < 10; j++) {

				calc_val[i][j] = 0;
				if (area_val[i][j] == 0)
					continue;

				if (i <= 0 || i >= 9 || j <= 0 || j >= 9)
					continue;

				calc_val[i][j] += 2 * area_val[i - 1][j];
				calc_val[i][j] += 2 * area_val[i][j + 1];
				calc_val[i][j] += 2 * area_val[i + 1][j];
				calc_val[i][j] += 2 * area_val[i][j - 1];

				calc_val[i][j] += area_val[i - 1][j - 1];
				calc_val[i][j] += area_val[i - 1][j + 1];
				calc_val[i][j] += area_val[i + 1][j - 1];
				calc_val[i][j] += area_val[i + 1][j + 1];
			}
		}

		for (i = 0; i < 10; i++) {
			for (j = 0; j < 10; j++) {
				area_val[i][j] = calc_val[i][j];
			}
		}
	}

	if (this->IsWatch(WATCH_NAV)) {
		String	s;
		char z[32];
		for (j=9; j>=0; j--) {
			sprintf(z, "c%d:", j);
			s = z;
			for (i=0; i<10; i++) {
				sprintf(z, "%3d ", calc_val[i][j]);
				s += z;
			}
			//s += "\n";
			//Trace("s=\"%s\"\n", (PCSTR)s);
			Watch(WATCH_NAV, s);
		}
	}
	/* now focus in to local 3x3 square */

	dx = (int)pl->pos.x;
	dy = (int)pl->pos.y;

	dx = dx - (dx / BLOCK_SZ * BLOCK_SZ);
	dy = dy - (dy / BLOCK_SZ * BLOCK_SZ);

	di = 3;
	dj = 3;

	if (dx > BLOCK_SZ / 2) {
		di++;
		dx -= BLOCK_SZ / 2;
	}
	if (dy > BLOCK_SZ / 2) {
		dj++;
		dy -= BLOCK_SZ / 2;
	}
	for (i = 0; i < 3; i++) {
		for (j = 0; j < 3; j++) {
			area_val[i][j] = area_val[di + i][dj + j];
		}
	}
	if (this->IsWatch(WATCH_NAV)) {
		String	s;
		char z[32];
		for (j=2; j>=0; j--) {
			sprintf(z, "f%d:", j);
			s = z;
			for (i=0; i<3; i++) {
				sprintf(z, "%3d ", area_val[i][j]);
				s += z;
			}
			Watch(WATCH_NAV, s);
		}
	}
	if (pl->velocity > 2.0 || ABS(pl->vel.x) > 1.5) {
		Watch(WATCH_NAV, "pl->vel=%f   vel.x=%f\n", pl->velocity, ABS(pl->vel.x) > 1.5);
		return false;
	}

	*isEvade = true;

	Watch(WATCH_NAV, "pl->vel=%f   vel.x,y=%.6f, %.6f", pl->velocity, pl->vel.x, pl->vel.y);
	if (ABS(pl->vel.x) < 0.5) {

		best_i = 1;
		best_j = (pl->vel.y > 0 ? 2 : 0);

	} else if (ABS(pl->vel.y) < 0.5) {

		best_i = (pl->vel.x > 0 ? 2 : 0);
		best_j = 1;

	} else {

		best_i = (pl->vel.x > 0 ? 2 : 0);
		best_j = (pl->vel.y > 0 ? 2 : 0);
	}

	best_val = area_val[best_i][best_j];
	near_wall = false;

	Watch(WATCH_NAV, "best i,j= %d,%d val=%d", best_i, best_j, best_val);
	for (j = 2; j >= 0; j--) {
		for (i = 0; i <= 2; i++) {

			if (i == 1 && j == 1)
				continue;

			if (area_val[i][j] == 0) {
				near_wall = true;
				if (i == 1 && (j == 0 || j == 2)) {
					best_i = 1;
					best_j = (2 - j);
					best_val = 99999;
				}
				continue;
			}
			if (area_val[i][j] > best_val) {
				best_i = i;
				best_j = j;
				best_val = area_val[i][j];
			}
		}
	}
	Watch(WATCH_NAV, "best i,j= %d,%d val=%5d [1][1]=%d", best_i, best_j, best_val, area_val[1][1]);

	if (area_val[1][1] >= best_val)
		return false;

	if (!near_wall) {
		if (BIT(pl->used, HAS_REFUEL)
			&& BIT(longterm_mode, NEED_FUEL)) {
			/* refueling, so hang around */
			best_i = 1;
			best_j = 1;
			best_val = area_val[1][1];
		} else {
			return false;
		}
	}
	if (best_j == 1) {
		if (dy < BLOCK_SZ / 6)
			best_j = 2;
		if (dy > BLOCK_SZ / 3)
			best_j = 0;
	}
	pl->turnspeed = MAX_PLAYER_TURNSPEED;
	pl->power = pl->mass / 2;
	LIMIT(pl->power, MIN_PLAYER_POWER, MAX_PLAYER_POWER);

//	best_vx = (best_j - 1) * 0.75;
//	best_vy = (best_i - 1) * 1.25;
	best_vx = (best_j - 1) * 1.05;
	best_vy = (best_i - 1) * 0.95;

	if (pl->vel.x > best_vx + 0.75)
		best_dir = 3 * RES / 8;
	else if (pl->vel.x < best_vx - 0.75)
		best_dir = RES / 8;
	else if (pl->vel.x > best_vx + 0.25)
		best_dir = 5 * RES / 16;
	else if (pl->vel.x < best_vx - 0.25)
		best_dir = 3 * RES / 16;
	else
		best_dir = RES / 4;

	static int dirtable[3][3] = {	{RES*5/8, RES*6/8, RES*7/8},
									{RES*4/8,       0, RES*0/8},
									{RES*3/8, RES*2/8, RES*1/8}};
	best_dir = dirtable[best_j][best_i];
	delta_dir = best_dir - pl->dir;
	delta_dir = MOD2(delta_dir, RES);
	Watch(WATCH_NAV, "Best vx,y=%f,%f", best_vx, best_vy);
	Watch(WATCH_NAV, "Best dir=%d, delta_dir=%d", best_dir, delta_dir);

	if (delta_dir > RES / 8 && delta_dir < 7 * RES / 8) {
		pl->turnacc = (delta_dir < RES / 2 ?
					   pl->turnspeed : (-pl->turnspeed));
	} else if (delta_dir > RES / 64 && delta_dir < 63 * RES / 64) {
		pl->turnspeed = MIN_PLAYER_TURNSPEED;
		pl->turnacc = (delta_dir < RES / 2 ?
					   pl->turnspeed : (-pl->turnspeed));
	} else {
		pl->turnacc = 0;
	}

//	if (lastDeltaDir == delta_dir)			// Are we stuck?
//		CLR_BIT(pl->status, THRUSTING);		// try not thrusting into the turn
//	else
		SET_BIT(pl->status, THRUSTING);
	lastDeltaDir = delta_dir;

//	if (pl->vel.y > ABS(best_vy) + 0.25) {
//		CLR_BIT(pl->status, THRUSTING);
//	} else if (pl->vel.y < ABS(best_vy) - 0.25) {
//		SET_BIT(pl->status, THRUSTING);
//	}
	return true;
}

///////////////////////////////////////////////////////////////////////////////
bool Robot5::Really_empty_space(int x, int y) {
	int 		type = w->block[x][y];

	if (EMPTY_SPACE(type))
		return true;
	switch (type) {
	case FILLED:
	case REC_LU:
	case REC_LD:
	case REC_RU:
	case REC_RD:
	case FUEL:
	case TREASURE:
		return false;

	case WORMHOLE:
		if (!w->options.wormholeVisible->GetBool()
			|| w->wormHoles[w->itemID[x][y]].type == WORM_OUT) {
			return true;
		} else {
			return false;
		}

	case TARGET:
		if (!w->options.targetTeamCollision->GetBool()
			&& BIT(w->rules->mode, TEAM_PLAY)
			&& w->targets[w->itemID[x][y]].team == pl->team) {
			return true;
		} else {
			return false;
		}

	case CANNON:
		if (w->options.teamImmunity->GetBool()
			&& BIT(w->rules->mode, TEAM_PLAY)
			&& w->cannons[w->itemID[x][y]].team == pl->team) {
			return true;
		} else {
			return false;
		}

	default:
		break;
	}
	return false;
}

///////////////////////////////////////////////////////////////////////////////
bool Robot5::Check_robot_evade(int mine_i, int ship_i) {
	int 						i;
	Object						*shot;
	Player						*ship;
	long						stop_dist;
	bool						evade;
	bool						left_ok, right_ok;
	int 						safe_width;
	int 						travel_dir;
	int 						delta_dir;
	int 						aux_dir;
	int 						px[3], py[3];
	long						dist;
	vector						*gravity;
	int 						gravity_dir;
	long						dx, dy;
	DFLOAT						velocity;

	safe_width = (defense / 200) * SHIP_SZ;
	/* Prevent overflow. */
	velocity = (pl->velocity <= SPEED_LIMIT) ? pl->velocity : SPEED_LIMIT;
	stop_dist =
		(long)((RES * velocity) / (MAX_PLAYER_TURNSPEED * pl->turnresistance)
		+ (velocity * velocity * pl->mass) / (2 * MAX_PLAYER_POWER)
		+ safe_width);
	/*
	 * Limit the look ahead.  For very high speeds the current code
	 * is ineffective and much too inefficient.
	 */
	if (stop_dist > 10 * BLOCK_SZ) {
		stop_dist = 10 * BLOCK_SZ;
	}
	evade = false;

	if (pl->velocity <= 0.2) {
		vector	*grav = &w->gravity
			[OBJ_X_IN_BLOCKS(pl)][OBJ_Y_IN_BLOCKS(pl)];
		travel_dir = (int)FindDir(grav->x, grav->y);
	} else {
		travel_dir = (int)FindDir(pl->vel.x, pl->vel.y);
	}

	aux_dir = MOD2(travel_dir + RES / 4, RES);
	px[0] = pl->pos.x;			/* ship center x */
	py[0] = pl->pos.y;			/* ship center y */
	px[1] = (int)(px[0] + safe_width * tcos(aux_dir));	/* ship left side x */
	py[1] = (int)(py[0] + safe_width * tsin(aux_dir));	/* ship left side y */
	px[2] = 2 * px[0] - px[1];	/* ship right side x */
	py[2] = 2 * py[0] - py[1];	/* ship right side y */

	left_ok = true;
	right_ok = true;

	for (dist = 0; dist < stop_dist + BLOCK_SZ / 2; dist += BLOCK_SZ / 2) {
		for (i = 0; i < 3; i++) {
			dx = (long)((px[i] + dist * tcos(travel_dir)) / BLOCK_SZ);
			dy = (long)((py[i] + dist * tsin(travel_dir)) / BLOCK_SZ);

			if (BIT(w->rules->mode, WRAP_PLAY)) {
				if (dx < 0) dx += w->blockWidth;
				else if (dx >= w->blockWidth) dx -= w->blockWidth;
				if (dy < 0) dy += w->blockHeight;
				else if (dy >= w->blockHeight) dy -= w->blockHeight;
			}
			if (dx < 0 || dx >= w->blockWidth || dy < 0 || dy >= w->blockHeight) {
				evade = true;
				if (i == 1)
					left_ok = false;
				if (i == 2)
					right_ok = false;
				continue;
			}
			if (!Really_empty_space(dx, dy)) {
				evade = true;
				if (i == 1)
					left_ok = false;
				if (i == 2)
					right_ok = false;
				continue;
			}
			/* Watch out for strong gravity */
			gravity = &w->gravity[dx][dy];
			if (sqr(gravity->x) + sqr(gravity->y) >= 0.5) {
				gravity_dir = (int)FindDir(gravity->x - pl->pos.x,
									  gravity->y - pl->pos.y);
				if (MOD2(gravity_dir - travel_dir, RES) <= RES / 4 ||
					MOD2(gravity_dir - travel_dir, RES) >= 3 * RES / 4) {
					evade = true;
					if (i == 1)
						left_ok = false;
					if (i == 2)
						right_ok = false;
					continue;
				}
			}
		}
	}

	if (mine_i >= 0) {
		shot = w->objs[mine_i];
		aux_dir = (int)w->WrapFindDir(shot->pos.x + shot->vel.x - pl->pos.x,
							   shot->pos.y + shot->vel.y - pl->pos.y);
		delta_dir = MOD2(aux_dir - travel_dir, RES);
		if (delta_dir < RES / 4) {
			left_ok = false;
			evade = true;
		}
		if (delta_dir > RES * 3 / 4) {
			right_ok = false;
			evade = true;
		}
	}
	if (ship_i >= 0) {
		ship = w->players[ship_i];
		aux_dir = (int)w->WrapFindDir(ship->pos.x - pl->pos.x + ship->vel.x * 2,
							   ship->pos.y - pl->pos.y + ship->vel.y * 2);
		delta_dir = MOD2(aux_dir - travel_dir, RES);
		if (delta_dir < RES / 4) {
			left_ok = false;
			evade = true;
		}
		if (delta_dir > RES * 3 / 4) {
			right_ok = false;
			evade = true;
		}
	}
	if (pl->velocity > robot_max_speed)
		evade = true;

	if (!evade)
		return false;

	delta_dir = 0;
	while (!left_ok && !right_ok && delta_dir < 7 * RES / 8) {
		delta_dir += RES / 16;

		left_ok = true;
		aux_dir = MOD2(travel_dir + delta_dir, RES);
		for (dist = 0; dist < stop_dist + BLOCK_SZ / 2; dist += BLOCK_SZ / 2) {
			dx = (long)((px[0] + dist * tcos(aux_dir)) / BLOCK_SZ);
			dy = (long)((py[0] + dist * tsin(aux_dir)) / BLOCK_SZ);

			if (BIT(w->rules->mode, WRAP_PLAY)) {
				if (dx < 0) dx += w->blockWidth;
				else if (dx >= w->blockWidth) dx -= w->blockWidth;
				if (dy < 0) dy += w->blockHeight;
				else if (dy >= w->blockHeight) dy -= w->blockHeight;
			}
			if (dx < 0 || dx >= w->blockWidth || dy < 0 || dy >= w->blockHeight) {
				left_ok = false;
				continue;
			}
			if (!Really_empty_space(dx, dy)) {
				left_ok = false;
				continue;
			}
			/* watch out for strong gravity */
			gravity = &w->gravity[dx][dy];
			if (sqr(gravity->x) + sqr(gravity->y) >= 0.5) {
				gravity_dir = (int)FindDir(gravity->x - pl->pos.x,
									  gravity->y - pl->pos.y);
				if (MOD2(gravity_dir - travel_dir, RES) <= RES / 4 ||
					MOD2(gravity_dir - travel_dir, RES) >= 3 * RES / 4) {

					left_ok = false;
					continue;
				}
			}
		}

		right_ok = true;
		aux_dir = MOD2(travel_dir - delta_dir, RES);
		for (dist = 0; dist < stop_dist + BLOCK_SZ / 2; dist += BLOCK_SZ / 2) {
			dx = (long)((px[0] + dist * tcos(aux_dir)) / BLOCK_SZ);
			dy = (long)((py[0] + dist * tsin(aux_dir)) / BLOCK_SZ);

			if (BIT(w->rules->mode, WRAP_PLAY)) {
				if (dx < 0) dx += w->blockWidth;
				else if (dx >= w->blockWidth) dx -= w->blockWidth;
				if (dy < 0) dy += w->blockHeight;
				else if (dy >= w->blockHeight) dy -= w->blockHeight;
			}
			if (dx < 0 || dx >= w->blockWidth || dy < 0 || dy >= w->blockHeight) {
				right_ok = false;
				continue;
			}
			if (!Really_empty_space(dx, dy)) {
				right_ok = false;
				continue;
			}
			/* watch out for strong gravity */
			gravity = &w->gravity[dx][dy];
			if (sqr(gravity->x) + sqr(gravity->y) >= 0.5) {
				gravity_dir = (int)FindDir(gravity->x - pl->pos.x,
									  gravity->y - pl->pos.y);
				if (MOD2(gravity_dir - travel_dir, RES) <= RES / 4 ||
					MOD2(gravity_dir - travel_dir, RES) >= 3 * RES / 4) {

					right_ok = false;
					continue;
				}
			}
		}
	}

	pl->turnspeed = MAX_PLAYER_TURNSPEED;
	pl->power = MAX_PLAYER_POWER;

	delta_dir = MOD2(pl->dir - travel_dir, RES);

	if (robot_mode != RM5_EVADE_LEFT
		&& robot_mode != RM5_EVADE_RIGHT) {
		if (left_ok && !right_ok)
			robot_mode = RM5_EVADE_LEFT;
		else if (right_ok && !left_ok)
			robot_mode = RM5_EVADE_RIGHT;
		else
			robot_mode = (delta_dir < RES / 2 ?
							  RM5_EVADE_LEFT : RM5_EVADE_RIGHT);
	}
	/*-BA If facing the way we want to go, thrust
	 *-BA If too far off, stop thrusting
	 *-BA If in between, keep doing whatever we are already doing
	 *-BA In all cases continue to straighten up
	 */
	if (delta_dir < RES / 4 || delta_dir > 3 * RES / 4) {
		pl->turnacc = (robot_mode == RM5_EVADE_LEFT ?
					   pl->turnspeed : (-pl->turnspeed));
		CLR_BIT(pl->status, THRUSTING);
	}
	else if (delta_dir < 3 * RES / 8 || delta_dir > 5 * RES / 8) {
		pl->turnacc = (robot_mode == RM5_EVADE_LEFT ?
					   pl->turnspeed : (-pl->turnspeed));
	} else {
		pl->turnacc = 0;
		SET_BIT(pl->status, THRUSTING);
		robot_mode = (delta_dir < RES/2 ? RM5_EVADE_LEFT : RM5_EVADE_RIGHT);
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////
static void Robot_check_new_modifiers(Player *pl, modifiers mods) {
	World*	w = pl->world;
	if (!BIT(w->rules->mode, ALLOW_NUKES))
		mods.nuclear = 0;
	if (!BIT(w->rules->mode, ALLOW_CLUSTERS))
		CLR_BIT(mods.warhead, CLUSTER);
	if (!BIT(w->rules->mode, ALLOW_MODIFIERS)) {
		mods.velocity =
		mods.mini =
		mods.spread =
		mods.power = 0;
		CLR_BIT(mods.warhead, IMPLOSION);
	}
	if (!BIT(w->rules->mode, ALLOW_LASER_MODS))
		mods.laser = 0;
	pl->mods = mods;
}

///////////////////////////////////////////////////////////////////////////////
void Robot5::Choose_weapon_modifier(int weapon_type) {
	int 				stock, min;
	modifiers			mods;
	World*				w = pl->world;

	CLEAR_MODS(mods);

	switch (weapon_type) {
	case HAS_TRACTOR_BEAM:
		Robot_check_new_modifiers(pl, mods);
		return;

	case HAS_LASER:
		/*
		 * Robots choose non-damage laser settings occasionally.
		 */
		if ((robot_count % 4) == 0)
			mods.laser = (int)(rfrac() * (MODS_LASER_MAX + 1));
		Robot_check_new_modifiers(pl, mods);
		return;

	case OBJ_SHOT:
		/*
		 * Robots usually use wide beam shots, however they may narrow
		 * the beam occasionally.
		 */
		mods.spread = 0;
		if ((robot_count % 4) == 0)
			mods.spread = (int)(rfrac() * (MODS_SPREAD_MAX + 1));
		Robot_check_new_modifiers(pl, mods);
		return;

	case OBJ_MINE:
		stock = pl->item[ITEM_MINE];
		min = w->options.nukeMinMines->GetInt();
		break;

	case OBJ_SMART_SHOT:
	case OBJ_HEAT_SHOT:
	case OBJ_TORPEDO:
		stock = pl->item[ITEM_MISSILE];
		min = w->options.nukeMinSmarts->GetInt();
		if ((robot_count % 4) == 0)
			mods.power = (int)(rfrac() * (MODS_POWER_MAX + 1));
		break;

	default:
		return;
	}

	if (stock >= min) {
		/*
		 * More aggressive robots will choose to use nuclear weapons, this
		 * means you can safely approach wimpy robots... perhaps.
		 */
		if ((robot_count % 100) <= attack) {
			SET_BIT(mods.nuclear, NUCLEAR);
			if (stock > min && (stock < (2 * min)
								|| (robot_count % 2) == 0))
					SET_BIT(mods.nuclear, FULLNUCLEAR);
		}
	}

	if (pl->fuel.sum > pl->fuel.l3) {
		if ((robot_count % 8) != 0) {
			if ((robot_count % 8) == 0)
				mods.velocity = (int)(rfrac() * MODS_VELOCITY_MAX) + 1;
			SET_BIT(mods.warhead, CLUSTER);
		}
	}
	else if ((robot_count % 3) == 0) {
		SET_BIT(mods.warhead, IMPLOSION);
	}

	/*
	 * Robot may change to use mini device setting occasionally.
	 */
	if ((robot_count % 10) == 0) {
		mods.mini = (int)(rfrac() * (MODS_MINI_MAX + 1));
		mods.spread = (int)(rfrac() * (MODS_SPREAD_MAX + 1));
	}

	Robot_check_new_modifiers(pl, mods);
}

///////////////////////////////////////////////////////////////////////////////
bool Robot5::Check_robot_target(int item_x, int item_y, int new_mode) {
	Player*		ship;
	long		item_dist;
	int 		item_dir;
	int 		travel_dir;
	int 		delta_dir;
	long		dx, dy;
	long		dist;
	bool		clear_path;
	bool		slowing;


	dx = item_x - pl->pos.x, dx = WRAP_DX(w, dx);
	dy = item_y - pl->pos.y, dy = WRAP_DY(w, dy);

	item_dist = (long)(LENGTH(dy, dx));

	if (dx == 0 && dy == 0) {
		vector	*grav = &w->gravity
			[OBJ_X_IN_BLOCKS(pl)][OBJ_Y_IN_BLOCKS(pl)];
		item_dir = (int)FindDir(grav->x, grav->y);
		item_dir = MOD2(item_dir + RES/2, RES);
	} else {
		item_dir = (int)FindDir(dx, dy);
	}

	if (new_mode == RM5_REFUEL)
		item_dist = item_dist - 90;

	clear_path = true;

	for (dist = 0; clear_path && dist < item_dist; dist += BLOCK_SZ / 2) {

		dx = (long)((pl->pos.x + dist * tcos(item_dir)) / BLOCK_SZ);
		dy = (long)((pl->pos.y + dist * tsin(item_dir)) / BLOCK_SZ);

		if (BIT(w->rules->mode, WRAP_PLAY)) {
			if (dx < 0) dx += w->blockWidth;
			else if (dx >= w->blockWidth) dx -= w->blockWidth;
			if (dy < 0) dy += w->blockHeight;
			else if (dy >= w->blockHeight) dy -= w->blockHeight;
		}
		if (dx < 0 || dx >= w->blockWidth || dy < 0 || dy >= w->blockHeight) {
			clear_path = false;
			continue;
		}
		if (!Really_empty_space(dx, dy)) {
			clear_path = false;
			continue;
		}
	}

	if (new_mode == RM5_CANNON_KILL)
		item_dist -= 4 * BLOCK_SZ;

	if (!clear_path && new_mode != RM5_NAVIGATE)
		return false;

	if (pl->velocity <= 0.2) {
		vector	*grav = &w->gravity
			[OBJ_X_IN_BLOCKS(pl)][OBJ_Y_IN_BLOCKS(pl)];
		travel_dir = (int)FindDir(grav->x, grav->y);
	} else {
		travel_dir = (int)FindDir(pl->vel.x, pl->vel.y);
	}

	pl->turnspeed = MAX_PLAYER_TURNSPEED / 2;
	pl->power = (BIT(w->rules->mode, TIMING) ?
				 MAX_PLAYER_POWER :
				 MAX_PLAYER_POWER / 2);

	delta_dir = MOD2(item_dir - travel_dir, RES);
	if (delta_dir >= RES/4 && delta_dir <= 3*RES/4) {

		if (new_mode == RM5_HARVEST ||
			(new_mode == RM5_NAVIGATE &&
				(clear_path || dist > 8 * BLOCK_SZ))) {
			/* reverse direction of travel */
			item_dir = MOD2(travel_dir + (delta_dir > RES / 2
											? -5 * RES / 8
											: 5 * RES / 8),
							RES);
		}
		pl->turnspeed = MAX_PLAYER_TURNSPEED;
		slowing = true;

		if (pl->item[ITEM_MINE] && item_dist < 8 * BLOCK_SZ) {
			Choose_weapon_modifier(OBJ_MINE);
			if (BIT(w->rules->mode, TIMING)) {
				pl->PlaceMine();
			} else {
				pl->PlaceMovingMine();
			}
			new_mode = (rfrac() < 0.5f) ? RM5_EVADE_RIGHT : RM5_EVADE_LEFT;
		}
	} else if (new_mode == RM5_CANNON_KILL && item_dist <= 0) {

		/* too close, so move away */
		pl->turnspeed = MAX_PLAYER_TURNSPEED;
		item_dir = MOD2(item_dir + RES / 2, RES);
		slowing = true;
	} else {

		slowing = false;
	}
	if (new_mode == RM5_NAVIGATE && !clear_path) {
		if (dist <= 8 * BLOCK_SZ && dist > 4 * BLOCK_SZ) {
			item_dir = MOD2(item_dir + (delta_dir > RES / 2 ? -3 * RES / 4 : 3 * RES / 4), RES);
		} else if (dist <= 4 * BLOCK_SZ) {
			item_dir = MOD2(item_dir + RES / 2, RES);
		}
		pl->turnspeed = MAX_PLAYER_TURNSPEED;
		slowing = true;
	}

	delta_dir = MOD2(item_dir - pl->dir, RES);

	if (delta_dir > RES / 8 && delta_dir < 7 * RES / 8) {
		pl->turnspeed = MAX_PLAYER_TURNSPEED;
	} else if (delta_dir > RES / 16 && delta_dir < 15 * RES / 16) {
		pl->turnspeed = MAX_PLAYER_TURNSPEED;
	} else if (delta_dir > RES / 64 && delta_dir < 63 * RES / 64) {
		pl->turnspeed = MAX_PLAYER_TURNSPEED;
	} else {
		pl->turnspeed = 0.0;
	}
	pl->turnacc = (delta_dir < RES / 2 ? pl->turnspeed : (-pl->turnspeed));

	if (slowing || BIT(pl->used, HAS_SHIELD)) {

		SET_BIT(pl->status, THRUSTING);

	} else if (item_dist < 0) {

		CLR_BIT(pl->status, THRUSTING);

	} else if (item_dist < 3*BLOCK_SZ && new_mode != RM5_HARVEST) {

		if (pl->velocity < robot_normal_speed / 2)
			SET_BIT(pl->status, THRUSTING);
		if (pl->velocity > robot_normal_speed)
			CLR_BIT(pl->status, THRUSTING);

	} else if ((new_mode != RM5_ATTACK
				&& new_mode != RM5_NAVIGATE)
			|| item_dist < 8*BLOCK_SZ
			|| (new_mode == RM5_NAVIGATE
				&& delta_dir > 3 * RES / 8
				&& delta_dir < 5 * RES / 8)) {

		if (pl->velocity < 2*robot_normal_speed)
			SET_BIT(pl->status, THRUSTING);
		if (pl->velocity > 3*robot_normal_speed)
			CLR_BIT(pl->status, THRUSTING);

	} else if (new_mode == RM5_ATTACK
			|| (new_mode == RM5_NAVIGATE
				&& (dist < 12 * BLOCK_SZ
					|| (delta_dir > RES / 8
						&& delta_dir < 7 * RES / 8)))) {

		if (pl->velocity < robot_attack_speed / 2)
			SET_BIT(pl->status, THRUSTING);
		if (pl->velocity > robot_attack_speed)
			CLR_BIT(pl->status, THRUSTING);
	} else if (clear_path
			&& (delta_dir < RES / 8
				|| delta_dir > 7 * RES / 8)
			&& item_dist > 18 * BLOCK_SZ) {
		if (pl->velocity < robot_max_speed - robot_normal_speed)
			SET_BIT(pl->status, THRUSTING);
		if (pl->velocity > robot_max_speed)
			CLR_BIT(pl->status, THRUSTING);
	} else {
		if (pl->velocity < robot_attack_speed)
			SET_BIT(pl->status, THRUSTING);
		if (pl->velocity > robot_max_speed - robot_normal_speed)
			CLR_BIT(pl->status, THRUSTING);
	}

	if (new_mode == RM5_ATTACK
		|| (BIT(w->rules->mode, TIMING)
			&& new_mode == RM5_NAVIGATE)) {
		if (pl->item[ITEM_ECM] > 0
			&& item_dist < ECM_DISTANCE / 4) {
			pl->FireEcm();
		}
		else if (pl->item[ITEM_TRANSPORTER] > 0
				 && item_dist < TRANSPORTER_DISTANCE
				 && pl->fuel.sum > -ED_TRANSPORTER) {
			w->DoTransporter(pl);
		}
		else if (pl->item[ITEM_LASER] > pl->num_pulses
				 && pl->fuel.sum + ED_LASER > pl->fuel.l3
				 && new_mode == RM5_ATTACK) {
			if (BIT(robot_lock, LOCK_PLAYER)
				&& BIT(w->players[w->getInd[robot_lock_id]]->status,
					   PLAYING|PAUSE|GAME_OVER) == PLAYING) {
				ship = w->players[w->getInd[robot_lock_id]];
			}
			else if (BIT(pl->lock.tagged, LOCK_PLAYER)) {
				ship = w->players[w->getInd[pl->lock.pl_id]];
			}
			else {
				ship = NULL;
			}
			if (ship
				&& BIT(ship->status, PLAYING|PAUSE|GAME_OVER) == PLAYING) {

				DFLOAT	x1, y1, x3, y3, x4, y4, x5, y5;
				DFLOAT	ship_dist, dir3, dir4, dir5;

				x1 = pl->pos.x + pl->vel.x + pl->ship->m_gun[pl->dir].x;
				y1 = pl->pos.y + pl->vel.y + pl->ship->m_gun[pl->dir].y;
				x3 = ship->pos.x + ship->vel.x;
				y3 = ship->pos.y + ship->vel.y;

				ship_dist = w->WrapLength(x3 - x1, y3 - y1);

				if (ship_dist < PULSE_SPEED*PULSE_LIFE(pl->item[ITEM_LASER]) + SHIP_SZ) {
					dir3 = w->WrapFindDir(x3 - x1, y3 - y1);
					x4 = x3 + tcos(MOD2((int)(dir3 - RES/4), RES)) * SHIP_SZ;
					y4 = y3 + tsin(MOD2((int)(dir3 - RES/4), RES)) * SHIP_SZ;
					x5 = x3 + tcos(MOD2((int)(dir3 + RES/4), RES)) * SHIP_SZ;
					y5 = y3 + tsin(MOD2((int)(dir3 + RES/4), RES)) * SHIP_SZ;
					dir4 = w->WrapFindDir(x4 - x1, y4 - y1);
					dir5 = w->WrapFindDir(x5 - x1, y5 - y1);
					if ((dir4 > dir5)
						? (pl->dir >= dir4 || pl->dir <= dir5)
						: (pl->dir >= dir4 && pl->dir <= dir5)) {
						SET_BIT(pl->used, HAS_LASER);
					}
				}
			}
		}
		else if (BIT(pl->have, HAS_TRACTOR_BEAM)) {
			CLR_BIT(pl->used, HAS_TRACTOR_BEAM);
			pl->tractor_is_pressor = false;

			if (BIT(pl->lock.tagged, LOCK_PLAYER)
				&& pl->fuel.sum > pl->fuel.l3
				&& pl->lock.distance
				   < TRACTOR_MAX_RANGE(pl->item[ITEM_TRACTOR_BEAM])) {

				DFLOAT xvd, yvd, vel;
				long dir;
				int away;

				ship = w->players[w->getInd[pl->lock.pl_id]];
				xvd = ship->vel.x - pl->vel.x;
				yvd = ship->vel.y - pl->vel.y;
				vel = LENGTH(xvd, yvd);
				dir = (long)(FindDir(pl->pos.x - ship->pos.x,
									 pl->pos.y - ship->pos.y)
							 - FindDir(xvd, yvd));
				dir = MOD2(dir, RES);
				away = (dir >= RES/4 && dir <= 3*RES/4);

				/*
				 * vel	- The relative velocity of ship to us.
				 * away - Heading away from us?
				 */
				if (pl->velocity <= robot_normal_speed) {
					if (pl->lock.distance < (SHIP_SZ * 4)
						|| (!away && vel > robot_attack_speed)) {
						SET_BIT(pl->used, HAS_TRACTOR_BEAM);
						pl->tractor_is_pressor = true;
					} else if (away
							   && vel < robot_max_speed
							   && vel > robot_normal_speed) {
						SET_BIT(pl->used, HAS_TRACTOR_BEAM);
					}
				}
				if (BIT(pl->used, HAS_TRACTOR_BEAM))
					SET_BIT(pl->lock.tagged, LOCK_VISIBLE);
			}
		}
		if (BIT(pl->used, HAS_LASER)) {
			pl->turnacc = 0.0;
			Choose_weapon_modifier(HAS_LASER);
		}
		/*-BA Be more agressive, esp if lots of ammo
		 * else if ((robot_count % 10) == 0 && pl->item[ITEM_MISSILE] > 0)
		 */
		else if ((robot_count % 10) < pl->item[ITEM_MISSILE]
				  && !WITHIN(robot_count,last_fired_missile,10)) {
			int type;

			switch (robot_count % 5) {
			case 0: case 1: case 2: 	type = OBJ_SMART_SHOT; break;
			case 3: 					type = OBJ_HEAT_SHOT; break;
			default:					type = OBJ_TORPEDO; break;
			}
			if (Detect_hunt(w->getInd[pl->lock.pl_id])
				&& !pl->visibility[w->getInd[pl->lock.pl_id]].canSee)
				type = OBJ_HEAT_SHOT;
			if (type == OBJ_SMART_SHOT && !w->options.allowSmartMissiles->GetBool())
				type = OBJ_HEAT_SHOT;
			Choose_weapon_modifier(type);
			pl->FireShot(type, pl->dir);
			if (type == OBJ_HEAT_SHOT)
				CLR_BIT(pl->status, THRUSTING);
			last_fired_missile=robot_count;
		}
		else if ((robot_count % 2) == 0
				   && item_dist < visibility_distance
				   /*&& BIT(robot_lock, LOCK_PLAYER)*/){
			if ((int)(rfrac() * 64) < pl->item[ITEM_MISSILE] ) {
				Choose_weapon_modifier(OBJ_SMART_SHOT);
				pl->FireShot(OBJ_SMART_SHOT, pl->dir);
				last_fired_missile=robot_count;
			} else {
				if ((new_mode == RM5_ATTACK && clear_path)
					|| (robot_count % 50) == 0) {
					Choose_weapon_modifier(OBJ_SHOT);
					pl->FireNormalShots();
				}
			}
		}
		/*-BA Be more agressive, esp if lots of ammo
		 * if ((robot_count % 32) == 0)
		 */
		else if ((robot_count % 32) < pl->item[ITEM_MINE]
				  && !WITHIN(robot_count, last_dropped_mine, 10)) {
			if (pl->fuel.sum > pl->fuel.l3) {
				Choose_weapon_modifier(OBJ_MINE);
				pl->PlaceMine();
			} else /*if (pl->fuel.sum < pl->fuel.l2)*/ {
				pl->PlaceMine();
				CLR_BIT(pl->used, HAS_CLOAKING_DEVICE);
			}
			last_dropped_mine=robot_count;
		}
	}
	if (new_mode == RM5_CANNON_KILL && !slowing) {
		if ((robot_count % 2) == 0
			&& item_dist < visibility_distance
			&& clear_path) {
			Choose_weapon_modifier(OBJ_SHOT);
			pl->FireNormalShots();
		}
	}
	robot_mode = new_mode;
	return true;
}

///////////////////////////////////////////////////////////////////////////////
bool Robot5::Check_robot_hunt() {
	Player*		ship;
	int 		ship_dir;
	int 		travel_dir;
	int 		delta_dir;
	int 		adj_dir;
	int 		toofast, tooslow;

	if (!BIT(robot_lock, LOCK_PLAYER)
		|| robot_lock_id == pl->id)
		return false;
	if (pl->fuel.sum < pl->fuel.l3 /*MAX_PLAYER_FUEL/2*/)
		return false;
	if (!Detect_hunt(w->getInd[robot_lock_id]))
		return false;

	ship = w->players[w->getInd[robot_lock_id]];

	ship_dir = (int)w->WrapFindDir(ship->pos.x - pl->pos.x, ship->pos.y - pl->pos.y);

	if (pl->velocity <= 0.2) {
		vector	*grav = &w->gravity
			[OBJ_X_IN_BLOCKS(pl)][OBJ_Y_IN_BLOCKS(pl)];
		travel_dir = (int)FindDir(grav->x, grav->y);
	} else {
		travel_dir = (int)FindDir(pl->vel.x, pl->vel.y);
	}

	delta_dir = MOD2(ship_dir - travel_dir, RES);
	tooslow = (pl->velocity < robot_attack_speed/2);
	toofast = (pl->velocity > robot_attack_speed);

	if (!tooslow && !toofast
		&& (delta_dir <= RES/16 || delta_dir >= 15*RES/16)) {

		pl->turnacc = 0;
		CLR_BIT(pl->status, THRUSTING);
		robot_mode = RM5_ROBOT_IDLE;
		return true;
	}

	adj_dir = (delta_dir<RES/2 ? RES/4 : (-RES/4));

	if (tooslow) adj_dir = adj_dir/2;	/* point forwards more */
	if (toofast) adj_dir = 3*adj_dir/2; /* point backwards more */

	adj_dir = MOD2(travel_dir + adj_dir, RES);
	delta_dir = MOD2(adj_dir - pl->dir, RES);

	if (delta_dir>=RES/16 && delta_dir<=15*RES/16) {
		pl->turnspeed = MAX_PLAYER_TURNSPEED/4;
		pl->turnacc = (delta_dir<RES/2 ? pl->turnspeed : (-pl->turnspeed));
	}

	if (delta_dir<RES/8 || delta_dir>7*RES/8) {
		SET_BIT(pl->status, THRUSTING);
	} else {
		CLR_BIT(pl->status, THRUSTING);
	}

	robot_mode = RM5_ROBOT_IDLE;
	return true;
}

///////////////////////////////////////////////////////////////////////////////
bool Robot5::Detect_hunt(int j) {
	Player* ship = w->players[j];
	int 		dx, dy;

		if (BIT(ship->status, PLAYING|PAUSE|GAME_OVER|KILLED) != PLAYING) {
				return(false);	/* can't go after non-playing ships */
		}
	if (BIT(ship->used, HAS_PHASING_DEVICE))
		return false;			/* can't do anything with phased ships */

	if (pl->visibility[j].canSee)
		return true;			/* trivial */

	/* can't see it, so it must be cloaked
		maybe we can detect it's presence from other clues? */

	dx = ship->pos.x - pl->pos.x, dx = WRAP_DX(w, dx);
	dy = ship->pos.y - pl->pos.y, dy = WRAP_DY(w, dy);
	if (sqr(dx) + sqr(dy) > sqr(visibility_distance))
		return false;			/* can't detect ships beyond visual range */

	if (BIT(ship->status, THRUSTING) && w->options.cloakedExhaust->GetBool())
		return true;

	if (BIT(ship->used, HAS_SHOT|
						HAS_LASER|
						HAS_REFUEL|
						HAS_REPAIR|
						HAS_CONNECTOR|
						HAS_TRACTOR_BEAM))
		return true;

	if (BIT(ship->have, HAS_BALL))
		return true;

	return false;				/* the sky seems clear.. */
}

///////////////////////////////////////////////////////////////////////////////
/*
 * Determine how important an item is to a given player.
 * Return one of the following 3 values:
 */
#define ROBOT_MUST_HAVE_ITEM	2		/* must have */
#define ROBOT_HANDY_ITEM		1		/* handy */
#define ROBOT_IGNORE_ITEM		0		/* ignore */
/*
 */
int Robot5::Rank_item_value(ITEM itemtype)
{
//	Player		*pl = w->players[ind];

	if (itemtype == ITEM_AUTOPILOT)
		return ROBOT_IGNORE_ITEM;				/* never useful for robots */
	if (pl->item[itemtype] >= w->items[itemtype].limit)
		return ROBOT_IGNORE_ITEM;				/* already full */
	if ((IsDefensiveItem(itemtype)
		 && CountDefensiveItems(pl) >= w->options.maxDefensiveItems->GetInt())
		|| (IsOffensiveItem(itemtype)
		 && CountOffensiveItems(pl) >= w->options.maxOffensiveItems->GetInt()))
		return ROBOT_IGNORE_ITEM;
	if (itemtype == ITEM_FUEL) {
		if (pl->fuel.sum >= pl->fuel.max * 0.90) {
			return ROBOT_IGNORE_ITEM;			/* already (almost) full */
		} else if (pl->fuel.sum <
					(BIT(w->rules->mode, TIMING) ?
						pl->fuel.l1 :
						pl->fuel.l2)) {
			return ROBOT_MUST_HAVE_ITEM;				/* ahh fuel at last */
		} else {
			return ROBOT_HANDY_ITEM;
		}
	}
	if (BIT(w->rules->mode, TIMING)) {
		switch (itemtype) {
		case ITEM_AFTERBURNER:	/* the more speed the better */
		case ITEM_EMERGENCY_THRUST: 	/* makes you go really fast */
		case ITEM_TRANSPORTER:	/* steal fuel when you overtake someone */
		case ITEM_REARSHOT: 	/* shoot competitors behind you */
		case ITEM_MINE: 		/* blows others off the track */
		case ITEM_ECM:			/* blinded players smash into walls */
			return ROBOT_MUST_HAVE_ITEM;
		case ITEM_CLOAK:		/* not important in racemode */
		case ITEM_LASER:		/* cost too much fuel */
		case ITEM_MISSILE:		/* likely to hit self */
		case ITEM_SENSOR:		/* who cares about seeing others? */
		case ITEM_TANK: 		/* makes you heavier */
		case ITEM_TRACTOR_BEAM: /* pushes/pulls owner off the track too */
		case ITEM_HYPERJUMP:	/* likely to end up in wrong place */
		case ITEM_PHASING:		/* robots don't know how to use them yet */
		case ITEM_DEFLECTOR:	/* cost too much fuel */
		case ITEM_ARMOR:		/* makes you heavier */
			return ROBOT_IGNORE_ITEM;
		}
	} else {
		switch (itemtype) {
		case ITEM_EMERGENCY_SHIELD:
		case ITEM_DEFLECTOR:
		case ITEM_ARMOR:
			if (BIT(pl->have, HAS_SHIELD)) {
				return ROBOT_HANDY_ITEM;
			} else {
				return ROBOT_MUST_HAVE_ITEM;
			}

		case ITEM_REARSHOT:
		case ITEM_WIDEANGLE:
			if (w->options.maxPlayerShots->GetInt() <= 0
				|| w->options.shotLife->GetInt() <= 0
				|| !w->options.allowPlayerKilling->GetBool()) {
				return ROBOT_HANDY_ITEM;
			} else {
				return ROBOT_MUST_HAVE_ITEM;
			}

		case ITEM_MISSILE:
			if (w->options.maxPlayerShots->GetInt() <= 0
				|| w->options.shotLife->GetInt() <= 0
				|| !w->options.allowPlayerKilling->GetBool()) {
				return ROBOT_IGNORE_ITEM;
			} else {
				return ROBOT_MUST_HAVE_ITEM;
			}

		case ITEM_MINE:
		case ITEM_CLOAK:
			return ROBOT_MUST_HAVE_ITEM;

		case ITEM_LASER:
			if (w->options.allowPlayerKilling->GetBool()) {
				return ROBOT_MUST_HAVE_ITEM;
			} else {
				return ROBOT_HANDY_ITEM;
			}

		case ITEM_PHASING:		/* robots don't know how to use them yet */
			return ROBOT_IGNORE_ITEM;

		default:
			break;
		}
	}
	return ROBOT_HANDY_ITEM;
}

///////////////////////////////////////////////////////////////////////////////
bool Robot5::Ball_handler()
{
	int 		i,
				closest_t = -1,
				closest_nt = -1,
				dist,
				closest_t_dist = INT_MAX,
				closest_nt_dist = INT_MAX,
				bdir,
				tdir;
	bool		clear_path = true;

	for (i = 0; i < w->numTreasures; i++) {
		if ((BIT(pl->have, HAS_BALL) || pl->ball)
			&& w->treasures[i].team == pl->team) {
			dist = (int)w->WrapLength((w->treasures[i].pos.x + 0.5) * BLOCK_SZ
								- pl->pos.x,
							   (w->treasures[i].pos.y + 0.5) * BLOCK_SZ
								- pl->pos.y);
			if (dist < closest_t_dist) {
				closest_t = i;
				closest_t_dist = dist;
			}
		} else if (w->treasures[i].team != pl->team
				   && w->teams[w->treasures[i].team].numMembers > 0
				   && !BIT(pl->have, HAS_BALL)
				   && !pl->ball
				   && w->treasures[i].have) {
			dist = (int)w->WrapLength((w->treasures[i].pos.x + 0.5) * BLOCK_SZ
								- pl->pos.x,
							   (w->treasures[i].pos.y + 0.5) * BLOCK_SZ
								- pl->pos.y);
			if (dist < closest_nt_dist) {
				closest_nt = i;
				closest_nt_dist = dist;
			}
		}
	}
	if (BIT(pl->have, HAS_BALL) || pl->ball) {
		BallObject *ball = NULL;
		int dist_np = INT_MAX;
		int xdist, ydist;
		int dx, dy;
		if (pl->ball) {
			ball = pl->ball;
		} else {
			for (i = 0; i < w->numObjs; i++) {
				if (BIT(w->objs[i]->type, OBJ_BALL) && w->objs[i]->id == pl->id) {
					ball = BALL_PTR(w->objs[i]);
					break;
				}
			}
		}
		for (i = 0; i < w->numPlayers; i++) {
			dist = (int)(LENGTH(ball->pos.x - w->players[i]->pos.x,
						  ball->pos.y - w->players[i]->pos.y));
			if (w->players[i]->id != pl->id
				&& (BIT(w->players[i]->status, PLAYING|PAUSE|GAME_OVER) == PLAYING)
				&& dist < dist_np)
				dist_np = dist;
		}
		bdir = (int)FindDir(ball->vel.x, ball->vel.y);
		tdir = (int)w->WrapFindDir((w->treasures[closest_t].pos.x + 0.5) * BLOCK_SZ
						- ball->pos.x,
						(w->treasures[closest_t].pos.y + 0.5) * BLOCK_SZ
						- ball->pos.y);
		xdist = (w->treasures[closest_t].pos.x)
				- OBJ_X_IN_BLOCKS(ball);
		ydist = (w->treasures[closest_t].pos.y)
				- OBJ_Y_IN_BLOCKS(ball);
		for (dist = 0;
			 clear_path && dist < (closest_t_dist - BLOCK_SZ);
			 dist += BLOCK_SZ / 2) {
			DFLOAT fraction = (DFLOAT)dist / closest_t_dist;
			dx = (int)((fraction * xdist) + OBJ_X_IN_BLOCKS(ball));
			dy = (int)((fraction * ydist) + OBJ_Y_IN_BLOCKS(ball));
			if (BIT(w->rules->mode, WRAP_PLAY)) {
				if (dx < 0)
					dx += w->blockWidth;
				else if (dx >= w->blockWidth)
					dx -= w->blockWidth;
				if (dy < 0)
					dy += w->blockHeight;
				else if (dy >= w->blockHeight)
					dy -= w->blockHeight;
			}
			if (dx < 0 || dx >= w->blockWidth || dy < 0 || dy >= w->blockHeight) {
				clear_path = false;
				continue;
			}
			if (!BIT(1U << w->block[dx][dy], SPACE_BLOCKS)) {
				clear_path = false;
				continue;
			}
		}
		if (tdir == bdir
			&& dist_np > closest_t_dist
			&& clear_path
			&& sqr(ball->vel.x) + sqr(ball->vel.y) > 60) {
			pl->DetachBall(-1);
			CLR_BIT(pl->used, HAS_CONNECTOR);
			last_thrown_ball = robot_count;
			CLR_BIT(longterm_mode, FETCH_TREASURE);
		} else {
			SET_BIT(longterm_mode, FETCH_TREASURE);
			return (Check_robot_target(
						((int)(w->treasures[closest_t].pos.x + 0.5) * BLOCK_SZ),
						((int)(w->treasures[closest_t].pos.y + 0.5) * BLOCK_SZ),
						RM5_NAVIGATE));
		}
	} else {
		int 	ball_dist;
		int 	closest_ball_dist = closest_nt_dist;
		int 	closest_ball = -1;

		for (i = 0; i < w->numObjs; i++) {
			if (w->objs[i]->type == OBJ_BALL) {
				BallObject *ball = BALL_IND(w, i);
				if ((ball->id == NO_ID)
					? (ball->owner != NO_ID)
					: (w->players[w->getInd[ball->id]]->team != pl->team)) {
					ball_dist = (int)LENGTH(pl->pos.x - ball->pos.x,
											pl->pos.y - ball->pos.y);
					if (ball_dist < closest_ball_dist) {
						closest_ball_dist = ball_dist;
						closest_ball = i;
					}
				}
			}
		}
		if (closest_ball == -1
			&& closest_nt_dist < (robot_count / 10) * BLOCK_SZ) {
			SET_BIT(longterm_mode, FETCH_TREASURE);
			return (Check_robot_target(
						((int)(w->treasures[closest_nt].pos.x + 0.5) * BLOCK_SZ),
						((int)(w->treasures[closest_nt].pos.y + 0.5) * BLOCK_SZ),
						RM5_NAVIGATE));
		} else if (closest_ball_dist < (robot_count / 10) * BLOCK_SZ
				   && closest_ball_dist > w->options.ballConnectorLength->GetDouble()) {
			SET_BIT(longterm_mode, FETCH_TREASURE);
			return (Check_robot_target(w->objs[closest_ball]->pos.x,
									   w->objs[closest_ball]->pos.y,
									   RM5_NAVIGATE));
		}
	}
	return false;
}

///////////////////////////////////////////////////////////////////////////////
int Robot5::Robot_default_play_check_map() {
	int 						j;
	int 						cannon_i, fuel_i, target_i;
	int 						dx, dy;
	int 						distance, cannon_dist, fuel_dist, target_dist;
	bool						fuel_checked;

	fuel_checked = false;

	cannon_i = -1;
	cannon_dist = (int)visibility_distance;
	fuel_i = -1;
	fuel_dist = (int)visibility_distance;
	target_i = -1;
	target_dist = (int)visibility_distance;

	for (j = 0; j < w->numFuels; j++) {

		if (w->fuels[j].fuel < 100 * FUEL_SCALE_FACT)
			continue;

		if (BIT(w->rules->mode, TEAM_PLAY)
			&& w->options.teamFuel->GetBool()
			&& w->fuels[j].team != pl->team)
			continue;

		if ((dx = (int)(w->fuels[j].pix_pos.x - pl->pos.x),
				dx = WRAP_DX(w, dx), ABS(dx)) < fuel_dist
			&& (dy = (int)(w->fuels[j].pix_pos.y - pl->pos.y),
				dy = WRAP_DY(w, dy), ABS(dy)) < fuel_dist
			&& (distance = (int)LENGTH(dx, dy)) < fuel_dist) {
			if (w->block[w->fuels[j].blk_pos.x]
						   [w->fuels[j].blk_pos.y] == FUEL) {
				fuel_i = j;
				fuel_dist = distance;
			}
		}
	}

	for (j = 0; j < w->numTargets; j++) {

		/* Ignore dead or owned targets */
		if (w->targets[j].dead_time > 0
			|| pl->team == w->targets[j].team
			|| w->teams[w->targets[j].team].numMembers == 0)
			continue;

		if ((dx = w->targets[j].pos.x*BLOCK_SZ + BLOCK_SZ/2 - pl->pos.x,
				dx = WRAP_DX(w, dx), ABS(dx)) < target_dist
			&& (dy = w->targets[j].pos.y*BLOCK_SZ+BLOCK_SZ/2-pl->pos.y,
				dy = WRAP_DY(w, dy), ABS(dy)) < target_dist
			&& (distance = (int)LENGTH(dx, dy)) < target_dist) {
			target_i = j;
			target_dist = distance;
		}
	}

	if (fuel_i >= 0
		&& (target_dist > fuel_dist
			|| !BIT(w->rules->mode, TEAM_PLAY))
		&& BIT(longterm_mode, NEED_FUEL)) {

		fuel_checked = true;
		dx = (int)w->fuels[fuel_i].pix_pos.x;
		dy = (int)w->fuels[fuel_i].pix_pos.y;

		SET_BIT(pl->used, HAS_REFUEL);
		pl->fs = fuel_i;

		if (Check_robot_target(dx, dy, RM5_REFUEL)) {
			return 1;
		}
	}
	if (target_i >= 0) {
		dx = (int)((w->targets[target_i].pos.x + 0.5) * BLOCK_SZ);
		dy = (int)((w->targets[target_i].pos.y + 0.5) * BLOCK_SZ);

		SET_BIT(longterm_mode, TARGET_KILL);
		if (Check_robot_target(dx, dy, RM5_CANNON_KILL)) {
			return 1;
		}
		CLR_BIT(longterm_mode, TARGET_KILL);
	}

	for (j = 0; j < w->numCannons; j++) {

		if (w->cannons[j].dead_time > 0)
			continue;

		if (BIT(w->rules->mode, TEAM_PLAY)
			&& w->cannons[j].team == pl->team)
			continue;

		if ((dx = (int)(w->cannons[j].pix_pos.x - pl->pos.x),
				dx = (int)(WRAP_DX(w, dx), ABS(dx))) < cannon_dist
			&& (dy= (int)(w->cannons[j].pix_pos.y - pl->pos.y),
				dy = WRAP_DY(w, dy), ABS(dy)) < cannon_dist
			&& (distance = (int)LENGTH(dx, dy)) < cannon_dist) {
			cannon_i = j;
			cannon_dist = distance;
		}
	}

	if (cannon_i >= 0) {

		dx = (int)w->cannons[cannon_i].pix_pos.x;
		dx += (int)(BLOCK_SZ * 0.1 * tcos(w->cannons[cannon_i].dir));
		dy = (int)w->cannons[cannon_i].pix_pos.y;
		dy += (int)(BLOCK_SZ * 0.1 * tsin(w->cannons[cannon_i].dir));

		if (Check_robot_target(dx, dy, RM5_CANNON_KILL)) {
			return 1;
		}
	}

	if (fuel_i >= 0
		&& !fuel_checked
		&& BIT(longterm_mode, NEED_FUEL)) {

		dx = (int)w->fuels[fuel_i].pix_pos.x;
		dy = (int)w->fuels[fuel_i].pix_pos.y;

		SET_BIT(pl->used, HAS_REFUEL);
		pl->fs = fuel_i;

		if (Check_robot_target(dx, dy, RM5_REFUEL)) {
			return 1;
		}
	}

	return 0;
}

void Robot5::PlayCheckObjects(int* item_dist,
							  int* itemImportance, Object** item,
							  int* mine_i, int* mine_dist)
{
	Object**	obj_list;
	Object*		shot;
	int 		j;
	int 		distance, obj_count;
	int 		dx, dy;
	int 		shield_range;
	long		killing_shots;

	/*-BA Neural overload - if NumObjs too high, only consider
	 *-BA max_objs many objects - improves performance under nukes
	 *-BA 1000 is a fairly arbitrary choice.  If you wish to tune it,
	 *-BA take into account the following.	A 4 mine cluster nuke produces
	 *-BA about 4000 short lived objects.  An 8 mine cluster nuke produces
	 *-BA about 14000 short lived objects.	By default, there is a limit
	 *-BA of about 16000 objects.  Each player/robot produces between
	 *-BA 20 and 40 objects just thrusting, and up to perhaps another 100
	 *-BA by firing.  If the number is set too low the robots just fly
	 *-BA around with thier shields on looking stupid and not doing
	 *-BA much.  If too high, your system will slow down too much when
	 *-BA the cluster nukes start going off.
	 */
	const int					max_objs = 1000;

	killing_shots = KILLING_SHOTS;
	if (w->options.treasureCollisionMayKill->GetBool()) {
		killing_shots |= OBJ_BALL;
	}
	if (w->options.wreckageCollisionMayKill->GetBool()) {
		killing_shots |= OBJ_WRECKAGE;
	}
	if (w->options.asteroidCollisionMayKill->GetBool()) {
		killing_shots |= OBJ_ASTEROID;
	}

	w->CellGetObjects(OBJ_X_IN_BLOCKS(pl), OBJ_Y_IN_BLOCKS(pl),
					 (int)(visibility_distance / BLOCK_SZ), max_objs,
					 &obj_list, &obj_count);

	DRT()
	for (j = 0; j < obj_count; j++) {

		DRT()
		shot = obj_list[j];

		/* Get rid of the most common object types first for speed. */
		if (BIT(shot->type, OBJ_DEBRIS|OBJ_SPARK)) {
			continue;
		}

		dx = WRAP_DX(w, shot->pos.x - pl->pos.x);
		dy = WRAP_DY(w, shot->pos.y - pl->pos.y);

		if (BIT(shot->type, OBJ_BALL)
			&& !WITHIN(last_thrown_ball,
					   robot_count,
					   3 * w->GetFPS())) {
			SET_BIT(pl->used, HAS_CONNECTOR);
		}
		DRT()

		/* Ignore shots if shields already up - nothing else to do anyway */
		if (BIT(shot->type, OBJ_SHOT|OBJ_CANNON_SHOT)
		 && BIT(pl->used, HAS_SHIELD)) {
			Watch(WATCH_SHIELDS, "Shields up, ignore shots");
			continue;
		}

		/*-BA This code shouldn't be executed for `friendly` shots
		 *-BA Moved down 2 paragraphs
		 *		if (BIT(shot->type, OBJ_SMART_SHOT|OBJ_HEAT_SHOT|OBJ_MINE)) {
		 *			fx = shot->pos.x - pl->pos.x;
		 *			fy = shot->pos.y - pl->pos.y;
		 *			if ((dx = fx, dx = WRAP_DX(dx), ABS(dx)) < mine_dist
		 *				&& (dy = fy, dy = WRAP_DY(dy), ABS(dy)) < mine_dist
		 *				&& (distance = LENGTH(dx, dy)) < mine_dist) {
		 *				mine_i = j;
		 *				mine_dist = distance;
		 *			}
		 *			if ((dx = fx + (shot->vel.x - pl->vel.x) * ROB_LOOK_AH,
		 *					dx = WRAP_DX(dx), ABS(dx)) < mine_dist
		 *				&& (dy = fy + (shot->vel.y - pl->vel.y) * ROB_LOOK_AH,
		 *					dy = WRAP_DY(dy), ABS(dy)) < mine_dist
		 *				&& (distance = LENGTH(dx, dy)) < mine_dist) {
		 *				mine_i = j;
		 *				mine_dist = distance;
		 *			}
		 *		}
		 */

		/*
		 * The only thing left to do regarding objects is to check if
		 * this robot needs to put up shields to protect against objects.
		 */
		if (!BIT(shot->type, killing_shots))
		{
			/* Find closest item */
			if (BIT(shot->type, OBJ_ITEM)) {
				if (ABS(dx) < *item_dist && ABS(dy) < *item_dist) {
					int imp;

					if (BIT(shot->status, RANDOM_ITEM)) {
						imp = ROBOT_HANDY_ITEM; 		/* It doesn't know what it is, so get it if it can */
					} else {
						imp = Rank_item_value((ITEM)obj_list[j]->info);
					}
					if (imp > ROBOT_IGNORE_ITEM && imp >= *itemImportance) {
						*itemImportance = imp;
						*item_dist = (int) LENGTH(dx, dy);
						*item = obj_list[j];
					}
				}
			}
			DRT()
			continue;
		}

		/*
		 * Any shot of team members excluding self are passive.
		 */
		if (pl->TeamImmune(shot->id)) {
			continue;
		}

		/*
		 * Self shots may be passive too...
		 */
		if (shot->id == pl->id
			&& w->options.selfImmunity->GetBool()) {
			continue;
		}

		DRT()
		/* Find nearest missile/mine */
		if (BIT(shot->type, OBJ_TORPEDO|OBJ_SMART_SHOT|OBJ_ASTEROID
							|OBJ_HEAT_SHOT|OBJ_BALL|OBJ_CANNON_SHOT)
			|| (BIT(shot->type, OBJ_SHOT)
				&& !BIT(w->rules->mode, TIMING)
				&& shot->id != pl->id
				&& shot->id != NO_ID)
			|| (BIT(shot->type, OBJ_MINE)
				&& shot->id != pl->id)
			|| (BIT(shot->type, OBJ_WRECKAGE)
				&& !BIT(w->rules->mode, TIMING))) {
			if (ABS(dx) < *mine_dist
				&&	ABS(dy) < *mine_dist
				&& (distance = (int)LENGTH(dx, dy)) < *mine_dist) {
				*mine_i = j;
				*mine_dist = distance;
			}
			if ((dx = (int)((shot->pos.x - pl->pos.x)
							 + (shot->vel.x - pl->vel.x)),
					dx = WRAP_DX(w, dx), ABS(dx)) < *mine_dist
				&& (dy = (int)((shot->pos.y - pl->pos.y)
								 + (shot->vel.y - pl->vel.y)),
					dy = WRAP_DY(w, dy), ABS(dy)) < *mine_dist
				&& (distance = (int)LENGTH(dx, dy)) < *mine_dist) {
				*mine_i = j;
				*mine_dist = distance;
			}
		}

		DRT()
		shield_range = 21 + SHIP_SZ + shot->pl_range;

		if ((dx = (int)(shot->pos.x + shot->vel.x - (pl->pos.x + pl->vel.x)),
			 dx = WRAP_DX(w, dx),
			 ABS(dx)) < shield_range
		 && (dy = (int)(shot->pos.y + shot->vel.y - (pl->pos.y + pl->vel.y)),
			 dy = WRAP_DY(w, dy),
			 ABS(dy)) < shield_range
		 && sqr(dx) + sqr(dy) <= sqr(shield_range)
		 && (int)(rfrac() * 100)
		    < (85 + (defense / 7) - (attack / 50)))
		{
			Watch(WATCH_SHIELDS, "Shields up, SHOT AT");
			SetShields();
			SET_BIT(pl->status, THRUSTING);

			DRT()
			if (BIT(shot->type, OBJ_TORPEDO|OBJ_SMART_SHOT|OBJ_ASTEROID
								|OBJ_HEAT_SHOT|OBJ_MINE)
				&& (pl->fuel.sum < pl->fuel.l3
					|| !BIT(pl->have, HAS_SHIELD))) {
				if (pl->item[ITEM_HYPERJUMP] > 0
					&& pl->fuel.sum > -ED_HYPERJUMP) {
					pl->item[ITEM_HYPERJUMP]--;
					Add_fuel(&(pl->fuel), ED_HYPERJUMP);
					pl->DoHyperjump();
					break;
				}
			}
			break;
		}
		DRT()
		// DR( assert(!(netServer->mainLoops == 2268 && j == 15));)
		if (BIT(shot->type, OBJ_SMART_SHOT)) {
			if (*mine_dist < ECM_DISTANCE / 4)
				pl->FireEcm();
		}
		DRT()
		if (BIT(shot->type, OBJ_MINE)) {
			if (*mine_dist < ECM_DISTANCE / 2)
				pl->FireEcm();
		}
		DRT()
		if (BIT(shot->type, OBJ_HEAT_SHOT)) {
			CLR_BIT(pl->status, THRUSTING);
			if (pl->fuel.sum < pl->fuel.l3
				&& pl->fuel.sum > pl->fuel.l1
				&& pl->fuel.num_tanks > 0) {
				Tank_handle_detach(pl);
			}
		}
		DRT()
		if (BIT(shot->type, OBJ_ASTEROID)) {
			int delta_dir = 0;
			if (*mine_dist > (WIRE_PTR(shot)->size == 1 ? 2 : 4) * BLOCK_SZ
				&& *mine_dist < 8 * BLOCK_SZ
				&& (delta_dir = (pl->dir
								 - w->WrapFindDir(shot->pos.x - pl->pos.x,
												shot->pos.y - pl->pos.y))
					< WIRE_PTR(shot)->size * (RES / 10)
					|| delta_dir > RES - WIRE_PTR(shot)->size * (RES / 10))) {
				SET_BIT(pl->used, HAS_SHOT);
			}
		}
		DRT()
	}
}

///////////////////////////////////////////////////////////////////////////////
void Robot5::Robot_default_play_check_lasers() {
	int 						j;
	int 						dx, dy;
	int 						distance2;
	int 						shield_range;

	/*
	 * Test if others are firing lasers at us.
	 * Maybe move this into the player loop.
	 */
	if (BIT(pl->used, HAS_SHIELD) == 0
		&& BIT(pl->have, HAS_SHIELD) != 0) {
		shield_range = 21 + SHIP_SZ;
		for (j = 0; j < w->numPulses; j++) {
			Pulse *pulse = w->pulses[j];
			if (pulse->id == pl->id
				&& !pulse->refl)
				continue;
				if (pl->TeamImmune(pulse->id))
						continue;
			if (pl->id == pulse->id
				&& w->options.selfImmunity->GetBool())
				continue;
			dx = (long)WRAP_DX(w, pl->pos.x - pulse->pos.x);
			dy = (long)WRAP_DY(w, pl->pos.y - pulse->pos.y);
			distance2 = sqr(dx) + sqr(dy);
			if ((distance2 < sqr(PULSE_LENGTH)
				 || (distance2 < sqr(2 * PULSE_LENGTH)
					 && ABS(FindDir(dx, dy) - pulse->dir) < RES / 8))
				&& (int)(rfrac() * 100) <
				   (85 + (defense / 7) - (attack / 50))) {
				Watch(WATCH_SHIELDS, "Shields up, LASER SHOT");
				SetShields();
				break;
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
void Robot5::Play() {
	Watch(WATCH_SHIELDS, "Shield page for %s\n", pl->name);
	Watch(WATCH_ITEMS, "Item page for %s\n", pl->name);
	Watch(WATCH_NAV, "Navigation page for %s\n", pl->name);

	lastDefensesUsed = BIT(pl->used, HAS_SHIELD | HAS_EMERGENCY_SHIELD | HAS_PHASING_DEVICE);

	Watch(WATCH_SHIELDS, "lastDefensesUsed=%c%c%c",
		lastDefensesUsed & HAS_SHIELD ? 'S' : ' ',
		lastDefensesUsed & HAS_EMERGENCY_SHIELD ? 'E' : ' ',
		lastDefensesUsed & HAS_PHASING_DEVICE ? 'P' : ' ');
	newDefensesUsed = 0;
	_Play();
	PostPlayShieldsCleanup();
}

///////////////////////////////////////////////////////////////////////////////
void Robot5::_Play() {
	Player*	ship;
	DFLOAT	distance, ship_dist,
			enemy_dist,
			speed, x_speed, y_speed;
	int 	mine_dist;
	int 	mine_i;
	int 	j;
	int		ship_i;
	int		enemy_i;
	int 	dx, dy, x, y;
	bool	harvest_checked;
	bool	evade_checked;
	bool	navigate_checked;
	int 	shoot_time;

	DRT()

	if (robot_count <= 0)
		robot_count = 1000 + (int)(rfrac() * 32);

	robot_count--;

//	CLR_BIT(pl->used, HAS_SHOT | HAS_SHIELD | HAS_CLOAKING_DEVICE | HAS_LASER | HAS_PHASING_DEVICE);
	CLR_BIT(pl->used, HAS_SHOT | HAS_SHIELD | HAS_CLOAKING_DEVICE | HAS_LASER);

	Watch(WATCH_ITEMS, "longterm_mode = %d", longterm_mode);
	Watch(WATCH_SHIELDS, "attack=%2d defense=%2d", attack, defense);

	harvest_checked = false;
	evade_checked = false;
	navigate_checked = false;

	mine_i = -1;
	mine_dist = SHIP_SZ + 200;

	if (BIT(pl->have, HAS_CLOAKING_DEVICE) && pl->fuel.sum > pl->fuel.l2)
		SET_BIT(pl->used, HAS_CLOAKING_DEVICE);

	if (BIT(pl->have, HAS_EMERGENCY_THRUST)
		&& !BIT(pl->used, HAS_EMERGENCY_THRUST)) {
		pl->EmergencyThrust(true);
	}
	if (BIT(pl->have, HAS_EMERGENCY_SHIELD)
		&& !BIT(pl->used, HAS_EMERGENCY_SHIELD)) {
		pl->EmergencyShield(true);
	}

	DRT()

	if (BIT(pl->have, HAS_DEFLECTOR) && !BIT(w->rules->mode, TIMING)) {
		pl->Deflector(true);
	}

	if (pl->fuel.sum <= (BIT(w->rules->mode, TIMING) ? 0 : pl->fuel.l1)) {
		if (!BIT(pl->status, SELF_DESTRUCT)) {
			SET_BIT(pl->status, SELF_DESTRUCT);
			pl->count = 150;
		}
	} else {
		CLR_BIT(pl->status, SELF_DESTRUCT);
	}

	/* blinded by ECM. since we're not supposed to see anything,
	   put up shields and return */
	if (pl->damaged > 0) {
		Watch(WATCH_SHIELDS, "Set shields, damaged\n");
		SetShields();
		return;
	}

	if (pl->fuel.sum < pl->fuel.max * 0.80) for (j = 0; j < w->numFuels; j++) {
		int dx, dy;
		if (BIT(w->rules->mode, TEAM_PLAY)
			&& w->options.teamFuel->GetBool()
			&& w->fuels[j].team != pl->team) {
			continue;
		}
		dx = (int)(w->fuels[j].pix_pos.x - pl->pos.x);
		dy = (int)(w->fuels[j].pix_pos.y - pl->pos.y);
		/* dx = WRAP_DX(dx);
		   dy = WRAP_DY(dy); */
		if (sqr(dx) + sqr(dy) <= sqr(90)
			&& w->fuels[j].fuel > REFUEL_RATE) {
			pl->fs = j;
			SET_BIT(pl->used, HAS_REFUEL);
			break;
		} else {
			CLR_BIT(pl->used, HAS_REFUEL);
		}
	}

	/* don't turn NEED_FUEL off until refueling stops */
	if (pl->fuel.sum < (BIT(w->rules->mode, TIMING) ?
						pl->fuel.l1 : pl->fuel.l3)) {
		SET_BIT(longterm_mode, NEED_FUEL);
	} else if (!BIT(pl->used, HAS_REFUEL)) {
		CLR_BIT(longterm_mode, NEED_FUEL);
	}

	DRT()

	if (BIT(w->rules->mode, TEAM_PLAY)) {
		for (j = 0; j < w->numTargets; j++) {
			if (w->targets[j].team == pl->team
				&& w->targets[j].damage < TARGET_DAMAGE
				&& w->targets[j].dead_time >= 0) {
				int dx = (w->targets[j].pos.x * BLOCK_SZ + BLOCK_SZ / 2)
						 - pl->pos.x;
				int dy = (w->targets[j].pos.y * BLOCK_SZ + BLOCK_SZ / 2)
						 - pl->pos.y;
				/* dx = WRAP_DX(dx);
				   dy = WRAP_DY(dy); */
				if (sqr(dx) + sqr(dy) <= sqr(90)) {
					pl->repair_target = j;
					SET_BIT(pl->used, HAS_REPAIR);
					break;
				}
			}
		}
	}

	///////////////////////////////////////////////////////////////////////////
	// int		item_i = -1;
	int		item_dist = (int) visibility_distance;
	int		item_imp = ROBOT_IGNORE_ITEM;
	Object* item;
	PlayCheckObjects(&item_dist, &item_imp, &item,
					 &mine_i, &mine_dist);
	DRT()
	Robot_default_play_check_lasers();
	DRT()


	/* Note: Only take time to navigate if not being shot at */
	/* KK: it seems that this 'Check_robot_navigate' function caused
		the infamous 'robot stuck under wall' bug, so I commented it out */
	/* KK: ps. I tried to change that function, but I don't grok it */


	/* dik: reenabled this... */
	if (!(BIT(pl->used, HAS_SHIELD) && BIT(pl->status, THRUSTING))
	 && CheckNavigate(&evade_checked))
	{
		if (!w->options.allowShields->GetBool()
		 && w->options.playerStartsShielded->GetBool()
		 && BIT(pl->have, HAS_SHIELD | HAS_PHASING_DEVICE)
		 && randomMT(99) < defense)
		{
				Watch(WATCH_SHIELDS, "Set shields, CheckNavigate:true evade_checked=%s", evade_checked ? "true" : "false");
				SetShields();
		}
		return;
	}

	/* KK: unfortunately, this introduced a new bug. robots with large
		shipshapes don't take off from their bases. here's an attempt to
		fix it */
	if (QUICK_LENGTH(pl->pos.x - (w->bases[pl->home_base].pos.x * BLOCK_SZ),
					 pl->pos.y - (w->bases[pl->home_base].pos.y * BLOCK_SZ))
		< BLOCK_SZ) {
		SET_BIT(pl->status, THRUSTING);
	}

	ship_i = -1;
	ship_dist = SHIP_SZ * 6;
	enemy_i = -1;
	if (pl->fuel.sum > pl->fuel.l3) {
		enemy_dist = (BIT(w->rules->mode, LIMITED_VISIBILITY) ?
					  MAX(pl->fuel.sum * ENERGY_RANGE_FACTOR,
						  visibility_distance)
					  : max_enemy_distance);
	}
	else {
		enemy_dist = visibility_distance;
	}

	DRT()

	if (BIT(pl->used, HAS_SHIELD))
		ship_dist = 0;

	if (BIT(robot_lock, LOCK_PLAYER)) {
		j = w->getInd[robot_lock_id];
		ship = w->players[j];

		if (BIT(w->players[w->getInd[robot_lock_id]]->status,
				PLAYING|GAME_OVER|PAUSE) == PLAYING) {

			if (Detect_hunt(j)) {

				if (BIT(robot_lock, LOCK_PLAYER)
					&& robot_lock_id == ship->id) {
					lock_last_seen = robot_count;
					lock_last_pos.x = ship->pos.x;
					lock_last_pos.y = ship->pos.y;
				}

				dx = ship->pos.x - pl->pos.x, dx = WRAP_DX(w, dx);
				dy = ship->pos.y - pl->pos.y, dy = WRAP_DY(w, dy);
				distance = LENGTH(dx, dy);

				if (distance < ship_dist) {
					ship_i = w->getInd[robot_lock_id];
					ship_dist = distance;
				}

				if (distance < enemy_dist) {
					enemy_i = j;
					enemy_dist = distance;
				}
			}
		}
	}

	DRT()

	if (ship_i == -1 || enemy_i == -1) {

		for (j = 0; j < w->numPlayers; j++) {
			ship = w->players[j];
			if (j == pl->Ind()
				|| BIT(ship->status, PLAYING|GAME_OVER|PAUSE) != PLAYING
				|| pl->TeamImmune(ship->id))
				continue;

			if (!Detect_hunt(j))
				continue;

			dx = ship->pos.x - pl->pos.x, dx = WRAP_DX(w, dx);
			dy = ship->pos.y - pl->pos.y, dy = WRAP_DY(w, dy);
			distance = LENGTH(dx, dy);

			if (distance < ship_dist) {
				ship_i = j;
				ship_dist = distance;
			}

			if (!BIT(robot_lock, LOCK_PLAYER)) {
				if ((robot_count % 3) == 0
					&& ((robot_count % 100) < attack)
					&& distance < enemy_dist) {
					enemy_i    = j;
					enemy_dist = distance;
				}
			}
		}
	}

	DRT()

	if (ship_dist < 3*SHIP_SZ) {
		Watch(WATCH_SHIELDS, "ship_dist=%f SetShields\n", ship_dist);
		SetShields();
	} else
		Watch(WATCH_SHIELDS, "ship_dist=%f\n", ship_dist);

	if (ship_dist <= 10*BLOCK_SZ && pl->fuel.sum <= pl->fuel.l3
		&& !BIT(w->rules->mode, TIMING)) {
		if (pl->item[ITEM_HYPERJUMP] > 0 && pl->fuel.sum > -ED_HYPERJUMP) {
			pl->item[ITEM_HYPERJUMP]--;
			Add_fuel(&(pl->fuel), ED_HYPERJUMP);
			pl->DoHyperjump();
			return;
		}
	}

	if (ship_i != -1
		&& BIT(robot_lock, LOCK_PLAYER)
		&& robot_lock_id == w->players[ship_i]->id) {
		ship_i = -1; /* don't avoid target */
	}

	if (enemy_i >= 0) {
		ship = w->players[enemy_i];
		if (!BIT(pl->lock.tagged, LOCK_PLAYER)
			|| (enemy_dist < pl->lock.distance/2
				&& (BIT(w->rules->mode, TIMING) ?
					(ship->check >= pl->check
					 && ship->round >= pl->round) : 1))
			|| (enemy_dist < pl->lock.distance*2
				&& BIT(w->rules->mode, TEAM_PLAY)
				&& BIT(ship->have, HAS_BALL))
			|| ship->score > w->players[w->getInd[pl->lock.pl_id]]->score) {
			pl->lock.pl_id = ship->id;
			SET_BIT(pl->lock.tagged, LOCK_PLAYER);
			pl->lock.distance = enemy_dist;
			pl->ComputeSensorRange();
		}
	}

	DRT()

	if (BIT(pl->lock.tagged, LOCK_PLAYER)) {
		int delta_dir;
		ship = w->players[w->getInd[pl->lock.pl_id]];
		delta_dir = (int)(pl->dir - w->WrapFindDir(ship->pos.x - pl->pos.x,
					ship->pos.y - pl->pos.y));
		delta_dir = MOD2(delta_dir, RES);
		if (BIT(ship->status, PLAYING|PAUSE|GAME_OVER) != PLAYING
			|| (BIT(robot_lock, LOCK_PLAYER)
				&& robot_lock_id != pl->lock.pl_id
				&& BIT(w->players[w->getInd[robot_lock_id]]->status,
					   PLAYING|PAUSE|GAME_OVER) == PLAYING)
			|| !Detect_hunt(w->getInd[ship->id])
			|| (pl->fuel.sum <= pl->fuel.l3
				&& !BIT(w->rules->mode, TIMING))
			|| (BIT(w->rules->mode, TIMING)
				&& (delta_dir < 3 * RES / 4 || delta_dir > RES / 4))
			|| pl->TeamImmune(ship->id)) {
			/* unset the player lock */
			CLR_BIT(pl->lock.tagged, LOCK_PLAYER);
			pl->lock.pl_id = 1;
			pl->lock.distance = 0;
		}
	}
	DRT()

	if (!evade_checked) {
		if (Check_robot_evade(mine_i, ship_i)) {
			if (w->options.allowShields->GetBool() == 0
			 && w->options.playerStartsShielded->GetBool() != 0
			 && BIT(pl->have, HAS_SHIELD)
			 && randomMT(99) < defense)
			{
				Watch(WATCH_SHIELDS, "Set shields, CheckRobotEvade");
				SetShields();
			}
			else if (w->options.maxShieldedWallBounceSpeed->GetDouble()
				   > w->options.maxUnshieldedWallBounceSpeed->GetDouble()
				&&   w->options.maxShieldedPlayerWallBounceAngle->GetDouble()
				  >= w->options.maxUnshieldedPlayerWallBounceAngle->GetDouble()
				&& BIT(pl->have, HAS_SHIELD)) {
				Watch(WATCH_SHIELDS, "Set shields, Bounce");
				SetShields();
			}
			return;
		}
	}
	DRT()

	// Race mode handling for items
	if (BIT(w->rules->mode, TIMING) && !navigate_checked) {
		int delta_dir;
		if (item) {
			delta_dir = (int)(pl->dir - w->WrapFindDir(item->pos.x - pl->pos.x,
											   item->pos.y - pl->pos.y));
			delta_dir = MOD2(delta_dir, RES);
		} else {
			delta_dir = RES;
			item_imp = ROBOT_IGNORE_ITEM;
		}
		if ((item_imp == ROBOT_MUST_HAVE_ITEM && item_dist > 4 * BLOCK_SZ)
			|| (item_imp == ROBOT_HANDY_ITEM && item_dist > 2 * BLOCK_SZ)
			|| (item_imp == ROBOT_IGNORE_ITEM)
			|| (delta_dir < 3 * RES / 4 && delta_dir > RES / 4)) {
			navigate_checked = true;
			if (Check_robot_target(w->check[pl->check].x * BLOCK_SZ,
								   w->check[pl->check].y * BLOCK_SZ,
								   RM5_NAVIGATE)) {
				return;
			}
		}
	}
	DRT()

	if (item
		&& enemy_dist*3 > item_dist*2
		&& item_dist < 12*BLOCK_SZ
		&& !BIT(longterm_mode, FETCH_TREASURE)
		&& (!BIT(longterm_mode, NEED_FUEL)
			|| item->info == ITEM_FUEL
			|| item->info == ITEM_TANK)) {

		if (item_imp != ROBOT_IGNORE_ITEM) {
			harvest_checked = true;
			dx = item->pos.x;
			dx += (long)(item->vel.x * (ABS(dx - pl->pos.x) /
										robot_normal_speed));
			dy = item->pos.y;
			dy += (long)(item->vel.y * (ABS(dy - pl->pos.y) /
										robot_normal_speed));

			if (Check_robot_target(dx, dy, RM5_HARVEST)) {
				Watch(WATCH_ITEMS, "Harvest Item %s\n", itemTypeText[item->info]);
				WatchPlay(WATCH_ITEMS, WatchHollowCircle, 1, &item->pos, 0);
				return;
			}
			Watch(WATCH_ITEMS, "Skip Item %s\n", itemTypeText[item->info]);
			WatchPlay(WATCH_ITEMS, WatchHollowCircle, 5, &item->pos, 50);
		}
	}
	if (BIT(pl->lock.tagged, LOCK_PLAYER) &&
		Detect_hunt(w->getInd[pl->lock.pl_id])) {

		ship = w->players[w->getInd[pl->lock.pl_id]];
		shoot_time = (int)(pl->lock.distance / (w->options.shotSpeed->GetDouble() + 1));
		dx = (long)(ship->pos.x + ship->vel.x * shoot_time);
		dy = (long)(ship->pos.y + ship->vel.y * shoot_time);
		/*-BA Also allow for our own momentum. */
		dx -= (long)(pl->vel.x * shoot_time);
		dy -= (long)(pl->vel.y * shoot_time);

		if (Check_robot_target(dx, dy, RM5_ATTACK)
			&& !BIT(longterm_mode, FETCH_TREASURE
											|TARGET_KILL
											|NEED_FUEL)) {
			return;
		}
	}
	DRT()

	if (BIT(w->rules->mode, TEAM_PLAY)
		&& w->numTreasures > 0
		&& w->teams[pl->team].numTreasures > 0
		&& !navigate_checked
		&& !BIT(longterm_mode, TARGET_KILL|NEED_FUEL)) {
		navigate_checked = true;
		if (Ball_handler())
			return;
	}
	DRT()

	if (item
		&& !harvest_checked
		&& item_dist < 12*BLOCK_SZ) {
		Watch(WATCH_ITEMS, "More item checking");
		if (item_imp != ROBOT_IGNORE_ITEM) {
			dx = item->pos.x;
			dx += (long)(item->vel.x * (ABS(dx - pl->pos.x) /
										robot_normal_speed));
			dy = item->pos.y;
			dy += (long)(item->vel.y * (ABS(dy - pl->pos.y) /
										robot_normal_speed));

			if (Check_robot_target(dx, dy, RM5_HARVEST))
				return;
		}
	}

	if (Check_robot_hunt()) {
		if (w->options.allowShields->GetBool()== 0
		 && w->options.playerStartsShielded->GetBool() != 0
		 && BIT(pl->have, HAS_SHIELD)
		 && randomMT(99) < defense)
		{
			Watch(WATCH_SHIELDS, "Set shields, CheckRobotHunt");
			SetShields();
		}
		return;
	}

	DRT()

	if (Robot_default_play_check_map() == 1)
		return;

	DRT()

	// If we made it this far, then put the shields up? just because?
	// That doesn't make sense
	// Maybe this should be, put shields up if they're allowed.
	/*
	if (w->options.allowShields->GetBool() == 0
	 && w->options.playerStartsShielded->GetBool() != 0
	 && BIT(pl->have, HAS_SHIELD)) {
		Watch(WATCH_SHIELDS, "Set shields, Because");
		SET_BIT(pl->used, HAS_SHIELD);
		if (!w->options.cloakedShield->GetBool())
			CLR_BIT(pl->used, HAS_CLOAKING_DEVICE);
	}
	*/
	x = OBJ_X_IN_BLOCKS(pl);
	y = OBJ_Y_IN_BLOCKS(pl);
	x_speed = pl->vel.x - 2 * w->gravity[x][y].x;
	y_speed = pl->vel.y - 2 * w->gravity[x][y].y;

	if (y_speed < (-robot_normal_speed) || (robot_count % 64) < 32) {

		robot_mode = RM5_ROBOT_CLIMB;
		pl->turnspeed = MAX_PLAYER_TURNSPEED / 2;
		pl->power = MAX_PLAYER_POWER / 2;
		if (ABS(pl->dir - RES / 4) > RES / 16) {
			pl->turnacc = (pl->dir < RES / 4
						   || pl->dir >= 3 * RES / 4
						   ? pl->turnspeed : (-pl->turnspeed));
		} else {
			pl->turnacc = 0.0;
		}
		if (y_speed < robot_normal_speed / 2
			&& pl->velocity < robot_attack_speed)
			SET_BIT(pl->status, THRUSTING);
		else if (y_speed > robot_normal_speed)
			CLR_BIT(pl->status, THRUSTING);
		return;
	}
	robot_mode = RM5_ROBOT_IDLE;
	pl->turnspeed = MAX_PLAYER_TURNSPEED / 2;
	pl->turnacc = 0;
	pl->power = MAX_PLAYER_POWER / 2;
	CLR_BIT(pl->status, THRUSTING);
	speed = LENGTH(x_speed, y_speed);
	if (speed < robot_normal_speed / 2)
		SET_BIT(pl->status, THRUSTING);
	else if (speed > robot_normal_speed)
		CLR_BIT(pl->status, THRUSTING);
	DRT()
}

///////////////////////////////////////////////////////////////////////////////
// Try to engage the shields, or the eshields, or a phasing.
void Robot5::SetShields(bool on) {
	if (on) {
		if (BIT(pl->have, HAS_SHIELD)) {
			SET_BIT(pl->used, HAS_SHIELD);
			if (!w->options.cloakedShield->GetBool())
			   CLR_BIT(pl->used, HAS_CLOAKING_DEVICE);
			return;
		}
		if (BIT(pl->have, HAS_EMERGENCY_SHIELD)
		 && !BIT(pl->used, HAS_EMERGENCY_SHIELD)) {
			newDefensesUsed |= HAS_EMERGENCY_SHIELD;
			SET_BIT(pl->used, HAS_SHIELD);
			if (!w->options.cloakedShield->GetBool())
			   CLR_BIT(pl->used, HAS_CLOAKING_DEVICE);
			return;
		}
		if (BIT(pl->have, HAS_PHASING_DEVICE)
		 || pl->phasing_left) {
			if (!BIT(lastDefensesUsed, HAS_PHASING_DEVICE))
				newDefensesUsed |= HAS_PHASING_DEVICE;
			return;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
void Robot5::PostPlayShieldsCleanup() {
	if (BIT(lastDefensesUsed, HAS_PHASING_DEVICE)
	 && !BIT(newDefensesUsed, HAS_PHASING_DEVICE))
		pl->Phasing(false);

	if (!BIT(lastDefensesUsed, HAS_PHASING_DEVICE)
	 && BIT(newDefensesUsed, HAS_PHASING_DEVICE))
		pl->Phasing(true);
/*
	if (BIT(lastDefensesUsed, HAS_EMERGENCY_SHIELD)
	 && !BIT(newDefensesUsed, HAS_EMERGENCY_SHIELD))
		pl->EmergencyShield(false);

	if (!BIT(lastDefensesUsed, HAS_EMERGENCY_SHIELD)
	 && BIT(newDefensesUsed, HAS_EMERGENCY_SHIELD))
		pl->EmergencyShield(true);
*/
}

///////////////////////////////////////////////////////////////////////////////
/*
 * This is called each round.
 * It allows us to adjust our file local parameters.
 */
 void Robot5::RoundTick() {
	DFLOAT				min_visibility = 256.0;
	DFLOAT				min_enemy_distance = 512.0;

	/* reduce visibility when there are a lot of robots. */
	visibility_distance = min_visibility
		+ (((VISIBILITY_DISTANCE - min_visibility)
			* (NUM_IDS - w->numRobots)) / NUM_IDS);

	/* limit distance to allowable enemies. */
	max_enemy_distance = w->hypotenuse;
	if (w->hypotenuse > visibility_distance) {
		max_enemy_distance = min_enemy_distance
			+ (((w->hypotenuse - min_enemy_distance)
				* (NUM_IDS - w->numRobots)) / NUM_IDS);
	}
}
