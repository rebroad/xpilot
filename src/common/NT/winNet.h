/* $Id: winNet.h,v 1.3 2001/08/12 11:03:34 dick Exp $
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

/***************************************************************************\
*  winNet.h - X11 to Windoze converter										*
*																			*
*  $Id: winNet.h,v 1.3 2001/08/12 11:03:34 dick Exp $					*
\***************************************************************************/

#ifndef	_WINNET_H_
#define	_WINNET_H_

#ifdef	_WINDOWS

#include <winsock.h>

#define	MAXHOSTNAMELEN	64

#define	EWOULDBLOCK		WSAEWOULDBLOCK

extern	long	alarm(long seconds, void(__cdecl*func)(int));

extern	HWND		notifyWnd;
extern	BOOL*		hostnameFound;

#define	WM_GETHOSTNAME	(WM_APP+100)

//extern	long	alarm(long seconds, int);
#endif	/* _WINDOWS */
#endif	/* _WINNET_H_ */
