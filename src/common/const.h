/* $Id: const.h,v 1.10 2004/05/20 22:14:37 dick Exp $
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
 *  $Log: const.h,v $
 *  Revision 1.10  2004/05/20 22:14:37  dick
 *  NO_ID is common through all projects, not just the server
 *
 *  Revision 1.9  2004/01/19 22:08:26  dick
 *  MAX_MAP_SIZE is a global const.
 *
 *  Revision 1.8  2002/08/01 14:53:55  dick
 *  Define 3 special cookies; NONE, NOSUP, and SPECIAL.
 *
 *  Revision 1.7  2002/06/01 06:06:46  dick
 *  Encapsulate (almost) everything.  Get rid of (almost) all refs to theWorld.
 *
 *  Revision 1.6  2002/05/18 20:55:35  dick
 *  Update to XPilot-4.5.1
 *
 *  Revision 1.5  2001/09/11 09:08:31  dick
 *  Encapsulate all client resources into new class Ini (Linux cleanups)
 *
 *  Revision 1.4  2001/07/24 09:03:59  dick
 *  Remove all global options.  Use World.options instead.
 *
 *  Revision 1.3  2001/07/23 21:08:36  dick
 *  Encapsulate ShipMass, ShotsMax, ShotsLife, ShotsMass, FPS.
 *
 *  Revision 1.2  2001/07/07 12:00:41  dick
 *  Rename classes to C++ "Style".  old World becomes theWorld.
 *
 */

#ifndef	CONST_H
#define	CONST_H

#ifndef _WINDOWS
#include <limits.h>
#include <math.h>
#endif

#ifndef TYPES_H
#include "types.h"
#endif

/*
 * FLT_MAX is ANSI C standard, but some systems (BSD) use
 * MAXFLOAT instead.
 */
#ifndef	FLT_MAX
#   if defined(__sgi) || defined(__FreeBSD__)
#       include <float.h>	/* FLT_MAX for SGI Personal Iris or FreeBSD */
#   else
#	if defined(__sun__)
#           include <values.h>	/* MAXFLOAT for suns */
#	endif
#   endif
#   if !defined(FLT_MAX)
#	if defined(MAXFLOAT)
#	    define FLT_MAX	MAXFLOAT
#	else
#	    define FLT_MAX	1e30f	/* should suffice :-) */
#	endif
#   endif
#endif

/* Not everyone has PI (or M_PI defined). */
#ifndef	M_PI
#   define PI		3.14159265358979323846
#else
#   define PI		M_PI
#endif

/* Not everyone has LINE_MAX either, *sigh* */
#ifndef LINE_MAX
#   define LINE_MAX 2048
#endif

#define RES		128

#define BLOCK_SZ	35

#define TABLE_SIZE	RES

extern DFLOAT		tbl_sin[];
extern DFLOAT		tbl_cos[];

#define DEF_SPARK_RAND	    0x55	/* 66% */

#if 0
  /* The way it was: one table, and always range checking. */
# define tsin(x)	(tbl_sin[MOD2(x, TABLE_SIZE)])
# define tcos(x)	(tbl_sin[MOD2((x)+TABLE_SIZE/4, TABLE_SIZE)])
#else
# if 0
   /* Range checking: find out where the table size is exceeded. */
#  define CHK2(x, m)	((MOD2(x, m) != x) ? (printf("MOD %s:%d:%s\n", __FILE__, __LINE__, #x), MOD2(x, m)) : (x))
# else
   /* No range checking. */
#  define CHK2(x, m)	(x)
# endif
  /* New table lookup with optional range checking and no extra calculations. */
# define tsin(x)	(tbl_sin[CHK2(x, TABLE_SIZE)])
# define tcos(x)	(tbl_cos[CHK2(x, TABLE_SIZE)])
#endif

#define NELEM(a)	((int)(sizeof(a) / sizeof((a)[0])))

