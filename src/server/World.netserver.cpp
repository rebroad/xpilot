/* $Id: World.netserver.cpp,v 1.60 2007/01/10 18:14:47 dick Exp $
 *
 * XPilot, a multiplayer gravity war game.  Copyright (C) 1991-2001 by
 *
 *      Bjørn Stabell        <bjoern@xpilot.org>
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
 *  $Log: World.netserver.cpp,v $
 *  Revision 1.60  2007/01/10 18:14:47  dick
 *  All robot actions are now handled through RobotMan.
 *  There is one RobotMan per World.
 *
 *  Revision 1.59  2004/05/30 16:18:41  dick
 *  Don't xp_strdup to the new connection because that just leaks
 *
 *  Revision 1.58  2004/05/24 07:37:05  dick
 *  Add support for 2 serverMessage, which can displayed on a client when he starts up.
 *
 *  Revision 1.57  2004/05/22 15:12:34  dick
 *  Remove #ifndef SILENT
 *
 *  Revision 1.56  2004/05/20 22:12:38  dick
 *  Attribute showLocks displays in the ScoreTable who everyone is locked on.
 *
 *  Revision 1.55  2004/05/07 04:27:41  dick
 *  _visibility becomes Visibility.  global updateScores becomes a member of World.
 *  Handle rank/rate data from the scoreserver and send it to the client.
 *
 *  Revision 1.54  2004/05/04 21:07:17  dick
 *  Need randommnt.h
 *
 *  Revision 1.53  2004/04/29 16:08:27  dick
 *  compile with SOUND enabled.
 *
 *  Revision 1.52  2004/04/26 07:07:14  dick
 *  Define a list of pages that can be displayed on the ScoreTable in the client.
 *  Each page can be controlled by the server (and the server admin tool).
 *
 *  Revision 1.51  2004/04/23 17:32:04  dick
 *  Retab
 *
 *  Revision 1.50  2004/03/04 03:47:31  dick
 *  Save off the version that the client reported as origVersion.
 *  This is for stats only and doesn't affect gameplay.
 *
 *  Revision 1.49  2004/02/23 02:06:09  dick
 *  robot.h becomes Robot.h
 *
 *  Revision 1.48  2004/02/20 06:27:47  dick
 *  SendPlayMessage becomes BroadcastPlayMessage() because it sends a play message
 *  to all connections.
 *
 *  Revision 1.47  2004/02/18 04:44:11  dick
 *  Use new Connection::InitBuffers() instead of manually setting them up.
 *
 *  Revision 1.46  2004/02/18 01:24:32  dick
 *  Can't pass String through ...
 *
 *  Revision 1.45  2004/02/16 14:29:44  dick
 *  server/sched.cpp and friends become serverlib/NetServer
 *  which disconnects the udp listen and receive functionality from the game.
 *
 *  Revision 1.44  2004/02/06 22:27:36  dick
 *  Use the default MOTD if the user didn't specify one
 *
 *  Revision 1.43  2004/02/03 03:39:54  dick
 *  Add Windows precompiled header support.
 *
 *  Revision 1.42  2004/01/09 17:32:12  dick
 *  Use world's firewallPortList for making UDP connections.
 *  Use String localAddr instead of "127.0.0.1".
 *  localAddr can connect as many IPs as he wants.
 *
 *  Revision 1.41  2004/01/09 05:28:25  dick
 *  When a Connection is destroyed, call World::NumLogoutsBump so meta will be
 *  notified during the loop.  Input becomes a member of World.
 *
 *  Revision 1.40  2004/01/08 18:10:51  dick
 *  Update to XPilot 4.5.5beta
 *
 *  Revision 1.39  2004/01/06 04:48:28  dick
 *  Add support for building under Cygwin.  Still needs work and the output
 *  is X11 and requires something like Cygwin's XWin.exe to run under.
 *
 *  Revision 1.38  2002/09/16 21:51:30  dick
 *  Add some constness to method parameters.
 *
 *  Revision 1.37  2002/09/11 16:42:04  dick
 *  Here endeth the server encapsulation task.
 *  theWorld is now an array of World(s) and get loaded dynamically.
 *
 *  Revision 1.36  2002/09/09 23:42:55  dick
 *  Handle connecting a LocalCtl to this server.
 *
 *  Revision 1.35  2002/09/01 00:13:45  dick
 *  Remove obsolete windows #include "winServer.h"
 *
 *  Revision 1.34  2002/08/01 14:45:14  dick
 *  Whitespace.
 *
 *  Revision 1.33  2002/07/14 19:05:24  dick
 *  When talking to the ScoreServer about PCSTR nick, always mention the PlayerType too.
 *
 *  Revision 1.32  2002/07/12 15:33:30  dick
 *  Track controlsConnected so we can run fast sched() if there is only a
 *  control connected.
 *
 *  Revision 1.31  2002/07/08 05:58:50  dick
 *  BroadcastOption(ServerOption*) sends this option to all controls who care.
 *
 *  Revision 1.30  2002/06/30 22:25:15  dick
 *  Notify meta is a control connects (may be a ScoreControl which changes the
 *  version number)
 *
 *  Revision 1.29  2002/06/30 07:01:22  dick
 *  Send each ConnectionControl the Play messages if he wants them.
 *
 *  Revision 1.28  2002/06/26 07:01:18  dick
 *  Use xperrno instead of errno and seterrno(0) instead of errno = 0.
 *
 *  Revision 1.27  2002/06/23 06:40:24  dick
 *  Connect an incoming ScoreServer to our server
 *
 *  Revision 1.26  2002/06/04 22:20:36  dick
 *  talk_seq_num was not initialized (caused some players to not be able to talk)
 *  Send Eyes status to players entering.
 *
 *  Revision 1.25  2002/06/01 06:29:59  dick
 *  cannon.* becomes Cannon.*
 *
 *  Revision 1.24  2002/06/01 06:06:48  dick
 *  Encapsulate (almost) everything.  Get rid of (almost) all refs to theWorld.
 *
 *  Revision 1.23  2002/05/22 05:09:00  dick
 *  Update to XPilot r4_5_4alpha.
 *
 *  Revision 1.22  2002/05/18 22:34:15  dick
 *  Move showtime to common.
 *
 *  Revision 1.21  2002/05/18 20:55:38  dick
 *  Update to XPilot-4.5.1
 *
 *  Revision 1.20  2001/09/03 00:51:50  dick
 *  Tuners cleanup.
 *
 *  Revision 1.19  2001/08/31 21:05:57  dick
 *  Linux wants me to use 0 instead of NULL
 *
 *  Revision 1.18  2001/08/31 20:58:13  dick
 *  Server needs to be able to receive reliable packets as well as send them.
 *  Use the new emh error handling.
 *
 *  Revision 1.17  2001/08/30 07:16:44  dick
 *  Send and receive options between the server and xpilotedit.
 *
 *  Revision 1.16  2001/08/26 10:34:26  dick
 *  Handle a connection from a control.
 *
 *  Revision 1.15  2001/08/14 21:39:41  dick
 *  Whitespace.
 *
 *  Revision 1.14  2001/08/12 11:19:31  dick
 *  A little bit closer to getting the control to work...
 *
 *  Revision 1.13  2001/08/11 09:38:20  dick
 *  new class ConnectionControl, a Connection for a control.
 *
 *  Revision 1.12  2001/08/10 08:11:37  dick
 *  New class ConnectionPlayer subclasses Connection to provide player
 *  functions.  New class Frame subclasses ConnectionPlayer which provides
 *  Frame update functionality.
 *
 *  Revision 1.11  2001/08/09 07:43:34  dick
 *  New class Connection
 *
 *  Revision 1.10  2001/08/07 20:57:09  dick
 *  sock.error becomes sock.sockError .  error is also a function call and C++
 *  can get confused depending on scope.
 *
 *  Revision 1.9  2001/08/06 05:41:50  dick
 *  struct sock_t becomes class Sock
 *
 *  Revision 1.8  2001/08/01 06:39:41  dick
 *  AFUNC cleanups. address to function...
 *
 *  Revision 1.7  2001/07/30 01:19:55  dick
 *  Encapsulate the last few options (Strings)
 *
 *  Revision 1.6  2001/07/29 23:40:49  dick
 *  Linux gcc tweaks.  The newer versions of g++ has stricter (and different)
 *  requirements.
 *
 *  Revision 1.5  2001/07/29 06:01:47  dick
 *  net.cpp becomes a very nice class Sockbuf.
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
 *  Revision 1.5  2001/07/12 10:01:02  dick
 *  Go_home() becomes GoHome()
 *
 *  Revision 1.4  2001/07/10 08:10:08  dick
 *  Don't need (deleted) map.h because World.h is included in global.h
 *
 *  Revision 1.3  2001/07/08 06:40:14  dick
 *  C++ conversion stuff.
 *
 *  Revision 1.2  2001/07/07 12:00:42  dick
 *  Rename classes to C++ "Style".  old World becomes theWorld.
 *
 */

