/* $Id: ServerOptions.init.cpp.h,v 1.38 2005/03/17 22:12:14 kps Exp $
 *
 * XPwhere - Where in the world can i find people playing XPilot.
 *      Copyright (C) 2001 by
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
 *
 * See the file COPYRIGHT.TXT for current copyright information.
 *
 */
/*
 * $Log: ServerOptions.init.cpp.h,v $
 * Revision 1.38  2005/03/17 22:12:14  kps
 * Get rid of warnings from makedepend about "non-portable whitespace".
 *
 * Revision 1.37  2004/05/24 07:37:05  dick
 * Add support for 2 serverMessage, which can displayed on a client when he starts up.
 *
 * Revision 1.36  2004/05/24 06:50:37  dick
 * Default for showLocks is on
 *
 * Revision 1.35  2004/05/20 22:27:24  dick
 * Attribute showLocks displays in the ScoreTable who everyone is locked on.
 *
 * Revision 1.34  2004/04/26 07:07:14  dick
 * Define a list of pages that can be displayed on the ScoreTable in the client.
 * Each page can be controlled by the server (and the server admin tool).
 *
 * Revision 1.33  2004/02/21 06:59:40  dick
 * Init() the attributes to null or 0 before filling
 *
 * Revision 1.32  2004/02/06 03:40:41  dick
 * Drop down the 'Tag' options one row for separation
 *
 * Revision 1.31  2004/02/05 14:56:58  cppc
 * First cut at integrating 'it' play mode.
 *
 * Revision 1.30  2004/02/05 04:57:29  dick
 * serverhost can also be found by servername.
 *
 * Revision 1.29  2004/01/28 00:04:01  dick
 * Use the PrefTab enum when talking about which tab a ServerOption is in.
 *
 * Revision 1.28  2004/01/27 04:36:49  dick
 * Default pauseScoreReduce is 0
 *
 * Revision 1.27  2004/01/12 05:23:06  dick
 * Add TunerPauseScoreReduce() TunerAllowViewing and TunerReportToMetaServer
 *
 * Revision 1.26  2004/01/09 17:25:39  dick
 * clientPortRange becomes firewallPortList
 *
 * Revision 1.25  2004/01/08 18:10:51  dick
 * Update to XPilot 4.5.5beta
 *
 * Revision 1.24  2002/09/16 21:45:01  dick
 * Speling counts in helptext
 *
 * Revision 1.23  2002/08/01 14:29:10  dick
 * Remove unused option scoreTableFileName
 *
 * Revision 1.22  2002/07/19 03:21:20  dick
 * Remove NoQuit option.
 *
 * Revision 1.21  2002/07/12 15:27:21  dick
 * tabs
 *
 * Revision 1.20  2002/07/09 04:21:41  dick
 * Wrap help text in HELPTEXT() and control it with WANTHELPTEXT.
 * Wrap labels in LABEL() and control it with WANTLABELS.
 *
 * Revision 1.19  2002/07/08 06:03:34  dick
 * New option saveTuned, which is not so much an option as it is an event.
 * By "setting" this option, the server "tunes" it, thus saving the tweaks
 * file.  A status message is returned as the value.
 *
 * Revision 1.18  2002/06/23 06:29:44  dick
 * Whitespace
 *
 * Revision 1.17  2002/05/30 03:33:41  jlmiller
 *
 * Linux cleanups for compilation after reorg of ServerList
 *
 * Revision 1.16  2002/05/23 21:21:23  dick
 * System tab becomes tab 1.  Shuffle a bunch of options to more logical places.
 *
 * Revision 1.15  2002/05/22 15:53:01  dick
 * FPS to map info panel, maxPauseTime to Connection panel.
 *
 * Revision 1.14  2002/05/21 22:39:24  jlmiller
 * Clean up option display, filter a few correctly
 *
 * Revision 1.13  2002/05/18 20:55:40  dick
 * Update to XPilot-4.5.1
 *
 * Revision 1.12  2001/09/03 00:51:50  dick
 * Tuners cleanup.
 *
 * Revision 1.11  2001/08/31 20:59:16  dick
 * Try to fix realtime tuning of asteroids.
 *
 * Revision 1.10  2001/08/14 21:31:29  dick
 * New option anonymousViewing.  When false (default), the player you are
 * viewing (eyes) appears in the score table as if you have declared war
 * on them.  This lets everyone know who you are watching.
 *
 * Revision 1.9  2001/08/13 03:01:17  dick
 * serverPassword becomes connectedToPassword
 *
 * Revision 1.8  2001/08/12 11:15:29  dick
 * New clientPortRange supercedes clientPortStart and clientPortEnd
 *
 * Revision 1.7  2001/08/11 09:39:33  dick
 * new option notifyServerChanges; do we spit a message to the clients when a
 * ServerOption changes?
 *
 * Revision 1.6  2001/08/10 08:05:29  dick
 * new option numControlConnections - number of allowed non-playing connections.
 *
 * Revision 1.5  2001/08/08 08:02:57  dick
 * Don't run the listeners during construction.
 *
 * Revision 1.4  2001/08/07 20:53:31  dick
 * Add connectedToPort, the port of the server we are connecting to.
 *
 * Revision 1.3  2001/08/06 05:26:23  dick
 * Need address of (&) functions
 *
 * Revision 1.2  2001/08/05 18:18:30  dick
 * Linux builds and runs again
 *
 * Revision 1.1  2001/08/05 08:05:37  dick
 * Add Tuners to the server.  ComputeSensorRange(), AddTank(), RemoveTank()
 * become members of Player.  ComputeGravity*() become members of World.
 *
 */
#ifndef	_SERVEROPTIONSINIT_H_
#define	_SERVEROPTIONSINIT_H_

#include "config.h"
#include "ServerOptions.h"

#if		HASTUNER
#define	TUNERDEF(__tuner)	&ServerOptionWorld::__tuner
#else
#define	TUNERDEF(__tuner)	((TunerFunc)NULL)
#endif

#if		WANTHELPTEXT
#define	HELPTEXT(__text)	(__text)
#else
#define	HELPTEXT(__text)	NULL
#endif

#if		WANTLABELS
#define	LABEL(__text)	(__text)
#else
#define	LABEL(__text)	NULL
#endif

