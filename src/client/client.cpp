/* $Id: client.cpp,v 1.30 2007/01/18 21:08:21 dick Exp $
 *
 * client - map stuff, radar stuff, network stuff, misc stuff, globals.
 *
 * client - the user interface to the game.
 *
 * XPilot, a multiplayer gravity war game.	Copyright (C) 1991-2001 by
 *
 *		Bjørn Stabell		 <bjoern@xpilot.org>
 *		Ken Ronny Schouten	 <ken@xpilot.org>
 *		Bert Gijsbers		 <bert@xpilot.org>
 *		Dick Balaska		 <dick@xpilot.org>
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
 * $Log: client.cpp,v $
 * Revision 1.30  2007/01/18 21:08:21  dick
 * Disable some debug
 *
 * Revision 1.29  2007/01/17 21:35:15  dick
 * Encapsulate all of the RobotWatch features into a RobotWatchMan object.
 *
 * Revision 1.28  2007/01/17 08:59:49  dick
 * RobotWatch is a list of Strings sent from the client when a player is paused
 * and watching a robot.  This list contains diagnostic information about
 * what the heck the robot thinks it's doing.
 * It's kinda like the Terminator view where he's looking at a 6502 dump.
 *
 * Revision 1.27  2006/09/24 04:54:11  dick
 * scoresChanged is an int
 *
 * Revision 1.26  2004/06/03 06:04:42  dick
 * struct other_t becomes class Other.
 * array Others becomes ObjList others.
 * shipobj becomes ShipObj.
 *
 * Revision 1.25  2004/06/01 18:20:40  dick
 * Set alliance for new Other to 0
 *
 * Revision 1.24  2004/05/20 22:23:42  dick
 * Add AddMessage(PCSTR, ...).  Wrap some debug with a D().
 *
 * Revision 1.23  2004/05/11 07:14:11  dick
 * Wrap some debug with a D()
 *
 * Revision 1.22  2004/05/07 04:45:04  dick
 * Display rank data in the ScoreTable.
 *
 * Revision 1.21  2004/04/29 16:08:26  dick
 * compile with SOUND enabled.
 *
 * Revision 1.20  2004/04/26 07:05:43  dick
 * Handle the new ScoreTable page types.
 *
 * Revision 1.19  2004/02/03 03:39:52  dick
 * Add Windows precompiled header support.
 *
 * Revision 1.18  2004/01/20 15:14:26  dick
 * Get rid of #define WINDOWSCALING.  Window scaling is always built, it's not an option.
 *
 * Revision 1.17  2004/01/16 01:35:07  dick
 * Turn off the "Eyes" indicator on the client when a player unpauses.
 *
 * Revision 1.16  2002/09/16 22:17:22  dick
 * Need cstring.h
 *
 * Revision 1.15  2002/09/02 21:06:06  dick
 * Encapsulate dpy and kdpy.  Singleton ini becomes iniClient.
 *
 * Revision 1.14  2002/09/02 15:42:40  dick
 * Singleton ini becomes iniClient.
 *
 * Revision 1.13  2002/08/15 06:15:51  dick
 * Retab
 *
 * Revision 1.12  2002/08/01 15:01:27  dick
 * Send the cookie with the verify message to the server. Retab.
 *
 * Revision 1.11  2002/07/16 08:51:47  dick
 * Fix showScoreDecimals
 *
 * Revision 1.10  2002/06/26 06:47:02  dick
 * Replace errno with xperrno and errno=0 with seterrno(0).
 *
 * Revision 1.9  2002/06/03 01:26:17  dick
 * Fix bug in showScoreDecimals
 *
 * Revision 1.8  2002/05/18 20:55:33  dick
 * Update to XPilot-4.5.1
 *
 * Revision 1.7  2001/09/17 08:32:50  dick
 * Items has moved to common
 *
 * Revision 1.6  2001/09/11 09:08:30  dick
 * Encapsulate all client resources into new class Ini (Linux cleanups)
 *
 * Revision 1.5  2001/09/11 07:33:44  dick
 * Encapsulate all client resources into new class Ini.
 *
 * Revision 1.4  2001/08/14 21:51:35  dick
 * New functionality !anonymousViewing.  When you are viewing someone, everyone
 * gets notified.  This appears in the score table as if you have declared war
 * on the player.  Older clients actually SendWar().  New clients reuse PKT_EYES
 * which was previously defined as a server SendEyes() and client HandleEyes(),
 * neither of which were actually ever used.
 *
 * Revision 1.3  2001/08/07 20:59:34  dick
 * Set theWorld.fps to match maxFPS
 *
 * Revision 1.2  2001/07/27 05:47:38  dick
 * Client builds and runs in C++
 *
 */
#include "StdAfx.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <math.h>
#include <time.h>
#include <sys/types.h>

#ifndef _WINDOWS
# include <unistd.h>
# ifndef __hpux
#  include <sys/time.h>
# endif
# include <X11/Xlib.h>
#endif

#ifdef _WINDOWS
# include "NT/winClient.h"
#endif

#include "cstring.h"
#include "version.h"
#include "config.h"
#include "const.h"
#include "setup.h"
#include "error.h"
#include "rules.h"
#include "bit.h"
#include "netclient.h"
#include "paint.h"
#include "xinit.h"
#include "Other.h"
#include "protoclient.h"
#include "portability.h"
#include "talk.h"
#include "commonproto.h"
#include "ClientWorld.h"
#include "Connectparam.h"
#include "Ini.h"
#include "password.h"
#include "ScoreTable.h"
#include "RobotWatchMan.h"

char client_version[] = VERSION;

#define MAX_CHECKPOINT	26

int 	scoresChanged = 0;
int 	RadarHeight = 0;
int 	RadarWidth = 256;		/* must always be 256! */

ipos	pos;
ipos	vel;
ipos	world;
ipos	realWorld;
short	heading;
short	nextCheckPoint;

u_byte	numItems[NUM_ITEMS];	/* Count of currently owned items */
u_byte	lastNumItems[NUM_ITEMS];/* Last item count shown */
int 	numItemsTime[NUM_ITEMS];/* Number of frames to show this item count */
//DFLOAT		showItemsTime;			/* How long to show changed item count for */

short	autopilotLight;

short	lock_id;				/* Id of player locked onto */
short	lock_dir;				/* Direction of lock */
short	lock_dist;				/* Distance to player locked onto */

short	selfVisible;			/* Are we alive and playing? */
short	damaged;				/* Damaged by ECM */
short	destruct;				/* If self destructing */
short	shutdown_delay;
short	shutdown_count;
short	thrusttime;
short	thrusttimemax;
short	shieldtime;
short	shieldtimemax;
short	phasingtime;
short	phasingtimemax;

int 	roundDelay; 					/* != 0 means we're in a delay */
int 	roundDelayMax;			/* (not yet) used for graph of time remaining in delay */

long	control_count;			/* Display control for how long? */
//u_byte		spark_rand; 			/* Sparkling effect */
u_byte	old_spark_rand; 		/* previous value of spark_rand */

