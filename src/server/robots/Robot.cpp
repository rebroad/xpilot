/* $Id: Robot.cpp,v 1.4 2007/02/17 20:29:18 dick Exp $
 *
 * XPilot, a multiplayer gravity war game.  Copyright (C) 1991-2007 by
 *
 *      Dick Balaska         <dick@xpilot.org>
 *      Bj�rn Stabell        <bjoern@xpilot.org>
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
 *
 *  $Log: Robot.cpp,v $
 *  Revision 1.4  2007/02/17 20:29:18  dick
 *  Wrap Trace(...) debug with D().
 *
 *  Revision 1.3  2007/02/12 07:51:05  dick
 *  Support RobotWatchDeco(*) which displays Robot debug objects on the playfield.
 *
 *  Revision 1.2  2007/01/17 09:12:12  dick
 *  Bring back Robot.cpp from the dead.  Most of his functions are now in RobotMan.cpp.
 *  Robot is the base class handling for a Robot.
 *  Manage the RobotWatch for all subclasses,
 *  and define the virtual table to drive a Robot.
 *
 */
/*
 * Robot code originally submitted by Maurice Abraham.
 */
#include "StdAfx.h"

#if 0
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>
#include <assert.h>

#ifndef _WINDOWS
# include <unistd.h>
#endif

#include "showtime.h"
#include "score.h"
#include "bit.h"
#include "saudio.h"
#include "NetServer.h"
#include "pack.h"
#include "error.h"
#include "server.h"
#include "commonproto.h"
#include "ConnectionPlayer.h"
#endif

#include "version.h"
#include "error.h"
#include "config.h"
#include "serverconst.h"
#include "global.h"
#include "proto.h"
#include "portability.h"

#include "Robot.h"

char robot_version[] = VERSION;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// A dummy robot
Robot::Robot() {}
Robot::~Robot() {}
PCSTR Robot::driverName = "dummy";
void Robot::Destroy() {}
void Robot::Invite(int) {}
void Robot::SetWar(int) {}
void Robot::GoHome() {}
void Robot::Play() {}
void Robot::RoundTick() {}
void Robot::Message(PCSTR) {}
int	 Robot::WarOnPlayer() { return(NO_ID); }
void Robot::Create(char*) {}
Robot* Robot::Factory() { return(new Robot()); }

///////////////////////////////////////////////////////////////////////////////
// Is there a paused player watching this robot?
bool Robot::IsWatch(int pageNumber) {
	for (int i=0; i<w->numPlayers; i++) {
		Player* pl = w->players[i];
		if (!pl->playerType == PlayerHuman)
			continue;
		if (BIT(pl->status, PLAYING|GAME_OVER|PAUSE) == PLAYING)
			continue;
		if (pl->myEyes == this->pl->id && pl->robotWatchPage == pageNumber)
			return(true);
	}
	return(false);
}

///////////////////////////////////////////////////////////////////////////////
void Robot::Watch(int pageNumber, PCSTR s, ...) {
	if (!IsWatch(pageNumber))
		return;
    va_list	 ap;			/* Argument pointer */
	va_start(ap, s);
	_Watch(pageNumber, -1, &s, ap);

}
///////////////////////////////////////////////////////////////////////////////
void Robot::Watch(int pageNumber, int lineNumber, PCSTR s, ...) {
	if (!IsWatch(pageNumber))
		return;
    va_list	 ap;			/* Argument pointer */
	va_start(ap, s);
	_Watch(pageNumber, lineNumber, &s, ap);
}

///////////////////////////////////////////////////////////////////////////////
void	Robot::_Watch(int pageNumber, int lineNumber, PCSTR* lpszFormat, va_list args, ...) {
	char	u[MSG_LEN];
	char*	s = u;

	vsnprintf(u, MSG_LEN, *lpszFormat, args);
	va_end(args);
	u[MSG_LEN-1] = '\0';

	for (int i=0; i<w->numPlayers; i++) {
		Player* pl = w->players[i];
		if (!pl->playerType == PlayerHuman)
			continue;
		if (BIT(pl->status, PLAYING|GAME_OVER|PAUSE) == PLAYING)
			continue;
		if (pl->robotWatchFull)
			continue;
		if (pl->myEyes == this->pl->id && pl->robotWatchPage == pageNumber) {
			RobotWatch* rw = new RobotWatch();
			rw->line = lineNumber;
			rw->s = u;
			pl->robotWatchList.Add(rw);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
void Robot::WatchPlay(int pageNumber, RobotWatchDecoType dt, int color,
					  ObjPosition* pos, int radius) {
	for (int i=0; i<w->numPlayers; i++) {
		Player* pl = w->players[i];
		if (!pl->playerType == PlayerHuman)
			continue;
		if (BIT(pl->status, PLAYING|GAME_OVER|PAUSE) == PLAYING)
			continue;
		if (pl->robotWatchFull)
			continue;
		if (pl->myEyes == this->pl->id && pl->robotWatchPage == pageNumber) {
			RobotWatchDeco* rwd = new RobotWatchDeco();
			rwd->type = dt;
			rwd->color = color;
			rwd->x = this->pl->pos.x - pos->x;
			rwd->y = this->pl->pos.y - pos->y;
			rwd->x = pos->x - this->pl->pos.x;
			rwd->y = pos->y - this->pl->pos.y;
			rwd->x = pos->x;
			rwd->y = pos->y;
			D(Trace("WatchDeco: pl=%d/%d o=%d/%d\n", this->pl->pos.x, this->pl->pos.y,
				pos->x, pos->y);)
			rwd->radius = radius;
			pl->robotWatchDecoList.Add(rwd);
		}
	}

}

///////////////////////////////////////////////////////////////////////////////
void	Robot::SetWatchFull() {
	for (int i=0; i<w->numPlayers; i++) {
		Player* pl = w->players[i];
		if (!pl->playerType == PlayerHuman)
			continue;
		if (BIT(pl->status, PLAYING|GAME_OVER|PAUSE) == PLAYING)
			continue;
		if (pl->robotWatchFull)
			continue;
		if (pl->myEyes == this->pl->id) {
			pl->robotWatchFull = true;
		}
	}
}

