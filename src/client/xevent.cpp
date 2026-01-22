/* $Id: xevent.cpp,v 1.25 2004/06/03 06:14:27 dick Exp $
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
 * $Log: xevent.cpp,v $
 * Revision 1.25  2004/06/03 06:14:27  dick
 * winAudio.* is gone.
 *
 * Revision 1.24  2004/05/23 23:52:06  dick
 * src/common/IniClient.Defaults.cpp
 *
 * Revision 1.23  2004/05/20 22:23:42  dick
 * Add AddMessage(PCSTR, ...).  Wrap some debug with a D().
 *
 * Revision 1.22  2004/05/11 07:14:11  dick
 * Wrap some debug with a D()
 *
 * Revision 1.21  2004/05/01 20:36:52  dick
 * audioEvents no longer needed with OpenAL
 *
 * Revision 1.20  2004/04/27 01:54:13  dick
 * Display urls in the message list as underlined.
 * Open a browser if the url is clicked on.
 *
 * Revision 1.19  2004/04/26 07:02:00  dick
 * Loop through the available score table types
 *
 * Revision 1.18  2004/04/25 05:51:11  dick
 * Retab
 *
 * Revision 1.17  2004/02/03 03:39:52  dick
 * Add Windows precompiled header support.
 *
 * Revision 1.16  2004/01/20 15:14:27  dick
 * Get rid of #define WINDOWSCALING.  Window scaling is always built, it's not an option.
 *
 * Revision 1.15  2004/01/06 04:48:27  dick
 * Add support for building under Cygwin.  Still needs work and the output
 * is X11 and requires something like Cygwin's XWin.exe to run under.
 *
 * Revision 1.14  2002/09/02 21:06:06  dick
 * Encapsulate dpy and kdpy.  Singleton ini becomes iniClient.
 *
 * Revision 1.13  2002/09/02 15:42:40  dick
 * Singleton ini becomes iniClient.
 *
 * Revision 1.12  2002/08/11 21:08:08  dick
 * Remove MFC from the Windows client.
 *
 * Revision 1.11  2002/07/15 21:54:36  bert
 * these files all had file corruptions at several places.
 * mostly two adjacent bytes were replaced with bogus random data.
 * fixed.
 *
 * Revision 1.10  2002/06/26 06:47:03  dick
 * Replace errno with xperrno and errno=0 with seterrno(0).
 *
 * Revision 1.9  2002/05/18 20:55:33  dick
 * Update to XPilot-4.5.1
 *
 * Revision 1.8  2001/09/27 17:30:17  dick
 * member keysym becomes keySym
 *
 * Revision 1.7  2001/09/17 08:32:50  dick
 * Items has moved to common
 *
 * Revision 1.6  2001/09/11 09:08:30  dick
 * Encapsulate all client resources into new class Ini (Linux cleanups)
 *
 * Revision 1.5  2001/09/11 07:33:44  dick
 * Encapsulate all client resources into new class Ini.
 *
 * Revision 1.4  2001/08/14 21:42:53  dick
 * Comment out old debug.
 *
 * Revision 1.3  2001/07/27 05:47:39  dick
 * Client builds and runs in C++
 */
#include "StdAfx.h"

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <math.h>

#if defined(_UNIX) || defined(_CYGWIN)
# include <X11/Xlib.h>
# include <X11/Xos.h>
# include <X11/Xutil.h>
# include <X11/keysym.h>
# include <X11/Xatom.h>
# include <X11/Xmd.h>
# ifdef	__apollo
#  include <X11/ap_keysym.h>
# endif
#else
# include "NT/winX.h"
# include "NT/winClient.h"
# include "NT/winXKey.h"
#endif

#include "version.h"
#include "config.h"
#include "const.h"
#include "ClientWorld.h"
#include "paint.h"
#include "xinit.h"
#include "keys.h"
#include "packet.h"
#include "bit.h"
#include "setup.h"
#include "netclient.h"
#include "widget.h"
#include "error.h"
#include "record.h"
#include "portability.h"
#include "paintdata.h"
#include "talk.h"
#include "configure.h"
#include "xeventhandlers.h"
#include "xevent.h"
#include "Ini.h"
#include "ScoreTable.h"

