/* $Id: ConnectionPlayer.cpp,v 1.34 2007/02/17 20:29:18 dick Exp $
 *
 * Describe a server's connection to a player.
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
 *  $Log: ConnectionPlayer.cpp,v $
 *  Revision 1.34  2007/02/17 20:29:18  dick
 *  Wrap Trace(...) debug with D().
 *
 *  Revision 1.33  2007/02/12 07:55:27  dick
 *  Support RobotWatchDeco, which is decorated shapes displayed on the playfield.
 *
 *  Revision 1.32  2007/01/17 09:06:22  dick
 *  Send the RobotWatch packets to the client
 *
 *  Revision 1.31  2004/05/31 23:45:39  dick
 *  Whitespace
 *
 *  Revision 1.30  2004/05/30 16:20:52  dick
 *  Don't check connType because it is totally unused.
 *
 *  Revision 1.29  2004/05/22 15:13:48  dick
 *  LASER_MODIFIERS becomes LASER_MODS
 *
 *  Revision 1.28  2004/05/20 21:45:08  dick
 *  Don't send ScoreTable stuff if he's not ready for it
 *
 *  Revision 1.27  2004/05/16 00:58:45  dick
 *  Comment out debug
 *
 *  Revision 1.26  2004/05/07 05:52:48  dick
 *  Turn off some debug
 *
 *  Revision 1.25  2004/05/07 04:27:41  dick
 *  _visibility becomes Visibility.  global updateScores becomes a member of World.
 *  Handle rank/rate data from the scoreserver and send it to the client.
 *
 *  Revision 1.24  2004/04/29 16:08:27  dick
 *  compile with SOUND enabled.
 *
 *  Revision 1.23  2004/04/26 07:07:14  dick
 *  Define a list of pages that can be displayed on the ScoreTable in the client.
 *  Each page can be controlled by the server (and the server admin tool).
 *
 *  Revision 1.22  2004/02/20 06:27:47  dick
 *  SendPlayMessage becomes BroadcastPlayMessage() because it sends a play message
 *  to all connections.
 *
 *  Revision 1.21  2004/02/18 01:24:57  dick
 *  Don't NULL out Strings
 *
 *  Revision 1.20  2004/02/18 00:58:09  dick
 *  Don't clear my id before calling DeletePlayer
 *
 *  Revision 1.19  2004/02/16 14:29:44  dick
 *  server/sched.cpp and friends become serverlib/NetServer
 *  which disconnects the udp listen and receive functionality from the game.
 *
 *  Revision 1.18  2004/02/15 19:50:42  dick
 *  Retab
 *
 *  Revision 1.17  2004/02/03 03:39:53  dick
 *  Add Windows precompiled header support.
 *
 *  Revision 1.16  2004/01/16 01:35:07  dick
 *  Turn off the "Eyes" indicator on the client when a player unpauses.
 *
 *  Revision 1.15  2002/08/31 23:31:48  dick
 *  User version 0x5000 instead of version++ 1
 *
 *  Revision 1.14  2002/08/01 14:37:58  dick
 *  If the player's cookie has changed (new cookie), then send it to the client.
 *
 *  Revision 1.13  2002/06/30 06:43:24  dick
 *  SendMessage is a bad word.  It's a Windows function that is a macro and gets
 *  translated to SendMessageA or SendMessageW.  Although it works, renaming ours
 *  to SendPlayMessage makes browsing the source code easier.
 *
 *  Revision 1.12  2002/06/26 07:01:18  dick
 *  Use xperrno instead of errno and seterrno(0) instead of errno = 0.
 *
 *  Revision 1.11  2002/06/02 06:15:39  dick
 *  Fix text in error message.
 *
 *  Revision 1.10  2002/06/01 06:06:47  dick
 *  Encapsulate (almost) everything.  Get rid of (almost) all refs to theWorld.
 *
 *  Revision 1.9  2002/05/20 02:17:45  dick
 *  Windows needs errno and seterrno in portability.h.
 *  Windows also needs rint() defined if not using fltk.
 *
 *  Revision 1.8  2002/05/18 22:51:59  dick
 *  Linux cleanups
 *
 *  Revision 1.7  2002/05/18 20:55:38  dick
 *  Update to XPilot-4.5.1
 *
 *  Revision 1.6  2001/08/31 20:58:13  dick
 *  Server needs to be able to receive reliable packets as well as send them.
 *  Use the new emh error handling.
 *
 *  Revision 1.5  2001/08/14 21:51:35  dick
 *  New functionality !anonymousViewing.  When you are viewing someone, everyone
 *  gets notified.  This appears in the score table as if you have declared war
 *  on the player.  Older clients actually SendWar().  New clients reuse PKT_EYES
 *  which was previously defined as a server SendEyes() and client HandleEyes(),
 *  neither of which were actually ever used.
 *
 *  Revision 1.4  2001/08/11 00:36:52  dick
 *  frame.cpp becomes Frame.cpp.  object.h becomes Object.h
 *
 *  Revision 1.3  2001/08/11 00:22:24  dick
 *  Fix shots and debris.
 *
 *  Revision 1.2  2001/08/10 08:19:38  dick
 *  Linux cleanups
 *
 *  Revision 1.1  2001/08/10 08:11:37  dick
 *  New class ConnectionPlayer subclasses Connection to provide player
 *  functions.  New class Frame subclasses ConnectionPlayer which provides
 *  Frame update functionality.
 *
 */
#include "StdAfx.h"

#ifdef	_UNIX
#include <stdlib.h>
#endif

#include <ctype.h>
#include <errno.h>
#include <string.h>

#include "config.h"
#include "packet.h"
#include "pack.h"
#include "item.h"
#include "global.h"
#include "error.h"
#include "Player.h"
#include "ConnectionPlayer.h"
#include "setup.h"
#include "NetServer.h"
#include "asteroid.h"
#include "proto.h"
#include "commonproto.h"
#include "saudio.h"
#include "portability.h"
#include "Cannon.h"
#include "ScoreTable.h"
#include "World.h"
#include "Robot.h"

///////////////////////////////////////////////////////////////////////////////
ConnectionPlayer::ConnectionPlayer()
{
	world = NULL;
	lastKeyChange = 0;
	motdOffset = -1;
	motdStop = 0;
	viewWidth = DEF_VIEW_SIZE;
	viewHeight = DEF_VIEW_SIZE;
	debris_colors = 0;
	spark_rand = DEF_SPARK_RAND;
	ship = NULL;
}

///////////////////////////////////////////////////////////////////////////////
void ConnectionPlayer::DestroyConnection(PCSTR reason)
{
	if (world)
		world->NumLogoutsBump();
		
	if (id != NO_ID) {
		DeletePlayer(world, world->getInd[id]);
	}

	ConnectionPlayerSUPERCLASS::DestroyConnection(reason);
}

///////////////////////////////////////////////////////////////////////////////
/*
 * Handle a connection that is in the transmit-server-configuration-data state.
 */
