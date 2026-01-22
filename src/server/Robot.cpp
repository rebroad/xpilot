/* $Id: Robot.cpp,v 1.38 2004/06/03 06:06:14 dick Exp $
 *
 * XPilot, a multiplayer gravity war game.  Copyright (C) 1991-2002 by
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
 *  $Log: Robot.cpp,v $
 *  Revision 1.38  2004/06/03 06:06:14  dick
 *  shipobj becomes ShipObj.
 *
 *  Revision 1.37  2004/05/22 15:16:01  dick
 *  Remove #ifndef SILENT
 *
 *  Revision 1.36  2004/05/07 04:03:05  dick
 *  Retab
 *
 *  Revision 1.35  2004/04/29 16:08:27  dick
 *  compile with SOUND enabled.
 *
 *  Revision 1.34  2004/04/23 17:57:04  dick
 *  Add some debug assert checks of the obj table
 *
 *  Revision 1.33  2004/02/23 02:06:09  dick
 *  robot.h becomes Robot.h
 *
 *  Revision 1.32  2004/02/23 01:57:43  dick
 *  Retab
 *
 *  Revision 1.31  2004/02/20 06:27:47  dick
 *  SendPlayMessage becomes BroadcastPlayMessage() because it sends a play message
 *  to all connections.
 *
 *  Revision 1.30  2004/02/18 01:22:14  dick
 *  Remove extraneous and obsolete #include "netserver.h"
 *
 *  Revision 1.29  2004/02/18 00:58:34  dick
 *  Delete_player becomes DeletePlayer
 *
 *  Revision 1.28  2004/02/16 14:29:44  dick
 *  server/sched.cpp and friends become serverlib/NetServer
 *  which disconnects the udp listen and receive functionality from the game.
 *
 *  Revision 1.27  2004/02/06 21:14:41  cppc
 *  Some changes to 'it'.
 *  * Pausing player loses 'it'.
 *  * Random 'it' selection will not choose paused player.
 *  ** Added World::RandomPlayerWithStatus(mask, bits) to find random player matching status.
 *  * Robots don't leave if they're 'it'.
 *
 *  Revision 1.26  2004/02/03 03:39:54  dick
 *  Add Windows precompiled header support.
 *
 *  Revision 1.25  2002/09/11 16:42:04  dick
 *  Here endeth the server encapsulation task.
 *  theWorld is now an array of World(s) and get loaded dynamically.
 *
 *  Revision 1.24  2002/09/01 00:13:46  dick
 *  Remove obsolete windows #include "winServer.h"
 *
 *  Revision 1.23  2002/07/14 19:03:47  dick
 *  type_ext becomes enum PlayerType
 *
 *  Revision 1.22  2002/07/09 21:37:46  dick
 *  Whitespace
 *
 *  Revision 1.21  2002/07/08 05:52:59  dick
 *  showtime() handles the space after itself
 *
 *  Revision 1.20  2002/06/30 06:43:24  dick
 *  SendMessage is a bad word.  It's a Windows function that is a macro and gets
 *  translated to SendMessageA or SendMessageW.  Although it works, renaming ours
 *  to SendPlayMessage makes browsing the source code easier.
 *
 *  Revision 1.19  2002/06/02 06:00:12  dick
 *  Whitespacing
 *
 *  Revision 1.18  2002/06/01 06:06:48  dick
 *  Encapsulate (almost) everything.  Get rid of (almost) all refs to theWorld.
 *
 *  Revision 1.17  2002/05/22 05:09:01  dick
 *  Update to XPilot r4_5_4alpha.
 *
 *  Revision 1.16  2002/05/20 07:31:48  dick
 *  Pick a robot's startpos after initialization
 *
 *  Revision 1.15  2002/05/18 22:34:15  dick
 *  Move showtime to common.
 *
 *  Revision 1.14  2002/05/18 20:55:39  dick
 *  Update to XPilot-4.5.1
 *
 *  Revision 1.13  2001/08/10 08:11:38  dick
 *  New class ConnectionPlayer subclasses Connection to provide player
 *  functions.  New class Frame subclasses ConnectionPlayer which provides
 *  Frame update functionality.
 *
 *  Revision 1.12  2001/08/04 17:34:07  dick
 *  TransportToHome and DoAutopilot become members of Player.
 *  RECOVERY_DELAY needs * GetFPS().
 *
 *  Revision 1.11  2001/07/24 09:04:00  dick
 *  Remove all global options.  Use World.options instead.
 *
 *  Revision 1.10  2001/07/23 21:08:37  dick
 *  Encapsulate ShipMass, ShotsMax, ShotsLife, ShotsMass, FPS.
 *
 *  Revision 1.9  2001/07/23 07:38:42  dick
 *  Fold contact, metaserver, netserver, rules, and server to be members of World.
 *
 *  Revision 1.8  2001/07/21 06:45:30  dick
 *  Hide the varInt varString, etc. and force access through the accessor funcs
 *
 *  Revision 1.7  2001/07/19 07:01:48  dick
 *  Integrate OptOrigin into our ServerOption.
 *  stringvar becomes varString, intvar becomes varInt, etc.
 *
 *  Revision 1.6  2001/07/19 05:00:02  dick
 *  There is no real reason to have optionsStruct, all of the ServerOption(s)
 *  are attributes of ServerOptions.  And rename Options.* to ServerOptions.*
 *  (They could be just Options, but when the client comes along with his
 *  different format options...)
 *
 *  Revision 1.5  2001/07/12 10:01:02  dick
 *  Go_home() becomes GoHome()
 *
 *  Revision 1.4  2001/07/10 08:10:08  dick
 *  Don't need (deleted) map.h because World.h is included in global.h
 *
 *  Revision 1.3  2001/07/08 06:40:14  dick
 *  C++ conversion stuff.
 *
 *  Revision 1.2  2001/07/07 12:00:43  dick
 *  Rename classes to C++ "Style".  old World becomes theWorld.
 *
 */
