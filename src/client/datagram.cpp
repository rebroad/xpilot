/* $Id: datagram.cpp,v 1.8 2004/04/26 07:04:13 dick Exp $
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
 * $Log: datagram.cpp,v $
 * Revision 1.8  2004/04/26 07:04:13  dick
 * Retab
 *
 * Revision 1.7  2004/02/03 03:39:52  dick
 * Add Windows precompiled header support.
 *
 * Revision 1.6  2004/01/06 04:48:27  dick
 * Add support for building under Cygwin.  Still needs work and the output
 * is X11 and requires something like Cygwin's XWin.exe to run under.
 *
 * Revision 1.5  2002/09/16 21:41:19  dick
 * Use const in declaration of create_dgram_addr_socket()
 *
 * Revision 1.4  2001/08/07 21:00:06  dick
 * error becomes sockError
 *
 * Revision 1.3  2001/08/06 05:41:49  dick
 * struct sock_t becomes class Sock
 *
 * Revision 1.2  2001/07/27 05:47:38  dick
 * Client builds and runs in C++
 *
 */
#include "StdAfx.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>

#if defined(_UNIX) || defined(_CYGWIN)
# include <unistd.h>
# include <sys/param.h>
# include <netdb.h>
#else
# include "NT/winNet.h"
# include "NT/winClient.h"
#endif

#include "version.h"
#include "config.h"
#include "error.h"
#include "client.h"
#include "Sock.h"
#include "protoclient.h"
#include "datagram.h"
#include "portability.h"
#include "IniClient.h"

//extern int	clientPortStart;	/* First UDP port for clients */
//extern int	clientPortEnd;		/* Last one (these are for firewalls) */

char datagram_version[] = VERSION;

int			dgram_one_socket = 0;

///////////////////////////////////////////////////////////////////////////////
int create_dgram_addr_socket(Sock* sock, PCSTR dotaddr, int port)
{
	static int	saved;
	static Sock save_sock;
	int 				status = SOCK_IS_ERROR;
	int 				i;

	if (saved == 0) {
		if (iniClient.clientPortStart
		  && (!iniClient.clientPortEnd || iniClient.clientPortEnd > 65535)) {
			iniClient.clientPortEnd = 65535;
		}
		if (iniClient.clientPortEnd 
		  && (!iniClient.clientPortStart || iniClient.clientPortStart < 1024)) {
			iniClient.clientPortStart = 1024;
		}

		if (port || !iniClient.clientPortStart
		  || (iniClient.clientPortStart > iniClient.clientPortEnd)) {
			status = sock->OpenUdp(dotaddr, port);
			if (status == SOCK_IS_ERROR) {
				error("Cannot create datagram socket (%d)", sock->sockError.error);
				return -1;
			}
		}
		else {
			int found_socket = 0;
			for (i = iniClient.clientPortStart; i <= iniClient.clientPortEnd; i++) {
				status = sock->OpenUdp(dotaddr, i);
				if (status != SOCK_IS_ERROR) {
					found_socket = 1;
					break;
				}
			}
			if (found_socket == 0) {
				error("Could not find a usable port in port range [%d,%d]",
					  iniClient.clientPortStart, iniClient.clientPortEnd);
				return -1;
			}
		}

		if (status == SOCK_IS_OK) {
			if (dgram_one_socket) {
				save_sock = *sock;
			}
		}
	} else {
		*sock = save_sock;
		status = SOCK_IS_OK;
	}

	return status;
}

///////////////////////////////////////////////////////////////////////////////
int create_dgram_socket(Sock* sock, int port)
{
	static char any_addr[] = "0.0.0.0";

	return create_dgram_addr_socket(sock, any_addr, port);
}

///////////////////////////////////////////////////////////////////////////////
void close_dgram_socket(Sock* sock)
{
	if (!dgram_one_socket) {
		sock->Close();
	}
}