int ConnectionPlayer::HandleSetup()
{
	char				*buf;
	int 				n,
						len;

	if (state != CONN_SETUP) {
		DestroyConnection("not setup");
		return -1;
	}

	if (setup == 0) {
		n = cw.printf(
						  "%ld" "%ld%hd" "%hd%hd" "%hd%hd" "%s%s",
						  Setup->map_data_len,
						  Setup->mode, Setup->lives,
						  Setup->x, Setup->y,
						  Setup->frames_per_second, Setup->map_order,
						  Setup->name, Setup->author);
		if (n <= 0) {
			DestroyConnection("setup 0 write error");
			return -1;
		}
		setup = (char *) &Setup->map_data[0] - (char *) Setup;
	}
	else if (setup < Setup->setup_size) {
		if (cw.len > 0) {
			/* If there is still unacked reliable data test for acks. */
			HandleInput(-1, (void *) this);
			if (state == CONN_FREE) {
				return -1;
			}
		}
	}
	if (setup < Setup->setup_size) {
		len = MIN(cw.size, 4096) - cw.len;
		if (len <= 0) {
			/* Wait for acknowledgement of previously transmitted data. */
			return 0;
		}
		if (len > Setup->setup_size - setup) {
			len = Setup->setup_size - setup;
		}
		buf = (char *) Setup;
		if (cw.Write(&buf[setup], len) != len) {
			DestroyConnection("sockbuf write setup error");
			return -1;
		}
		setup += len;
		if (len >= 512) {
			start += (len * world->GetFPS()) / (8 * 512) + 1;
		}
	}
	if (setup >= Setup->setup_size) {
		SetState(CONN_DRAIN, CONN_LOGIN);
	}

	return 0;
}


///////////////////////////////////////////////////////////////////////////////
static int str2num (char **strp, int min, int max)
{
	char *str = *strp;
	int num = 0;

	while (isdigit(*str)) {
		num *= 10;
		num += *str++ - '0';
	}
	*strp = str;
	if (num < min || num > max)
		return min;
	return num;
}


///////////////////////////////////////////////////////////////////////////////
int ConnectionPlayer::SendModifiers(char *mods)
{
	return w.printf("%c%s", PKT_MODIFIERS, mods);
}

///////////////////////////////////////////////////////////////////////////////
/*
 * Send items.
 * The advantage of this scheme is that it only uses bytes for items
 * which the player actually owns.	This reduces the packet size.
 * Another advantage is that here it doesn't matter if an old client
 * receives counts for items it doesn't know about.
 * This is new since pack version 4203.
 */
int ConnectionPlayer::SendSelfItems(Player *pl)
{
	unsigned			item_mask = 0;
	int 				i, n;
	int 				item_count = 0;

	/* older clients should have the items sent as part of the self packet. */
	if (version < 0x4203) {
		return 1;
	}
	/* build mask with one bit for each item type which the player owns. */
	for (i = 0; i < NUM_ITEMS; i++) {
		if (pl->item[i] > 0) {
			item_mask |= (1 << i);
			item_count++;
		}
	}
	/* don't send anything if there are no items. */
	if (item_count == 0) {
		return 1;
	}
	/* check if enough buffer space is available for the complete packet. */
	if (w.size - w.len <= 5 + item_count) {
		return 0;
	}
	/* build the header. */
	n = w.printf("%c%u", PKT_SELF_ITEMS, item_mask);
	if (n <= 0) {
		return n;
	}
	/* build rest of packet containing the per item counts. */
	for (i = 0; i < NUM_ITEMS; i++) {
		if (item_mask & (1 << i)) {
			w.buf[w.len++] = pl->item[i];
		}
	}
	/* return the number of bytes added to the packet. */
	return 5 + item_count;
}

///////////////////////////////////////////////////////////////////////////////
/*
 * Send all frame data related to the player self and his HUD.
 */
int ConnectionPlayer::SendSelf(Player *pl, int lock_id, int lock_dist,
			  int lock_dir, int autopilotlight, long status, char *mods)
{
	int 				n;
	u_byte				stat = (u_byte)status;
	int 				sbuf_len = w.len;

	if (version >= 0x4203) {
		n = w.printf(
			  "%c"
			  "%hd%hd%hd%hd%c"
			  "%c%c%c"
			  "%hd%hd%c%c"
			  "%c%hd%hd"
			  "%hd%hd%c"
			  "%c%c"
			  ,
			  PKT_SELF,
			  (int) (pl->pos.x + 0.5), (int) (pl->pos.y + 0.5),
			  (int) pl->vel.x, (int) pl->vel.y,
			  pl->dir,
			  (int) (pl->power + 0.5),
			  (int) (pl->turnspeed + 0.5),
			  (int) (pl->turnresistance * 255.0 + 0.5),
			  lock_id, lock_dist, lock_dir,
			  pl->check,

			  pl->fuel.current,
			  pl->fuel.sum >> FUEL_SCALE_BITS,
			  pl->fuel.max >> FUEL_SCALE_BITS,

			  viewWidth, viewHeight,
			  debris_colors,

			  stat,
			  autopilotlight

			  );
		if (n <= 0) {
			return n;
		}
		n = SendSelfItems(pl);
		if (n <= 0) {
			return n;
		}
		return SendModifiers(mods);
	}

	n = w.printf(
		  "%c"
		  "%hd%hd%hd%hd%c"
		  "%c%c%c"
		  "%hd%hd%c%c"
		  "%c%c%c%c%c"
		  "%c%c%c%c%c"
		  "%c%c%c%c"
		  "%c%hd%hd"
		  "%hd%hd%c"
		  "%c%c"
		  ,
		  PKT_SELF,
		  (int) (pl->pos.x + 0.5), (int) (pl->pos.y + 0.5),
		  (int) pl->vel.x, (int) pl->vel.y,
		  pl->dir,
		  (int) (pl->power + 0.5),
		  (int) (pl->turnspeed + 0.5),
		  (int) (pl->turnresistance * 255.0 + 0.5),
		  lock_id, lock_dist, lock_dir,
		  pl->check,

		  pl->item[ITEM_CLOAK],
		  pl->item[ITEM_SENSOR],
		  pl->item[ITEM_MINE],
		  pl->item[ITEM_MISSILE],
		  pl->item[ITEM_ECM],

		  pl->item[ITEM_TRANSPORTER],
		  pl->item[ITEM_WIDEANGLE],
		  pl->item[ITEM_REARSHOT],
		  pl->item[ITEM_AFTERBURNER],
		  pl->fuel.num_tanks,

		  pl->item[ITEM_LASER],
		  pl->item[ITEM_EMERGENCY_THRUST],
		  pl->item[ITEM_TRACTOR_BEAM],
		  pl->item[ITEM_AUTOPILOT],

		  pl->fuel.current,
		  pl->fuel.sum >> FUEL_SCALE_BITS,
		  pl->fuel.max >> FUEL_SCALE_BITS,

		  viewWidth, viewHeight,
		  debris_colors,

		  stat,
		  autopilotlight

		  );
	if (n <= 0) {
		return n;
	}
	if (version >= 0x3800) {
		n = w.printf(
			  "%c%c%c%c",	/* %c", */
			  pl->item[ITEM_EMERGENCY_SHIELD],
			  pl->item[ITEM_DEFLECTOR],
			  pl->item[ITEM_HYPERJUMP],
			  pl->item[ITEM_PHASING]
			  );
		if (n <= 0) {
			w.len = sbuf_len;
			return n;
		}
		if (version >= 0x4100) {
			n = w.printf(
				  "%c",
				  pl->item[ITEM_MIRROR]
				  );
			if (n <= 0) {
				w.len = sbuf_len;
				return n;
			}
			if (version >= 0x4201) {
				n = w.printf(
					  "%c",
					  pl->item[ITEM_ARMOR]
					  );
				if (n <= 0) {
					w.len = sbuf_len;
					return n;
				}
			}
		}
	}
	else if (version >= 0x3200) {
		n = w.printf(
			  "%c",
			  pl->item[ITEM_EMERGENCY_SHIELD]);
		if (n <= 0) {
			w.len = sbuf_len;
			return n;
		}
	}
	return SendModifiers(mods);
}

