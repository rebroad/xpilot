/* $Id: Fl_KbKbSelector.h,v 1.1 2001/09/25 15:47:54 dick Exp $
 *
 * Fl_KbConfig - Display a picture of a keyboard with all of the keys defined
 *
 *      Copyright (C) 2001 by
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
 * $Log: Fl_KbKbSelector.h,v $
 * Revision 1.1  2001/09/25 15:47:54  dick
 * A widget to select the current keyboard layout.
 *
 */

#ifndef	_FL_KBKBSELECTOR_H_
#define	_FL_KBKBSELECTOR_H_

#include "FL/Fl_Menu_Button.H"

#define	Fl_KbKbSelectorSUPERCLASS	Fl_Menu_Button
class Fl_KbKbSelector : public Fl_KbKbSelectorSUPERCLASS
{
public:
	Fl_KbKbSelector(int X, int Y, int W, int H, const char* label = 0);

	virtual	int		handle(int event);

};


#endif		// _FL_KBKBSELECTOR_H_
