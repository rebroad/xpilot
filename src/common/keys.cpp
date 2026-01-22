/* $Id: keys.cpp,v 1.10 2004/02/03 03:39:52 dick Exp $
 *
 * XPilot, a multiplayer gravity war game.  Copyright (C) 1991-2001 by
 *
 *      Dick Balaska         <dick@xpilot.org>
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
 */
/*
 * $Log: keys.cpp,v $
 * Revision 1.10  2004/02/03 03:39:52  dick
 * Add Windows precompiled header support.
 *
 * Revision 1.9  2001/10/09 15:44:24  dick
 * Don't InitKeysyms() on special keys beginning with '_'
 *
 * Revision 1.8  2001/10/04 17:33:39  dick
 * Use uint instead of UINT.
 *
 * Revision 1.7  2001/10/04 16:05:09  dick
 * Add constants for "_Unused" and "_Copy" so we can compare the constants
 * rather than strcmp the string.
 * Add some constness to prototypes (this may not go over well on Unix)
 *
 * Revision 1.6  2001/09/27 17:52:20  dick
 * Add dummy key "Copy" (which is a command target for kbconfig)
 *
 * Revision 1.5  2001/09/25 15:31:34  dick
 * Add a special key for the "unhandled" commands.
 *
 * Revision 1.4  2001/09/17 23:02:16  dick
 * InitKeysyms() must be called before using the xpkeydefs table to adjust
 * Unix keysyms.
 *
 * Revision 1.3  2001/09/07 21:23:22  dick
 * Need types.h
 *
 * Revision 1.2  2001/09/07 08:44:23  dick
 * Linux updates
 *
 * Revision 1.1  2001/09/07 06:50:57  dick
 * Move old common/NT/winXKey to common/keys because KbConfig needs these
 * functions.
 *
 */
#include "StdAfx.h"

#include <FL/Enumerations.H>
#include <string.h>
#include <stdio.h>

#ifndef	_WINDOWS
# include <X11/Xos.h>
# include <X11/keysym.h>
# include <X11/Xlib.h>
# include <X11/Xresource.h>
#endif

#include "types.h"
#include "keys.h"

PCSTR	s_unused =	"_Unused";
PCSTR	s_copy =	"_Copy";

