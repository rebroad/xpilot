/* $Id: RobotMan.cpp,v 1.7 2007/01/19 20:29:25 dick Exp $
 *
 * The Robot Manager.  A singleton attached to a World.
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
 *  $Log: RobotMan.cpp,v $
 *  Revision 1.7  2007/01/19 20:29:25  dick
 *  Rename Robot4a to Robot5 to promote the xp5 brand.
 *
 *  Revision 1.6  2007/01/19 07:13:41  dick
 *  Turn off some Debug.
 *  Remove unused Watch cruft.
 *
 *  Revision 1.5  2007/01/17 09:15:29  dick
 *  Handle the FlipRobotPage for a player.  Move Robot base class to his own file.
 *
 *  Revision 1.4  2007/01/16 06:07:56  dick
 *  Use the correct lib dir to find the robots file
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
 *  Revision 1.1  2007/01/10 18:14:47  dick
 *  All robot actions are now handled through RobotMan.
 *  There is one RobotMan per World.
 *
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
#include "Robot4.h"
#include "Robot5.h"
#include "RobotMan.h"
#include "error.h"
#include "server.h"
#include "commonproto.h"
#include "ConnectionPlayer.h"


PCSTR	robotman_version = VERSION;

#define	USE_TEST_ROBOT_FACTORY	true
#define	DONT_READ_ROBOTS_FILE	false

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
 * Function prototype for robot type setup routines.
 * Add your own robot type setup routine here.
 */
extern int Robot_default_setup(Robot* type_ptr);

#if defined(DEVLOPMENT) || defined(ALLBOTS)
extern int Stratbot_setup(RobotType* type_ptr);
#endif

///////////////////////////////////////////////////////////////////////////////
RobotMan::RobotMan() {
	numAvailable = 0;
}

///////////////////////////////////////////////////////////////////////////////
/*
 * First time initialization of all the robot stuff.
 */
void RobotMan::Init(World* _w)
{
	this->w = _w;

	listOfDrivers.Add((Obj*)(new RobotCreator("robot4", &Robot4::Factory)));
	listOfDrivers.Add((Obj*)(new RobotCreator("robot5", &Robot5::Factory)));
	ParseRobotFile();

	if (w->options.robotTeam->GetInt() < 0 || w->options.robotTeam->GetInt() >= MAX_TEAMS)
		w->options.robotTeam->Set(0);
}

