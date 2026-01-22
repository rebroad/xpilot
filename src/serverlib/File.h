/*
* Xpilotedit, the cross platform map editor for Xpilot.  Copyright (C) 2001 by
*
*      The XPilot Authors           <xpilot@xpilot.org>
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
 $Log: File.h,v $
 Revision 1.4  2004/02/16 14:29:44  dick
 server/sched.cpp and friends become serverlib/NetServer
 which disconnects the udp listen and receive functionality from the game.

 Revision 1.3  2002/06/04 20:18:16  dick
 Use String instead of (char*)malloc() to squash some leaks.

 Revision 1.2  2001/08/01 17:37:16  dick
 Add ServerOption Listeners.  When a ServerOption is changed, notify all
 registered listeners. (Try opening two prefs windows on the same map).

 Revision 1.1  2001/07/20 05:20:11  dick
 Yet another reorg.  Break server/xpilotedit common files into serverlib.

 Revision 1.1  2001/07/18 20:15:29  dick
 The first pass at integrating xpilotedit's options with the server options.
 The server does not build yet.  xpilotedit works as before, with the new
 definitions.

 */

#ifndef	_FILE_H_
#define	_FILE_H_

#include <stdio.h>
//#include "../common/cstring.h"

void	toeol(FILE *);
char	skipspace(FILE *);
String	GetMultilineValue(FILE *, int& line, char * );
int		YesNo(PCSTR);
char*	StrToNum(char *, int, int);

#endif	// _FILE_H_
