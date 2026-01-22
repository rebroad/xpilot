/*
 * Xpilotedit, the cross platform map editor for Xpilot.  Copyright (C) 2001 by
 *
 *      The Xpilot Authors           <xpilot@xpilot.org>
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
 * See the file COPYRIGHT.TXT for current copyright information.
 *
 */
/*
 * $Log: map.h,v $
 * Revision 1.3  2004/01/19 22:08:26  dick
 * MAX_MAP_SIZE is a global const.
 *
 * Revision 1.2  2004/01/19 22:05:06  dick
 * Move MAX_MAP_SIZE from serverlib to common (it *is* a global).
 *
 * Revision 1.1  2004/01/17 19:37:16  dick
 * WildMap is now in the common library. Control and server use this,
 * instead of each having it's own copy.
 *
 */

#ifndef	_MAP_H_
#define	_MAP_H_


/* Blank Space */
#define MAP_SPACE	' '

/* Wall */
#define MAP_REC_RD		'q'
#define MAP_REC_LD		'w'
#define MAP_FILLED		'x'
#define MAP_REC_RU		'a'
#define MAP_REC_LU		's'

/* Cannon */
#define MAP_CAN_LEFT	'd'
#define MAP_CAN_UP		'r'
#define MAP_CAN_DOWN	'c'
#define MAP_CAN_RIGHT	'f'

/* Bases */
#define MAP_BASE		'_'
#define MAP_BASE_ORNT   '$'

/* Misc */
#define MAP_FUEL		'#'
#define MAP_TARGET		'!'
#define MAP_TREASURE	'*'
#define MAP_ITEM_CONC   '%'
#define MAP_FRICTION	'z'
#define MAP_EMPTYTREASURE '^'
#define MAP_ASTEROID_CONC '&'

/* Gravity */
#define MAP_GRAV_POS	'+'
#define MAP_GRAV_NEG	'-'
#define MAP_GRAV_ACWISE	'<'
#define MAP_GRAV_CWISE	'>'

/* Wormholes */
#define MAP_WORM_NORMAL	'@'
#define MAP_WORM_OUT	')'
#define MAP_WORM_IN		'('

/* Currents */
#define MAP_CRNT_UP     'i'
#define MAP_CRNT_LT     'j'
#define MAP_CRNT_RT     'k'
#define MAP_CRNT_DN     'm'

/* Decor */
#define MAP_DEC_RD		't'
#define MAP_DEC_LD		'y'
#define MAP_DEC_FLD		'b'
#define MAP_DEC_RU		'g'
#define MAP_DEC_LU		'h'

#endif// _MAP_H_
