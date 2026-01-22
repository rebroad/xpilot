/* $Id: keys.h,v 1.16 2004/05/23 23:52:07 dick Exp $
 *
 * XPilot, a multiplayer gravity war game.  Copyright (C) 1991-2001 by
 *
 *      Bjørn Stabell        <bjoern@xpilot.org>
 *      Ken Ronny Schouten   <ken@xpilot.org>
 *      Bert Gijsbers        <bert@xpilot.org>
 *      Dick Balaska         <dick@xpilot.org>
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
 * $Log: keys.h,v $
 * Revision 1.16  2004/05/23 23:52:07  dick
 * src/common/IniClient.Defaults.cpp
 *
 * Revision 1.15  2004/01/06 04:48:28  dick
 * Add support for building under Cygwin.  Still needs work and the output
 * is X11 and requires something like Cygwin's XWin.exe to run under.
 *
 * Revision 1.14  2002/09/02 15:46:44  dick
 * Get_keyHelpString become GetKeyHelpString
 *
 * Revision 1.13  2002/09/01 00:12:23  dick
 * Comment out unused #ifdef SERVER.
 *
 * Revision 1.12  2002/07/15 06:48:47  dick
 * Don't need to check for KeySym before including...
 *
 * Revision 1.11  2001/10/04 17:33:39  dick
 * Use uint instead of UINT.
 *
 * Revision 1.10  2001/10/04 16:05:09  dick
 * Add constants for "_Unused" and "_Copy" so we can compare the constants
 * rather than strcmp the string.
 * Add some constness to prototypes (this may not go over well on Unix)
 *
 * Revision 1.9  2001/09/27 17:29:36  dick
 * member keysym becomes keySym
 *
 * Revision 1.8  2001/09/17 23:03:02  dick
 * InitKeysyms() must be called before using the xpkeydefs table to adjust
 * Unix keysyms.
 *
 * Revision 1.7  2001/09/17 08:53:28  dick
 * keyDefs becomes part of Ini
 *
 * Revision 1.6  2001/09/07 08:44:23  dick
 * Linux updates
 *
 * Revision 1.5  2001/09/07 06:50:57  dick
 * Move old common/NT/winXKey to common/keys because KbConfig needs these
 * functions.
 *
 * Revision 1.4  2001/08/09 08:24:15  dick
 * Get rid of the Windows "13" version number.
 *
 * Revision 1.3  2001/08/09 07:56:41  dick
 * Linux cleanups
 *
 */

#ifndef KEYS_H
#define KEYS_H

#if defined(_WINDOWS) && !defined(_CYGWIN)
#include "NT/winX.h"
#else
#include <X11/X.h>
#endif

#include "types.h"
#include "cstring.h"
/*
 * The following enum type defines the possible actions as a result of
 * a keypress or keyrelease.
 *
 * NB, if you add your own keys then please use the last ones first,
 * so that they don't clash with our future keys.
 */