///////////////////////////////////////////////////////////////////////////////
/*
 * Somebody is leaving the game.
 */
int ConnectionPlayer::SendLeave(int id)
{

	if (!BIT(state, CONN_PLAYING | CONN_READY)) 
	{
		seterrno(0);
		emh(emhThis, EmError, "Connection not ready for leave info (%d,%d)",
				  state, id);
		return 0;
	}
	return cw.printf("%c%hd", PKT_LEAVE, id);
}

///////////////////////////////////////////////////////////////////////////////
/*
 * Somebody is declaring war.
 */
int ConnectionPlayer::SendWar(int robot_id, int killer_id)
{

	if (!BIT(state, CONN_PLAYING | CONN_READY)) 
	{
		seterrno(0);
		emh(emhThis, EmError, "Connection not ready for war declaration (%d,%d,%d)",
				  myInd, state, id);
		return 0;
	}
	return cw.printf("%c%hd%hd", PKT_WAR, robot_id, killer_id);
}

///////////////////////////////////////////////////////////////////////////////
/*
 * Somebody is snooping/watching another player.
 */
int ConnectionPlayer::SendEyes(int watcherId, int watchedId)
{
	if (!BIT(state, CONN_PLAYING | CONN_READY)) 
	{
		seterrno(0);
		emh(emhThis, EmError, "Connection not ready for SendEyes (%d,%d,%d)",
				  myInd, state, id);
		return 0;
	}
	if (version >= 0x5000)
		return cw.printf("%c%hd%hd", PKT_EYES, watcherId, watchedId);
	else
		return(SendWar(watcherId, watchedId));
}

///////////////////////////////////////////////////////////////////////////////
/*
 * Somebody is programming a robot to seek some player.
 */
int ConnectionPlayer::SendSeek(int programmer_id, int robot_id, int sought_id)
{

	if (!BIT(state, CONN_PLAYING | CONN_READY))
	{
		seterrno(0);
		emh(emhThis, EmError, "Connection not ready for seek declaration (%d,%d,%d)",
				  myInd, state, id);
		return 0;
	}
	return cw.printf("%c%hd%hd%hd", PKT_SEEK,
						 programmer_id, robot_id, sought_id);
}

///////////////////////////////////////////////////////////////////////////////
/*
 * Somebody is joining the game.
 */
int ConnectionPlayer::SendPlayer(Player* pl)
{
	//Player			*pl = world->players[GetInd[id]];
	int 				n;
	char				buf[MSG_LEN], ext[MSG_LEN];
	int 				sbuf_len = cw.len;

	if (!BIT(state, CONN_PLAYING|CONN_READY)) {
		seterrno(0);
		emh(emhThis, EmError, "Connection not ready for player info (%d,%d)",
			  state, id);
		return 0;
	}
	Convert_ship_2_string(pl->ship, buf, ext,
						  (version < 0x3200) ? 0x3100 : 0x3200);
	n = cw.printf(
			  "%c%hd" "%c%c" "%s%s%s" "%S",
			  PKT_PLAYER, pl->id,
			  pl->team, pl->mychar,
			  pl->name, pl->realname, pl->hostname,
			  buf);
	if (version > 0x3200) {
		if (n > 0) {
			n = cw.printf("%S", ext);
			if (n <= 0) {
				cw.len = sbuf_len;
			}
		}
	}
	if (SendScoreTableIPVer(pl) < 0)
		return(-1);
	return n;
}


///////////////////////////////////////////////////////////////////////////////
/*
 * Send a players ip/version string to a client
 */
int ConnectionPlayer::SendScoreTableIPVer(Player* pl)
{
	if (!BIT(state, CONN_PLAYING|CONN_READY)) {
		seterrno(0);
		emh(emhThis, EmError, "Connection not ready for player info (%d,%d)",
			  state, id);
		return 0;
	}
	if (version < 0x5000)
		return(0);
	String	s;
	if (pl->conn)
		s.printf("%s:%04X", (PCSTR)pl->conn->addr, pl->conn->origVersion);
	else
		s.printf("0.0.0.0:%04X", MY_VERSION);	// builtins run our version
//	if (pl->conn && pl->version != pl->conn->origVersion)
//	{
//		String t;
//		t.printf("(%04X)", pl->conn->version);
//		s += t;
//	}
	return(cw.printf("%c%c%hd%s", PKT_SCORE_TABLE, STIPVer, pl->id, (PCSTR)s));
}


///////////////////////////////////////////////////////////////////////////////
/*
 * Send a players rank and rate to a client
 */
int ConnectionPlayer::SendScoreTableRank(Player* pl)
{
	if (!BIT(state, CONN_PLAYING|CONN_READY)) {
		seterrno(0);
		emh(emhThis, EmError, "Connection not ready for player info (%d,%d)",
			  state, id);
		return 0;
	}
	if (version < 0x5000)
		return(0);
	// D(xpprintf("SendScoreTableRank: %s %d %d\n", pl->name, pl->rank, (int)(pl->rate*100.0));)
	return(cw.printf("%c%c%hd%d%d", PKT_SCORE_TABLE, STRank, 
									pl->id, pl->rank, (int)(pl->rate*100.0)));
}

///////////////////////////////////////////////////////////////////////////////
/*
 * Send the new score for some player to a client.
 */
//int ConnectionPlayer::SendScore(int id, DFLOAT score, int life, int mychar, int alliance)
int ConnectionPlayer::SendScore(Player* pl)
{

	if (!BIT(state, CONN_PLAYING | CONN_READY)) {
		seterrno(0);
		emh(emhThis, EmError, "Connection not ready for score(%d,%d)",
				state, id);
		return 0;
	}
	if (version < 0x4500) {
		/* older clients don't get alliance info or decimals of the score */
		return(cw.printf("%c%hd%hd%hd%c", PKT_SCORE,
				pl->id, (int)(pl->score + (pl->score > 0 ? 0.5 : -0.5)),
				pl->life, pl->mychar));
	} else {
		int allchar = ' ';
		if (pl->alliance != ALLIANCE_NOT_SET) {
			if (world->options.announceAlliances->GetBool()) {
					allchar = pl->alliance + '0';
			} else {
					if (GetPlayer()->alliance == pl->alliance)
							allchar = '+';
			}
		}
		if (version < 0x5000)
			return cw.printf("%c%hd%d%hd%c%c", PKT_SCORE, pl->id, 
											 (int)(pl->score * 100 + (pl->score > 0 ? 0.5 : 0.5)),
											 pl->life, pl->mychar, allchar);
		else
			return cw.printf("%c%hd%d%hd%c%c%hd%hd", PKT_SCORE, pl->id, 
											 (int)(pl->score * 100 + (pl->score > 0 ? 0.5 : 0.5)),
											 pl->life, pl->mychar, allchar,
											 pl->kills, pl->deaths);
	}
}

///////////////////////////////////////////////////////////////////////////////
/*
 * Send the new score for some team to a client
 */
