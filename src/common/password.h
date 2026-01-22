/* $Id: password.h,v 1.2 2002/08/01 14:47:41 dick Exp $
 *
 * XPilot, a multiplayer gravity war game.  Copyright (C) 2002 by
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
 */
/*
 * $Log: password.h,v $
 * Revision 1.2  2002/08/01 14:47:41  dick
 * Add support for a cookie file.
 *
 * Revision 1.1  2002/07/23 17:35:50  dick
 * Read the ServerPassword and XPilotPasswords files.  Write to XPilotPasswords.
 *
 */

#ifndef	_PASSWORD_H_
#define	_PASSWORD_H_

extern	PCSTR	s_localhost;

extern	String	GetPassword(PCSTR host, int port);
extern	bool	SetPassword(PCSTR host, int port,
							PCSTR password, String& error);

extern	int		GetCookie(PCSTR host, int port);
extern	bool	SetCookie(PCSTR host, int port,
						  uint cookie, String& error);

#endif	// _PASSWORD_H_
