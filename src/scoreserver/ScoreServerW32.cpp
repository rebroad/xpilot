/* $Id: ScoreServerW32.cpp,v 1.7 2004/07/07 19:28:34 dick Exp $
 *
 * XPScoreServer - Who's on first?
 * The Windows' (95/98/ME/NT/2K/XP/Win32/W32) main entry file
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
 * $Log: ScoreServerW32.cpp,v $
 * Revision 1.7  2004/07/07 19:28:34  dick
 * Clean up the Windows class on exit
 *
 * Revision 1.6  2004/04/21 23:28:44  dick
 * Startup a UDP server (via NetServer) that allows xp5 to connect to us,
 * monitor our health, and shut us down.
 *
 * Revision 1.5  2004/04/16 21:07:59  dick
 * NetServer becomes a little UDP engine that can be plugged into different apps.
 *
 * Revision 1.4  2004/02/21 01:22:33  dick
 * Windows ScoreServer minimizes to mini task tray.
 *
 * Revision 1.3  2004/02/03 03:39:53  dick
 * Add Windows precompiled header support.
 *
 * Revision 1.2  2002/07/09 21:57:27  dick
 * xpprintf needs showtime()
 *
 * Revision 1.1  2002/06/25 05:19:24  dick
 * The Windows frontend to the ScoreServer.
 *
 */
#include "StdAfx.h"

#include <windows.h>
#include <windowsx.h>
#include <stdio.h>

#include "config.h"
#include "error.h"
#include "xpprintf.h"
#include "NetServer.h"
#include "Http.h"
#include "ScoreServer.h"
#include "ScoreServerCfg.h"
#include "ScoreServerW32.h"

const char* szAppName= APPNAME;

#define	MAINWIN_WIDTH	490
#define	MAINWIN_HEIGHT	200

// mini-tray menu identifiers
#define	IDM_TERMINATE	1
#define	IDM_SHOWWINDOW	2

ScoreServerW32	scoreServerW32;

HWND		notifyWnd;

int			serverKilled;
UINT		myTimer;

LRESULT CALLBACK MainWndProc(HWND hwnd,	UINT uMsg, WPARAM wParam, LPARAM lParam);
//LONG OnWMB_HTTP(UINT wParam, LONG lParam);
//LONG OnWMB_UDP(UINT wParam, LONG lParam);
//VOID CALLBACK ScoreServerW32TimerProc(HWND hwnd, UINT uMsg, UINT idEvent, DWORD dwTime);

///////////////////////////////////////////////////////////////////////////////
ScoreServerW32::ScoreServerW32()
{
	virgin = true;
}

///////////////////////////////////////////////////////////////////////////////
int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpszCmdLine, int nCmdShow)
{
	return(scoreServerW32.WinMain(hInstance, hPrevInstance, lpszCmdLine, nCmdShow));
}

///////////////////////////////////////////////////////////////////////////////
int ScoreServerW32::WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
								   LPSTR lpszCmdLine, int nCmdShow)
{
	MSG msg;
	BOOL bRet;
	WNDCLASS wc;
	UNREFERENCED_PARAMETER(lpszCmdLine);

	// Register the window class for the main window.

	if (!hPrevInstance)
	{
		wc.style = 0;
		wc.lpfnWndProc = (WNDPROC) ::MainWndProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = hInstance;
		wc.hIcon = LoadIcon((HINSTANCE) hInstance,
			MAKEINTRESOURCE(100));
		wc.hCursor = LoadCursor((HINSTANCE) NULL,
			IDC_ARROW);
		wc.hbrBackground = (HBRUSH)GetStockObject(DKGRAY_BRUSH);
		wc.lpszMenuName =  "MainMenu";
		wc.lpszClassName = "MainWndClass";

		if (!RegisterClass(&wc))
			return FALSE;
	}

	hinst = hInstance;	// save instance handle

	// Create the main window.

	mainWnd = CreateWindow("MainWndClass", "XPScoreServer",
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
		MAINWIN_WIDTH, MAINWIN_HEIGHT, (HWND) NULL,
		(HMENU) NULL, hinst, (LPVOID) NULL);

	// If the main window cannot be created, terminate
	// the application.

	if (!mainWnd)
		return FALSE;

	theApp.cfg.hWnd = mainWnd;
	notifyWnd = mainWnd;	// where the network layer sends his messages

	// Show the window and paint its contents.

	int ret;
	WSADATA	wsaData;
	ret = WSAStartup(1, &wsaData);
	if (ret)
	{
		MessageBox(mainWnd, "Failed to initialize Windows sockets", "Error", MB_OK);
		return(ret);
	}

	ShowWindow(mainWnd, nCmdShow);
	UpdateWindow(mainWnd);

	AddMiniTrayIcon();

	// Start the message loop.
 	while( (bRet = GetMessage( &msg, NULL, 0, 0 )) != 0)
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

	UnregisterClass(wc.lpszClassName, wc.hInstance);

	// Return the exit code to the system.
	return msg.wParam;
}