/*
 * This is the server side of the network connnection stuff.
 *
 * We try very hard to not let the game be disturbed by
 * players logging in.  Therefore a new connection
 * passes through several states before it is actively
 * playing.
 * First we make a new connection structure available
 * with a new socket to listen on.  This socket port
 * number is told to the client via the pack mechanism.
 * In this state the client has to send a packet to this
 * newly created socket with its name and playing parameters.
 * If this succeeds the connection advances to its second state.
 * In this second state the essential server configuration
 * like the map and so on is transmitted to the client.
 * If the client has acknowledged all this data then it
 * advances to the third state, which is the
 * ready-but-not-playing-yet state.  In this state the client
 * has some time to do its final initializations, like mapping
 * its user interface windows and so on.
 * When the client is ready to accept frame updates and process
 * keyboard events then it sends the start-play packet.
 * This play packet advances the connection state into the
 * actively-playing state.  A player structure is allocated and
 * initialized and the other human players are told about this new player.
 * The newly started client is told about the already playing players and
 * play has begun.
 * Apart from these four states there are also two intermediate states.
 * These intermediate states are entered when the previous state
 * has filled the reliable data buffer and the client has not
 * acknowledged all the data yet that is in this reliable data buffer.
 * They are so called output drain states.  Not doing anything else
 * then waiting until the buffer is empty.
 * The difference between these two intermediate states is tricky.
 * The second intermediate state is entered after the
 * ready-but-not-playing-yet state and before the actively-playing state.
 * The difference being that in this second intermediate state the client
 * is already considered an active player by the rest of the server
 * but should not get frame updates yet until it has acknowledged its last
 * reliable data.
 *
 * Communication between the server and the clients is only done
 * using UDP datagrams.  The first client/serverized version of XPilot
 * was using TCP only, but this was too unplayable across the Internet,
 * because TCP is a data stream always sending the next byte.
 * If a packet gets lost then the server has to wait for a
 * timeout before a retransmission can occur.  This is too slow
 * for a real-time program like this game, which is more interested
 * in recent events than in sequenced/reliable events.
 * Therefore UDP is now used which gives more network control to the
 * program.
 * Because some data is considered crucial, like the names of
 * new players and so on, there also had to be a mechanism which
 * enabled reliable data transmission.  Here this is done by creating
 * a data stream which is piggybacked on top of the unreliable data
 * packets.  The client acknowledges this reliable data by sending
 * its byte position in the reliable data stream.  So if the client gets
 * a new reliable data packet and it has not had this data before and
 * there is also no data packet missing inbetween, then it advances
 * its byte position and acknowledges this new position to the server.
 * Otherwise it discards the packet and sends its old byte position
 * to the server meaning that it detected a packet loss.
 * The server maintains an acknowledgement timeout timer for each
 * connection so that it can retransmit a reliable data packet
 * if the acknowledgement timer expires.
 */
