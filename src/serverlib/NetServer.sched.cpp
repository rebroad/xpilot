/* $Id: NetServer.sched.cpp,v 1.29 2005/03/17 22:12:14 kps Exp $
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
 * $Log: NetServer.sched.cpp,v $
 * Revision 1.29  2005/03/17 22:12:14  kps
 * Get rid of warnings from makedepend about "non-portable whitespace".
 *
 * Revision 1.28  2004/05/20 22:26:03  dick
 * Bump stupid Windows 'opaque' socket limit to 4000
 *
 * Revision 1.27  2004/04/22 05:52:16  dick
 * Attribute schedIsLoop is used to control the Sched() loop.  The Un*x server
 * wants Sched() to be the main loop.  Windows server and all ScoreServers
 * want Sched() to be pumped repeatedly by an outside source.
 *
 * Revision 1.26  2004/04/21 21:16:39  dick
 * Whitespace
 *
 * Revision 1.25  2004/04/16 21:07:59  dick
 * NetServer becomes a little UDP engine that can be plugged into different apps.
 *
 * Revision 1.24  2004/03/10 04:24:27  dick
 * Some #include whitespace to shut up make depend
 *
 * Revision 1.23  2004/02/20 06:37:17  dick
 * Unix needs declaration of PCSTR
 *
 * Revision 1.22  2004/02/16 14:29:44  dick
 * server/sched.cpp and friends become serverlib/NetServer
 * which disconnects the udp listen and receive functionality from the game.
 *
 * Revision 1.21  2004/02/03 03:39:54  dick
 * Add Windows precompiled header support.
 *
 * Revision 1.20  2004/01/06 04:48:28  dick
 * Add support for building under Cygwin.  Still needs work and the output
 * is X11 and requires something like Cygwin's XWin.exe to run under.
 *
 * Revision 1.19  2002/09/11 16:42:04  dick
 * Here endeth the server encapsulation task.
 * theWorld is now an array of World(s) and get loaded dynamically.
 *
 * Revision 1.18  2002/09/01 00:13:46  dick
 * Remove obsolete windows #include "winServer.h"
 *
 * Revision 1.17  2002/08/31 23:34:40  dick
 * winSvrThread.h becomes ServerThreadW32.h
 *
 * Revision 1.16  2002/07/12 15:33:30  dick
 * Track controlsConnected so we can run fast sched() if there is only a
 * control connected.
 *
 * Revision 1.15  2002/07/01 04:33:27  dick
 * Windows' timer runs like Unix'.  A system tick increments a counter and the
 * sched loop actually calls the game timer loop.
 *
 * Revision 1.14  2002/06/30 06:46:16  dick
 * Windows can also use the 0 timeout to sched
 *
 * Revision 1.13  2002/06/26 07:01:18  dick
 * Use xperrno instead of errno and seterrno(0) instead of errno = 0.
 *
 * Revision 1.12  2002/06/01 06:06:48  dick
 * Encapsulate (almost) everything.  Get rid of (almost) all refs to theWorld.
 *
 * Revision 1.11  2002/05/18 20:55:39  dick
 * Update to XPilot-4.5.1
 *
 * Revision 1.10  2001/08/12 11:11:14  dick
 * Deadwood delete
 *
 * Revision 1.9  2001/08/01 08:06:32  dick
 * Another address of func woe *sigh*
 *
 * Revision 1.8  2001/08/01 06:39:41  dick
 * AFUNC cleanups. address to function...
 *
 * Revision 1.7  2001/07/29 23:40:49  dick
 * Linux gcc tweaks.  The newer versions of g++ has stricter (and different)
 * requirements.
 *
 * Revision 1.6  2001/07/24 18:47:37  dick
 * Linux (gcc) needs the address of (&) member functions passed around.
 * Windows doesn't like this, so define AFUNC which is a '&'.
 *
 * Revision 1.5  2001/07/24 09:04:00  dick
 * Remove all global options.  Use World.options instead.
 *
 * Revision 1.4  2001/07/23 21:08:37  dick
 * Encapsulate ShipMass, ShotsMax, ShotsLife, ShotsMass, FPS.
 *
 * Revision 1.3  2001/07/23 07:38:43  dick
 * Fold contact, metaserver, netserver, rules, and server to be members of World.
 *
 * Revision 1.2  2001/07/15 01:24:03  dick
 * Commentality
 *
 */
