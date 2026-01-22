/* $Id: Fl_KbConfigBuiltIn.cpp,v 1.3 2004/02/03 03:39:53 dick Exp $
 *
 * Fl_KbConfigBuiltIn - A Keyboard definition file built into the Fl_KbConfig
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
 * $Log: Fl_KbConfigBuiltIn.cpp,v $
 * Revision 1.3  2004/02/03 03:39:53  dick
 * Add Windows precompiled header support.
 *
 * Revision 1.2  2001/09/27 17:40:10  dick
 * Add the name of the keyboard to the data
 *
 * Revision 1.1  2001/09/20 07:41:02  dick
 * Move Fl_Kb* to it's own library xp-flwidgets/kbconfig/libxp-kbconfig.a
 *
 * Revision 1.2  2001/09/17 23:00:07  dick
 * The X11 keynames for alpha chars (a-z) are lower case
 *
 * Revision 1.1  2001/09/07 07:12:47  dick
 * Move the builtin keyboard layout definition to its own file.
 *
 */
#include "StdAfx.h"

#include "cstring.h"

PCSTR	usGenericKB =
"<NAME string=\"US Generic KB\"/>\n"
"<KEYS>\n"
"Escape			0	1	1\n"
"!				0	1	1\n"
"F1				0	1	1\n"
"F2				0	1	1\n"
"F3				0	1	1\n"
"F4				0	1	1\n"
"!				0	0.5	1\n"
"F5				0	1	1\n"
"F6				0	1	1\n"
"F7				0	1	1\n"
"F8				0	1	1\n"
"!				0	0.5	1\n"
"F9				0	1	1\n"
"F10			0	1	1\n"
"F11			0	1	1\n"
"F12			0	1	1\n"
"grave			2	1	1\n"
"1				2	1	1\n"
"2				2	1	1\n"
"3				2	1	1\n"
"4				2	1	1\n"
"5				2	1	1\n"
"6				2	1	1\n"
"7				2	1	1\n"
"8				2	1	1\n"
"9				2	1	1\n"
"0				2	1	1\n"
"minus			2	1	1\n"
"equal			2	1	1\n"
"backslash		2	1	1\n"
"BackSpace		2	1	1\n"
"Tab			3	1.5	1\n"
"q				3	1	1\n"
"w				3	1	1\n"
"e				3	1	1\n"
"r				3	1	1\n"
"t				3	1	1\n"
"y				3	1	1\n"
"u				3	1	1\n"
"i				3	1	1\n"
"o				3	1	1\n"
"p				3	1	1\n"
"bracketleft	3	1	1\n"
"bracketright	3	1	1\n"
"Caps_Lock		4	1.7	1\n"
"a				4	1	1\n"
"s				4	1	1\n"
"d				4	1	1\n"
"f				4	1	1\n"
"g				4	1	1\n"
"h				4	1	1\n"
"j				4	1	1\n"
"k				4	1	1\n"
"l				4	1	1\n"
"semicolon		4	1	1\n"
"quoteright		4	1	1\n"
"Return			4	2.3	1\n"
"Shift_L		5	2.1	1\n"
"z				5	1	1\n"
"x				5	1	1\n"
"c				5	1	1\n"
"v				5	1	1\n"
"b				5	1	1\n"
"n				5	1	1\n"
"m				5	1	1\n"
"comma			5	1	1\n"
"period			5	1	1\n"
"slash			5	1	1\n"
"Shift_R		5	2.9 1\n"
"Control_L		6	1.4	1\n"
"!				6	2.4	1\n"
"space			6	6	1\n"
"Alt_R			6	1.2	1\n"
"!				6	2.75	1\n"	// Bug! this shouldn't line up (should be 2.8)
"Control_R		6	1.3	1\n"
"!				0	1	1\n"
"Sys_Req		0	1	1\n"
"Scroll_Lock	0	1	1\n"
"Pause			0	1	1\n"
"!				2	1	1\n"
"Insert			2	1	1\n"
"Home			2	1	1\n"
"Page_Up		2	1	1\n"
"!				3	2.5	1\n"
"Delete			3	1	1\n"
"End			3	1	1\n"
"Page_Down		3	1	1\n"
"!				5	2	1\n"
"Up				5	1	1\n"
"!				6	1	1\n"
"Left			6	1	1\n"
"Down			6	1	1\n"
"Right			6	1	1\n"
"!				2	1	1\n"
"Num_Lock		2	1	1\n"
"KP_Divide		2	1	1\n"
"KP_Multiply	2	1	1\n"
"KP_Subtract	2	1	1\n"
"!				3	1	1\n"
"KP_7			3	1	1\n"
"KP_8			3	1	1\n"
"KP_9			3	1	1\n"
"KP_Add			3	1	2\n"
"!				4	5	1\n"
"KP_4			4	1	1\n"
"KP_5			4	1	1\n"
"KP_6			4	1	1\n"
"!				5	2	1\n"
"KP_1			5	1	1\n"
"KP_2			5	1	1\n"
"KP_3			5	1	1\n"
"KP_Enter		5	1	2\n"
"!				6	1	1\n"
"KP_0			6	2	1\n"
"KP_Decimal		6	1	1\n"
;
