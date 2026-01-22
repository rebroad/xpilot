/* $Id: World.main.cpp,v 1.16 2004/05/30 16:20:03 dick Exp $
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
 */
/*
 *  $Log: World.main.cpp,v $
 *  Revision 1.16  2004/05/30 16:20:03  dick
 *  Add "TEAM BASES" to the ServerInfo (for PrivatePanel parsing).
 *  Don't srand() because we don't rand() anymore.
 *
 *  Revision 1.15  2004/05/22 15:12:34  dick
 *  Remove #ifndef SILENT
 *
 *  Revision 1.14  2004/05/05 01:04:29  dick
 *  Need randommnt.h
 *
 *  Revision 1.13  2004/04/16 21:07:59  dick
 *  NetServer becomes a little UDP engine that can be plugged into different apps.
 *
 *  Revision 1.12  2004/03/03 08:04:01  dick
 *  call ConfigStartup() to seed our directories
 *
 *  Revision 1.11  2004/02/23 02:06:09  dick
 *  robot.h becomes Robot.h
 *
 *  Revision 1.10  2004/02/20 06:27:47  dick
 *  SendPlayMessage becomes BroadcastPlayMessage() because it sends a play message
 *  to all connections.
 *
 *  Revision 1.9  2004/02/18 00:58:34  dick
 *  Delete_player becomes DeletePlayer
 *
 *  Revision 1.8  2004/02/16 14:29:44  dick
 *  server/sched.cpp and friends become serverlib/NetServer
 *  which disconnects the udp listen and receive functionality from the game.
 *
 *  Revision 1.7  2004/02/03 03:39:54  dick
 *  Add Windows precompiled header support.
 *
 *  Revision 1.6  2004/01/26 16:57:20  dick
 *  Get_process_id becomes GetProcessID
 *
 *  Revision 1.5  2004/01/21 17:52:35  dick
 *  EndGame returns a bool
 *
 *  Revision 1.4  2004/01/17 19:37:17  dick
 *  WildMap is now in the common library. Control and server use this,
 *  instead of each having it's own copy.
 *
 *  Revision 1.3  2004/01/06 04:48:28  dick
 *  Add support for building under Cygwin.  Still needs work and the output
 *  is X11 and requires something like Cygwin's XWin.exe to run under.
 *
 *  Revision 1.2  2002/09/16 21:52:33  dick
 *  Use Stringized ServerT.
 *
 *  Revision 1.1  2002/09/12 19:46:21  dick
 *  play.cpp becomes World.play.cpp; World.server.cpp becomes World.main.cpp
 *
 *  Revision 1.30  2002/09/11 17:40:53  dick
 *  Linux needs to reference theWorld[0]->EndGame
 *
 *  Revision 1.29  2002/09/11 16:42:04  dick
 *  Here endeth the server encapsulation task.
 *  theWorld is now an array of World(s) and get loaded dynamically.
 *
 *  Revision 1.28  2002/09/09 23:42:55  dick
 *  Handle connecting a LocalCtl to this server.
 *
 *  Revision 1.27  2002/09/01 00:13:45  dick
 *  Remove obsolete windows #include "winServer.h"
 *
 *  Revision 1.26  2002/08/31 23:32:35  dick
 *  Use IsServerKilled() to detect early termination of the server thread.
 *
 *  Revision 1.25  2002/08/01 14:32:10  dick
 *  Remove references to theWorld
 *
 *  Revision 1.24  2002/07/19 07:50:12  dick
 *  Remove option NoQuit
 *
 *  Revision 1.23  2002/07/19 03:21:20  dick
 *  Remove NoQuit option.
 *
 *  Revision 1.22  2002/07/12 15:47:14  dick
 *  Bring back commented out serverAddr fixups.
 *
 *  Revision 1.21  2002/07/11 16:36:03  dick
 *  Update to XPilot-4.5.4beta
 *
 *  Revision 1.20  2002/07/08 05:52:59  dick
 *  showtime() handles the space after itself
 *
 *  Revision 1.19  2002/07/01 04:33:25  dick
 *  Windows' timer runs like Unix'.  A system tick increments a counter and the
 *  sched loop actually calls the game timer loop.
 *
 *  Revision 1.18  2002/06/30 06:48:52  dick
 *  SendMessage is a bad word.  It's a Windows function that is a macro and gets
 *  translated to SendMessageA or SendMessageW.  Although it works, renaming ours
 *  to SendPlayMessage makes browsing the source code easier.
 *
 *  Revision 1.17  2002/06/26 07:01:18  dick
 *  Use xperrno instead of errno and seterrno(0) instead of errno = 0.
 *
 *  Revision 1.16  2002/06/02 05:48:55  dick
 *  EndGame takes an optional string, which gets sent to the connections.
 *
 *  Revision 1.15  2002/06/01 06:06:48  dick
 *  Encapsulate (almost) everything.  Get rid of (almost) all refs to theWorld.
 *
 *  Revision 1.14  2002/05/18 22:51:59  dick
 *  Linux cleanups
 *
 *  Revision 1.13  2002/05/18 22:34:15  dick
 *  Move showtime to common.
 *
 *  Revision 1.12  2002/05/18 20:55:38  dick
 *  Update to XPilot-4.5.1
 *
 *  Revision 1.11  2001/08/26 10:35:40  dick
 *  Playing with constant seeds to try to easily recreate a bug (where type ends up
 *  being a 0.  Very rare.)
 *
 *  Revision 1.10  2001/08/14 21:37:14  dick
 *  Array Obj[] becomes objs[].  We now have a class called Obj.
 *
 *  Revision 1.9  2001/08/10 08:11:37  dick
 *  New class ConnectionPlayer subclasses Connection to provide player
 *  functions.  New class Frame subclasses ConnectionPlayer which provides
 *  Frame update functionality.
 *
 *  Revision 1.8  2001/08/09 07:43:34  dick
 *  New class Connection
 *
 *  Revision 1.7  2001/08/06 05:41:50  dick
 *  struct sock_t becomes class Sock
 *
 *  Revision 1.6  2001/08/05 08:05:36  dick
 *  Add Tuners to the server.  ComputeSensorRange(), AddTank(), RemoveTank()
 *  become members of Player.  ComputeGravity*() become members of World.
 *
 *  Revision 1.5  2001/08/01 06:39:41  dick
 *  AFUNC cleanups. address to function...
 *
 *  Revision 1.4  2001/07/24 18:47:37  dick
 *  Linux (gcc) needs the address of (&) member functions passed around.
 *  Windows doesn't like this, so define AFUNC which is a '&'.
 *
 *  Revision 1.3  2001/07/24 09:03:59  dick
 *  Remove all global options.  Use World.options instead.
 *
 *  Revision 1.2  2001/07/23 21:08:36  dick
 *  Encapsulate ShipMass, ShotsMax, ShotsLife, ShotsMass, FPS.
 *
 *  Revision 1.1  2001/07/23 07:38:42  dick
 *  Fold contact, metaserver, netserver, rules, and server to be members of World.
 *
 *  Revision 1.6  2001/07/22 08:14:42  dick
 *  Call theWorld.Parser()
 *
 *  Revision 1.5  2001/07/20 05:20:11  dick
 *  Yet another reorg.  Break server/xpilotedit common files into serverlib.
 *
 *  Revision 1.4  2001/07/10 08:10:08  dick
 *  Don't need (deleted) map.h because World.h is included in global.h
 *
 *  Revision 1.3  2001/07/08 06:17:27  dick
 *  Allow constant random seeds for debugging
 *
 *  Revision 1.2  2001/07/07 12:00:43  dick
 *  Rename classes to C++ "Style".  old World becomes theWorld.
 *
 */
