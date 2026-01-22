/* $Id: World.cpp,v 1.33 2004/05/22 15:12:34 dick Exp $
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
 */
/*
 *  $Log: World.cpp,v $
 *  Revision 1.33  2004/05/22 15:12:34  dick
 *  Remove #ifndef SILENT
 *
 *  Revision 1.32  2004/05/07 04:27:41  dick
 *  _visibility becomes Visibility.  global updateScores becomes a member of World.
 *  Handle rank/rate data from the scoreserver and send it to the client.
 *
 *  Revision 1.31  2004/02/16 14:29:44  dick
 *  server/sched.cpp and friends become serverlib/NetServer
 *  which disconnects the udp listen and receive functionality from the game.
 *
 *  Revision 1.30  2004/02/05 14:56:58  cppc
 *  First cut at integrating 'it' play mode.
 *
 *  Revision 1.29  2004/02/03 03:39:54  dick
 *  Add Windows precompiled header support.
 *
 *  Revision 1.28  2004/01/29 06:36:47  dick
 *  Initialize numTeamBases to 0
 *
 *  Revision 1.27  2004/01/20 20:01:07  dick
 *  Map loading debug to try to help venable's boot woes
 *
 *  Revision 1.26  2004/01/19 01:29:09  dick
 *  Deadwood delete
 *
 *  Revision 1.25  2004/01/17 19:37:17  dick
 *  WildMap is now in the common library. Control and server use this,
 *  instead of each having it's own copy.
 *
 *  Revision 1.24  2004/01/16 17:32:22  dick
 *  Fix ReportToMetaTuner().  If initially disabled, enabling reportToMeta while
 *  running requires calling MetaInit() to lookup the metaservers.
 *
 *  Revision 1.23  2004/01/09 17:33:58  dick
 *  Add a firewallPortList and initialize it with our game defaults.
 *
 *  Revision 1.22  2002/09/16 01:34:19  dick
 *  Fix bug in up base attractor
 *
 *  Revision 1.21  2002/09/11 16:42:04  dick
 *  Here endeth the server encapsulation task.
 *  theWorld is now an array of World(s) and get loaded dynamically.
 *
 *  Revision 1.20  2002/09/01 00:13:45  dick
 *  Remove obsolete windows #include "winServer.h"
 *
 *  Revision 1.19  2002/08/01 14:34:15  dick
 *  Preparing to fix numLogins and numLogouts.
 *
 *  Revision 1.18  2002/07/05 21:24:55  dick
 *  Free the walldist array on cleanup
 *
 *  Revision 1.17  2002/06/23 06:39:21  dick
 *  Cleanup the ScoreServer, if it exists
 *
 *  Revision 1.16  2002/06/09 21:13:12  dick
 *  Deadwood delete
 *
 *  Revision 1.15  2002/06/01 06:29:59  dick
 *  cannon.* becomes Cannon.*
 *
 *  Revision 1.14  2002/06/01 06:06:47  dick
 *  Encapsulate (almost) everything.  Get rid of (almost) all refs to theWorld.
 *
 *  Revision 1.13  2002/05/20 02:17:45  dick
 *  Windows needs errno and seterrno in portability.h.
 *  Windows also needs rint() defined if not using fltk.
 *
 *  Revision 1.12  2002/05/18 22:51:59  dick
 *  Linux cleanups
 *
 *  Revision 1.11  2002/05/18 20:55:38  dick
 *  Update to XPilot-4.5.1
 *
 *  Revision 1.10  2001/08/11 02:17:52  dick
 *  Commented out a parent link during a panic debugging session on Windows.
 *  I added a ServerOption and forgot to increase NUMPREFS.  This causes a crash
 *  even before main() is called. very hard to find.
 *
 *  Revision 1.9  2001/08/10 08:11:37  dick
 *  New class ConnectionPlayer subclasses Connection to provide player
 *  functions.  New class Frame subclasses ConnectionPlayer which provides
 *  Frame update functionality.
 *
 *  Revision 1.8  2001/08/05 08:05:36  dick
 *  Add Tuners to the server.  ComputeSensorRange(), AddTank(), RemoveTank()
 *  become members of Player.  ComputeGravity*() become members of World.
 *
 *  Revision 1.7  2001/08/02 19:49:53  dick
 *  Better grokking of the World (from mapdata to worlddata)
 *
 *  Revision 1.6  2001/07/24 09:03:59  dick
 *  Remove all global options.  Use World.options instead.
 *
 *  Revision 1.5  2001/07/23 21:08:36  dick
 *  Encapsulate ShipMass, ShotsMax, ShotsLife, ShotsMass, FPS.
 *
 *  Revision 1.4  2001/07/23 07:36:54  dick
 *  map block data is now derived from options/mapData.
 *
 *  Revision 1.3  2001/07/22 08:18:41  dick
 *  Setup the locals mapWidth and mapHeight
 *
 *  Revision 1.2  2001/07/18 20:15:32  dick
 *  The first pass at integrating xpilotedit's options with the server options.
 *  The server does not build yet.  xpilotedit works as before, with the new
 *  definitions.
 *
 *  Revision 1.1  2001/07/10 08:06:16  dick
 *  map becomes World.  A tough choice... The object was called world but the
 *  functions were called Map.
 *
 *  Revision 1.2  2001/07/07 12:00:42  dick
 *  Rename classes to C++ "Style".  old World becomes theWorld.
 *
 */
#include "StdAfx.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>

#ifndef _WINDOWS
# include <sys/file.h>
#endif

#include "version.h"
#include "config.h"
#include "const.h"
#include "global.h"
#include "proto.h"
#include "bit.h"
#include "error.h"
#include "commonproto.h"
#include "Cannon.h"
#include "ConnectionControlScoreServer.h"
#include "WildMap.h"

char map_version[] = VERSION;


