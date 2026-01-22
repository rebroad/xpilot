/* $Id: rules.h,v 1.5 2004/05/22 15:18:04 dick Exp $
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
 *  $Log: rules.h,v $
 *  Revision 1.5  2004/05/22 15:18:04  dick
 *  LASER_MODIFIERS becomes LASER_MODS
 *
 *  Revision 1.4  2002/05/18 20:55:35  dick
 *  Update to XPilot-4.5.1
 *
 *  Revision 1.3  2001/07/08 06:02:58  dick
 *  commentality
 *
 *  Revision 1.2  2001/07/07 12:00:41  dick
 *  Rename classes to C++ "Style".  old World becomes theWorld.
 *
 */

#ifndef RULES_H
#define RULES_H

/*
 * Bitfield definitions for playing mode.
 */
#define CRASH_WITH_PLAYER	(1<<0)		// 0001
#define BOUNCE_WITH_PLAYER	(1<<1)		// 0002
#define PLAYER_KILLINGS		(1<<2)		// 0004
#define LIMITED_LIVES		(1<<3)		// 0008
#define TIMING				(1<<4)		// 0010
#define PLAYER_SHIELDING	(1<<6)		// 0020
#define LIMITED_VISIBILITY	(1<<7)		// 0040
#define TEAM_PLAY			(1<<8)		// 0080
#define WRAP_PLAY			(1<<9)		// 0100
#define ALLOW_NUKES			(1<<10)		// 0200
#define ALLOW_CLUSTERS		(1<<11)		// 0400
#define ALLOW_MODIFIERS		(1<<12)		// 0800
#define ALLOW_LASER_MODS	(1<<13)		// 1000
#define	ALLIANCES			(1<<14)		// 2000

/*
 * Client uses only a subset of them:
 */
#define CLIENT_RULES_MASK	(WRAP_PLAY|TEAM_PLAY|TIMING|LIMITED_LIVES|\
							 ALLIANCES)

/*
 * Possible object and player status bits.
 * Needed here because client needs them too.
 * The bits that the client needs must fit into a byte,
 * so the first 8 bitvalues are reserved for that purpose.
 */
#define PLAYING			(1L<<0)		/* 0001 Not returning to base */
#define PAUSE			(1L<<1) 	/* 0002 Must stay below 8 */
#define GAME_OVER		(1L<<2)		/* 0004 Must stay below 8 */
#define THRUSTING		(1L<<3)		/* 0008 not used by client? */
#define SELF_DESTRUCT	(1L<<4)		/* not used by client? */
#define WANT_AUDIO		(1L<<5)		/* 0010 whether client has SOUND */

#define KILLED			(1L<<10)	/* 0400 */
#define GRAVITY			(1L<<11)	/* 0800 */
#define WARPING			(1L<<12)	/* 1000 */
#define WARPED			(1L<<13)	/* 2000 */
#define CONFUSED		(1L<<14)	/* 4000 */
#define FROMCANNON		(1L<<15)	/* 8000 Object from cannon */
#define HOVERPAUSE		(1L<<16)	/* 0001 Hovering pause */
#define RECREATE		(1L<<17)	/* 0002 Recreate ball */
#define FROMBOUNCE		(1L<<18)	/* 0004 Spark from wall bounce */
#define OWNERIMMUNE		(1L<<19)	/* 0008 Owner is immune to object */
#define REPROGRAM		(1L<<20)	/* 0010 Player reprogramming */
#define NOEXPLOSION		(1L<<21)	/* 0020 No ball recreate explosion */
#define COLLISIONSHOVE	(1L<<22)	/* 0040 Collision counts as shove */
#define FINISH			(1L<<23)	/* 0080 Reached race finish */
#define RACE_OVER		(1L<<24)	/* 0100 After finished and score. */
#define RANDOM_ITEM		(1L<<25)	/* 0200 If an item shows up as random */

class Rules
{
public:
    int		lives;
    long	mode;
};

#endif