int ConnectionPlayer::SendTeamScore(int team, DFLOAT score)
{
	if (!BIT(state, CONN_PLAYING | CONN_READY))
	{
		seterrno(0);
		emh(emhThis, EmError, "Connection not ready for team score(%d,%d)",
				state, id);
		return 0;
	}
	if (version < 0x4500) {
		/* older clients don't know about team scores */
		return(0);
	}
	return(cw.printf("%c%hd%d", PKT_TEAM_SCORE,
				team, (int)(score * 100 + (score > 0 ? 0.5 : -0.5))));
}

///////////////////////////////////////////////////////////////////////////////
/*
 * Send the info for available ScoreTable pages
 */
int ConnectionPlayer::SendScoreTablePages()
{
	if (version < 0x5000)
		return(0);

	int	f = STScoreB | STNameB;	// These classic pages always available
	if (world->options.scoreTableKills->GetBool())
		f |= STKillsB;
	if (world->options.scoreTableIPVer->GetBool())
		f |= STIPVerB;
	if (world->options.scoreTableRank->GetBool())
		f |= STRankB;
	return(cw.printf("%c%c%hd", PKT_SCORE_TABLE, STPages, f));
}
///////////////////////////////////////////////////////////////////////////////
/*
 * Send the new race info for some player to a client.
 */
int ConnectionPlayer::SendTiming(int id, int check, int round)
{

	if (version < 0x3261) {
		return 1;
	}
	if (!BIT(state, CONN_PLAYING | CONN_READY)) {
		seterrno(0);
		emh(emhThis, EmError, "Connection not ready for timing(%d,%d)",
				  state, id);
		return 0;
	}
	return cw.printf("%c%hd%hu", PKT_TIMING,
						 id, round * MAX_CHECKS + check);
}

///////////////////////////////////////////////////////////////////////////////
/*
 * Send info about a player having which base.
 */
int ConnectionPlayer::SendBase(int id, int num)
{

	if (!BIT(state, CONN_PLAYING | CONN_READY)) {
		seterrno(0);
		emh(emhThis, EmError, "Connection not ready for base info (%d,%d)",
				state, id);
		return 0;
	}
	return cw.printf("%c%hd%hu", PKT_BASE, id, num);
}

///////////////////////////////////////////////////////////////////////////////
/*
 * Send the amount of fuel in a fuelstation.
 */
int ConnectionPlayer::SendFuel(int num, int fuel)
{
	return w.printf("%c%hu%hu", PKT_FUEL,
						 num, fuel >> FUEL_SCALE_BITS);
}

///////////////////////////////////////////////////////////////////////////////
int ConnectionPlayer::SendScoreObject(DFLOAT score, int x, int y, const char *string)
{

	if (!BIT(state, CONN_PLAYING | CONN_READY)) {
		seterrno(0);
		emh(emhThis, EmError, "Connection not ready for score info (%d,%d)",
				state, id);
		return 0;
	}
	if (version < 0x4500) {
		/* older clients don't get decimals of the score */
		return cw.printf("%c%hd%hu%hu%s",PKT_SCORE_OBJECT, 
				(int)(score + (score > 0 ? 0.5 : -0.5)),
				x, y, string);
	} else {
		return(cw.printf("%c%d%hu%hu%s", PKT_SCORE_OBJECT,
				(int)(score * 100 + (score > 0 ? 0.5 : -0.5)),
				x, y, string));
	}
}

///////////////////////////////////////////////////////////////////////////////
int ConnectionPlayer::SendCannon(int num, int dead_time)
{
	return w.printf("%c%hu%hu", PKT_CANNON, num, dead_time);
}

///////////////////////////////////////////////////////////////////////////////
int ConnectionPlayer::SendDestruct(int count)
{
	return w.printf("%c%hd", PKT_DESTRUCT, count);
}

///////////////////////////////////////////////////////////////////////////////
int ConnectionPlayer::SendThrustTime(int count, int max)
{
	return w.printf("%c%hd%hd", PKT_THRUSTTIME, count, max);
}

///////////////////////////////////////////////////////////////////////////////
int ConnectionPlayer::SendShieldTime(int count, int max)
{
	if (version < 0x3200) {
		return 1;
	}
	return w.printf("%c%hd%hd", PKT_SHIELDTIME, count, max);
}

///////////////////////////////////////////////////////////////////////////////
int ConnectionPlayer::SendPhasingTime(int count, int max)
{
	if (version < 0x3800) {
		return 1;
	}
	return w.printf("%c%hd%hd", PKT_PHASINGTIME, count, max);
}

///////////////////////////////////////////////////////////////////////////////
int ConnectionPlayer::SendRoundDelay(int count, int max)
{
	if (version < 0x3800) {
		return 1;
	}
	return(w.printf("%c%hd%hd", PKT_ROUNDDELAY, count, max));
}

///////////////////////////////////////////////////////////////////////////////
int ConnectionPlayer::SendDebris(int type, unsigned char *p, int n)
{
	int 				avail;
//	  Sockbuf*	w = &Conn[ind].w;

	if ((n & 0xFF) != n) {
				seterrno(0);
				emh(emhThis, EmError, "Bad number of debris %d", n);
				return 0;
	}
	avail = w.size - w.len - SOCKBUF_WRITE_SPARE - 2;
	if (n * 2 >= avail) {
		if (avail > 2) {
			n = (avail - 1) / 2;
		} else {
			return 0;
		}
	}
	w.buf[w.len++] = PKT_DEBRIS + type;
	w.buf[w.len++] = n;
	memcpy(&w.buf[w.len], p, n * 2);
	w.len += n * 2;

	return n;
}

///////////////////////////////////////////////////////////////////////////////
int ConnectionPlayer::SendWreckage(int x, int y, u_byte wrtype, u_byte size, u_byte rot)
{
	if (version < 0x3800) {
		return 1;
	}

	if (world->options.wreckageCollisionMayKill->GetBool()
				&& version > 0x4201) {
		/* Set the highest bit when wreckage is deadly. */
		wrtype |= 0x80;
	} else {
		wrtype &= ~0x80;
	}

	return w.printf("%c%hd%hd%c%c%c", PKT_WRECKAGE,
						 x, y, wrtype, size, rot);
}

///////////////////////////////////////////////////////////////////////////////
int ConnectionPlayer::SendAsteroid(int x, int y, u_byte type, u_byte size, u_byte rot)
{
	u_byte		type_size;

	if (version < 0x4400) {
		return SendEcm(x, y, 2 * (int) ASTEROID_RADIUS(size));
	}

	type_size = ((type & 0x0F) << 4) | (size & 0x0F);

	return w.printf("%c%hd%hd%c%c", PKT_ASTEROID,
						 x, y, type_size, rot);
}

///////////////////////////////////////////////////////////////////////////////
int ConnectionPlayer::SendFastshot(int type, unsigned char *p, int n)
{
	int 				avail;
	// Sockbuf* w = &Conn[ind].w;

	if ((n & 0xFF) != n) {
		seterrno(0);
		error("Bad number of fastshot %d", n);
		return 0;
	}
	avail = w.size - w.len - SOCKBUF_WRITE_SPARE - 3;
	if (n * 2 >= avail) {
		if (avail > 2) {
			n = (avail - 1) / 2;
		} else {
			return 0;
		}
	}
	w.buf[w.len++] = PKT_FASTSHOT;
	w.buf[w.len++] = type;
	w.buf[w.len++] = n;
	memcpy(&w.buf[w.len], p, n * 2);
	w.len += n * 2;

	return n;
}

