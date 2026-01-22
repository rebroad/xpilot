/* $Id: Connectparam.cpp,v 1.5 2004/02/03 03:39:52 dick Exp $
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
 * $Log: Connectparam.cpp,v $
 * Revision 1.5  2004/02/03 03:39:52  dick
 * Add Windows precompiled header support.
 *
 * Revision 1.4  2002/09/16 21:27:13  dick
 * Stringize the Connectparam attributes.
 *
 * Revision 1.3  2002/08/01 14:55:57  dick
 * real_name becomes realName, etc.
 *
 * Revision 1.2  2001/10/10 13:32:07  dick
 * operator= to copy a Connectparam (needed by welcome.cpp)
 *
 * Revision 1.1  2001/08/13 02:57:48  dick
 * Needs a constructor
 *
 */
#include "StdAfx.h"

#include <string.h>

#include "types.h"
#include "Connectparam.h"

Connectparam::Connectparam()
{
	cookie = 0;
}


Connectparam& Connectparam::operator=(const Connectparam& in)
{
	realName = in.realName;
	serverAddr = in.serverAddr;
	serverName = in.serverName;
	dispName = in.dispName;
	password = in.password;
    serverVersion = in.serverVersion;
    team = in.team;
    contactPort = in.contactPort;
	serverPort = in.serverPort;
	loginPort = in.loginPort;
	cookie = in.cookie;
	return(*this);
}