char xevent_version[] = VERSION;

//extern char *talk_fast_msgs[];	/* talk macros */

extern setup_t		*Setup;

static BITV_DECL(keyv, NUM_KEYS);

bool		initialPointerControl = false;
bool		pointerControl = false;
extern Cursor	pointerControlCursor;

#if defined(JOYSTICK) && defined(__linux__)
/*
 * Joystick support for Linux 1.0 by Eckard Kopatzki (eko@isar.muc.de).
 * Needs joystick-0.7 by Art Smith, Jeff Tranter, Carlos Puchol.
 * Which in turn requires Linux 1.0 or higher.
 */
#include <linux/joystick.h>

#define JS_DEVICE	"/dev/js0"

/*
 * center position of the joystick in X and Y, resp.
 * thresholds which lead to the emulation of the key action
 */
#define JS_X0	630
#define JS_Y0	630
#define JS_DX	100
#define JS_DY	100

/*
 * Functions which are bound to the joystick actions.
 * These should be specified as defined in default.c.
 */
#define JS_LEFT         KEY_TURN_LEFT
#define JS_RIGHT        KEY_TURN_RIGHT
#define JS_UP           KEY_THRUST
#define JS_DOWN         KEY_SWAP_SETTINGS
#define JS_BUTTON0      KEY_FIRE_SHOT
#define JS_BUTTON1      KEY_SHIELD

///////////////////////////////////////////////////////////////////////////////
static int Key_set(int key, int onoff)
{
	if (onoff) {
		if (!BITV_ISSET(keyv, key)) {
			BITV_SET(keyv, key);
			return true;
		}
	} else {
		if (BITV_ISSET(keyv, key)) {
			BITV_CLR(keyv, key);
			return true;
		}
	}
	return false;
}

///////////////////////////////////////////////////////////////////////////////
static void Joystick_event(void)
{
	static int			js_fd = 0;
	static bool 		js_avail = false;
	struct JS_DATA_TYPE js;
	int 				change = 0;

	if (!draw) {
		return;
	}
	if (!js_fd && !js_avail) {
		if ((js_fd = open(JS_DEVICE, O_RDONLY)) == -1) {
			return;
		}
		js_avail = true;
	}
	if (js_avail && read(js_fd, &js, JS_RETURN) == JS_RETURN) {
		change |= Key_set(JS_BUTTON0, (js.buttons & 1));
		change |= Key_set(JS_BUTTON1, (js.buttons & 2));
		change |= Key_set(JS_LEFT,	  (js.x < JS_X0 - JS_DX));
		change |= Key_set(JS_RIGHT,   (js.x > JS_X0 + JS_DX));
		change |= Key_set(JS_UP,	  (js.y < JS_Y0 - JS_DY));
		change |= Key_set(JS_DOWN,	  (js.y > JS_Y0 + JS_DY));
		if (change) {
			Net_key_change();
		}
	}
}
#endif


///////////////////////////////////////////////////////////////////////////////
keys_t Lookup_key(XEvent *event, KeySym ks, bool reset)
{
	keys_t ret = KEY_DUMMY;
	static int i = 0;

	if (reset) {
		/* binary search since keyDefs is sorted on keysym. */
		int lo = 0, hi = iniClient.maxKeyDefs - 1;
		while (lo < hi) {
			i = (lo + hi) >> 1;
			if (ks > iniClient.keyDefs[i].keySym) {
				lo = i + 1;
			} else {
				hi = i;
			}
		}
		if (lo == hi && ks == iniClient.keyDefs[lo].keySym) {
			while (lo > 0 && ks == iniClient.keyDefs[lo - 1].keySym) {
				lo--;
			}
			i = lo;
			ret = iniClient.keyDefs[i].key;
			i++;
		}
	}
	else {
		if (i < iniClient.maxKeyDefs && ks == iniClient.keyDefs[i].keySym) {
			ret = iniClient.keyDefs[i].key;
			i++;
		}
	}

	// IFWINDOWS( Trace("Lookup_key: got key ks=%04X ret=%d\n", ks, ret); )

#ifdef DEVELOPMENT
	if (reset && ret == KEY_DUMMY) {
		static XComposeStatus	compose;
		char					str[4];
		int 					count;

		memset(str, 0, sizeof str);
		count = XLookupString(&event->xkey, str, 1, &ks, &compose);
		if (count == NoSymbol) {
			printf("Unknown keysym: 0x%03lx", ks);
		}
		else {
			printf("No action bound to keysym 0x%03lx", ks);
			if (*str) {
				printf(", which is key \"%s\"", str);
			}
		}
		printf("\n");
	}
#endif

	return (ret);
}

