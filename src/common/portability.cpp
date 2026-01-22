/* $Id: portability.cpp,v 1.10 2007/02/17 06:12:06 dick Exp $
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
 * This file contains function wrappers around OS specific services.
 */
#include "StdAfx.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#if defined(_UNIX) || defined(_CYGWIN)
# include <unistd.h>
# include <pwd.h>
#endif

#ifdef PLOCKSERVER
# if defined(__linux__)
#  include <sys/mman.h>
# else
#  include <sys/lock.h>
# endif
#endif

#if defined(_WINDOWS) && !defined(_CYGWIN)
# include <windows.h>
# include <process.h>
#endif

#if defined(_WINDOWS)
# include <math.h>
#endif

#include "cstring.h"
#include "version.h"
#include "config.h"
#include "portability.h"
#include "commonproto.h"


char portability_version[] = VERSION;

#ifdef	_WINDOWS
PCSTR	eol = "\r\n";
PCSTR	fileEol = "\n";
#else
PCSTR	eol = "\n";
PCSTR	fileEol = "\n";
#endif


int GetProcessId(void)
{
#if defined(_WINDOWS) && !defined(_CYGWIN)
    return _getpid();
#else
    return getpid();
#endif
}


void GetLoginName(String& name)
{
#if defined(_WINDOWS) && !defined(_CYGWIN)
	char	buf[64];
    DWORD nsize = 64;
    GetUserName(buf, &nsize);
    buf[63] = '\0';
	name = buf;
#else
    /* Unix */
    struct passwd *pwent = getpwuid(geteuid());
    name = pwent->pw_name;
#endif
}


#ifdef sony_news
int sigprocmask(int how, const sigset_t *set, sigset_t *oset)
{
    int			mask;

    if (how == SIG_BLOCK) {
	mask = sigblock(0) | *set;
    }
    else if (how == SIG_UNBLOCK) {
	mask = sigblock(0) & ~(*set);
    }
    else if (how == SIG_SETMASK) {
	mask = *set;
    }
    else {
	mask = sigblock(0);
    }
    mask = sigsetmask(mask);
    if (oset != NULL) {
	*oset = mask;
    }

    return 0;
}
#endif	/* sony_news */

void move_memory(void *dst, void *src, size_t len)
{
#if defined(__hpux) || defined(__apollo) || defined(SVR4) || defined(_SEQUENT_) || defined(SYSV) || defined(_WINDOWS)
        memmove(dst, src, len);
#else
        bcopy(src, dst, len);
#endif
}


int is_this_windows()
{
#if defined(_WINDOWS) && !defined(_CYGWIN)
    return 1;
#else
    return 0;
#endif
}


/*
 * Round to nearest integer.
 */
#if defined(_WINDOWS) && !defined(FLTK)
double rint(double x)
{
    return floor((x < 0.0) ? (x - 0.5) : (x + 0.5));
}
#endif


