/* $Id: World.contact.cpp,v 1.53 2007/01/20 19:28:07 dick Exp $
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
 *  $Log: World.contact.cpp,v $
 *  Revision 1.53  2007/01/20 19:28:07  dick
 *  Comment out unused debug
 *
 *  Revision 1.52  2007/01/10 18:14:47  dick
 *  All robot actions are now handled through RobotMan.
 *  There is one RobotMan per World.
 *
 *  Revision 1.51  2006/09/24 05:05:23  dick
 *  Better diagnostic when failing to create contact port
 *
 *  Revision 1.50  2004/05/22 15:12:34  dick
 *  Remove #ifndef SILENT
 *
 *  Revision 1.49  2004/05/20 22:12:38  dick
 *  Attribute showLocks displays in the ScoreTable who everyone is locked on.
 *
 *  Revision 1.48  2004/05/04 21:07:17  dick
 *  Need randommnt.h
 *
 *  Revision 1.47  2004/04/26 07:07:14  dick
 *  Define a list of pages that can be displayed on the ScoreTable in the client.
 *  Each page can be controlled by the server (and the server admin tool).
 *
 *  Revision 1.46  2004/04/23 17:44:50  dick
 *  Print the unknown packet as hex which is how they are defined
 *
 *  Revision 1.45  2004/04/16 21:07:59  dick
 *  NetServer becomes a little UDP engine that can be plugged into different apps.
 *
 *  Revision 1.44  2004/03/04 03:47:31  dick
 *  Save off the version that the client reported as origVersion.
 *  This is for stats only and doesn't affect gameplay.
 *
 *  Revision 1.43  2004/03/03 16:02:48  dick
 *  Handle version forks between 4.5.1 and 5.0.0
 *
 *  Revision 1.42  2004/02/23 02:06:09  dick
 *  robot.h becomes Robot.h
 *
 *  Revision 1.41  2004/02/20 06:27:47  dick
 *  SendPlayMessage becomes BroadcastPlayMessage() because it sends a play message
 *  to all connections.
 *
 *  Revision 1.40  2004/02/18 01:22:14  dick
 *  Remove extraneous and obsolete #include "netserver.h"
 *
 *  Revision 1.39  2004/02/18 00:58:34  dick
 *  Delete_player becomes DeletePlayer
 *
 *  Revision 1.38  2004/02/16 14:29:44  dick
 *  server/sched.cpp and friends become serverlib/NetServer
 *  which disconnects the udp listen and receive functionality from the game.
 *
 *  Revision 1.37  2004/02/06 03:39:58  dick
 *  Only cleanup listen sockets if they're actually open
 *
 *  Revision 1.36  2004/02/03 03:39:54  dick
 *  Add Windows precompiled header support.
 *
 *  Revision 1.35  2004/01/26 19:09:42  dick
 *  localhost always has control permission
 *
 *  Revision 1.34  2004/01/26 16:57:20  dick
 *  Get_process_id becomes GetProcessID
 *
 *  Revision 1.33  2004/01/21 17:53:21  dick
 *  Open the localAddr always on 15345.  Otherwise the localCtl can't connect.
 *
 *  Revision 1.32  2004/01/09 17:37:03  dick
 *  Use localAddr instead of "127.0.0.1"
 *
 *  Revision 1.31  2004/01/08 18:31:17  dick
 *  Return E_TOOMANYIPS if that is the case.
 *
 *  Revision 1.30  2004/01/08 18:10:51  dick
 *  Update to XPilot 4.5.5beta
 *
 *  Revision 1.29  2004/01/07 04:33:41  dick
 *  Don't open the serverAddr listen socket if it is 127.0.0.1, cause we already did that one
 *
 *  Revision 1.28  2002/09/16 22:15:24  dick
 *  Use Stringized conpar.
 *
 *  Revision 1.27  2002/09/16 21:52:33  dick
 *  Use Stringized ServerT.
 *
 *  Revision 1.26  2002/09/09 23:42:55  dick
 *  Handle connecting a LocalCtl to this server.
 *
 *  Revision 1.25  2002/09/01 00:13:45  dick
 *  Remove obsolete windows #include "winServer.h"
 *
 *  Revision 1.24  2002/07/12 15:36:48  dick
 *  Open a listen port on localhost (127.0.0.1) as well as any requested.
 *
 *  Revision 1.23  2002/07/11 19:04:12  dick
 *  Retab
 *
 *  Revision 1.22  2002/07/08 05:52:59  dick
 *  showtime() handles the space after itself
 *
 *  Revision 1.21  2002/06/30 06:43:24  dick
 *  SendMessage is a bad word.  It's a Windows function that is a macro and gets
 *  translated to SendMessageA or SendMessageW.  Although it works, renaming ours
 *  to SendPlayMessage makes browsing the source code easier.
 *
 *  Revision 1.20  2002/06/26 07:01:18  dick
 *  Use xperrno instead of errno and seterrno(0) instead of errno = 0.
 *
 *  Revision 1.19  2002/06/25 05:00:08  dick
 *  Comment out the "got Contact" and "Invalid version" messages that i seem
 *  to get a lot of.
 *
 *  Revision 1.18  2002/06/01 06:06:47  dick
 *  Encapsulate (almost) everything.  Get rid of (almost) all refs to theWorld.
 *
 *  Revision 1.17  2002/05/18 22:34:15  dick
 *  Move showtime to common.
 *
 *  Revision 1.16  2002/05/18 20:55:38  dick
 *  Update to XPilot-4.5.1
 *
 *  Revision 1.15  2001/08/14 21:46:04  dick
 *  Whitespace
 *
 *  Revision 1.14  2001/08/13 03:38:09  dick
 *  Remove old debug
 *
 *  Revision 1.13  2001/08/12 11:10:09  dick
 *  Handle contact from new control client.
 *
 *  Revision 1.12  2001/08/11 09:38:20  dick
 *  new class ConnectionControl, a Connection for a control.
 *
 *  Revision 1.11  2001/08/10 08:11:37  dick
 *  New class ConnectionPlayer subclasses Connection to provide player
 *  functions.  New class Frame subclasses ConnectionPlayer which provides
 *  Frame update functionality.
 *
 *  Revision 1.10  2001/08/09 07:43:34  dick
 *  New class Connection
 *
 *  Revision 1.9  2001/08/07 20:43:40  dick
 *  Use xpprintf instead of printf
 *
 *  Revision 1.8  2001/08/06 05:41:50  dick
 *  struct sock_t becomes class Sock
 *
 *  Revision 1.7  2001/08/01 06:39:40  dick
 *  AFUNC cleanups. address to function...
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
 *  Revision 1.5  2001/07/21 07:22:16  dick
 *  Bogus use of String for strtok (can't be NULL)
 *
 *  Revision 1.4  2001/07/19 07:01:48  dick
 *  Integrate OptOrigin into our ServerOption.
 *  stringvar becomes varString, intvar becomes varInt, etc.
 *
 *  Revision 1.3  2001/07/10 08:10:08  dick
 *  Don't need (deleted) map.h because World.h is included in global.h
 *
 *  Revision 1.2  2001/07/07 12:00:42  dick
 *  Rename classes to C++ "Style".  old World becomes theWorld.
 *
 */
