/* $Id: configure.cpp,v 1.18 2007/02/17 06:18:15 dick Exp $
 *
 * XPilot, a multiplayer gravity war game.	Copyright (C) 1991-2001 by
 *
 *		Bjørn Stabell		 <bjoern@xpilot.org>
 *		Ken Ronny Schouten	 <ken@xpilot.org>
 *		Bert Gijsbers		 <bert@xpilot.org>
 *		Dick Balaska		 <dick@xpilot.org>
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
 * $Log: configure.cpp,v $
 * Revision 1.18  2007/02/17 06:18:15  dick
 * client/Audio becomes common/AudioMan.
 *
 * Revision 1.17  2004/05/05 18:00:32  dick
 * Move maxVolume to the top of the list of options
 *
 * Revision 1.16  2004/05/05 00:58:37  dick
 * Need randommnt.h
 *
 * Revision 1.15  2004/04/29 16:08:26  dick
 * compile with SOUND enabled.
 *
 * Revision 1.14  2004/04/27 01:54:13  dick
 * Display urls in the message list as underlined.
 * Open a browser if the url is clicked on.
 *
 * Revision 1.13  2004/02/03 03:39:52  dick
 * Add Windows precompiled header support.
 *
 * Revision 1.12  2004/01/20 15:14:26  dick
 * Get rid of #define WINDOWSCALING.  Window scaling is always built, it's not an option.
 *
 * Revision 1.11  2004/01/07 04:37:10  dick
 * More CYGWIN changes
 *
 * Revision 1.10  2004/01/06 04:48:27  dick
 * Add support for building under Cygwin.  Still needs work and the output
 * is X11 and requires something like Cygwin's XWin.exe to run under.
 *
 * Revision 1.9  2002/09/02 15:42:40  dick
 * Singleton ini becomes iniClient.
 *
 * Revision 1.8  2002/08/15 02:57:57  dick
 * Retab
 *
 * Revision 1.7  2002/07/16 08:51:47  dick
 * Fix showScoreDecimals
 *
 * Revision 1.6  2002/05/18 20:55:33  dick
 * Update to XPilot-4.5.1
 *
 * Revision 1.5  2001/09/11 09:08:30  dick
 * Encapsulate all client resources into new class Ini (Linux cleanups)
 *
 * Revision 1.4  2001/09/11 07:33:44  dick
 * Encapsulate all client resources into new class Ini.
 *
 * Revision 1.3  2001/07/27 06:30:37  dick
 * Client builds and runs on Linux
 *
 * Revision 1.2  2001/07/27 05:47:38  dick
 * Client builds and runs in C++
 *
 */


/*
 * Configure.c: real-time option control.
 * To add your own option to the XPilot client do the following:
 * 1: Define storage for the option value in either client.c/paint.c/xinit.c
 *	  or use a bit in the instruments option set (using the SHOW_XXX macros).
 * 2: Add a declaration for this storage to either client.h/paint.h/xinit.h
 *	  or, in case a bit in instruments is used, add a SHOW_ macro to client.h.
 * 3: Add an X resource record to the XrmOptionDescRec options[] table in
 *	  default.c to have it recognised by the X resource manager routines.
 * 4: Have it set at startup by the Parse_options() routine in default.c.
 * 5: Add the functionality of your option, probably in the same file
 *	  as the storage for the option was defined in.
 * 6: Add it to configure.c (this file) as follows:
 *	 a) Determine if it needs either a bool/int/float widget
 *		and find a similar option from which you can copy code.
 *	 b) Add the Config_create_XXX function prototype at the top of this file.
 *	 c) Add the Config_create_XXX function name to the config_creator[] table.
 *		The order in this table determines the order of the options on screen.
 *	 d) Define the Config_create_XXX function similar to one of the others.
 *	 e) If it needs a callback when the value changes then add a
 *		Config_update_XXX() function after the other update callbacks
 *		and declare a prototype for the callback at the top of this file.
 *		The Config_update_XXX() function should be given as an argument to
 *		the Config_create_bool/int/float() creator in Config_create_XXX().
 *		If the option doesn't need a callback then the calback argument
 *		should be given as NULL.
 *	 f) Add one line to the Config_save() routine to have the option saved.
 * 7: Document your option in the manual page for the client.
 * 8: Mail a context diff (diff -c old new) of your changes to
 *	  xpilot@xpilot.org.
 */
#include "StdAfx.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>

#if defined(_UNIX) || defined(_CYGWIN)
# include <unistd.h>
# include <pwd.h>
# include <X11/Xlib.h>
# include <X11/Xos.h>
# include <X11/Xutil.h>
#else
# include "NT/winX.h"
# include "NT/winClient.h"
#endif

#include "version.h"
#include "config.h"
#include "const.h"
#include "paint.h"
#include "xinit.h"
#include "bit.h"
#include "keys.h"
#include "netclient.h"
#include "widget.h"
#include "configure.h"
#include "setup.h"
#include "error.h"
#include "protoclient.h"
#include "portability.h"
#include "commonproto.h"
#include "cstring.h"
#include "IniClient.h"
#include "AudioMan.h"

char configure_version[] = VERSION;

#ifndef PATH_MAX
#define PATH_MAX		1023
#endif

extern const char		*Get_keyResourceString(keys_t key);