/***************************************************************************/
/* ServerOptions::ServerOptions()                                          */
/* Arguments :                                                             */
/* Purpose :   Construct and fill all builtin defaults             .       */
/***************************************************************************/
ServerOptions::ServerOptions()
{
	Init();

	int i = 0;	// Be sure to init 'i' to 0!!!
				// And be sure to set NUMPREFS !!!
	isInit = true;
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// Map Info tab
	prefsArray[i++] = mapWidth = ServerOptionFactory(this,
		"mapwidth", "", LABEL("Width"),
		3, valInt,
		"100",
		0,0,TabMapInfo,OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"Width of the world in blocks.\n"
		)
	);

	prefsArray[i++] = mapHeight = ServerOptionFactory(this,
		"mapheight", "",						LABEL("Height"),
		3, valInt,
		"100",
		1,0,TabMapInfo, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"Height of the world in blocks.\n"
		)
	);

	prefsArray[i++] = mapName = ServerOptionFactory(this,
		"mapname", "",							LABEL("Name"),
		255, valString,
		"<New Map>",
		2,0,TabMapInfo, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"The title of the map.\n"
		)
	);

	prefsArray[i++] = mapAuthor = ServerOptionFactory(this,
		"mapauthor", "",						LABEL("Author"),
		255, valString,
		"<Your Name>",
		3,0,TabMapInfo, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"The name of the map author.\n"
		)
	);

	prefsArray[i++] = framesPerSecond = ServerOptionFactory(this,
		"framespersecond","FPS",				LABEL("Frames/Second:"),
		6, valInt,
		"14",
		4,0,TabMapInfo, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(   "The number of frames per second the server should strive for.\n"
		)
	);

	prefsArray[i++] = selfImmunity = ServerOptionFactory(this,
		"selfimmunity","",						LABEL("Self Immunity?"),
		0, valBool,
		"no",
		10,0,TabMapInfo, OPTIONALL,
		TUNERDEF(TunerDummy),
		HELPTEXT(	"Are players immune to their own weapons?\n"
		)
	);

	prefsArray[i++] = gravity = ServerOptionFactory(this,
		"gravity"	   ,"",						LABEL("Gravity"),
		6, valReal,
		"-0.14",
		0, 1, TabMapInfo,  OPTIONALL,
		TUNERDEF(TunerGravity),
		HELPTEXT(	"Gravity strength.\n"
		)
	);

	prefsArray[i++] = gravityAngle = ServerOptionFactory(this,
		"gravityangle" ,"",						LABEL("Gravity Angle"),
		3, valInt,
		"90"  ,
		1,1,TabMapInfo, OPTIONALL,
		TUNERDEF(TunerGravity),
		HELPTEXT(	"If gravity is along a uniform line, at what angle is that line?\n"
		)
	);

	prefsArray[i++] = gravityPoint = ServerOptionFactory(this,
		"gravitypoint" ,"",						LABEL("Gravity Point"),
		7, valIPos,
		"0,0",
		2,1,TabMapInfo, OPTIONALL,
		TUNERDEF(TunerGravity),
		HELPTEXT(	"If the gravity is a point source where does that gravity originate?\n"
					"Specify the point int the form: x,y.\n"
		)
	);

	prefsArray[i++] = gravityPointSource = ServerOptionFactory(this,
		"gravitypointsource","",				LABEL("Point Source?"),
		0, valBool,
		"no",
		3,1,TabMapInfo, OPTIONALL,
		TUNERDEF(TunerGravity),
		HELPTEXT(	"Is gravity originating from a single point?\n"
		)
	);

	prefsArray[i++] = gravityClockwise = ServerOptionFactory(this,
		"gravityclockwise","",					LABEL("Clockwise?"),
		0, valBool,
		"no",
		4,1,TabMapInfo, OPTIONALL,
		TUNERDEF(TunerGravity),
		HELPTEXT(	"If the gravity is a point source, is it clockwise?\n"
		)
	);

	prefsArray[i++] = gravityAnticlockwise = ServerOptionFactory(this,
		"gravityanticlockwise","",				LABEL("Anti-Clockwise?"),
		0, valBool,
		"no",
		5,1,TabMapInfo, OPTIONALL,
		TUNERDEF(TunerGravity),
		HELPTEXT(	"If the gravity is a point source, is it anticlockwise?\n"
		)
	);

	prefsArray[i++] = shotsGravity = ServerOptionFactory(this,
		"shotsgravity" ,"",						LABEL("Shots Gravity?"),
		0, valBool,
		"yes",
		6,1,TabMapInfo, OPTIONALL,
		TUNERDEF(TunerGravity),
		HELPTEXT(	"Are bullets afflicted by gravity.\n"
		)
	);

	prefsArray[i++] = gravityVisible = ServerOptionFactory(this,
		"gravityvisible","",					LABEL("Gravity Visible?"),
		6, valBool,
		"yes",
		7,1,TabMapInfo, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"Are gravity mapsymbols visible to players?\n"
		)
	);

	prefsArray[i++] = coriolis = ServerOptionFactory(this,
		"coriolis","",							LABEL("Coriolis:"),
		6, valInt,
		"0",
		8,1,TabMapInfo, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"The clockwise angle (in degrees) a ship's velocity turns each frame.\n"
		)
	);

	prefsArray[i++] = friction = ServerOptionFactory(this,
		"friction","",							LABEL("Friction:"),
		19, valReal,
		"0.0",
		10,1,TabMapInfo, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"Fraction of velocity ship loses each frame.\n"
		)
	);

	prefsArray[i++] = blockFriction = ServerOptionFactory(this,
		"blockfriction","",						LABEL("Block Friction:"),
		19, valReal,
		"0.0",
		11,1,TabMapInfo, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"Fraction of velocity ship loses each frame when it is in friction blocks.\n"
		)
	);

	prefsArray[i++] = defaultShipShape = ServerOptionFactory(this,
		"defaultshipshape","",					LABEL("Default Ship Shape:"),
		255, valString,
		"(NM:Default)(AU:Unknown)(SH: 15,0 -9,8 -9,-8)(MG: 15,0)(LG: 15,0)(RG: 15,0)(EN: -9,0)(LR: -9,8)(RR: -9,-8)(LL: -9,8)(RL: -9,-8)(MR: 15,0)",
		13,1,TabMapInfo, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"What is the default ship shape for people who do not have a ship\n"
					"shape defined?"
		)
	);

	prefsArray[i++] = tankShipShape = ServerOptionFactory(this,
		"tankshipshape","",						LABEL("Tank Ship Shape:"),
		255, valString,
		"(NM:fueltank)(AU:John E. Norlin)(SH: 15,0 14,-5 9,-8 -5,-8 -3,-8 -3,0 2,0 2,2 -3,2 -3,6 5,6 5,8 -5,8 -5,-8 -9,-8 -14,-5 -15,0 -14,5 -9,8 9,8 14,5)(EN: -15,0)(MG: 15,0)",
		14,1,TabMapInfo, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"What is the ship shape used for tanks?"
		)
	);

	prefsArray[i++] = minItemMass = ServerOptionFactory(this,
		"itemmass"	   ,"",						LABEL("Item Mass:"),
		6, valReal,
		"1.0",
		0,2,TabMapInfo, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"Mass of items.\n"
		)
	);

	prefsArray[i++] = shipMass = ServerOptionFactory(this,
		"shipmass"	   ,"",						LABEL("Ship Mass:"),
		6, valReal,
		"20.0",
		1,2,TabMapInfo, OPTIONALL,
		TUNERDEF(TunerShipMass),
		HELPTEXT(	"Mass of fighters.\n"
		)
	);

	prefsArray[i++] = shotMass = ServerOptionFactory(this,
		"shotmass","",							LABEL("Shot Mass:"),
		6, valReal,
		"0.1",
		3,2,TabMapInfo, OPTIONALL,
		TUNERDEF(TunerDummy),
		HELPTEXT(	"Mass of bullets.\n"
		)
	);

	prefsArray[i++] = shotSpeed = ServerOptionFactory(this,
		"shotspeed","",							LABEL("Shot Speed:"),
		6, valReal,
		"21.0",
		4,2,TabMapInfo, OPTIONALL,
		TUNERDEF(TunerDummy),
		HELPTEXT(	"Maximum speed of bullets.\n"
		)
	);

	prefsArray[i++] = shotLife = ServerOptionFactory(this,
		"shotlife","",							LABEL("Shot Life:"),
		3, valInt,
		"60"  ,
		5,2,TabMapInfo, OPTIONALL,
		TUNERDEF(TunerDummy),
		HELPTEXT(	"Life of bullets in ticks.\n"
		)
	);

	prefsArray[i++] = shotHitFuelDrainUsesKineticEnergy = ServerOptionFactory(this,
		"shotHitFuelDrainUsesKineticEnergy","",	LABEL("Kinetic Energy?"),
		0, valBool,
		"yes",
		6,2,TabMapInfo, OPTIONALL,
		TUNERDEF(TunerDummy),
		HELPTEXT(	"Does the fuel drain from shot hits depend on their mass and speed?\n"
					"This is a temporary option that will be removed in March 2002.\n"
		)
	);

	prefsArray[i++] = maxPlayerShots = ServerOptionFactory(this,
		"maxplayershots","shots",				LABEL("Max. Shots:"),
		3, valInt,
		"256",
		7,2,TabMapInfo, OPTIONALL,
		TUNERDEF(TunerShotsMax),
		HELPTEXT(	"Maximum allowed bullets per player.\n"
		)
	);

	prefsArray[i++] = fireRepeatRate = ServerOptionFactory(this,
		"firerepeatrate","firerepeat",			LABEL("Fire Repeat Rate:"),
		19, valInt,
		"2",
		8,2,TabMapInfo, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"Number of frames per automatic fire (0=off).\n"
		)
	);

	prefsArray[i++] = keepShots = ServerOptionFactory(this,
		"keepshots","",							LABEL("KeepShots"),
		0, valBool,
		"no",
		9,2,TabMapInfo, OPTIONALL,
		TUNERDEF(TunerDummy),
		HELPTEXT(	"Do shots, mines and missiles remain after their owner leaves?\n"
		)
	);

	prefsArray[i++] = edgeBounce = ServerOptionFactory(this,
		"edgebounce","", 						LABEL("Edge Bounce?"),
		0, valBool,
		"yes"	,
		10,2,TabMapInfo, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"Players and bullets bounce when they hit the (non-wrapping) edge.\n"
		)
	);

	prefsArray[i++] = edgeWrap = ServerOptionFactory(this,
		"edgewrap"	   ,"",						LABEL("Edge Wrap?"),
		0, valBool,
		"no"   ,
		11,2,TabMapInfo, OPTIONALL,
		TUNERDEF(TunerRules),
		HELPTEXT(	"Wrap around edges.\n"
		)
	);

	prefsArray[i++] = extraBorder = ServerOptionFactory(this,
		"extraborder"  ,"",						LABEL("Extra Border?"),
		0, valBool,
		"no",
		12,2,TabMapInfo, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"Give map an extra border of solid rock.\n"
		)
	);

	prefsArray[i++] = turnThrust = ServerOptionFactory(this,
		"turnthrust"  ,"turnfuel",				LABEL( "Turn Thrust?"),
		0, valBool,
		"no",
		14,2,TabMapInfo, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"Does turning use fuel and create sparks?\n"
		)
	);

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
// Robots/Bounce tab
	prefsArray[i++] = robotsTalk = ServerOptionFactory(this,
		"robotstalk","",						LABEL("Robots Talk?"),
		0, valBool,
		"no",
		0,0,TabRbt, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"Do robots talk when they kill, die etc.?\n"
		)
	);

	prefsArray[i++] = robotsLeave = ServerOptionFactory(this,
		"robotsleave","",						LABEL("Robots Leave?"),
		0, valBool,
		"yes",
		1,0,TabRbt, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"Do robots leave the game?\n"
		)
	);

	prefsArray[i++] = robotLeaveLife = ServerOptionFactory(this,
		"robotleavelife","",					LABEL("Robot Leave Life:"),
		19, valInt,
		"50",
		2,0,TabRbt, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"Max life per robot (0=off).\n"
		)
	);

	prefsArray[i++] = robotLeaveScore = ServerOptionFactory(this,
		"robotleavescore","",					LABEL("Robot Leave Score:"),
		19, valInt,
		"-90",
		3,0,TabRbt, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"Min score for robot to play (0=off).\n"
		)
	);

	prefsArray[i++] = robotLeaveRatio = ServerOptionFactory(this,
		"robotleaveratio","",					LABEL("Robot Leave Ratio:"),
		19, valReal,
		"-5",
		4,0,TabRbt, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"Min ratio for robot to play (0=off).\n"
		)
	);

	prefsArray[i++] = minRobots= ServerOptionFactory(this,
		"minrobots","",							LABEL("Min. Robots"),
		19, valReal,
		"-1",
		6,0,TabRbt, OPTIONALL,
		TUNERDEF(TunerMinRobots),
		HELPTEXT(	"The minimum number of robots wanted.\n"
					"At least minRobots robots will be in the game, if there is room.\n"
		)
	);

	prefsArray[i++] = maxRobots = ServerOptionFactory(this,
		"maxrobots"    ,"robots",				LABEL("Max. Robots:"),
		2, valInt,
		"4"  ,
		7,0,TabRbt, OPTIONALL,
		TUNERDEF(TunerMaxRobots),
		HELPTEXT(	"The maximum number of robots wanted.\n"
					"Adds robots if there are less than maxRobots players.\n"
		)
	);

	prefsArray[i++] = robotTeam = ServerOptionFactory(this,
		"robotteam"    ,"",						LABEL("Robot Team:"),
		2, valInt,
		"0"  ,
		9,0,TabRbt, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"Team to use for robots.\n"
		)
	);

	prefsArray[i++] = restrictRobots = ServerOptionFactory(this,
		"restrictrobots","",					LABEL("Restrict Robots?"),
		0, valBool,
		"yes",
		10,0,TabRbt, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"Are robots restricted to their own team?\n"
		)
	);

	prefsArray[i++] = reserveRobotTeam = ServerOptionFactory(this,
		"reserverobotteam","",					LABEL("Resrve Rob Team?"),
		0, valBool,
		"yes",
		11,0,TabRbt, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"Is the robot team only for robots?\n"
		)
	);

	prefsArray[i++] = robotRealName = ServerOptionFactory(this,
		"robotrealname","",						LABEL("Robot Real Name:"),
		255, valString,
		"robot",
		13,0,TabRbt, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"What is the robots' apparent real name?\n"
		)
	);

	prefsArray[i++] = robotHostName = ServerOptionFactory(this,
		"robothostname","",						LABEL("Robot Host Name:"),
		255, valString,
		"xpilot.org",
		14,0,TabRbt, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"What is the robots' apparent host name?\n"
		)
	);

	prefsArray[i++] = tankRealName = ServerOptionFactory(this,
		"tankrealname","",						LABEL("Tank Real Name:"),
		255, valString,
		"tank",
		16,0,TabRbt, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"What is the tanks' apparent real name?\n"
		)
	);

	prefsArray[i++] = tankHostName = ServerOptionFactory(this,
		"tankhostname","",						LABEL("Tank Host Name:"),
		255, valString,
		"tanks.org",
		17,0,TabRbt, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"What is the tanks' apparent host name?\n"
		)
	);

	prefsArray[i++] = shotsWallBounce = ServerOptionFactory(this,
		"shotswallbounce","",					LABEL("Shots Bounce?"),
		0, valBool,
		"no",
		0,1,TabRbt, OPTIONALL,
		TUNERDEF(TunerBounce),
		HELPTEXT(	"Do shots bounce off walls?\n"
		)
	);

	prefsArray[i++] = ballsWallBounce = ServerOptionFactory(this,
		"ballswallbounce","",					LABEL("Balls Bounce?"),
		0, valBool,
		"yes",
		1,1,TabRbt, OPTIONALL,
		TUNERDEF(TunerBounce),
		HELPTEXT(	"Do balls bounce off walls?\n"
		)
	);

	prefsArray[i++] = ballCollisions = ServerOptionFactory(this,
		"ballCollisions", "",					LABEL("Collide with Balls?"),
		0, valBool,
		"no",
		2,1,TabRbt, OPTIONALL,
		TUNERDEF(TunerDummy),
		HELPTEXT(	"Can balls collide with other objects?\n"
		)
	);

	prefsArray[i++] = ballSparkCollisions = ServerOptionFactory(this,
		"ballSparkCollisions", "",				LABEL("Ball Spark Collisions?"),
		0, valBool,
		"yes",
		3,1,TabRbt, OPTIONALL,
		TUNERDEF(TunerDummy),
		HELPTEXT(	"Can balls be blown around by exhaust? (Needs ballCollsions too)\n"
		)
	);

	prefsArray[i++] = minesWallBounce = ServerOptionFactory(this,
		"mineswallbounce","",					LABEL("Mines Bounce?"),
		0, valBool,
		"no",
		4,1,TabRbt, OPTIONALL,
		TUNERDEF(TunerBounce),
		HELPTEXT(	"Do mines bounce off walls?\n"
		)
	);

	prefsArray[i++] = itemsWallBounce = ServerOptionFactory(this,
		"itemswallbounce","",					LABEL("Items Bounce?"),
		0, valBool,
		"yes",
		5,1,TabRbt, OPTIONALL,
		TUNERDEF(TunerBounce),
		HELPTEXT(	"Do items bounce off walls?\n"
		)
	);

	prefsArray[i++] = missilesWallBounce = ServerOptionFactory(this,
		"missileswallbounce","",				LABEL("Missiles Bounce?"),
		0, valBool,
		"no",
		6,1,TabRbt, OPTIONALL,
		TUNERDEF(TunerBounce),
		HELPTEXT(	"Do missiles bounce off walls?\n"
		)
	);

	prefsArray[i++] = sparksWallBounce = ServerOptionFactory(this,
		"sparkswallbounce","",					LABEL("Sparks Bounce?"),
		0, valBool,
		"no",
		7,1,TabRbt, OPTIONALL,
		TUNERDEF(TunerBounce),
		HELPTEXT(	"Do thrust spark particles bounce off walls to give reactive thrust?\n"
		)
	);

	prefsArray[i++] = debrisWallBounce = ServerOptionFactory(this,
		"debriswallbounce","",					LABEL("Debris Bounce?"),
		0, valBool,
		"no",
		8,1,TabRbt, OPTIONALL,
		TUNERDEF(TunerBounce),
		HELPTEXT(	"Do explosion debris particles bounce off walls?\n"
		)
	);

	prefsArray[i++] = asteroidsWallBounce = ServerOptionFactory(this,
		"asteroidswallbounce","",				LABEL("Asteroids Bounce?"),
		0, valBool,
		"no",
		9,1,TabRbt, OPTIONALL,
		TUNERDEF(TunerBounce),
		HELPTEXT(	"Do asteroids bounce off walls?\n"
		)
	);

	prefsArray[i++] = maxObjectWallBounceSpeed = ServerOptionFactory(this,
		"maxobjectwallbouncespeed","maxobjectbouncespeed",	LABEL("MaxObjectBounceSpeed"),
		19, valReal,
		"40",
		0,2,TabRbt, OPTIONALL,
		TUNERDEF(TunerBounce),
		HELPTEXT(	"The maximum allowed speed for objects to bounce off walls.\n"
		)
	);

	prefsArray[i++] = maxShieldedWallBounceSpeed = ServerOptionFactory(this,
		"maxshieldedwallbouncespeed","maxshieldedbouncespeed",	LABEL("MaxShieldedBounceSpeed"),
		19, valReal,
		"50",
		1,2,TabRbt, OPTIONALL,
		TUNERDEF(TunerBounce),
		HELPTEXT(	"The maximum allowed speed for a shielded player to bounce off walls.\n"
		)
	);

	prefsArray[i++] = maxUnshieldedWallBounceSpeed = ServerOptionFactory(this,
		"maxunshieldedwallbouncespeed","maxunshieldedbouncespeed",	LABEL("MaxUnshieldedBounceSpeed"),
		19, valReal,
		"20",
		2,2,TabRbt, OPTIONALL,
		TUNERDEF(TunerBounce),
		HELPTEXT(	"Maximum allowed speed for an unshielded player to bounce off walls.\n"
		)
	);

	prefsArray[i++] = maxShieldedPlayerWallBounceAngle = ServerOptionFactory(this,
		"maxshieldedplayerwallbounceangle","maxshieldedbounceangle", LABEL("MaxShieldedBounceAngle"),
		19, valReal,
		"90",
		3,2,TabRbt, OPTIONALL,
		TUNERDEF(TunerBounce),
		HELPTEXT(	"Maximum allowed angle for a shielded player to bounce off walls.\n"
		)
	);

	prefsArray[i++] = maxUnshieldedPlayerWallBounceAngle = ServerOptionFactory(this,
		"maxunshieldedplayerwallbounceangle","maxunshieldedbounceangle",LABEL("MaxUnshieldedBounceAngle:"),
		19, valReal,
		"30",
		4,2,TabRbt, OPTIONALL,
		TUNERDEF(TunerBounce),
		HELPTEXT(	"Maximum allowed angle for an unshielded player to bounce off walls.\n"
		)
	);

	prefsArray[i++] = playerWallBounceBrakeFactor = ServerOptionFactory(this,
		"playerwallbouncebrakefactor","playerwallbrake",	LABEL("Plyr Brake Fact"),
		19, valReal,
		"0.89",
		5,2,TabRbt, OPTIONALL,
		TUNERDEF(TunerBounce),
		HELPTEXT(	"Factor to slow down players when they hit the wall (between 0 and 1).\n"
		)
	);

	prefsArray[i++] = objectWallBounceBrakeFactor = ServerOptionFactory(this,
		"objectwallbouncebrakefactor","objectwallbrake",	LABEL("Obj Brake Fact:"),
		19, valReal,
		"0.95",
		6,2,TabRbt, OPTIONALL,
		TUNERDEF(TunerBounce),
		HELPTEXT(	"Factor to slow down objects when they hit the wall (between 0 and 1).\n"
		)
	);

	prefsArray[i++] = objectWallBounceLifeFactor = ServerOptionFactory(this,
		"objectwallbouncelifefactor","",					LABEL("Object Life Factor:"),
		19, valReal,
		"0.80",
		7,2,TabRbt, OPTIONALL,
		TUNERDEF(TunerBounce),
		HELPTEXT(	"Factor to reduce the life of objects after bouncing (between 0 and 1).\n"
		)
	);

	prefsArray[i++] = wallBounceFuelDrainMult = ServerOptionFactory(this,
		"wallbouncefueldrainmult","wallbouncedrail",		LABEL("Fuel Drain Mult:"),
		19, valReal,
		"1.0",
		8,2,TabRbt, OPTIONALL,
		TUNERDEF(TunerBounce),
		HELPTEXT(	"Multiplication factor for player wall bounce fuel cost.\n"
		)
	);

	prefsArray[i++] = wallBounceDestroyItemProb = ServerOptionFactory(this,
		"wallbouncedestroyitemprob","",						LABEL("BounceDestroyItemProb:"),
		19, valReal,
		"0.0",
		9,2,TabRbt, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"The probability for each item a player owns to get destroyed\n"
			"when the player bounces against a wall.\n"
		)
	);

	prefsArray[i++] = loseItemDestroys = ServerOptionFactory(this,
		"loseitemdestroys","",								LABEL("Lose Item Dests?"),
		0, valBool,
		"no",
		11,2,TabRbt, OPTIONALL,
		TUNERDEF(TunerDummy),
		HELPTEXT(	"Destroy item that player drops. Otherwise drop it.\n"
		)
	);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// Visibility/Teams tab
	prefsArray[i++] = limitedVisibility = ServerOptionFactory(this,
		"limitedvisibility","",							LABEL("Limited Visibility?"),
		0, valBool,
		"no",
		0,0,TabVis, OPTIONALL,
		TUNERDEF(TunerRules),
		HELPTEXT(	"Should the players have a limited visibility?\n"
		)
	);

	prefsArray[i++] = minVisibilityDistance = ServerOptionFactory(this,
		"minvisibilitydistance","minvisibility",			LABEL("Min Visibility Dist:"),
		19, valReal,
		"0.0",
		1,0,TabVis, OPTIONALL,
		TUNERDEF(TunerDummy),
		HELPTEXT(	"Minimum block distance for limited visibility, 0 for default.\n"
		)
	);

	prefsArray[i++] = maxVisibilityDistance = ServerOptionFactory(this,
		"maxvisibilitydistance","maxvisibility",			LABEL("Max Visibility Dist:"),
		19, valReal,
		"0.0",
		2,0,TabVis, OPTIONALL,
		TUNERDEF(TunerDummy),
		HELPTEXT(	"Maximum block distance for limited visibility, 0 for default.\n"
		)
	);

	prefsArray[i++] = wormholeVisible = ServerOptionFactory(this,
		"wormholevisible","",								LABEL("Wormhole Visible?"),
		6, valBool,
		"yes",
		3,0,TabVis, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"Are wormhole mapsymbols visible to players?\n"
		)
	);

	prefsArray[i++] = itemConcentratorVisible = ServerOptionFactory(this,
		"itemConcentratorVisible","",						LABEL("Item Conc Vis?"),
		6, valBool,
		"yes",
		4,0,TabVis, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"Are itemconcentrator mapsymbols visible to players?\n"
		)
	);

	prefsArray[i++] = asteroidConcentratorVisible = ServerOptionFactory(this,
		"asteroidConcentratorVisible", "",					LABEL("Asteroid Conc Vis?"),
		0, valBool,
		"yes",
		5,0,TabVis, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"Are asteroid concentrator map symbols visible to players?\n"
		)
	);

	prefsArray[i++] = blockFrictionVisible = ServerOptionFactory(this,
		"blockfrictionvisible","",							LABEL("BlockFriction Vis?"),
		6, valBool,
		"true",
		6,0,TabVis, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"Are friction blocks visible?\n"
					"If true, friction blocks show up as decor;\n"
					"if false, they don't show up at all.\n"
		)
	);

	prefsArray[i++] = wormTime = ServerOptionFactory(this,
		"wormtime","",										LABEL("Wormhole Time:"),
		19, valInt,
		"0",
		8,0,TabVis, OPTIONALL,
		TUNERDEF(TunerWormTime),
		HELPTEXT(	"Number of seconds wormholes will remain stable.\n"
		)
	);

	prefsArray[i++] = playerStartsShielded = ServerOptionFactory(this,
		"playerstartsshielded","playerstartshielded",		LABEL("Start Shielded?"),
		0, valBool,
		"yes",
		10,0,TabVis, OPTIONALL,
		TUNERDEF(TunerPlayerStartsShielded),
		HELPTEXT(	"Do players start with shields up?\n"
		)
	);

	prefsArray[i++] = shieldedItemPickup = ServerOptionFactory(this,
		"shieldeditempickup","shielditem",					LABEL("Shielded Pickup?"),
		0, valBool,
		"no",
		11,0,TabVis, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"Can items be picked up while shields are up?\n"
		)
	);

	prefsArray[i++] = shieldedMining = ServerOptionFactory(this,
		"shieldedmining","shieldmine",						LABEL("Shielded Mining?"),
		0, valBool,
		"no",
		12,0,TabVis, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"Can mines be thrown and placed while shields are up?\n"
		)
	);

	prefsArray[i++] = targetKillTeam = ServerOptionFactory(this,
		"targetkillteam","",								LABEL("Target Kill Team?"),
		0, valBool,
		"no",
		0,1,TabVis, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"Do team members die when their last target explodes?\n"
		)
	);

	prefsArray[i++] = targetTeamCollision = ServerOptionFactory(this,
		"targetteamcollision","targetcollision",			LABEL("Target Collision?"),
		0, valBool,
		"yes",
		1,1,TabVis, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"Do team members collide with their own target or not.\n"
		)
	);

	prefsArray[i++] = targetSync = ServerOptionFactory(this,
		"targetsync","",									LABEL("Target Sync?"),
		0, valBool,
		"no",
		2,1,TabVis, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"Do all the targets of a team reappear/repair at the same time?"
		)
	);

	prefsArray[i++] = targetDeadTime = ServerOptionFactory(this,
		"targetdeadtime","",								LABEL("Target Dead Time:"),
		19, valReal,
		"60",
		3,1,TabVis, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"How many seconds do targets stay destroyed?\n"
		)
	);

	prefsArray[i++] = treasureKillTeam = ServerOptionFactory(this,
		"treasurekillteam","",								LABEL("Treas. Kill Team?"),
		0, valBool,
		"no",
		5,1,TabVis, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"Do team members die when their treasure is destroyed?\n"
		)
	);

	prefsArray[i++] = treasureCollisionDestroys = ServerOptionFactory(this,
		"treasurecollisiondestroys","treasurecollisiondestroy",	LABEL("Tres Col Dests?"),
		0, valBool,
		"yes",
		6,1,TabVis, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"Are balls destroyed when a player touches it?\n"
		)
	);

	prefsArray[i++] = treasureCollisionMayKill = ServerOptionFactory(this,
		"treasurecollisionmaykill","treasureunshieldedcollisionkills",LABEL("Tres Col Kills?"),
		0, valBool,
		"no",
		7,1,TabVis, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"Does a ball kill a player when the player touches it unshielded?\n"
		)
	);

	prefsArray[i++] = ballConnectorLength = ServerOptionFactory(this,
		"ballconnectorlength","",							LABEL("BallConnectorLength:"),
		19, valReal,
		"120",
		8,1,TabVis, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"How long is a normal connector string?\n"
		)
	);

	prefsArray[i++] = connectorIsString = ServerOptionFactory(this,
		"connectorIsString", "",							LABEL("ConnectorIsString:"),
		0, valBool,
		"no",
		9,1,TabVis, OPTIONALL,
		TUNERDEF(TunerDummy),
		HELPTEXT(	"Is the ball connector made of string?\n"
		)
	);

	prefsArray[i++] = maxBallConnectorRatio = ServerOptionFactory(this,
		"maxballconnectorratio","",							LABEL("MaxBallConnectorRatio:"),
		19, valReal,
		"0.30",
		10,1,TabVis, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"How much longer or shorter can a connecter get before it breaks?\n"
		)
	);

	prefsArray[i++] = ballConnectorDamping = ServerOptionFactory(this,
		"ballconnectordamping","",							LABEL("BallConnectorDamping:"),
		19, valReal,
		"2.0",
		11,1,TabVis, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"What is the damping force on connectors?\n"
		)
	);

	prefsArray[i++] = ballConnectorSpringConstant = ServerOptionFactory(this,
		"ballconnectorspringconstant","",					LABEL("BallConnectorSpringConst:"),
		19, valReal,
		"1500.0",
		12,1,TabVis, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"What is the spring constant for connectors?\n"
		)
	);

	prefsArray[i++] = ballMass = ServerOptionFactory(this,
		"ballmass","",										LABEL("BallMass:"),
		19, valReal,
		"50.0",
		13,1,TabVis, OPTIONALL,
		TUNERDEF(TunerBallMass),
		HELPTEXT(	"Mass of balls.\n"
		)
	);

	prefsArray[i++] = playersOnRadar = ServerOptionFactory(this,
		"playersonradar","playersradar",					LABEL("Players on Radar?"),
		0, valBool,
		"yes",
		0,2,TabVis, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"Are players visible on the radar.\n"
		)
	);

	prefsArray[i++] = missilesOnRadar = ServerOptionFactory(this,
		"missilesonradar","missilesradar",					LABEL("Missles on Radar?"),
		0, valBool,
		"yes",
		1,2,TabVis, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"Are missiles visible on the radar.\n"
		)
	);

	prefsArray[i++] = minesOnRadar = ServerOptionFactory(this,
		"minesonradar","minesradar",						LABEL("Mines on Radar?"),
		0, valBool,
		"no",
		2,2,TabVis, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"Are mines visible on the radar.\n"
		)
	);

	prefsArray[i++] = nukesOnRadar = ServerOptionFactory(this,
		"nukesonradar","nukesradar",						LABEL("Nukes on Radar?"),
		0, valBool,
		"no",
		3,2,TabVis, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"Are nukes visible or highlighted on the radar.\n"
		)
	);

	prefsArray[i++] = treasuresOnRadar = ServerOptionFactory(this,
		"treasuresonradar","treasuresradar",				LABEL("Treas. on Radar?"),
		0, valBool,
		"no",
		4,2,TabVis, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"Are treasure balls visible or highlighted on the radar.\n"
		)
	);

	prefsArray[i++] = teamPlay = ServerOptionFactory(this,
		"teamplay"	   ,"teams",							LABEL("Team Play?"),
		0, valBool,
		"no",
		6,2,TabVis, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"Is the map a team play map?\n"
		)
	);

	prefsArray[i++] = teamAssign = ServerOptionFactory(this,
		"teamassign","",									LABEL("Team Assign?"),
		0, valBool,
		"yes",
		7,2,TabVis, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"If players have not specified which team they like to join\n"
					"should the server choose a team for them automatically?\n"
		)
	);

	prefsArray[i++] = teamImmunity = ServerOptionFactory(this,
		"teamimmunity","",									LABEL("Team Immunity?"),
		0, valBool,
		"yes",
		8,2,TabVis, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"Should other team members be immune to various shots thrust etc.?\n"
					"This works for alliances too.\n"
		)
	);

	prefsArray[i++] = teamShareScore = ServerOptionFactory(this,
		"teamShareScore", "",								LABEL("Teams Share Score?"),
		0, valBool,
		"no",
		9,2,TabVis, OPTIONALL,
		TUNERDEF(TunerDummy),
		HELPTEXT(	"All points gained or lost divided equally over all team members?\n"
					"This works for alliances too.\n"
		)
	);

	prefsArray[i++] = teamCannons = ServerOptionFactory(this,
		"teamcannons","",									LABEL("Team Cannons?"),
		0, valBool,
		"no",
		10,2,TabVis, OPTIONALL,
		TUNERDEF(TunerTeamCannons),
		HELPTEXT(	"Do cannons choose sides in teamPlay?\n"
		)
	);

	prefsArray[i++] = teamFuel = ServerOptionFactory(this,
		"teamfuel","",										LABEL("Team Fuel?"),
		0, valBool,
		"no",
		11,2,TabVis, OPTIONALL,
		TUNERDEF(TunerDummy),
		HELPTEXT(	"Are fuelstations only available to team members?\n"
		)
	);

	prefsArray[i++] = captureTheFlag = ServerOptionFactory(this,
		"capturetheflag","ctf",								LABEL("Capture The Flag?"),
		0, valBool,
		"no",
		12,2,TabVis, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"Does a team's treasure have to be safe before enemy balls\n"
					"can be cashed?\n"
		)
	);

	prefsArray[i++] = cloakedExhaust = ServerOptionFactory(this,
		"cloakedexhaust","",								LABEL("Cloaked Exhaust?"),
		6, valBool,
		"no",
		14,2,TabVis, OPTIONALL,
		TUNERDEF(TunerDummy),
		HELPTEXT(	"Do engines of cloaked ships generate exhaust?\n"
		)
	);

	prefsArray[i++] = cloakedShield = ServerOptionFactory(this,
		"cloakedshield","",									LABEL("Cloaked Shield?"),
		6, valBool,
		"no",
		15,2,TabVis, OPTIONALL,
		TUNERDEF(TunerDummy),
		HELPTEXT(	"Can players use shields when cloaked?\n"
		)
	);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// Cannons/Mines tab
	prefsArray[i++] = cannonSmartness = ServerOptionFactory(this,
		"cannonsmartness","",								LABEL("Cannon Smartness:"),
		19, valInt,
		"1",
		0,0,TabCan, OPTIONALL,
		TUNERDEF(TunerCannonSmartness),
		HELPTEXT(	"0: dumb (straight ahead),\n"
					"1: default (random direction),\n"
					"2: good (small error),\n"
					"3: accurate (aims at predicted player position).\n"
					"Also influences use of weapons if cannonsUseItems is on.\n"
		)
	);

	prefsArray[i++] = cannonsUseItems = ServerOptionFactory(this,
		"cannonsuseitems","cannonspickupitems",				LABEL("Cannons Use Items?"),
		0, valBool,
		"no",
		1,0,TabCan, OPTIONALL,
		TUNERDEF(TunerCannonsUseItems),
		HELPTEXT(	"Do cannons use items?\n"
		)
	);

	prefsArray[i++] = cannonsDefend = ServerOptionFactory(this,
		"cannonsDefend", "",								LABEL("Cannons Defend?"),
		0, valBool,
		"yes",
		2,0,TabCan, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"Do cannons actively use defensive items like emergency shields and\n"
					"phasing devices?\n"
					"This only works if cannons are actually allowed to use items.\n"
		)
	);

	prefsArray[i++] = cannonFlak = ServerOptionFactory(this,
		"cannonFlak", "cannonAAA",							LABEL("Cannon Flak?"),
		0, valBool,
		"yes",
		3,0,TabCan, OPTIONALL,
		TUNERDEF(TunerDummy),
		HELPTEXT(	"Do cannons fire flak or normal bullets?"
		)
	);

	prefsArray[i++] = cannonDeadTime = ServerOptionFactory(this,
		"cannondeadtime","",								LABEL("Cannon Dead Time:"),
		19, valInt,
		"72",
		4,0,TabCan, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"How many seconds do cannons stay dead?\n"
		)
	);

	prefsArray[i++] = identifyMines = ServerOptionFactory(this,
		"identifymines","",									LABEL("Identify Mines?"),
		0, valBool,
		"yes",
		7,0,TabCan, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"Are mine owner's names displayed.\n"
		)
	);

	prefsArray[i++] = maxMinesPerPack = ServerOptionFactory(this,
		"maxminesperpack","",								LABEL("Mines/Pac"),
		19, valInt,
		"2",
		8,0,TabCan, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"The number of mines gotten by picking up one mine item.\n"
		)
	);

	prefsArray[i++] = mineLife = ServerOptionFactory(this,
		"minelife","",										LABEL("Mine Life"),
		19, valReal,
		"0",
		9,0,TabCan, OPTIONALL,
		TUNERDEF(TunerMineLife),
		HELPTEXT(	"Life of mines in ticks, zero means use default.\n"
		)
	);

	prefsArray[i++] = mineFuseTime = ServerOptionFactory(this,
		"minefusetime","",									LABEL("Mine Fuse Time:"),
		19, valInt,
		"0.0",
		10,0,TabCan, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"Number of seconds after which owned mines become deadly, zero means never.\n"
		)
	);

	prefsArray[i++] = baseMineRange = ServerOptionFactory(this,
		"baseminerange","",									LABEL("Base Mine Range:"),
		19, valInt,
		"0",
		11,0,TabCan, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"Range within which mines/bombs are not allowed.\n"
		)
	);

	prefsArray[i++] = mineShotDetonateDistance = ServerOptionFactory(this,
		"mineShotDetonateDistance", "",						LABEL("MineShotDetonateDistance:"),
		6, valInt,
		"0",
		12,0,TabCan, OPTIONALL,
		TUNERDEF(TunerDummy),
		HELPTEXT(	"How close must a shot be to detonate a mine?\n"
					"Set this to 0 to turn it off."
		)
	);

	prefsArray[i++] = rogueMineProb = ServerOptionFactory(this,
		"roguemineprob","",									LABEL("Rogue Mine Prob:"),
		19, valReal,
		"1.0",
		13,0,TabCan, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"Probability that unclaimed mine items will activate."
		)
	);

	prefsArray[i++] = nukeMinMines = ServerOptionFactory(this,
		"nukeminmines","",									LABEL("Min Nuke Mines:"),
		6, valInt,
		"4",
		14,0,TabCan, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"The minimum number of mines needed to make a nuclear mine.\n"
		)
	);

	prefsArray[i++] = minMineSpeed = ServerOptionFactory(this,
		"minminespeed","",									LABEL("Min Mine Speed:"),
		19, valReal,
		"0",
		15,0,TabCan, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"Minimum speed of mines.\n"
		)
	);

	prefsArray[i++] = nukeClusterDamage = ServerOptionFactory(this,
		"nukeclusterdamage","",								LABEL("Nuke Clust Dam:"),
		19, valReal,
		"1.0",
		16,0,TabCan, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"How much each cluster debris does damage wise from a \n"
					"nuke mine.  This helps to reduce the number of particles\n"
					"caused by nuclear mine explosions, which improves server\n"
					"response time for such explosions.\n"
		)
	);

	prefsArray[i++] = ecmsReprogramRobots = ServerOptionFactory(this,
		"ecmsreprogramrobots","",							LABEL("EcmsReprgmRbts?"),
		6, valBool,
		"yes",
		0,1,TabCan, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"Are robots reprogrammed by ECMs instead of blinded?\n"
		)
	);

	prefsArray[i++] = ecmsReprogramMines = ServerOptionFactory(this,
		"ecmsreprogrammines","",							LABEL("EcmsRprgMines?"),
		0, valBool,
		"yes",
		1,1,TabCan, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"Is it possible to reprogram mines with ECMs?\n"
		)
	);

	prefsArray[i++] = distinguishMissiles = ServerOptionFactory(this,
		"distinguishmissiles","",							LABEL("Distng Missiles?"),
		0, valBool,
		"yes",
		3,1,TabCan, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"Are different types of missiles distinguished (by length).\n"
		)
	);

	prefsArray[i++] = maxMissilesPerPack = ServerOptionFactory(this,
		"maxmissilesperpack","",							LABEL("Missiles/Pack:"),
		6, valInt,
		"4",
		4,1,TabCan, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"The number of missiles gotten by picking up one missile item.\n"
		)
	);

	prefsArray[i++] = missileLife = ServerOptionFactory(this,
		"missilelife","",									LABEL("Missile Life"),
		19, valReal,
		"0",
		5,1,TabCan, OPTIONALL,
		TUNERDEF(TunerMissileLife),
		HELPTEXT(	"Life of missiles in ticks, zero means use default.\n"
		)
	);

	prefsArray[i++] = rogueHeatProb = ServerOptionFactory(this,
		"rogueheatprob","",									LABEL("Rogue Heat Prob:"),
		19, valReal,
		"1.0",
		6,1,TabCan, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"Probability that unclaimed missile packs will go rogue."
		)
	);

	prefsArray[i++] = nukeMinSmarts = ServerOptionFactory(this,
		"nukeminsmarts","",									LABEL("Min Nuke Miss:"),
		6, valInt,
		"7",
		7,1,TabCan, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"The minimum number of missiles needed to fire one nuclear missile.\n"
		)
	);

	prefsArray[i++] = asteroidCollisionMayKill = ServerOptionFactory(this,
		"asteroidcollisionmaykill","asteroidunshieldedcollisionkills", LABEL("Asteroid Col Kills?"),
		0, valBool,
		"yes",
		9,1,TabCan, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"Can ships be destroyed when hit by an asteroid?\n"
		)
	);

	prefsArray[i++] = asteroidsOnRadar = ServerOptionFactory(this,
		"asteroidsonradar","asteroidsradar",				LABEL("Asteroids on Radar?"),
		0, valBool,
		"no",
		10,1,TabCan, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"Are asteroids visible on the radar.\n"
		)
	);

	prefsArray[i++] = maxAsteroidDensity = ServerOptionFactory(this,
		"maxasteroiddensity","",							LABEL("MaxAsteroidDnsty:"),
		19, valReal,
		"0",
		11,1,TabCan, OPTIONALL,
		TUNERDEF(TunerAsteroidProb),
		HELPTEXT(	"Maximum density [0.0-1.0] for asteroids (max asteroids per block.\n"
		)
	);

	prefsArray[i++] = allowShipShapes = ServerOptionFactory(this,
		"allowshipshapes","shipshapes",						LABEL("Ship Shapes?"),
		0, valBool,
		"yes",
		0,2,TabCan, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"Are players allowed to define their own ship shape?\n"
		)
	);

	prefsArray[i++] = allowSmartMissiles = ServerOptionFactory(this,
		"allowsmartmissiles","allowsmarts",					LABEL("Allow Smarts?"),
		6, valBool,
		"yes",
		1,2,TabCan, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"Should smart missiles be allowed?\n"
		)
	);

	prefsArray[i++] = allowHeatSeekers = ServerOptionFactory(this,
		"allowheatseekers","allowheats",					LABEL("Allow Heats?"),
		6, valBool,
		"yes",
		2,2,TabCan, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"Should heatseekers be allowed?\n"
		)
	);

	prefsArray[i++] = allowTorpedoes = ServerOptionFactory(this,
		"allowtorpedoes","allowtorps",						LABEL("Allow Torps?"),
		6, valBool,
		"yes",
		3,2,TabCan, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"Should torpedoes be allowed?\n"
		)
	);

	prefsArray[i++] = allowPlayerCrashes = ServerOptionFactory(this,
		"allowplayercrashes","",							LABEL("Allow Crashes?"),
		0, valBool,
		"yes",
		4,2,TabCan, OPTIONALL,
		TUNERDEF(TunerRules),
		HELPTEXT(	"Can players overrun other players?\n"
		)
	);

	prefsArray[i++] = allowPlayerBounces = ServerOptionFactory(this,
		"allowplayerbounces","",							LABEL("Allow Bounces?"),
		0, valBool,
		"yes",
		5,2,TabCan, OPTIONALL,
		TUNERDEF(TunerRules),
		HELPTEXT(	"Can players bounce with other players?\n"
		)
	);

	prefsArray[i++] = allowPlayerKilling = ServerOptionFactory(this,
		"allowplayerkilling","killings",					LABEL("Allow Killing?"),
		0, valBool,
		"yes",
		6,2,TabCan, OPTIONALL,
		TUNERDEF(TunerRules),
		HELPTEXT(	"Should players be allowed to kill one other?\n"
		)
	);

	prefsArray[i++] = allowShields = ServerOptionFactory(this,
		"allowshields","shields",							LABEL("Allow Shields?"),
		0, valBool,
		"yes",
		7,2,TabCan, OPTIONALL,
		TUNERDEF(TunerPlayerShielding),
		HELPTEXT(	"Are shields allowed?\n"
		)
	);

	prefsArray[i++] = allowNukes = ServerOptionFactory(this,
		"allownukes","nukes",								LABEL("Allow Nukes?"),
		0, valBool,
		"no",
		8,2,TabCan, OPTIONALL,
		TUNERDEF(TunerModifiers),
		HELPTEXT(	"Should nuclear weapons be allowed?\n"
		)
	);

	prefsArray[i++] = allowClusters = ServerOptionFactory(this,
		"allowclusters","clusters",							LABEL("Allow Clusters?"),
		0, valBool,
		"no",
		9,2,TabCan, OPTIONALL,
		TUNERDEF(TunerModifiers),
		HELPTEXT(	"Should cluster weapons be allowed?\n"
		)
	);

	prefsArray[i++] = allowModifiers = ServerOptionFactory(this,
		"allowmodifiers","",								LABEL("Allow Mods?"),
		0, valBool,
		"no",
		10,2,TabCan, OPTIONALL,
		TUNERDEF(TunerModifiers),
		HELPTEXT(	"Should the weapon modifiers be allowed?\n"
		)
	);

	prefsArray[i++] = allowLaserModifiers = ServerOptionFactory(this,
		"allowlasermodifiers","lasermodifiers",				LABEL("Laser Mods?"),
		0, valBool,
		"no",
		11,2,TabCan, OPTIONALL,
		TUNERDEF(TunerModifiers),
		HELPTEXT(	"Can lasers be modified to be a different weapon?\n"
		)
	);

	prefsArray[i++] = laserIsStunGun = ServerOptionFactory(this,
		"laserisstungun","stungun",							LABEL("Laser Stun Gun?"),
		0, valBool,
		"no",
		13,2,TabCan, OPTIONALL,
		TUNERDEF(TunerModifiers),
		HELPTEXT(	"Is the laser weapon a stun gun weapon?\n"
		)
	);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// Rounds/Connection tab
	prefsArray[i++] = maxRoundTime = ServerOptionFactory(this,
		"maxroundtime","",									LABEL("MaxRoundTime:"),
		19, valInt,
		"0",
		0,0,TabRnd, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"The maximum duration of each round, in seconds.\n"
		)
	);

	prefsArray[i++] = gameDuration = ServerOptionFactory(this,
		"gameduration","",									LABEL("Game Duration:"),
		19, valReal,
		"0.0",
		1,0,TabRnd, OPTIONALL,
		TUNERDEF(TunerGameDuration),
		HELPTEXT(	"The duration of the game in minutes (aka. pizza mode).\n"
		)
	);

	prefsArray[i++] = roundDelay = ServerOptionFactory(this,
		"rounddelay","",									LABEL("Round Delay:"),
		19, valInt,
		"0",
		2,0,TabRnd, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"Delay before start of each round, in seconds.\n"
		)
	);

	prefsArray[i++] = roundsToPlay = ServerOptionFactory(this,
		"roundstoplay","",									LABEL("Rounds2Play:"),
		19, valInt,
		"0",
		3,0,TabRnd, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"The number of rounds to play.  Unlimited if 0.\n"
		)
	);

	prefsArray[i++] = endOfRoundReset = ServerOptionFactory(this,
		"reset","endOfRoundReset",							LABEL("World Reset?"),
		0, valBool,
		"yes",
		5,0,TabRnd, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"Does the world reset when the end of round is reached?\n"
					"When true all mines, missiles, shots and explosions will be\n"
					"removed from the world and all players including the winner(s)\n"
					"will be transported back to their homebases.\n"
					"This option is only effective when limitedLives is turned on.\n"
		)
	);

	prefsArray[i++] = resetOnHuman = ServerOptionFactory(this,
		"resetonhuman","humanreset",						LABEL("ResetOnHuman:"),
		4, valInt,
		"0",
		6,0,TabRnd, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"Normally, new players have to wait until a round is finished\n"
					"before they can start playing. With this option, the first N\n"
					"human players to enter will cause the round to be restarted.\n"
					"In other words, if this option is set to 0, nothing special\n"
					"happens and you have to wait as usual until the round ends (if\n"
					"there are rounds at all, otherwise this option doesn't do\n"
					"anything). If it is set to 1, the round is ended when the first\n"
					"human player enters. This is useful if the robots have already\n"
					"started a round and you don't want to wait for them to finish.\n"
					"If it is set to 2, this also happens for the second human player.\n"
					"This is useful when you got bored waiting for another player to\n"
					"show up and have started playing against the robots. When someone\n"
					"finally joins you, they won't have to wait for you to finish the\n"
					"round before they can play too. For higher numbers it works the\n"
					"same. So this option gives the last human player for whom the\n"
					"round is restarted. Anyone who enters after that does have to\n"
					"wait until the round is over.\n"
		)
	);

	prefsArray[i++] = allowAlliances = ServerOptionFactory(this,
		"allowAlliances", "alliances",						LABEL("Allow Alliances?"),
		0, valBool,
		"yes",
		7,0,TabRnd, OPTIONALL,
		TUNERDEF(TunerAllowAlliances),
		HELPTEXT(	"Are alliances between players allowed?\n"
					"Alliances are like teams, except they can be formed and dissolved\n"
					"at any time. Notably, teamImmunity and teamShareScore work for\n"
					"alliances too. To manage alliances, use the '/ally' talk command:\n"
					"'/ally invite <player name>' to invite another player to join you.\n"
					"'/ally cancel' to cancel such an invitation.\n"
					"'/ally refuse <player name>' to decline an invitation from a player.\n"
					"'/ally refuse' to decline all the invitations you received.\n"
					"'/ally accept <player name>' to join the other player.\n"
					"'/ally accept' to accept all the invitations you received.\n"
					"'/ally leave' to leave the alliance you are currently in.\n"
					"'/ally list' lists the members of your current alliance.\n"
					"If members from different alliances join, all their allies do so.\n"
		)
	);

	prefsArray[i++] = announceAlliances = ServerOptionFactory(this,
		"announceAlliances", "",							LABEL("Announce Alliances?"),
		0, valBool,
		"no",
		8,0,TabRnd, OPTIONALL,
		TUNERDEF(TunerAnnounceAlliances),
		HELPTEXT(	"Are changes in alliances made public?\n"
			"If this option is on, changes in alliances are sent to all players\n"
			"and all alliances are shown in the score list. Invitations for\n"
			"alliances are never sent to anyone but the invited players.\n"
		)
	);

	prefsArray[i++] = limitedLives = ServerOptionFactory(this,
		"limitedlives", "",									LABEL("Limited Lives?"),
		0, valBool,
		"no",
		10,0,TabRnd, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"Should players have limited lives?\n"
			"See also worldLives.\n"
		)
	);

	prefsArray[i++] = worldLives = ServerOptionFactory(this,
		"worldlives"   ,"lives",							LABEL("Lives"),
		3, valInt,
		"0",
		11,0,TabRnd, OPTIONALL,
		TUNERDEF(TunerWorldLives),
		HELPTEXT(	"Number of lives each player has (no sense without limitedLives).\n"
		)
	);

	prefsArray[i++] = timing = ServerOptionFactory(this,
		"timing"	 ,"race",								LABEL("Race-Timing?"),
		0, valBool,
		"no",
		0,1,TabRnd, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"Is the map a race mode map?\n"
		)
	);

	prefsArray[i++] = checkpointRadius = ServerOptionFactory(this,
		"checkpointradius","",								LABEL("Checkpoint Rad:"),
		19, valReal,
		"6.0",
		1,1,TabRnd, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"How close you have to be to a checkpoint to register - in blocks.\n"
		)
	);

	prefsArray[i++] = raceLaps = ServerOptionFactory(this,
		"racelaps","",										LABEL("Race Laps:"),
		6, valInt,
		"3",
		2,1,TabRnd, OPTIONALL,
		TUNERDEF(TunerRaceLaps),
		HELPTEXT(	"How many laps a race is run over.\n"
		)
	);

	prefsArray[i++] = ballRace = ServerOptionFactory(this,
		"ballRace", "",										LABEL("Ball Race?"),
		0, valBool,
		"no",
		3,1,TabRnd, OPTIONALL,
		TUNERDEF(TunerDummy),
		HELPTEXT(	"Is timing done for balls (on) or players (off)?\n"
					"Only used if timing is on.\n"
		)
	);

	prefsArray[i++] = ballRaceConnected = ServerOptionFactory(this,
		"ballraceConnected", "",							LABEL("BallRaceConnected?"),
		0, valBool,
		"no",
		4,1,TabRnd, OPTIONALL,
		TUNERDEF(TunerDummy),
		HELPTEXT(	"Should a player be connected to a ball to pass a checkpoint?\n"
					"Only used if timing and ballrace are both on.\n"
		)
	);

	prefsArray[i++] = tagGame = ServerOptionFactory(this,
		"taggame","",								LABEL("Tag?"),
		0, valBool,
		"no",
		6,1,TabRnd, OPTIONALL,
		TUNERDEF(TunerDummy),
		HELPTEXT(	"Are we going to play a game of tag?\n"
					"One player is 'it' (is worth more points when killed)\n"
					"Player who is it scores double points for kills.\n"
					"The player who kills 'it' then becomes 'it'.\n"
		)
	);

	prefsArray[i++] = tagKillItMult = ServerOptionFactory(this,
		"tagkillitmult","",									LABEL("Kill 'it' multiplier:"),
		19, valReal,
		"10",
		7,1,TabRnd, OPTIONALL,
		TUNERDEF(TunerDummy),
		HELPTEXT(	"Score multiplier for killing 'it'.\n"
		)
	);

	prefsArray[i++] = tagItKillMult = ServerOptionFactory(this,
		"tagitkillmult","",									LABEL("'It' score multiplier:"),
		19, valReal,
		"2",
		8,1,TabRnd, OPTIONALL,
		TUNERDEF(TunerDummy),
		HELPTEXT(	"Score multiplier when 'it' kills another player.\n"
		)
	);

	prefsArray[i++] = useWreckage = ServerOptionFactory(this,
		"usewreckage","",									LABEL("Use Wreckage?"),
		6, valBool,
		"yes",
		0,2,TabRnd, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"Do destroyed ships leave wreckage?\n"
		)
	);

	prefsArray[i++] = wreckageCollisionMayKill = ServerOptionFactory(this,
		"wreckagecollisionmaykill","wreckageunshieldedcollisionkills", LABEL("Wreck Collsn. Kills?"),
		0, valBool,
		"no",
		1,2,TabRnd, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"Can ships be destroyed when hit by wreckage?\n"
		)
	);

	prefsArray[i++] = lockOtherTeam = ServerOptionFactory(this,
		"lockotherteam","",									LABEL("Lock Other Team?"),
		0, valBool,
		"yes",
		2,2,TabRnd, OPTIONALL,
		TUNERDEF(TunerDummy),
		HELPTEXT(	"Can you lock on players from other teams when you're dead.\n"
		)
	);

	prefsArray[i++] = showLocks = ServerOptionFactory(this,
		"showLocks","",								LABEL("Show Locks:"),
		0, valBool,
		"yes",
		3,2,TabRnd, OPTIONALL,
		TUNERDEF(TunerShowLocks),
		HELPTEXT(	"Show who a player is locked on by displaying their name\n"
					"in parentheses in the score table, like a robot who has\n"
					"declared war.\n"
		)
	);

	prefsArray[i++] = allowViewing = ServerOptionFactory(this,
		"allowviewing","",									LABEL("Allow Viewing?"),
		0, valBool,
		"no",
		4,2,TabRnd, OPTIONALL,
		TUNERDEF(TunerAllowViewing),
		HELPTEXT(	"Are players allowed to watch any other player while paused, waiting or dead?\n"
		)
	);

	prefsArray[i++] = anonymousViewing = ServerOptionFactory(this,
		"anonymousViewing","",								LABEL("Anonymous Viewing?"),
		0, valBool,
		"no",
		6,2,TabRnd, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"Are other players notified when someone is viewing another player?\n"
		)
	);

	prefsArray[i++] = maxPauseTime = ServerOptionFactory(this,
		"maxPauseTime","",									LABEL("Maximum Pause Time"),
		6, valSec,
		"3600",
		7,2,TabRnd, OPTIONALL,
		TUNERDEF(TunerDummy),
		HELPTEXT(	"The maximum time a player can stay paused for, in seconds.\n"
					"After being paused this long, the player will be kicked off.\n"
					"Setting this option to 0 disables the feature.\n"
		)
	);

	prefsArray[i++] = pauseScoreReduce = ServerOptionFactory(this,
		"pauseScoreReduce","",								LABEL("Pause Score Reduce:"),
		19, valReal,
		"0.0",
		8,2,TabRnd, OPTIONALL,
		TUNERDEF(TunerPauseScoreReduce),
		HELPTEXT(	"How much to reduce a paused player's score each frame.\n"
		)
	);

	prefsArray[i++] = scoreTableIPVer = ServerOptionFactory(this,
		"scoreTableIP","",									LABEL("Score Table IP:"),
		0, valBool,
		"0.0",
		10,2,TabRnd, OPTIONALL,
		TUNERDEF(TunerScoreTable),
		HELPTEXT(	"Allow the client's score table to show the IP of all players.\n"
		)
	);

	prefsArray[i++] = scoreTableKills = ServerOptionFactory(this,
		"scoreTableKills","",								LABEL("Score Table Kills:"),
		0, valBool,
		"0.0",
		11,2,TabRnd, OPTIONALL,
		TUNERDEF(TunerScoreTable),
		HELPTEXT(	"Allow the client's score table to show the Kill/Death\n"
					"ratio of all players.\n"
		)
	);

	prefsArray[i++] = scoreTableRank = ServerOptionFactory(this,
		"scoreTableRank","",								LABEL("Score Table Rank:"),
		0, valBool,
		"0.0",
		12,2,TabRnd, OPTIONALL,
		TUNERDEF(TunerScoreTable),
		HELPTEXT(	"Allow the client's score table to show the Rank of all players.\n"
					"This is only valid if the ScoreServer is connected to the server.\n"
		)
	);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// Items tab
	prefsArray[i++] = initialFuel = ServerOptionFactory(this,
		"initialfuel","",									LABEL("Initial Fuel:"),
		19, valReal,
		"1000",
		0,0,TabItem, OPTIONALL,
		TUNERDEF(TunerInitialItems),
		HELPTEXT(	"How much fuel players start with, or the minimum after being killed.\n"
		)
	);

	prefsArray[i++] = initialTanks = ServerOptionFactory(this,
		"initialtanks","",									LABEL("Initial Tanks:"),
		19, valInt,
		"0",
		1,0,TabItem, OPTIONALL,
		TUNERDEF(TunerInitialItems),
		HELPTEXT(	"How many tanks players start with.\n"
		)
	);

	prefsArray[i++] = initialECMs = ServerOptionFactory(this,
		"initialecms","",									LABEL("Initial ECMs:"),
		19, valInt,
		"0",
		2,0,TabItem, OPTIONALL,
		TUNERDEF(TunerInitialItems),
		HELPTEXT(	"How many ECMs players start with.\n"
		)
	);

	prefsArray[i++] = initialMines = ServerOptionFactory(this,
		"initialmines","",									LABEL("Initial Mines:"),
		19, valInt,
		"0",
		3,0,TabItem, OPTIONALL,
		TUNERDEF(TunerInitialItems),
		HELPTEXT(	"How many mines players start with.\n"
		)
	);

	prefsArray[i++] = initialMissiles = ServerOptionFactory(this,
		"initialmissiles","",								LABEL("Initial Missiles:"),
		19, valInt,
		"0",
		4,0,TabItem, OPTIONALL,
		TUNERDEF(TunerInitialItems),
		HELPTEXT(	"How many missiles players start with.\n"
		)
	);

	prefsArray[i++] = initialCloaks = ServerOptionFactory(this,
		"initialcloaks","",									LABEL("Initial Cloaks:"),
		19, valInt,
		"0",
		5,0,TabItem, OPTIONALL,
		TUNERDEF(TunerInitialItems),
		HELPTEXT(	"How many cloaks players start with.\n"
		)
	);

	prefsArray[i++] = initialSensors = ServerOptionFactory(this,
		"initialsensors","",								LABEL("Initial Sensors:"),
		19, valInt,
		"0",
		6,0,TabItem, OPTIONALL,
		TUNERDEF(TunerInitialItems),
		HELPTEXT(	"How many sensors players start with.\n"
		)
	);

	prefsArray[i++] = initialWideangles = ServerOptionFactory(this,
		"initialwideangles","",								LABEL("Initial Wideangles:"),
		19, valInt,
		"0",
		7,0,TabItem, OPTIONALL,
		TUNERDEF(TunerInitialItems),
		HELPTEXT(	"How many wideangles players start with.\n"
		)
	);

	prefsArray[i++] = initialRearshots = ServerOptionFactory(this,
		"initialrearshots","",								LABEL("Initial Rearshots:"),
		19, valInt,
		"0",
		8,0,TabItem, OPTIONALL,
		TUNERDEF(TunerInitialItems),
		HELPTEXT(	"How many rearshots players start with.\n"
		)
	);

	prefsArray[i++] = initialAfterburners = ServerOptionFactory(this,
		"initialafterburners","",							LABEL("Initial Afterburners:"),
		19, valInt,
		"0",
		9,0,TabItem, OPTIONALL,
		TUNERDEF(TunerInitialItems),
		HELPTEXT(	"How many afterburners players start with.\n"
		)
	);

	prefsArray[i++] = initialTransporters = ServerOptionFactory(this,
		"initialtransporters","",							LABEL("Initial Transporters:"),
		19, valInt,
		"0",
		10,0,TabItem, OPTIONALL,
		TUNERDEF(TunerInitialItems),
		HELPTEXT(	"How many transporters players start with.\n"
		)
	);

	prefsArray[i++] = initialDeflectors = ServerOptionFactory(this,
		"initialdeflectors","",								LABEL("Initial Deflectors:"),
		19, valInt,
		"0",
		11,0,TabItem, OPTIONALL,
		TUNERDEF(TunerInitialItems),
		HELPTEXT(	"How many deflectors players start with.\n"
		)
	);

	prefsArray[i++] = initialPhasings = ServerOptionFactory(this,
		"initialphasings","",								LABEL("Initial Phasings:"),
		19, valInt,
		"0",
		12,0,TabItem, OPTIONALL,
		TUNERDEF(TunerInitialItems),
		HELPTEXT(	"How many phasing devices players start with.\n"
		)
	);

	prefsArray[i++] = initialHyperJumps = ServerOptionFactory(this,
		"initialhyperjumps","",								LABEL("Init. HyperJumps:"),
		19, valInt,
		"0",
		13,0,TabItem, OPTIONALL,
		TUNERDEF(TunerInitialItems),
		HELPTEXT(	"How many hyperjumps players start with.\n"
		)
	);

	prefsArray[i++] = initialEmergencyThrusts = ServerOptionFactory(this,
		"initialemergencythrusts","",						LABEL("Init. Emrg.Thrsts:"),
		19, valInt,
		"0",
		14,0,TabItem, OPTIONALL,
		TUNERDEF(TunerInitialItems),
		HELPTEXT(	"How many emergency thrusts players start with.\n"
		)
	);

	prefsArray[i++] = initialLasers = ServerOptionFactory(this,
		"initiallasers","",									LABEL("Initial Lasers:"),
		19, valInt,
		"0",
		15,0,TabItem, OPTIONALL,
		TUNERDEF(TunerInitialItems),
		HELPTEXT(	"How many lasers players start with.\n"
		)
	);

	prefsArray[i++] = initialTractorBeams = ServerOptionFactory(this,
		"initialtractorbeams","",							LABEL("Initial TractorBms:"),
		19, valInt,
		"0",
		16,0,TabItem, OPTIONALL,
		TUNERDEF(TunerInitialItems),
		HELPTEXT(	"How many tractor/pressor beams players start with.\n"
		)
	);

	prefsArray[i++] = initialAutopilots = ServerOptionFactory(this,
		"initialautopilots","",								LABEL("Initial Autopilot:"),
		19, valInt,
		"0",
		17,0,TabItem, OPTIONALL,
		TUNERDEF(TunerInitialItems),
		HELPTEXT(	"How many autopilots players start with.\n"
		)
	);

	prefsArray[i++] = initialEmergencyShields = ServerOptionFactory(this,
		"initialemergencyshields","",						LABEL("Init. Emrg.Shields:"),
		19, valInt,
		"0",
		18,0,TabItem, OPTIONALL,
		TUNERDEF(TunerInitialItems),
		HELPTEXT(	"How many emergency shields players start with.\n"
		)
	);

	prefsArray[i++] = initialMirrors = ServerOptionFactory(this,
		"initialmirrors","",								LABEL("Initial Mirrors:"),
		19, valInt,
		"0",
		19,0,TabItem, OPTIONALL,
		TUNERDEF(TunerInitialItems),
		HELPTEXT(	"How many mirrors players start with.\n"
		)
	);

	prefsArray[i++] = initialArmor = ServerOptionFactory(this,
		"initialarmor","initialarmors",						LABEL("Initial Armor:"),
		19, valInt,
		"0",
		20,0,TabItem, OPTIONALL,
		TUNERDEF(TunerInitialItems),
		HELPTEXT(	"How much armor players start with.\n"
		)
	);

	prefsArray[i++] = maxFuel = ServerOptionFactory(this,
		"maxfuel","",										LABEL("Max Fuel:"),
		19, valInt,
		"10000",
		0,1,TabItem, OPTIONALL,
		TUNERDEF(TunerInitialItems),
		HELPTEXT(	"Upper limit on the amount of fuel per player.\n"
		)
	);

	prefsArray[i++] = maxTanks = ServerOptionFactory(this,
		"maxtanks","",										LABEL("Max Tanks:"),
		19, valInt,
		"8",
		1,1,TabItem, OPTIONALL,
		TUNERDEF(TunerInitialItems),
		HELPTEXT(	"Upper limit on the number of tanks per player.\n"
		)
	);

	prefsArray[i++] = maxECMs = ServerOptionFactory(this,
		"maxecms","",										LABEL("Max ECMS:"),
		19, valInt,
		"10",
		2,1,TabItem, OPTIONALL,
		TUNERDEF(TunerInitialItems),
		HELPTEXT(	"Upper limit on the number of ECMs per player.\n"
		)
	);

	prefsArray[i++] = maxMines= ServerOptionFactory(this,
		"maxmines","",										LABEL("Max Mines:"),
		19, valInt,
		"10",
		3,1,TabItem, OPTIONALL,
		TUNERDEF(TunerInitialItems),
		HELPTEXT(	"Upper limit on the number of mines per player.\n"
		)
	);

	prefsArray[i++] = maxMissiles = ServerOptionFactory(this,
		"maxmissiles","",									LABEL("Max Missiles:"),
		19, valInt,
		"10",
		4,1,TabItem, OPTIONALL,
		TUNERDEF(TunerInitialItems),
		HELPTEXT(	"Upper limit on the number of missiles per player.\n"
		)
	);

	prefsArray[i++] = maxCloaks = ServerOptionFactory(this,
		"maxcloaks","",										LABEL("Max Cloaks:"),
		19, valInt,
		"10",
		5,1,TabItem, OPTIONALL,
		TUNERDEF(TunerInitialItems),
		HELPTEXT(	"Upper limit on the number of cloaks per player.\n"
		)
	);

	prefsArray[i++] = maxSensors = ServerOptionFactory(this,
		"maxsensors","",									LABEL("Max Sensors:"),
		19, valInt,
		"10",
		6,1,TabItem, OPTIONALL,
		TUNERDEF(TunerInitialItems),
		HELPTEXT(	"Upper limit on the number of sensors per player.\n"
		)
	);

	prefsArray[i++] = maxWideangles = ServerOptionFactory(this,
		"maxwideangles","",									LABEL("Max Widesangles:"),
		19, valInt,
		"10",
		7,1,TabItem, OPTIONALL,
		TUNERDEF(TunerInitialItems),
		HELPTEXT(	"Upper limit on the number of wides per player.\n"
		)
	);

	prefsArray[i++] = maxRearshots = ServerOptionFactory(this,
		"maxrearshots","",									LABEL("Max Rearshots:"),
		19, valInt,
		"10",
		8,1,TabItem, OPTIONALL,
		TUNERDEF(TunerInitialItems),
		HELPTEXT(	"Upper limit on the number of rearshots per player.\n"
		)
	);

	prefsArray[i++] = maxAfterburners = ServerOptionFactory(this,
		"maxafterburners","",								LABEL("Max Afterburners:"),
		19, valInt,
		"10",
		9,1,TabItem, OPTIONALL,
		TUNERDEF(TunerInitialItems),
		HELPTEXT(	"Upper limit on the number of afterburners per player.\n"
		)
	);

	prefsArray[i++] = maxTransporters = ServerOptionFactory(this,
		"maxtransporters","",								LABEL("Max Transporters:"),
		19, valInt,
		"10",
		10,1,TabItem, OPTIONALL,
		TUNERDEF(TunerInitialItems),
		HELPTEXT(	"Upper limit on the number of transporters per player.\n"
		)
	);

	prefsArray[i++] = maxDeflectors = ServerOptionFactory(this,
		"maxdeflectors","",									LABEL("Max Deflectors:"),
		19, valInt,
		"10",
		11,1,TabItem, OPTIONALL,
		TUNERDEF(TunerInitialItems),
		HELPTEXT(	"Upper limit on the number of deflectors per player.\n"
		)
	);

	prefsArray[i++] = maxPhasings = ServerOptionFactory(this,
		"maxphasings","",									LABEL("Max Phasings:"),
		19, valInt,
		"10",
		12,1,TabItem, OPTIONALL,
		TUNERDEF(TunerInitialItems),
		HELPTEXT(	"Upper limit on the number of phasing devices per players.\n"
		)
	);

	prefsArray[i++] = maxHyperJumps = ServerOptionFactory(this,
		"maxhyperjumps","",									LABEL("Max HyperJumps:"),
		19, valInt,
		"10",
		13,1,TabItem, OPTIONALL,
		TUNERDEF(TunerInitialItems),
		HELPTEXT(	"Upper limit on the number of hyperjumps per player.\n"
		)
	);

	prefsArray[i++] = maxEmergencyThrusts = ServerOptionFactory(this,
		"maxemergencythrusts","",							LABEL("Max Emrg.Thrsts:"),
		19, valInt,
		"10",
		14,1,TabItem, OPTIONALL,
		TUNERDEF(TunerInitialItems),
		HELPTEXT(	"Upper limit on the number of emergency thrusts per player.\n"
		)
	);

	prefsArray[i++] = maxLasers = ServerOptionFactory(this,
		"maxlasers","",										LABEL("Max Lasers:"),
		19, valInt,
		"5",
		15,1,TabItem, OPTIONALL,
		TUNERDEF(TunerInitialItems),
		HELPTEXT(	"Upper limit on the number of lasers per player.\n"
		)
	);

	prefsArray[i++] = maxTractorBeams = ServerOptionFactory(this,
		"maxtractorbeams","",								LABEL("Max TractorBms:"),
		19, valInt,
		"4",
		16,1,TabItem, OPTIONALL,
		TUNERDEF(TunerInitialItems),
		HELPTEXT(	"Upper limit on the number of tractorbeams per player.\n"
		)
	);

	prefsArray[i++] = maxAutopilots = ServerOptionFactory(this,
		"maxautopilots","",									LABEL("Max AutoPilots:"),
		19, valInt,
		"10",
		17,1,TabItem, OPTIONALL,
		TUNERDEF(TunerInitialItems),
		HELPTEXT(	"Upper limit on the number of autopilots per player.\n"
		)
	);

	prefsArray[i++] = maxEmergencyShields = ServerOptionFactory(this,
		"maxemergencyshields","",							LABEL("Max Emrg.Shields:"),
		19, valInt,
		"10",
		18,1,TabItem, OPTIONALL,
		TUNERDEF(TunerInitialItems),
		HELPTEXT(	"Upper limit on the number of emergency shields per player.\n"
		)
	);

	prefsArray[i++] = maxMirrors = ServerOptionFactory(this,
		"maxmirrors","",									LABEL("Max Mirrors:"),
		19, valInt,
		"10",
		19,1,TabItem, OPTIONALL,
		TUNERDEF(TunerInitialItems),
		HELPTEXT(	"Upper limit on the number of mirrors per player.\n"
		)
	);

	prefsArray[i++] = maxArmor = ServerOptionFactory(this,
		"maxarmor","maxarmors",								LABEL("Max Armor:"),
		19, valInt,
		"10",
		20,1,TabItem, OPTIONALL,
		TUNERDEF(TunerInitialItems),
		HELPTEXT(	"Upper limit on the amount of armor per player.\n"
		)
	);

	prefsArray[i++] = maxOffensiveItems = ServerOptionFactory(this,
		"maxoffensiveitems","",								LABEL("MaxOffenseItems:"),
		6, valInt,
		"100",
		0,2,TabItem, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"How many defensive items a player can carry.\n"
		)
	);

	prefsArray[i++] = maxDefensiveItems = ServerOptionFactory(this,
		"maxdefensiveitems","",								LABEL("MaxDefenseItems:"),
		6, valInt,
		"100",
		1,2,TabItem, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"How many offensive items a player can carry.\n"
		)
	);

	prefsArray[i++] = maxItemDensity = ServerOptionFactory(this,
		"maxitemdensity","",								LABEL("MaxItemDensity:"),
		19, valReal,
		"0.00012",
		3,2,TabItem, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"Maximum density [0.0-1.0] for items (max items per block).\n"
		)
	);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// Probabilities tab
	prefsArray[i++] = itemEnergyPackProb = ServerOptionFactory(this,
		"itemenergypackprob","",							LABEL("Fuel Pack Prob:"),
		19, valReal,
		"1e-9",
		0,0,TabProb, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"Probability for an energy pack to appear.\n"
		)
	);

	prefsArray[i++] = itemTankProb = ServerOptionFactory(this,
		"itemtankprob","",									LABEL("Tank Prob:"),
		19, valReal,
		"1e-9",
		1,0,TabProb, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"Probability for an extra tank to appear.\n"
		)
	);

	prefsArray[i++] = itemECMProb = ServerOptionFactory(this,
		"itemecmprob","",									LABEL("ECM Prob:"),
		19, valReal,
		"1e-9",
		2,0,TabProb, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"Probability for an ECM item to appear.\n"
		)
	);

	prefsArray[i++] = itemMineProb = ServerOptionFactory(this,
		"itemmineprob","",									LABEL("Mine Prob:"),
		19, valReal,
		"1e-9",
		3,0,TabProb, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"Probability for a mine item to appear.\n"
		)
	);

	prefsArray[i++] = itemMissileProb = ServerOptionFactory(this,
		"itemmissileprob","",								LABEL("Missile Prob:"),
		19, valReal,
		"1e-9",
		4,0,TabProb, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"Probability for a missile item to appear.\n"
		)
	);

	prefsArray[i++] = itemCloakProb = ServerOptionFactory(this,
		"itemcloakprob","",									LABEL("Cloak Prob:"),
		19, valReal,
		"1e-9",
		5,0,TabProb, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"Probability for a cloak item to appear.\n"
		)
	);

	prefsArray[i++] = itemSensorProb = ServerOptionFactory(this,
		"itemsensorprob","",								LABEL("Sensor Prob:"),
		19, valReal,
		"1e-9",
		6,0,TabProb, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"Probability for a sensor item to appear.\n"
		)
	);

	prefsArray[i++] = itemWideangleProb = ServerOptionFactory(this,
		"itemwideangleprob","",								LABEL("Wideangle Prob:"),
		19, valReal,
		"1e-9",
		7,0,TabProb, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"Probability for a wideangle item to appear.\n"
		)
	);

	prefsArray[i++] = itemRearshotProb = ServerOptionFactory(this,
		"itemrearshotprob","",								LABEL("Rearshot Prob:"),
		19, valReal,
		"1e-9",
		8,0,TabProb, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"Probability for a rearshot item to appear.\n"
		)
	);

	prefsArray[i++] = itemAfterburnerProb = ServerOptionFactory(this,
		"itemafterburnerprob","",							LABEL("Afterburner Prob:"),
		19, valReal,
		"1e-9",
		9,0,TabProb, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"Probability for an afterburner item to appear.\n"
		)
	);

	prefsArray[i++] = itemTransporterProb = ServerOptionFactory(this,
		"itemtransporterprob","",							LABEL("Transporter Prob:"),
		19, valReal,
		"1e-9",
		0,1,TabProb, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"Probability for a transporter item to appear.\n"
		)
	);

	prefsArray[i++] = itemLaserProb = ServerOptionFactory(this,
		"itemlaserprob","",									LABEL("Laser Prob:"),
		19, valReal,
		"1e-9",
		1,1,TabProb, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"Probability for a Laser item to appear.\n"
		)
	);

	prefsArray[i++] = itemEmergencyThrustProb = ServerOptionFactory(this,
		"itememergencythrustprob","",						LABEL("EmerThrst Prob:"),
		19, valReal,
		"1e-9",
		2,1,TabProb, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"Probability for an Emergency Thrust item to appear.\n"
		)
	);

	prefsArray[i++] = itemTractorBeamProb = ServerOptionFactory(this,
		"itemtractorbeamprob","",							LABEL("Tractor Prob:"),
		19, valReal,
		"1e-9",
		3,1,TabProb, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"Probability for a Tractor Beam item to appear.\n"
		)
	);

	prefsArray[i++] = itemAutopilotProb = ServerOptionFactory(this,
		"itemautopilotprob","",								LABEL("Autopilot Prob:"),
		19, valReal,
		"1e-9",
		4,1,TabProb, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"Probability for an Autopilot item to appear.\n"
		)
	);

	prefsArray[i++] = itemEmergencyShieldProb = ServerOptionFactory(this,
		"itememergencyshieldprob","",						LABEL("EmerShield Prob:"),
		19, valReal,
		"1e-9",
		5,1,TabProb, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"Probability for an Emergency Shield item to appear.\n"
		)
	);

	prefsArray[i++] = itemDeflectorProb = ServerOptionFactory(this,
		"itemdeflectorprob","",								LABEL("Deflector Prob:"),
		19, valReal,
		"1e-9",
		6,1,TabProb, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"Probability for a deflector item to appear.\n"
		)
	);

	prefsArray[i++] = itemHyperJumpProb = ServerOptionFactory(this,
		"itemhyperjumpprob","",								LABEL("HyperJump Prob:"),
		19, valReal,
		"1e-9",
		7,1,TabProb, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"Probability for a hyperjump item to appear.\n"
		)
	);

	prefsArray[i++] = itemPhasingProb = ServerOptionFactory(this,
		"itemphasingprob","",								LABEL("Phasing Prob:"),
		19, valReal,
		"1e-9",
		8,1,TabProb, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"Probability for a phasing item to appear.\n"
		)
	);

	prefsArray[i++] = itemMirrorProb = ServerOptionFactory(this,
		"itemmirrorprob","",								LABEL("Mirror Prob:"),
		19, valReal,
		"1e-9",
		9,1,TabProb, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"Probability for a mirror item to appear.\n"
		)
	);

	prefsArray[i++] = itemArmorProb = ServerOptionFactory(this,
		"itemarmorprob","",									LABEL("Armor Prob:"),
		19, valReal,
		"1e-9",
		0,2,TabProb, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"Probability for an armor item to appear.\n"
		)
	);

	prefsArray[i++] = movingItemProb = ServerOptionFactory(this,
		"movingitemprob","",								LABEL("Moving Item Prob:"),
		19, valReal,
		"0.2",
		1,2,TabProb, OPTIONALL,
		TUNERDEF(TunerMiscItemLimits),
		HELPTEXT(	"Probability for an item to appear as moving.\n"
		)
	);

	prefsArray[i++] = dropItemOnKillProb = ServerOptionFactory(this,
		"dropitemonkillprob","",							LABEL("DropItemKillProb:"),
		19, valReal,
		"0.5",
		2,2,TabProb, OPTIONALL,
		TUNERDEF(TunerMiscItemLimits),
		HELPTEXT(	"Probability for dropping an item (each item) when you are killed.\n"
		)
	);

	prefsArray[i++] = detonateItemOnKillProb = ServerOptionFactory(this,
		"detonateitemonkillprob","",						LABEL("DestItemKillProb:"),
		19, valReal,
		"0.5",
		3,2,TabProb, OPTIONALL,
		TUNERDEF(TunerMiscItemLimits),
		HELPTEXT(	"Probability for undropped items to detonate when you are killed.\n"
		)
	);

	prefsArray[i++] = destroyItemInCollisionProb = ServerOptionFactory(this,
		"destroyitemincollisionprob","",					LABEL("DestItemCollProb:"),
		19, valReal,
		"0.0",
		4,2,TabProb, OPTIONALL,
		TUNERDEF(TunerMiscItemLimits),
		HELPTEXT(	"Probability for items (some items) to be destroyed in a collision.\n"
		)
	);

	prefsArray[i++] = asteroidItemProb = ServerOptionFactory(this,
		"asteroidItemProb","",								LABEL("Asteroid Item Prob:"),
		19, valReal,
		"0.0",
		5,2,TabProb, OPTIONALL,
		TUNERDEF(TunerMiscItemLimits),
		HELPTEXT(	"Probability for an asteroid to release items when it breaks.\n"
		)
	);

	prefsArray[i++] = asteroidMaxItems = ServerOptionFactory(this,
		"asteroidMaxItems", "",								LABEL("Asteroid Max Items:"),
		6, valInt,
		"0",
		6,2,TabProb, OPTIONALL,
		TUNERDEF(TunerMiscItemLimits),
		HELPTEXT(	"The maximum number of items a broken asteroid can release.\n"
		)
	);

	prefsArray[i++] = itemConcentratorRadius = ServerOptionFactory(this,
		"itemconcentratorradius","itemconcentratorrange",	LABEL("Item Con Radius:"),
		19, valReal,
		"10",
		7,2,TabProb, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"The maximum distance from an item concentator for items to\n"
					"appear in.  Sensible values are in the range 1 to 20.\n"
					"If no item concentators are defined in a map then items can\n"
					"popup anywhere.  If any are any then items popup in the\n"
					"vicinity of an item concentrator with probability\n"
					"itemConcentratorProb and anywhere the remainder of the\n"
					"time.  An item concentrator is drawn on screen as three\n"
					"rotating triangles.  The map symbol is the percentage symbol '%'.\n"
		)
	);

	prefsArray[i++] = itemConcentratorProb = ServerOptionFactory(this,
		"itemconcentratorprob","",							LABEL("Item Conc. Prob:"),
		19, valReal,
		"1.0",
		8,2,TabProb, OPTIONALL,
		TUNERDEF(TunerMiscItemLimits),
		HELPTEXT(	"The probability, if any item concentrators are present,\n"
					"that they will be used.  This proportion of items will\n"
					"be placed near item concentrators, within\n"
					"itemConcentratorRadius.\n"
		)
	);

	prefsArray[i++] = itemProbMult = ServerOptionFactory(this,
		"itemprobmult","itemprobfact",						LABEL("Item Prob Mult:"),
		19, valReal,
		"1.0",
		9,2,TabProb, OPTIONALL,
		TUNERDEF(TunerMiscItemLimits),
		HELPTEXT(	"Item Probability Multiplication Factor scales all item\n"
					"probabilities.\n"
		)
	);

	prefsArray[i++] = cannonItemProbMult = ServerOptionFactory(this,
		"cannonitemprobmult","",							LABEL("Can.ItemProbMlt:"),
		19, valReal,
		"1.0",
		10,2,TabProb, OPTIONALL,
		TUNERDEF(TunerMiscItemLimits),
		HELPTEXT(	"Average number of items a cannon gets per minute.\n"
		)
	);

	prefsArray[i++] = randomItemProb = ServerOptionFactory(this,
		"randomitemprob","",								LABEL("RandomItemProb:"),
		19, valReal,
		"0.0",
		11,2,TabProb, OPTIONALL,
		TUNERDEF(TunerMiscItemLimits),
		HELPTEXT(	"Probability for an item to appear random.\n"
					"Random items change their appearance every frame, so players\n"
					"cannot tell what item they have until they get it.\n"
		)
	);

	prefsArray[i++] = asteroidProb = ServerOptionFactory(this,
		"asteroidprob","",									LABEL("AsteroidProb:"),
		19, valReal,
		"0.001",
		12,2,TabProb, OPTIONALL,
		TUNERDEF(TunerAsteroidProb),
		HELPTEXT(	"The probability that an asteroid may appear\n"
		)
	);

	prefsArray[i++] = asteroidConcentratorRadius = ServerOptionFactory(this,
		"asteroidConcentratorRadius","",					LABEL("AsteroidConcentratorRadius"),
		6, valInt,
		"10",
		13,2,TabProb, OPTIONALL,
		TUNERDEF(TunerAsteroidProb),
		HELPTEXT(	"The maximum distance from an asteroid concentrator for\n"
					"asteroids to appear in.  Sensible values are in the \n"
					"range 1 to 20.  If no asteroid concentrators are defined\n"
					"in a map then asteroids can popup anywhere.  If any are\n"
					"then asteroids popup in the vicinity of an asteroid \n"
					"concentrator with probability asteroidConcentratorProb\n"
					"and anywhere the remainder of the time.  An asteroid\n"
					"concentrator is drawn on the screen as three rotating\n"
					"squares.  The map symbol is the ampersand '&'.\n"
		)
	);

	prefsArray[i++] = asteroidConcentratorProb = ServerOptionFactory(this,
		"asteroidConcentratorProb","",						LABEL("AsteroidConcentratorProb:"),
		19, valReal,
		"1.0",
		14,2,TabProb, OPTIONALL,
		TUNERDEF(TunerAsteroidProb),
		HELPTEXT(	"The probability, if any asteroid concentrators are present, that they will\n"
					"be used.  This proportion of asteroids will be placed near asteroid\n"
					"concentrators, within asteroidConcentratorRadius.\n"
		)
	);

