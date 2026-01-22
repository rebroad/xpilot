/* $Id: sockerrs.h,v 1.2 2004/01/06 04:48:28 dick Exp $
 *
 * XPwhere - Where in the world can i find people playing XPilot.
 *      Copyright (C) 2001 by
 *
 *      The XPilot Authors   <xpilot@xpilot.org>
 *      Dick Balaska         <dick@xpilot.org>
 *      Jarrod Miller        <jarrod@xpilot.org>
 *      Bert Gijsbers        <bert@xpilot.org>
 *      Ken Ronny Schouten   <ken@xpilot.org>
 *      Bjørn Stabell        <bjoern@xpilot.org>
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
 * $Log: sockerrs.h,v $
 * Revision 1.2  2004/01/06 04:48:28  dick
 * Add support for building under Cygwin.  Still needs work and the output
 * is X11 and requires something like Cygwin's XWin.exe to run under.
 *
 * Revision 1.1  2001/07/26 10:57:13  dick
 * Convert socket error numbers into text
 *
 */

/***************************************************************************\
*  wsockerrs.h - Translate winsock error numbers into text					*
*  Copyright© 1996-1998 - BuckoSoft, Inc.									*
*																			*
*																			*
*  $Id: sockerrs.h,v 1.2 2004/01/06 04:48:28 dick Exp $						*
\***************************************************************************/

/*
 *  $Log: sockerrs.h,v $
 *  Revision 1.2  2004/01/06 04:48:28  dick
 *  Add support for building under Cygwin.  Still needs work and the output
 *  is X11 and requires something like Cygwin's XWin.exe to run under.
 *
 *  Revision 1.1  2001/07/26 10:57:13  dick
 *  Convert socket error numbers into text
 *
 *  Revision 1.4  1999/11/15 19:34:35  dick
 *  Add the "Live" CVS log entries to the files
 *
 */

#ifndef	_SOCKERRS_H_
#define	_SOCKERRS_H_

#if defined(_WINDOWS) && !defined(_CYGWIN)
#include <winsock.h>

#ifndef	EINTR
// M$ decided to include these as of VC++6.0
#define	EINTR					WSAEINTR
#define	EBADF					WSAEBADF
#define	EACCES					WSAEACCES
#define	EFAULT					WSAEFAULT
#define	EINVAL					WSAEINVAL
#define	EMFILE					WSAEMFILE
#define	ENOENT					2
#define ENAMETOOLONG            WSAENAMETOOLONG
#define ENOTEMPTY               WSAENOTEMPTY
#endif

#define EWOULDBLOCK             WSAEWOULDBLOCK
#define EINPROGRESS             WSAEINPROGRESS
#define EALREADY                WSAEALREADY
#define ENOTSOCK                WSAENOTSOCK
#define EDESTADDRREQ            WSAEDESTADDRREQ
#define EMSGSIZE                WSAEMSGSIZE
#define EPROTOTYPE              WSAEPROTOTYPE
#define ENOPROTOOPT             WSAENOPROTOOPT
#define EPROTONOSUPPORT         WSAEPROTONOSUPPORT
#define ESOCKTNOSUPPORT         WSAESOCKTNOSUPPORT
#define EOPNOTSUPP              WSAEOPNOTSUPP
#define EPFNOSUPPORT            WSAEPFNOSUPPORT
#define EAFNOSUPPORT            WSAEAFNOSUPPORT
#define EADDRINUSE              WSAEADDRINUSE
#define EADDRNOTAVAIL           WSAEADDRNOTAVAIL
#define ENETDOWN                WSAENETDOWN
#define ENETUNREACH             WSAENETUNREACH
#define ENETRESET               WSAENETRESET
#define ECONNABORTED            WSAECONNABORTED
#define ECONNRESET              WSAECONNRESET
#define ENOBUFS                 WSAENOBUFS
#define EISCONN                 WSAEISCONN
#define ENOTCONN                WSAENOTCONN
#define ESHUTDOWN               WSAESHUTDOWN
#define ETOOMANYREFS            WSAETOOMANYREFS
#define ETIMEDOUT               WSAETIMEDOUT
#define ECONNREFUSED            WSAECONNREFUSED
#define ELOOP                   WSAELOOP
#define EHOSTDOWN               WSAEHOSTDOWN
#define EHOSTUNREACH            WSAEHOSTUNREACH
#define EPROCLIM                WSAEPROCLIM
#define EUSERS                  WSAEUSERS
#define EDQUOT                  WSAEDQUOT
#define ESTALE                  WSAESTALE
#define EREMOTE                 WSAEREMOTE
#define	EDISCON					WSAEDISCON

#else
//////////////// UNIX
#include <errno.h>

#endif

const char*	GetSockErrText(int	error);

#endif		// _SOCKERRS_H_
