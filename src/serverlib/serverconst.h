/* $Id: serverconst.h,v 1.6 2004/05/20 22:27:42 dick Exp $
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
 */
/*
 * $Log: serverconst.h,v $
 * Revision 1.6  2004/05/20 22:27:42  dick
 * NO_ID belongs in common
 *
 * Revision 1.5  2004/01/19 22:05:06  dick
 * Move MAX_MAP_SIZE from serverlib to common (it *is* a global).
 *
 * Revision 1.4  2002/09/11 16:46:07  dick
 * Get rid of all GetFPS() in the constants.  WRAP_D[XY] takes a World* parameter.
 *
 */

#ifndef SERVERCONST_H
#define SERVERCONST_H

#ifndef CONST_H
#include "const.h"
#endif


/*
 * Two macros for edge wrap of x and y coordinates measured in pixels.
 * Note that the correction needed shouldn't ever be bigger than one mapsize.
 */
#define WRAP_XPIXEL(_w, x_)	\
	(BIT(_w->rules->mode, WRAP_PLAY) \
	    ? ((x_) < 0 \
		? (x_) + _w->width \
		: ((x_) >= _w->width \
		    ? (x_) - _w->width \
		    : (x_))) \
	    : (x_))

#define WRAP_YPIXEL(_w, y_)	\
	(BIT(_w->rules->mode, WRAP_PLAY) \
	    ? ((y_) < 0 \
		? (y_) + _w->height \
		: ((y_) >= _w->height \
		    ? (y_) - _w->height \
		    : (y_))) \
	    : (y_))

/*
 * Two macros for edge wrap of x and y coordinates measured in map blocks.
 * Note that the correction needed shouldn't ever be bigger than one mapsize.
 */
#define WRAP_XBLOCK(_w, x_)	\
	(BIT(_w->rules->mode, WRAP_PLAY) \
	    ? ((x_) < 0 \
		? (x_) + _w->blockWidth \
		: ((x_) >= _w->blockWidth \
		    ? (x_) - _w->blockWidth \
		    : (x_))) \
	    : (x_))

#define WRAP_YBLOCK(_w, y_)	\
	(BIT(_w->rules->mode, WRAP_PLAY) \
	    ? ((y_) < 0 \
		? (y_) + _w->blockHeight \
		: ((y_) >= _w->blockHeight \
		    ? (y_) - _w->blockHeight \
		    : (y_))) \
	    : (y_))

/*
 * Two macros for edge wrap of differences in position.
 * If the absolute value of a difference is bigger than
 * half the map size then it is wrapped.
 */
#define WRAP_DX(_w, dx)	\
	(BIT(_w->rules->mode, WRAP_PLAY) \
	    ? ((dx) < - (_w->width >> 1) \
		? (dx) + _w->width \
		: ((dx) > (_w->width >> 1) \
		    ? (dx) - _w->width \
		    : (dx))) \
	    : (dx))

#define WRAP_DY(_w, dy)	\
	(BIT(_w->rules->mode, WRAP_PLAY) \
	    ? ((dy) < - (_w->height >> 1) \
		? (dy) + _w->height \
		: ((dy) > (_w->height >> 1) \
		    ? (dy) - _w->height \
		    : (dy))) \
	    : (dy))


#define PSEUDO_TEAM(__i,__j)\
	(players[(__i)]->pseudo_team == players[(__j)]->pseudo_team)

/*
 * Used where we wish to know if a player is simply on the same team.
 */
/* #define TEAM(i, j)							\
	(BIT(Players[i]->status|Players[j]->status, PAUSE)		\
	|| (BIT(World.rules->mode, TEAM_PLAY)				\
	   && (Players[i]->team == Players[j]->team)			\
	   && (Players[i]->team != TEAM_NOT_SET))) */
#define TEAM(__w, __i, __j) \
	(BIT(__w->rules->mode, TEAM_PLAY) \
	&& (__w->players[__i]->team == __w->players[__j]->team) \
	&& (__w->players[__i]->team != TEAM_NOT_SET))

