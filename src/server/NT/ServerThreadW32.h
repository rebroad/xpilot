/* $Id: ServerThreadW32.h,v 1.5 2004/05/07 01:13:39 dick Exp $
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
 * $Log: ServerThreadW32.h,v $
 * Revision 1.5  2004/05/07 01:13:39  dick
 * Win32 has no window for SendDialogUpdate (was an MFC server thing).
 *
 * Revision 1.4  2004/04/16 21:06:54  dick
 * ServerThread becomes NetServer
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
 */

#ifndef	_WINSVRTHREAD_H_
#define	_WINSVRTHREAD_H_

typedef	struct	CServerThreadInfo CServerThreadInfo;

struct ServerThreadW32
{
//	HWND m_hwndNotifyRecalcDone;
	HANDLE m_hEventStartGame;
	HANDLE m_hEventGameTerminated;
	HANDLE m_hEventKillServerThread;
	HANDLE m_hEventServerThreadKilled;
	HWND m_hwndNotifyProgress;
	BOOL	m_hostNameFound;

	int		argc;
	char*	argv[256];
};

#define	WM_MSGAVAILABLE		(WM_APP+1)
#define	WM_UPDATESCORES		(WM_APP+2)
#define	WM_STARTTIMER		(WM_APP+3)
#define	WM_SERVERKILLED		(WM_APP+4)


// Controlling function for the worker thread.
extern	unsigned long ServerThreadW32Proc(LPVOID pParam /* CServerThreadInfo ptr */);

extern	void	ServerThreadW32NotifyMainShutdown();

#endif	// _WINSVRTHREAD_H_
