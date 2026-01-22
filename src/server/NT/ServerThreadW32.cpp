/* $Id: ServerThreadW32.cpp,v 1.7 2004/05/07 01:13:39 dick Exp $
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
 * $Log: ServerThreadW32.cpp,v $
 * Revision 1.7  2004/05/07 01:13:39  dick
 * Win32 has no window for SendDialogUpdate (was an MFC server thing).
 *
 * Revision 1.6  2004/02/16 14:29:44  dick
 * server/sched.cpp and friends become serverlib/NetServer
 * which disconnects the udp listen and receive functionality from the game.
 *
 * Revision 1.5  2004/02/03 03:39:54  dick
 * Add Windows precompiled header support.
 *
 * Revision 1.4  2002/09/11 16:42:04  dick
 * Here endeth the server encapsulation task.
 * theWorld is now an array of World(s) and get loaded dynamically.
 *
 * Revision 1.3  2002/09/09 23:41:19  dick
 * ServerThreadW32NotifyMainShutdown() handles shutting down the Windows server.
 *
 * Revision 1.2  2002/09/01 00:13:46  dick
 * Remove obsolete windows #include "winServer.h"
 *
 * Revision 1.1  2002/08/31 22:19:17  dick
 * Remove MFC from the server
 *
 * Revision 1.18  2002/07/08 05:52:59  dick
 * showtime() handles the space after itself
 *
 * Revision 1.17  2002/07/01 04:33:28  dick
 * Windows' timer runs like Unix'.  A system tick increments a counter and the
 * sched loop actually calls the game timer loop.
 *
 * Revision 1.16  2002/06/30 06:26:08  dick
 * ServerKilled becomes serverKilled
 *
 * Revision 1.15  2002/06/26 07:01:18  dick
 * Use xperrno instead of errno and seterrno(0) instead of errno = 0.
 *
 * Revision 1.14  2002/06/01 06:06:49  dick
 * Encapsulate (almost) everything.  Get rid of (almost) all refs to theWorld.
 *
 * Revision 1.13  2002/05/22 05:09:02  dick
 * Update to XPilot r4_5_4alpha.
 *
 * Revision 1.12  2002/05/20 01:51:33  dick
 * Don't try to display a message when we're going down
 *
 * Revision 1.11  2002/05/18 22:34:16  dick
 * Move showtime to common.
 *
 * Revision 1.10  2002/05/18 20:55:40  dick
 * Update to XPilot-4.5.1
 *
 * Revision 1.9  2001/08/26 10:36:18  dick
 * Make the dialog box a bit bigger.
 *
 * Revision 1.8  2001/08/14 21:47:11  dick
 * Windows halts the server on "overlapping timer" which is really usually
 * an unhandled thrown exception in the timer func.
 *
 * Revision 1.7  2001/08/08 08:04:04  dick
 * Need error.h
 *
 * Revision 1.6  2001/08/02 19:53:29  dick
 * Still dealing with printf _Trace xpprintf etc.
 *
 * Revision 1.5  2001/07/27 07:02:41  dick
 * Duplicate symbols, hmm.
 *
 * Revision 1.4  2001/07/24 09:04:01  dick
 * Remove all global options.  Use World.options instead.
 *
 * Revision 1.3  2001/07/23 07:38:43  dick
 * Fold contact, metaserver, netserver, rules, and server to be members of World.
 *
 * Revision 1.2  2001/07/08 06:40:14  dick
 * C++ conversion stuff.
 */

/* Entry point for Windows Server Thread */
#include "StdAfx.h"

#include <windows.h>

#include "ServerThreadW32.h"
#include "config.h"
//#include "../sched.h"
#include "../global.h"
#include "NetServer.h"
#include <stdio.h>
#include <stdarg.h>
#include "NT/winNet.h"
#include "error.h"
#include "portability.h"
#include "../proto.h"
#include "showtime.h"

int		MsgAvail0 = FALSE;
int		MsgAvail1 = FALSE;
char	Msg0[512];
char	Msg1[512];
ServerThreadW32*	pServerInfo;
int		serverKilled;
PCSTR	serverDeathMessage;

//extern long	timer_freq;	/* rate at which timer ticks. (in FPS) */
//extern volatile long	timer_ticks;	/* SIGALRMs that have occurred */
//extern int		sched_running;

extern	int			main(int argc, char** argv);