/*
 * Used where we wish to know if a player is on the same team
 * and has immunity to shots, thrust sparks, lasers, ecms, etc.
 */
#define TEAM_IMMUNE(__w, __i, __j) \
	(__w->options.teamImmunity->GetBool() \
		 && TEAM(__w, __i, __j))

/*
 * Used where we wish to know if two players are members of the same alliance.
 */
#define ALLIANCE(__w, __i, __j)	\
	((__w->players[__i]->alliance != ALLIANCE_NOT_SET) \
	&& (__w->players[__j]->alliance == __w->players[__i]->alliance))

/*
 * Used where we wish to know if a player (i) owns a tank (j).
 */
#define OWNS_TANK(__w, __i, __j) \
	(IS_TANK_IND(__w, __j) \
	&& (__w->players[__j]->lock.pl_id != -1) \
	&& (__w->getInd[__w->players[__j]->lock.pl_id] == (__i)))


#define RECOVERY_DELAY			(3)			// # of seconds
#define ROBOT_CREATE_DELAY		(2)			// # of seconds

#define NUM_IDS 				256
#define MAX_PSEUDO_PLAYERS		16

#define MAX_TOTAL_SHOTS 		16384	/* must be <= 65536 */
#define MAX_TOTAL_PULSES		(5 * 64)
#define MAX_TOTAL_ECMS			64
#define MAX_TOTAL_TRANSPORTERS	(2 * 64)


#define LG2_MAX_AFTERBURNER 	4
#define ALT_SPARK_MASS_FACT 	4.2
#define ALT_FUEL_FACT			3
#define MAX_AFTERBURNER 	   ((1<<LG2_MAX_AFTERBURNER)-1)
#define AFTER_BURN_SPARKS(s,n)	(((s)*(n))>>LG2_MAX_AFTERBURNER)
#define AFTER_BURN_POWER_FACTOR(n) \
 (1.0+(n)*((ALT_SPARK_MASS_FACT-1.0)/(MAX_AFTERBURNER+1.0)))
#define AFTER_BURN_POWER(p,n)	\
 ((p)*AFTER_BURN_POWER_FACTOR(n))
#define AFTER_BURN_FUEL(f,n)	\
 (((f)*((MAX_AFTERBURNER+1)+(n)*(ALT_FUEL_FACT-1)))/(MAX_AFTERBURNER+1.0))

#define TURN_FUEL(acc)			(0.005*FUEL_SCALE_FACT*ABS(acc))
#define TURN_SPARKS(tf) 		(5+((tf)>>((FUEL_SCALE_BITS)-6)))

#define THRUST_MASS 			0.7

#define ARMOR_MASS(_w)			(_w->options.shipMass->GetDouble() / 14)

#define MAX_TANKS				8
#define TANK_MASS(_w)			(_w->options.shipMass->GetDouble()/10)
#define TANK_CAP(n) 			(!(n)?MAX_PLAYER_FUEL:(MAX_PLAYER_FUEL/3))
#define TANK_FUEL(n)			((TANK_CAP(n)*(5+(randomMT()&3)))/32)
#define TANK_REFILL_LIMIT		(MIN_PLAYER_FUEL/8)
#define TANK_THRUST_FACT		0.7
#define TANK_NOTHRUST_TIME		(HEAT_CLOSE_TIMEOUT/2+2)
#define TANK_THRUST_TIME		(TANK_NOTHRUST_TIME/2+1)

#define GRAVS_POWER 			2.7

#define ECM_DISTANCE			(VISIBILITY_DISTANCE*0.4)
#define TRANSPORTER_DISTANCE	(VISIBILITY_DISTANCE*0.2)

#define SHOT_MULT(o) \
		((BIT((o)->mods.nuclear, NUCLEAR) && BIT((o)->mods.warhead, CLUSTER)) \
		 ? options.nukeClusterDamage->GetDouble() : 1.0f)

