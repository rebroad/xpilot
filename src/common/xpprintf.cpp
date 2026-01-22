/* $Id: xpprintf.cpp,v 1.12 2005/03/17 22:12:13 kps Exp $
 *
 * xpprintf - printf to someplace, because Windows doesn't have a stdout
 *      Copyright (C) 2002 by
 *
 *      The XPilot Authors   <xpilot@xpilot.org>
 *      Dick Balaska         <dick@xpilot.org>
 *      Jarrod Miller        <jarrod@xpilot.org>
 *      Bert Gijsbers        <bert@xpilot.org>
 *      Ken Ronny Schouten   <ken@xpilot.org>
 *      Bj�rn Stabell        <bjoern@xpilot.org>
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
 * $Log: xpprintf.cpp,v $
 * Revision 1.12  2005/03/17 22:12:13  kps
 * Get rid of warnings from makedepend about "non-portable whitespace".
 *
 * Revision 1.11  2004/05/01 19:13:05  dick
 * AudioTest needs Winsock2.h. hmm.
 *
 * Revision 1.10  2004/02/07 06:27:34  dick
 * xpprintf can print 2048 char strings now
 *
 * Revision 1.9  2004/02/03 03:39:52  dick
 * Add Windows precompiled header support.
 *
 * Revision 1.8  2004/01/07 06:32:19  dick
 * Deadwood delete
 *
 * Revision 1.7  2004/01/06 04:48:28  dick
 * Add support for building under Cygwin.  Still needs work and the output
 * is X11 and requires something like Cygwin's XWin.exe to run under.
 *
 * Revision 1.6  2002/08/31 22:00:03  dick
 * Check if printfWnd went away after the private message loop.  Maybe the
 * server has been terminated.
 *
 * Revision 1.5  2002/08/11 21:08:08  dick
 * Remove MFC from the Windows client.
 *
 * Revision 1.4  2002/07/09 19:55:15  dick
 * xpprintf doesn't do showtime()
 *
 * Revision 1.3  2002/06/26 06:56:36  dick
 * use xperrno instead of errno, seterrno(0) instead of errno = 0.
 *
 * Revision 1.2  2002/06/26 04:45:59  dick
 * HWND is a Windows phenomenon
 *
 * Revision 1.1  2002/06/21 00:37:39  dick
 * break xpprintf() out to his own file.
 *
 */
#include "StdAfx.h"

#if defined(_WINDOWS) && !defined(_CYGWIN)
#	include <windows.h>
#	include <io.h>
#	include <sys/stat.h>
#	ifndef	FD_SET
#		include <Winsock2.h>
#	endif
#else
#	define	_GNU_SOURCE
#	include <stdio.h>
#	include <time.h>
#	include <stdarg.h>
#	include <errno.h>
#	include <dirent.h>
#	include <sys/stat.h>
#	include <unistd.h>
#	include <ctype.h>
#	include <string.h>
#endif

#include <fcntl.h>

#include "Obj.h"
#include "xpprintf.h"
#include "config.h"
#include "error.h"
#include "const.h"
#include "portability.h"

ObjList	logEntries;
int		liveLogEntries = 100;

int		logH = 0;
bool	logToFile = false;
int		logType = LOGAPPEND;
int		logLevel = LOGMAX;

#if defined(_WINDOWS) && !defined(_CYGWIN)
	HWND printfWnd;
#endif

///////////////////////////////////////////////////////////////////////////////
PCSTR	versionBuildOpts =
#ifdef	_WINDOWS
	"Windows"
#endif
#ifdef	_UNIX
	"Unix"
#endif
#ifdef	_CYGWIN
	" Cygwin"
#endif
#ifdef	_DEBUG
	" DEBUG"
#endif
;

