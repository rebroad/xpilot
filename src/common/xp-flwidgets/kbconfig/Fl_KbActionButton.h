/* $Id: Fl_KbActionButton.h,v 1.2 2001/09/27 17:44:57 dick Exp $
 *
 * Fl_KbActionButton - One of the Red buttons that perform an action
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
 * $Log: Fl_KbActionButton.h,v $
 * Revision 1.2  2001/09/27 17:44:57  dick
 * Handle callbacks to this button type.
 *
 * Revision 1.1  2001/09/25 15:48:38  dick
 * A widget to handle the "Action" commands, Load/Save/Undo/Redo.
 *
 */

#ifndef	_FL_KBACTIONBUTTON_H_
#define	_FL_KBACTIONBUTTON_H_

#include "FL/Fl_Button.H"

#define	ACTIONBUTTONWIDTH	50
#define	ACTIONBUTTONHEIGHT	30

class Fl_KbConfig;

#define	Fl_KbActionButtonSUPERCLASS	Fl_Button
class Fl_KbActionButton : public Fl_KbActionButtonSUPERCLASS
{
public:
	Fl_KbActionButton(int X, int Y, int W, int H, const char* label = 0);

//	virtual	int		handle(int event);
	static Fl_Callback	myCallback;
	Fl_KbConfig*	myKbConfig;
};


#endif		// _FL_KBACTIONBUTTON_H_
