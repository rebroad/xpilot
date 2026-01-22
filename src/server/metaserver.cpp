/* $Id: metaserver.cpp,v 1.15 2004/05/22 14:50:18 dick Exp $
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
 *
 *  $Log: metaserver.cpp,v $
 *  Revision 1.15  2004/05/22 14:50:18  dick
 *  Remove #ifndef SILENT
 *
 *  Revision 1.14  2004/02/18 01:23:38  dick
 *  netserver.h becomes NetServer.h
 *
 *  Revision 1.13  2004/02/16 14:29:44  dick
 *  server/sched.cpp and friends become serverlib/NetServer
 *  which disconnects the udp listen and receive functionality from the game.
 *
 *  Revision 1.12  2004/02/03 03:39:54  dick
 *  Add Windows precompiled header support.
 *
 *  Revision 1.11  2004/01/16 17:32:22  dick
 *  Fix ReportToMetaTuner().  If initially disabled, enabling reportToMeta while
 *  running requires calling MetaInit() to lookup the metaservers.
 *
 *  Revision 1.10  2004/01/12 05:18:21  dick
 *  MetaUpdate takes a bool, not an int
 *
 *  Revision 1.9  2004/01/09 04:48:18  dick
 *  Move the meta defs from #define to String
 *
 *  Revision 1.8  2004/01/06 04:48:28  dick
 *  Add support for building under Cygwin.  Still needs work and the output
 *  is X11 and requires something like Cygwin's XWin.exe to run under.
 *
 *  Revision 1.7  2002/09/16 21:48:00  dick
 *  Function parameters need const.
 *
 *  Revision 1.6  2002/09/01 00:13:46  dick
 *  Remove obsolete windows #include "winServer.h"
 *
 *  Revision 1.5  2002/08/31 22:09:29  dick
 *  metaserver won't become part of World, so rename it back to metaserver.cpp
 *
 *  Revision 1.14  2002/08/01 14:36:07  dick
 *  Add some debug to figure out why meta is not updated as players leave.
 *
 *  Revision 1.13  2002/07/23 18:00:35  dick
 *  Some debug trying to figure out why frank (win2k) comes and goes from meta.
 *
 *  Revision 1.12  2002/07/12 15:36:48  dick
 *  Open a listen port on localhost (127.0.0.1) as well as any requested.
 *
 *  Revision 1.11  2002/07/08 05:52:59  dick
 *  showtime() handles the space after itself
 */
#include "StdAfx.h"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <time.h>
#include <sys/types.h>

#if defined(_UNIX) || defined(_CYGWIN)
# include <unistd.h>
# ifndef __hpux
#  include <sys/time.h>
# endif
#endif

#include "config.h"
#include "version.h"
#include "serverconst.h"
#include "portability.h"
#include "types.h"
#include "global.h"
#include "proto.h"
#include "Sock.h"
#include "pack.h"
#include "metaserver.h"
#include "saudio.h"
#include "error.h"
#include "NetServer.h"
#include "commonproto.h"
#include "showtime.h"

#if defined(_WINDOWS) && !defined(_CYGWIN)
#define	META_VERSION	TITLE
#else
#define META_VERSION	VERSION
#endif

char metaserver_version[] = VERSION;

const int	META_PORT		= 5500;
const char*	META_HOST		= "meta.xpilot.org";
const char*	META_HOST_TWO	= "meta2.xpilot.org";
//const char*	META_IP			= "129.242.16.151";
//const char*	META_IP_TWO		= "132.235.197.27";


struct MetaServer {
    String		name;
    String		addr;
};
struct MetaServer	metaServers[2];
/*
 = {
    {
	META_HOST,
	META_IP
    },
    {
	META_HOST_TWO,
	META_IP_TWO
    },
};
*/
static char	msg[MSG_LEN];

extern time_t	serverStartTime;

void MetaSend(World* w, PCSTR mesg, int len)
{
	int			i;

	if (!w->options.reportToMetaServer->GetBool())
		return;

	//D(xpprintf("%sMetaSend: %.20s\n", showtime(), mesg);)
	for (i = 0; i < NELEM(metaServers); i++) {
		if (w->remoteContactSocket.SendDest(metaServers[i].addr, META_PORT, mesg, len) != len) {
			w->remoteContactSocket.GetError();
			w->remoteContactSocket.SendDest(metaServers[i].addr, META_PORT, mesg, len);
		}
	}
}

int MetaFrom(World* w, PCSTR addr, int port)
{
	int			i;

	for (i = 0; i < NELEM(metaServers); i++) {
		if (!strcmp(addr, metaServers[i].addr)) {
			return (port == META_PORT);
		}
	}
	return 0;
}

void MetaGone(World* w)
{
	if (w->options.reportToMetaServer->GetBool()) {
		sprintf(msg, "server %s\nremove", (PCSTR)w->server.host);
		MetaSend(w, msg, strlen(msg) + 1);
	}
}