#include "StdAfx.h"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <signal.h>
#include <errno.h>
#include <time.h>
#include <limits.h>
#include <sys/stat.h>
#include <sys/types.h>


#if defined(_UNIX) || defined(_CYGWIN)
# include <unistd.h>
# ifndef __hpux
#  include <sys/time.h>
# endif
# include <pwd.h>
# include <sys/param.h>
#else
# include <windows.h>
# include <io.h>
# include "NT/ServerThreadW32.h"
#endif


#ifdef PLOCKSERVER
# if defined(__linux__)
#  include <sys/mman.h>
# else
#  include <sys/lock.h>
# endif
#endif

#include "version.h"
#include "config.h"
#include "types.h"
#include "const.h"
#include "global.h"
#include "proto.h"
#include "Sock.h"
#include "NetServerServer.h"
#include "ConnectionPlayer.h"
#include "ConnectionControl.h"
#include "bit.h"
#include "error.h"
#include "portability.h"
#include "server.h"
#include "commonproto.h"
#include "randommt.h"
#include "showtime.h"
#include "metaserver.h"
#include "Robot.h"

char server_version[] = VERSION;

#ifndef	lint
char xpilots_versionid[] = "@(#)$" TITLE " $";
#endif

/*
 * Global variables
 */
const char* szAppName= APPNAME;
bool saveAllPrefs = true;	// DIK: sure, why not. I don't know what this does.