///////////////////////////////////////////////////////////////////////////////
int ConnectionPlayer::SendMissile(int x, int y, int len, int dir)
{
	return w.printf("%c%hd%hd%c%c", PKT_MISSILE, x, y, len, dir);
}

///////////////////////////////////////////////////////////////////////////////
int ConnectionPlayer::SendBall(int x, int y, int id)
{
	return w.printf("%c%hd%hd%hd", PKT_BALL, x, y, id);
}

///////////////////////////////////////////////////////////////////////////////
int ConnectionPlayer::SendMine(int x, int y, int teammine, int id)
{
	return w.printf("%c%hd%hd%c%hd", PKT_MINE, x, y, teammine, id);
}

///////////////////////////////////////////////////////////////////////////////
int ConnectionPlayer::SendTarget(int num, int dead_time, int damage)
{
	return w.printf("%c%hu%hu%hu", PKT_TARGET, num, dead_time, damage);
}

///////////////////////////////////////////////////////////////////////////////
int ConnectionPlayer::SendWormhole(int x, int y)
{
	if (version < 0x4501) {
		const int wormStep = 5;
		int wormAngle = (frame_loops & 7) * (RES / 8);

		return(SendEcm(x, y,  BLOCK_SZ - 2)
			+  SendEcm((int)(x + wormStep * tcos(wormAngle)),
							   (int)(y + wormStep * tsin(wormAngle)),
							   BLOCK_SZ - 2 - 2 * wormStep)
			+  SendEcm((int)(x + 2 * wormStep * tcos(wormAngle)),
							   (int)(y + 2 * wormStep * tsin(wormAngle)),
							   BLOCK_SZ - 2 - 4 * wormStep));
	}
	return(w.printf("%c%hd%hd", PKT_WORMHOLE, x, y));
}

///////////////////////////////////////////////////////////////////////////////
int ConnectionPlayer::SendItem(int x, int y, int type)
{
	if (type >= ITEM_EMERGENCY_SHIELD) {
		if (version < 0x3200) {
			return 1;
		}
	}
	return w.printf("%c%hd%hd%c", PKT_ITEM, x, y, type);
}

///////////////////////////////////////////////////////////////////////////////
int ConnectionPlayer::SendPaused(int x, int y, int count)
{
	return w.printf("%c%hd%hd%hd", PKT_PAUSED, x, y, count);
}

///////////////////////////////////////////////////////////////////////////////
int ConnectionPlayer::SendEcm(int x, int y, int size)
{
	D(Trace("Ecm: %d/%d\n", x, y);)
	return w.printf("%c%hd%hd%hd", PKT_ECM, x, y, size);
}

///////////////////////////////////////////////////////////////////////////////
int ConnectionPlayer::SendTrans(int x1, int y1, int x2, int y2)
{
	return w.printf("%c%hd%hd%hd%hd", PKT_TRANS, x1, y1, x2, y2);
}

///////////////////////////////////////////////////////////////////////////////
int ConnectionPlayer::SendShip(int x, int y, int id, int dir,
			  int shield, int cloak, int emergency_shield, int phased, int deflector)
{
	if (version < 0x4300) {
		/* cloaking bit was also true if phased and that was used
		 * to determine how to draw the ship.
		 */
		cloak = (cloak || phased);
	}
	return w.printf(
			 "%c%hd%hd%hd" "%c" "%c",
			 PKT_SHIP, x, y, id,
			 dir,
			 (shield != 0) 
					| ((cloak != 0) << 1)
					| ((emergency_shield != 0) << 2)
					| ((phased != 0) << 3)			/* clients older than 3.8.0 will ignore this */
					| ((deflector != 0) << 4)		/* clients older than 3.8.0 will ignore this */
			);
}

///////////////////////////////////////////////////////////////////////////////
int ConnectionPlayer::SendRefuel(int x0, int y0, int x1, int y1)
{
	return w.printf("%c%hd%hd%hd%hd", PKT_REFUEL, x0, y0, x1, y1);
}

///////////////////////////////////////////////////////////////////////////////
int ConnectionPlayer::SendConnector(int x0, int y0, int x1, int y1, int tractor)
{
	return w.printf("%c%hd%hd%hd%hd%c", PKT_CONNECTOR, x0, y0, x1, y1, tractor);
}

///////////////////////////////////////////////////////////////////////////////
int ConnectionPlayer::SendLaser(int color, int x, int y, int len, int dir)
{
	return w.printf("%c%c%hd%hd%hd%c", PKT_LASER, color, x, y, len, dir);
}

///////////////////////////////////////////////////////////////////////////////
int ConnectionPlayer::SendRadar(int x, int y, int size)
{

	/* Only since 4.2.1 can clients correctly handle teammates in green. */
	/* Except the original patch from kth.se was 4.1.0 "experimental 1" */
	if (version < 0x4210 && version != 0x4101)
			size &= ~0x80;
	return w.printf("%c%hd%hd%c", PKT_RADAR, x, y, size);
}

///////////////////////////////////////////////////////////////////////////////
int ConnectionPlayer::SendFastRadar(unsigned char *buf, int n)
{
	int 				avail;

	if ((n & 0xFF) != n) {
		seterrno(0);
		error("Bad number of fastradar %d", n);
		return 0;
	}
	avail = w.size - w.len - SOCKBUF_WRITE_SPARE - 3;
	if (n * 3 >= avail) {
		if (avail > 3) {
			n = (avail - 2) / 3;
		} else {
			return 0;
		}
	}
	w.buf[w.len++] = PKT_FASTRADAR;
	w.buf[w.len++] = (unsigned char)(n & 0xFF);
	memcpy(&w.buf[w.len], buf, n * 3);
	w.len += n * 3;

	return (2 + (n * 3));
}

///////////////////////////////////////////////////////////////////////////////
int ConnectionPlayer::SendDamaged(int damaged)
{
	return w.printf("%c%c", PKT_DAMAGED, damaged);
}

///////////////////////////////////////////////////////////////////////////////
int ConnectionPlayer::SendAudio(int type, int vol)
{
	if (w.size - w.len <= 32)
			return 0;
	return w.printf("%c%c%c", PKT_AUDIO, type, vol);
}

///////////////////////////////////////////////////////////////////////////////
int ConnectionPlayer::SendTimeLeft(long sec)
{
	return w.printf("%c%ld", PKT_TIME_LEFT, sec);
}

///////////////////////////////////////////////////////////////////////////////
int ConnectionPlayer::SendPlayMessage(PCSTR msg)
{
	if (!BIT(state, CONN_PLAYING | CONN_READY)) {
				seterrno(0);
				error("Connection not ready for message (%d,%d)",
						  state, id);
				return 0;
	}
	return cw.printf("%c%S", PKT_MESSAGE, msg);
}

///////////////////////////////////////////////////////////////////////////////
int ConnectionPlayer::SendRobotWatch(int y, PCSTR msg)
{
	if (!BIT(state, CONN_PLAYING | CONN_READY)) {
				seterrno(0);
				error("Connection not ready for message (%d,%d)",
						  state, id);
				return 0;
	}
	if (version < 0x5010)
		return(0);
	return w.printf("%c%hd%S", PKT_ROBOT_WATCH, y, msg);
}

///////////////////////////////////////////////////////////////////////////////
int	ConnectionPlayer::SendRobotWatchDeco(const RobotWatchDeco* rwd) {
	if (!BIT(state, CONN_PLAYING | CONN_READY)) {
				seterrno(0);
				error("Connection not ready for message (%d,%d)",
						  state, id);
				return 0;
	}
	if (version < 0x5010)
		return(0);
	D(Trace("Deco: %d/%d\n", rwd->x, rwd->y);)
	return w.printf("%c%c%hd%hd%c%c", PKT_ROBOT_WATCHDECO, rwd->type, rwd->x, rwd->y,
									  rwd->radius, rwd->color);

}

