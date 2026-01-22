/* $Id: XPilotServerW32.cpp,v 1.7 2004/05/30 16:17:54 dick Exp $
 *
 * XPilotServerW32 - Windows interface to the XPilot server.
 * The Windows' (95/98/ME/NT/2K/XP/Win32/W32) main entry file
 *
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
 * $Log: XPilotServerW32.cpp,v $
 * Revision 1.7  2004/05/30 16:17:54  dick
 * Unregister our class so it doesn't leak
 *
 * Revision 1.6  2004/04/16 21:06:54  dick
 * ServerThread becomes NetServer
 *
 * Revision 1.5  2004/02/21 01:20:40  dick
 * Deadwood delete
 *
 * Revision 1.4  2004/02/20 05:11:39  dick
 * Windows server saves his window position and state.
 *
 * Revision 1.3  2004/02/03 03:39:54  dick
 * Add Windows precompiled header support.
 *
 * Revision 1.2  2002/09/09 23:42:56  dick
 * Handle connecting a LocalCtl to this server.
 *
 * Revision 1.1  2002/08/31 22:19:17  dick
 * Remove MFC from the server
 *
 */
#include "StdAfx.h"

#include <windows.h>
#include <windowsx.h>
#include <stdio.h>

#include "config.h"
#include "portability.h"
#include "error.h"
#include "xpprintf.h"
#include "IniServer.h"
#include "XPilotServerW32.h"
#include "NT/winNet.h"
#include "NetServer.h"

#define	MAINWIN_WIDTH	490
#define	MAINWIN_HEIGHT	200


// mini-tray menu identifiers
#define	IDM_TERMINATE	1
#define	IDM_SHOWWINDOW	2

// The one and only app
XPilotServerW32	theApp;
HWND			notifyWnd;

LRESULT CALLBACK MainWndProc(HWND hwnd,	UINT uMsg, WPARAM wParam, LPARAM lParam);
//LONG OnWMB_HTTP(UINT wParam, LONG lParam);
//LONG OnWMB_UDP(UINT wParam, LONG lParam);
//VOID CALLBACK XPilotServerW32TimerProc(HWND hwnd, UINT uMsg, UINT idEvent, DWORD dwTime);

XPilotServerW32::XPilotServerW32()
{
	virgin = true;
	trayIconOn = false;
	serverRunning = false;
	gTimer = 0;

	m_hEventStartGame = CreateEvent(NULL, FALSE, FALSE, NULL); // auto reset, initially reset
	m_hEventGameTerminated = CreateEvent(NULL, TRUE, TRUE, NULL); // manual reset, initially set
	m_hEventKillServerThread = CreateEvent(NULL, FALSE, FALSE, NULL); // auto reset, initially reset
	m_hEventServerThreadKilled = CreateEvent(NULL, FALSE, FALSE, NULL); // auto reset, initially reset

	m_ServerThreadInfo.m_hEventStartGame = m_hEventStartGame;
	m_ServerThreadInfo.m_hEventGameTerminated = m_hEventGameTerminated;
	m_ServerThreadInfo.m_hEventKillServerThread = m_hEventKillServerThread;
	m_ServerThreadInfo.m_hEventServerThreadKilled = m_hEventServerThreadKilled;

	m_pServerWorkerThread = NULL;
}

///////////////////////////////////////////////////////////////////////////////
int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpszCmdLine, int nCmdShow)
{
	return(theApp.WinMain(hInstance, hPrevInstance, lpszCmdLine, nCmdShow));
}

///////////////////////////////////////////////////////////////////////////////
int XPilotServerW32::WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
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
		wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(1));
		wc.hCursor = LoadCursor((HINSTANCE) NULL,
			IDC_ARROW);
		wc.hbrBackground = (HBRUSH)GetStockObject(DKGRAY_BRUSH);
		wc.lpszMenuName =  "MainMenu";
		wc.lpszClassName = "MainsWndClass";

		if (!RegisterClass(&wc))
			return FALSE;
	}

	hinst = hInstance;	// save instance handle

	RECT	rect;
	int		x, y, cx, cy;
	if (!iniServer.GetWindowPositions(&rect))
	{
		x  = CW_USEDEFAULT;
		cx = MAINWIN_WIDTH;
		y  = CW_USEDEFAULT;
		cy = MAINWIN_HEIGHT;
	}
	else
	{
		x = rect.left;
		cx = rect.right - rect.left;
		y = rect.top;
		cy = rect.bottom - rect.top;
	}
	// Create the main window.

	mainWnd = CreateWindow("MainsWndClass", "XPilotServer",
		WS_OVERLAPPEDWINDOW, x, y,
		cx, cy, (HWND)NULL,
		(HMENU) NULL, hinst, (LPVOID) NULL);

	// If the main window cannot be created, terminate
	// the application.

	if (!mainWnd)
		return FALSE;

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
LRESULT CALLBACK MainWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return(theApp.MainWndProc(hwnd, uMsg, wParam, lParam));
}