///////////////////////////////////////////////////////////////////////////////
void RobotMan::ParseRobotFile() {
	parsingRobs = NULL;
	numParsingRobs = 0;
	maxParsingRobs = 0;
	if (!DONT_READ_ROBOTS_FILE && !w->options.robotFile->GetString().IsEmpty()) {
		String s;
		s = w->options.robotFile->GetString();
		ParseRobotFile(s);
	}
	if (numAvailable == 0) {
		if (!USE_TEST_ROBOT_FACTORY) {
			robotFactory = &defaultFactory[0];
			numAvailable = defaultFactoryNELEM();
		} else {
			robotFactory = &testFactory[0];			// use dick's test factory of robots
			numAvailable = testFactoryNELEM();
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
void RobotMan::ParseRobotFile(String s) {
	String path;
	FILE* fp = fopen(s, "r");
	if (!fp) {
		path = Conf_libdir();
		path += s;
		fp = fopen(path, "r");
		if (!fp)
			xpprintf("Can't open robots file \"%s\"\n", (PCSTR)s);
	}
	if (fp) {
		char buf[1024];
		char name_buf[MAX_NAME_LEN];
		char ship_buf[2*MSG_LEN];
		char type_buf[MAX_NAME_LEN];
		char para_buf[MSG_LEN];
		int got_name = 0;
		xpprintf("%sParsing robot file: %s\n", showtime(), (PCSTR)s);
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
			else if (!strncmp(buf, "include", 7)) {
				String	inc;
				inc = &buf[7];
				inc.TrimWhiteSpace();
				ParseRobotFile(inc);
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
				if (numParsingRobs == maxParsingRobs) {
					if (maxParsingRobs == 0) {
						maxParsingRobs = 10;
						parsingRobs = (RobotDescriptor *)malloc(maxParsingRobs * sizeof(RobotDescriptor));
					} else {
						maxParsingRobs += 10;
						parsingRobs = (RobotDescriptor *)realloc(parsingRobs,
												  maxParsingRobs * sizeof(RobotDescriptor));
					}
					if (!parsingRobs) {
						error("Not enough memory to parse robotsfile");
						fclose(fp);
						break;
					}
				}
				strcpy(parsingRobs[numParsingRobs].driver, type_buf);
				strcpy(parsingRobs[numParsingRobs].name, name_buf);
				strcpy(parsingRobs[numParsingRobs].config, para_buf);
				strcpy(parsingRobs[numParsingRobs].shape, ship_buf);
				parsingRobs[numParsingRobs].used = 0;
				numParsingRobs++;
			}
		}
		if (numParsingRobs > 0) {
			robotFactory = parsingRobs;
			numAvailable = numParsingRobs;
		}
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

///////////////////////////////////////////////////////////////////////////////
void RobotMan::RobotTalks(enum robot_talk_t says_what,
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

///////////////////////////////////////////////////////////////////////////////
void RobotMan::RobotCreate() {
	int 			i;
	int				num;
	int 			most_used;
	int				least_used;
	char			msg[MSG_LEN];

	if (peek_ID() == 0) {
		return;
	}

	//
	// Select a robot to use
	//
	most_used = 0;
	for (i = 0; i < numAvailable; i++) {
		if (robotFactory[i].used > robotFactory[most_used].used) {
			most_used = i;
		}
	}
	for (i = 0; i < w->numPlayers; i++) {
		Robot*	trobot;
		if (IS_ROBOT_IND(w, i)) {
			trobot = w->players[i]->robot;
			if (robotFactory[trobot->robots_ind].used < robotFactory[most_used].used) {
				robotFactory[trobot->robots_ind].used = robotFactory[most_used].used;
			}
		}
	}
	least_used = 0;
	for (i = 0; i < numAvailable; i++) {
		if (robotFactory[i].used < robotFactory[least_used].used) {
			least_used = i;
		}
	}
	num = (int)(rfrac() * numAvailable);
	while (robotFactory[num].used > robotFactory[least_used].used) {
		if (++num >= numAvailable) {
			num = 0;
		}
	}

	//
	// Create the robot
	//
	RobotDescriptor*	rob;
	Player*				pl;
	Robot*				newRobot;

	rob = &robotFactory[num];
	rob->used++;

	RobotCreator* rc = (RobotCreator*)listOfDrivers.GetHead();
	while (rc) {
		if (rc->driver == robotFactory[num].driver)
			break;
		rc= (RobotCreator*)rc->GetNext();

	}
	if (!rc) {
		xpprintf("%sCan't find robot driver for type \"%s\"\n",
			showtime(), (PCSTR)robotFactory[num].driver);
		return;
	}
	newRobot = (*rc->creator)();
	newRobot->SetWorld(w);
	newRobot->robots_ind = num;
	w->players[w->numPlayers]->Init((w->options.allowShipShapes->GetBool())
							? Parse_shape_str(rob->shape)
							: (ShipObj *)NULL);
	pl = w->players[w->numPlayers];
	pl->playerType = PlayerRobot;
	pl->robot = newRobot;
	newRobot->pl = pl;

	strlcpy(pl->name, rob->name, MAX_CHARS);
	strlcpy(pl->realname, w->options.robotRealName->GetString(), MAX_CHARS);
	strlcpy(pl->hostname, w->options.robotHostName->GetString(), MAX_CHARS);

	pl->color = WHITE;
	pl->turnspeed = MAX_PLAYER_TURNSPEED;
	pl->turnspeed_s = MAX_PLAYER_TURNSPEED;
	pl->turnresistance = 0.12;
	pl->turnresistance_s = 0.12;
	pl->power = MAX_PLAYER_POWER;
	pl->power_s = MAX_PLAYER_POWER;
	pl->check = 0;
	if (BIT(w->rules->mode, TEAM_PLAY)) {
		pl->team = w->PickTeam(PickForRobot);
		w->teams[pl->team].numMembers++;
		w->teams[pl->team].numRobots++;
	}
	if (pl->mychar != 'W')
		pl->mychar = 'R';

	w->ScoreServerPlayerEvent(pl, Join);
	w->ScoreServerRequestPlayerScore(pl);
	pl->fuel.l1 = 100 * FUEL_SCALE_FACT;
	pl->fuel.l2 = 200 * FUEL_SCALE_FACT;
	pl->fuel.l3 = 500 * FUEL_SCALE_FACT;

	// Pick_startpos(NumPlayers);
	newRobot->Create(rob->config);

	pl->PickStartpos();

	pl->GoHome();

	request_ID();
	w->numPlayers++;
	w->numRobots++;

	if (BIT(w->rules->mode, TEAM_PLAY)
	 && w->options.teamShareScore->GetBool()) {
		if (w->teams[pl->team].numMembers == 1) {
			/* reset team score on first player */
			w->teams[pl->team].score = 0;
		}
		TEAM_SCORE(w, pl->team, 0);
	}
	for (i = 0; i < w->numPlayers - 1; i++) {
		if (w->players[i]->conn) {
			w->players[i]->conn->SendPlayer(pl);
			w->players[i]->conn->SendBase(pl->id, pl->home_base);
		}
	}

	RobotTalks(ROBOT_TALK_ENTER, pl->name, "");

	if (w->options.logRobots->GetBool())
				xpprintf("%s%s (%d, %s) starts at startpos %d.\n",
						showtime(), pl->name,
						w->numPlayers, pl->realname, pl->home_base);
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

///////////////////////////////////////////////////////////////////////////////
void RobotMan::Destroy(int ind) {
	Player*		pl = w->players[ind];

	pl->robot->Destroy();
	delete (w->players[ind]->robot);
	w->players[ind]->robot = NULL;
}

///////////////////////////////////////////////////////////////////////////////
void RobotMan::Delete(int ind, int kicked) {
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
		//D( xpprintf("%s robot %s is leaving the game\n", showtime(), w->players[ind]->name);)
		DeletePlayer(w, ind);
		//D( xpprintf("%s robot left the game\n", showtime()); )
	}
}

///////////////////////////////////////////////////////////////////////////////
void	RobotMan::FlipRobotPage(Player* guru) {
	int		i;


	D( xpprintf("%s FlipRobotPage\n", showtime()); )
	// Who are we watching?
	if (BIT(guru->status, PLAYING|PAUSE|GAME_OVER|KILLED) == PLAYING)
		return;	/* can't go after non-playing ships */
	if (guru->myEyes == NO_ID)		// not watching nobody
		return;
	Robot* watchedBot = NULL;
	for (i=0; i<w->numPlayers; i++) {
		Player* pl = w->players[i];
		if (IS_ROBOT_PTR(pl) && pl->id == guru->myEyes) {
			watchedBot = pl->robot;
			break;
		}
	}
	if (!watchedBot)
		return;
	if (++guru->robotWatchPage >= watchedBot->GetWatchPageCount())
		guru->robotWatchPage = 0;

}

///////////////////////////////////////////////////////////////////////////////
//                   Passthrough functions to a robot                        //
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/*
 * Ask a robot for an alliance
 */
void RobotMan::Invite(int ind, int invInd) {
	w->players[ind]->robot->Invite(invInd);
}

///////////////////////////////////////////////////////////////////////////////
/*
 * Turn on a war lock.
 */
void RobotMan::RobotSetWar(int ind, int victim_id) {
	w->players[ind]->robot->SetWar(victim_id);
}

///////////////////////////////////////////////////////////////////////////////
/*
 * Turn off a war lock.
 * The only time when this can be called is if
 * a player a robot has war on leaves the game.
 */
void RobotMan::ResetWar(int ind) {
	RobotSetWar(ind, -1);
}

///////////////////////////////////////////////////////////////////////////////
/*
 * Someone has programmed a robot (using ECM) to seek some player.
 */
void RobotMan::Program(int ind, int victim_id) {
	RobotSetWar(ind, victim_id);
}

///////////////////////////////////////////////////////////////////////////////
/*
 * Return the id of the player this robot has war on.
 * If the robot is not in peace mode then return -1.
 */
int RobotMan::WarOnPlayer(int ind) {
	return(w->players[ind]->robot->WarOnPlayer());
}

///////////////////////////////////////////////////////////////////////////////
/*
 * A robot has killed someone.
 * Or a robot has been killed by someone.
 * Maybe this is enough reason for the killed robot to change
 * its behavior with respect to the player it has been killed by.
 */
void RobotMan::War(int ind, int killer) {
	Player*	pl = w->players[ind];
	Player*	kp = w->players[killer];
	int		i;

	if (killer == ind) {
		return;
	}

	if (IS_ROBOT_PTR(kp)) {
		RobotTalks(ROBOT_TALK_KILL, kp->name, pl->name);

		if (WarOnPlayer(killer) == pl->id) {
			for (i = 0; i < w->numPlayers; i++) {
				if (w->players[i]->conn) {
					w->players[i]->conn->SendWar(kp->id, NO_ID);
				}
			}
		}
		RobotSetWar(killer, -1);
	}

	if (IS_ROBOT_PTR(pl)
		&& (int)(rfrac() * 100) < kp->score - pl->score
		&& !TEAM(w, ind, killer)
		&& !ALLIANCE(w, ind, killer)) {

		RobotTalks(ROBOT_TALK_WAR, pl->name, kp->name);

		/*
		 * Give fuel for offensive.
		 * KK: unfair advantage.
		 */
		/* pl->fuel.sum = MAX_PLAYER_FUEL; */

		if (WarOnPlayer(ind) != kp->id) {
			for (i = 0; i < w->numPlayers; i++) {
				if (w->players[i]->conn) {
					w->players[i]->conn->SendWar(pl->id, kp->id);
				}
			}
			SoundPlayAll(w, DECLARE_WAR_SOUND);
			RobotSetWar(ind, kp->id);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
/*
 * A robot starts on its homebase.
 */
void RobotMan::GoHome(int ind) {
	w->players[ind]->robot->GoHome();
}

///////////////////////////////////////////////////////////////////////////////
/*
 * Someone sends a message to a robot.
 * The format of the message is: "This is the real message [receiver]:[sender]"
 */
void RobotMan::Message(int ind, const char *message) {
	w->players[ind]->robot->Message(message);
}

///////////////////////////////////////////////////////////////////////////////
/*
 * A robot plays this frame.
 */
void RobotMan::RobotPlay(int ind) {
	w->players[ind]->robot->Play();
	w->players[ind]->robot->SetWatchFull();
}

///////////////////////////////////////////////////////////////////////////////
/*
 * Check if robot is still considered good enough to continue playing.
 * Return FALSE if robot continues playing,
 * return TRUE if robot leaves the game.
 */
int RobotMan::RobotCheckLeave(int ind) {
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
			RobotTalks(ROBOT_TALK_LEAVE, pl->name, "");
			w->BroadcastPlayMessage(msg);
			Delete(ind, false);
			return TRUE;
		}
	}

	return FALSE;
}


///////////////////////////////////////////////////////////////////////////////
/*
 * On each round we call the robot type round ticker.
 */
void RobotMan::RoundTick() {
	int			i;

	for (i = 0; i < w->numPlayers; i++) {
		if (w->players[i]->robot) {
			w->players[i]->robot->RoundTick();
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
/*
 * Update tanks here.
 */
static void Tank_play(World* w, int ind) {
	Player*	pl = w->players[ind];
	int		t = frame_loops % (int)(TANK_NOTHRUST_TIME + TANK_THRUST_TIME);

	if (t == 0) {
		SET_BIT(pl->status, THRUSTING);
	} else if (t == TANK_THRUST_TIME) {
		CLR_BIT(pl->status, THRUSTING);
	}
}

///////////////////////////////////////////////////////////////////////////////
void RobotMan::Update() {
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
		&& w->numRobots < numAvailable
		&& !(BIT(w->rules->mode, TEAM_PLAY)
		&& w->options.restrictRobots->GetBool()
		&& w->teams[w->options.robotTeam->GetInt()].numMembers >=
		   w->teams[w->options.robotTeam->GetInt()].numBases))
	{
		if (++new_robot_delay >= ROBOT_CREATE_DELAY * w->GetFPS()) {
			RobotCreate();
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
				Delete(-1, false);
			}
		}
	}

	if (w->numRobots <= 0 && w->numPseudoPlayers <= 0) {
		return;
	}

	CHECK_OBJS();

	RoundTick();

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
				if (RobotCheckLeave(i)) {
					i--;
				}
			}
			continue;
		}

		if (RobotCheckLeave(i)) {
			i--;
			continue;
		}

		if (w->roundDelay > 0) {
			continue;
		}

		/*
		 * Let the robot code control this robot.
		 */
		RobotPlay(i);
		CHECK_OBJS();
	}
}