#define MINE_RADIUS 			8
#define MINE_RANGE				(VISIBILITY_DISTANCE*0.1)
#define MINE_SENSE_BASE_RANGE	(MINE_RANGE*1.3)
#define MINE_SENSE_RANGE_FACTOR (MINE_RANGE*0.3)
#define MINE_MASS				30.0
#define MINE_LIFETIME			(5000+(randomMT()&255))
#define MINE_SPEED_FACT 		1.3

#define MISSILE_LIFETIME		(randomMT()%63+128)		// 128+ seconds
#define MISSILE_MASS			5.0
#define MISSILE_RANGE			4
#define SMART_SHOT_ACC			0.6
#define SMART_SHOT_DECFACT		3
#define SMART_SHOT_MIN_SPEED	(SMART_SHOT_ACC*8)
#define SMART_TURNSPEED 		2.6
#define SMART_SHOT_MAX_SPEED	22.0
#define SMART_SHOT_LOOK_AH		4
#define TORPEDO_SPEED_TIME		2.0			// seconds to max accelleration?
#define TORPEDO_ACC 			(18.0*SMART_SHOT_MAX_SPEED)
#define TORPEDO_RANGE			(MINE_RANGE*0.45)

#define NUKE_SPEED_TIME 		2.0			// seconds
#define NUKE_ACC				(5*TORPEDO_ACC)
#define NUKE_RANGE				(MINE_RANGE*1.5)
#define NUKE_MASS_MULT			1
#define NUKE_MINE_EXPL_MULT 	3
#define NUKE_SMART_EXPL_MULT	4

#define HEAT_RANGE				(VISIBILITY_DISTANCE/2)
#define HEAT_SPEED_FACT 		1.7
#define HEAT_CLOSE_TIMEOUT		2.0			// seconds
#define HEAT_CLOSE_RANGE		HEAT_RANGE
#define HEAT_CLOSE_ERROR		0
#define HEAT_MID_TIMEOUT		4.0			// seconds
#define HEAT_MID_RANGE			(2*HEAT_RANGE)
#define HEAT_MID_ERROR			8
#define HEAT_WIDE_TIMEOUT		8.0			// seconds
#define HEAT_WIDE_ERROR 		16

#define CLUSTER_MASS_SHOTS(_w, mass) ((mass) * 0.9 / _w->options.shotMass->GetDouble())
#define CLUSTER_MASS_DRAIN(_w, mass) (CLUSTER_MASS_SHOTS(_w, mass)*ED_SHOT)

#define SMART_SHOT_LEN			12
#define HEAT_SHOT_LEN			15
#define TORPEDO_LEN 			18

#define PULSE_SPEED 			90
#define PULSE_SAMPLE_DISTANCE	5
#define PULSE_LENGTH			(PULSE_SPEED - PULSE_SAMPLE_DISTANCE)
#define PULSE_MIN_LIFE			4.5
#define PULSE_LIFE(lasers)		(PULSE_MIN_LIFE + ((lasers) / 4))

#define TRACTOR_MAX_RANGE(items)  (200 + (items) * 50)
#define TRACTOR_MAX_FORCE(items)  (-40 + (items) * -20)
#define TRACTOR_PERCENT(dist, maxdist) \
		(1.0 - (0.5 * (dist) / (maxdist)))
#define TRACTOR_COST(percent) (-1.5 * FUEL_SCALE_FACT * (percent))
#define TRACTOR_FORCE(tr_pr, percent, maxforce) \
		((percent) * (maxforce) * ((tr_pr) ? -1 : 1))

#define WARN_TIME				2

#define ALLIANCE_NOT_SET		(-1)

#define DEBRIS_MASS 			4.5

#define ENERGY_RANGE_FACTOR 	(2.5/FUEL_SCALE_FACT)

#define WORM_BRAKE_FACTOR		1
#define WORMCOUNT				64

#endif
