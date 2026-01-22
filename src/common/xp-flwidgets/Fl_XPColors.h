/* $Id: Fl_XPColors.h,v 1.4 2004/05/13 09:05:31 dick Exp $
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
 * $Log: Fl_XPColors.h,v $
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

enum Fl_Color;

extern	Fl_Color	xpblue;
extern	Fl_Color	xpred;
extern	Fl_Color	xpblack;
extern	Fl_Color	xpwhite;
extern	Fl_Color	xpgreen;
extern	Fl_Color	xpgrey;

extern	Fl_Color	xpLabelColor;
extern	Fl_Color	xpBackColor;


extern void InitFlXPColors();