// The copy of the world used while running xpilots or XPilotServer
// The above "Global variables" should all be encapsulated under here
World* theWorld[MAX_WORLDS];

// Our one and only NetServer
NetServer*	netServer;

#ifdef LOG
static bool		Log = true;
#endif
static bool		NoPlayersEnteredYet = true;
int			game_lock = false;
time_t			gameOverTime = 0;
time_t			serverStartTime = 0;

static void Check_server_versions(void);
static void Handle_signal(int sig_no);

#if defined(_UNIX) || defined(_CYGWIN)
#define	xpexit(_i)		exit (_i);
#else
#define	xpexit(_i)		return(FALSE);
#endif


///////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{
	int	i;
	for (i=0; i<MAX_WORLDS; i++)
		theWorld[i] = NULL;

	netServer = new NetServerServer;
	InitConnectionControl();

	theWorld[0] = new World;
	return(theWorld[0]->Main(argc, argv));
}


///////////////////////////////////////////////////////////////////////////////
int World::Main(int argc, char **argv)
{
	int		timer_tick_rate;
	/*
	 * Make output always linebuffered.  By default pipes
	 * and remote shells cause stdout to be fully buffered.
	 */
	setvbuf(stdout, NULL, _IOLBF, BUFSIZ);
	setvbuf(stderr, NULL, _IOLBF, BUFSIZ);

	/*
	 * --- Output copyright notice ---
	 */

	xpprintf("  " COPYRIGHT ".\n"
			 "  " TITLE " comes with ABSOLUTELY NO WARRANTY; "
			 "for details see the\n"
			 "  provided LICENSE file.\n\n");

	init_error(argv[0]);
	ConfigStartup();

	Check_server_versions();

#ifndef	NOTRANDOM
	// Windows DEBUG builds defines NOTRANDOM, so the game plays the same
	// every time (until a random human joins)
	// this is very useful for reproducing world bug anomolies
	seedMT(time((time_t *)0) * GetProcessId());
#else
	seedMT(1906);
#endif

	if (Parser(argc, argv) == FALSE)
		xpexit (0);
	if (!GrokMap())
		xpexit(0);

	plock_server(options.pLockServer->GetBool()); /* Lock the server into memory */
	Make_table();			/* Make trigonometric tables */
	ComputeGravity();
	FindBaseDirection();
	WallsInit();

	/* Allocate memory for players, shots and messages */
	Alloc_players(this, numBases + MAX_PSEUDO_PLAYERS);
	Alloc_shots(this, MAX_TOTAL_SHOTS);
	AllocCells();

	MoveInit();

	Robot_init(this);

	TreasureInit();

	/*
	* Get server's official name.
	*/
	if (serverHost.GetLength())
	{
		serverAddr = sock_get_addr_by_name(serverHost);
		if (!serverAddr.GetLength())
		{
			seterrno(0);
			error("Failed name lookup on: %s", (PCSTR)serverHost);
			return 1;
		}
		server.host = serverHost;
	}
	else
	{
		SockGetLocalHostname(server.host,
								(options.reportToMetaServer->GetBool() != 0 &&
								options.searchDomainForXPilot->GetBool() != 0));
	}
	if (IsServerKilled())
		return(0);
	if (!serverAddr.GetLength())	// Still no address?
	{
		serverAddr = sock_get_addr_by_name(server.host);
	}
	if (IsServerKilled())
		return(0);
	if (!serverAddr.GetLength())	// Still no address?
	{
		serverAddr = sock_get_addr_by_name("localhost");
	}
	if (IsServerKilled())
		return(0);

	GetLoginName(server.owner);

	/*
	 * Log, if enabled.
	 */
	LogGame("START");

//	if (!ContactInit())
//		return(FALSE);

	if (IsServerKilled())
		return(0);
	MetaInit(this);

	if (netServer->Setup( numBases + options.numControlConnections->GetInt()) == -1) {
		return(EndGame());
	}
	if (IsServerKilled())
		return(0);
	if (InitSetup() == -1) {
		return -1;
	}
	if (IsServerKilled())
		return(0);
#if defined(_UNIX) || defined(_CYGWIN)
	signal(SIGHUP, SIG_IGN);
	signal(SIGTERM, Handle_signal);
	signal(SIGINT, Handle_signal);
	signal(SIGPIPE, SIG_IGN);
#ifdef IGNORE_FPE
	signal(SIGFPE, SIG_IGN);
#endif
#endif	/* _WINDOWS */
	/*
	 * Set the time the server started
	 */
	serverStartTime = time(NULL);

    xpprintf("%sServer runs at %d frames per second\n", showtime(), GetFPS());

	if (options.timerResolution->GetInt() > 0) {
		timer_tick_rate = options.timerResolution->GetInt();
	}
	else {
		timer_tick_rate = GetFPS();
	}
	netServer->ContactInit();
	netServer->InstallTimerTick(&World::MainLoop, timer_tick_rate, this);
#if defined(_WINDOWS) && !defined(_CYGWIN)
	/* Windows returns here, we let the server thread call sched() */
	return(1);
#endif
	netServer->Sched();
	xpprintf("sched returned!?");
	EndGame();

	return 1;
}

