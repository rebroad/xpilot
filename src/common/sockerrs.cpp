/* $Id: sockerrs.cpp,v 1.3 2004/02/03 03:39:52 dick Exp $
 *
 * XPwhere - Where in the world can i find people playing XPilot.
 *      Copyright (C) 2001 by
 *
 *      The XPilot Authors   <xpilot@xpilot.org>
 *      Dick Balaska         <dick@xpilot.org>
 *      Jarrod Miller        <jarrod@xpilot.org>
 *      Bert Gijsbers        <bert@xpilot.org>
 *      Ken Ronny Schouten   <ken@xpilot.org>
 *      Bj�rn Stabell        <bjoern@xpilot.org>
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
 * $Log: sockerrs.cpp,v $
 * Revision 1.3  2004/02/03 03:39:52  dick
 * Add Windows precompiled header support.
 *
 * Revision 1.2  2004/01/06 04:48:28  dick
 * Add support for building under Cygwin.  Still needs work and the output
 * is X11 and requires something like Cygwin's XWin.exe to run under.
 *
 * Revision 1.1  2001/07/26 10:57:13  dick
 * Convert socket error numbers into text
 *
 */

/***************************************************************************\
*  sockerrs.cpp - Translate winsock error numbers into text					*
*  Copyright� 1996-1999 - BuckoSoft, Inc.									*
*																			*
*																			*
*  $Id: sockerrs.cpp,v 1.3 2004/02/03 03:39:52 dick Exp $					*
\***************************************************************************/

/*
 *  $Log: sockerrs.cpp,v $
 *  Revision 1.3  2004/02/03 03:39:52  dick
 *  Add Windows precompiled header support.
 *
 *  Revision 1.2  2004/01/06 04:48:28  dick
 *  Add support for building under Cygwin.  Still needs work and the output
 *  is X11 and requires something like Cygwin's XWin.exe to run under.
 *
 *  Revision 1.1  2001/07/26 10:57:13  dick
 *  Convert socket error numbers into text
 *
 *  Revision 1.3  1999/11/15 19:34:35  dick
 *  Add the "Live" CVS log entries to the files
 *
 */
#include "StdAfx.h"

#include "sockerrs.h"

struct Wsockerrs {
	int		error;
	char*	text;
} Wsockerrs;
struct Wsockerrs wsockerrs[] = {
EINTR, "EINTR",
EBADF, "EBADF",
EACCES, "EACCES",
EFAULT, "EFAULT",
EINVAL, "EINVAL",
EMFILE, "EMFILE",

ENOENT, "ENOENT",
EWOULDBLOCK, "EWOULDBLOCK",
EINPROGRESS, "EINPROGRESS",
EALREADY, "EALREADY",
ENOTSOCK, "ENOTSOCK",
EDESTADDRREQ, "EDESTADDRREQ",
EMSGSIZE, "EMSGSIZE",
EPROTOTYPE, "EPROTOTYPE",
ENOPROTOOPT, "ENOPROTOOPT",
EPROTONOSUPPORT, "EPROTONOSUPPORT",
ESOCKTNOSUPPORT, "ESOCKTNOSUPPORT",
EOPNOTSUPP, "EOPNOTSUPP",
EPFNOSUPPORT, "EPFNOSUPPORT",
EAFNOSUPPORT, "EAFNOSUPPORT",
EADDRINUSE, "EADDRINUSE",
EADDRNOTAVAIL, "EADDRNOTAVAIL",
ENETDOWN, "ENETDOWN",
ENETUNREACH, "ENETUNREACH",
ENETRESET, "ENETRESET",
ECONNABORTED, "ECONNABORTED",
ECONNRESET, "ECONNRESET",
ENOBUFS, "ENOBUFS",
EISCONN, "EISCONN",
ENOTCONN, "ENOTCONN",
ESHUTDOWN, "ESHUTDOWN",
ETOOMANYREFS, "ETOOMANYREFS",
ETIMEDOUT, "ETIMEDOUT",
ECONNREFUSED, "ECONNREFUSED",
ELOOP, "ELOOP",
ENAMETOOLONG, "ENAMETOOLONG",
EHOSTDOWN, "EHOSTDOWN",
EHOSTUNREACH, "EHOSTUNREACH",
ENOTEMPTY, "ENOTEMPTY",
EUSERS, "EUSERS",
EDQUOT, "EDQUOT",
ESTALE, "ESTALE",
EREMOTE, "EREMOTE",

#ifdef	_UNIX
EPERM, "EPERM",
ENOENT, "ENOENT",
ESRCH, "ESRCH",
EINTR, "EINTR",
EIO, "EIO",
ENXIO, "ENXIO",
E2BIG, "E2BIG",
ENOEXEC, "ENOEXEC",
EBADF, "EBADF",
ECHILD, "ECHILD",
EAGAIN, "EAGAIN",
ENOMEM, "ENOMEM",
EACCES, "EACCES",
EFAULT, "EFAULT",
ENOTBLK, "ENOTBLK",
EBUSY, "EBUSY",
EEXIST, "EEXIST",
EXDEV, "EXDEV",
ENODEV, "ENODEV",
ENOTDIR, "ENOTDIR",
EISDIR, "EISDIR",
EINVAL, "EINVAL",
ENFILE, "ENFILE",
EMFILE, "EMFILE",
ENOTTY, "ENOTTY",
ETXTBSY, "ETXTBSY",
EFBIG, "EFBIG",
ENOSPC, "ENOSPC",
ESPIPE, "ESPIPE",
EROFS, "EROFS",
EMLINK, "EMLINK",
EPIPE, "EPIPE",
EDOM, "EDOM",
ERANGE, "ERANGE",
EDEADLK, "EDEADLK",
ENAMETOOLONG, "ENAMETOOLONG",
ENOLCK, "ENOLCK",
ENOSYS, "ENOSYS",
ENOTEMPTY, "ENOTEMPTY",
ELOOP, "ELOOP",
ENOMSG, "ENOMSG",
EIDRM, "EIDRM",
ECHRNG, "ECHRNG",
EL2NSYNC, "EL2NSYNC",
EL3HLT, "EL3HLT",
EL3RST, "EL3RST",
ELNRNG, "ELNRNG",
EUNATCH, "EUNATCH",
ENOCSI, "ENOCSI",
EL2HLT, "EL2HLT",
EBADE, "EBADE",
EBADR, "EBADR",
EXFULL, "EXFULL",
ENOANO, "ENOANO",
EBADRQC, "EBADRQC",
EBADSLT, "EBADSLT",
#endif

#if defined(_WINDOWS) && !defined(_CYGWIN)
EPROCLIM, "EPROCLIM",
EDISCON, "EDISCON",

/*
 * Extended Windows Sockets error constant definitions
 */
WSASYSNOTREADY, "WSASYSNOTREADY",
WSAVERNOTSUPPORTED, "WSAVERNOTSUPPORTED",
WSANOTINITIALISED, "WSANOTINITIALISED",
#endif

-1, "UNKNOWN"
};

const char*	GetSockErrText(int	error)
{
#ifndef	_WINDOWS
//	error -= 10000;
#endif
	int		i;
	for (i=0; wsockerrs[i].error != -1; i++)
		if (wsockerrs[i].error == error)
			break;
	return(wsockerrs[i].text);
}