void MetaInit(World* w)
{
	int		i;
	String	addr;

	if (!w->options.reportToMetaServer->GetBool()) {
		return;
	}

	metaServers[0].name = META_HOST;
//	metaServers[0].addr = META_IP;
	metaServers[1].name = META_HOST_TWO;
//	metaServers[1].addr = META_IP_TWO;

	xpprintf("%sLocating Internet Meta server... ", showtime()); fflush(stdout);
	for (i = 0; i < NELEM(metaServers); i++) {
		if (metaServers[i].addr.IsEmpty())
		{
			addr = sock_get_addr_by_name(metaServers[i].name);
			if (IsServerKilled())
				return;
			if (addr.GetLength())
				metaServers[i].addr = addr;
		}
		if (addr)
			xpprintf("found %d", i + 1);
		else
			xpprintf("%d not found", i + 1);
		if (i + 1 == NELEM(metaServers))
			xpprintf("\n");
		else
			xpprintf("... ");
		fflush(stdout);
    }
}

void MetaUpdate(World* w, bool change)
{
#ifdef SOUND
#define SOUND_SUPPORT_STR	"yes"
#else
#define SOUND_SUPPORT_STR	"no"
#endif
#define GIVE_META_SERVER_A_HINT	180

	char 		string[MAX_STR_LEN];
	int			i, j;
	int			len;
	int			num_active_players;
	bool		first = true;
	time_t		currentTime;
	const char		*game_mode;
	char		freebases[120];
	int			active_per_team[MAX_TEAMS];
	static time_t	lastMetaSendTime = 0;
	static int		queue_length = 0;


	if (!w->options.reportToMetaServer->GetBool())
		return;

	currentTime = time(NULL);
	if (!change) {
		if (currentTime - lastMetaSendTime < GIVE_META_SERVER_A_HINT) {
			if (w->numQueuedPlayers == queue_length 
			 ||	currentTime - lastMetaSendTime < 5) {
				return;
			}
		}
	}
	lastMetaSendTime = currentTime;
	queue_length = w->numQueuedPlayers;

	/* Find out the number of active players. */
	num_active_players = 0;
	memset(active_per_team, 0, sizeof active_per_team);
	for (i = 0; i < w->numPlayers; i++) {
		if (IS_HUMAN_IND(w, i) && !BIT(w->players[i]->status, PAUSE)) {
			num_active_players++;
			if (BIT(w->rules->mode, TEAM_PLAY)) {
				active_per_team[i]++;
			}
		}
	}

	game_mode = (w->gameLocked && w->shutdownServer == -1) ? "locked"
	: (!w->gameLocked && w->shutdownServer != -1) ? "shutting down"
	: (w->gameLocked && w->shutdownServer != -1) ? "locked and shutting down"
	: "ok";

	/* calculate number of available homebases per team. */
	freebases[0] = '\0';
	if (BIT(w->rules->mode, TEAM_PLAY)) {
		j = 0;
		for (i = 0; i < MAX_TEAMS; i++) {
			if (i == w->options.robotTeam->GetInt() 
			 && w->options.reserveRobotTeam->GetBool()) {
				continue;
			}
			if (w->teams[i].numBases > 0) {
				sprintf(&freebases[j], "%d=%d,", i,
						w->teams[i].numBases - active_per_team[i]);
				j += strlen(&freebases[j]);
			}
		}
		/* strip trailing comma. */
		if (j) {
			freebases[j-1] = '\0';
		}
	}
	else {
		sprintf(freebases, "=%d",
		w->numBases - num_active_players - netServer->loginInProgress);
	}

	String	version;
	version = META_VERSION;
	if (w->scoreServer)
		version += "-rank";

	sprintf(string,
			"add server %s\n"
			"add users %d\n"
			"add version %s\n"
			"add map %s\n"
			"add sizeMap %3dx%3d\n"
			"add author %s\n"
			"add bases %d\n"
			"add fps %d\n"
			"add port %d\n"
			"add mode %s\n"
			"add teams %d\n"
			"add free %s\n"
			"add timing %d\n"
			"add stime %ld\n"
			"add queue %d\n"
			"add sound " SOUND_SUPPORT_STR "\n",
			(PCSTR)w->server.host, num_active_players,
			(PCSTR)version, (PCSTR)w->options.mapName->GetString(),
			w->blockWidth, w->blockHeight, (PCSTR)w->options.mapAuthor->GetString(),
			w->numBases, w->GetFPS(), w->options.contactPort->GetInt(),
			game_mode, w->numTeamBases, freebases,
			BIT(w->rules->mode, TIMING) ? 1:0,
			(long)(time(NULL) - serverStartTime),
			queue_length);


	/*
	* 'len' must always hold the exact number of
	* non-zero bytes which are in string[].
	*/
	len = strlen(string);

	for (i = 0; i < w->numPlayers; i++) {
		if (IS_HUMAN_IND(w, i) && !BIT(w->players[i]->status, PAUSE)) {
			if ((len + (4 * MAX_CHARS)) < sizeof(string)) {
			sprintf(string + len,
					"%s%s=%s@%s",
					(first) ? "add players " : ",",
					w->players[i]->name,
					w->players[i]->realname,
					w->players[i]->hostname);
			len += strlen(&string[len]);

			if (BIT(w->rules->mode, TEAM_PLAY)) {
				sprintf(string + len,"{%d}",w->players[i]->team);
				len += strlen(&string[len]);
			}

			first = false;
			}
		}
	}

	if (len + MSG_LEN < sizeof(string)) {
		char status[MAX_STR_LEN];

		strlcpy(&string[len], "\nadd status ", sizeof(string) - len);
		len += strlen(&string[len]);

		w->ServerInfo(status, sizeof(status));

		strlcpy(&string[len], status, sizeof(string) - len);
		len += strlen(&string[len]);
	}
	MetaSend(w, string, len + 1);
}
