/*
* XPilotedit, the cross platform map editor for XPilot.  Copyright (C) 2001 by
*
*      The XPilot Authors           <xpilot@xpilot.org>
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
 * $Log: Clipboard.cpp,v $
 * Revision 1.10  2004/02/03 03:39:53  dick
 * Add Windows precompiled header support.
 *
 * Revision 1.9  2004/01/06 04:48:28  dick
 * Add support for building under Cygwin.  Still needs work and the output
 * is X11 and requires something like Cygwin's XWin.exe to run under.
 *
 * Revision 1.8  2002/07/30 19:51:37  jlmiller
 * Rename Class ClipBoard to Clipboard.
 *
 * Revision 1.7  2002/07/30 17:20:55  jlmiller
 * Clipboard becomes its own class.
 *
 * Revision 1.6  2002/06/25 20:20:08  jlmiller
 * Massive restructure to clean up includes, and objectify code more.
 *
 * Revision 1.5  2002/05/28 07:19:51  dick
 * Rename xpilotedit to XPilotControl
 *
 */
#include "StdAfx.h"

#include "Clipboard.h"
#include "map.h"

///////////////////////////////////////////////////////////////////////////////
void Clipboard::EmptyClipData()
{
        int x,y;

        for(x = 0; x < MAX_MAP_SIZE; x++)
                for(y = 0; y < MAX_MAP_SIZE; y++)
                        data[x][y] = MAP_SPACE;
}
///////////////////////////////////////////////////////////////////////////////
void Clipboard::SetData(int x, int y, char ch)
{
	data[x][y] = ch;
}
///////////////////////////////////////////////////////////////////////////////
char Clipboard::GetData(int x, int y)
{
	return data[x][y];
}

Clipboard clipboard;
