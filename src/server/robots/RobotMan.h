/* $Id: RobotMan.h,v 1.5 2007/01/21 07:26:02 dick Exp $
 *
 * The Robot Manager.  A singleton attached to a World.
 *
 * XPilot, a multiplayer gravity war game.  Copyright (C) 1991-2007 by
 *
 *      Dick Balaska         <dick@xpilot.org>
 *      Bert Gijsbers        <bert@xpilot.org>
 *      Bj�rn Stabell        <bjoern@xpilot.org>
 *      Ken Ronny Schouten   <ken@xpilot.org>
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
 *  $Log: RobotMan.h,v $
 *  Revision 1.5  2007/01/21 07:26:02  dick
 *  Disable RobotCreator bean constructor.
 *
 *  Revision 1.4  2007/01/17 09:15:29  dick
 *  Handle the FlipRobotPage for a player.  Move Robot base class to his own file.
 *
 *  Revision 1.3  2007/01/14 22:10:15  dick
 *  Robot4 is the original xpilot 4 series robot.
 *  Robot4a is dick's playing around with making them better.
 *
 *  Revision 1.2  2007/01/13 22:27:28  dick
 *  Now, a Robot* is a member of Player.
 *  Robot contains the interface to drive a robot.
 *  Robot4 is the old robotdef.cpp handler.
 *
 *  Revision 1.1  2007/01/11 04:18:57  dick
 *  Robots move to their own subdirectory
 *
 *  Revision 1.2  2007/01/11 00:16:35  dick
 *  enum robot_talk_t moves to RobotMan
 *
 *  Revision 1.1  2007/01/10 18:14:47  dick
 *  All robot actions are now handled through RobotMan.
 *  There is one RobotMan per World.
 *
 */

/* The robotFactory contains a list of all the possible robots that may play.
 * At any given time, a handful are active.
 * robotFactory contains RobotDescriptors.
 * A RobotDescriptor contains the parameters defining an individual robot
 * (name, shipshape) the Robot driver name
 * A Robot contains the intelligence for driving a robot.
 */

#ifndef	_ROBOTMAN_H_
#define	_ROBOTMAN_H_

/*
 * Different talk commands.
 * The talk code is done by the robot manager,
 * not by the robot type implementation.
 */
enum robot_talk_t {
    ROBOT_TALK_ENTER,
    ROBOT_TALK_LEAVE,
    ROBOT_TALK_KILL,
    ROBOT_TALK_WAR,
    NUM_ROBOT_TALK_TYPES
};

class World;
class RobotDescriptor;

// This is the World's API into the robot manager
class RobotMan {
  public:
	RobotMan();

	void	Init(World* w);

	void	Delete(int ind, int kicked);
	void	Destroy(int ind);
	void	Update();

	// Change the Robot Debugging page
	void	FlipRobotPage(Player* guru);

	// Methods to a robot
	void	Invite(int ind, int invInd);
	void	War(int ind, int killer);
	void	ResetWar(int ind);
	int		WarOnPlayer(int ind);
	void	GoHome(int ind);
	void	Message(int ind, const char *message);

	void	Program(int ind, int victim_id);

  private:
	World*	w;

	void	ParseRobotFile();
	void	ParseRobotFile(String s);
	void	RoundTick();

	// Robot manipulation
	void	RobotCreate();
	void	RobotPlay(int ind);
	void	RobotTalks(enum robot_talk_t says_what,
					   char* robot_name, PCSTR other_name);
	void	RobotSetWar(int ind, int victim_id);
	void	RobotResetWar(int ind);
	int		RobotCheckLeave(int ind);

	int					numAvailable;		// number of different robot parameters.
	RobotDescriptor*	robotFactory;		// array of robot parameters.
	ObjList				listOfDrivers;		// list of RobotDriver

	// for robot file parsing
	RobotDescriptor* parsingRobs;
	int numParsingRobs;
	int maxParsingRobs;

};


///////////////////////////////////////////////////////////////////////////////
// Configuration data for each robot available.
//
class RobotDescriptor {
public:
    char	driver[MAX_NAME_LEN];	/* Which code controls robot? */
    char	name[MAX_NAME_LEN];		/* Name of this robot. */
    char	config[MAX_CHARS];		/* Attack + defense ~ 100 (for Robot4) */
    uint	used;					/* How often has this robot been used */
    char	shape[2*MSG_LEN];		/* shipshape string definition */
};

///////////////////////////////////////////////////////////////////////////////
// The RobotCreator is used by RobotMan::Init() to load the list of available Robot types.
//
typedef	Robot*	(*RobotFactoryFunc)();

class RobotCreator : public Obj {
public:
	RobotCreator(PCSTR s, RobotFactoryFunc c);
	String	driver;
	RobotFactoryFunc	creator;
private:
	RobotCreator();		// disable bean constructor
};
inline RobotCreator::RobotCreator(PCSTR s, RobotFactoryFunc c)	{ driver = s; creator = c; }


///////////////////////////////////////////////////////////////////////////////
// An array of Robots to use when there is no robots.txt file to parse
// Distribute with this one enabled
//
extern	RobotDescriptor defaultFactory[];
extern	int				defaultFactoryNELEM();

// A different array of Robots used for robot development.
extern	RobotDescriptor testFactory[];
extern	int				testFactoryNELEM();

#endif		// _ROBOTMAN_H_
