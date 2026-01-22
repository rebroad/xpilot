/* $Id: TalkW32.cpp,v 1.2 2004/02/03 03:39:52 dick Exp $
 *
 * Talk window for Windows
 *
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
 * $Log: TalkW32.cpp,v $
 * Revision 1.2  2004/02/03 03:39:52  dick
 * Add Windows precompiled header support.
 *
 * Revision 1.1  2002/08/11 21:08:08  dick
 * Remove MFC from the Windows client.
 *
 */
#include "StdAfx.h"

#include <windows.h>
#include <windowsx.h>
#include "cstring.h"
#include "TalkW32.h"
#include "resource.h"

String	TalkWindow::ts;

BOOL CALLBACK TalkWindowDlgProc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_INITDIALOG:
	{
		RECT	myRect;
		RECT	parRect;
		GetWindowRect(hwndDlg, &myRect);
		GetWindowRect(GetParent(hwndDlg), &parRect);
		RECT	newRect;
		newRect.left = (parRect.right-parRect.left)/2+parRect.left-((myRect.right-myRect.left)/2);
		newRect.right = (myRect.right-myRect.left)+newRect.left;
		newRect.top = (parRect.bottom-parRect.top)/2+parRect.top-((myRect.bottom-myRect.top)/2);
		newRect.bottom = (myRect.bottom-myRect.top)+newRect.top;
		MoveWindow(hwndDlg, newRect.left, newRect.top,
			newRect.right-newRect.left, newRect.bottom-newRect.top, TRUE);
		return(TRUE);
	}

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
		{
			char szItem[80];
			TalkWindow::ts = "";
			if (GetDlgItemText(hwndDlg, IDC_EDIT1, szItem, 80))
				TalkWindow::ts = szItem;
			EndDialog(hwndDlg, wParam);
			return TRUE;
		}
		case IDCANCEL:
			TalkWindow::ts = "";
			EndDialog(hwndDlg, wParam);
			return TRUE;
		}
	}
    return FALSE;
}

void TalkWindow::Do(HWND parWnd, HINSTANCE hInst)
{
	INT_PTR ret = DialogBox(hInst, MAKEINTRESOURCE(IDD_TALKWINDOW), parWnd, TalkWindowDlgProc);
	if (ret == -1)
	{
		DWORD ret = GetLastError();
	}
}