#include "StdAfx.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>
#include <errno.h>
#include <time.h>
#include <sys/types.h>
#include <assert.h>

#if defined(_UNIX) || defined(_CYGWIN)
#include <unistd.h>
#ifndef __hpux
#include <sys/time.h>
#endif
#ifdef _AIX
#include <sys/select.h> /* _BSD not defined in <sys/types.h>, so done by hand */
#endif
#else
#include <windows.h>
#endif

#ifdef _OS2_
#	define  INCL_DOSSEMAPHORES
#	define  INCL_DOSDATETIME
#	define  INCL_DOSPROCESS
#	include <os2emx.h>
#endif

#include "version.h"
#include "config.h"
#include "error.h"
#include "types.h"
#include "const.h"
//#include "global.h"
#include "sockerrs.h"
#include "portability.h"
#include "cstring.h"
#include "NetServer.h"

char sched_version[] = VERSION;

int sched_running = false;

volatile long	timer_ticks;	/* SIGALRMs that have occurred */
static long		timers_used;	/* SIGALRMs that have been used */
static long		timer_freq;		/* rate at which timer ticks. (in FPS) */
static void*	timer_data;

//typedef	void	(World::*XPTIMERPROC)();
//typedef	void	(*XPTIMERPROC)(void);

//static XPTIMERPROC	timer_handler;
static TimerFunc	timer_handler;

typedef	void	(*TIMERFUNC)(int signum);
static	TIMERFUNC	timerFunc = NULL;


static time_t		current_time;
static int		ticks_till_second;


/* Windows incorrectly uses u_int in FD_CLR */
#if defined(_WINDOWS) && !defined(_CYGWIN)
typedef	u_int	FDTYPE;
#else
typedef	int		FDTYPE;
#endif

/* Windows event number returned by SetTimer */
#if defined(_WINDOWS) && !defined(_CYGWIN)
UINT	winTimerEvent = 0;
#endif
/*
 * Block or unblock a single signal.
 */
static void sig_ok(int signum, int flag)
{
#if defined(_UNIX) || defined(_CYGWIN)
    sigset_t    sigset;

    sigemptyset(&sigset);
    sigaddset(&sigset, signum);
    if (sigprocmask((flag) ? SIG_UNBLOCK : SIG_BLOCK, &sigset, NULL) == -1) {
	error("sigprocmask(%d,%d)", signum, flag);
	exit(1);
    }
#endif
}

/*
 * Prevent the real-time timer from interrupting system calls.
 * Globally accessible.
 */
static void block_timer(void)
{
#if defined(_UNIX) || defined(_CYGWIN)
    sig_ok(SIGALRM, 0);
#endif
}

/*
 * Unblock the real-time timer.
 * Globally accessible.
 */
void allow_timer(void)
{
#if defined(_UNIX) || defined(_CYGWIN)
    sig_ok(SIGALRM, 1);
#endif
}


/*
 * Catch SIGALRM.
 * Simple timer ticker.
 */
static void catch_timer_ticks(int signum)
{
	timer_ticks++;
}


/*
 * Catch SIGALRM.
 * Use timerResolution to increment timer_ticks.
 */
static void catch_timer_counts(int signum)
{
	static unsigned int		timer_count = 0;

	timer_count += netServer->GetFPS();
	if (timer_count >= netServer->timerResolution) {
		timer_count -= netServer->timerResolution;
		timer_ticks++;
	}
}


#ifdef _OS2_
/*
 *  Thread to catch the OS/2 timer.  Raise a SIGALRM on the main thread
 *  at each timer tick.  I.e., perform the function of setitimer in Unix.
 */