///////////////////////////////////////////////////////////////////////////////
void World::MainLoop(void* v)
{
	World* w = (World*)v;
	w->MainLoop();
}
///////////////////////////////////////////////////////////////////////////////
void World::MainLoop(void)
{

	if ((netServer->mainLoops & 0x3F) == 0)
		MetaUpdate(this, 0);

	netServer->mainLoops++;
	/*
	 * Check for possible shutdown, the server will
	 * shutdown when shutdownServer (a counter) reaches 0.
	 * If the counter is < 0 then no shutdown is in progress.
	 */
	if (shutdownServer >= 0)
	{
		if (shutdownServer == 0)
			EndGame();
		else
			shutdownServer--;
	}

	netServer->Input();

	if (numPlayers > numRobots + numPseudoPlayers || options.idleRun->GetBool())
	{
		if (NoPlayersEnteredYet)
		{
			if (numPlayers > numRobots + numPseudoPlayers)
			{
				NoPlayersEnteredYet = false;
				if (options.gameDuration->GetDouble() > 0.0)
				{
					xpprintf("%sServer will stop in %g minutes.\n", showtime(), options.gameDuration->GetDouble());
					gameOverTime = (time_t)(options.gameDuration->GetDouble() * 60) + time((time_t *)NULL);
				}
			}
		}

		UpdateObjects();

		if ((netServer->mainLoops % UPDATES_PR_FRAME) == 0)
			Frame_update(this);
	}

	QueueLoop();
}

///////////////////////////////////////////////////////////////////////////////
/*
 *  Last function, exit with grace.
 */
bool World::EndGame(PCSTR lastRites)
{
	Player*		pl;
	String		msg;

	if (lastRites) {
		msg.printf("server exiting: %s", lastRites);
	}
	else if (shutdownServer == 0) {
		seterrno(0);
		error("Shutting down...");
		msg.printf("shutting down: %s", shutdownReason);
	} else {
		msg.printf("server exiting");
	}

	while (numPlayers > 0)		/* Kick out all remaining players */
	{
		pl = players[numPlayers - 1];
		if (pl->conn)
			pl->conn->DestroyConnection(msg);
		else
			DeletePlayer(this, numPlayers - 1);
	}

	/* Tell meta server that we are gone. */
	MetaGone(this);

	ContactCleanup();

	Free_players(this);
	Free_shots(this);
	FreeMap();
	FreeCells();
//    Free_options();
	LogGame("END");			    /* Log end */

#if defined(_UNIX) || defined(_CYGWIN)
	exit (0);
#else
	serverKilled = TRUE;
	return(FALSE);                  /* return FALSE so windows bubbles out of the main loop */
#endif
}

///////////////////////////////////////////////////////////////////////////////
/*
 * Return a good team number for a player.
 *
 * If the team is not specified, the player is assigned
 * to a non-empty team which has space.
 *
 * If there is none or only one team with playing (i.e. non-paused)
 * players the player will be assigned to a randomly chosen empty team.
 *
 * If there is more than one team with playing players,
 * the player will be assigned randomly to a team which
 * has the least number of playing players.
 *
 * If all non-empty teams are full, the player is assigned
 * to a randomly chosen available team.
 *
 * Prefer not to place players in the robotTeam if possible.
 */
