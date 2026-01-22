/* $Id: error.cpp,v 1.14 2004/02/03 03:39:52 dick Exp $
 *
 * Adapted from 'The UNIX Programming Environment' by Kernighan & Pike
 * and an example from the manualpage for vprintf by
 * Gaute Nessan, University of Tromsoe (gaute@staff.cs.uit.no).
 *
 * Modified by Bjoern Stabell <bjoern@xpilot.org>.
 * Windows mods and memory leak detection by Dick Balaska <dick@xpilot.org>.
 */
/*
 * $Log: error.cpp,v $
 * Revision 1.14  2004/02/03 03:39:52  dick
 * Add Windows precompiled header support.
 *
 * Revision 1.13  2004/01/30 15:54:50  dick
 * Always log to the window, even if we popped up a messagebox
 *
 * Revision 1.12  2004/01/17 19:34:58  dick
 * Don't put up dialog boxes on Windows server (unless we're debug).
 *
 * Revision 1.11  2004/01/06 04:48:28  dick
 * Add support for building under Cygwin.  Still needs work and the output
 * is X11 and requires something like Cygwin's XWin.exe to run under.
 *
 * Revision 1.10  2002/07/12 15:38:15  dick
 * Deadwood delete (Windows specific yea!)
 *
 * Revision 1.9  2002/06/23 06:20:50  dick
 * WinTrace() only appears in Windows
 *
 * Revision 1.8  2002/06/20 21:52:52  dick
 * Add WinTrace(), which gives us output for programs without MFC.
 *
 * Revision 1.7  2002/05/18 20:55:35  dick
 * Update to XPilot-4.5.1
 *
 * Revision 1.6  2001/08/14 21:42:16  dick
 * Windows puts up a MessageBox for error and fatal, to let the user actually
 * see the message.
 *
 * Revision 1.5  2001/08/09 07:56:41  dick
 * Linux cleanups
 *
 */
#include "StdAfx.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <time.h>

#if defined(_WINDOWS)
#include <windows.h>
#	ifdef	_XPILOTNTSERVER_
#		include "../server/NT/winServer.h"
#	endif
	static void Win_show_error(char *errmsg, bool msgBox = false);
#endif

#include "version.h"
#include "config.h"
#include "const.h"
#include "error.h"
#include "portability.h"
#include "commonproto.h"
#include "cstring.h"


#undef HAVE_STDARG
#undef HAVE_VARARG
#if defined(_UNIX) || defined(_CYGWIN)
# if (defined(__STDC__) && !defined(__sun__) || defined(__cplusplus))
#  define HAVE_STDARG 1
# else
#  define HAVE_VARARG 1
# endif
#endif


char error_version[] = VERSION;

extern const char *showtime(void);


/*
 * This file defines two entry points:
 *
 * init_error()		- Initialize the error routine, accepts program name
 *			  as input.
 * error()		- perror() with printf functionality.
 */



/*
 * File local static data.
 */
#define	MAX_PROG_LENGTH	32
static char		progname[MAX_PROG_LENGTH];



static const char* prog_basename(const char *prog)
{
#ifndef _WINDOWS
    char *p;

    p = strrchr(prog, '/');

    return (p != NULL) ? (p + 1) : prog;
#else
    return "xpilot";
#endif
}


/*
 * Functions.
 */
void init_error(const char *prog)
{
    const char *p = prog_basename(prog);   /* Beautify arv[0] */

    strlcpy(progname, p, MAX_PROG_LENGTH);
}



#if HAVE_STDARG
/*
 * Ok, let's do it the ANSI C way.
 */
void error(const char *fmt, ...)
{
    va_list	 ap;
    int		 e = errno;

    va_start(ap, fmt);

    if (progname[0] != '\0') {
	fprintf(stderr, "%s: ", progname);
    }

    vfprintf(stderr, fmt, ap);

    if (e != 0) {
	fprintf(stderr, ": (%s)", strerror(e));
    }
    fprintf(stderr, "\n");

    va_end(ap);
}

void warn(const char *fmt, ...)
{
    int		len;
    va_list	ap;

    va_start(ap, fmt);

    if (progname[0] != '\0') {
	fprintf(stderr, "%s: ", progname);
    }

    vfprintf(stderr, fmt, ap);

    len = strlen(fmt);
    if (len == 0 || fmt[len - 1] != '\n') {
	fprintf(stderr, "\n");
    }

    va_end(ap);
}

void fatal(const char *fmt, ...)
{
    va_list	 ap;

    va_start(ap, fmt);

    if (progname[0] != '\0') {
	fprintf(stderr, "%s: ", progname);
    }

    vfprintf(stderr, fmt, ap);

    fprintf(stderr, "\n");

    va_end(ap);

    exit(1);
}

void dumpcore(const char *fmt, ...)
{
    va_list	 ap;

    va_start(ap, fmt);

    if (progname[0] != '\0') {
	fprintf(stderr, "%s: ", progname);
    }

    vfprintf(stderr, fmt, ap);

    fprintf(stderr, "\n");

    va_end(ap);

    abort();
}

#endif

#if defined(_WINDOWS) && !defined(_CYGWIN)
static void Win_show_error(char *s, bool msgBox)
{
//    IFWINDOWS( Trace("Error: %s\n", s); )
	if (msgBox)
	{
		MessageBox(NULL, s, "Error", MB_OK | MB_TASKMODAL);
//		return;
	}

	/* putting up a message box on the server is a bad thing.
	   It kinda halts the server, which is a bad thing to do for
	   the simple info messages (nick in use) that call this routine
	*/
	xpprintf("%s %s\n", showtime(), s);
}

#ifdef	_DEBUG
#define	PAUSEONERROR	true
#else
#define	PAUSEONERROR	false
#endif
void error(const char *fmt, ...)
{
    va_list	ap;
    char	s[512];

    va_start(ap, fmt);

    vsprintf(s, fmt, ap);

    Win_show_error(s, PAUSEONERROR);

    va_end(ap);
}

void warn(const char *fmt, ...)
{
    va_list	ap;
    char	s[512];

    va_start(ap, fmt);

    vsprintf(s, fmt, ap);

    Win_show_error(s, false);

    va_end(ap);
}

void fatal(const char *fmt, ...)
{
    va_list	ap;
    char	s[512];

    va_start(ap, fmt);

    vsprintf(s, fmt, ap);

    Win_show_error(s, true);

    va_end(ap);

    exit(1);
}

void dumpcore(const char *fmt, ...)
{
    va_list	ap;
    char	s[512];

    va_start(ap, fmt);

    vsprintf(s, fmt, ap);

    Win_show_error(s, true);

    va_end(ap);

    exit(1);
}

#if defined(_WINDOWS) && !defined(_CYGWIN)
void WinTrace(const char *fmt, ...)
{
    va_list	ap;
    char	s[512];

    va_start(ap, fmt);
    vsprintf(s, fmt, ap);

    OutputDebugString(s);

    va_end(ap);
}
#endif

#endif