/*
 * Robot code originally submitted by Maurice Abraham.
 */
#include "StdAfx.h"

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

#include "version.h"
#include "config.h"
#include "serverconst.h"
#include "global.h"
#include "proto.h"
#include "showtime.h"
#include "score.h"
#include "bit.h"
#include "saudio.h"
#include "NetServer.h"
#include "pack.h"
#include "Robot.h"
#include "error.h"
#include "server.h"
#include "commonproto.h"
#include "ConnectionPlayer.h"

#define DEFAULT_ROBOT_TYPE	"default"

char robot_version[] = VERSION;

///////////////////////////////////////////////////////////////////////////////
/*
 * Debugging macro.  Turn this on to have the objs[] table consistency checked
 * at various places along the calculations.
 */
#if (0)
#define	CHECK_OBJS()								\
	D(	int	_i;										\
		for (_i=0; _i<w->numObjs; _i++) {			\
			assert(w->objs[_i]->type <= 0x2000);	\
		}											\
	)
#else
#define	CHECK_OBJS()
#endif


/*
 * Array of different robots which are used
 * when we cannot read a robot configuration file.
 *
 * Each robot has a robot driver,
 * a name as seen by the human players,
 * some optional configuration string,
 * a usage count,
 * and a shipshape.
 */
static Robot DefaultRobots[] = {
	{
		DEFAULT_ROBOT_TYPE,
		"Mad Max",
		"94 20",
		0,
		"(15,8,7)(15,0)(7,1)(7,2)(2,4)(-1,11)"
		"(-3,11)(-2,3)(-8,6)(-8,-6)(-2,-3)(-3,-11)"
		"(-1,-11)(2,-4)(7,-2)(7,-1)"
	},
	{
		DEFAULT_ROBOT_TYPE,
		"Blackie",
		"10 90",
		0,
		"(16,6,10)(15,0)(6,2)(-2,3)(-1,4)(-2,5)"
		"(-10,8)(-13,8)(-13,1)(-15,0)(-13,-1)"
		"(-13,-8)(-10,-8)(-2,-5)(-1,-4)(-2,-3)(6,-2)"
	},
	{
		DEFAULT_ROBOT_TYPE,
		"Kryten",
		"70 40",
		0,
		"(4,1,3)(15,0)(0,8)(-8,0)(0,-8)"
	},
	{
		DEFAULT_ROBOT_TYPE,
		"Marvin",
		"30 70",
		0,
		"(15,4,5)(10,0)(10,7)(5,14)(-5,14)(-10,7)"
		"(-10,-7)(-5,-14)(5,-14)(10,-7)(10,0)(5,5)"
		"(2,7)(5,0)(2,-7)(5,-5)"
	},
	{
		DEFAULT_ROBOT_TYPE,
		"R2D2",
		"50 60",
		0,
		"(15,8,9)(15,0)(14,1)(-1,2)(-2,9)(0,10)"
		"(-4,10)(-7,2)(-8,2)(-8,-2)(-7,-2)(-4,-10)"
		"(0,-10)(-2,-9)(-1,-2)(14,-1)"
	},
	{
		DEFAULT_ROBOT_TYPE,
		"C3PO",
		"60 50",
		0,
		"(16,1,15)(10,0)(0,5)(0,15)(15,10)(0,15)"
		"(-15,10)(0,15)(0,5)(-7,0)(0,-5)(0,-15)"
		"(-15,-10)(0,-15)(15,-10)(0,-15)(0,-5)"
	},
	{
		DEFAULT_ROBOT_TYPE,
		"K9",
		"50 50",
		0,
		"(14,0,5)(15,0)(15,5)(5,5)(5,-5)(15,-5)"
		"(15,0)(-15,0)(-15,5)(5,5)(5,-5)(-15,-5)"
		"(-15,-8)(-15,8)(-15,0)"
	},
	{
		DEFAULT_ROBOT_TYPE,
		"Robby",
		"45 55",
		0,
		"(5,2,3)(15,0)(0,12)(-9,8)(-9,-8)(0,-12)"
	},
	{
		DEFAULT_ROBOT_TYPE,
		"Mickey",
		"05 95",
		0,
		"(15,6,7)(5,-1)(8,-5)(7,-9)(4,-11)(-1,-10)"
		"(-5,-6)(-8,-10)(-8,10)(-5,6)(-1,10)(4,11)"
		"(7,9)(8,5)(5,1)(0,0)"
	},
	{
		DEFAULT_ROBOT_TYPE,
		"Hermes",
		"15 85",
		0,
		"(16,12,11)(10,1)(12,8)(-11,8)(-10,3)"
		"(-7,0)(-5,2)(-7,0)(-10,-1)(-10,-3)"
		"(-13,-4)(-13,-7)(-15,-8)(-15,-13)(-5,-5)"
		"(-2,-4)(5,-2)"
	},
	{
		DEFAULT_ROBOT_TYPE,
		"Pan",
		"60 60",
		0,
		"(14,6,5)(15,-1)(15,0)(5,0)(5,-1)(5,9)(-15,9)"
		"(-15,-4)(-5,-7)(-3,-8)(-7,-8)(-5,-7)(5,-4)"
		"(5,-1)(-15,-1)"
	},
	{
		DEFAULT_ROBOT_TYPE,
		"Azurion",
		"40 30",
		0,
		"(6,2,4)(15,0)(0,2)(-9,8)(-3,0)(-9,-8)(0,-2)"
	},
	{
		DEFAULT_ROBOT_TYPE,
		"Droidion",
		"60 30",
		0,
		"(6,2,4)(9,0)(4,8)(-5,8)(-10,0)(-5,-8)(4,-8)"
	},
	{
		DEFAULT_ROBOT_TYPE,
		"Terminator",
		"80 40",
		0,
		"(6,2,4)(15,0)(0,2)(-9,8)(-3,0)"
		"(-9,-8)(0,-2)"
	},
	{
		DEFAULT_ROBOT_TYPE,
		"Sniper",
		"30 90",
		0,
		"(15,6,9)(15,0)(4,2)(-2,8)(-4,7)(-3,2)"
		"(-8,5)(-8,2)(-6,1)(-6,-1)(-8,-2)(-8,-5)"
		"(-3,-2)(-4,-7)(-2,-8)(4,-2)"
	},
	{
		DEFAULT_ROBOT_TYPE,
		"Slugger",
		"40 40",
		0,
		"(15,8,7)(13,0)(11,1)(3,2)(-1,8)(-3,8)"
		"(-3,2)(-5,2)(-8,5)(-8,-5)(-5,-2)(-3,-2)"
		"(-3,-8)(-1,-8)(3,-2)(11,-1)"
	},
	{
		DEFAULT_ROBOT_TYPE,
		"Uzi",
		"95 5",
		0,
		"(16,9,8)(15,3)(7,3)(7,-8)(3,-8)(3,1)(-2,1)"
		"(-3,-1)(-5,-1)(-14,-5)(-15,2)(-3,4)(-1,8)"
		"(0,6)(13,6)(14,8)(15,6)"
	},
	{
		DEFAULT_ROBOT_TYPE,
		"Capone",
		"80 50",
		0,
		"(15,9,6)(14,0)(2,2)(0,8)(1,8)(-3,8)(-3,2)"
		"(-8,4)(-7,1)(-7,-1)(-8,-4)(-3,-2)(-3,-8)"
		"(1,-8)(0,-8)(2,-2)"
	},
	{
		DEFAULT_ROBOT_TYPE,
		"Tanx",
		"40 70",
		0,
		"(16,7,8)(15,1)(2,0)(1,-2)(6,-3)(6,-5)(3,-8)"
		"(-10,-8)(-13,-6)(-13,-3)(-10,-2)(-11,2)(-7,2)"
		"(-7,8)(-7,2)(1,2)(2,1)"
	},
	{
		DEFAULT_ROBOT_TYPE,
		"Chrome Star",
		"60 60",
		0,
		"(5,1,4)(8,0)(-8,5)(2,-8)(2,8)(-8,-5)"
	},
	{
		DEFAULT_ROBOT_TYPE,
		"Bully",
		"80 10",
		0,
		"(15,6,9)(11,0)(12,-3)(9,-3)(8,-2)(-5,-5)"
		"(-9,-11)(-14,-14)(-5,-3)(-5,3)(-14,14)"
		"(-9,11)(-5,5)(8,2)(9,3)(12,3)"
	},
	{
		DEFAULT_ROBOT_TYPE,
		"Metal Hero",
		"40 45",
		0,
		"(16,7,9)(15,5)(12,-2)(9,-2)(10,-1)"
		"(-8,-1)(-4,-1)(1,-3)(-13,-9)(-9,0)"
		"(-15,8)(1,3)(-4,1)(-8,1)(-8,-1)(-8,1)"
		"(11,1)"
	},
	{
		DEFAULT_ROBOT_TYPE,
		"Aurora",
		"60 55",
		0,
		"(16,5,11)(15,0)(-1,3)(-3,5)(-3,9)(7,10)"
		"(-12,10)(-6,9)(-6,4)(-8,0)(-6,-4)(-6,-9)"
		"(-12,-10)(7,-10)(-3,-9)(-3,-5)(-1,-3)"
	},
	{
		DEFAULT_ROBOT_TYPE,
		"Dalt Wisney",
		"30 75",
		0,
		"(16,10,6)(14,0)(7,-4)(0,-1)(-5,-4)"
		"(2,-8)(0,-10)(-14,-10)(-5,-7)"
		"(-14,0)(-5,7)(-14,10)(0,10)(2,8)"
		"(-5,4)(0,1)(7,4)"
	},
	{
		DEFAULT_ROBOT_TYPE,
		"Psycho",
		"65 55",
		0,
		"(11,5,6)(8,0)(5,8)(3,12)(0,15)(0,0)"
		"(-8,3)(-8,-3)(0,0)(0,-15)(3,-12)(5,-8)"
	},
	{
		DEFAULT_ROBOT_TYPE,
		"Gorgon",
		"30 40",
		0,
		"(15,7,8)(15,0)(5,2)(3,8)(2,2)(-9,2)"
		"(-10,4)(-12,2)(-14,4)(-14,-4)(-12,-2)"
		"(-10,-4)(-9,-2)(2,-2)(3,-8)(5,-2)"
	},
	{
		DEFAULT_ROBOT_TYPE,
		"Pompel",
		"50 50",
		0,
		"(15,7,8)(15,0)(14,4)(10,5)(5,2)(-7,3)"
		"(-7,6)(5,8)(-9,8)(-9,-8)(5,-8)(-7,-6)"
		"(-7,-3)(5,-2)(10,-5)(14,-4)"
	},
	{
		DEFAULT_ROBOT_TYPE,
		"Pilt",
		"50 50",
		0,
		"(16,8,7)(15,0)(13,-2)(9,-3)(3,-3)(-3,-3)"
		"(-5,-2)(-13,-2)(-15,-3)(-15,3)(-13,2)"
		"(-5,2)(-3,3)(-3,-8)(-3,8)(-3,3)(8,3)"
	},
	{
		DEFAULT_ROBOT_TYPE,
		"Sparky",
		"20 40",
		0,
		"(15,8,7)(15,-8)(6,-5)(7,-4)(1,-2)(2,-1)"
		"(-4,0)(-3,2)(-15,8)(-15,2)(-8,0)(-9,-2)"
		"(-3,-3)(-4,-4)(3,-5)(2,-7)"
	},
	{
		DEFAULT_ROBOT_TYPE,
		"Cobra",
		"85 60",
		0,
		"(16,5,11)(8,0)(8,-6)(6,-8)(0,-7)(5,-6)"
		"(-8,-4)(5,-2)(0,-1)(5,0)(0,1)(5,2)(-8,4)"
		"(5,6)(0,7)(6,8)(8,6)"
	},
	{
		DEFAULT_ROBOT_TYPE,
		"Falcon",
		"70 20",
		0,
		"(16,5,6)(14,2)(14,4)(2,10)(-5,10)(-10,8)"
		"(-12,3)(-12,-3)(-10,-8)(-5,-10)(9,-11)"
		"(10,-8)(7,-8)(14,-4)(14,-2)(4,-2)(4,2)"
	},
	{
		DEFAULT_ROBOT_TYPE,
		"Boson",
		"25 35",
		0,
		"(16,11,12)(15,0)(10,-5)(4,-8)(7,-2)(7,2)"
		"(4,8)(6,0)(4,-8)(-10,-8)(-10,8)(-10,-8)"
		"(-15,-7)(-15,7)(-10,8)(4,8)(10,5)"
	},
	{
		DEFAULT_ROBOT_TYPE,
		"Blazy",
		"40 40",
		0,
		"(12,4,8)(4,0)(2,4)(-5,11)(10,12)(-8,12)"
		"(-4,6)(-2,0)(-4,-6)(-8,-12)(10,-12)"
		"(-5,-11)(2,-4)"
	},
	{
		DEFAULT_ROBOT_TYPE,
		"Pixie",
		"15 93",
		0,
		"(13,6,7)(15,0)(7,4)(11,1)(-4,3)(3,5)"
		"(-7,10)(-9,2)(-9,-2)(-7,-10)(3,-5)(-4,-3)"
		"(11,-1)(7,-4)"
	},
	{
		DEFAULT_ROBOT_TYPE,
		"Wimpy",
		"5 98",
		0,
		"(16,9,7)(3,0)(6,5)(8,10)(5,11)(1,10)(-1,8)"
		"(-4,9)(-8,6)(-5,0)(-8,-6)(-4,-9)(-1,-8)"
		"(1,-10)(5,-11)(8,-10)(6,-5)"
	},
	{
		DEFAULT_ROBOT_TYPE,
		"Bonnie",
		"30 40",
		0,
		"(16,9,6)(13,3)(5,3)(5,1)(4,-1)(0,-1)"
		"(-2,-8)(-8,-8)(-5,3)(-6,6)(-8,7)(-7,8)"
		"(-4,7)(8,7)(10,8)(12,8)(13,7)"
	},
	{
		DEFAULT_ROBOT_TYPE,
		"Clyde",
		"40 45",
		0,
		"(16,5,11)(14,0)(5,5)(6,2)(0,2)(0,8)(-13,8)"
		"(-13,4)(-4,4)(-6,0)(-4,-4)(-13,-4)(-13,-8)"
		"(0,-8)(0,-2)(6,-2)(5,-5)"
	},
	{
		DEFAULT_ROBOT_TYPE,
		"Neuro",
		"70 70",
		0,
		"(16,7,5)(12,-7)(12,-12)(5,-12)(2,-10)"
		"(1,-5)(-9,-4)(-11,2)(-8,8)(-3,11)(3,11)"
		"(9,8)(11,2)(13,0)(12,-3)(12,-7)(7,-7)"
	},
};

