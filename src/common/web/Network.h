/* $Id: Network.h,v 1.3 2004/01/06 04:48:28 dick Exp $
 *
 * Linkable network objects
 *
 *  There are four primary subclasses of network.
 *  httplisten - listen on a port for connections from bsac or a browser
 *  http       - Handle a single http request.
 *  agent      - A talker. (Not used for xpilot)
 *  udplisten  - listen on a UDP port for packets. (Not used for xpilot)
 *
 *      Copyright (C) 2002 by
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
 * $Log: Network.h,v $
 * Revision 1.3  2004/01/06 04:48:28  dick
 * Add support for building under Cygwin.  Still needs work and the output
 * is X11 and requires something like Cygwin's XWin.exe to run under.
 *
 * Revision 1.2  2002/06/26 04:45:01  dick
 * Use xpprintf, not bprintf
 *
 * Revision 1.1  2002/06/25 04:41:59  dick
 * A base class used by the Http server.  This should probably be merged into Sock
 * and this class should go away.
 *
 */


#ifndef	_NETWORK_H_
#define	_NETWORK_H_

#include "Obj.h"
#include "portability.h"
#include "const.h"

#if defined(_WINDOWS) && !defined(_CYGWIN)
#include <winsock.h> 
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#endif

#ifdef	_WINDOWS
#define	WMB_HTTP		WM_APP+2
#define	WMB_AGENT		WM_APP+3
#define	WMB_UDP			WM_APP+4
#endif

// socket shutdown options
#ifndef	SD_SEND
#define	SD_SEND		1
#endif


enum _netMode {
	nm_blocking,
	nm_nonblocking,
};

class Network : public Obj 
{
  public:
	Network();
	virtual ~Network();

	SOCKET	sock;
	bool	nosock;			// if the connection aborted, don't send
	int		retryCount;		// number of attempts
	time_t	retryTime;		// when to retry this connection
	bool	SetRetryTime();	// set the next time to retry.  Return false to give up (too many attempts)

#ifdef	_WINDOWS
	int			wMsg;		// Which windows message we use (WMB_AGENT or WMB_HTTP)
#endif

	int			SetBlockingMode();
	int			SetNonBlockingMode();
	void		printf(const char* lpszFormat, ...);
	int			send(const char* buf, unsigned int length);

  private:
};

// return our hostname as a string
extern	LString*	GetHostAsLstring();
extern	void		AddHost(ObjList* list);	// Add the host to a list of headers

#endif	// _NETWORK_H_
