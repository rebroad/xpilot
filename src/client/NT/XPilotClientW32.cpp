/* $Id: XPilotClientW32.cpp,v 1.10 2004/06/03 05:41:33 dick Exp $
 *
 * Main Entry point for Windows

 * XPilot, a multiplayer gravity war game.  Copyright (C) 2002 by
 *
 *      Dick Balaska         <dick@xpilot.org>
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
 */
/*
 * $Log: XPilotClientW32.cpp,v $
 * Revision 1.10  2004/06/03 05:41:33  dick
 * Unregister the class when we are done
 *
 * Revision 1.9  2004/05/23 23:51:42  dick
 * ToggleShowStdoutW32() toggles the initial edit window to the front.
 *
 * Revision 1.8  2004/05/20 22:15:18  dick
 * Don't process network messages after closing the window during shutdown
 *
 * Revision 1.7  2004/05/01 20:32:04  dick
 * Comment out debug
 *
 * Revision 1.6  2004/04/26 07:00:21  dick
 * remove old debug
 *
 * Revision 1.5  2004/02/03 03:39:52  dick
 * Add Windows precompiled header support.
 *
 * Revision 1.4  2003/12/22 09:42:53  dick
 * don't copy argv
 *
 * Revision 1.3  2002/09/02 16:13:30  dick
 * Singleton ini becomes iniClient.
 *
 * Revision 1.2  2002/08/18 17:57:18  dick
 * Save and restore the window position for the client
 *
 * Revision 1.1  2002/08/11 21:08:08  dick
 * Remove MFC from the Windows client.
 *
 */
#include "StdAfx.h"

#include <windows.h>
#include <windowsx.h>
#include "cstring.h"
#include "Ini.h"
#include "error.h"
#include "xpprintf.h"
#include "winClient.h"
#include "sockerrs.h"
#include "xinit.h"
#include "paint.h"
#include "TalkW32.h"

extern void Key_event(XEvent *event);

static HWND		mainWnd;
static HWND		editWnd;
HWND		notifyWnd;
static bool		editWndShown = false;
static bool		virgin = true;
static bool		shuttingdown = false;
static LPCSTR	myCommandLine;

static	HFONT	editFont;

int		MAINWIN_WIDTH = 960;
int		MAINWIN_HEIGHT = 960;

LRESULT CALLBACK MainWndProc(HWND hwnd,	UINT uMsg, WPARAM wParam, LPARAM lParam);
long OnWSA_EVENT(WPARAM wParam, LPARAM lParam);
//LONG OnWMB_HTTP(UINT wParam, LONG lParam);
//LONG OnWMB_UDP(UINT wParam, LONG lParam);
//VOID CALLBACK ScoreServerW32TimerProc(HWND hwnd, UINT uMsg, UINT idEvent, DWORD dwTime);

void CallXPilotMain();

#define	WSA_EVENT		WM_USER+300			// from WSAAsyncSelect