//int			NumRobots = 0;	/* number of currently playing robots. */
static int		MAX_ROBOTS = 0; /* number of different robot parameters. */
static Robot	*Robots;		/* array of robot parameters. */

/*
 * Function prototype for robot type setup routines.
 * Add your own robot type setup routine here.
 */
extern int Robot_default_setup(RobotType* type_ptr);

#if defined(DEVLOPMENT) || defined(ALLBOTS)
extern int Stratbot_setup(RobotType* type_ptr);
#endif

/*
 * Array to store function pointers to robot type setup routines.
 * The default robot type should be first.
 * Add your own robot type setup routine here too.
 */
//static int (*robot_type_setups[])(RobotType *type_ptr) = {
//	  Robot_default_setup,
//};

static struct robot_setup {
	int 	(*setup_func)(RobotType* type_ptr);
} robot_type_setups[] = {
	{ Robot_default_setup },
#if defined(DEVLOPMENT) || defined(ALLBOTS)
	{ Stratbot_setup },
#endif
};
/*
 * Array of the different robot types available.
 */
static int num_robot_types = NELEM(robot_type_setups);
static RobotType robot_types[NELEM(robot_type_setups)];

void Parse_robot_file(World* w)
{
	if (!w->options.robotFile->GetString().IsEmpty()) {
		FILE *fp = fopen(w->options.robotFile->GetString(), "r");
		if (fp) {
			char buf[1024];
			char name_buf[MAX_NAME_LEN];
			char ship_buf[2*MSG_LEN];
			char type_buf[MAX_NAME_LEN];
			char para_buf[MSG_LEN];
			int got_name = 0;
			int num_robs = 0;
			int max_robs = 0;
			Robot* robs = 0;

			/*
			 * Fill in some default values.
			 */
			strcpy(ship_buf, "(15,0)(-9,8)(-9,-8)");
			strcpy(type_buf, DEFAULT_ROBOT_TYPE);
			strcpy(para_buf, "");

			while (fp) {
				int end_of_record = 0;

				if (!fgets(buf, sizeof buf, fp)) {
					end_of_record = 1;
					fclose(fp);
					fp = NULL;
				}
				else if (*buf == '\n') {
					end_of_record = 1;
				}
				else {
					int size = 0;
					int key = 0;
					char *dst = 0;

					if (!strncmp(buf, "name:", 5)) {
						dst = name_buf;
						size = sizeof name_buf;
						key = 1;
						got_name = 1;
					}
					else if (!strncmp(buf, "ship:", 5)) {
						dst = ship_buf;
						size = sizeof ship_buf;
						key = 2;
					}
					else if (!strncmp(buf, "type:", 5)) {
						dst = type_buf;
						size = sizeof type_buf;
						key = 3;
					}
					else if (!strncmp(buf, "para:", 5)) {
						dst = para_buf;
						size = sizeof para_buf;
						key = 4;
					}
					if (key > 0) {
						char *ptr = strchr(buf, ':') + 1;
						while (isspace(*ptr)) {
							ptr++;
						}
						strlcpy(dst, ptr, size);
						ptr = &dst[strlen(dst)];
						while (--ptr >= dst && isspace(*ptr)) {
							*ptr = '\0';
						}
					}
				}
				if (end_of_record && got_name) {
					got_name = 0;
					if (num_robs == max_robs) {
						if (max_robs == 0) {
							max_robs = 10;
							robs = (Robot *)malloc(max_robs * sizeof(Robot));
						} else {
							max_robs += 10;
							robs = (Robot *)realloc(robs,
													  max_robs * sizeof(Robot));
						}
						if (!robs) {
							error("Not enough memory to parse robotsfile");
							fclose(fp);
							break;
						}
					}
					strcpy(robs[num_robs].driver, type_buf);
					strcpy(robs[num_robs].name, name_buf);
					strcpy(robs[num_robs].config, para_buf);
					strcpy(robs[num_robs].shape, ship_buf);
					robs[num_robs].used = 0;
					num_robs++;
				}
			}
			if (num_robs > 0) {
				Robots = robs;
				MAX_ROBOTS = num_robs;
			}
		}
	}
	if (MAX_ROBOTS == 0) {
		Robots = &DefaultRobots[0];
		MAX_ROBOTS = NELEM(DefaultRobots);
	}

#ifdef DEVELOPMENT
	if (getenv("XPILOTS_DUMP_ROBOTS_TO_ROBOT_FILE") != NULL) {
		if (robotFile && *robotFile) {
			FILE *fp = fopen(robotFile, "w");
			if (fp) {
				int i;
				for (i = 0; i < MAX_ROBOTS; i++) {
					fprintf(fp,
							"type:\t%s\n"
							"para:\t%s\n"
							"ship:\t%s\n"
							"name:\t%s\n"
							"\n",
							Robots[i].driver,
							Robots[i].config,
							Robots[i].shape,
							Robots[i].name);
				}
				fclose(fp);
			}
		}
	}
#endif
}