ULONG ServerThreadW32Proc(LPVOID pParam)
{
	/*
	 * Purify stomps on the command line arguments that i pass
	 * (It complains about reading past the end of the stack and then
	 *  *WHOMPS* the data)
	 * I don't know if this is a real problem or just Purify's inability
	 * to deal with data coming across from a different thread.
	 * So i clone them to be local.  Purify still complains, but at least
	 * it doesn't kill my data.
	 * 
	 */
	char**	zargv;
	int	zargc;
	int	i;

	pServerInfo = (ServerThreadW32*)pParam;
	serverKilled = FALSE;		// if the user kills this thread
								// then we need to take a different exit
	serverDeathMessage = NULL;
//	UINT cr;


	while (TRUE)
	{

		// Wait until the main application thread asks this thread to do
		//      something
		if (WaitForSingleObject(pServerInfo->m_hEventStartGame, INFINITE)
			!= WAIT_OBJECT_0)
			break;

		zargc = pServerInfo->argc;
		zargv = (char **) malloc((zargc + 1) *sizeof(char*));
		for (i=0; i<zargc; i++)
		{
			//xpprintf("i=%d strlen'%s' = %d\n", i, pServerInfo->argv[i], strlen(pServerInfo->argv[i]));
			zargv[i] = (char*)malloc(strlen(pServerInfo->argv[i])+1);
			strcpy(zargv[i], pServerInfo->argv[i]);
		}
		zargv[i] = NULL;

		notifyWnd = pServerInfo->m_hwndNotifyProgress;
		// force the system to create a message queue for this thread
		{ 
			MSG msg;
			PeekMessage(&msg, NULL, WM_USER, WM_USER, PM_NOREMOVE);
		}
		// Exit the thread if the main application sets the "kill server"
		// event. The main application will set the "start game" event
		// before setting the "kill server" event.

		if (WaitForSingleObject(pServerInfo->m_hEventKillServerThread, 0)
			== WAIT_OBJECT_0)
			break; // Terminate this thread by existing the proc.

		// Reset event to indicate "not done", that is, game is in progress.
		// DIK20010819 m_hEventGameTerminated appears to be unused for any real purpose.
		ResetEvent(pServerInfo->m_hEventGameTerminated);
		if (!main(zargc, zargv))
		{
			xpprintf("%sServer startup failed\n", showtime());
			SendMessage(pServerInfo->m_hwndNotifyProgress, WM_SERVERKILLED, 0, 0);
			SetEvent(pServerInfo->m_hEventServerThreadKilled);
			return(0);
		}
//		SendMessage(pServerInfo->m_hwndNotifyProgress, WM_STARTTIMER, 0, (LPARAM)theWorld.GetFPS());

		while (TRUE)
		{
			if (serverKilled)
				break;
			netServer->Sched();
			// Just check for the event, don't actually wait for anything
			if (WaitForSingleObject(pServerInfo->m_hEventKillServerThread, 0)
				== WAIT_OBJECT_0)
			{
				serverKilled = TRUE;
				//xpprintf("%s Server thread killed\n", showtime());
				break; // Terminate this thread by exiting the proc.
			}
		}
		if (serverKilled)		// If interrupted by kill then...
			break;				// terminate this thread by exiting the proc.

	}
	netServer->StopSched();				// no more timer ticks please
	// DIK: FIXME
	theWorld[0]->EndGame(serverDeathMessage);
	for (i=0; i<zargc; i++)
	    free(zargv[i]);
	free(zargv);
	SetEvent(pServerInfo->m_hEventServerThreadKilled);

	return 0;
}
/* to get a printf from the worker/server thread into the main window
	i maintain 2 buffers.  After a call to xpprintfW, i post a message to the main
	window, telling him there's a buffer ready.
*/
void xpprintfW(const char *fmt, ...)
{
    va_list	 ap;			/* Argument pointer */
    int		 e = xperrno;	/* Store errno */
//	char	s[512];
	char*	s;
	char*	t;
	char	u[512];
	char*	w;
	int		susing;

//	if (!MsgAvail0)
	if (1)
	{
		s = Msg0;
		susing = 0;
		MsgAvail0 = TRUE;
	}
	else if (!MsgAvail1)
	{
		s = Msg1;
		susing = 1;
		MsgAvail1 = TRUE;
	}
	else
	{
	    va_start(ap, fmt);
	    vsprintf(u, fmt, ap);
		//_Trace("Error: no message space: %s\n", u);
 		if (MessageBox(NULL, u, "Error No message space", MB_OKCANCEL | MB_TASKMODAL) == IDCANCEL)
		{
			#ifdef	_DEBUG
//				xpmemShutdown();
			#endif
			ExitProcess(1);
		}
	    va_end(ap);
		return;
	}
    va_start(ap, fmt);
    vsprintf(u, fmt, ap);
	/* the stupid edit control (text window) needs \r\n to function within reason */
	t = s;
	w = u;
	while (*w)
	{
		if (*w == '\n')
			*t++ = '\r';
		*t++ = *w++;
	}
	*t = '\0';
	/* I ended up using SendMessage (which waits till the message is processed)
	   instead of PostMessage (which justs puts it in the queue).
	   I guess i should have a nice dynamic array
	*/
	Trace("Message: %s", s);
	if (pServerInfo && !serverKilled)
		SendMessage(pServerInfo->m_hwndNotifyProgress, WM_MSGAVAILABLE, susing, (LPARAM)s);
	else
		printf(s);	// no Window?  try stdout (probably won't go anywhere)
	
    va_end(ap);
}

///////////////////////////////////////////////////////////////////////////////
void StartWinServerTimer(long freq)
{
	SendMessage(pServerInfo->m_hwndNotifyProgress, WM_STARTTIMER, 0, (LPARAM)freq);
}

///////////////////////////////////////////////////////////////////////////////
void ServerThreadW32NotifyMainShutdown()
{
	SendMessage(pServerInfo->m_hwndNotifyProgress, WM_CLOSE, 0, 0);
}