int World::PickTeam(int pick_for_type)
{
	int			i,
			least_players,
			num_available_teams = 0,
			playing_teams = 0,
			losing_team;
	Player		*pl;
	int			playing[MAX_TEAMS];
	int			free_bases[MAX_TEAMS];
	int			available_teams[MAX_TEAMS];
	DFLOAT		team_score[MAX_TEAMS];
	DFLOAT		losing_score;

	for (i = 0; i < MAX_TEAMS; i++) {
		free_bases[i] = teams[i].numBases - teams[i].numMembers;
		playing[i] = 0;
		team_score[i] = 0;
		available_teams[i] = 0;
	}
	if (options.restrictRobots->GetBool()) {
		if (pick_for_type == PickForRobot) {
			if (free_bases[options.robotTeam->GetInt()] > 0) {
				return options.robotTeam->GetInt();
			} else {
				return TEAM_NOT_SET;
			}
		}
	}
	if (options.reserveRobotTeam->GetBool()) {
		if (pick_for_type != PickForRobot) {
			free_bases[options.robotTeam->GetInt()] = 0;
		}
	}

	/*
	 * Find out which teams have actively playing members.
	 * Exclude paused players and tanks.
	 * And calculate the score for each team.
	 */
	for (i = 0; i < numPlayers; i++) {
		pl = players[i];
		if (IS_TANK_PTR(pl)) {
			continue;
		}
		if (BIT(pl->status, PAUSE)) {
			continue;
		}
		if (!playing[pl->team]++) {
			playing_teams++;
		}
		if (IS_HUMAN_PTR(pl) || IS_ROBOT_PTR(pl)) {
			team_score[pl->team] += pl->score;
		}
	}
	if (playing_teams <= 1) {
		for (i = 0; i < MAX_TEAMS; i++) {
			if (!playing[i] && free_bases[i] > 0) {
				available_teams[num_available_teams++] = i;
			}
		}
	} else {
		least_players = numPlayers;
		for (i = 0; i < MAX_TEAMS; i++) {
			/* We fill teams with players first. */
			if (playing[i] > 0 && free_bases[i] > 0) {
				if (playing[i] < least_players) {
					least_players = playing[i];
				}
			}
		}

		for (i = 0; i < MAX_TEAMS; i++) {
			if (free_bases[i] > 0) {
				if (least_players == numPlayers
					|| playing[i] == least_players) {
					available_teams[num_available_teams++] = i;
				}
			}
		}
	}

	if (!num_available_teams) {
		for (i = 0; i < MAX_TEAMS; i++) {
			if (free_bases[i] > 0) {
				available_teams[num_available_teams++] = i;
			}
		}
	}

	if (num_available_teams == 1) {
		return available_teams[0];
	}

	if (num_available_teams > 1) {
		losing_team = -1;
		losing_score = LONG_MAX;
		for (i = 0; i < num_available_teams; i++) {
			if (team_score[available_teams[i]] < losing_score
				&& available_teams[i] != options.robotTeam->GetInt()) {
				losing_team = available_teams[i];
				losing_score = team_score[losing_team];
			}
		}
		return losing_team;
	}

	/*NOTREACHED*/
	return TEAM_NOT_SET;
}