long	fuelSum;						/* Sum of fuel in all tanks */
long	fuelMax;						/* How much fuel can you take? */
short	fuelCurrent;					/* Number of currently used tank */
short	numTanks;						/* Number of tanks */
long	fuelCount;						/* Display fuel for how long? */
//int	fuelLevel1; 					/* Fuel critical level */
//int	fuelLevel2; 					/* Fuel warning level */
//int	fuelLevel3; 					/* Fuel notify level */

//char	*shipShape; 			/* Shape of player's ship */
//DFLOAT		power;					/* Force of thrust */
//DFLOAT		power_s;				/* Saved power fiks */
//DFLOAT		turnspeed;				/* How fast player acc-turns */
//DFLOAT		turnspeed_s;			/* Saved turnspeed */
//DFLOAT		turnresistance; 		/* How much is lost in % */
//DFLOAT		turnresistance_s;		/* Saved (see above) */
DFLOAT	displayedPower; 		/* What the server is sending us */
DFLOAT	displayedTurnspeed; 	/* What the server is sending us */
DFLOAT	displayedTurnresistance;/* What the server is sending us */
//DFLOAT		spark_prob; 			/* Sparkling effect user configurable */
int 	charsPerSecond; 		/* Message output speed (configurable) */

//DFLOAT		hud_move_fact;			/* scale the hud-movement (speed) */
//DFLOAT		ptr_move_fact;			/* scale the speed pointer length */
//long	instruments;			/* Instruments on screen (bitmask) */
char	mods[MAX_CHARS];		/* Current modifiers in effect */
int 			packet_size;			/* Current frame update packet size */
int 			packet_loss;			/* lost packets per second */
int 			packet_drop;			/* dropped packets per second */
int 			packet_lag; 					/* approximate lag in frames */
char*	packet_measure; 		/* packet measurement in a second */
long	packet_loop;			/* start of measurement */

uint	scoreTablePages = STScoreB | STNameB;
//bool	showRealName = false;	/* Show realname instead of nick name */
ScoreTableType	scoreTableType = STScore;

char	servername[MAX_CHARS];	/* Name of server connecting to */
unsigned		version;		/* Version of the server */
int 	shields = 1;			/* When shields are considered up */

int 	oldMaxFPS;

u_byte	lose_item;				/* index for dropping owned item */
int 	lose_item_active;		/* one of the lose keys is pressed */

static ObjList	others;

static	DFLOAT	teamscores[MAX_TEAMS];
static fuelstation_t	*fuels = 0;
static int				num_fuels = 0;

static homebase_t		*bases = 0;
static int				num_bases = 0;

static cannontime_t 	*cannons = 0;
static int				num_cannons = 0;

static target_t 		*targets = 0;
static int				num_targets = 0;

static checkpoint_t 	checks[MAX_CHECKPOINT];

score_object_t			score_objects[MAX_SCORE_OBJECTS];
int 					score_object = 0;

RobotWatchMan	robotWatchMan;		// Watch robot's brain activities

///////////////////////////////////////////////////////////////////////////////
#ifndef  _WINDOWS
/* provide cut&paste and message history */
extern	selection_t 	selection;
static	char			*HistoryBlock = NULL;
extern	char			*HistoryMsg[MAX_HIST_MSGS];
#endif

///////////////////////////////////////////////////////////////////////////////
static fuelstation_t *Fuelstation_by_pos(int x, int y)
{
	int 				i, lo, hi, pos;

	lo = 0;
	hi = num_fuels - 1;
	pos = x * Setup->y + y;
	while (lo < hi) {
		i = (lo + hi) >> 1;
		if (pos > fuels[i].pos) {
			lo = i + 1;
		} else {
			hi = i;
		}
	}
	if (lo == hi && pos == fuels[lo].pos) {
		return &fuels[lo];
	}
		seterrno(0);
	error("No fuelstation at (%d,%d)", x, y);
	return NULL;
}

///////////////////////////////////////////////////////////////////////////////
int Fuel_by_pos(int x, int y)
{
	fuelstation_t		*fuelp;

	if ((fuelp = Fuelstation_by_pos(x, y)) == NULL) {
		return 0;
	}
	return fuelp->fuel;
}