///////////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK MainWndProc(
	HWND hwnd,		  // handle to window
	UINT uMsg,		  // message identifier
	WPARAM wParam,	  // first message parameter
	LPARAM lParam)	  // second message parameter
{
	return(scoreServerW32.MainWndProc(hwnd, uMsg, wParam, lParam));
}

///////////////////////////////////////////////////////////////////////////////
LRESULT ScoreServerW32::MainWndProc(
	HWND hwnd,		  // handle to window
	UINT uMsg,		  // message identifier
	WPARAM wParam,	  // first message parameter
	LPARAM lParam)	  // second message parameter
{

	//Trace("Wnd: %x Msg: %x\n", hwnd, uMsg);
	switch (uMsg)
	{
		case WM_CREATE:
		{
			// Initialize the window.
			LPCREATESTRUCT	lpcs = (LPCREATESTRUCT)lParam;
			Trace("lpcs x/y = %d/%d\n", lpcs->cx, lpcs->cy);
			editWnd = CreateWindow("EDIT", "",
				WS_CHILD|WS_VISIBLE|WS_VSCROLL|ES_MULTILINE|ES_AUTOVSCROLL/*|ES_READONLY*/,
				5, 5, lpcs->cx - 20, lpcs->cy - 40, hwnd, NULL, hinst, NULL);
			printfWnd = editWnd;
			editFont = CreateFont(-12, 0, 0, 0, FW_MEDIUM, FALSE, FALSE, FALSE,
				ANSI_CHARSET, OUT_DEVICE_PRECIS, CLIP_DEFAULT_PRECIS,
				PROOF_QUALITY, FF_SWISS|DEFAULT_PITCH, NULL);
			if (editFont)
				SendMessage(editWnd, WM_SETFONT, (WPARAM)editFont, TRUE);
			else
				xpprintf("Couldn't create edit font\n");
			return 0;
		}
		case WM_PAINT:
			// Paint the window's client area.
			//if (hwnd == editWnd)
			if (virgin)
			{
				virgin = false;
				theApp.Startup();

				SetTimer(mainWnd, 1, 100, TimerProc);
			}
			return DefWindowProc(hwnd, uMsg, wParam, lParam);

			return 0;
		case WM_SIZE:
		{
			// Set the size and position of the window.
			int w = GET_X_LPARAM(lParam);
			int	h = GET_Y_LPARAM(lParam);

			MoveWindow(editWnd, 5, 5, w-10, h-10, TRUE);
			Trace("Size: wnd=%x, w/h = %d/%d\n", hwnd, w, h);
			return 0;
		}
		case WM_CLOSE:
		{
			RECT	r;
			if (GetWindowRect(hwnd, &r))
			{
				if (r.left > -30000 && r.top > -30000)
				{
					Trace("Rect: %d/%d  %d/%d\n", r.left, r.top, r.right, r.bottom);
					theApp.cfg.window.left = r.left;
					theApp.cfg.window.top = r.top;
					theApp.cfg.window.bottom = r.bottom;
					theApp.cfg.window.right = r.right;
				}
			}
			theApp.Shutdown();
			RemoveMiniTrayIcon();
			DestroyWindow(hwnd);
			break;
		}
		case WM_DESTROY:
			// Clean up window-specific data objects.
			printfWnd = NULL;
			DeleteObject(editFont);
			WSACleanup();
			PostQuitMessage(0);
			return 0;
		case WM_SYSCOMMAND:
 			if (trayIconOn && wParam == SC_MINIMIZE)
			{
				DefWindowProc(hwnd, uMsg, wParam, lParam);
				ShowWindow(mainWnd, SC_MINIMIZE);
				// theApp.OnIdle(0);
				theApp.cfg.windowMode = 1;
				return(ShowWindow(mainWnd, SW_HIDE));
			}
			return DefWindowProc(hwnd, uMsg, wParam, lParam);

		case WM_COMMAND:
			switch (wParam)
			{
			case IDM_TERMINATE:
				OnPopupExit();
				break;
			case IDM_SHOWWINDOW:
				OnPopupShowWindow();
			default:
				return DefWindowProc(hwnd, uMsg, wParam, lParam);
			}
		//
		// Process other non-Windows(tm) messages.
		//
		case WM_TRAYICONMSG:
			return(OnTrayIconMsg(wParam, lParam));
		case WMB_HTTP:
			return(OnWMB_HTTP(wParam, lParam));
		case WMB_UDP:
			return(OnWMB_UDP(wParam, lParam));

		default:
			return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
LONG ScoreServerW32::OnWMB_HTTP(UINT wParam, LONG lParam)
{
	switch (WSAGETSELECTEVENT(lParam))
	{
	case FD_ACCEPT:
		SOTRACE("WMB_HTTP: accept %d\n", wParam);
		HttpAccept((SOCKET)wParam);
		return(0);
	case FD_READ:
		SOTRACE("WMB_HTTP: read %d\n", wParam);
		HttpRead((SOCKET)wParam);
		return(0);
	case FD_CLOSE:
		SOTRACE("WMB_HTTP: close %d\n", wParam);
		HttpClose((SOCKET)wParam);
		return(0);
	}
	xpprintf("%sOnWMB_HTTP: unhandled event %d for sock %d\n", showtime(), lParam, wParam);
	return(0);
}

///////////////////////////////////////////////////////////////////////////////
LONG ScoreServerW32::OnWMB_UDP(UINT wParam, LONG lParam)
{
	theApp.UdpSelected();
	return(0);
}

///////////////////////////////////////////////////////////////////////////////
// static
VOID CALLBACK ScoreServerW32::TimerProc(HWND hwnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{
	theApp.TimerTick();
}

///////////////////////////////////////////////////////////////////////////////
void ScoreServerW32::AddMiniTrayIcon()
{
	// add icon to icon tray
	if (!trayIconOn)
	{
		NOTIFYICONDATA nd;
		nd.cbSize = sizeof(NOTIFYICONDATA);
		nd.hWnd = mainWnd;
		nd.uID  = 1;
		nd.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
		nd.uCallbackMessage = WM_TRAYICONMSG;
		//nd.hIcon = AfxGetApp()->LoadIcon(IDI_ICON_TRAY);
		nd.hIcon = ::LoadIcon(hinst, MAKEINTRESOURCE(100));
		strcpy(nd.szTip, szAppName);
		Shell_NotifyIcon(NIM_ADD, &nd);

		miniMenu = CreatePopupMenu();
		AppendMenu(miniMenu, MF_STRING, IDM_SHOWWINDOW, "Show Window");
//		AppendMenu(miniMenu, MF_STRING|MF_POPUP, (UINT)mainWnd, szAppName);
		AppendMenu(miniMenu, MF_MENUBREAK, 0, NULL);
		AppendMenu(miniMenu, MF_STRING, IDM_TERMINATE, "Terminate Server");
		//AppendMenu(miniMenu, MF_MENUBREAK, 0, NULL);
	}
	trayIconOn = true;
}

///////////////////////////////////////////////////////////////////////////////
void ScoreServerW32::RemoveMiniTrayIcon()
{
	if (trayIconOn)
	{
		DestroyMenu(miniMenu);
		NOTIFYICONDATA nd;
		nd.cbSize = sizeof(NOTIFYICONDATA);
		nd.hWnd = mainWnd;
		nd.uID  = 1;
		nd.uFlags = 0;
		nd.uCallbackMessage = 0;
		nd.hIcon = NULL;
		Shell_NotifyIcon(NIM_DELETE, &nd);
	}
	trayIconOn = false;
}

///////////////////////////////////////////////////////////////////////////////
LONG ScoreServerW32::OnTrayIconMsg(WPARAM /*wParam*/, LPARAM lParam)
{
	WORD wMsg = LOWORD(lParam);
	switch (wMsg)
	{
		case WM_LBUTTONDBLCLK:
			OnPopupShowWindow();
			break;

		case WM_RBUTTONDOWN:
		{
			SetForegroundWindow(mainWnd);
			HMENU submenu = GetSubMenu(miniMenu, 0);
			POINT	cursor;
			GetCursorPos(&cursor);
			RECT	ignored;
			TrackPopupMenu(miniMenu, TPM_RIGHTBUTTON, cursor.x, cursor.y, 0, mainWnd, &ignored);

			PostMessage(mainWnd, WM_NULL, 0, 0);
			break;
		}

		default:
			break;
	}
	return(0);
}

///////////////////////////////////////////////////////////////////////////////
void ScoreServerW32::OnPopupShowWindow()
{
	//ShowWindow(mainWnd, SW_SHOW);
	ShowWindow(mainWnd, SW_RESTORE);
	theApp.cfg.windowMode = 0;
	SetForegroundWindow(mainWnd);
}

///////////////////////////////////////////////////////////////////////////////
void ScoreServerW32::OnPopupExit()
{
	SendMessage(mainWnd, WM_CLOSE, 0, 0);
}

///////////////////////////////////////////////////////////////////////////////
void StartWinServerTimer(long freq)
{
	//SendMessage(pServerInfo->m_hwndNotifyProgress, WM_STARTTIMER, 0, (LPARAM)freq);
	myTimer= ::SetTimer(NULL, 0, freq, (TIMERPROC)NetServerTimerProc);
}

///////////////////////////////////////////////////////////////////////////////
void ScoreServerW32::SendWindowsShutdownMessage()
{
	SendMessage(mainWnd, WM_CLOSE, 0, 0);
}
