/* $Id: draw.h,v 1.11 2004/06/03 06:05:36 dick Exp $
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
 *  $Log: draw.h,v $
 *  Revision 1.11  2004/06/03 06:05:36  dick
 *  shipobj becomes ShipObj
 *
 *  Revision 1.10  2004/01/08 18:10:50  dick
 *  Update to XPilot 4.5.5beta
 *
 *  Revision 1.9  2002/08/01 14:57:07  dick
 *  Remove reference to theWorld
 *
 *  Revision 1.8  2002/07/11 16:36:03  dick
 *  Update to XPilot-4.5.4beta
 *
 *  Revision 1.7  2001/10/04 17:33:19  dick
 *  Include file cleanup
 *
 *  Revision 1.6  2001/09/11 07:33:45  dick
 *  Encapsulate all client resources into new class Ini.
 *
 *  Revision 1.5  2001/08/09 07:31:32  dick
 *  shipobj becomes a basic class
 *
 *  Revision 1.4  2001/07/30 01:08:04  dick
 *  Ship parsing uses const(s).
 *
 *  Revision 1.3  2001/07/23 21:08:36  dick
 *  Encapsulate ShipMass, ShotsMax, ShotsLife, ShotsMass, FPS.
 *
 *  Revision 1.2  2001/07/07 12:00:41  dick
 *  Rename classes to C++ "Style".  old World becomes theWorld.
 *
 */

#ifndef	DRAW_H
#define	DRAW_H

#include "types.h"
#include "const.h"
#include "cstring.h"

/*
 * Abstract (non-display system specific) drawing definitions.
 *
 * This file should not contain any X window stuff.
 */

/*
 * The server supports only 4 colors, except for spark/debris, which
 * may have 8 different colors.
 */

#define BLACK		    0
#define WHITE		    1
#define BLUE		    2
#define RED		    3

/*
 * Windows deals in Pens, not Colors.  So each pen has to have all of its
 * attributes defined.
 */
#if defined(_WINDOWS) && !defined(PENS_OF_PLENTY)
#define	CLOAKCOLOROFS	15		/* colors 16 and 17 are dashed white/blue */
#define	MISSILECOLOR	18		/* wide white pen */
#define	LASERCOLOR		19		/* wide red pen */
#define	LASERTEAMCOLOR	20		/* wide blue pen */
#define	FUNKCOLORS		6		/* 6 funky colors here (15-20) */
#endif

/*
 * The minimum and maximum playing window sizes supported by the server.
 */
#define MIN_VIEW_SIZE	    384
#define MAX_VIEW_SIZE	    1024
#define DEF_VIEW_SIZE	    768

#define DSIZE		    4	    /* Size of diamond (on radar) */

#define MSG_DURATION	    1024
#define MSG_FLASH	    892

#define TITLE_DELAY	    500
#define	UPDATE_SCORE_DELAY  (GetFPS())
#define CONTROL_DELAY	    100

/*
 * Please don't change any of these maxima.
 * It will create incompatibilities and frustration.
 */
#define MIN_SHIP_PTS	    3
#define MAX_SHIP_PTS	    24
#define MAX_GUN_PTS	    3
#define MAX_LIGHT_PTS	    3
#define MAX_RACK_PTS	    4

class ShipObj {			/* Defines wire-obj, i.e. ship */
public:
	ShipObj();
    position	*pts[MAX_SHIP_PTS];	/* the shape rotated many ways */
    int		num_points;		/* total points in object */
    position	engine[RES];		/* Engine position */
    position	m_gun[RES];		/* Main gun position */
    int		num_l_gun,
		num_r_gun,
		num_l_rgun,
		num_r_rgun;		/* number of additional cannons */
    position	*l_gun[MAX_GUN_PTS],	/* Additional cannon positions, left*/
		*r_gun[MAX_GUN_PTS],	/* Additional cannon positions, right*/
		*l_rgun[MAX_GUN_PTS],	/* Additional rear cannon positions, left*/
		*r_rgun[MAX_GUN_PTS];	/* Additional rear cannon positions, right*/
    int		num_l_light,		/* Number of lights */
		num_r_light;
    position	*l_light[MAX_LIGHT_PTS], /* Left and right light positions */
		*r_light[MAX_LIGHT_PTS];
    int		num_m_rack;		/* Number of missile racks */
    position	*m_rack[MAX_RACK_PTS];
    int		shield_radius;		/* Radius of shield used by client. */

    String	name;
    String	author;
};

extern ShipObj*	Default_ship(void);
extern void		Free_ship_shape(ShipObj* w);
extern ShipObj*	Parse_shape_str(PCSTR str);
extern ShipObj*	Convert_shape_str(PCSTR str);
extern void		Calculate_shield_radius(ShipObj* w);
extern int		Validate_shape_str(PCSTR str);
extern void		Convert_ship_2_string(ShipObj* w, char* buf, char *ext,
				  unsigned shape_version);
void Rotate_point(position pt[RES]);

extern double rfrac(void);

extern int mod(int x, int y);

#endif
