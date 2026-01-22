/* $Id: score.h,v 1.6 2002/07/11 16:36:03 dick Exp $
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
 * $Log: score.h,v $
 * Revision 1.6  2002/07/11 16:36:03  dick
 * Update to XPilot-4.5.4beta
 *
 * Revision 1.5  2002/06/30 06:44:48  dick
 * Add some strings. s_br* for bracketed strings. s_brWall = "[Wall]";
 *
 * Revision 1.4  2002/06/25 05:04:24  dick
 * Score_players takes an additional parameter: the ScoreType (describe the kill)
 *
 */

#ifndef SCORE_H
#define SCORE_H

#define ED_SHOT 				(-0.2*FUEL_SCALE_FACT)
#define ED_SMART_SHOT			(-30*FUEL_SCALE_FACT)
#define ED_MINE 				(-60*FUEL_SCALE_FACT)
#define ED_ECM					(-60*FUEL_SCALE_FACT)
#define ED_TRANSPORTER			(-60*FUEL_SCALE_FACT)
#define ED_HYPERJUMP			(-60*FUEL_SCALE_FACT)
#define ED_SHIELD				(-0.20*FUEL_SCALE_FACT)
#define	ED_PHASING_DEVICE		(-0.40*FUEL_SCALE_FACT)
#define ED_CLOAKING_DEVICE		(-0.07*FUEL_SCALE_FACT)
#define ED_DEFLECTOR			(-0.15*FUEL_SCALE_FACT)
#define ED_SHOT_HIT 			(-25.0*FUEL_SCALE_FACT)
#define ED_SMART_SHOT_HIT		(-120.0*FUEL_SCALE_FACT)
#define ED_PL_CRASH 			(-100.0*FUEL_SCALE_FACT)
#define ED_BALL_HIT 			(-50.0*FUEL_SCALE_FACT)
#define ED_LASER				(-10.0*FUEL_SCALE_FACT)
/* was 90 -> 2 -> 40 -> 20 -> 10 */
#define ED_LASER_HIT			(-100.0*FUEL_SCALE_FACT)
/* was 120 -> 80 -> 40 -> 50 -> 60 -> 100 */

#define CANNON_SCORE			-1436
#define WALL_SCORE				2000

#define RATE_SIZE				20
#define RATE_RANGE				1024

/* score.c */

void SCORE(Player* pl, DFLOAT points, int x, int y, const char *msg);
void TEAM_SCORE(World* w, int team, DFLOAT points);
void Alliance_score(World* w, int id, DFLOAT points);
DFLOAT Rate(DFLOAT winner, DFLOAT loser);

/*
 * Cause `winner' to get `winner_score' points added with message
 * `winner_msg', and similarly with the `loser' and equivalent
 * variables.
 *
 * In general the winner_score should be positive, and the loser_score
 * negative, but this need not be true.
 *
 * If the winner and loser players are on the same team, the scores are
 * made negative, since you shouldn't gain points by killing team members,
 * or being killed by a team member (it is both players faults).
 *
 * BD 28-4-98: Same for killing your own tank.
 */
void Score_players(Player* winner, DFLOAT winner_score, char *winner_msg,
				   Player* loser, DFLOAT loser_score, char *loser_msg, ScoreType st);


/*
 * Strings to be sent to the scoreserver; like [Wall], [Cannon]
 */
extern	PCSTR	s_brAsteroid;
extern	PCSTR	s_brBall;
extern	PCSTR	s_brCannon;
extern	PCSTR	s_brExplosion;
extern	PCSTR	s_brTarget;
extern	PCSTR	s_brTreasure;
extern	PCSTR	s_brSelfDestruct;
extern	PCSTR	s_brWall;
#endif