///////////////////////////////////////////////////////////////////////////////
/*
 * Return status for server
 *
 * TODO
*/
void World::ServerInfo(char *str, unsigned max_size)
{
	int 				i, j, k;
	Player				*pl, **order, *best = NULL;
	DFLOAT				ratio, best_ratio = -1e7;
	char				name[MAX_CHARS];
	char				lblstr[MAX_CHARS];
	char				msg[MSG_LEN];
	String				s;

	sprintf(str,
			"SERVER VERSION...: %s\n"
			"STATUS...........: %s\n"
			"MAX SPEED........: %d fps\n"
			"WORLD (%3dx%3d)..: %s\n"
			"      AUTHOR.....: %s\n"
			"PLAYERS (%2d/%2d)..:\n",
			server_version,
			(game_lock && shutdownServer == -1) ? "locked" :
			(!game_lock && shutdownServer != -1) ? "shutting down" :
			(game_lock && shutdownServer != -1) ? "locked and shutting down" : "ok",
			GetFPS(),
			blockWidth, blockHeight,
				(PCSTR)options.mapName->GetString(),
				(PCSTR)options.mapAuthor->GetString(),
			numPlayers, numBases);

	/* calculate number of available homebases per team. */
	if (BIT(rules->mode, TEAM_PLAY)) {
		char		freebases[120];
		int			num_active_players;
		int			active_per_team[MAX_TEAMS];
		freebases[0] = '\0';
		/* Find out the number of active players. */
		num_active_players = 0;
		memset(active_per_team, 0, sizeof active_per_team);
		for (i = 0; i < numPlayers; i++) {
			if (IS_HUMAN_IND(this, i) && !BIT(players[i]->status, PAUSE)) {
				num_active_players++;
				if (BIT(rules->mode, TEAM_PLAY)) {
					active_per_team[i]++;
				}
			}
		}

		j = 0;
		for (i = 0; i < MAX_TEAMS; i++) {
			if (i == options.robotTeam->GetInt()
			 && options.reserveRobotTeam->GetBool()) {
				continue;
			}
			if (teams[i].numBases > 0) {
				sprintf(&freebases[j], "%d=%d,", i,
						teams[i].numBases - active_per_team[i]);
				j += strlen(&freebases[j]);
			}
		}
		/* strip trailing comma. */
		if (j) {
			freebases[j-1] = '\0';
		}
		s.printf("TEAM BASES.......: %d,%s\n", numTeamBases, freebases);
		strlcat(str, s, max_size);
	}

	if (strlen(str) >= max_size) {
				seterrno(0);
				error("Server_info string overflow (%d)", max_size);
				str[max_size - 1] = '\0';
				return;
	}
	if (numPlayers <= 0) {
				return;
	}

	sprintf(msg,
		   "\nNO:  TM: NAME:             LIFE:   SC:    PLAYER:\n"
		   "-------------------------------------------------\n");
	if (strlen(msg) + strlen(str) >= max_size) {
		return;
	}
	strlcat(str, msg, max_size);

	if ((order = (Player **) malloc(numPlayers * sizeof(Player *))) == NULL) {
		error("No memory for order");
		return;
	}
	for (i=0; i < numPlayers; i++) {
		pl = players[i];
		if (BIT(rules->mode, LIMITED_LIVES)) {
			ratio = (DFLOAT) pl->score;
		} else {
			ratio = (DFLOAT) pl->score / (pl->life + 1);
		}
		if ((best == NULL
				|| ratio > best_ratio)
			&& !BIT(pl->status, PAUSE)) {
			best_ratio = ratio;
			best = pl;
		}
		for (j = 0; j < i; j++) {
			if (order[j]->score < pl->score) {
				for (k = i; k > j; k--) {
					order[k] = order[k - 1];
				}
				break;
			}
		}
		order[j] = pl;
	}
	for (i=0; i < numPlayers; i++) {
		pl = order[i];
		strlcpy(name, pl->name, MAX_CHARS);
		if (IS_ROBOT_PTR(pl)) {
			if ((k = Robot_war_on_player(this, getInd[pl->id])) != NO_ID) {
				sprintf(name + strlen(name), " (%s)", players[getInd[k]]->name);
				if (strlen(name) >= 19) {
					strcpy(&name[17], ")");
				}
			}
		}
		sprintf(lblstr, "%c%c %-19s%03d%6d",
				(pl == best) ? '*' : pl->mychar,
				(pl->team == TEAM_NOT_SET) ? ' ' : (pl->team + '0'),
				name, (int)pl->life, (int)pl->score);
		sprintf(msg, "%2d... %-36s%s@%s\n",
				i+1, lblstr, pl->realname,
				IS_HUMAN_PTR(pl)
				? pl->hostname
				: "xpilot.org");
		if (strlen(msg) + strlen(str) >= max_size)
			break;
		strlcat(str, msg, max_size);
	}
	free(order);
}


///////////////////////////////////////////////////////////////////////////////
static void Handle_signal(int sig_no)
{
		seterrno(0);

#if defined(_UNIX) || defined(_CYGWIN)
	switch (sig_no) {

	case SIGHUP:
		//if (theWorld.options.noQuit->GetBool()) {
		//	  signal(SIGHUP, SIG_IGN);
			return;
		//}
		error("Caught SIGHUP, terminating.");
		theWorld[0]->EndGame();
		break;
	case SIGINT:
		error("Caught SIGINT, terminating.");
		theWorld[0]->EndGame();
		break;
	case SIGTERM:
		error("Caught SIGTERM, terminating.");
		theWorld[0]->EndGame();
		break;

	default:
		error("Caught unkown signal: %d", sig_no);
		theWorld[0]->EndGame();
		break;
	}
#endif
	_exit(sig_no);		/* just in case */
}


///////////////////////////////////////////////////////////////////////////////
void World::LogGame(const char *heading)
{
#ifdef LOG
	char str[1024];
	FILE *fp;
	char timenow[81];
	struct tm *ptr;
	time_t lt;

	if (!Log)
		return;

	lt = time(NULL);
	ptr = localtime(&lt);
	strftime(timenow,79,"%I:%M:%S %p %Z %A, %B %d, %Y",ptr);

	sprintf(str,"%-50.50s\t%10.10s@%-15.15s\tWorld: %-25.25s\t%10.10s\n",
			timenow,
			Server.owner,
			Server.host,
			World.name,
			heading);

	if ((fp = fopen(Conf_logfile(), "a")) == NULL) {	/* Couldn't open file */
		error("Couldn't open log file, contact %s", Conf_localguru());
		return;
	}

	fputs(str, fp);
	fclose(fp);
#endif
}

