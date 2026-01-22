/* $Id: Other.cpp,v 1.1 2004/06/03 06:04:42 dick Exp $
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
 * $Log: Other.cpp,v $
 * Revision 1.1  2004/06/03 06:04:42  dick
 * struct other_t becomes class Other.
 * array Others becomes ObjList others.
 * shipobj becomes ShipObj.
 *
 */
#include "StdAfx.h"
#include "const.h"
#include "draw.h"		// free_ship_shape

#include "Other.h"

Other*	self = NULL;			/* Player info */

///////////////////////////////////////////////////////////////////////////////
Other::Other()
{
	ship = NULL;
	id = NO_ID;
	warId = NO_ID;
	team = NO_ID;
	life = 0;
	kills = 0;
	deaths = 0;
	score = 0.0;
	kdratio = 0.0;
	ratio = 0.0;
	rate = 0.0;
	rank = 0;
	check = 0;
	round = 0;
	timing = 0;
	timing_loops = 0;
	mychar = 0;
	alliance = 0;
	name_width = 0;		// should go away with use of String...
	name_len = 0;
}

///////////////////////////////////////////////////////////////////////////////
Other::~Other()
{
	if (ship)
		Free_ship_shape(ship);
	ship = NULL;
}