/*
 * First time initialization of all the robot stuff.
 */
void Robot_init(World* w)
{
	int 		i;
		int 					result;
	int 				n;

		/*
		 * For each robot driver call its initialization function.
		 * If this function returns 0, then remember this robot driver.
		 */
	n = 0;
	for (i = 0; i < num_robot_types; i++) {
		memset(&robot_types[n], 0, sizeof(RobotType));
		//if ((*robot_type_setups[i])(&robot_types[n]) == 0) {
		result = (*robot_type_setups[i].setup_func)(&robot_types[n]);
		if (result == 0) {
			n++;
		}
	}
	num_robot_types = n;

	Parse_robot_file(w);

		if (w->options.robotTeam->GetInt() < 0 || w->options.robotTeam->GetInt() >= MAX_TEAMS)
				w->options.robotTeam->Set(0);
}


static void Robot_talks(World* w, enum robot_talk_t says_what,
						char *robot_name, const char *other_name)
{
	/*
	 * Insert your own witty messages here and remove the silly ones.
	 */

	static const char *enter_msgs[] = {
		"%s just can't stand you anymore.",
		"%s has come to give you a hard time.",
		"%s is looking for trouble.",
		"%s has a very loose trigger finger.",
		"Have fear, %s is here.",
		"Prepare to die by the hands of %s.",
		"%s is untouchable.",
		"%s is in a gruesome mood.",
		"%s is in a killing mood.",
		"%s wants you for desert.",
		"%s vows to torment you in this life and the next.",
		"%s has no sense of humour.",
		"%s is back from the Sirius wars, and he's in a violent mood.",
	};
	static const char *leave_msgs[] = {
		"That's it, I've had enough. :(   I'm outta here. [%s]",
		"Later people.  It's been fun. [%s]",
		"Gotta go, ... er ... this ... er ... lab is closing. [%s]",
		"Er...  Oh!  It it really that late?  I gotta go. [%s].",
		"I'm signing off now.  Bye! [%s]",
		"Gotta go...  Er... I have some work to be done. [%s]",
		"This sucks! :(  I'm going back to cyberspace. [%s]",
		"I've taken enough beating for today. [%s]",
		"Oh man.  Playing with humans sucks. [%s]",
		"You can't beat us robots; we always return, stronger than ever! [%s]",
		"Geez, this just isn't my lucky day.  See ya some other time. [%s]",
		"Wow, this game is just killing me. :( [%s]",
		"I'll be back when you stop cheating! :-( [%s]",
	};
	static const char *kill_msgs[] = {
		"Have some %s.  Have some! [%s]",
		"You want some more %s? [%s]",
		"%s lost his stuff again.  That's just tooooooo bad. :) [%s]",
		"%s: did you like that one? [%s]",
		"Face it %s, you just can't compete with me. [%s]",
		"%s, my grandmother plays better than you. [%s]",
		"Hey %s, go play chess instead. [%s]",
		"I think Darwin would've said you're too unfit to survive %s :) [%s]",
		"Oh my, what colourful explosions you make %s. :) [%s]",
		"Hey %s, maybe its time you upgraded that old 386. [%s]",
	};
	static const char *war_msgs[] = {
		"UNBELIEVABLE, me shot down by %s?!?!  This means war [%s]",
		"People like %s just piss me off. [%s]",
		"Nice %s.  But now its my turn. [%s]",
		"Red alert... target: %s. [%s]",
		"$%#^@#$^#$%  That's the last time you do that %s! [%s]",
		"Enough's enough!  It's only room enough for one of us %s here. [%s]",
		"I'm sorry %s, but you must... DIE!!!!! [%s]",
		"Jihad!  Die %s!  Die! [%s]",
		"%s will be assimilated [%s]",
	};

	static int			next_msg = -1;
	const char			**msgsp;
	int 				two, i, n;
	char				msg[MSG_LEN];

	if (w->options.robotsTalk->GetBool() != true && says_what != ROBOT_TALK_ENTER) {
		return;
	}

	switch (says_what) {
	case ROBOT_TALK_ENTER:
		msgsp = enter_msgs;
		n = NELEM(enter_msgs);
		two = 1;
		break;
	case ROBOT_TALK_LEAVE:
		msgsp = leave_msgs;
		n = NELEM(leave_msgs);
		two = 1;
		break;
	case ROBOT_TALK_KILL:
		msgsp = kill_msgs;
		n = NELEM(kill_msgs);
		two = 2;
		break;
	case ROBOT_TALK_WAR:
		msgsp = war_msgs;
		n = NELEM(war_msgs);
		two = 2;
		break;
	default:
		return;
	}

	if (next_msg == -1) {
		next_msg = (int)(rfrac() * 997);
	}
	if (++next_msg > 997) {
		next_msg = 0;
	}
	i = next_msg % n;
	if (two == 2) {
		sprintf(msg, msgsp[i], other_name, robot_name);
	} else {
		sprintf(msg, msgsp[i], robot_name);
	}
	w->BroadcastPlayMessage(msg);
}