static int Config_create_power(int widget_desc, int *height);
static int Config_create_turnSpeed(int widget_desc, int *height);
static int Config_create_turnResistance(int widget_desc, int *height);
static int Config_create_altPower(int widget_desc, int *height);
static int Config_create_altTurnSpeed(int widget_desc, int *height);
static int Config_create_altTurnResistance(int widget_desc, int *height);
static int Config_create_showMessages(int widget_desc, int *height);
static int Config_create_showHUD(int widget_desc, int *height);
static int Config_create_showHUDRadar(int widget_desc, int* height);
static int Config_create_horizontalHUDLine(int widget_desc, int *height);
static int Config_create_verticalHUDLine(int widget_desc, int *height);
static int Config_create_speedFactHUD(int widget_desc, int *height);
static int Config_create_speedFactPTR(int widget_desc, int *height);
static int Config_create_fuelNotify(int widget_desc, int *height);
static int Config_create_fuelWarning(int widget_desc, int *height);
static int Config_create_fuelCritical(int widget_desc, int *height);
static int Config_create_fuelGauge(int widget_desc, int *height);
static int Config_create_outlineWorld(int widget_desc, int *height);
static int Config_create_filledWorld(int widget_desc, int *height);
static int Config_create_texturedWalls(int widget_desc, int *height);
static int Config_create_texturedObjects(int widget_desc, int *height);
static int Config_create_slidingRadar(int widget_desc, int *height);
static int Config_create_showItems(int widget_desc, int *height);
static int Config_create_showItemsTime(int widget_desc, int *height);
static int Config_create_showScoreDecimals(int widget_desc, int* height);
static int Config_create_backgroundPointDist(int widget_desc, int *height);
static int Config_create_backgroundPointSize(int widget_desc, int *height);
static int Config_create_sparkSize(int widget_desc, int *height);
static int Config_create_charsPerSecond(int widget_desc, int *height);
static int Config_create_toggleShield(int widget_desc, int *height);
static int Config_create_autoShield(int widget_desc, int *height);
static int Config_create_sparkProb(int widget_desc, int *height);
static int Config_create_shotSize(int widget_desc, int *height);
static int Config_create_teamShotSize(int widget_desc, int *height);
static int Config_create_showNastyShots(int widget_desc, int *height);
static int Config_create_hudColor(int widget_desc, int *height);
static int Config_create_hudLockColor(int widget_desc, int *height);
static int Config_create_wallColor(int widget_desc, int *height);
static int Config_create_decorColor(int widget_desc, int *height);
static int Config_create_showDecor(int widget_desc, int *height);
static int Config_create_outlineDecor(int widget_desc, int *height);
static int Config_create_filledDecor(int widget_desc, int *height);
static int Config_create_texturedDecor(int widget_desc, int *height);
static int Config_create_texturedBalls(int widget_desc, int *height);
static int Config_create_maxFPS(int widget_desc, int *height);
static int Config_create_maxMessages(int widget_desc, int *height);
static int Config_create_messagesToStdout(int widget_desc, int* height);
static int Config_create_reverseScroll(int widget_desc, int *height);
static int Config_create_oldMessagesColor(int widget_desc, int *height);
#ifdef SOUND
static int Config_create_maxVolume(int widget_desc, int *height);
#endif
static int Config_create_showShipName(int widget_desc, int *height);
static int Config_create_showMineName(int widget_desc, int *height);
static int Config_create_fuelMeter(int widget_desc, int *height);
static int Config_create_powerMeter(int widget_desc, int *height);
static int Config_create_turnSpeedMeter(int widget_desc, int *height);
static int Config_create_packetSizeMeter(int widget_desc, int *height);
static int Config_create_packetLossMeter(int widget_desc, int *height);
static int Config_create_packetDropMeter(int widget_desc, int *height);
static int Config_create_packetLagMeter(int widget_desc, int* height);
static int Config_create_clock(int widget_desc, int *height);
static int Config_create_clockAMPM(int widget_desc, int *height);
static int Config_create_markingLights(int widget_desc, int *height);
#if defined(_WINDOWS) && !defined(_CYGWIN)
static int Config_create_threadedDraw(int widget_desc, int *height);
#endif

static int Config_create_scaleFactor(int widget_desc, int *height);
static int Config_create_altScaleFactor(int widget_desc, int *height);

static int Config_create_save(int widget_desc, int *height);

static int Config_update_bool(int widget_desc, void *data, bool *val);
static int Config_update_instruments(int widget_desc, void *data, bool *val);
static int Config_update_dots(int widget_desc, void *data, int *val);
static int Config_update_altPower(int widget_desc, void *data, DFLOAT *val);
static int Config_update_altTurnResistance(int widget_desc, void *data,
										   DFLOAT *val);
static int Config_update_altTurnSpeed(int widget_desc, void *data, DFLOAT *val);
static int Config_update_power(int widget_desc, void *data, DFLOAT *val);
static int Config_update_turnResistance(int widget_desc, void *data,
										DFLOAT *val);
static int Config_update_turnSpeed(int widget_desc, void *data, DFLOAT *val);
static int Config_update_sparkProb(int widget_desc, void *data, DFLOAT *val);
static int Config_update_charsPerSecond(int widget_desc, void *data, int *val);
static int Config_update_toggleShield(int widget_desc, void *data, bool *val);
static int Config_update_autoShield(int widget_desc, void *data, bool *val);
static int Config_update_maxFPS(int widget_desc, void *data, int *val);
static int Config_update_volume(int widget_desc, void *data, int *val);
static int Config_update_texturedObjects(int widget_desc, void *data, bool *val);
static int Config_update_scaleFactor(int widget_desc, void *data, DFLOAT *val);

static int Config_close(int widget_desc, void *data, const char **strptr);
static int Config_next(int widget_desc, void *data, const char **strptr);
static int Config_prev(int widget_desc, void *data, const char **strptr);
static int Config_save(int widget_desc, void *data, const char **strptr);
static int Config_save_confirm_callback(int widget_desc, void *popup_desc,
										const char **strptr);


static bool 	config_created = false,
				config_mapped = false;
static int		config_page,
				config_x,
				config_y,
				config_width,
				config_height,
				config_space,
				config_max,
				config_button_space,
				config_text_space,
				config_text_height,
				config_button_height,
				config_entry_height,
				config_bool_width,
				config_bool_height,
				config_int_width,
				config_float_width,
				config_arrow_width,
				config_arrow_height;
static int		*config_widget_desc,
				config_save_confirm_desc = NO_WIDGET;