#include "StdAfx.h"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <time.h>
#include <errno.h>
#include <limits.h>
#include <sys/types.h>

#ifndef _WINDOWS
# include <unistd.h>
# ifndef __hpux
#  include <sys/time.h>
# endif
#endif

#ifdef _WINDOWS
# include <process.h>
#endif

#include "version.h"
#include "config.h"
#include "types.h"
#include "serverconst.h"
#include "global.h"
#include "proto.h"
#include "Sock.h"
#include "pack.h"
#include "bit.h"
#include "Sockbuf.h"
#include "ConnectionPlayer.h"
#include "NetServer.h"
#include "error.h"
#include "checknames.h"
#include "server.h"
#include "commonproto.h"
#include "randommt.h"
#include "showtime.h"
#include "portability.h"
#include "metaserver.h"
#include "RobotMan.h"

char contact_version[] = VERSION;


/*
 * Global variables
 */
int 					MaxQueuedPlayers = 20;


static Sock		localContactSocket;
static Sockbuf	localContactBuf;

static char 			msg[MSG_LEN];

static int Check_address(char *addr);

#if 0					// Debug Contact
#define DC(__d) D(__d)
#else
#define DC(__d)
#endif

///////////////////////////////////////////////////////////////////////////////
void World::ContactCleanup()
{
	if (remoteContactSocket.IsValidSock())
	{
		netServer->RemoveInput(remoteContactSocket.fd);
		remoteContactSocket.Close();
	}
	if (localContactSocket.IsValidSock())
	{
		netServer->RemoveInput(localContactSocket.fd);
		localContactSocket.Close();
	}

}

///////////////////////////////////////////////////////////////////////////////
bool World::ContactInit()
{
	if (ContactInit(localContactBuf, localContactSocket, localAddr, SERVER_PORT))
	{
		netServer->InstallInput(&World::ContactFromLocal, localContactSocket.fd,
			(void *)this);
		if (localAddr != serverAddr)
		{
			if (ContactInit(remoteContactBuf, remoteContactSocket, 
					serverAddr, options.contactPort->GetInt()))
			{
				netServer->InstallInput(&World::ContactFromRemote, remoteContactSocket.fd,
					(void *)this);
				return(true);
			}
		}
		else
			return(true);
	}
	return(false);
}

///////////////////////////////////////////////////////////////////////////////
bool World::ContactInit(Sockbuf& ibuf, Sock& initSock, PCSTR addr, int port)
{
	int 		status;

	/*
	 * Create a socket which we can listen on.
	 */
	xpprintf("ContactInit on %s:%d\n", addr, port);
	if ((status = initSock.OpenUdp(addr, port)) == -1) {
		error("Could not create contact socket at %s:%d", addr, port);
		error("Perhaps %s is already running?", APPNAME);
		return(EndGame());
	}
	initSock.SetTimeout(0, 0);
	if (initSock.SetNonBlocking(1) == -1) {
		error("Can't make contact socket non-blocking");
		return(EndGame());
	}
	if (ibuf.Init(&initSock, SERVER_SEND_SIZE,
					 SOCKBUF_READ | SOCKBUF_WRITE | SOCKBUF_DGRAM) == -1) {
		error("No memory for contact buffer");
		return(EndGame());
	}
	return(TRUE);
}

///////////////////////////////////////////////////////////////////////////////
/*
 * Kick robot players?
 * Return the number of kicked robots.
 * Don't kick more than one robot.
 */
