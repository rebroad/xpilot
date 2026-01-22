/* $Id: join.cpp,v 1.16 2007/02/17 06:18:15 dick Exp $
 *
 * XPilot, a multiplayer gravity war game.	Copyright (C) 1991-2001 by
 *
 *		Bjørn Stabell		 <bjoern@xpilot.org>
 *		Ken Ronny Schouten	 <ken@xpilot.org>
 *		Bert Gijsbers		 <bert@xpilot.org>
 *		Dick Balaska		 <dick@xpilot.org>
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
 * $Log: join.cpp,v $
 * Revision 1.16  2007/02/17 06:18:15  dick
 * client/Audio becomes common/AudioMan.
 *
 * Revision 1.15  2004/06/03 02:21:48  dick
 * Need (PCSTR) on String serverAddr
 *
 * Revision 1.14  2004/05/02 03:08:00  dick
 * Audio support.  Separate client and server audio support.
 * Servers will probably always build with -DSOUND
 * Clients will build -DSOUND only if OpenAL is present.
 *
 * Revision 1.13  2004/05/01 20:37:36  dick
 * Use the new OpenAL audio object
 *
 * Revision 1.12  2004/04/29 16:08:26  dick
 * compile with SOUND enabled.
 *
 * Revision 1.11  2004/02/03 03:39:52  dick
 * Add Windows precompiled header support.
 *
 * Revision 1.10  2004/01/06 04:48:27  dick
 * Add support for building under Cygwin.  Still needs work and the output
 * is X11 and requires something like Cygwin's XWin.exe to run under.
 *
 * Revision 1.9  2002/09/16 22:17:22  dick
 * Need cstring.h
 *
 * Revision 1.8  2002/09/16 21:42:04  dick
 * Use Stringized Connectparam.
 *
 * Revision 1.7  2002/09/02 15:42:40  dick
 * Singleton ini becomes iniClient.
 *
 * Revision 1.6  2002/08/01 15:01:27  dick
 * Send the cookie with the verify message to the server. Retab.
 *
 * Revision 1.5  2002/07/15 21:54:36  bert
 * these files all had file corruptions at several places.
 * mostly two adjacent bytes were replaced with bogus random data.
 * fixed.
 *
 * Revision 1.4  2002/06/26 06:47:03  dick
 * Replace errno with xperrno and errno=0 with seterrno(0).
 *
 * Revision 1.3  2001/09/11 07:33:44  dick
 * Encapsulate all client resources into new class Ini.
 *
 * Revision 1.2  2001/07/27 05:47:39  dick
 * Client builds and runs in C++
 *
 */
#include "StdAfx.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <time.h>
#include <sys/types.h>

#if defined(_UNIX) || defined(_CYGWIN)
# include <unistd.h>
# ifndef __hpux
#  include <sys/time.h>
# endif
# ifdef _AIX
#  include <sys/select.h> /* _BSD not defined in <sys/types.h>, so done by hand */
# endif
# include <sys/socket.h>
# include <netinet/in.h>
# include <netdb.h>
#else
# include <winsock.h>
# include "NT/winClient.h"
#endif

#include "version.h"
#include "cstring.h"
#include "config.h"
#include "const.h"
#include "error.h"
#include "client.h"
#include "types.h"
#include "netclient.h"
#include "protoclient.h"
#include "portability.h"
#include "Ini.h"
#include "Connectparam.h"
#include "AudioMan.h"

char join_version[] = VERSION;

#ifndef SCORE_UPDATE_DELAY
# define SCORE_UPDATE_DELAY 4
#endif

void xpilotShutdown(void);

extern void Record_cleanup(void);
extern Connectparam		myConpar;