///////////////////////////////////////////////////////////////////////////////
void Pointer_control_set_state(int onoff)
{
	if (onoff) {
		pointerControl = true;
		XGrabPointer(iniClient.dpy, draw, true, 0, GrabModeAsync,
					 GrabModeAsync, draw, pointerControlCursor, CurrentTime);
		XWarpPointer(iniClient.dpy, None, draw,
					 0, 0, 0, 0,
					 draw_width/2, draw_height/2);
		XDefineCursor(iniClient.dpy, draw, pointerControlCursor);
		XSelectInput(iniClient.dpy, draw,
					 PointerMotionMask | ButtonPressMask | ButtonReleaseMask);
	} else {
		pointerControl = false;
		XUngrabPointer(iniClient.dpy, CurrentTime);
		XDefineCursor(iniClient.dpy, draw, None);
		if (!iniClient.selectionAndHistory)
			XSelectInput(iniClient.dpy, draw, ButtonPressMask | ButtonReleaseMask);
		else
			XSelectInput(iniClient.dpy, draw, ButtonPressMask | ButtonReleaseMask);
	}
	XFlush(iniClient.dpy);
}

///////////////////////////////////////////////////////////////////////////////
#if defined(_UNIX) || defined(_CYGWIN)
static void Talk_set_state(bool onoff)
{

	if (onoff) {
		/* Enable talking, disable pointer control if it is enabled. */
		if (pointerControl) {
			initialPointerControl = true;
			Pointer_control_set_state(false);
		}
		if (iniClient.selectionAndHistory) {
			XSelectInput(iniClient.dpy, draw, PointerMotionMask | ButtonPressMask
									| ButtonReleaseMask);
		}
		Talk_map_window(true);
	}
	else {
		/* Disable talking, enable pointer control if it was enabled. */
		Talk_map_window(false);
		if (initialPointerControl) {
			initialPointerControl = false;
			Pointer_control_set_state(true);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
#else
static void Talk_set_state(bool onoff)
{
		char* wintalkstr;

	if (pointerControl) {
		initialPointerControl = true;
		Pointer_control_set_state(false);
	}
		wintalkstr = (char*)DoTalkWindow();
		if (*wintalkstr)
			Net_talk(wintalkstr);

	if (initialPointerControl) {
		initialPointerControl = false;
		Pointer_control_set_state(true);
	}
		scoresChanged = 1;
}
#endif


///////////////////////////////////////////////////////////////////////////////
int Key_init(void)
{
	if (sizeof(keyv) != KEYBOARD_SIZE) {
		error ("%s, %d: keyv size %d, KEYBOARD_SIZE is %d",
			   __FILE__, __LINE__,
			   sizeof(keyv), KEYBOARD_SIZE);
		exit(1);
	}
	memset(keyv, 0, sizeof keyv);
	BITV_SET(keyv, KEY_SHIELD);

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
int Key_update(void)
{
	return Send_keyboard(keyv);
}

///////////////////////////////////////////////////////////////////////////////
bool Key_check_talk_macro(keys_t key)
{
	if (key >= KEY_MSG_1 && key < KEY_MSG_1 + TALK_FAST_NR_OF_MSGS) {
	/* talk macros */
		Talk_macro(iniClient.talk_fast_msgs[key - KEY_MSG_1]);
	}
	return true;
}

///////////////////////////////////////////////////////////////////////////////
bool Key_press_id_mode(keys_t key)
{
	while (1)
	{
		scoreTableType = (ScoreTableType)(scoreTableType + 1);
		if (scoreTableType == STMAX)
			scoreTableType = STFirst;
		if (1<<scoreTableType & scoreTablePages)
			break;
	}
	D(xpprintf("scoreTableType=%d\n", scoreTableType);)
	scoresChanged++;
	return false;		/* server doesn't need to know */
}

///////////////////////////////////////////////////////////////////////////////
bool Key_press_autoshield_hack(keys_t key)
{
	if (iniClient.autoShield && BITV_ISSET(keyv, KEY_SHIELD)) {
		BITV_CLR(keyv, KEY_SHIELD);
	}
	return false;
}

///////////////////////////////////////////////////////////////////////////////
bool Key_press_shield(keys_t key)
{
	if (iniClient.toggleShield) {
		shields = !shields;
		if (shields) {
			BITV_SET(keyv, key);
		} else {
			BITV_CLR(keyv, key);
		}
		return true;
	}
	else if (iniClient.autoShield) {
		shields = 1;
#if 0
		shields = 0;
		BITV_CLR(keyv, key);
		return true;
#endif
	}
	return false;
}

///////////////////////////////////////////////////////////////////////////////
bool Key_press_fuel(keys_t key)
{
	fuelCount = FUEL_NOTIFY;
	return false;
}

///////////////////////////////////////////////////////////////////////////////
bool Key_press_swap_settings(keys_t key)
{
	DFLOAT _tmp;
#define SWAP(a, b) (_tmp = (a), (a) = (b), (b) = _tmp)
		
	SWAP(iniClient.power, iniClient.power_s);
	SWAP(iniClient.turnspeed, iniClient.turnspeed_s);
	SWAP(iniClient.turnresistance, iniClient.turnresistance_s);
	control_count = CONTROL_DELAY;
	Config_redraw();

	return true;
}

///////////////////////////////////////////////////////////////////////////////
bool Key_press_swap_scalefactor(keys_t key)
{
	DFLOAT tmp;
	tmp = iniClient.scaleFactor;
	iniClient.scaleFactor = iniClient.scaleFactor_s;
	iniClient.scaleFactor_s = tmp;

	Init_scale_array();
	Scale_dashes();
	Config_redraw();

	return false;
}

///////////////////////////////////////////////////////////////////////////////
bool Key_press_increase_power(keys_t key)
{
	iniClient.power = iniClient.power * 1.10;
	iniClient.power = MIN(iniClient.power, MAX_PLAYER_POWER);
	Send_power(iniClient.power);

	Config_redraw();
	control_count = CONTROL_DELAY;
	return false;		/* server doesn't see these keypresses anymore */

}

///////////////////////////////////////////////////////////////////////////////
bool Key_press_decrease_power(keys_t key)
{
	iniClient.power = iniClient.power * 0.90;
	iniClient.power = MAX(iniClient.power, MIN_PLAYER_POWER);
	Send_power(iniClient.power);

	Config_redraw();
	control_count = CONTROL_DELAY;
	return false;		/* server doesn't see these keypresses anymore */
}

///////////////////////////////////////////////////////////////////////////////
bool Key_press_increase_turnspeed(keys_t key)
{
	iniClient.turnspeed = iniClient.turnspeed * 1.05;
	iniClient.turnspeed = MIN(iniClient.turnspeed, MAX_PLAYER_TURNSPEED);
	Send_turnspeed(iniClient.turnspeed);

	Config_redraw();
	control_count = CONTROL_DELAY;
	return false;		/* server doesn't see these keypresses anymore */
}

///////////////////////////////////////////////////////////////////////////////
bool Key_press_decrease_turnspeed(keys_t key)
{
	iniClient.turnspeed = iniClient.turnspeed * 0.95;
	iniClient.turnspeed = MAX(iniClient.turnspeed, MIN_PLAYER_TURNSPEED);
	Send_turnspeed(iniClient.turnspeed);

	Config_redraw();
	control_count = CONTROL_DELAY;
	return false;		/* server doesn't see these keypresses anymore */
}

///////////////////////////////////////////////////////////////////////////////
bool Key_press_talk(keys_t key)
{
	Talk_set_state((talk_mapped == false) ? true : false);
	return false;		/* server doesn't need to know */
}

///////////////////////////////////////////////////////////////////////////////
bool Key_press_show_items(keys_t key)
{
	TOGGLE_BIT(iniClient.instruments, SHOW_ITEMS);
	return false;		/* server doesn't need to know */
}

///////////////////////////////////////////////////////////////////////////////
bool Key_press_show_messages(keys_t key)
{
	TOGGLE_BIT(iniClient.instruments, SHOW_MESSAGES);
	return false;		/* server doesn't need to know */
}

///////////////////////////////////////////////////////////////////////////////
bool Key_press_pointer_control(keys_t key)
{
	if (version < 0x3202) {
		error("Cannot use pointer control below version 3.2.3");
	} else	{
		Pointer_control_set_state(!pointerControl);
	}
	return false;		/* server doesn't need to know */
}

///////////////////////////////////////////////////////////////////////////////
bool Key_press_toggle_record(keys_t key)
{
	Record_toggle();
	return false;		/* server doesn't need to know */
}

///////////////////////////////////////////////////////////////////////////////
#if defined(_UNIX) || defined(_CYGWIN)
bool Key_press_msgs_stdout(keys_t key)
{
	if (iniClient.selectionAndHistory)
		Print_messages_to_stdout();
	return false;		/* server doesn't need to know */
}
#endif

///////////////////////////////////////////////////////////////////////////////
bool KeyPressShowStdout(keys_t)
{
#ifdef	_WINDOWS
	extern void ToggleShowStdoutW32();
	ToggleShowStdoutW32();
#endif
	return(false);
}

///////////////////////////////////////////////////////////////////////////////
bool Key_press_select_lose_item(keys_t key)
{
	if (version < 0x3400) {
		static int before;
		if (!before++) {
			seterrno(0);
			error("Servers less than 3.4.0 dont know how to drop items");
		}
		return false;
	}
	if (lose_item_active == 1) {
		lose_item_active = 2;
	} else {
		lose_item_active = 1;
	}
	return true;
}

///////////////////////////////////////////////////////////////////////////////
bool Key_press(keys_t key)
{
	Key_check_talk_macro(key);

	switch (key) {
	case KEY_ID_MODE:
		return (Key_press_id_mode(key));

	case KEY_FIRE_SHOT:
	case KEY_FIRE_LASER:
	case KEY_FIRE_MISSILE:
	case KEY_FIRE_TORPEDO:
	case KEY_FIRE_HEAT:
	case KEY_DROP_MINE:
	case KEY_DETACH_MINE:
		Key_press_autoshield_hack(key);    
		break;

	case KEY_SHIELD:
		if (Key_press_shield(key))
			return true; 
		break;

	case KEY_REFUEL:
	case KEY_REPAIR:
	case KEY_TANK_NEXT:
	case KEY_TANK_PREV:
		Key_press_fuel(key);
		break;

	case KEY_SWAP_SETTINGS:
		if (!Key_press_swap_settings(key))
			return false;
		break;

	case KEY_SWAP_SCALEFACTOR:
		if (!Key_press_swap_scalefactor(key))
			return false;
		break;

	case KEY_SHOW_STDOUT:
		return (KeyPressShowStdout(key));

	case KEY_INCREASE_POWER:
		return Key_press_increase_power(key);

	case KEY_DECREASE_POWER:
		return Key_press_decrease_power(key);

	case KEY_INCREASE_TURNSPEED:
		return Key_press_increase_turnspeed(key);

	case KEY_DECREASE_TURNSPEED:
		return Key_press_decrease_turnspeed(key);

	case KEY_TALK:
		return Key_press_talk(key);

	case KEY_TOGGLE_OWNED_ITEMS:
		return Key_press_show_items(key);

	case KEY_TOGGLE_MESSAGES:
		return Key_press_show_messages(key);

	case KEY_POINTER_CONTROL:
		return Key_press_pointer_control(key);

	case KEY_TOGGLE_RECORD:
		return Key_press_toggle_record(key);
#if defined(_UNIX) || defined(_CYGWIN)
	case KEY_PRINT_MSGS_STDOUT:
		return Key_press_msgs_stdout(key);
#endif
	case KEY_SELECT_ITEM:
	case KEY_LOSE_ITEM:
		if (!Key_press_select_lose_item(key)) 
			return false;
	default:
		break;
	}

	if (key < NUM_KEYS) {
		BITV_SET(keyv, key);
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////
bool Key_release(keys_t key)
{
	switch (key) {
	case KEY_ID_MODE:
	case KEY_TALK:
	case KEY_TOGGLE_OWNED_ITEMS:
	case KEY_TOGGLE_MESSAGES:
		return false;	/* server doesn't need to know */

	/* Don auto-shield hack */
	/* restore shields */
	case KEY_FIRE_SHOT:
	case KEY_FIRE_LASER:
	case KEY_FIRE_MISSILE:
	case KEY_FIRE_TORPEDO:
	case KEY_FIRE_HEAT:
	case KEY_DROP_MINE:
	case KEY_DETACH_MINE:
		if (iniClient.autoShield && shields && !BITV_ISSET(keyv, KEY_SHIELD)) {
			/* Here We need to know if any other weapons are still on */
			/*		before we turn shield back on	*/
			BITV_CLR(keyv, key);
			if (!BITV_ISSET(keyv, KEY_FIRE_SHOT) &&
				!BITV_ISSET(keyv, KEY_FIRE_LASER) &&
				!BITV_ISSET(keyv, KEY_FIRE_MISSILE) &&
				!BITV_ISSET(keyv, KEY_FIRE_TORPEDO) &&
				!BITV_ISSET(keyv, KEY_FIRE_HEAT) &&
				!BITV_ISSET(keyv, KEY_DROP_MINE) &&
				!BITV_ISSET(keyv, KEY_DETACH_MINE)
			) {
				BITV_SET(keyv, KEY_SHIELD);
			}
		}
		break;

	case KEY_SHIELD:
		if (iniClient.toggleShield) {
			return false;
		}
		else if (iniClient.autoShield) {
			shields = 0;
#if 0
			shields = 1;
			BITV_SET(keyv, key);
			return true;
#endif
		}
		break;

	case KEY_REFUEL:
	case KEY_REPAIR:
		fuelCount = FUEL_NOTIFY;
		break;

	case KEY_SELECT_ITEM:
	case KEY_LOSE_ITEM:
		if (version < 0x3400) {
			return false;
		}
		if (lose_item_active == 2) {
			lose_item_active = 1;
		} else {
			lose_item_active = -FPS;
		}
		break;

	default:
		break;
	}
	if (key < NUM_KEYS) {
		BITV_CLR(keyv, key);
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////
void Key_event(XEvent *event)
{
	KeySym				ks;
	keys_t				key;
	bool				change = false;
	bool				(*key_do)(keys_t key);

	switch(event->type) {
	case KeyPress:
		key_do = Key_press;
		break;
	case KeyRelease:
		key_do = Key_release;
		break;
	default:
		return;
	}

	if ((ks = XLookupKeysym(&event->xkey, 0)) == NoSymbol) {
		return;
	}

	for (key = Lookup_key(event, ks, true);
		 key != KEY_DUMMY;
		 key = Lookup_key(event, ks, false)) {

		change |= (*key_do)(key);
	}
	if (change) {
		Net_key_change();
	}
}

///////////////////////////////////////////////////////////////////////////////
void Reset_shields(void)
{
	if (iniClient.toggleShield || iniClient.autoShield) {
		BITV_SET(keyv, KEY_SHIELD);
		shields = 1;
		if (iniClient.autoShield) {
			if (BITV_ISSET(keyv, KEY_FIRE_SHOT) ||
				BITV_ISSET(keyv, KEY_FIRE_LASER) ||
				BITV_ISSET(keyv, KEY_FIRE_MISSILE) ||
				BITV_ISSET(keyv, KEY_FIRE_TORPEDO) ||
				BITV_ISSET(keyv, KEY_FIRE_HEAT) ||
				BITV_ISSET(keyv, KEY_DROP_MINE) ||
				BITV_ISSET(keyv, KEY_DETACH_MINE)) {
				BITV_CLR(keyv, KEY_SHIELD);
			}
		}
		Net_key_change();
	}
}

///////////////////////////////////////////////////////////////////////////////
void Set_auto_shield(bool onoff)
{
	iniClient.autoShield = onoff;
}

///////////////////////////////////////////////////////////////////////////////
void Set_toggle_shield(bool onoff)
{
	iniClient.toggleShield = onoff;
	if (iniClient.toggleShield) {
		if (iniClient.autoShield) {
			shields = 1;
		}
		else {
			shields = (BITV_ISSET(keyv, KEY_SHIELD) != 0);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
void Talk_event(XEvent *event)
{
	if (!Talk_do_event(event)) {
		Talk_set_state(false);
	}
}

///////////////////////////////////////////////////////////////////////////////
int 	talk_key_repeat_count;
XEvent	talk_key_repeat_event;

///////////////////////////////////////////////////////////////////////////////
void xevent_keyboard(int queued)
{
#if defined(_UNIX) || defined(_CYGWIN)
	int 				i, n;
	XEvent				event;
#endif

	if (talk_key_repeat_count > 0) {
		if (++talk_key_repeat_count >= FPS
			&& (talk_key_repeat_count - FPS) % ((FPS + 2) / 3) == 0) {
			Talk_event(&talk_key_repeat_event);
			if (!talk_mapped)
				talk_key_repeat_count = 0;
		}
	}

#if defined(_UNIX) || defined(_CYGWIN)
	if (iniClient.kdpy) {
		n = XEventsQueued(iniClient.kdpy, queued);
		for (i = 0; i < n; i++) {
			XNextEvent(iniClient.kdpy, &event);
			switch (event.type) {
			case KeyPress:
			case KeyRelease:
				Key_event(&event);
				break;

				/* Back in play */
			case FocusIn:
				gotFocus = true;
				XAutoRepeatOff(iniClient.kdpy);
				break;

				/* Probably not playing now */
			case FocusOut:
			case UnmapNotify:
				gotFocus = false;
				XAutoRepeatOn(iniClient.kdpy);
				break;

			case MappingNotify:
				XRefreshKeyboardMapping(&event.xmapping);
				break;
			}
		}
	}
#endif
}

///////////////////////////////////////////////////////////////////////////////
ipos	delta;
ipos	mouse;			/* position of mouse pointer. */
int 	movement;		/* horizontal mouse movement. */


///////////////////////////////////////////////////////////////////////////////
void xevent_pointer(void)
{ 
#if defined(_UNIX) || defined(_CYGWIN)
	XEvent				event;
#endif

	if (pointerControl) {
		if (!talk_mapped) {

#if defined(_WINDOWS) && !defined(_CYGWIN)
		/* This is a HACK to fix mouse control under windows. */
		{
		 POINT point;

		 GetCursorPos(&point);
		 movement = point.x - draw_width/2; 
		 XWarpPointer(iniClient.dpy, None, draw,
				  0, 0, 0, 0,
				  draw_width/2, draw_height/2);
		}
		/* fix end */
#endif 

			if (movement != 0) {
				Send_pointer_move(movement);
				delta.x = draw_width / 2 - mouse.x;
				delta.y = draw_height / 2 - mouse.y;
				if (ABS(delta.x) > 3 * draw_width / 8
					|| ABS(delta.y) > 1 * draw_height / 8) {

#if defined(_UNIX) || defined(_CYGWIN)
					memset(&event, 0, sizeof(event));
					event.type = MotionNotify;
					event.xmotion.display = iniClient.dpy;
					event.xmotion.window = draw;
					event.xmotion.x = draw_width/2;
					event.xmotion.y = draw_height/2;
					XSendEvent(iniClient.dpy, draw, False, PointerMotionMask, &event);
					XWarpPointer(iniClient.dpy, None, draw,
								 0, 0, 0, 0,
								 draw_width/2, draw_height/2);
#endif
					XFlush(iniClient.dpy);
				}
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
#if defined(_UNIX) || defined(_CYGWIN)
int x_event(int new_input)
#else
int win_xevent(XEvent event)
#endif
{
	int 				queued = 0;
#if defined(_UNIX) || defined(_CYGWIN)
	int 				i, n;
	XEvent				event;
#endif

#ifdef JOYSTICK
	Joystick_event();
#endif /* JOYSTICK */

	movement = 0;

#if defined(_UNIX) || defined(_CYGWIN)
	switch (new_input) {
	case 0: queued = QueuedAlready; break;
	case 1: queued = QueuedAfterReading; break;
	case 2: queued = QueuedAfterFlush; break;
	default:
		errno = 0;
		error("Bad input queue type (%d)", new_input);
		return -1;
	}
	n = XEventsQueued(iniClient.dpy, queued);
	for (i = 0; i < n; i++) {
		XNextEvent(iniClient.dpy, &event);
#endif
		switch (event.type) {

#if defined(_UNIX) || defined(_CYGWIN)
			/*
			 * after requesting a selection we are notified that we
			 * can access it.
			 */
		case SelectionNotify:
			SelectionNotify_event(&event);	  
			break;
			/*
			 * we are requested to provide a selection.
			 */
		case SelectionRequest:
			SelectionRequest_event(&event);
			break;

		case SelectionClear:
			if (iniClient.selectionAndHistory)
				Clear_selection();
			break;

		case MapNotify:
			MapNotify_event(&event);
			break;

		case ClientMessage:
			if (ClientMessage_event(&event) == -1) {
				return -1;
			}
			break;

			/* Back in play */
		case FocusIn:
			FocusIn_event(&event);
			break;

			/* Probably not playing now */
		case FocusOut:
		case UnmapNotify:
			UnmapNotify_event(&event);
			break;
			
		case MappingNotify:
			XRefreshKeyboardMapping(&event.xmapping);
			break;


		case ConfigureNotify:
			ConfigureNotify_event(&event);
			break;
#endif

		case KeyPress:
			talk_key_repeat_count = 0;
			/* FALLTHROUGH */
		case KeyRelease:
			KeyChanged_event(&event);
			break;

		case ButtonPress:
			ButtonPress_event(&event);
			break;

		case MotionNotify:
			MotionNotify_event(&event);
			break;

		case ButtonRelease:
			if (ButtonRelease_event(&event) == -1) 
				return -1;
			break;

		case Expose:
			Expose_event(&event);
			break;

		case EnterNotify:
		case LeaveNotify:
			Widget_event(&event);
			break;

		default:
			break;
		}
#if defined(_UNIX) || defined(_CYGWIN)
	}
#endif

	xevent_keyboard(queued);	
	xevent_pointer();
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
void CheckButtonUrl(XEvent* event)
{
	int		i;
	int		x, y;

	x = event->xbutton.x;
	y = event->xbutton.y;
	D(xpprintf("CheckButtonUrl: x=%d y=%d\n", x, y);)

	for (i=0; i<MAX_MSGS; i++)
	{
		if (urlRects[i].InRect(x, y))
		{
			String	s;
			s = TalkMsg[i]->txt;
			i = s.RFind('[');
			if (i != -1)
				s = s.Left(i);
			s.TrimTrailingWhiteSpace();
			D(xpprintf("Got url \"%s\"\n", (PCSTR)s);)
			OpenUrl(s);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
void OpenUrl(PCSTR s)
{

#if defined(_WINDOWS) && !defined(_CYGWIN)
	BOOL	ret;

	SHELLEXECUTEINFO	si;
	memset(&si, 0, sizeof(SHELLEXECUTEINFO));
	si.cbSize = sizeof(SHELLEXECUTEINFO);
	si.lpVerb = "open";
	si.lpFile = s;
	si.nShow = SW_SHOW;
	si.fMask = SEE_MASK_NOCLOSEPROCESS;
	
	ret = ShellExecuteEx(&si);

#else
	extern char** environ;

	pid_t pid;
	pid = fork();
	if (pid == -1)
		return;
	if (pid == 0)
	{
		String	exe;
		exe = Conf_browser();
		exe += " ";
		exe += s;

		char* argv[7];
		argv[0] = "sh";
		argv[1] = "-c";
		argv[2] = (char*)(PCSTR)exe;
		argv[3] = 0;
		execve("/bin/sh", argv, environ);
		exit(127);
	}
#endif
}