#include "StdAfx.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <fcntl.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>

#if defined(_UNIX) || defined(_CYGWIN)
# include <unistd.h>
# include <sys/param.h>
# ifdef _AIX
#  ifndef _BSD_INCLUDES
#   define _BSD_INCLUDES
#  endif
# endif
# ifndef __hpux
#  include <sys/time.h>
# endif
# include <sys/socket.h>
# include <netinet/in.h>
# include <netdb.h>
#else
# include <io.h>
#endif

#include "portability.h"
#include "version.h"
#include "config.h"
#include "serverconst.h"
#include "global.h"
#include "proto.h"
#include "pack.h"
#include "bit.h"
#include "types.h"
#include "Sock.h"
#include "Sockbuf.h"
#include "error.h"
#include "ConnectionPlayer.h"
#include "ConnectionControl.h"
#include "ConnectionControlLocalCtl.h"
#include "ConnectionControlScoreServer.h"
#include "NetServerServer.h"
#include "packet.h"
#include "setup.h"
#include "saudio.h"
#include "checknames.h"
#include "server.h"
#include "commonproto.h"
#include "randommt.h"
#include "showtime.h"
#include "asteroid.h"
#include "score.h"
#include "Cannon.h"
#include "metaserver.h"
#include "RobotMan.h"

char netserver_version[] = VERSION;

setup_t		*Setup = NULL;

const int	compress_maps = 1;


///////////////////////////////////////////////////////////////////////////////
/*
 * Compress the map data using a simple Run Length Encoding algorithm.
 * If there is more than one consecutive byte with the same type
 * then we set the high bit of the byte and then the next byte
 * gives the number of repetitions.
 * This works well for most maps which have lots of series of the
 * same map object and is simple enough to got implemented quickly.
 */
static int Compress_map(unsigned char *map, int size)
{
    int			i, j, k;

    for (i = j = 0; i < size; i++, j++) {
	if (i + 1 < size
	    && map[i] == map[i + 1]) {
	    for (k = 2; i + k < size; k++) {
		if (map[i] != map[i + k]) {
		    break;
		}
		if (k == 255) {
		    break;
		}
	    }
	    map[j] = (map[i] | SETUP_COMPRESSED);
	    map[++j] = k;
	    i += k - 1;
	} else {
	    map[j] = map[i];
	}
    }
    return j;
}

///////////////////////////////////////////////////////////////////////////////
/*
 * Initialize the structure that gives the client information
 * about our setup.  Like the map and playing rules.
 * We only setup this structure once to save time when new
 * players log in during play.
 */