#undef ABS
#define ABS(x)			( (x)<0 ? -(x) : (x) )
#ifndef MAX
#   define MIN(x, y)		( (x)>(y) ? (y) : (x) )
#   define MAX(x, y)		( (x)>(y) ? (x) : (y) )
#endif
#define sqr(x)			( (x)*(x) )
#define DELTA(a, b)		(((a) >= (b)) ? ((a) - (b)) : ((b) - (a)))
#define LENGTH(x, y)		( hypot( (double) (x), (double) (y) ) )
#define VECTOR_LENGTH(v)	( hypot( (double) (v).x, (double) (v).y ) )
#define QUICK_LENGTH(x,y)	( ABS(x)+ABS(y) ) /*-BA Only approx, but v. quick */
#define LIMIT(val, lo, hi)	( val=(val)>(hi)?(hi):((val)<(lo)?(lo):(val)) )

#ifndef MOD2
#  define MOD2(x, m)		( (x) & ((m) - 1) )
#endif	/* MOD2 */

/* Do NOT change these! */
#define MAX_MAP_SIZE	900	/* map dimension limitation: ((0x7FFF - 1280) / 35) */
#define MAX_CHECKS		26
#define MAX_TEAMS		10

#define EXPIRED_MINE_ID		4096   /* assume no player has this id */

#define	NO_ID			-1

#define	COOKIENONE		0		// no cookie from this client
#define	COOKIENOSUP		1		// older clients don't support cookies
#define	COOKIESPECIAL	2		// less than this are special cookies

#define MAX_CHARS		80
#define MSG_LEN			256

#define FONT_LEN		256

#define NUM_MODBANKS		4

#define SPEED_LIMIT		65.0
#define MAX_PLAYER_TURNSPEED	64.0
#define MIN_PLAYER_TURNSPEED	4.0
#define MAX_PLAYER_POWER	55.0
#define MIN_PLAYER_POWER	5.0
#define MAX_PLAYER_TURNRESISTANCE	1.0
#define MIN_PLAYER_TURNRESISTANCE	0.0

#define FUEL_SCALE_BITS         8
#define FUEL_SCALE_FACT         (1<<FUEL_SCALE_BITS)
#define FUEL_MASS(f)            ((f)*0.005/FUEL_SCALE_FACT)
#define MAX_STATION_FUEL	(500<<FUEL_SCALE_BITS)
#define START_STATION_FUEL	(20<<FUEL_SCALE_BITS)
#define STATION_REGENERATION	(0.06*FUEL_SCALE_FACT)
#define MAX_PLAYER_FUEL		(2600<<FUEL_SCALE_BITS)
#define MIN_PLAYER_FUEL		(350<<FUEL_SCALE_BITS)
#define REFUEL_RATE		(5<<FUEL_SCALE_BITS)
#define ENERGY_PACK_FUEL        ((500+(randomMT()&511))<<FUEL_SCALE_BITS)

#define TARGET_DAMAGE		(250<<FUEL_SCALE_BITS)
#define TARGET_FUEL_REPAIR_PER_FRAME (TARGET_DAMAGE / (GetFPS() * 10))
#define TARGET_REPAIR_PER_FRAME	(TARGET_DAMAGE / (GetFPS() * 600))
#define TARGET_UPDATE_DELAY	(TARGET_DAMAGE / (TARGET_REPAIR_PER_FRAME \
				    * BLOCK_SZ))

/*
 * Size (pixels) of radius for legal HIT!
 * Was 14 until 4.2. Increased due to `analytical collision detection'
 * which inspects a real circle and not just a square anymore.
 */
#define SHIP_SZ		        16

#define VISIBILITY_DISTANCE	1000.0
#define BALL_RADIUS		10

#define MISSILE_LEN		15
#define TEAM_NOT_SET		0xffff
#define TEAM_NOT_SET_STR	"4095"
#define DEBRIS_TYPES		(8 * 4 * 4)


#ifndef FALSE
#define FALSE   0
#endif
#ifndef TRUE
#define TRUE    1
#endif

#ifdef __GNUC__
#define	INLINE	inline
#else
#define INLINE
#endif /* __GNUC__ */

// fltk uses rand.  It is very bad to redefine it.
#undef	rand
//#define	rand(void)	please dont use rand.

#endif
