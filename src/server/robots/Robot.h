/* $Id: Robot.h,v 1.5 2007/02/12 07:51:05 dick Exp $
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
 *
 *  $Log: Robot.h,v $
 *  Revision 1.5  2007/02/12 07:51:05  dick
 *  Support RobotWatchDeco(*) which displays Robot debug objects on the playfield.
 *
 *  Revision 1.4  2007/01/19 07:11:13  dick
 *  Cleanup unused Watch cruft.
 *
 *  Revision 1.3  2007/01/17 09:17:08  dick
 *  RobotWatch is a list of Strings sent from the client when a player is paused
 *  and watching a robot.  This list contains diagnostic information about
 *  what the heck the robot thinks it's doing.
 *  It's kinda like the Terminator view where he's looking at a 6502 dump.
 *
 *  Revision 1.2  2007/01/13 22:27:28  dick
 *  Now, a Robot* is a member of Player.
 *  Robot contains the interface to drive a robot.
 *  Robot4 is the old robotdef.cpp handler.
 *
 *  Revision 1.1  2007/01/11 04:18:57  dick
 *  Robots move to their own subdirectory
 *
 *  Revision 1.7  2007/01/11 00:16:35  dick
 *  enum robot_talk_t moves to RobotMan
 *
 *  Revision 1.6  2007/01/10 18:14:47  dick
 *  All robot actions are now handled through RobotMan.
 *  There is one RobotMan per World.
 *
 *  Revision 1.5  2004/02/23 01:58:13  dick
 *  Move Robot's proto.h decls to here
 *
 *  Revision 1.4  2002/06/01 06:06:48  dick
 *  Encapsulate (almost) everything.  Get rid of (almost) all refs to theWorld.
 *
 *  Revision 1.3  2002/05/18 20:55:39  dick
 *  Update to XPilot-4.5.1
 *
 *  Revision 1.2  2001/07/07 12:00:43  dick
 *  Rename classes to C++ "Style".  old World becomes theWorld.
 *
 */
/* Robot code originally submitted by Maurice Abraham. */

/*
 * We would like to support several different robot types.
 * Each robot type is defined by one name and is accessed
 * by several functions or methods.
 * The name is used to enable configuration files to
 * specify which robot type to use for which robot name.
 * The functions are:
 *
 *    1) The global initialization function for this specific robot type.
 *       It is called once at startup to initialise a structure with
 *       function pointers to the robot type action routines.
 *
 *    2) The next round function, which is called only once per round
 *       to allow the robot type adjust type generic parameters.
 *
 *    3) The initialization function to enable robot type
 *       specific adjustments and possible memory allocation
 *       of private data structures for one specific robot instance.
 *       This function will be called with an extra string argument.
 *       The string argument may contain robot type specific
 *       configuration data, like attack and defend mode.
 *       Each robot type is free to define the format
 *       for this string as it sees fit.  But when this string
 *       is empty the robot type initialization code will
 *       have to provide suitable default configuration values.
 *
 *    4) The go home function which is called whenever the robot
 *       is placed at its homebase.
 *
 *    5) The playing function which gets called each loop.
 *       The programming challenge here is to implement
 *       different characters per robot, while at the
 *       same time not calculating `everything' every
 *       time this function is called.  i.e., try to give
 *       the robot some long term goals which are only
 *       recalculated once every couple of seconds or so.
 *
 *    6) The war function which is called when the robot declares war.
 *       This function takes an extra killer argument, which is
 *       the player id of the player the robot has declared war against.
 *       If this id equals NO_ID then the routine should reset the war state.
 *
 *    7) The war_on_player function returns the id of the player
 *       who the robot is in a state of war with.
 *       If the robot is not in war with another player
 *       then this function should return NO_ID.
 *
 *    8) The message function can accept any possible commands to the robot.
 *       The suggestion here is that if you plan to let the robot type
 *       be controllable by messages that you give feedback to all
 *       of the players what commands the robot has accepted.
 *
 *    9) The cleanup function should free any allocated resources
 *       for this specific robot instance.
 *
 * The recommended practice is to define your new robot types
 * in a separate file and to only declare your robot type
 * specific initialisation function prototype in robot.c and add one
 * function pointer to the robot_type_setups array.
 */
/*
 * Watching is a development time concept.
 * If we are paused while playing the game and watching a robot,
 * it would be nice to try to figure out what the heck the robot
 * is thinking.
 */

#ifndef _ROBOT_H_
#define _ROBOT_H_

#include "Obj.h"
#include "pack.h"

class World;
class Player;
class ObjPosition;

// An object to describe what and how to send robotWatch data to the client
class RobotWatch : public Obj {
  public:
	int		line;
	String	s;
};

///////////////////////////////////////////////////////////////////////////////
enum RobotWatchDecoType {
	WatchHollowCircle,
	WatchFilledCicle,
	WatchHollowRect,
	WatchFilledRect
};

///////////////////////////////////////////////////////////////////////////////
class RobotWatchDeco : public Obj {
  public:
	RobotWatchDecoType		type;
	int		x, y;
	int		radius;
	int		color;
};

///////////////////////////////////////////////////////////////////////////////
// base class for a robot driver
class Robot : public Obj {
	friend	class RobotMan;

  public:
	Robot();
	virtual ~Robot();

	void	SetWorld(World* w);

	virtual	void	RoundTick();
	virtual	void	Create(char* str);
	virtual	void	GoHome();
	virtual	void	Play();
	virtual	void	SetWar(int killer);
	virtual	int		WarOnPlayer();
	virtual	void	Message(PCSTR str);
	virtual	void	Destroy();
	virtual	void	Invite(int invInd);

    String			name;

	static	PCSTR	driverName;
	static	PCSTR	description;
	static	Robot*	Factory();
	static	PCSTR	robotHostName;

	// Robot development and debugging.
	// When a player is paused, robot logic can be displayed in the upper right of the screen

	void	SetWatch(int pageNumber);	// Someone is watching this robot.
	void	SetWatchFull();				// after one play frame, lock the watch buffer until a client empties it.
	virtual	int	GetWatchPageCount();

  protected:
    int		robots_ind;			/* index into robotFactory[] */
    int		robot_types_ind;	/* index into robot_types[] */
	Player*	pl;					// which player are we attached to
	World*	w;
	bool	IsWatch(int pageNumber);
	void	Watch(int pageNumber, PCSTR s, ...);
	void	Watch(int pageNumber, int lineNumber, PCSTR s, ...);
	void	WatchPlay(int pageNumber, RobotWatchDecoType dt, int color,
					  ObjPosition* pos, int radius);


  private:
	void	_Watch(int pageNumber, int lineNumber, PCSTR* lpszFormat, va_list args, ...);
};

inline	void Robot::SetWorld(World* _w) { w = _w; }
inline	int  Robot::GetWatchPageCount() { return(0); }
#define DEFAULT_ROBOT_TYPE	"robot4"


#endif		// _ROBOT_H_