///////////////////////////////////////////////////////////////////////////////
int ConnectionPlayer::SendLoseItem(int lose_item_index)
{
	if (version < 0x3400) { /* this should never hit since */
							/* only a 3.4+ client would send */
							/* the loseitem key */
		return 1;
	}
	return w.printf("%c%c", PKT_LOSEITEM, lose_item_index);
}

///////////////////////////////////////////////////////////////////////////////
int ConnectionPlayer::SendStartOfFrame()
{

	if (state != CONN_PLAYING) {
		if (state != CONN_READY) {
			seterrno(0);
			error("Connection not ready for frame (%d,%d)",
				state, id);
		}
		return -1;
	}
	/*
	 * We tell the client which frame number this is and
	 * which keyboard update we have last received.
	 */
	w.Clear();
	if (w.printf(
					  "%c%ld%ld",
					  PKT_START, frame_loops, lastKeyChange) <= 0) {
		DestroyConnection("write error");
		return -1;
	}

	/* Return ok */
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
int ConnectionPlayer::SendEndOfFrame()
{
	int 				n;

	last_packet_of_frame = 1;
	n = w.printf("%c%ld", PKT_END, frame_loops);
	last_packet_of_frame = 0;
	if (n == -1) {
		DestroyConnection("write error");
		return -1;
	}
	if (n == 0) {
		/*
		 * Frame update size exceeded buffer size.
		 * Drop this packet.
		 */
		w.Clear();
		return 0;
	}
	while (motdOffset >= 0
		&& cw.len + w.len < MAX_RELIABLE_DATA_PACKET_SIZE) {
		SendMotd();
	}
	if (cw.len > 0 && w.len < MAX_RELIABLE_DATA_PACKET_SIZE) {
		if (SendReliable() == -1) {
			return -1;
		}
		if (w.len == 0) {
			return 1;
		}
	}
	if (w.Flush() == -1) {
		DestroyConnection("flush error");
		return -1;
	}
	w.Clear();
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
int ConnectionPlayer::SetCookie(uint _cookie)
{
	if (cookie == _cookie)
		return(0);
	cookie = _cookie;
	if (version < 0x5000)
		return(0);
	return(cw.printf("%c%d", PKT_COOKIE, cookie));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int ConnectionPlayer::ReceiveKeyboard()
{
	Player				*pl;
	long				change;
	u_byte				ch;
	int 				size = KEYBOARD_SIZE;

	if (version < 0x3800) {
		/* older servers have a keyboard_size of 8 bytes instead of 9. */
		size--;
	}
	if (r.ptr - r.buf + size + 1 + 4 > r.len) {
		/*
		 * Incomplete client packet.
		 */
		return 0;
	}
	r.scanf("%c%ld", &ch, &change);
	if (change <= lastKeyChange) {
		/*
		 * We already have this key.
		 * Nothing to do.
		 */
		r.ptr += size;
	}
	else {
		lastKeyChange = change;
		pl = world->players[world->getInd[id]];
		memcpy(pl->last_keyv, r.ptr, size);
		r.ptr += size;
		world->HandleKeyboard(pl);
	}
// FIXME Should this ever really happen anyway?
//	if (numKeyboardUpdates++ && (state & CONN_PLAYING)) {
//		DestroyConnection("no macros");
//		return -1;
//	}

	return 1;
}

///////////////////////////////////////////////////////////////////////////////
int ConnectionPlayer::ReceivePlay()
{
	unsigned char	ch;
	int 					n;
	char			errmsg[MAX_CHARS];

	if ((n = r.scanf("%c", &ch)) != 1) {
		seterrno(0);
		error("Cannot receive play packet");
		DestroyConnection("receive error");
		return -1;
	}
	if (ch != PKT_PLAY) {
		seterrno(0);
		error("Packet is not of play type");
		DestroyConnection("not play");
		return -1;
	}
	if (state != CONN_LOGIN) {
		if (state != CONN_PLAYING) {
			if (state == CONN_READY) {
				r.ptr = r.buf + r.len;
				return 0;
			}
			seterrno(0);
			error("Connection not in login state (%02x)", state);
			DestroyConnection("not login");
			return -1;
		}
		if (SendReliable() == -1) {
			return -1;
		}
		return 0;
	}
	w.Clear();
	strlcpy(errmsg, "login failed", sizeof(errmsg));
	if (world->HandleLogin(myInd, errmsg, sizeof(errmsg)) == -1) {
		DestroyConnection(errmsg);
		return -1;
	}

	return 2;
}

///////////////////////////////////////////////////////////////////////////////
int ConnectionPlayer::ReceivePower()
{
	Player				*pl;
	unsigned char		ch;
	short				tmp;
	int 				n;
	DFLOAT				power;
	int 				autopilot;

	if ((n = r.scanf("%c%hd", &ch, &tmp)) <= 0) {
		if (n == -1) {
			DestroyConnection("read error");
		}
		return n;
	}
	power = (DFLOAT) tmp / 256.0F;
	pl = world->players[world->getInd[id]];
	autopilot = BIT(pl->used, HAS_AUTOPILOT);
	/* old client are going to send autopilot-mangled data, ignore it */
	if (autopilot && pl->version < 0x4200)
		return 1;

	switch (ch) {
	case PKT_POWER:
		if (autopilot)
			pl->auto_power_s = power;
		else
			pl->power = power;
		break;
	case PKT_POWER_S:
		pl->power_s = power;
		break;
	case PKT_TURNSPEED:
		if (autopilot)
			pl->auto_turnspeed_s = power;
		else
			pl->turnspeed = power;
		break;
	case PKT_TURNSPEED_S:
		pl->turnspeed_s = power;
		break;
	case PKT_TURNRESISTANCE:
		if (autopilot)
			pl->auto_turnresistance_s = power;
		else
			pl->turnresistance = power;
		break;
	case PKT_TURNRESISTANCE_S:
		pl->turnresistance_s = power;
		break;
	default:
		seterrno(0);
		error("Not a power packet (%d,%02x)", ch, state);
		DestroyConnection("not power");
		return -1;
	}
	return 1;
}

///////////////////////////////////////////////////////////////////////////////
int ConnectionPlayer::ReceiveAckCannon()
{
	long				loops_ack;
	unsigned char		ch;
	int 				n;
	unsigned short		num;

	if ((n = r.scanf("%c%ld%hu",
						  &ch, &loops_ack, &num)) <= 0) {
		if (n == -1) {
			DestroyConnection("read error");
		}
		return n;
	}
	if (num >= world->numCannons) {
		DestroyConnection("bad cannon ack");
		return -1;
	}
	if (loops_ack > world->cannons[num].last_change) {
		SET_BIT(world->cannons[num].conn_mask, 1 << myInd);
	}
	return 1;
}

///////////////////////////////////////////////////////////////////////////////
int ConnectionPlayer::ReceiveAckFuel()
{
	long				loops_ack;
	unsigned char		ch;
	int 				n;
	unsigned short		num;

	if ((n = r.scanf("%c%ld%hu",
						  &ch, &loops_ack, &num)) <= 0) {
		if (n == -1) {
			DestroyConnection("read error");
		}
		return n;
	}
	if (num >= world->numFuels) {
		DestroyConnection("bad fuel ack");
		return -1;
	}
	if (loops_ack > world->fuels[num].last_change) {
		SET_BIT(world->fuels[num].conn_mask, 1 << myInd);
	}
	return 1;
}

///////////////////////////////////////////////////////////////////////////////
int ConnectionPlayer::ReceiveAckTarget()
{
	long				loops_ack;
	unsigned char		ch;
	int 				n;
	unsigned short		num;

	if ((n = r.scanf("%c%ld%hu",
					  &ch, &loops_ack, &num)) <= 0) {
		if (n == -1) {
			DestroyConnection("read error");
		}
		return n;
	}
	if (num >= world->numTargets) {
		DestroyConnection("bad target ack");
		return -1;
	}
	/*
	 * Because the "loops" value as received by the client as part
	 * of a frame update is 1 higher than the actual change to the
	 * target in collision.c a valid map object change
	 * acknowledgement must be at least 1 higher.
	 * That's why we should use the '>' symbol to compare
	 * and not the '>=' symbol.
	 * The same applies to cannon and fuelstation updates.
	 * This fix was discovered for 3.2.7, previously some
	 * destroyed targets could have been displayed with
	 * a diagonal cross through them.
	 */
	if (loops_ack > world->targets[num].last_change) {
		SET_BIT(world->targets[num].conn_mask, 1 << myInd);
		CLR_BIT(world->targets[num].update_mask, 1 << myInd);
	}
	return 1;
}

///////////////////////////////////////////////////////////////////////////////
int ConnectionPlayer::ReceiveDisplay()
{
	unsigned char		ch, _debris_colors, _spark_rand;
	short				width, height;
	int 				n;

	if ((n = r.scanf("%c%hd%hd%c%c", &ch, &width, &height,
						  &_debris_colors, &_spark_rand)) <= 0) {
		if (n == -1) {
			DestroyConnection("read error");
		}
		return n;
	}
	LIMIT(width, MIN_VIEW_SIZE, MAX_VIEW_SIZE);
	LIMIT(height, MIN_VIEW_SIZE, MAX_VIEW_SIZE);
	viewWidth = width;
	viewHeight = height;
	debris_colors = _debris_colors;
	spark_rand = _spark_rand;
	return 1;
}

///////////////////////////////////////////////////////////////////////////////
int ConnectionPlayer::ReceiveModifierBank()
{
	Player				*pl;
	unsigned char		bank;
	char				str[MAX_CHARS];
	unsigned char		ch;
	char				*cp;
	modifiers			mods;
	int 				n;

	if ((n = r.scanf("%c%c%s", &ch, &bank, str)) <= 0) {
		if (n == -1) {
			DestroyConnection("read modbank");
		}
		return n;
	}
	pl = world->players[world->getInd[id]];
	if (bank < NUM_MODBANKS) {
		CLEAR_MODS(mods);
		if (BIT(world->rules->mode, ALLOW_MODIFIERS)) {
			for (cp = str; *cp; cp++) {
				switch (*cp) {
				case 'F': case 'f':
					if (!BIT(world->rules->mode, ALLOW_NUKES))
								break;
					if (*(cp+1) == 'N' || *(cp+1) == 'n')
						SET_BIT(mods.nuclear, FULLNUCLEAR);
					break;
				case 'N': case 'n':
					if (!BIT(world->rules->mode, ALLOW_NUKES))
								break;
					SET_BIT(mods.nuclear, NUCLEAR);
					break;
				case 'C': case 'c':
					if (!BIT(world->rules->mode, ALLOW_CLUSTERS))
								break;
					SET_BIT(mods.warhead, CLUSTER);
					break;
				case 'I': case 'i':
					SET_BIT(mods.warhead, IMPLOSION);
					break;
				case 'V': case 'v':
					cp++; mods.velocity = str2num (&cp, 0, MODS_VELOCITY_MAX);
					cp--;
					break;
				case 'X': case 'x':
					cp++; mods.mini = str2num (&cp, 1, MODS_MINI_MAX+1) - 1;
					cp--;
					break;
				case 'Z': case 'z':
					cp++; mods.spread = str2num (&cp, 0, MODS_SPREAD_MAX);
					cp--;
					break;
				case 'B': case 'b':
					cp++; mods.power = str2num (&cp, 0, MODS_POWER_MAX);
					cp--;
					break;
				case 'L': case 'l':
					cp++;
					if (!BIT(world->rules->mode, ALLOW_LASER_MODS))
						break;
					if (*cp == 'S' || *cp == 's')
						SET_BIT(mods.laser, STUN);
					if (*cp == 'B' || *cp == 'b')
						SET_BIT(mods.laser, BLIND);
					break;
				}
			}
		}
		pl->modbank[bank] = mods;
	}
	return 1;
}

///////////////////////////////////////////////////////////////////////////////
void ConnectionPlayer::GetDisplayParameters(int *width, int *height,
							int *_debris_colors, int *_spark_rand)
{

	*width = viewWidth;
	*height = viewHeight;
	*_debris_colors = debris_colors;
	*_spark_rand = spark_rand;
}

///////////////////////////////////////////////////////////////////////////////
int ConnectionPlayer::ReceivePointerMove()
{
	Player				*pl;
	unsigned char		ch;
	short				movement;
	int 				n;
	DFLOAT				turnspeed, turndir;

	if ((n = r.scanf("%c%hd", &ch, &movement)) <= 0) {
		if (n == -1) {
			DestroyConnection("read error");
		}
		return n;
	}
	pl = world->players[world->getInd[id]];
	if (BIT(pl->status, HOVERPAUSE))
		return 1;

	if (BIT(pl->used, HAS_AUTOPILOT))
		pl->Autopilot(false);
	turnspeed = movement * pl->turnspeed / MAX_PLAYER_TURNSPEED;
	if (turnspeed < 0) {
		turndir = -1.0;
		turnspeed = -turnspeed;
	}
	else {
		turndir = 1.0;
	}
	if (pl->turnresistance)
		LIMIT(turnspeed, MIN_PLAYER_TURNSPEED, MAX_PLAYER_TURNSPEED);
	  /* Minimum amount of turning if you want to turn at all?
		And the only effect of that maximum is making
		finding the correct settings harder for new mouse players,
		because the limit is checked BEFORE multiplying by turnres!
		Kept here to avoid changing the feeling for old players who
		are already used to this odd behavior. New players should set
		turnresistance to 0.
	  */
	else
		LIMIT(turnspeed, 0, 5*RES);

	pl->turnvel -= turndir * turnspeed;

	return 1;
}

///////////////////////////////////////////////////////////////////////////////
int ConnectionPlayer::ReceiveShape()
{
	int			n;
	char		ch;
	char		str[2*MSG_LEN];

	if ((n = r.scanf("%c%S", &ch, str)) <= 0) {
		if (n == -1) {
			DestroyConnection("read shape");
		}
		return n;
	}
	if (version > 0x3200) {
		if ((n = r.scanf("%S", &str[strlen(str)])) <= 0) {
			if (n == -1) {
				DestroyConnection("read shape ext");
			}
			return n;
		}
	}
	if (state == CONN_LOGIN && ship == NULL) {
		ship = Parse_shape_str(str);
	}
	return 1;
}


///////////////////////////////////////////////////////////////////////////////
int ConnectionPlayer::ReceiveFpsRequest()
{
	Player				*pl;
	int 				n;
	unsigned char		ch;
	unsigned char		fps;

	if ((n = r.scanf("%c%c", &ch, &fps)) <= 0) {
		if (n == -1) {
			DestroyConnection("read error");
		}
		return n;
	}
	pl = GetPlayer();
	if (pl) {
		pl->player_fps = fps;
		if (fps > world->GetFPS()) pl->player_fps = world->GetFPS();
		if (fps < (world->GetFPS() / 2)) pl->player_fps = (world->GetFPS()+1) / 2;
		if (fps == 0) 
				pl->player_fps = world->GetFPS();
		if ((fps == 20) && world->options.ignore20MaxFPS->GetBool())
				pl->player_fps = world->GetFPS();
		n = world->GetFPS() - pl->player_fps;
		if (n <= 0) {
			pl->player_count = 0;
		} else {
			pl->player_count = world->GetFPS() / n;
		}
	}

	return 1;
}

///////////////////////////////////////////////////////////////////////////////
int ConnectionPlayer::ReceiveAudioRequest()
{
	Player* 	pl;
	int 				n;
	unsigned char		ch;
	unsigned char		onoff;

	if ((n = r.scanf("%c%c", &ch, &onoff)) <= 0) {
		if (n == -1) {
			DestroyConnection("read error");
		}
		return n;
	}
	if (id != NO_ID) {
		pl = GetPlayer();
		SoundPlayerOnOff(pl, onoff);
	}

	return 1;
}

///////////////////////////////////////////////////////////////////////////////
int ConnectionPlayer::ReceiveMotd()
{
    unsigned char       ch;
    long                offset;
    int                 n;
    long                bytes;

    if ((n = r.scanf(
                          "%c%ld%ld",
                          &ch, &offset, &bytes)) <= 0) {
        if (n == -1) {
            DestroyConnection("read error");
        }
        return n;
    }
    motdOffset = offset;
    motdStop = offset + bytes;

    return 1;
}

///////////////////////////////////////////////////////////////////////////////
/*
 * Send the server MOTD to the client.
 * The last time we send a motd packet it should
 * have datalength zero to mean EOMOTD.
 */
int ConnectionPlayer::SendMotd()
{
	int 				len;
	int 				off = motdOffset,
						size = 0;
	char				buf[MAX_MOTD_CHUNK];

	len = MIN(MAX_MOTD_CHUNK, MAX_RELIABLE_DATA_PACKET_SIZE - cw.len - 10);
	if (len >= 10) {
		len = world->GetMotd(buf, off, len, &size);
		if (len <= 0) {
			len = 0;
			motdOffset = -1;
		}
		if (cw.printf(
						  "%c%ld%hd%ld",
						  PKT_MOTD, off, len, size) <= 0) {
			DestroyConnection("motd header");
			return -1;
		}
		if (len > 0) {
			motdOffset += len;
			if (cw.Write(buf, len) != len) {
				DestroyConnection("motd data");
				return -1;
			}
		}
	}

	/* Return ok */
	return 1;
}

///////////////////////////////////////////////////////////////////////////////
int CheckPlayerConnection(PCSTR real, PCSTR nick, PCSTR dpy, PCSTR addr)
{
	int			i;
	ConnectionPlayer*	connp;
	for (i = 0; i < netServer->maxConnections; i++) {
		connp = (ConnectionPlayer*)netServer->conn[i];
		if (connp
		 // && connp->connType == ctPlayer		// totally unused never initialized!?
		 && connp->state == CONN_LISTENING) {
			if (strcasecmp(connp->nick, nick) == 0) {
				if (!strcmp(real, connp->real)
				 && !strcmp(dpy, connp->dpy)
				 && !strcmp(addr, connp->addr)) {
					return connp->my_port;
				}
				return -1;
			}
		}
	}
	return -1;
}

///////////////////////////////////////////////////////////////////////////////
int ConnectionPlayer::ReceiveTalk()
{
	unsigned char		ch;
	int 				n;
	long				seq;
	char				str[MAX_CHARS];

	if ((n = r.scanf("%c%ld%s", &ch, &seq, str)) <= 0) {
		if (n == -1) {
			DestroyConnection("read error");
		}
		return n;
	}
	if (seq > talkSequenceNum) {
		if ((n = cw.printf("%c%ld", PKT_TALK_ACK, seq)) <= 0) {
			if (n == -1) {
				DestroyConnection("write error");
			}
			return n;
		}
		talkSequenceNum = seq;
		if (*str == '/') {
			Handle_player_command(world->players[world->getInd[id]], str + 1);
		}
		else {
			HandleTalk(str);
		}
	}
	return 1;
}


///////////////////////////////////////////////////////////////////////////////
/*
 * If a message contains a colon then everything before that colon is
 * either a unique player name prefix, or a team number with players.
 * If the string does not match one team or one player the message is not sent.
 * If no colon, the message is general.
 */
void ConnectionPlayer::HandleTalk(char *str)
{
	Player				*pl = world->players[world->getInd[id]];
	int 				i, sent, team;
		unsigned int	len;
	char				*cp,
						msg[MSG_LEN * 2];

	if ((cp = strchr (str, ':')) == NULL
		|| cp == str
		|| strchr("-_~)(/\\}{[]", cp[1])		/* smileys are smileys */
		) {
		sprintf(msg, "%s [%s]", str, pl->name);
		world->BroadcastPlayMessage(msg);
		return;
	}
	*cp++ = '\0';
	len = strlen (str);
	sprintf(msg, "%s [%s]", cp, pl->name);

	if (strspn(str, "0123456789") == len) { 			/* Team message */
		team = atoi (str);
		sprintf(msg + strlen(msg), ":[%d]", team);
		for (sent = i = 0; i < world->numPlayers; i++) {
			if (world->players[i]->team != TEAM_NOT_SET 
				 && world->players[i]->team == team) 
				{
						sent++;
						world->players[i]->SetMessage(msg);
			}
		}
		if (sent) {
			if (pl->team != team)
				pl->SetMessage(msg);
		} else {
			sprintf(msg, "Message not sent, nobody in team %d!",
					team);
			pl->SetMessage(msg);
		}
	}
	else if (strcasecmp(str, "god") == 0) {
				ServerLogAdminMessage(pl, cp);
	}
	else {												/* Player message */
		sent = -1;
		/* first look for an exact match on player nickname. */
		for (i = 0; i < world->numPlayers; i++) {
			if (strcasecmp(world->players[i]->name, str) == 0) {
				sent = i;
				break;
			}
		}
		if (sent == -1) {
			/* now look for a partial match on both nick and realname. */
			for (sent = -1, i = 0; i < world->numPlayers; i++) {
				if (strncasecmp(world->players[i]->name, str, len) == 0
					|| strncasecmp(world->players[i]->realname, str, len) == 0)
					sent = (sent == -1) ? i : -2;
			}
		}
		switch (sent) {
		case -2:
			sprintf(msg, "Message not sent, %s matches more than one player!",
					str);
			pl->SetMessage(msg);
			break;
		case -1:
			sprintf(msg, "Message not sent, %s does not match any player!",
					str);
			pl->SetMessage(msg);
			break;
		default:
			if (world->players[sent] != pl) {
				sprintf(msg + strlen(msg), ":[%s]", world->players[sent]->name);
				world->players[sent]->SetMessage(msg);
				pl->SetMessage(msg);
			}
			break;
		}
	}
}
