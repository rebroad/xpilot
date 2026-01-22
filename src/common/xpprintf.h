/* $Id: xpprintf.h,v 1.4 2004/01/30 15:54:28 dick Exp $
 *
 * xpprintf - printf to someplace, because Windows doesn't have a stdout
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
 * $Log: xpprintf.h,v $
 * Revision 1.4  2004/01/30 15:54:28  dick
 * Publish the logLevel attribute
 *
 * Revision 1.3  2002/06/25 04:31:28  dick
 * Define the number of log lines held in memory.
 *
 * Revision 1.2  2002/06/23 06:22:16  dick
 * Add a LOGERR level
 *
 * Revision 1.1  2002/06/21 00:37:39  dick
 * break xpprintf() out to his own file.
 *
 */

#ifndef	_XPPRINTF_H_
#define	_XPPRINTF_H_

#include "Obj.h"

#if defined(_WINDOWS) && !defined(_WINDOWS_)
#include <windows.h>
#endif

#define	LOGNONE		0		// log nothing
#define	LOGERR		1		// log only errors
#define	LOGMIN		2		// minimal info
#define	LOGMED		3
#define	LOGLOTS		4
#define	LOGMAX		5		// ludicrous speed!

#define	LOGAPPEND		0
#define	LOGOVERWRITE	1


#ifdef	_WINDOWS
extern	HWND	printfWnd;
#endif

extern	void	xpprintf(const char* lpszFormat, ...);
extern	void	xpprintf(int logLevel, const char* lpszFormat, ...);
extern	const char *showtime(void);

extern	ObjList	logEntries;
extern	int		liveLogEntries;
extern	int		logLevel;

// must be supplied by app
extern	const char* szAppName;

#endif	// _XPPRINTF_H_