XPKeyDef xpkeydef[] = {
/* s              k       c                f */
"BackSpace",	0x000E, "<-",			FL_BackSpace,
"Tab",			0x000F, "->",			FL_Tab,
"Return",		0x001C,	"<_|",			FL_Enter,		/* Return, enter */
"Pause",		0x0045,	"Pause",		FL_Pause,
"Scroll_Lock",	0x0046, "Scroll Lock",	FL_Scroll_Lock,
"Sys_Req",		0xFF15, "SysRq",		0, // no fltk SysRq
"Escape",		0x0001, "Esc",			FL_Escape,
"Delete",		0x0153,	"Delete",		FL_Delete,
"Home",			0x0147, "Home",			FL_Home,
"Left",			0x014B,	"<-",			FL_Left,
"Up",			0x0148,	"^",			FL_Up,
"Right",		0x014D,	"->",			FL_Right,
"Down",			0x0150,	"V",			FL_Down,
"Page_Up",		0x0149, "Page Up",		FL_Page_Up,
"Page_Down",	0x0151, "Page Down",	FL_Page_Down,
"End",			0x014F,	"End",			FL_End,
"Insert",		0x0152,	"Insert",		FL_Insert,
"Num_Lock",		0x0145, "Num Lock",		FL_Num_Lock,
"KP_Enter",		0x011C,	"Enter",		FL_KP_Enter,	// note fltk bugs with this key
"KP_Multiply",	0x0037,	"*",			FL_KP_Multiply,
"KP_Add",		0x004E,	"+",			FL_KP_Plus,
"KP_Subtract",	0x004A,	"-",			FL_KP_Minus,
"KP_Decimal",	0x0053,	".",			FL_KP_Period,
"KP_Divide",	0x0135,	"/",			FL_KP_Slash,
"KP_0",			0x0052,	"0",			FL_KP+'0',
"KP_1",			0x004F,	"1",			FL_KP+'1',
"KP_2",			0x0050,	"2",			FL_KP+'2',
"KP_3",			0x0051,	"3",			FL_KP+'3',
"KP_4",			0x004B,	"4",			FL_KP+'4',
"KP_5",			0x004C,	"5",			FL_KP+'5',
"KP_6",			0x004D,	"6",			FL_KP+'6',
"KP_7",			0x0047,	"7",			FL_KP+'7',
"KP_8",			0x0048,	"8",			FL_KP+'8',
"KP_9",			0x0049,	"9",			FL_KP+'9',
"F1",			0x003B,	"F1",			FL_F+1,
"F2",			0x003C,	"F2",			FL_F+2,
"F3",			0x003D,	"F3",			FL_F+3,
"F4",			0x003E,	"F4",			FL_F+4,
"F5",			0x003F,	"F5",			FL_F+5,
"F6",			0x0040,	"F6",			FL_F+6,
"F7",			0x0041,	"F7",			FL_F+7,
"F8",			0x0042,	"F8",			FL_F+8,
"F9",			0x0043,	"F9",			FL_F+9,
"F10",			0x0044,	"F10",			FL_F+10,
"F11",			0x0057,	"F11",			FL_F+11,
"F12",			0x0058,	"F12",			FL_F+12,
"Shift_L",		0x002A,	"Shift",		FL_Shift_L,		/* Left shift */
"Shift_R",		0x0036,	"Shift",		FL_Shift_R,		/* Right shift */
"Control_L",	0x001D,	"Ctrl",			FL_Control_L,	/* Left control */
"Control_R",	0x011D,	"Ctrl",			FL_Control_R,	/* Right control */
"Alt_L",		0xFFE9, "Alt",			FL_Alt_L,		// NOT SUPPORTED FOR GAME PLAY
"Alt_R",		0xFFEA, "Alt",			FL_Alt_R,		// NOT SUPPORTED FOR GAME PLAY
"Caps_Lock",	0x003A,	"CapsLock",		FL_Caps_Lock,	/* Caps lock */
"space",		0x0039,	"",				' ',
"apostrophe",	0x0028,	"'",			'\'',
"quoteright",	0x0028,	"\"",			'"',
"comma",		0x0033,	",",			',',
"minus",		0x000C,	"-",			'-',
"period",		0x0034,	".",			'.',
"slash",		0x0035,	"/",			'/',
"0",			0x000B,	"0",			'0',
"1",			0x0002,	"1",			'1',
"2",			0x0003,	"2",			'2',
"3",			0x0004,	"3",			'3',
"4",			0x0005,	"4",			'4',
"5",			0x0006,	"5",			'5',
"6",			0x0007,	"6",			'6',
"7",			0x0008,	"7",			'7',
"8",			0x0009,	"8",			'8',
"9",			0x000A,	"9",			'9',
"semicolon",	0x0027,	";",			';',
"equal",		0x000D,	"=",			'=',
"A",			0x001E,	"A",			'a',
"B",			0x0030,	"B",			'b',
"C",			0x002E,	"C",			'c',
"D",			0x0020,	"D",			'd',
"E",			0x0012,	"E",			'e',
"F",			0x0021,	"F",			'f',
"G",			0x0022,	"G",			'g',
"H",			0x0023,	"H",			'h',
"I",			0x0017,	"I",			'i',
"J",			0x0024,	"J",			'j',
"K",			0x0025,	"K",			'k',
"L",			0x0026,	"L",			'l',
"M",			0x0032,	"M",			'm',
"N",			0x0031,	"N",			'n',
"O",			0x0018,	"O",			'o',
"P",			0x0019,	"P",			'p',
"Q",			0x0010,	"Q",			'q',
"R",			0x0013,	"R",			'r',
"S",			0x001F,	"S",			's',
"T",			0x0014,	"T",			't',
"U",			0x0016,	"U",			'u',
"V",			0x002F,	"V",			'v',
"W",			0x0011,	"W",			'w',
"X",			0x002D,	"X",			'x',
"Y",			0x0015,	"Y",			'y',
"Z",			0x002C,	"Z",			'z',
"a",			0x001E,	"A",			'a',
"b",			0x0030,	"B",			'b',
"c",			0x002E,	"C",			'c',
"d",			0x0020,	"D",			'd',
"e",			0x0012,	"E",			'e',
"f",			0x0021,	"F",			'f',
"g",			0x0022,	"G",			'g',
"h",			0x0023,	"H",			'h',
"i",			0x0017,	"I",			'i',
"j",			0x0024,	"J",			'j',
"k",			0x0025,	"K",			'k',
"l",			0x0026,	"L",			'l',
"m",			0x0032,	"M",			'm',
"n",			0x0031,	"N",			'n',
"o",			0x0018,	"O",			'o',
"p",			0x0019,	"P",			'p',
"q",			0x0010,	"Q",			'q',
"r",			0x0013,	"R",			'r',
"s",			0x001F,	"S",			's',
"t",			0x0014,	"T",			't',
"u",			0x0016,	"U",			'u',
"v",			0x002F,	"V",			'v',
"w",			0x0011,	"W",			'w',
"x",			0x002D,	"X",			'x',
"y",			0x0015,	"Y",			'y',
"z",			0x002C,	"Z",			'z',
"bracketleft",	0x001A,	"[",			'[',
"backslash",	0x002B,	"\\",			'\\',
"bracketright",	0x001B,	"]",			']',
"grave",		0x0029,	"`",			'`',
"quoteleft",	0x0029,	"`",			'`',	/* deprecated */

s_unused,		0x0000, "Unused",		'\0',	// special key for unused commands
s_copy,			0x0000, "Copy",			'\0',	// special key for unused commands
0,				0,		0,				0,
};

