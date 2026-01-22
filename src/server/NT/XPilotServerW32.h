/* $Id: XPilotServerW32.h,v 1.1 2002/08/31 22:19:17 dick Exp $
 *
 * XPilotServerW32 - Windows interface to the World
 * The Windows' (95/98/ME/NT/2K/XP/Win32/W32) main entry file
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
 * $Log: XPilotServerW32.h,v $
 * Revision 1.1  2002/08/31 22:19:17  dick
 * Remove MFC from the server
 *
 */

#ifndef	_XPILOTSERVERW32_H_
#define	_XPILOTSERVERW32_H_

#include "ServerThreadW32.h"

#define	WM_TRAYICONMSG	(WM_APP+200)

class XPilotServerW32
{
public:
	XPilotServerW32();

	int			WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, 
						LPSTR lpszCmdLine, int nCmdShow);
	LRESULT		MainWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
	void		AddMiniTrayIcon();
	void		RemoveMiniTrayIcon();

	void		StartServer();
	void		StopServer();

	LONG		OnGetHostName(WPARAM wParam, LPARAM lParam);
	LONG		OnStartTimer(UINT unused, LONG tickcount);
	LONG		OnTrayIconMsg(WPARAM /*wParam*/, LPARAM lParam);

	void		OnPopupShowWindow();
	void		OnPopupExit();

private:

	HINSTANCE	hinst; 
	HWND		mainWnd;
	HWND		editWnd;
	bool		virgin;

	bool		trayIconOn;
	HMENU		miniMenu;

	HFONT		editFont;
	UINT		gTimer;			// game timer

	bool		serverRunning;
	DWORD		serverThreadId;
	HANDLE		m_pServerWorkerThread;
	ServerThreadW32 m_ServerThreadInfo;
	HANDLE		m_hEventStartGame;
	HANDLE		m_hEventGameTerminated;
	HANDLE		m_hEventKillServerThread;
	HANDLE		m_hEventServerThreadKilled;

};

#endif	// _XPILOTSERVERW32_H_