enum keys_t {
	KEY_DUMMY,					/* 0 */
	KEY_LOCK_NEXT,
	KEY_LOCK_PREV,
	KEY_LOCK_CLOSE,
	KEY_CHANGE_HOME,
	KEY_SHIELD, 				/* 5 */
	KEY_FIRE_SHOT,
	KEY_FIRE_MISSILE,
	KEY_FIRE_TORPEDO,
	KEY_TOGGLE_NUCLEAR,			/* Was KEY_FIRE_NUKE */
	KEY_FIRE_HEAT,				/* 10 */
	KEY_DROP_MINE,
	KEY_DETACH_MINE,
	KEY_TURN_LEFT,
	KEY_TURN_RIGHT,
	KEY_SELF_DESTRUCT,
	KEY_LOSE_ITEM,				/* Was KEY_ID_MODE up to 3.2.5 */
	KEY_PAUSE,
	KEY_TANK_DETACH,
	KEY_TANK_NEXT,
	KEY_TANK_PREV,				/* 20 */
	KEY_TOGGLE_VELOCITY,
	KEY_TOGGLE_CLUSTER, 		/* Was KEY_TOGGLE_COMPASS */
	KEY_SWAP_SETTINGS,
	KEY_REFUEL,
	KEY_CONNECTOR,
	KEY_INCREASE_POWER,
	KEY_DECREASE_POWER,
	KEY_INCREASE_TURNSPEED,
	KEY_DECREASE_TURNSPEED,
	KEY_THRUST, 				/* 30 */
	KEY_CLOAK,
	KEY_ECM,
	KEY_DROP_BALL,
	KEY_TRANSPORTER,
	KEY_TALK,
	KEY_FIRE_LASER,
	KEY_LOCK_NEXT_CLOSE,
	KEY_TOGGLE_COMPASS,
	KEY_TOGGLE_MINI,
	KEY_TOGGLE_SPREAD,			/* 40 */
	KEY_TOGGLE_POWER,
	KEY_TOGGLE_AUTOPILOT,
	KEY_TOGGLE_LASER,
	KEY_EMERGENCY_THRUST,
	KEY_TRACTOR_BEAM,
	KEY_PRESSOR_BEAM,
	KEY_CLEAR_MODIFIERS,
	KEY_LOAD_MODIFIERS_1,
	KEY_LOAD_MODIFIERS_2,
	KEY_LOAD_MODIFIERS_3,		/* 50 */
	KEY_LOAD_MODIFIERS_4,
	KEY_SELECT_ITEM,			/* Was KEY_TOGGLE_OWNED_ITEMS up to 3.2.5 */
	KEY_PHASING,				/* Was KEY_TOGGLE_MESSAGES up to 3.2.5 */
	KEY_REPAIR,
	KEY_TOGGLE_IMPLOSION,
	KEY_REPROGRAM,
	KEY_LOAD_LOCK_1,
	KEY_LOAD_LOCK_2,
	KEY_LOAD_LOCK_3,
	KEY_LOAD_LOCK_4,			/* 60 */
	KEY_EMERGENCY_SHIELD,
	KEY_HYPERJUMP,				/* Was KEY_POINTER_CONTROL up to 3.2.5 */
	KEY_DETONATE_MINES,
	KEY_DEFLECTOR,				/* since 3.8.0 */
	KEY_UNUSED_65,
	KEY_UNUSED_66,
	KEY_UNUSED_67,
	KEY_UNUSED_68,
	KEY_UNUSED_69,
	KEY_UNUSED_70,				/* 70 */
	KEY_UNUSED_71,
	NUM_KEYS,					/* The number of different keys_t */
	/*
	 * Hack (patent pending BG):
	 * Here all keys only used by the client can be defined.
	 * Be careful that the key vector is not set with these keys or
	 * array boundaries will be exceeded.
	 * The reason for this hack is to create new empty key slots while
	 * retaining compatibility.  Change this at the next major cleanup.
	 */
	KEY_MSG_1,					/* talk macros */
	KEY_MSG_2,
	KEY_MSG_3,
	KEY_MSG_4,
	KEY_MSG_5,
	KEY_MSG_6,
	KEY_MSG_7,
	KEY_MSG_8,
	KEY_MSG_9,
	KEY_MSG_10,
	KEY_MSG_11,
	KEY_MSG_12,
	KEY_MSG_13,
	KEY_MSG_14,
	KEY_MSG_15,
	KEY_MSG_16,
	KEY_MSG_17,
	KEY_MSG_18,
	KEY_MSG_19,
	KEY_MSG_20,

	KEY_ID_MODE,
	KEY_TOGGLE_OWNED_ITEMS,
	KEY_TOGGLE_MESSAGES,
	KEY_POINTER_CONTROL,
	KEY_TOGGLE_RECORD,
	KEY_PRINT_MSGS_STDOUT,
	KEY_TALK_CURSOR_LEFT,
	KEY_TALK_CURSOR_RIGHT,
	KEY_TALK_CURSOR_UP,
	KEY_TALK_CURSOR_DOWN,
	KEY_SWAP_SCALEFACTOR,
	KEY_SHOW_STDOUT,			// Windows only
	NUM_CLIENT_KEYS				/* The number of keys really used by the client. */
};


class KeyDef {
public:
	KeySym	keySym; 		/* Keysym-to-action array */
	keys_t	key;
};

extern char* GetKeyHelpString(keys_t key);
extern const char *GetKeyResourceString(keys_t key);


extern	PCSTR	s_unused;
extern	PCSTR	s_copy;

// A key definition.
// Stores all the possible representations of a key
struct XPKeyDef {
	PCSTR	s;		// X11 Keyname
	KeySym	k;		// X11 Key ID
	PCSTR	c;		// Name (to print on KbConfig)
	uint	f;		// FLTK Key ID
};

// missing FLTK key definitions
#define FL_KP_Slash 	0xFFAF
#define FL_KP_Multiply	0xFFAA
#define FL_KP_Minus 	0xFFAD
#define FL_KP_Plus		0xFFAB
#define FL_KP_Period	0xFFAE

#if defined(_WINDOWS) && !defined(_CYGWIN)
extern	KeySym		XStringToKeysym(char* s);
extern	PCSTR		XKeysymToString(KeySym keysym);
#endif

extern	XPKeyDef*	FltkToXPKeyDef(uint flkey);
extern	XPKeyDef*	XStringToXPKeyDef(PCSTR s);

extern	void	InitKeysyms();		// before using the xpkeydef table, you must call this
#endif
