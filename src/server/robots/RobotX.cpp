/* $Id: RobotX.cpp,v 1.1 2007/01/11 04:18:57 dick Exp $
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
 *  $Log: RobotX.cpp,v $
 *  Revision 1.1  2007/01/11 04:18:57  dick
 *  Robots move to their own subdirectory
 *
 *  Revision 1.25  2004/05/22 15:10:46  dick
 *  LASER_MODIFIERS becomes LASER_MODS
 *
 *  Revision 1.24  2004/02/23 07:17:12  dick
 *  Better living through _DEBUG.
 *
 *  Revision 1.23  2004/02/23 02:06:09  dick
 *  robot.h becomes Robot.h
 *
 *  Revision 1.22  2004/02/18 01:22:14  dick
 *  Remove extraneous and obsolete #include "netserver.h"
 *
 *  Revision 1.21  2004/02/03 03:39:54  dick
 *  Add Windows precompiled header support.
 *
 *  Revision 1.20  2002/09/11 16:42:04  dick
 *  Here endeth the server encapsulation task.
 *  theWorld is now an array of World(s) and get loaded dynamically.
 *
 *  Revision 1.19  2002/09/01 00:13:46  dick
 *  Remove obsolete windows #include "winServer.h"
 *
 *  Revision 1.18  2002/07/11 16:36:03  dick
 *  Update to XPilot-4.5.4beta
 *
 *  Revision 1.17  2002/07/08 05:52:59  dick
 *  showtime() handles the space after itself
 *
 *  Revision 1.16  2002/06/30 06:45:54  dick
 *  Add some asserts around the availability of robot data.
 *
 *  Revision 1.15  2002/06/01 06:29:59  dick
 *  cannon.* becomes Cannon.*
 *
 *  Revision 1.14  2002/06/01 06:06:48  dick
 *  Encapsulate (almost) everything.  Get rid of (almost) all refs to theWorld.
 *
 *  Revision 1.13  2002/05/22 05:09:01  dick
 *  Update to XPilot r4_5_4alpha.
 *
 *  Revision 1.12  2002/05/18 22:34:15  dick
 *  Move showtime to common.
 *
 *  Revision 1.11  2002/05/18 20:55:39  dick
 *  Update to XPilot-4.5.1
 *
 *  Revision 1.10  2001/08/14 21:37:14  dick
 *  Array Obj[] becomes objs[].  We now have a class called Obj.
 *
 *  Revision 1.9  2001/08/05 08:05:36  dick
 *  Add Tuners to the server.  ComputeSensorRange(), AddTank(), RemoveTank()
 *  become members of Player.  ComputeGravity*() become members of World.
 *
 *  Revision 1.8  2001/07/25 21:11:19  dick
 *  float to int warnings
 *
 *  Revision 1.7  2001/07/24 09:04:00  dick
 *  Remove all global options.  Use World.options instead.
 *
 *  Revision 1.6  2001/07/23 21:08:37  dick
 *  Encapsulate ShipMass, ShotsMax, ShotsLife, ShotsMass, FPS.
 *
 *  Revision 1.5  2001/07/23 07:38:42  dick
 *  Fold contact, metaserver, netserver, rules, and server to be members of World.
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
#include "proto.h"
#include "showtime.h"
#include "score.h"
#include "bit.h"
#include "saudio.h"
#include "pack.h"
#include "Robot.h"
#include "error.h"
#include "portability.h"
#include "commonproto.h"
#include "Cannon.h"

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


char robotdef_version[] = VERSION;

#define ROB_LOOK_AH 			2

#define WITHIN(NOW,THEN,DIFF) (NOW<=THEN && (THEN-NOW)<DIFF)

/*
 * Flags for the default robots being in different modes (or moods).
 */
#define RM_ROBOT_IDLE			(1 << 2)
#define RM_EVADE_LEFT			(1 << 3)
#define RM_EVADE_RIGHT			(1 << 4)
#define RM_ROBOT_CLIMB			(1 << 5)
#define RM_HARVEST				(1 << 6)
#define RM_ATTACK				(1 << 7)
#define RM_TAKE_OFF 			(1 << 8)
#define RM_CANNON_KILL			(1 << 9)
#define RM_REFUEL				(1 << 10)
#define RM_NAVIGATE 			(1 << 11)

/* long term modes */
#define FETCH_TREASURE			(1 << 0)
#define TARGET_KILL 			(1 << 1)
#define NEED_FUEL				(1 << 2)

/*
 * Map objects a robot can fly through without damage.
 */
#define EMPTY_SPACE(s)	BIT(1 << (s), SPACE_BLOCKS)


/*
 * Prototypes for methods of the default robot type.
 */
static void Robot_default_round_tick(World* w);
static void Robot_default_create(World* w, int ind, char *str);
static void Robot_default_go_home(World* w, int ind);
static void Robot_default_play(World* w, int ind);
static void Robot_default_set_war(World* w,int ind, int victim_id);
static int	Robot_default_war_on_player(World* w, int ind);
static void Robot_default_message(World* w, int ind, const char *str);
static void Robot_default_destroy(World* w, int ind);
static void Robot_default_invite(World* w, int ind, int inv_ind);
	   int	Robot_default_setup(World* w, RobotType *type_ptr);


/*
 * Local static variables
 */
static DFLOAT	Visibility_distance;
static DFLOAT	Max_enemy_distance;


/*
 * The robot type structure for the default robot.
 */
static RobotType robot_default_type = {
	"default",
	Robot_default_round_tick,
	Robot_default_create,
	Robot_default_go_home,
	Robot_default_play,
	Robot_default_set_war,
	Robot_default_war_on_player,
	Robot_default_message,
	Robot_default_destroy,
	Robot_default_invite
};


/*
 * The only thing we export from this file.
 * A function to initialize the robot type structure
 * with our name and the pointers to our action routines.
 *
 * Return 0 if all is OK, anything else will ignore this
 * robot type forever.
 */
int Robot_default_setup(RobotType *type_ptr)
{
	/* Not much to do for the default robot except init the type structure. */

	*type_ptr = robot_default_type;

	return 0;
}

/*
 * Private functions.
 */