static int		(*config_creator[])(int widget_desc, int *height) = {
#ifdef SOUND
	Config_create_maxVolume,
#endif
	Config_create_power,
	Config_create_turnSpeed,
	Config_create_turnResistance,
	Config_create_altPower,
	Config_create_altTurnSpeed,
	Config_create_altTurnResistance,
	Config_create_showMessages,
	Config_create_maxMessages,
	Config_create_messagesToStdout,
	Config_create_reverseScroll,
	Config_create_oldMessagesColor,
	Config_create_showHUD,
	Config_create_showHUDRadar,
	Config_create_horizontalHUDLine,
	Config_create_verticalHUDLine,
	Config_create_speedFactHUD,
	Config_create_speedFactPTR,
	Config_create_fuelNotify,
	Config_create_fuelWarning,
	Config_create_fuelCritical,
	Config_create_fuelGauge,
	Config_create_outlineWorld,
	Config_create_filledWorld,
	Config_create_texturedWalls,
	Config_create_texturedObjects,
	Config_create_slidingRadar,
	Config_create_showItems,
	Config_create_showItemsTime,
	Config_create_showScoreDecimals,
	Config_create_backgroundPointDist,
	Config_create_backgroundPointSize,
	Config_create_sparkSize,
	Config_create_sparkProb,
	Config_create_charsPerSecond,
	Config_create_markingLights,
	Config_create_toggleShield,
	Config_create_autoShield,
	Config_create_showNastyShots,
	Config_create_shotSize,
	Config_create_teamShotSize,
	Config_create_hudColor,
	Config_create_hudLockColor,
	Config_create_wallColor,
	Config_create_decorColor,
	Config_create_showDecor,
	Config_create_outlineDecor,
	Config_create_filledDecor,
	Config_create_texturedDecor,
	Config_create_texturedBalls,
	Config_create_maxFPS,
	Config_create_showShipName,
	Config_create_showMineName,
	Config_create_fuelMeter,
	Config_create_powerMeter,
	Config_create_turnSpeedMeter,
	Config_create_packetSizeMeter,
	Config_create_packetLossMeter,
	Config_create_packetDropMeter,
	Config_create_packetLagMeter,
	Config_create_clock,
	Config_create_clockAMPM,
#if defined(_WINDOWS) && !defined(_CYGWIN)
	Config_create_threadedDraw,
#endif
	Config_create_scaleFactor,
	Config_create_altScaleFactor,
	Config_create_save					/* must be last */
};
static int				config_widget_ids[NELEM(config_creator)];

static void Create_config(void)
{
	int 				i,
						num,
						height,
						offset,
						width,
						widget_desc;
	bool				full;

	/*
	 * Window dimensions relative to the top window.
	 */
	config_x = 0;
	config_y = RadarHeight + ButtonHeight + 2;
	config_width = 256;
	config_height = top_height - config_y;

	/*
	 * Space between label-text and label-border.
	 */
	config_text_space = 3;
	/*
	 * Height of a label window.
	 */
	config_text_height = 2 * 1 + textFont->ascent + textFont->descent;

	/*
	 * Space between button-text and button-border.
	 */
	config_button_space = 3;
	/*
	 * Height of a button window.
	 */
	config_button_height = buttonFont->ascent + buttonFont->descent
							+ 2 * 1;

	config_entry_height = MAX(config_text_height, config_button_height);

	/*
	 * Space between entries and between an entry and the border.
	 */
	config_space = 6;

	/*
	 * Sizes of the different widget types.
	 */
	config_bool_width = XTextWidth(buttonFont, "Yes", 3)
						+ 2 * config_button_space;
	config_bool_height = config_button_height;
	config_arrow_height = config_text_height;
	config_arrow_width = config_text_height;
	config_int_width = 4 + XTextWidth(buttonFont, "1000", 4);
	config_float_width = 4 + XTextWidth(buttonFont, "0.22", 4);

	config_max = NELEM(config_creator);
	config_widget_desc = (int *) malloc(config_max * sizeof(int));
	if (config_widget_desc == NULL) {
		error("No memory for config");
		return;
	}

	num = -1;
	full = true;
	for (i = 0; i < NELEM(config_creator); i++) {
		if (full == true) {
			full = false;
			num++;
			config_widget_desc[num]
				= Widget_create_form(NO_WIDGET, top,
									 config_x, config_y,
									 config_width, config_height,
									 0);
			if (config_widget_desc[num] == 0) {
				break;
			}
			height = config_height - config_space - config_button_height;
			width = 2 * config_button_space + XTextWidth(buttonFont,
														  "PREV", 4);
			offset = config_width - width - config_space;
			widget_desc =
				Widget_create_activate(config_widget_desc[num],
									   offset, height,
									   width, config_button_height,
									   0, "PREV", Config_prev, 
										   (void *)(long)num);
			if (widget_desc == 0) {
				break;
			}
			width = 2 * config_button_space + XTextWidth(buttonFont,
														  "NEXT", 4);
			offset = (config_width - width) / 2;
			widget_desc =
				Widget_create_activate(config_widget_desc[num],
									   offset, height,
									   width, config_button_height,
									   0, "NEXT", Config_next,
										   (void *)(long)num);
			if (widget_desc == 0) {
				break;
			}
			width = 2 * config_button_space + XTextWidth(buttonFont,
														  "CLOSE", 5);
			offset = config_space;
			widget_desc =
				Widget_create_activate(config_widget_desc[num],
									   offset, height,
									   width, config_button_height,
									   0, "CLOSE", Config_close,
										   (void *)(long)num);
			if (widget_desc == 0) {
				break;
			}
			height = config_space;
		}
		if ((config_widget_ids[i] =
			 (*config_creator[i])(config_widget_desc[num], &height)) == 0) {
			i--;
			full = true;
			if (height == config_space) {
				break;
			}
			continue;
		}
	}
	if (i < NELEM(config_creator)) {
		for (; num >= 0; num--) {
			if (config_widget_desc[num] != 0) {
				Widget_destroy(config_widget_desc[num]);
			}
		}
		config_created = false;
		config_mapped = false;
	} else {
		config_max = num + 1;
		config_widget_desc = (int *)realloc(config_widget_desc,
											config_max * sizeof(int));
		config_page = 0;
		for (i = 0; i < config_max; i++) {
			Widget_map_sub(config_widget_desc[i]);
		}
		config_created = true;
		config_mapped = false;
	}
}

static int Config_close(int widget_desc, void *data, const char **strptr)
{
	Widget_unmap(config_widget_desc[config_page]);
	config_mapped = false;
	return 0;
}

static int Config_next(int widget_desc, void *data, const char **strptr)
{
	int 				prev_page = config_page;

	if (config_max > 1) {
		config_page = (config_page + 1) % config_max;
		Widget_raise(config_widget_desc[config_page]);
		Widget_unmap(config_widget_desc[prev_page]);
		config_mapped = true;
	}
	return 0;
}

static int Config_prev(int widget_desc, void *data, const char **strptr)
{
	int 				prev_page = config_page;

	if (config_max > 1) {
		config_page = (config_page - 1 + config_max) % config_max;
		Widget_raise(config_widget_desc[config_page]);
		Widget_unmap(config_widget_desc[prev_page]);
		config_mapped = true;
	}
	return 0;
}