static void Robot_create(World* w)
{
	Player				*arobot;
	Robot				*rob;
	int 				i,
						num;
	int 				most_used,
						least_used;
	RobotData*			data;
	RobotData*			new_data;
	RobotType*			rob_type;
	char				msg[MSG_LEN];

	if (peek_ID() == 0) {
		return;
	}

//	  if ((new_data = (robot_data *)malloc(sizeof(robot_data_t))) == NULL) {
	if ((new_data = new RobotData) == NULL) {
		perror("malloc RobotData");
		return;
	}
	new_data->private_data = NULL;

	most_used = 0;
	for (i = 0; i < MAX_ROBOTS; i++) {
		if (Robots[i].used > Robots[most_used].used) {
			most_used = i;
		}
	}
	for (i = 0; i < w->numPlayers; i++) {
		if (IS_ROBOT_IND(w, i)) {
			data = (RobotData *)w->players[i]->robot_data_ptr;
			if (Robots[data->robots_ind].used < Robots[most_used].used) {
				Robots[data->robots_ind].used = Robots[most_used].used;
			}
		}
	}
	least_used = 0;
	for (i = 0; i < MAX_ROBOTS; i++) {
		if (Robots[i].used < Robots[least_used].used) {
			least_used = i;
		}
	}
	num = (int)(rfrac() * MAX_ROBOTS);
	while (Robots[num].used > Robots[least_used].used) {
		if (++num >= MAX_ROBOTS) {
			num = 0;
		}
	}
	rob = &Robots[num];
	rob->used++;
	new_data->robots_ind = num;
	new_data->robot_types_ind = 0;
	for (i = 1; i < num_robot_types; i++) {
		if (!strcmp(robot_types[i].name, rob->driver)) {
			new_data->robot_types_ind = i;
		}
	}
	rob_type = &robot_types[new_data->robot_types_ind];

	w->players[w->numPlayers]->Init((w->options.allowShipShapes->GetBool())
							? Parse_shape_str(rob->shape)
							: (ShipObj *)NULL);
	arobot = w->players[w->numPlayers];
		arobot->playerType = PlayerRobot;
	arobot->robot_data_ptr = new_data;

	strlcpy(arobot->name, rob->name, MAX_CHARS);
	strlcpy(arobot->realname, w->options.robotRealName->GetString(), MAX_CHARS);
	strlcpy(arobot->hostname, w->options.robotHostName->GetString(), MAX_CHARS);

	arobot->color = WHITE;
	arobot->turnspeed = MAX_PLAYER_TURNSPEED;
	arobot->turnspeed_s = MAX_PLAYER_TURNSPEED;
	arobot->turnresistance = 0.12;
	arobot->turnresistance_s = 0.12;
	arobot->power = MAX_PLAYER_POWER;
	arobot->power_s = MAX_PLAYER_POWER;
	arobot->check = 0;
	if (BIT(w->rules->mode, TEAM_PLAY)) {
		arobot->team = w->PickTeam(PickForRobot);
		w->teams[arobot->team].numMembers++;
		w->teams[arobot->team].numRobots++;
	}
	if (arobot->mychar != 'W')
		arobot->mychar = 'R';

	w->ScoreServerPlayerEvent(arobot, Join);
	w->ScoreServerRequestPlayerScore(arobot);
	arobot->fuel.l1 = 100 * FUEL_SCALE_FACT;
	arobot->fuel.l2 = 200 * FUEL_SCALE_FACT;
	arobot->fuel.l3 = 500 * FUEL_SCALE_FACT;

	// Pick_startpos(NumPlayers);
	(*rob_type->create)(w, w->numPlayers, rob->config);

	arobot->PickStartpos();

	arobot->GoHome();

	request_ID();
	w->numPlayers++;
	w->numRobots++;

		if (BIT(w->rules->mode, TEAM_PLAY)
				&& w->options.teamShareScore->GetBool())
		{
				if (w->teams[arobot->team].numMembers == 1) {
						/* reset team score on first player */
						w->teams[arobot->team].score = 0;
				}
				TEAM_SCORE(w, arobot->team, 0);
		}
		for (i = 0; i < w->numPlayers - 1; i++)
		{
				if (w->players[i]->conn)
				{
						w->players[i]->conn->SendPlayer(arobot);
						w->players[i]->conn->SendBase(arobot->id, arobot->home_base);
				}
	}

	Robot_talks(w, ROBOT_TALK_ENTER, arobot->name, "");

	if (w->options.logRobots->GetBool())
				xpprintf("%s%s (%d, %s) starts at startpos %d.\n",
						showtime(), arobot->name,
						w->numPlayers, arobot->realname, arobot->home_base);
	if (w->roundDelay > 0 || w->numPlayers == 1) {
		w->roundDelay = w->options.roundDelay->GetInt() * w->GetFPS();
		w->roundDelaySend = w->roundDelay + w->GetFPS();  /* delay him an extra second */
		if (w->options.maxRoundTime->GetInt() > 0
				&& w->options.roundDelay->GetInt() == 0)
		{
				w->roundTime = w->options.maxRoundTime->GetInt() * w->GetFPS();
		} else {
				w->roundTime = -1;
		}
		sprintf(msg, "Player entered. Delaying %d seconds until next %s.",
				w->options.roundDelay->GetInt(),
				(BIT(w->rules->mode, TIMING)? "race" : "round"));
		w->BroadcastPlayMessage(msg);
	}

	w->updateScores = true;
}