/*static bool Check_robot_navigate(int ind, bool * num_evade);*/
static bool Check_robot_evade(World* w, int ind, int mine_i, int ship_i);
static bool Check_robot_target(World* w, int ind, int item_x, int item_y, int new_mode);
static bool Detect_hunt(World* w, int ind, int j);
static int Rank_item_value(World* w, int ind, Items itemtype);
static bool Ball_handler(World* w, int ind);


/*
 * Function to cast from player structure to robot data structure.
 * This isolates casts (aka. type violations) to a few places.
 */
static RobotDefaultData *Robot_default_get_data(Player *pl)
{
		assert(pl != 0);
		assert(pl->robot_data_ptr != 0);
		assert(pl->robot_data_ptr->private_data != 0);
	return (RobotDefaultData *)pl->robot_data_ptr->private_data;
}

/*
 * A default robot is created.
 */
static void Robot_default_create(World* w, int ind, char *str)
{
	Player						*pl = w->players[ind];
	RobotDefaultData	*my_data;

	if (!(my_data = (RobotDefaultData *)malloc(sizeof(*my_data)))) {
		error("no mem for default robot");
		w->EndGame();
		return;
	}

	my_data->robot_mode 	 = RM_TAKE_OFF;
	my_data->robot_count	 = 0;
	my_data->robot_lock 	 = LOCK_NONE;
	my_data->robot_lock_id	 = 0;
	my_data->longterm_mode	 = 0;

	if (str != NULL
		&& *str != '\0'
		&& sscanf(str, " %d %d", &my_data->attack, &my_data->defense) != 2) {
		if (str && *str) {
			xpprintf("%sinvalid parameters for default robot: \"%s\"\n", showtime(), str);
			my_data->attack = (int)(rfrac() * 99.5f);
			my_data->defense = 100 - my_data->attack;
		}
		LIMIT(my_data->attack, 1, 99);
		LIMIT(my_data->defense, 1, 99);
	}
	/*
	 * some parameters which may be changed to be dependent upon
	 * the `attack' and `defense' settings of this robot.
	 */
	if (BIT(w->rules->mode, TIMING)) {
		my_data->robot_normal_speed = 10.0;
		my_data->robot_attack_speed = 25.0 + (my_data->attack / 10);
		my_data->robot_max_speed = 50.0 + (my_data->attack / 20) - (my_data->defense / 50);
	} else {
		my_data->robot_normal_speed = 6.0;
		my_data->robot_attack_speed = 15.0 + (my_data->attack / 25);
		my_data->robot_max_speed = 30.0 + (my_data->attack / 50) - (my_data->defense / 50);
	}

	pl->fuel.l3 += my_data->defense - my_data->attack + (int)((rfrac() - 0.5f) * 20);
	pl->fuel.l2 += 2 * (my_data->defense - my_data->attack) / 5 + (int)((rfrac() - 0.5f) * 8);
	pl->fuel.l1 += (my_data->defense - my_data->attack) / 5 + (int)((rfrac() - 0.5f) * 4);

	my_data->last_used_ecm		= 0;
	my_data->last_dropped_mine	= 0;
	my_data->last_fired_missile = 0;
	my_data->last_thrown_ball	= 0;

	my_data->longterm_mode		= 0;

	pl->robot_data_ptr->private_data = (void *)my_data;
}

/*
 * A default robot is placed on its homebase.
 */
static void Robot_default_go_home(World* w, int ind)
{
	Player						*pl = w->players[ind];
	RobotDefaultData	*my_data = Robot_default_get_data(pl);

	my_data->robot_mode 	 = RM_TAKE_OFF;
	my_data->longterm_mode	 = 0;
}

/*
 * A default robot is declaring war (or resetting war).
 */
static void Robot_default_set_war(World* w, int ind, int victim_id)
{
	Player						*pl = w->players[ind];
	RobotDefaultData	*my_data = Robot_default_get_data(pl);

	if (victim_id == NO_ID) {
		CLR_BIT(my_data->robot_lock, LOCK_PLAYER);
	} else {
		my_data->robot_lock_id = victim_id;
		SET_BIT(my_data->robot_lock, LOCK_PLAYER);
	}
}

/*
 * Return the id of the player a default robot has war against (or NO_ID).
 */
static int Robot_default_war_on_player(World* w, int ind)
{
	Player						*pl = w->players[ind];
	RobotDefaultData	*my_data = Robot_default_get_data(pl);

	if (BIT(my_data->robot_lock, LOCK_PLAYER)) {
		return my_data->robot_lock_id;
	} else {
		return NO_ID;
	}
}

/*
 * A default robot receives a message.
 */
