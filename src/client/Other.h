/* $Id: Other.h,v 1.1 2004/06/03 06:04:42 dick Exp $
 *
 * Other - Describe an other player (as well as us :-/ )
 *
 * client - the user interface to the game.
 *
 * XPilot, a multiplayer gravity war game.	Copyright (C) 1991-2004 by
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
 * $Log: Other.h,v $
 * Revision 1.1  2004/06/03 06:04:42  dick
 * struct other_t becomes class Other.
 * array Others becomes ObjList others.
 * shipobj becomes ShipObj.
 *
 */

#include "Obj.h"
#include "types.h"

class ShipObj;

class Other : public Obj
{
public:
	Other();
	virtual	~Other();
    String		real;
    String		host;
    short		id;
    short		warId;
    short		team;
    short		life;
	short		kills;
	short		deaths;
	DFLOAT		kdratio;	// kills/deaths
    DFLOAT		ratio;
    DFLOAT		score;
	DFLOAT		rate;
	int			rank;
	String		ipVer;
    short		check;
    short		round;
    short		timing;
    long		timing_loops;
    short		mychar;
	short		alliance;
    short		name_width;	/* In pixels */
    short		name_len;	/* In bytes */
    ShipObj*	ship;
};

extern Other*	self;			/* Player info */