int World::InitSetup()
{
	int 				i, x, y, team, type, size,
						wormhole = 0,
						treasure = 0,
						target = 0,
						base = 0,
						cannon = 0;
	unsigned char		*mapdata, *mapptr;

	if ((mapdata = (unsigned char *) malloc(blockWidth * blockHeight)) == NULL) {
		error("No memory for mapdata");
		return -1;
	}
	memset(mapdata, SETUP_SPACE, blockWidth * blockHeight);
	mapptr = mapdata;
		seterrno(0);
	for (x = 0; x < blockWidth; x++) {
		for (y = 0; y < blockHeight; y++, mapptr++) {
			type = block[x][y];
			switch (type) {
			case ACWISE_GRAV:
			case CWISE_GRAV:
			case POS_GRAV:
			case NEG_GRAV:
			case UP_GRAV:
			case DOWN_GRAV:
			case RIGHT_GRAV:
			case LEFT_GRAV:
				if (!options.gravityVisible->GetBool())
					type = SPACE;
				break;
			case WORMHOLE:
				if (!options.wormholeVisible->GetBool())
					type = SPACE;
				break;
			case ITEM_CONCENTRATOR:
				if (!options.itemConcentratorVisible->GetBool())
					type = SPACE;
				break;
			case ASTEROID_CONCENTRATOR:
				if (!options.asteroidConcentratorVisible->GetBool())
					type = SPACE;
				break;
			case FRICTION:
				if (!options.blockFrictionVisible->GetBool())
					type = SPACE;
				else
					type = DECOR_FILLED;
				break;
			default:
				break;
			}
			switch (type) {
			case SPACE: 		*mapptr = SETUP_SPACE; break;
			case FILLED:		*mapptr = SETUP_FILLED; break;
			case REC_RU:		*mapptr = SETUP_REC_RU; break;
			case REC_RD:		*mapptr = SETUP_REC_RD; break;
			case REC_LU:		*mapptr = SETUP_REC_LU; break;
			case REC_LD:		*mapptr = SETUP_REC_LD; break;
			case FUEL:			*mapptr = SETUP_FUEL; break;
			case ACWISE_GRAV:	*mapptr = SETUP_ACWISE_GRAV; break;
			case CWISE_GRAV:	*mapptr = SETUP_CWISE_GRAV; break;
			case POS_GRAV:		*mapptr = SETUP_POS_GRAV; break;
			case NEG_GRAV:		*mapptr = SETUP_NEG_GRAV; break;
			case UP_GRAV:		*mapptr = SETUP_UP_GRAV; break;
			case DOWN_GRAV: 	*mapptr = SETUP_DOWN_GRAV; break;
			case RIGHT_GRAV:	*mapptr = SETUP_RIGHT_GRAV; break;
			case LEFT_GRAV: 	*mapptr = SETUP_LEFT_GRAV; break;
			case ITEM_CONCENTRATOR: *mapptr = SETUP_ITEM_CONCENTRATOR; break;
				case ASTEROID_CONCENTRATOR: *mapptr = SETUP_ASTEROID_CONCENTRATOR; break;
			case DECOR_FILLED:	*mapptr = SETUP_DECOR_FILLED; break;
			case DECOR_RU:		*mapptr = SETUP_DECOR_RU; break;
			case DECOR_RD:		*mapptr = SETUP_DECOR_RD; break;
			case DECOR_LU:		*mapptr = SETUP_DECOR_LU; break;
			case DECOR_LD:		*mapptr = SETUP_DECOR_LD; break;
			case WORMHOLE:
				switch (wormHoles[wormhole++].type) {
				case WORM_NORMAL: *mapptr = SETUP_WORM_NORMAL; break;
				case WORM_IN:	  *mapptr = SETUP_WORM_IN; break;
				case WORM_OUT:	  *mapptr = SETUP_WORM_OUT; break;
				default:
					error("Bad wormhole (%d,%d).", x, y);
					free(mapdata);
					return -1;
				}
				break;
			case TREASURE:
				*mapptr = SETUP_TREASURE + treasures[treasure++].team;
				break;
			case TARGET:
				*mapptr = SETUP_TARGET + targets[target++].team;
				break;
			case BASE:
				if (bases[base].team == TEAM_NOT_SET) {
					team = 0;
				} else {
					team = bases[base].team;
				}
				switch (bases[base++].dir) {
				case DIR_UP:	*mapptr = SETUP_BASE_UP + team; break;
				case DIR_RIGHT: *mapptr = SETUP_BASE_RIGHT + team; break;
				case DIR_DOWN:	*mapptr = SETUP_BASE_DOWN + team; break;
				case DIR_LEFT:	*mapptr = SETUP_BASE_LEFT + team; break;
				default:
					error("Bad base at (%d,%d).", x, y);
					free(mapdata);
					return -1;
				}
				break;
			case CANNON:
				switch (cannons[cannon++].dir) {
				case DIR_UP:	*mapptr = SETUP_CANNON_UP; break;
				case DIR_RIGHT: *mapptr = SETUP_CANNON_RIGHT; break;
				case DIR_DOWN:	*mapptr = SETUP_CANNON_DOWN; break;
				case DIR_LEFT:	*mapptr = SETUP_CANNON_LEFT; break;
				default:
					error("Bad cannon at (%d,%d).", x, y);
					free(mapdata);
					return -1;
				}
				break;
			case CHECK:
				for (i = 0; i < numChecks; i++) {
					if (x != check[i].x
						|| y != check[i].y) {
						continue;
					}
					*mapptr = SETUP_CHECK + i;
					break;
				}
				if (i >= numChecks) {
					error("Bad checkpoint at (%d,%d).", x, y);
					free(mapdata);
					return -1;
				}
				break;
			default:
				error("Unknown map type (%d) at (%d,%d).", type, x, y);
				*mapptr = SETUP_SPACE;
				break;
			}
		}
	}
	if (compress_maps == 0) {
		type = SETUP_MAP_UNCOMPRESSED;
		size = blockWidth * blockHeight;
	} else {
		type = SETUP_MAP_ORDER_XY;
		size = Compress_map(mapdata, blockWidth * blockHeight);
		if (size <= 0 || size > blockWidth * blockHeight) {
				seterrno(0);
			error("Map compression error (%d)", size);
			free(mapdata);
			return -1;
		}
		if ((mapdata = (unsigned char *)realloc(mapdata, size)) == NULL) {
			error("Cannot reallocate mapdata");
			return -1;
		}
	}

	if (type != SETUP_MAP_UNCOMPRESSED) {
		xpprintf("%sMap compression ratio is %-4.2f%%\n", showtime(),
			100.0 * size / (blockWidth * blockHeight));
	}
	if ((Setup = (setup_t *) malloc(sizeof(setup_t) + size)) == NULL) {
		error("No memory to hold setup");
		free(mapdata);
		return -1;
	}
	memset(Setup, 0, sizeof(setup_t) + size);
	memcpy(Setup->map_data, mapdata, size);
	free(mapdata);
	Setup->setup_size = ((char *) &Setup->map_data[0] - (char *) Setup) + size;
	Setup->map_data_len = size;
	Setup->map_order = type;
	Setup->frames_per_second = GetFPS();
	Setup->lives = rules->lives;
	Setup->mode = rules->mode;
	Setup->x = blockWidth;
	Setup->y = blockHeight;
	strlcpy(Setup->name, options.mapName->GetString(), sizeof(Setup->name));
	strlcpy(Setup->author, options.mapAuthor->GetString(), sizeof(Setup->author));

	return 0;
}



///////////////////////////////////////////////////////////////////////////////
/*
 * A client has requested a playing connection with this server.
 * See if we have room for one more player and if his name is not
 * already in use by some other player.  Because the confirmation
 * may get lost we are willing to send it another time if the
 * client connection is still in the CONN_LISTENING state.
 */
