/* $Id: RobotWatchMan.cpp,v 1.4 2007/02/12 08:18:15 dick Exp $
 *
 * RobotWatchMan - manage watching robot's brain activities
 *                 (terminator mode)
 *
 * client - the user interface to the game.
 *
 * XPilot, a multiplayer gravity war game.  Copyright (C) 1991-2007 by
 *
 *      Dick Balaska         <dick@xpilot.org>
 *      Bjørn Stabell        <bjoern@xpilot.org>
 *      Ken Ronny Schouten   <ken@xpilot.org>
 *      Bert Gijsbers        <bert@xpilot.org>
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
 * $Log: RobotWatchMan.cpp,v $
 * Revision 1.4  2007/02/12 08:18:15  dick
 * Linux (ansi) cleanup
 *
 * Revision 1.3  2007/02/12 07:57:18  dick
 * Support RobotWatchDeco, which is decorated shapes displayed on the playfield.
 *
 * Revision 1.2  2007/02/03 08:04:06  dick
 * Comment out unused includes
 *
 * Revision 1.1  2007/01/17 21:35:55  dick
 * Encapsulate all of the RobotWatch features into a RobotWatchMan object.
 *
 */
#include "StdAfx.h"

//#include <stdlib.h>
//#include <stdio.h>
//#include <string.h>
//#include <errno.h>
//#include <math.h>
//#include <sys/types.h>

#if defined(_UNIX) || defined(_CYGWIN)
//# include <unistd.h>
//# include <X11/Xlib.h>
//# include <X11/Xos.h>
#else
# include "NT/winX.h"
# include "NT/winClient.h"
#endif


#include "cstring.h"
#include "version.h"
#include "config.h"
#include "const.h"
#include "setup.h"
#include "portability.h"
#include "paint.h"
#include "paintdata.h"
#include "RobotWatchMan.h"
#include "record.h"

char robotwatchman_version[] = VERSION;

///////////////////////////////////////////////////////////////////////////////
// Side effects: msg will be altered
int RobotWatchMan::HandleRobotWatch(int y, char *msg)
{
	char* s = msg;
	while (1) {
		RobotWatchObject*	rwo = &robotWatchObjects[currentRobotWatchObject];
		char* t = s;
		while (*t != '\0' && *t != '\n')
			t++;
		bool end = true;
		if (*t == '\n')
			end = false;

		*t = '\0';
		if (*s != '\0') {	// If it's not a blank line
			rwo->y = y;		// then store it.
			rwo->msg = s;
			currentRobotWatchObject = (currentRobotWatchObject + 1) % MAX_ROBOTWATCH_OBJECTS;
		}

		if (y != -1)
			y++;
		t++;
		s = t;
		if (end)
			break;
	}

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
int	 RobotWatchMan::HandleRobotWatchDeco(u_byte type, int x, int y, int radius, int color) {
	RobotWatchDeco*	rwd = &robotWatchDecos[currentRobotWatchDeco];
	rwd->type = (RobotWatchDecoType)type;
	rwd->x = x;
	rwd->y = y;
	rwd->radius = radius;
	rwd->color = color;
	currentRobotWatchDeco = (currentRobotWatchDeco + 1) % MAX_ROBOTWATCH_DECOS;
	return(0);
}


///////////////////////////////////////////////////////////////////////////////
void RobotWatchMan::FrameInit() {
	int		i;
	for (i=0; i < MAX_ROBOTWATCH_OBJECTS; i++) {
		RobotWatchObject* rwo = &robotWatchObjects[i];
		rwo->msg = "";
	}
	for (i=0; i<MAX_ROBOTWATCH_DECOS; i++) {
		RobotWatchDeco* rwd = &robotWatchDecos[i];
		rwd->type = -1;
	}
	currentRobotWatchObject = 0;
	currentRobotWatchDeco = 0;
}

///////////////////////////////////////////////////////////////////////////////
#define	ROBOTWATCH_X	500
void RobotWatchMan::Paint()
{
	int 	i, x, y;
	int		currentLine = 0;

	for (i=0; i < MAX_ROBOTWATCH_OBJECTS; i++) {
		RobotWatchObject* rwo = &robotWatchObjects[i];

		x = ROBOTWATCH_X;
		if (rwo->y == -1)
			y = currentLine;
		else
			y = rwo->y;
		currentLine = y+1;
		y = (y * scoreListFont->ascent)               + scoreListFont->ascent;
		SET_FG(colors[WHITE].pixel);
		rd.drawString(iniClient.dpy, p_draw, scoreListGC,
					x, y,
					rwo->msg,
					rwo->msg.GetLength());
		Erase_rectangle(x - 1, y - gameFont->ascent,
						rwo->msgWidth + 2,
						gameFont->ascent + gameFont->descent);
	}

	for (i=0; i < MAX_ROBOTWATCH_DECOS; i++) {
		RobotWatchDeco* rwd = &robotWatchDecos[i];
		if (rwd->type == -1)
			continue;
		//xpprintf("Deco %d/%d (%d/%d)\n", rwd->x, rwd->y,
		//	WINSCALE(X(rwd->x - rwd->radius/2)), WINSCALE(Y(rwd->y + rwd->radius/2)));
		SET_FG(colors[rwd->color].pixel);
		//SET_FG(colors[WHITE].pixel);
		rd.drawArc(iniClient.dpy, p_draw, gc,
			WINSCALE(X(rwd->x - rwd->radius/2)),
			WINSCALE(Y(rwd->y + rwd->radius/2)),
			rwd->radius, rwd->radius, 0, 64*360);
	}

}