static int Config_create_bool(int widget_desc, int *height,
							  const char *str, bool val,
							  int (*callback)(int, void *, bool *),
							  void *data)
{
	int 				offset,
						label_width,
						boolw;

	if (*height + 2*config_entry_height + 2*config_space >= config_height) {
		return 0;
	}
	label_width = XTextWidth(textFont, str, strlen(str))
				  + 2 * config_text_space;
	offset = config_width - (config_space + config_bool_width);
	if (config_space + label_width > offset) {
		if (*height + 3*config_entry_height + 2*config_space
			>= config_height) {
			return 0;
		}
	}

	Widget_create_label(widget_desc, config_space, *height
							+ (config_entry_height - config_text_height) / 2,
						label_width, config_text_height,
						0, str);
	if (config_space + label_width > offset) {
		*height += config_entry_height;
	}
	boolw = Widget_create_bool(widget_desc,
					   offset, *height
						   + (config_entry_height - config_bool_height) / 2,
					   config_bool_width,
					   config_bool_height,
					   0, val, callback, data);
	*height += config_entry_height + config_space;

	return boolw;
}

static int Config_create_int(int widget_desc, int *height,
							 const char *str, int *val, int min, int max,
							 int (*callback)(int, void *, int *), void *data)
{
	int 				offset,
						label_width,
						intw;

	if (*height + 2*config_entry_height + 2*config_space >= config_height) {
		return 0;
	}
	label_width = XTextWidth(textFont, str, strlen(str))
				  + 2 * config_text_space;
	offset = config_width - (config_space + 2 * config_arrow_width
			+ config_int_width);
	if (config_space + label_width > offset) {
		if (*height + 3*config_entry_height + 2*config_space
			>= config_height) {
			return 0;
		}
	}
	Widget_create_label(widget_desc, config_space, *height
						+ (config_entry_height - config_text_height) / 2,
						label_width, config_text_height,
						0, str);
	if (config_space + label_width > offset) {
		*height += config_entry_height;
	}
	intw = Widget_create_int(widget_desc, offset, *height
							  + (config_entry_height - config_text_height) / 2,
							 config_int_width, config_text_height,
							 0, val, min, max, callback, data);
	offset += config_int_width;
	Widget_create_arrow_left(widget_desc, offset, *height
							 + (config_entry_height - config_arrow_height) / 2,
							 config_arrow_width, config_arrow_height,
							 0, intw);
	offset += config_arrow_width;
	Widget_create_arrow_right(widget_desc, offset, *height
							  + (config_entry_height - config_arrow_height) / 2,
							  config_arrow_width, config_arrow_height,
							  0, intw);
	*height += config_entry_height + config_space;

	return intw;
}

static int Config_create_float(int widget_desc, int *height,
							   const char *str, DFLOAT *val, DFLOAT min, DFLOAT max,
							   int (*callback)(int, void *, DFLOAT *),
							   void *data)
{
	int 				offset,
						label_width,
						floatw;

	if (*height + 2*config_entry_height + 2*config_space >= config_height) {
		return 0;
	}
	label_width = XTextWidth(textFont, str, strlen(str))
				  + 2 * config_text_space;
	offset = config_width - (config_space + 2 * config_arrow_width
			+ config_float_width);
	if (config_space + label_width > offset) {
		if (*height + 3*config_entry_height + 2*config_space
			>= config_height) {
			return 0;
		}
	}
	Widget_create_label(widget_desc, config_space, *height
						+ (config_entry_height - config_text_height) / 2,
						label_width, config_text_height,
						0, str);
	if (config_space + label_width > offset) {
		*height += config_entry_height;
	}
	floatw = Widget_create_float(widget_desc, offset, *height
								 + (config_entry_height
								 - config_text_height) / 2,
								 config_float_width, config_text_height,
								 0, val, min, max, callback, data);
	offset += config_float_width;
	Widget_create_arrow_left(widget_desc, offset, *height
							 + (config_entry_height - config_arrow_height) / 2,
							 config_arrow_width, config_arrow_height,
							 0, floatw);
	offset += config_arrow_width;
	Widget_create_arrow_right(widget_desc, offset, *height
							  + (config_entry_height - config_arrow_height) / 2,
							  config_arrow_width, config_arrow_height,
							  0, floatw);
	*height += config_entry_height + config_space;

	return floatw;
}

static int Config_create_power(int widget_desc, int *height)
{
	return Config_create_float(widget_desc, height,
							   "power", &iniClient.power,
							   MIN_PLAYER_POWER, MAX_PLAYER_POWER,
							   Config_update_power, NULL);
}

static int Config_create_turnSpeed(int widget_desc, int *height)
{
	return Config_create_float(widget_desc, height,
							   "turnSpeed", &iniClient.turnspeed,
							   MIN_PLAYER_TURNSPEED, MAX_PLAYER_TURNSPEED,
							   Config_update_turnSpeed, NULL);
}

static int Config_create_turnResistance(int widget_desc, int *height)
{
	return Config_create_float(widget_desc, height,
							   "turnResistance", &iniClient.turnresistance,
							   MIN_PLAYER_TURNRESISTANCE,
							   MAX_PLAYER_TURNRESISTANCE,
							   Config_update_turnResistance, NULL);
}

static int Config_create_altPower(int widget_desc, int *height)
{
	return Config_create_float(widget_desc, height,
							   "altPower", &iniClient.power_s,
							   MIN_PLAYER_POWER, MAX_PLAYER_POWER,
							   Config_update_altPower, NULL);
}

static int Config_create_altTurnSpeed(int widget_desc, int *height)
{
	return Config_create_float(widget_desc, height,
							   "altTurnSpeed", &iniClient.turnspeed_s,
							   MIN_PLAYER_TURNSPEED, MAX_PLAYER_TURNSPEED,
							   Config_update_altTurnSpeed, NULL);
}

static int Config_create_altTurnResistance(int widget_desc, int *height)
{
	return Config_create_float(widget_desc, height,
							   "altTurnResistance", &iniClient.turnresistance_s,
							   MIN_PLAYER_TURNRESISTANCE,
							   MAX_PLAYER_TURNRESISTANCE,
							   Config_update_altTurnResistance, NULL);
}

