/* 
 *
 * XPilot, a multiplayer gravity war game.  Copyright (C) 1991-2005 by
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

#ifndef	VERSION_H
#define	VERSION_H

#if defined(__hpux)
#   pragma COPYRIGHT_DATE	"1991-2005"
#   pragma COPYRIGHT		"Dick Balaska, Jarrod Miller, Bert Gijsbers, Bjørn Stabell & Ken Ronny Schouten"
#   pragma VERSIONID		"XPilot 5.0.1a3"
#endif

#define VERSION			"5.0.1a3"
#ifdef	_WINDOWS
#define	TITLE			"5.0.1a3-NT"
#else
#define TITLE			"XPilot 5.0.1a3"
#endif
#define AUTHORS			"Dick Balaska, Jarrod Miller, Bert Gijsbers, Bjørn Stabell & Ken Ronny Schouten"
#define COPYRIGHT		"Copyright © 1991-2005 by Dick Balaska, Jarrod Miller, Bert Gijsbers, Bjørn Stabell & Ken Ronny Schouten"

#endif /* VERSION_H */