void Robot_destroy(World* w, int ind)
{
	Player*		pl = w->players[ind];

	(*robot_types[pl->robot_data_ptr->robot_types_ind].destroy)(w, ind);
	delete (w->players[ind]->robot_data_ptr);
	w->players[ind]->robot_data_ptr = NULL;
}


void Robot_delete(World* w, int ind, int kicked)
{
	long		i;
	DFLOAT		low_score = LONG_MAX;
	long		low_i = -1;
	char		msg[MSG_LEN];

	if (ind == -1) {
	   /*
		* Find the robot with the lowest score.
		*/
		for (i = 0; i < w->numPlayers; i++) {
			if (!IS_ROBOT_IND(w, i))
				continue;

			if (w->players[i]->score < low_score) {
				low_i = i;
				low_score = w->players[i]->score;
			}
		}
		if (low_i >= 0) {
			ind = low_i;
		}
	}

	if (ind >= 0) {
		if (kicked) {
			sprintf(msg, "\"%s\" upset the gods and was kicked out "
						 "of the game.", w->players[ind]->name);
							w->BroadcastPlayMessage(msg);
		}
		D( xpprintf("%s robot %s is leaving the game\n", showtime(),
			w->players[ind]->name);
		)
		DeletePlayer(w, ind);
		D( xpprintf("%s robot left the game\n", showtime()); )
	}
}

