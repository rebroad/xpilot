/* $Id: types.h,v 1.10 2004/04/26 07:07:14 dick Exp $
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
 *  $Log: types.h,v $
 *  Revision 1.10  2004/04/26 07:07:14  dick
 *  Define a list of pages that can be displayed on the ScoreTable in the client.
 *  Each page can be controlled by the server (and the server admin tool).
 *
 *  Revision 1.9  2004/02/05 04:49:15  dick
 *  Line up the EmTypes to match with the LOG types
 *
 *  Revision 1.8  2001/10/04 17:33:39  dick
 *  Use uint instead of UINT.
 *
 *  Revision 1.7  2001/09/17 21:05:35  dick
 *  Windows needs a ushort definition
 *
 *  Revision 1.6  2001/08/31 20:50:15  dick
 *  ErrMsgHandler is now global
 *
 *  Revision 1.5  2001/08/26 09:39:43  dick
 *  define uchar
 *
 *  Revision 1.4  2001/08/13 02:32:31  dick
 *  new type ulong
 *
 *  Revision 1.3  2001/07/19 04:02:35  dick
 *  Working on encapsulating options.
 *
 *  Revision 1.2  2001/07/07 12:00:41  dick
 *  Rename classes to C++ "Style".  old World becomes theWorld.
 *
 */

#ifndef	TYPES_H
#define	TYPES_H

typedef unsigned char	u_byte;

/*
 * On some systems an enum is smaller than an int.
 * On others bool is already a builtin type.
 * Using preprocessor macros to circumvent both situations.
 */
#define false	0
#define true	1

/*
 * Windows does all its FPU work in doubles.  Using floats gives warnings
 * and causes everything to be promoted to doubles anyway...
 */
#ifndef _WINDOWS
typedef	float	DFLOAT;
#else
typedef	double	DFLOAT;
#endif

typedef struct { DFLOAT x, y; }	vector;
typedef vector			position;
typedef struct { int x, y; }	ivec;
typedef ivec			ipos;

typedef	unsigned long	ulong;
typedef	unsigned char	uchar;
typedef	unsigned char	ubyte;
typedef	unsigned short	ushort;
typedef	unsigned int	uint;

#ifdef _WINDOWS
# define strncasecmp(__s, __t, __l)	strnicmp(__s, __t, __l)
# define strcasecmp(__s, __t)	stricmp(__s, __t)
#endif

/*
 * Error message handling
 */
enum ErrMsgType {
	EmNone,			// LOGNONE
	EmError,		// LOGERR
	EmWarning,		// LOGMIN
	EmInfo,			// LOGMED
	EmDebug,		// LOGLOTS
	EmAll,			// LOGMAX
	EmAppend	// append this string to the last one displayed
};
typedef void	(*ErrMsgHandler)(void* myThis, ErrMsgType emt, const char* fmt, ...);

inline	int		ErrMsgTypeToLogLevel(ErrMsgType emt) { return((int)emt); }

#endif