void timerThread( void *arg )
{
	ULONG count;         /*  Post count of hev.  Required by call.  */
	ULONG pid = 0;       /*  Process ID for this process  */
	PTIB  ptib = NULL;   /*  Thread Information Block     */
	PPIB  ppib = NULL;   /*  Process Information Block    */

	HEV     hev;         /*  Event semaphore handle  */
	HTIMER  htimer;      /*  Timer handle            */

	APIRET  rc;          /*  Return code for Dos* calls  */

#	ifdef OS2DEBUG
	        ULONG  aulSysInfo[1] = {0};
		DosQuerySysInfo( 22, 22, (PVOID)aulSysInfo, sizeof(ULONG) );
		printf( "Timer interval in tenths of milliseconds: %ld\n", aulSysInfo[0] );
#	endif

#	ifdef OS2DEBUG
		printf( "Entering timer processing thread.  Will set for %ld FPS.\n", timer_freq );
#	endif

	/*  Get the ID of this process, so we can send a SIGALRM
	 *  to the main thread.
	 */
	if( rc = DosGetInfoBlocks( &ptib, &ppib ) )
	{
		error("Error getting process information.  rc = %d", rc );
		exit( 1 );
	}

	pid = ppib->pib_ulpid;

	/*  Increase the priority of this thread to regular time critical.
	 *  This should ensure that our timer ticks get delivered as quickly
	 *  as possible.
	 */
	if( rc = DosSetPriority(  PRTYS_THREAD, PRTYC_TIMECRITICAL, 0L, 0L ) )
	{
		error("Error setting timer thread priority.  rc = %d", rc );
		exit(1);
	}

	/*  Create the event semaphore that will be posted by the timer.  */
	if( rc = DosCreateEventSem( NULL, &hev, DC_SEM_SHARED, FALSE ) )
	{
		error("DosCreateEventSem - error creating timer semaphore.  rc = %d", rc );
		exit( 1 );
	}

	/*  Start the timer.  The semaphore hev will be posted at each
	 *  timer tick.
	 */
	if( rc = DosStartTimer( 1000/timer_freq, (HSEM)hev, &htimer ) )
	{
		error("DosStartTimer - error starting timer.  rc = %d", rc );
		exit( 1 );
	}

	/*  Now just loop forever, waiting for the timer and then
	 *  signalling a SIGALRM on the main thread.
	 */
	while( 1 )
	{
		/*  Wait for the timer tick.  */
		DosWaitEventSem( hev, SEM_INDEFINITE_WAIT );

		/*  Reset the semaphore.  */
		DosResetEventSem( hev, &count );

		/*  raise( SIGALRM ) sends the signal to this thread,
		 *  not to the main thread where we have the handler
		 *  installed.  So, we must use kill( ) instead to raise
		 *  the signal.
		 */
		kill( pid, SIGALRM );
	}
	return;
}

#endif