/*
 * Ask a robot for an alliance
 */
void Robot_invite(World* w, int ind, int inv_ind)
{
	Player*	pl = w->players[ind];
	(*robot_types[pl->robot_data_ptr->robot_types_ind].invite)(w, ind, inv_ind);
}

/*
 * Turn on a war lock.
 */
static void Robot_set_war(World* w, int ind, int victim_id)
{
	Player*	pl = w->players[ind];

	(*robot_types[pl->robot_data_ptr->robot_types_ind].set_war)(w, ind, victim_id);
}


/*
 * Turn off a war lock.
 * The only time when this can be called is if
 * a player a robot has war on leaves the game.
 */
void Robot_reset_war(World* w, int ind)
{
	Robot_set_war(w, ind, -1);
}


/*
 * Someone has programmed a robot (using ECM) to seek some player.
 */
void Robot_program(World* w, int ind, int victim_id)
{
	Robot_set_war(w, ind, victim_id);
}


/*
 * Return the id of the player this robot has war on.
 * If the robot is not in peace mode then return -1.
 */
int Robot_war_on_player(World* w, int ind)
{
	Player		*pl = w->players[ind];
	RobotType	*rob_type =
				&robot_types[pl->robot_data_ptr->robot_types_ind];

	return (*rob_type->war_on_player)(w, ind);
}


/*
 * A robot has killed someone.
 * Or a robot has been killed by someone.
 * Maybe this is enough reason for the killed robot to change
 * its behavior with respect to the player it has been killed by.
 */
void Robot_war(World* w, int ind, int killer)
{
	Player*	pl = w->players[ind];
	Player*	kp = w->players[killer];
	int		i;

	if (killer == ind) {
		return;
	}

	if (IS_ROBOT_PTR(kp)) {
		Robot_talks(w, ROBOT_TALK_KILL, kp->name, pl->name);

		if (Robot_war_on_player(w, killer) == pl->id) {
			for (i = 0; i < w->numPlayers; i++) {
				if (w->players[i]->conn) {
					w->players[i]->conn->SendWar(kp->id, NO_ID);
				}
			}
		}
		Robot_set_war(w, killer, -1);
	}

	if (IS_ROBOT_PTR(pl)
		&& (int)(rfrac() * 100) < kp->score - pl->score
		&& !TEAM(w, ind, killer)
		&& !ALLIANCE(w, ind, killer)) {

		Robot_talks(w, ROBOT_TALK_WAR, pl->name, kp->name);

		/*
		 * Give fuel for offensive.
		 * KK: unfair advantage.
		 */
		/* pl->fuel.sum = MAX_PLAYER_FUEL; */

		if (Robot_war_on_player(w, ind) != kp->id) {
			for (i = 0; i < w->numPlayers; i++) {
				if (w->players[i]->conn) {
					w->players[i]->conn->SendWar(pl->id, kp->id);
				}
			}
			SoundPlayAll(w, DECLARE_WAR_SOUND);
			Robot_set_war(w, ind, kp->id);
		}
	}
}


