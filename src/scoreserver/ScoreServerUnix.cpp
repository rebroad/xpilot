/* $Id: ScoreServerUnix.cpp,v 1.7 2004/04/22 14:57:26 dick Exp $
 *
 * XPScoreServer - Who's on first?
 * The Unix main entry file
 *
 *      Copyright (C) 2002 by
 *
 *      The XPilot Authors   <xpilot@xpilot.org>
 *      Dick Balaska         <dick@xpilot.org>
 *      Jarrod Miller        <jarrod@xpilot.org>
 *      Bert Gijsbers        <bert@xpilot.org>
 *      Ken Ronny Schouten   <ken@xpilot.org>
 *      Bjørn Stabell        <bjoern@xpilot.org>
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
 * $Log: ScoreServerUnix.cpp,v $
 * Revision 1.7  2004/04/22 14:57:26  dick
 * Handle terminating the Un*x ScoreServer
 *
 * Revision 1.6  2004/04/22 05:54:11  dick
 * Call netServer->Sched() once per main loop
 *
 * Revision 1.5  2004/01/24 00:57:25  dick
 * Socket cleanups for Linux.
 *
 * Revision 1.4  2003/12/23 01:23:07  dick
 * Linbuf stdout.  Print timestamp on exit
 *
 * Revision 1.3  2002/06/27 12:57:47  dick
 * ScoreServer runs on Linux
 *
 * Revision 1.2  2002/06/26 04:39:32  dick
 * Get unix main to build
 *
 */

#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <stdarg.h>
#include <signal.h>
#include <errno.h>

#include "xpprintf.h"
#include "sockerrs.h"
#include "Http.h"
#include "NetList.h"
#include "ScoreServer.h"
#include "ScoreServerCfg.h"
#include "ScoreServerNetClient.h"
#include "NetServer.h"

PCSTR	szAppName = "xpscoreserver";
extern	PCSTR	s_HttpListen;
bool	terminateFlag = false;

#if 0
#define	SOTRACE	xpprintf
#else
#define	SOTRACE
#endif

#define	_fdadd(_s, _a) \
	if (_s) { \
		/*SOTRACE("_fdadd socket %d\n", _s);*/ \
		FD_SET(_s, &_a); \
		if (_s > maxfd) maxfd = _s; \
		if (_s < minfd) minfd = _s; }

static	void	Handle_signal(int signo);

////////////////////////////////////////////////////////////////////////////////
static	void main_loop()
{
	HttpListen*		n;
	ScoreServerNetClient*	sc;

	while(1)
	{
		fd_set	selset;
		fd_set	readmask;
		fd_set	exceptset;
		fd_set	exceptmask;

		struct	timeval	tv;
		unsigned int		maxfd = 1;
		unsigned int		minfd = ~0;
		int		i;
		int		retsel;;

		tv.tv_sec = 0;
		tv.tv_usec = 100000;	// 1/10 second

//		tv.tv_sec = 2;			// ! DIK: Using slow timeouts crashes the NT server while connecting
//		tv.tv_usec = 0000000;

		if (terminateFlag)
			break;
		netServer->Sched();

		FD_ZERO(&selset);

		Obj* o = netList.GetHead();
		while (o)
		{
			if (o->type == s_HttpListen)
			{
				n = (HttpListen*) o;
				for (i=0; i<SOMAXCONN; i++)
				{
					if (n->connectsock[i])
						SOTRACE("select: http adding %d\n", n->connectsock[i]);
					_fdadd(n->connectsock[i], selset);
					// _fdadd(n->connectsock[i], exceptset);
				}
				//SOTRACE("select: add httplistensock %d\n", n->listensock);
				_fdadd(n->listensock, selset);
				// _fdadd(n->listensock, exceptset);
			}
			if (o->type == s_ScoreServerNetClient)
			{
				sc = (ScoreServerNetClient*)o;
				if (sc->IsConnected())
				{
					_fdadd(sc->GetRBufSockFd(), selset);
					//SOTRACE("select: add netClient sock %d\n", sc->GetRBufSockFd());
				}
			}

			o = o->GetNext();
		}

		readmask = selset;
		exceptmask = exceptset;
		retsel = select(maxfd+1, &readmask, 0, 0, &tv);
		
		if (retsel < 0)
		{
			xpprintf("select error %d (%s)\n", errno, GetSockErrText(errno));
			break;
		}
		if (retsel == 0)
		{
			theApp.TimerTick();
			continue;		// timeout
		}
		for (i=minfd; i<maxfd+1; i++)
		{
			if (FD_ISSET(i, &readmask)) 
			{
				n = FindHttpListener();		// should eventually be type network
				if (n)
				{
					if (i == n->listensock)
					{
						HttpAccept(i);
						SOTRACE("http accept on socket %d\n", i);
						FD_CLR(i, &readmask);
						continue;
					}
				}
				Http* h = FindHttpBySock(i);
				if (h)
				{
					SOTRACE("http read on socket %d\n", i);
					HttpRead(i);
					FD_CLR(i, &readmask);
					continue;
				}
				if (theApp.netClient && theApp.netClient->IsConnected())
				{
					if (i == theApp.netClient->GetRBufSockFd())
					{
						SOTRACE("RbufSelected:\n");
						theApp.netClient->RbufSelected();
						FD_CLR(i, &readmask);
						continue;
					}
				}
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
int	main(int argc, char* argv[])
{
	theApp.Startup();

	signal(SIGINT, Handle_signal);
	signal(SIGTERM, Handle_signal);
	signal(SIGQUIT, Handle_signal);

	/*
	 * Make output always linebuffered.  By default pipes
	 * and remote shells cause stdout to be fully buffered.
	 */
	setvbuf(stdout, NULL, _IOLBF, BUFSIZ);
	setvbuf(stderr, NULL, _IOLBF, BUFSIZ);

	main_loop();

	theApp.Shutdown();
	exit(0);
}

////////////////////////////////////////////////////////////////////////////////
void TerminateScoreServer()
{
	terminateFlag = true;
}

////////////////////////////////////////////////////////////////////////////////
static char	*sigtext[]= {
	"SIGNULL",
	"SIGHUP",
	"SIGINT",
	"SIGQUIT",
	"SIGILL",
	"SIGTRAP",
	"SIGABRT/IOT",
	"SIGBUS",
	"SIGFPE",
	"SIGKILL",
	"SIGUSR1",
	"SIGSEGV",
	"SIGUSR2",
	"SIGPIPE",
	"SIGALRM",
	"SIGTERM",
	"SIGSTKFL",
	"SIGCHLD",
	"SIGCONT",
	"SIGSTOP",
	"SIGTSTP",
	"SIGTTIN",
	"SIGTTOU",
	"SIGURG",
	"SIGXCPU",
	"SIGXFSZ",
	"SIGVTALRM",
	"SIGPROF",
	"SIGWINCH",
	"SIGIO/POLL",
	"SIGPWR",
	"SIGUNUSED"
};

////////////////////////////////////////////////////////////////////////////////
static void	Handle_signal(int signo)
{
	xpprintf("%sScoreServer: Shutdown on signal %d %s\n", showtime(), signo, sigtext[signo&31]);
	theApp.Shutdown();
	exit(0);
}