#ifndef _WINDOWS
static void Input_loop(void)
{
	fd_set		rfds;
	fd_set		tfds;
	int 		max,
			n,
			netfd,
			result,
			clientfd;
	struct timeval	tv;

	if ((result = Net_input()) == -1) {
		error("Bad server input");
	return;
	}
	if (Client_input(2) == -1) {
		return;
	}
	if (Net_flush() == -1) {
		return;
	}
	if ((clientfd = Client_fd()) == -1) {
		error("Bad client filedescriptor");
		return;
	}
	if ((netfd = Net_fd()) == -1) {
		error("Bad socket filedescriptor");
		return;
	}
	Net_key_change();
	FD_ZERO(&rfds);
	FD_SET(clientfd, &rfds);
	FD_SET(netfd, &rfds);
	max = (clientfd > netfd) ? clientfd : netfd;
	for (tfds = rfds; ; rfds = tfds) {
	if ((scoresChanged != 0 && ++scoresChanged > SCORE_UPDATE_DELAY)
		|| result > 1) 
	{
		if (scoresChanged > 2 * SCORE_UPDATE_DELAY) {
			Client_score_table();
			tv.tv_sec = 10;
			tv.tv_usec = 0;
		} else {
			tv.tv_sec = 0;
			tv.tv_usec = 0;
		}
	} else {
		tv.tv_sec = 10;
		tv.tv_usec = 0;
	}
	if ((n = select(max + 1, &rfds, NULL, NULL, &tv)) == -1) {
		if (errno == EINTR) {
			continue;
		}
		error("Select failed");
		return;
	}
	if (n == 0) {
		if (scoresChanged > SCORE_UPDATE_DELAY) {
			Client_score_table();
			if (Client_input(2) == -1) {
				return;
			}
			continue;
		}
		else if (result <= 1) {
			errno = 0;
			error("No response from server");
			continue;
		}
	}
	if (FD_ISSET(clientfd, &rfds)) {
		if (Client_input(1) == -1) {
			return;
		}
		if (Net_flush() == -1) {
			error("Bad net flush after X input");
			return;
		}
	}
	if (FD_ISSET(netfd, &rfds) || result > 1) {
		if ((result = Net_input()) == -1) {
			errno = 0;
			error("Bad net input.  Have a nice day!");
			return;
		}
		if (result > 0) {
		/*
		 * Now there's a frame being drawn by the X server.
		 * So we shouldn't try to send more drawing
		 * requests to the X server or it might get
		 * overloaded which could cause problems with
		 * keyboard input.	Besides, we wouldn't even want
		 * to send more drawing requests because there
		 * may arive a more recent frame update soon
		 * and using the CPU now may even slow down the X server
		 * if it is running on the same CPU.
		 * So we only check if the X server has sent any more
		 * keyboard events and then we wait until the X server
		 * has finished the drawing of our current frame.
		 */
		if (Client_input(1) == -1) {
			return;
		}
		if (Net_flush() == -1) {
			error("Bad net flush before sync");
			return;
		}
		Client_sync();
		if (Client_input(1) == -1) {
			return;
		}
		}
	}
	}
}
#endif	/* _WINDOWS */

void xpilotShutdown()
{
#ifdef SOUND
    audioMan.Cleanup();
#endif /* SOUND */
	Net_cleanup();
	Client_cleanup();
	Record_cleanup();
	aboutCleanup();
	paintdataCleanup();
}

static void sigcatch(int signum)
{
	signal(SIGINT, SIG_IGN);
	signal(SIGTERM, SIG_IGN);
	xpilotShutdown();
	error("Got signal %d\n", signum);
	exit(1);
}

//int Join(char *server_addr, char *server_name, int port, char *real,
//	 char *nick, int my_team, char *display, unsigned version)
int Join(Connectparam* conpar)
{
	signal(SIGINT, sigcatch);
	signal(SIGTERM, sigcatch);
#ifndef _WINDOWS
	signal(SIGHUP, SIG_IGN);
	signal(SIGPIPE, SIG_IGN);
#endif

	IFWINDOWS( received_self = FALSE; )
	IFWINDOWS( Progress("Client_init"); )
//	if (Client_init(conpar->serverName, conpar->serverVersion) == -1) {
	if (ClientInit(conpar) == -1) {
		return -1;
	}

	IFWINDOWS( Progress("Net_init %s", (PCSTR)conpar->serverAddr); )
	if (Net_init(conpar->serverAddr, conpar->loginPort) == -1) {
		Client_cleanup();
		return -1;
	}
	IFWINDOWS( Progress("Net_verify '%s'= '%s'", (PCSTR)conpar->nick, (PCSTR)conpar->realName); )
	//if (Net_verify(conpar->realName, conpar->nick, conpar->dispName, conpar->team) == -1) {
	if (Net_verify(conpar) == -1) {
		Net_cleanup();
		Client_cleanup();
		return -1;
	}
	IFWINDOWS( Progress("Net_setup"); )
	if (Net_setup() == -1) {
		Net_cleanup();
		Client_cleanup();
		return -1;
	}
	IFWINDOWS( Progress("Client_setup"); )
	if (Client_setup() == -1) {
		Net_cleanup();
		Client_cleanup();
		return -1;
	}
	IFWINDOWS( Progress("Net_start"); )
	if (Net_start() == -1) {
		seterrno(0);
		error("Network start failed");
		Net_cleanup();
		Client_cleanup();
		return -1;
	}
	IFWINDOWS( Progress("Client_start"); )
	if (Client_start() == -1) {
		seterrno(0);
		error("Window init failed");
		Net_cleanup();
		Client_cleanup();
		return -1;
	}

	myConpar = *conpar;

#ifndef _WINDOWS	/* windows continues to run at this point */
	Input_loop();
	xpilotShutdown();
#endif

	return 0;
}