#if defined(_WINDOWS) && !defined(_CYGWIN)
void CALLBACK NetServerTimerProc(HWND hwnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{
	assert(timerFunc != NULL);
	(*timerFunc)(0);
}
#endif

/*
 * Setup the handling of the SIGALRM signal
 * and setup the real-time interval timer.
 */
void NetServer::SetupTimer(void)
{
	timerFunc = timerResolution > 0
						? (catch_timer_counts)
						: (catch_timer_ticks);
#if defined(_UNIX) || defined(_CYGWIN)

#ifndef _OS2_
	struct itimerval itv;
#endif
	struct sigaction act;

	/*
	 * Prevent SIGALRMs from disturbing the initialization.
	 */
	block_timer();

	/*
	 * Install a signal handler for the alarm signal.
	 */
	act.sa_handler = timerFunc;
	act.sa_flags = 0;
	sigemptyset(&act.sa_mask);
	sigaddset(&act.sa_mask, SIGALRM);
	if (sigaction(SIGALRM, &act, (struct sigaction *)NULL) == -1) {
		error("sigaction SIGALRM");
		exit(1);
	}

	/*
	* Install a real-time timer.
	*/
	if (timer_freq <= 0 || timer_freq > 100) {
		error("illegal timer frequency: %ld", timer_freq);
		exit(1);
	}

#ifndef _OS2_
	itv.it_interval.tv_sec = 0;
	itv.it_interval.tv_usec = 1000000 / timer_freq;
	itv.it_value = itv.it_interval;
	if (setitimer(ITIMER_REAL, &itv, NULL) == -1) {
		error("setitimer");
		exit(1);
	}
#else  /*  !defined( _OS2_ )  */

	/*  setitimer  isn't implemented in EMX, so we must fake our
	 *  own.  Create a thread that starts a timer and raises SIGALRM
	 *  at each tick.
	 *
	 *  Of course, this timer is rather course, given the time-slicing
	 *  of OS/2, but it is the best we can do at the moment.  The next
	 *  step is to look into the high-resolution timer that Timur Tabi
	 *  wrote (which, I believe, is now part of OS/2?).
	 */

	if( _beginthread( timerThread, NULL, 8192L, NULL ) == -1 ) {
		error("_beginthread - error starting timer thread");
		exit( 1 );
	}

#endif  /*  !defined( _OS2_ )  */

	timers_used = timer_ticks;
	time(&current_time);
	ticks_till_second = timer_freq;
#else
	extern	void StartWinServerTimer(long freq);
//	winTimerEvent = SetTimer(NULL, 0, 1000/timer_freq, (TIMERPROC)ServerThreadTimerProc);
	StartWinServerTimer(1000/timer_freq);
//	winTimerEvent = SetTimer(NULL, 0, 20, (TIMERPROC)ServerThreadTimerProc);
//	if (!winTimerEvent)
//		error("Can't create timer");

#endif
    /*
     * Allow the real-time timer to generate SIGALRM signals.
     */
    allow_timer();
}

/*
 * Configure timer tick callback.
 */
void NetServer::InstallTimerTick(TimerFunc func, int freq, void* data)
{
	timer_handler = func;
	timer_freq = freq;
	timer_data = data;
	SetupTimer();
} 

/*
 * Linked list of timeout callbacks.
 */
struct to_handler {
	struct to_handler	*next;
	time_t		when;
	void		(*func)(void *);
	void		*arg;
};
static struct to_handler *to_busy_list = NULL;
static struct to_handler *to_free_list = NULL;
static int		to_min_free = 3;
static int		to_max_free = 5;
static int		to_cur_free = 0;

static void to_fill(void)
{
	if (to_cur_free < to_min_free) {
		do {
			struct to_handler *top =
				(struct to_handler *)malloc(sizeof(struct to_handler));
			if (!top) {
				break;
			}
			top->next = to_free_list;
			to_free_list = top;
			to_cur_free++;
		} while (to_cur_free < to_max_free);
	}
}

static struct to_handler *to_alloc(void)
{
	struct to_handler *top;

	to_fill();
	if (!to_free_list) {
		error("Not enough memory for timeouts");
		exit(1);
	}

	top = to_free_list;
	to_free_list = top->next;
	to_cur_free--;
	top->next = 0;

	return top;
}

static void to_free(struct to_handler *top)
{
	if (to_cur_free < to_max_free) {
		top->next = to_free_list;
		to_free_list = top;
		to_cur_free++;
	}
	else {
		free(top);
	}
}

/*
 * Configure timout callback.
 */
void install_timeout(void (*func)(void *), int offset, void *arg)
{
	struct to_handler *top = to_alloc();

	top->func = func;
	top->when = current_time + offset;
	top->arg = arg;
	if (!to_busy_list || to_busy_list->when >= top->when) {
		top->next = NULL;
		to_busy_list = top;
	}
	else {
		struct to_handler *prev = to_busy_list;
		struct to_handler *lp = prev->next;
		while (lp && lp->when < top->when) {
			prev = lp;
			lp = lp->next;
		}
		top->next = lp;
		prev->next = top;
	}
}

void remove_timeout(void (*func)(void *), void *arg)
{
	struct to_handler *prev = 0;
	struct to_handler *lp = to_busy_list;
	while (lp) {
		if (lp->func == func && lp->arg == arg) {
			struct to_handler *top = lp;
			lp = lp->next;
			if (prev) {
				prev->next = lp;
			} else {
				to_busy_list = lp;
			}
			to_free(top);
		}
		else {
			prev = lp;
			lp = lp->next;
		}
	}
}

static void timeout_chime(void)
{
	while (to_busy_list && to_busy_list->when <= current_time) {
		struct to_handler *top = to_busy_list;
		void (*func)(void *) = top->func;
		void *arg = top->arg;
		to_busy_list = top->next;
		to_free(top);
		(*func)(arg);
	}
}

#if defined(_UNIX) || defined(_CYGWIN)
#define NUM_SELECT_FD		((int)sizeof(int) * 8)
#else
/*
    Windoze:
    The first call to socket() returns 560ish.  Successive calls keep bumping
    up the SOCKET returned until about 880 when it wraps back to 8.
    (It seems to increment by 8 with each connect - but that's not important)
    I can't find a manifest constant to tell me what the upper limit will be
    *sigh*

    --- Now, the Windoze gurus tell me that SOCKET is an opaque data type.
    So i need to make a lookup array for the lookup array :(
*/
#define	NUM_SELECT_FD		4000
#endif

struct io_handler {
	int			fd;
	InputFunc	func;
	void		*arg;
};

static struct io_handler	input_handlers[NUM_SELECT_FD];
static fd_set			input_mask;
static int			max_fd, min_fd;
static int			input_inited = false;

void io_dummy(int fd, void *arg)
{
	xpprintf("io_dummy called!  (%d, %p)\n", fd, arg);
}

//void install_input(void (*func)(int, void *), int fd, void *arg)
void NetServer::InstallInput(InputFunc func, int fd, void *arg)
{
	int i;

	if (input_inited == false) {
		input_inited = true;
		FD_ZERO(&input_mask);
#		if defined(_UNIX) || defined(_CYGWIN)
			min_fd = fd;
#		else
			min_fd = 0;
#		endif
		max_fd = fd;
		for (i = 0; i < NELEM(input_handlers); i++) {
			input_handlers[i].fd = -1;
			input_handlers[i].func = &io_dummy;
			input_handlers[i].arg = 0;
		}
	}
	/* IFWINDOWS(xpprintf("install_input: fd %d min_fd=%d\n", fd, min_fd);) */
	if (fd < min_fd || fd >= min_fd + NUM_SELECT_FD) {
		error("install illegal input handler fd %d (%d)", fd, min_fd);
		ServerExit();
	}
	if (FD_ISSET(fd, &input_mask)) {
		error("input handler %d busy", fd);
		ServerExit();
	}
	input_handlers[fd - min_fd].fd = fd;
	input_handlers[fd - min_fd].func = func;
	input_handlers[fd - min_fd].arg = arg;
	FD_SET(fd, &input_mask);
	if (fd > max_fd) {
		max_fd = fd;
	}
}

void NetServer::RemoveInput(int fd)
{
	if (fd < min_fd || fd >= min_fd + NUM_SELECT_FD)
	{
		error("remove illegal input handler fd %d (%d)", fd, min_fd);
		ServerExit();
	}
	if (FD_ISSET(fd, &input_mask))
	{
		input_handlers[fd - min_fd].fd = -1;
		input_handlers[fd - min_fd].func = &io_dummy;
		input_handlers[fd - min_fd].arg = 0;
		FD_CLR((FDTYPE)fd, &input_mask);
		if (fd == max_fd)
		{
			int i = fd;
			max_fd = -1;
			while (--i >= min_fd)
			{
				if (FD_ISSET(i, &input_mask))
				{
					max_fd = i;
					break;
				}
			}
		}
	}
}

void NetServer::StopSched()
{
	sched_running = 0;
#if defined(_WINDOWS) && !defined(_CYGWIN)
	KillTimer(NULL, winTimerEvent);
#endif
}



static void sched_select_error()
{
#if defined(_UNIX) || defined(_CYGWIN)
	error("sched select error");
#else
	char	msg[MSG_LEN];

	sprintf(msg, "sched select error e=%d (%s)",
		xperrno, GetSockErrText(xperrno));
	error("%s", msg);
#endif

//	theWorld[0]->EndGame();
	ServerExit();
	return;
}


/*
 * I/O + timer dispatcher.
 *
 * Windows wraps this (with Windows thread stuff) and pumps it continually
 * because the thread may be windows signaled to exit and we can't block here
 * indefinitely.
 */
void NetServer::Sched(void)
{
	int			i, n;
	static int io_todo = 3;
	struct timeval	tv, *tvp = &tv;

//#if defined_UNIX) || defined(_CYGWIN)
	if (schedIsLoop)
	{
		if (sched_running) 
		{
			error("sched already running");
			exit(1);
		}

		sched_running = 1;

		tv.tv_sec = 0;
		tv.tv_usec = 0;
	}
	while (1)
//	while (sched_running) 
	{
		if (schedIsLoop && !sched_running)
			break;

//#else
// FIXME:
//		if (theWorld[0]->numPlayers > theWorld[0]->numRobots + theWorld[0]->numPseudoPlayers
//			|| login_in_progress != 0
//			|| controlsConnected != 0	// really only needed during download options. Otherwise slow is fine.
//			|| theWorld[0]->numQueuedPlayers > 0) 
		if (!schedIsLoop)
		{

			/* need fast I/O checks now! (2 or 3 times per frames) */
			tv.tv_sec = 0;
			/* KOERBER */
			/*	tv.tv_usec = 1000000 / (3 * timer_freq + 1); */
			tv.tv_usec = 1000000 / (10 * timer_freq + 1); 
		}
//		else {
//			/* slow I/O checks are possible here... (2 times per second) */ ; 
//			tv.tv_sec = 0;
//			tv.tv_usec = 500000;
//		}
//
//#endif


		if (io_todo == 0 && timers_used < timer_ticks) 
		{
			io_todo = 1 + (timer_ticks - timers_used);
			tvp = &tv;

			if (timer_handler) {
				(*timer_handler)(timer_data);
			}

			do {
				++timers_used;
				if (--ticks_till_second <= 0) {
					ticks_till_second += timer_freq;
					current_time++;
					timeout_chime();
				}
			} while (timers_used + 1 < timer_ticks);
		}
		else {
			fd_set readmask;
			readmask = input_mask;

#			if 0
				{
					int i;
					int countfd = 0;
					for (i=0; i<max_fd; i++)
						if (FD_ISSET(i, &readmask))
							countfd++;
					_Trace("max_fd = %d Selecting on %d fds\n", max_fd, countfd);
				}
#			endif
			n = select(max_fd + 1, &readmask, 0, 0, tvp);
			if (n <= 0) {
				if (n == -1 && xperrno != EINTR) {
					sched_select_error();
				}
				io_todo = 0;
			}
			else {
				for (i = max_fd; i >= min_fd; i--) {
					if (FD_ISSET(i, &readmask)) {
						struct io_handler *ioh;
						ioh = &input_handlers[i - min_fd];
//						(theWorld[0]->*ioh->func)(ioh->fd, ioh->arg);
						(*ioh->func)(ioh->fd, ioh->arg);
						if (--n == 0) {
							break;
						}
					}
				}
				if (io_todo > 0) {
					io_todo--;
				}
			}
			if (io_todo == 0) {
				tvp = NULL;
			}
		}
		if (!schedIsLoop)
			return;
	}
}