///////////////////////////////////////////////////////////////////////////////
// Scoring Tab
	prefsArray[i++] = shotKillScoreMult = ServerOptionFactory(this,
		"shotkillscoremult","",								LABEL("ShotKillScoreMultiplier:"),
		19, valReal,
		"1.0",
		0,1,TabScor, OPTIONALL,
		TUNERDEF(TunerDummy),
		HELPTEXT(	"Multiplication factor to scale score for shot kills.\n"
		)
	);

	prefsArray[i++] = torpedoKillScoreMult = ServerOptionFactory(this,
		"torpedokillscoremult","",							LABEL("TorpedoKillScoreMultiplier:"),
		19, valReal,
		"1.0",
		1,1,TabScor, OPTIONALL,
		TUNERDEF(TunerDummy),
		HELPTEXT(	"Multiplication factor to scale score for torpedo kills.\n"
		)
	);

	prefsArray[i++] = smartKillScoreMult = ServerOptionFactory(this,
		"smartkillscoremult","",							LABEL("SmartKillScoreMultiplier:"),
		19, valReal,
		"1.0",
		2,1,TabScor, OPTIONALL,
		TUNERDEF(TunerDummy),
		HELPTEXT(	"Multiplication factor to scale score for smart missile kills.\n"
		)
	);

	prefsArray[i++] = heatKillScoreMult = ServerOptionFactory(this,
		"heatkillscoremult","",								LABEL("HeatKillScoreMultiplier:"),
		19, valReal,
		"1.0",
		3,1,TabScor, OPTIONALL,
		TUNERDEF(TunerDummy),
		HELPTEXT(	"Multiplication factor to scale score for heatseeker kills.\n"
		)
	);

	prefsArray[i++] = clusterKillScoreMult = ServerOptionFactory(this,
		"clusterkillscoremult","",							LABEL("ClusterKillScoreMultiplier:"),
		19, valReal,
		"1.0",
		4,1,TabScor, OPTIONALL,
		TUNERDEF(TunerDummy),
		HELPTEXT(	"Multiplication factor to scale score for cluster debris kills.\n"
		)
	);

	prefsArray[i++] = laserKillScoreMult = ServerOptionFactory(this,
		"laserkillscoremult","",							LABEL("LaserKillScoreMultiplier:"),
		19, valReal,
		"1.0",
		5,1,TabScor, OPTIONALL,
		TUNERDEF(TunerDummy),
		HELPTEXT(	"Multiplication factor to scale score for laser kills.\n"
		)
	);

	prefsArray[i++] = tankKillScoreMult = ServerOptionFactory(this,
		"tankkillscoremult","",								LABEL("TankKillScoreMultiplier:"),
		19, valReal,
		"0.44",
		6,1,TabScor, OPTIONALL,
		TUNERDEF(TunerDummy),
		HELPTEXT(	"Multiplication factor to scale score for tank kills.\n"
		)
	);

	prefsArray[i++] = runoverKillScoreMult = ServerOptionFactory(this,
		"runoverkillscoremult","",							LABEL("RunoverKillScoreMultiplier:"),
		19, valReal,
		"0.33",
		7,1,TabScor, OPTIONALL,
		TUNERDEF(TunerDummy),
		HELPTEXT(	"Multiplication factor to scale score for player runovers.\n"
		)
	);

	prefsArray[i++] = ballKillScoreMult = ServerOptionFactory(this,
		"ballkillscoremult","",								LABEL("BallKillScoreMultiplier:"),
		19, valReal,
		"1.0",
		 8,1,TabScor, OPTIONALL,
		TUNERDEF(TunerDummy),
		HELPTEXT(	"Multiplication factor to scale score for ball kills.\n"
		)
	);

	prefsArray[i++] = explosionKillScoreMult = ServerOptionFactory(this,
		"explosionkillscoremult","",						LABEL("ExplodeKillScoreMultiplier:"),
		19, valReal,
		"0.33",
		9,1,TabScor, OPTIONALL,
		TUNERDEF(TunerDummy),
		HELPTEXT(	"Multiplication factor to scale score for explosion kills.\n"
		)
	);

	prefsArray[i++] = shoveKillScoreMult = ServerOptionFactory(this,
		"shovekillscoremult","",							LABEL("ShoveKillScoreMultiplier:"),
		19, valReal,
		"0.5",
		10,1,TabScor, OPTIONALL,
		TUNERDEF(TunerDummy),
		HELPTEXT(	"Multiplication factor to scale score for shove kills.\n"
		)
	);

	prefsArray[i++] = crashScoreMult = ServerOptionFactory(this,
		"crashscoremult","",								LABEL("CrashScoreMultiplier:"),
		19, valReal,
		"0.33",
		11,1,TabScor, OPTIONALL,
		TUNERDEF(TunerDummy),
		HELPTEXT(	"Multiplication factor to scale score for player crashes.\n"
		)
	);

	prefsArray[i++] = mineScoreMult = ServerOptionFactory(this,
		"minescoremult","",									LABEL("MineScoreMultiplier:"),
		19, valReal,
		"0.17",
		12,1,TabScor, OPTIONALL,
		TUNERDEF(TunerDummy),
		HELPTEXT(	"Multiplication factor to scale score for mine hits.\n"
		)
	);

	prefsArray[i++] = selfKillScoreMult = ServerOptionFactory(this,
		"selfKillScoreMult","",								LABEL("SelfKillScoreMultiplier:"),
		19, valReal,
		"0.5",
		14,1,TabScor, OPTIONALL,
		TUNERDEF(TunerDummy),
		HELPTEXT(	"Multiplication factor to scale score for killing yourself.\n"
		)
	);

	prefsArray[i++] = selfDestructScoreMult = ServerOptionFactory(this,
		"selfDestructScoreMult","",							LABEL("SelfDestructScoreMult:"),
		19, valReal,
		"0",
		15,1,TabScor, OPTIONALL,
		TUNERDEF(TunerDummy),
		HELPTEXT(	"Multiplication factor to scale score for self-destructing.\n"
		)
	);

	prefsArray[i++] = unownedKillScoreMult = ServerOptionFactory(this,
		"unownedKillScoreMult","",							LABEL("UnownedKillScoreMult:"),
		19, valReal,
		"0.5",
		16,1,TabScor, OPTIONALL,
		TUNERDEF(TunerDummy),
		HELPTEXT(	"Multiplication factor to scale score for being killed by\n"
					"asteroids or other objects without an owner.\n"
		)
	);

	prefsArray[i++] = tankScoreDecrement = ServerOptionFactory(this,
		"tankscoredecrement","",							LABEL("Tank Decrement:"),
		6, valInt,
		"500",
		0,2,TabScor, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"How much lower is the tank's score than the player's?\n"
		)
	);

	prefsArray[i++] = asteroidPoints = ServerOptionFactory(this,
		"asteroidscoring","",								LABEL("Asteroid Points:"),
		19, valReal,
		"1.0",
		5,2,TabScor, OPTIONALL,
		TUNERDEF(TunerDummy),
		HELPTEXT(	"Points awarded for breaking an asteroid\n"
		)
	);

	prefsArray[i++] = cannonPoints = ServerOptionFactory(this,
		"cannonPoints","",									LABEL("Cannon Points:"),
		19, valReal,
		"1.0",
		6,2,TabScor, OPTIONALL,
		TUNERDEF(TunerDummy),
		HELPTEXT(	"Points awarded for killing a cannon.\n"
		)
	);

	prefsArray[i++] = asteroidMaxScore = ServerOptionFactory(this,
		"asteroidMaxScore","",								LABEL("Asteroid Max Score:"),
		19, valReal,
		"100.0",
		7,2,TabScor, OPTIONALL,
		TUNERDEF(TunerDummy),
		HELPTEXT(	"Maximum score to receive points for breaking an asteroid.\n"
		)
	);

	prefsArray[i++] = cannonMaxScore = ServerOptionFactory(this,
		"cannonMaxScore","",								LABEL("Cannon Max Score:"),
		19, valReal,
		"100.0",
		8,2,TabScor, OPTIONALL,
		TUNERDEF(TunerDummy),
		HELPTEXT(	"Maximum score to receive points for killing a cannon.\n"
		)
	);