static int Config_create_showMessages(int widget_desc, int *height)
{
	return Config_create_bool(widget_desc, height, "showMessages",
							BIT(iniClient.instruments, SHOW_MESSAGES)
								? true : false,
							Config_update_instruments,
							(void *) SHOW_MESSAGES);
}

static int Config_create_maxMessages(int widget_desc, int *height)
{
	return Config_create_int(widget_desc, height,
						   "maxMessages", &iniClient.maxMessages, 1, MAX_MSGS,
						   NULL, NULL);
}

static int Config_create_messagesToStdout(int widget_desc, int *height)
{
	return Config_create_int(widget_desc, height,
						   "messagesToStdout", &iniClient.messagesToStdout, 0, 2,
						   NULL, NULL);
}

static int Config_create_reverseScroll(int widget_desc, int *height)
{
	return Config_create_bool(widget_desc, height, "reverseScroll",
							BIT(iniClient.instruments, SHOW_REVERSE_SCROLL)
								? true : false,
							Config_update_instruments,
							(void *) SHOW_REVERSE_SCROLL);
}

static int Config_create_oldMessagesColor(int widget_desc, int *height)
{
	return Config_create_int(widget_desc, height,
						   "oldMessagesColor", &iniClient.oldMessagesColor, 0, iniClient.maxColors - 1,
						   NULL, NULL);
}

static int Config_create_showHUD(int widget_desc, int *height)
{
	return Config_create_bool(widget_desc, height, "showHUD",
							  BIT(iniClient.instruments, SHOW_HUD_INSTRUMENTS)
								  ? true : false,
							  Config_update_instruments,
							  (void *) SHOW_HUD_INSTRUMENTS);
}

static int Config_create_showHUDRadar(int widget_desc, int *height)
{
	return Config_create_bool(widget_desc, height, "showHUDRadar",
							  BIT(iniClient.instruments, SHOW_HUD_RADAR)
								  ? true : false,
							  Config_update_instruments,
							  (void *) SHOW_HUD_RADAR);
}

static int Config_create_horizontalHUDLine(int widget_desc, int *height)
{
	return Config_create_bool(widget_desc, height, "horizontalHUDLine",
							  BIT(iniClient.instruments, SHOW_HUD_HORIZONTAL)
								  ? true : false,
							  Config_update_instruments,
							  (void *) SHOW_HUD_HORIZONTAL);
}

static int Config_create_verticalHUDLine(int widget_desc, int *height)
{
	return Config_create_bool(widget_desc, height, "verticalHUDLine",
							  BIT(iniClient.instruments, SHOW_HUD_VERTICAL)
								  ? true : false,
							  Config_update_instruments,
							  (void *) SHOW_HUD_VERTICAL);
}

static int Config_create_speedFactHUD(int widget_desc, int *height)
{
	return Config_create_float(widget_desc, height,
							   "speedFactHUD", &iniClient.hudMoveFact, -10.0, 10.0,
							   NULL, NULL);
}

static int Config_create_speedFactPTR(int widget_desc, int *height)
{
	return Config_create_float(widget_desc, height,
							   "speedFactPTR", &iniClient.ptrMoveFact, -10.0, 10.0,
							   NULL, NULL);
}

static int Config_create_fuelNotify(int widget_desc, int *height)
{
	return Config_create_int(widget_desc, height,
							 "fuelNotify", &iniClient.fuelLevel3, 0, 1000,
							 NULL, NULL);
}

static int Config_create_fuelWarning(int widget_desc, int *height)
{
	return Config_create_int(widget_desc, height,
							 "fuelWarning", &iniClient.fuelLevel2, 0, 1000,
							 NULL, NULL);
}

static int Config_create_fuelCritical(int widget_desc, int *height)
{
	return Config_create_int(widget_desc, height,
							 "fuelCritical", &iniClient.fuelLevel1, 0, 1000,
							 NULL, NULL);
}

static int Config_create_fuelGauge(int widget_desc, int *height)
{
	return Config_create_bool(widget_desc, height, "fuelGauge",
							  BIT(iniClient.instruments, SHOW_FUEL_GAUGE)
								  ? true : false,
							  Config_update_instruments,
							  (void *) SHOW_FUEL_GAUGE);
}

static int Config_create_outlineWorld(int widget_desc, int *height)
{
	return Config_create_bool(widget_desc, height, "outlineWorld",
							  BIT(iniClient.instruments, SHOW_OUTLINE_WORLD)
								  ? true : false,
							  Config_update_instruments,
							  (void *) SHOW_OUTLINE_WORLD);
}

static int Config_create_filledWorld(int widget_desc, int *height)
{
	return Config_create_bool(widget_desc, height, "filledWorld",
							  BIT(iniClient.instruments, SHOW_FILLED_WORLD)
								  ? true : false,
							  Config_update_instruments,
							  (void *) SHOW_FILLED_WORLD);
}

static int Config_create_texturedWalls(int widget_desc, int *height)
{
	return Config_create_bool(widget_desc, height, "texturedWalls",
							  BIT(iniClient.instruments, SHOW_TEXTURED_WALLS)
								  ? true : false,
							  Config_update_instruments,
							  (void *) SHOW_TEXTURED_WALLS);
}

static int Config_create_texturedObjects(int widget_desc, int *height)
{
	return Config_create_bool(widget_desc, height, "texturedObjects",
							  (iniClient.blockBitmaps) ? true : false,
							  Config_update_texturedObjects,
							  NULL);
}


static int Config_create_slidingRadar(int widget_desc, int *height)
{
	if (Client_wrap_mode() == 0) {
		return 1;
	}
	return Config_create_bool(widget_desc, height, "slidingRadar",
							  BIT(iniClient.instruments, SHOW_SLIDING_RADAR)
								  ? true : false,
							  Config_update_instruments,
							  (void *) SHOW_SLIDING_RADAR);
}

static int Config_create_backgroundPointDist(int widget_desc, int *height)
{
	return Config_create_int(widget_desc, height,
							 "backgroundPointDist", &iniClient.mapPointDistance, 0, 10,
							 Config_update_dots, NULL);
}

static int Config_create_showItems(int widget_desc, int *height)
{
	return Config_create_bool(widget_desc, height, "showItems",
							BIT(iniClient.instruments, SHOW_ITEMS)
								? true : false,
							Config_update_instruments,
							(void *) SHOW_ITEMS);
}

