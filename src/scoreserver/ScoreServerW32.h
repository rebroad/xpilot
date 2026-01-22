/* $Id: ScoreServerW32.h,v 1.2 2004/04/21 23:28:44 dick Exp $
 *
 * ScoreServerW32 - Windows interface to the ScoreServer
 * The Windows' (95/98/ME/NT/2K/XP/Win32/W32) main entry file
 *
 *      Copyright (C) 2004 by
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
 * $Log: ScoreServerW32.h,v $
 * Revision 1.2  2004/04/21 23:28:44  dick
 * Startup a UDP server (via NetServer) that allows xp5 to connect to us,
 * monitor our health, and shut us down.
 *
 * Revision 1.1  2004/02/21 01:22:33  dick
 * Windows ScoreServer minimizes to mini task tray.
 *
 */

#ifndef	_SCORESERVERW32_H_
#define	_SCORESERVERW32_H_


#define	WM_TRAYICONMSG	(WM_APP+200)

class ScoreServerW32
{
public:
	ScoreServerW32();

	int			WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, 
						LPSTR lpszCmdLine, int nCmdShow);
	LRESULT		MainWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	void		SendWindowsShutdownMessage();

private:
	void		AddMiniTrayIcon();
	void		RemoveMiniTrayIcon();

//	void		StartServer();
//	void		StopServer();
//	LONG		OnGetHostName(WPARAM wParam, LPARAM lParam);
//	LONG		OnStartTimer(UINT unused, LONG tickcount);
	LONG		OnTrayIconMsg(WPARAM /*wParam*/, LPARAM lParam);
	LONG OnWMB_HTTP(UINT wParam, LONG lParam);
	LONG OnWMB_UDP(UINT wParam, LONG lParam);
	static VOID CALLBACK TimerProc(HWND hwnd, UINT uMsg, UINT idEvent, DWORD dwTime);

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
//	UINT		gTimer;			// game timer


};

extern	ScoreServerW32	scoreServerW32;		// The singleton

#endif	// _SCORESERVERW32_H_