///////////////////////////////////////////////////////////////////////////////
void World::GameOver()
{
	DFLOAT				maxsc, minsc;
	int 				i, win, loose;
	char				msg[128];

	BroadcastPlayMessage("Game over...");

	/*
	 * Hack to prevent Compute_Game_Status from starting over again...
	 */
	options.gameDuration->Set(-1.0);

	if (BIT(rules->mode, TEAM_PLAY)) {
		DFLOAT teamscore[MAX_TEAMS];
		maxsc = -32767;
		minsc = 32767;
		win = loose = -1;

		for (i=0; i < MAX_TEAMS; i++) {
			teamscore[i] = 1234567; /* These teams are not used... */
		}
		for (i=0; i < numPlayers; i++) {
			int team;
			if (IS_HUMAN_IND(this, i)) {
				team = players[i]->team;
				if (teamscore[team] == 1234567) {
					teamscore[team] = 0;
				}
				teamscore[team] += players[i]->score;
			}
		}

		for (i=0; i < MAX_TEAMS; i++) {
			if (teamscore[i] != 1234567) {
				if (teamscore[i] > maxsc) {
					maxsc = teamscore[i];
					win = i;
				}
				if (teamscore[i] < minsc) {
					minsc = teamscore[i];
					loose = i;
				}
			}
		}

		if (win != -1) {
			sprintf(msg,"Best team (%ld Pts): Team %d", maxsc, win);
			BroadcastPlayMessage(msg);
			xpprintf("%s\n", msg);
		}

		if (loose != -1 && loose != win) {
			sprintf(msg,"Worst team (%ld Pts): Team %d", minsc, loose);
			BroadcastPlayMessage(msg);
			xpprintf("%s\n", msg);
		}
	}

	maxsc = -32767;
	minsc = 32767;
	win = loose = -1;

	for (i = 0; i < numPlayers; i++) {
		SET_BIT(players[i]->status, GAME_OVER);
		if (IS_HUMAN_IND(this, i)) {
			if (players[i]->score > maxsc) {
				maxsc = players[i]->score;
				win = i;
			}
			if (players[i]->score < minsc) {
				minsc = players[i]->score;
				loose = i;
			}
		}
	}
	if (win != -1) {
		sprintf(msg,"Best human player: %s", players[win]->name);
		BroadcastPlayMessage(msg);
		xpprintf("%s\n", msg);
	}
	if (loose != -1 && loose != win) {
		sprintf(msg,"Worst human player: %s", players[loose]->name);
		BroadcastPlayMessage(msg);
		xpprintf("%s\n", msg);
	}
}


///////////////////////////////////////////////////////////////////////////////
/*
 * Verify that all source files making up this program have been
 * compiled for the same version.  Too often bugs have been reported
 * for incorrectly compiled programs.
 */
extern char asteroid_version[];
extern char cannon_version[];
extern char cell_version[];
extern char checknames_version[];
//extern char cmdline_version[];
extern char collision_version[];
extern char command_version[];
extern char config_version[];
extern char contact_version[];
extern char error_version[];
extern char event_version[];
extern char fileparser_version[];
extern char frame_version[];
extern char id_version[];
extern char item_version[];
extern char laser_version[];
extern char map_version[];
extern char math_version[];
extern char metaserver_version[];
extern char net_version[];
extern char netserver_version[];
extern char objpos_version[];
extern char option_version[];
extern char parser_version[];
extern char play_version[];
extern char player_version[];
extern char portability_version[];
extern char robot_version[];
extern char robotdef_version[];
extern char rules_version[];
extern char saudio_version[];
extern char sched_version[];
extern char score_version[];
extern char server_version[];
extern char ship_version[];
extern char shipshape_version[];
extern char shot_version[];
extern char socklib_version[];
extern char update_version[];
extern char walls_version[];
//extern char wildmap_version[];