/*
 * A robot starts on its homebase.
 */
void Robot_go_home(World* w, int ind)
{
	(*robot_types[w->players[ind]->robot_data_ptr->robot_types_ind].go_home)(w, ind);
}


/*
 * Someone sends a message to a robot.
 * The format of the message is: "This is the real message [receiver]:[sender]"
 */
void Robot_message(World* w, int ind, const char *message)
{
	Player		*pl = w->players[ind];
	RobotType	*rob_type =
				&robot_types[pl->robot_data_ptr->robot_types_ind];

	(*rob_type->message)(w, ind, message);
}


/*
 * A robot plays this frame.
 */
static void Robot_play(World* w, int ind)
{
	(*robot_types[w->players[ind]->robot_data_ptr->robot_types_ind].play)(w, ind);
}


/*
 * Check if robot is still considered good enough to continue playing.
 * Return FALSE if robot continues playing,
 * return TRUE if robot leaves the game.
 */
static int Robot_check_leave(World* w, int ind)
{
	Player*		pl = w->players[ind];
	char		msg[MSG_LEN];

	if (w->options.robotsLeave->GetBool()
		&& pl->life > 0
		&& !BIT(w->rules->mode, LIMITED_LIVES)
		&& (BIT(pl->status, PLAYING) || pl->count <= 0)) {
		//
		// Don't leave if 'it'.
		//
		if (w->options.tagGame->GetBool()
		 && w->tag == pl->id) {
			return FALSE;
		}
		msg[0] = '\0';
		if (w->options.robotLeaveLife->GetInt() > 0
		 && pl->life >= w->options.robotLeaveLife->GetInt()) {
			sprintf(msg, "%s retired.", pl->name);
		} else if (w->options.robotLeaveScore->GetInt() != 0 &&
			pl->score < w->options.robotLeaveScore->GetInt()) {
			sprintf(msg, "%s left out of disappointment.", pl->name);
		} else if (w->options.robotLeaveRatio->GetDouble() != 0 && pl->score / (pl->life + 1)
				< w->options.robotLeaveRatio->GetDouble()) {
			sprintf(msg, "%s played too badly.", pl->name);
		}
		if (msg[0] != '\0') {
			Robot_talks(w, ROBOT_TALK_LEAVE, pl->name, "");
			w->BroadcastPlayMessage(msg);
			Robot_delete(w, ind, false);
			return TRUE;
		}
	}

	return FALSE;
}


/*
 * On each round we call the robot type round ticker.
 */
static void Robot_round_tick(World* w)
{
	int			i;

	if (w->numRobots > 0) {
		for (i = 0; i < num_robot_types; i++) {
			(*robot_types[i].round_tick)(w);
		}
	}
}


/*
 * Update tanks here.
 */
static void Tank_play(World* w, int ind)
{
	Player*	pl = w->players[ind];
	int		t = frame_loops % (int)(TANK_NOTHRUST_TIME + TANK_THRUST_TIME);

	if (t == 0) {
		SET_BIT(pl->status, THRUSTING);
	} else if (t == TANK_THRUST_TIME) {
		CLR_BIT(pl->status, THRUSTING);
	}
}

void Robot_update(World* w)
{
	Player*		pl;
	int			i;
	static int	new_robot_delay;
	int			num_playing_ships;
	int			num_any_ships;

	num_any_ships = w->numPlayers + netServer->loginInProgress;
	num_playing_ships = num_any_ships - w->numPseudoPlayers;
	if ((num_playing_ships < w->options.maxRobots->GetInt()
			|| w->numRobots < w->options.minRobots->GetInt())
		&& num_playing_ships < w->numBases
		&& num_any_ships < NUM_IDS
		&& w->numRobots < MAX_ROBOTS
		&& !(BIT(w->rules->mode, TEAM_PLAY)
		&& w->options.restrictRobots->GetBool()
		&& w->teams[w->options.robotTeam->GetInt()].numMembers >=
		   w->teams[w->options.robotTeam->GetInt()].numBases))
	{
		if (++new_robot_delay >= ROBOT_CREATE_DELAY * w->GetFPS()) {
			Robot_create(w);
			new_robot_delay = 0;
		}
	}
	else {
		new_robot_delay = 0;
		if (w->numRobots > 0) {
			if ((num_playing_ships > w->numBases)
				|| (num_any_ships > NUM_IDS)
				|| (num_playing_ships > w->options.maxRobots->GetInt()
					&& w->numRobots > w->options.minRobots->GetInt())) {
				Robot_delete(w, -1, false);
			}
		}
	}

	if (w->numRobots <= 0 && w->numPseudoPlayers <= 0) {
		return;
	}

	CHECK_OBJS();

	Robot_round_tick(w);

	CHECK_OBJS();

	for (i = 0; i < w->numPlayers; i++) {
		pl = w->players[i];

		if (IS_TANK_PTR(pl)) {
			Tank_play(w, i);
			continue;
		}

		if (!IS_ROBOT_PTR(pl)) {
			/* Ignore non-robots. */
			continue;
		}

		if (BIT(pl->status, PLAYING|GAME_OVER) != PLAYING) {
			/* Only check for leave if not being transported to homebase. */
			if (!pl->count) {
				if (Robot_check_leave(w, i)) {
					i--;
				}
			}
			continue;
		}

		if (Robot_check_leave(w, i)) {
			i--;
			continue;
		}

		if (w->roundDelay > 0) {
			continue;
		}

		/*
		 * Let the robot code control this robot.
		 */
		Robot_play(w, i);
		CHECK_OBJS();
	}
}

