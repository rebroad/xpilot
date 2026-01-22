/* $Id: Ini.Win.cpp,v 1.13 2004/05/11 06:45:45 dick Exp $
 *
 * Class to provide the client side resource file handling.
 * Unix this is ~/.xpilotrc and Windows is XPilot.ini
 * This file contains the Windows specific handling.
 *
 * XPilot, a multiplayer gravity war game.	Copyright (C) 1991-2001 by
 *
 *		Bjørn Stabell		 <bjoern@xpilot.org>
 *		Ken Ronny Schouten	 <ken@xpilot.org>
 *		Bert Gijsbers		 <bert@xpilot.org>
 *		Dick Balaska		 <dick@xpilot.org>
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
 * $Log: Ini.Win.cpp,v $
 * Revision 1.13  2004/05/11 06:45:45  dick
 * Handle the wildcard trailing prefix
 *
 * Revision 1.12  2004/05/10 06:26:13  dick
 * Allow an optional prefix to be passed, so that the same Options can fill
 * different objects (inet columns and private columns).
 *
 * Revision 1.11  2004/02/20 05:11:39  dick
 * Windows server saves his window position and state.
 *
 * Revision 1.10  2004/02/03 03:39:52  dick
 * Add Windows precompiled header support.
 *
 * Revision 1.9  2002/09/02 15:45:49  dick
 * Break Ini up into base class Ini and subclass IniClient.
 *
 * Revision 1.8  2002/08/18 17:57:18  dick
 * Save and restore the window position for the client
 *
 * Revision 1.7  2002/08/11 21:08:08  dick
 * Remove MFC from the Windows client.
 *
 * Revision 1.6  2002/08/06 14:48:40  jlmiller
 * Hack Ini to make -team switch work on windows.
 *
 * Revision 1.5  2002/07/19 11:05:55  jlmiller
 * Make name, realname, & hostname String(s)
 *
 * Revision 1.4  2002/05/18 20:55:35  dick
 * Update to XPilot-4.5.1
 *
 * Revision 1.3  2001/09/20 06:10:28  dick
 * Base the ini file path off of the directory we were launched in.
 *
 * Revision 1.2  2001/09/17 08:46:56  dick
 * Move keyDefs into Ini
 *
 * Revision 1.1  2001/09/11 07:33:45  dick
 * Encapsulate all client resources into new class Ini.
 *
 */
#include "StdAfx.h"

#include <direct.h>

#include "error.h"
#include "Ini.h"

void Ini::SaveResource(FILE* fp, PCSTR resource, PCSTR value)
{
	WritePrivateProfileString("Settings",
							  resource,
							  value,
							  fileName);
}

/* save our window's position */
void Ini::SaveWindowPositions(HWND hWnd)
{
	WINDOWPLACEMENT 	wp;
//	  Window w;
	RECT rect;
	char		s[50];
#if 0
	w = WinXGetParent(top);
	WinXGetWindowRect(w, &rect);
	WinXGetWindowPlacement(w, &wp);
#endif
	HWND parWnd = GetParent(hWnd);
	if (!parWnd)
		parWnd = hWnd;
	GetWindowRect(parWnd, &rect);
	GetWindowPlacement(parWnd, &wp);
	if (wp.showCmd != SW_SHOWMINIMIZED)
	{
		itoa(rect.left, s, 10);
		WritePrivateProfileString(s_Settings, s_L,
								  itoa(rect.left, s, 10),
								  fileName);
		WritePrivateProfileString(s_Settings, s_T,
								  itoa(rect.top, s, 10),
								  fileName);
		WritePrivateProfileString(s_Settings, s_R,
								  itoa(rect.right, s, 10),
								  fileName);
		WritePrivateProfileString(s_Settings, s_B,
								  itoa(rect.bottom, s, 10),
								  fileName);
	}
	WritePrivateProfileString(s_Settings, s_windowMode,
							  itoa(windowMode, s, 10),
							  fileName);
}

bool Ini::GetWindowPositions(RECT* rect)
{
	rect->left = GetPrivateProfileInt(s_Settings, s_L, 0, fileName);
	rect->top = GetPrivateProfileInt(s_Settings, s_T, 0, fileName);
	rect->right = GetPrivateProfileInt(s_Settings, s_R, 0, fileName);
	rect->bottom = GetPrivateProfileInt(s_Settings, s_B, 0, fileName);
	windowMode = GetPrivateProfileInt(s_Settings, s_windowMode, 0, fileName);
	if (!rect->left && !rect->right)
		return(false);
	return(true);
}

void Ini::GetProfileString(PCSTR key,	PCSTR def, char* result, int size)
{
	GetPrivateProfileString(s_Settings, key, def, result, size, fileName);
//	if (result[0] != '\0')
//		return;
//	strncpy(result, def, size);
}

void Ini::GetResource(PCSTR resource, char *result, unsigned size, int* index, PCSTR prefix)
{
//	  unsigned			hash = StringHash(resource);
	int 	i;
	String	s;

	for (i = 0;;)
	{
		if (!strcmp(resource, options[i].name))
		{
			*index = i;
			break;
		}
		if (++i >= numOptions)
		{
			error("BUG: Can't find option \"%s\"", resource);
			exit(1);
		}
	}
	s = resource;
	if (s.EndsWith('*'))
	{
		s = s.Left(s.GetLength()-1);
		s += prefix;
	}
	else if (prefix)
	{
		s = prefix;
		s += resource;
	}
	GetProfileString(s, options[i].fallback, result, size);
}