///////////////////////////////////////////////////////////////////////////////
// System tab
	prefsArray[i++] = mapFileName = ServerOptionFactory(this,
		"mapfilename", "map",								LABEL("MapFileName:"),
		255, valString,
		"\0",
		0,0,TabSys, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"The filename of the map to use.\n"
					"Or \"wild\" if you want a map by The Wild Map Generator.\n"
					"The geometry of a \"wild\" map is given by the -mapWidth\n"
					"and the -mapHeight options.\n"
		)
	);

	prefsArray[i++] = robotFile = ServerOptionFactory(this,
		"robotfile", "",									LABEL("RobotFileName:"),
		255, valString,
		"\0",
		1,0,TabSys, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"The file containing parameters for robot details.\n"
		)
	);

	prefsArray[i++] = idleRun = ServerOptionFactory(this,
		"idlerun", "rawmode",								LABEL("Idle Run?"),
		0, valBool,
		"no",
		2,0,TabSys, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"Do robots keep on playing even if all human players quit?\n"
		)
	);

	prefsArray[i++] = logRobots = ServerOptionFactory(this,
		"logrobots", "",									LABEL("Log Robots?"),
		0, valBool,
		"yes",
		4,0,TabSys, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"Log the comings and goings of robots.\n"
		)
	);

	prefsArray[i++] = contactPort = ServerOptionFactory(this,
		"contactport", "port",								LABEL("Contact Port:"),
		19, valInt,
		"15345",
		5,0,TabSys, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"The server contact port number.\n"
		)
	);

	prefsArray[i++] = serverHost = ServerOptionFactory(this,
		"serverhost", "servername",							LABEL("Server Host:"),
		255, valString,
		"\0",
		6,0,TabSys, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"The server's fully qualified domain name (for multihomed hosts).\n"
		)
	);

	prefsArray[i++] = reportToMetaServer = ServerOptionFactory(this,
		"reporttometaserver", "reportmeta",					LABEL("ReportToMetaServer?"),
		0, valBool,
		"yes",
		7,0,TabSys, OPTIONALL,
		TUNERDEF(TunerReportToMetaServer),
		HELPTEXT(	"Keep the meta server informed about our game?\n"
		)
	);

	prefsArray[i++] = searchDomainForXPilot = ServerOptionFactory(this,
		"searchdomainforxpilot", "",						LABEL("searchDomainForXPilot?"),
		0, valBool,
		"no",
		8,0,TabSys, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"Search the local domain for the existence of xpilot.domain?\n"
		)
	);

	prefsArray[i++] = denyHosts = ServerOptionFactory(this,
		"denyhosts", "",									LABEL("Deny Hosts:"),
		255, valString,
		"\0",
		9,0,TabSys, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"List of network addresses of computers which are denied\n"
					"service.  Each address may optionally be followed by a\n"
					"slash and a network mask.\n"
		)
	);

	prefsArray[i++] = ignore20MaxFPS = ServerOptionFactory(this,
		"ignore20maxfps", "",								LABEL("ignore20MaxFPS?"),
		0, valBool,
		"yes",
		10,0,TabSys, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"Ignore client maxFPS request if it is 20 (the default setting).\n"
		)
	);

	prefsArray[i++] = defaultsFileName = ServerOptionFactory(this,
		"defaultsfilename", "defaults",						LABEL("defaultsFileName:"),
		255, valString,
		"\0",
		11,0,TabSys, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"The filename of the defaults file to read on startup.\n"
		)
	);

	prefsArray[i++] = passwordFileName = ServerOptionFactory(this,
		"passwordfilename", "",								LABEL("passwordFileName:"),
		255, valString,
		"\0",
		12,0,TabSys, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"The filename of the password file to read on startup.\n"
		)
	);

	prefsArray[i++] = motdFileName = ServerOptionFactory(this,
		"motdfilename", "motd",								LABEL("motdFileName:"),
		255, valString,
		"\0",
		13,0,TabSys, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"The filename of the MOTD file to show to clients when they join.\n"
		)
	);

	prefsArray[i++] = adminMessageFileName = ServerOptionFactory(this,
		"adminMessageFileName", "adminMessage",				LABEL("Admin message filename:"),
		255, valString,
		"\0",
		15,0,TabSys, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"The name of the file where player messages for the\n"
					"server administrator will be saved.  For the messages\n"
					"to be saved the file must already exist.  Players can\n"
					"send these messages by writing to god."
		)
	);

	prefsArray[i++] = adminMessageFileSizeLimit = ServerOptionFactory(this,
		"adminMessageFileSizeLimit", "adminMessageLimit",	LABEL("Admin message limit:"),
		19, valInt,
		"20202",
		16,0,TabSys, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"The maximum size in bytes of the admin message file.\n"
		)
	);

	prefsArray[i++] = maxVisibleObjects = ServerOptionFactory(this,
		"maxvisibleobject", "maxvisibleobjects",			LABEL("Max Vis. Objects:"),
		19, valInt,
		"1000",
		0,1,TabSys, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"What is the maximum number of objects a player can see.\n"
		)
	);

	prefsArray[i++] = pLockServer = ServerOptionFactory(this,
		"plockserver", "",									LABEL("pLockServer?"),
		0, valBool,
#	ifdef PLOCKSERVER
		"yes",
#	else
		"no",
#	endif
		1,1,TabSys, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"Whether the server is prevented from being swapped out of memory.\n"
		)
	);

	prefsArray[i++] = timerResolution = ServerOptionFactory(this,
		"timerresolution", "",								LABEL("Timer Resolution:"),
		19, valInt,
		"0",
		2,1,TabSys, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"If set to nonzero xpilots will requests signals from the OS at\n"
					"1/timerResolution second intervals.  The server will then compute\n"
					"a new frame FPS times out of every timerResolution signals.\n"
		)
	);

	prefsArray[i++] = password = ServerOptionFactory(this,
		"password", "",										LABEL("password:"),
		255, valString,
		"\0",
		3,1,TabSys, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"The password needed to obtain operator privileges.\n"
					"If specified on the command line, on many systems other\n"
					"users will be able to see the password.  Therefore, using\n"
					"the password file instead is recommended."
		)
	);

	password->optControl = (OptControl)(OPT_DEFAULTS|OPT_PASSWORD|OPT_COMMAND);
	prefsArray[i++] = clientPortStart = ServerOptionFactory(this,
		"clientportstart", "",								LABEL("clientPortStart:"),
		19, valInt,
		"0",
		4,1,TabSys, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"Use UDP ports clientPortStart - clientPortEnd (for firewalls)\n"
		)
	);

	prefsArray[i++] = clientPortEnd = ServerOptionFactory(this,
		"clientportend", "",								LABEL("clientPortEnd:"),
		19, valInt,
		"0",
		5,1,TabSys, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"Use UDP ports clientPortStart - clientPortEnd (for firewalls)\n"
		)
	);

	prefsArray[i++] = firewallPortList = ServerOptionFactory(this,
		"firewallPortList", "",								LABEL("firewallPortList:"),
		255, valString,
		"50000-50100",
		6,1,TabSys, OPTIONALL,
		TUNERDEF(TunerFirewallPortList),
		HELPTEXT(	"Use UDP ports in this range(for firewalls)\n"
					"You can specify multiple ranges, separated with a comma\n"
					"i.e. 53,80,1024-2000,10000-11000\n"
		)
	);

	prefsArray[i++] = maxClientsPerIP = ServerOptionFactory(this,
		"maxClientsPerIP", "",								LABEL("maxClientsPerIP:"),
		19, valInt,
		"2",
		7,1,TabSys, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"maxClientsPerIP"
					"This prevents unfriendly players from occupying all the bases"
					", effectively \"kicking\" paused players and denying other players to join.\n"
		)
	);

	prefsArray[i++] = numControlConnections = ServerOptionFactory(this,
		"numControlConnections", "",						LABEL("ControlConnections:"),
		19, valInt,
		"1",
		9,1,TabSys, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"Number of additional control ports allowed (not including players)\n"
					"These ports are used by the server control program (XPilotControl)\n"
					"the score server (XPScoreServer)\n"
					"And later perhaps by the (nonexistant) XPilot proxy program.\n"
		)
	);

	prefsArray[i++] = notifyServerChanges = ServerOptionFactory(this,
		"notifyServerChanges", "",							LABEL("Notify of Changes?"),
		19, valBool,
		"yes",
		10,1,TabSys, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"Notify players when there are runtime tweaks to the server.\n"
		)
	);

	prefsArray[i++] = serverMessage1 = ServerOptionFactory(this,
		"serverMessage1", "",								LABEL("server Message 1:"),
		255, valString,
		"",
		12,1,TabSys, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"An optional message that can be displayed to a player\n"
					"when joining this server.\n"
					"To have the message appear in the top (talk) section,\n"
					"It must end in ']' i.e. \"Hi Mom [*]\"\n"
		)
	);

	prefsArray[i++] = serverMessage2 = ServerOptionFactory(this,
		"serverMessage2", "",								LABEL("server Message 2:"),
		255, valString,
		"",
		13,1,TabSys, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"An optional second line of a message that can be displayed\n"
					"to a player when joining this server.\n"
					"To have the message appear in the top (talk) section,\n"
					"It must end in ']' i.e. \"Hi Mom [*]\"\n"
		)
	);