static int Config_create_showItemsTime(int widget_desc, int *height)
{
	return Config_create_float(widget_desc, height,
							 "showItemsTime", &iniClient.showItemsTime,
							 MIN_SHOW_ITEMS_TIME,
							 MAX_SHOW_ITEMS_TIME,
							 NULL, NULL);
}

static int Config_create_showScoreDecimals(int widget_desc, int *height)
{
	return Config_create_int(widget_desc, height,
							 "showScoreDecimals", &iniClient.showScoreDecimals,
							 0,
							 2,
							 NULL, NULL);
}

static int Config_create_backgroundPointSize(int widget_desc, int *height)
{
	return Config_create_int(widget_desc, height,
							 "backgroundPointSize", &iniClient.mapPointSize,
							 MIN_MAP_POINT_SIZE, MAX_MAP_POINT_SIZE,
							 Config_update_dots, NULL);
}

static int Config_create_sparkSize(int widget_desc, int *height)
{
	return Config_create_int(widget_desc, height,
							 "sparkSize", &iniClient.sparkSize,
							 MIN_SPARK_SIZE, MAX_SPARK_SIZE,
							 NULL, NULL);
}

static int Config_create_sparkProb(int widget_desc, int *height)
{
	return Config_create_float(widget_desc, height,
							   "sparkProb", &iniClient.sparkProb,
							   0.0, 1.0,
							   Config_update_sparkProb, NULL);
}

static int Config_create_charsPerSecond(int widget_desc, int *height)
{
	return Config_create_int(widget_desc, height,
							 "charsPerSecond", &iniClient.charsPerSecond,
							 10, 255,
							 Config_update_charsPerSecond, NULL);
}

static int Config_create_toggleShield(int widget_desc, int *height)
{
	return Config_create_bool(widget_desc, height, "toggleShield",
							  (iniClient.toggleShield) ? true : false,
							  Config_update_toggleShield, NULL);
}

static int Config_create_autoShield(int widget_desc, int *height)
{
	return Config_create_bool(widget_desc, height, "autoShield",
							  (iniClient.autoShield) ? true : false,
							  Config_update_autoShield, NULL);
}

static int Config_create_shotSize(int widget_desc, int *height)
{
	return Config_create_int(widget_desc, height,
						   "shotSize", &iniClient.shotSize,
						   MIN_SHOT_SIZE, MAX_SHOT_SIZE,
						   NULL, NULL);
}

static int Config_create_teamShotSize(int widget_desc, int *height)
{
	return Config_create_int(widget_desc, height,
						   "teamShotSize", &iniClient.teamShotSize,
						   MIN_TEAMSHOT_SIZE, MAX_TEAMSHOT_SIZE,
						   NULL, NULL);
}

static int Config_create_showNastyShots(int widget_desc, int *height)
{
	return Config_create_bool(widget_desc, height, "nastyShots",
							  (iniClient.showNastyShots) ? true : false,
							  Config_update_bool, &iniClient.showNastyShots);
}

static int Config_create_hudColor(int widget_desc, int *height)
{
	return Config_create_int(widget_desc, height,
						   "hudColor", &iniClient.hudColor,
						   1, iniClient.maxColors - 1,
						   NULL, NULL);
}

static int Config_create_hudLockColor(int widget_desc, int *height)
{
	return Config_create_int(widget_desc, height,
						   "hudLockColor", &iniClient.hudLockColor,
						   1, iniClient.maxColors - 1,
						   NULL, NULL);
}

static int Config_create_wallColor(int widget_desc, int *height)
{
	return Config_create_int(widget_desc, height,
						   "wallColor", &iniClient.wallColor,
						   1, iniClient.maxColors - 1,
						   NULL, NULL);
}

static int Config_create_decorColor(int widget_desc, int *height)
{
	return Config_create_int(widget_desc, height,
						   "decorColor", &iniClient.decorColor,
						   1, iniClient.maxColors - 1,
						   NULL, NULL);
}

static int Config_create_showDecor(int widget_desc, int *height)
{
	return Config_create_bool(widget_desc, height, "showDecor",
							  BIT(iniClient.instruments, SHOW_DECOR)
							  ? true : false,
							  Config_update_instruments,
							  (void *) SHOW_DECOR);
}

static int Config_create_outlineDecor(int widget_desc, int *height)
{
	return Config_create_bool(widget_desc, height, "outlineDecor",
							  BIT(iniClient.instruments, SHOW_OUTLINE_DECOR)
								  ? true : false,
							  Config_update_instruments,
							  (void *) SHOW_OUTLINE_DECOR);
}

static int Config_create_filledDecor(int widget_desc, int *height)
{
	return Config_create_bool(widget_desc, height, "filledDecor",
							  BIT(iniClient.instruments, SHOW_FILLED_DECOR)
							  ? true : false,
							  Config_update_instruments,
							  (void *) SHOW_FILLED_DECOR);
}

static int Config_create_texturedDecor(int widget_desc, int *height)
{
	return Config_create_bool(widget_desc, height, "texturedDecor",
							  BIT(iniClient.instruments, SHOW_TEXTURED_DECOR)
								  ? true : false,
							  Config_update_instruments,
							  (void *) SHOW_TEXTURED_DECOR);
}

static int Config_create_texturedBalls(int widget_desc, int *height)
{
	return Config_create_bool(widget_desc, height, "texturedBalls",
							  BIT(iniClient.instruments, SHOW_TEXTURED_BALLS)
								  ? true : false,
							  Config_update_instruments,
							  (void *) SHOW_TEXTURED_BALLS);
}

#ifdef SOUND
static int Config_create_maxVolume(int widget_desc, int *height)
{
	return Config_create_int(widget_desc, height,
							 "maxVolume", &iniClient.maxVolume, 0, 255,
							 Config_update_volume, NULL);
}
#endif

static int Config_create_maxFPS(int widget_desc, int *height)
{
	return Config_create_int(widget_desc, height,
							 "maxFPS", &iniClient.maxFPS, FPS / 2, FPS,
							 Config_update_maxFPS, NULL);
}

static int Config_create_showShipName(int widget_desc, int *height)
{
	return Config_create_bool(widget_desc, height, "showShipName",
							  BIT(iniClient.instruments, SHOW_SHIP_NAME)
								  ? true : false,
							  Config_update_instruments,
							  (void *) SHOW_SHIP_NAME);
}