int World::SetupConnectionPlayer(PCSTR real, PCSTR nick, PCSTR dpy, int team,
		     PCSTR addr, PCSTR host, uint version, uint origVersion)
{
	int			i;
	int			free_conn_index = netServer->maxConnections;
	int			my_port;
	Sock		sock;
	ConnectionPlayer*	connp;

	for (i = 0; i < netServer->maxConnections; i++) {
		connp = (ConnectionPlayer*)netServer->conn[i];
		if (!connp) {
			if (free_conn_index == netServer->maxConnections) {
				free_conn_index = i;
			}
			continue;
		}
		if (strcasecmp(connp->nick, nick) == 0) {
			if (connp->state == CONN_LISTENING
				&& strcmp(real, connp->real) == 0
				&& strcmp(dpy, connp->dpy) == 0
				&& version == connp->version) {
				/*
				 * May happen for multi-homed hosts
				 * and if previous packet got lost.
				 */
				return connp->my_port;
			} else {
				/*
				 * Nick already in use.
				 */
				return -1;
			}
		}
	}

    if (free_conn_index >= netServer->maxConnections) {
			xpprintf("%sFull house for %s(%s)@%s(%s)\n",
					 showtime(), real, nick, host, dpy);
		return -1;
    }

	/////////////////////////////////////////////
	// Create the connection
	connp = new ConnectionPlayer;
	connp->world = this;
	connp->SetErrMsgHandler(&World::ErrMsgHandler, this);
	netServer->conn[free_conn_index] = connp;

	if (sock.OpenUdp(serverAddr, firewallPortList) == SOCK_IS_ERROR)
	{
		return(-1);
	}
    if ((my_port = sock.GetPort()) == -1) {
		error("Cannot get port from socket");
		sock.Close();
	return -1;
    }
    if (sock.SetNonBlocking(1) == -1) {
		error("Cannot make client socket non-blocking");
		sock.Close();
	return -1;
    }
    if (sock.SetReceiveBufferSize(SERVER_RECV_SIZE + 256) == -1) {
		error("Cannot set receive buffer size to %d", SERVER_RECV_SIZE + 256);
    }
    if (sock.SetSendBufferSize(SERVER_SEND_SIZE + 256) == -1) {
		error("Cannot set send buffer size to %d", SERVER_SEND_SIZE + 256);
    }

	if (!connp->InitBuffers(&sock))
	{
		error("Not enough memory for connection");
		/* socket is not yet connected, but it doesn't matter much. */
		connp->DestroyConnection("no memory");
		return -1;
	}
	connp->myInd = free_conn_index;
    connp->my_port = my_port;
    connp->real = real;
    connp->nick = nick;
    connp->dpy  = dpy;
    connp->addr = addr;
    connp->host = host;
    connp->ship = NULL;
    connp->team = team;
    connp->version = version;
	connp->origVersion = version;
	connp->talkSequenceNum = 0;

    connp->magic = randomMT() + my_port + sock.fd + team + netServer->mainLoops;


    netServer->InstallInput(&Connection::HandleInput, sock.fd, (void *)connp);

    return my_port;
}

///////////////////////////////////////////////////////////////////////////////
int World::SetupConnectionControl(PCSTR real, PCSTR nick, PCSTR dpy,
								  PCSTR addr, PCSTR host, int* port,
								  uint version, uint origVersion)
{
	int			i;
	int			free_conn_index = netServer->maxConnections;
	int			my_port;
	Sock		sock;
	ConnectionControl*	connp;

	for (i = 0; i < netServer->maxConnections; i++)
	{
		connp = (ConnectionControl*)netServer->conn[i];
		if (!connp)	//  || connp->state == CONN_FREE) 	// DIK: checking _FREE is wrong, since we will leak this conn with the new below
		{
			if (free_conn_index == netServer->maxConnections) 
			{
				free_conn_index = i;
			}
			continue;
		}
	}

	if (free_conn_index >= netServer->maxConnections) {
		xpprintf("%sFull house for %s(%s)@%s\n", showtime(), real, nick, host);
		return E_GAME_FULL;
	}

	/////////////////////////////////////////////
	// Create the connection
	if (!strcmp(dpy, "ScoreServer"))
	{
		if (scoreServer)
		{
			error("Already have a Score Server attached");
			sock.Close();
			return(E_SCOREALREADY);
		}
		scoreServer = new ConnectionControlScoreServer;
		connp = scoreServer;
	}
	else if (!strcmp(dpy, "localctl"))
	{
		if (localAddr != addr)
		{
			xpprintf("%s%s tried to connect a localctl\n", showtime(), addr);
			sock.Close();
			return(E_NOTLOCAL);
		}
		connp = new ConnectionControlLocalCtl;
	}
	else
		connp = new ConnectionControl;
	connp->world = this;
	netServer->conn[free_conn_index] = connp;

	if (sock.OpenUdp(serverAddr, firewallPortList) == SOCK_IS_ERROR)
	{
		// error already reported
		return E_SERVER;
	}

	if ((my_port = sock.GetPort()) == -1) {
	error("Cannot get port from socket");
	sock.Close();
	return E_SERVER;
	}
	if (sock.SetNonBlocking(1) == -1) {
		error("Cannot make client socket non-blocking");
		sock.Close();
		return E_SERVER;
	}
	*port = my_port;

	if (sock.SetReceiveBufferSize(SERVER_RECV_SIZE + 256) == -1) {
		error("Cannot set receive buffer size to %d", SERVER_RECV_SIZE + 256);
	}
	if (sock.SetSendBufferSize(SERVER_SEND_SIZE + 256) == -1) {
		error("Cannot set send buffer size to %d", SERVER_SEND_SIZE + 256);
	}

	if (!connp->InitBuffers(&sock))
	{
		error("Not enough memory for connection");
		/* socket is not yet connected, but it doesn't matter much. */
		connp->DestroyConnection("no memory");
		return -1;
	}
	connp->myInd = free_conn_index;
	connp->my_port = my_port;
	connp->real = real;
	connp->nick = nick;
	connp->dpy  = dpy;
	connp->addr = addr;
	connp->host = host;
	connp->version = version;
	connp->origVersion = version;
	connp->magic = randomMT() + my_port + sock.fd + netServer->mainLoops;
	netServer->InstallInput(&Connection::HandleInput, sock.fd, connp);
	MetaUpdate(this, TRUE);	// tell meta (indirectly) of the new control

	return E_SUCCESS;
}