///////////////////////////////////////////////////////////////////////////////
static void	_xpprintf(const char** lpszFormat, va_list args, ...)
{

	char	u[2048];
	char*	s = u;

	vsnprintf(u, 2048, *lpszFormat, args);
	va_end(args);

#if defined(_WINDOWS) && !defined(_CYGWIN)
    int		 e = xperrno;		/* Store errno */
	char	m[2048];
	char*	t;
	char*	w;
	/* the stupid edit control (text window) needs \r\n to function within reason */
	s = m;
	t = s;
	w = u;
	while (*w)
	{
		if (*w == '\n')
			*t++ = '\r';
		*t++ = *w++;
	}
	*t = '\0';


	int		l;

//	CWnd*	d = GetCfg()->cWnd;
//	int		id = GetCfg()->idc_edit1;
//	if (!d || !d->m_hWnd )		// Don't try to send messages if the window is shutting down
//		return;
//	if (!d->m_hWnd)
//		return;
	l = SendMessage(printfWnd, WM_GETTEXTLENGTH, 0, 0);
//	TRACE("l=%d\n", l);
	if (l > 25000)				// dumb guess of how many lines the edit control can handle
	{
		int		l1;
		l1 = SendMessage(printfWnd, EM_LINEINDEX, 100, 0);
		SendMessage(printfWnd, EM_SETSEL, (WPARAM)0, (LPARAM)l1);
		SendMessage(printfWnd, EM_REPLACESEL, FALSE, (LPARAM)"");
	}
	l = SendMessage(printfWnd, WM_GETTEXTLENGTH, 0, 0);
	SendMessage(printfWnd, EM_SETSEL, (WPARAM)l, (LPARAM)l);
	SendMessage(printfWnd, EM_REPLACESEL, FALSE, (LPARAM)s);
	l = SendMessage(printfWnd, WM_GETTEXTLENGTH, 0, 0);
	SendMessage(printfWnd, EM_SETSEL, (WPARAM)l, (LPARAM)l);
	SendMessage(printfWnd, EM_SCROLLCARET, 0, 0);

	// Flush the Windows message queue to display this message now.
	MSG		msg;
	BOOL	bRet;
 	while(bRet = PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) != 0)
	{
		if (bRet == -1)
		{
			// handle the error and possibly exit
		}
		else
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	Trace(u);
	if (!printfWnd)		// maybe the message loop picked up a terminate command
		return;
#else
	printf("%s", s);
#endif

	LString* ls;
	if (logEntries.GetCount() >= liveLogEntries)
		ls = (LString*)logEntries.RemoveHead();
	else
		ls = new LString;
	ls->s = (PCSTR)s;
	ls->s.Enbracket();
	logEntries.Add(ls);

	// do that file logging thing
	if (logToFile && logH)
	{
		write(logH, s, strlen(s));
		return;
	}
//#define	FOPTS	O_WRONLY | O_CREAT
#if defined(_WINDOWS) && !defined(_CYGWIN)
#define	FMODE	(S_IREAD | S_IWRITE)
#else
#define	FMODE	(S_IREAD | S_IWRITE)
#endif
	if (logToFile)
	{
		String	fname;
		int		fopts;
		if (logType == LOGAPPEND)
			fopts = O_WRONLY | O_CREAT | O_APPEND;
		else
			fopts = O_WRONLY | O_CREAT | O_TRUNC;
		fname = szAppName;
		fname += ".log";
		if (!(logH = open(fname, fopts, FMODE)))
		{
			logToFile = FALSE;
			xpprintf("sys: Can't open log for output.\n");
			return;
		}
		write(logH, s, strlen(s));
		return;
	}
	if (logH)
	{
		close(logH);
		logH = 0;
	}
}

///////////////////////////////////////////////////////////////////////////////
void	xpprintf(const char* lpszFormat, ...)
{
    va_list	 ap;			/* Argument pointer */
	va_start(ap, lpszFormat);
	_xpprintf(&lpszFormat, ap);
}

///////////////////////////////////////////////////////////////////////////////
void	xpprintf(int level, const char* lpszFormat, ...)
{
    va_list	 ap;			/* Argument pointer */
	va_start(ap, lpszFormat);
	if (level <= logLevel)
		_xpprintf(&lpszFormat, ap);
}

///////////////////////////////////////////////////////////////////////////////
#if defined(_WINDOWS) && !defined(_CYGWIN)
BOOL WaitWithMessageLoop(HANDLE hEvent)
{
	DWORD dwRet;
	MSG msg;

	while(1)
	{
		dwRet = MsgWaitForMultipleObjects( 1,    // One event to wait for
		       &hEvent,        // The array of events
		       FALSE,          // Wait for 1 event
		       INFINITE,       // Timeout value
		       QS_ALLINPUT);   // Any message wakes up
		if(dwRet == WAIT_OBJECT_0)
		{
		  // The event was signaled, return
			return TRUE;
		} else if(dwRet == WAIT_OBJECT_0 + 1)
		{
		  // There is a window message available. Dispatch it.
			while(PeekMessage(&msg,NULL,0,0,PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		} else
		{
		  // Something else happened. Return.
			return FALSE;
		}
	}
}
#endif
