/* $Id: Fl_Status.h,v 1.2 2007/02/17 20:27:42 dick Exp $
 *
 * Fl_Status - a one line status bar with a backscroll buffer
 *
 *      Copyright (C) 2001 by
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
 * $Log: Fl_Status.h,v $
 * Revision 1.2  2007/02/17 20:27:42  dick
 * Hack that darn index macro bug
 *
 * Revision 1.1  2001/08/26 09:32:54  dick
 * The status line becomes his own widget which has a backscroll buffer.
 *
 */

#ifndef	_FL_STATUS_H_
#define	_FL_STATUS_H_

#include "FL/Fl_Group.H"

// On Linux, gcc 2.95.3 (glibc 2.2.3), somewhere, string.h defines index as a macro:
// #define index(s,c) (strchr((s),(c)))
// why this confuses Fl_Input from only this module i don't know
#undef	index
#include "FL/Fl_Output.H"

//#include "FL/Fl_Scrollbar.h"
#include "FL/Fl_Repeat_Button.H"

#include "cstring.h"

#define	BACKSCROLL	100		// number of lines we store

#define	Fl_StatusSUPERCLASS	Fl_Group
class Fl_Status : public Fl_StatusSUPERCLASS
{
public:
	Fl_Status(int x, int y, int w, int h, PCSTR label = 0);

	void		value(PCSTR s);

	static	void UpHandler(Fl_Widget*, void* myThis);
			void UpHandler();
	static	void DownHandler(Fl_Widget*, void* myThis);
			void DownHandler();

private:
	Fl_Output*		text;
	Fl_Repeat_Button*	up;
	Fl_Repeat_Button*	down;

	String	backScroll[BACKSCROLL];
	int		current;
	int		last;
};


#endif	// _FL_STATUS_H_
