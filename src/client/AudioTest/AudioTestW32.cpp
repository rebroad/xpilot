/* $Id: AudioTestW32.cpp,v 1.1 2004/05/02 00:17:06 dick Exp $
 *
 * XPilot, a multiplayer gravity war game.
 *
 *      Copyright (C) 2004 by
 *
 *      The XPilot Authors   <xpilot@xpilot.org>
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
 * Original XPilot audio code provided by Greg Renda (greg@ncd.com).
 *
 * Audio development tests for XPilot5.
 * Just load 'em up and play 'em.
 *
 * See http://www.openal.org for details of the engine
 */
/*
 * $Log: AudioTestW32.cpp,v $
 * Revision 1.1  2004/05/02 00:17:06  dick
 * XPilot OpenAL development and debugging.
 *
 */

#include "StdAfx.h"

#include "Audio.h"

PCSTR	szAppName = "AudioTest";

LRESULT CALLBACK MainWndProc(HWND hwnd,	UINT uMsg, WPARAM wParam, LPARAM lParam);

HWND		mainWnd;

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	WNDCLASS wc;
	MSG msg;
	BOOL bRet;

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

//	::hInstance = hInstance;	// save instance handle

	int		x, y, cx, cy;
		x  = CW_USEDEFAULT;
		cx = 400;
		y  = CW_USEDEFAULT;
		cy = 400;
	// Create the main window.
 	mainWnd = CreateWindow("XPilotWndClass", "XPilotAudioTest",
		WS_OVERLAPPEDWINDOW, x, y,
		cx, cy, (HWND)NULL,
		(HMENU)NULL, hInstance, (LPVOID) NULL);

	// If the main window cannot be created, terminate
	// the application.

	if (!mainWnd)
		return FALSE;


	ShowWindow(mainWnd, nCmdShow);
	UpdateWindow(mainWnd);

	audio.Init("lib/sounds.conf");

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

		case WM_CLOSE:
			DestroyWindow(hwnd);
			break;
		case WM_DESTROY:
			// Clean up window-specific data objects.
			PostQuitMessage(0);
			return 0;

		default:
			return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
	return 0;
}



