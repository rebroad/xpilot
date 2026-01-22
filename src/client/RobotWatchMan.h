/* $Id: RobotWatchMan.h,v 1.3 2007/02/12 07:57:18 dick Exp $
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
 * $Log: RobotWatchMan.h,v $
 * Revision 1.3  2007/02/12 07:57:18  dick
 * Support RobotWatchDeco, which is decorated shapes displayed on the playfield.
 *
 * Revision 1.2  2007/01/17 23:10:47  dick
 * Bump the count up to 32
 *
 * Revision 1.1  2007/01/17 21:35:55  dick
 * Encapsulate all of the RobotWatch features into a RobotWatchMan object.
 *
 */

#ifndef	_ROBOTWATCHMAN_H_
#define	_ROBOTWATCHMAN_H_

#define	MAX_ROBOTWATCH_OBJECTS		32
#define	MAX_ROBOTWATCH_DECOS		32

class RobotWatchObject {
  public:
	int		y;
	int		msgWidth;
	String	msg;
};

///////////////////////////////////////////////////////////////////////////////
enum RobotWatchDecoType {
	WatchHollowCircle,
	WatchFilledCicle,
	WatchHollowRect,
	WatchFilledRect
};

class RobotWatchDeco : public Obj {
  public:
	int		type;
	int		x, y;
	int		radius;
	int		color;
};

class RobotWatchMan {
  public:
	void	FrameInit();	// call on frame start
	int		HandleRobotWatch(int y, char *msg);
	int		HandleRobotWatchDeco(u_byte type, int x, int y, int radius, int color);
	void	Paint();


	RobotWatchObject		robotWatchObjects[MAX_ROBOTWATCH_OBJECTS];
	int 					currentRobotWatchObject;
	RobotWatchDeco			robotWatchDecos[MAX_ROBOTWATCH_DECOS];
	int 					currentRobotWatchDeco;


};


#endif	// _ROBOTWATCHMAN_H_
