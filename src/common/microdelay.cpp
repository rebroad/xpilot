/*-
 * usleep.c - OS dependant implementation of usleep().
 *
 * Copyright (c) 1991 by Patrick J. Naughton.
 *
 * Revision History:
 * 30-Aug-90: written.
 *
 * 07-Dec-94: Bert Gijsbers
 *	Changed "void usleep(unsigned long)" to "int usleep(unsigned)"
 *	as this is what it seems to be on systems which do have usleep(3) (AIX).
 *	Changed usleep into micro_delay to forego any possible prototype clashes.
 */
/*
 * $Log: microdelay.cpp,v $
 * Revision 1.3  2004/02/03 03:39:52  dick
 * Add Windows precompiled header support.
 *
 * Revision 1.2  2004/01/06 04:48:28  dick
 * Add support for building under Cygwin.  Still needs work and the output
 * is X11 and requires something like Cygwin's XWin.exe to run under.
 *
 * Revision 1.1  2001/08/07 22:12:32  dick
 * usleep becomes microdelay
 *
 * Revision 1.1  2001/08/07 20:51:55  dick
 * move client usleep/micro_delay to common usleep/usleep.
 *
 * Revision 1.2  2001/07/27 05:47:39  dick
 * Client builds and runs in C++
 *
 */
#include "StdAfx.h"

#include <time.h>
#include <sys/types.h>

#if !defined(_WINDOWS) || defined(_CYGWIN)
# include <unistd.h>
# ifndef __hpux
#  include <sys/time.h>
# endif
#endif

#if defined(_WINDOWS) && !defined(_CYGWIN)
# include "NT/winNet.h"
#endif

#include "types.h"


//int usleep(unsigned usec);

int microdelay(unsigned usec)
{
#if 0 /* SYSV */
    poll((struct poll *) 0, (size_t) 0, usec / 1000);	/* ms RES */
#endif
    struct timeval timeout;
    timeout.tv_usec = usec % (unsigned long) 1000000;
    timeout.tv_sec = usec / (unsigned long) 1000000;
    (void) select(0, NULL, NULL, NULL, &timeout);

    return 0;
}