///////////////////////////////////////////////////////////////////////////////
int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpszCmdLine, int nCmdShow)
{
	MSG msg;
	BOOL bRet;
	WNDCLASS wc;
	myCommandLine = lpszCmdLine;

	// Register the window class for the main window.

	if (!hPrevInstance)
	{
		wc.style = 0;
		wc.lpfnWndProc = (WNDPROC) MainWndProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = hInstance;
		wc.hIcon = LoadIcon((HINSTANCE) hInstance,
			MAKEINTRESOURCE(100));
		wc.hCursor = LoadCursor((HINSTANCE) NULL,
			IDC_ARROW);
		wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
		wc.lpszMenuName =  "MainMenu";
		wc.lpszClassName = "XPilotWndClass";

		if (!RegisterClass(&wc))
			return FALSE;
	}

	::hInstance = hInstance;	// save instance handle

	RECT	rect;
	int		x, y, cx, cy;
	if (!iniClient.GetWindowPositions(&rect))
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
 	mainWnd = CreateWindow("XPilotWndClass", "XPilotClient",
		WS_OVERLAPPEDWINDOW, x, y,
		cx, cy, (HWND)NULL,
		(HMENU)NULL, hInstance, (LPVOID) NULL);

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

//	Trace("Wnd: %x Msg: $%x\n", hwnd, uMsg);
	switch (uMsg)
	{
		case WM_CREATE:
		{
			// Initialize the window.
			LPCREATESTRUCT	lpcs = (LPCREATESTRUCT)lParam;
			//CREATESTRUCT	editcs = {
			Trace("lpcs x/y = %d/%d\n", lpcs->cx, lpcs->cy);
//#if 0
			editWnd = CreateWindow("EDIT", "",
				WS_CHILD|WS_VISIBLE|WS_VSCROLL|ES_MULTILINE|ES_AUTOVSCROLL/*|ES_READONLY*/,
				5, 5, lpcs->cx - 20, lpcs->cy - 40, hwnd, NULL, hInstance, NULL);
			editWndShown = true;
			printfWnd = editWnd;
			editFont = CreateFont(-12, 0, 0, 0, FW_MEDIUM, FALSE, FALSE, FALSE,
				ANSI_CHARSET, OUT_DEVICE_PRECIS, CLIP_DEFAULT_PRECIS,
				PROOF_QUALITY, FF_SWISS|DEFAULT_PITCH, NULL);
			if (editFont)
				SendMessage(editWnd, WM_SETFONT, (WPARAM)editFont, TRUE);
			else
				xpprintf("Couldn't create edit font\n");
//#endif
			InitWinX(hwnd);
			return 0;
		}
		case WM_PAINT:
			// Paint the window's client area.
			if (virgin)
			{
				virgin = false;
				CallXPilotMain();
//				theApp.Startup();
//				SetTimer(mainWnd, 1, 100, ScoreServerW32TimerProc);
			}
			return DefWindowProc(hwnd, uMsg, wParam, lParam);

			return 0;
		case WM_ERASEBKGND:
		{
			if (hwnd == editWnd)
			{
				RECT	r;
				HDC		hDC = (HDC)wParam;
				GetClientRect(editWnd, &r);
				FillRect(hDC, &r, (HBRUSH)GetStockObject(BLACK_BRUSH));
				return(1);
			}
			else
				return DefWindowProc(hwnd, uMsg, wParam, lParam);
		}
		case WM_CTLCOLOR:
			if (hwnd == editWnd)
			{
				Trace("Size: wnd=%x\n", hwnd);

			}

		case WM_SIZE:
		{
			// Set the size and position of the window.
			int w = GET_X_LPARAM(lParam);
			int	h = GET_Y_LPARAM(lParam);
			if (!virgin)
				Resize(top, w, h);

			MoveWindow(editWnd, 0, 0, w, h, TRUE);
			Trace("Size: wnd=%x, w/h = %d/%d\n", hwnd, w, h);
			return 0;
		}
		case WM_CLOSE:
		{
			RECT	r;

			xpilotShutdown();
			shuttingdown = true;
			iniClient.SaveWindowPositions(hwnd);
			if (GetWindowRect(hwnd, &r))
			{
				if (r.left > -30000 && r.top > -30000)
				{
					Trace("Rect: %d/%d  %d/%d\n", r.left, r.top, r.right, r.bottom);
				}
			}
			DestroyWindow(hwnd);
			break;
		}
		case WM_DESTROY:
			// Clean up window-specific data objects.
			printfWnd = NULL;
			DeleteObject(editFont);
			WSACleanup();
			PostQuitMessage(0);
			WinXShutdown();
			#if defined(_XPMEM)
				xpmemShutdown();
			#endif
			return 0;

		case WM_KEYDOWN:
		{
			XKeyEvent xk;

			if (lParam & 0x4000)		// don't bother with auto-repeat
				return(0);
			xk.keycode = (lParam >> 16) & 0x1FF;
			xk.ascii = wParam;
			xk.type = KeyPress;
			//Trace("KeyDown: c=%04X k=%04X flags=%04X\n", xk.ascii, xk.keycode, lParam);
			Key_event((XEvent*)&xk);
			return(0);
		}
		case WM_KEYUP:
		{
			XKeyEvent xk;

			xk.keycode = (lParam >> 16) & 0x1FF;
			xk.ascii = wParam;
			xk.type = KeyRelease;
			//Trace("KeyUp:   c=%04X k=%04X flags=%04X\n", xk.ascii, xk.keycode, lParam);
			Key_event((XEvent*)&xk);
			return(0);
		}

		//
		// Process other messages.
		//
		case WSA_EVENT:
			return(OnWSA_EVENT(wParam, lParam));
//		case WMB_HTTP:
//			return(OnWMB_HTTP(wParam, lParam));
//		case WMB_UDP:
//			return(OnWMB_UDP(wParam, lParam));


		default:
			return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
long OnWSA_EVENT(WPARAM wParam, LPARAM lParam)
{
	if (shuttingdown)		// once, i received a winsock event after nuking
		return(0);			// the network layer
	int ret = Net_input();
//	PaintWinClient();
	return(0);
}

///////////////////////////////////////////////////////////////////////////////
void ToggleShowStdoutW32()
{
	editWndShown = !editWndShown;
	Trace("editWndShown = %d\n", editWndShown);
	if (editWndShown)
	{
		ShowWindow(editWnd, SW_SHOW);
		SetWindowPos(editWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);
	}
	else
	{
		//SetWindowPos(editWnd, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);
		ShowWindow(editWnd, SW_HIDE);
	}

}

///////////////////////////////////////////////////////////////////////////////
COLORREF	GetXPilotColor(int which, COLORREF defcolor)
{
	String		key;
	String		cs;
//	COLORREF	newcolor;
	char		out[128];
	key.printf("color%d", which);
	cs = ::GetPrivateProfileString("Settings", key, "", out, 128, "XPilot.ini");
	if (!cs.GetLength())
		return(defcolor);
// DIK FIXME:
//	newcolor = cs.ParseColor();
//	return(newcolor);
	return(defcolor);
}

#if 0
///////////////////////////////////////////////////////////////////////////////
int		GetMaxColors()
{
	int	color;
	color = GetPrivateProfileInt("Settings", "maxColors", 0, ini.GetXpilotIniFile(0));
	if (!color)
		color = GetPrivateProfileInt("Settings", "maxColors", 0, ini.GetXpilotIniFile(1));
	if (!color)
		color = GetPrivateProfileInt("Settings", "maxColors", 0, ini.GetXpilotIniFile(2));
	return(color);
}
#endif

///////////////////////////////////////////////////////////////////////////////
void	Progress(const char* s, ...)
{
	String	cs;
	va_list args;
	va_start(args, s);
	cs.vprintf(s, args);
	xpprintf("%s%s\n", showtime(), (PCSTR)cs);
}

///////////////////////////////////////////////////////////////////////////////
// TEMP FIXME
const char*	DoTalkWindow(void)
{
//	return("hi");
	TalkWindow::Do(mainWnd, hInstance);
	return(TalkWindow::ts);
}

///////////////////////////////////////////////////////////////////////////////
void CallXPilotMain()
{
	int		ret;
	int		argc = 1;
	char*	argv[256];
	char	cs[1024];

//	String*		ccs[256];
	int			args=0;

//	GetParentFrame()->SetWindowText("XPilot");
	strncpy(cs, myCommandLine, 1024);

	argv[0] = "xpilot";

	argv[1] = strtok(cs, " \t\n\r\0");
	if (argv[1])
	{
//		if ((ccs[args] = CheckFileOpts(argv[1], &argc, argv)) != NULL)
//			args++;
		argc++;
		while ((argv[argc] = strtok(NULL, " \t\n\r\0")) != (char*)NULL)
		{
//			if ((ccs[args] = CheckFileOpts(argv[1], &argc, argv)) != NULL)
//				args++;
			argc++;
		}
	}
	// Here is where we call xpilot proper.
	// This gives Windows time to setup/settle down before we do fun things
	Trace("Eat Me\n");
	ret = main(argc, argv);

	Trace("ret=%d\n", ret);
//	for (int i=0; i<args; i++)
//		delete ccs[i];

	if (!ret)
	{
		ShowWindow(editWnd, SW_HIDE);
		editWndShown = false;
		ret = WSAAsyncSelect(Net_fd(), mainWnd, WSA_EVENT, FD_CLOSE|FD_READ);
		if (ret)
		{
			char	s[256];
			sprintf(s, "AsyncSelect error=%d (%s)",
				WSAGetLastError(), GetSockErrText(WSAGetLastError()));
			MessageBox(notifyWnd, s, NULL, MB_OK);
		}
		Net_input();
		ret = WSAAsyncSelect(Net_fd(), mainWnd, WSA_EVENT, FD_CLOSE|FD_READ);

	}
	else
	{
		Progress("XPilotClient init failed");
	}
}