static int Config_create_showMineName(int widget_desc, int *height)
{
	return Config_create_bool(widget_desc, height, "showMineName",
							  BIT(iniClient.instruments, SHOW_MINE_NAME)
								  ? true : false,
							  Config_update_instruments,
							  (void *) SHOW_MINE_NAME);
}

static int Config_create_fuelMeter(int widget_desc, int *height)
{
	return Config_create_bool(widget_desc, height, "fuelMeter",
							  BIT(iniClient.instruments, SHOW_FUEL_METER)
								  ? true : false,
							  Config_update_instruments,
							  (void *) SHOW_FUEL_METER);
}

static int Config_create_powerMeter(int widget_desc, int *height)
{
	return Config_create_bool(widget_desc, height, "powerMeter",
							  BIT(iniClient.instruments, SHOW_POWER_METER)
								  ? true : false,
							  Config_update_instruments,
							  (void *) SHOW_POWER_METER);
}

static int Config_create_turnSpeedMeter(int widget_desc, int *height)
{
	return Config_create_bool(widget_desc, height, "turnSpeedMeter",
							  BIT(iniClient.instruments, SHOW_TURNSPEED_METER)
								  ? true : false,
							  Config_update_instruments,
							  (void *) SHOW_TURNSPEED_METER);
}

static int Config_create_packetSizeMeter(int widget_desc, int *height)
{
	return Config_create_bool(widget_desc, height, "packetSizeMeter",
							  BIT(iniClient.instruments, SHOW_PACKET_SIZE_METER)
								  ? true : false,
							  Config_update_instruments,
							  (void *) SHOW_PACKET_SIZE_METER);
}

static int Config_create_packetLossMeter(int widget_desc, int *height)
{
	return Config_create_bool(widget_desc, height, "packetLossMeter",
							  BIT(iniClient.instruments, SHOW_PACKET_LOSS_METER)
								  ? true : false,
							  Config_update_instruments,
							  (void *) SHOW_PACKET_LOSS_METER);
}

static int Config_create_packetDropMeter(int widget_desc, int *height)
{
	return Config_create_bool(widget_desc, height, "packetDropMeter",
							  BIT(iniClient.instruments, SHOW_PACKET_DROP_METER)
								  ? true : false,
							  Config_update_instruments,
							  (void *) SHOW_PACKET_DROP_METER);
}

static int Config_create_packetLagMeter(int widget_desc, int *height)
{
	return Config_create_bool(widget_desc, height, "packetLagMeter",
							  BIT(iniClient.instruments, SHOW_PACKET_LAG_METER)
								  ? true : false,
							  Config_update_instruments,
							  (void *) SHOW_PACKET_LAG_METER);
}

static int Config_create_clock(int widget_desc, int *height)
{
	return Config_create_bool(widget_desc, height, "clock",
							  BIT(iniClient.instruments, SHOW_CLOCK)
								  ? true : false,
							  Config_update_instruments,
							  (void *) SHOW_CLOCK);
}

static int Config_create_clockAMPM(int widget_desc, int *height)
{
	return Config_create_bool(widget_desc, height, "clockAMPM",
							  BIT(iniClient.instruments, SHOW_CLOCK_AMPM_FORMAT)
								  ? true : false,
							  Config_update_instruments,
							  (void *) SHOW_CLOCK_AMPM_FORMAT);
}

#if defined(_WINDOWS) && !defined(_CYGWIN)
static int Config_create_threadedDraw(int widget_desc, int *height)
{
	return Config_create_bool(widget_desc, height, "threadedDraw",
							  iniClient.threadedDraw, Config_update_bool,
							  &iniClient.threadedDraw);
}
#endif

static int Config_create_scaleFactor(int widget_desc, int *height)
{
	return Config_create_float(widget_desc, height,
							   "scaleFactor", &iniClient.scaleFactor,
							   MIN_SCALEFACTOR, MAX_SCALEFACTOR,
							   Config_update_scaleFactor, NULL);
}

static int Config_create_altScaleFactor(int widget_desc, int *height)
{
	return Config_create_float(widget_desc, height,
							   "altScaleFactor", &iniClient.scaleFactor_s,
							   MIN_SCALEFACTOR, MAX_SCALEFACTOR,
							   NULL, NULL);
}

static int Config_create_markingLights(int widget_desc, int *height)
{
	return Config_create_bool(widget_desc, height, "markingLights",
							  iniClient.markingLights,
							  Config_update_bool, &iniClient.markingLights);
}


static int Config_create_save(int widget_desc, int *height)
{
	static char 		save_str[] = "Save Configuration";
	int 				space,
						button_desc,
						width = 2 * config_button_space
								+ XTextWidth(buttonFont, save_str,
											 strlen(save_str));

	space = config_height - (*height + 2*config_entry_height + 2*config_space);
	if (space < 0) {
		return 0;
	}
	button_desc =
		Widget_create_activate(widget_desc,
							   (config_width - width) / 2,
							   *height + space / 2,
							   width, config_button_height,
							   0, save_str,
							   Config_save, (void *)save_str);
	if (button_desc == NO_WIDGET) {
		return 0;
	}
	*height += config_entry_height + config_space + space;

	return 1;
}

/* General purpose update callback for booleans.
 * Requires that a pointer to the boolean value has been given as
 * client_data argument, and updates this value to the real value.
 */
static int Config_update_bool(int widget_desc, void *data, bool *val)
{
	bool*		client_data = (bool *) data;
	*client_data = *val;
	return 0;
}


static int Config_update_instruments(int widget_desc, void *data, bool *val)
{
	long				old_instruments = iniClient.instruments;
	long				bit = (long) data;
	long				outline_mask = SHOW_OUTLINE_WORLD
									 | SHOW_FILLED_WORLD
									 | SHOW_TEXTURED_WALLS;

	if (*val == false) {
		CLR_BIT(iniClient.instruments, bit);
	} else {
		SET_BIT(iniClient.instruments, bit);
	}
	if (bit == SHOW_SLIDING_RADAR) {
		Paint_sliding_radar();
	}
	else if (bit == SHOW_DECOR) {
		Map_dots();
		Paint_world_radar();
	}
	
	if (BIT(bit, outline_mask)) {
		/* only do the map recalculations if really needed. */
		if (!BIT(old_instruments, outline_mask)
			 != !BIT(iniClient.instruments, outline_mask)) {
			Map_restore(0, 0, Setup->x, Setup->y);
			Map_blue(0, 0, Setup->x, Setup->y);
		}
	}
	if (BIT(bit, SHOW_PACKET_DROP_METER | SHOW_PACKET_LOSS_METER)) {
		Net_init_measurement();
	}
		if (BIT(bit, SHOW_PACKET_LAG_METER)) {
				Net_init_lag_measurement();
		}
	if (BIT(bit, SHOW_REVERSE_SCROLL)) {
		/* a callback for `reverseScroll' in the config menu */
		IFNWINDOWS( Talk_reverse_cut(); )
	}

	return 0;
}