// Windoze doesn't have these X11 functions
// which we need to map key names
#ifdef	_WINDOWS
KeySym XStringToKeysym(char* s)
{
	XPKeyDef* wks;
	for (wks = &xpkeydef[0]; wks->s; wks++)
	{
		if (!strcmp(s, wks->s))
			return(wks->k);
	}
	for (wks = &xpkeydef[0]; wks->s; wks++)
	{
		if (!stricmp(s, wks->s))
			return(wks->k);
	}
	return(NoSymbol);
}

PCSTR XKeysymToString(KeySym keysym)
{
	XPKeyDef* wks;
	for (wks = &xpkeydef[0]; wks->s; wks++)
	{
		if (keysym == wks->k)
			return(wks->s);
	}
	return(0);
}
#endif

XPKeyDef* FltkToXPKeyDef(uint flkey)
{
	XPKeyDef* wks;
	for (wks = &xpkeydef[0]; wks->s; wks++)
	{
		if (flkey == wks->f)
			return(wks);
	}
	return(0);
}

XPKeyDef* XStringToXPKeyDef(PCSTR s)
{
	XPKeyDef* wks;
	for (wks = &xpkeydef[0]; wks->s; wks++)
	{
		if (!strcmp(s, wks->s))		// first try case sensitive
			return(wks);
	}
	for (wks = &xpkeydef[0]; wks->s; wks++)
	{
		if (!strcasecmp(s, wks->s))
			return(wks);
	}
	return(0);

}

void InitKeysyms()
{
#ifndef	_WINDOWS
	XPKeyDef* wks;
	for (wks = &xpkeydef[0]; wks->s; wks++)
	{
		if (wks->s[0] == '_')
			continue;
		if ((wks->k = XStringToKeysym(wks->s)) == NoSymbol)
		{
			printf("InitKeysyms: Invalid keysym \"%s\".\n", wks->s);
		}
		// printf("String \"%s\" becomes %d\n", wks->s, wks->k);
	}
#endif
}