///////////////////////////////////////////////////////////////////////////////
int Target_by_index(int ind, int *xp, int *yp, int *dead_time, int *damage)
{
	if (ind < 0 || ind >= num_targets) {
		return -1;
	}
	*xp = targets[ind].pos / Setup->y;
	*yp = targets[ind].pos % Setup->y;
	*dead_time = targets[ind].dead_time;
	*damage = targets[ind].damage;
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
int Target_alive(int x, int y, int *damage)
{
	int 				i, lo, hi, pos;

	lo = 0;
	hi = num_targets - 1;
	pos = x * Setup->y + y;
	while (lo < hi) {
		i = (lo + hi) >> 1;
		if (pos > targets[i].pos) {
			lo = i + 1;
		} else {
			hi = i;
		}
	}
	if (lo == hi && pos == targets[lo].pos) {
		*damage = targets[lo].damage;
		return targets[lo].dead_time;
	}
		seterrno(0);
	error("No targets at (%d,%d)", x, y);
	return -1;
}

///////////////////////////////////////////////////////////////////////////////
int Handle_fuel(int ind, int fuel)
{
	if (ind < 0 || ind >= num_fuels) {
				seterrno(0);
				error("Bad fuelstation index (%d)", ind);
				return -1;
	}
	fuels[ind].fuel = fuel;
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
static cannontime_t *Cannon_by_pos(int x, int y)
{
	int 				i, lo, hi, pos;

	lo = 0;
	hi = num_cannons - 1;
	pos = x * Setup->y + y;
	while (lo < hi) {
		i = (lo + hi) >> 1;
		if (pos > cannons[i].pos) {
			lo = i + 1;
		} else {
			hi = i;
		}
	}
	if (lo == hi && pos == cannons[lo].pos) {
		return &cannons[lo];
	}
		seterrno(0);
	error("No cannon at (%d,%d)", x, y);
	return NULL;
}

///////////////////////////////////////////////////////////////////////////////
int Cannon_dead_time_by_pos(int x, int y, int *dot)
{
	cannontime_t		*cannonp;

	if ((cannonp = Cannon_by_pos(x, y)) == NULL) {
		return -1;
	}
	*dot = cannonp->dot;
	return cannonp->dead_time;
}

///////////////////////////////////////////////////////////////////////////////
int Handle_cannon(int ind, int dead_time)
{
	if (ind < 0 || ind >= num_cannons) {
		seterrno(0);
		error("Bad cannon index (%d)", ind);
		return 0;
	}
	cannons[ind].dead_time = dead_time;
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
int Handle_target(int num, int dead_time, int damage)
{
	if (num < 0 || num >= num_targets) {
				seterrno(0);
				error("Bad target index (%d)", num);
				return 0;
	}
	if (dead_time == 0
		&& (damage < 1
		|| damage > TARGET_DAMAGE)) {
		printf ("BUG target %d, dead %d, damage %d\n", num, dead_time, damage);
	}
	if (targets[num].dead_time > 0 && dead_time == 0) {
		int pos = targets[num].pos;
		Paint_radar_block(pos / Setup->y, pos % Setup->y, iniClient.targetRadarColor);
	}
	else if (targets[num].dead_time == 0 && dead_time > 0) {
		int pos = targets[num].pos;
		Paint_radar_block(pos / Setup->y, pos % Setup->y, BLACK);
	}

	targets[num].dead_time = dead_time;
	targets[num].damage = damage;

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
static homebase_t *Homebase_by_pos(int x, int y)
{
	int 				i, lo, hi, pos;

	lo = 0;
	hi = num_bases - 1;
	pos = x * Setup->y + y;
	while (lo < hi) {
		i = (lo + hi) >> 1;
		if (pos > bases[i].pos) {
			lo = i + 1;
		} else {
			hi = i;
		}
	}
	if (lo == hi && pos == bases[lo].pos) {
		return &bases[lo];
	}
		seterrno(0);
	error("No homebase at (%d,%d)", x, y);
	return NULL;
}

///////////////////////////////////////////////////////////////////////////////
int Base_info_by_pos(int x, int y, int *idp, int *teamp)
{
	homebase_t	*basep;

	if ((basep = Homebase_by_pos(x, y)) == NULL) {
		return -1;
	}
	*idp = basep->id;
	*teamp = basep->team;
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
int Handle_base(int id, int ind)
{
	int 		i;

	if (ind < 0 || ind >= num_bases) {
				seterrno(0);
				error("Bad homebase index (%d)", ind);
				return -1;
	}
	for (i = 0; i < num_bases; i++) {
		if (bases[i].id == id) {
			bases[i].id = -1;
		}
	}
	bases[ind].id = id;

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
int Check_pos_by_index(int ind, int *xp, int *yp)
{
	if (ind < 0 || ind >= MAX_CHECKPOINT) {
				seterrno(0);
				error("Bad checkpoint index (%d)", ind);
				*xp = 0;
				*yp = 0;
				return -1;
	}
	*xp = checks[ind].pos / Setup->y;
	*yp = checks[ind].pos % Setup->y;
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
int Check_index_by_pos(int x, int y)
{
	int 				i, pos;

	pos = x * Setup->y + y;
	for (i = 0; i < MAX_CHECKPOINT; i++) {
		if (pos == checks[i].pos) {
			return i;
		}
	}
		seterrno(0);
	error("Can't find checkpoint (%d,%d)", x, y);
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
/*
 * Convert a `space' map block into a dot.
 */
static void Map_make_dot(unsigned char *data)
{
	if (*data == SETUP_SPACE) {
		*data = SETUP_SPACE_DOT;
	}
	else if (*data == SETUP_DECOR_FILLED) {
		*data = SETUP_DECOR_DOT_FILLED;
	}
	else if (*data == SETUP_DECOR_RU) {
		*data = SETUP_DECOR_DOT_RU;
	}
	else if (*data == SETUP_DECOR_RD) {
		*data = SETUP_DECOR_DOT_RD;
	}
	else if (*data == SETUP_DECOR_LU) {
		*data = SETUP_DECOR_DOT_LU;
	}
	else if (*data == SETUP_DECOR_LD) {
		*data = SETUP_DECOR_DOT_LD;
	}
}

///////////////////////////////////////////////////////////////////////////////
/*
 * Optimize the drawing of all blue space dots by converting
 * certain map objects into a specialised form of their type.
 */
void Map_dots(void)
{
	int 				i,
						x,
						y,
						start;
	unsigned char		dot[256];

	/*
	 * Lookup table to recognize dots.
	 */
	memset(dot, 0, sizeof dot);
	dot[SETUP_SPACE_DOT] = 1;
	dot[SETUP_DECOR_DOT_FILLED] = 1;
	dot[SETUP_DECOR_DOT_RU] = 1;
	dot[SETUP_DECOR_DOT_RD] = 1;
	dot[SETUP_DECOR_DOT_LU] = 1;
	dot[SETUP_DECOR_DOT_LD] = 1;

	/*
	 * Restore the map to unoptimized form.
	 */
	for (i = Setup->x * Setup->y; i-- > 0; ) {
		if (dot[Setup->map_data[i]]) {
			if (Setup->map_data[i] == SETUP_SPACE_DOT) {
				Setup->map_data[i] = SETUP_SPACE;
			}
			else if (Setup->map_data[i] == SETUP_DECOR_DOT_FILLED) {
				Setup->map_data[i] = SETUP_DECOR_FILLED;
			}
			else if (Setup->map_data[i] == SETUP_DECOR_DOT_RU) {
				Setup->map_data[i] = SETUP_DECOR_RU;
			}
			else if (Setup->map_data[i] == SETUP_DECOR_DOT_RD) {
				Setup->map_data[i] = SETUP_DECOR_RD;
			}
			else if (Setup->map_data[i] == SETUP_DECOR_DOT_LU) {
				Setup->map_data[i] = SETUP_DECOR_LU;
			}
			else if (Setup->map_data[i] == SETUP_DECOR_DOT_LD) {
				Setup->map_data[i] = SETUP_DECOR_LD;
			}
		}
	}

	/*
	 * Lookup table to test for map data which can be turned into a dot.
	 */
	memset(dot, 0, sizeof dot);
	dot[SETUP_SPACE] = 1;
	if (!BIT(iniClient.instruments, SHOW_DECOR)) {
		dot[SETUP_DECOR_FILLED] = 1;
		dot[SETUP_DECOR_RU] = 1;
		dot[SETUP_DECOR_RD] = 1;
		dot[SETUP_DECOR_LU] = 1;
		dot[SETUP_DECOR_LD] = 1;
	}

	/*
	 * Optimize.
	 */
	if (iniClient.mapPointSize > 0) {
		if (BIT(Setup->mode, WRAP_PLAY)) {
			for (x = 0; x < Setup->x; x++) {
				if (dot[Setup->map_data[x * Setup->y]]) {
					Map_make_dot(&Setup->map_data[x * Setup->y]);
				}
			}
			for (y = 0; y < Setup->y; y++) {
				if (dot[Setup->map_data[y]]) {
					Map_make_dot(&Setup->map_data[y]);
				}
			}
			start = iniClient.mapPointDistance;
		} else {
			start = 0;
		}
		if (iniClient.mapPointDistance > 0) {
			for (x = start; x < Setup->x; x += iniClient.mapPointDistance) {
				for (y = start; y < Setup->y; y += iniClient.mapPointDistance) {
					if (dot[Setup->map_data[x * Setup->y + y]]) {
						Map_make_dot(&Setup->map_data[x * Setup->y + y]);
					}
				}
			}
		}
		for (i = 0; i < num_cannons; i++) {
			x = cannons[i].pos / Setup->y;
			y = cannons[i].pos % Setup->y;
			if ((x == 0 || y == 0) && BIT(Setup->mode, WRAP_PLAY)) {
				cannons[i].dot = 1;
			}
			else if (iniClient.mapPointDistance > 0
				&& x % iniClient.mapPointDistance == 0
				&& y % iniClient.mapPointDistance == 0) {
				cannons[i].dot = 1;
			} else {
				cannons[i].dot = 0;
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
/*
 * Optimize the drawing of all blue map objects by converting
 * their map type to a bitmask with bits for each blue segment.
 */
void Map_restore(int startx, int starty, int width, int height)
{
	int 				i, j,
						x, y,
						map_index,
						type;

	/*
	 * Restore an optimized map to its original unoptimized state.
	 */
	x = startx;
	for (i = 0; i < width; i++, x++) {
		if (x < 0) {
			x += Setup->x;
		}
		else if (x >= Setup->x) {
			x -= Setup->x;
		}

		y = starty;
		for (j = 0; j < height; j++, y++) {
			if (y < 0) {
				y += Setup->y;
			}
			else if (y >= Setup->y) {
				y -= Setup->y;
			}

			map_index = x * Setup->y + y;

			type = Setup->map_data[map_index];
			if ((type & BLUE_BIT) == 0) {
				if (type == SETUP_FILLED_NO_DRAW) {
					Setup->map_data[map_index] = SETUP_FILLED;
				}
			}
			else if ((type & BLUE_FUEL) == BLUE_FUEL) {
				Setup->map_data[map_index] = SETUP_FUEL;
			}
			else if (type & BLUE_OPEN) {
				if (type & BLUE_BELOW) {
					Setup->map_data[map_index] = SETUP_REC_RD;
				} else {
					Setup->map_data[map_index] = SETUP_REC_LU;
				}
			}
			else if (type & BLUE_CLOSED) {
				if (type & BLUE_BELOW) {
					Setup->map_data[map_index] = SETUP_REC_LD;
				} else {
					Setup->map_data[map_index] = SETUP_REC_RU;
				}
			} else {
				Setup->map_data[map_index] = SETUP_FILLED;
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
void Map_blue(int startx, int starty, int width, int height)
{
	int 				i, j,
						x, y,
						map_index,
						type,
						newtype;
	unsigned char		blue[256];
	const long			outline_mask = SHOW_OUTLINE_WORLD
									 | SHOW_FILLED_WORLD
									 | SHOW_TEXTURED_WALLS;

	/*
	 * Optimize the map for blue.
	 */
	memset(blue, 0, sizeof blue);
	blue[SETUP_FILLED] = BLUE_LEFT | BLUE_UP | BLUE_RIGHT | BLUE_DOWN;
	blue[SETUP_FILLED_NO_DRAW] = blue[SETUP_FILLED];
	blue[SETUP_FUEL] = blue[SETUP_FILLED];
	blue[SETUP_REC_RU] = BLUE_RIGHT | BLUE_UP;
	blue[SETUP_REC_RD] = BLUE_RIGHT | BLUE_DOWN;
	blue[SETUP_REC_LU] = BLUE_LEFT | BLUE_UP;
	blue[SETUP_REC_LD] = BLUE_LEFT | BLUE_DOWN;
	blue[BLUE_BIT|BLUE_OPEN] =
	blue[BLUE_BIT|BLUE_OPEN|BLUE_LEFT] =
	blue[BLUE_BIT|BLUE_OPEN|BLUE_UP] =
	blue[BLUE_BIT|BLUE_OPEN|BLUE_LEFT|BLUE_UP] =
		blue[SETUP_REC_LU];
	blue[BLUE_BIT|BLUE_OPEN|BLUE_BELOW] =
	blue[BLUE_BIT|BLUE_OPEN|BLUE_BELOW|BLUE_RIGHT] =
	blue[BLUE_BIT|BLUE_OPEN|BLUE_BELOW|BLUE_DOWN] =
	blue[BLUE_BIT|BLUE_OPEN|BLUE_BELOW|BLUE_RIGHT|BLUE_DOWN] =
		blue[SETUP_REC_RD];
	blue[BLUE_BIT|BLUE_CLOSED] =
	blue[BLUE_BIT|BLUE_CLOSED|BLUE_RIGHT] =
	blue[BLUE_BIT|BLUE_CLOSED|BLUE_UP] =
	blue[BLUE_BIT|BLUE_CLOSED|BLUE_RIGHT|BLUE_UP] =
		blue[SETUP_REC_RU];
	blue[BLUE_BIT|BLUE_CLOSED|BLUE_BELOW] =
	blue[BLUE_BIT|BLUE_CLOSED|BLUE_BELOW|BLUE_LEFT] =
	blue[BLUE_BIT|BLUE_CLOSED|BLUE_BELOW|BLUE_DOWN] =
	blue[BLUE_BIT|BLUE_CLOSED|BLUE_BELOW|BLUE_LEFT|BLUE_DOWN] =
		blue[SETUP_REC_LD];
	for (i = BLUE_BIT; i < sizeof blue; i++) {
		if ((i & BLUE_FUEL) == BLUE_FUEL
			|| (i & (BLUE_OPEN|BLUE_CLOSED)) == 0) {
			blue[i] = blue[SETUP_FILLED];
		}
	}

	x = startx;
	for (i = 0; i < width; i++, x++) {
		if (x < 0) {
			x += Setup->x;
		}
		else if (x >= Setup->x) {
			x -= Setup->x;
		}

		y = starty;
		for (j = 0; j < height; j++, y++) {
			if (y < 0) {
				y += Setup->y;
			}
			else if (y >= Setup->y) {
				y -= Setup->y;
			}

			map_index = x * Setup->y + y;

			type = Setup->map_data[map_index];
			newtype = 0;
			switch (type) {
			case SETUP_FILLED:
			case SETUP_FILLED_NO_DRAW:
			case SETUP_FUEL:
				newtype = BLUE_BIT;
				if (type == SETUP_FUEL) {
					newtype |= BLUE_FUEL;
				}
				if ((x == 0)
					? (!BIT(Setup->mode, WRAP_PLAY) ||
						!(blue[Setup->map_data[(Setup->x - 1) * Setup->y + y]]
							& BLUE_RIGHT))
					: !(blue[Setup->map_data[(x - 1) * Setup->y + y]]
						& BLUE_RIGHT))
					newtype |= BLUE_LEFT;
				if ((y == 0)
					? (!BIT(Setup->mode, WRAP_PLAY) ||
						!(blue[Setup->map_data[x * Setup->y + Setup->y - 1]]
							& BLUE_UP))
					: !(blue[Setup->map_data[x * Setup->y + (y - 1)]]
						& BLUE_UP))
					newtype |= BLUE_DOWN;
				if (!BIT(iniClient.instruments, outline_mask)
					|| ((x == Setup->x - 1)
						? (!BIT(Setup->mode, WRAP_PLAY)
						   || !(blue[Setup->map_data[y]]
								& BLUE_LEFT))
						: !(blue[Setup->map_data[(x + 1) * Setup->y + y]]
							& BLUE_LEFT)))
					newtype |= BLUE_RIGHT;
				if (!BIT(iniClient.instruments, outline_mask)
					|| ((y == Setup->y - 1)
						? (!BIT(Setup->mode, WRAP_PLAY)
						   || !(blue[Setup->map_data[x * Setup->y]]
								& BLUE_DOWN))
						: !(blue[Setup->map_data[x * Setup->y + (y + 1)]]
							& BLUE_DOWN)))
					newtype |= BLUE_UP;
				break;

			case SETUP_REC_LU:
				newtype = BLUE_BIT | BLUE_OPEN;
				if (x == 0
					? (!BIT(Setup->mode, WRAP_PLAY) ||
						!(blue[Setup->map_data[(Setup->x - 1) * Setup->y + y]]
							& BLUE_RIGHT))
					: !(blue[Setup->map_data[(x - 1) * Setup->y + y]]
						& BLUE_RIGHT))
					newtype |= BLUE_LEFT;
				if (!BIT(iniClient.instruments, outline_mask)
					|| ((y == Setup->y - 1)
						? (!BIT(Setup->mode, WRAP_PLAY)
						   || !(blue[Setup->map_data[x * Setup->y]]
								& BLUE_DOWN))
						: !(blue[Setup->map_data[x * Setup->y + (y + 1)]]
							& BLUE_DOWN)))
					newtype |= BLUE_UP;
				break;

			case SETUP_REC_RU:
				newtype = BLUE_BIT | BLUE_CLOSED;
				if (!BIT(iniClient.instruments, outline_mask)
					|| ((x == Setup->x - 1)
						? (!BIT(Setup->mode, WRAP_PLAY)
						   || !(blue[Setup->map_data[y]]
								& BLUE_LEFT))
						: !(blue[Setup->map_data[(x + 1) * Setup->y + y]]
							& BLUE_LEFT)))
					newtype |= BLUE_RIGHT;
				if (!BIT(iniClient.instruments, outline_mask)
					|| ((y == Setup->y - 1)
						? (!BIT(Setup->mode, WRAP_PLAY)
						   || !(blue[Setup->map_data[x * Setup->y]]
								& BLUE_DOWN))
						: !(blue[Setup->map_data[x * Setup->y + (y + 1)]]
							& BLUE_DOWN)))
					newtype |= BLUE_UP;
				break;

			case SETUP_REC_LD:
				newtype = BLUE_BIT | BLUE_BELOW | BLUE_CLOSED;
				if ((x == 0)
					? (!BIT(Setup->mode, WRAP_PLAY) ||
						!(blue[Setup->map_data[(Setup->x - 1) * Setup->y + y]]
							& BLUE_RIGHT))
					: !(blue[Setup->map_data[(x - 1) * Setup->y + y]]
						& BLUE_RIGHT))
					newtype |= BLUE_LEFT;
				if ((y == 0)
					? (!BIT(Setup->mode, WRAP_PLAY) ||
						!(blue[Setup->map_data[x * Setup->y + Setup->y - 1]]
							& BLUE_UP))
					: !(blue[Setup->map_data[x * Setup->y + (y - 1)]]
						& BLUE_UP))
					newtype |= BLUE_DOWN;
				break;

			case SETUP_REC_RD:
				newtype = BLUE_BIT | BLUE_BELOW | BLUE_OPEN;
				if (!BIT(iniClient.instruments, outline_mask)
					|| ((x == Setup->x - 1)
						? (!BIT(Setup->mode, WRAP_PLAY)
						   || !(blue[Setup->map_data[y]]
								& BLUE_LEFT))
						: !(blue[Setup->map_data[(x + 1) * Setup->y + y]]
							& BLUE_LEFT)))
					newtype |= BLUE_RIGHT;
				if ((y == 0)
					? (!BIT(Setup->mode, WRAP_PLAY) ||
						!(blue[Setup->map_data[x * Setup->y + Setup->y - 1]]
							& BLUE_UP))
					: !(blue[Setup->map_data[x * Setup->y + (y - 1)]]
						& BLUE_UP))
					newtype |= BLUE_DOWN;
				break;

			default:
				continue;
			}
			if (newtype != 0) {
				if (newtype == BLUE_BIT) {
					newtype = SETUP_FILLED_NO_DRAW;
				}
				Setup->map_data[map_index] = newtype;
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
static int Map_init(void)
{
	int 				i,
						max,
						type;
	u_byte				types[256];

	num_fuels = 0;
	num_bases = 0;
	num_cannons = 0;
	num_targets = 0;
	fuels = NULL;
	bases = NULL;
	cannons = NULL;
	targets = NULL;
	memset(types, 0, sizeof types);
	types[SETUP_FUEL] = 1;
	types[SETUP_CANNON_UP] = 2;
	types[SETUP_CANNON_RIGHT] = 2;
	types[SETUP_CANNON_DOWN] = 2;
	types[SETUP_CANNON_LEFT] = 2;
	for (i = SETUP_TARGET; i < SETUP_TARGET + 10; i++) {
		types[i] = 3;
	}
	for (i = SETUP_BASE_LOWEST; i <= SETUP_BASE_HIGHEST; i++) {
		types[i] = 4;
	}
	max = Setup->x * Setup->y;
	for (i = 0; i < max; i++) {
		switch (types[Setup->map_data[i]]) {
		case 1: num_fuels++; break;
		case 2: num_cannons++; break;
		case 3: num_targets++; break;
		case 4: num_bases++; break;
		}
	}
	if (num_bases != 0) {
		bases = (homebase_t *) malloc(num_bases * sizeof(homebase_t));
		if (bases == NULL) {
			error("No memory for Map bases (%d)", num_bases);
			return -1;
		}
		num_bases = 0;
	}
	if (num_fuels != 0) {
		fuels = (fuelstation_t *) malloc(num_fuels * sizeof(fuelstation_t));
		if (fuels == NULL) {
			error("No memory for Map fuels (%d)", num_fuels);
			return -1;
		}
		num_fuels = 0;
	}
	if (num_targets != 0) {
		targets = (target_t *) malloc(num_targets * sizeof(target_t));
		if (targets == NULL) {
			error("No memory for Map targets (%d)", num_targets);
			return -1;
		}
		num_targets = 0;
	}
	if (num_cannons != 0) {
		cannons = (cannontime_t *) malloc(num_cannons * sizeof(cannontime_t));
		if (cannons == NULL) {
			error("No memory for Map cannons (%d)", num_cannons);
			return -1;
		}
		num_cannons = 0;
	}
	for (i = 0; i < MAX_CHECKPOINT; i++) {
		types[SETUP_CHECK + i] = 5;
	}
	for (i = 0; i < max; i++) {
		type = Setup->map_data[i];
		switch (types[type]) {
		case 1:
			fuels[num_fuels].pos = i;
			fuels[num_fuels].fuel = MAX_STATION_FUEL;
			num_fuels++;
			break;
		case 2:
			cannons[num_cannons].pos = i;
			cannons[num_cannons].dead_time = 0;
			cannons[num_cannons].dot = 0;
			num_cannons++;
			break;
		case 3:
			targets[num_targets].pos = i;
			targets[num_targets].dead_time = 0;
			targets[num_targets].damage = TARGET_DAMAGE;
			num_targets++;
			break;
		case 4:
			bases[num_bases].pos = i;
			bases[num_bases].id = -1;
			bases[num_bases].team = type % 10;
			num_bases++;
			Setup->map_data[i] = type - (type % 10);
			break;
		case 5:
			checks[type - SETUP_CHECK].pos = i;
			Setup->map_data[i] = SETUP_CHECK;
			break;
		}
	}
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
static int Map_cleanup(void)
{
	if (num_bases > 0) {
		if (bases != NULL) {
			free(bases);
			bases = NULL;
		}
		num_bases = 0;
	}
	if (num_fuels > 0) {
		if (fuels != NULL) {
			free(fuels);
			fuels = NULL;
		}
		num_fuels = 0;
	}
	if (num_targets > 0) {
		if (targets != NULL) {
			free(targets);
			targets = NULL;
		}
		num_targets = 0;
	}
	if (num_cannons > 0) {
		if (cannons != NULL) {
			free(cannons);
			cannons = NULL;
		}
		num_cannons = 0;
		}
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
Other* Other_by_id(int id)
{
	if (id == -1)
		return(NULL);

	Other* o = (Other*)others.GetHead();
	while (o)
	{
		if (o->id == id)
			return(o);
		o = (Other*)o->GetNext();
	}
	return NULL;
}

///////////////////////////////////////////////////////////////////////////////
ShipObj* Ship_by_id(int id)
{
	Other*	other;

	if ((other = Other_by_id(id)) == NULL) {
		return Parse_shape_str(NULL);
	}
	return other->ship;
}

///////////////////////////////////////////////////////////////////////////////
int Handle_leave(int id)
{
	Other*	other;
	char	msg[MSG_LEN];

	if ((other = Other_by_id(id)) != NULL) {
		if (other == self) {
			seterrno(0);
			error("Self left?!");
			self = NULL;
		}
//		Free_ship_shape(other->ship);
//		other->ship = NULL;
		/*
		 * Silent about tanks and robots.
		 */
		if (other->mychar != 'T'
		 && other->mychar != 'R')
		{
			sprintf(msg, "%s left this world.", (PCSTR)other->name);
			AddMessage(msg);
		}
		others.Remove(other);
		delete other;
		scoresChanged = 1;
	}
	Other* o = (Other*)others.GetHead();
	while (o) {
		if (o->warId == id)	{
			o->warId = NO_ID;
			scoresChanged = true;
			// D(xpprintf("Handle_leave warId = -1\n");)
		}
		o = (Other*)o->GetNext();
	}
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
int Handle_player(int id, int player_team, int mychar, char *player_name,
				  char *real_name, char *host_name, char *shape)
{
	Other*	other;

	if ((other = Other_by_id(id)) == NULL) {
		other = new Other();
		others.Add(other);
	}
	if (self == NULL
	 && strcmp(iniClient.name, player_name) == 0)
	{
		self = other;
		iniClient.team = player_team;
	}
	other->id = id;
	other->team = player_team;
	other->mychar = mychar;
	other->name = player_name;
	other->real = real_name;
	other->host = host_name;
	scoresChanged = 1;
	other->ship = Convert_shape_str(shape);
	Calculate_shield_radius(other->ship);

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
int Handle_war(int robot_id, int killer_id)
{
	Other*	robot;
	Other*	killer;
	char	msg[MSG_LEN];

	if ((robot = Other_by_id(robot_id)) == NULL) {
		seterrno(0);
		IFNWINDOWS(error("Can't update war for non-existing player (%d,%d)", robot_id, killer_id);)
		return 0;
	}
	if (killer_id == -1) {
		/*
		 * Robot is no longer in war mode.
		 */
		robot->warId = NO_ID;
		// D(xpprintf("Handle_war warId = -1\n");)
		return 0;
	}
	if ((killer = Other_by_id(killer_id)) == NULL) {
				seterrno(0);
				IFNWINDOWS(error("Can't update war against non-existing player (%d,%d)", robot_id, killer_id);)
				return 0;
	}
	robot->warId = killer_id;
	sprintf(msg, "%s declares war on %s.", (PCSTR)robot->name, (PCSTR)killer->name);
	AddMessage(msg);
	scoresChanged = true;

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
int Handle_eyes(int watcherId, int watchedId)
{
	Other*	watcher;
	Other*	watched;

	if ((watcher = Other_by_id(watcherId)) == NULL)
	{
		seterrno(0);
		IFNWINDOWS(error("Can't update eyes for non-existing player (%d,%d)",
				watcherId, watchedId);)
		return 0;
	}
	if (watchedId == NO_ID) 
	{
		/*
		 * "Robot is no longer in war mode."
		 */
		watcher->warId = NO_ID;
		scoresChanged = 1;
		return 0;
	}
	if ((watched = Other_by_id(watchedId)) == NULL) 
	{
		seterrno(0);
		IFNWINDOWS(error("Can't update eyes against non-existing player (%d,%d)",
				watcherId, watchedId);)
		return 0;
	}
	watcher->warId = watchedId;
	scoresChanged = true;

	return 0;
}


///////////////////////////////////////////////////////////////////////////////
int Handle_seek(int programmer_id, int robot_id, int sought_id)
{
	Other*	programmer;
	Other*	robot;
	Other*	sought;
	char	msg[MSG_LEN];

	if ((programmer = Other_by_id(programmer_id)) == NULL
		|| (robot = Other_by_id(robot_id)) == NULL
		|| (sought = Other_by_id(sought_id)) == NULL) 
		{
				seterrno(0);
				error("Bad player seek (%d,%d,%d)",
						  programmer_id, robot_id, sought_id);
				return 0;
	}
	robot->warId = sought_id;
	sprintf(msg, "%s has programmed %s to seek %s.",
			(PCSTR)programmer->name, (PCSTR)robot->name, (PCSTR)sought->name);
	AddMessage(msg);
	scoresChanged = 1;


	return 0;
}

///////////////////////////////////////////////////////////////////////////////
int Handle_score(int id, DFLOAT score, int life, int mychar, int alliance, 
				 short kills, short deaths)
{
	Other*	other;

	if ((other = Other_by_id(id)) == NULL) {
#ifndef _WINDOWS
		seterrno(0);
		error("Can't update score for non-existing player %d,%.2f,%d",
			  id, score, life);
#endif
		return 0;
	}
	else if (other->score != score
		|| other->life != life
		|| other->mychar != mychar
		|| other->alliance != alliance) {
		other->kills = kills;
		other->deaths = deaths;
		other->score = score;
		other->life = life;
		other->mychar = mychar;
		other->alliance = alliance;
		scoresChanged = 1;
	}

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
int Handle_team_score(int team, DFLOAT score)
{
	if (teamscores[team] != score) {
		teamscores[team] = score;
		scoresChanged = 1;
	}
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
int HandleScoreTablePages(uint pages)
{
	scoreTablePages = pages;
	CreateXPilotMenu();
	return(0);
}

///////////////////////////////////////////////////////////////////////////////
int	HandleScoreTableIPVer(int id, PCSTR s)
{
	Other*	other;
	if ((other = Other_by_id(id)) == NULL) {
		return(0);
	}
	other->ipVer = s;
	return(0);
}

///////////////////////////////////////////////////////////////////////////////
int HandleScoreTableRank(int id, int rank, DFLOAT rate)
{
	Other*	other;
	if ((other = Other_by_id(id)) == NULL) {
		return(0);
	}
	D(xpprintf("HandleScoreTableRank: %s %d %6.2f\n", (PCSTR)other->name, rank, rate);)
	if (scoreTableType == STRank)
	{
		if (other->rank != rank
		 || other->rate != rate)
			scoresChanged = 1;
	}
	other->rank = rank;
	other->rate = rate;
	return(0);
}

///////////////////////////////////////////////////////////////////////////////
int Handle_timing(int id, int check, int round)
{
	Other*	other;

	if ((other = Other_by_id(id)) == NULL) {
		seterrno(0);
		error("Can't update timing for non-existing player %d,%d,%d", id, check, round);
		return 0;
	}
	else if (other->check != check
		|| other->round != round) {
		other->check = check;
		other->round = round;
		other->timing = round * MAX_CHECKS + check;
		other->timing_loops = last_loops;
		scoresChanged = 1;
	}

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
int Handle_score_object(DFLOAT score, int x, int y, char *msg)
{
	score_object_t* 	sobj = &score_objects[score_object];

	sobj->score = score;
	sobj->x = x;
	sobj->y = y;
	sobj->count = 1;

	/* Initialize sobj->hud_msg (is shown on the HUD) */
	if (msg[0] != '\0') {
		if (iniClient.showScoreDecimals > 0 && version >= 0x4500) {
			sprintf(sobj->hud_msg, "%s %.*f", msg, iniClient.showScoreDecimals, score);
		}
		else {
			sprintf(sobj->hud_msg, "%s %d", msg, (int)score);
		}
		sobj->hud_msg_len = strlen(sobj->hud_msg);
		sobj->hud_msg_width = XTextWidth(gameFont,
										 sobj->hud_msg, sobj->hud_msg_len);
	} else
		sobj->hud_msg_len = 0;

	/* Initialize sobj->msg data (is shown on game area) */
		if (iniClient.showScoreDecimals > 0 && version >= 0x4500) {
			sprintf(sobj->msg, "%.*f", iniClient.showScoreDecimals, score);
		}
		else {
			sprintf(sobj->msg, "%d", (int)score);
		}
	sobj->msg_len = strlen(sobj->msg);
	sobj->msg_width = XTextWidth(gameFont, sobj->msg, sobj->msg_len);

	/* Update global index variable */
	score_object = (score_object + 1) % MAX_SCORE_OBJECTS;

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
void Client_score_table(void)
{
	struct team_score {
		DFLOAT	score;
		int 			life;
		int 			playing;
	};
	struct team_score	team[MAX_TEAMS],
						*team_order[MAX_TEAMS];
	Other*		other;
	Other**		order;
	Other*		best = NULL;
	int 		j, k;
	DFLOAT		ratio;
	DFLOAT		best_ratio = -1e7;

	if (scoresChanged == 0) {
		return;
	}

	if (players_exposed == false) {
		return;
	}

	if (others.GetCount() < 1) {
		Paint_score_start();
		scoresChanged = 0;
		return;
	}

	if ((order = (Other **)malloc(others.GetCount() * sizeof(Other*))) == NULL) {
		error("No memory for score");
		return;
	}
	if (BIT(Setup->mode, TEAM_PLAY|TIMING) == TEAM_PLAY) {
		memset(&team[0], 0, sizeof team);
	}
//	for (i = 0; i < num_others; i++) {
//		other = &Others[i];
	other = (Other*)others.GetHead();
	int	i = -1;
	while (other)
	{
		i++;
		if (BIT(Setup->mode, TIMING)) {
			/*
			 * Sort the score table on position in race.
			 * Put paused and waiting players last as well as tanks.
			 */
			if (strchr("PTW", other->mychar)) {
				j = i;
			}
			else {
				for (j = 0; j < i; j++) {
					if (order[j]->timing < other->timing) {
						break;
					}
					if (strchr("PTW", order[j]->mychar)) {
						break;
					}
					if (order[j]->timing == other->timing) {
						if (order[j]->timing_loops > other->timing_loops) {
							best = other;
							break;
						}
					}
				}
			}
		}
		else 
		{
			if (scoreTableType == STKills) 
			{
				if (other->deaths)
					other->ratio = (DFLOAT)other->kills / (DFLOAT)other->deaths;
				else
					other->ratio = other->kills * 2;
			}
			if (BIT(Setup->mode, LIMITED_LIVES)
			 || scoreTableType == STKills
			 || scoreTableType == STRank)
				ratio = other->score;
			else
				ratio = other->score / (other->life + 1);
			if (best == NULL || ratio > best_ratio) 
			{
				best_ratio = ratio;
				best = other;
			}
			for (j = 0; j < i; j++) 
			{
				if (scoreTableType == STKills)
				{
					if (order[j]->ratio < other->ratio)
						break;
				}
				else if (scoreTableType == STRank)
				{
					if (other->rank)
					{
						if (order[j]->rank > other->rank)
							break;
						if (order[j]->rank < other->rank)
							continue;
						if (order[j]->rate < other->rate)
							break;
						if (order[j]->rate > other->rate)
							continue;
					}
					if (order[j]->score < other->score)
						break;
				}
				else
				{
					if (order[j]->score < other->score)
						break;
				}
			}
		}
		for (k = i; k > j; k--) {
			order[k] = order[k - 1];
		}
		order[j] = other;

		if (BIT(Setup->mode, TEAM_PLAY|TIMING) == TEAM_PLAY) {
			switch (other->mychar) {
			case 'P':
			case 'W':
			case 'T':
				break;
			case ' ':
			case 'R':
				if (BIT(Setup->mode, LIMITED_LIVES)) {
					team[other->team].life += other->life + 1;
				} else {
					team[other->team].life += other->life;
				}
				/*FALLTHROUGH*/
			default:
				team[other->team].playing++;
				if (version < 0x4500) {
						team[other->team].score += other->score;
				}
				break;
			}
			if (version >= 0x4500) {
					team[other->team].score = teamscores[other->team];
			}
		}
		other = (Other*)other->GetNext();
	}
	Paint_score_start();
//	if (BIT(Setup->mode, TIMING)) {
//		best = order[0] - Others;
//	}
	for (i = 0; i < others.GetCount(); i++) {
		other = order[i];
//		j = other - Others;
//		Paint_score_entry(i, other, (j == best) ? true : false);
		Paint_score_entry(i, other, (other == best) ? true : false);
	}
	if (BIT(Setup->mode, TEAM_PLAY|TIMING) == TEAM_PLAY) {
		int pos = others.GetCount() + 1;
		int num_playing_teams = 0;
		for (i = 0; i < MAX_TEAMS; i++) {
			if (team[i].playing) {
				for (j = 0; j < num_playing_teams; j++) {
					if (team[i].score > team_order[j]->score
						|| (team[i].score == team_order[j]->score
							&& ((BIT(Setup->mode, LIMITED_LIVES))
								? (team[i].life > team_order[j]->life)
								: (team[i].life < team_order[j]->life)))) {
						for (k = i; k > j; k--) {
							team_order[k] = team_order[k - 1];
						}
						break;
					}
				}
				team_order[j] = &team[i];
				num_playing_teams++;
			}
		}
		for (i = 0; i < num_playing_teams; i++) {
			Other tmp;
			tmp.team = team_order[i] - &team[0];
			tmp.name.printf("Team %d", tmp.team);
			tmp.real = tmp.name;
			if (BIT(Setup->mode, LIMITED_LIVES) && team_order[i]->life == 0) {
				tmp.mychar = 'D';
			} else {
				tmp.mychar = ' ';
			}
			tmp.score = team_order[i]->score;
			tmp.life = team_order[i]->life;
			Paint_score_entry(pos++, &tmp, false);
		}
	}

	free(order);

	IFWINDOWS( MarkPlayersForRedraw(); )

	scoresChanged = 0;
}

///////////////////////////////////////////////////////////////////////////////
#ifndef _WINDOWS
static int Alloc_history(void)
{
	char		*hist_ptr;
	int 		i;

	/* maxLinesInHistory is a runtime constant */
	if ((hist_ptr = (char *)malloc(iniClient.maxLinesInHistory * MAX_CHARS)) == NULL) {
		error("No memory for history");
		return -1;
	}
	HistoryBlock		= hist_ptr;

	for (i = 0; i < iniClient.maxLinesInHistory; i++) {
		HistoryMsg[i]	= hist_ptr;
		hist_ptr[0] 	= '\0';
		hist_ptr		+= MAX_CHARS;
	}
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
static void Free_selectionAndHistory(void)
{
	if (HistoryBlock) {
		free(HistoryBlock);
		HistoryBlock = NULL;
	}
	if (selection.txt) {
		free(selection.txt);
		selection.txt = NULL;
	}
}

///////////////////////////////////////////////////////////////////////////////
#else
static int Alloc_history(void)
{
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
static void Free_selectionAndHistory(void)
{
}
#endif

///////////////////////////////////////////////////////////////////////////////
//int Client_init(char *server, unsigned server_version)
int ClientInit(Connectparam* conpar)
{
	version = conpar->serverVersion;

	Make_table();
	Init_scale_array();

	if ( Init_wreckage() == -1 ) {
		return -1;
	}

	if (Init_asteroids() == -1) {
		return -1;
	}

	strlcpy(servername, conpar->serverName, sizeof(conpar->serverName));

		conpar->cookie = GetCookie(conpar->serverName, conpar->contactPort);
//		conpar->cookie = 0x12345678;

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
int Client_setup(void)
{
	if (Map_init() == -1) {
		return -1;
	}
	Map_dots();
	Map_restore(0, 0, Setup->x, Setup->y);
	Map_blue(0, 0, Setup->x, Setup->y);

	RadarHeight = (RadarWidth * Setup->y) / Setup->x;

	if (Init_playing_windows() == -1) {
		return -1;
	}
	if (Alloc_msgs() == -1) {
		return -1;
	}
	if (Alloc_history() == -1) {
		return -1;
	}

	/* Old servers can't deal with 0.0 turnresistance, so swap to
	 * the alternate bank, and hope there's something better there. */
	/* HACK: Hanging Gardens runs an old server (version code 0x4101)
	 * which happens to have the turnresistance patch. */
	if (iniClient.turnresistance == 0.0 && version < 0x4200 && version != 0x4101)
	{
		DFLOAT tmp;
#define SWAP(a,b) (tmp = (a), (a) = (b), (b) = tmp)
		SWAP(iniClient.power, iniClient.power_s);
		SWAP(iniClient.turnspeed, iniClient.turnspeed_s);
		SWAP(iniClient.turnresistance, iniClient.turnresistance_s);
#undef SWAP
		control_count = CONTROL_DELAY;
		AddMessage("Old server can't handle turnResistance=0.0; "
					"swapping to alternate settings [*Client message*]");
	}

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
int Client_fps_request(void)
{
	LIMIT(iniClient.maxFPS, FPS / 2, FPS);
	oldMaxFPS = iniClient.maxFPS;
		theWorld.fps = iniClient.maxFPS;
	return Send_fps_request(iniClient.maxFPS);
}

///////////////////////////////////////////////////////////////////////////////
int Client_power(void)
{
	int 		i;

	if (Send_power(iniClient.power) == -1
		|| Send_power_s(iniClient.power_s) == -1
		|| Send_turnspeed(iniClient.turnspeed) == -1
		|| Send_turnspeed_s(iniClient.turnspeed_s) == -1
		|| Send_turnresistance(iniClient.turnresistance) == -1
		|| Send_turnresistance_s(iniClient.turnresistance_s) == -1
		|| Send_display() == -1
		|| Startup_server_motd() == -1) {
		return -1;
	}
	for (i = 0; i < NUM_MODBANKS; i++) {
		if (Send_modifier_bank(i) == -1) {
			return -1;
		}
	}

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
int Client_start(void)
{
	Key_init();
		if (Client_wrap_mode == 0)
		{
				CLR_BIT(iniClient.instruments, SHOW_SLIDING_RADAR);
		}

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
void Client_cleanup(void)
{
//	int 		i;

	Quit();
	Free_selectionAndHistory();
	/*
	if (max_others > 0) {
		for (i = 0; i < num_others; i++) {
			other_t* other = &Others[i];
			Free_ship_shape(other->ship);
		}
		free(Others);
		num_others = 0;
		max_others = 0;
	}
	*/
	Map_cleanup();
}

///////////////////////////////////////////////////////////////////////////////
int Client_fd(void)
{
	return ConnectionNumber(iniClient.dpy);
}

///////////////////////////////////////////////////////////////////////////////
int Client_input(int new_input)
{
#ifndef _WINDOWS
	return x_event(new_input);
#else
	return 0;
#endif
}

///////////////////////////////////////////////////////////////////////////////
void Client_flush(void)
{
	XFlush(iniClient.dpy);
}

///////////////////////////////////////////////////////////////////////////////
void Client_sync(void)
{
	XSync(iniClient.dpy, False);
}

///////////////////////////////////////////////////////////////////////////////
int Client_wrap_mode(void)
{
	return (BIT(Setup->mode, WRAP_PLAY) != 0);
}

///////////////////////////////////////////////////////////////////////////////
int Check_client_fps(void) 
{
	if (oldMaxFPS != iniClient.maxFPS) {
		LIMIT(iniClient.maxFPS, FPS / 2, FPS);
		oldMaxFPS = iniClient.maxFPS;
		return Send_fps_request(iniClient.maxFPS);
	}
	return 0;
}
