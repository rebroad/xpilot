/* $Id: Network.cpp,v 1.8 2005/03/17 22:12:13 kps Exp $
 *
 * Linkable netword objects
 *
 *      Copyright (C) 2002 by
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
 * $Log: Network.cpp,v $
 * Revision 1.8  2005/03/17 22:12:13  kps
 * Get rid of warnings from makedepend about "non-portable whitespace".
 *
 * Revision 1.7  2004/02/03 03:39:53  dick
 * Add Windows precompiled header support.
 *
 * Revision 1.6  2004/01/06 04:48:28  dick
 * Add support for building under Cygwin.  Still needs work and the output
 * is X11 and requires something like Cygwin's XWin.exe to run under.
 *
 * Revision 1.5  2002/07/30 21:56:37  jlmiller
 *
 * must include <string.h>
 *
 * Revision 1.4  2002/07/09 22:53:32  dick
 * Add showtime to xpprintf()
 *
 * Revision 1.3  2002/06/26 04:45:01  dick
 * Use xpprintf, not bprintf
 *
 * Revision 1.2  2002/06/25 06:58:37  dick
 * Windows doesn't use MFC for new xpilot++ work.
 *
 * Revision 1.1  2002/06/25 04:41:59  dick
 * A base class used by the Http server.  This should probably be merged into Sock
 * and this class should go away.
 *
 */
#include "StdAfx.h"

#if defined(_WINDOWS) && !defined(_CYGWIN)
#	include <windows.h>
#else
#	include <unistd.h>
#	include <sys/socket.h>
#	include <arpa/inet.h>
#	include <stdarg.h>
#	include <sys/ioctl.h>
#endif
#include <string.h>

//#include "BuckoRoot.h"
#include "CfgBase.h"
#include "Network.h"
#include "sockerrs.h"
#include "config.h"
#include "portability.h"
#include "xpprintf.h"

PCSTR	s_Host		= "Host";
///////////////////////////////////////////////////////////////////////////////
Network::Network()
{
	sock = 0;
	retryCount = 0;
	retryTime = 0;
	nosock = FALSE;
}

///////////////////////////////////////////////////////////////////////////////
Network::~Network()
{
}

///////////////////////////////////////////////////////////////////////////////
bool	Network::SetRetryTime()
{
	time_t now = time(NULL);
	if      (!retryCount)		// attempt 0
		retryTime = now + 2;	// retry in 2 seconds
	else if (retryCount < 4)	// attempts 1,2,3
		retryTime = now + 20;	// retry in 20 seconds
	else if (retryCount < 10)	// >  1 minute  attempts 4,5,6,7,8,9
		retryTime = now + 60;	// retry in 60 seconds
	else if (retryCount < 30)	// > 11 minutes attempts 10-29
		retryTime = now + 600;	// retry in 10 minutes
	else if (retryCount < 50)	// > 3.5 hours attempts 30-49
		retryTime = now + 3600;	// retry in 1 hour
	else if (retryCount < 73)	// > 24 hours
		retryTime = now + 21600;// retry in 6 hours
	else						// > 2.5 days
	{
		retryTime = now + 86400;// retry in 1 day
		return(FALSE);			// and abandon
	}
	retryCount++;
	return(TRUE);
}

///////////////////////////////////////////////////////////////////////////////
int Network::SetBlockingMode()
{
	int		ret = 0;
	u_long	argp = 0;
#if defined(_WINDOWS) && !defined(_CYGWIN)
	if (GetCfg()->hWnd)
		ret = WSAAsyncSelect(sock, GetCfg()->hWnd,
							 wMsg, NULL);
	ret = ioctlsocket(sock, FIONBIO, &argp);			// set blocking mode
#else
	ret = ioctl(sock, FIONBIO, &argp);			// set blocking mode
#endif
	if (ret)
		xpprintf("network: can't set %d to be a blocking socket %d (%s)\n", sock, h_errno, GetSockErrText(h_errno));
//	mode = nm_blocking;
	return ret;

}

///////////////////////////////////////////////////////////////////////////////
int Network::SetNonBlockingMode()
{
	int		ret = 0;
	u_long	argp = 1;
#if defined(_WINDOWS) && !defined(_CYGWIN)
	SOTRACE("SetNonBlockingMode: socket %d\n", sock);
	ret = WSAAsyncSelect(sock, GetCfg()->hWnd,
						 wMsg, FD_ACCEPT|FD_READ|FD_CLOSE);
	if (ret)
	{
		xpprintf("Network: WSAAsyncSelect failed %d <%s>\n",h_errno, GetSockErrText(h_errno));
		return(ret);
	}
#else
	ret = ioctl(sock, FIONBIO, &argp);			// set blocking mode
	if (ret)
		xpprintf("network: can't set %d to be a nonblocking socket %d (%s)\n", sock, h_errno, GetSockErrText(h_errno));
#endif
//	mode = nm_nonblocking;
	return(ret);
}

///////////////////////////////////////////////////////////////////////////////
void Network::printf(const char* lpszFormat, ...)
{
    va_list	 ap;			/* Argument pointer */
    // int		 e = errno;		/* Store errno */
	char	u[2048];

    va_start(ap, lpszFormat);
    vsprintf(u, lpszFormat, ap);
    va_end(ap);
	send(u, strlen(u));
}

///////////////////////////////////////////////////////////////////////////////
int Network::send(const char* buf, unsigned int length)
{
	unsigned int		e;
	if (nosock)
		return(0);
	D(SOTRACE("send: %s sock %d sent %d\n", type, sock, length);)
	e = ::send(sock, buf, length, 0);
	if (e != length)
	{
		int err = h_errno;
		xpprintf("%snetwork: %s send failed socket error %d (%s)\n", showtime(),
				(PCSTR)name, err, GetSockErrText(err));
		if (err == ECONNABORTED || err == ECONNRESET)
			nosock = TRUE;
	}
	return(e);
}


///////////////////////////////////////////////////////////////////////////////
LString* GetHostAsLstring()
{
	LString* nf;
	char	buf[MAXGETHOSTSTRUCT];
	nf = new LString;
	nf->name = s_Host;
	gethostname(buf, MAXGETHOSTSTRUCT);
	HOSTENT*	hent;
	hent = gethostbyname(buf);
//	char buf1[MAXGETHOSTSTRUCT];
	IN_ADDR*	b = (IN_ADDR*)hent->h_addr;
	sprintf(buf, "%s:%d",
		inet_ntoa(*b), GetCfg()->webPort);
	nf->s = buf;
	return(nf);
}

///////////////////////////////////////////////////////////////////////////////
void AddHost(ObjList* headers)
{
	LString* ls = GetHostAsLstring();
	headers->Add(ls);
}