static void Check_server_versions(void)
{
	static struct file_version {
		char			filename[16];
		char			*versionstr;
	} file_versions[] = {
		{ "asteroid", asteroid_version },
		{ "cannon", cannon_version },
		{ "cell", cell_version },
		{ "checknames", checknames_version },
//		{ "cmdline", cmdline_version },
		{ "collision", collision_version },
		{ "command", command_version },
		{ "config", config_version },
		{ "contact", contact_version },
		{ "error", error_version },
		{ "event", event_version },
		{ "fileparser", fileparser_version },
		{ "frame", frame_version },
		{ "id", id_version },
		{ "item", item_version },
		{ "laser", laser_version },
		{ "map", map_version },
		{ "math", math_version },
		{ "metaserver", metaserver_version },
		{ "net", net_version },
		{ "netserver", netserver_version },
		{ "objpos", objpos_version },
		{ "option", option_version },
		{ "parser", parser_version },
		{ "play", play_version },
		{ "player", player_version },
		{ "portability", portability_version },
		{ "robot", robot_version },
		{ "robotdef", robotdef_version },
		{ "rules", rules_version },
		{ "saudio", saudio_version },
		{ "sched", sched_version },
		{ "score", score_version },
		{ "server", server_version },
		{ "ship", ship_version },
		{ "shipshape", shipshape_version },
		{ "shot", shot_version },
		{ "socklib", socklib_version },
		{ "update", update_version },
		{ "walls", walls_version },
//		{ "wildmap", wildmap_version },
	};
	int 				i;
	int 				oops = 0;

	for (i = 0; i < NELEM(file_versions); i++) {
		if (strcmp(VERSION, file_versions[i].versionstr)) {
			oops++;
			error("Source file %s.cpp (\"%s\") is not compiled "
				  "for the current version (\"%s\")!",
				  file_versions[i].filename,
				  file_versions[i].versionstr,
				  VERSION);
		}
	}
	if (oops) {
		error("%d version inconsistency errors, cannot continue.", oops);
		error("Please recompile this program properly.");
		exit(1);
	}
}

///////////////////////////////////////////////////////////////////////////////
#if defined(PLOCKSERVER) && defined(__linux__)
/*
 * Patches for Linux plock support by Steve Payne <srp20@cam.ac.uk>
 * also added the -pLockServer command line option.
 * All messed up by BG again, with thanks and apologies to Steve.
 */
/* Linux doesn't seem to have plock(2).  *sigh* (BG) */
#if !defined(PROCLOCK) || !defined(UNLOCK)
#define PROCLOCK		0x01
#define UNLOCK			0x00
#endif
static int plock(int op)
{
#if defined(MCL_CURRENT) && defined(MCL_FUTURE)
	return op ? mlockall(MCL_CURRENT | MCL_FUTURE) : munlockall();
#else
	return -1;
#endif
}
#endif

///////////////////////////////////////////////////////////////////////////////
/*
 * Lock the server process data and code segments into memory
 * if this program has been compiled with the PLOCKSERVER flag.
 * Or unlock the server process if the argument is false.
 */
int plock_server(int onoff)
{
#ifdef PLOCKSERVER
	int 				op;

	if (onoff) {
		op = PROCLOCK;
	}
	else {
		op = UNLOCK;
	}
	if (plock(op) == -1) {
		static int num_plock_errors;
		if (++num_plock_errors <= 3) {
			error("Can't plock(%d)", op);
		}
		return -1;
	}
	return onoff;
#else
	if (onoff) {
		xpprintf("Can't plock: Server was not compiled with plock support\n");
	}
	return 0;
#endif
}

///////////////////////////////////////////////////////////////////////////////
void World::TunerPLock()
{
	//pLockServer = (plock_server(pLockServer) == 1) ? true : false;
		options.pLockServer->Set((plock_server(options.pLockServer->GetBool()) == 1)
																? true : false);
}

///////////////////////////////////////////////////////////////////////////////
void ServerLogAdminMessage(Player* pl, const char *str)
{
	/*
	 * Only log the message if logfile already exists,
	 * is writable and less than some KBs in size.
	 */
	String		logfilename = pl->world->options.adminMessageFileName->GetString();
	const int			logfile_size_limit = pl->world->options.adminMessageFileSizeLimit->GetInt();
	FILE				*fp;
	struct stat 		st;
	char				msg[MSG_LEN * 2];

	if ((!logfilename.IsEmpty()) &&
		(logfilename[0] != '\0') &&
		(logfile_size_limit > 0) &&
		(access(logfilename, 2) == 0) &&
		(stat(logfilename, &st) == 0) &&
		(st.st_size + 80 < logfile_size_limit) &&
		((size_t)(logfile_size_limit - st.st_size - 80) > strlen(str)) &&
		((fp = fopen(logfilename, "a")) != NULL))
	{
		fprintf(fp,
				"%s[%s]{%s@%s(%s)|%s}:\n"
				"\t%s\n",
				showtime(),
				pl->name,
				pl->realname, pl->hostname,
				pl->conn->GetAddr(),
				pl->conn->GetDpy(),
				str);
		fclose(fp);
		sprintf(msg, "%s [%s]:[%s]", str, pl->name, "GOD");
		pl->SetMessage(msg);
	}
	else {
		pl->SetMessage(" < GOD doesn't seem to be listening>");
	}
}