///////////////////////////////////////////////////////////////////////////////
// Server Tab
	prefsArray[i++] = isConnected = ServerOptionFactory(this,
		"", "",												LABEL("Connected to Server:"),
		19, valBool,
		"no",
		0,2,TabServer, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"Whether we are connected to a server or not\n"
		)
	);

	prefsArray[i++] = saveTuned = ServerOptionFactory(this,
		"saveTuned", "",									LABEL("Save Tuned"),
		19, valEvent,
		"no",
		1,2,TabServer, OPTIONALL,
		TUNERDEF(TunerSaveTuned),
		HELPTEXT(	"Save the tuned events in a file.\n"
					"The next time this map is loaded, the server will load the\n"
					"tuned options as well\n"
		)
	);

	prefsArray[i++] = refreshValues = ServerOptionFactory(this,
		"", "",												LABEL("Refresh from Server:"),
		19, valBool,
		"no",
		0,1,TabServer, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"Refresh the values from the current server\n"
		)
	);

	prefsArray[i++] = connectedToName = ServerOptionFactory(this,
		"", "",												LABEL("Server:"),
		19, valString,
		"localhost",
		0,0,TabServer, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"Name of the server that we are connected to\n"
		)
	);

	prefsArray[i++] = connectedToPort = ServerOptionFactory(this,
		"", "",												LABEL("Port:"),
		19, valInt,
		"15345",
		1,0,TabServer, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"Port number of the server that we are connected to\n"
		)
	);

	prefsArray[i++] = connectedToPassword = ServerOptionFactory(this,
		"", "",												LABEL("Server Password:"),
		19, valString,
		"Marlene",
		2,0,TabServer, OPTIONALL,
		TUNERDEF(TunerNone),
		HELPTEXT(	"Password that this server requires\n"
		)
	);


	mapData = new ServerOptionBlocks(this, "mapdata","","",0,"",0,0,0, OPTIONALL,
//									 TUNERDEF(TunerNone));		this doesn't compile on Linux?
									 (TunerFunc)NULL);

	numPrefs = i;

	if (numPrefs != NUMPREFS)
	{
		// You must adjust NUMPREFS to be = numPrefs
		fatal("Danger! numPrefs %d <> NUMPREFS %d\n", numPrefs, NUMPREFS);
	}
	isInit = false;
	OptionsChanged();		// go update each option
}
#endif	// _SERVEROPTIONSINIT_H_
