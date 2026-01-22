/*
 * Xpilotedit, the cross platform map editor for Xpilot.  Copyright (C) 2001 by
 *
 *      The Xpilot Authors           <xpilot@xpilot.org>
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
 * $Log: Clipboard.h,v $
 * Revision 1.4  2004/01/06 04:48:28  dick
 * Add support for building under Cygwin.  Still needs work and the output
 * is X11 and requires something like Cygwin's XWin.exe to run under.
 *
 */
#ifndef	_CLIPBOARD_H_
#define	_CLIPBOARD_H_

#include "xpcTypes.h"

class Clipboard
{
private:
	map_data_t data;

public:
	int cWidth;
	int cHeight;
	bool fClipboarded;

	void EmptyClipData();
	void SetData(int x, int y, char ch);
	char GetData(int x, int y);
};

extern Clipboard clipboard;

#endif// _CLIPBOARD_H_
