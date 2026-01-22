/* $Id: Connectparam.h,v 1.6 2002/09/16 21:27:13 dick Exp $
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
 * $Log: Connectparam.h,v $
 * Revision 1.6  2002/09/16 21:27:13  dick
 * Stringize the Connectparam attributes.
 *
 * Revision 1.5  2002/08/01 14:55:57  dick
 * real_name becomes realName, etc.
 *
 * Revision 1.4  2001/10/10 13:32:07  dick
 * operator= to copy a Connectparam (needed by welcome.cpp)
 *
 * Revision 1.3  2001/08/13 03:03:15  dick
 * Needs a constructor
 *
 * Revision 1.2  2001/08/12 11:02:39  dick
 * Add the password used to gain priviledges on a server.
 *
 * Revision 1.1  2001/08/07 20:47:22  dick
 * Move client's Connect_param to common Connectparam.  This might actually
 * end up being melded into SockbufClient
 *
 */

#ifndef _CONNECTPARAM_H_
#define _CONNECTPARAM_H_

#include "cstring.h"
#include "pack.h"

class Connectparam
{
public:
	Connectparam();
	Connectparam&	operator=(const Connectparam&);

    int			contactPort;
	int			serverPort;
	int			loginPort;
    String		nick;
	String		realName;
	String		serverAddr;
	String		serverName;
	String		dispName;
    uint		serverVersion;
    int			team;
	String		password;
	uint		cookie;
};

#endif	// _CONNECTPARAM_H_
