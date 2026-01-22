/* $Id: Fl_XPColors.cpp,v 1.4 2004/05/13 09:05:31 dick Exp $
 *
 * XPilot5 - The Cross Platform Control Program for XPilot.
 *      Copyright (C) 2004 by
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
 * $Log: Fl_XPColors.cpp,v $
 * Revision 1.4  2004/05/13 09:05:31  dick
 * New color xpgrey
 *
 * Revision 1.3  2004/02/29 03:05:49  dick
 * Add xpgreen, and add a little macro to make specing Fl_Color(s) easier.
 *
 * Revision 1.2  2004/02/29 00:11:50  dick
 * Add xpLabelColor and xpBackColor for an extra level of abstraction of color
 *
 * Revision 1.1  2004/02/28 07:30:56  dick
 * define some xpilot colors
 *
 */
#include "StdAfx.h"

#include "FL/Enumerations.H"

Fl_Color	xpblue;
Fl_Color	xpred;
Fl_Color	xpblack;
Fl_Color	xpwhite;
Fl_Color	xpgreen;
Fl_Color	xpgrey;

Fl_Color	xpLabelColor;
Fl_Color	xpBackColor;

#define FL_COLOR(_r, _g, _b)					\
	fl_color_cube(_r * (FL_NUM_RED - 1) / 255,	\
				  _g * (FL_NUM_GREEN - 1) / 255,	\
				  _b * (FL_NUM_BLUE - 1) / 255)



void InitFlXPColors()
{
	xpblack = FL_BLACK;
	xpwhite = FL_WHITE;
	xpblue	= FL_COLOR(78,124,255);
	xpred	= FL_COLOR(255,58,39);
	xpgreen = FL_COLOR(51,187,68);
	xpgrey	= FL_COLOR(190,190,190);

	xpBackColor = xpblack;
	xpLabelColor = xpblue;
}
