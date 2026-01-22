/* $Id: portability.h,v 1.24 2005/03/17 22:12:13 kps Exp $
 *
 * XPilot, a multiplayer gravity war game.  Copyright (C) 1991-2001 by
 *
 *      Bj�rn Stabell        <bjoern@xpilot.org>
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
 * $Log: portability.h,v $
 * Revision 1.24  2005/03/17 22:12:13  kps
 * Get rid of warnings from makedepend about "non-portable whitespace".
 *
 * Revision 1.23  2004/02/16 14:30:16  dick
 * server/sched.cpp and friends become serverlib/NetServer
 * which disconnects the udp listen and receive functionality from the game.
 *
 * Revision 1.22  2004/01/26 16:57:01  dick
 * Get_process_id becomes GetProcessID
 *
 * Revision 1.21  2004/01/19 01:32:05  dick
 * Don't undef errno.  Deal with xperrno for our purposes
 *
 * Revision 1.20  2004/01/07 04:35:54  dick
 * More CYGWIN changes
 *
 * Revision 1.19  2004/01/06 04:48:28  dick
 * Add support for building under Cygwin.  Still needs work and the output
 * is X11 and requires something like Cygwin's XWin.exe to run under.
 *
 * Revision 1.18  2002/09/16 21:24:48  dick
 * Stringize GetLoginName().
 *
 * Revision 1.17  2002/08/31 21:58:53  dick
 * IsServerKilled() a macro to help with early aborts of Windows server thread.
 *
 * Revision 1.16  2002/06/30 06:21:35  dick
 * ServerKilled becomes serverKilled
 *
 * Revision 1.15  2002/06/26 06:56:36  dick
 * use xperrno instead of errno, seterrno(0) instead of errno = 0.
 *
 * Revision 1.14  2002/06/26 04:46:25  dick
 * Unix needs PSTR defined
 *
 * Revision 1.13  2002/06/20 22:25:31  dick
 * Bring back strcasecmp for Windows.
 *
 * Revision 1.12  2002/05/20 02:17:44  dick
 * Windows needs errno and seterrno in portability.h.
 * Windows also needs rint() defined if not using fltk.
 *
 * Revision 1.11  2002/05/18 20:55:35  dick
 * Update to XPilot-4.5.1
 *
 * Revision 1.10  2001/08/26 09:43:28  dick
 * Fix the log
 *
 */

/*
 * Include portability related stuff in one file.
 */
#ifndef PORTABILITY_H_INCLUDED
#define PORTABILITY_H_INCLUDED

#ifndef	_WINDOWS
#define	_UNIX	1		// opposite of _WINDOWS
#endif

#ifdef _WINDOWS
#undef max
#undef min

#ifndef	_CYGWIN
#define	strncasecmp(__a, __b, __c)	strnicmp(__a, __b, __c)
#define	vsnprintf	_vsnprintf
#endif

#define strcasecmp(__s, __t)	stricmp(__s, __t)

#define	snprintf	_snprintf

/*
#ifdef	_CYGWIN
typedef u_int           SOCKET;
typedef struct in_addr IN_ADDR;
typedef struct hostent HOSTENT;
typedef struct sockaddr SOCKADDR;
typedef struct sockaddr_in SOCKADDR_IN;
#define INVALID_SOCKET  (SOCKET)(~0)
#define SOCKET_ERROR            (-1)

#endif
*/
#endif /* _WINDOWS */

/*
 * Portability fixes for Sony NeWS.
 */
#ifdef sony_news
#define setvbuf(A,B,C,D)	setlinebuf(A)

typedef unsigned int    sigset_t;

#define sigemptyset(set)        (*(set) = 0)
#define sigfillset(set)         (*(set) = ~(sigset_t)0, 0)
#define sigaddset(set,signo)    (*(set) |= sigmask(signo), 0)
#define sigdelset(set,signo)    (*(set) &= ~sigmask(signo), 0)
#define sigismember(set,signo)  ((*(set) & sigmask(signo)) != 0)

#define SIG_BLOCK		1
#define SIG_UNBLOCK		2
#define SIG_SETMASK		3

extern int sigprocmask(int how, const sigset_t *set, sigset_t *oset);

/*
 * Sony NEWS doesn't have sigaction(), using sigvec() instead.
 */
#define sigaction	sigvec
#define sa_handler	sv_handler
#define sa_mask		sv_mask
#define sa_flags	sv_flags

#endif

/*
 * In Windows, just exiting won't tell the user the reason.
 * So, try to gracefully shutdown just the server thread
 */
#if defined(_WINDOWS) && !defined(_CYGWIN)
extern	int serverKilled;
#define	ServerExit() { serverKilled = TRUE; netServer->StopSched(); return; }
#define	IsServerKilled()	(serverKilled == true)
#else
#define	ServerExit() exit(1);
#define	IsServerKilled()	(false)
#endif

#if defined(_WINDOWS) && !defined(_CYGWIN)
#define	xpassert(_exp) {if (!(#_exp)) { stop_sched(); assert(#_exp); }
#else
#define	xpassert(_exp) assert(_exp)
#endif

/*
 * Macros to block out Windows only code (and never Windows code)
 */
#if defined(_WINDOWS) && !defined(_CYGWIN)
#define IFWINDOWS(x)	x
#else
#define IFWINDOWS(x)
#endif

#if defined(_UNIX) || defined(_CYGWIN)
#define IFNWINDOWS(x)	x
#else
#define IFNWINDOWS(x)
#endif


#if defined(_WINDOWS) && !defined(_CYGWIN)
#define PATHNAME_SEP    '\\'
#else
#define PATHNAME_SEP    '/'
#endif

#if defined(_UNIX) || defined(_CYGWIN)
typedef	int			SOCKET;
#ifndef	SOCKADDR_IN
	typedef	struct sockaddr		SOCKADDR;
	typedef	struct sockaddr_in	SOCKADDR_IN;
	typedef struct in_addr		IN_ADDR;

	typedef struct hostent		HOSTENT;
#	define INVALID_SOCKET  		(SOCKET)(~0)
#	define SOCKET_ERROR     	(-1)
#	define closesocket		close
#	define MAXGETHOSTSTRUCT		1024
#endif
typedef char*	PSTR;
#endif

/*
 * Prototypes for OS function wrappers in portability.c.
 */
class String;
extern int GetProcessId();	/* getpid */
extern void GetLoginName(String& name);

/*
 * Prototypes for other (ANSI C library) wrapper functions.
 */
extern void move_memory(void *dst, void *src, size_t len);

/*
 * Prototypes for testing if we are running under a certain OS.
 */
extern int is_this_windows(void);


/*
 * Round to nearest integer.
 */
#if defined(_WINDOWS) && !defined(FLTK)
extern double rint(double x);
#endif

/*
 * errno
 *
 * Windows defines at least 3 different errno(s) !!!
 * The one we are interested in is WSAGetLastError
 * which deals with network errno
 *
 * XPilot++ should use xperrno only, and we'll map it
 * to the correct one.
 */
#if defined(_UNIX) || defined(_CYGWIN)
#define	xperrno		errno
#else
#define	xperrno		WSAGetLastError()
#endif

#ifndef	_WINDOWS
#define	seterrno(__i)	errno=__i
#else
#define	seterrno(__i)
#endif


#endif /* PORTABILITY_H_INCLUDED */