///////////////////////////////////////////////////////////////////////////////
LRESULT XPilotServerW32::MainWndProc(
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
			//CREATESTRUCT	editcs = {
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
			if (virgin)
			{
				virgin = false;
				StartServer();
				if (iniServer.windowMode)
				{
					ShowWindow(hwnd, SC_MINIMIZE);
					return(ShowWindow(hwnd, SW_HIDE));
				}
			}
			return DefWindowProc(hwnd, uMsg, wParam, lParam);

			//return 0;
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
			StopServer();
			iniServer.SaveWindowPositions(hwnd);
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
				iniServer.windowMode = 1;
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
		// Process other messages.
		//
//		case WMB_HTTP:
//			return(OnWMB_HTTP(wParam, lParam));
//		case WMB_UDP:
//			return(OnWMB_UDP(wParam, lParam));
//	ON_MESSAGE(WM_MSGAVAILABLE, OnMsgAvailable)
//	ON_MESSAGE(WM_UPDATESCORES, OnUpdateScores)
//	ON_MESSAGE(WM_SERVERKILLED, OnServerKilled)
		case WM_GETHOSTNAME:
			return(OnGetHostName(wParam, lParam));
		case WM_STARTTIMER:
			return(OnStartTimer(wParam, lParam));
		case WM_TRAYICONMSG:
			return(OnTrayIconMsg(wParam, lParam));

		default:
			return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
	return 0;
}

#if 0
///////////////////////////////////////////////////////////////////////////////
LONG OnWMB_HTTP(UINT wParam, LONG lParam)
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

LONG OnWMB_UDP(UINT wParam, LONG lParam)
{
	theApp.UdpSelected();
	return(0);
}
#endif

///////////////////////////////////////////////////////////////////////////////
void XPilotServerW32::AddMiniTrayIcon()
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
		nd.hIcon = ::LoadIcon(hinst, MAKEINTRESOURCE(1));
		strcpy(nd.szTip, szAppName);
		Shell_NotifyIcon(NIM_ADD, &nd);

		miniMenu = CreatePopupMenu();
		AppendMenu(miniMenu, MF_STRING, IDM_SHOWWINDOW, "Show Window");
//		AppendMenu(miniMenu, MF_STRING|MF_POPUP, (UINT)mainWnd, szAppName);
		AppendMenu(miniMenu, MF_MENUBREAK, 0, NULL);
		AppendMenu(miniMenu, MF_STRING, IDM_TERMINATE, "Terminate Server");
		AppendMenu(miniMenu, MF_MENUBREAK, 0, NULL);
	}
	trayIconOn = true;
}