///////////////////////////////////////////////////////////////////////////////
World::World()
{
	options.parent			= this;
    blockWidth				= 256;
    blockHeight				= 256;
    diagonal				= (int) LENGTH(blockWidth, blockHeight);
    width					= blockWidth * BLOCK_SZ;
    height					= blockHeight * BLOCK_SZ;
    hypotenuse				= (int) LENGTH(width, height);
	getInd_1				= 0;
//	mainLoops				= 0;
	numObjs					= 0;
	numPulses				= 0;
	numEcms					= 0;
	numTransporters			= 0;
	numRobots				= 0;
	minRobots				= 0;
	maxRobots				= 0;
	gameLocked				= 0;
	roundsPlayed			= 0;
	roundDelay				= 0;
	roundDelaySend			= 0;
	roundTime				= 0;
	metaInited				= false;
    numFuels				= 0;
	numTeamBases			= 0;
    numBases				= 0;
    numGravs				= 0;
    numCannons				= 0;
    numWormholes			= 0;
    numTreasures			= 0;
    numTargets				= 0;
    numItemConcentrators	= 0;
	numPlayers				= 0;
	numAlliances			= 0;
	numPseudoPlayers		= 0;
	numQueuedPlayers		= 0;
	numControlConnections	= 0;
	numAsteroidConcs		= 0;
	shutdownServer			= -1;
	shutdownDelay			= 1000;
	scoreServer				= NULL;
//	numLogins = 0;
//	numLogouts = 0;
	block					= NULL;
	itemID					= NULL;
	gravity					= NULL;
	gravs					= NULL;
	bases					= NULL;
	cannons					= NULL;
	fuels					= NULL;
	wormHoles				= NULL;
	itemConcentrators		= NULL;
	asteroidConcs			= NULL;
	walldist				= NULL;
	cells					= NULL;
	objectNodeOffset		= 0;
	cellDist				= NULL;
	cellDistSize			= 0;
	tag						= NO_ID;
	updateScores			= true;
	firewallPortList.Set("50000-50100");
}

///////////////////////////////////////////////////////////////////////////////
World::~World()
{
	if (scoreServer)
		delete scoreServer;
}

///////////////////////////////////////////////////////////////////////////////
void World::FreeMap()
{
	if (block) {
		free(block);
		block = NULL;
	}
	if (itemID) {
		free(itemID);
		itemID = NULL;
	}
	if (gravity) {
		free(gravity);
		gravity = NULL;
	}
	if (gravs) {
		free(gravs);
		gravs = NULL;
	}
	if (bases) {
		free(bases);
		bases = NULL;
	}
	if (cannons) {
		free(cannons);
		cannons = NULL;
	}
	if (fuels) {
		free(fuels);
		fuels = NULL;
	}
	if (wormHoles) {
		free(wormHoles);
		wormHoles = NULL;
	}
	if (itemConcentrators) {
		free(itemConcentrators);
		itemConcentrators = NULL;
	}
	if (asteroidConcs) {
		free(asteroidConcs);
		asteroidConcs = NULL;
	}
	if (walldist) {
		free(walldist);
		walldist = NULL;
	}
}