static void Robot_default_message(World* w, int ind, const char *message)
{
#if 0
	Player						*pl = w->players[ind];
	RobotDefaultData	*my_data = Robot_default_get_data(pl);
	int 						len;
	char						*ptr;
	char						sender_name[MAX_NAME_LEN];
	char						msg[MSG_LEN];

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

/*
 * A default robot is destroyed.
 */
static void Robot_default_destroy(World* w, int ind)
{
	Player						*pl = w->players[ind];

	free(pl->robot_data_ptr->private_data);
	pl->robot_data_ptr->private_data = NULL;
}

/*
 * A default robot is asked to join an alliance
 */
static void Robot_default_invite(World* w, int ind, int inv_ind)
{
	Player* 			pl = w->players[ind];
		Player* 		inviter = w->players[inv_ind];
	int 						war_id = Robot_default_war_on_player(w, ind);
	RobotDefaultData*	my_data = Robot_default_get_data(pl);
	int 						i;
	DFLOAT						limit;
	int 						accept = 1; 	/* accept by default */

	if (pl->alliance != ALLIANCE_NOT_SET) {
		/* if there is a human in our alliance, they should decide
		   let robots refuse in this case */
				for (i = 0; i < w->numPlayers; i++) {
						if (IS_HUMAN_IND(w, i) && ALLIANCE(w, ind, i)) {
								accept = 0;
								break;
						}
				}
				if (!accept) {
						pl->RefuseAlliance(inv_ind);
						return;
				}
	}
	limit = MAX(ABS(pl->score / MAX((my_data->attack / 10), 10)),
				my_data->defense);
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


/* XXX looks like this is no longer used.  why? */
#if 0
static bool Check_robot_navigate(int ind, bool * num_evade)
{
	int 		i, j, k;
	Player	   *pl = Players[ind];
	RobotDefaultData	*my_data = Robot_default_get_data(pl);
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

	if (pl->velocity > 2.0 || ABS(pl->vel.x) > 1.5)
		return false;

	for (i = 0; i < 10; i++) {
		for (j = 0; j < 10; j++) {
			area_val[i][j] = 0;
		}
	}

	found_wall = false;
	found_grav = false;

	for (i = 0; i < 10; i += 2) {
		for (j = 0; j < 10; j += 2) {

			dx = OBJ_X_IN_BLOCKS(pl) + (i / 2) - 2;
			dy = OBJ_Y_IN_BLOCKS(pl) + (j / 2) - 2;

			if (BIT(World.rules->mode, WRAP_PLAY)) {
				if (dx < 0) dx += World.x;
				else if (dx >= World.x) dx -= World.x;
				if (dy < 0) dy += World.y;
				else if (dy >= World.y) dy -= World.y;
			}
			if (dx < 0 || dx >= World.x || dy < 0 || dy >= World.y)
				locn_block = FILLED;
			else
				locn_block = World.block[dx][dy];

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
				k = World.itemID[dx][dy];
				if (World.wormHoles[k].type == WORM_OUT
					|| !wormholeVisible) {
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
				k = World.itemID[dx][dy];
				if (!targetTeamCollision
					&& BIT(World.rules->mode, TEAM_PLAY)
					&& World.targets[k].team == pl->team) {
					area_val[i][j] = 1;
					area_val[i + 1][j] = 1;
					area_val[i + 1][j + 1] = 1;
					area_val[i][j + 1] = 1;
				} else {
					found_wall = true;
				}
				break;

			case CANNON:
				k = World.itemID[dx][dy];
				if (BIT(World.rules->mode, TEAM_PLAY)
					&& teamImmunity
					&& World.cannon[k].team == pl->team) {
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

	*num_evade = true;

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

	if (area_val[1][1] >= best_val)
		return false;

	if (!near_wall) {
		if (BIT(pl->used, HAS_REFUEL)
			&& BIT(my_data->longterm_mode, NEED_FUEL)) {
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

	best_vx = (best_i - 1) * 0.75;
	best_vy = (best_j - 1) * 1.25;

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

	delta_dir = best_dir - pl->dir;
	delta_dir = MOD2(delta_dir, RES);

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

	if (pl->vel.y > best_vy + 0.25) {
		CLR_BIT(pl->status, THRUSTING);
	} else if (pl->vel.y < best_vy - 0.25) {
		SET_BIT(pl->status, THRUSTING);
	}
	return true;
}
#endif

static bool Really_empty_space(World* w, int ind, int x, int y)
{
	Player		*pl = w->players[ind];
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

static bool Check_robot_evade(World* w, int ind, int mine_i, int ship_i)
{
	int 						i;
	Player						*pl = w->players[ind];
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
	RobotDefaultData	*my_data = Robot_default_get_data(pl);

	safe_width = (my_data->defense / 200) * SHIP_SZ;
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
			if (!Really_empty_space(w, ind, dx, dy)) {
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
	if (pl->velocity > my_data->robot_max_speed)
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
			if (!Really_empty_space(w, ind, dx, dy)) {
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
			if (!Really_empty_space(w, ind, dx, dy)) {
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

	if (my_data->robot_mode != RM_EVADE_LEFT
		&& my_data->robot_mode != RM_EVADE_RIGHT) {
		if (left_ok && !right_ok)
			my_data->robot_mode = RM_EVADE_LEFT;
		else if (right_ok && !left_ok)
			my_data->robot_mode = RM_EVADE_RIGHT;
		else
			my_data->robot_mode = (delta_dir < RES / 2 ?
							  RM_EVADE_LEFT : RM_EVADE_RIGHT);
	}
	/*-BA If facing the way we want to go, thrust
	 *-BA If too far off, stop thrusting
	 *-BA If in between, keep doing whatever we are already doing
	 *-BA In all cases continue to straighten up
	 */
	if (delta_dir < RES / 4 || delta_dir > 3 * RES / 4) {
		pl->turnacc = (my_data->robot_mode == RM_EVADE_LEFT ?
					   pl->turnspeed : (-pl->turnspeed));
		CLR_BIT(pl->status, THRUSTING);
	}
	else if (delta_dir < 3 * RES / 8 || delta_dir > 5 * RES / 8) {
		pl->turnacc = (my_data->robot_mode == RM_EVADE_LEFT ?
					   pl->turnspeed : (-pl->turnspeed));
	} else {
		pl->turnacc = 0;
		SET_BIT(pl->status, THRUSTING);
		my_data->robot_mode = (delta_dir < RES/2 ? RM_EVADE_LEFT : RM_EVADE_RIGHT);
	}

	return true;
}

static void Robot_check_new_modifiers(Player *pl, modifiers mods)
{
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

static void Choose_weapon_modifier(Player* pl, int weapon_type)
{
	int 				stock, min;
	modifiers			mods;
	RobotDefaultData*	my_data = Robot_default_get_data(pl);
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
		if ((my_data->robot_count % 4) == 0)
			mods.laser = (int)(rfrac() * (MODS_LASER_MAX + 1));
		Robot_check_new_modifiers(pl, mods);
		return;

	case OBJ_SHOT:
		/*
		 * Robots usually use wide beam shots, however they may narrow
		 * the beam occasionally.
		 */
		mods.spread = 0;
		if ((my_data->robot_count % 4) == 0)
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
		if ((my_data->robot_count % 4) == 0)
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
		if ((my_data->robot_count % 100) <= my_data->attack) {
			SET_BIT(mods.nuclear, NUCLEAR);
			if (stock > min && (stock < (2 * min)
								|| (my_data->robot_count % 2) == 0))
					SET_BIT(mods.nuclear, FULLNUCLEAR);
		}
	}

	if (pl->fuel.sum > pl->fuel.l3) {
		if ((my_data->robot_count % 2) == 0) {
			if ((my_data->robot_count % 8) == 0)
				mods.velocity = (int)(rfrac() * MODS_VELOCITY_MAX) + 1;
			SET_BIT(mods.warhead, CLUSTER);
		}
	}
	else if ((my_data->robot_count % 3) == 0) {
		SET_BIT(mods.warhead, IMPLOSION);
	}

	/*
	 * Robot may change to use mini device setting occasionally.
	 */
	if ((my_data->robot_count % 10) == 0) {
		mods.mini = (int)(rfrac() * (MODS_MINI_MAX + 1));
		mods.spread = (int)(rfrac() * (MODS_SPREAD_MAX + 1));
	}

	Robot_check_new_modifiers(pl, mods);
}

static bool Check_robot_target(World* w, int ind,
							   int item_x, int item_y,
							   int new_mode)
{
	Player						*pl = w->players[ind],
								*ship;
	long						item_dist;
	int 						item_dir;
	int 						travel_dir;
	int 						delta_dir;
	long						dx, dy;
	long						dist;
	bool						clear_path;
	bool						slowing;
	RobotDefaultData	*my_data = Robot_default_get_data(pl);


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

	if (new_mode == RM_REFUEL)
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
		if (!Really_empty_space(w, ind, dx, dy)) {
			clear_path = false;
			continue;
		}
	}

	if (new_mode == RM_CANNON_KILL)
		item_dist -= 4 * BLOCK_SZ;

	if (!clear_path && new_mode != RM_NAVIGATE)
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

		if (new_mode == RM_HARVEST ||
			(new_mode == RM_NAVIGATE &&
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
			Choose_weapon_modifier(pl, OBJ_MINE);
			if (BIT(w->rules->mode, TIMING)) {
				pl->PlaceMine();
			} else {
				pl->PlaceMovingMine();
			}
			new_mode = (rfrac() < 0.5f) ? RM_EVADE_RIGHT : RM_EVADE_LEFT;
		}
	} else if (new_mode == RM_CANNON_KILL && item_dist <= 0) {

		/* too close, so move away */
		pl->turnspeed = MAX_PLAYER_TURNSPEED;
		item_dir = MOD2(item_dir + RES / 2, RES);
		slowing = true;
	} else {

		slowing = false;
	}
	if (new_mode == RM_NAVIGATE && !clear_path) {
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

	} else if (item_dist < 3*BLOCK_SZ && new_mode != RM_HARVEST) {

		if (pl->velocity < my_data->robot_normal_speed / 2)
			SET_BIT(pl->status, THRUSTING);
		if (pl->velocity > my_data->robot_normal_speed)
			CLR_BIT(pl->status, THRUSTING);

	} else if ((new_mode != RM_ATTACK
				&& new_mode != RM_NAVIGATE)
			|| item_dist < 8*BLOCK_SZ
			|| (new_mode == RM_NAVIGATE
				&& delta_dir > 3 * RES / 8
				&& delta_dir < 5 * RES / 8)) {

		if (pl->velocity < 2*my_data->robot_normal_speed)
			SET_BIT(pl->status, THRUSTING);
		if (pl->velocity > 3*my_data->robot_normal_speed)
			CLR_BIT(pl->status, THRUSTING);

	} else if (new_mode == RM_ATTACK
			|| (new_mode == RM_NAVIGATE
				&& (dist < 12 * BLOCK_SZ
					|| (delta_dir > RES / 8
						&& delta_dir < 7 * RES / 8)))) {

		if (pl->velocity < my_data->robot_attack_speed / 2)
			SET_BIT(pl->status, THRUSTING);
		if (pl->velocity > my_data->robot_attack_speed)
			CLR_BIT(pl->status, THRUSTING);
	} else if (clear_path
			&& (delta_dir < RES / 8
				|| delta_dir > 7 * RES / 8)
			&& item_dist > 18 * BLOCK_SZ) {
		if (pl->velocity < my_data->robot_max_speed - my_data->robot_normal_speed)
			SET_BIT(pl->status, THRUSTING);
		if (pl->velocity > my_data->robot_max_speed)
			CLR_BIT(pl->status, THRUSTING);
	} else {
		if (pl->velocity < my_data->robot_attack_speed)
			SET_BIT(pl->status, THRUSTING);
		if (pl->velocity > my_data->robot_max_speed - my_data->robot_normal_speed)
			CLR_BIT(pl->status, THRUSTING);
	}

	if (new_mode == RM_ATTACK
		|| (BIT(w->rules->mode, TIMING)
			&& new_mode == RM_NAVIGATE)) {
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
				 && new_mode == RM_ATTACK) {
			if (BIT(my_data->robot_lock, LOCK_PLAYER)
				&& BIT(w->players[w->getInd[my_data->robot_lock_id]]->status,
					   PLAYING|PAUSE|GAME_OVER) == PLAYING) {
				ship = w->players[w->getInd[my_data->robot_lock_id]];
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
				if (pl->velocity <= my_data->robot_normal_speed) {
					if (pl->lock.distance < (SHIP_SZ * 4)
						|| (!away && vel > my_data->robot_attack_speed)) {
						SET_BIT(pl->used, HAS_TRACTOR_BEAM);
						pl->tractor_is_pressor = true;
					} else if (away
							   && vel < my_data->robot_max_speed
							   && vel > my_data->robot_normal_speed) {
						SET_BIT(pl->used, HAS_TRACTOR_BEAM);
					}
				}
				if (BIT(pl->used, HAS_TRACTOR_BEAM))
					SET_BIT(pl->lock.tagged, LOCK_VISIBLE);
			}
		}
		if (BIT(pl->used, HAS_LASER)) {
			pl->turnacc = 0.0;
			Choose_weapon_modifier(pl, HAS_LASER);
		}
		/*-BA Be more agressive, esp if lots of ammo
		 * else if ((my_data->robot_count % 10) == 0 && pl->item[ITEM_MISSILE] > 0)
		 */
		else if ((my_data->robot_count % 10) < pl->item[ITEM_MISSILE]
				  && !WITHIN(my_data->robot_count,my_data->last_fired_missile,10)) {
			int type;

			switch (my_data->robot_count % 5) {
			case 0: case 1: case 2: 	type = OBJ_SMART_SHOT; break;
			case 3: 					type = OBJ_HEAT_SHOT; break;
			default:					type = OBJ_TORPEDO; break;
			}
			if (Detect_hunt(w, ind, w->getInd[pl->lock.pl_id])
				&& !pl->visibility[w->getInd[pl->lock.pl_id]].canSee)
				type = OBJ_HEAT_SHOT;
			if (type == OBJ_SMART_SHOT && !w->options.allowSmartMissiles->GetBool())
				type = OBJ_HEAT_SHOT;
			Choose_weapon_modifier(pl, type);
			pl->FireShot(type, pl->dir);
			if (type == OBJ_HEAT_SHOT)
				CLR_BIT(pl->status, THRUSTING);
			my_data->last_fired_missile=my_data->robot_count;
		}
		else if ((my_data->robot_count % 2) == 0
				   && item_dist < Visibility_distance
				   /*&& BIT(my_data->robot_lock, LOCK_PLAYER)*/){
			if ((int)(rfrac() * 64) < pl->item[ITEM_MISSILE] ) {
				Choose_weapon_modifier(pl, OBJ_SMART_SHOT);
				pl->FireShot(OBJ_SMART_SHOT, pl->dir);
				my_data->last_fired_missile=my_data->robot_count;
			} else {
				if ((new_mode == RM_ATTACK && clear_path)
					|| (my_data->robot_count % 50) == 0) {
					Choose_weapon_modifier(pl, OBJ_SHOT);
					pl->FireNormalShots();
				}
			}
		}
		/*-BA Be more agressive, esp if lots of ammo
		 * if ((my_data->robot_count % 32) == 0)
		 */
		else if ((my_data->robot_count % 32) < pl->item[ITEM_MINE]
				  && !WITHIN(my_data->robot_count, my_data->last_dropped_mine, 10)) {
			if (pl->fuel.sum > pl->fuel.l3) {
				Choose_weapon_modifier(pl, OBJ_MINE);
				pl->PlaceMine();
			} else /*if (pl->fuel.sum < pl->fuel.l2)*/ {
				pl->PlaceMine();
				CLR_BIT(pl->used, HAS_CLOAKING_DEVICE);
			}
			my_data->last_dropped_mine=my_data->robot_count;
		}
	}
	if (new_mode == RM_CANNON_KILL && !slowing) {
		if ((my_data->robot_count % 2) == 0
			&& item_dist < Visibility_distance
			&& clear_path) {
			Choose_weapon_modifier(pl, OBJ_SHOT);
			pl->FireNormalShots();
		}
	}
	my_data->robot_mode = new_mode;
	return true;
}


static bool Check_robot_hunt(World* w, int ind)
{
	Player						*pl = w->players[ind];
	Player						*ship;
	int 						ship_dir;
	int 						travel_dir;
	int 						delta_dir;
	int 						adj_dir;
	int 						toofast, tooslow;
	RobotDefaultData	*my_data = Robot_default_get_data(pl);

	if (!BIT(my_data->robot_lock, LOCK_PLAYER)
		|| my_data->robot_lock_id == pl->id)
		return false;
	if (pl->fuel.sum < pl->fuel.l3 /*MAX_PLAYER_FUEL/2*/)
		return false;
	if (!Detect_hunt(w, ind, w->getInd[my_data->robot_lock_id]))
		return false;

	ship = w->players[w->getInd[my_data->robot_lock_id]];

	ship_dir = (int)w->WrapFindDir(ship->pos.x - pl->pos.x, ship->pos.y - pl->pos.y);

	if (pl->velocity <= 0.2) {
		vector	*grav = &w->gravity
			[OBJ_X_IN_BLOCKS(pl)][OBJ_Y_IN_BLOCKS(pl)];
		travel_dir = (int)FindDir(grav->x, grav->y);
	} else {
		travel_dir = (int)FindDir(pl->vel.x, pl->vel.y);
	}

	delta_dir = MOD2(ship_dir - travel_dir, RES);
	tooslow = (pl->velocity < my_data->robot_attack_speed/2);
	toofast = (pl->velocity > my_data->robot_attack_speed);

	if (!tooslow && !toofast
		&& (delta_dir <= RES/16 || delta_dir >= 15*RES/16)) {

		pl->turnacc = 0;
		CLR_BIT(pl->status, THRUSTING);
		my_data->robot_mode = RM_ROBOT_IDLE;
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

	my_data->robot_mode = RM_ROBOT_IDLE;
	return true;
}

static bool Detect_hunt(World* w, int ind, int j)
{
	Player* 	pl = w->players[ind];
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
	if (sqr(dx) + sqr(dy) > sqr(Visibility_distance))
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

/*
 * Determine how important an item is to a given player.
 * Return one of the following 3 values:
 */
#define ROBOT_MUST_HAVE_ITEM	2		/* must have */
#define ROBOT_HANDY_ITEM		1		/* handy */
#define ROBOT_IGNORE_ITEM		0		/* ignore */
/*
 */
static int Rank_item_value(World* w, int ind, Items itemtype)
{
	Player		*pl = w->players[ind];

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

static bool Ball_handler(World* w, int ind)
{
	Player		*pl = w->players[ind];
	int 		i,
				closest_t = -1,
				closest_nt = -1,
				dist,
				closest_t_dist = INT_MAX,
				closest_nt_dist = INT_MAX,
				bdir,
				tdir;
	bool		clear_path = true;
	RobotDefaultData	*my_data = Robot_default_get_data(pl);

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
			my_data->last_thrown_ball = my_data->robot_count;
			CLR_BIT(my_data->longterm_mode, FETCH_TREASURE);
		} else {
			SET_BIT(my_data->longterm_mode, FETCH_TREASURE);
			return (Check_robot_target(w, ind,
						((int)(w->treasures[closest_t].pos.x + 0.5) * BLOCK_SZ),
						((int)(w->treasures[closest_t].pos.y + 0.5) * BLOCK_SZ),
						RM_NAVIGATE));
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
			&& closest_nt_dist < (my_data->robot_count / 10) * BLOCK_SZ) {
			SET_BIT(my_data->longterm_mode, FETCH_TREASURE);
			return (Check_robot_target(w, ind,
						((int)(w->treasures[closest_nt].pos.x + 0.5) * BLOCK_SZ),
						((int)(w->treasures[closest_nt].pos.y + 0.5) * BLOCK_SZ),
						RM_NAVIGATE));
		} else if (closest_ball_dist < (my_data->robot_count / 10) * BLOCK_SZ
				   && closest_ball_dist > w->options.ballConnectorLength->GetDouble()) {
			SET_BIT(my_data->longterm_mode, FETCH_TREASURE);
			return (Check_robot_target(w, ind, w->objs[closest_ball]->pos.x,
									   w->objs[closest_ball]->pos.y,
									   RM_NAVIGATE));
		}
	}
	return false;
}

static int Robot_default_play_check_map(World* w, int ind)
{
	Player						*pl = w->players[ind];
	int 						j;
	int 						cannon_i, fuel_i, target_i;
	int 						dx, dy;
	int 						distance, cannon_dist, fuel_dist, target_dist;
	bool						fuel_checked;
	RobotDefaultData	*my_data = Robot_default_get_data(pl);

	fuel_checked = false;

	cannon_i = -1;
	cannon_dist = (int)Visibility_distance;
	fuel_i = -1;
	fuel_dist = (int)Visibility_distance;
	target_i = -1;
	target_dist = (int)Visibility_distance;

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
		&& BIT(my_data->longterm_mode, NEED_FUEL)) {

		fuel_checked = true;
		dx = (int)w->fuels[fuel_i].pix_pos.x;
		dy = (int)w->fuels[fuel_i].pix_pos.y;

		SET_BIT(pl->used, HAS_REFUEL);
		pl->fs = fuel_i;

		if (Check_robot_target(w, ind, dx, dy, RM_REFUEL)) {
			return 1;
		}
	}
	if (target_i >= 0) {
		dx = (int)((w->targets[target_i].pos.x + 0.5) * BLOCK_SZ);
		dy = (int)((w->targets[target_i].pos.y + 0.5) * BLOCK_SZ);

		SET_BIT(my_data->longterm_mode, TARGET_KILL);
		if (Check_robot_target(w, ind, dx, dy, RM_CANNON_KILL)) {
			return 1;
		}
		CLR_BIT(my_data->longterm_mode, TARGET_KILL);
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

		if (Check_robot_target(w, ind, dx, dy, RM_CANNON_KILL)) {
			return 1;
		}
	}

	if (fuel_i >= 0
		&& !fuel_checked
		&& BIT(my_data->longterm_mode, NEED_FUEL)) {

		dx = (int)w->fuels[fuel_i].pix_pos.x;
		dy = (int)w->fuels[fuel_i].pix_pos.y;

		SET_BIT(pl->used, HAS_REFUEL);
		pl->fs = fuel_i;

		if (Check_robot_target(w, ind, dx, dy, RM_REFUEL)) {
			return 1;
		}
	}

	return 0;
}

static void Robot_default_play_check_objects(World* w, int ind,
											 int *item_i, int *item_dist,
											 int *item_imp,
											 int *mine_i, int *mine_dist)
{
	Player						*pl = w->players[ind];
	int 						j;
	Object						*shot, **obj_list;
	int 						distance, obj_count;
	int 						dx, dy;
	int 						shield_range;
	long						killing_shots;
	RobotDefaultData	*my_data = Robot_default_get_data(pl);

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
					 (int)(Visibility_distance / BLOCK_SZ), max_objs,
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
			&& !WITHIN(my_data->last_thrown_ball,
					   my_data->robot_count,
					   3 * w->GetFPS())) {
			SET_BIT(pl->used, HAS_CONNECTOR);
		}
		DRT()

		/* Ignore shots if shields already up - nothing else to do anyway */
		if (BIT(shot->type, OBJ_SHOT|OBJ_CANNON_SHOT)
				&& BIT(pl->used, HAS_SHIELD)) {
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
			if (BIT(shot->type, OBJ_ITEM))
			{
				if (ABS(dx) < *item_dist && ABS(dy) < *item_dist)
				{
					int imp;

					if (BIT(shot->status, RANDOM_ITEM))
					{
						imp = ROBOT_HANDY_ITEM; 		/* It doesn't know what it is, so get it if it can */
					} else {
						imp = Rank_item_value(w, ind, (Items)obj_list[j]->info);
					}
					if (imp > ROBOT_IGNORE_ITEM && imp >= *item_imp)
					{
						*item_imp = imp;
						*item_dist = (int) LENGTH(dx, dy);
						*item_i = j;
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
			&& (int)(rfrac() * 100) <
			   (85 + (my_data->defense / 7) - (my_data->attack / 50))) {
			SET_BIT(pl->used, HAS_SHIELD);
			if (!w->options.cloakedShield->GetBool())
				CLR_BIT(pl->used, HAS_CLOAKING_DEVICE);
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
	/* Convert *item_i from index in local obj_list[] to index in objs[] */
	if (*item_i >= 0) {
			for (j=0; (j < w->numObjs) && (w->objs[j]->id != obj_list[*item_i]->id); j++)
					;
			if (j >= w->numObjs) {
					*item_i = -1;			/* Perhaps an error should be printed, too? */
			} else {
					*item_i = j;
			}
	}
}

static void Robot_default_play_check_lasers(World* w, int ind)
{
	Player						*pl = w->players[ind];
	int 						j;
	int 						dx, dy;
	int 						distance2;
	int 						shield_range;
	RobotDefaultData	*my_data = Robot_default_get_data(pl);

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
				   (85 + (my_data->defense / 7) - (my_data->attack / 50))) {
				SET_BIT(pl->used, HAS_SHIELD);
				if (!w->options.cloakedShield->GetBool())
					CLR_BIT(pl->used, HAS_CLOAKING_DEVICE);
				break;
			}
		}
	}
}

static void Robot_default_play(World* w, int ind)
{
	Player						*pl = w->players[ind],
								*ship;
	DFLOAT						distance, ship_dist,
								enemy_dist,
								speed, x_speed, y_speed;
	int 						item_dist, mine_dist;
	int 						item_i, mine_i;
	int 						j, ship_i, item_imp,
								enemy_i;
	int 						dx, dy, x, y;
	bool						harvest_checked;
	bool						evade_checked;
	bool						navigate_checked;
	int 						shoot_time;
	RobotDefaultData	*my_data = Robot_default_get_data(pl);

	DRT()

	if (my_data->robot_count <= 0)
		my_data->robot_count = 1000 + (int)(rfrac() * 32);

	my_data->robot_count--;

	CLR_BIT(pl->used, HAS_SHOT | HAS_SHIELD | HAS_CLOAKING_DEVICE | HAS_LASER);
	if (BIT(pl->have, HAS_EMERGENCY_SHIELD)
		&& !BIT(pl->used, HAS_EMERGENCY_SHIELD)) {
		pl->EmergencyShield(true);
	}
	harvest_checked = false;
	evade_checked = false;
	navigate_checked = false;

	mine_i = -1;
	mine_dist = SHIP_SZ + 200;
	item_i = -1;
	item_dist = (int) Visibility_distance;
	item_imp = ROBOT_IGNORE_ITEM;

	if (BIT(pl->have, HAS_CLOAKING_DEVICE) && pl->fuel.sum > pl->fuel.l2)
		SET_BIT(pl->used, HAS_CLOAKING_DEVICE);

	if (BIT(pl->have, HAS_EMERGENCY_THRUST)
		&& !BIT(pl->used, HAS_EMERGENCY_THRUST)) {
		pl->EmergencyThrust(true);
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
		SET_BIT(pl->used, HAS_SHIELD);
		if (!w->options.cloakedShield->GetBool())
			CLR_BIT(pl->used, HAS_CLOAKING_DEVICE);
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
		SET_BIT(my_data->longterm_mode, NEED_FUEL);
	} else if (!BIT(pl->used, HAS_REFUEL)) {
		CLR_BIT(my_data->longterm_mode, NEED_FUEL);
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

	Robot_default_play_check_objects(w, ind,

									 &item_i, &item_dist, &item_imp,
									 &mine_i, &mine_dist);
	DRT()
	Robot_default_play_check_lasers(w, ind);
	DRT()


	/* Note: Only take time to navigate if not being shot at */
	/* KK: it seems that this 'Check_robot_navigate' function caused
		the infamous 'robot stuck under wall' bug, so I commented it out */
	/* KK: ps. I tried to change that function, but I don't grok it */
	/*if (!(BIT(pl->used, HAS_SHIELD) && BIT(pl->status, THRUSTING))
		&& Check_robot_navigate(ind, &evade_checked)) {
		if (playerShielding == 0
			&& playerStartsShielded != 0
			&& BIT(pl->have, HAS_SHIELD)) {
			SET_BIT(pl->used, HAS_SHIELD);
		}
		return;
	}*/
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
						  Visibility_distance)
					  : Max_enemy_distance);
	}
	else {
		enemy_dist = Visibility_distance;
	}

	DRT()

	if (BIT(pl->used, HAS_SHIELD))
		ship_dist = 0;

	if (BIT(my_data->robot_lock, LOCK_PLAYER)) {
		j = w->getInd[my_data->robot_lock_id];
		ship = w->players[j];

		if (BIT(w->players[w->getInd[my_data->robot_lock_id]]->status,
				PLAYING|GAME_OVER|PAUSE) == PLAYING) {

			if (Detect_hunt(w, ind, j)) {

				if (BIT(my_data->robot_lock, LOCK_PLAYER)
					&& my_data->robot_lock_id == ship->id) {
					my_data->lock_last_seen = my_data->robot_count;
					my_data->lock_last_pos.x = ship->pos.x;
					my_data->lock_last_pos.y = ship->pos.y;
				}

				dx = ship->pos.x - pl->pos.x, dx = WRAP_DX(w, dx);
				dy = ship->pos.y - pl->pos.y, dy = WRAP_DY(w, dy);
				distance = LENGTH(dx, dy);

				if (distance < ship_dist) {
					ship_i = w->getInd[my_data->robot_lock_id];
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
			if (j == ind
				|| BIT(ship->status, PLAYING|GAME_OVER|PAUSE) != PLAYING
				|| pl->TeamImmune(ship->id))
				continue;

			if (!Detect_hunt(w, ind, j))
				continue;

			dx = ship->pos.x - pl->pos.x, dx = WRAP_DX(w, dx);
			dy = ship->pos.y - pl->pos.y, dy = WRAP_DY(w, dy);
			distance = LENGTH(dx, dy);

			if (distance < ship_dist) {
				ship_i = j;
				ship_dist = distance;
			}

			if (!BIT(my_data->robot_lock, LOCK_PLAYER)) {
				if ((my_data->robot_count % 3) == 0
					&& ((my_data->robot_count % 100) < my_data->attack)
					&& distance < enemy_dist) {
					enemy_i    = j;
					enemy_dist = distance;
				}
			}
		}
	}

	DRT()

	if (ship_dist < 3*SHIP_SZ && BIT(pl->have, HAS_SHIELD)) {
		SET_BIT(pl->used, HAS_SHIELD);
		if (!w->options.cloakedShield->GetBool()) {
		   CLR_BIT(pl->used, HAS_CLOAKING_DEVICE);
		}
	}

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
		&& BIT(my_data->robot_lock, LOCK_PLAYER)
		&& my_data->robot_lock_id == w->players[ship_i]->id) {
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
			|| (BIT(my_data->robot_lock, LOCK_PLAYER)
				&& my_data->robot_lock_id != pl->lock.pl_id
				&& BIT(w->players[w->getInd[my_data->robot_lock_id]]->status,
					   PLAYING|PAUSE|GAME_OVER) == PLAYING)
			|| !Detect_hunt(w, ind, w->getInd[ship->id])
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
		if (Check_robot_evade(w, ind, mine_i, ship_i)) {
			if (w->options.allowShields->GetBool() == 0
				&& w->options.playerStartsShielded->GetBool() != 0
				&& BIT(pl->have, HAS_SHIELD)) {
				SET_BIT(pl->used, HAS_SHIELD);
				if (!w->options.cloakedShield->GetBool())
					CLR_BIT(pl->used, HAS_CLOAKING_DEVICE);
			}
			else if (w->options.maxShieldedWallBounceSpeed->GetDouble() >
					w->options.maxUnshieldedWallBounceSpeed->GetDouble()
				&& w->options.maxShieldedPlayerWallBounceAngle->GetDouble() >=
					w->options.maxUnshieldedPlayerWallBounceAngle->GetDouble()
				&& BIT(pl->have, HAS_SHIELD)) {
				SET_BIT(pl->used, HAS_SHIELD);
				if (!w->options.cloakedShield->GetBool())
					CLR_BIT(pl->used, HAS_CLOAKING_DEVICE);
			}
			return;
		}
	}
	DRT()

	if (BIT(w->rules->mode, TIMING) && !navigate_checked) {
		int delta_dir;
		if (item_i >= 0) {
			delta_dir = (int)(pl->dir - w->WrapFindDir(w->objs[item_i]->pos.x - pl->pos.x,
											   w->objs[item_i]->pos.y - pl->pos.y));
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
			if (Check_robot_target(w, ind,
								   w->check[pl->check].x * BLOCK_SZ,
								   w->check[pl->check].y * BLOCK_SZ,
								   RM_NAVIGATE)) {
				return;
			}
		}
	}
	DRT()

	if (item_i >= 0
		&& 3*enemy_dist > 2*item_dist
		&& item_dist < 12*BLOCK_SZ
		&& !BIT(my_data->longterm_mode, FETCH_TREASURE)
		&& (!BIT(my_data->longterm_mode, NEED_FUEL)
			|| w->objs[item_i]->info == ITEM_FUEL
			|| w->objs[item_i]->info == ITEM_TANK)) {

		if (item_imp != ROBOT_IGNORE_ITEM) {
			harvest_checked = true;
			dx = w->objs[item_i]->pos.x;
			dx += (long)(w->objs[item_i]->vel.x * (ABS(dx - pl->pos.x) /
										my_data->robot_normal_speed));
			dy = w->objs[item_i]->pos.y;
			dy += (long)(w->objs[item_i]->vel.y * (ABS(dy - pl->pos.y) /
										my_data->robot_normal_speed));

			if (Check_robot_target(w, ind, dx, dy, RM_HARVEST)) {
				return;
			}
		}
	}
	if (BIT(pl->lock.tagged, LOCK_PLAYER) &&
		Detect_hunt(w, ind, w->getInd[pl->lock.pl_id])) {

		ship = w->players[w->getInd[pl->lock.pl_id]];
		shoot_time = (int)(pl->lock.distance / (w->options.shotSpeed->GetDouble() + 1));
		dx = (long)(ship->pos.x + ship->vel.x * shoot_time);
		dy = (long)(ship->pos.y + ship->vel.y * shoot_time);
		/*-BA Also allow for our own momentum. */
		dx -= (long)(pl->vel.x * shoot_time);
		dy -= (long)(pl->vel.y * shoot_time);

		if (Check_robot_target(w, ind, dx, dy, RM_ATTACK)
			&& !BIT(my_data->longterm_mode, FETCH_TREASURE
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
		&& !BIT(my_data->longterm_mode, TARGET_KILL|NEED_FUEL)) {
		navigate_checked = true;
		if (Ball_handler(w, ind))
			return;
	}
	DRT()

	if (item_i >= 0
		&& !harvest_checked
		&& item_dist < 12*BLOCK_SZ) {

		if (item_imp != ROBOT_IGNORE_ITEM) {
			dx = w->objs[item_i]->pos.x;
			dx += (long)(w->objs[item_i]->vel.x * (ABS(dx - pl->pos.x) /
										my_data->robot_normal_speed));
			dy = w->objs[item_i]->pos.y;
			dy += (long)(w->objs[item_i]->vel.y * (ABS(dy - pl->pos.y) /
										my_data->robot_normal_speed));

			if (Check_robot_target(w, ind, dx, dy, RM_HARVEST)) {
				return;
			}
		}
	}

	if (Check_robot_hunt(w, ind)) {
		if (w->options.allowShields->GetBool()== 0
			&& w->options.playerStartsShielded->GetBool() != 0
			&& BIT(pl->have, HAS_SHIELD)) {
			SET_BIT(pl->used, HAS_SHIELD);
			if (!w->options.cloakedShield->GetBool())
				CLR_BIT(pl->used, HAS_CLOAKING_DEVICE);
		}
		return;
	}

	DRT()

	if (Robot_default_play_check_map(w, ind) == 1) {
		return;
	}

	DRT()

	if (w->options.allowShields->GetBool() == 0
		&& w->options.playerStartsShielded->GetBool() != 0
		&& BIT(pl->have, HAS_SHIELD)) {
		SET_BIT(pl->used, HAS_SHIELD);
		if (!w->options.cloakedShield->GetBool())
			CLR_BIT(pl->used, HAS_CLOAKING_DEVICE);
	}

	x = OBJ_X_IN_BLOCKS(pl);
	y = OBJ_Y_IN_BLOCKS(pl);
	x_speed = pl->vel.x - 2 * w->gravity[x][y].x;
	y_speed = pl->vel.y - 2 * w->gravity[x][y].y;

	if (y_speed < (-my_data->robot_normal_speed) || (my_data->robot_count % 64) < 32) {

		my_data->robot_mode = RM_ROBOT_CLIMB;
		pl->turnspeed = MAX_PLAYER_TURNSPEED / 2;
		pl->power = MAX_PLAYER_POWER / 2;
		if (ABS(pl->dir - RES / 4) > RES / 16) {
			pl->turnacc = (pl->dir < RES / 4
						   || pl->dir >= 3 * RES / 4
						   ? pl->turnspeed : (-pl->turnspeed));
		} else {
			pl->turnacc = 0.0;
		}
		if (y_speed < my_data->robot_normal_speed / 2
			&& pl->velocity < my_data->robot_attack_speed)
			SET_BIT(pl->status, THRUSTING);
		else if (y_speed > my_data->robot_normal_speed)
			CLR_BIT(pl->status, THRUSTING);
		return;
	}
	my_data->robot_mode = RM_ROBOT_IDLE;
	pl->turnspeed = MAX_PLAYER_TURNSPEED / 2;
	pl->turnacc = 0;
	pl->power = MAX_PLAYER_POWER / 2;
	CLR_BIT(pl->status, THRUSTING);
	speed = LENGTH(x_speed, y_speed);
	if (speed < my_data->robot_normal_speed / 2)
		SET_BIT(pl->status, THRUSTING);
	else if (speed > my_data->robot_normal_speed)
		CLR_BIT(pl->status, THRUSTING);
	DRT()

}


/*
 * This is called each round.
 * It allows us to adjust our file local parameters.
 */
static void Robot_default_round_tick(World* w)
{
	DFLOAT				min_visibility = 256.0;
	DFLOAT				min_enemy_distance = 512.0;

	/* reduce visibility when there are a lot of robots. */
	Visibility_distance = min_visibility
		+ (((VISIBILITY_DISTANCE - min_visibility)
			* (NUM_IDS - w->numRobots)) / NUM_IDS);

	/* limit distance to allowable enemies. */
	Max_enemy_distance = w->hypotenuse;
	if (w->hypotenuse > Visibility_distance) {
		Max_enemy_distance = min_enemy_distance
			+ (((w->hypotenuse - min_enemy_distance)
				* (NUM_IDS - w->numRobots)) / NUM_IDS);
	}
}