///////////////////////////////////////////////////////////////////////////////
void XPilotServerW32::RemoveMiniTrayIcon()
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
LONG XPilotServerW32::OnTrayIconMsg(WPARAM /*wParam*/, LPARAM lParam)
{
	WORD wMsg = LOWORD(lParam);
	switch (wMsg)
	{
		case WM_LBUTTONDBLCLK:
			OnPopupShowWindow();
			break;

		case WM_RBUTTONDOWN:
		{		/*
				ASSERT(AfxGetMainWnd() != NULL);

				CMenu Menu;
				if (!Menu.LoadMenu(IDR_ICON_TRAY_POPUP_MENU))
				{
					AfxMessageBox(_T("Unable to load menu"));
					return;
				}

				CMenu* pSubMenu = Menu.GetSubMenu(0);
				ASSERT(pSubMenu != NULL);

				// see KB-article Q135788
				::SetForegroundWindow(AfxGetMainWnd()->m_hWnd);

				CPoint Cursor;
				GetCursorPos(&Cursor);
				VERIFY(pSubMenu->TrackPopupMenu(0, Cursor.x, Cursor.y, AfxGetMainWnd()));

				// see KB-article Q135788
				::PostMessage(AfxGetMainWnd()->m_hWnd, WM_NULL, 0, 0);
				*/

			// see KB-article Q135788
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
void XPilotServerW32::OnPopupShowWindow()
{
	//ShowWindow(mainWnd, SW_SHOW);
	ShowWindow(mainWnd, SW_RESTORE);
	iniServer.windowMode = 0;
	SetForegroundWindow(mainWnd);
}

///////////////////////////////////////////////////////////////////////////////
void XPilotServerW32::OnPopupExit()
{
	SendMessage(mainWnd, WM_CLOSE, 0, 0);
}


///////////////////////////////////////////////////////////////////////////////
void XPilotServerW32::StartServer()
{
	if (!serverRunning)
	{
//		int		ret;
//		char	cs[1024];
//		char	ct[1024];
//		alarmWnd = GetSafeHwnd();
		if (m_pServerWorkerThread == NULL)
		{
			// Begin the worker thread.  It is ok to fill in the CThreadInfo
			// structure after the thread has started, because the thread
			// waits for the "start recalc" event before referring to the structure.
			m_pServerWorkerThread =
				//AfxBeginThread(ServerThreadProc, &m_ServerThreadInfo);
				CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ServerThreadW32Proc,
							 &m_ServerThreadInfo, 0, &serverThreadId);

		}

		m_ServerThreadInfo.m_hwndNotifyProgress = mainWnd;

		// The events are initially set or reset in the CreateEvent call;
		// but they may be left in an improperly initialized state if
		// a worker thread has been previously started and then prematurely
		// killed.  Set/reset the events to the proper initial state.
		// Set the "start recalc" event last, since it is the event the
		// triggers the starting of the worker thread recalculation.
		SetEvent(m_hEventGameTerminated);
		ResetEvent(m_hEventKillServerThread);
		ResetEvent(m_hEventServerThreadKilled);

		//m_start_server.SetWindowText("&Stop Server");
		serverRunning = TRUE;
//		strcpy(cs, theApp.m_lpCmdLine);
		m_ServerThreadInfo.argv[0] = "xpilots";
		m_ServerThreadInfo.argc = 1;

/*		m_ServerThreadInfo.argv[1] = strtok(cs, " \t\n\r\0");
		if (m_ServerThreadInfo.argv[1])
		{
			m_ServerThreadInfo.argc++;
			while ((m_ServerThreadInfo.argv[m_ServerThreadInfo.argc] = strtok(NULL, "\t\n\r\0")) != (char*)NULL)
				m_ServerThreadInfo.argc++;
		}
		strcpy(ct, m_commandline);

		m_ServerThreadInfo.argv[m_ServerThreadInfo.argc] = strtok(ct, " \t\n\r\0");
		if (m_ServerThreadInfo.argv[m_ServerThreadInfo.argc])
		{
			m_ServerThreadInfo.argc++;
			while ((m_ServerThreadInfo.argv[m_ServerThreadInfo.argc] = strtok(NULL, " \t\n\r\0")) != (char*)NULL)
				m_ServerThreadInfo.argc++;
		}
*/		Trace("Eat Me\n");
		SetEvent(m_hEventStartGame);

	}
}

///////////////////////////////////////////////////////////////////////////////
void XPilotServerW32::StopServer()
{
//	CReallyShutdown	crs;
//	if (crs.DoModal() != IDOK)
//		return;
	serverKilled = true;
	//::KillTimer(NULL, gTimer);
	SetEvent(m_hEventKillServerThread);
	SetEvent(m_hEventStartGame);

	WaitForSingleObject(m_hEventServerThreadKilled, 5*1000);

	m_pServerWorkerThread = NULL;
//	m_start_server.SetWindowText("&Start Server");
//	m_start_server.EnableWindow(FALSE);				/* can't restart yet... */
//	m_connectClient.EnableWindow(FALSE);	// can't connect to this server anymore
	serverRunning = FALSE;

}

///////////////////////////////////////////////////////////////////////////////
LONG XPilotServerW32::OnGetHostName(WPARAM wParam, LPARAM lParam)
{
	PostThreadMessage(serverThreadId, WM_GETHOSTNAME, wParam, lParam);
	return(0);
}

///////////////////////////////////////////////////////////////////////////////
LONG XPilotServerW32::OnStartTimer(UINT unused, LONG tickcount)
{
	gTimer= ::SetTimer(NULL, 0, tickcount, (TIMERPROC)NetServerTimerProc);
//	m_connectClient.EnableWindow(TRUE);
	return(0);
}