///////////////////////////////////////////////////////////////////////////////
void World::AllocMap(void)
{
	int _x;

	if (block || gravity)
		FreeMap();

	block =
		(unsigned char **)malloc(sizeof(unsigned char *)*blockWidth
								 + blockWidth*sizeof(unsigned char)*blockHeight);
	itemID =
		(unsigned short **)malloc(sizeof(unsigned short *)*blockWidth
								 + blockWidth*sizeof(unsigned short)*blockHeight);
	gravity =
		(vector **)malloc(sizeof(vector *)*blockWidth
						  + blockWidth*sizeof(vector)*blockHeight);
	gravs = NULL;
	bases = NULL;
	fuels = NULL;
	cannons = NULL;
	wormHoles = NULL;
	itemConcentrators = NULL;
		asteroidConcs = NULL;

	if (block == NULL || itemID == NULL || gravity == NULL) {
		FreeMap();
		error("Couldn't allocate memory for map (%d bytes)",
			  blockWidth * (blockHeight * (sizeof(unsigned char) + sizeof(vector))
						 + sizeof(vector*)
						 + sizeof(unsigned char*)));
		exit(-1);
	} else {
		unsigned char *map_line;
		unsigned char **map_pointer;
		unsigned short *item_line;
		unsigned short **item_pointer;
		vector *grav_line;
		vector **grav_pointer;

		map_pointer = block;
		map_line = (unsigned char*) ((unsigned char**)map_pointer + blockWidth);
		item_pointer = itemID;
		item_line = (unsigned short*) ((unsigned short**)item_pointer + blockWidth);
		grav_pointer = gravity;
		grav_line = (vector*) ((vector**)grav_pointer + blockWidth);

		for (_x=0; _x<blockWidth; _x++) {
			*map_pointer = map_line;
			map_pointer += 1;
			map_line += blockHeight;
			*item_pointer = item_line;
			item_pointer += 1;
			item_line += blockHeight;
			*grav_pointer = grav_line;
			grav_pointer += 1;
			grav_line += blockHeight;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
#ifdef DEBUG
void World::PrintMap(void)						/* Debugging only. */
{
	int _x, _y;

	for (_y=blockHeight-1; _y>=0; _y--) {
		for (_x=0; _x<blockWidth; _x++)
			switch (block[_x][_y]) {
			case SPACE:
				putchar(' ');
				break;
			case BASE:
				putchar('_');
				break;
			default:
				putchar('X');
				break;
			}
		putchar('\n');
	}
}
#endif


///////////////////////////////////////////////////////////////////////////////
static void Map_extra_error(int line_num)
{
#ifndef SILENT
	static int prev_line_num, error_count;
	const int max_error = 5;

	if (line_num > prev_line_num) {
		prev_line_num = line_num;
		if (++error_count <= max_error) {
			xpprintf("Map file contains extranous characters on line %d\n",
					 line_num);
		}
		else if (error_count - max_error == 1) {
			xpprintf("And so on...\n");
		}
	}
#endif
}


///////////////////////////////////////////////////////////////////////////////
static void Map_missing_error(int line_num)
{
#ifndef SILENT
	static int prev_line_num, error_count;
	const int max_error = 5;

	if (line_num > prev_line_num) {
		prev_line_num = line_num;
		if (++error_count <= max_error) {
			xpprintf("Not enough map data on map data line %d\n", line_num);
		}
		else if (error_count - max_error == 1) {
			xpprintf("And so on...\n");
		}
	}
#endif
}

///////////////////////////////////////////////////////////////////////////////
// DIK: I had to change this from the old map.c way.
// They ran the loop from y->0; i run the loop from 0->y.
bool World::GrokMap(void)
{
	int i, _x, _y, c;

	// Init_map();
	mapWidth = options.mapWidth->GetInt();
	mapHeight = options.mapHeight->GetInt();

	if (mapWidth <= 0 || mapWidth > MAX_MAP_SIZE ||
			mapHeight <= 0 || mapHeight > MAX_MAP_SIZE)
	{
		errno = 0;
		error("mapWidth or mapHeight exceeds map size limit [1, %d]", MAX_MAP_SIZE);
	}
	else
	{
		blockWidth = mapWidth;
		blockHeight = mapHeight;
	}
	if (options.extraBorder->GetBool())
	{
		blockWidth += 2;
		blockHeight += 2;
	}
	diagonal = (int) LENGTH(blockWidth, blockHeight);
	width = blockWidth * BLOCK_SZ;
	height = blockHeight * BLOCK_SZ;
	hypotenuse = (int) LENGTH(width, height);
//	  strlcpy(name, mapName, sizeof(name));
//	  strlcpy(author, mapAuthor, sizeof(author));

	if (options.mapData->optOrigin == OPT_INIT)
	{
		errno = 0;
		warn("Generating random map");
		GenerateRandomMap();
		if (!options.mapData)
			return FALSE;
	}

	AllocMap();

//	  _x = 0;
//	  _y = y - 1;
//	  _y = y;

	SetWorldRules();
	SetWorldItems();
	SetWorldAsteroids();

	if (BIT(rules->mode, TEAM_PLAY|TIMING) == (TEAM_PLAY|TIMING))
	{
		warn("Cannot teamplay while in race mode -- ignoring teamplay");
		CLR_BIT(rules->mode, TEAM_PLAY);
	}

	ServerOptionBlocks* sob = options.mapData;
	_y = 0;
	_x = -1;
//	  while (_y >= 0)
	while(_y < blockHeight)
	{
		_x++;

		if (options.extraBorder->GetBool() && (_x == 0 || _x == blockWidth - 1
				|| _y == 0 || _y == blockHeight - 1))
		{
			if (_x >= blockWidth)
			{
				_x = -1;
//				_y--;
				_y++;
				continue;
			}
			else
			{
				/* make extra border of solid rock */
				c = 'x';
			}
		}
		else
		{
			c = sob->blocks[_x][blockHeight-1-_y].cdata;
			if (c == '\0' || c == EOF)
			{
				if (_x < blockWidth)
				{
					/* not enough map data on this line */
					Map_missing_error(blockHeight - _y);
					c = ' ';
				}
				else
				{
					c = '\n';
				}
			}
			else
			{
				if (c == '\n' && _x < blockWidth)
				{
					/* not enough map data on this line */
					Map_missing_error(blockHeight - _y);
					c = ' ';
				}
				else
				{
					//bd++;
				}
			}
		}
		if (_x >= blockWidth)
		{
//						_y--;
			_y++;
			_x = -1;
			continue;
		}
		/*
		if (_x >= x || c == '\n')
		{
			_y--; _x = -1;
			if (c != '\n')
			{						// Get rest of line
				Map_extra_error(y - _y);
				while (c != '\n' && c != EOF)
				{
					c = bd->cdata;
					//bd++;
				}
			}
			continue;
		}
		*/

		switch (block[_x][_y] = c) {
		case 'r':
		case 'd':
		case 'f':
		case 'c':
			numCannons++;
			break;
		case '*':
		case '^':
			numTreasures++;
			break;
		case '#':
			numFuels++;
			break;
		case '!':
			numTargets++;
			break;
		case '%':
			numItemConcentrators++;
			break;
		case '&':
			numAsteroidConcs++;
			break;
		case '_':
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			numBases++;
			break;
		case '+':
		case '-':
		case '>':
		case '<':
			case 'i':
			case 'm':
			case 'j':
			case 'k':
			numGravs++;
			break;
		case '@':
		case '(':
		case ')':
			numWormholes++;
			break;
		case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
		case 'G': case 'H': case 'I': case 'J': case 'K': case 'L':
		case 'M': case 'N': case 'O': case 'P': case 'Q': case 'R':
		case 'S': case 'T': case 'U': case 'V': case 'W': case 'X':
		case 'Y': case 'Z':
			if (BIT(rules->mode, TIMING))
			numChecks++;
			break;
		default:
			break;
		}
	}

	/*
	 * Get space for special objects.
	 */
	if (numCannons > 0
		&& (cannons = (Cannon *)
			malloc(numCannons * sizeof(Cannon))) == NULL) {
		error("Out of memory - cannons");
		exit(-1);
	}
	if (numFuels > 0
		&& (fuels = (Fuel *)
			malloc(numFuels * sizeof(Fuel))) == NULL) {
		error("Out of memory - fuel depots");
		exit(-1);
	}
	if (numGravs > 0
		&& (gravs = (Grav *)
			malloc(numGravs * sizeof(Grav))) == NULL) {
		error("Out of memory - gravs");
		exit(-1);
	}
	if (numWormholes > 0
		&& (wormHoles = (Wormhole *)
			malloc(numWormholes * sizeof(Wormhole))) == NULL) {
		error("Out of memory - wormholes");
		exit(-1);
	}
	if (numTreasures > 0
		&& (treasures = (Treasure *)
			malloc(numTreasures * sizeof(Treasure))) == NULL) {
		error("Out of memory - treasures");
		exit(-1);
	}
	if (numTargets > 0
		&& (targets = (Target *)
			malloc(numTargets * sizeof(Target))) == NULL) {
		error("Out of memory - targets");
		exit(-1);
	}
	if (numItemConcentrators > 0
		&& (itemConcentrators = (ItemConcentrator *)
			malloc(numItemConcentrators * sizeof(ItemConcentrator))) == NULL) {
		error("Out of memory - item concentrators");
		exit(-1);
	}
	if (numAsteroidConcs > 0
		&& (asteroidConcs = (AsteroidConcentrator*)
			malloc(numAsteroidConcs * sizeof(AsteroidConcentrator))) == NULL)
	{
		error("Out of memory - asteroid concentrators");
		exit(-1);
	}
	if (numBases > 0) {
		if ((bases = (Base *)
			malloc(numBases * sizeof(Base))) == NULL) {
			error("Out of memory - bases");
			exit(-1);
		}
	} else {
		error("WARNING: map has no bases!");
	}

	/*
	 * Now reset all counters since we will recount everything
	 * and reuse these counters while inserting the objects
	 * into structures.
	 */
	numCannons = 0;
	numFuels = 0;
	numGravs = 0;
	numWormholes = 0;
	numTreasures = 0;
	numTargets = 0;
	numBases = 0;
	numItemConcentrators = 0;
		numAsteroidConcs = 0;

	for (i = 0; i < MAX_TEAMS; i++) {
		teams[i].numMembers = 0;
		teams[i].numRobots = 0;
		teams[i].numBases = 0;
		teams[i].numTreasures = 0;
		teams[i].numEmptyTreasures = 0;
		teams[i].treasuresDestroyed = 0;
		teams[i].treasuresLeft = 0;
		teams[i].score = 0;
		teams[i].prevScore = 0;
	}

	/*
	 * Change read tags to internal data, create objects
	 */
	{
		int 	worm_in = 0,
				worm_out = 0,
				worm_norm = 0;

		for (_x=0; _x<blockWidth; _x++) {
			u_byte *line = block[_x];
			unsigned short *_itemID = itemID[_x];

			for (_y=0; _y<blockHeight; _y++) {
				char c = line[_y];

				_itemID[_y] = (unsigned short) -1;

				switch (c) {
				case ' ':
				case '.':
				default:
					line[_y] = SPACE;
					break;

				case 'x':
					line[_y] = FILLED;
					break;
				case 's':
					line[_y] = REC_LU;
					break;
				case 'a':
					line[_y] = REC_RU;
					break;
				case 'w':
					line[_y] = REC_LD;
					break;
				case 'q':
					line[_y] = REC_RD;
					break;

				case 'r':
					line[_y] = CANNON;
					_itemID[_y] = numCannons;
					cannons[numCannons].dir = DIR_UP;
					cannons[numCannons].blk_pos.x = _x;
					cannons[numCannons].blk_pos.y = _y;
					cannons[numCannons].pix_pos.x =
												(_x + 0.5) * BLOCK_SZ;
					cannons[numCannons].pix_pos.y =
												(_y + 0.333) * BLOCK_SZ;
					cannons[numCannons].dead_time = 0;
					cannons[numCannons].conn_mask = (unsigned)-1;
					cannons[numCannons].team = TEAM_NOT_SET;
						cannons[numCannons].world = this;
					cannons[numCannons].Init();
					numCannons++;
					break;
				case 'd':
					line[_y] = CANNON;
					_itemID[_y] = numCannons;
					cannons[numCannons].dir = DIR_LEFT;
					cannons[numCannons].blk_pos.x = _x;
					cannons[numCannons].blk_pos.y = _y;
					cannons[numCannons].pix_pos.x =
												(_x + 0.667) * BLOCK_SZ;
					cannons[numCannons].pix_pos.y =
												(_y + 0.5) * BLOCK_SZ;
					cannons[numCannons].dead_time = 0;
					cannons[numCannons].conn_mask = (unsigned)-1;
					cannons[numCannons].team = TEAM_NOT_SET;
						cannons[numCannons].world = this;
					cannons[numCannons].Init();
					numCannons++;
					break;
				case 'f':
					line[_y] = CANNON;
					_itemID[_y] = numCannons;
					cannons[numCannons].dir = DIR_RIGHT;
					cannons[numCannons].blk_pos.x = _x;
					cannons[numCannons].blk_pos.y = _y;
					cannons[numCannons].pix_pos.x =
												(_x + 0.333) * BLOCK_SZ;
					cannons[numCannons].pix_pos.y =
												(_y + 0.5) * BLOCK_SZ;
					cannons[numCannons].dead_time = 0;
					cannons[numCannons].conn_mask = (unsigned)-1;
					cannons[numCannons].team = TEAM_NOT_SET;
						cannons[numCannons].world = this;
					cannons[numCannons].Init();
					numCannons++;
					break;
				case 'c':
					line[_y] = CANNON;
					_itemID[_y] = numCannons;
					cannons[numCannons].dir = DIR_DOWN;
					cannons[numCannons].blk_pos.x = _x;
					cannons[numCannons].blk_pos.y = _y;
					cannons[numCannons].pix_pos.x =
												(_x + 0.5) * BLOCK_SZ;
					cannons[numCannons].pix_pos.y =
												(_y + 0.667) * BLOCK_SZ;
					cannons[numCannons].dead_time = 0;
					cannons[numCannons].conn_mask = (unsigned)-1;
					cannons[numCannons].team = TEAM_NOT_SET;
						cannons[numCannons].world = this;
					cannons[numCannons].Init();
					numCannons++;
					break;

				case '#':
					line[_y] = FUEL;
					_itemID[_y] = numFuels;
					fuels[numFuels].blk_pos.x = _x;
					fuels[numFuels].blk_pos.y = _y;
					fuels[numFuels].pix_pos.x = (_x+0.5f)*BLOCK_SZ;
					fuels[numFuels].pix_pos.y = (_y+0.5f)*BLOCK_SZ;
					fuels[numFuels].fuel = START_STATION_FUEL;
					fuels[numFuels].conn_mask = (unsigned)-1;
					fuels[numFuels].last_change = frame_loops;
					fuels[numFuels].team = TEAM_NOT_SET;
					numFuels++;
					break;

				case '*':
				case '^':
					line[_y] = TREASURE;
					_itemID[_y] = numTreasures;
					treasures[numTreasures].pos.x = _x;
					treasures[numTreasures].pos.y = _y;
					treasures[numTreasures].have = false;
					treasures[numTreasures].destroyed = 0;
					treasures[numTreasures].empty = (c == '^');
					/*
					 * Determining which team it belongs to is done later,
					 * in Find_closest_team().
					 */
					treasures[numTreasures].team = 0;
					numTreasures++;
					break;
				case '!':
					line[_y] = TARGET;
					_itemID[_y] = numTargets;
					targets[numTargets].pos.x = _x;
					targets[numTargets].pos.y = _y;
					/*
					 * Determining which team it belongs to is done later,
					 * in Find_closest_team().
					 */
					targets[numTargets].team = 0;
					targets[numTargets].dead_time = 0;
					targets[numTargets].damage = TARGET_DAMAGE;
					targets[numTargets].conn_mask = (unsigned)-1;
					targets[numTargets].update_mask = 0;
					targets[numTargets].last_change = frame_loops;
					numTargets++;
					break;
				case '%':
					line[_y] = ITEM_CONCENTRATOR;
					_itemID[_y] = numItemConcentrators;
					itemConcentrators[numItemConcentrators].pos.x = _x;
					itemConcentrators[numItemConcentrators].pos.y = _y;
					numItemConcentrators++;
					break;
				case '&':
						line[_y] = ASTEROID_CONCENTRATOR;
						_itemID[_y] = numAsteroidConcs;
					asteroidConcs[numAsteroidConcs].pos.x = _x;
					asteroidConcs[numAsteroidConcs].pos.y = _y;
					numAsteroidConcs++;
						break;
				case '$':
					line[_y] = BASE_ATTRACTOR;
					break;
				case '_':
				case '0':
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
				case '8':
				case '9':
					line[_y] = BASE;
					_itemID[_y] = numBases;
					bases[numBases].pos.x = _x;
					bases[numBases].pos.y = _y;
					/*
					 * The direction of the base should be so that it points
					 * up with respect to the gravity in the region.  This
					 * is fixed in FindBaseDirection() when the gravity has
					 * been computed.
					 */
					bases[numBases].dir = DIR_UP;
					if (BIT(rules->mode, TEAM_PLAY)) {
						if (c >= '0' && c <= '9') {
							bases[numBases].team = c - '0';
						} else {
							bases[numBases].team = 0;
						}
						teams[bases[numBases].team].numBases++;
						if (teams[bases[numBases].team].numBases == 1)
							numTeamBases++;
					} else {
						bases[numBases].team = TEAM_NOT_SET;
					}
					numBases++;
					break;

				case '+':
					line[_y] = POS_GRAV;
					_itemID[_y] = numGravs;
					gravs[numGravs].pos.x = _x;
					gravs[numGravs].pos.y = _y;
					gravs[numGravs].force = -GRAVS_POWER;
					numGravs++;
					break;
				case '-':
					line[_y] = NEG_GRAV;
					_itemID[_y] = numGravs;
					gravs[numGravs].pos.x = _x;
					gravs[numGravs].pos.y = _y;
					gravs[numGravs].force = GRAVS_POWER;
					numGravs++;
					break;
				case '>':
					line[_y]= CWISE_GRAV;
					_itemID[_y] = numGravs;
					gravs[numGravs].pos.x = _x;
					gravs[numGravs].pos.y = _y;
					gravs[numGravs].force = GRAVS_POWER;
					numGravs++;
					break;
				case '<':
					line[_y] = ACWISE_GRAV;
					_itemID[_y] = numGravs;
					gravs[numGravs].pos.x = _x;
					gravs[numGravs].pos.y = _y;
					gravs[numGravs].force = -GRAVS_POWER;
					numGravs++;
					break;
				case 'i':
					line[_y] = UP_GRAV;
					_itemID[_y] = numGravs;
					gravs[numGravs].pos.x = _x;
					gravs[numGravs].pos.y = _y;
					gravs[numGravs].force = GRAVS_POWER;
					numGravs++;
					break;
				case 'm':
					line[_y] = DOWN_GRAV;
					_itemID[_y] = numGravs;
					gravs[numGravs].pos.x = _x;
					gravs[numGravs].pos.y = _y;
					gravs[numGravs].force = -GRAVS_POWER;
					numGravs++;
					break;
				case 'k':
					line[_y] = RIGHT_GRAV;
					_itemID[_y] = numGravs;
					gravs[numGravs].pos.x = _x;
					gravs[numGravs].pos.y = _y;
					gravs[numGravs].force = GRAVS_POWER;
					numGravs++;
					break;
				case 'j':
					line[_y] = LEFT_GRAV;
					_itemID[_y] = numGravs;
					gravs[numGravs].pos.x = _x;
					gravs[numGravs].pos.y = _y;
					gravs[numGravs].force = -GRAVS_POWER;
					numGravs++;
					break;

				case '@':
				case '(':
				case ')':
					wormHoles[numWormholes].pos.x = _x;
					wormHoles[numWormholes].pos.y = _y;
					wormHoles[numWormholes].countdown = 0;
					wormHoles[numWormholes].lastdest = -1;
					wormHoles[numWormholes].temporary = 0;
						wormHoles[numWormholes].lastblock = SPACE;
						wormHoles[numWormholes].lastID = (ushort)-1;
					if (c == '@') {
						wormHoles[numWormholes].type = WORM_NORMAL;
						worm_norm++;
					} else if (c == '(') {
						wormHoles[numWormholes].type = WORM_IN;
						worm_in++;
					} else {
						wormHoles[numWormholes].type = WORM_OUT;
						worm_out++;
					}
					line[_y] = WORMHOLE;
					_itemID[_y] = numWormholes;
					numWormholes++;
					break;

				case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
				case 'G': case 'H': case 'I': case 'J': case 'K': case 'L':
				case 'M': case 'N': case 'O': case 'P': case 'Q': case 'R':
				case 'S': case 'T': case 'U': case 'V': case 'W': case 'X':
				case 'Y': case 'Z':
					if (BIT(rules->mode, TIMING)) {
						check[c-'A'].x = _x;
						check[c-'A'].y = _y;
						line[_y] = CHECK;
					} else {
						line[_y] = SPACE;
					}
					break;

				case 'z':
					line[_y] = FRICTION;
					break;

				case 'b':
					line[_y] = DECOR_FILLED;
					break;
				case 'h':
					line[_y] = DECOR_LU;
					break;
				case 'g':
					line[_y] = DECOR_RU;
					break;
				case 'y':
					line[_y] = DECOR_LD;
					break;
				case 't':
					line[_y] = DECOR_RD;
					break;
				}
			}
		}

		/*
		 * Verify that the wormholes are consistent, i.e. that if
		 * we have no 'out' wormholes, make sure that we don't have
		 * any 'in' wormholes, and (less critical) if we have no 'in'
		 * wormholes, make sure that we don't have any 'out' wormholes.
		 */
		if ((worm_norm) ? (worm_norm + worm_out < 2)
			: (worm_in) ? (worm_out < 1)
			: (worm_out > 0))
		{
			int i;

			xpprintf("Inconsistent use of wormholes, removing them.\n");
			for (i = 0; i < numWormholes; i++)
			{
				block [wormHoles[i].pos.x]
					  [wormHoles[i].pos.y] = SPACE;
				itemID[wormHoles[i].pos.x]
					  [wormHoles[i].pos.y] = (unsigned short) -1;
			}
			numWormholes = 0;
		}

		if (!options.wormTime->GetInt()) {
			for (i = 0; i < numWormholes; i++) {
				int j = (int)(rfrac() * numWormholes);
				while (wormHoles[j].type == WORM_IN)
					j = (int)(rfrac() * numWormholes);
				wormHoles[i].lastdest = j;
			}
		}

		if (BIT(rules->mode, TIMING) && numChecks == 0) {
			xpprintf("No checkpoints found while race mode (timing) was set.\n");
			xpprintf("Turning off race mode.\n");
			CLR_BIT(rules->mode, TIMING);
		}

		/*
		 * Determine which team a treasure belongs to.
		 */
		if (BIT(rules->mode, TEAM_PLAY)) {
			unsigned short team = TEAM_NOT_SET;
			for (i = 0; i < numTreasures; i++) {
				team = FindClosestTeam(treasures[i].pos.x,
										 treasures[i].pos.y);
				treasures[i].team = team;
				if (team == TEAM_NOT_SET) {
					error("Couldn't find a matching team for the treasure.");
				} else {
					teams[team].numTreasures++;
					if (!treasures[i].empty) {
						teams[team].treasuresLeft++;
					} else {
						teams[team].numEmptyTreasures++;
					}
				}
			}
			for (i = 0; i < numTargets; i++) {
				team = FindClosestTeam(targets[i].pos.x,
										 targets[i].pos.y);
				if (team == TEAM_NOT_SET) {
					error("Couldn't find a matching team for the target.");
				}
				targets[i].team = team;
			}
			if (options.teamCannons->GetBool()) {
				for (i = 0; i < numCannons; i++) {
					team = FindClosestTeam(cannons[i].blk_pos.x,
											 cannons[i].blk_pos.y);
					if (team == TEAM_NOT_SET) {
						error("Couldn't find a matching team for the cannon.");
					}
					cannons[i].team = team;
				}
			}
				for (i = 0; i < numFuels; i++) {
						team = FindClosestTeam(fuels[i].blk_pos.x,
																   fuels[i].blk_pos.y);
					if (team == TEAM_NOT_SET) {
								error("Couldn't find a matching team for fuelstation.");
					}
					fuels[i].team = team;
			}
		}
	}

	if (options.maxRobots->GetInt() == -1) {
		maxRobots = numBases;
	}
	if (options.minRobots->GetInt() == -1) {
		minRobots = maxRobots;
	}
	if (BIT(rules->mode, TIMING)) {
		FindBaseOrder();
	}

	xpprintf("World....: %s\nBases....: %d\nMapsize..: %dx%d\nTeam play: %s\n",
		   (PCSTR)options.mapName->GetString(), numBases, blockWidth, blockHeight,
		   BIT(rules->mode, TEAM_PLAY) ? "on" : "off");

	//D( PrintMap(); )

	return TRUE;
}


///////////////////////////////////////////////////////////////////////////////
/*
 * Use wildmap to generate a random map.
 */
void World::GenerateRandomMap(void)
{
	int 		_width, _height;

	options.edgeWrap->Set(true);
	_width = blockWidth;
	_height = blockHeight;

	WildMap::MakeWildMap(&this->options);
	blockWidth = _width;
	blockHeight = _height;
	diagonal = (int) LENGTH(blockWidth, blockHeight);
	width = blockWidth * BLOCK_SZ;
	height = blockHeight * BLOCK_SZ;
	hypotenuse = (int) LENGTH(width, height);
}


///////////////////////////////////////////////////////////////////////////////
/*
 * Find the correct direction of the base, according to the gravity in
 * the base region.
 *
 * If a base attractor is adjacent to a base then the base will point
 * to the attractor.
 */
void World::FindBaseDirection()
{
	int i;

	for (i = 0; i < numBases; i++) {
		int 	_x = bases[i].pos.x;
		int		_y = bases[i].pos.y;
		int		dir;
		int		att;
		double	dx = gravity[_x][_y].x;
		double	dy = gravity[_x][_y].y;

		if (dx == 0.0 && dy == 0.0) {	/* Undefined direction? */
			dir = DIR_UP;		/* Should be set to direction of gravity! */
		} else {
			dir = (int)FindDir(-dx, -dy);
			dir = ((dir + RES/8) / (RES/4)) * (RES/4);	/* round it */
			dir = MOD2(dir, RES);
		}
		att = -1;
		/*BASES SNAP TO UPWARDS ATTRACTOR FIRST*/
		if (_y == blockHeight - 1
			&& block[blockWidth][0] == BASE_ATTRACTOR
			&& BIT(rules->mode, WRAP_PLAY))
		{  /*check wrapped*/
			if (att == -1 || dir == DIR_UP)
			{
				att = DIR_UP;
			}
		}
		if (_y < blockHeight - 1 && block[_x][_y + 1] == BASE_ATTRACTOR) {
			if (att == -1 || dir == DIR_UP) {
				att = DIR_UP;
			}
		}
		/*THEN DOWNWARDS ATTRACTORS*/
		if (_y == 0
			&& block[blockWidth][blockHeight-1] == BASE_ATTRACTOR
			&& BIT(rules->mode, WRAP_PLAY))
		{ /*check wrapped*/
			if (att == -1 || dir == DIR_DOWN) {
				att = DIR_DOWN;
			}
		}
		if (_y > 0 && block[_x][_y - 1] == BASE_ATTRACTOR) {
			if (att == -1 || dir == DIR_DOWN) {
				att = DIR_DOWN;
			}
		}
		/*THEN RIGHTWARDS ATTRACTORS*/
		if (_x == blockWidth - 1
			&& block[0][_y] == BASE_ATTRACTOR
			&& BIT(rules->mode, WRAP_PLAY))
		{ /*check wrapped*/
			if (att == -1 || dir == DIR_RIGHT)
			{
				att = DIR_RIGHT;
			}
		}
		if (_x < blockWidth - 1 && block[_x + 1][_y] == BASE_ATTRACTOR) {
			if (att == -1 || dir == DIR_RIGHT) {
				att = DIR_RIGHT;
			}
		}
		/*THEN LEFTWARDS ATTRACTORS*/
		if (_x == 0
			&& block[blockWidth-1][_y] == BASE_ATTRACTOR
			&& BIT(rules->mode, WRAP_PLAY))
		{ /*check wrapped*/
			if (att == -1 || dir == DIR_LEFT) {
				att = DIR_LEFT;
			}
		}
		if (_x > 0 && block[_x - 1][_y] == BASE_ATTRACTOR) {
			if (att == -1 || dir == DIR_LEFT) {
				att = DIR_LEFT;
			}
		}
		if (att != -1) {
			dir = att;
		}
		bases[i].dir = dir;
	}
	for (i = 0; i < blockWidth; i++) {
		int j;
		for (j = 0; j < blockHeight; j++) {
			if (block[i][j] == BASE_ATTRACTOR) {
				block[i][j] = SPACE;
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
/*
 * Return the team that is closest to this position.
 */
unsigned short World::FindClosestTeam(int posx, int posy)
{
	unsigned short team = TEAM_NOT_SET;
	int i;
	DFLOAT closest = FLT_MAX, l;

	for (i = 0; i < numBases; i++) {
		if (bases[i].team == TEAM_NOT_SET)
			continue;

		l = WrapLength((posx - bases[i].pos.x)*BLOCK_SZ,
										(posy - bases[i].pos.y)*BLOCK_SZ);

		if (l < closest) {
			team = bases[i].team;
			closest = l;
		}
	}

	return team;
}


///////////////////////////////////////////////////////////////////////////////
/*
 * Determine the order in which players are placed
 * on starting positions after race mode reset.
 */
void World::FindBaseOrder()
{
	int 				i, j, k, n;
	DFLOAT				cx, cy, dist;

	if (!BIT(rules->mode, TIMING)) {
		baseorders = NULL;
		return;
	}
	if ((n = numBases) <= 0) {
		error("Cannot support race mode in a map without bases");
		exit(-1);
	}

	if ((baseorders = (BaseOrder *)
			malloc(n * sizeof(BaseOrder))) == NULL) {
		error("Out of memory - baseorder");
		exit(-1);
	}

	cx = check[0].x * BLOCK_SZ;
	cy = check[0].y * BLOCK_SZ;
	for (i = 0; i < n; i++) {
		dist = WrapLength(bases[i].pos.x * BLOCK_SZ - cx,
						   bases[i].pos.y * BLOCK_SZ - cy);
		for (j = 0; j < i; j++) {
			if (baseorders[j].dist > dist) {
				break;
			}
		}
		for (k = i - 1; k >= j; k--) {
			baseorders[k + 1] = baseorders[k];
		}
		baseorders[j].base_idx = i;
		baseorders[j].dist = dist;
	}
}


///////////////////////////////////////////////////////////////////////////////
DFLOAT World::WrapFindDir(DFLOAT dx, DFLOAT dy)
{
	dx = WRAP_DX(this, dx);
	dy = WRAP_DY(this, dy);
	return FindDir(dx, dy);
}


///////////////////////////////////////////////////////////////////////////////
DFLOAT World::WrapLength(DFLOAT dx, DFLOAT dy)
{
	dx = WRAP_DX(this, dx);
	dy = WRAP_DY(this, dy);
	return LENGTH(dx, dy);
}

///////////////////////////////////////////////////////////////////////////////
void World::ComputeGlobalGravity()
{
	int 				xi, yi, dx, dy;
	DFLOAT				xforce, yforce, strength;
	double				theta;
	vector				*grav;
		ipos			gravPoint;

		options.gravityPoint->Get(gravPoint);

	if (options.gravityPointSource->GetBool() == false) {
		theta = (options.gravityAngle->GetDouble() * PI) / 180.0;
		xforce = cos(theta) * options.gravity->GetDouble();
		yforce = sin(theta) * options.gravity->GetDouble();
		for (xi=0; xi<blockWidth; xi++) {
			grav = gravity[xi];

			for (yi=0; yi<blockHeight; yi++, grav++) {
				grav->x = xforce;
				grav->y = yforce;
			}
		}
	} else {
		for (xi=0; xi<blockWidth; xi++) {
			grav = gravity[xi];
			dx = (xi - gravPoint.x) * BLOCK_SZ;
			dx = WRAP_DX(this, dx);

			for (yi=0; yi<blockHeight; yi++, grav++) {
				dy = (yi - gravPoint.y) * BLOCK_SZ;
				dy = WRAP_DX(this, dy);

				if (dx == 0 && dy == 0) {
					grav->x = 0.0;
					grav->y = 0.0;
					continue;
				}
				strength = options.gravity->GetDouble() / LENGTH(dx, dy);
				if (options.gravityClockwise->GetBool()) {
					grav->x =  dy * strength;
					grav->y = -dx * strength;
				}
				else if (options.gravityAnticlockwise->GetBool()) {
					grav->x = -dy * strength;
					grav->y =  dx * strength;
				}
				else {
					grav->x =  dx * strength;
					grav->y =  dy * strength;
				}
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
void World::ComputeGravTab(vector grav_tab[gravRange+1][gravRange+1])
{
	int 				x, y;
	double				strength;

	grav_tab[0][0].x = grav_tab[0][0].y = 0;
	for (x = 0; x < gravRange+1; x++) {
		for (y = (x == 0); y < gravRange+1; y++) {
			strength = pow((double)(sqr(x) + sqr(y)), -1.5);
			grav_tab[x][y].x = x * strength;
			grav_tab[x][y].y = y * strength;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
void World::ComputeLocalGravity(void)
{
	int 				xi, yi, g, gx, gy, ax, ay, dx, dy, gtype;
	int 				first_xi, last_xi, first_yi, last_yi, mod_xi, mod_yi;
	int 				min_xi, max_xi, min_yi, max_yi;
	DFLOAT				force, fx, fy;
	vector				*v, *grav, *tab, grav_tab[gravRange+1][gravRange+1];


	ComputeGravTab(grav_tab);

	min_xi = 0;
	max_xi = blockWidth - 1;
	min_yi = 0;
	max_yi = blockHeight - 1;
	if (BIT(rules->mode, WRAP_PLAY)) {
		min_xi -= MIN(gravRange, blockWidth);
		max_xi += MIN(gravRange, blockWidth);
		min_yi -= MIN(gravRange, blockHeight);
		max_yi += MIN(gravRange, blockHeight);
	}
	for (g=0; g<numGravs; g++) {
		gx = gravs[g].pos.x;
		gy = gravs[g].pos.y;
		force = gravs[g].force;

		if ((first_xi = gx - gravRange) < min_xi) {
			first_xi = min_xi;
		}
		if ((last_xi = gx + gravRange) > max_xi) {
			last_xi = max_xi;
		}
		if ((first_yi = gy - gravRange) < min_yi) {
			first_yi = min_yi;
		}
		if ((last_yi = gy + gravRange) > max_yi) {
			last_yi = max_yi;
		}
		gtype = block[gx][gy];
		mod_xi = (first_xi < 0) ? (first_xi + blockWidth) : first_xi;
		dx = gx - first_xi;
		fx = force;
		for (xi = first_xi; xi <= last_xi; xi++, dx--) {
			if (dx < 0) {
				fx = -force;
				ax = -dx;
			} else {
				ax = dx;
			}
			mod_yi = (first_yi < 0) ? (first_yi + blockHeight) : first_yi;
			dy = gy - first_yi;
			grav = &gravity[mod_xi][mod_yi];
			tab = grav_tab[ax];
			fy = force;
			for (yi = first_yi; yi <= last_yi; yi++, dy--) {
				if (dx || dy) {
					if (dy < 0) {
						fy = -force;
						ay = -dy;
					} else {
						ay = dy;
					}
					v = &tab[ay];
					if (gtype == CWISE_GRAV || gtype == ACWISE_GRAV) {
						grav->x -= fy * v->y;
						grav->y += fx * v->x;
					} else if (gtype == UP_GRAV || gtype == DOWN_GRAV) {
						grav->y += force * v->x;
					} else if (gtype == RIGHT_GRAV || gtype == LEFT_GRAV) {
						grav->x += force * v->y;
					} else {
						grav->x += fx * v->x;
						grav->y += fy * v->y;
					}
				}
				else {
					if (gtype == UP_GRAV || gtype == DOWN_GRAV) {
						grav->y += force;
					}
					else if (gtype == LEFT_GRAV || gtype == RIGHT_GRAV) {
						grav->x += force;
					}
				}
				mod_yi++;
				grav++;
				if (mod_yi >= blockHeight) {
					mod_yi = 0;
					grav = gravity[mod_xi];
				}
			}
			if (++mod_xi >= blockWidth) {
				mod_xi = 0;
			}
		}
	}
	/*
	 * We may want to free the World.gravity memory here
	 * as it is not used anywhere else.
	 * e.g.: free(World.gravity);
	 *		 World.gravity = NULL;
	 *		 World.NumGravs = 0;
	 * Some of the more modern maps have quite a few gravity symbols.
	 */
}

///////////////////////////////////////////////////////////////////////////////
void World::ComputeGravity()
{
	ComputeGlobalGravity();
	ComputeLocalGravity();
}

///////////////////////////////////////////////////////////////////////////////
void World::AddTempWormholes(int xin, int yin, int xout, int yout)
{
	Wormhole inhole, outhole, *wwhtemp;

	if ((wwhtemp = (Wormhole *)realloc(wormHoles,
										 (numWormholes + 2)
										 * sizeof(Wormhole)))
		== NULL) {
		error("No memory for temporary wormholes.");
		return;
	}
	wormHoles = wwhtemp;

	inhole.pos.x = xin;
	inhole.pos.y = yin;
	outhole.pos.x = xout;
	outhole.pos.y = yout;
	inhole.countdown = outhole.countdown = options.wormTime->GetInt();
	inhole.lastdest = numWormholes + 1;
	inhole.temporary = outhole.temporary = 1;
	inhole.type = WORM_IN;
	outhole.type = WORM_OUT;
		inhole.lastblock = block[xin][yin];
		outhole.lastblock = block[xout][yout];
		inhole.lastID = itemID[xin][yin];
		outhole.lastID = itemID[xout][yout];
	wormHoles[numWormholes] = inhole;
	wormHoles[numWormholes + 1] = outhole;
	block[xin][yin] = block[xout][yout] = WORMHOLE;
	itemID[xin][yin] = numWormholes;
	itemID[xout][yout] = numWormholes + 1;
	numWormholes += 2;
}

///////////////////////////////////////////////////////////////////////////////
void World::RemoveTempWormhole(int ind)
{
	Wormhole hole;

	hole = wormHoles[ind];
	block[hole.pos.x][hole.pos.y] = hole.lastblock;
	itemID[hole.pos.x][hole.pos.y] = hole.lastID;
	numWormholes--;
	if (ind != numWormholes) {
		wormHoles[ind] = wormHoles[numWormholes];
	}
	wormHoles = (Wormhole *)realloc(wormHoles,
											numWormholes
											* sizeof(Wormhole));
}