///////////////////////////////////////////////////////////////////////////////
/*
 * A client has requested to start active play.
 * See if we can allocate a player structure for it
 * and if this succeeds update the player information
 * to all connected players.
 */
int World::HandleLogin(int ind, char *errmsg, int errsize)
{
	ConnectionPlayer*	connp = (ConnectionPlayer*)netServer->conn[ind];
	Player*				pl;
	int 				i,
						war_on_id,
						conn_bit;
	char				msg[MSG_LEN];

	if (numPlayers - numPseudoPlayers >= numBases) {
		seterrno(0);
		strlcpy(errmsg, "Not enough bases for players", errsize);
		error("%s", errmsg);
		return -1;
	}
	if (BIT(rules->mode, TEAM_PLAY)) {
		if (connp->team < 0 || connp->team >= MAX_TEAMS
			|| (options.reserveRobotTeam->GetBool()
						&& (connp->team == options.robotTeam->GetInt()))) {
			connp->team = TEAM_NOT_SET;
		}
		else if (teams[connp->team].numBases <= 0) {
			connp->team = TEAM_NOT_SET;
		}
		else {
			CheckTeamMembers(connp->team);
			if (teams[connp->team].numMembers
				- teams[connp->team].numRobots
				>= teams[connp->team].numBases) {
				connp->team = TEAM_NOT_SET;
			}
		}
		if (connp->team == TEAM_NOT_SET) {
			connp->team = PickTeam(PickForHuman);
			if (connp->team == TEAM_NOT_SET ||
						(connp->team == options.robotTeam->GetInt() 
						  && options.reserveRobotTeam->GetBool())) 
			{
				seterrno(0);
				strlcpy(errmsg, "Can't pick team", errsize);
				error("%s", errmsg);
				return -1;
			}
		}
	} else {
		connp->team = TEAM_NOT_SET;
	}
	for (i = 0; i < numPlayers; i++) {
		if (strcasecmp(players[i]->name, connp->nick) == 0) {
				seterrno(0);
			error("Name already in use %s", (PCSTR)connp->nick);
			strlcpy(errmsg, "Name already in use", errsize);
			return -1;
		}
	}
	if (players[numPlayers]->Init(connp->ship) <= 0) {
		strlcpy(errmsg, "Init_player failed: no free ID", errsize);
		return -1;
	}
	pl = players[numPlayers];
	strlcpy(pl->name, connp->nick, MAX_CHARS);
	strlcpy(pl->realname, connp->real, MAX_CHARS);
	strlcpy(pl->hostname, connp->host, MAX_CHARS);
	pl->isowner = (!strcmp(pl->realname, server.owner) &&
				   localAddr == connp->addr);
	if (connp->team != TEAM_NOT_SET) {
			pl->team = connp->team;
	}
	pl->version = connp->version;

	pl->PickStartpos(true);
	pl->GoHome();
	if (pl->team != TEAM_NOT_SET) {
		teams[pl->team].numMembers++;
		if (options.teamShareScore->GetBool()) {
				if (teams[pl->team].numMembers == 1) {
				/* reset team score on first player */
				teams[pl->team].score = 0;
				}
		}
		TEAM_SCORE(this, pl->team, 0);
	}
	numPlayers++;
	request_ID();
	connp->id = pl->id;
	pl->conn = connp;
	ScoreServerPlayerEvent(pl, Join);
	ScoreServerRequestPlayerScore(pl);

	memset(pl->last_keyv, 0, sizeof(pl->last_keyv));
	memset(pl->prev_keyv, 0, sizeof(pl->prev_keyv));

	connp->SetState(CONN_READY, CONN_PLAYING);

	if (connp->SendReply(PKT_PLAY, PKT_SUCCESS) <= 0) {
				strlcpy(errmsg, "Cannot send play reply", errsize);
				error("%s", errmsg);
		return -1;
	}

	xpprintf("%s%s (%d) starts at startpos %d.\n", showtime(),
			 pl->name, numPlayers, pl->home_base);

	/*
	 * Tell him about scoretable pages available
	 */
	pl->conn->SendScoreTablePages();

	/*
	 * Tell him about himself first.
	 */
	pl->conn->SendPlayer(pl);
	pl->conn->SendScore(pl);
	pl->conn->SendBase(pl->id, pl->home_base);
	if (options.scoreTableRank->GetBool())
		pl->conn->SendScoreTableRank(pl);
	/*
	 * And tell him about all the others.
	 */
	for (i = 0; i < numPlayers - 1; i++) 
	{
		pl->conn->SendPlayer(players[i]);
		pl->conn->SendScore(players[i]);
		if (options.scoreTableRank->GetBool())
			pl->conn->SendScoreTableRank(players[i]);
		if (!IS_TANK_IND(this, i)) 
			pl->conn->SendBase(players[i]->id, players[i]->home_base);
	}
	/*
	 * And about all the teams.
	 */
	if (BIT(rules->mode, TEAM_PLAY))
	{
		for (i=0; i<MAX_TEAMS; i++)
			if (teams[i].numMembers > 0)
				pl->conn->SendTeamScore(i, teams[i].score);
	}
	/*
	 * And tell all the others about him.
	 */
	for (i = 0; i < numPlayers - 1; i++) 
	{
		if (players[i]->conn) 
		{
			players[i]->conn->SendPlayer(pl);
			players[i]->conn->SendScore(pl);
			players[i]->conn->SendBase(pl->id, pl->home_base);
			players[i]->conn->SendEyes(pl->id, NO_ID);
		}
		/*
		 * And tell him about the relationships others have with each other.
		 */
		else if (IS_ROBOT_IND(this, i)) {
			if ((war_on_id = robotMan->WarOnPlayer(i)) != NO_ID)
				pl->conn->SendWar(players[i]->id, war_on_id);
		}
	}
	// Tell him of any lurkers
	for (i = 0; i < numPlayers - 1; i++) 
	{
		if (players[i]->myEyes != NO_ID)
			pl->conn->SendEyes(players[i]->id, players[i]->myEyes);
		else if (!players[i]->IsRobot())
			pl->conn->SendEyes(players[i]->id, players[i]->lock.pl_id);
	}

	if (numPlayers == 1) {
		sprintf(msg, "Welcome to \"%s\", made by %s.",
			(PCSTR)options.mapName->GetString(), (PCSTR)options.mapAuthor->GetString());
	} else if (BIT(rules->mode, TEAM_PLAY)) {
		sprintf(msg, "%s (%s, team %d) has entered \"%s\", made by %s.",
			pl->name, pl->realname, pl->team, 
			(PCSTR)options.mapName->GetString(), (PCSTR)options.mapAuthor->GetString());
	} else {
		sprintf(msg, "%s (%s) has entered \"%s\", made by %s.",
			pl->name, pl->realname, 
			(PCSTR)options.mapName->GetString(), (PCSTR)options.mapAuthor->GetString());
	}
	BroadcastPlayMessage(msg);

	PCSTR serverNotice = "[*Server notice*]";
	if (connp->version < MY_VERSION) {
		sprintf(msg, "Server runs version %s. %s", VERSION, serverNotice);
		pl->SetMessage(msg);
		if (connp->version < 0x4401) {
			sprintf(msg,
				"Your client does not support the fast radar packet. %s",
				serverNotice);
			pl->SetMessage(msg);
		}
		if (connp->version < 0x4400 && options.maxAsteroidDensity->GetDouble() > 0) {
			sprintf(msg,
				"Your client will see the %d asteroids as balls. %s",
				asteroids.max, serverNotice);
			pl->SetMessage(msg);
		}
	}

	if (scoreServer)
	{
		String info = scoreServer->GetWebInfo();
		if (info.GetLength())
		{
			sprintf(msg, "%s [*ScoreServer*]", (PCSTR)info);
					pl->SetMessage(msg);
		}
	}
	if (!options.serverMessage1->GetString().IsEmpty())
		pl->SetMessage(options.serverMessage1->GetString());
	if (!options.serverMessage2->GetString().IsEmpty())
		pl->SetMessage(options.serverMessage2->GetString());
	conn_bit = (1 << ind);
	for (i = 0; i < numCannons; i++) {
		/*
		 * The client assumes at startup that all cannons are active.
		 */
		if (cannons[i].dead_time == 0) {
			SET_BIT(cannons[i].conn_mask, conn_bit);
		} else {
			CLR_BIT(cannons[i].conn_mask, conn_bit);
		}
	}
	for (i = 0; i < numFuels; i++) {
		/*
		 * The client assumes at startup that all fuelstations are filled.
		 */
		if (fuels[i].fuel == MAX_STATION_FUEL) {
			SET_BIT(fuels[i].conn_mask, conn_bit);
		} else {
			CLR_BIT(fuels[i].conn_mask, conn_bit);
		}
	}
	for (i = 0; i < numTargets; i++) {
		/*
		 * The client assumes at startup that all targets are not damaged.
		 */
		if (targets[i].dead_time == 0
			&& targets[i].damage == TARGET_DAMAGE) {
			SET_BIT(targets[i].conn_mask, conn_bit);
			CLR_BIT(targets[i].update_mask, conn_bit);
		} else {
			CLR_BIT(targets[i].conn_mask, conn_bit);
			SET_BIT(targets[i].update_mask, conn_bit);
		}
	}

	SoundPlayerInit(pl);

	SoundPlayAll(this, START_SOUND);

	netServer->numLogins++;

	if (options.resetOnHuman->GetInt() > 0
		&& (numPlayers - numPseudoPlayers - numRobots) <= options.resetOnHuman->GetInt()
		&& !roundDelay) {
		if (BIT(rules->mode, TIMING)) {
			RaceGameOver();
		} else if (BIT(rules->mode, TEAM_PLAY)) {
			TeamGameOver(-1, "");
		} else if (BIT(rules->mode, LIMITED_LIVES)) {
			IndividualGameOver(-1);
		}
	}

	/* if the next round is delayed, delay it again */
	if (roundDelay > 0 || numPlayers == 1) {
		roundDelay = options.roundDelay->GetInt() * GetFPS();
		if (options.maxRoundTime->GetInt() > 0 
			&& options.roundDelay->GetInt() == 0) {
			roundTime = options.maxRoundTime->GetInt() * GetFPS();
		} else {
			roundTime = -1;
		}
		sprintf(msg, "Player entered. Delaying %d seconds until next %s.",
			options.roundDelay->GetInt(), (BIT(rules->mode, TIMING) ?
					 "race" : "round"));
		BroadcastPlayMessage(msg);
	}

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
void World::NumLogoutsBump()
{
	netServer->numLogouts++;
}

///////////////////////////////////////////////////////////////////////////////
/*
 * Return part of the MOTD into buf starting from offset
 * and continuing at most for maxlen bytes.
 * Return the total MOTD size in size_ptr.
 * The return value is the actual amount of MOTD bytes copied
 * or -1 on error.  A value of 0 means EndOfMOTD.
 *
 * The MOTD is completely read into a dynamic buffer.
 * If this MOTD buffer hasn't been accessed for a while
 * then on the next access the MOTD file is checked for changes.
 */
#if defined(_WINDOWS) && !defined(_CYGWIN)
#define	close(__a)	_close(__a)
#endif
int World::GetMotd(char *buf, int offset, int maxlen, int *size_ptr)
{
	static int			motd_size;
	static char 		*motd_buf;
	static long 		motd_loops;
	static time_t		motd_mtime;

	if (size_ptr) {
		*size_ptr = 0;
	}
	if (offset < 0 || maxlen < 0) {
		return -1;
	}

	if (!motd_loops
		|| (motd_loops + MAX_MOTD_LOOPS*GetFPS() < netServer->mainLoops
			&& offset == 0)) {

		int 					fd, size;
		struct stat 			st;

		motd_loops = netServer->mainLoops;

		if ((fd = open(options.motdFileName->GetString(), O_RDONLY)) == -1) 
		{
			if ((fd = open(Conf_servermotdfile(), O_RDONLY)) == -1)
			{
				motd_size = 0;
				return -1;
			}
		}
		if (fstat(fd, &st) == -1 || st.st_size == 0) 
		{
			motd_size = 0;
			close(fd);
			return -1;
		}
		size = st.st_size;
		if (size > MAX_MOTD_SIZE) {
			size = MAX_MOTD_SIZE;
		}
		if (size != motd_size) {
			motd_mtime = 0;
			motd_size = size;
			if (motd_size == 0) {
				close(fd);
				return 0;
			}
			if (motd_buf) {
				free(motd_buf);
			}
			if ((motd_buf = (char *) malloc(size)) == NULL) {
				close(fd);
				return -1;
			}
		}
		if (motd_mtime != st.st_mtime) {
			motd_mtime = st.st_mtime;
			if ((size = read(fd, motd_buf, motd_size)) <= 0) {
				free(motd_buf);
				motd_buf = 0;
				close(fd);
				motd_size = 0;
				return -1;
			}
			motd_size = size;
		}
		close(fd);
	}

	motd_loops = netServer->mainLoops;

	if (size_ptr) {
		*size_ptr = motd_size;
	}
	if (offset + maxlen > motd_size) {
		maxlen = motd_size - offset;
	}
	if (maxlen <= 0) {
		return 0;
	}
	memcpy(buf, motd_buf + offset, maxlen);
	return maxlen;
}

///////////////////////////////////////////////////////////////////////////////
void World::BroadcastPlayMessage(PCSTR message)
{
	Player		*pl;
	int 		i;
	PCSTR		msg;
	char		tmp[MSG_LEN];

	if ((i = strlen(message)) >= MSG_LEN) 
	{
#ifndef SILENT
		seterrno(0);
		error("Max message len exceed (%d,%s)", i, message);
#endif
		strlcpy(tmp, message, MSG_LEN);
		msg = tmp;
	} else 
	{
		msg = message;
	}
	for (i = 0; i < numPlayers; i++) 
	{
		pl = players[i];
		if (pl->conn)
			pl->conn->SendPlayMessage(msg);
	}
	for (i=0; i<netServer->maxConnections; i++)
	{
		Connection* connp = netServer->conn[i];
		if (connp && connp->ctl && ((ConnectionControl*)connp)->wantPlayMessages)
			((ConnectionControl*)connp)->SendPlayMessage(msg);
	}
}

///////////////////////////////////////////////////////////////////////////////
void World::BroadcastOption(ServerOption* so)
{
	int		i;
	for (i=0; i<netServer->maxConnections; i++)
	{
		Connection* connp = netServer->conn[i];
		if (connp && connp->ctl)
			((ConnectionControl*)connp)->SendOption(so);
	}
}

///////////////////////////////////////////////////////////////////////////////
void World::ErrMsgHandler(void* myThis, ErrMsgType emt, PCSTR ctl, ...)
{
	World* w = (World*)myThis;		// unused.
	char szBuffer[512];
	va_list marker;

	// Figure through the extra arguments.
	va_start(marker, ctl);
	vsprintf(szBuffer, ctl, marker);
	va_end(marker);
	xpprintf(szBuffer);

}

///////////////////////////////////////////////////////////////////////////////
bool World::CheckMaxClientsPerIP(PCSTR hostAddr)
{
	int			i;
	int			clients_per_ip = 0;
	Connection*	connp;

	if (options.maxClientsPerIP->GetInt() <= 0)
		return(true);
	if (localAddr == hostAddr)
		return(true);

	for (i = 0; i < netServer->maxConnections; i++) {
		connp = netServer->conn[i];
		if (connp && connp->state != CONN_FREE && !strcasecmp(connp->addr, hostAddr))
			clients_per_ip++;
	}

	if (clients_per_ip >= options.maxClientsPerIP->GetInt())
		return(false);

	return(true);
}

