/* $Id: showtime.cpp,v 1.4 2004/02/03 03:39:52 dick Exp $
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
 */
/*
 * $Log: showtime.cpp,v $
 * Revision 1.4  2004/02/03 03:39:52  dick
 * Add Windows precompiled header support.
 *
 * Revision 1.3  2002/07/09 22:54:04  dick
 * Use strftime
 *
 */
#include "StdAfx.h"

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#ifndef _WINDOWS
# ifndef __hpux
#  include <sys/time.h>
# endif
#endif

#include "cstring.h"



const char *showtime(void)
{
    time_t		now;
    static time_t	past = 0;
    struct tm		*tmp;
    static char		buf[80];

    time(&now);
	if (now != past) {
		tmp = localtime(&now);
#if 1
		static PCSTR formatString = "%d %b %H:%M:%S ";
		strftime(buf, 79, formatString, tmp);
#else
		// Old non-strftime way
		static char		month_names[13][4] = {
						"Jan", "Feb", "Mar", "Apr", "May", "Jun",
						"Jul", "Aug", "Sep", "Oct", "Nov", "Dec",
						"Bug"
		};
		sprintf(buf, "%02d %s %02d:%02d:%02d ",
				tmp->tm_mday, month_names[tmp->tm_mon],
				tmp->tm_hour, tmp->tm_min, tmp->tm_sec);
#endif
		past = now;
	}

	return buf;
}