int World::KickRobotPlayers(int team)
{
	if (numRobots == 0) 		/* no robots available for kicking */
		return 0;
	if (team == TEAM_NOT_SET) {
		if (BIT(rules->mode, TEAM_PLAY) 
				&& options.reserveRobotTeam->GetBool()) {
			/* kick robot with lowest score from any team but robotTeam */
			DFLOAT low_score = INT_MAX;
			int low_i = -1;
			int i;
			for (i = 0; i < numPlayers; i++) {
				if (!IS_ROBOT_IND(this, i) || players[i]->team == options.robotTeam->GetInt())
					continue;
				if (players[i]->score < low_score) {
					low_i = i;
					low_score = players[i]->score;
				}
			}
			if (low_i >= 0) {
				robotMan->Delete(low_i, true);
				return 1;
			}
			return 0;
		} else {
			/* kick random robot */
			robotMan->Delete(-1, true);
			return 1;
		}
	} else {
		if (teams[team].numRobots > 0) {
			/* kick robot with lowest score from this team */
			DFLOAT low_score = INT_MAX;
			int low_i = -1;
			int i;
			for (i = 0; i < numPlayers; i++) {
				if (!IS_ROBOT_IND(this, i) || players[i]->team != team)
					continue;
				if (players[i]->score < low_score) {
					low_i = i;
					low_score = players[i]->score;
				}
			}
			if (low_i >= 0) {
				robotMan->Delete(low_i, true);
				return 1;
			}
			return 0;
		} else {
			return 0;			/* no robots in this team */
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
/*
 * Kick paused players?
 * Return the number of kicked players.
 */
int World::KickPausedPlayers(int team)
{
	int 				i;
	int 				num_unpaused = 0;

	for (i = numPlayers - 1; i >= 0; i--) 
	{
		if (players[i]->conn
			&& BIT(players[i]->status, PAUSE)
			&& (team == TEAM_NOT_SET || players[i]->team == team)) 
		{
			if (team == TEAM_NOT_SET) 
			{
				sprintf(msg,	// DIK: whoa.  global msg?
						"The paused \"%s\" was kicked because the game is full.",
						players[i]->name);
				players[i]->conn->DestroyConnection("no pause with full game");
			}
			else 
			{
				sprintf(msg,
						"The paused \"%s\" was kicked because team %d is full.",
						players[i]->name, team);
				players[i]->conn->DestroyConnection("no pause with full team");
			}
			BroadcastPlayMessage(msg);
			num_unpaused++;
		}
	}

		return num_unpaused;
}

///////////////////////////////////////////////////////////////////////////////
static int Reply(Sockbuf& ibuf, char *host_addr, int port)
{
	int 				i, result = -1;
	const int			max_send_retries = 3;

	for (i = 0; i < max_send_retries; i++) {
		if ((result = ibuf.sock.SendDest(host_addr, port, ibuf.buf, ibuf.len)) == -1) {
			ibuf.sock.GetError();
		} else {
			break;
		}
	}

	return result;
}

///////////////////////////////////////////////////////////////////////////////
static int CheckNames(World* w, String& nick_name, PCSTR real_name, PCSTR host_name)
{
	int 				i;

	/*
	 * Bad input parameters?
	 */
	if (real_name[0] == 0
		|| host_name[0] == 0
		|| nick_name[0] < 'A'
		|| nick_name[0] > 'Z') {
		return E_INVAL;
	}

	/*
	 * All names must be unique (so we know who we're talking about).
	 */
	nick_name.TrimTrailingWhiteSpace();
	for (i = 0; i < w->numPlayers; i++) {
		if (strcasecmp(w->players[i]->name, nick_name) == 0) {
			D(xpprintf("%s%s %s\n", showtime(), w->players[i]->name, (PCSTR)nick_name);)
			return E_IN_USE;
		}
	}

	return SUCCESS;
}


///////////////////////////////////////////////////////////////////////////////
/*
 * Support some older clients, which don't know
 * that they can join the current version.
 *
 * IMPORTANT! Adjust the next code if you're changing version numbers.
 */
static unsigned Version_to_magic(unsigned version)
{
	if (version >= 0x3100 && version <= MY_VERSION) {
		return VERSION2MAGIC(version);
	}
	return MAGIC;
}

///////////////////////////////////////////////////////////////////////////////
void World::ContactFromLocal(int fd, void* v)
{
	World* w = (World*)v;
	DC(xpprintf("%s got localhost Contact\n", showtime());)
	w->Contact(localContactBuf);
}

///////////////////////////////////////////////////////////////////////////////
void World::ContactFromRemote(int fd, void* v)
{
	World* w = (World*)v;
	DC(xpprintf("%s got Contact\n", showtime());)
	w->Contact(w->remoteContactBuf);
}

///////////////////////////////////////////////////////////////////////////////
void World::Contact(Sockbuf& ibuf)
{
	int 				i,
						team,
						bytes,
						delay,
						login_port,
						qpos,
						status;
	char				reply_to;
	uint				magic;
	uint				version;
	uint				origVersion;
	uint				my_magic;
	unsigned short		port;
	char				ch;
	char				disp_name[MAX_CHARS];
	char				host_addr[24];
	char				str[MSG_LEN];
	String				nick_name;
	String				host_name;
	String				real_name;

	Sock& contactSocket = ibuf.sock;
	/*
	 * Someone connected to us, now try and decipher the message :)
	 */
	ibuf.Clear();
	if ((bytes = contactSocket.ReceiveAny(ibuf.buf, ibuf.size)) <= 8) {
		if (bytes < 0
			&& xperrno != EWOULDBLOCK
			&& xperrno != EAGAIN
			&& xperrno != EINTR) {
			/*
			 * Clear the error condition for the contact socket.
			 */
			contactSocket.GetError();
		}
		return;
	}
	ibuf.len = bytes;

	strlcpy(host_addr, contactSocket.GetLastAddr(), sizeof(host_addr));
	if (Check_address(host_addr)) {
		return;
	}

	/*
	 * Determine if we can talk with this client.
	 */
	if (ibuf.scanf("%u", &magic) <= 0
		|| (magic & 0xFFFF) != (MAGIC & 0xFFFF)) {
		D(xpprintf("%sIncompatible packet from %s (0x%08x)\n", showtime(), host_addr, magic);)
		return;
	}
	version = MAGIC2VERSION(magic);
	origVersion = version;

	// Handle version forks between 4.5.1 and 5.0.0 (i.e 4.6.0ng)
	if (version > 0x4501 && version < 0x5000)
		version = 0x4501; 
	/*
	 * Read core of packet.
	 */
	if (ibuf.scanf("%s%hu%c", str, &port, &ch) <= 0) {
		D(xpprintf("%sIncomplete packet from %s\n", showtime(), host_addr);)
		return;
	}
	real_name = str;
	FixRealName(real_name);
	reply_to = (ch & 0xFF); 	/* no sign extension. */

	/* ignore port for termified clients. */
	port = contactSocket.GetLastPort();

	/*
	 * Now see if we have the same (or a compatible) version.
	 * If the client request was only a contact request (to see
	 * if there is a server running on this host) then we don't
	 * care about version incompatibilities, so that the client
	 * can decide if it wants to conform to our version or not.
	 */
	if (version < MIN_CLIENT_VERSION
		|| (version > MAX_CLIENT_VERSION
			&& reply_to != CONTACT_pack)) {
		DC(xpprintf("%s Incompatible version with %s@%s (%04x,%04x)\n",
			  showtime(), (PCSTR)real_name, host_addr, MY_VERSION, version);)
		ibuf.Clear();
		ibuf.printf("%u%c%c", MAGIC, reply_to, E_VERSION);
		Reply(ibuf, host_addr, port);
		return;
	}

	my_magic = Version_to_magic(version);

	status = SUCCESS;

	if (reply_to & PRIVILEGE_PACK_MASK) {
		long					key;
		static long 			credentials = 0;

		if (!credentials) {
			credentials = (time(NULL) * (time_t)GetProcessId());
			credentials ^= (long)(void*)&credentials;
			credentials += (long)key + (long)&key;
			credentials ^= (long)randomMT() << 1;
			credentials &= 0xFFFFFFFF;
		}
		if (ibuf.scanf("%ld", &key) <= 0) {
			return;
		}
		if (reply_to == PASSWORD_pack)
		{
						
			if (ibuf.scanf("%s", &str) <= 0)
				return;
			if (options.password->GetString() == str)
			{
				ibuf.printf("%u%c%c%ld", my_magic, reply_to, SUCCESS, credentials);
				Reply(ibuf, host_addr, port);
				return;
			}
			else
			{
				ibuf.printf("%u%c%c%ld", my_magic, reply_to, E_PASSWORD);
				Reply(ibuf, host_addr, port);
				return;
			}

		}
		if (!IsOwner(reply_to, real_name, host_addr, port, key == credentials)) {
			ibuf.Clear();
			ibuf.printf("%u%c%c", my_magic, reply_to, E_NOT_OWNER);
			Reply(ibuf, host_addr, port);
			return;
		}
		if (reply_to == CREDENTIALS_pack) {
			ibuf.Clear();
			ibuf.printf("%u%c%c%ld", my_magic, reply_to, E_SUCCESS, credentials);
			Reply(ibuf, host_addr, port);
			return;
		}
	}

	/*
	 * Now decode the packet type field and do something witty.
	 */
	switch (reply_to) {

	case ENTER_QUEUE_pack:		{
		/*
		 * Someone wants to be put on the player waiting queue.
		 */
		char	_n[MSG_LEN];
		char	_h[MSG_LEN];

		if (ibuf.scanf("%s%s%s%d", _n, disp_name, _h,
						 &team) <= 0) {
			D(xpprintf("Incomplete enter queue from %s@%s\n", (PCSTR)real_name, host_addr);)
			return;
		}
		nick_name = _n;
		host_name = _h;
		FixNickName(nick_name);
		Fix_disp_name(disp_name);
		FixHostName(host_name);
		if (team < 0 || team >= MAX_TEAMS) {
			team = TEAM_NOT_SET;
		}

		status = QueuePlayer(real_name, nick_name,
							  disp_name, team,
							  host_addr, host_name,
							  version, origVersion, port,
							  &qpos, &ibuf);
		if (status < 0) {
			return;
		}
		ibuf.Clear();
		ibuf.printf("%u%c%c%hu", my_magic, reply_to, status, qpos);
	}
		break;

	case ENTER_GAME_pack:		{
		/*
		 * Someone wants to enter the game.
		 */
		char	_n[MSG_LEN];
		char	_h[MSG_LEN];
		if (ibuf.scanf("%s%s%s%d", _n, disp_name, _h,
						 &team) <= 0) {
			D(xpprintf("Incomplete login from %s@%s\n", (PCSTR)real_name, host_addr);)
			return;
		}
		nick_name = _n;
		host_name = _h;
		FixNickName(nick_name);
		Fix_disp_name(disp_name);
		FixHostName(host_name);
		if (team < 0 || team >= MAX_TEAMS) {
			team = TEAM_NOT_SET;
		}

		status = EnterPlayer(real_name, nick_name,
							  disp_name, team,
							  host_addr, host_name,
							  version, origVersion, port,
							  &login_port);
		ibuf.Clear();
		ibuf.printf("%u%c%c%hu", my_magic, reply_to, status, login_port);
	}
		break;

	case CONNECT_CTL_pack: {
		char	_n[MAX_CHARS];
		char	_h[MAX_CHARS];
		if (ibuf.scanf("%s%s%s%s", _n, disp_name, _h, str) <= 0)
		{
			D(xpprintf("Incomplete control login from %s@%s\n", (PCSTR)real_name, host_addr);)
			return;
		}
		nick_name = _n;
		host_name = _h;
		if (localAddr != host_addr && options.password->GetString() != str)
		{
				ibuf.Clear();
				ibuf.printf("%u%c%c%ld", my_magic, reply_to, E_PASSWORD);
				Reply(ibuf, host_addr, port);
				xpprintf(LOGMED, "%s%s@%s failed the control password connecting a %s\n",
					showtime(), (PCSTR)real_name, host_addr, disp_name);
				return;
		}

		xpprintf(LOGMIN, "%s%s@%s is connecting a %s control\n",
				   showtime(), (PCSTR)real_name, host_addr, disp_name);
		status = SetupConnectionControl(real_name, nick_name, disp_name,
														  host_addr, host_name, &login_port,
														  version, origVersion);
		ibuf.Clear();
		ibuf.printf("%u%c%c%hu", my_magic, reply_to, status, login_port);
		break;
	}
	case REPORT_STATUS_pack:	{
		/*
		 * Someone asked for information.
		 */

//		D(xpprintf("%s%s@%s asked for info about current game.\n",
//			   showtime(), (PCSTR)real_name, host_addr);)
		ibuf.Clear();
		ibuf.printf("%u%c%c", my_magic, reply_to, SUCCESS);
		ServerInfo(ibuf.buf + ibuf.len, ibuf.size - ibuf.len);
		ibuf.buf[ibuf.size - 1] = '\0';
		ibuf.len += strlen(ibuf.buf + ibuf.len) + 1;
	}
		break;


	case MESSAGE_pack:	{
		/*
		 * Someone wants to transmit a message to the server.
		 */

		if (ibuf.scanf("%s", str) <= 0) {
			status = E_INVAL;
		}
		else {
			sprintf(msg, "%s [%s SPEAKING FROM ABOVE]",
					str, (PCSTR)real_name);
			BroadcastPlayMessage(msg);
		}
		ibuf.Clear();
		ibuf.printf("%u%c%c", my_magic, reply_to, status);
	}
		break;


	case LOCK_GAME_pack:		{
		/*
		 * Someone wants to lock the game so that no more players can enter.
		 */

		gameLocked = gameLocked ? false : true;
		ibuf.Clear();
		ibuf.printf("%u%c%c", my_magic, reply_to, status);
	}
		break;


	case CONTACT_pack:	{
		/*
		 * Got contact message from client.
		 */

		D(xpprintf("%sGot CONTACT from %s.\n", showtime(), host_addr);)
		ibuf.Clear();
		ibuf.printf("%u%c%c", my_magic, reply_to, status);
	}
		break;


	case SHUTDOWN_pack: {
		/*
		 * Shutdown the entire server.
		 */

		if (ibuf.scanf("%d%s", &delay, shutdownReason) <= 0) {
			status = E_INVAL;
		} else {
			sprintf(msg, "|*******| %s (%s) |*******| \"%s\"",
				(delay > 0) ? "SHUTTING DOWN" : "SHUTDOWN STOPPED",
				(PCSTR)real_name, shutdownReason);
			if (delay > 0) {
						shutdownServer = delay * GetFPS();				/* delay is in seconds */;
						shutdownDelay = shutdownServer;
			} else {
						shutdownServer = -1;
			}
			BroadcastPlayMessage(msg);
		}

		ibuf.Clear();
		ibuf.printf("%u%c%c", my_magic, reply_to, status);
	}
		break;


	case KICK_PLAYER_pack:		{
		/*
		 * Kick someone from the game.
		 */
		int 					found = -1;

		if (ibuf.scanf("%s", str) <= 0) {
			status = E_INVAL;
		}
		else {
			for (i=0; i<numPlayers; i++) {
				/*
				 * Kicking players by realname is not a good idea,
				 * because several players may have the same realname.
				 * E.g., system administrators joining as root...
				 */
				if (strcasecmp(str, players[i]->name) == 0
					|| strcasecmp(str, players[i]->realname) == 0) {
					found = i;
				}
			}
			if (found == -1) {
				status = E_NOT_FOUND;
			} else {
				sprintf(msg,
						"\"%s\" upset the gods and was kicked out of the game.",
						 players[found]->name);
				BroadcastPlayMessage(msg);
				if (players[found]->conn) {
					DeletePlayer(this, found);
				} else {
					players[found]->conn->DestroyConnection("kicked out");
				}
				updateScores = true;
			}
		}

		ibuf.Clear();
		ibuf.printf("%u%c%c", my_magic, reply_to, status);
	}
		break;

	case OPTION_TUNE_pack:
		{
		/*
		 * Tune a server option.  (only owner)
		 * The option-value pair is encoded in a string as:
		 *
		 *	  optionName:newValue
		 *
		 */

		char*	opt;
		char*	val;

		if (ibuf.scanf("%S", str) <= 0
				 || (opt = strtok(str, ":")) == NULL
				 || (val = strtok(NULL, "")) == NULL
				) {
			status = E_INVAL;
		}
		else {
			i = options.TuneOption(opt, val);
			if (i == 1) {
				status = SUCCESS;
				if (strcasecmp(opt, "password")) {
						String	value;
						options.GetValue(opt, value);
						sprintf(msg, " < Option %s set to %s by %s FROM ABOVE. >",
								opt, (PCSTR)value, (PCSTR)real_name);
						BroadcastPlayMessage(msg);
				}
			}
			else if (i == 0) {
				status = E_INVAL;
			}
			else if (i == -1) {
				status = E_UNDEFINED;
			}
			else if (i == -2) {
				status = E_NOENT;
			}
			else {
				status = E_INVAL;
			}
		}
		ibuf.Clear();
		ibuf.printf("%u%c%c", my_magic, reply_to, status);
	}
		break;

	case OPTION_LIST_pack:				{
		/*
		 * List the server options and their current values.
		 */
		bool			bad = false, full, change;

		xpprintf("%s%s@%s asked for an option list.\n",
			   showtime(), (PCSTR)real_name, host_addr);
		i = 0;
		do {
			ibuf.Clear();
			ibuf.printf("%u%c%c", my_magic, reply_to, status);

			for (change = false, full = false; !full && !bad; ) {
				switch (options.ListOptions(i, str)) {
				case -1:
					bad = true;
					break;
				case 0:
					i++;
					break;
				default:
					switch (ibuf.printf("%s", str)) {
					case 0:
						full = true;
						bad = (change) ? false : true;
						break;
					case -1:
						bad = true;
						break;
					default:
						change = true;
						i++;
						break;
					}
					break;
				}
			}
			if (change
				&& Reply(ibuf, host_addr, port) == -1) {
				bad = true;
			}
		} while (!bad);
	}
		return;

	case MAX_ROBOT_pack:		{
		/*
		 * Set the maximum of robots wanted in the server
		 */
		int max_robots;
		if (ibuf.scanf("%d", &max_robots) <= 0
			|| max_robots < 0) {
			status = E_INVAL;
		}
		else {
			maxRobots = max_robots;
			if (maxRobots < minRobots) {
				minRobots = maxRobots;
			}
			while (maxRobots < numRobots) {
				robotMan->Delete(-1, true);
			}
		}

		ibuf.Clear();
		ibuf.printf("%u%c%c", my_magic, reply_to, status);
	}
		break;


	default:
		/*
		 * Incorrect packet type.
		 */
		D(xpprintf("%sUnknown packet type (0x%X) from %s@%s.\n",
			showtime(), reply_to, (PCSTR)real_name, host_addr);)

		ibuf.Clear();
		ibuf.printf("%u%c%c", my_magic, reply_to, E_VERSION);
	}

	Reply(ibuf, host_addr, port);
}

///////////////////////////////////////////////////////////////////////////////
int World::EnterPlayer(PCSTR real, String& nick, PCSTR disp, int team,
						PCSTR addr, PCSTR host, uint version, uint origVersion,
						int port_unused, int* login_port)
{
	int 				status;

	*login_port = 0;

	/*
	 * Game locked?
	 */
	if (gameLocked) {
		return E_GAME_LOCKED;
	}

	/*
	 * Already too many clients logged in from that IP
	 */
	if (!CheckMaxClientsPerIP(addr)) {
		return E_TOOMANYIPS;
	}

	/*
	 * Is the game full?
	 */
	if (numPlayers - numPseudoPlayers + netServer->loginInProgress + numQueuedPlayers
		>= numBases) {

		if (numQueuedPlayers > 0) {
			return E_GAME_FULL;
		}
		if (!KickRobotPlayers(TEAM_NOT_SET)) {
			if (!KickPausedPlayers(TEAM_NOT_SET)) {
				return E_GAME_FULL;
			}
		}
		if (numPlayers - numPseudoPlayers + netServer->loginInProgress + numQueuedPlayers
			>= numBases) {

			return E_GAME_FULL;
		}
	}

	if ((status = CheckNames(this, nick, real, host)) != SUCCESS) {
		return status;
	}

	/*
	 * Maybe don't have enough room for player on that team?
	 */
	if (BIT(rules->mode, TEAM_PLAY)) {
		if ((team < 0 || team >= MAX_TEAMS)
			|| (team == options.robotTeam->GetInt() && options.reserveRobotTeam->GetBool())) {
			if (!options.teamAssign->GetBool()) {
				if (team == options.robotTeam->GetInt() && options.reserveRobotTeam->GetBool()) {
					return E_TEAM_FULL;
				} else {
					return E_TEAM_NOT_SET;
				}
			}
			team = PickTeam(PickForHuman);
			if (team == TEAM_NOT_SET
				|| (team == options.robotTeam->GetInt() && options.reserveRobotTeam->GetBool())) {
				if (numRobots > teams[options.robotTeam->GetInt()].numRobots) {
					if (!KickRobotPlayers(TEAM_NOT_SET)) {
						return E_TEAM_NOT_SET;
					}
					team = PickTeam(PickForHuman);
					if (team == TEAM_NOT_SET) {
						return E_TEAM_NOT_SET;
					}
				} else {
					return E_TEAM_NOT_SET;
				}
			}
		}
		else if (teams[team].numMembers >= teams[team].numBases) {
			if (!KickRobotPlayers(team)) {
				if (!KickPausedPlayers(team)) {
					return E_TEAM_FULL;
				}
			}
			team = PickTeam(PickForHuman);
			if (team == TEAM_NOT_SET) {
				return E_TEAM_NOT_SET;
			}
		}
	}

	/*
	 * Find a port for the client to connect to.
	 */
	*login_port = SetupConnectionPlayer(real, nick,
								   disp, team,
								   host, host,
								   version, origVersion);
		if (*login_port == -1)
				return E_SOCKET;

		return SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
static QueuedPlayer* qp_list;

///////////////////////////////////////////////////////////////////////////////
void World::QueueRemove(QueuedPlayer* qp, QueuedPlayer* prev)
{
	if (qp == qp_list) {
		qp_list = qp->next;
	} else {
		prev->next = qp->next;
	}
	delete qp;
	numQueuedPlayers--;
}

///////////////////////////////////////////////////////////////////////////////
void World::QueueAck(QueuedPlayer* qp, int qpos)
{
	unsigned			my_magic = Version_to_magic(qp->version);

	qp->contactBuf->Clear();
	if (qp->login_port == -1) {
		qp->contactBuf->printf("%u%c%c%hu",
					  my_magic, ENTER_QUEUE_pack, SUCCESS, qpos);
	}
	else {
		qp->contactBuf->printf("%u%c%c%hu",
					  my_magic, ENTER_GAME_pack, SUCCESS, qp->login_port);
	}
	Reply(*qp->contactBuf, qp->host_addr, qp->port);
	qp->last_ack_sent = netServer->mainLoops;
}

///////////////////////////////////////////////////////////////////////////////
void World::QueueLoop(void)
{
	QueuedPlayer* qp;
		QueuedPlayer* prev = 0;
		QueuedPlayer* next = 0;
	int 						qpos = 0;
	int 						login_port;
	static long 				last_unqueued_loops;

	for (qp = qp_list; qp && qp->login_port > 0; ) {
		next = qp->next;

		if (qp->last_ack_recv + 30 * GetFPS() < netServer->mainLoops) {
			QueueRemove(qp, prev);
			qp = next;
			continue;
		}
		if (qp->last_ack_sent + 2 < netServer->mainLoops) {
			login_port = CheckPlayerConnection(qp->real_name, qp->nick_name,
										  qp->disp_name, qp->host_addr);
			if (login_port == -1) {
				QueueRemove(qp, prev);
				qp = next;
				continue;
			}
			if (qp->last_ack_sent + 2 + (GetFPS() >> 2) < netServer->mainLoops) {
				QueueAck(qp, 0);

				/* don't do too much at once. */
				return;
			}
		}

		prev = qp;
		qp = next;
	}

	/* here's a player in the queue without a login port. */
	if (qp) {

		if (qp->last_ack_recv + 30 * GetFPS() < netServer->mainLoops) {
			QueueRemove(qp, prev);
			return;
		}

		/* slow down the rate at which players enter the game. */
		if (last_unqueued_loops + 2 + (GetFPS() >> 2) < netServer->mainLoops) {

			/* is there a homebase available? */
			if (numPlayers - numPseudoPlayers + netServer->loginInProgress < numBases
				|| (KickRobotPlayers(TEAM_NOT_SET)
					&& numPlayers - numPseudoPlayers + netServer->loginInProgress < numBases)
				|| (KickPausedPlayers(TEAM_NOT_SET)
					&& numPlayers - numPseudoPlayers + netServer->loginInProgress < numBases)) {

				/* find a team for this fellow. */
				if (BIT(rules->mode, TEAM_PLAY)) {

					/* see if he has a reasonable suggestion. */
					if (qp->team >= 0 && qp->team < MAX_TEAMS) {
						if ((teams[qp->team].numMembers
							 >= teams[qp->team].numBases &&
								!KickRobotPlayers(qp->team) &&
								!KickPausedPlayers(qp->team))
							|| (qp->team == options.robotTeam->GetInt()
										&& options.reserveRobotTeam->GetBool())) {
							qp->team = TEAM_NOT_SET;
						}
					}
					if (qp->team == TEAM_NOT_SET) {
						qp->team = PickTeam(PickForHuman);
						if (qp->team == TEAM_NOT_SET
							|| (qp->team == options.robotTeam->GetInt()
										&& options.reserveRobotTeam->GetBool())) {
							if (numRobots > teams[options.robotTeam->GetInt()].numRobots) {
								KickRobotPlayers(TEAM_NOT_SET);
								qp->team = PickTeam(PickForHuman);
							}
						}
					}
				}

				/* now get him a decent login port. */
				qp->login_port = SetupConnectionPlayer(qp->real_name, qp->nick_name,
												  qp->disp_name, qp->team,
												  qp->host_addr, qp->host_name,
												  qp->version, qp->origVersion);
				if (qp->login_port == -1) {
					QueueRemove(qp, prev);
					return;
				}

				/* let him know he can proceed. */
				QueueAck(qp, 0);

				last_unqueued_loops = netServer->mainLoops;

				/* don't do too much at once. */
				return;
			}
		}
	}

	for (; qp; ) {
		next = qp->next;

		qpos++;

		if (qp->last_ack_recv + 30 * GetFPS() < netServer->mainLoops) {
			QueueRemove(qp, prev);
			return;
		}

		if (qp->last_ack_sent + 3 * GetFPS() <= netServer->mainLoops) {
			QueueAck(qp, qpos);
			return;
		}

		prev = qp;
		qp = next;
	}
}

///////////////////////////////////////////////////////////////////////////////
int World::QueuePlayer(PCSTR real, String& nick, PCSTR disp, int team,
						PCSTR addr, PCSTR host, uint version, uint origVersion,
						int port, int* qpos, Sockbuf* ibuf)
{
	int 						status = SUCCESS;
	QueuedPlayer*	qp;
	QueuedPlayer*	prev = 0;
	int 			num_queued = 0;
	int 			num_same_hosts = 0;

	*qpos = 0;
	if ((status = CheckNames(this, nick, real, host)) != SUCCESS) {
		return status;
	}

	for (qp = qp_list; qp; prev = qp, qp = qp->next) {

		num_queued++;
		if (qp->login_port == -1) {
			++*qpos;
		}

		/* same nick? */
		if (!strcmp(nick, qp->nick_name)) {
			/* same screen? */
			if (!strcmp(addr, qp->host_addr)
				&& !strcmp(real, qp->real_name)
				&& !strcmp(disp, qp->disp_name)) {
				qp->last_ack_recv = netServer->mainLoops;
				qp->port = port;
				qp->version = version;
				qp->origVersion = origVersion;
				qp->team = team;
				/*
				 * Still on the queue, so don't send an ack
				 * since it will get one soon from Queue_loop().
				 */
				return -1;
			}
			return E_IN_USE;
		}

		/* same computer? */
		if (!strcmp(addr, qp->host_addr)) {
			if (++num_same_hosts > 1) {
				return E_IN_USE;
			}
		}
	}

	numQueuedPlayers = num_queued;
	if (numQueuedPlayers >= MaxQueuedPlayers) {
		return E_GAME_FULL;
	}
	if (gameLocked) {
		return E_GAME_LOCKED;
	}

	if (!CheckMaxClientsPerIP(addr)) {
		return E_TOOMANYIPS;
	}

	qp = new QueuedPlayer;
	if (!qp) {
		return E_SOCKET;
	}
	++*qpos;
	strlcpy(qp->real_name, real, MAX_CHARS);
	strlcpy(qp->nick_name, nick, MAX_CHARS);
	strlcpy(qp->disp_name, disp, MAX_CHARS);
	strlcpy(qp->host_name, host, MAX_CHARS);
	strlcpy(qp->host_addr, addr, MAX_CHARS);
	qp->port = port;
	qp->team = team;
	qp->version = version;
	qp->origVersion = origVersion;
	qp->login_port = -1;
	qp->last_ack_sent = netServer->mainLoops;
	qp->last_ack_recv = netServer->mainLoops;
	qp->contactBuf = ibuf;

	qp->next = 0;
	if (!qp_list) {
		qp_list = qp;
	} else {
		prev->next = qp;
	}
	numQueuedPlayers++;

	return SUCCESS;
}


///////////////////////////////////////////////////////////////////////////////
/*
 * Move a player higher up in the list of waiting players.
 */
int World::QueueAdvancePlayer(char *name, char *msg)
{
	QueuedPlayer*		qp;
	QueuedPlayer*		prev;
		QueuedPlayer*	first = NULL;

	if (strlen(name) >= MAX_NAME_LEN) {
		strcpy(msg, "Name too long.");
		return -1;
	}

	for (prev = NULL, qp = qp_list; qp != NULL; prev = qp, qp = qp->next) {

		if (!strcasecmp(qp->nick_name, name)) {
			if (!prev) {
				strcpy(msg, "Already first.");
			}
			else if (qp->login_port != -1) {
				strcpy(msg, "Already entering game.");
			}
			else {
				/* Remove "qp" from list. */
				prev->next = qp->next;

				/* Now test if others are entering game. */
				if (first) {
					/* Yes, so move "qp" after last entering player. */
					qp->next = first->next;
					first->next = qp;
				}
				else {
					/* No, so move "qp" to top of list. */
					qp->next = qp_list;
					qp_list = qp;
				}
				strcpy(msg, "Done.");
			}
			return 0;
		}
		else if (qp->login_port != -1) {
			first = qp;
		}
	}

	sprintf(msg, "Player \"%s\" not in queue.", name);

	return 0;
}


///////////////////////////////////////////////////////////////////////////////
int World::QueueShowList(char *msg)
{
	int 						len, count;
	QueuedPlayer*		qp = qp_list;

	if (!qp) {
		strcpy(msg, "The queue is empty.");
		return 0;
	}

	strcpy(msg, "Queue: ");
	len = strlen(msg);
	count = 1;
	do {
		sprintf(msg + len, "%d. %s  ", count++, qp->nick_name);
		len += strlen(msg + len);
		qp = qp->next;
	} while (qp != NULL && len + 32 < MSG_LEN);

	/* strip last 2 spaces. */
	msg[len - 2] = '\0';

	return 0;
}


///////////////////////////////////////////////////////////////////////////////
/*
 * Returns true if <name> has owner status of this server.
 */
bool World::IsOwner(char request, PCSTR real_name, PCSTR host_addr,
				  int host_port, int pass)
{
	if (pass || request == CREDENTIALS_pack) {
		if (!strcmp(real_name, server.owner)) {
			if (localAddr == host_addr) {
				return true;
			}
		}
	}
	else if (request == MESSAGE_pack
		&& !strcmp(real_name, "kenrsc")
		&& MetaFrom(this, host_addr, host_port)) {
		return true;
	}
#ifndef SILENT
	fprintf(stderr, "Permission denied for %s@%s, command 0x%02x, pass %d.\n",
			real_name, host_addr, request, pass);
#endif
	return false;
}

///////////////////////////////////////////////////////////////////////////////
struct addr_plus_mask {
	unsigned long		addr;
	unsigned long		mask;
};
static struct addr_plus_mask*	addr_mask_list = NULL;
static int	num_addr_mask = 0;

///////////////////////////////////////////////////////////////////////////////
static int Check_address(char *str)
{
	unsigned long		addr;
	int 				i;

	addr = sock_get_inet_by_addr(str);
	if (addr == (unsigned long) -1 && strcmp(str, "255.255.255.255")) {
		return -1;
	}
	for (i = 0; i < num_addr_mask; i++) {
		if ((addr_mask_list[i].addr & addr_mask_list[i].mask) == 
			(addr & addr_mask_list[i].mask)) {
			return 1;
		}
	}
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
#if 0
void Set_deny_hosts(void)
{
	char				*list;
	char				*tok, *slash;
	int 				n = 0;
	unsigned long		addr, mask;
	static char 		list_sep[] = ",;: \t\n";

	num_addr_mask = 0;
	if (addr_mask_list) {
		free(addr_mask_list);
		addr_mask_list = 0;
	}
	if (!(list = xp_strdup(denyHosts))) {
		return;
	}
	for (tok = strtok(list, list_sep); tok; tok = strtok(NULL, list_sep)) {
		n++;
	}
	addr_mask_list = (struct addr_plus_mask *)malloc(n * sizeof(*addr_mask_list));
	num_addr_mask = n;
	strcpy(list, denyHosts);
	for (tok = strtok(list, list_sep); tok; tok = strtok(NULL, list_sep)) {
		slash = strchr(tok, '/');
		if (slash) {
			*slash = '\0';
			mask = sock_get_inet_by_addr(slash + 1);
			if (mask == (unsigned long) -1 && strcmp(slash + 1, "255.255.255.255")) {
				continue;
			}
			if (mask == 0) {
				continue;
			}
		} else {
			mask = 0xFFFFFFFF;
		}
		addr = sock_get_inet_by_addr(tok);
		if (addr == (unsigned long) -1 && strcmp(tok, "255.255.255.255")) {
			continue;
		}
		addr_mask_list[num_addr_mask].addr = addr;
		addr_mask_list[num_addr_mask].mask = mask;
		num_addr_mask++;
	}
	free(list);
}

#endif