static int Config_update_dots(int widget_desc, void *data, int *val)
{
	if (val == &iniClient.mapPointSize && iniClient.mapPointSize > 1) {
		return 0;
	}
	Map_dots();
	return 0;
}

static int Config_update_power(int widget_desc, void *data, DFLOAT *val)
{
	Send_power(*val);
	control_count = CONTROL_DELAY;
	return 0;
}

static int Config_update_turnSpeed(int widget_desc, void *data, DFLOAT *val)
{
	Send_turnspeed(*val);
	control_count = CONTROL_DELAY;
	return 0;
}

static int Config_update_turnResistance(int widget_desc, void *data, DFLOAT *val)
{
	Send_turnresistance(*val);
	return 0;
}

static int Config_update_altPower(int widget_desc, void *data, DFLOAT *val)
{
	Send_power_s(*val);
	return 0;
}

static int Config_update_altTurnSpeed(int widget_desc, void *data, DFLOAT *val)
{
	Send_turnspeed_s(*val);
	return 0;
}

static int Config_update_altTurnResistance(int widget_desc, void *data, DFLOAT *val)
{
	Send_turnresistance_s(*val);
	return 0;
}

static int Config_update_sparkProb(int widget_desc, void *data, DFLOAT *val)
{
	iniClient.sparkRand = (int)(iniClient.sparkProb * MAX_SPARK_RAND + 0.5f);
	Send_display();
	return 0;
}

static int Config_update_charsPerSecond(int widget_desc, void *data, int *val)
{
	charsPerTick = (DFLOAT)iniClient.charsPerSecond / FPS;
	return 0;
}

static int Config_update_toggleShield(int widget_desc, void *data, bool *val)
{
	Set_toggle_shield(*val != false);
	return 0;
}

static int Config_update_autoShield(int widget_desc, void *data, bool *val)
{
	Set_auto_shield(*val != false);
	return 0;
}

static int Config_update_maxFPS(int widget_desc, void *data, int *val)
{
	Check_client_fps();
	return 0;
}

#ifdef	SOUND
static int Config_update_volume(int widget_desc, void *data, int *val)
{
	audioMan.SetGain((double)(iniClient.maxVolume)/100.0);
	return(0);
}
#endif

static int Config_update_texturedObjects(int widget_desc, void *data, bool *val)
{
	if ((*val != false) != iniClient.blockBitmaps) {
		if (iniClient.blockBitmaps == false) {
			/* see if we can use blockBitmaps at all. */
			iniClient.blockBitmaps = true;
			if (Colors_init_block_bitmaps() == -1) {
				/* no we can't have blockBitmaps. */
				iniClient.blockBitmaps = false;
				/* and redraw our widget as false. */
				*val = false;
				return 1;
			}
		}
		else {
			Colors_free_block_bitmaps();
			iniClient.blockBitmaps = false;
		}
	}
	return 0;
}

static int Config_update_scaleFactor(int widget_desc, void *data, DFLOAT *val)
{
	Init_scale_array();
	Resize(top, top_width, top_height);
	Scale_dashes();
	return 0;
}

static int Config_save_confirm_callback(int widget_desc, void *popup_desc, const char **strptr)
{
	if (config_save_confirm_desc != NO_WIDGET) {
		Widget_destroy((int)(long int)popup_desc);
		config_save_confirm_desc = NO_WIDGET;
	}
	return 0;
}

int Config(bool doit)
{
	IFWINDOWS( Trace("***Config %d\n", doit); )
	if (config_created == false) {
		if (doit == false) {
			return 0;
		}
		Create_config();
		if (config_created == false) {
			return false;
		}
	}
	if (config_mapped == false) {
		if (doit == true) {
			Widget_raise(config_widget_desc[config_page]);
			config_mapped = true;
		}
	} else {
		if (doit == false) {
			Widget_unmap(config_widget_desc[config_page]);
			config_mapped = false;
		}
	}
	return (config_mapped == true);
}

void Config_destroy(void)
{
	int 				i;

	if (config_created == true) {
		if (config_mapped == true) {
			Widget_unmap(config_widget_desc[config_page]);
			config_mapped = false;
		}
		for (i = 0; i < config_max; i++) {
			Widget_destroy(config_widget_desc[i]);
		}
		config_created = false;
		free(config_widget_desc);
		config_widget_desc = NULL;
		config_max = 0;
		config_page = 0;
	}
}

void Config_resize(void)
{
	bool				mapped = config_mapped;

	if (config_created == true) {
		Config_destroy();
		if (mapped == true) {
			Config(mapped);
		}
	}
}

void Config_redraw(void)
{
	int i;

	if (!config_mapped)
		return;

	for (i = 0; i < NELEM(config_creator); i++) {
		Widget_draw(config_widget_ids[i]);
	}
}

static int Config_save(int widget_desc, void *button_str, const char **strptr)
{
	*strptr = "Saving...";
	Widget_draw(widget_desc);
	Client_flush();

		iniClient.SaveAll();

		if (*strptr[0] == '\0')
				*strptr = (char *) button_str;

		if (config_save_confirm_desc != NO_WIDGET)
		{
				Widget_destroy(config_save_confirm_desc);
				config_save_confirm_desc = NO_WIDGET;
		}
		return(1);
}

static void Config_save_failed(const char *reason, const char **strptr)
{
	if (config_save_confirm_desc != NO_WIDGET) {
		Widget_destroy(config_save_confirm_desc);
	}
	config_save_confirm_desc
		= Widget_create_confirm(reason, Config_save_confirm_callback);
	if (config_save_confirm_desc != NO_WIDGET) {
		Widget_raise(config_save_confirm_desc);
	}
	*strptr = "Saving failed...";
}

